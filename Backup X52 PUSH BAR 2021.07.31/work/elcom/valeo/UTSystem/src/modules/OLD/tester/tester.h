#if !defined(__TESTER_H__)
#define __TESTER_H__

#include <eltypes.h>
#include <exception/Exception.h>
#include <windows.h>
#include <sensors/sensors.h>
#include <axis/class/axis.h>
#include <profilometer/class/profilometers.h>
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
#define EVNT_JIG_ITEM_INSERTED				"EVNT_JIG_ITEM_INSERTED"
#define EVNT_JIG_ITEM_RELEASED				"EVNT_JIG_ITEM_RELEASED"
#define EVNT_JIG_ITEM_CLIPPED_PUNZONI		"EVNT_JIG_ITEM_CLIPPED_PUNZONI"
#define EVNT_JIG_ITEM_STOP_CLIPPED_PUNZONI	"EVNT_JIG_ITEM_STOP_CLIPPED_PUNZONI"
#define EVNT_JIG_ITEM_CLIPPED_CONTRASTI		"EVNT_JIG_ITEM_CLIPPED_CONTRASTI"
#define EVNT_JIG_ITEM_STOP_CLIPPED_CONTRASTI"EVNT_JIG_ITEM_STOP_CLIPPED_CONTRASTI"
#define EVNT_JIG_ITEM_CHECKED				"EVNT_JIG_ITEM_CHECKED"
#define EVNT_JIG_ITEM_STOP_CHECKED			"EVNT_JIG_ITEM_STOP_CHECKED"
#define EVNT_JIG_ITEM_START_RODAGE			"EVNT_JIG_ITEM_START_RODAGE"
#define EVNT_JIG_ITEM_STOP_RODAGE			"EVNT_JIG_ITEM_STOP_RODAGE"
#define EVNT_JIG_ITEM_LOCKED_STATION_10		"EVNT_JIG_ITEM_LOCKED_STATION_10"
#define EVNT_JIG_ITEM_UNLOCKED_STATION_10	"EVNT_JIG_ITEM_UNLOCKED_STATION_10"
#define EVNT_JIG_ITEM_VS11_SELECTED_STATION_40		"EVNT_JIG_ITEM_VS11_SELECTED_STATION_40"
#define EVNT_JIG_ITEM_X52_SELECTED_STATION_40	"EVNT_JIG_ITEM_X52_SELECTED_STATION_40"
#define EVNT_JIG_ITEM_PLUGGED_STATION_40_X52	"EVNT_JIG_ITEM_PLUGGED_STATION_40_X52"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_40_X52	"EVNT_JIG_ITEM_UNPLUGGED_STATION_40_X52"
#define EVNT_JIG_ITEM_PLUGGED_STATION_40_VS11	"EVNT_JIG_ITEM_PLUGGED_STATION_40_VS11"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_40_VS11	"EVNT_JIG_ITEM_UNPLUGGED_STATION_40_VS11"
#define EVNT_JIG_ITEM_VS11_SELECTED_STATION_50		"EVNT_JIG_ITEM_VS11_SELECTED_STATION_50"
#define EVNT_JIG_ITEM_X52_SELECTED_STATION_50	"EVNT_JIG_ITEM_X52_SELECTED_STATION_50"
#define EVNT_JIG_ITEM_PLUGGED_STATION_50_X52	"EVNT_JIG_ITEM_PLUGGED_STATION_50_X52"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_50_X52	"EVNT_JIG_ITEM_UNPLUGGED_STATION_50_X52"
#define EVNT_JIG_ITEM_PLUGGED_STATION_50_VS11	"EVNT_JIG_ITEM_PLUGGED_STATION_50_VS11"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_50_VS11	"EVNT_JIG_ITEM_UNPLUGGED_STATION_50_VS11"
#define EVNT_JIG_ITEM_VS11_SELECTED_STATION_60		"EVNT_JIG_ITEM_VS11_SELECTED_STATION_60"
#define EVNT_JIG_ITEM_X52_SELECTED_STATION_60	"EVNT_JIG_ITEM_X52_SELECTED_STATION_60"
#define EVNT_JIG_ITEM_PLUGGED_STATION_60_X52	"EVNT_JIG_ITEM_PLUGGED_STATION_60_X52"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_60_X52	"EVNT_JIG_ITEM_UNPLUGGED_STATION_60_X52"
#define EVNT_JIG_ITEM_PLUGGED_STATION_60_VS11	"EVNT_JIG_ITEM_PLUGGED_STATION_60_VS11"
#define EVNT_JIG_ITEM_UNPLUGGED_STATION_60_VS11	"EVNT_JIG_ITEM_UNPLUGGED_STATION_60_VS11"
#define EVNT_JIG_ITEM_LOCKED_STATION_80	"EVNT_JIG_ITEM_LOCKED_STATION_80"
#define EVNT_JIG_ITEM_UNLOCKED_STATION_80	"EVNT_JIG_ITEM_UNLOCKED_STATION_80"
#define EVNT_AXIS_X_FRONT_STATION_80	    "EVNT_AXIS_X_FRONT_STATION_80"
#define EVNT_AXIS_X_REAR_STATION_80	        "EVNT_AXIS_X_REAR_STATION_80" 
#define EVNT_AXIS_Z_UP_STATION_80	        "EVNT_AXIS_Z_UP_STATION_80"
#define EVNT_AXIS_Z_DOWN_STATION_80	        "EVNT_AXIS_Z_DOWN_STATION_80" 
#define EVNT_PINCE_OPEN_STATION_80	        "EVNT_PINCE_OPEN_STATION_80"
#define EVNT_PINCE_CLOSE_STATION_80	        "EVNT_PINCE_CLOSE_STATION_80" 
#define EVNT_BARCODE_FRONT					"EVNT_BARCODE_FRONT"
#define EVNT_BARCODE_REAR					"EVNT_BARCODE_REAR"
		
