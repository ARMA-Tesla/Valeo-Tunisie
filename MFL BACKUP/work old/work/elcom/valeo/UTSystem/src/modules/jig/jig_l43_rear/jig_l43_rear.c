#include "msxmldom.h"
#include <utility.h>
#include <guid.h>
#include <time.h>
#include <sensors/sensors.h>
#include "../jigSpecific.h"

#define TIMEOUT_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > ((double)timeout/1000.0) ) { EXCTHROW( JIG_ERROR_TIMEOUT, "Function Timeouted"); } else

#define MAX_FILE_NAME						1024
#define COLD_MARKING_LOOP					3
#define COLD_MARKING_TIME_UP				300

static char		gs_DllName[MAX_FILE_NAME] = "";
static int32_t	gs_inserted = 0;
static int32_t	gs_release = 0;
static int32_t	gs_locked = 0;
static int32_t	gs_unlocked = 0;
static int32_t	gs_connectorsUp = 0;
static int32_t	gs_connectorsDown = 0;
static int32_t	gs_position = 0;

#define MAX_LENGTH_SENSOR_LOGICAL_NAME				64

typedef struct _SLogicName
{
	char		aliasName[MAX_LENGTH_SENSOR_LOGICAL_NAME];
	char		logicalName[MAX_LENGTH_SENSOR_LOGICAL_NAME];
} SLogicName, *SLogicNamePtr;

typedef struct _SLogicNameList
{
	SLogicNamePtr	pLigicNames;
	int32_t			ListCount;
} SLogicNameList, *SLogicNameListPtr;

int CompareFnc(const void* v1, const void* v2);

/*---------------------------------------------------------------------------*/

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	char*		file_name = NULL;
	char*		p2ext = NULL;
	int32_t		error = 0,
				ret_size = 0,
				size = 1024;
	
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)
				return 0;	  /* out of memory */

			file_name = malloc((size + 1) * sizeof(char));
	
			ret_size = GetModuleFileName(hinstDLL, file_name, size);
			while (ret_size == size && ret_size != 0)
			{
				size += 1024;
				file_name = realloc(file_name, (size + 1) * sizeof(char));
				CHECKALLOC(file_name);
		
				ret_size = GetModuleFileName(hinstDLL, file_name, size);
			}
			
			p2ext = strrchr(file_name, '.');
			if (p2ext)
				*p2ext = '\0';
			
			sprintf(gs_DllName, "%s.xml", file_name);
			break;
			
		case DLL_PROCESS_DETACH:
			CloseCVIRTE ();
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
			_CrtDumpMemoryLeaks();
#endif
			break;
	}

Error:
	if (file_name)
		free(file_name);
	
	if (error < 0)
		return 0;
	
	return 1;
}

/*---------------------------------------------------------------------------*/

