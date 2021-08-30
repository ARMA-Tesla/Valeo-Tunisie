#include "tester.h"
#include "msxmldom.h"
#include <guid.h>
#include <time.h>
#include <utility.h>
#include <sdk/mmsystem.h>
#include <multithread/multithread.h>
#include <userint.h>
#include <ansi_c.h>
#include <cviauto.h>
#include <formatio.h>
#include "toolbox.h"
#include <cvirte.h>

#define PATH_PRODUCTION					"%s\\imprimante\\%04d.%02d.%02d"
#define DATE_TIME_FORMAT				"%02d.%02d.%02d %02d.%02d.%02d"


char				TicketDirPath[1024];
char                Production_Serial[4+1];
char				Time[32];
char				Date[32];
char				TicketFilePath[1024];

CRITICAL_SECTION   _Lock;


#define MAX_LENGTH_SENSOR_LOGICAL_NAME				64

#define TIMEOUT_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_CHECK(timeout) \
	if ( timeout>0 && ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout"); } else

#define TIMEOUT_AXIS_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_AXIS_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( TESTER_ERROR_TIMEOUT, "Axis position error"); } else

#define TIMEOUT_PRINTER_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( TESTER_ERROR_CYCLE_PRINTER, "Cycle print error"); } else

#define NUMBER_OF_STATIONS	  8+1

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2

#define CHECKER_RIGHT  		0
#define CHECKER_LEFT  		1
#define CHECKER_CENTRAL  	2

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
static int32_t	gs_clipped_punzoni= 0;
static int32_t	gs_stop_clipped_punzoni= 0;
static int32_t	gs_clipped_contrasti= 0;
static int32_t	gs_stop_clipped_contrasti= 0;
static int32_t	gs_checked = 0;
static int32_t	gs_stop_checked = 0;
static int32_t	gs_started_rodage = 0;
static int32_t	gs_stopped_rodage = 0;
static int32_t	gs_locked_station_10 = 0;
static int32_t	gs_unlocked_station_10 = 0;
static int32_t	gs_vs11_selected_station_40 = 0;
static int32_t	gs_x52_selected_station_40 = 0;
static int32_t	gs_plugged_station_40_x52 = 0;
static int32_t	gs_unplugged_station_40_x52 = 0;
static int32_t	gs_plugged_station_40_vs11 = 0;
static int32_t	gs_unplugged_station_40_vs11 = 0;
static int32_t	gs_vs11_selected_station_50 = 0;
static int32_t	gs_x52_selected_station_50 = 0;
static int32_t	gs_plugged_station_50_x52 = 0;
static int32_t	gs_unplugged_station_50_x52 = 0;
static int32_t	gs_plugged_station_50_vs11 = 0;
static int32_t	gs_unplugged_station_50_vs11 = 0;
static int32_t	gs_vs11_selected_station_60 = 0;
static int32_t	gs_x52_selected_station_60 = 0;
static int32_t	gs_plugged_station_60_x52 = 0;
static int32_t	gs_unplugged_station_60_x52 = 0;
static int32_t	gs_plugged_station_60_vs11 = 0;
static int32_t	gs_unplugged_station_60_vs11 = 0;
static int32_t	gs_locked_station_80 = 0;
static int32_t	gs_unlocked_station_80 = 0;
static int32_t	gs_axis_x_rear = 0; 
static int32_t	gs_axis_x_front = 0; 
static int32_t	gs_axis_z_up = 0;
static int32_t	gs_axis_z_down = 0;
static int32_t	gs_pince_open = 0;
static int32_t	gs_pince_close = 0;
static int32_t	gs_barcode_already_front = 0;
static int32_t	gs_barcode_already_rear = 0;
static int32_t	gs_table_started_rotation = -1;
static int32_t	gs_table_finished_rotation = -1;

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2
bool_t			is_axis_X_In_Home_40,
	 			is_axis_Y_In_Home_40,
				is_axis_Z_In_Home_40,
	 			is_axis_X_In_Home_50,
	 			is_axis_Y_In_Home_50,
	 			is_axis_Z_In_Home_50,
				is_axis_X_In_position_40,
	 			is_axis_Y_In_position_40,
	 			is_axis_Z_In_position_40,
				is_axis_X_In_position_50,
	 			is_axis_Y_In_position_50,
	 			is_axis_Z_In_position_50;


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
	OPEN_PINCE = 0,
	UNLOCK_PANEL,
	MOVE_TO_PALETT,
	MOVE_TO_PANEL,
	TAKE_ON_PANEL,
	PICK_UP_PANEL,
	MOVE_TO_BOX,
	PLACE_PANEL,
	TAKE_OFF_PANEL,
	MOVE_TO_HOME,
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

static SElExceptionPtr Initialize(struct _STester*	me, const char*	fileName);
static SElExceptionPtr RegisterSensorCallbacks( struct _STester* me);
static SElExceptionPtr TableRotate(struct _STester* me, int32_t timeout, bool_t wait_end);
static SElExceptionPtr GetStationStatus(struct _STester* me, char status[9]);
static SElExceptionPtr IsProductFaultTook(struct _STester* me, int32_t timeout);
static SElExceptionPtr Clipping_punzoni(struct _STester* me, int32_t timeout);
static SElExceptionPtr StopClipping_punzoni(struct _STester* me, int32_t timeout);
static SElExceptionPtr Clipping_contrasti(struct _STester* me, int32_t timeout);
static SElExceptionPtr StopClipping_contrasti(struct _STester* me, int32_t timeout);
static SElExceptionPtr CheckClipping(struct _STester* me, int32_t timeout);
static SElExceptionPtr StopCheckClipping(struct _STester* me, int32_t timeout);
static SElExceptionPtr SelectRodageX52(struct _STester* me, int32_t timeout);
static SElExceptionPtr SelectRodageVS11(struct _STester* me, int32_t timeout);
static SElExceptionPtr Rodage(struct _STester* me, int32_t timeout, int32_t cycle, int32_t timeon, int32_t timeoff, char version[8+1]);
static SElExceptionPtr LockPanelStation10(struct _STester* me, int32_t timeout);     
static SElExceptionPtr UnlockPanelStation10(struct _STester* me, int32_t timeout);     
static SElExceptionPtr SelectVS11Station40(struct _STester* me, int32_t timeout);
static SElExceptionPtr SelectX52Station40(struct _STester* me, int32_t timeout);
static SElExceptionPtr PlugPanelStation40_X52(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation40_X52(struct _STester* me, int32_t timeout);
static SElExceptionPtr PlugPanelStation40_VS11(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation40_VS11(struct _STester* me, int32_t timeout);
static SElExceptionPtr SelectVS11Station50(struct _STester* me, int32_t timeout);
static SElExceptionPtr SelectX52Station50(struct _STester* me, int32_t timeout);
static SElExceptionPtr PlugPanelStation50_X52(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation50_X52(struct _STester* me, int32_t timeout);
static SElExceptionPtr PlugPanelStation50_VS11(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation50_VS11(struct _STester* me, int32_t timeout);
static SElExceptionPtr SelectVS11Station60(struct _STester* me, int32_t timeout);
static SElExceptionPtr SelectX52Station60(struct _STester* me, int32_t timeout);
static SElExceptionPtr PlugPanelStation60_X52(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation60_X52(struct _STester* me, int32_t timeout);
static SElExceptionPtr PlugPanelStation60_VS11(struct _STester* me, int32_t timeout);
static SElExceptionPtr UnplugPanelStation60_VS11(struct _STester* me, int32_t timeout);
static SElExceptionPtr IlluminationStation60(struct _STester* me, int32_t timeout);
static SElExceptionPtr TestChecker(struct _STester* me, int32_t	checker, int32_t *output0, int32_t *output1, int32_t *output2, int32_t *output3);
static SElExceptionPtr PickPlace(struct _STester* me, int32_t timeout, bool_t wait_end);
static SElExceptionPtr IsProductFaultTook(struct _STester* me, int32_t timeout);
static SElExceptionPtr ResetPickPlace(struct _STester* me, int32_t timeout, bool_t wait_end);
static SElExceptionPtr IsResetPickPlace(struct _STester* me, int32_t timeout);
static SElExceptionPtr ReadyPrint(struct _STester* me, int32_t timeout);
static SElExceptionPtr CyclePrint(struct _STester* me, int32_t timeout);
static SElExceptionPtr Activate_Air_Printer(struct _STester* me);
static SElExceptionPtr Disactivate_Air_Printer(struct _STester* me);
static SElExceptionPtr GetStatusRedBox(struct _STester* me, int32_t	*full, int32_t	*closed);
static SElExceptionPtr DetectProductPresence(struct _STester* me, int32_t	*right, int32_t	*left);  
static SElExceptionPtr GetStatusPrinter(struct _STester* me, int32_t	*finished, int32_t	*error);
static SElExceptionPtr WaitRotationFinished(struct _STester* me, int32_t timeout);
static SElExceptionPtr OkNokCheck(struct _STester* me, int32_t timeout, bool_t* ok);
static SElExceptionPtr NokCheck(struct _STester* me, int32_t timeout);
static SElExceptionPtr TesterBeep(struct _STester* me, int32_t time);
static SElExceptionPtr SetSensor(struct _STester* me, const char* sensorId, uint32_t value);
static SElExceptionPtr GetSensor(struct _STester* me, const char* sensorId, uint32_t* value);
static SElExceptionPtr SetSensorCallback(struct _STester* me, const char* sensorId, SensorCallbackFnc_t fCall, void* pData);
static SElExceptionPtr GetItemCode(struct _STester* me, const char* product, int32_t index, long bufferSize, char* buffer,int32_t timeout);
static SElExceptionPtr CallbackFncButtonOk(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonNok(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonMenu(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonLabel(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonDowntime(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncAbis(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonStart(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncRotation(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncRotationStarted(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncButtonTotalStop(const char* name, uint32_t state, void* pData);
static SElExceptionPtr InitializeStations(struct _STester* me, int32_t station, int32_t timeout);
static SElExceptionPtr ResetStations(struct _STester* me, int32_t station, int32_t timeout);

static SElExceptionPtr Move_axis_x_station_40(struct _STester* me, int32_t	position, int32_t timeout);
static SElExceptionPtr Move_axis_y_station_40(struct _STester* me, int32_t	position, int32_t timeout);
static SElExceptionPtr Move_axis_z_station_40(struct _STester* me, int32_t	position, int32_t timeout);
static SElExceptionPtr Move_axis_x_station_50(struct _STester* me, int32_t	position, int32_t timeout);
static SElExceptionPtr Move_axis_y_station_50(struct _STester* me, int32_t	position, int32_t timeout);
static SElExceptionPtr Move_axis_z_station_50(struct _STester* me, int32_t	position, int32_t timeout);
static SElExceptionPtr CallbackFncaxis_x_station_40(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncaxis_y_station_40(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncaxis_z_station_40(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncaxis_x_station_50(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncaxis_y_station_50(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncaxis_z_station_50(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncRedBoxClosed(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncRedBoxEmpty(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncPrintReady(const char* name, uint32_t state, void* pData);
static SElExceptionPtr CallbackFncPrintFinished(const char* name, uint32_t state, void* pData);


static SElExceptionPtr MoveXFront(struct _STester* me, int32_t timeout);  
static SElExceptionPtr MoveXRear(struct _STester* me, int32_t timeout); 
static SElExceptionPtr MoveZUp(struct _STester* me, int32_t timeout); 
static SElExceptionPtr MoveZDown(struct _STester* me, int32_t timeout); 
static SElExceptionPtr OpenPince(struct _STester* me, int32_t timeout);
static SElExceptionPtr ClosePince(struct _STester* me, int32_t timeout);
static SElExceptionPtr LockPanelStation80(struct _STester* me, int32_t timeout); 
static SElExceptionPtr UnlockPanelStation80(struct _STester* me, int32_t timeout); 

char				TicketDirPath[1024];
char                Production_Serial[4+1];
char				Time[32];
char				Date[32];
char				TicketFilePath[1024]; /* output parameter */


//void CyclePrintStatus(void);  
void CyclePrintStatus(bool_t is_barcode_front,bool_t is_barcode_rear,bool_t is_cylinder_x_rear,bool_t is_cylinder_x_front, int32_t microCam);
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

	(*pTester)->Initialize = Initialize;
	(*pTester)->SetSensor = SetSensor;
	(*pTester)->GetSensor = GetSensor;
	(*pTester)->TableRotate = TableRotate;
	(*pTester)->PickPlace = PickPlace;
	(*pTester)->ResetPickPlace = ResetPickPlace;
	(*pTester)->GetStationStatus = GetStationStatus;
	(*pTester)->IsProductFaultTook = IsProductFaultTook;
	(*pTester)->IsResetPickPlace = IsResetPickPlace;
	(*pTester)->Clipping_punzoni = Clipping_punzoni;
	(*pTester)->StopClipping_punzoni = StopClipping_punzoni;
	(*pTester)->Clipping_contrasti = Clipping_contrasti;
	(*pTester)->StopClipping_contrasti = StopClipping_contrasti;
	(*pTester)->CheckClipping = CheckClipping;
	(*pTester)->StopCheckClipping = StopCheckClipping;
	(*pTester)->SelectRodageX52= SelectRodageX52;
	(*pTester)->SelectRodageVS11 = SelectRodageVS11;
	(*pTester)->Rodage = Rodage;
	(*pTester)->LockPanelStation10 = LockPanelStation10;
	(*pTester)->UnlockPanelStation10 = UnlockPanelStation10;
	(*pTester)->SelectVS11Station40 = SelectVS11Station40;
	(*pTester)->SelectX52Station40 = SelectX52Station40;
	(*pTester)->PlugPanelStation40_X52 = PlugPanelStation40_X52;
	(*pTester)->UnplugPanelStation40_X52 = UnplugPanelStation40_X52;
	(*pTester)->PlugPanelStation40_VS11 = PlugPanelStation40_VS11;
	(*pTester)->UnplugPanelStation40_VS11 = UnplugPanelStation40_VS11;
	(*pTester)->SelectVS11Station50 = SelectVS11Station50;
	(*pTester)->SelectX52Station50 = SelectX52Station50;
	(*pTester)->PlugPanelStation50_X52 = PlugPanelStation50_X52;
	(*pTester)->UnplugPanelStation50_X52 = UnplugPanelStation50_X52;
	(*pTester)->PlugPanelStation50_VS11 = PlugPanelStation50_VS11;
	(*pTester)->UnplugPanelStation50_VS11 = UnplugPanelStation50_VS11;
	(*pTester)->SelectVS11Station60 = SelectVS11Station60;
	(*pTester)->SelectX52Station60 = SelectX52Station60;
	(*pTester)->PlugPanelStation60_X52 = PlugPanelStation60_X52;
	(*pTester)->UnplugPanelStation60_X52 = UnplugPanelStation60_X52;
	(*pTester)->PlugPanelStation60_VS11 = PlugPanelStation60_VS11;
	(*pTester)->UnplugPanelStation60_VS11 = UnplugPanelStation60_VS11;
	(*pTester)->IlluminationStation60 = IlluminationStation60;
	(*pTester)->TestChecker = TestChecker;
	(*pTester)->ReadyPrint = ReadyPrint;
	(*pTester)->CyclePrint = CyclePrint;
	(*pTester)->Activate_Air_Printer = Activate_Air_Printer;
	(*pTester)->Disactivate_Air_Printer = Disactivate_Air_Printer;
	(*pTester)->GetStatusPrinter = GetStatusPrinter;
	(*pTester)->GetStatusRedBox = GetStatusRedBox;
	(*pTester)->DetectProductPresence = DetectProductPresence; 
	(*pTester)->InitializeStations = InitializeStations;
	(*pTester)->ResetStations = ResetStations;
	(*pTester)->WaitRotationFinished = WaitRotationFinished;
	(*pTester)->GetItemCode = GetItemCode;
	(*pTester)->OkNokCheck = OkNokCheck;
	(*pTester)->NokCheck = NokCheck;
	(*pTester)->TesterBeep = TesterBeep;
	(*pTester)->RegisterSensorCallbacks = RegisterSensorCallbacks;
	(*pTester)->SetSensorCallback = SetSensorCallback;
	
    (*pTester)->LockPanelStation80 = LockPanelStation80;
	(*pTester)->UnlockPanelStation80 = UnlockPanelStation80;
	(*pTester)->MoveXFront = MoveXFront; 
	(*pTester)->MoveXRear = MoveXRear;  
	(*pTester)->MoveZUp = MoveZUp;  
	(*pTester)->MoveZDown = MoveZDown;  
	(*pTester)->OpenPince = OpenPince;  
	(*pTester)->ClosePince = ClosePince;  

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
	EXCCHECK( me->SetSensorCallback(me, "BUTTON_NOK", CallbackFncButtonNok, me));
	EXCCHECK( me->SetSensorCallback(me, "BUTTON_MENU", CallbackFncButtonMenu, me));
	//EXCCHECK( me->SetSensorCallback(me, "BUTTON_LABEL", CallbackFncButtonLabel, me));
	//EXCCHECK( me->SetSensorCallback(me, "BUTTON_DOWNTIME", CallbackFncButtonDowntime, me));
	EXCCHECK( me->SetSensorCallback(me, "SELECTEUR_ABRIS", CallbackFncAbis, me));
	EXCCHECK( me->SetSensorCallback(me, "BUTTON_START", CallbackFncButtonStart, me));
	EXCCHECK( me->SetSensorCallback(me, "MICRO_CAM", CallbackFncRotation, me));
	EXCCHECK( me->SetSensorCallback(me, "BUTTON_TOTAL_STOP", CallbackFncButtonTotalStop, me));
	EXCCHECK( me->SetSensorCallback(me, "SENSOR_RED_BOX_FULL", CallbackFncRedBoxEmpty, me));
	EXCCHECK( me->SetSensorCallback(me, "SENSOR_RED_BOX_REAR", CallbackFncRedBoxClosed, me));
	EXCCHECK( me->SetSensorCallback(me, "IS_PRINT_CYCLE_ERROR", CallbackFncPrintReady, me));
	EXCCHECK( me->SetSensorCallback(me, "IS_PRINTER_READY", CallbackFncPrintFinished, me));


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


SElExceptionPtr checkAllAxis (struct _STester* me)
{
SElExceptionPtr 	pexception = NULL; 
int statusX;
int statusY;
int statusZ;

me->paxis->ControllerStatus(me->paxis, AXIS_X, &statusX);
me->paxis->ControllerStatus(me->paxis, AXIS_X, &statusX);
is_axis_X_In_Home_40 = (statusX >> 4) & 1;	
is_axis_X_In_position_40 = (statusX >> 3) & 1;	
me->paxis->ControllerStatus(me->paxis, AXIS_Y, &statusY);
me->paxis->ControllerStatus(me->paxis, AXIS_Y, &statusY);
is_axis_Y_In_Home_40 = (statusY >> 4) & 1;	
is_axis_Y_In_position_40 = (statusY >> 3) & 1;
me->paxis->ControllerStatus(me->paxis, AXIS_Z, &statusZ);
me->paxis->ControllerStatus(me->paxis, AXIS_Z, &statusZ);
is_axis_Z_In_Home_40 = (statusZ >> 4) & 1;	
is_axis_Z_In_position_40 = (statusZ >> 3) & 1;

	
Error:
	return pexception;	
}




/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::TableRotate"
static SElExceptionPtr TableRotate(struct _STester* me, int32_t timeout, bool_t wait_end)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		step = 0;
	
	bool_t			status_micro_cam = FALSE,
					status_barrier = FALSE,
					is_table_rotation_finished = FALSE;
	

	uint32_t		status_potentiometer_inter_on,
					status_potentiometer_inter_off,
					status_potentiometer_exter_on,
					status_potentiometer_exter_off,
					status_clippage_punzoni_on,
					status_clippage_punzoni_off,
					status_clippage_contrasti_on,
					status_clippage_contrasti_off,
					status_cylinder_rodage_up,
					status_cylinder_rodage_down,
					status_cylinder_rodage_push_1_up,
					status_cylinder_rodage_push_1_down,
					status_cylinder_rodage_push_2_up,
					status_cylinder_rodage_push_2_down,
					status_cylinder_rodage_push_3_up,
					status_cylinder_rodage_push_3_down,
					status_cylinder_rodage_push_4_up,
					status_cylinder_rodage_push_4_down,
					status_cylinder_rodage_push_5_up,
					status_cylinder_rodage_push_5_down,
					status_cylinder_rodage_push_6_up,
					status_cylinder_rodage_push_6_down,
					status_cylinder_rodage_push_7_up,
					status_cylinder_rodage_push_7_down,
					status_cylinder_rodage_push_8_up,
					status_cylinder_rodage_push_8_down,
					status_cylinder_rodage_push_9_up,
					status_cylinder_rodage_push_9_down,
					is_panel_locked_10,
					is_panel_unlocked_10,
					is_panel_locked_40,
					is_panel_unlocked_40,
					is_panel_plugged_40_x52,
					is_panel_unplugged_40_x52,
					is_panel_plugged_40_vs11,
					is_panel_unplugged_40_vs11,
					is_axis_Z_home_40,
					is_panel_locked_50,
					is_panel_unlocked_50,
					is_panel_plugged_50_x52,
					is_panel_unplugged_50_x52,
					is_panel_plugged_50_vs11,
					is_panel_unplugged_50_vs11,
					is_axis_Z_home_50,
					is_panel_locked_60,
					is_panel_unlocked_60,
					is_panel_plugged_60_x52,
					is_panel_unplugged_60_x52,
					is_panel_plugged_60_vs11,
					is_panel_unplugged_60_vs11,
					is_cylinder_x_front,
					is_cylinder_x_rear,
					is_cylinder_z_up,
					is_cylinder_z_down,
					is_pince_open,
					is_pince_close,
					is_panel_locked_80,
					is_panel_unlocked_80,
					is_cylinder_printer_rear,
					is_cylinder_printer_up,
					is_sensor_cylinder_printer_rear;

	bool_t			is_station_10_ready = FALSE,
					is_station_20_ready = FALSE,
					is_station_30_ready = FALSE,
					is_station_40_ready = FALSE,
					is_station_50_ready = FALSE,
					is_station_60_ready = FALSE,
					is_station_70_ready = FALSE,
					is_station_80_ready = FALSE;
	
	 		CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
			/* STATION 10 */
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_STATION_10", &is_panel_locked_10));
			EXCCHECK( me->GetSensor(me, "SENSOR_UNLOCK_STATION_10", &is_panel_unlocked_10));
			
			
			if(is_panel_locked_10 && !is_panel_unlocked_10)
			{
				is_station_10_ready = TRUE;
			}
			else						 
				is_station_10_ready = FALSE;

			/* STATION 20 */
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_ON", &status_potentiometer_inter_on));
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_OFF", &status_potentiometer_inter_off));
			
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_ON", &status_potentiometer_exter_on));
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_OFF", &status_potentiometer_exter_off));
			
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_ON", &status_clippage_punzoni_on));
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_OFF", &status_clippage_punzoni_off));
			
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_ON", &status_clippage_contrasti_on));
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_OFF", &status_clippage_contrasti_off));
			
			if(status_potentiometer_inter_off && !status_potentiometer_inter_on &&
			   status_potentiometer_exter_off && !status_potentiometer_exter_on &&
			   status_clippage_punzoni_off    && !status_clippage_punzoni_on    &&
			   status_clippage_contrasti_off  && !status_clippage_contrasti_on)
			{
				is_station_20_ready = TRUE;
			}
			else						 
				is_station_20_ready = FALSE;


			/* STATION 30 */
		//	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_UP", &status_cylinder_rodage_up));
		//	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_DOWN", &status_cylinder_rodage_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_UP", &status_cylinder_rodage_push_1_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_DOWN", &status_cylinder_rodage_push_1_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_UP", &status_cylinder_rodage_push_2_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_DOWN", &status_cylinder_rodage_push_2_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_UP", &status_cylinder_rodage_push_3_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_DOWN", &status_cylinder_rodage_push_3_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_UP", &status_cylinder_rodage_push_4_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_DOWN", &status_cylinder_rodage_push_4_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_UP", &status_cylinder_rodage_push_5_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_DOWN", &status_cylinder_rodage_push_5_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_UP", &status_cylinder_rodage_push_6_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_DOWN", &status_cylinder_rodage_push_6_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_UP", &status_cylinder_rodage_push_7_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_DOWN", &status_cylinder_rodage_push_7_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_UP", &status_cylinder_rodage_push_8_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_DOWN", &status_cylinder_rodage_push_8_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_UP", &status_cylinder_rodage_push_9_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_DOWN", &status_cylinder_rodage_push_9_down));

			
			if(//status_cylinder_rodage_up && !status_cylinder_rodage_down && 
			   status_cylinder_rodage_push_1_up && !status_cylinder_rodage_push_1_down &&
			  /* status_cylinder_rodage_push_2_up && */!status_cylinder_rodage_push_2_down &&
			   status_cylinder_rodage_push_3_up && !status_cylinder_rodage_push_3_down &&
			   status_cylinder_rodage_push_4_up && !status_cylinder_rodage_push_4_down &&
			   status_cylinder_rodage_push_5_up && !status_cylinder_rodage_push_5_down &&
			   status_cylinder_rodage_push_6_up && !status_cylinder_rodage_push_6_down &&
			   status_cylinder_rodage_push_7_up && !status_cylinder_rodage_push_7_down &&
			   status_cylinder_rodage_push_8_up && !status_cylinder_rodage_push_8_down &&
			   status_cylinder_rodage_push_9_up && !status_cylinder_rodage_push_9_down)
			{
				is_station_30_ready = TRUE;
			}
			else 
				is_station_30_ready = FALSE;

			
			/* STATION 40 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_X52", &is_panel_plugged_40_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_X52", &is_panel_unplugged_40_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_VS11", &is_panel_plugged_40_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_VS11", &is_panel_unplugged_40_vs11));
			EXCCHECK( me->GetSensor(me, "AXIS_Z_HOME_STATION_40", &is_axis_Z_home_40));

			if(is_panel_unplugged_40_x52 && !is_panel_plugged_40_x52 && is_panel_unplugged_40_vs11 && !is_panel_plugged_40_vs11 && is_axis_Z_home_40)
			{
				is_station_40_ready = TRUE;
			}
			else 
				is_station_40_ready = FALSE;
		
		   	/* STATION 50 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_X52", &is_panel_plugged_50_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_X52", &is_panel_unplugged_50_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_VS11", &is_panel_plugged_50_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_VS11", &is_panel_unplugged_50_vs11));
			EXCCHECK( me->GetSensor(me, "AXIS_Z_HOME_STATION_50", &is_axis_Z_home_50));

			if(is_panel_unplugged_50_x52 && !is_panel_plugged_50_x52 && is_panel_unplugged_50_vs11 && !is_panel_plugged_50_vs11 && is_axis_Z_home_50)
			{
				is_station_50_ready = TRUE;
			}
			else 
				is_station_50_ready = FALSE;
		
		   	/* STATION 60 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_X52", &is_panel_plugged_60_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_X52", &is_panel_unplugged_60_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_VS11", &is_panel_plugged_60_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_VS11", &is_panel_unplugged_60_vs11));

			if(is_panel_unplugged_60_x52 && !is_panel_plugged_60_x52 && is_panel_unplugged_60_vs11 && !is_panel_plugged_60_vs11)
			{
				is_station_60_ready = TRUE;
			}
			else 
				is_station_60_ready = FALSE;
			
			
			/* STATION 70 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_PRINTER_REAR", &is_cylinder_printer_rear));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_PRINTER_REAR", &is_sensor_cylinder_printer_rear));

			if(is_cylinder_printer_rear && is_sensor_cylinder_printer_rear)
			{
				is_station_70_ready = TRUE;
			}
	    	else 
				is_station_70_ready = FALSE;
			
			
			/* STATION 80 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &is_panel_locked_80)); 
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &is_panel_unlocked_80)); 
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &is_cylinder_x_front));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &is_cylinder_x_rear));
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &is_cylinder_z_up));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &is_cylinder_z_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_OPEN_STATION_80", &is_pince_open));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_CLOSE_STATION_80", &is_pince_close));

			if(is_panel_locked_80 && !is_panel_unlocked_80 && is_cylinder_x_rear && !is_cylinder_x_front && is_cylinder_z_up && !is_cylinder_z_down && is_pince_open && !is_pince_close)
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
//	printf("Start Rotation\n\r");
	
	while( TRUE)
	{
	  me->GetSensor(me, "MICRO_CAM", &status_micro_cam);
	  if(status_micro_cam == 0)// rotation  
	  {
		  
	/* STATION 10 */
	  me->SetSensor(me, "CYLINDER_LOCK_STATION_10", 1);
	  me->SetSensor(me, "CYLINDER_UNLOCK_STATION_10",0);

	  //STATION 20
	  me->SetSensor(me, "LOCK_PUNZONI_OFF", 1);
	  me->SetSensor(me, "LOCK_PUNZONI_ON", 0);
	  me->SetSensor(me, "LOCK_CONTRASTI_OFF", 1);
	  me->SetSensor(me, "LOCK_CONTRASTI_ON", 0);
	  me->SetSensor(me, "POTENTIOMETER_OFF", 1);
	  me->SetSensor(me, "POTENTIOMETER_ON", 0);
		
	 //STATION 30
	//  EXCCHECK( me->SetSensor(me, "RODAGE_UP", 1));
	 // EXCCHECK( me->SetSensor(me, "RODAGE_DOWN", 0));
	  
	 //STATION 40
	  me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40_X52", 1);
	  me->SetSensor(me, "CYLINDER_PLUG_STATION_40_X52", 0);
	  me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40_VS11", 1);
	  me->SetSensor(me, "CYLINDER_PLUG_STATION_40_VS11", 0);
	  
	 //STATION 50
	  me->SetSensor(me, "CYLINDER_UNPLUG_STATION_50_X52", 1);
	  me->SetSensor(me, "CYLINDER_PLUG_STATION_50_X52", 0);
	  me->SetSensor(me, "CYLINDER_UNPLUG_STATION_50_VS11", 1);
	  me->SetSensor(me, "CYLINDER_PLUG_STATION_50_VS11", 0);
	  
	 //STATION 60
	  me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60_X52", 1);
	  me->SetSensor(me, "CYLINDER_PLUG_STATION_60_X52", 0);
	  me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60_VS11", 1);
	  me->SetSensor(me, "CYLINDER_PLUG_STATION_60_VS11", 0);
	  
	  //STATION_70 
	  //me->SetSensor(me, "ACTIVATE_AIR_PRINTER", 0);
	  
	  //STATION 80
	  me->SetSensor(me, "CYLINDER_X_FRONT_STATION_80", 0);
	  me->SetSensor(me, "CYLINDER_X_REAR_STATION_80", 1);
	  me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1);
	  me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0);
	  me->SetSensor(me, "OPEN_PINCE_STATION_80", 1);
	  me->SetSensor(me, "CLOSE_PINCE_STATION_80", 0);
	  me->SetSensor(me, "CYLINDER_LOCK_STATION_80", 1);
	  me->SetSensor(me, "CYLINDER_UNLOCK_STATION_80", 0);
	  
	  }	
		
	  me->GetSensor(me, "INFRARED_BARRIER", &status_barrier);
	  if (status_barrier == 0)
	  {
	  Sleep(300);
	  me->GetSensor(me, "MICRO_CAM", &status_micro_cam);
	  if(status_micro_cam == 0)// in not indexed poition
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	  }
		
		/* si aprés passage barriére un véin sorte il doit vérifier un autre fois is_table_ready should be = 0 */
		if (!is_table_rotation_finished)
		{
			switch  (step)
			{
			 case  CHECK_TABLE_POITION : /* check the table in wich position and start rotation */
				   		
				   EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
				   
						if(status_micro_cam == 1)// in indexed poition 
							{
				 				EXCCHECK( me->SetSensor(me, "START_ROTATION", 1));
								EXCCHECK( me->SetSensor(me, "FORCAGE_ROTATION", 1));
								step ++;
							//	printf("Rotation Done\n\r");
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
						EXCCHECK( me->SetSensor(me, "FORCAGE_ROTATION", 0));
						step ++;
					}

			break;
			
			case  CHECK_IF_ROTATION_FINIHED :
				   		
				   EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
						if(status_micro_cam == 1)// rotation finished 
							{
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
	
	uint32_t		status_cylinder_presse_up,
					status_cylinder_presse_down,
					status_cylinder_gefran_up,
					status_cylinder_gefran_down,
					status_cylinder_rodage_up,
					status_cylinder_rodage_down,
					status_cylinder_rodage_push_1_up,
					status_cylinder_rodage_push_1_down,
					status_cylinder_rodage_push_2_up,
					status_cylinder_rodage_push_2_down,
					status_cylinder_rodage_push_3_up,
					status_cylinder_rodage_push_3_down,
					status_cylinder_rodage_push_4_up,
					status_cylinder_rodage_push_4_down,
					status_cylinder_rodage_push_5_up,
					status_cylinder_rodage_push_5_down,
					status_cylinder_rodage_push_6_up,
					status_cylinder_rodage_push_6_down,
					status_cylinder_rodage_push_7_up,
					status_cylinder_rodage_push_7_down,
					status_cylinder_rodage_push_8_up,
					status_cylinder_rodage_push_8_down,
					status_cylinder_rodage_push_9_up,
					status_cylinder_rodage_push_9_down,
					is_cylinder_x_front,
					is_cylinder_x_rear,
					is_cylinder_printer_rear,
					cycle_print_error,
					is_panel_locked_40,
					is_panel_unlocked_40,
					is_panel_plugged_40_x52,
					is_panel_unplugged_40_x52,
					is_panel_plugged_40_vs11,
					is_panel_unplugged_40_vs11,
					is_axis_Z_home_40,
					is_panel_locked_50,
					is_panel_unlocked_50,
					is_panel_plugged_50_x52,
					is_panel_unplugged_50_x52,
					is_panel_plugged_50_vs11,
					is_panel_unplugged_50_vs11,
					is_axis_Z_home_50,
					is_panel_locked_60,
					is_panel_unlocked_60,
					is_panel_plugged_60_x52,
					is_panel_unplugged_60_x52,
					is_panel_plugged_60_vs11,
					is_panel_unplugged_60_vs11,
					status_cylinder_lock_punzoni,
					status_cylinder_unlock_punzoni,
					status_cylinder_lock_contrasti,
					status_cylinder_unlock_contrasti,
					status_gefran_front_int,
					status_gefran_rear_int,
					status_gefran_front_ext,
					status_gefran_rear_ext,
					is_panel_locked_10,
					is_panel_unlocked_10,
					is_cylinder_z_up,
					is_cylinder_z_down,
					is_pince_open,
					is_pince_close,
					is_panel_locked_80,
					is_panel_unlocked_80,
					is_cylinder_printer_up,
					is_sensor_cylinder_printer_rear;

	bool_t			is_station_10_ready = FALSE,
					is_station_20_ready = FALSE,
					is_station_30_ready = FALSE,
					is_station_40_ready = FALSE,
					is_station_50_ready = FALSE,
					is_station_60_ready = FALSE,
					is_station_70_ready = FALSE,
					is_station_80_ready = FALSE;
	
	
		/* STATION 10 */
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_STATION_10", &is_panel_locked_10));
			EXCCHECK( me->GetSensor(me, "SENSOR_UNLOCK_STATION_10", &is_panel_unlocked_10));
			
			
			if(is_panel_locked_10 && !is_panel_unlocked_10)
			{
				is_station_10_ready = TRUE;
			}
			else						 
				is_station_10_ready = FALSE;
			
		//	printf(" is_panel_locked_10 = %d \n\r", is_panel_locked_10);
		//	printf(" is_panel_unlocked_10 = %d \n\r", is_panel_unlocked_10);
		//	printf(" is_station_10_ready = %d \n\r", is_station_10_ready);
	
		  	/* STATION 20 */
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_ON", &status_cylinder_lock_punzoni));
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_OFF", &status_cylinder_unlock_punzoni));
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_ON", &status_cylinder_lock_contrasti));
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_OFF", &status_cylinder_unlock_contrasti));
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_ON", &status_gefran_front_int)); 
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_OFF", &status_gefran_rear_int)); 
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_ON", &status_gefran_front_ext)); 
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_OFF", &status_gefran_rear_ext)); 
			
			if(!status_cylinder_lock_punzoni && status_cylinder_unlock_punzoni &&
			   !status_cylinder_lock_contrasti && status_cylinder_unlock_contrasti &&
			   !status_gefran_front_int && status_gefran_rear_int && 
			   !status_gefran_front_ext && status_gefran_rear_ext)
			{
				is_station_20_ready = TRUE;
			}
			else						 
				is_station_20_ready = FALSE;
			


			/* STATION 30 */
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_X52", &status_cylinder_rodage_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_VS11", &status_cylinder_rodage_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_UP", &status_cylinder_rodage_push_1_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_DOWN", &status_cylinder_rodage_push_1_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_UP", &status_cylinder_rodage_push_2_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_DOWN", &status_cylinder_rodage_push_2_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_UP", &status_cylinder_rodage_push_3_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_DOWN", &status_cylinder_rodage_push_3_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_UP", &status_cylinder_rodage_push_4_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_DOWN", &status_cylinder_rodage_push_4_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_UP", &status_cylinder_rodage_push_5_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_DOWN", &status_cylinder_rodage_push_5_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_UP", &status_cylinder_rodage_push_6_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_DOWN", &status_cylinder_rodage_push_6_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_UP", &status_cylinder_rodage_push_7_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_DOWN", &status_cylinder_rodage_push_7_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_UP", &status_cylinder_rodage_push_8_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_DOWN", &status_cylinder_rodage_push_8_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_UP", &status_cylinder_rodage_push_9_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_DOWN", &status_cylinder_rodage_push_9_down));

			
			if( 
			   status_cylinder_rodage_push_1_up && !status_cylinder_rodage_push_1_down &&
			  /* status_cylinder_rodage_push_2_up &&*/ !status_cylinder_rodage_push_2_down &&
			   status_cylinder_rodage_push_3_up && !status_cylinder_rodage_push_3_down &&
			   status_cylinder_rodage_push_4_up && !status_cylinder_rodage_push_4_down &&
			   status_cylinder_rodage_push_5_up && !status_cylinder_rodage_push_5_down &&
			   status_cylinder_rodage_push_6_up && !status_cylinder_rodage_push_6_down &&
			   status_cylinder_rodage_push_7_up && !status_cylinder_rodage_push_7_down &&
			   status_cylinder_rodage_push_8_up && !status_cylinder_rodage_push_8_down &&
			   status_cylinder_rodage_push_9_up && !status_cylinder_rodage_push_9_down)
			{
				is_station_30_ready = TRUE;
			}
			else 
				is_station_30_ready = FALSE;
			

			
			/* STATION 40 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_X52", &is_panel_plugged_40_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_X52", &is_panel_unplugged_40_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_VS11", &is_panel_plugged_40_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_VS11", &is_panel_unplugged_40_vs11));
			EXCCHECK( me->GetSensor(me, "AXIS_Z_HOME_STATION_40", &is_axis_Z_home_40));

			if(is_panel_unplugged_40_x52 && !is_panel_plugged_40_x52 && is_panel_unplugged_40_vs11 && !is_panel_plugged_40_vs11 && is_axis_Z_home_40)
			{
				is_station_40_ready = TRUE;
			}
			else 
				is_station_40_ready = FALSE;
			
		
		   	/* STATION 50 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_X52", &is_panel_plugged_50_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_X52", &is_panel_unplugged_50_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_VS11", &is_panel_plugged_50_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_VS11", &is_panel_unplugged_50_vs11));
			EXCCHECK( me->GetSensor(me, "AXIS_Z_HOME_STATION_50", &is_axis_Z_home_50));

			if(is_panel_unplugged_50_x52 && !is_panel_plugged_50_x52 && is_panel_unplugged_50_vs11 && !is_panel_plugged_50_vs11 && is_axis_Z_home_50)
			{
				is_station_50_ready = TRUE;
			}
			else 
				is_station_50_ready = FALSE;
			
		
		   	/* STATION 60 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_X52", &is_panel_plugged_60_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_X52", &is_panel_unplugged_60_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_VS11", &is_panel_plugged_60_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_VS11", &is_panel_unplugged_60_vs11));

			if(is_panel_unplugged_60_x52 && !is_panel_plugged_60_x52 && is_panel_unplugged_60_vs11 && !is_panel_plugged_60_vs11)
			{
				is_station_60_ready = TRUE;
			}
			else 
				is_station_60_ready = FALSE;
			
			
			
			/* STATION 70 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_PRINTER_REAR", &is_cylinder_printer_rear));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_PRINTER_REAR", &is_sensor_cylinder_printer_rear));

			if(is_cylinder_printer_rear && is_sensor_cylinder_printer_rear)
			{
				is_station_70_ready = TRUE;
			}
	    	else 
				is_station_70_ready = FALSE;
			
			/* STATION 80 */
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &is_cylinder_x_front));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &is_cylinder_x_rear));
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &is_cylinder_z_up));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &is_cylinder_z_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_OPEN_STATION_80", &is_pince_open));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_CLOSE_STATION_80", &is_pince_close));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &is_panel_locked_80));
	  		EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &is_panel_unlocked_80));
			

			if(is_panel_locked_80 && !is_panel_unlocked_80 && is_cylinder_x_rear && !is_cylinder_x_front && is_cylinder_z_up && !is_cylinder_z_down && is_pince_open && !is_pince_close)
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

//	 printf(" Table_Ready[0] = %s \n\r", Table_Ready[0]); 
		
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

	TIMEOUT_INIT();
	
	if(ok) *ok = FALSE;

//	printf("OkNokCheck\r\n");
	EXCCHECK( me->SetSensor(me, "INDICATOR_OK", 1));
	EXCCHECK( me->SetSensor(me, "INDICATOR_NOK", 1));

	while(TRUE)
	{
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
		Sleep(100);
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
#define __FUNC__	"Tester::TesterBeep"
static SElExceptionPtr TesterBeep(struct _STester* me, int32_t time)
{
	SElExceptionPtr		pexception = NULL;
	SBeepThreadDataPtr	pthread_data = NULL;
		
	pthread_data = (SBeepThreadDataPtr)calloc(1, sizeof(SBeepThreadData)); 
	EXCCHECKALLOC( pthread_data);

	pthread_data->me = me;
	pthread_data->time = time;
	
	EXCCHECK( ThreadStart(BeepThreadFunc, pthread_data, &me->hThreadBeep, NULL));

				
Error:
	EXCRETHROW(pexception);
}

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
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncAbis"
static SElExceptionPtr CallbackFncAbis(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	char                TableStatus[9+1];
	bool_t				is_table_ready = FALSE;
	bool_t				is_station_10_ready = FALSE,
	                	is_station_20_ready = FALSE,
						is_station_30_ready = FALSE,
						is_station_40_ready = FALSE,
						is_station_50_ready = FALSE,
						is_station_60_ready = FALSE,
						is_station_80_ready = FALSE;

					
	if( state==1 && me->eventAPI)
	{
#if 0    /* formerly excluded lines */
		do{
			pexception = me->GetStationStatus(me, TableStatus);
			is_station_10_ready = TableStatus[1];
			is_station_20_ready = TableStatus[2];
			is_station_30_ready = TableStatus[3];
			is_station_40_ready = TableStatus[4];
			is_station_50_ready = TableStatus[5];
			is_station_60_ready = TableStatus[6];
			is_station_80_ready = TableStatus[7];

			if(is_station_10_ready &&
			   is_station_20_ready &&
			   is_station_30_ready &&
	  		   is_station_40_ready &&
	  		   is_station_50_ready &&
	   		   is_station_60_ready)
				{
				is_table_ready = TRUE;
				}	
			
		}while (is_table_ready == FALSE);
#endif   /* formerly excluded lines */
		
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_DROITE", 1));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_GAUCHE", 1));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_ARRIERE", 1));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_AVANT", 1));
	}
	else if( state==0 && me->eventAPI)
	{
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_DROITE", 0));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_GAUCHE", 0));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_ARRIERE", 0));
		EXCCHECK( me->SetSensor(me, "VEROUILLAGE_ABIS_AVANT", 0));
	}

Error:
	EXCRETHROW(pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CallbackFncButtonStart"
static SElExceptionPtr CallbackFncButtonStart(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	bool_t          status_micro_cam, status_barrier;
	
	if( state==1 && me->eventAPI)
	{
		
	/* check where is the table */
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &status_micro_cam));
	EXCCHECK( me->GetSensor(me, "INFRARED_BARRIER", &status_barrier));

	if (status_micro_cam == 0)
	{
		me->TableRotate(me, 5000,TRUE);
	}
	
	if (status_barrier == 0)
		Sleep(400);
	
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
#define __FUNC__	"CallbackFncaxis_x_station_40"
static SElExceptionPtr CallbackFncaxis_x_station_40(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_X_START_STATION_40, NULL));
	if( state==1 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_X_STOP_STATION_40, NULL));
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"CallbackFncaxis_y_station_40"
static SElExceptionPtr CallbackFncaxis_y_station_40(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Y_START_STATION_40, NULL));
	if( state==1 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Y_STOP_STATION_40, NULL));
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"CallbackFncaxis_z_station_40"
static SElExceptionPtr CallbackFncaxis_z_station_40(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Z_START_STATION_40, NULL));
	if( state==1 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Z_STOP_STATION_40, NULL));
	
