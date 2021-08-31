#include "msxmldom.h"
#include <jig/barCodeReader/BarCodeReader.h>
#include <cviauto.h>
#include <userint.h>
#include <stdio.h>
#include <stdlib.h>
#include <multithread/multithread.h>

#pragma warning( push )
#pragma warning(disable: 4100 4127)

#define LF      "\x0A"        // Line feed
#define CR      "\x0D"        // Carriage return
#define ESC     "\x1B"        // Escape character

static SElExceptionPtr bcr_Initialize
(
	struct _BarCodeReader*	me,
	const char*				configString
);
static SElExceptionPtr bcr_Close(struct _BarCodeReader* me);
static SElExceptionPtr bcr_StartReading(struct _BarCodeReader* me);
static SElExceptionPtr bcr_StopReading(struct _BarCodeReader* me);
static SElExceptionPtr bcr_GetBarCode
(
	struct _BarCodeReader*	me,
	char*					buffer,
	int32_t					bufferSize
);
static SElExceptionPtr bcr_RegisterReadCallback
(
	struct _BarCodeReader*	me,
	SElExceptionPtr			(*callbackFnc)(struct _BarCodeReader*,char*,char*)
);
static SElExceptionPtr bcr_ReadThread(struct _BarCodeReader* me);

static SElExceptionPtr bcr_WriteData(struct _BarCodeReader* me, char* data);
DWORD WINAPI Thread_Barcode( LPVOID lpParameter);

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
#if (!defined(_CVI_)&&defined(_DEBUG))
			//_CrtSetBreakAlloc(78);
#endif
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)
				return 0;	  /* out of memory */
			break;
		case DLL_PROCESS_DETACH:
			CloseCVIRTE ();   
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
			_CrtDumpMemoryLeaks();
#endif
			break;
		}
	
	return 1;
}

