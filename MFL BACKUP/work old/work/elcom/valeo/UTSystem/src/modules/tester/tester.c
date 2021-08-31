#include "tester.h"
#include "msxmldom.h"
#include <guid.h>
#include <time.h>
#include <utility.h>
#include <sdk/mmsystem.h>
#include <multithread/multithread.h>
#include <userint.h>
#include <ansi_c.h>
#include "KeyenceLaser.h" 
#include "lasers.h"

CRITICAL_SECTION   _Lock;


#define MAX_LENGTH_SENSOR_LOGICAL_NAME				64

#define TIMEOUT_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_CHECK(timeout) \
	if ( timeout>0 && ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout"); } else

#define NUMBER_OF_STATIONS	  8+1

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2

#define PASS  1
#define FAIL  0


typedef enum _EStation
{
	STATION_10 = 0,
	STATION_20,
	STATION_30,
	STATION_40,
	STATION_50,
	STATION_60,
	STATION_70,
	STATION_80,
} EStation;

static int32_t	gs_inserted = 0;
static int32_t	gs_released = 0;
static int32_t	gs_locked_station_10 = 0;
static int32_t	gs_unlocked_station_10 = 0;

static int32_t	gs_started_rodage_station_20 = 0;
static int32_t	gs_stopped_rodage_station_20 = 0;

static int32_t	gs_plugged_station_30 = 0;
static int32_t	gs_unplugged_station_30 = 0;

static int32_t	gs_plugged_station_40 = 0;
static int32_t	gs_unplugged_station_40 = 0;

static int32_t	gs_started_rodage_left_station_50 = 0;
static int32_t	gs_stopped_rodage_left_station_50 = 0;
static int32_t	gs_started_rodage_right_station_50 = 0;
static int32_t	gs_stopped_rodage_right_station_50 = 0;

static int32_t	gs_plugged_station_60 = 0;
static int32_t	gs_unplugged_station_60 = 0;
static int32_t	gs_started_test_left_station_60 = 0;
static int32_t	gs_stopped_test_left_station_60 = 0;
static int32_t	gs_started_test_right_station_60 = 0;
static int32_t	gs_stopped_test_right_station_60 = 0;

static int32_t	gs_plugged_station_70 = 0;
static int32_t	gs_unplugged_station_70 = 0;

static int32_t	gs_locked_station_80 = 0;
static int32_t	gs_unlocked_station_80 = 0;
static int32_t	gs_barcode_already_front_station_80 = 0;
static int32_t	gs_barcode_already_rear_station_80 = 0;

static int32_t	gs_table_started_rotation = -1;
static int32_t	gs_table_finished_rotation = -1;

#define SYSTEM_AXIS_STATION_30_RIGHT  	0
#define SYSTEM_AXIS_STATION_30_LEFT  	1
#define SYSTEM_AXIS_STATION_40_RIGHT  	2
#define SYSTEM_AXIS_STATION_40_LEFT  	3

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2


char Table_Ready[NUMBER_OF_STATIONS+1];
typedef enum _EStep
{
	CHECK_TABLE_POITION = 0,
	CHECK_TABLE_IN_ROTATION,
	CHECK_IF_ROTATION_FINIHED,
	ROTATION_FINIHED_OK,
} EStep;

typedef enum _EStepPickPlace
{
	INIT = 0,
	MOVE_TO_PALETT,
	MOVE_TO_PANEL,
	TAKE_ON_PANEL,
	PICK_UP_PANEL,
	MOVE_TO_BOX,
	MOVE_DOWN_TO_BOX,
	TAKE_OFF_PANEL,
	PICK_PLACE_FINIHED,
} EStepPickPlace;

typedef struct _SLogicName
{
	char		aliasName[MAX_LENGTH_SENSOR_LOGICAL_NAME+1];
	char		logicalName[MAX_LENGTH_SENSOR_LOGICAL_NAME+1];
} SLogicName, *SLogicNamePtr;

typedef struct _SLogicNameList
{
	SLogicNamePtr	pLigicNames;
	int32_t			ListCount;
} SLogicNameList, *SLogicNameListPtr;

/* Tester */
static SElExceptionPtr Initialize(struct _STester*	me, const char*	fileName);
static SElExceptionPtr RegisterSensorCallbacks( struct _STester* me);
static SElExceptionPtr TableRotate(struct _STester* me, int32_t timeout, bool_t wait_end);
static SElExceptionPtr GetStationStatus(struct _STester* me, char status[9]);
static SElExceptionPtr WaitRotationFinished(struct _STester* me, int32_t timeout);
static SElExceptionPtr OkNokCheck(struct _STester* me, int32_t timeout, bool_t* ok);
static SElExceptionPtr NokCheck(struct _STester* me, int32_t timeout);
static SElExceptionPtr SetSensor(struct _STester* me, const char* sensorId, uint32_t value);
static SElExceptionPtr GetSensor(struct _STester* me, const char* sensorId, uint32_t* value);
static SElExceptionPtr SetSensorCallback(struct _STester* me, const char* sensorId, SensorCallbackFnc_t fCall, void* pData);
static SElExceptionPtr CallbackFncButtonOk(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonNok(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonMenu(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonLabel(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonDowntime(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncAbis(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonStart(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncJigNew(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncRotation(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncRotationStarted(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonTotalStop(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncLaserEnd(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncRedBoxEmpty(const char* name, uint32_t state, void* pData);
static SElExceptionPtr InitializeStations(struct _STester* me, int32_t station, int32_t timeout);
//#if 0    /* formerly excluded lines */
static SElExceptionPtr Activate_Axis(struct _STester* me);
static SElExceptionPtr Disactivate_Axis(struct _STester* me);
//#endif   /* formerly excluded lines */
/* Station 10 */
static SElExceptionPtr LockPanelStation10(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnlockPanelStation10(struct _STester* me, int32_t timeout);
/* Station 20 */
static SElExceptionPtr StartRodageStation20(struct _STester* me, int32_t timeout);
static SElExceptionPtr StopRodageStation20(struct _STester* me, int32_t timeout);
static SElExceptionPtr StatusPushsStation20(struct _STester* me, char status[7+1]);
static SElExceptionPtr RodageStation20(struct _STester* me, int32_t timeout, int32_t cycle, int32_t timeon, int32_t timeoff, char version[15+1]);
/* Station 30 */
static SElExceptionPtr PlugPanelStation30(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation30(struct _STester* me, int32_t timeout);
/* Station 40 */
static SElExceptionPtr PlugPanelStation40(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation40(struct _STester* me, int32_t timeout);
/* Station 50 */
static SElExceptionPtr StartRodageLeftStation50(struct _STester* me, int32_t timeout);
static SElExceptionPtr StopRodageLeftStation50(struct _STester* me, int32_t timeout);
static SElExceptionPtr StartRodageRightStation50(struct _STester* me, int32_t timeout);
static SElExceptionPtr StopRodageRightStation50(struct _STester* me, int32_t timeout);
/* Station 60 */
static SElExceptionPtr PlugPanelStation60(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation60(struct _STester* me, int32_t timeout);
static SElExceptionPtr StartTestLeftStation60(struct _STester* me, int32_t timeout);
static SElExceptionPtr StopTestLeftStation60(struct _STester* me, int32_t timeout);
static SElExceptionPtr StartTestRightStation60(struct _STester* me, int32_t timeout);
static SElExceptionPtr StopTestRightStation60(struct _STester* me, int32_t timeout);
/* Station 70 */
static SElExceptionPtr PlugPanelStation70(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation70(struct _STester* me, int32_t timeout);
static SElExceptionPtr IlluminationStation70(struct _STester* me, int32_t timeout);
/* Station 80 */
static SElExceptionPtr LockPanelStation80(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnlockPanelStation80(struct _STester* me, int32_t timeout);
static SElExceptionPtr PickPlace(struct _STester* me, int32_t timeout, bool_t wait_end);
static SElExceptionPtr IsProductFaultTook(struct _STester* me, int32_t timeout);
static SElExceptionPtr ResetPickPlace(struct _STester* me, int32_t timeout, bool_t wait_end);
static SElExceptionPtr IsResetPickPlace(struct _STester* me, int32_t timeout);
static SElExceptionPtr MoveBarcodeFront(struct _STester* me, int32_t timeout);
static SElExceptionPtr MoveBarcodeRear(struct _STester* me, int32_t timeout);
static SElExceptionPtr CycleLaser(struct _STester* me, int32_t timeout);
static SElExceptionPtr GetItemCode(struct _STester* me, const char* product, int32_t index, long bufferSize, char* buffer,int32_t timeout);
static SElExceptionPtr GetStatusRedBox(struct _STester* me, int32_t	*full, int32_t	*closed);


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "fncLock"
static SElExceptionPtr fncLock(void)
{
   EnterCriticalSection( &_Lock);
   return NULL;
} 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "fncUnlock"
static SElExceptionPtr fncUnlock(void)
{
   LeaveCriticalSection( &_Lock);
   return NULL;
} 

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "tester_new"
 SElExceptionPtr tester_new(STesterPtr*	pTester)
{
	SElExceptionPtr    	pexception = NULL;
	int32_t				error = 0;

	*pTester = calloc( 1, sizeof(STester));
	EXCCHECKALLOC( *pTester);

	/* Tester */
	(*pTester)->Initialize = Initialize;
	(*pTester)->SetSensor = SetSensor;
	(*pTester)->GetSensor = GetSensor;
	(*pTester)->TableRotate = TableRotate;
	(*pTester)->GetStationStatus = GetStationStatus;
	(*pTester)->InitializeStations = InitializeStations;
	(*pTester)->WaitRotationFinished = WaitRotationFinished;
	(*pTester)->OkNokCheck = OkNokCheck;
	(*pTester)->NokCheck = NokCheck;
	(*pTester)->RegisterSensorCallbacks = RegisterSensorCallbacks;
	(*pTester)->SetSensorCallback = SetSensorCallback;
//#if 0    /* formerly excluded lines */
	(*pTester)->Activate_Axis = Activate_Axis;
	(*pTester)->Disactivate_Axis = Disactivate_Axis;
//#endif   /* formerly excluded lines */
	/* Station 10 */
	(*pTester)->LockPanelStation10 = LockPanelStation10;
	(*pTester)->UnlockPanelStation10 = UnlockPanelStation10;
	/* Station 20 */
	(*pTester)->StartRodageStation20 = StartRodageStation20;
	(*pTester)->StopRodageStation20 = StopRodageStation20;
	(*pTester)->StatusPushsStation20 = StatusPushsStation20;
	(*pTester)->RodageStation20 = RodageStation20;
	/* Station 30 */
	(*pTester)->PlugPanelStation30 = PlugPanelStation30;
	(*pTester)->UnplugPanelStation30 = UnplugPanelStation30;
	/* Station 40 */
	(*pTester)->PlugPanelStation40 = PlugPanelStation40;
	(*pTester)->UnplugPanelStation40 = UnplugPanelStation40;
	/* Station 50 */
	(*pTester)->StartRodageLeftStation50 = StartRodageLeftStation50;
	(*pTester)->StopRodageLeftStation50 = StopRodageLeftStation50;
	(*pTester)->StartRodageRightStation50 = StartRodageRightStation50;
	(*pTester)->StopRodageRightStation50 = StopRodageRightStation50;
	/* Station 60 */
	(*pTester)->PlugPanelStation60 = PlugPanelStation60;
	(*pTester)->UnplugPanelStation60 = UnplugPanelStation60;
	(*pTester)->StartTestLeftStation60 = StartTestLeftStation60;
	(*pTester)->StopTestLeftStation60 = StopTestLeftStation60;
	(*pTester)->StartTestRightStation60 = StartTestRightStation60;
	(*pTester)->StopTestRightStation60 = StopTestRightStation60;
	/* Station 70 */
	(*pTester)->PlugPanelStation70 = PlugPanelStation70;
	(*pTester)->UnplugPanelStation70 = UnplugPanelStation70;
	(*pTester)->IlluminationStation70 = IlluminationStation70;
	/* Station 80 */
	(*pTester)->CycleLaser = CycleLaser; 
	(*pTester)->PickPlace = PickPlace; 
	(*pTester)->IsProductFaultTook = IsProductFaultTook;
	(*pTester)->ResetPickPlace = ResetPickPlace; 
	(*pTester)->IsResetPickPlace = IsResetPickPlace;
	(*pTester)->MoveBarcodeFront = MoveBarcodeFront;
	(*pTester)->MoveBarcodeRear = MoveBarcodeRear;
	(*pTester)->GetItemCode = GetItemCode;
	(*pTester)->GetStatusRedBox = GetStatusRedBox;
	(*pTester)->LockPanelStation80 = LockPanelStation80;
	(*pTester)->UnlockPanelStation80 = UnlockPanelStation80;
	
Error:
	EXCRETHROW( pexception); 
}/* vline_new */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "tester_delete"
SElExceptionPtr	tester_delete(STesterPtr* pTester)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			status;

	if ( pTester && *pTester )
	{
		(*pTester)->eventAPI = NULL;
		(*pTester)->sensorAPI = NULL;

		if((*pTester)->hThreadTableRotate)
		{
			EXCCHECKWIN( TerminateThread((*pTester)->hThreadTableRotate, 0));
			CloseHandle((*pTester)->hThreadTableRotate);
		}
		
		if((*pTester)->hThreadPickPlace)
		{
			EXCCHECKWIN( TerminateThread((*pTester)->hThreadPickPlace, 0));
			CloseHandle((*pTester)->hThreadPickPlace);
		}

		free( (*pTester)->pLogicalNames);
		(*pTester)->pLogicalNames = NULL;
		(*pTester)->LogicalCounts = 0;
		
		free( *pTester);
		*pTester = NULL;
	}

Error:
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::RegisterSensorCallbacks"
static SElExceptionPtr RegisterSensorCallbacks( struct _STester* me)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( me->SetSensorCallback(me, "BUTTON_OK", CallbackFncButtonOk, me));
	//EXCCHECK( me->SetSensorCallback(me, "BUTTON_OK", CallbackFncButtonStart, me));
	EXCCHECK( me->SetSensorCallback(me, "BUTTON_NOK", CallbackFncButtonNok, me));
	EXCCHECK( me->SetSensorCallback(me, "BUTTON_MENU", CallbackFncButtonMenu, me));
	//EXCCHECK( me->SetSensorCallback(me, "BUTTON_LABEL", CallbackFncButtonLabel, me));
	//EXCCHECK( me->SetSensorCallback(me, "BUTTON_DOWNTIME", CallbackFncButtonDowntime, me));
	EXCCHECK( me->SetSensorCallback(me, "BUTTON_START", CallbackFncButtonStart, me));
	EXCCHECK( me->SetSensorCallback(me, "SENSOR_PANEL_LOCKED_STATION_10", CallbackFncJigNew, me));
	EXCCHECK( me->SetSensorCallback(me, "MICRO_CAM", CallbackFncRotation, me));
	EXCCHECK( me->SetSensorCallback(me, "BUTTON_TOTAL_STOP", CallbackFncButtonTotalStop, me));
	EXCCHECK( me->SetSensorCallback(me, "SELECTEUR_ABRIS", CallbackFncAbis, me));
	//EXCCHECK( me->SetSensorCallback(me, "SENSOR_RED_BOX_FULL", CallbackFncRedBoxEmpty, me));
	EXCCHECK( me->SetSensorCallback(me, "LASER_WORK", CallbackFncLaserEnd, me));

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
static int CompareFnc(const void* v1, const void* v2)
{
	SLogicNamePtr	value1 = (SLogicNamePtr) v1;
	SLogicNamePtr	value2 = (SLogicNamePtr) v2;
	
	return strcmp(value1->aliasName, value2->aliasName);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::Initialize"
static SElExceptionPtr Initialize
(
	struct _STester*	me,
	const char*		fileName
)
{
	SElExceptionPtr	pexception = NULL;
	CAObjHandle		xml_doc = 0;
	CAObjHandle		sensors_list = 0;
	CAObjHandle		sensor = 0;
	CAObjHandle		alias = 0;
	VBOOL			is_xml_success = 1;
	char*			alias_name = NULL;
	char*			logical_name = NULL;
	const char*     pfile_name = (fileName && fileName[0] != '\0') ? fileName : "modules\\tester.xml";
	char			module_dir[512];
	VARIANT         pfile_name_var = CA_VariantEmpty();
	SLogicNamePtr	plogical_list = NULL;
	int32_t			count = 0,
					error = 0;
	HRESULT			hr;
	char			sNode[512];
	int             iStation;
	CAObjHandle		bcr_list = 0;
	CAObjHandle		barcode = 0;
	CAObjHandle		dll_name = 0;
	char*			dll_name_str = NULL;
	char			directory_path[FILE_NAME_SIZE] = "";
	char			bcr_dll_file[FILE_NAME_SIZE] = "";
	char			message[2048] = "";
	char*			config_xml = NULL;
	int size;
	
	
	
	InitializeCriticalSection( &_Lock);

	strcpy( me->path, pfile_name);
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &xml_doc));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_name));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (xml_doc, NULL, pfile_name_var, &is_xml_success));

	/* Tester */
	sprintf(sNode, "//module[@id='"MODULE_TESTER"']/Sensors/Sensor");
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectNodes (xml_doc, NULL, sNode, &sensors_list));
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
		strncpy((plogical_list + count)->aliasName, alias_name, MAX_LENGTH_SENSOR_LOGICAL_NAME);
		strncpy((plogical_list + count)->logicalName, logical_name, MAX_LENGTH_SENSOR_LOGICAL_NAME);
		count++;
		
		CA_FreeMemory(alias_name); alias_name = NULL;
		CA_FreeMemory(logical_name); logical_name = NULL;
		CA_DiscardObjHandle(sensor); sensor = 0;
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode (sensors_list, NULL, &sensor));
	}
	/* Stations */
	for (iStation = 1 ; iStation < NUMBER_OF_STATIONS ; iStation++)
	{
	sprintf(sNode, "//module[@id='"MODULE_TESTER"']/Station%d/Sensor", iStation);
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectNodes (xml_doc, NULL,sNode,&sensors_list));
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
		strncpy((plogical_list + count)->aliasName, alias_name, MAX_LENGTH_SENSOR_LOGICAL_NAME);
		strncpy((plogical_list + count)->logicalName, logical_name, MAX_LENGTH_SENSOR_LOGICAL_NAME);
		count++;
		
		CA_FreeMemory(alias_name); alias_name = NULL;
		CA_FreeMemory(logical_name); logical_name = NULL;
		CA_DiscardObjHandle(sensor); sensor = 0;
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode (sensors_list, NULL, &sensor));
	}
	}
	
	qsort(plogical_list, count, sizeof(SLogicName), CompareFnc);
	
	me->pLogicalNames = (void*) plogical_list;
	me->LogicalCounts = count;
	
/* TODO: load barcode drivers */
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectNodes (xml_doc, NULL,
						"//module[@id='"MODULE_TESTER"']/BarCodeReaders/BarCodeReader",
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
			last_bcr = me->pBarCodeReader + (me->BCRCounts);
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
					EXCTHROW( TESTER_ERROR_LOAD_LIBRARY, message);
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
					last_bcr->reader->eventFactory = me->eventAPI;
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
	if (alias) CA_DiscardObjHandle(alias);
	if (sensor) CA_DiscardObjHandle(sensor);
	if (sensors_list) CA_DiscardObjHandle(sensors_list);
	if (xml_doc) CA_DiscardObjHandle(xml_doc);
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

typedef struct _SMaskThreadData{
	struct _STester*	me;
	int32_t				timeout;
} SMaskThreadData, *SMaskThreadDataPtr;


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::TableRotate"
static SElExceptionPtr TableRotate(struct _STester* me, int32_t timeout, bool_t wait_end)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
		uint32_t		step = 0;
	
	bool_t			status_barrier = FALSE,
					status_micro_cam = FALSE,
					is_table_rotation_finished = FALSE;
	

	uint32_t		is_panel_locked_station_10,
					is_panel_unlocked_station_10,
					status_cylinder_rodage_up_station_20,
					status_cylinder_rodage_down_station_20,
					is_panel_plugged_station_30,
					is_panel_unplugged_station_30,
					is_panel_plugged_station_40,
					is_panel_unplugged_station_40,
					status_cylinder_rodage_left_up_station_50,
					status_cylinder_rodage_left_down_station_50,
					status_cylinder_rodage_right_up_station_50,
					status_cylinder_rodage_right_down_station_50,
					status_cylinder_test_left_up_station_60,
					status_cylinder_test_left_down_station_60,
					status_cylinder_test_right_up_station_60,
					status_cylinder_test_right_down_station_60,
					is_panel_plugged_station_60,
					is_panel_unplugged_station_60,
					is_panel_plugged_station_70,
					is_panel_unplugged_station_70,
					is_panel_locked_station_80,
					is_panel_unlocked_station_80,
					is_cylinder_x_rear_station_80,
					is_cylinder_x_front_station_80;

	bool_t			is_station_10_ready = FALSE,
					is_station_20_ready = FALSE,
					is_station_30_ready = FALSE,
					is_station_40_ready = FALSE,
					is_station_50_ready = FALSE,
					is_station_60_ready = FALSE,
					is_station_70_ready = FALSE,
					is_station_80_ready = FALSE;
	
   	SaxisPtr		paxis = NULL; 
	
	double 			position_axis_Z_right_station_30;
	double 			position_axis_Z_left_station_30;
	double 			position_axis_Z_right_station_40;
	double 			position_axis_Z_left_station_40;
		
	bool_t			is_z_home_right_station_30,
					is_z_home_left_station_30,
					is_z_home_right_station_40,
					is_z_home_left_station_40;
				   
	
			/* STATION 10 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_10", &is_panel_locked_station_10));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_10", &is_panel_unlocked_station_10));
			if(is_panel_locked_station_10 && !is_panel_unlocked_station_10)
			{
				is_station_10_ready = TRUE;
			}
			else						 
				is_station_10_ready = FALSE;

			
			/* STATION 20 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_UP_STATION_20", &status_cylinder_rodage_up_station_20));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_DOWN_STATION_20", &status_cylinder_rodage_down_station_20));
			if(status_cylinder_rodage_up_station_20 && !status_cylinder_rodage_down_station_20)
			{
				is_station_20_ready = TRUE;
			}
			else 
				is_station_20_ready = FALSE;

			
			/* STATION 30 */
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_30_RIGHT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_right_station_30));
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_30_LEFT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_left_station_30));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_30_RIGHT", &is_z_home_right_station_30));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_30_LEFT", &is_z_home_left_station_30));	
			
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_30", &is_panel_plugged_station_30));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_30", &is_panel_unplugged_station_30));
			if(is_panel_unplugged_station_30 && !is_panel_plugged_station_30 
			   && position_axis_Z_right_station_30 < 0.2 && position_axis_Z_left_station_30 < 0.2
			   && is_z_home_right_station_30 && is_z_home_left_station_30) 
			{
				is_station_30_ready = TRUE;
			}
			else 
				is_station_30_ready = FALSE;
			
			/* STATION 40 */
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_40_RIGHT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_right_station_40));
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_40_LEFT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_left_station_40));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_40_RIGHT", &is_z_home_right_station_40));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_40_LEFT", &is_z_home_left_station_40));
	
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40", &is_panel_plugged_station_40));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40", &is_panel_unplugged_station_40));
			
			if(is_panel_unplugged_station_40 && !is_panel_plugged_station_40 
			   && position_axis_Z_right_station_40 < 0.2 && position_axis_Z_left_station_40 < 0.2
			   && is_z_home_right_station_40 && is_z_home_left_station_40)
			{
				is_station_40_ready = TRUE;
			}
			else 
				is_station_40_ready = FALSE;
		

			/* STATION 50 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_UP_STATION_50", &status_cylinder_rodage_left_up_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_DOWN_STATION_50", &status_cylinder_rodage_left_down_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_UP_STATION_50", &status_cylinder_rodage_right_up_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_DOWN_STATION_50", &status_cylinder_rodage_right_down_station_50));
			if(status_cylinder_rodage_left_up_station_50 && !status_cylinder_rodage_left_down_station_50 &&
			   status_cylinder_rodage_right_up_station_50 && !status_cylinder_rodage_right_down_station_50)
			{
				is_station_50_ready = TRUE;
			}
			else 
				is_station_50_ready = FALSE;
		
			/* STATION 60 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_UP_STATION_60", &status_cylinder_test_left_up_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_DOWN_STATION_60", &status_cylinder_test_left_down_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_UP_STATION_60", &status_cylinder_test_right_up_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_DOWN_STATION_60", &status_cylinder_test_right_down_station_60));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60", &is_panel_plugged_station_60));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60", &is_panel_unplugged_station_60));
			if(status_cylinder_test_left_up_station_60 && !status_cylinder_test_left_down_station_60 &&
			   status_cylinder_test_right_up_station_60 && !status_cylinder_test_right_down_station_60 &&
			   is_panel_unplugged_station_60 && !is_panel_plugged_station_60)
			{
				is_station_60_ready = TRUE;
			}
			else 
				is_station_60_ready = FALSE;

			
			/* STATION 70 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_70", &is_panel_plugged_station_70));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_70", &is_panel_unplugged_station_70));
			if(is_panel_unplugged_station_70 && !is_panel_plugged_station_70)
			{
				is_station_70_ready = TRUE;
			}
			else 
				is_station_70_ready = FALSE;
			
			
			/* STATION 80 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &is_panel_locked_station_80));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &is_panel_unlocked_station_80));
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &is_cylinder_x_front_station_80));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &is_cylinder_x_rear_station_80));
			if(is_cylinder_x_rear_station_80 && !is_cylinder_x_front_station_80 &&
			   is_panel_locked_station_80 && !is_panel_unlocked_station_80)
			{
				is_station_80_ready = TRUE;
			}
			else 
				is_station_80_ready = FALSE;
			
		
	if(is_station_10_ready &&
	   is_station_20_ready &&
	   is_station_30_ready &&
	   is_station_40_ready && 
	   is_station_50_ready && 
	   is_station_60_ready && 
	   is_station_70_ready && 
	   is_station_80_ready)
	{
		
	TIMEOUT_INIT();
	
	while( TRUE)
	{
		
	EXCCHECK( me->GetSensor(me, "INFRARED_BARRIER", &status_barrier));
//	printf("status_barrier=%d\r\n",status_barrier);
	  if (status_barrier == 0)
	  {
  	   Sleep(500);
	   EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
	   if(status_micro_cam == 0)// in not indexed poition 
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	  }
	  
	/* si aprés passage barriére un véin sorte il doit vérifier un autre fois is_table_ready should be = 0 */
		else if (!is_table_rotation_finished)
		{
			
		    EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
			if (!status_micro_cam)
			{
				
			/* cylnders not work */
			me->SetSensor(me, "CYLINDER_LOCK_STATION_10", 1);
			me->SetSensor(me, "CYLINDER_UNLOCK_STATION_10", 0);
			
			me->SetSensor(me, "BRAKE_RODAGE_STATION_20", 1);
			me->SetSensor(me, "RODAGE_UP_STATION_20", 1);
			me->SetSensor(me, "RODAGE_DOWN_STATION_20", 0);
			
			me->SetSensor(me, "CYLINDER_UNPLUG_STATION_30", 1);
			me->SetSensor(me, "CYLINDER_PLUG_STATION_30", 0);
			
			me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40", 1);
			me->SetSensor(me, "CYLINDER_PLUG_STATION_40", 0);

			me->SetSensor(me, "RODAGE_UP_RIGHT_STATION_50", 1);
			me->SetSensor(me, "RODAGE_DOWN_RIGHT_STATION_50", 0);
			me->SetSensor(me, "RODAGE_UP_LEFT_STATION_50", 1);
			me->SetSensor(me, "RODAGE_DOWN_LEFT_STATION_50", 0);
			
			me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60", 1);
			me->SetSensor(me, "CYLINDER_PLUG_STATION_60", 0);
			
			me->SetSensor(me, "TEST_UP_LEFT_STATION_60", 1);
			me->SetSensor(me, "TEST_DOWN_LEFT_STATION_60", 0);
			me->SetSensor(me, "TEST_UP_RIGHT_STATION_60", 1);
			me->SetSensor(me, "TEST_DOWN_RIGHT_STATION_60", 0);
			
			me->SetSensor(me, "CYLINDER_UNPLUG_STATION_70", 1);
			me->SetSensor(me, "CYLINDER_PLUG_STATION_70", 0);
			
			me->SetSensor(me, "CYLINDER_LOCK_STATION_80", 1);
			me->SetSensor(me, "CYLINDER_UNLOCK_STATION_80", 0);
			me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1);
		 	me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0);
			
			}
			
			switch  (step)
			{
			 case  CHECK_TABLE_POITION : /* check the table in wich position and start rotation */
				   		
				   EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
				   
						if(status_micro_cam == 1)// in indexed poition 
							{
				 				EXCCHECK( me->SetSensor(me, "START_ROTATION", 1));
								EXCCHECK( me->SetSensor(me, "FORCAGE_ROTATION", 1));
								step ++;		
				 			}  
				 		else if(status_micro_cam == 0)// in not indexed poition 
							{
								EXCCHECK( me->SetSensor(me, "START_ROTATION", 1));
								step ++;		
				 			} 
			break;
			
			case  CHECK_TABLE_IN_ROTATION :
				   		
				   EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
					if(status_micro_cam == 0)// in rotation now
					{
						EXCCHECK( me->SetSensor(me, "FORCAGE_ROTATION", 0));
						step ++;
					}

			break;
			
			case  CHECK_IF_ROTATION_FINIHED :
				   		
				   EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
						if(status_micro_cam == 1)// rotation finished 
							{
				 			//	printf("Rotation finished \r\n");
								EXCCHECK( me->SetSensor(me, "FORCAGE_ROTATION", 0));
								EXCCHECK( me->SetSensor(me, "START_ROTATION", 0));
								step ++;		
				 			}  
			break;
				
			case  ROTATION_FINIHED_OK :
					
				 is_table_rotation_finished = TRUE;
		
			break;
				
			}
		}
		
		else  if (is_table_rotation_finished) break;
			

		TIMEOUT_CHECK( timeout);
	}

	}

	else 
	{
	EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	}