Error:
	EXCRETHROW(pexception);
}
#undef __FUNC__
#define __FUNC__	"CallbackFncaxis_x_station_50"
static SElExceptionPtr CallbackFncaxis_x_station_50(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_X_START_STATION_50, NULL));
	if( state==1 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_X_STOP_STATION_50, NULL));
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"CallbackFncaxis_y_station_50"
static SElExceptionPtr CallbackFncaxis_y_station_50(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Y_START_STATION_50, NULL));
	if( state==1 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Y_STOP_STATION_50, NULL));
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"CallbackFncaxis_z_station_50"
static SElExceptionPtr CallbackFncaxis_z_station_50(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Z_START_STATION_50, NULL));
	if( state==1 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Z_STOP_STATION_50, NULL));
	
Error:
	EXCRETHROW(pexception);
}
#undef __FUNC__
#define __FUNC__	"CallbackFncRedBoxClosed"
static SElExceptionPtr CallbackFncRedBoxClosed(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_RED_BOX_CLOSED, NULL));
	
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
#define __FUNC__	"CallbackFncPrintReady"
static SElExceptionPtr CallbackFncPrintReady(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==0 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_PRINTER_READY, NULL));
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"CallbackFncPrintFinished"
static SElExceptionPtr CallbackFncPrintFinished(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	STesterPtr		me = (STesterPtr)pData;
	
	if( state==1 && me->eventAPI)
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_CYCLE_PRINT_FINISHED, NULL));
	
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

	if( state == 1 && me->eventAPI)  // state==1 : TABLE IN POSITION
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
	int32_t			insert1 = 0,
					insert2 = 0,
					insert3 = 0,
					insert4 = 0,
					insert5 = 0,
					insert6 = 0,
					gs_presse_up_punzoni = 0,
					gs_presse_down_punzoni = 0,
					gs_presse_up_contrasti = 0,
					gs_presse_down_contrasti = 0,
					gs_gefran_up_int = 0,
					gs_gefran_down_int = 0,
					gs_gefran_up_ext = 0,
					gs_gefran_down_ext = 0,
					gs_rodage_up = 0,
					gs_rodage_down = 0,
					gs_lock_station_10= 0,
					gs_unlock_station_10= 0,
					gs_select_vs11_station_40= 0,
					gs_select_x52_station_40= 0,
					gs_plug_station_40_x52= 0,
					gs_unplug_station_40_x52= 0,
					gs_plug_station_40_vs11= 0,
					gs_unplug_station_40_vs11= 0,
					gs_select_vs11_station_50= 0,
					gs_select_x52_station_50= 0,
					gs_plug_station_50_x52= 0,
					gs_unplug_station_50_x52= 0,
					gs_plug_station_50_vs11= 0,
					gs_unplug_station_50_vs11= 0,
					gs_select_vs11_station_60= 0,
					gs_select_x52_station_60= 0,
					gs_plug_station_60_x52= 0,
					gs_unplug_station_60_x52= 0,
					gs_plug_station_60_vs11= 0,
					gs_unplug_station_60_vs11= 0,
					gs_barcode_front= 0,
					gs_plug_station_40_VS11= 0,
					gs_unplug_station_40_VS11= 0,
					gs_lock_station_80= 0,
					gs_unlock_station_80= 0,
					gs_barcode_rear= 0;
	
	int32_t			microCam;

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));

	/* Check if pannel is inserted */
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_GO_LEFT", &insert1));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_GO_RIGHT", &insert2));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_LO_LEFT", &insert3));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_LO_RIGHT", &insert4));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_HI_LEFT", &insert5));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_HI_RIGHT", &insert6));
	if (insert1 && insert2 && microCam)
	{
		if (me->eventAPI && !gs_inserted)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_INSERTED, NULL));
			//printf ("EVNT_JIG_ITEM_INSERTED \r\n");
		}
		gs_inserted = 1;
	}
	else
	{
		gs_inserted = 0;
	}
	/* Check if pannel is released */
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_GO_LEFT", &insert1));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_GO_RIGHT", &insert2));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_LO_LEFT", &insert3));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_LO_RIGHT", &insert4));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_HI_LEFT", &insert5));
	EXCCHECK( me->GetSensor(me, "OUTPUT_LASER_HI_RIGHT", &insert6));
	if ((!insert1 || !insert2) && microCam)
	{
		if (me->eventAPI && !gs_released)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_RELEASED, NULL));
			//printf ("EVNT_JIG_ITEM_RELEASED \r\n");
		}

		gs_released = 1;
	}
	else
	{
		gs_released = 0;
	}
	
	/* Check if panel clipped punzoni*/
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_ON", &gs_presse_up_punzoni));
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_OFF", &gs_presse_down_punzoni));
	if (gs_presse_up_punzoni && !gs_presse_down_punzoni)
	{
		if (me->eventAPI && !gs_clipped_punzoni)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_CLIPPED_PUNZONI, NULL));
			//printf ("EVNT_JIG_ITEM_CLIPPED \r\n");
		}

		
		gs_clipped_punzoni = 1;
	}
	else
	{
		gs_clipped_punzoni = 0;
	}
	/* Check if panel stop clipped punzoni*/
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_ON", &gs_presse_up_punzoni));
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_OFF", &gs_presse_down_punzoni));
	if (gs_presse_down_punzoni && !gs_presse_up_punzoni)
	{
		if (me->eventAPI && !gs_stop_clipped_punzoni)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_CLIPPED_PUNZONI, NULL));
			//printf ("EVNT_JIG_ITEM_STOP_CLIPPED \r\n");
		}

		gs_stop_clipped_punzoni = 1;
	}
	else
	{
		gs_stop_clipped_punzoni = 0;
	}
	
	
		
	/* Check if panel clipped contrasti*/
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_ON", &gs_presse_up_contrasti));
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_OFF", &gs_presse_down_contrasti));
	if (gs_presse_up_contrasti && !gs_presse_down_contrasti)
	{
		if (me->eventAPI && !gs_clipped_contrasti)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_CLIPPED_CONTRASTI, NULL));
			//printf ("EVNT_JIG_ITEM_CLIPPED \r\n");
		}

		
		gs_clipped_contrasti = 1;
	}
	else
	{
		gs_clipped_contrasti = 0;
	}
	/* Check if panel stop clipped contrasti*/
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_ON", &gs_presse_up_contrasti));
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_OFF", &gs_presse_down_contrasti));
	if (gs_presse_down_contrasti && !gs_presse_up_contrasti)
	{
		if (me->eventAPI && !gs_stop_clipped_contrasti)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_CLIPPED_CONTRASTI, NULL));
			//printf ("EVNT_JIG_ITEM_STOP_CLIPPED \r\n");
		}

		gs_stop_clipped_contrasti = 1;
	}
	else
	{
		gs_stop_clipped_contrasti = 0;
	}
	
	
