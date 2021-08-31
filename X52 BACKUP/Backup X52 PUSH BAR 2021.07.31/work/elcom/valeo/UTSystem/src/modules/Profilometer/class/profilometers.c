#include "profilometers.h"
#include <string.h>
#include <stdio.h>
#include <cviauto.h>
#include "msxmldom.h"

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

static SElExceptionPtr profilometer_open(
		struct _Profilometer*	me,
		const char*			resource,
		const char*			optionString
);
static SElExceptionPtr profilometer_close(struct _Profilometer* me);

static SElExceptionPtr Profilometer_StartMeasure(SProfilometerPtr me);
static SElExceptionPtr Profilometer_StopMeasure(SProfilometerPtr me);
static SElExceptionPtr Profilometer_GetMeasurementValue(SProfilometerPtr me, MEASURE_DATA* pMeasureData);

static SElExceptionPtr profilometer_write(SProfilometerPtr me, const char* command);
static SElExceptionPtr profilometer_getStatus(SProfilometerPtr me, int32_t* status);

DWORD WINAPI Thread_Profilometer(
  LPVOID lpParameter
);

/*****************************************************************************/

static SElExceptionPtr profilometers_GetProfilometerByName(
	struct _Profilometers*	me,
	const char*			profilometerName,
	SProfilometerPtr*		profilometer
);
static SElExceptionPtr profilometers_GetProfilometerByIndex(
	struct _Profilometers*	me,
	int32_t				index,
	SProfilometerPtr*		profilometer
);
static SElExceptionPtr profilometers_GetCurrentProfilometer(
	struct _Profilometers*	me,
	SProfilometerPtr*		profilometer
);
static SElExceptionPtr profilometers_SetCurrentProfilometerByName(
	struct _Profilometers*	me,
	const char*			profilometerName
);
static SElExceptionPtr profilometers_GetFirstProfilometer(
	struct _Profilometers*	me,
	SProfilometerPtr*		profilometer
);
static SElExceptionPtr profilometers_GetNextProfilometer(
	struct _Profilometers*	me,
	SProfilometerPtr*		profilometer
);
	