#undef __FUNC__
#define __FUNC__	"bcr_new"
SElExceptionPtr	bcr_new( SBarCodeReaderPtr* pBcr)
{
	SElExceptionPtr		pexception = NULL;
	SBarCodeReaderPtr	pbcr = NULL;
	
	pbcr = calloc(1, sizeof(SBarCodeReader));
	EXCCHECKALLOC( pbcr);
	
	pbcr->Initialize	= bcr_Initialize;
	pbcr->Close			= bcr_Close;
	pbcr->StartReading	= bcr_StartReading;
	pbcr->StopReading	= bcr_StopReading;
	pbcr->GetBarCode	= bcr_GetBarCode;
	pbcr->WriteData		= bcr_WriteData;
	pbcr->RegisterReadCallback = bcr_RegisterReadCallback;
	pbcr->_ReadThread	= bcr_ReadThread;
	
	InitializeCriticalSection(&pbcr->lock);
	
Error:
	if (pBcr)
		*pBcr = pbcr;
	
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"bcr_delete"
SElExceptionPtr	bcr_delete( SBarCodeReaderPtr* pBcr)
{
	SElExceptionPtr	pexception = NULL;
	
	if (pBcr && *pBcr)
	{
		EXCCHECK((*pBcr)->Close(*pBcr));
		
		if ((*pBcr)->idBCR != NULL)
		{
			free((*pBcr)->idBCR);
			(*pBcr)->idBCR = NULL;
		}
		
		DeleteCriticalSection(&((*pBcr)->lock));
		
		free(*pBcr);
		*pBcr = NULL;
	}
	
Error:
	EXCRETHROW(pexception);
}


/**
 *	This function initialize bar code reader.
 *
 *  @param me - pointer to self
 *  @param configString - XML string with configuration
 *                        <BarCodeReader id="1">
 *           		        <Driver name="SickCLP100.dll" />
 *           		        <Configuration>
 *           			      <Communication type="serial">
 *           				    <Port>COM1</Port>
 *							    <Baudrate>9600</Baudrate>
 *           				    <DataBits>8</DataBits>
 *           				    <StopBits>1.0</StopBits>
 *           				    <Parity>None</Parity>
 *           				    <Handshake>None</Handshake>
 *           			      </Communication>
 *           			      <Settings>
 *           				    <CommandString>'Settings commands'</CommandString>
 *           			      </Settings>
 *           		        </Configuration>
 *           	          </BarCodeReader>
 */
#undef __FUNC__
#define __FUNC__	"BCR::Initialize"
static SElExceptionPtr bcr_Initialize
(
	struct _BarCodeReader*	me,
	const char*				configString
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0,
					size = 0;
	char			message[256] = "";
	char*			init_string = NULL;
	char*			id_str = NULL;
	char*			type = NULL;
	char*			port = NULL;
	char*			tmp = NULL;
	DCB				dcb = {0};
	HRESULT			hr = S_OK;
	VBOOL			is_xml_success = 1;
	CAObjHandle		xml_doc = 0;
	CAObjHandle		pid = 0;
	CAObjHandle		presource = 0;
	CAObjHandle		ptype = 0;
	CAObjHandle		pport = 0;
	CAObjHandle		pbaudrate = 0;
	CAObjHandle		pdatabits = 0;
	CAObjHandle		pstopbits = 0;
	CAObjHandle		pparity = 0;
	CAObjHandle		phandshake = 0;
	CAObjHandle		pinit_string = 0;
	
	/* TODO: read configuration from XML configString */
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &xml_doc));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (xml_doc, NULL, configString, &is_xml_success));
	
	/* Get ID of barcode reader - same as index */
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectSingleNode (xml_doc, NULL,
														"/BarCodeReader/@id", &pid));
	EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (pid, NULL, &id_str));
	size = strlen(id_str) + 1;
	me->idBCR = (char*) calloc( size, sizeof(char));
	strcpy_s(me->idBCR, size, id_str);

	/* Get communication type */
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectSingleNode (xml_doc, NULL,
														"/BarCodeReader/Configuration/Communication",
														&presource));
	if (presource == 0)
	{
		sprintf_s(message, sizeof(message), "Missing configuration node");
		EXCTHROW( BCR_ERROR_INVALID_CONFIGURATION, message);
	}
	
	EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(presource, NULL, "@type", &ptype));
	if (ptype == 0)
	{
		sprintf_s(message, sizeof(message), "Missing communication type");
		EXCTHROW( BCR_ERROR_INVALID_CONFIGURATION, message);
	}
	EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (ptype, NULL, &type));
	if (!strcmp(type, "serial"))
	{
		/* Serial communication */
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(presource, NULL, "Port", &pport));
		if (pport == 0)
		{
			sprintf_s(message, sizeof(message), "Missing communication port");
			EXCTHROW( BCR_ERROR_INVALID_CONFIGURATION, message);
		}
		EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (pport, NULL, &port));
		
		/* Open COM port */
		me->fd = CreateFile(port,
							GENERIC_READ|GENERIC_WRITE,//access ( read and write)
					        0,    					//(share) 0:cannot share the COM port                        
					        0,    					//security  (None)                
					        OPEN_EXISTING,			// creation : open_existing
					        FILE_FLAG_OVERLAPPED,	// we want overlapped operation
					        0						// no templates file for COM port...
            			   );
		if (me->fd == INVALID_HANDLE_VALUE)
		{
			DWORD	bcr_error = GetLastError();
			LPVOID	lpMsgBuf;

			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    					   NULL, bcr_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    					   (LPTSTR) &lpMsgBuf, 0, NULL );
			sprintf_s(message, sizeof(message), "Serial port \"%s\" can not be open\nError: %s", port, (LPCTSTR) lpMsgBuf);
			EXCTHROW( BCR_ERROR_OPEN_FAILED, message);
		}
		
		dcb.DCBlength = sizeof(DCB);
		EXCCHECKWIN( GetCommState (me->fd, &dcb));
		
		/* Set default values */
		dcb.BaudRate    = 9600;
		dcb.ByteSize    = 8;
		dcb.Parity		= NOPARITY;
		dcb.StopBits    = ONESTOPBIT;
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(presource, NULL, "Baudrate", &pbaudrate));
		if (pbaudrate)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (pbaudrate, NULL, &tmp));
			dcb.BaudRate = atoi(tmp);
			CA_FreeMemory(tmp);
			tmp = NULL;
		}
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(presource, NULL, "DataBits", &pdatabits));
		if (pdatabits)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (pdatabits, NULL, &tmp));
			dcb.ByteSize = (BYTE) atoi(tmp);
			CA_FreeMemory(tmp);
			tmp = NULL;
		}
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(presource, NULL, "StopBits", &pstopbits));
		if (pstopbits)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (pstopbits, NULL, &tmp));
			if (!strcmp(tmp, "1.0"))
				dcb.StopBits = ONESTOPBIT;
			else if (!strcmp(tmp, "1.5"))
				dcb.StopBits = ONE5STOPBITS;
			else if (!strcmp(tmp, "2.0"))
				dcb.StopBits = TWOSTOPBITS;
			else
			{
				sprintf_s(message, sizeof(message), "Invalid stop bits configuration (%s)", tmp);
				EXCTHROW( BCR_ERROR_INVALID_CONFIGURATION, message);
			}
			CA_FreeMemory(tmp);
			tmp = NULL;
		}
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(presource, NULL, "Parity", &pparity));
		if (pparity)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (pparity, NULL, &tmp));
			if (!_stricmp(tmp, "None"))
				dcb.Parity = NOPARITY;
			else if (!_stricmp(tmp, "Odd"))
				dcb.Parity = ODDPARITY;
			else if (!_stricmp(tmp, "Even"))
				dcb.Parity = EVENPARITY;
			else if (!_stricmp(tmp, "Mark"))
				dcb.Parity = MARKPARITY;
			else if (!_stricmp(tmp, "Space"))
				dcb.Parity = SPACEPARITY;
			else
			{
				sprintf_s(message, sizeof(message), "Invalid parity configuration (%s)", tmp);
				EXCTHROW( BCR_ERROR_INVALID_CONFIGURATION, message);
			}
			CA_FreeMemory(tmp);
			tmp = NULL;
		}
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(presource, NULL, "Handshake", &phandshake));
		if (phandshake)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (phandshake, NULL, &tmp));
			if (_stricmp(tmp, "None"))
			{
				sprintf_s(message, sizeof(message), "Invalid handshake configuration (%s)", tmp);
				EXCTHROW( BCR_ERROR_INVALID_CONFIGURATION, message);
			}
			CA_FreeMemory(tmp);
			tmp = NULL;
		}
		EXCCHECKWIN( SetCommState (me->fd, &dcb));
		EXCCHECKWIN (SetCommMask(me->fd, EV_RXCHAR));
	}
	else
	{
		sprintf_s(message, sizeof(message), "Unknown communication type (%s)", type);
		EXCTHROW( BCR_ERROR_INVALID_CONFIGURATION, message);
	}
	
	/* Get Initialization String */
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectSingleNode (xml_doc, NULL,
														"/BarCodeReader/Configuration/Settings/CommandString",
														&pinit_string));
	if (pinit_string)
	{
		EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (pinit_string, NULL, &init_string));
	}
	
	if (init_string && strlen(init_string) > 0)
		EXCCHECK( me->WriteData( me, init_string));
	
	EXCCHECK( ThreadStart(Thread_Barcode, (void*) me, &me->threadHandle, &me->endEvent));
	