Error:
	me->SetSensor(me, "FORCAGE_ROTATION", 0);
	me->SetSensor(me, "START_ROTATION", 0);
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::GetStationStatus"
static SElExceptionPtr GetStationStatus(struct _STester* me, char Table_Ready[9])
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		is_panel_locked_station_10,
					is_panel_unlocked_station_10,
					status_cylinder_rodage_up_station_20,
					status_cylinder_rodage_down_station_20,
					is_panel_plugged_station_30,
					is_panel_unplugged_station_30,
					is_panel_plugged_station_40,
					is_panel_unplugged_station_40,
					status_cylinder_rodage_left_up_station_50,
					status_cylinder_rodage_left_down_station_50,
					status_cylinder_rodage_right_up_station_50,
					status_cylinder_rodage_right_down_station_50,
					status_cylinder_test_left_up_station_60,
					status_cylinder_test_left_down_station_60,
					status_cylinder_test_right_up_station_60,
					status_cylinder_test_right_down_station_60,
					is_panel_plugged_station_60,
					is_panel_unplugged_station_60,
					is_panel_plugged_station_70,
					is_panel_unplugged_station_70,
					is_panel_locked_station_80,
					is_panel_unlocked_station_80,
					is_cylinder_x_rear_station_80,
					is_cylinder_x_front_station_80;

	bool_t			is_station_10_ready = FALSE,
					is_station_20_ready = FALSE,
					is_station_30_ready = FALSE,
					is_station_40_ready = FALSE,
					is_station_50_ready = FALSE,
					is_station_60_ready = FALSE,
					is_station_70_ready = FALSE,
					is_station_80_ready = FALSE;
	
  	SaxisPtr		paxis = NULL; 
	
	double 			position_axis_Z_right_station_30;
	double 			position_axis_Z_left_station_30;
	double 			position_axis_Z_right_station_40;
	double 			position_axis_Z_left_station_40;
	
	bool_t			is_z_home_right_station_30,
					is_z_home_left_station_30,
					is_z_home_right_station_40,
					is_z_home_left_station_40;
				   
	

			/* STATION 10 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_10", &is_panel_locked_station_10));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_10", &is_panel_unlocked_station_10));
			if(is_panel_locked_station_10 && !is_panel_unlocked_station_10)
			{
				is_station_10_ready = TRUE;
			}
			else						 
				is_station_10_ready = FALSE;

			
			/* STATION 20 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_UP_STATION_20", &status_cylinder_rodage_up_station_20));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_DOWN_STATION_20", &status_cylinder_rodage_down_station_20));
			if(status_cylinder_rodage_up_station_20 && !status_cylinder_rodage_down_station_20)
			{
				is_station_20_ready = TRUE;
			}
			else 
				is_station_20_ready = FALSE;

			
			/* STATION 30 */
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_30_RIGHT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_right_station_30));
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_30_LEFT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_left_station_30));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_30_RIGHT", &is_z_home_right_station_30));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_30_LEFT", &is_z_home_left_station_30));		
			
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_30", &is_panel_plugged_station_30));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_30", &is_panel_unplugged_station_30));
			if(is_panel_unplugged_station_30 && !is_panel_plugged_station_30 
			   && position_axis_Z_right_station_30 < 0.2 && position_axis_Z_left_station_30 < 0.2
			   && is_z_home_right_station_30 && is_z_home_left_station_30)
			{
				is_station_30_ready = TRUE;
			}
			else 
				is_station_30_ready = FALSE;
			
			/* STATION 40 */
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_40_RIGHT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_right_station_40));
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_40_LEFT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_left_station_40));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_40_RIGHT", &is_z_home_right_station_40));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_40_LEFT", &is_z_home_left_station_40));
	
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40", &is_panel_plugged_station_40));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40", &is_panel_unplugged_station_40));
			
			if(is_panel_unplugged_station_40 && !is_panel_plugged_station_40 
			   && position_axis_Z_right_station_40 < 0.2 && position_axis_Z_left_station_40 < 0.2
			   && is_z_home_right_station_40 && is_z_home_left_station_40)
			{
				is_station_40_ready = TRUE;
			}
			else 
				is_station_40_ready = FALSE;
		

			/* STATION 50 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_UP_STATION_50", &status_cylinder_rodage_left_up_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_DOWN_STATION_50", &status_cylinder_rodage_left_down_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_UP_STATION_50", &status_cylinder_rodage_right_up_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_DOWN_STATION_50", &status_cylinder_rodage_right_down_station_50));
			if(status_cylinder_rodage_left_up_station_50 && !status_cylinder_rodage_left_down_station_50 &&
			   status_cylinder_rodage_right_up_station_50 && !status_cylinder_rodage_right_down_station_50)
			{
				is_station_50_ready = TRUE;
			}
			else 
				is_station_50_ready = FALSE;
		
			/* STATION 60 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_UP_STATION_60", &status_cylinder_test_left_up_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_DOWN_STATION_60", &status_cylinder_test_left_down_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_UP_STATION_60", &status_cylinder_test_right_up_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_DOWN_STATION_60", &status_cylinder_test_right_down_station_60));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60", &is_panel_plugged_station_60));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60", &is_panel_unplugged_station_60));
			if(status_cylinder_test_left_up_station_60 && !status_cylinder_test_left_down_station_60 &&
			   status_cylinder_test_right_up_station_60 && !status_cylinder_test_right_down_station_60 &&
			   is_panel_unplugged_station_60 && !is_panel_plugged_station_60)
			{
				is_station_60_ready = TRUE;
			}
			else 
				is_station_60_ready = FALSE;

			
			/* STATION 70 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_70", &is_panel_plugged_station_70));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_70", &is_panel_unplugged_station_70));
			if(is_panel_unplugged_station_70 && !is_panel_plugged_station_70)
			{
				is_station_70_ready = TRUE;
			}
			else 
				is_station_70_ready = FALSE;
			
			
			/* STATION 80 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &is_panel_locked_station_80));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &is_panel_unlocked_station_80));
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &is_cylinder_x_front_station_80));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &is_cylinder_x_rear_station_80));
			if(is_cylinder_x_rear_station_80 && !is_cylinder_x_front_station_80 &&
			   is_panel_locked_station_80 && !is_panel_unlocked_station_80)
			{
				is_station_80_ready = TRUE;
			}
			else 
				is_station_80_ready = FALSE;

					
			Table_Ready[0] = is_station_10_ready;
			Table_Ready[1] = is_station_20_ready;
			Table_Ready[2] = is_station_30_ready;
			Table_Ready[3] = is_station_40_ready;
			Table_Ready[4] = is_station_50_ready;
			Table_Ready[5] = is_station_60_ready;
			Table_Ready[6] = is_station_70_ready;
			Table_Ready[7] = is_station_80_ready;
			Table_Ready[8] = 0X00;
		
Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::OkNokCheck"
static SElExceptionPtr OkNokCheck(struct _STester* me, int32_t timeout, bool_t* ok)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		button_ok,
					button_nok;

	int red = 0;
	int green = 1;
	
	TIMEOUT_INIT();
	
	if(ok) *ok = FALSE;


	
	while(TRUE)
	{
		EXCCHECK( me->GetSensor(me, "BUTTON_OK", &button_ok));
		EXCCHECK( me->GetSensor(me, "BUTTON_NOK", &button_nok));
	
		if (red == 0)
		{
			EXCCHECK( me->SetSensor(me, "INDICATOR_NOK", 1));
			red = 1;
		}
		else  if (red == 1)
		{
			EXCCHECK( me->SetSensor(me, "INDICATOR_NOK", 0));
			red = 0;
		}
		Sleep(120);
		EXCCHECK( me->GetSensor(me, "BUTTON_OK", &button_ok));
		EXCCHECK( me->GetSensor(me, "BUTTON_NOK", &button_nok));
		
		if (green == 0)
		{
			EXCCHECK( me->SetSensor(me, "INDICATOR_OK", 1));
			green = 1;
		}
		else  if (green == 1)
		{
			EXCCHECK( me->SetSensor(me, "INDICATOR_OK", 0));
			green = 0;
		}
		Sleep(120);
		EXCCHECK( me->GetSensor(me, "BUTTON_OK", &button_ok));
		EXCCHECK( me->GetSensor(me, "BUTTON_NOK", &button_nok));
		
		if(button_ok && !button_nok)
		{
			if(ok) *ok = TRUE;
			break;
		}
		else if(!button_ok && button_nok)
		{
			if(ok) *ok = FALSE;
			break;
		}
		
		TIMEOUT_CHECK( timeout);
	};

Error:
	EXCIGNORE( TESTER_ERROR_TIMEOUT);
	me->SetSensor(me, "INDICATOR_OK", 0);
	me->SetSensor(me, "INDICATOR_NOK", 0);
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::NokCheck"
static SElExceptionPtr NokCheck(struct _STester* me, int32_t timeout)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		button_nok;

	TIMEOUT_INIT();

	EXCCHECK( me->SetSensor(me, "INDICATOR_NOK", 1));

	while(TRUE)
	{
		EXCCHECK( me->GetSensor(me, "BUTTON_NOK", &button_nok));
		
		if(button_nok)
			break;

		if(timeout>=0)
			TIMEOUT_CHECK( timeout);

		Sleep(100);
	};

Error:
	EXCIGNORE( TESTER_ERROR_TIMEOUT);
	me->SetSensor(me, "INDICATOR_NOK", 0);
	EXCRETHROW(pexception);
}

typedef struct _SBeepThreadData{
	struct _STester*	me;
	int32_t				time;
} SBeepThreadData, *SBeepThreadDataPtr;

/*----------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "BeepThreadFunc"
#if defined(_CVI_)
	static DWORD WINAPI BeepThreadFunc (LPVOID param)
#else
	unsigned __cdecl BeepThreadFunc( void* pArguments )
#endif
{
	SElExceptionPtr		pexception = NULL;
	SBeepThreadDataPtr	pthread_data = (SBeepThreadDataPtr)param;
	
	EXCCHECK( pthread_data->me->SetSensor(pthread_data->me, "TESTER_BEEP", 1)); 

	timeBeginPeriod(1);
	Sleep(pthread_data->time); 
	timeEndPeriod(1);

	EXCCHECK( pthread_data->me->SetSensor(pthread_data->me, "TESTER_BEEP", 0));
	
Error:
	free( pthread_data);
	EXCDBGSTR( pexception);
#if defined(_CVI_)
	CmtExitThreadPoolThread (0);
#else
    _endthread( 0 );
#endif
    return 0;
}	/* BeepThreadFunc */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::SetSensor"
static SElExceptionPtr SetSensor
(
	struct _STester* me,
	const char*		sensorId,
	uint32_t		value
)
{
	SElExceptionPtr	pexception = NULL;
	char*			plogic_name = "";
	SLogicName		tmp;
	SLogicNamePtr	palias_pair = NULL;
	char*			plogical_name = NULL;
	
	/* convert sensorId to logical name in tester */
	strcpy( tmp.aliasName, sensorId);
	
	palias_pair = bsearch( &tmp,
						   me->pLogicalNames,
						   me->LogicalCounts,
						   sizeof(SLogicName),
						   CompareFnc);

	plogical_name = (palias_pair) ? palias_pair->logicalName : (char*) sensorId;

	if( me->sensorAPI)
		EXCCHECK( me->sensorAPI->SetState(me->sensorAPI, plogical_name, value));

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::SetSensorCallback"
static SElExceptionPtr SetSensorCallback(
	struct _STester* me, 
	const char* sensorId, 
	SensorCallbackFnc_t fCall,
	void* pData
)
{
	SElExceptionPtr	pexception = NULL;
	char*			plogic_name = "";
	SLogicName		tmp;
	SLogicNamePtr	palias_pair = NULL;
	char*			plogical_name = NULL;
	
	/* convert sensorId to logical name in tester */
	strcpy( tmp.aliasName, sensorId);
	
	palias_pair = bsearch( &tmp,
						   me->pLogicalNames,
						   me->LogicalCounts,
						   sizeof(SLogicName),
						   CompareFnc);

	plogical_name = (palias_pair) ? palias_pair->logicalName : (char*) sensorId;

	if( me->sensorAPI)
		EXCCHECK( me->sensorAPI->RegisterCallback( me->sensorAPI, plogical_name, fCall, pData));

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::GetSensor"
static SElExceptionPtr GetSensor
(
	struct _STester*	me,
	const char*		sensorId,
	uint32_t*		value
)
{
	SElExceptionPtr	pexception = NULL;
	SLogicName		tmp;
	SLogicNamePtr	palias_pair = NULL;
	char*			plogical_name = NULL;
	
	/* TODO: convert sensorId to logical name in tester */
	strcpy( tmp.aliasName, sensorId);
	
	palias_pair = bsearch( &tmp,
						   me->pLogicalNames,
						   me->LogicalCounts,
						   sizeof(SLogicName),
						   CompareFnc);

	if (value == NULL)
	{
		EXCTHROW(TESTER_ERROR_INVALID_PARAMETER, "Parameter value is NULL");
	}

	plogical_name = (palias_pair)? palias_pair->logicalName : (char*) sensorId;
	
	if( me->sensorAPI)
		EXCCHECK( me->sensorAPI->GetState(me->sensorAPI, plogical_name, value));

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncButtonOk"
static SElExceptionPtr CallbackFncButtonOk(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;

	if(state==1 && me->eventAPI)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_TESTER_BUTTON_PRESSED_OK, NULL));

	if(state==1)
	{
		HWND hwnd_excp = NULL;
		HWND hwnd_exit = NULL;
	
		/* confirm excption */
		hwnd_excp = FindWindow(NULL, "Exception");
		if(hwnd_excp)
		{
			SendMessage(hwnd_excp, WM_COMMAND, IDYES, BN_CLICKED);
		}
		else
		{
			/* confirm exit */
			hwnd_exit = FindWindow(NULL, "Exit");
			if(hwnd_exit)
				SendMessage(hwnd_exit, WM_COMMAND, IDYES, BN_CLICKED);
		}
	}

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncButtonNok"
static SElExceptionPtr CallbackFncButtonNok(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;

	if( state==1 && me->eventAPI)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_TESTER_BUTTON_PRESSED_NOK, NULL));

	if(state==1)
	{
		HWND hwnd_exit = NULL;

		/* storno exit */
		hwnd_exit = FindWindow(NULL, "Exit");
		if(hwnd_exit)
			SendMessage(hwnd_exit, WM_COMMAND, IDNO, BN_CLICKED);
	}

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncButtonMenu"
static SElExceptionPtr CallbackFncButtonMenu(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;

	if( state==1 && me->eventAPI)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_TESTER_BUTTON_PRESSED_MENU, NULL));

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncButtonLabel"
static SElExceptionPtr CallbackFncButtonLabel(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;

	if( state==1 && me->eventAPI)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_TESTER_BUTTON_PRESSED_LABEL, NULL));

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncButtonDowntime"
static SElExceptionPtr CallbackFncButtonDowntime(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;

	if( state==1 && me->eventAPI)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_TESTER_BUTTON_PRESSED_DOWNTIME, NULL));

