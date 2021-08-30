#include "lasers.h"
#include <string.h>
#include <stdio.h>
#include <cviauto.h>
#include "msxmldom.h"
#include <utility.h>

#define DISCARD_HANDLE(handle) \
	if (handle) \
	{ \
		CA_DiscardObjHandle(handle); \
		handle = 0; \
	}

#define CA_FREE_MEMORY(mem) \
	if (mem) \
	{ \
		CA_FreeMemory(mem); \
		mem = NULL; \
	}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static SElExceptionPtr laser_open(
		struct _Laser*	me,
		const char*			resource,
		const char*			optionString
);
static SElExceptionPtr laser_close(struct _Laser* me);
static SElExceptionPtr laser_connect(struct _Laser* me);
static SElExceptionPtr laser_disconnect(struct _Laser* me);
static SElExceptionPtr laser_writeString(SLaserPtr me, int program, int block, const char* string);
static SElExceptionPtr laser_runProgram(SLaserPtr me, int program, int block);
static SElExceptionPtr laser_enableMarking(SLaserPtr);
static SElExceptionPtr laser_markerReady(SLaserPtr me, int*Status);
static SElExceptionPtr laser_startLasering(SLaserPtr me);
static SElExceptionPtr laser_ErrorClear(SLaserPtr me); 
static SElExceptionPtr laser_ErrorStatus(SLaserPtr me, int *ErrorStatus, char *ErrorCode); 
static SElExceptionPtr laser_ReadString(SLaserPtr me, int program, int block, const char* File_Line_String, int* string_OK ,char *buffer);

DWORD WINAPI Thread_Laser(
  LPVOID lpParameter
);

/*****************************************************************************/

static SElExceptionPtr lasers_GetLaserByName(
	struct _Lasers*	me,
	const char*			laserName,
	SLaserPtr*		laser
);
static SElExceptionPtr lasers_GetLaserByIndex(
	struct _Lasers*	me,
	int32_t				index,
	SLaserPtr*		laser
);
static SElExceptionPtr lasers_GetCurrentLaser(
	struct _Lasers*	me,
	SLaserPtr*		laser
);
static SElExceptionPtr lasers_SetCurrentLaserByName(
	struct _Lasers*	me,
	const char*			laserName
);
static SElExceptionPtr lasers_GetFirstLaser(
	struct _Lasers*	me,
	SLaserPtr*		laser
);
static SElExceptionPtr lasers_GetNextLaser(
	struct _Lasers*	me,
	SLaserPtr*		laser
);
	