/* Check if panel checked */
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_ON", &gs_gefran_up_int));
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_OFF", &gs_gefran_down_int));
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_ON", &gs_gefran_up_ext));
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_OFF", &gs_gefran_down_ext));
	if (gs_gefran_up_int && gs_gefran_up_ext && !gs_gefran_down_int && !gs_gefran_down_ext)
	{
		if (me->eventAPI && !gs_checked)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_CHECKED, NULL));
			//printf ("EVNT_JIG_ITEM_CHECKED \r\n");
		}

		
		gs_checked = 1;
	}
	else
	{
		gs_checked = 0;
	}
	/* Check if panel stop checked */
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_ON", &gs_gefran_up_int));
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_OFF", &gs_gefran_down_int));
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_ON", &gs_gefran_up_ext));
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_OFF", &gs_gefran_down_ext));
	if (!gs_gefran_up_int && !gs_gefran_up_ext && gs_gefran_down_int && gs_gefran_down_ext) 
	{
	if (me->eventAPI && !gs_stop_checked)
	{
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_CHECKED, NULL));
		//printf ("EVNT_JIG_ITEM_CHECKED \r\n");
	}

		gs_stop_checked = 1;
	}
	else
	{
		gs_stop_checked = 0;
	}
	
#if 0    /* formerly excluded lines */
	/* Check if rodage started */
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_X52", &gs_rodage_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_VS11", &gs_rodage_down));
	if (!gs_rodage_up && gs_rodage_down)
	{
		if (me->eventAPI  && !gs_started_rodage)
		{
		EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_START_RODAGE, NULL));
		//printf ("EVNT_JIG_ITEM_START_RODAGE \r\n");
		}
		gs_started_rodage = 1;
	}
	else
	{
		gs_started_rodage = 0;
	}