Error:
	EXCRETHROW(pexception);
}


#undef __FUNC__
#define __FUNC__	"CallbackFncJigNew"
static SElExceptionPtr CallbackFncJigNew(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;									 ;
	STesterPtr		me = (STesterPtr)pData;
	bool_t          status_micro_cam;
	bool_t          status_cylinder_lock;
	bool_t          status_cylinder_unlock;
	
	if( state==1 && me->eventAPI)
	{
	/* check where is the table */
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_10", &status_cylinder_lock));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_10", &status_cylinder_unlock));

	if( state==1 && status_cylinder_lock && !status_cylinder_unlock && me->eventAPI)
	{
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_NEW, NULL));
	}
	}
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"CallbackFncButtonStart"
static SElExceptionPtr CallbackFncButtonStart(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	bool_t          status_micro_cam, status_barrier;
	
	if( state==1 && me->eventAPI)
	{
	/* wait to avtivate relay Barrier */	
	//Sleep (1000);

	/* check where is the table */
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
	EXCCHECK( me->GetSensor(me, "INFRARED_BARRIER", &status_barrier));

	

	if (status_micro_cam == 0)
		me->TableRotate(me, 5000,TRUE);
	
	if (status_barrier == 0)
		Sleep(200);
	
	EXCCHECK( me->GetSensor(me, "INFRARED_BARRIER", &status_barrier));

	if (status_micro_cam == 1 && gs_inserted == 1 && status_barrier == 1)
	{
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_CYCLE_START, NULL));
	}
	else if (status_micro_cam == 1 && gs_inserted == 0 && status_barrier == 1)
	{
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_START, NULL));
	}
	}
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"CallbackFncLaserEnd"
static SElExceptionPtr CallbackFncLaserEnd(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_LASER_CYCLE_END, NULL));
	