/* Initialize specific jig */
#undef __FUNC__
#define __FUNC__	"L43_REAR::Initialize"
SElExceptionPtr Initialize
(
	struct _SJig*	me,
	const char*		fileName
)
{
	SElExceptionPtr	pexception = NULL;
	CAObjHandle		xml_doc = 0;
	CAObjHandle		sensors_list = 0;
	CAObjHandle		sensor = 0;
	CAObjHandle		alias = 0;
	CAObjHandle		bcr_list = 0;
	CAObjHandle		barcode = 0;
	CAObjHandle		dll_name = 0;
	VBOOL			is_xml_success = 1;
	char*			alias_name = NULL;
	char*			logical_name = NULL;
	char*			config_xml = NULL;
	char*			dll_name_str = NULL;
	char			directory_path[FILE_NAME_SIZE] = "";
	char			bcr_dll_file[FILE_NAME_SIZE] = "";
	char			message[2048] = "";
	const char*     pfile_name = (fileName && *fileName != '\0') ? fileName : gs_DllName;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	SLogicNamePtr	plogical_list = NULL;
	int32_t			count = 0,
					error = 0;
	HRESULT			hr;
	
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &xml_doc));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_name));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (xml_doc, NULL, pfile_name_var, &is_xml_success));

	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectNodes (xml_doc, NULL,
						"//module[@id='"MODULE_JIG_L43_REAR"']/Sensors/Sensor",
						&sensors_list));
	EXCCHECKCOM( MSXML_IXMLDOMNodeListreset (sensors_list, NULL));
	EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode (sensors_list, NULL, &sensor));
	
	while (sensor != 0)
	{
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(sensor, NULL, "@name", &alias));
		EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (alias, NULL, &alias_name));
		EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (sensor, NULL, &logical_name));
		
		plogical_list = realloc(plogical_list, (count + 1) * sizeof(SLogicName));
		EXCCHECKALLOC( plogical_list);
		memset(plogical_list + count, 0, sizeof(SLogicName));
		strcpy_s((plogical_list + count)->aliasName, sizeof((plogical_list + count)->aliasName), alias_name);
		strcpy_s((plogical_list + count)->logicalName, sizeof((plogical_list + count)->logicalName), logical_name);
		count++;
		
		CA_FreeMemory(alias_name); alias_name = NULL;
		CA_FreeMemory(logical_name); logical_name = NULL;
		CA_DiscardObjHandle(sensor); sensor = 0;
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode (sensors_list, NULL, &sensor));
	}
	
	qsort(plogical_list, count, sizeof(SLogicName), CompareFnc);
	
	me->pLogicalNames = (void*) plogical_list;
	me->LogicalCounts = count;
	
	/* TODO: load barcode drivers */
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectNodes (xml_doc, NULL,
						"//module[@id='"MODULE_JIG_L43_REAR"']/BarCodeReaders/BarCodeReader",
						&bcr_list));
	if (bcr_list)
	{
		EXCCHECKCOM( MSXML_IXMLDOMNodeListreset (bcr_list, NULL));
		EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode (bcr_list, NULL, &barcode));
		while (barcode)
		{
			SBCRPtr	last_bcr = NULL;
			
			me->pBarCodeReader = realloc(me->pBarCodeReader, (me->BCRCounts + 1) * sizeof(SBCR));
			EXCCHECKALLOC(me->pBarCodeReader);
			last_bcr = me->pBarCodeReader + (me->BCRCounts * sizeof(SBCR));
			memset(last_bcr, 0, sizeof(SBCR));
			me->BCRCounts++;
			
			EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(barcode, NULL, "Driver/@name", &dll_name));
			if (dll_name != 0)
			{
				EXCCHECKCOM( MSXML_IXMLDOMNodeGettext (dll_name, NULL, &dll_name_str));
				EXCCHECKCVI( GetModuleDir (__CVIUserHInst, directory_path));
				sprintf(bcr_dll_file, "%s\\%s", directory_path, dll_name_str);
				last_bcr->dllHandle = LoadLibrary(bcr_dll_file);
				if (last_bcr->dllHandle == NULL)
				{
					sprintf(message, "Unable to load barcode reader driver\n(%s)", bcr_dll_file);
					EXCTHROW( JIG_ERROR_LOAD_LIBRARY, message);
				}
				
				last_bcr->bcr_new = (SElExceptionPtr (*)(SBarCodeReaderPtr*))
											GetProcAddress(last_bcr->dllHandle,
														   "bcr_new");
				last_bcr->bcr_delete = (SElExceptionPtr (*)(SBarCodeReaderPtr*))
											GetProcAddress(last_bcr->dllHandle,
														   "bcr_delete");
				
				EXCCHECKCOM( MSXML_IXMLDOMNodeGetxml (barcode, NULL, &config_xml));
			
				if (last_bcr->bcr_new)
				{
					last_bcr->bcr_new(&(last_bcr->reader));
					EXCCHECK( (last_bcr->reader)->Initialize(last_bcr->reader, config_xml));
					last_bcr->reader->eventFactory = me->eventFactory;
				}
				
				CA_FreeMemory(dll_name_str); dll_name_str = NULL;
				CA_FreeMemory(config_xml); config_xml = NULL;
				CA_DiscardObjHandle(dll_name); dll_name = 0;
			}
			CA_DiscardObjHandle(barcode); barcode = 0;
			EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode (bcr_list, NULL, &barcode));
		}
	}
	