//	printf("gs_started_rodage =%d\n\r", gs_started_rodage); 
	
	/* Check if rodage stopped */
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_X52", &gs_rodage_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_VS11", &gs_rodage_down));
	if (!gs_rodage_down && gs_rodage_up)
	{
		if (me->eventAPI /*&& !gs_stopped_rodage*/)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_RODAGE, NULL));
			//printf ("EVNT_JIG_ITEM_STOP_RODAGE \r\n");
		}

		
		gs_stopped_rodage = 1;
		
	}
	else
	{
		gs_stopped_rodage = 0;
	}
#endif   /* formerly excluded lines */
//	printf("gs_stopped_rodage =%d\n\r", gs_stopped_rodage);
	
		/* Check if panel locked station 10 */
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_STATION_10", &gs_lock_station_10));
	EXCCHECK( me->GetSensor(me, "SENSOR_UNLOCK_STATION_10", &gs_unlock_station_10));
	if (!gs_unlock_station_10 && gs_lock_station_10)
	{
		if (me->eventAPI && !gs_locked_station_10)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_LOCKED_STATION_10, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_40 \r\n");
		}
		
		gs_locked_station_10 = 1;
	}
	else
	{
		gs_locked_station_10 = 0;
	}
	/* Check if panel unlocked station 10 */
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_STATION_10", &gs_lock_station_10));
	EXCCHECK( me->GetSensor(me, "SENSOR_UNLOCK_STATION_10", &gs_unlock_station_10));
	if (gs_unlock_station_10 && !gs_lock_station_10)
	{
		if (me->eventAPI && !gs_unlocked_station_10)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNLOCKED_STATION_10, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_40 \r\n");
		}
		
		gs_unlocked_station_10 = 1;
	}
	else
	{
		gs_unlocked_station_10 = 0;
	}
	
	/* Check if vs11 selected station 40 */
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_40", &gs_select_vs11_station_40));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_40", &gs_select_x52_station_40));
	if (!gs_select_x52_station_40 && gs_select_vs11_station_40)
	{
		if (me->eventAPI && !gs_vs11_selected_station_40)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_VS11_SELECTED_STATION_40, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_40 \r\n");
		}
		
		gs_vs11_selected_station_40 = 1;
	}
	else
	{
		gs_vs11_selected_station_40 = 0;
	}
	/* Check if x52 selected station 40 */
    EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_40", &gs_select_vs11_station_40));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_40", &gs_select_x52_station_40));
	if (gs_select_x52_station_40 && !gs_select_vs11_station_40)
	{
		if (me->eventAPI && !gs_x52_selected_station_40)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_X52_SELECTED_STATION_40, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_40 \r\n");
		}
		
		gs_x52_selected_station_40 = 1;
	}
	else
	{
		gs_x52_selected_station_40 = 0;
	}
	
	/* Check if panel plugged station 40 X52 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_X52", &gs_plug_station_40_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_X52", &gs_unplug_station_40_x52));
	
	if (!gs_unplug_station_40_x52 && gs_plug_station_40_x52)
	{
		if (me->eventAPI && !gs_plugged_station_40_x52)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_40_X52, NULL));
			//printf ("EVNT_JIG_ITEM_PLUGGED_STATION_40 \r\n");
		}
		
		gs_plugged_station_40_x52 = 1;
	}
	else
	{
		gs_plugged_station_40_x52 = 0;
	}
	/* Check if panel unplugged station 40 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_X52", &gs_plug_station_40_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_X52", &gs_unplug_station_40_x52));
	
	if (gs_unplug_station_40_x52 && !gs_plug_station_40_x52)
	{
		if (me->eventAPI && !gs_unplugged_station_40_x52)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_40_X52, NULL));
	//		printf ("EVNT_JIG_ITEM_UNPLUGGED_STATION_40_X52 \r\n");
		}

		
		gs_unplugged_station_40_x52 = 1;
	//	printf("gs_unplugged_station_40_x52 =%d\r\n",gs_unplugged_station_40_x52); 
	}
	else
	{
		gs_unplugged_station_40_x52 = 0;
	}
	
		/* Check if panel plugged station 40 VS11 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_VS11", &gs_plug_station_40_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_VS11", &gs_unplug_station_40_vs11));
	if (!gs_unplug_station_40_vs11 && gs_plug_station_40_vs11)
	{
		if (me->eventAPI && !gs_plugged_station_40_vs11)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_40_VS11, NULL));
	//		printf ("EVNT_JIG_ITEM_UNPLUGGED_STATION_40_VS11 \r\n"); 
		}
		
		gs_plugged_station_40_vs11 = 1;
	//	printf("gs_plugged_station_40_vs11 =%d\r\n",gs_plugged_station_40_vs11);
	}
	else
	{   
		gs_plugged_station_40_vs11 = 0;
	//	printf("gs_plugged_station_40_vs11 in else =%d\r\n",gs_plugged_station_40_vs11); 
	}
	/* Check if panel unplugged station 40 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_VS11", &gs_plug_station_40_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_VS11", &gs_unplug_station_40_vs11));
	
	if (gs_unplug_station_40_vs11 && !gs_plug_station_40_vs11)
	{
		if (me->eventAPI && !gs_unplugged_station_40_vs11)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_40_VS11, NULL));
			//printf ("EVNT_JIG_ITEM_UNPLUGGED_STATION_40 \r\n");
		}

		
		gs_unplugged_station_40_vs11 = 1;
	}
	else
	{
		gs_unplugged_station_40_vs11 = 0;
	}

		/* Check if vs11 selected station 50 */
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_50", &gs_select_vs11_station_50));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_50", &gs_select_x52_station_50));
	if (!gs_select_x52_station_50 && gs_select_vs11_station_50)
	{
		if (me->eventAPI && !gs_vs11_selected_station_50)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_VS11_SELECTED_STATION_50, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_40 \r\n");
		}
		
		gs_vs11_selected_station_50 = 1;
	}
	else
	{
		gs_vs11_selected_station_50 = 0;
	}
	/* Check if x52 selected station 50 */
    EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_50", &gs_select_vs11_station_50));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_50", &gs_select_x52_station_50));
	if (gs_select_x52_station_50 && !gs_select_vs11_station_50)
	{
		if (me->eventAPI && !gs_x52_selected_station_50)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_X52_SELECTED_STATION_50, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_40 \r\n");
		}
		
		gs_x52_selected_station_50 = 1;
	}
	else
	{
		gs_x52_selected_station_50 = 0;
	}
	
	/* Check if panel plugged station 50 X52*/
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_X52", &gs_plug_station_50_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_X52", &gs_unplug_station_50_x52));
	
	if (!gs_unplug_station_50_x52 && gs_plug_station_50_x52)
	{
		if (me->eventAPI && !gs_plugged_station_50_x52)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_50_X52, NULL));
			//printf ("EVNT_JIG_ITEM_PLUGGED_STATION_50 \r\n");
		}

		gs_plugged_station_50_x52 = 1;
	}
	else
	{
		gs_plugged_station_50_x52 = 0;
	}
	/* Check if panel unplugged station 50 X52*/
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_X52", &gs_plug_station_50_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_X52", &gs_unplug_station_50_x52));
	
	if (gs_unplug_station_50_x52 && !gs_plug_station_50_x52)
	{
		if (me->eventAPI && !gs_unplugged_station_50_x52)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_50_X52, NULL));
			//printf ("EVNT_JIG_ITEM_UNPLUGGED_STATION_50 \r\n");
		}

		
		gs_unplugged_station_50_x52 = 1;
	}
	else
	{
		gs_unplugged_station_50_x52 = 0;
	}
	
	
	/* Check if panel plugged station 50 VS11*/
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_VS11", &gs_plug_station_50_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_VS11", &gs_unplug_station_50_vs11));
	
	if (!gs_unplug_station_50_vs11 && gs_plug_station_50_vs11)
	{
		if (me->eventAPI && !gs_plugged_station_50_vs11)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_50_VS11, NULL));
			//printf ("EVNT_JIG_ITEM_PLUGGED_STATION_50 \r\n");
		}

		gs_plugged_station_50_vs11 = 1;
	}
	else
	{
		gs_plugged_station_50_vs11 = 0;
	}
	/* Check if panel unplugged station 50 VS11*/
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_VS11", &gs_plug_station_50_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_VS11", &gs_unplug_station_50_vs11));
	
	if (gs_unplug_station_50_vs11 && !gs_plug_station_50_vs11)
	{
		if (me->eventAPI && !gs_unplugged_station_50_vs11)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_50_VS11, NULL));
			//printf ("EVNT_JIG_ITEM_UNPLUGGED_STATION_50 \r\n");
		}

		
		gs_unplugged_station_50_vs11 = 1;
	}
	else
	{
		gs_unplugged_station_50_vs11 = 0;
	}
	
	/* Check if vs11 selected station 60 */
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_60", &gs_select_vs11_station_60));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_60", &gs_select_x52_station_60));
	if (!gs_select_x52_station_60 && gs_select_vs11_station_60)
	{
		if (me->eventAPI && !gs_vs11_selected_station_60)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_VS11_SELECTED_STATION_60, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_40 \r\n");
		}
		
		gs_vs11_selected_station_60 = 1;
	}
	else
	{
		gs_vs11_selected_station_60 = 0;
	}
	/* Check if X52 selected station 60 */
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_60", &gs_select_vs11_station_60));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_60", &gs_select_x52_station_60));
	if (!gs_select_vs11_station_60 && gs_select_x52_station_60)
	{
		if (me->eventAPI && !gs_x52_selected_station_60)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_X52_SELECTED_STATION_60, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_40 \r\n");
		}
		
		gs_x52_selected_station_60 = 1;
	}
	else
	{
		gs_x52_selected_station_60 = 0;
	}
	
	/* Check if panel plugged station 60 X52*/
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_X52", &gs_plug_station_60_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_X52", &gs_unplug_station_60_x52));
	
	if (!gs_unplug_station_60_x52 && gs_plug_station_60_x52)
	{
		if (me->eventAPI && !gs_plugged_station_60_x52)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_60_X52, NULL));
			//printf ("EVNT_JIG_ITEM_PLUGGED_STATION_60 \r\n");
		}

		gs_plugged_station_60_x52 = 1;
	}
	else
	{
		gs_plugged_station_60_x52 = 0;
	}
	/* Check if panel unplugged station 60 X52*/
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_X52", &gs_plug_station_60_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_X52", &gs_unplug_station_60_x52));
	if (gs_unplug_station_60_x52 && !gs_plug_station_60_x52)
	{
		if (me->eventAPI && !gs_unplugged_station_60_x52)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_60_X52, NULL));
			//printf ("EVNT_JIG_ITEM_UNPLUGGED_STATION_60 \r\n");
		}

		
		gs_unplugged_station_60_x52 = 1;
	}
	else
	{
		gs_unplugged_station_60_x52 = 0;
	}
	
	
	
		/* Check if panel plugged station 60 VS11*/
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_VS11", &gs_plug_station_60_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_VS11", &gs_unplug_station_60_vs11));
	
	if (!gs_unplug_station_60_vs11 && gs_plug_station_60_vs11)
	{
		if (me->eventAPI && !gs_plugged_station_60_vs11)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_60_VS11, NULL));
			//printf ("EVNT_JIG_ITEM_PLUGGED_STATION_60 \r\n");
		}

		gs_plugged_station_60_vs11 = 1;
	}
	else
	{
		gs_plugged_station_60_vs11 = 0;
	}
	/* Check if panel unplugged station 60 VS11*/
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_VS11", &gs_plug_station_60_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_VS11", &gs_unplug_station_60_vs11));
	if (gs_unplug_station_60_vs11 && !gs_plug_station_60_vs11)
	{
		if (me->eventAPI && !gs_unplugged_station_60_vs11)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_60_VS11, NULL));
			//printf ("EVNT_JIG_ITEM_UNPLUGGED_STATION_60 \r\n");
		}

		
		gs_unplugged_station_60_vs11 = 1;
	}
	else
	{
		gs_unplugged_station_60_vs11 = 0;
	}
	
		/* Check if panel locked station 80 */
	EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &gs_lock_station_80));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &gs_unlock_station_80));
	if (!gs_unlock_station_80 && gs_lock_station_80)
	{
		if (me->eventAPI && !gs_locked_station_80)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_LOCKED_STATION_80, NULL));
			//printf ("EVNT_JIG_ITEM_LOCKED_STATION_60 \r\n");
		}

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
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_JIG_ITEM_UNLOCKED_STATION_80, NULL));
			//printf ("EVNT_JIG_ITEM_UNLOCKED_STATION_60 \r\n");
		}

		
		gs_unlocked_station_80 = 1;
	}
	else
	{
		gs_unlocked_station_80 = 0;
	}
	
		/* Check if axis X in Rear Station 80 */
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &gs_axis_x_rear));
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &gs_axis_x_front));
	if (gs_axis_x_rear && !gs_axis_x_front)
	{
		if (me->eventAPI && !gs_axis_x_rear)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_X_REAR_STATION_80, NULL));
			//printf ("EVNT_JIG_ITEM_UNLOCKED_STATION_60 \r\n");
		}

		
		gs_axis_x_rear = 1;
	}
	else
	{
		gs_axis_x_rear = 0;
	}
	
	
		/* Check if axis X in Front Station 80 */
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &gs_axis_x_front));
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &gs_axis_x_rear));
	if (gs_axis_x_rear && !gs_axis_x_front)
	{
		if (me->eventAPI && !gs_axis_x_front)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_X_FRONT_STATION_80, NULL));
			//printf ("EVNT_JIG_ITEM_UNLOCKED_STATION_60 \r\n");
		}

		
		gs_axis_x_front = 1;
	}
	else
	{
		gs_axis_x_front = 0;
	}
	
	
		/* Check if axis Z in UP Station 80 */
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &gs_axis_z_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &gs_axis_z_down));
	if (gs_axis_z_up && !gs_axis_z_down)
	{
		if (me->eventAPI && !gs_axis_z_up)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Z_UP_STATION_80, NULL));
			//printf ("EVNT_JIG_ITEM_UNLOCKED_STATION_60 \r\n");
		}

		
		gs_axis_z_up = 1;
	}
	else
	{
		gs_axis_z_up = 0;
	}
	
	
			/* Check if axis Z in UP Station 80 */
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &gs_axis_z_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &gs_axis_z_up));
	if (gs_axis_z_down && !gs_axis_z_up)
	{
		if (me->eventAPI && !gs_axis_z_down)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_AXIS_Z_DOWN_STATION_80, NULL));
			//printf ("EVNT_JIG_ITEM_UNLOCKED_STATION_60 \r\n");
		}

		
		gs_axis_z_down = 1;
	}
	else
	{
		gs_axis_z_down = 0;
	}
	
			/* Check if pince Open Station 80 */
	EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_OPEN_STATION_80", &gs_pince_open));
	EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_CLOSE_STATION_80", &gs_pince_close));
	if (gs_pince_open && !gs_pince_close)
	{
		if (me->eventAPI && !gs_pince_open)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_PINCE_OPEN_STATION_80, NULL));
			//printf ("EVNT_JIG_ITEM_UNLOCKED_STATION_60 \r\n");
		}

		
		gs_pince_open = 1;
	}
	else
	{
		gs_pince_open = 0;
	}
	
	
				/* Check if pince Closed Station 80 */
	EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_CLOSE_STATION_80", &gs_pince_close));
	EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_OPEN_STATION_80", &gs_pince_open));
	if (gs_pince_close && !gs_pince_open)
	{
		if (me->eventAPI && !gs_pince_open)
		{
			EXCCHECK( me->eventAPI->PulseEvent(me->eventAPI, EVNT_PINCE_CLOSE_STATION_80, NULL));
			//printf ("EVNT_JIG_ITEM_UNLOCKED_STATION_60 \r\n");
		}

		
		gs_pince_close = 1;
	}
	else
	{
		gs_pince_close = 0;
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
	

	uint32_t		status_cylinder_lock_punzoni,
					status_cylinder_unlock_punzoni,
					status_cylinder_lock_contrasti,
					status_cylinder_unlock_contrasti,
					status_gefran_front_int,
					status_gefran_rear_int,
					status_gefran_front_ext,
					status_gefran_rear_ext,
					status_cylinder_rodage_up,
					status_cylinder_rodage_down,
					status_cylinder_rodage_push_1_up,
					status_cylinder_rodage_push_1_down,
					status_cylinder_rodage_push_2_up,
					status_cylinder_rodage_push_2_down,
					status_cylinder_rodage_push_3_up,
					status_cylinder_rodage_push_3_down,
					status_cylinder_rodage_push_4_up,
					status_cylinder_rodage_push_4_down,
					status_cylinder_rodage_push_5_up,
					status_cylinder_rodage_push_5_down,
					status_cylinder_rodage_push_6_up,
					status_cylinder_rodage_push_6_down,
					status_cylinder_rodage_push_7_up,
					status_cylinder_rodage_push_7_down,
					status_cylinder_rodage_push_8_up,
					status_cylinder_rodage_push_8_down,
					status_cylinder_rodage_push_9_up,
					status_cylinder_rodage_push_9_down,
					is_panel_locked_10,
					is_panel_unlocked_10,
					is_panel_locked_40,
					is_panel_unlocked_40,
					is_panel_plugged_40_x52,
					is_panel_unplugged_40_x52,
					is_panel_plugged_40_vs11,
					is_panel_unplugged_40_vs11,
					is_axis_Z_home_40,
					is_panel_locked_50,
					is_panel_unlocked_50,
					is_panel_plugged_50_x52,
					is_panel_unplugged_50_x52,
					is_panel_plugged_50_vs11,
					is_panel_unplugged_50_vs11,
					is_axis_Z_home_50,
					is_panel_locked_60,
					is_panel_unlocked_60,
					is_panel_plugged_60_x52,
					is_panel_unplugged_60_x52,
					is_panel_plugged_60_vs11,
					is_panel_unplugged_60_vs11,
					is_panel_locked_80,
					is_panel_unlocked_80,
					is_cylinder_x_front,
					is_cylinder_x_rear,
					is_cylinder_z_up,
					is_cylinder_z_down,
					is_pince_open,
					is_pince_close,  
					is_cylinder_printer_rear,
					is_cylinder_printer_up,
					is_sensor_cylinder_printer_rear;

	bool_t			is_station_10_ready = FALSE,
					is_station_20_ready = FALSE,
					is_station_30_ready = FALSE,
					is_station_40_ready = FALSE,
					is_station_50_ready = FALSE,
					is_station_60_ready = FALSE,
					is_station_70_ready = FALSE, 
					is_station_80_ready = FALSE;
	
	
         	/* STATION 10 */
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_STATION_10", &is_panel_locked_10));
			EXCCHECK( me->GetSensor(me, "SENSOR_UNLOCK_STATION_10", &is_panel_unlocked_10));
			
			if(!is_panel_unlocked_10 && is_panel_locked_10)
			   
			{
				is_station_10_ready = TRUE;
			}
			else						 
				is_station_10_ready = FALSE;

	

			/* STATION 20 */
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_ON", &status_cylinder_lock_punzoni));
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_OFF", &status_cylinder_unlock_punzoni));
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_ON", &status_cylinder_lock_contrasti));
			EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_OFF", &status_cylinder_unlock_contrasti));
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_ON", &status_gefran_front_int)); 
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_OFF", &status_gefran_rear_int)); 
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_ON", &status_gefran_front_ext)); 
			EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_OFF", &status_gefran_rear_ext)); 
			
			if(!status_cylinder_lock_punzoni && status_cylinder_unlock_punzoni &&
			   !status_cylinder_lock_contrasti && status_cylinder_unlock_contrasti &&
			   !status_gefran_front_int && status_gefran_rear_int && 
			   !status_gefran_front_ext && status_gefran_rear_ext)
			{
				is_station_20_ready = TRUE;
			}
			else						 
				is_station_20_ready = FALSE;


			/* STATION 30 */
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_UP", &status_cylinder_rodage_push_1_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_DOWN", &status_cylinder_rodage_push_1_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_UP", &status_cylinder_rodage_push_2_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_DOWN", &status_cylinder_rodage_push_2_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_UP", &status_cylinder_rodage_push_3_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_DOWN", &status_cylinder_rodage_push_3_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_UP", &status_cylinder_rodage_push_4_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_DOWN", &status_cylinder_rodage_push_4_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_UP", &status_cylinder_rodage_push_5_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_DOWN", &status_cylinder_rodage_push_5_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_UP", &status_cylinder_rodage_push_6_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_DOWN", &status_cylinder_rodage_push_6_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_UP", &status_cylinder_rodage_push_7_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_DOWN", &status_cylinder_rodage_push_7_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_UP", &status_cylinder_rodage_push_8_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_DOWN", &status_cylinder_rodage_push_8_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_UP", &status_cylinder_rodage_push_9_up));
			EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_DOWN", &status_cylinder_rodage_push_9_down));

			
			if(status_cylinder_rodage_push_1_up && !status_cylinder_rodage_push_1_down &&
			 /*  status_cylinder_rodage_push_2_up && */!status_cylinder_rodage_push_2_down &&
			   status_cylinder_rodage_push_3_up && !status_cylinder_rodage_push_3_down &&
			   status_cylinder_rodage_push_4_up && !status_cylinder_rodage_push_4_down &&
			   status_cylinder_rodage_push_5_up && !status_cylinder_rodage_push_5_down &&
			   status_cylinder_rodage_push_6_up && !status_cylinder_rodage_push_6_down &&
			   status_cylinder_rodage_push_7_up && !status_cylinder_rodage_push_7_down &&
			   status_cylinder_rodage_push_8_up && !status_cylinder_rodage_push_8_down && 
			   status_cylinder_rodage_push_9_up && !status_cylinder_rodage_push_9_down)
			{
				is_station_30_ready = TRUE;
			}
			else 
				is_station_30_ready = FALSE;

			
			/* STATION 40 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_X52", &is_panel_plugged_40_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_X52", &is_panel_unplugged_40_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_VS11", &is_panel_plugged_40_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_VS11", &is_panel_unplugged_40_vs11));
			EXCCHECK( me->GetSensor(me, "AXIS_Z_HOME_STATION_40", &is_axis_Z_home_40));

			if(is_panel_unplugged_40_x52 && !is_panel_plugged_40_x52 &&
			   is_panel_unplugged_40_vs11 && !is_panel_plugged_40_vs11 &&
			   is_axis_Z_home_40)
			{
				is_station_40_ready = TRUE;
			}
			else 
				is_station_40_ready = FALSE;
		

			/* STATION 50 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_X52", &is_panel_plugged_50_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_X52", &is_panel_unplugged_50_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_VS11", &is_panel_plugged_50_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_VS11", &is_panel_unplugged_50_vs11));
			EXCCHECK( me->GetSensor(me, "AXIS_Z_HOME_STATION_50", &is_axis_Z_home_50));
			
			if(is_panel_unplugged_50_x52 && !is_panel_plugged_50_x52 &&
			   is_panel_unplugged_50_vs11 && !is_panel_plugged_50_vs11 &&  
			   is_axis_Z_home_50)
			{
				is_station_50_ready = TRUE;
			}
			else 
				is_station_50_ready = FALSE;
		

			/* STATION 60 */
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_X52", &is_panel_plugged_60_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_X52", &is_panel_unplugged_60_x52));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_VS11", &is_panel_plugged_60_vs11));
			EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_VS11", &is_panel_unplugged_60_vs11));
			
			if(is_panel_unplugged_60_x52 && !is_panel_plugged_60_x52 &&
			   is_panel_unplugged_60_vs11 && !is_panel_plugged_60_vs11)
			{
				is_station_60_ready = TRUE;
			}
			else 
				is_station_60_ready = FALSE;
			
			/* STATION 70 */
			EXCCHECK( me->GetSensor(me, "IS_CYLINDER_PRINTER_REAR", &is_cylinder_printer_rear));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_PRINTER_REAR", &is_sensor_cylinder_printer_rear));
			if(is_cylinder_printer_rear && is_sensor_cylinder_printer_rear)
			{
				is_station_70_ready = TRUE;
			}
	    	else 
				is_station_70_ready = FALSE;
						
			
			/* STATION 80 */
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &is_cylinder_x_front));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &is_cylinder_x_rear));
			EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &is_cylinder_z_up));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &is_cylinder_z_down));
			EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_OPEN_STATION_80", &is_pince_open));
	  		EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_CLOSE_STATION_80", &is_pince_close));

			if(is_cylinder_x_rear && !is_cylinder_x_front && is_cylinder_z_up && !is_cylinder_z_down && is_pince_open && !is_pince_close)
				is_station_80_ready = TRUE;
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