#define EVNT_AXIS_X_START_STATION_40		"EVNT_AXIS_X_START_STATION_40"
#define EVNT_AXIS_X_STOP_STATION_40			"EVNT_AXIS_X_STOP_STATION_40"
#define EVNT_AXIS_Y_START_STATION_40		"EVNT_AXIS_Y_START_STATION_40"
#define EVNT_AXIS_Y_STOP_STATION_40			"EVNT_AXIS_Y_STOP_STATION_40"
#define EVNT_AXIS_Z_START_STATION_40		"EVNT_AXIS_Z_START_STATION_40"
#define EVNT_AXIS_Z_STOP_STATION_40			"EVNT_AXIS_Z_STOP_STATION_40"
#define EVNT_AXIS_X_START_STATION_50		"EVNT_AXIS_X_START_STATION_50"
#define EVNT_AXIS_X_STOP_STATION_50			"EVNT_AXIS_X_STOP_STATION_50"
#define EVNT_AXIS_Y_START_STATION_50		"EVNT_AXIS_Y_START_STATION_50"
#define EVNT_AXIS_Y_STOP_STATION_50			"EVNT_AXIS_Y_STOP_STATION_50"
#define EVNT_AXIS_Z_START_STATION_50		"EVNT_AXIS_Z_START_STATION_50"
#define EVNT_AXIS_Z_STOP_STATION_50			"EVNT_AXIS_Z_STOP_STATION_50"
#define EVNT_RED_BOX_CLOSED					"EVNT_RED_BOX_CLOSED"
#define EVNT_RED_BOX_EMPTY					"EVNT_RED_BOX_EMPTY"
#define EVNT_PRINTER_READY					"EVNT_PRINTER_READY"
#define EVNT_CYCLE_PRINT_FINISHED			"EVNT_CYCLE_PRINT_FINISHED"
#define EVNT_JIG_CYCLE_START				"EVNT_JIG_CYCLE_START"
#define EVNT_JIG_START						"EVNT_JIG_START"
		
		
		
		
typedef struct _SBCR
{
	SBarCodeReaderPtr	reader;
	SElExceptionPtr		(*bcr_new)( SBarCodeReaderPtr* pBcr);
	SElExceptionPtr		(*bcr_delete)( SBarCodeReaderPtr* pBcr);
	HANDLE				dllHandle;
} SBCR, *SBCRPtr;
		