Error:
	EXCRETHROW(pexception);
}
#undef __FUNC__
#define __FUNC__	"CallbackFncRedBoxEmpty"
static SElExceptionPtr CallbackFncRedBoxEmpty(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_RED_BOX_EMPTY, NULL));
	
Error:
	EXCRETHROW(pexception);
}
	
#undef __FUNC__
#define __FUNC__	"CallbackFncRotation"
static SElExceptionPtr CallbackFncRotation(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	bool_t          status_micro_cam;

	if( state==1 && me->eventAPI)  // state==1 : TABLE IN POSITION
	{
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_TESTER_ROTATION_FINISHED, NULL));
	}
	
Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncButtonTotalStop"
static SElExceptionPtr CallbackFncButtonTotalStop(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;

	//EXCCHECK( me->SetSensor(me, "INDICATOR_START", state));	

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"JigThread"
SElExceptionPtr JigThread(struct _STester* me)
{
	SElExceptionPtr	pexception = NULL;
	bool_t			microCam = 0;
	int32_t			insert1 = 0,
					insert2 = 0,
					gs_lock_station_10,
					gs_unlock_station_10,
					sensor_locked_station_10,
					
					gs_rodage_up_station_20 = 0,
					gs_rodage_down_station_20 = 0,
					
					gs_plug_station_30,
					gs_unplug_station_30,
					
					gs_plug_station_40,
					gs_unplug_station_40,
					
					gs_rodage_start_left_station_50 = 0,
					gs_rodage_stop_left_station_50 = 0,
					gs_rodage_start_right_station_50 = 0,
					gs_rodage_stop_right_station_50 = 0,
					
					gs_plug_station_60,
					gs_unplug_station_60,
					gs_test_start_left_station_60 = 0,
					gs_test_stop_left_station_60 = 0,
					gs_test_start_right_station_60 = 0,
					gs_test_stop_right_station_60 = 0,
					
					gs_plug_station_70,
					gs_unplug_station_70,
					gs_lock_station_80,
					gs_unlock_station_80,
					gs_barcode_front,
					gs_barcode_rear;
	
	
	/* Check if pannel is inserted STATION 10*/
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_LEFT", &insert1));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_RIGHT", &insert2));
	if (insert1 && insert2)
	{
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
		if (me->eventAPI && !gs_inserted && microCam)
		{
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_INSERTED, NULL));
		gs_inserted = 1;
		}
	}
	else
	{
		gs_inserted = 0;
	}
	/* Check if pannel is released STATION 10 */
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_LEFT", &insert1));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_RIGHT", &insert2));
	if (!insert1 && !insert2)
	{
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
		if (me->eventAPI && !gs_released && microCam)
		{
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_RELEASED, NULL));
		gs_released = 1;
		}
	}
	else
	{
		gs_released = 0;
	}
	
	/* Check if panel locked station 10 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_10", &gs_lock_station_10));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_10", &gs_unlock_station_10));
	EXCCHECK( me->GetSensor(me, "SENSOR_PANEL_LOCKED_STATION_10", &sensor_locked_station_10));
	if (!gs_unlock_station_10 && gs_lock_station_10 && sensor_locked_station_10)
	{
	if (me->eventAPI && !gs_locked_station_10)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_LOCKED_STATION_10, NULL));
		gs_locked_station_10 = 1;
	}
	else
	{
		gs_locked_station_10 = 0;
	}
	/* Check if panel unlocked station 10 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_10", &gs_lock_station_10));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_10", &gs_unlock_station_10));
	EXCCHECK( me->GetSensor(me, "SENSOR_PANEL_LOCKED_STATION_10", &sensor_locked_station_10));
	if (gs_unlock_station_10 && !gs_lock_station_10 && !sensor_locked_station_10 )
	{
		if (me->eventAPI && !gs_unlocked_station_10)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNLOCKED_STATION_10, NULL));
		
		gs_unlocked_station_10 = 1;
	}
	else													  
	{
		gs_unlocked_station_10 = 0;
	}
	
	/* Check if rodage started STATION 20 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_UP_STATION_20", &gs_rodage_up_station_20));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_DOWN_STATION_20", &gs_rodage_down_station_20));
	if (gs_rodage_down_station_20 && !gs_rodage_up_station_20)
	{
	if (me->eventAPI && !gs_started_rodage_station_20)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_START_RODAGE_STATION_20, NULL));
		gs_started_rodage_station_20 = 1;
	}
	else
	{
		gs_started_rodage_station_20 = 0;
	}
	
	/* Check if rodage stopped STATION 20 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_UP_STATION_20", &gs_rodage_up_station_20));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_DOWN_STATION_20", &gs_rodage_down_station_20));
	if (!gs_rodage_down_station_20 && gs_rodage_up_station_20)
	{
		if (me->eventAPI && !gs_stopped_rodage_station_20)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_RODAGE_STATION_20, NULL));
		
		gs_stopped_rodage_station_20 = 1;
	}
	else
	{
		gs_stopped_rodage_station_20 = 0;
	}
	
	
	/* Check if panel plugged STATION 30 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_30", &gs_plug_station_30));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_30", &gs_unplug_station_30));
	if (!gs_unplug_station_30 && gs_plug_station_30)
	{
		if (me->eventAPI && !gs_plugged_station_30)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_30, NULL));
		
		gs_plugged_station_30 = 1;
	}
	else
	{
		gs_plugged_station_30 = 0;
	}
	/* Check if panel unplugged STATION 30 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_30", &gs_plug_station_30));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_30", &gs_unplug_station_30));
	if (gs_unplug_station_30 && !gs_plug_station_30)
	{
		if (me->eventAPI && !gs_unplugged_station_30)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_30, NULL));
		
		gs_unplugged_station_30 = 1;
	}
	else
	{
		gs_unplugged_station_30 = 0;
	}
	
	/* Check if panel plugged STATION 40 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40", &gs_plug_station_40));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40", &gs_unplug_station_40));
	if (!gs_unplug_station_40 && gs_plug_station_40)
	{
		if (me->eventAPI && !gs_plugged_station_40)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_40, NULL));
		
		gs_plugged_station_40 = 1;
	}
	else
	{
		gs_plugged_station_40 = 0;
	}
	/* Check if panel unplugged STATION 40 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40", &gs_plug_station_40));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40", &gs_unplug_station_40));
	if (gs_unplug_station_40 && !gs_plug_station_40)
	{
		if (me->eventAPI && !gs_unplugged_station_40)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_40, NULL));
		
		gs_unplugged_station_40 = 1;
	}
	else
	{
		gs_unplugged_station_40 = 0;
	}

	/* Check if rodage left started STATION 50 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_DOWN_STATION_50", &gs_rodage_start_left_station_50));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_UP_STATION_50", &gs_rodage_stop_left_station_50));
	if (gs_rodage_start_left_station_50 && !gs_rodage_stop_left_station_50)
	{
	if (me->eventAPI && !gs_started_rodage_left_station_50)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_START_RODAGE_LEFT_STATION_50, NULL));
		gs_started_rodage_left_station_50 = 1;
	}
	else
	{
		gs_started_rodage_left_station_50 = 0;
	}
	
	/* Check if rodage left stopped STATION 50 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_DOWN_STATION_50", &gs_rodage_start_left_station_50));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_UP_STATION_50", &gs_rodage_stop_left_station_50));
	if (gs_rodage_stop_left_station_50 && !gs_rodage_start_left_station_50)
	{
	if (me->eventAPI && !gs_stopped_rodage_left_station_50)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_RODAGE_LEFT_STATION_50, NULL));
		gs_stopped_rodage_left_station_50 = 1;
	}
	else
	{
		gs_stopped_rodage_left_station_50 = 0;
	}
	
	/* Check if rodage right started STATION 50 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_DOWN_STATION_50", &gs_rodage_start_right_station_50));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_UP_STATION_50", &gs_rodage_stop_right_station_50));
	if (gs_rodage_start_right_station_50 && !gs_rodage_stop_right_station_50)
	{
	if (me->eventAPI && !gs_started_rodage_right_station_50)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_START_RODAGE_RIGHT_STATION_50, NULL));
		gs_started_rodage_right_station_50 = 1;
	}
	else
	{
		gs_started_rodage_right_station_50 = 0;
	}
	
	/* Check if rodage right stopped STATION 50 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_DOWN_STATION_50", &gs_rodage_start_right_station_50));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_UP_STATION_50", &gs_rodage_stop_right_station_50));
	if (gs_rodage_stop_right_station_50 && !gs_rodage_start_right_station_50)
	{
	if (me->eventAPI && !gs_stopped_rodage_right_station_50)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_RODAGE_RIGHT_STATION_50, NULL));
		gs_stopped_rodage_right_station_50 = 1;
	}
	else
	{
		gs_stopped_rodage_right_station_50 = 0;
	}
	
   	/* Check if panel plugged STATION 60 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60", &gs_plug_station_60));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60", &gs_unplug_station_60));
	if (!gs_unplug_station_60 && gs_plug_station_60)
	{
		if (me->eventAPI && !gs_plugged_station_60)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_60, NULL));
		
		gs_plugged_station_60 = 1;
	}
	else
	{
		gs_plugged_station_60 = 0;
	}
	/* Check if panel unplugged STATION 60 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60", &gs_plug_station_60));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60", &gs_unplug_station_60));
	if (gs_unplug_station_60 && !gs_plug_station_60)
	{
		if (me->eventAPI && !gs_unplugged_station_60)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_60, NULL));
		
		gs_unplugged_station_60 = 1;
	}
	else
	{
		gs_unplugged_station_60 = 0;
	}
	
	/* Check if test left started STATION 60 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_DOWN_STATION_60", &gs_test_start_left_station_60));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_UP_STATION_60", &gs_test_stop_left_station_60));
	if (gs_test_start_left_station_60 && !gs_test_stop_left_station_60)
	{
	if (me->eventAPI && !gs_started_test_left_station_60)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_START_TEST_LEFT_STATION_60, NULL));
		gs_started_test_left_station_60 = 1;
	}
	else
	{
		gs_started_test_left_station_60 = 0;
	}
	
	/* Check if test left stopped STATION 60 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_DOWN_STATION_60", &gs_test_start_left_station_60));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_UP_STATION_60", &gs_test_stop_left_station_60));
	if (gs_test_stop_left_station_60 && !gs_test_start_left_station_60)
	{
	if (me->eventAPI && !gs_stopped_test_left_station_60)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_TEST_LEFT_STATION_60, NULL));
		gs_stopped_test_left_station_60 = 1;
	}
	else
	{
		gs_stopped_test_left_station_60 = 0;
	}
	
	/* Check if test right started STATION 60 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_DOWN_STATION_60", &gs_test_start_right_station_60));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_UP_STATION_60", &gs_test_stop_right_station_60));
	if (gs_test_start_right_station_60 && !gs_test_stop_right_station_60)
	{
	if (me->eventAPI && !gs_started_test_right_station_60)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_START_TEST_RIGHT_STATION_60, NULL));
		gs_started_test_right_station_60 = 1;
	}
	else
	{
		gs_started_test_right_station_60 = 0;
	}
	
	/* Check if test right stopped STATION 60 */
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_DOWN_STATION_60", &gs_test_start_right_station_60));
	EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_UP_STATION_60", &gs_test_stop_right_station_60));
	if (gs_test_stop_right_station_60 && !gs_test_start_right_station_60)
	{
	if (me->eventAPI && !gs_stopped_test_right_station_60)
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_TEST_RIGHT_STATION_60, NULL));
		gs_stopped_test_right_station_60 = 1;
	}
	else
	{
		gs_stopped_test_right_station_60 = 0;
	}
	
	/* Check if panel plugged station 70 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_70", &gs_plug_station_70));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_70", &gs_unplug_station_70));
	if (!gs_unplug_station_70 && gs_plug_station_70)
	{
		if (me->eventAPI && !gs_plugged_station_70)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_70, NULL));
		
		gs_plugged_station_70 = 1;
	}
	else
	{
		gs_plugged_station_70 = 0;
	}
	/* Check if panel unplugged station 70 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_70", &gs_plug_station_70));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_70", &gs_unplug_station_70));
	if (gs_unplug_station_70 && !gs_plug_station_70)
	{
		if (me->eventAPI && !gs_unplugged_station_70)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_70, NULL));
		
		gs_unplugged_station_70 = 1;
	}
	else
	{
		gs_unplugged_station_70 = 0;
	}
	
	
	/* Check if panel locked station 80 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &gs_lock_station_80));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &gs_unlock_station_80));
	if (!gs_unlock_station_80 && gs_lock_station_80)
	{
		if (me->eventAPI && !gs_locked_station_80)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_LOCKED_STATION_80, NULL));
		
		gs_locked_station_80 = 1;
	}
	else
	{
		gs_locked_station_80 = 0;
	}
	/* Check if panel unlocked station 80 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &gs_lock_station_80));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &gs_unlock_station_80));
	if (gs_unlock_station_80 && !gs_lock_station_80)
	{
		if (me->eventAPI && !gs_unlocked_station_80)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNLOCKED_STATION_80, NULL));
		
		gs_unlocked_station_80 = 1;
	}
	else
	{
		gs_unlocked_station_80 = 0;
	}
	
	
	/* Check if Barcode Front */
	EXCCHECK( me->GetSensor(me, "IS_BARCODE_FRONT_STATION_80", &gs_barcode_front));
	EXCCHECK( me->GetSensor(me, "IS_BARCODE_REAR_STATION_80", &gs_barcode_rear));
	if (gs_barcode_front && !gs_barcode_rear)
	{
		if (me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_BARCODE_FRONT, NULL));
		
		gs_barcode_already_front_station_80 = 1;
	}
	else
	{
		gs_barcode_already_front_station_80 = 0;
	}
	/* Check if Barcode Rear */
	EXCCHECK( me->GetSensor(me, "IS_BARCODE_FRONT_STATION_80", &gs_barcode_front));
	EXCCHECK( me->GetSensor(me, "IS_BARCODE_REAR_STATION_80", &gs_barcode_rear));
	if (gs_barcode_rear && !gs_barcode_front)
	{
		if (me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_BARCODE_REAR, NULL));
		
		gs_barcode_already_rear_station_80 = 0;
	}
	else
	{
		gs_barcode_already_rear_station_80 = 0;
	}
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"TableThread"
SElExceptionPtr TableThread(struct _STester* me)
{
	SElExceptionPtr	pexception = NULL;
		bool_t			status_micro_cam = FALSE,
					is_table_rotation_finished = FALSE;
	
	uint32_t		is_panel_locked_station_10,
					is_panel_unlocked_station_10,
					status_cylinder_rodage_up_station_20,
					status_cylinder_rodage_down_station_20,
					is_panel_plugged_station_30,
					is_panel_unplugged_station_30,
					is_panel_plugged_station_40,
					is_panel_unplugged_station_40,
					status_cylinder_rodage_left_up_station_50,
					status_cylinder_rodage_left_down_station_50,
					status_cylinder_rodage_right_up_station_50,
					status_cylinder_rodage_right_down_station_50,
					status_cylinder_test_left_up_station_60,
					status_cylinder_test_left_down_station_60,
					status_cylinder_test_right_up_station_60,
					status_cylinder_test_right_down_station_60,
					is_panel_plugged_station_60,
					is_panel_unplugged_station_60,
					is_panel_plugged_station_70,
					is_panel_unplugged_station_70,
					is_panel_locked_station_80,
					is_panel_unlocked_station_80,
					is_cylinder_x_rear_station_80,
					is_cylinder_x_front_station_80;

	bool_t			is_station_10_ready = FALSE,
					is_station_20_ready = FALSE,
					is_station_30_ready = FALSE,
					is_station_40_ready = FALSE,
					is_station_50_ready = FALSE,
					is_station_60_ready = FALSE,
					is_station_70_ready = FALSE,
					is_station_80_ready = FALSE;
	
	bool_t			status_barrier = FALSE;

	
   	SaxisPtr		paxis = NULL; 
	
	double 			position_axis_Z_right_station_30;
	double 			position_axis_Z_left_station_30;
	double 			position_axis_Z_right_station_40;
	double 			position_axis_Z_left_station_40;
		
	bool_t			is_z_home_right_station_30,
					is_z_home_left_station_30,
					is_z_home_right_station_40,
					is_z_home_left_station_40;

	


			/* STATION 10 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_10", &is_panel_locked_station_10));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_10", &is_panel_unlocked_station_10));
			if(is_panel_locked_station_10 && !is_panel_unlocked_station_10)
			{
				is_station_10_ready = TRUE;
			}
			else						 
				is_station_10_ready = FALSE;

			
			/* STATION 20 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_UP_STATION_20", &status_cylinder_rodage_up_station_20));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_DOWN_STATION_20", &status_cylinder_rodage_down_station_20));
			if(status_cylinder_rodage_up_station_20 && !status_cylinder_rodage_down_station_20)
			{
				is_station_20_ready = TRUE;
			}
			else 
				is_station_20_ready = FALSE;

			
			/* STATION 30 */
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_30_RIGHT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_right_station_30));
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_30_LEFT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_left_station_30));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_30_RIGHT", &is_z_home_right_station_30));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_30_LEFT", &is_z_home_left_station_30));	
			
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_30", &is_panel_plugged_station_30));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_30", &is_panel_unplugged_station_30));
			if(is_panel_unplugged_station_30 && !is_panel_plugged_station_30 
			   && position_axis_Z_right_station_30 < 0.2 && position_axis_Z_left_station_30 < 0.2
			   && is_z_home_right_station_30 && is_z_home_left_station_30) 
			{
				is_station_30_ready = TRUE;
			}
			else 
				is_station_30_ready = FALSE;
			
			/* STATION 40 */
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_40_RIGHT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_right_station_40));
			EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_40_LEFT, &paxis));
			EXCCHECK( paxis->CurrentPosition(paxis, AXIS_Z, &position_axis_Z_left_station_40));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_40_RIGHT", &is_z_home_right_station_40));
			EXCCHECK( me->GetSensor(me, "SENSOR_Z_HOME_STATION_40_LEFT", &is_z_home_left_station_40));
	
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40", &is_panel_plugged_station_40));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40", &is_panel_unplugged_station_40));
			
			if(is_panel_unplugged_station_40 && !is_panel_plugged_station_40 
			   && position_axis_Z_right_station_40 < 0.2 && position_axis_Z_left_station_40 < 0.2
			   && is_z_home_right_station_40 && is_z_home_left_station_40)
			{
				is_station_40_ready = TRUE;
			}
			else 
				is_station_40_ready = FALSE;
		

			/* STATION 50 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_UP_STATION_50", &status_cylinder_rodage_left_up_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_LEFT_DOWN_STATION_50", &status_cylinder_rodage_left_down_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_UP_STATION_50", &status_cylinder_rodage_right_up_station_50));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_RODAGE_RIGHT_DOWN_STATION_50", &status_cylinder_rodage_right_down_station_50));
			if(status_cylinder_rodage_left_up_station_50 && !status_cylinder_rodage_left_down_station_50 &&
			   status_cylinder_rodage_right_up_station_50 && !status_cylinder_rodage_right_down_station_50)
			{
				is_station_50_ready = TRUE;
			}
			else 
				is_station_50_ready = FALSE;
		
			/* STATION 60 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_UP_STATION_60", &status_cylinder_test_left_up_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_LEFT_DOWN_STATION_60", &status_cylinder_test_left_down_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_UP_STATION_60", &status_cylinder_test_right_up_station_60));
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_TEST_RIGHT_DOWN_STATION_60", &status_cylinder_test_right_down_station_60));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60", &is_panel_plugged_station_60));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60", &is_panel_unplugged_station_60));
			if(status_cylinder_test_left_up_station_60 && !status_cylinder_test_left_down_station_60 &&
			   status_cylinder_test_right_up_station_60 && !status_cylinder_test_right_down_station_60 &&
			   is_panel_unplugged_station_60 && !is_panel_plugged_station_60)
			{
				is_station_60_ready = TRUE;
			}
			else 
				is_station_60_ready = FALSE;

			
			/* STATION 70 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_70", &is_panel_plugged_station_70));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_70", &is_panel_unplugged_station_70));
			if(is_panel_unplugged_station_70 && !is_panel_plugged_station_70)
			{
				is_station_70_ready = TRUE;
			}
			else 
				is_station_70_ready = FALSE;
			
			
			/* STATION 80 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &is_panel_locked_station_80));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &is_panel_unlocked_station_80));
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &is_cylinder_x_front_station_80));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &is_cylinder_x_rear_station_80));
			if(is_cylinder_x_rear_station_80 && !is_cylinder_x_front_station_80 &&
			   is_panel_locked_station_80 && !is_panel_unlocked_station_80)
			{
				is_station_80_ready = TRUE;
			}
			else 
				is_station_80_ready = FALSE;
	
			
	if(!is_station_10_ready || 
	   !is_station_20_ready || 
	   !is_station_30_ready || 
	   !is_station_40_ready || 
	   !is_station_50_ready || 
	   !is_station_60_ready || 
	   !is_station_70_ready ||
	   !is_station_80_ready)
		{
		me->SetSensor(me, "FORCAGE_ROTATION", 0);
		me->SetSensor(me, "START_ROTATION", 0); 	
		}
	
Error:
	EXCRETHROW(pexception);
}

/* Lock Panel station 10 */
#undef __FUNC__
#define __FUNC__	"LockPanelStation10"
static SElExceptionPtr LockPanelStation10
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;
	int32_t			sensor_locked = FALSE;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_LOCKED_STATION_10, &hevent));


	EXCCHECK( me->SetSensor(me, "CYLINDER_LOCK_STATION_10", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNLOCK_STATION_10", 0));
	
	if (timeout)
	{
		if(gs_locked_station_10) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			EXCCHECK( me->GetSensor(me, "SENSOR_PANEL_LOCKED_STATION_10", &sensor_locked));
			if(gs_locked_station_10) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Unlock Panel station 10 */
#undef __FUNC__
#define __FUNC__	"UnlockPanelStation10"
static SElExceptionPtr UnlockPanelStation10
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;

	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNLOCKED_STATION_10, &hevent));
	
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
//	printf("MICRO_CAM = %d\r\n",microCam);
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNLOCK_STATION_10", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_LOCK_STATION_10", 0));
	}
	if (timeout)
	{
		if(gs_unlocked_station_10) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unlocked_station_10) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Start rodage cylinder all pushs*/