/* Clip panel */
#undef __FUNC__
#define __FUNC__	"Clipping_punzoni"
static SElExceptionPtr Clipping_punzoni
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t				 status_gefran_front_int,
		                 status_gefran_rear_int,
						 status_gefran_front_ext,
						 status_gefran_rear_ext,
		                 microCam;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_CLIPPED_PUNZONI, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_ON", &status_gefran_front_int)); 
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_OFF", &status_gefran_rear_int)); 
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_ON", &status_gefran_front_ext)); 
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_OFF", &status_gefran_rear_ext)); 
	if (microCam && status_gefran_rear_int && !status_gefran_front_int && !status_gefran_front_ext && status_gefran_rear_ext)
	{
	EXCCHECK( me->SetSensor(me, "LOCK_PUNZONI_ON", 1));
	EXCCHECK( me->SetSensor(me, "LOCK_PUNZONI_OFF", 0));
	}
	

	if (timeout)
	{
		if(gs_clipped_punzoni) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_clipped_punzoni) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}


/* Activate Air Printer */
#undef __FUNC__
#define __FUNC__	"Activate_Air_Printer"
static SElExceptionPtr Activate_Air_Printer
(
	struct _STester* me
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			microCam;
	
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "ACTIVATE_AIR_PRINTER", 1));
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Disactivate Air Printer */
#undef __FUNC__
#define __FUNC__	"Disactivate_Air_Printer"
static SElExceptionPtr Disactivate_Air_Printer
(
	struct _STester* me
)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( me->SetSensor(me, "ACTIVATE_AIR_PRINTER", 0));
	
Error:
	EXCRETHROW(pexception);
}



/* Stop Clip panel */
#undef __FUNC__
#define __FUNC__	"StopClipping_punzoni"
static SElExceptionPtr StopClipping_punzoni
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t				 status_gefran_front_int,
		                 status_gefran_rear_int,
						 status_gefran_front_ext,
						 status_gefran_rear_ext,
		                 microCam;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_CLIPPED_PUNZONI, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_ON", &status_gefran_front_int)); 
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_INT_OFF", &status_gefran_rear_int)); 
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_ON", &status_gefran_front_ext)); 
	EXCCHECK( me->GetSensor(me, "SENSOR_POTENT_EXT_OFF", &status_gefran_rear_ext)); 
	if (status_gefran_rear_int && !status_gefran_front_int && !status_gefran_front_ext && status_gefran_rear_ext)
	{
	EXCCHECK( me->SetSensor(me, "LOCK_PUNZONI_ON", 0));
	EXCCHECK( me->SetSensor(me, "LOCK_PUNZONI_OFF", 1));
	}
	

	if (timeout)
	{
		if(gs_stop_clipped_punzoni) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_stop_clipped_punzoni) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Clip panel */
#undef __FUNC__
#define __FUNC__	"Clipping_contrasti"
static SElExceptionPtr Clipping_contrasti
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_CLIPPED_CONTRASTI, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "LOCK_CONTRASTI_ON", 1));
	EXCCHECK( me->SetSensor(me, "LOCK_CONTRASTI_OFF", 0));
	}
	

	if (timeout)
	{
		if(gs_clipped_contrasti) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_clipped_contrasti) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Stop Clip panel */
#undef __FUNC__
#define __FUNC__	"StopClipping_contrasti"
static SElExceptionPtr StopClipping_contrasti
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_CLIPPED_CONTRASTI, &hevent));


	EXCCHECK( me->SetSensor(me, "LOCK_CONTRASTI_ON", 0));
	EXCCHECK( me->SetSensor(me, "LOCK_CONTRASTI_OFF", 1));
	
	

	if (timeout)
	{
		if(gs_stop_clipped_contrasti) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_stop_clipped_contrasti) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Check Clip panel */
#undef __FUNC__
#define __FUNC__	"CheckClipping"
static SElExceptionPtr CheckClipping
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            status_cylinder_lock_punzoni,
					status_cylinder_unlock_punzoni,
					status_cylinder_lock_contrasti,
					status_cylinder_unlock_contrasti;
					

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_CHECKED, &hevent));
	
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_ON", &status_cylinder_lock_punzoni));
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_PUNZONI_OFF", &status_cylinder_unlock_punzoni));
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_ON", &status_cylinder_lock_contrasti));
	EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_CONTRASTI_OFF", &status_cylinder_unlock_contrasti));
	
	if (microCam && status_cylinder_lock_punzoni && !status_cylinder_unlock_punzoni /*&& status_cylinder_lock_contrasti && !status_cylinder_unlock_contrasti*/)
	{
	EXCCHECK( me->SetSensor(me, "POTENTIOMETER_ON", 1));
	EXCCHECK( me->SetSensor(me, "POTENTIOMETER_OFF", 0));
	}

	if (timeout)
	{
		if(gs_checked) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_checked) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Stop Check Clip panel */
#undef __FUNC__
#define __FUNC__	"StopCheckClipping"
static SElExceptionPtr StopCheckClipping
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_CHECKED, &hevent));

	EXCCHECK( me->SetSensor(me, "POTENTIOMETER_OFF", 1));
	EXCCHECK( me->SetSensor(me, "POTENTIOMETER_ON", 0));
//	printf("Stop Check clipping executed \n\r");

	if (timeout)
	{
		if(gs_stop_checked) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_stop_checked) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Start rodage cylinder all pushs*/
#undef __FUNC__
#define __FUNC__	"SelectRodageX52"
static SElExceptionPtr SelectRodageX52
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            status_cylinder_rodage_push_1_up,
					status_cylinder_rodage_push_1_down,
					status_cylinder_rodage_push_2_up,
					status_cylinder_rodage_push_2_down,
					status_cylinder_rodage_push_3_up,
					status_cylinder_rodage_push_3_down,
					status_cylinder_rodage_push_4_up,
					status_cylinder_rodage_push_4_down,
					status_cylinder_rodage_push_5_up,
					status_cylinder_rodage_push_5_down,
					status_cylinder_rodage_push_6_up,
					status_cylinder_rodage_push_6_down,
					status_cylinder_rodage_push_7_up,
					status_cylinder_rodage_push_7_down,
					status_cylinder_rodage_push_8_up,
					status_cylinder_rodage_push_8_down,
					status_cylinder_rodage_push_9_up,
					status_cylinder_rodage_push_9_down;
	
	int32_t         Selected_X52, Selected_VS11	;
	
	TIMEOUT_INIT();

	/* activate pushs up */
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_7", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_8", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_9", 0));
	Sleep(100);
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_8", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_9", 1));
	
	Sleep(500);

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_START_RODAGE, &hevent));
	
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam)); 
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_UP", &status_cylinder_rodage_push_1_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_DOWN", &status_cylinder_rodage_push_1_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_UP", &status_cylinder_rodage_push_2_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_DOWN", &status_cylinder_rodage_push_2_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_UP", &status_cylinder_rodage_push_3_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_DOWN", &status_cylinder_rodage_push_3_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_UP", &status_cylinder_rodage_push_4_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_DOWN", &status_cylinder_rodage_push_4_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_UP", &status_cylinder_rodage_push_5_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_DOWN", &status_cylinder_rodage_push_5_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_UP", &status_cylinder_rodage_push_6_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_DOWN", &status_cylinder_rodage_push_6_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_UP", &status_cylinder_rodage_push_7_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_DOWN", &status_cylinder_rodage_push_7_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_UP", &status_cylinder_rodage_push_8_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_DOWN", &status_cylinder_rodage_push_8_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_UP", &status_cylinder_rodage_push_9_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_DOWN", &status_cylinder_rodage_push_9_down));

	if (microCam 
		&& status_cylinder_rodage_push_1_up && /*status_cylinder_rodage_push_2_up && */status_cylinder_rodage_push_3_up
		&& status_cylinder_rodage_push_4_up && status_cylinder_rodage_push_5_up && status_cylinder_rodage_push_6_up 
		&& status_cylinder_rodage_push_7_up && status_cylinder_rodage_push_8_up && status_cylinder_rodage_push_9_up 
		&& !status_cylinder_rodage_push_1_down && !status_cylinder_rodage_push_2_down && !status_cylinder_rodage_push_3_down
		&& !status_cylinder_rodage_push_4_down && !status_cylinder_rodage_push_5_down && !status_cylinder_rodage_push_6_down
		&& !status_cylinder_rodage_push_7_down && !status_cylinder_rodage_push_8_down && !status_cylinder_rodage_push_9_down)
	{
	EXCCHECK( me->SetSensor(me, "SELECT_RODAGE_X52", 1)); 	
	EXCCHECK( me->SetSensor(me, "SELECT_RODAGE_VS11", 0));
	}
	
	else 
	{
		EXCTHROW( TESTER_ERROR_TIMEOUT, "condition de chagement vérin non valide : \r\n microcam = 0 \r\n ou véins de rodage pushs est en bas ");
	}

	/* Check if rodage selected */
	while(TRUE)
	{
		
		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_X52", &Selected_X52));
		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_VS11", &Selected_VS11));
		if(Selected_X52 && !Selected_VS11)
			break;

		if(timeout>=0)
			TIMEOUT_CHECK( timeout);

	};
	