Error:
	if (init_string)
		CA_FreeMemory(init_string);

	if (tmp)
		CA_FreeMemory(tmp);
	
	if (port)
		CA_FreeMemory(port);
	
	if (type)
		CA_FreeMemory(type);
	
	if (id_str)
		CA_FreeMemory(id_str);
	
	if (pinit_string)
		CA_DiscardObjHandle(pinit_string);
	
	if (phandshake)
		CA_DiscardObjHandle(phandshake);
	
	if (pparity)
		CA_DiscardObjHandle(pparity);
	
	if (pstopbits)
		CA_DiscardObjHandle(pstopbits);
	
	if (pdatabits)
		CA_DiscardObjHandle(pdatabits);
	
	if (pbaudrate)
		CA_DiscardObjHandle(pbaudrate);
	
	if (pport)
		CA_DiscardObjHandle(pport);
	
	if (ptype)
		CA_DiscardObjHandle(ptype);
	
	if (presource)
		CA_DiscardObjHandle(presource);
	
	if (pid)
		CA_DiscardObjHandle(pid);
	
	if (xml_doc)
		CA_DiscardObjHandle(xml_doc);
	
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"BCR::Close"
static SElExceptionPtr bcr_Close(struct _BarCodeReader* me)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( ThreadStop(&me->threadHandle, &me->endEvent));
	
	if (me->fd)
	{
		CloseHandle( me->fd);
		me->fd = 0;
	}
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"BCR::StartReading"
static SElExceptionPtr bcr_StartReading(struct _BarCodeReader* me)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( me->WriteData( me, "LON" CR));
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"BCR::StopReading"
static SElExceptionPtr bcr_StopReading(struct _BarCodeReader* me)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( me->WriteData( me, "LOFF" CR));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"BCR::GetBarCode"
static SElExceptionPtr bcr_GetBarCode
(
	struct _BarCodeReader*	me,
	char*					buffer,
	int32_t					bufferSize
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			len = strlen(me->barCodeBuffer),
					size = 0;
	
	if (buffer == NULL || bufferSize == 0)
	{
		EXCTHROW( BCR_ERROR_INVALID_PARAMETER, "Buffer is NULL or size is equal to 0");
	}
	
	EnterCriticalSection(&me->lock);
	
	if (*(me->barCodeBuffer) != '\0')
	{
		size = ( len > bufferSize) ? bufferSize - 1 : len;
		strncpy_s(buffer, bufferSize, me->barCodeBuffer, size);
		buffer[size] = '\0';
	
		me->bufferPosition = (len - size);
	
		memmove(me->barCodeBuffer, me->barCodeBuffer + size, MAX_BCR_BUFFER - size);
	}
	else
	{
		memset(buffer, 0, bufferSize);
	}
	LeaveCriticalSection(&me->lock);

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"BCR::RegisterReadCallback"
static SElExceptionPtr bcr_RegisterReadCallback
(
	struct _BarCodeReader*	me,
	SElExceptionPtr			(*callbackFnc)(struct _BarCodeReader*,char*,char*)
)
{
	SElExceptionPtr	pexception = NULL;
	
	me->_callbackFnc = callbackFnc;
	
//Error:
	EXCRETHROW(pexception);
}
	
#undef __FUNC__
#define __FUNC__	"BCR::ReadThread"
static SElExceptionPtr bcr_ReadThread(struct _BarCodeReader* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			wait = 0;
	DWORD			event_mask = 0,
					last_read = 0;
	HANDLE			event_list[2];
	OVERLAPPED		ov,
					ov_read;
	char			read_byte;
	SElEventDataPtr pData = NULL;
	bool_t			is_locked = FALSE;
	
	event_list[0] = me->endEvent;
	memset(&ov, 0, sizeof(ov));
	ov.hEvent = CreateEvent(NULL, 1, 0, NULL);
	if(ov.hEvent==NULL)
		EXCTHROW( -1, "Create Event Fault");

	while (1)
	{
		WaitCommEvent(me->fd, &event_mask, &ov);
		event_list[1] = ov.hEvent;
		wait = WaitForMultipleObjects( 2, event_list, 0, INFINITE);
		switch (wait)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_OBJECT_0:
			ExitThread(1);
			break;
		case WAIT_OBJECT_0 + 1:
			EXCCHECKWIN( ResetEvent(ov.hEvent));
			EXCCHECKWIN( GetCommMask(me->fd, &event_mask));
			if ((event_mask & EV_RXCHAR) == 0)
			{
				char	message[256];
				/* TODO: log invalid event */
				sprintf_s(message, sizeof(message), "Invalid Read Event Mask (%d)", event_mask);
				EXCTHROW( BCR_ERROR_INVALID_READ_EVENT, message);
			}

			/* Read data from serial port */
			EnterCriticalSection(&me->lock);
			is_locked = TRUE;
			
			last_read = 0;
			memset(&ov_read, 0, sizeof(ov_read));
			ov_read.hEvent = CreateEvent(NULL, TRUE, 0, NULL);
			
			do
			{
				EXCCHECKWIN( ResetEvent(ov_read.hEvent));
				ReadFile(me->fd, &read_byte, 1, &last_read, &ov_read);
				
				if (last_read)
				{
					if (read_byte == 0x0D) /* CR */
					{
						*(me->barCodeBuffer + me->bufferPosition) = '\0';
						/* TODO: pulse barcode read event */
						me->bufferPosition = 0;
						if (me->eventFactory)
						{
							EXCCHECK( eleventdata_new(&pData));
							EXCCHECK( pData->SetStringPtr(pData, me->barCodeBuffer));
							EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_BARCODE_READ, pData));
							EXCCHECK( eleventdata_delete(&pData));
						}
						
						if (me->_callbackFnc != NULL)
							EXCCHECK( me->_callbackFnc(me, me->idBCR, me->barCodeBuffer));
						break;
					}
					else
					{
						if (read_byte != 0x0A) /* LF */
						{
							*(me->barCodeBuffer + me->bufferPosition) = read_byte;
							me->bufferPosition++;
							*(me->barCodeBuffer + me->bufferPosition) = '\0';
						}
					}
				}
			} while (last_read > 0 && me->bufferPosition < MAX_BCR_BUFFER - 1);
			
			LeaveCriticalSection(&me->lock);
			is_locked = FALSE;

			/* Close read event and cancel read function */
			EXCCHECKWIN( CloseHandle(ov_read.hEvent));
			EXCCHECKWIN( CancelIo(me->fd));
			break;
		}
	}
	
Error:
	if(is_locked)
		LeaveCriticalSection(&me->lock);
	eleventdata_delete(&pData);
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"BCR::WriteData"
static SElExceptionPtr bcr_WriteData(struct _BarCodeReader* me, char* data)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			wait = 0;
	OVERLAPPED		ov;
	
	memset(&ov, 0, sizeof(ov));
	ov.hEvent = CreateEvent( 0,1,0,0);
	if(ov.hEvent==NULL)
		EXCTHROW( -1, "Create Event Fault");
	
	WriteFile(me->fd, (void*) data, strlen(data), NULL, &ov);
	
	wait = WaitForSingleObject(ov.hEvent, 500);
	if (wait == WAIT_TIMEOUT)
		EXCTHROW( BCR_ERROR_WRITE_TIMEOUT, "Write Timeout");

Error:
	CloseHandle(ov.hEvent);
	
	EXCRETHROW(pexception);
}

DWORD WINAPI Thread_Barcode(  LPVOID lpParameter)
{
	SBarCodeReaderPtr	me = (SBarCodeReaderPtr) lpParameter;
	SElExceptionPtr		pexception = NULL;
	
	EXCDISPLAY( me->_ReadThread(me));
	
	return 0;
}

#pragma warning( pop )