Error:
	if (barcode) CA_DiscardObjHandle(barcode);	
	if (dll_name) CA_DiscardObjHandle(dll_name);
	if (bcr_list) CA_DiscardObjHandle(bcr_list);	
	if (alias) CA_DiscardObjHandle(alias);	
	if (sensor) CA_DiscardObjHandle(sensor);	
	if (sensors_list) CA_DiscardObjHandle(sensors_list);	
	if (xml_doc) CA_DiscardObjHandle(xml_doc);	
	if (dll_name_str) CA_FreeMemory(dll_name_str);	
	if (config_xml) CA_FreeMemory(config_xml);	
	if (alias_name) CA_FreeMemory(alias_name);	
	if (logical_name) CA_FreeMemory(logical_name);	
	CA_VariantClear(&pfile_name_var);
	
	if (pexception)
	{
		free(plogical_list);
		plogical_list = NULL;
		me->pLogicalNames = NULL;
		me->LogicalCounts = 0;
	}
	
	EXCRETHROW(pexception);
}

/* Destroy specific jig */
#undef __FUNC__
#define __FUNC__	"L43_REAR::Destroy"
SElExceptionPtr Destroy(struct _SJig* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	
	for (i = 0; i < me->BCRCounts; i++)
	{
		if (me->pBarCodeReader[i].reader != NULL)
		{
			EXCCHECK( (me->pBarCodeReader[i].reader)->Close(me->pBarCodeReader[i].reader));
			EXCCHECK( me->pBarCodeReader[i].bcr_delete(&(me->pBarCodeReader[i].reader)));
		}
		if (me->pBarCodeReader[i].dllHandle != 0)
			FreeLibrary( me->pBarCodeReader[i].dllHandle);
	}
	free(me->pBarCodeReader);
	me->pBarCodeReader = NULL;
	me->BCRCounts = 0;
	
	free((SLogicNamePtr) me->pLogicalNames);
	me->pLogicalNames = NULL;
	me->LogicalCounts = 0;
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"L43_REAR::GetUUID"
SElExceptionPtr GetUUID(struct _SJig* me, char* uuid, uint32_t size)
{
	SElExceptionPtr	pexception = NULL;
	
	strncpy_s(uuid, size, MODULE_JIG_L43_REAR, size);

Error:
	EXCRETHROW(pexception);
}

/* Lock product */
#undef __FUNC__
#define __FUNC__	"L43_REAR::LockItem"
SElExceptionPtr LockItem
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	
	EXCCHECK( me->jig_SetSensor(me, "LockValve", 1));
	
	if (timeout)
	{
		if(gs_locked && !gs_unlocked) goto Error;		
		EXCCHECK( me->eventFactory->GetEvent(me->eventFactory, EVNT_JIG_ITEM_LOCK, &hevent));	
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_locked && !gs_unlocked) goto Error;
			EXCTHROW( JIG_ERROR_TIMEOUT, "Function Timeouted");
		}
	}

Error:
	EXCRETHROW(pexception);
}