static SElExceptionPtr lasers_InitLasers(struct _Lasers* me);
static SElExceptionPtr lasers_CloseLasers(struct _Lasers* me);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "laser_new"
SElExceptionPtr laser_new(
	SLaserPtr*	laser,
	const char*		name,
	const char*		optionString
)
{
	SElExceptionPtr	pexception = NULL;
	SLaserPtr		pnew_laser = NULL;
	
	pnew_laser = calloc(1, sizeof(SLaser));
	EXCCHECKALLOC( pnew_laser);
	InitializeCriticalSection(&(pnew_laser->lock));
	
	pnew_laser->laserName = calloc(strlen(name) + 1, sizeof(char));
	EXCCHECKALLOC( pnew_laser->laserName);
	strcpy(pnew_laser->laserName, name);
	
	pnew_laser->Open			= laser_open;
	pnew_laser->Close			= laser_close;
	pnew_laser->Connect			= laser_connect;
	pnew_laser->Disconnect		= laser_disconnect;
	pnew_laser->WriteString		= laser_writeString;
	pnew_laser->RunProgram		= laser_runProgram;
	pnew_laser->EnableMarking 	= laser_enableMarking;
   	pnew_laser->MarkerReady 	= laser_markerReady;
	pnew_laser->StartLasering 	= laser_startLasering;
	pnew_laser->ErrorStatus 	= laser_ErrorStatus;
	pnew_laser->ErrorClear  	= laser_ErrorClear;
	pnew_laser->ReadString		= laser_ReadString;

	if (laser)
		*laser = pnew_laser;
	
Error:
	if (pexception && pnew_laser)
		free(pnew_laser);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "laser_delete"
SElExceptionPtr laser_delete(SLaserPtr* laser)
{
	SElExceptionPtr	pexception = NULL;
	
	if (laser && *laser)
	{
		(*laser)->Close(*laser);
		
		if ((*laser)->laserName)
			free((*laser)->laserName);
		
		if ((*laser)->optionString)
			free((*laser)->optionString);
		
		DeleteCriticalSection( &((*laser)->lock) );
		
		free(*laser);
		*laser = NULL;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "lasers_new"
SElExceptionPtr lasers_new(SLasersPtr*	lasers)
{
	SElExceptionPtr	pexception = NULL;
	SLasersPtr	pnew_lasers = NULL;
	
	pnew_lasers = calloc(1, sizeof(SLasers));
	EXCCHECKALLOC( pnew_lasers);
	
	pnew_lasers->lasers = NULL;
	pnew_lasers->numLasers = 0;
	pnew_lasers->currentIndex = -1;
	
	pnew_lasers->GetLaserByName			= lasers_GetLaserByName;
	pnew_lasers->GetLaserByIndex		= lasers_GetLaserByIndex;
	pnew_lasers->GetCurrentLaser		= lasers_GetCurrentLaser;
	pnew_lasers->SetCurrentLaserByName	= lasers_SetCurrentLaserByName;
	pnew_lasers->GetFirstLaser			= lasers_GetFirstLaser;
	pnew_lasers->GetNextLaser			= lasers_GetNextLaser;
	pnew_lasers->InitLasers				= lasers_InitLasers;
	pnew_lasers->CloseLasers			= lasers_CloseLasers;
	
	if (lasers)
		*lasers = pnew_lasers;
	
Error:
	if (pexception && pnew_lasers)
		free(pnew_lasers);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "lasers_delete"
SElExceptionPtr lasers_delete(SLasersPtr* lasers)
{
	SElExceptionPtr	pexception = NULL;
	
	if (lasers && *lasers)
	{
		EXCCHECK( (*lasers)->CloseLasers(*lasers));
		
		free(*lasers);
		*lasers = NULL;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "{Laser}::Open"
static SElExceptionPtr laser_open(
		struct _Laser*	me,
		const char*			resource,
		const char*			optionString
)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr = S_OK;
	VARIANT_BOOL	is_xml_success = 1;
	CAObjHandle		document = 0;
	CAObjHandle		xml_configuration = 0;
	CAObjHandle		xml_node = 0;
	char*			xml_type = NULL;
	char*			xml_value = NULL;
	char			message[1024] = "";
	
	if (me->fd)
	{
		EXCTHROW( -11, "Laser Allready Open");
	}
	
	me->optionString = calloc(strlen(optionString) + 1, sizeof(char));
	EXCCHECKALLOC( me->optionString);
	strcpy(me->optionString, optionString);
	
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &document));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (document, NULL, optionString, &is_xml_success));
	
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectSingleNode(document, NULL,
													   "/Laser/Configuration",
													   &xml_configuration));
	if (xml_configuration == 0)
		EXCTHROW( -1, "Missing Configuration Node");
	
	/* Load Specific DLL functions */
	EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL, "@specificDll", &xml_node));
	if (xml_node == 0)
		EXCTHROW( -3, "Missing Laser Specific Library");
	
	EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
	me->specLib = LoadLibrary(xml_value);
	if (me->specLib == 0)
	{
		sprintf(message, "Laser Library (%s) Loading Failed", xml_value);
		EXCTHROW( -4, message);
	}
	
	me->_ConnectSpec		= (SElExceptionPtr (*)(SLaserPtr))
									GetProcAddress(me->specLib, "Connect");
	me->_DisconnectSpec		= (SElExceptionPtr (*)(SLaserPtr))
									GetProcAddress(me->specLib, "Disconnect");
	
	me->_WriteStringSpec	= (SElExceptionPtr (*)(SLaserPtr,int, int, const char*))
									GetProcAddress(me->specLib, "WriteString");
	
   	me->_RunProgramSpec		= (SElExceptionPtr (*)(SLaserPtr,int, int))
									GetProcAddress(me->specLib, "RunProgram");
	
	me->_EnableMarkingSpec	= (SElExceptionPtr (*)(SLaserPtr))
									GetProcAddress(me->specLib, "EnableMarking");
	
	me->_MarkerReadySpec	= (SElExceptionPtr (*)(SLaserPtr, int *))
									GetProcAddress(me->specLib, "MarkerReady");
	
	me->_StartLaseringSpec	= (SElExceptionPtr (*)(SLaserPtr))
									GetProcAddress(me->specLib, "StartLasering");
	
	me->_ErrorStatusSpec	= (SElExceptionPtr (*)(SLaserPtr,int*, char*))
									GetProcAddress(me->specLib, "ErrorStatus");
	
	me->_ErrorClearSpec	= (SElExceptionPtr (*)(SLaserPtr))
									GetProcAddress(me->specLib, "ErrorClear");
	me->_ReadStringSpec	= (SElExceptionPtr (*)(SLaserPtr,int, int, const char*, int*,char*))
									GetProcAddress(me->specLib, "ReadString");
	
	CA_FREE_MEMORY(xml_value);
	DISCARD_HANDLE(xml_node);
	
	/* Get Type of communication */
	EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
												   "@type", &xml_node));
	if (xml_node == 0)
		EXCTHROW( -2, "Missing Configuration Type");

	EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_type));
	DISCARD_HANDLE(xml_node);
	
	if (!stricmp(xml_type, "Serial"))
	{
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "Port", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			me->port = atoi(xml_value);
			CA_FREE_MEMORY(xml_value);
		}
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "BaudRate", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			me->baudRate = atoi(xml_value);
			CA_FREE_MEMORY(xml_value);
		}
	}
	
	else
	{
		EXCTHROW( -10, "Unsupported Configuration Type");
	}


			
