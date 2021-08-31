#if !defined(__TESTER_H__)
#define __TESTER_H__

#include <eltypes.h>
#include <exception/Exception.h>
#include <windows.h>
#include <sensors/sensors.h>
#include <axis/class/axis.h>
#include <eventfactory/EventFactory.h>
#include <tester/barcodereader/BarCodeReader.h>

#if defined(__cplusplus) || defined(__cplusplus__)
	extern "C" {
#endif

#define EVNT_TESTER_BUTTON_PRESSED_OK		"EVNT_TESTER_BUTTON_PRESSED_OK"
#define EVNT_TESTER_BUTTON_PRESSED_NOK		"EVNT_TESTER_BUTTON_PRESSED_NOK"
#define EVNT_TESTER_BUTTON_PRESSED_MENU		"EVNT_TESTER_BUTTON_PRESSED_MENU"
#define EVNT_TESTER_BUTTON_PRESSED_LABEL	"EVNT_TESTER_BUTTON_PRESSED_LABEL"
#define EVNT_TESTER_BUTTON_PRESSED_DOWNTIME	"EVNT_TESTER_BUTTON_PRESSED_DOWNTIME"
#define EVNT_CONTINUE						"EVNT_CONTINUE"
#define EVNT_TESTER_ROTATION_FINISHED		"EVNT_TESTER_ROTATION_FINISHED"
#define EVNT_TESTER_CONTINUE_ROTATION		"EVNT_TESTER_CONTINUE_ROTATION"
#define EVNT_JIG_CYCLE_START				"EVNT_JIG_CYCLE_START"
#define EVNT_JIG_NEW						"EVNT_JIG_NEW"

#define EVNT_JIG_ITEM_INSERTED						"EVNT_JIG_ITEM_INSERTED"
#define EVNT_JIG_ITEM_RELEASED						"EVNT_JIG_ITEM_RELEASED"
#define EVNT_JIG_ITEM_LOCKED_STATION_10				"EVNT_JIG_ITEM_LOCKED_STATION_10"
#define EVNT_JIG_ITEM_UNLOCKED_STATION_10			"EVNT_JIG_ITEM_UNLOCKED_STATION_10"
		
#define EVNT_JIG_ITEM_START_RODAGE_STATION_20 		"EVNT_JIG_ITEM_START_RODAGE_STATION_20"
#define EVNT_JIG_ITEM_STOP_RODAGE_STATION_20  		"EVNT_JIG_ITEM_STOP_RODAGE_STATION_20"
		
#define EVNT_JIG_ITEM_PLUGGED_STATION_30			"EVNT_JIG_ITEM_PLUGGED_STATION_30"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_30			"EVNT_JIG_ITEM_UNPLUGGED_STATION_30"
		
#define EVNT_JIG_ITEM_PLUGGED_STATION_40			"EVNT_JIG_ITEM_PLUGGED_STATION_40"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_40			"EVNT_JIG_ITEM_UNPLUGGED_STATION_40"
		
#define EVNT_JIG_ITEM_START_RODAGE_LEFT_STATION_50  "EVNT_JIG_ITEM_START_RODAGE_LEFT_STATION_50"
#define EVNT_JIG_ITEM_STOP_RODAGE_LEFT_STATION_50   "EVNT_JIG_ITEM_STOP_RODAGE_LEFT_STATION_50"
#define EVNT_JIG_ITEM_START_RODAGE_RIGHT_STATION_50 "EVNT_JIG_ITEM_START_RODAGE_RIGHT_STATION_50"
#define EVNT_JIG_ITEM_STOP_RODAGE_RIGHT_STATION_50  "EVNT_JIG_ITEM_STOP_RODAGE_RIGHT_STATION_50"

#define EVNT_JIG_ITEM_PLUGGED_STATION_60			"EVNT_JIG_ITEM_PLUGGED_STATION_60"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_60			"EVNT_JIG_ITEM_UNPLUGGED_STATION_60"
#define EVNT_JIG_ITEM_START_TEST_LEFT_STATION_60  	"EVNT_JIG_ITEM_START_TEST_LEFT_STATION_60"
#define EVNT_JIG_ITEM_STOP_TEST_LEFT_STATION_60   	"EVNT_JIG_ITEM_STOP_TEST_LEFT_STATION_60"
#define EVNT_JIG_ITEM_START_TEST_RIGHT_STATION_60 	"EVNT_JIG_ITEM_START_TEST_RIGHT_STATION_60"
#define EVNT_JIG_ITEM_STOP_TEST_RIGHT_STATION_60  	"EVNT_JIG_ITEM_STOP_TEST_RIGHT_STATION_60"


#define EVNT_JIG_ITEM_PLUGGED_STATION_70			"EVNT_JIG_ITEM_PLUGGED_STATION_70"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_70			"EVNT_JIG_ITEM_UNPLUGGED_STATION_70"
		
#define EVNT_JIG_ITEM_LOCKED_STATION_80				"EVNT_JIG_ITEM_LOCKED_STATION_80"
#define EVNT_JIG_ITEM_UNLOCKED_STATION_80			"EVNT_JIG_ITEM_UNLOCKED_STATION_80"
#define EVNT_BARCODE_FRONT							"EVNT_BARCODE_FRONT"
#define EVNT_BARCODE_REAR							"EVNT_BARCODE_REAR"
#define EVNT_LASER_CYCLE_END						"EVNT_LASER_CYCLE_END"
#define EVNT_RED_BOX_EMPTY							"EVNT_RED_BOX_EMPTY"
#define EVNT_JIG_START								"EVNT_JIG_START"
		
		
		
typedef struct _SBCR
{
	SBarCodeReaderPtr	reader;
	SElExceptionPtr		(*bcr_new)( SBarCodeReaderPtr* pBcr);
	SElExceptionPtr		(*bcr_delete)( SBarCodeReaderPtr* pBcr);
	HANDLE				dllHandle;
} SBCR, *SBCRPtr;
		
typedef struct _STester
{
	SElExceptionPtr (*Initialize)(struct _STester*	me, const char*	fileName);
	SElExceptionPtr (*RegisterSensorCallbacks)( struct _STester* me);
	
	SElExceptionPtr (*LockPanelStation10)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnlockPanelStation10)(struct _STester* me, int32_t timeout);
	
	SElExceptionPtr (*StartRodageStation20)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StopRodageStation20)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StatusPushsStation20)(struct _STester* me, char status[7+1]);
	SElExceptionPtr (*RodageStation20)(struct _STester* me, int32_t timeout, int32_t cycle, int32_t timeon, int32_t timeoff,char version[15+1]);

	SElExceptionPtr (*PlugPanelStation30)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation30)(struct _STester* me, int32_t timeout);
	
	SElExceptionPtr (*PlugPanelStation40)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation40)(struct _STester* me, int32_t timeout);
	
	SElExceptionPtr (*StartRodageLeftStation50)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StopRodageLeftStation50)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StartRodageRightStation50)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StopRodageRightStation50)(struct _STester* me, int32_t timeout);
	
	SElExceptionPtr (*PlugPanelStation60)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation60)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StartTestLeftStation60)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StopTestLeftStation60)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StartTestRightStation60)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StopTestRightStation60)(struct _STester* me, int32_t timeout);
	
	SElExceptionPtr (*PlugPanelStation70)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation70)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*IlluminationStation70)(struct _STester* me, int32_t timeout);
	
	SElExceptionPtr (*LockPanelStation80)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnlockPanelStation80)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*MoveBarcodeFront)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*MoveBarcodeRear)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*GetStatusRedBox)(struct _STester* me, int32_t	*full, int32_t	*closed);
	SElExceptionPtr (*PickPlace)(struct _STester* me, int32_t timeout, bool_t wait_end);
	SElExceptionPtr (*IsProductFaultTook)(struct _STester* me, int32_t timeout);
   	SElExceptionPtr (*ResetPickPlace)(struct _STester* me, int32_t timeout, bool_t wait_end);
	SElExceptionPtr (*IsResetPickPlace)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*CycleLaser)(struct _STester* me, int32_t timeout);
	
	SElExceptionPtr (*TableRotate)(struct _STester* me, int32_t timeout, bool_t wait_end);
	SElExceptionPtr (*IsTableRotated)(struct _STester* me, int32_t timeout);SElExceptionPtr (*GetStationStatus)(struct _STester* me, char status[9]);
	SElExceptionPtr (*WaitRotationFinished)(struct _STester* me, int32_t timeout);

	SElExceptionPtr (*OkNokCheck)(struct _STester* me, int32_t timeout, bool_t* ok);
	SElExceptionPtr (*NokCheck)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*InitializeStations)(struct _STester* me, int32_t station, int32_t timeout);

	SElExceptionPtr (*SetSensor)(struct _STester* me, const char* sensorId, uint32_t value);
	SElExceptionPtr (*GetSensor)(struct _STester* me, const char* sensorId, uint32_t* value);
	SElExceptionPtr (*SetSensorCallback)(struct _STester* me, const char* sensorId, SensorCallbackFnc_t fCall, void* pData);
	SElExceptionPtr (*GetItemCode)(struct _STester* me, const char* product, int32_t index, long bufferSize, char* buffer,int32_t timeout);
	