static SElExceptionPtr profilometers_InitProfilometers(struct _Profilometers* me);
static SElExceptionPtr profilometers_CloseProfilometers(struct _Profilometers* me);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "profilometer_new"
SElExceptionPtr profilometer_new(
	SProfilometerPtr*	profilometer,
	const char*		name,
	const char*		optionString
)
{
	SElExceptionPtr	pexception = NULL;
	SProfilometerPtr		pnew_profilometer = NULL;
	
	pnew_profilometer = calloc(1, sizeof(SProfilometer));
	EXCCHECKALLOC( pnew_profilometer);
	InitializeCriticalSection(&(pnew_profilometer->lock));
	
	pnew_profilometer->profilometerName = calloc(strlen(name) + 1, sizeof(char));
	EXCCHECKALLOC( pnew_profilometer->profilometerName);
	strcpy(pnew_profilometer->profilometerName, name);
	
	pnew_profilometer->Open		= profilometer_open;
	pnew_profilometer->Close		= profilometer_close;
	
	pnew_profilometer->StartMeasure		= Profilometer_StartMeasure;
	pnew_profilometer->StopMeasure	= Profilometer_StopMeasure;
	pnew_profilometer->GetMeasurementValue	= Profilometer_GetMeasurementValue;

	pnew_profilometer->Write		= profilometer_write;
	pnew_profilometer->readEvent = CreateEvent(NULL, 1, 0, NULL);
	
	if (profilometer)
		*profilometer = pnew_profilometer;
	
Error:
	if (pexception && pnew_profilometer)
		free(pnew_profilometer);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "profilometer_delete"
SElExceptionPtr profilometer_delete(SProfilometerPtr* profilometer)
{
	SElExceptionPtr	pexception = NULL;
	
	if (profilometer && *profilometer)
	{
		(*profilometer)->Close(*profilometer);
		
		if ((*profilometer)->profilometerName)
			free((*profilometer)->profilometerName);
		
		if ((*profilometer)->optionString)
			free((*profilometer)->optionString);
		
		DeleteCriticalSection( &((*profilometer)->lock) );
		
		free(*profilometer);
		*profilometer = NULL;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "profilometers_new"
SElExceptionPtr profilometers_new(SProfilometersPtr*	profilometers)
{
	SElExceptionPtr	pexception = NULL;
	SProfilometersPtr	pnew_profilometers = NULL;
	
	pnew_profilometers = calloc(1, sizeof(SProfilometers));
	EXCCHECKALLOC( pnew_profilometers);
	
	pnew_profilometers->profilometers = NULL;
	pnew_profilometers->numProfilometers = 0;
	pnew_profilometers->currentIndex = -1;
	
	pnew_profilometers->GetProfilometerByName			= profilometers_GetProfilometerByName;
	pnew_profilometers->GetProfilometerByIndex		= profilometers_GetProfilometerByIndex;
	pnew_profilometers->GetCurrentProfilometer		= profilometers_GetCurrentProfilometer;
	pnew_profilometers->SetCurrentProfilometerByName	= profilometers_SetCurrentProfilometerByName;
	pnew_profilometers->GetFirstProfilometer			= profilometers_GetFirstProfilometer;
	pnew_profilometers->GetNextProfilometer			= profilometers_GetNextProfilometer;
	pnew_profilometers->InitProfilometers				= profilometers_InitProfilometers;
	pnew_profilometers->CloseProfilometers			= profilometers_CloseProfilometers;
	
	if (profilometers)
		*profilometers = pnew_profilometers;
	
Error:
	if (pexception && pnew_profilometers)
		free(pnew_profilometers);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "profilometers_delete"
SElExceptionPtr profilometers_delete(SProfilometersPtr* profilometers)
{
	SElExceptionPtr	pexception = NULL;
	
	if (profilometers && *profilometers)
	{
		EXCCHECK( (*profilometers)->CloseProfilometers(*profilometers));
		
		free(*profilometers);
		*profilometers = NULL;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "{Profilometer}::Open"
static SElExceptionPtr profilometer_open(
		struct _Profilometer*	me,
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
	LONG            error;
	ETHERNET_CONFIG* pEthernetConfig;
	MEASURE_DATA pMeasure[16];
	int a,b,c,d;
	FLOAT out0,out1;
	
	if (me->fd)
	{
		EXCTHROW( -11, "Profilometer Allready Open");
	}
	
	me->optionString = calloc(strlen(optionString) + 1, sizeof(char));
	EXCCHECKALLOC( me->optionString);
	strcpy(me->optionString, optionString);
	
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &document));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (document, NULL, optionString, &is_xml_success));
	
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectSingleNode(document, NULL,
													   "/Profilometer/Configuration",
													   &xml_configuration));
	if (xml_configuration == 0)
		EXCTHROW( -1, "Missing Configuration Node");
	
	/* Load Specific DLL functions */
	EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL, "@specificDll", &xml_node));
	if (xml_node == 0)
		EXCTHROW( -3, "Missing Profilometer Specific Library");
	
	EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
	me->specLib = LoadLibrary(xml_value);
	if (me->specLib == 0)
	{
		sprintf(message, "Profilometer Library (%s) Loading Failed", xml_value);
		EXCTHROW( -4, message);
	}
	
	me->_InitializeSpec	= (void*) GetProcAddress(me->specLib, "LJV7IF_Initialize");
	
	me->_FinalizeSpec	= (void*)GetProcAddress(me->specLib, "LJV7IF_Finalize");
	
	me->_EthernetOpenSpec	= (void*)GetProcAddress(me->specLib, "LJV7IF_EthernetOpen");
	
	me->_StartMeasureSpec	= (void*)GetProcAddress(me->specLib, "LJV7IF_StartMeasure");
	
	me->_StopMeasureSpec	= (void*)GetProcAddress(me->specLib, "LJV7IF_StartMeasure");
	
	me->_TriggerSpec		= (void*)GetProcAddress(me->specLib, "LJV7IF_Trigger");
	
	me->_GetMeasurementValueSpec		= (void*)GetProcAddress(me->specLib, "LJV7IF_GetMeasurementValue");
	
	
	CA_FREE_MEMORY(xml_value);
	DISCARD_HANDLE(xml_node);
	
	/* Get Type of communication */
	EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
												   "@type", &xml_node));
	if (xml_node == 0)
		EXCTHROW( -2, "Missing Configuration Type");

	EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_type));
	DISCARD_HANDLE(xml_node);
	
	if (!stricmp(xml_type, "Ethernet"))
	{
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "Index", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			me->Index = atoi(xml_value);
			CA_FREE_MEMORY(xml_value);
		}
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "IP", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			sprintf (me->adressIP,xml_value);
			CA_FREE_MEMORY(xml_value);
		}
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "Port", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			me->port = atoi(xml_value);
			CA_FREE_MEMORY(xml_value);
		}
		
		DISCARD_HANDLE(xml_node);
		
		SetCommMask(me->fd, EV_RXCHAR);
		me->termEvent = CreateEvent(NULL, 1, 0, NULL);
		
		if (me->termEvent == NULL)
		{
			EXCTHROW( -8, "Terminal Event Creation Failed");
		}
		
		me->hThread = CreateThread(NULL, 0, Thread_Profilometer,
								   (void*) me, 0, 0);
		
		if (me->hThread == NULL)
		{
			EXCTHROW( -9, "Thread Start Failed");
		}
		