Error:

	if (pexception)
	{
		if (me->specLib)
		{
			FreeLibrary(me->specLib);
			me->specLib = 0;
		}
		
		me->_ConnectSpec		= NULL;
		me->_DisconnectSpec		= NULL;
		me->_WriteStringSpec	= NULL;
		me->_RunProgramSpec		= NULL;
		me->_EnableMarkingSpec	= NULL;
		me->_MarkerReadySpec	= NULL;
		me->_StartLaseringSpec	= NULL;
		me->_ErrorStatusSpec	= NULL; 
		me->_ErrorClearSpec  	= NULL;
		me->_ReadStringSpec	    = NULL;
	
		if (me->termEvent)
		{
			CloseHandle(me->termEvent);
			me->termEvent = 0;
		}
		
		if (me->fd)
		{
			CloseHandle(me->fd);
			me->fd = 0;
		}
	}
		
	DISCARD_HANDLE(xml_node);
	CA_FREE_MEMORY(xml_value);
	CA_FREE_MEMORY(xml_type);
	DISCARD_HANDLE(xml_configuration);
	DISCARD_HANDLE(document);

	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::Close"
static SElExceptionPtr laser_close(struct _Laser* me)
{
	SElExceptionPtr	pexception = NULL;
	
	if (me->fd)
	{
		SetEvent(me->termEvent);
		/* todo: wait for thread end */
		Sleep(500);
		
		me->_ConnectSpec		= NULL;
		me->_DisconnectSpec		= NULL;
		me->_WriteStringSpec	= NULL;
		me->_RunProgramSpec		= NULL;
		me->_EnableMarkingSpec	= NULL;
		me->_MarkerReadySpec	= NULL;
		me->_StartLaseringSpec	= NULL;
		me->_ErrorStatusSpec	= NULL; 
		me->_ErrorClearSpec  	= NULL; 

		if (me->specLib)
		{
			FreeLibrary(me->specLib);
			me->specLib = 0;
		}
		
		CloseHandle(me->fd);
		me->fd = 0;
		
		CloseHandle(me->hThread);
		me->hThread = 0;
		
		CloseHandle(me->termEvent);
		me->termEvent = 0;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "fncLock"
static SElExceptionPtr laserLock(SLaserPtr me)
{
	EnterCriticalSection( &me->lock);    
	return NULL;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "laserUnLock"
static SElExceptionPtr laserUnLock(SLaserPtr me)
{
	LeaveCriticalSection( &me->lock);   
	return NULL;
}

#undef __FUNC__
#define __FUNC__ "{Laser}::Connect"
static SElExceptionPtr laser_connect(SLaserPtr me)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_ConnectSpec)
		EXCCHECK( me->_ConnectSpec(me));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::Disconnect"
static SElExceptionPtr laser_disconnect(SLaserPtr me)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_ConnectSpec)
		EXCCHECK( me->_ConnectSpec(me));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::RunProgram"
static SElExceptionPtr laser_runProgram(SLaserPtr me, int program, int block)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_RunProgramSpec)
		EXCCHECK( me->_RunProgramSpec(me, program, block));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::WriteString"