/* Unlock product */
#undef __FUNC__
#define __FUNC__	"L43_REAR::UnlockItem"
SElExceptionPtr UnlockItem
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventFactory->GetEvent(me->eventFactory, EVNT_JIG_ITEM_UNLOCK, &hevent));

	EXCCHECK( me->jig_SetSensor(me, "LockValve", 0));

	if (timeout)
	{
		if(!gs_locked && gs_unlocked) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(!gs_locked && gs_unlocked) goto Error;
			EXCTHROW( JIG_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Connectors connect to product */
#undef __FUNC__
#define __FUNC__	"L43_REAR::ConnectToItem"
SElExceptionPtr ConnectToItem
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventFactory->GetEvent(me->eventFactory, EVNT_JIG_ITEM_CONNECTED, &hevent));

	EXCCHECK( me->jig_SetSensor(me, "ConnectValve", 1));
	EXCCHECK( me->jig_SetSensor(me, "ConnectorCounter", 1));

	if (timeout)
	{
		if(!gs_connectorsDown && gs_connectorsUp) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(!gs_connectorsDown && gs_connectorsUp) goto Error;
			EXCTHROW( JIG_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Connectors disconnect from product */
#undef __FUNC__
#define __FUNC__	"L43_REAR::DisconnectFromItem"
SElExceptionPtr DisconnectFromItem
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventFactory->GetEvent(me->eventFactory, EVNT_JIG_ITEM_DISCONNECTED, &hevent));

	EXCCHECK( me->jig_SetSensor(me, "ConnectValve", 0));
	EXCCHECK( me->jig_SetSensor(me, "ConnectorCounter", 0));

	if (timeout)
	{
		if(gs_connectorsDown && !gs_connectorsUp) goto Error;	
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_connectorsDown && !gs_connectorsUp) goto Error;
			EXCTHROW( JIG_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Connectors disconnect from product */
#undef __FUNC__
#define __FUNC__	"L43_REAR::ColdMarking"
SElExceptionPtr ColdMarking
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
SElExceptionPtr	pexception = NULL;
	uint32_t		state_marking_on = 0,
					state_marking_off = 0;
	int32_t			count = 0;

	TIMEOUT_INIT();
	
	while(count++<COLD_MARKING_LOOP)
	{
		state_marking_on = 0;
		state_marking_off = 0;

		EXCCHECK( me->jig_SetSensor(me, "ColdMarking", 1));
	
		while(!state_marking_on)
		{
			EXCCHECK( me->jig_GetSensor(me, "IsMarking_On", &state_marking_on));
			TIMEOUT_CHECK( timeout);
			Sleep(50);
		}
		
		EXCCHECK( me->jig_SetSensor(me, "ColdMarking", 0));
		while(!state_marking_off)
		{
			EXCCHECK( me->jig_GetSensor(me, "IsMarking_Off", &state_marking_off));
			TIMEOUT_CHECK( timeout);
			Sleep(50);
		}	
	};

Error:
	EXCRETHROW(pexception);
}

/* Move JIG to specified position */
#undef __FUNC__
#define __FUNC__	"L43_REAR::SetItemPosition"
SElExceptionPtr SetItemPosition
(
	struct _SJig*	me,
	const char*		product,
	int32_t			position,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;

	switch(position)
	{
		case JIG_POSITION_INSERT:
			EXCCHECK( me->jig_SetSensor(me, "PanelPosition_0", 0));
			EXCCHECK( me->jig_SetSensor(me, "PanelPosition_1", 1));

			/* TODO: wait for sensors */
			break;
		case JIG_POSITION_HORIZONTAL:
			EXCCHECK( me->jig_SetSensor(me, "PanelPosition_1", 0));
			EXCCHECK( me->jig_SetSensor(me, "PanelPosition_0", 1));

			/* TODO: wait for sensors */
			break;
	}

Error:
	EXCRETHROW(pexception);
}

/* Get JIG position */
#undef __FUNC__
#define __FUNC__	"L43_REAR::GetItemPosition"
SElExceptionPtr GetItemPosition
(
	struct _SJig*	me,
	const char*		product,
	int32_t*		actualPosition
)
{
	if(actualPosition) *actualPosition = gs_position;

	return NULL;
}

/* Get bar code indentification of panel */
#undef __FUNC__
#define __FUNC__	"L43_REAR::GetItemCode"
SElExceptionPtr GetItemCode
(
	struct _SJig*	me,
	const char*		product,
	int32_t			index,
	long			bufferSize,
	char*			buffer,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			barcode_event = 0;
	
	if (me->pBarCodeReader->reader)
	{
		EXCCHECK( (me->pBarCodeReader->reader)->StartReading(me->pBarCodeReader->reader));
		/* TODO: Wait for barcode is read or timeout */
		EXCCHECK( me->eventFactory->GetEvent(me->eventFactory, "EVNT_BARCODE_READ", &barcode_event));
		
		if (WaitForSingleObject( barcode_event, timeout) != WAIT_TIMEOUT)
		{
			EXCCHECK( (me->pBarCodeReader->reader)->GetBarCode(me->pBarCodeReader->reader, buffer, bufferSize));
		}
		else
		{
			if (timeout)
			{
				EXCTHROW( JIG_ERROR_TIMEOUT, "Function Timeouted");
			}
		}
	}

Error:
	if (me->pBarCodeReader->reader)
		(me->pBarCodeReader->reader)->StopReading(me->pBarCodeReader->reader);
	EXCRETHROW(pexception);
}

/* Low-level function get sensor state directly */
#undef __FUNC__
#define __FUNC__	"L43_REAR::GetSensor"
SElExceptionPtr GetSensor
(
	struct _SJig*	me,
	const char*		sensorId,
	uint32_t*		value
)
{
	SElExceptionPtr	pexception = NULL;
	SLogicName		tmp;
	SLogicNamePtr	palias_pair = NULL;
	char*			plogical_name = NULL;
	SDIOSensorsPtr	pSensor = (SDIOSensorsPtr)me->sensorAPI;
	
	/* TODO: convert sensorId to logical name in tester */
	strcpy_s(tmp.aliasName, sizeof(tmp.aliasName), sensorId);
	
	palias_pair = bsearch( &tmp,
						   me->pLogicalNames,
						   me->LogicalCounts,
						   sizeof(SLogicName),
						   CompareFnc
						 );
	if (value == NULL)
	{
		EXCTHROW(JIG_ERROR_INVALID_PARAMETER, "Parameter value is NULL");
	}
	plogical_name = (palias_pair) ? palias_pair->logicalName : (char*) sensorId;
	
	EXCCHECK( pSensor->GetState(pSensor, plogical_name, value));

Error:
	EXCRETHROW(pexception);
}

/* Low-level function set sensor state directly */
#undef __FUNC__
#define __FUNC__	"L43_REAR::SetSensor"
SElExceptionPtr SetSensor
(
	struct _SJig*	me,
	const char*		sensorId,
	uint32_t		value
)
{
	SElExceptionPtr	pexception = NULL;
	char*			plogic_name = "";
	SLogicName		tmp;
	SLogicNamePtr	palias_pair = NULL;
	char*			plogical_name = NULL;
	SDIOSensorsPtr	pSensor = (SDIOSensorsPtr)me->sensorAPI;
	
	/* convert sensorId to logical name in tester */
	strcpy_s(tmp.aliasName, sizeof(tmp.aliasName), sensorId);
	
	palias_pair = bsearch( &tmp,
						   me->pLogicalNames,
						   me->LogicalCounts,
						   sizeof(SLogicName),
						   CompareFnc
						 );
	plogical_name = (palias_pair) ? palias_pair->logicalName : (char*) sensorId;
	EXCCHECK( pSensor->SetState(pSensor, plogical_name, value));

Error:
	EXCRETHROW(pexception);
}

/* Get Attribute Int32 */
#undef __FUNC__
#define __FUNC__	"L43_REAR::GetAttributeInt32"
SElExceptionPtr GetAttributeInt32
(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int32_t*		value
)
{
	SElExceptionPtr	pexception = NULL;

	/* TODO: convert JIG virtual name to Tester logical name or physical name */
	
Error:
	EXCRETHROW(pexception);
}

/* Set Attribute Int32 */
#undef __FUNC__
#define __FUNC__	"L43_REAR::SetAttributeInt32"
SElExceptionPtr SetAttributeInt32
(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int32_t			value
)
{
	SElExceptionPtr	pexception = NULL;

	switch (attributeId)
	{
	case JIG_ATTR_HWND:
		hWnd = (HANDLE) value;
		break;

	case JIG_ATTR_IS_PANEL_UNLOCKED:
	case JIG_ATTR_IS_PANEL_INSERTED:
	case JIG_ATTR_IS_PANEL_RELEASED:
	case JIG_ATTR_IS_CONNECTORS_UP:
	case JIG_ATTR_IS_CONNECTORS_DOWN:
	case JIG_ATTR_IS_JIG_LOADED:
	case JIG_ATTR_IS_JIG_SETTINGS_OK:
		EXCTHROW( JIG_ERROR_ATTR_INVALID_TYPE, "Invalid Type of Attribute");
		break;
		
	default:
		EXCTHROW( JIG_ERROR_ATTR_UNKNOWN, "Unknown Attribute");
		break;
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Get Attribute Boolean */
#undef __FUNC__
#define __FUNC__	"L43_REAR::GetAttributeBoolean"
SElExceptionPtr GetAttributeBoolean
(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int8_t*			value
)
{
	SElExceptionPtr	pexception = NULL;

Error:
	EXCRETHROW(pexception);
}

/* Set Attribute Boolean */
#undef __FUNC__
#define __FUNC__	"L43_REAR::SetAttributeBoolean"
SElExceptionPtr SetAttributeBoolean
(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int8_t			value
)
{
	SElExceptionPtr	pexception = NULL;

Error:
	EXCRETHROW(pexception);
}

int CompareFnc(const void* v1, const void* v2)
{
	SLogicNamePtr	value1 = (SLogicNamePtr) v1;
	SLogicNamePtr	value2 = (SLogicNamePtr) v2;
	
	return strcmp(value1->aliasName, value2->aliasName);
}

#undef __FUNC__
#define __FUNC__	"L43_REAR::JigThread"
SElExceptionPtr JigThread
(
	struct _SJig*	me
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			insert1 = 0,
					insert2 = 0,
					lock1 = 0,
					lock2 = 0,
					lock3 = 0,
					lock4 = 0;
	
	/* Check if pannel is inserted or released */
	EXCCHECK( me->jig_GetSensor(me, "IsPanelInserted_1", &insert1));
	EXCCHECK( me->jig_GetSensor(me, "IsPanelInserted_2", &insert2));
	if (insert1 && insert2)
	{
		if (me->eventFactory && !gs_inserted)
			EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_JIG_ITEM_INSERTED, NULL));
		
		gs_inserted = 1;
	}
	else
	{
		gs_inserted = 0;
	}
	
	if (!insert1 && !insert2)
	{
		if (me->eventFactory && !gs_release)
			EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_JIG_ITEM_RELEASED, NULL));
		
		gs_release = 1;
	}
	else
	{
		gs_release = 0;
	}
	
	/* Check if panel is locked */
	EXCCHECK( me->jig_GetSensor(me, "IsPanelLock_Left_Up", &lock1));
	EXCCHECK( me->jig_GetSensor(me, "IsPanelLock_Left_Down", &lock2));
	EXCCHECK( me->jig_GetSensor(me, "IsPanelLock_Right_Up", &lock3));
	EXCCHECK( me->jig_GetSensor(me, "IsPanelLock_Right_Down", &lock4));
	if (lock1 && lock2 && lock3 && lock4)
	{
		if (me->eventFactory && !gs_locked)
			EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_JIG_ITEM_LOCK, NULL));
		
		gs_locked = 1;
	}
	else
	{
		gs_locked = 0;
	}
	
	/* Check if panel is unlocked */
	EXCCHECK( me->jig_GetSensor(me, "IsPanelUnlock_Left_Up", &lock1));
	EXCCHECK( me->jig_GetSensor(me, "IsPanelUnlock_Left_Down", &lock2));
	EXCCHECK( me->jig_GetSensor(me, "IsPanelUnlock_Right_Up", &lock3));
	EXCCHECK( me->jig_GetSensor(me, "IsPanelUnlock_Right_Down", &lock4));
	if (lock1 && lock2 && lock3 && lock4)
	{
		if (me->eventFactory && !gs_unlocked)
			EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_JIG_ITEM_UNLOCK, NULL));
		
		gs_unlocked = 1;
	}
	else
	{
		gs_unlocked = 0;
	}
	
	/* Check if Connectors are up */
	EXCCHECK( me->jig_GetSensor(me, "IsConnectorsUp", &lock1));
	EXCCHECK( me->jig_GetSensor(me, "IsConnectorsUp_2", &lock2));
	if (lock1 && lock2)
	{
		if (me->eventFactory && !gs_connectorsUp)
			EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_JIG_ITEM_CONNECTED, NULL));
		
		gs_connectorsUp = 1;
	}
	else
	{
		gs_connectorsUp = 0;
	}

	/* Check if Connectors are ddown */
	EXCCHECK( me->jig_GetSensor(me, "IsConnectorsDown", &lock1));
	EXCCHECK( me->jig_GetSensor(me, "IsConnectorsDown_2", &lock2));
	if (lock1 && lock2)
	{
		if (me->eventFactory && !gs_connectorsDown)
			EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_JIG_ITEM_DISCONNECTED, NULL));
		
		gs_connectorsDown = 1;
	}
	else
	{
		gs_connectorsDown = 0;
	}
	
Error:
	EXCRETHROW(pexception);
}