#undef __FUNC__
#define __FUNC__	"StartRodageStation20"
static SElExceptionPtr StartRodageStation20
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_START_RODAGE_STATION_20, &hevent));


	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "BRAKE_RODAGE_STATION_20", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_STATION_20", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_STATION_20", 0));
	}
	
	if (timeout)
	{
		if(gs_started_rodage_station_20) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_started_rodage_station_20) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Stop rodage cylinder all pushs*/
#undef __FUNC__
#define __FUNC__	"StopRodageStation20"
static SElExceptionPtr StopRodageStation20
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_RODAGE_STATION_20, &hevent));

	EXCCHECK( me->SetSensor(me, "BRAKE_RODAGE_STATION_20", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_STATION_20", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_STATION_20", 0));

	if (timeout)
	{
		if(gs_stopped_rodage_station_20) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_stopped_rodage_station_20) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Status pushs */
#undef __FUNC__
#define __FUNC__	"StatusPushsStation20"
static SElExceptionPtr StatusPushsStation20
(
	struct _STester* me,
	char     status[7+1]
)
{
	SElExceptionPtr	pexception = NULL;
	int timeON = 500;
	int state;
	
	/* pushs down */
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_1", 1));
		
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_2", 1));
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_3", 1));	
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_4", 1));
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_5", 1));
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_6", 1));
	
	Sleep (timeON);
	
	EXCCHECK( me->GetSensor(me, "SENSOR_PUSH_LEFT_1", &state));
	status[0] = state? 0 : 1;
	
	EXCCHECK( me->GetSensor(me, "SENSOR_PUSH_LEFT_2", &state));
	status[1] = state? 0 : 1;
	
	EXCCHECK( me->GetSensor(me, "SENSOR_PUSH_LEFT_3", &state));
	status[2] = state? 0 : 1;
	
	EXCCHECK( me->GetSensor(me, "SENSOR_PUSH_LEFT_4", &state));
	status[3] = state? 0 : 1;
	
	EXCCHECK( me->GetSensor(me, "SENSOR_PUSH_LEFT_5", &state));
	status[4] = state? 0 : 1;
	
	EXCCHECK( me->GetSensor(me, "SENSOR_PUSH_LEFT_6", &state));
	status[5] = state? 0 : 1;

	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_1", 0));
		
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_2", 0));
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_3", 0));

	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_4", 0));
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_5", 0));
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_6", 0));
	
	Sleep (100);
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_7", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_7", 1));
	Sleep (timeON);
	EXCCHECK( me->GetSensor(me, "SENSOR_PUSH_LEFT_7", &state));
	status[6] = state? 0 : 1;
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_7", 0));
	
	
Error:
	EXCRETHROW(pexception);
}