static SElExceptionPtr laser_writeString(SLaserPtr me, int program, int block, const char* string)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_WriteStringSpec)
		EXCCHECK( me->_WriteStringSpec(me, program, block, string));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::ReadString"
static SElExceptionPtr laser_ReadString(SLaserPtr me, int program, int block, const char* File_Line_String, int* String_OK,char *buffer)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_ReadStringSpec)
		EXCCHECK( me->_ReadStringSpec(me, program, block, File_Line_String,String_OK,buffer));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::EnableMarking"
static SElExceptionPtr laser_enableMarking(SLaserPtr me)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_EnableMarkingSpec)
		EXCCHECK( me->_EnableMarkingSpec(me));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::MarkerReady"
static SElExceptionPtr laser_markerReady(SLaserPtr me, int *Ready)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_MarkerReadySpec)
		EXCCHECK( me->_MarkerReadySpec(me, Ready));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::StartLasering"
static SElExceptionPtr laser_startLasering(SLaserPtr me)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_StartLaseringSpec)
		EXCCHECK( me->_StartLaseringSpec(me));
	
Error:
	EXCRETHROW( pexception);
}
/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "{Lasers}::GetLaserByName"
static SElExceptionPtr lasers_GetLaserByName(
	struct _Lasers*	me,
	const char*			laserName,
	SLaserPtr*		laser
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	SLaserPtr		found_laser = NULL;
	char			message[1024] = "";
	
	for (i = 0; i < me->numLasers; i++)
	{
		if (!strcmp(me->lasers[i]->laserName, laserName))
		{
			found_laser = me->lasers[i];
			break;
		}
	}
	
	if (laser)
		*laser = found_laser;
	
	if (found_laser == NULL)
	{
		sprintf(message, "Laser Name (%s) Not Found", laserName);
		EXCTHROW( -12, message);
	}
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Lasers}::GetLaserByIndex"
static SElExceptionPtr lasers_GetLaserByIndex(
	struct _Lasers*	me,
	int32_t				index,
	SLaserPtr*		laser
)
{
	SElExceptionPtr	pexception = NULL;
	
	if (index < me->numLasers && index >= 0)
	{
		if (laser)
			*laser = me->lasers[index];
	}
	else
	{
		if (laser)
			*laser = NULL;
		
		EXCTHROW( -13, "Index Out of Range");
	}
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Lasers}::GetCurrentLaser"
static SElExceptionPtr lasers_GetCurrentLaser(
	struct _Lasers*	me,
	SLaserPtr*		laser
)
{
	SElExceptionPtr	pexception = NULL;
	
	if (me->currentIndex >= 0 && me->numLasers > 0)
	{
		if (laser)
			*laser = me->lasers[me->currentIndex];
	}
	else
	{
		if (laser)
			*laser = NULL;
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Lasers}::SetCurrentLaserByName"
static SElExceptionPtr lasers_SetCurrentLaserByName(
	struct _Lasers*	me,
	const char*			laserName
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	SLaserPtr		laser = NULL;
	char			message[1024] = "";

	for (i = 0; i < me->numLasers; i++)
	{
		if (!strcmp(me->lasers[i]->laserName, laserName))
		{
			me->currentIndex = i;
			break;
		}
	}
	
	if (i >= me->numLasers)
	{
		sprintf(message, "Laser Name (%s) Not Found", laserName);
		EXCTHROW( -12, message);
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Lasers}::GetFirstLaser"
static SElExceptionPtr lasers_GetFirstLaser(
	struct _Lasers*	me,
	SLaserPtr*		laser
)
{
	SElExceptionPtr	pexception = NULL;
	
	me->currentIndex = 0;

	if(me->numLasers > 0)
	{
		if (laser)
			*laser = me->lasers[0];
	}
	else
	{
		if (laser)
			*laser = NULL;
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Lasers}::GetNextLaser"
static SElExceptionPtr lasers_GetNextLaser(
	struct _Lasers*	me,
	SLaserPtr*		laser
)
{
	SElExceptionPtr	pexception = NULL;

	if (++(me->currentIndex) >= me->numLasers)
	{
		me->currentIndex = me->numLasers - 1;
		if (laser)
			*laser = NULL;
	}
	else
	{
		if (laser)
			*laser = me->lasers[me->currentIndex];
	}
	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Lasers}::InitLasers"
static SElExceptionPtr lasers_InitLasers(struct _Lasers* me)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr = S_OK;
	VARIANT_BOOL	is_xml_success = 1;
	VARIANT			file;
	CAObjHandle		document = 0;
	CAObjHandle		laser = 0;
	CAObjHandle		laser_name_node = 0;
	CAObjHandle		laser_list = 0;
	char*			xml_value = NULL;
	char*			laser_xml = NULL;
	
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &document));
	
	CA_VariantSetCString (&file, "modules/lasers.xml");
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentload(document, NULL, file, &is_xml_success));
	
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectNodes(document, NULL, "/Lasers/Laser", &laser_list));
	EXCCHECKCOM( MSXML_IXMLDOMNodeListreset (laser_list, NULL));
	
	EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode(laser_list, NULL, &laser));
	while (laser)
	{
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(laser, NULL, "@name", &laser_name_node));
		EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(laser_name_node, NULL, &xml_value));
		EXCCHECKCOM( MSXML_IXMLDOMNodeGetxml(laser, NULL, &laser_xml));
		
		me->lasers = realloc( me->lasers, (me->numLasers + 1) * sizeof(SLaserPtr));
		EXCCHECKALLOC( me->lasers);
		
		EXCCHECK( laser_new( &me->lasers[me->numLasers], xml_value, NULL));
		EXCCHECK( me->lasers[me->numLasers]->Open(me->lasers[me->numLasers], NULL, laser_xml));
		me->numLasers++;
		
		CA_FREE_MEMORY(xml_value);
		CA_FREE_MEMORY(laser_xml);
		DISCARD_HANDLE(laser_name_node);
		DISCARD_HANDLE(laser);
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode(laser_list, NULL, &laser));
	}
	
Error:
	CA_FREE_MEMORY(xml_value);
	CA_FREE_MEMORY(laser_xml);
	DISCARD_HANDLE(laser_name_node);
	DISCARD_HANDLE(laser);
	DISCARD_HANDLE(laser_list);
	DISCARD_HANDLE(document);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Lasers}::CloseLasers"
static SElExceptionPtr lasers_CloseLasers(struct _Lasers* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	
	for (i = 0; i < me->numLasers; i++)
	{
		EXCCHECK( laser_delete(&(me->lasers[i])));
	}
	free(me->lasers);
	me->lasers = NULL;
	me->numLasers = 0;
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::ErrorStatus"
static SElExceptionPtr laser_ErrorStatus(SLaserPtr me, int *ErrorStatus, char *ErrorCode)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_ErrorStatusSpec)
		EXCCHECK( me->_ErrorStatusSpec(me,ErrorStatus,ErrorCode));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Laser}::ErrorClear"
static SElExceptionPtr laser_ErrorClear(SLaserPtr me)
{
	SElExceptionPtr	pexception = NULL;
	

	if (me->_ErrorClearSpec)
		EXCCHECK( me->_ErrorClearSpec(me));
	
Error:
	EXCRETHROW( pexception);
}