Error:
	EXCRETHROW(pexception);
}

/* Stop rodage cylinder all pushs*/
#undef __FUNC__
#define __FUNC__	"SelectRodageVS11"
static SElExceptionPtr SelectRodageVS11
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            status_cylinder_rodage_push_1_up,
					status_cylinder_rodage_push_1_down,
					status_cylinder_rodage_push_2_up,
					status_cylinder_rodage_push_2_down,
					status_cylinder_rodage_push_3_up,
					status_cylinder_rodage_push_3_down,
					status_cylinder_rodage_push_4_up,
					status_cylinder_rodage_push_4_down,
					status_cylinder_rodage_push_5_up,
					status_cylinder_rodage_push_5_down,
					status_cylinder_rodage_push_6_up,
					status_cylinder_rodage_push_6_down,
					status_cylinder_rodage_push_7_up,
					status_cylinder_rodage_push_7_down,
					status_cylinder_rodage_push_8_up,
					status_cylinder_rodage_push_8_down,
					status_cylinder_rodage_push_9_up,
					status_cylinder_rodage_push_9_down;
	
	int32_t         Selected_X52, Selected_VS11	;	

	
	TIMEOUT_INIT();

		/* activate pushs up */
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_7", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_8", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_9", 0));
	Sleep(100);
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_8", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_9", 1));
	
	Sleep(500);
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_STOP_RODAGE, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam)); 
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_UP", &status_cylinder_rodage_push_1_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_DOWN", &status_cylinder_rodage_push_1_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_UP", &status_cylinder_rodage_push_2_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_DOWN", &status_cylinder_rodage_push_2_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_UP", &status_cylinder_rodage_push_3_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_DOWN", &status_cylinder_rodage_push_3_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_UP", &status_cylinder_rodage_push_4_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_DOWN", &status_cylinder_rodage_push_4_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_UP", &status_cylinder_rodage_push_5_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_DOWN", &status_cylinder_rodage_push_5_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_UP", &status_cylinder_rodage_push_6_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_DOWN", &status_cylinder_rodage_push_6_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_UP", &status_cylinder_rodage_push_7_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_DOWN", &status_cylinder_rodage_push_7_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_UP", &status_cylinder_rodage_push_8_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_DOWN", &status_cylinder_rodage_push_8_down));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_UP", &status_cylinder_rodage_push_9_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_DOWN", &status_cylinder_rodage_push_9_down));

	if (microCam 
		&& status_cylinder_rodage_push_1_up &&/* status_cylinder_rodage_push_2_up &&*/ status_cylinder_rodage_push_3_up
		&& status_cylinder_rodage_push_4_up && status_cylinder_rodage_push_5_up && status_cylinder_rodage_push_6_up 
		&& status_cylinder_rodage_push_7_up && status_cylinder_rodage_push_8_up && status_cylinder_rodage_push_9_up 
		&& !status_cylinder_rodage_push_1_down && !status_cylinder_rodage_push_2_down && !status_cylinder_rodage_push_3_down
		&& !status_cylinder_rodage_push_4_down && !status_cylinder_rodage_push_5_down && !status_cylinder_rodage_push_6_down
		&& !status_cylinder_rodage_push_7_down && !status_cylinder_rodage_push_8_down && !status_cylinder_rodage_push_9_down)
	{
	EXCCHECK( me->SetSensor(me, "SELECT_RODAGE_VS11", 1)); 	
	EXCCHECK( me->SetSensor(me, "SELECT_RODAGE_X52", 0));
	
	}
	else 
	{
		EXCTHROW( TESTER_ERROR_TIMEOUT, "condition de chagement vérin non valide : \r\n microcam = 0 \r\n ou véins de rodage pushs est en bas ");
	}

	while(TRUE)
	{
		
		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_X52", &Selected_X52));
		EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_VS11", &Selected_VS11));
		if(!Selected_X52 && Selected_VS11)
			break;

		if(timeout>=0)
			TIMEOUT_CHECK( timeout);

	};
	
	
Error:
	EXCRETHROW(pexception);
}

/* Start rodage pushs */
#undef __FUNC__
#define __FUNC__	"Rodage"
static SElExceptionPtr Rodage
(
	struct _STester* me,
	int32_t			timeout,
	int32_t			cycle,
	int32_t			timeon,
	int32_t			timeoff,
	char            version[9+1]
)
{
	SElExceptionPtr	pexception = NULL;
	int i;
	int32_t			enable_push_1 = 0,
					enable_push_2 = 0,
					enable_push_3 = 0,
					enable_push_4 = 0,
					enable_push_5 = 0,
					enable_push_6 = 0,
					enable_push_7 = 0,
					enable_push_8 = 0,
	                enable_push_9 = 0;
	
	int32_t			sensor_push_up = 0,
					sensor_push_down = 0;
	   
	int32_t			microCam,
	                status_rodage_x52,
					status_rodage_vs11;
	
	
	enable_push_1 = version[0];
	enable_push_2 = version[1];
	enable_push_3 = version[2];
	enable_push_4 = version[3];
	enable_push_5 = version[4];
	enable_push_6 = version[5];
	enable_push_7 = version[6];
	enable_push_8 = version[7];
	enable_push_9 = version[8];

		
	/* all pushs up initialy */
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_7", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_8", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_8", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_9", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_9", 0));
	Sleep(500);

EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_X52", &status_rodage_x52));
EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_RODAGE_VS11", &status_rodage_vs11));
if (microCam && (status_rodage_x52 || status_rodage_vs11))
	{
#if 0    /* formerly excluded lines */
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_DOWN", &sensor_push_down));
	if (!sensor_push_up || sensor_push_down)
		EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_DOWN", &sensor_push_down));
	if (!sensor_push_up || sensor_push_down)
		EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_DOWN", &sensor_push_down));
	if (!sensor_push_up || sensor_push_down)
		EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_DOWN", &sensor_push_down));
	if (!sensor_push_up || sensor_push_down)
		EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_DOWN", &sensor_push_down));
	if (!sensor_push_up || sensor_push_down)
		EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	
		
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_DOWN", &sensor_push_down));
	if (!sensor_push_up || sensor_push_down)
		EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_DOWN", &sensor_push_down));
	if (!sensor_push_up || sensor_push_down)
		EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
	
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_DOWN", &sensor_push_down));
	if (!sensor_push_up || sensor_push_down)
		EXCTHROW( TESTER_ERROR_TIMEOUT, "Timeout");
#endif   /* formerly excluded lines */
	
	
	/*Check Dummy pushs */
	
#if 0    /* formerly excluded lines */
	/* all pushs down */
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_7", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_8", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_8", 0));
	Sleep(500);
	
	/* Check Dummy pushs */
	if (enable_push_1)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_1, "ERROR PUSH 1");	
	}
	else if (!enable_push_1)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_1_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_1, "ERROR_PUSH_1");	
	}
	
	if (enable_push_2)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_2, "ERROR_PUSH_2");	
	}
	else if (!enable_push_2)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_2_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_2, "ERROR_PUSH_2");	
	}
	
	if (enable_push_3)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_3, "ERROR_PUSH_3");	
	}
	else if (!enable_push_3)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_3_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_3, "ERROR_PUSH_3");	
	}
	
	if (enable_push_4)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_4, "ERROR_PUSH_4");	
	}
	else if (!enable_push_4)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_4_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_4, "ERROR_PUSH_4");	
	}
	
	if (enable_push_5)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_5, "ERROR_PUSH_5");	
	}
	else if (!enable_push_5)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_5_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_5, "ERROR_PUSH_5");	
	}
	
	if (enable_push_6)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_6, "ERROR_PUSH_6");	
	}
	else if (!enable_push_6)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_6_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_6, "ERROR_PUSH_6");	
	}
	
	if (enable_push_7)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_7, "ERROR_PUSH_7");	
	}
	else if (!enable_push_7)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_7_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_7, "ERROR_PUSH_7");	
	}
	
	if (enable_push_8)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_8, "ERROR_PUSH_8");	
	}
	else if (!enable_push_8)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_8_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_8, "ERROR_PUSH_8");	
	}
	
	if (enable_push_9)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_DOWN", &sensor_push_down));
		if (sensor_push_up || !sensor_push_down)
			EXCTHROW( ERROR_PUSH_9, "ERROR_PUSH_9");	
	}
	else if (!enable_push_9)
	{
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_UP", &sensor_push_up));
	EXCCHECK( me->GetSensor(me, "SENSOR_RODAGE_PUSH_9_DOWN", &sensor_push_down));
		if (sensor_push_down || !sensor_push_up)
			EXCTHROW( ERROR_PUSH_9, "ERROR_PUSH_9");	
	}
#endif   /* formerly excluded lines */
		
	/* Rodage with X cycles */
	for (i=0;i<cycle;i++)
	{
		
	/* activate pushs  down */
	if (enable_push_1)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_1", 0));
	if (enable_push_2)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_2", 0));
	if (enable_push_3)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_3", 0));
	if (enable_push_4)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_4", 0));
	if (enable_push_5)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_5", 0));
	if (enable_push_6)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_6", 0));
	if (enable_push_7)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_7", 0));
	if (enable_push_8)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_8", 0));
	if (enable_push_9)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_9", 0));
	Sleep(100);
	if (enable_push_1)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_1", 1));
	if (enable_push_2)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_2", 1));
	if (enable_push_3)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_3", 1));
	if (enable_push_4)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_4", 1));
	if (enable_push_5)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_5", 1));
	if (enable_push_6)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_6", 1));
	if (enable_push_7)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_7", 1));
	if (enable_push_8)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_8", 1));
	if (enable_push_9)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_9", 1));

	
	Sleep(timeon); // time for cylinder down  to sill 
	
	/* activate pushs up */
	if (enable_push_1)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_1", 0));
	if (enable_push_2)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_2", 0));
	if (enable_push_3)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_3", 0));
	if (enable_push_4)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_4", 0));
	if (enable_push_5)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_5", 0));
	if (enable_push_6)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_6", 0));
	if (enable_push_7)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_7", 0));
	if (enable_push_8)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_8", 0));
	if (enable_push_9)
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_9", 0));
	Sleep(100);
	if (enable_push_1)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_1", 1));
	if (enable_push_2)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_2", 1));
	if (enable_push_3)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_3", 1));
	if (enable_push_4)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_4", 1));
	if (enable_push_5)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_5", 1));
	if (enable_push_6)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_6", 1));
	if (enable_push_7)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_7", 1));
	if (enable_push_8)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_8", 1));
	if (enable_push_9)
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_9", 1));

	Sleep(timeoff); // time for cylinder up to sill 
	}
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
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_LOCKED_STATION_10, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->SetSensor(me, "CYLINDER_LOCK_STATION_10", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNLOCK_STATION_10", 0));
	
	
	if (timeout)
	{
		if(gs_locked_station_10) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
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
	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_LOCK_STATION_10", 0));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNLOCK_STATION_10", 1));
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


/* Lock Panel station 40 */
#undef __FUNC__
#define __FUNC__	"SelectVS11Station40"
static SElExceptionPtr SelectVS11Station40
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_panel_plugged_40_x52,
					is_panel_unplugged_40_x52,
					is_panel_plugged_40_vs11,
					is_panel_unplugged_40_vs11;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_VS11_SELECTED_STATION_40, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_X52", &is_panel_plugged_40_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_X52", &is_panel_unplugged_40_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_VS11", &is_panel_plugged_40_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_VS11", &is_panel_unplugged_40_vs11));
	if (microCam && !is_panel_plugged_40_x52 && is_panel_unplugged_40_x52 && !is_panel_plugged_40_vs11 && is_panel_unplugged_40_vs11)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_VS11_STATION_40", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_X52_STATION_40", 0));
	}
	
	if (timeout)
	{
		if(gs_vs11_selected_station_40) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_vs11_selected_station_40) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Unlock Panel station 40 */
#undef __FUNC__
#define __FUNC__	"SelectX52Station40"
static SElExceptionPtr SelectX52Station40
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_panel_plugged_40_x52,
					is_panel_unplugged_40_x52,
					is_panel_plugged_40_vs11,
					is_panel_unplugged_40_vs11;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_X52_SELECTED_STATION_40, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_X52", &is_panel_plugged_40_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_X52", &is_panel_unplugged_40_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_40_VS11", &is_panel_plugged_40_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_40_VS11", &is_panel_unplugged_40_vs11));
	if (microCam && !is_panel_plugged_40_x52 && is_panel_unplugged_40_x52 && !is_panel_plugged_40_vs11 && is_panel_unplugged_40_vs11)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_VS11_STATION_40", 0));
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_X52_STATION_40", 1));
	}

	if (timeout)
	{
		if(gs_x52_selected_station_40) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_x52_selected_station_40) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 40 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation40_X52"
static SElExceptionPtr PlugPanelStation40_X52
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_x52_selected,
					is_vs11_selected;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_40_X52, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_40", &is_vs11_selected));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_40", &is_x52_selected));
	if (microCam && !is_vs11_selected && is_x52_selected)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_40_X52", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40_X52", 0));
	}
	
	if (timeout)
	{
		if(gs_plugged_station_40_x52) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_40_x52) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}
#undef __FUNC__
#define __FUNC__	"PlugPanelStation40_VS11"
static SElExceptionPtr PlugPanelStation40_VS11
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_x52_selected,
					gs_plug_station_40_vs11,
					is_vs11_selected;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_40_VS11, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_40", &is_vs11_selected));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_40", &is_x52_selected));
	if (microCam && is_vs11_selected && !is_x52_selected)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_40_VS11", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40_VS11", 0));
	}
	if (timeout)
	{
		if(gs_plugged_station_40_vs11) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_40_vs11) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 40 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation40_X52"
static SElExceptionPtr UnplugPanelStation40_X52
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_40_X52, &hevent));
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
 	if (microCam)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40_X52", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_40_X52", 0));
	 }
	if (timeout)
	{
		if(gs_unplugged_station_40_x52) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_40_x52) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 40 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation40_VS11"
static SElExceptionPtr UnplugPanelStation40_VS11
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_40_VS11, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_40_VS11", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_40_VS11", 0));

	if (timeout)
	{
		if(gs_unplugged_station_40_vs11) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_40_vs11) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}



/* Lock Panel station 50 */
#undef __FUNC__
#define __FUNC__	"SelectVS11Station50"
static SElExceptionPtr SelectVS11Station50
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_panel_plugged_50_x52,
					is_panel_unplugged_50_x52,
					is_panel_plugged_50_vs11,
					is_panel_unplugged_50_vs11;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_VS11_SELECTED_STATION_50, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_X52", &is_panel_plugged_50_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_X52", &is_panel_unplugged_50_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_VS11", &is_panel_plugged_50_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_VS11", &is_panel_unplugged_50_vs11));
	if (microCam && !is_panel_plugged_50_x52 && is_panel_unplugged_50_x52 && !is_panel_plugged_50_vs11 && is_panel_unplugged_50_vs11)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_VS11_STATION_50", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_X52_STATION_50", 0));
	}
	
	if (timeout)
	{
		if(gs_vs11_selected_station_50) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_vs11_selected_station_50) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Unlock Panel station 50 */
#undef __FUNC__
#define __FUNC__	"SelectX52Station50"
static SElExceptionPtr SelectX52Station50
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_panel_plugged_50_x52,
					is_panel_unplugged_50_x52,
					is_panel_plugged_50_vs11,
					is_panel_unplugged_50_vs11;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_X52_SELECTED_STATION_50, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_X52", &is_panel_plugged_50_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_X52", &is_panel_unplugged_50_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_50_VS11", &is_panel_plugged_50_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_50_VS11", &is_panel_unplugged_50_vs11));
	if (microCam && !is_panel_plugged_50_x52 && is_panel_unplugged_50_x52 && !is_panel_plugged_50_vs11 && is_panel_unplugged_50_vs11)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_VS11_STATION_50", 0));
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_X52_STATION_50", 1));
	}

	if (timeout)
	{
		if(gs_x52_selected_station_50) goto Error;												  
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_x52_selected_station_50) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 50 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation50_X52"
static SElExceptionPtr PlugPanelStation50_X52
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_x52_selected,
					is_vs11_selected;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_50_X52, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_50", &is_vs11_selected));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_50", &is_x52_selected));
	if (microCam && !is_vs11_selected && is_x52_selected)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_50_X52", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_50_X52", 0));
	}
	
	if (timeout)
	{
		if(gs_plugged_station_50_x52) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_50_x52) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 50 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation50_VS11"
static SElExceptionPtr PlugPanelStation50_VS11
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_x52_selected,
					is_vs11_selected;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_50_VS11, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_50", &is_vs11_selected));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_50", &is_x52_selected));
	if (microCam && is_vs11_selected && !is_x52_selected)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_50_VS11", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_50_VS11", 0));
	}
	
	if (timeout)
	{
		if(gs_plugged_station_50_vs11) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_50_vs11) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Unplug Panel station 50 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation50_X52"