/* Start rodage pushs */
#undef __FUNC__
#define __FUNC__	"RodageStation20"
static SElExceptionPtr RodageStation20
(
	struct _STester* me,
	int32_t			timeout,
	int32_t			cycle,
	int32_t			timeon,
	int32_t			timeoff,
	char            version[15+1]
)
{
	SElExceptionPtr	pexception = NULL;
	int i;
	int32_t			enable_push_left_1 = 0,
					enable_push_left_2 = 0,
					enable_push_left_3 = 0,
					enable_push_left_4 = 0,
					enable_push_left_5 = 0,
					enable_push_left_6 = 0,
					enable_push_left_7 = 0,
					enable_push_right_1 = 0,
					enable_push_right_2 = 0,
					enable_push_right_3 = 0,
					enable_push_right_4 = 0,
					enable_push_right_5 = 0,
					enable_push_right_6 = 0,
					enable_push_right_7 = 0,
					enable_push_right_8 = 0;
	
	int32_t			sensor_push_up = 0,
					sensor_push_down = 0;
	   
	int32_t			microCam;
	
	enable_push_left_1 = version[0];
	enable_push_left_2 = version[1];
	enable_push_left_3 = version[2];
	enable_push_left_4 = version[3];
	enable_push_left_5 = version[4];
	enable_push_left_6 = version[5];
	enable_push_left_7 = version[6];
	enable_push_right_1 = version[7];
	enable_push_right_2 = version[8];
	enable_push_right_3 = version[9];
	enable_push_right_4 = version[10];
	enable_push_right_5 = version[11];
	enable_push_right_6 = version[12];
	enable_push_right_7 = version[13];
	enable_push_right_8 = version[14];
		
	/* all pushs up initialy */
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_7", 0));
	
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_7", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_8", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_8", 0));

	Sleep(500);

EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
if (microCam)
	{
	
/* Rodage with X cycles */
for (i=0;i<cycle;i++)
	{
	/* LEFT & RIGHT PUSH 1 */
	if (enable_push_left_1)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_1", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_1", 0));
		}
	if (enable_push_right_1)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_1", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_1", 0));
	//	printf("down 1\r\n");
		}
	
		Sleep(timeon); 
		
	if (enable_push_left_1)
		{
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_1", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_1", 1));
		}
	if (enable_push_right_1)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_1", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_1", 1));
	//	printf("UP 1\r\n");
		}
	
		Sleep(50); 
		
	/* LEFT & RIGHT PUSH 2 */
	if (enable_push_left_2)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_2", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_2", 0));
		}
	if (enable_push_right_2)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_2", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_2", 0));
		}
	
		Sleep(timeon); 
		
	if (enable_push_left_2)
		{
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_2", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_2", 1));
		}
	if (enable_push_right_2)	
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_2", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_2", 1));
		}
		
	Sleep(50);
	
	/* LEFT & RIGHT PUSH 3 */
	if (enable_push_left_3)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_3", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_3", 0));
		}
	if (enable_push_right_3)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_3", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_3", 0));
		}
	
		Sleep(timeon); 
		
	if (enable_push_left_3)
		{
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_3", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_3", 1));
		}
	if (enable_push_right_3)	
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_3", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_3", 1));
		}
		
	Sleep(50);
	
	/* LEFT & RIGHT PUSH 4 */
	if (enable_push_left_4)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_4", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_4", 0));
		}
	if (enable_push_right_4)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_4", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_4", 0));
		}
	
		Sleep(timeon); 
		
	if (enable_push_left_4)
		{
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_4", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_4", 1));
		}
	if (enable_push_right_4)	
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_4", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_4", 1));
		}
		
	Sleep(50);
	
	/* LEFT & RIGHT PUSH 5 */
	if (enable_push_left_5)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_5", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_5", 0));
		}
//	if (enable_push_right_5)
//		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_5", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_5", 0));
	//	printf("down 5\r\n"); 
//		}
	
		Sleep(timeon); 
		
	if (enable_push_left_5)
		{
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_5", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_5", 1));
		}
//	if (enable_push_right_5)	
//		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_5", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_5", 1));
	//	printf("UP 5\r\n"); 
//		}
		
	Sleep(50);
	
	/* LEFT & RIGHT PUSH 6 */
	if (enable_push_left_6)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_6", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_6", 0));
		}
	if (enable_push_right_6)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_6", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_6", 0));
		}
	
		Sleep(timeon); 
		
	if (enable_push_left_6)
		{
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_6", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_6", 1));
		}
	if (enable_push_right_6)	
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_6", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_6", 1));
		}
		
	Sleep(50);
	
	/* LEFT & RIGHT PUSH 7+8 */
	if (enable_push_left_7)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_7", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_7", 0));
		}
	if (enable_push_right_7)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_7", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_7", 0));
		}
	if (enable_push_right_8)
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_8", 1));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_8", 0));
		}
	
		Sleep(timeon); 
		
	if (enable_push_left_7)
		{
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_LEFT_7", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_LEFT_7", 1));
		}
	if (enable_push_right_7)	
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_7", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_7", 1));
		}
	if (enable_push_right_8)	
		{	
		EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_RIGHT_8", 0));
		EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_RIGHT_8", 1));
		}
		
	Sleep(50);
	
	}

}

Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 30 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation30"
static SElExceptionPtr PlugPanelStation30
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_30, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_30", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_30", 0));
	}
	
	if (timeout)
	{
		if(gs_plugged_station_30) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_30) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 30 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation30"
static SElExceptionPtr UnplugPanelStation30
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_30, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_30", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_30", 0));

	if (timeout)
	{
		if(gs_unplugged_station_30) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_30) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 40 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation40"
static SElExceptionPtr PlugPanelStation40
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_40, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_40", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40", 0));
	}
	
	if (timeout)
	{
		if(gs_plugged_station_40) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_40) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Unplug Panel station 40 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation40"
static SElExceptionPtr UnplugPanelStation40
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_40, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_40", 0));

	if (timeout)
	{
		if(gs_unplugged_station_40) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_40) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Start Rodage Left station 50 */
#undef __FUNC__
#define __FUNC__	"StartRodageLeftStation50"
static SElExceptionPtr StartRodageLeftStation50
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_START_RODAGE_LEFT_STATION_50, &hevent));

	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_LEFT_STATION_50", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_LEFT_STATION_50", 0));

	if (timeout)
	{
		if(gs_started_rodage_left_station_50) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_started_rodage_left_station_50) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Stop Rodage Left station 50 */
#undef __FUNC__
#define __FUNC__	"StopRodageLeftStation50"
static SElExceptionPtr StopRodageLeftStation50
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_RODAGE_LEFT_STATION_50, &hevent));

	EXCCHECK( me->SetSensor(me, "RODAGE_UP_LEFT_STATION_50", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_LEFT_STATION_50", 0));

	if (timeout)
	{
		if(gs_stopped_rodage_left_station_50) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_stopped_rodage_left_station_50) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Start Rodage Right station 50 */
#undef __FUNC__
#define __FUNC__	"StartRodageRightStation50"
static SElExceptionPtr StartRodageRightStation50
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_START_RODAGE_RIGHT_STATION_50, &hevent));

	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_RIGHT_STATION_50", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_RIGHT_STATION_50", 0));

	if (timeout)
	{
		if(gs_started_rodage_right_station_50) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_started_rodage_right_station_50) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Stop Rodage Right station 50 */
#undef __FUNC__
#define __FUNC__	"StopRodageRightStation50"
static SElExceptionPtr StopRodageRightStation50
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_RODAGE_RIGHT_STATION_50, &hevent));

	EXCCHECK( me->SetSensor(me, "RODAGE_UP_RIGHT_STATION_50", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_RIGHT_STATION_50", 0));

	if (timeout)
	{
		if(gs_stopped_rodage_right_station_50) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_stopped_rodage_right_station_50) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 60 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation60"
static SElExceptionPtr PlugPanelStation60
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_60, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_60", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60", 0));
	}
	
	if (timeout)
	{
		if(gs_plugged_station_60) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_60) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Unplug Panel station 60 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation60"
static SElExceptionPtr UnplugPanelStation60
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_60, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_60", 0));

	if (timeout)
	{
		if(gs_unplugged_station_60) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_60) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Start Test Left station 60 */
#undef __FUNC__
#define __FUNC__	"StartTestLeftStation60"
static SElExceptionPtr StartTestLeftStation60
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_START_TEST_LEFT_STATION_60, &hevent));

	EXCCHECK( me->SetSensor(me, "TEST_DOWN_LEFT_STATION_60", 1));
	EXCCHECK( me->SetSensor(me, "TEST_UP_LEFT_STATION_60", 0));

	if (timeout)
	{
		if(gs_started_test_left_station_60) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_started_test_left_station_60) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*StOP Test Left station 60 */
#undef __FUNC__
#define __FUNC__	"StopTestLeftStation60"
static SElExceptionPtr StopTestLeftStation60
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_TEST_LEFT_STATION_60, &hevent));

	EXCCHECK( me->SetSensor(me, "TEST_UP_LEFT_STATION_60", 1));
	EXCCHECK( me->SetSensor(me, "TEST_DOWN_LEFT_STATION_60", 0));
//	printf("STOP TEST LEFT ");  

	if (timeout)
	{
		if(gs_stopped_test_left_station_60) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_stopped_test_left_station_60) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Start Test Right station 60 */
#undef __FUNC__
#define __FUNC__	"StartTestRightStation60"
static SElExceptionPtr StartTestRightStation60
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_START_TEST_RIGHT_STATION_60, &hevent));

	EXCCHECK( me->SetSensor(me, "TEST_DOWN_RIGHT_STATION_60", 1));
	EXCCHECK( me->SetSensor(me, "TEST_UP_RIGHT_STATION_60", 0));

	if (timeout)
	{
		if(gs_started_test_right_station_60) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_started_test_right_station_60) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*StOP Test Right station 60 */
#undef __FUNC__
#define __FUNC__	"StopTestRightStation60"
static SElExceptionPtr StopTestRightStation60
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_TEST_RIGHT_STATION_60, &hevent));

	EXCCHECK( me->SetSensor(me, "TEST_UP_RIGHT_STATION_60", 1));
	EXCCHECK( me->SetSensor(me, "TEST_DOWN_RIGHT_STATION_60", 0));
//	printf("STOP TEST RIGHT ");

	if (timeout)
	{
		if(gs_stopped_test_right_station_60) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_stopped_test_right_station_60) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 70 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation70"