#if 0    /* formerly excluded lines */
		if (me->_InitializeSpec)
			EXCCHECKCVI( me->_InitializeSpec());
#endif   /* formerly excluded lines */
		
		pEthernetConfig = malloc (sizeof(ETHERNET_CONFIG));

		/* Open connexion Ethernet */
		sscanf(me->adressIP, "%d.%d.%d.%d",&a,&b,&c,&d);
		pEthernetConfig->abyIpAddress[0] = a;
		pEthernetConfig->abyIpAddress[1] = b;
		pEthernetConfig->abyIpAddress[2] = c;
		pEthernetConfig->abyIpAddress[3] = d;
		pEthernetConfig->wPortNo = me->port;

		int retour = -1;
		
		if (me->_EthernetOpenSpec)
		retour = me->_EthernetOpenSpec(me->Index, pEthernetConfig);
		
		if (me->_GetMeasurementValueSpec)
		retour = me->_GetMeasurementValueSpec(me->Index, pMeasure);
		
		out0 = pMeasure[0].fValue;
		out1 = pMeasure[1].fValue;
		
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
		
		me->_InitializeSpec	= NULL;
		me->_GetStatusSpec	= NULL;
		me->_WriteSpec		= NULL;
		me->_SendCommandSpec= NULL;
		me->_callbackThreadSpec= NULL;
		
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
#define __FUNC__ "{Profilometer}::Close"
static SElExceptionPtr profilometer_close(struct _Profilometer* me)
{
	SElExceptionPtr	pexception = NULL;
	
	if (me->fd)
	{
		SetEvent(me->termEvent);
		/* todo: wait for thread end */
		Sleep(500);
		
		me->_FinalizeSpec	= NULL;
		me->_InitializeSpec	= NULL;
		me->_GetStatusSpec	= NULL;
		me->_WriteSpec		= NULL;
		me->_SendCommandSpec= NULL;
		me->_callbackThreadSpec= NULL;
		
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
static SElExceptionPtr profilometerLock(SProfilometerPtr me)
{
	EnterCriticalSection( &me->lock);    
	return NULL;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "profilometerUnLock"
static SElExceptionPtr profilometerUnLock(SProfilometerPtr me)
{
	LeaveCriticalSection( &me->lock);   
	return NULL;
}

#undef __FUNC__
#define __FUNC__ "{Profilometer}::Write"
static SElExceptionPtr profilometer_write(SProfilometerPtr me, const char* command)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( profilometerLock(me));

	if (me->_WriteSpec)
		EXCCHECK( me->_WriteSpec(me, command));
	
Error:
	profilometerUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometer}::GetStatus"
static SElExceptionPtr profilometer_getStatus(SProfilometerPtr me, int32_t* status)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( profilometerLock(me));

	if (me->_GetStatusSpec)
		EXCCHECK( me->_GetStatusSpec(me, status));
	
Error:
	profilometerUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometer}::Thread_Profilometer"
DWORD WINAPI Thread_Profilometer(
  LPVOID lpParameter
)
{
	SElExceptionPtr	pexception = NULL;
	SProfilometerPtr	me = (SProfilometerPtr) lpParameter;
	
	if (me->_callbackThreadSpec)
		EXCCHECK( me->_callbackThreadSpec(me));
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

#undef __FUNC__
#define __FUNC__ "{Profilometer}::StartMeasure"
static SElExceptionPtr Profilometer_StartMeasure(SProfilometerPtr me)
{
	LONG	error = 0;
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( profilometerLock(me));
	
	if (me->_StartMeasureSpec)
		EXCCHECKCVI( me->_StartMeasureSpec(me->Index));
	
Error:
	profilometerUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometer}::StopMeasure"
static SElExceptionPtr Profilometer_StopMeasure(SProfilometerPtr me)
{
	LONG	error = 0;
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( profilometerLock(me));
	
	if (me->_StopMeasureSpec)
		EXCCHECKCVI( me->_StopMeasureSpec(me->Index));
	
Error:
	profilometerUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometer}::Trigger"
static SElExceptionPtr Profilometer_Trigger(SProfilometerPtr me)
{
	LONG	error = 0;
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( profilometerLock(me));
	
	if (me->_StopMeasureSpec)
		EXCCHECKCVI( me->_TriggerSpec(me->Index));
	
Error:
	profilometerUnLock(me);
	EXCRETHROW( pexception);
}
#undef __FUNC__
#define __FUNC__ "{Profilometer}::GetMeasurementValue"
static SElExceptionPtr Profilometer_GetMeasurementValue(SProfilometerPtr me, MEASURE_DATA* pMeasureData)
{
	LONG	error = 0;
	SElExceptionPtr	pexception = NULL;
	FLOAT out0,out1;
	
	EXCCHECK( profilometerLock(me));
	
	if (me->_GetMeasurementValueSpec)
		EXCCHECKCVI( me->_GetMeasurementValueSpec(me->Index, pMeasureData));
	
	out0 = pMeasureData[0].fValue;
	out1 = pMeasureData[1].fValue;

Error:
	profilometerUnLock(me);
	EXCRETHROW( pexception);
}



/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "{Profilometers}::GetProfilometerByName"
static SElExceptionPtr profilometers_GetProfilometerByName(
	struct _Profilometers*	me,
	const char*			profilometerName,
	SProfilometerPtr*		profilometer
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	SProfilometerPtr		found_profilometer = NULL;
	char			message[1024] = "";
	
	for (i = 0; i < me->numProfilometers; i++)
	{
		if (!strcmp(me->profilometers[i]->profilometerName, profilometerName))
		{
			found_profilometer = me->profilometers[i];
			break;
		}
	}
	
	if (profilometer)
		*profilometer = found_profilometer;
	
	if (found_profilometer == NULL)
	{
		sprintf(message, "Profilometer Name (%s) Not Found", profilometerName);
		EXCTHROW( -12, message);
	}
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometers}::GetProfilometerByIndex"
static SElExceptionPtr profilometers_GetProfilometerByIndex(
	struct _Profilometers*	me,
	int32_t				index,
	SProfilometerPtr*		profilometer
)
{
	SElExceptionPtr	pexception = NULL;
	
	if (index < me->numProfilometers && index >= 0)
	{
		if (profilometer)
			*profilometer = me->profilometers[index];
	}
	else
	{
		if (profilometer)
			*profilometer = NULL;
		
		EXCTHROW( -13, "Index Out of Range");
	}
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometers}::GetCurrentProfilometer"
static SElExceptionPtr profilometers_GetCurrentProfilometer(
	struct _Profilometers*	me,
	SProfilometerPtr*		profilometer
)
{
	SElExceptionPtr	pexception = NULL;
	
	if (me->currentIndex >= 0 && me->numProfilometers > 0)
	{
		if (profilometer)
			*profilometer = me->profilometers[me->currentIndex];
	}
	else
	{
		if (profilometer)
			*profilometer = NULL;
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometers}::SetCurrentProfilometerByName"
static SElExceptionPtr profilometers_SetCurrentProfilometerByName(
	struct _Profilometers*	me,
	const char*			profilometerName
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	SProfilometerPtr		profilometer = NULL;
	char			message[1024] = "";

	for (i = 0; i < me->numProfilometers; i++)
	{
		if (!strcmp(me->profilometers[i]->profilometerName, profilometerName))
		{
			me->currentIndex = i;
			break;
		}
	}
	
	if (i >= me->numProfilometers)
	{
		sprintf(message, "Profilometer Name (%s) Not Found", profilometerName);
		EXCTHROW( -12, message);
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometers}::GetFirstProfilometer"
static SElExceptionPtr profilometers_GetFirstProfilometer(
	struct _Profilometers*	me,
	SProfilometerPtr*		profilometer
)
{
	SElExceptionPtr	pexception = NULL;
	
	me->currentIndex = 0;

	if(me->numProfilometers > 0)
	{
		if (profilometer)
			*profilometer = me->profilometers[0];
	}
	else
	{
		if (profilometer)
			*profilometer = NULL;
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometers}::GetNextProfilometer"
static SElExceptionPtr profilometers_GetNextProfilometer(
	struct _Profilometers*	me,
	SProfilometerPtr*		profilometer
)
{
	SElExceptionPtr	pexception = NULL;

	if (++(me->currentIndex) >= me->numProfilometers)
	{
		me->currentIndex = me->numProfilometers - 1;
		if (profilometer)
			*profilometer = NULL;
	}
	else
	{
		if (profilometer)
			*profilometer = me->profilometers[me->currentIndex];
	}
	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometers}::InitProfilometers"
static SElExceptionPtr profilometers_InitProfilometers(struct _Profilometers* me)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr = S_OK;
	VARIANT_BOOL	is_xml_success = 1;
	VARIANT			file;
	CAObjHandle		document = 0;
	CAObjHandle		profilometer = 0;
	CAObjHandle		profilometer_name_node = 0;
	CAObjHandle		profilometer_list = 0;
	char*			xml_value = NULL;
	char*			profilometer_xml = NULL;
	
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &document));
	
	CA_VariantSetCString (&file, "modules/profilometers.xml");
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentload(document, NULL, file, &is_xml_success));
	
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectNodes(document, NULL, "/Profilometers/Profilometer", &profilometer_list));
	EXCCHECKCOM( MSXML_IXMLDOMNodeListreset (profilometer_list, NULL));
	
	EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode(profilometer_list, NULL, &profilometer));
	while (profilometer)
	{
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(profilometer, NULL, "@name", &profilometer_name_node));
		EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(profilometer_name_node, NULL, &xml_value));
		EXCCHECKCOM( MSXML_IXMLDOMNodeGetxml(profilometer, NULL, &profilometer_xml));
		
		me->profilometers = realloc( me->profilometers, (me->numProfilometers + 1) * sizeof(SProfilometerPtr));
		EXCCHECKALLOC( me->profilometers);
		
		EXCCHECK( profilometer_new( &me->profilometers[me->numProfilometers], xml_value, NULL));
		EXCCHECK( me->profilometers[me->numProfilometers]->Open(me->profilometers[me->numProfilometers], NULL, profilometer_xml));
		me->numProfilometers++;
		
		CA_FREE_MEMORY(xml_value);
		CA_FREE_MEMORY(profilometer_xml);
		DISCARD_HANDLE(profilometer_name_node);
		DISCARD_HANDLE(profilometer);
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode(profilometer_list, NULL, &profilometer));
	}
	
Error:
	CA_FREE_MEMORY(xml_value);
	CA_FREE_MEMORY(profilometer_xml);
	DISCARD_HANDLE(profilometer_name_node);
	DISCARD_HANDLE(profilometer);
	DISCARD_HANDLE(profilometer_list);
	DISCARD_HANDLE(document);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometers}::CloseProfilometers"
static SElExceptionPtr profilometers_CloseProfilometers(struct _Profilometers* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	
	for (i = 0; i < me->numProfilometers; i++)
	{
		EXCCHECK( profilometer_delete(&(me->profilometers[i])));
	}
	free(me->profilometers);
	me->profilometers = NULL;
	me->numProfilometers = 0;
	
Error:
	EXCRETHROW( pexception);
}