//#if 0    /* formerly excluded lines */
	SElExceptionPtr (*Activate_Axis)(struct _STester* me);
	SElExceptionPtr (*Disactivate_Axis)(struct _STester* me);
//#endif   /* formerly excluded lines */
	
/*******************************************
 * PRIVATE
 *******************************************/
	SDIOSensorsPtr		sensorAPI;
	SaxisPtr			paxis;
	SaxissPtr			axissAPI;
	SElEventFactoryPtr	eventAPI;
	void*				pLogicalNames;
	int32_t				LogicalCounts;
	HANDLE				hThreadTableRotate;
	HANDLE				hThreadPickPlace;
	char				path[512]; /* xml config path */
	HANDLE				hThread;
	HANDLE				hEventStop;
	void*				LogApi;
	uint32_t			LogChannel;
	SBCRPtr				pBarCodeReader;
	int32_t				BCRCounts;
	
} STester, *STesterPtr;

SElExceptionPtr	tester_new(STesterPtr* pTester);
SElExceptionPtr	tester_delete(STesterPtr* pTester);
SElExceptionPtr JigThread(struct _STester* me);
SElExceptionPtr TableThread(struct _STester* me);

#define TESTER_ERROR_INVALID_PARAMETER		-10001
#define TESTER_ERROR_TIMEOUT				-10002
#define TESTER_ERROR_STILL_ACTIVE			-10003
#define TESTER_ERROR_WAIT_TIMEOUT			-10004
#define TESTER_ERROR_LOAD_LIBRARY			-10005
#define TESTER_ERROR_PICK_PLACE				-10006
#define TESTER_LASER_ERROR					-10007




#define ERROR_PUSH_1						-10010
#define ERROR_PUSH_2						-10011
#define ERROR_PUSH_3						-10012
#define ERROR_PUSH_4						-10013
#define ERROR_PUSH_5						-10014
#define ERROR_PUSH_6						-10015
#define ERROR_PUSH_7						-10016
#define ERROR_PUSH_8						-10017


#if defined(__cplusplus) || defined(__cplusplus__)
	}
#endif

#endif /* __TESTER_H__*/ 