static SElExceptionPtr UnplugPanelStation50_X52
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_50_X52, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_50_X52", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_50_X52", 0));

	if (timeout)
	{
		if(gs_unplugged_station_50_x52) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_50_x52) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__	"UnplugPanelStation50_VS11"
static SElExceptionPtr UnplugPanelStation50_VS11
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_50_VS11, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_50_VS11", 1)); 
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_50_VS11", 0));

	if (timeout)
	{
		if(gs_unplugged_station_50_vs11) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_50_vs11) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}


/* Lock Panel station 60 */
#undef __FUNC__
#define __FUNC__	"SelectVS11Station60"
static SElExceptionPtr SelectVS11Station60
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_panel_plugged_60_x52,
					is_panel_unplugged_60_x52,
					is_panel_plugged_60_vs11,
					is_panel_unplugged_60_vs11;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_VS11_SELECTED_STATION_60, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_X52", &is_panel_plugged_60_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_X52", &is_panel_unplugged_60_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_VS11", &is_panel_plugged_60_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_VS11", &is_panel_unplugged_60_vs11));
	if (microCam && !is_panel_plugged_60_x52 && is_panel_unplugged_60_x52 && !is_panel_plugged_60_vs11 && is_panel_unplugged_60_vs11)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_VS11_STATION_60", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_X52_STATION_60", 0));
	}
	
	if (timeout)
	{
		if(gs_vs11_selected_station_60) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_vs11_selected_station_60) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Unlock Panel station 60 */
#undef __FUNC__
#define __FUNC__	"SelectX52Station60"
static SElExceptionPtr SelectX52Station60
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_panel_plugged_60_x52,
					is_panel_unplugged_60_x52,
					is_panel_plugged_60_vs11,
					is_panel_unplugged_60_vs11;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_X52_SELECTED_STATION_60, &hevent));

    EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_X52", &is_panel_plugged_60_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_X52", &is_panel_unplugged_60_x52));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_PLUGGED_STATION_60_VS11", &is_panel_plugged_60_vs11));
	EXCCHECK( me->GetSensor(me, "IS_PANEL_UNPLUGGED_STATION_60_VS11", &is_panel_unplugged_60_vs11));
	if (microCam && !is_panel_plugged_60_x52 && is_panel_unplugged_60_x52 && !is_panel_plugged_60_vs11 && is_panel_unplugged_60_vs11)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_VS11_STATION_60", 0));
	EXCCHECK( me->SetSensor(me, "CYLINDER_SELECT_X52_STATION_60", 1));
	}

	if (timeout)
	{
		if(gs_x52_selected_station_60) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_x52_selected_station_60) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 60 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation60_X52"
static SElExceptionPtr PlugPanelStation60_X52
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_x52_selected,
					is_vs11_selected;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_60_X52, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_60", &is_vs11_selected));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_60", &is_x52_selected));
	if (microCam && !is_vs11_selected && is_x52_selected)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_60_X52", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60_X52", 0));
	}
	if (timeout)
	{
		if(gs_plugged_station_60_x52) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_60_x52) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 60 */
#undef __FUNC__
#define __FUNC__	"PlugPanelStation60_VS11"
static SElExceptionPtr PlugPanelStation60_VS11
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;
	int32_t			microCam,
		            is_x52_selected,
					is_vs11_selected;
	
	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_PLUGGED_STATION_60_VS11, &hevent));

	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_VS11_STATION_60", &is_vs11_selected));
	EXCCHECK( me->GetSensor(me, "SENSOR_SELECT_X52_STATION_60", &is_x52_selected));
	if (microCam && is_vs11_selected && !is_x52_selected)
	{
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_60_VS11", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60_VS11", 0));
	}
	if (timeout)
	{
		if(gs_plugged_station_60_vs11) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_plugged_station_60_vs11) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 60 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation60_X52"
static SElExceptionPtr UnplugPanelStation60_X52
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_60_X52, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60_X52", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_60_X52", 0));

	if (timeout)
	{
		if(gs_unplugged_station_60_x52) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_60_x52) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 60 */
#undef __FUNC__
#define __FUNC__	"UnplugPanelStation60_VS11"
static SElExceptionPtr UnplugPanelStation60_VS11
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_JIG_ITEM_UNPLUGGED_STATION_60_VS11, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_UNPLUG_STATION_60_VS11", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_PLUG_STATION_60_VS11", 0));

	if (timeout)
	{
		if(gs_unplugged_station_60_vs11) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_unplugged_station_60_vs11) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*Plug Panel station 60 */
#undef __FUNC__
#define __FUNC__	"IlluminationStation60"
static SElExceptionPtr IlluminationStation60
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

/*Test Checker station 60 */
#undef __FUNC__
#define __FUNC__	"TestChecker"
static SElExceptionPtr TestChecker
(
	struct _STester* me,
	int32_t			checker,
	int32_t			*output0,
	int32_t			*output1,
	int32_t			*output2,
	int32_t			*output3


)
{
	int timeout = 1,timer,iCmpt; 
	*output0=-1;
	*output1=-1;
	*output2=-1;
	*output3=-1;
	clock_t _start_test_clock;
	double time_debug;


	
	SElExceptionPtr	pexception = NULL;
	int pass,fail;
	iCmpt=1;
	switch (checker)
	{
	case CHECKER_RIGHT : 
	//_start_test_clock = clock();
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_DROITE", 1));
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_DROITE", 1));
	Sleep(900);
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_1_DROITE", output0));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_2_DROITE", output1));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_3_DROITE", output2));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_4_DROITE", output3));
//	time_debug = (double)(clock()-_start_test_clock);
//	printf("time = %f\r\n", time_debug);  
//	printf("CHECKER_TRIGGER_DROITE => output0 = %d | output1 = %d | output2 = %d | output3 = %d\r\n\r\n", *output0,*output1,*output2,*output3);  
	break;
	
	case CHECKER_LEFT : 
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_GAUCHE", 1));
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_GAUCHE", 1));
	Sleep(900);
    EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_1_GAUCHE", output0));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_2_GAUCHE", output1));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_3_GAUCHE", output2));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_4_GAUCHE", output3));
	break;
	
	case CHECKER_CENTRAL : 
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_CENTRAL", 1));
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_CENTRAL", 1));
	Sleep(900);
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_1_CENTRAL", output0));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_2_CENTRAL", output1));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_3_CENTRAL", output2));
   	EXCCHECK( me->GetSensor(me, "CHECKER_SORTIE_3_CENTRAL", output3));
	break;
	
	break;
	}
	
	