static SElExceptionPtr PlugPanelStation70
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_70, &hevent));
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_70", 0)); 	
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_70", 1));
	}
	
	if (timeout)
	{
		if(gs_plugged_station_70) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_70) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 70 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation70"
static SElExceptionPtr UnplugPanelStation70
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_70, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_70", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_70", 0));

	if (timeout)
	{
		if(gs_unplugged_station_70) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_70) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 70 */
#undef __FUNC__
#define __FUNC__	"IlluminationStation70"
static SElExceptionPtr IlluminationStation70
(
	struct _STester* me,
	int32_t			status
)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( me->SetSensor(me, "LIGHT", status));
	
Error:
	EXCRETHROW(pexception);
}

/* Lock Panel station 80 */
#undef __FUNC__
#define __FUNC__	"LockPanelStation80"
static SElExceptionPtr LockPanelStation80
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_LOCKED_STATION_80, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_LOCK_STATION_80", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNLOCK_STATION_80", 0));
	}
	
	if (timeout)
	{
		if(gs_locked_station_80) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_locked_station_80) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Unlock Panel station 80 */
#undef __FUNC__
#define __FUNC__	"UnlockPanelStation80"
static SElExceptionPtr UnlockPanelStation80
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNLOCKED_STATION_80, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNLOCK_STATION_80", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_LOCK_STATION_80", 0));

	if (timeout)
	{
		if(gs_unlocked_station_80) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unlocked_station_80) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Thread_PickPlace station 80 */
#undef __FUNC__
#define __FUNC__ "Thread_PickPlace"
static DWORD WINAPI Thread_PickPlace(LPVOID param)
{
	SMaskThreadDataPtr	pthread_data = (SMaskThreadDataPtr)param;
	STesterPtr		me = pthread_data->me;
	uint32_t		timeout = pthread_data->timeout;
	SElExceptionPtr	pexception = NULL;
	int32_t			step = INIT;
	bool_t			is_product_took = FALSE;
	bool_t			cylinder_x_front,
					cylinder_x_rear,
					cylinder_z_up,
					cylinder_z_down,
					hand_block_left,
					hand_unblock_left,
					hand_block_right,
					hand_unblock_right;

	EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1));
 	EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0));
	Sleep (500);
	
	TIMEOUT_INIT();
	step = INIT;
	
	while( TRUE)
	{
		if (!is_product_took)
		{
			switch  (step)
			{
			case  INIT :
			
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &cylinder_z_up));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &cylinder_z_down));
				   
				   if(!cylinder_z_up && cylinder_z_down) 
							{
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0));
				 			}  
				   else if (cylinder_z_up && !cylinder_z_down) 
							{
							step ++;		
				 			} 
			break;
			
			case  MOVE_TO_PALETT : 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &cylinder_x_front));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &cylinder_x_rear));
				   
				   if(!cylinder_x_front && cylinder_x_rear) 
							{
							EXCCHECK( me->SetSensor(me, "CYLINDER_X_REAR_STATION_80", 0));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_X_FRONT_STATION_80", 1));
				 			}  
				   else if (cylinder_x_front && !cylinder_x_rear) 
							{
							step ++;		
				 			} 
			break;
			
			case  MOVE_TO_PANEL : 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &cylinder_z_up));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &cylinder_z_down));
				   
				   if(!cylinder_z_down && cylinder_z_up) 
							{
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 0));
				 			}  
				   else if (cylinder_z_down && !cylinder_z_up) 
							{
							step ++;		
				 			} 
			break;											 
																
			case  TAKE_ON_PANEL: 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_BLOCK_LEFT_STATION_80", &hand_block_left));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_UNBLOCK_LEFT_STATION_80", &hand_unblock_left));
					EXCCHECK( me->GetSensor(me, "SENSOR_BLOCK_RIGHT_STATION_80", &hand_block_right));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_UNBLOCK_RIGHT_STATION_80", &hand_unblock_right));
				   if(!hand_block_left && hand_unblock_left &&
					  !hand_block_right && hand_unblock_right) 
							{
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_BLOCK_LEFT_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_UNBLOCK_LEFT_STATION_80", 0));
							EXCCHECK( me->SetSensor(me, "CYLINDER_BLOCK_RIGHT_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_UNBLOCK_RIGHT_STATION_80", 0));
				 			}  
				   else if(hand_block_left && !hand_unblock_left &&
					  	   hand_block_right && !hand_unblock_right) 
				   			{
							Sleep(300);
							step ++;		
				 			} 
			break;
			
			case  PICK_UP_PANEL : 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &cylinder_z_up));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &cylinder_z_down));
				   
				   if(!cylinder_z_up && cylinder_z_down) 
							{
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0));
				 			}  
				   else if (cylinder_z_up && !cylinder_z_down) 
							{
							step ++;		
				 			} 
			break;
			
			
			case  MOVE_TO_BOX : 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &cylinder_x_front));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &cylinder_x_rear));
				   
				   if(!cylinder_x_rear && cylinder_x_front) 
							{
							EXCCHECK( me->SetSensor(me, "CYLINDER_X_REAR_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_X_FRONT_STATION_80", 0));
				 			}  
				   else if (cylinder_x_rear && !cylinder_x_front) 
							{
							step ++;		
				 			} 
			break;

			case  MOVE_DOWN_TO_BOX : 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &cylinder_z_up));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &cylinder_z_down));
				   if(!cylinder_z_down && cylinder_z_up) 
							{
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 0));
				 			}  
				   else if (cylinder_z_down && !cylinder_z_up) 
							{
							step ++;		
				 			} 
			break;
			
			case  TAKE_OFF_PANEL: 
				
	  				EXCCHECK( me->GetSensor(me, "SENSOR_BLOCK_LEFT_STATION_80", &hand_block_left));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_UNBLOCK_LEFT_STATION_80", &hand_unblock_left));
					EXCCHECK( me->GetSensor(me, "SENSOR_BLOCK_RIGHT_STATION_80", &hand_block_right));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_UNBLOCK_RIGHT_STATION_80", &hand_unblock_right));
				   if(!hand_unblock_left && hand_block_left &&
					  !hand_unblock_right && hand_block_right) 
							{
							EXCCHECK( me->SetSensor(me, "CYLINDER_UNBLOCK_LEFT_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_BLOCK_LEFT_STATION_80", 0));
							EXCCHECK( me->SetSensor(me, "CYLINDER_UNBLOCK_RIGHT_STATION_80", 1));
							EXCCHECK( me->SetSensor(me, "CYLINDER_BLOCK_RIGHT_STATION_80", 0));
				 			}  
				   else if(hand_unblock_left && !hand_block_left &&
					  	   hand_unblock_right && !hand_block_right) 
				   			{
							step ++;		
				 			}
			break;
				
			case  PICK_PLACE_FINIHED :
				 is_product_took = TRUE;
			break;
				
			}
		}

		else
			break;
		TIMEOUT_CHECK( timeout);
		Sleep(100);
	}

Error:
	free(pthread_data);
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;	
}

/* PickPlace station 80 */
#undef __FUNC__
#define __FUNC__	"Tester::PickPlace"
static SElExceptionPtr PickPlace(struct _STester* me, int32_t timeout, bool_t wait_end)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SMaskThreadDataPtr	pthread_data = NULL;
	
	bool_t				is_hand_block_left = FALSE,
						is_hand_unblock_left = FALSE,
						is_hand_block_right = FALSE,
						is_hand_unblock_right = FALSE,
						is_cylinder_z_up = FALSE,
						is_cylinder_z_down = FALSE,
						is_cylinder_x_rear = FALSE,
						is_cylinder_x_front = FALSE,
						is_panel_unlocked = FALSE,
						is_panel_locked = FALSE;

	int32_t				microCam;
	

	  EXCCHECK( me->GetSensor(me, "SENSOR_BLOCK_LEFT_STATION_80", &is_hand_block_left));
	  EXCCHECK( me->GetSensor(me, "SENSOR_UNBLOCK_LEFT_STATION_80", &is_hand_unblock_left));
	  EXCCHECK( me->GetSensor(me, "SENSOR_BLOCK_RIGHT_STATION_80", &is_hand_block_right));
	  EXCCHECK( me->GetSensor(me, "SENSOR_UNBLOCK_RIGHT_STATION_80", &is_hand_unblock_right));
	  EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &is_cylinder_z_up));
	  EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &is_cylinder_z_down));
	  EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &is_cylinder_x_rear));
	  EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &is_cylinder_x_front));
 	  EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &is_panel_locked));
	  EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &is_panel_unlocked));

	if(is_panel_unlocked && !is_panel_locked &&
	   is_hand_unblock_left && !is_hand_block_left &&
	   is_hand_unblock_right && !is_hand_block_right &&
	   is_cylinder_z_up && !is_cylinder_z_down &&
	   is_cylinder_x_rear && !is_cylinder_x_front)
	{
		
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "BRAKE_RODAGE_STATION_20", 1));
	pthread_data = (SMaskThreadDataPtr)calloc(1, sizeof(SMaskThreadData)); 
	EXCCHECKALLOC( pthread_data);

	pthread_data->me = me;
	pthread_data->timeout = timeout;
	
		if(me->hThreadPickPlace)
		{
		EXCCHECKWIN( TerminateThread(me->hThreadPickPlace, THREAD_EXIT_CODE));
		}

		if(me->hThreadPickPlace)
		{
		CloseHandle(me->hThreadPickPlace);
		me->hThreadPickPlace = NULL;
		}
		
		EXCCHECK( ThreadStart(Thread_PickPlace, pthread_data, &me->hThreadPickPlace, NULL));

		if(wait_end)
		{
		EXCCHECK( me->IsProductFaultTook(me, timeout + 2000));
		}
	}
	}
	else 
		EXCTHROW( TESTER_ERROR_PICK_PLACE, "Error pick & place");
	

Error:
	EXCRETHROW(pexception);
}

/* IsProductFaultTook station 80 */
#undef __FUNC__
#define __FUNC__	"Tester::IsProductFaultTook"
static SElExceptionPtr IsProductFaultTook(struct _STester* me, int32_t timeout)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			status;
		
	pexception = ThreadStatus(me->hThreadPickPlace, timeout, &status);
	if(pexception)
	{
		EXCDELETE(pexception);
		EXCTHROW(TESTER_ERROR_WAIT_TIMEOUT, "Thread Handle Status: WAIT_TIMEOUT");
	}
	else
	{
		switch(status)
		{
			case THREAD_STATUS_STILL_ACTIVE:
				EXCTHROW( TESTER_ERROR_STILL_ACTIVE, "Thread Handle Status: STILL_ACTIVE");
				break;
			case THREAD_STATUS_ERROR_CODE:
				EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
				break;
		}
	}

Error:
	EXCRETHROW(pexception);
}

/* Thread_ResetPickPlace station 80 */
#undef __FUNC__
#define __FUNC__ "Thread_ResetPickPlace"
static DWORD WINAPI Thread_ResetPickPlace(LPVOID param)
{
	SMaskThreadDataPtr	pthread_data = (SMaskThreadDataPtr)param;
	STesterPtr		me = pthread_data->me;
	uint32_t		timeout = pthread_data->timeout;
	SElExceptionPtr	pexception = NULL;
	uint32_t		step = PICK_UP_PANEL ;     //TAKE_OFF_PANEL;
	bool_t			is_product_took = FALSE;
	bool_t			cylinder_x_front,
					cylinder_x_rear,
					cylinder_z_up,
					cylinder_z_down,
					hand_block_left,
					hand_unblock_left,
					hand_block_right,
					hand_unblock_right;

	TIMEOUT_INIT();
	
	while( TRUE)
	{  
		
//	printf("step = %d\r\n", step);   
	
	if (!is_product_took)					 
	{
		switch  (step)
			{
//			case  TAKE_OFF_PANEL: 
//				
//	  				EXCCHECK( me->GetSensor(me, "SENSOR_BLOCK_LEFT_STATION_80", &hand_block_left));
//	  				EXCCHECK( me->GetSensor(me, "SENSOR_UNBLOCK_LEFT_STATION_80", &hand_unblock_left));
//					EXCCHECK( me->GetSensor(me, "SENSOR_BLOCK_RIGHT_STATION_80", &hand_block_right));
//	  				EXCCHECK( me->GetSensor(me, "SENSOR_UNBLOCK_RIGHT_STATION_80", &hand_unblock_right));
//					
//				//	printf("hand_unblock_left= %d\r\n",hand_unblock_left);
//				//	printf("hand_unblock_right= %d\r\n",hand_unblock_right);
//						 
//				    if(hand_unblock_left && !hand_block_left &&
//					  	   hand_unblock_right && !hand_block_right) 
//				   			{
//							step = PICK_UP_PANEL;		
//				 			}
//					else 
//					{
//					EXCCHECK( me->SetSensor(me, "CYLINDER_UNBLOCK_LEFT_STATION_80", 1));
//				   	EXCCHECK( me->SetSensor(me, "CYLINDER_BLOCK_LEFT_STATION_80", 0));
//					EXCCHECK( me->SetSensor(me, "CYLINDER_UNBLOCK_RIGHT_STATION_80", 1));
//					EXCCHECK( me->SetSensor(me, "CYLINDER_BLOCK_RIGHT_STATION_80", 0));
//					}
//			break;
			
			case  PICK_UP_PANEL : 
				   	
		   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1));
				  	EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0));
					
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &cylinder_z_up));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &cylinder_z_down));
				   
				   if((!cylinder_z_up && cylinder_z_down) || (!cylinder_z_up && !cylinder_z_down))
							{
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0));
				 			}  
				   else if (cylinder_z_up && !cylinder_z_down) 
							{
							step = MOVE_TO_BOX;		
				 			} 
			break;
			
			case  MOVE_TO_BOX : 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &cylinder_x_front));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &cylinder_x_rear));
				   
				   if( (!cylinder_x_rear && !cylinder_x_front) || (!cylinder_x_rear && cylinder_x_front)) 
							{
							EXCCHECK( me->SetSensor(me, "CYLINDER_X_REAR_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_X_FRONT_STATION_80", 0));
				 			}  
				   else if (cylinder_x_rear && !cylinder_x_front) 
							{
							step  = PICK_PLACE_FINIHED;		
				 			} 
			break;
			
			case  PICK_PLACE_FINIHED :
				
				 is_product_took = TRUE;
				 
			break;
				
			}
		}

	else break;
		
		TIMEOUT_CHECK( timeout);
		Sleep(100);
		
	}