typedef struct _STester
{
	SElExceptionPtr (*UnlockPanelStation10)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*Initialize)(struct _STester*	me, const char*	fileName);
	SElExceptionPtr (*RegisterSensorCallbacks)( struct _STester* me);
	SElExceptionPtr (*GetStationStatus)(struct _STester* me, char status[9]);
	SElExceptionPtr (*IsTableRotated)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*Clipping_punzoni)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StopClipping_punzoni)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*Clipping_contrasti)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StopClipping_contrasti)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*CheckClipping)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*StopCheckClipping)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*SelectRodageX52)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*SelectRodageVS11)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*Rodage)(struct _STester* me, int32_t timeout, int32_t cycle, int32_t timeon, int32_t timeoff,	char version[8+1]);
	SElExceptionPtr (*SelectVS11Station40)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*SelectX52Station40)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*PlugPanelStation40_X52)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation40_X52)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*PlugPanelStation40_VS11)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation40_VS11)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*SelectVS11Station50)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*SelectX52Station50)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*PlugPanelStation50_X52)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation50_X52)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*PlugPanelStation50_VS11)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation50_VS11)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*SelectVS11Station60)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*SelectX52Station60)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*PlugPanelStation60_X52)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation60_X52)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*PlugPanelStation60_VS11)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnplugPanelStation60_VS11)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*IlluminationStation60)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*TestChecker)(struct _STester* me, int32_t	checker, int32_t *output0, int32_t *output1, int32_t *output2, int32_t *output3); 
	SElExceptionPtr (*ReadyPrint)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*CyclePrint)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*Activate_Air_Printer)(struct _STester* me);
	SElExceptionPtr (*Disactivate_Air_Printer)(struct _STester* me);
	SElExceptionPtr (*GetStatusRedBox)(struct _STester* me, int32_t	*full, int32_t	*closed);
	SElExceptionPtr (*DetectProductPresence)(struct _STester* me, int32_t	*right, int32_t	*left);  
	SElExceptionPtr (*GetStatusPrinter)(struct _STester* me, int32_t	*finished, int32_t	*error);
	SElExceptionPtr (*WaitRotationFinished)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*TableRotate)(struct _STester* me, int32_t timeout, bool_t wait_end);
	SElExceptionPtr (*PickPlace)(struct _STester* me, int32_t timeout, bool_t wait_end);
	SElExceptionPtr (*IsProductFaultTook)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*ResetPickPlace)(struct _STester* me, int32_t timeout, bool_t wait_end);
	SElExceptionPtr (*IsResetPickPlace)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*OkNokCheck)(struct _STester* me, int32_t timeout, bool_t* ok);
	SElExceptionPtr (*NokCheck)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*TesterBeep)(struct _STester* me, int32_t time);
	SElExceptionPtr (*InitializeStations)(struct _STester* me, int32_t station, int32_t timeout);
	SElExceptionPtr (*ResetStations)(struct _STester* me, int32_t station, int32_t timeout);
	SElExceptionPtr (*SetSensor)(struct _STester* me, const char* sensorId, uint32_t value);
	SElExceptionPtr (*GetSensor)(struct _STester* me, const char* sensorId, uint32_t* value);
	SElExceptionPtr (*SetSensorCallback)(struct _STester* me, const char* sensorId, SensorCallbackFnc_t fCall, void* pData);
	
	SElExceptionPtr (*GetItemCode)(struct _STester* me, const char* product, int32_t index, long bufferSize, char* buffer,int32_t timeout);
	
	SElExceptionPtr (*MoveXFront)(struct _STester* me, int32_t timeout); 
	SElExceptionPtr (*MoveXRear)(struct _STester* me, int32_t timeout); 
	SElExceptionPtr (*MoveZUp)(struct _STester* me, int32_t timeout); 
	SElExceptionPtr (*MoveZDown)(struct _STester* me, int32_t timeout); 
	SElExceptionPtr (*OpenPince)(struct _STester* me, int32_t timeout); 
	SElExceptionPtr (*ClosePince)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*LockPanelStation80)(struct _STester* me, int32_t timeout);
	SElExceptionPtr (*UnlockPanelStation80)(struct _STester* me, int32_t timeout); 
	SElExceptionPtr (*LockPanelStation10)(struct _STester* me, int32_t timeout);
	 
	
	
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
	HANDLE				hThreadMaskOpen;
	HANDLE				hThreadBeep;
	HANDLE				hThreadStation1;
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
SElExceptionPtr checkAllAxis (struct _STester* me);
SElExceptionPtr JigThread(struct _STester* me);
SElExceptionPtr TableThread(struct _STester* me);

#define TESTER_ERROR_INVALID_PARAMETER		-10001
#define TESTER_ERROR_TIMEOUT				-10002
#define TESTER_ERROR_STILL_ACTIVE			-10003
#define TESTER_ERROR_WAIT_TIMEOUT			-10004
#define TESTER_ERROR_LOAD_LIBRARY			-10005
#define TESTER_ERROR_PICK_PLACE				-10006
#define TESTER_ERROR_CYCLE_PRINTER			-10007
#define TESTER_ERROR_BARCODE				-10008


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