Error:
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_DROITE", 0));
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_GAUCHE", 0));
	EXCCHECK( me->SetSensor(me, "CHECKER_TRIGGER_CENTRAL", 0));
	EXCRETHROW(pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_PickPlace"
static DWORD WINAPI Thread_PickPlace(LPVOID param)
{
	SMaskThreadDataPtr	pthread_data = (SMaskThreadDataPtr)param;
	STesterPtr		me = pthread_data->me;
	uint32_t		timeout = pthread_data->timeout;
	SElExceptionPtr	pexception = NULL;
	uint32_t		step =OPEN_PINCE;
	bool_t			is_product_took = FALSE;
	bool_t			cylinder_x_front,
					cylinder_x_rear,
					cylinder_z_up,
					cylinder_z_down,
					lock_station_80,
					unlock_station_80,
					open_pince,
					close_pince,
					hand_block,
					hand_unblock;

	TIMEOUT_INIT();
	
	while( TRUE)
	{
		if (!is_product_took)
		{
			switch  (step)
			{
			case  OPEN_PINCE: 
				   		
	  				EXCCHECK( me->GetSensor(me, "OPEN_PINCE_STATION_80", &open_pince));
	  				EXCCHECK( me->GetSensor(me, "CLOSE_PINCE_STATION_80", &close_pince));
					
				   
				   if(!open_pince && close_pince) 
							{
				   			EXCCHECK( me->SetSensor(me, "OPEN_PINCE_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CLOSE_PINCE_STATION_80", 0));
				 			}  
				   else if (open_pince && !close_pince) 
							{
						//	step++;
							step = MOVE_TO_PALETT;
						//	printf("pince open\n\r"); 
				 			} 
				   
			//	   printf("step = %d\n\r", step);
			break;
			
			
#if 0    /* formerly excluded lines */
			case  UNLOCK_PANEL: 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_LOCK_STATION_80", &lock_station_80));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_UNLOCK_STATION_80", &unlock_station_80));
					
				//	printf("Start unlock\n\r");
				   
				   if(!unlock_station_80 && lock_station_80) 
							{
							EXCCHECK( me->SetSensor(me, "CYLINDER_LOCK_STATION_80", 0));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_UNLOCK_STATION_80", 1));
				 			}  
				   else if (!lock_station_80 && unlock_station_80) 
							{
						//	step ++;
							step = MOVE_TO_PALETT;
				//			printf("product unlocked\n\r"); 
				 			} 
			break;	
#endif   /* formerly excluded lines */
				
				
				
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
							//step ++;
							step = MOVE_TO_PANEL;
						//	printf("X front\n\r"); 
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
						//	step ++;
								step = TAKE_ON_PANEL ;
					//		printf("Z down\n\r"); 
				 			} 
			break;											 
																
			case  TAKE_ON_PANEL: 
				   		
	  					
	  				EXCCHECK( me->GetSensor(me, "OPEN_PINCE_STATION_80", &open_pince));
	  				EXCCHECK( me->GetSensor(me, "CLOSE_PINCE_STATION_80", &close_pince));
				   
				   if(open_pince && !close_pince) 
							{
				   			EXCCHECK( me->SetSensor(me, "OPEN_PINCE_STATION_80", 0));
				   			EXCCHECK( me->SetSensor(me, "CLOSE_PINCE_STATION_80", 1));
				 			}  
				   else if (!open_pince && close_pince) 
							{
					//		step++;
								step = PICK_UP_PANEL;
					//		printf("pince close\n\r"); 
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
						//	step ++;
								step = MOVE_TO_BOX;	
					//		printf("Z up\n\r"); 
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
						//	step ++;
								step = PLACE_PANEL;	
						//	printf("X rear\n\r"); 
				 			} 
			break;

			case  PLACE_PANEL : 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &cylinder_z_up));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &cylinder_z_down));
				   
				   if(cylinder_z_up && !cylinder_z_down) 
							{
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 0));
				 			}  
				   else if (!cylinder_z_up && cylinder_z_down) 
							{
						//	step ++;
							step = TAKE_OFF_PANEL;
						//	printf("Z down\n\r"); 
				 			} 
			break;
			
			case  TAKE_OFF_PANEL: 
				   		
	  					
	  				EXCCHECK( me->GetSensor(me, "OPEN_PINCE_STATION_80", &open_pince));
	  				EXCCHECK( me->GetSensor(me, "CLOSE_PINCE_STATION_80", &close_pince));
				   
				   if(!open_pince && close_pince) 
							{
				   			EXCCHECK( me->SetSensor(me, "OPEN_PINCE_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CLOSE_PINCE_STATION_80", 0));
				 			}  
				   else if (open_pince && !close_pince) 
							{
						//	step++;
								step = MOVE_TO_HOME; 
					//		printf("pince open\n\r"); 
				 			}  
			break;
			
			case  MOVE_TO_HOME : 
				   		
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &cylinder_z_up));
	  				EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &cylinder_z_down));
				   
				   if(!cylinder_z_up && cylinder_z_down) 
							{
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0));
				 			}  
				    else if (cylinder_z_up && !cylinder_z_down) 
							{
						//	step ++;
							step = PICK_PLACE_FINIHED;  
						//	printf("z up\n\r"); 
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

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Tester::PickPlace"
static SElExceptionPtr PickPlace(struct _STester* me, int32_t timeout, bool_t wait_end)
{
	SElExceptionPtr	pexception = NULL;
	SMaskThreadDataPtr	pthread_data = NULL;
	bool_t				is_hand_block = FALSE,
						is_hand_unblock = FALSE,
						is_cylinder_z_up = FALSE,
						is_cylinder_z_down = FALSE,
						is_cylinder_x_rear = FALSE,
						is_cylinder_x_front = FALSE,
						is_barcode_front = FALSE,
						is_barcode_rear = FALSE,
						is_cylinder_printer_rear = FALSE,
						is_cycle_print_error = FALSE,
						is_red_box_rear = FALSE,
						is_red_box_full = FALSE,  
						is_cylinder_printer_up  = FALSE,
	                    lock_station_80= FALSE,
						unlock_station_80= FALSE;
	int32_t				microCam;
    
	int error;
		
 	  EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	  EXCCHECK( me->GetSensor(me, "SENSOR_RED_BOX_REAR", &is_red_box_rear)); 
	  EXCCHECK( me->GetSensor(me, "SENSOR_RED_BOX_FULL", &is_red_box_full));
	  EXCCHECK( me->GetSensor(me, "IS_PANEL_LOCKED_STATION_80", &lock_station_80));
	  EXCCHECK( me->GetSensor(me, "IS_PANEL_UNLOCKED_STATION_80", &unlock_station_80));
	  EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_CLOSE_STATION_80", &is_hand_block));
	  EXCCHECK( me->GetSensor(me, "SENSOR_PINCE_OPEN_STATION_80", &is_hand_unblock));
	  EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_UP_STATION_80", &is_cylinder_z_up));
	  EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_Z_DOWN_STATION_80", &is_cylinder_z_down));
	  EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_REAR_STATION_80", &is_cylinder_x_rear));
	  EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_X_FRONT_STATION_80", &is_cylinder_x_front));
 	  
	  
	  //printf("is_red_box_rear = %d\n\r", is_red_box_rear);
	  //printf("is_red_box_full = %d\n\r", is_red_box_full);

	if(microCam && unlock_station_80 &&  !lock_station_80
	   && is_cylinder_z_up && !is_cylinder_z_down
	   && is_cylinder_x_rear && !is_cylinder_x_front
	   && is_hand_unblock && !is_hand_block && is_red_box_rear && !is_red_box_full)
	{
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
	//	printf("Thread Start \n\r");
		EXCCHECK( ThreadStart(Thread_PickPlace, pthread_data, &me->hThreadPickPlace, NULL));

		if(wait_end)
		{
		EXCCHECK( me->IsProductFaultTook(me, timeout + 2000));
		}
	}
		
	else 
		{
			EXCTHROW( TESTER_ERROR_PICK_PLACE, "Erreur cycle PickPlace vérifier les capteurs suivants: \r\n"
												  "- capteur Plateau dans une position non indexé\r\n ");
		}

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_ResetPickPlace"
static DWORD WINAPI Thread_ResetPickPlace(LPVOID param)
{
	SMaskThreadDataPtr	pthread_data = (SMaskThreadDataPtr)param;
	STesterPtr		me = pthread_data->me;
	uint32_t		timeout = pthread_data->timeout;
	SElExceptionPtr	pexception = NULL;
	uint32_t		step = TAKE_OFF_PANEL;
	bool_t			is_product_took = FALSE;
	bool_t			cylinder_x_front,
					cylinder_x_rear,
					cylinder_z_up,
					cylinder_z_down,
					hand_block,
					hand_unblock;

	TIMEOUT_INIT();
	
	while( TRUE)
	{
		if (!is_product_took)   
		{
			switch  (step)
			{
			case  TAKE_OFF_PANEL: 
				   		
	  				EXCCHECK( me->GetSensor(me, "CLOSE_PINCE_STATION_80", &hand_block));
	  				EXCCHECK( me->GetSensor(me, "OPEN_PINCE_STATION_80", &hand_unblock));
				 //  if((!hand_unblock && hand_block) || (!hand_unblock && !hand_block)) 
					//		{
				   			EXCCHECK( me->SetSensor(me, "OPEN_PINCE_STATION_80", 1));
				   			EXCCHECK( me->SetSensor(me, "CLOSE_PINCE_STATION_80", 0));
				 	//		}  
				   //else
					   if (hand_unblock && !hand_block) 
							{
							step = PICK_UP_PANEL;		
				 			} 
			break;
			
			case  PICK_UP_PANEL : 
				   		
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
							step = PICK_PLACE_FINIHED;		
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
#define __FUNC__	"Tester::ResetPickPlace"
static SElExceptionPtr ResetPickPlace(struct _STester* me, int32_t timeout, bool_t wait_end)
{
	SElExceptionPtr	pexception = NULL;
	SMaskThreadDataPtr	pthread_data = NULL;
	bool_t				is_hand_block = FALSE,
						is_hand_unblock = FALSE,
						is_cylinder_z_up = FALSE,
						is_cylinder_z_down = FALSE,
						is_cylinder_x_rear = FALSE,
						is_cylinder_x_front = FALSE,
						is_barcode_front = FALSE,
						is_barcode_rear = FALSE,
						is_cylinder_printer_rear = FALSE,
						is_cycle_print_error = FALSE,
						is_cylinder_printer_up  = FALSE;
	int32_t				microCam;
	int error;


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

/*Ready Cycle print */
#undef __FUNC__
#define __FUNC__	"ReadyPrint"
static SElExceptionPtr ReadyPrint
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	static HANDLE			hevent;
	DWORD 					result;

	bool_t				printer_in_position = FALSE, 
						printer_input1 = FALSE,
						printer_input2 = FALSE,
						printer_input3 = FALSE,
						printer_input4 = FALSE,
						printer_input5 = FALSE,
						microCam;
	
	
	//wait cycle_print_finished OFF
	TIMEOUT_INIT();
	do{
		me->GetSensor(me, "SENSOR_CYLINDER_PRINTER_FRONT", &printer_input1);
		me->GetSensor(me, "IS_CYLINDER_PRINTER_REAR", &printer_input2);
		me->GetSensor(me, "IS_PRINT_CYCLE_ERROR_RIBBON", &printer_input3);
		//me->GetSensor(me, "PRINTER_INPUT4", &printer_input4);
		me->GetSensor(me, "IS_PRINT_CYCLE_ERROR", &printer_input5);
		me->GetSensor(me, "MICRO_CAM", &microCam);

	if(printer_input5==1)
	me->SetSensor(me, "PRINTER_RESET", 1);	
		
	if(!printer_input1 && 
	   printer_input2 && 
	   printer_input3 /*&& 
	   printer_input4 */&&
	   printer_input5&&
	   !microCam
	  )
		{
		/* SART = 0 */
		me->SetSensor(me, "START_PRINT", 0);
		Sleep(100);
		break;
		}
	
	else if (microCam)
	{
	
	/* Reset Errors */
	EXCCHECK( me->SetSensor(me, "PRINTER_RESET", 1));
	}

	TIMEOUT_PRINTER_CHECK(timeout);
	
	}while(TRUE);

	
Error:
	me->SetSensor(me, "PRINTER_RESET", 0);
	EXCRETHROW(pexception);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckDirectory"
void CheckDirectory(const char* directory)
{
	int					size;

	FileExists (directory, &size);
	if(size==-1)
	{
		/* create directory */	
		 MakeDir(directory);
	}
}
 
/*---------------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CyclePrintStatus"
void CyclePrintStatus(bool_t is_barcode_front,bool_t is_barcode_rear,bool_t is_cylinder_x_rear,bool_t is_cylinder_x_front, int32_t microCam)   
{
	SElExceptionPtr	pexception = NULL;
	static HANDLE			hevent;
	DWORD 					result;
	HRESULT         	hr = 0;
	CAObjHandle     	pxml = 0; 
	CAObjHandle     	pnode = 0; 
	const char*     	pfile_form_xml = "test_form.xml";
	VBOOL            	is_xml_success = 1;
	VARIANT         	pfile_name_var = CA_VariantEmpty();
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN],
						fileName[MAX_PATHNAME_LEN],
						csv_dir[MAX_PATHNAME_LEN],
						sdate_time[256],
						buffer[2048] = "";
	SYSTEMTIME			st;

	
	CAObjHandle 		pTicket;
	CAObjHandle 		pTicket_new = 0;
	char*           	ptext = NULL;
    int                 HandleFile;
	char				log[512];
	
	
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, PATH_PRODUCTION, project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	
	/* enreg des data */
	
	memset(log, 0x00, 512);
	
	sprintf (fileName, "%s/Log_Sensors.txt", result_dir);
	
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	
//	sprintf (log, "18: 30 -- hello && it is me");
//	sprintf (log, sdate_time, DATE_TIME_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf (log,"---------------------------------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\nIS_BARCODE_FRONT=%d\r\nIS_BARCODE_REAR=%d\r\nSENSOR_CYLINDER_X_REAR_STATION_80=%d\r\nSENSOR_CYLINDER_X_FRONT_STATION_80=%d\r\nMICRO_CAM=%d\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, is_barcode_front, is_barcode_rear, is_cylinder_x_rear, is_cylinder_x_front, microCam);  
//	sprintf (log, "IS_BARCODE_REAR=%d\r\n", is_barcode_rear); 
//	sprintf (log, "SENSOR_CYLINDER_X_REAR_STATION_80=", is_cylinder_x_rear); 
	
	
	WriteLine (HandleFile, log, sizeof (log));	
	
	CloseFile (HandleFile);
}

/*-------------------------------------------------------------------*/

/*Start Cycle print */
#undef __FUNC__
#define __FUNC__	"CyclePrint"
static SElExceptionPtr CyclePrint
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	static HANDLE			hevent;
	DWORD 					result;

	bool_t				is_hand_block = FALSE,
						is_hand_unblock = FALSE,
						is_cylinder_z_up = FALSE,
						is_cylinder_z_down = FALSE,
						is_cylinder_x_rear = FALSE,
						is_cylinder_x_front = FALSE,
						is_barcode_front = FALSE,
						is_barcode_rear = FALSE,
						is_cylinder_printer_rear = FALSE;
	int32_t				microCam;
	int32_t				printer_input2 = FALSE;
	int32_t				sensor_printer_sensor_rear = FALSE;
	
	
	/* check for avoid crash */
	EXCCHECK( me->GetSensor(me, "MICRO_CAM", &microCam));
	EXCCHECK( me->GetSensor(me, "SENSOR_CYLINDER_PRINTER_REAR", &sensor_printer_sensor_rear));
	//printf("SENSOR_CYLINDER_PRINTER_REAR INIT : %d  \r \n",is_cylinder_printer_rear);

	if(microCam && sensor_printer_sensor_rear)
		{ 
	   		/* START_PRINT */
			EXCCHECK( me->SetSensor(me, "START_PRINT", 1));
			Sleep(350);
			EXCCHECK( me->SetSensor(me, "START_PRINT", 0));
			//printf(" *********************** Cycle Print : Start Print ************************* \r \n");
			TIMEOUT_INIT();
			
			//wait cycle_print_finished OFF
  			do{
				me->GetSensor(me, "MICRO_CAM", &microCam);
				if(microCam == 0) EXCCHECK( me->SetSensor(me, "PRINTER_AIR_VALVE", 0));
	  			me->GetSensor(me, "SENSOR_CYLINDER_PRINTER_REAR", &sensor_printer_sensor_rear);
				me->GetSensor(me, "IS_CYLINDER_PRINTER_REAR", &is_cylinder_printer_rear);
	   			if (!sensor_printer_sensor_rear && !is_cylinder_printer_rear)
					break;
        		TIMEOUT_PRINTER_CHECK(timeout);
    		}while(TRUE);
		
			
			//wait cycle_print_finished ON
  			do{
				me->GetSensor(me, "MICRO_CAM", &microCam);
				if(microCam == 0) EXCCHECK( me->SetSensor(me, "PRINTER_AIR_VALVE", 0));
				me->GetSensor(me, "SENSOR_CYLINDER_PRINTER_REAR", &sensor_printer_sensor_rear);
	  			me->GetSensor(me, "IS_CYLINDER_PRINTER_REAR", &is_cylinder_printer_rear);				
	   			if (sensor_printer_sensor_rear && is_cylinder_printer_rear)
					break;
        		TIMEOUT_PRINTER_CHECK(timeout);
    		}while(TRUE);
		}

	else 
		{
			
			EXCTHROW( TESTER_ERROR_CYCLE_PRINTER, "Erreur cycle impression vérifier les capteurs suivants: \r\n" 
												  "- capteur Lecteure code à barre en avant\r\n" 
												  "- capteur Axe X de pick and Place en avant\r\n"
												  "- capteur Plateau dans une position non indexé\r\n ");
		}
	

Error:
	//wait sensor applicator in UP
  	do{
		me->GetSensor(me, "SENSOR_CYLINDER_PRINTER_REAR", &sensor_printer_sensor_rear);
		//printf("SENSOR_CYLINDER_PRINTER_REAR : %d \r \n",sensor_printer_sensor_rear);
		me->GetSensor(me, "IS_CYLINDER_PRINTER_REAR", &is_cylinder_printer_rear);
	   	if (sensor_printer_sensor_rear && is_cylinder_printer_rear)
			break;
    }while(TRUE);
	
	me->SetSensor(me, "START_PRINT", 0);
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
    EXCCHECK( me->GetSensor(me, "SENSOR_RED_BOX_REAR", closed));
	
Error:
	EXCRETHROW(pexception);
}
/*GetStatusPrinter */
#undef __FUNC__
#define __FUNC__	"GetStatusPrinter"
static SElExceptionPtr GetStatusPrinter
(
	struct _STester* me,
	int32_t			*finished,
	int32_t			*error
)
{
	SElExceptionPtr	pexception = NULL;
    EXCCHECK( me->GetSensor(me, "IS_PRINTER_READY", finished));
    EXCCHECK( me->GetSensor(me, "IS_PRINT_CYCLE_ERROR", error));
	
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
	int             status;
	switch (station)
	{
	case STATION_10 :
	EXCCHECK( me->UnlockPanelStation10(me, timeout));	  
		
		
	case STATION_20 :
	EXCCHECK( me->StopCheckClipping(me, timeout)); 	
	EXCCHECK( me->StopClipping_punzoni(me, timeout));
	EXCCHECK( me->StopClipping_contrasti(me, timeout)); 
	

	break;
	
	case STATION_30 :

	//EXCCHECK( me->StopRodage(me, timeout));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_7", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_8", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_8", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_9", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_9", 0));
		
	break;

	case STATION_40 :
	EXCCHECK( me->UnplugPanelStation40_X52(me, timeout));
	EXCCHECK( me->UnplugPanelStation40_VS11(me, timeout)); 
	EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, 2, &paxis));
	EXCCHECK( paxis->PresentAlarm(paxis, AXIS_Z, &status));
	if (status & 0X0080 == 128)
		{
			EXCCHECK( paxis->StopAxis(paxis, AXIS_Z));
			Sleep(100);
			EXCCHECK( paxis->AlarmReset(paxis, AXIS_Z));
			Sleep(100);
			EXCCHECK( paxis->EnableAxis(paxis, AXIS_Z));
			Sleep(100);
			EXCCHECK( paxis->HomeReturn(paxis, AXIS_Z));
			Sleep(1500);
		}
	else 
	{
	EXCCHECK( paxis->EnableAxis(paxis, AXIS_Z));
	EXCCHECK( paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0));
	EXCCHECK( paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0));
	}
	break;

	case STATION_50 :
	EXCCHECK( me->UnplugPanelStation50_X52(me, timeout));
	EXCCHECK( me->UnplugPanelStation50_VS11(me, timeout));
	EXCCHECK( me->axissAPI->GetaxisByIndex(me->axissAPI, 3, &paxis));
	EXCCHECK( paxis->PresentAlarm(paxis, AXIS_Z, &status));
	if (status & 0X0080 == 128)
		{
			//printf ("PresentAlarm = (0X%04X)h\r\n", status);
			EXCCHECK( paxis->StopAxis(paxis, AXIS_Z));
			Sleep(100);
			EXCCHECK( paxis->AlarmReset(paxis, AXIS_Z));
			Sleep(100);
			EXCCHECK( paxis->EnableAxis(paxis, AXIS_Z));
			Sleep(100);
			EXCCHECK( paxis->HomeReturn(paxis, AXIS_Z));
			Sleep(1500);
		}
	else 
	{
	EXCCHECK( paxis->EnableAxis(paxis, AXIS_Z));
	EXCCHECK( paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0));
	EXCCHECK( paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0));
	}
	break;

	case STATION_60 :
	
	EXCCHECK( me->UnplugPanelStation60_X52(me, timeout));
	EXCCHECK( me->UnplugPanelStation60_VS11(me, timeout)); 
	EXCCHECK( me->IlluminationStation60(me, 0));
	break;
	
	case STATION_70 :
	

	break;


	case STATION_80 :
	EXCCHECK( me->ResetPickPlace(me, timeout, TRUE));
	Sleep(500);
	EXCCHECK( me->LockPanelStation80(me, 2000));
	break;
	}
	
Error:
	EXCDISPLAY(pexception);
	return 0;
}

/* Initialize Stations */
#undef __FUNC__
#define __FUNC__	"ResetStations"
static SElExceptionPtr ResetStations
(							
	struct _STester* me,
	int32_t			station,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	HANDLE			hevent = NULL;
	
	switch (station)
	{
	case STATION_10 :
		
	//EXCCHECK( me->UnlockPanelStation10(me, timeout));
	
	case STATION_20 :
		
	EXCCHECK( me->StopCheckClipping(me, timeout));
	EXCCHECK( me->StopClipping_punzoni(me, timeout));
	EXCCHECK( me->StopClipping_contrasti(me, timeout)); 

	break;
	
	case STATION_30 :

	//EXCCHECK( me->StopRodage(me, timeout));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_1", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_1", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_2", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_2", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_3", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_3", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_4", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_4", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_5", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_5", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_6", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_6", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_7", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_7", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_8", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_8", 0));
	EXCCHECK( me->SetSensor(me, "RODAGE_UP_PUSH_9", 1));
	EXCCHECK( me->SetSensor(me, "RODAGE_DOWN_PUSH_9", 0));
		
	break;

	case STATION_40 :
		
	EXCCHECK( me->UnplugPanelStation40_X52(me, timeout));
	EXCCHECK( me->UnplugPanelStation40_VS11(me, timeout)); 

	break;

	case STATION_50 :
		
	EXCCHECK( me->UnplugPanelStation50_X52(me, timeout));
	EXCCHECK( me->UnplugPanelStation50_VS11(me, timeout)); 
	
	break;
	

	case STATION_70 :
	
	break;


	case STATION_60 :
	
	EXCCHECK( me->UnplugPanelStation60_X52(me, timeout));
	EXCCHECK( me->UnplugPanelStation60_VS11(me, timeout)); 
	EXCCHECK( me->IlluminationStation60(me, 0));
	break;

	case STATION_80 :
	EXCCHECK( me->ResetPickPlace(me, timeout, TRUE));  //MA
	Sleep(500);
	EXCCHECK( me->LockPanelStation80(me, 2000));
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

/* Get bar code indentification of panel */
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
		EXCCHECK( (last_bcr->reader)->GetBarCode(last_bcr->reader, buffer, bufferSize));
		Sleep(100);
		EXCCHECK( (last_bcr->reader)->StopReading(last_bcr->reader));
	}

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
	EXCCHECK( me->SetSensor(me, "CYLINDER_LOCK_STATION_80", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_UNLOCK_STATION_80", 0));
	
	
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

/* Move X front station 80 */
#undef __FUNC__
#define __FUNC__	"MoveXFront"
static SElExceptionPtr MoveXFront
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_AXIS_X_FRONT_STATION_80, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_X_FRONT_STATION_80", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_X_REAR_STATION_80", 0));

	if (timeout)
	{
		if(gs_axis_x_front) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_axis_x_front) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Move X rear station 80 */
#undef __FUNC__
#define __FUNC__	"MoveXRear"
static SElExceptionPtr MoveXRear
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_AXIS_X_REAR_STATION_80, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_X_FRONT_STATION_80", 0));
	EXCCHECK( me->SetSensor(me, "CYLINDER_X_REAR_STATION_80", 1));

	if (timeout)
	{
		if(gs_axis_x_rear) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_axis_x_rear) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Move X rear station 80 */
#undef __FUNC__
#define __FUNC__	"MoveZUp"
static SElExceptionPtr MoveZUp
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_AXIS_Z_UP_STATION_80, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 1));
	EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 0));

	if (timeout)
	{
		if(gs_axis_z_up) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_axis_z_up) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}


/* Move X rear station 80 */
#undef __FUNC__
#define __FUNC__	"MoveZDown"
static SElExceptionPtr MoveZDown
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_AXIS_Z_DOWN_STATION_80, &hevent));

	EXCCHECK( me->SetSensor(me, "CYLINDER_Z_UP_STATION_80", 0));
	EXCCHECK( me->SetSensor(me, "CYLINDER_Z_DOWN_STATION_80", 1));

	if (timeout)
	{
		if(gs_axis_z_down) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_axis_z_down) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Move X rear station 80 */
#undef __FUNC__
#define __FUNC__	"OpenPince"
static SElExceptionPtr OpenPince
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_PINCE_OPEN_STATION_80, &hevent));

	EXCCHECK( me->SetSensor(me, "OPEN_PINCE_STATION_80", 1));
	EXCCHECK( me->SetSensor(me, "CLOSE_PINCE_STATION_80", 0));

	if (timeout)
	{
		if(gs_pince_open) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_pince_open) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/* Move X rear station 80 */
#undef __FUNC__
#define __FUNC__	"ClosePince"
static SElExceptionPtr ClosePince
(
	struct _STester* me,
	int32_t			timeout
)
{
	SElExceptionPtr	pexception = NULL;
	HANDLE			hevent = NULL;

	EXCCHECK( me->eventAPI->GetEvent(me->eventAPI, EVNT_PINCE_CLOSE_STATION_80, &hevent));

	EXCCHECK( me->SetSensor(me, "OPEN_PINCE_STATION_80", 0));
	EXCCHECK( me->SetSensor(me, "CLOSE_PINCE_STATION_80", 1));

	if (timeout)
	{
		if(gs_pince_close) goto Error;
		if (WaitForSingleObject( hevent, timeout) == WAIT_TIMEOUT)
		{
			if(gs_pince_close) goto Error;
			EXCTHROW( TESTER_ERROR_TIMEOUT, "Function Timeouted");
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*GetStatusRedBox */
#undef __FUNC__
#define __FUNC__	"DetectProductPresence"
static SElExceptionPtr DetectProductPresence																		
(
	struct _STester* me,
	int32_t			*left,
	int32_t			*right
)
{
	SElExceptionPtr	pexception = NULL;
	
    EXCCHECK( me->GetSensor(me, "LASER_PRESENCE_RIGHT", right));
    EXCCHECK( me->GetSensor(me, "LASER_PRESENCE_LEFT", left));
	
Error:
	EXCRETHROW(pexception);
}
#if 0    /* formerly excluded lines */

#endif   /* formerly excluded lines */