Error:
	free(pthread_data);
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;	
}

/* PickPlace station 80 */
#undef __FUNC__
#define __FUNC__	"Tester::ResetPickPlace"
static SElExceptionPtr ResetPickPlace(struct _STester* me, int32_t timeout, bool_t wait_end)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SMaskThreadDataPtr	pthread_data = NULL;
	

	pthread_data = (SMaskThreadDataPtr)calloc(1, sizeof(SMaskThreadData)); 
	EXCCHECKALLOC( pthread_data);

	pthread_data->me = me;
	pthread_data->timeout = timeout;
	
		EXCCHECK( me->SetSensor(me, "BRAKE_RODAGE_STATION_20", 1));
		
		if(me->hThreadPickPlace)
		{
		EXCCHECKWIN( TerminateThread(me->hThreadPickPlace, THREAD_EXIT_CODE));
		}
			
		if(me->hThreadPickPlace)
		{
		CloseHandle(me->hThreadPickPlace);
		me->hThreadPickPlace = NULL;
		}
		
		EXCCHECK( ThreadStart(Thread_ResetPickPlace, pthread_data, &me->hThreadPickPlace, NULL));

		if(wait_end)
		{
		EXCCHECK( me->IsResetPickPlace(me, timeout + 2000));
		}
	

Error:
	EXCRETHROW(pexception);
}

/* IsProductFaultTook station 80 */
#undef __FUNC__
#define __FUNC__	"Tester::IsResetPickPlace"
static SElExceptionPtr IsResetPickPlace(struct _STester* me, int32_t timeout)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			status;
		
	pexception = ThreadStatus(me->hThreadPickPlace, timeout, &status);
	if(pexception)
	{
		EXCDELETE(pexception);
		EXCTHROW(TESTER_ERROR_WAIT_TIMEOUT, "Thread Handle Status: WAIT_TIMEOUT");
	}
	else
	{
		switch(status)
		{
			case THREAD_STATUS_STILL_ACTIVE:
				EXCTHROW( TESTER_ERROR_STILL_ACTIVE, "Thread Handle Status: STILL_ACTIVE");
				break;
			case THREAD_STATUS_ERROR_CODE:
				EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
				break;
		}
	}

Error:
	EXCRETHROW(pexception);
}
/*Move Cylinder Barcode Front */
#undef __FUNC__
#define __FUNC__	"MoveBarcodeFront"
static SElExceptionPtr MoveBarcodeFront
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr		pexception = NULL;
	HANDLE				hevent = NULL;
	
	bool_t				is_laser_work = TRUE,
						is_barcode_rear = FALSE,
						is_cycle_laser_finished = FALSE,
						is_cycle_laser_error = FALSE;
	
	clock_t 					_start_clock;
	double   					time;
	
	/* check for avoid crash */
	_start_clock =  clock();
	do{
		Sleep(100);
		me->GetSensor(me, "LASER_WORK", &is_laser_work);
		time = (double)(clock()-_start_clock);
	}while (is_laser_work == 1 && time < timeout);	
	
	if (time >= timeout)
	{
		EXCTHROW( TESTER_ERROR_TIMEOUT, "MoveBarcodeFront Timeouted");
	}
	
	EXCCHECK( me->SetSensor(me, "CYLINDER_BARCODE_FRONT", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_BARCODE_REAR", 0));
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_BARCODE_FRONT, &hevent));
	if (timeout)
	{
		if(gs_barcode_already_front_station_80) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_barcode_already_front_station_80) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Move Cylinder Barcode Rear */
#undef __FUNC__
#define __FUNC__	"MoveBarcodeRear"
static SElExceptionPtr MoveBarcodeRear
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_BARCODE_REAR, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_BARCODE_REAR", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_BARCODE_FRONT", 0));

	if (timeout)
	{
		if(gs_barcode_already_rear_station_80) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_barcode_already_rear_station_80) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* cycle laser   */
#undef __FUNC__
#define __FUNC__	"CycleLaser"
static SElExceptionPtr CycleLaser
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	bool_t			laser_work = 1;
	bool_t			laser_error;
	clock_t 					_start_clock;
	double   					time;
	static SObjListPtr     		gs_Modules = NULL;
	int confirmation;
	SLasersPtr	plasers = (SLasersPtr)gs_Modules->Get(gs_Modules, MODULE_LASERS);  
	SLaserPtr		            plaser = NULL;
	int*			            Ready=0;
	
	
	EXCCHECK( plasers->GetLaserByIndex(plasers, 0, &plaser));  
	
	
	
	      EXCCHECK( plaser->RunProgram(plaser, 0000, 000));               //RunProgram(SLaserPtr me, int program, int block)
	 	  EXCCHECK( plaser->WriteString(plaser,0000,000, "Hello world!"));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		  EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
		  EXCCHECK( plaser->MarkerReady(plaser,Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
		  EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
		  Sleep(500);
		 _start_clock =  clock(); 
		do
		   {
			   EXCCHECK( plaser->MarkerReady(plaser,Ready));
			   
			   if (*Ready=0)
				   break;
			  Sleep(100);
			  time = (double)(clock()-_start_clock);
			   
		   } while(time<15000);
		 EXCCHECK( plaser->RunProgram(plaser, 0001, 000));               //RunProgram(SLaserPtr me, int program, int block)
	 	  EXCCHECK( plaser->WriteString(plaser,0001,000, "Hello world!"));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		  EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
		  EXCCHECK( plaser->MarkerReady(plaser,Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
		  EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
		  Sleep(500);
		 _start_clock =  clock(); 
		do
		   {
			   EXCCHECK( plaser->MarkerReady(plaser,Ready));
			   
			   if (*Ready=0)
				   break;
			  Sleep(100);
			  time = (double)(clock()-_start_clock);
			   
		   } while(time<15000);
		 EXCCHECK( plaser->RunProgram(plaser, 0002, 000));               //RunProgram(SLaserPtr me, int program, int block)
	 	  EXCCHECK( plaser->WriteString(plaser,0002,000, "Hello world!"));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		  EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
		  EXCCHECK( plaser->MarkerReady(plaser,Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
		  EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
		  Sleep(500);
		 _start_clock =  clock(); 
		do
		   {
			   EXCCHECK( plaser->MarkerReady(plaser,Ready));
			   
			   if (*Ready=0)
				   break;
			  Sleep(100);
			  time = (double)(clock()-_start_clock);
			   
		   } while(time<15000);
		 EXCCHECK( plaser->RunProgram(plaser, 0003, 000));               //RunProgram(SLaserPtr me, int program, int block)
	 	  EXCCHECK( plaser->WriteString(plaser,0003,000, "Hello world!"));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		  EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
		  EXCCHECK( plaser->MarkerReady(plaser,Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
		  EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
		  Sleep(500);
		 _start_clock =  clock(); 
		do
		   {
			   EXCCHECK( plaser->MarkerReady(plaser,Ready));
			   
			   if (*Ready=0)
				   break;
			  Sleep(100);
			  time = (double)(clock()-_start_clock);
			   
		   } while(time<15000);
		if (time >= 15000)	
	{		
		EXCTHROW(-1, "Timeout axis");
	}
Error:
//	me->SetSensor(me, "ASPIRATOR", 0);
//	me->SetSensor(me, "START_LASER", 0);
	EXCRETHROW(pexception);
}

/*GetStatusRedBox */
#undef __FUNC__
#define __FUNC__	"GetStatusRedBox"
static SElExceptionPtr GetStatusRedBox
(
	struct _STester* me,
	int32_t			*full,
	int32_t			*closed
)
{
	SElExceptionPtr	pexception = NULL;
    EXCCHECK( me->GetSensor(me, "SENSOR_RED_BOX_FULL", full));
    EXCCHECK( me->GetSensor(me, "SENSOR_RED_BOX", closed));
	
Error:
	EXCRETHROW(pexception);
}

/* Initialize Stations */
#undef __FUNC__
#define __FUNC__	"InitializeStations"
static SElExceptionPtr InitializeStations
(
	struct _STester* me,
	int32_t			station,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	HANDLE			hevent = NULL;
	SaxisPtr		paxis = NULL; 

	switch (station)
	{
	
	case STATION_10 :
#if 0    /* formerly excluded lines */
	EXCCHECK( me->UnlockPanelStation10(me, timeout));
#endif   /* formerly excluded lines */
	break;
	
	case STATION_20 :
	EXCCHECK( me->StopRodageStation20(me, timeout));
	break;
	
	case STATION_30 :
	me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_30_RIGHT, &paxis);
	paxis->EnableAxis(paxis, AXIS_Z); 
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z); 
	me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_30_LEFT, &paxis);
	paxis->EnableAxis(paxis, AXIS_Z); 
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z); 
	me->UnplugPanelStation30(me, timeout);
	break;
	
	case STATION_40 :
	me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_40_RIGHT, &paxis);
	paxis->EnableAxis(paxis, AXIS_Z); 
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z); 
	me->axissAPI->GetaxisByIndex(me->axissAPI, SYSTEM_AXIS_STATION_40_LEFT, &paxis);
	paxis->EnableAxis(paxis, AXIS_Z); 
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z); 
	me->UnplugPanelStation40(me, timeout);
	break;

	case STATION_50 :
	EXCCHECK( me->StopRodageLeftStation50(me, timeout));
	EXCCHECK( me->StopRodageRightStation50(me, timeout));
	break;

	case STATION_60 :
	EXCCHECK( me->UnplugPanelStation60(me, timeout));
	EXCCHECK( me->StopTestLeftStation60(me, timeout));
	EXCCHECK( me->StopTestRightStation60(me, timeout));
	break;

	case STATION_70 :
	EXCCHECK( me->UnplugPanelStation70(me, timeout));
	EXCCHECK( me->IlluminationStation70(me, 0));
	break;

	case STATION_80 :
	EXCCHECK( me->ResetPickPlace(me, 10000, TRUE));
	EXCCHECK( me->LockPanelStation80(me, timeout));
	EXCCHECK( me->MoveBarcodeRear(me, timeout));
	break;
	}
	
Error:
	EXCDISPLAY(pexception);
	return 0;
}

/* Wait for table rotation */
#undef __FUNC__
#define __FUNC__	"WaitRotationFinished"
static SElExceptionPtr WaitRotationFinished
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	HANDLE			hevent = NULL;
	bool_t   		microCam;

		EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_TESTER_ROTATION_FINISHED, &hevent));
		
		if (timeout)
		{
			if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
			{
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
			}
		}

Error:
	EXCRETHROW(pexception);
}

/* Get barcode indentification of panel */
#undef __FUNC__
#define __FUNC__	"GetItemCode"
static SElExceptionPtr GetItemCode
(
	struct _STester* me,
	const char*		product,
	int32_t			index,
	long			bufferSize,
	char*			buffer,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			barcode_event = 0;
	SBCRPtr	last_bcr = NULL;

	if (me->pBarCodeReader->reader)
	{
		last_bcr = me->pBarCodeReader + (index);

		EXCCHECK( (last_bcr->reader)->StartReading(last_bcr->reader));
		Sleep(800);

		pexception = (last_bcr->reader)->GetBarCode(last_bcr->reader, buffer, bufferSize);
		if (pexception)
		{
		EXCCHECK( (last_bcr->reader)->GetBarCode(last_bcr->reader, buffer, bufferSize));
		}

		Sleep(100);
		EXCCHECK( (last_bcr->reader)->StopReading(last_bcr->reader));

	}

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncAbis"
static SElExceptionPtr CallbackFncAbis(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;

	if( state==1 && me->eventAPI)
	{
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_GAUCHE", 1));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_ARRIERE", 1));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_AVANT", 1));
	}
	else if( state==0 && me->eventAPI)
	{
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_GAUCHE", 0));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_ARRIERE", 0));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_AVANT", 0));
	}

Error:
	EXCRETHROW(pexception);
}

/*----------------------------------------------------------------------------*/
/* Activate Axis */
//#if 0    /* formerly excluded lines */
#undef __FUNC__
#define __FUNC__	"Activate_Axis"
static SElExceptionPtr Activate_Axis
(
	struct _STester* me
)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( me->SetSensor(me, "DISACTIVATE_Z_30_RIGHT", 0));
	Sleep(200);
	EXCCHECK( me->SetSensor(me, "DISACTIVATE_Z_30_LEFT", 0));
	Sleep(200);
	EXCCHECK( me->SetSensor(me, "DISACTIVATE_Z_40_RIGHT", 0)); 
	Sleep(200);
	EXCCHECK( me->SetSensor(me, "DISACTIVATE_Z_40_LEFT", 0)); 
	
	
Error:
	EXCRETHROW(pexception);
}

/* Disactivate Axis */
#undef __FUNC__
#define __FUNC__	"Disactivate_Axis"
static SElExceptionPtr Disactivate_Axis
(
	struct _STester* me
)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( me->SetSensor(me, "DISACTIVATE_Z_30_RIGHT", 1));
	EXCCHECK( me->SetSensor(me, "DISACTIVATE_Z_30_LEFT", 1)); 
	EXCCHECK( me->SetSensor(me, "DISACTIVATE_Z_40_RIGHT", 1)); 
	EXCCHECK( me->SetSensor(me, "DISACTIVATE_Z_40_LEFT", 1)); 
	
	
Error:
	EXCRETHROW(pexception);
}
//#endif   /* formerly excluded lines */

