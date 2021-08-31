#if !defined(__EL_TEST_H__)
#define __EL_TEST_H__

#include <windows.h>
#include <exception/Exception.h>
#include <time.h>
#include <elstring.h>
#include "msxmldom.h" 

#ifndef EXCLUDE_TEST_MAKROS
#include <guid.h>
#include <objlist/objlist.h> 
#include <vline/vline.h>
#include <tester/tester.h>
#include <Camera/class/cameras.h>
#include <can/can.h>
#include <counter/counter.h>
#include <ivi.h>
#include <iviswtch.h>
#include <IviDCPwr.h>
#include <IviDmm.h>
#include <ivibridge/IviBridge.h>
#include <resistor/class/Resistor.h>
#include <eldaq/class/eldaq.h>
#include <printer/class/printers.h>
#include <motor/class/motor.h>
#include <labels/tester_labels.h>
#include <dbs/dbs.h>
#endif

#define		MAX_LENGTH_CSV	   1024*5

#if defined(WINDOWS)
	#define TEST_API		__declspec( dllexport )
#elif defined(UNIX)
	#define TEST_API		extern "C"
#else
	#define TEST_API
#endif

#ifdef __cplusplus
    extern "C" {
#endif 

typedef SElExceptionPtr (_FUNCC *TestFce_t)(void *TestPtr);

typedef struct _STestSeq {
    const char*     InitName;
    const char*     CleanupName;
    TestFce_t       Init;   /*!< Function which initialize test */
    TestFce_t       Cleanup;/*!< Function which cleanups test */
} STestSeq, *STestSeqPtr;

typedef struct _STestListItem {
    const char*     name;   /*!< Test name */
    const char*     HelpName;
    const char*     CheckName;
    const char*     InitName;
    const char*     RunName;
    const char*     CleanupName;
    TestFce_t         Help;   /*!< Function whitch returns description of the test */
    TestFce_t         Check;  /*!< Function which checks the all test resources */
    TestFce_t         Init;   /*!< Function which initialize test */
    TestFce_t         Run;    /*!< function which launch test */
    TestFce_t         Cleanup;/*!< Function which cleanups test */
} STestListItem, *STestListItemPtr;

typedef struct _STestParam {
	uint32_t			sequence;
	uint32_t			step;
	CAObjHandle			pTicket;
	int					pTicket_CSV;
	int                 RnR_counter;
	char				csv_dir[MAX_DIRNAME_LEN];
	char				line_CSV[MAX_LENGTH_CSV];
	SELStringPtr		pHelp;
	int32_t				shift_id;
	int32_t				fault_continue;
	int32_t				fault_retest;
	int32_t				Test_RnR;
	int32_t				Retested;
	int32_t				Write_Tracability_MMS;
	int32_t				Skip_All_Test;
	int32_t				Check_DTC;
	int32_t				Check_Laser;
	int32_t				Retest_Check_MAN_DATA_2;
	int32_t				Autorisation_retest;
	char                Production_Serial[4+1];
	char				barcode_Right[256+1];
	char				barcode_Left[256+1];
	char				Barcode[256];
	char				EMS_SERIAL[4+1];
	char				EMS_PN_PCBA[8+1];
	char				EMS_DATE[8+1];
	char				PALETT[2+1];
	char				Time[32];
	char				Date[32];
	char				TicketDirPath[MAX_PATHNAME_LEN];
	char				TicketFilePath[MAX_PATHNAME_LEN]; /* output parameter */
	SElExceptionPtr 	pexception;
} STestParam, *STestParamPtr;

typedef SElExceptionPtr (_FUNCC *IniTestPanel_t)(const char* picture, bool_t reset);
typedef SElExceptionPtr (_FUNCC *SetTestPanelDescription_t)(int32_t control, const char* description);
typedef SElExceptionPtr (_FUNCC *SetPanelWarning_t)(int32_t control,  bool_t status);
typedef SElExceptionPtr (_FUNCC *SetTestPanelReferences_t)(char* RefValeo, char* RefCustomer);
typedef SElExceptionPtr (_FUNCC *SetTestPanelMode_t)(int32_t mode);
typedef SElExceptionPtr	(_FUNCC *GetTestFunctionFcePtr_t)(int32_t index, STestListItemPtr* pTest, int32_t* count);
typedef SElExceptionPtr	(_FUNCC *GetTestSeqFunctionFcePtr_t)(STestSeqPtr* pTestSeq); 
typedef SElExceptionPtr (_FUNCC *CloseTestPanel_t)(void);
typedef SElExceptionPtr (_FUNCC *SetPanelStationStatus_t)(int32_t station,  bool_t status);


enum {
    TEST_FCE_TYPE_HELP = 1,
    TEST_FCE_TYPE_CHECK,
    TEST_FCE_TYPE_INIT,
    TEST_FCE_TYPE_RUN,
    TEST_FCE_TYPE_CLEAN
};
/*!
 * Specifies test information
 */ 
typedef struct _STest 
{
    SElExceptionPtr (*SetFunctionPtr)(
        struct _STest* me,
        int32_t     fceType,
		const char*		fceName,
        TestFce_t   pFce
    );
    SElExceptionPtr (*Run)(
    	struct _STest* pTest,
    	STestParamPtr pParams,
    	HANDLE* pHandle,
		bool_t* used 
	);
	SElExceptionPtr (*CloneSelf)(
    	struct _STest* pTest,
    	struct _STest** pTestClone 
	);

    int32_t     _LoopCount;
    bool_t      _IsUsed;
	bool_t		_CanBeUsed;
	int32_t 	_Step;
    STestListItem   test;
} STestObj, *STestPtr;

SElExceptionPtr test_new( STestPtr* pTest);
SElExceptionPtr test_delete( STestPtr* pTest);


#define TEST_ERROR_NOT_VALID_TESTER_CONFIGURATION    (-2222)
#define TEST_ERR_UNKNOWN_FCE_TYPE					-11111
#define TEST_ERR_UNKNOWN_FCE_PTR					-11112
#define TEST_ERR_TIMEOUT							-11113
#define TEST_ERR_VALUE_OUT_OF_LIMIT					-11114
#define TEST_ERROR_NOT_POSIBLE_TEST					-11115
#define TEST_ERROR_UNKNOWN_CONFIG_VARIABLE			-11116
#define TEST_ERROR_UNKNOWN_VALUE					-11117
#define TEST_ERROR_BUTTON_NOK						-11118
#define TEST_ERROR_CAMERA_SET_PARAMETER				-11119
#define TEST_ERROR_CAMERA_RESPONSE_TIMEOUT			-11120


#ifndef EXCLUDE_TEST_MAKROS

#define CAMERA_PICTO	0
#define CAMERA_LED		1


SElExceptionPtr QueryParameterReal64(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	double*	pValue
);
SElExceptionPtr QueryParameterUInt32(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	uint32_t*	pValue
);
SElExceptionPtr QueryParameterInt32(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	int32_t*	pValue
);
SElExceptionPtr QueryParameterString(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	char		buffer[],
	int32_t		bufferSize
);
SElExceptionPtr SetParameterString(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	const char* buffer
);
SElExceptionPtr SetParameterInt32(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	int32_t		pValue
);
SElExceptionPtr SetParameterUInt32(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	uint32_t		pValue
);
SElExceptionPtr SetParameterReal64(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	double		pValue
);
SElExceptionPtr CreateNewLimit(
	STestParamPtr	ppar,
	int			step,
	int			index,
	char*		limit_str,
	const char*	description
);
SElExceptionPtr CreateNewLimitEx(
	STestParamPtr	ppar,
	int			step,
	int			index,
	char*		min,
	char*		max,
	const char*	description
);
SElExceptionPtr GetLimitConfiguration(
	STestParamPtr	ppar,
	int			step,
	int			index,
	char		buffer[],
	int32_t		bufferSize
);
bool_t LimitExist(
	STestParamPtr	ppar,
	int			step,
	int			index
);
SElExceptionPtr CheckLimitSTRING(
	STestParamPtr	ppar,
	int			step,
	int			index,
	char*		value
);
SElExceptionPtr CheckLimitINT32(
	STestParamPtr	ppar,
	int			step,
	int			index,
	int			value
);
SElExceptionPtr CheckLimitUINT32(
	STestParamPtr	ppar,
	int			step,
	int			index,
	uint32_t	value
);
SElExceptionPtr CheckLimitREAL64(
	STestParamPtr	ppar,
	int			step,
	int			index,
	double		value
);

SElExceptionPtr GetTestDescription(
	STestParamPtr	ppar,
	int			step,
	char*		desc,
	uint32_t	descSize 
);

SElExceptionPtr GetTestResult(
	STestParamPtr	ppar,
	int32_t*		result
);

SElExceptionPtr SetTestResult(
	STestParamPtr	ppar,
	int			step,
	int			index, 
	SElExceptionPtr	pexception
);

SElExceptionPtr SetTestSequenceResult(
	STestParamPtr	ppar,
	int			mode,
	SElExceptionPtr pexception
);

SElExceptionPtr SetTestSequenceDuration(
	STestParamPtr	ppar,
	int			mode,
	double 		duration
);

SElExceptionPtr SetTestUsed(
	STestParamPtr	ppar,
	int			step
);

SElExceptionPtr SetTestDuration(
	STestParamPtr	ppar,
	int			step,
	double 		duration
);

SElExceptionPtr SetSubTestDuration(
	STestParamPtr	ppar,
	int			step,
	int			index,
	double 		duration
);

SElExceptionPtr SetTestSequenceTime(
	STestParamPtr	ppar,
	double 		duration
);

SElExceptionPtr GetTestSequenceBarcode(
	STestParamPtr	ppar,
	char* barcode,
	uint32_t barcode_size
);

SElExceptionPtr SetTestValue(
	STestParamPtr		ppar,
	int					step,
	const char*			name,
	const char*			value
);

SElExceptionPtr SetTestSequenceValue(
	STestParamPtr		ppar,
	int					mode,
	const char*			name,
	const char*			value
);

SElExceptionPtr CameraSetJob(
	STestParamPtr		ppar,
	SCamerasPtr			pcameras
);

SElExceptionPtr CameraSetLiveOn(
	STestParamPtr		ppar,
	SCamerasPtr			pcameras
);

SElExceptionPtr CameraSetLiveOff(
	STestParamPtr		ppar,
	SCamerasPtr			pcameras
);

SElExceptionPtr CameraGetdata(
	SCamerasPtr			pcameras,
	int32_t 			step,
	int32_t				timeout,
	char				*data
);

SElExceptionPtr GetResultREAL64formXMLString(
	SELStringPtr	result, 
	const char*		path, 
	double*			value
);
SElExceptionPtr GetResultSTRINGformXMLString(
	SELStringPtr		result, 
	const char*			path,
	char*				buffer
);

void SaveXmlResultFileCopy(STestParamPtr pTestParam, int step, int index, bool_t failed, char* pathCopy);
void SaveXmlResultFileFailed(char* pathCopy, int step, int index, char* name, char* value);

#define DEFINE_GLOBAL_VARIABLE() \
	static clock_t			_start_testseq_clock; \
	static clock_t			_start_test_clock; \
	extern int				gs_TestPanel; \
	extern SObjListPtr 		gs_Modules; \
	static bool_t			_kline_init = FALSE

#define MAX_AXIS_RESSOURCES	   		10
#define MAX_MOTOR_RESSOURCES	    4


	
#define TEST_USE( resource)		TEST_USE_##resource
#define TEST_USE_LIN			TEST_USE_CAN
#define TEST_USE_CAN			SCANListPtr	pcan = (SCANListPtr)gs_Modules->Get(gs_Modules, MODULE_CAN)
#define TEST_USE_KLINE			SVLinePtr	pvline = (SVLinePtr)gs_Modules->Get(gs_Modules, MODULE_VLINE)
#define TEST_USE_TESTER			STesterPtr	ptester = (STesterPtr)gs_Modules->Get(gs_Modules, MODULE_TESTER)
#define TEST_USE_CAMERA			SCamerasPtr	pcameras = (SCamerasPtr)gs_Modules->Get(gs_Modules, MODULE_CAMERA)
#define TEST_USE_JIG			SJigPtr		pjig = (SJigPtr)gs_Modules->Get(gs_Modules, MODULE_JIG_CLASS)
#define TEST_USE_COUNTER		SHWCounterPtr	pcounter = (SHWCounterPtr)gs_Modules->Get(gs_Modules, MODULE_COUNTER)
#define TEST_USE_SENSORS		SDIOSensorsPtr	psensors = (SDIOSensorsPtr)gs_Modules->Get(gs_Modules, MODULE_SENSORS)
#define TEST_USE_PRINTERS		SPrintersPtr	pprinters = (SPrintersPtr)gs_Modules->Get(gs_Modules, MODULE_PRINTERS)
#define TEST_USE_LABELS			SLabelListPtr	plabels = (SLabelListPtr)gs_Modules->Get(gs_Modules, MODULE_LABELS)
#define TEST_USE_DBS			SDBSPtr		pdbs = (SDBSPtr)gs_Modules->Get(gs_Modules, MODULE_DBS)
#define TEST_USE_POWER  		ViSession	vi_POWER = 0
#define TEST_USE_PSW	  		ViSession	vi_PSW = 0
#define TEST_USE_PSW1	  		ViSession	vi_PSW1 = 0
#define TEST_USE_PSW2	  		ViSession	vi_PSW2 = 0
#define TEST_USE_SW4	  		ViSession	vi_SW4 = 0
#define TEST_USE_SW5	  		ViSession	vi_SW5 = 0
#define TEST_USE_RESISTOR		ViSession	vi_RESISTOR = 0
#define TEST_USE_SCOPE			ViSession	vi_SCOPE = 0
#define TEST_USE_DAQ			ViSession	vi_DAQ = 0
#define TEST_USE_DMM			ViSession	vi_DMM = 0
#define TEST_USE_VAN			SVANPtr		pvan = NULL
#define TEST_USE_LINBUS			SCANPtr		plinbus = NULL
#define TEST_USE_AXIS			SaxissPtr	paxiss = (SaxissPtr)gs_Modules->Get(gs_Modules, MODULE_AXIS); SaxisPtr	paxis[MAX_AXIS_RESSOURCES] 
#define TEST_USE_MOTOR			SmotorsPtr	pmotors = (SmotorsPtr)gs_Modules->Get(gs_Modules, MODULE_MOTOR); SmotorPtr	pmotor[MAX_MOTOR_RESSOURCES] 

			


#define CAMERA_SET(camName) \
	EXCCHECK( CameraSet(pTID, pcamera, camName))

#define IF_NOT_CONNECT( resource) IF_NOT_CONNECT_##resource
#define IF_CONNECT( resource) IF_CONNECT_##resource
#define END_IF() } }while(0)

#define CONNECT( resource) CONNECT_##resource

#define DISCONNECT( resource) DISCONNECT_##resource
#define DISCONNECT_ALL( resource) checkErr( IviSwtch_DisconnectAll (vi_##resource))
	
#define RESISTOR_SET( channel, resistance) \
	do { \
		checkErr( Resistor_ConfigureOutputResistance( vi_RESISTOR, channel, resistance )); \
	} while(0)

#define POWER_VOLTAGE_SET( channel, voltage) \
	if ( voltage > 0.1 ) { /* TODO: Change IT */ \
		EXCCHECK( PowerSetPower( gs_Modules, CONFIG_FILE, channel, voltage, -1.0)); \
	} else \
		EXCCHECK( PowerDisconnect( gs_Modules, CONFIG_FILE, channel))

#define POWER_SET( channel, voltage, current) \
	EXCCHECK( PowerSetPower( gs_Modules, CONFIG_FILE, channel, voltage, current))

#define POWER_RESET( channel) \
	do { \
		checkErr( IviDCPwr_ConfigureOutputEnabled( vi_POWER, channel, VI_FALSE)); \
	} while(0)

#define VARIABLE( dtype, name) VARIABLE_##dtype(name)
#define VARIABLE_REAL64(name)	double name
#define VARIABLE_INT32(name)	int32_t name
#define VARIABLE_UINT32(name)	uint32_t name
#define STRING(name, size)		char name[size+1]=""

#define INITIALIZE( resource)	\
		if ( !vi_##resource ) { \
			SIviBridgePtr  pIvi = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);\
			EXCCHECK( pIvi->GetSession( pIvi, #resource, &vi_##resource)); \
		} else vi_##resource = vi_##resource

#define INITIALIZE_AXIS(resource) \
		EXCCHECK(paxiss->GetaxisByIndex(paxiss, resource, &paxis[resource])); 

#define INITIALIZE_MOTOR(resource) \
		EXCCHECK( pmotors->GetmotorByIndex(pmotors, resource, &pmotor[resource])); 

#define KLINE_RESET() \
	do{ \
		EXCCHECK( pvline->ClearCache(pvline)); \
		EXCCHECK( pvline->Reset(pvline)); \
		_kline_init = TRUE; \
	}while(0)

#define KLINE_QUERY(command,result) \
	do{ \
		EXCCHECK( pvline->ClearCache(pvline)); \
		if(!_kline_init) { \
			EXCCHECK( pvline->Reset(pvline)); \
			_kline_init = TRUE; } \
		pexception = pvline->Query(pvline, command, result, sizeof(result)); \
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", pvline->GetCachePtr(pvline)); \
		EXCCHECK( pexception); \
	}while(0)

#define KLINE_QUERY_EX(command,result) \
	do{ \
		EXCCHECK( pvline->ClearCache(pvline)); \
		if(!_kline_init) { \
			EXCCHECK( pvline->Reset(pvline)); \
			_kline_init = TRUE; } \
		pexception = pvline->Query(pvline, command, result, sizeof(result)); \
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", pvline->GetCachePtr(pvline)); \
		if (pexception != NULL) { \
			pexception = pvline->Query(pvline, command, result, sizeof(result)); \
			SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", pvline->GetCachePtr(pvline)); } \
		EXCCHECK( pexception); \
	}while(0)

#define CAN_REQUEST(channel, wIdent, wData) \
	do{ \
		EXCCHECK( pcan->ClearCache(pcan)); \
		pexception = pcan->WriteLowLevel(pcan, channel, wIdent, 0, 8, wData); \
		if ( pexception ) { \
		EXCDELETE( pexception );\
		pexception = pcan->WriteLowLevel(pcan, channel, wIdent, 0, 8, wData); \
		}\
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", pcan->GetCache(pcan)); \
		EXCCHECK( pexception); \
	}while(0)

#define CAN_RESPONSE(channel, rIdent, rDataSize, rData) \
	do{ \
		uint32_t		identifier = 0; \
		identifier = rIdent; \
		EXCCHECK( pcan->ClearCache(pcan)); \
		pexception = pcan->ReadLowLevel(pcan, channel, &identifier, rDataSize, rData); \
		if ( pexception ) { \
		EXCDELETE( pexception );\
		pexception = pcan->ReadLowLevel(pcan, channel, &identifier, rDataSize, rData); \
		}\
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", pcan->GetCache(pcan)); \
		EXCCHECK( pexception); \
	}while(0)

#define CAN_RESPONSE_EX(channel, rIdent, rDataSize, rData) \
	do{ \
		uint32_t		identifier = 0; \
		identifier = rIdent; \
		EXCCHECK( pcan->ClearCache(pcan)); \
		pcan->ReadLowLevel(pcan, channel, &identifier, rDataSize, rData); \
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", pcan->GetCache(pcan)); \
	}while(0)

#define CAN_QUERY(channel, wIdent, wData, rIdent, rDataSize, rData) \
	do{ \
		uint32_t	__size = *(rDataSize);\
		EXCCHECK( pcan->ClearCache(pcan)); \
		pexception = pcan->Query(pcan, channel, wIdent, wData, sizeof(wData), rIdent, rDataSize, rData); \
		if ( pexception ) { \
		EXCDELETE( pexception );\
		*(rDataSize) = __size;\
		pexception = pcan->Query(pcan, channel, wIdent, wData, sizeof(wData), rIdent, rDataSize, rData); \
		}\
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", pcan->GetCache(pcan)); \
		EXCCHECK( pexception); \
	}while(0)

#define CAN_QUERYEX(channel, wIdent, wData, rIdent, rDataSize, rData) \
	do{ \
		uint32_t	__size = *(rDataSize);\
		EXCCHECK( pcan->ClearCache(pcan)); \
		pexception = pcan->QueryEx(pcan, channel, wIdent, wData, sizeof(wData), rIdent, rDataSize, rData); \
		if ( pexception ) { \
		EXCDELETE( pexception );\
		*(rDataSize) = __size;\
		pexception = pcan->QueryEx(pcan, channel, wIdent, wData, sizeof(wData), rIdent, rDataSize, rData); \
		}\
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", pcan->GetCache(pcan)); \
		EXCCHECK( pexception); \
	}while(0)

#define WAIT_TABLE_ROTATION_FINISHED(timeout) \
		EXCCHECK( ptester->WaitRotationFinished(ptester, timeout))


#define TESTER_START_RODAGE_STATION_20(timeout) \
	EXCCHECK( ptester->StartRodageStation20(ptester, timeout))

#define TESTER_STOP_RODAGE_STATION_20(timeout) \
	EXCCHECK( ptester->StopRodageStation20(ptester, timeout))

#define TESTER_CHECK_DUMMY_STATION_20(timeout,status) \
	EXCCHECK( ptester->StatusPushsStation20(ptester, status))

#define TESTER_RODAGE_STATION_20(timeout,cycle,timeon,timeoff,version) \
	EXCCHECK( ptester->RodageStation20(ptester, timeout, cycle, timeon, 0, version))

#define TESTER_PLUG_PANEL_STATION_30(timeout) \
	EXCCHECK( ptester->PlugPanelStation30(ptester, timeout))

#define TESTER_UNPLUG_PANEL_STATION_30(timeout) \
	EXCCHECK( ptester->UnplugPanelStation30(ptester, timeout))

#define TESTER_PLUG_PANEL_STATION_40(timeout) \
	EXCCHECK( ptester->PlugPanelStation40(ptester, timeout))

#define TESTER_UNPLUG_PANEL_STATION_40(timeout) \
	EXCCHECK( ptester->UnplugPanelStation40(ptester, timeout))

#define TESTER_START_RODAGE_LEFT_STATION_50(timeout) \
	EXCCHECK( ptester->StartRodageLeftStation50(ptester, timeout))

#define TESTER_STOP_RODAGE_LEFT_STATION_50(timeout) \
	EXCCHECK( ptester->StopRodageLeftStation50(ptester, timeout))

#define TESTER_START_RODAGE_RIGHT_STATION_50(timeout) \
	EXCCHECK( ptester->StartRodageRightStation50(ptester, timeout))

#define TESTER_STOP_RODAGE_RIGHT_STATION_50(timeout) \
	EXCCHECK( ptester->StopRodageRightStation50(ptester, timeout))

#define TESTER_PLUG_PANEL_STATION_60(timeout) \
	EXCCHECK( ptester->PlugPanelStation60(ptester, timeout))

#define TESTER_UNPLUG_PANEL_STATION_60(timeout) \
	EXCCHECK( ptester->UnplugPanelStation60(ptester, timeout))

#define TESTER_START_TEST_RIGHT_STATION_60(timeout) \
	EXCCHECK( ptester->StartTestRightStation60(ptester, timeout))

#define TESTER_STOP_TEST_RIGHT_STATION_60(timeout) \
	EXCCHECK( ptester->StopTestRightStation60(ptester, timeout))

#define TESTER_START_TEST_LEFT_STATION_60(timeout) \
	EXCCHECK( ptester->StartTestLeftStation60(ptester, timeout))

#define TESTER_STOP_TEST_LEFT_STATION_60(timeout) \
	EXCCHECK( ptester->StopTestLeftStation60(ptester, timeout))

#define TESTER_PLUG_PANEL_STATION_70(timeout) \
	EXCCHECK( ptester->PlugPanelStation70(ptester, timeout))

#define TESTER_UNPLUG_PANEL_STATION_70(timeout) \
	EXCCHECK( ptester->UnplugPanelStation70(ptester, timeout))

#define TESTER_ILLUMINATION_STATION_70(status) \
	EXCCHECK( ptester->IlluminationStation70(ptester, status))


/* Home system axis station 30 */
#define AXIS_HOME_ALL_STATION_30(check) \
	MoveAxis2(paxis[SYSTEM_AXIS_Z_STATION_30_RIGHT], AXIS_Z,0.0, 100.0); \
	MoveAxis2(paxis[SYSTEM_AXIS_Z_STATION_30_LEFT], AXIS_Z,0.0, 100.0);  

/* Axis Z up right station 30 */
#define AXIS_Z_UP_STATION_30(check) \
	double position_x, position_y, position_z; \
	EXCCHECK( paxis[SYSTEM_AXIS_STATION_30_RIGHT]->CurrentPosition(paxis[SYSTEM_AXIS_STATION_30_RIGHT], AXIS_X, &position_x)); \
	EXCCHECK( paxis[SYSTEM_AXIS_STATION_30_RIGHT]->CurrentPosition(paxis[SYSTEM_AXIS_STATION_30_RIGHT], AXIS_Y, &position_y)); \
	EXCCHECK( paxis[SYSTEM_AXIS_Z_STATION_30_RIGHT]->CurrentPosition(paxis[SYSTEM_AXIS_Z_STATION_30_RIGHT], AXIS_Z, &position_z)); \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_Z_STATION_30_RIGHT], paxis[SYSTEM_AXIS_STATION_30_RIGHT], check, position_z - 4.0, position_y, position_x); 

/* Home system axis station 40 */
#define AXIS_HOME_ALL_STATION_40(check) \
	MoveAxis2(paxis[SYSTEM_AXIS_Z_STATION_40_RIGHT], AXIS_Z,0.0, 100.0); \
	MoveAxis2(paxis[SYSTEM_AXIS_Z_STATION_40_LEFT], AXIS_Z,0.0, 100.0);

/* Axis Z up right station 30 */
#define AXIS_Z_UP_STATION_40(check) \
	double position_x, position_y, position_z; \
	EXCCHECK( paxis[SYSTEM_AXIS_STATION_40_RIGHT]->CurrentPosition(paxis[SYSTEM_AXIS_STATION_40_RIGHT], AXIS_X, &position_x)); \
	EXCCHECK( paxis[SYSTEM_AXIS_STATION_40_RIGHT]->CurrentPosition(paxis[SYSTEM_AXIS_STATION_40_RIGHT], AXIS_Y, &position_y)); \
	EXCCHECK( paxis[SYSTEM_AXIS_Z_STATION_40_RIGHT]->CurrentPosition(paxis[SYSTEM_AXIS_Z_STATION_40_RIGHT], AXIS_Z, &position_z)); \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_Z_STATION_40_RIGHT], paxis[SYSTEM_AXIS_STATION_40_RIGHT], check, position_z - 4.0, position_y, position_x); 

#define MOTOR_INIT(resource) \
	EXCCHECK( pmotor[resource]->_InitializeSpec(pmotor[resource])); 
	//Sleep(100);

#define MOTOR_STOP(resource) \
	EXCCHECK( pmotor[resource]->EnableMotor(pmotor[resource], 0)); 
	//Sleep(100);

#define MOTOR_INIT_ANGLE(resource) \
	EXCCHECK( pmotor[resource]->AlarmReset(pmotor[resource], 0)); 
	//Sleep(100);

#define MOTOR_ROTATE(resource, angle, speed) \
	EXCCHECK( pmotor[resource]->MoveMotorPosition(pmotor[resource], 0, angle, speed)); 
	//Sleep(100);

#define TESTER_INIT(istation) \
	ptester->InitializeStations(ptester, istation, TIMEOUT_DEFAULT)

#define TESTER_OKNOK(timeout) \
	do{	\
		bool_t ok; \
		EXCCHECK( ptester->OkNokCheck(ptester, timeout,&ok)); \
		if(!ok) EXCTHROW(TEST_ERROR_BUTTON_NOK, "Button NOK pressed!" ); \
	}while(0)

#define TESTER_BEEP()  \
	EXCCHECK( ptester->TesterBeep(ptester,100))

#define TEST_BEGIN() \
    SElExceptionPtr     pexception = NULL; \
	int32_t				error = 0; \
	clock_t				_clock = clock(); \
	int32_t				_test_index = 1		

#define TEST_END() \
	IVI_CHECK_ERR(error); \
	if(pexception) TEST_DURATION((double)(clock()-_start_test_clock)/(double)CLOCKS_PER_SEC); \
	TEST_FAILED( pexception ); \
    return  pexception

#define TEST_END_STATION(istation) \
	IVI_CHECK_ERR(error); \
	TEST_DURATION((double)(clock()-_start_test_clock)/(double)CLOCKS_PER_SEC); \
 	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE) { Write_Traca_MMS_NOK (istation, ((STestParamPtr)pTID)); \
	StopThreads(istation); \
	TESTER_INIT (istation); \
	SetCtrlIndex( gs_TestPanel, panelStatusStation[istation], 1); } \
	TEST_FAILED( pexception ); \
    return  pexception
    
#define HELP_BEGIN() \
    SElExceptionPtr pexception = NULL

#define HELP_TEST(help) \
	EXCCHECK(((STestParamPtr)pTID)->pHelp->Format(((STestParamPtr)pTID)->pHelp, \
		"	<help>%s</help>\n", help))

#define HELP_PARAMETER(option, name, type, help) HELP_PARAMETER_##option(name, type, help) /* required, name, value type, help */

#define HELP_PARAMETER_OPTIONAL(name, type, help) /* required, name, value type, help */ \
	EXCCHECK(((STestParamPtr)pTID)->pHelp->Format(((STestParamPtr)pTID)->pHelp, \
		"	<parameter>\n" \
		"		<option>Optional</option>\n" \
		"		<name>%s</name>\n" \
		"		<type>%s</type>\n" \
		"		<help>%s</help>\n" \
		"	</parameter>\n", name, type, help))

#define HELP_PARAMETER_REQUIRED(name, type, help) /* required, name, value type, help */ \
	EXCCHECK(((STestParamPtr)pTID)->pHelp->Format(((STestParamPtr)pTID)->pHelp, \
		"	<parameter>\n" \
		"		<option>Required</option>\n" \
		"		<name>%s</name>\n" \
		"		<type>%s</type>\n" \
		"		<help>%s</help>\n" \
		"	</parameter>\n", name, type, help))

#define HELP_END() \
    return  pexception
    
#define TESTREGUIRE_BEGIN() \
	SElExceptionPtr     pexception = NULL;\
	ViStatus	error = VI_SUCCESS

#define RESISTOR_REQUIRED( channel, value) \
	if ( !error ) { \
		error = Resistor_CheckAttributeViReal64( vi_RESISTOR, channel, RESISTOR_ATTR_RESISTANCE, value); \
		if ( error == IVI_ERROR_INVALID_VALUE ) {\
			char message[512] = "";\
			sprintf(message, _("It's not possible set resistance %.2Lf for channel %s"), value, channel);\
			EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, message); } \
		checkErr( error); \
	} else error = error

#define POWER_REQUIRED_VOLTAGE( channel, value, min, max) \
	if ( !error ) { \
		error = IviDCPwr_CheckAttributeViReal64( vi_POWER, channel, IVIDCPWR_ATTR_VOLTAGE_LEVEL, value); \
		if ( error == IVI_ERROR_INVALID_VALUE ) { \
			char message[512] = "";\
			sprintf(message, _("It's not possible set voltage %.2Lf for channel %s"), value, channel);\
			EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, message); } \
		checkErr( error);\
	} else error = error

#define POWER_REQUIRED_CURRENT( channel, value, min, max) \
	if ( !error ) { \
		error = IviDCPwr_CheckAttributeViReal64( vi_POWER, channel, IVIDCPWR_ATTR_CURRENT_LIMIT, value); \
		if ( error == IVI_ERROR_INVALID_VALUE ) {\
			char message[512] = "";\
			sprintf(message, _("It's not possible set current %.2Lf for channel %s"), value, channel);\
			EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, message); } \
		checkErr( error); \
	} else error = error

#define POWER_REQUIRED( channel, voltage, current) \
	if ( !error ) { \
		ViReal64 max_curr;\
		error = IviDCPwr_QueryMaxCurrentLimit( vi_POWER, channel, voltage, &max_curr);\
		if ( error == IVI_ERROR_INVALID_VALUE || error == IVI_ERROR_INVALID_PARAMETER) {\
			char message[512] = "";\
			sprintf(message, _("It's not possible set voltage %.2Lf for channel %s"), voltage, channel);\
			EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, message); } \
		checkErr( error); \
		if ( current > max_curr ) { \
			char message[512] = "";\
			sprintf(message, _("It's not possible set current %.2Lf for channel %s"), current, channel);\
			EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, message); } \
	} else error = error
	
#define REQUIRE( res)  REQUIRE_##res	
	
#define REQUIRE_POWER if (!error) { \
	ViSession vi_POWER = 0; \
	SIviBridgePtr pIvi = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE); \
	if ( pIvi == NULL ) EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, _("Ivi Bridge Module is missing!")); \
	EXCCHECK( pIvi->GetSession( pIvi, "POWER", &vi_POWER))

#define REQUIRE_KLINE if (!error) { \
	SVLinePtr pvline = (SVLinePtr)gs_Modules->Get(gs_Modules, MODULE_VLINE);\
	if ( pvline == NULL ) EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, _("K-line Module is missing!"))

#define REQUIRE_LIN REQUIRE_CAN

#define REQUIRE_CAN if (!error) { \
	SCANPtr pcan = (SCANPtr)gs_Modules->Get(gs_Modules, MODULE_CAN);\
	if ( pcan == NULL ) EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, _("CAN Module is missing!"))

#define REQUIRE_TESTER if (!error) { \
	STesterPtr ptester = (STesterPtr)gs_Modules->Get(gs_Modules, MODULE_TESTER);\
	if ( ptester == NULL ) EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, _("Tester Module is missing!"))

#define REQUIRE_AXIS if (!error) { \
	SaxissPtr	paxiss = (SaxissPtr)gs_Modules->Get(gs_Modules, MODULE_AXIS);\
	if ( paxiss == NULL ) EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, _("Axis Module is missing!"))

#define REQUIRE_MOTOR if (!error) { \
	SaxissPtr	paxiss = (SaxissPtr)gs_Modules->Get(gs_Modules, MODULE_MOTOR);\
	if ( paxiss == NULL ) EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, _("Axis Module is missing!"))

#define REQUIRE_CAMERA if (!error) { \
	SIMAQPtr pcamera = (SIMAQPtr)gs_Modules->Get(gs_Modules, MODULE_CAMERA);\
	if ( pcamera == NULL ) EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, _("Camera Module is missing!"))

#define REQUIRE_PRINTERS if (!error) { \
	SPrintersPtr pprinters = (SPrintersPtr)gs_Modules->Get(gs_Modules, MODULE_PRINTERS);\
	if ( pprinters == NULL ) EXCTHROW( TEST_ERROR_NOT_POSIBLE_TEST, _("Printers Module is missing!"))

#define REQUIRE_END() } else error = error

#define TESTREGUIRE_END() \
	return  pexception
	
#define TESTINIT_BEGIN() \
	char				_test_description[256] = "unknown"; \
	clock_t				_start_clock = clock(); \
	SElExceptionPtr     pexception = NULL; \
	int32_t				_test_index = 1; \
	int32_t				error = 0

/*
#define TESTINIT_END(iStation) \
	IVI_CHECK_ERR(error); \
	TEST_DESCRIPTION(_test_description, 256); \
	if (pexception == NULL) { \
	EXCCHECKCVI( SetCtrlVal(gs_TestPanel, panelHelpStation[iStation], _test_description));\
	} \
	TEST_USED(); \
	_start_test_clock = _start_clock; \
	if(pexception) TEST_DURATION((double)(clock()-_start_test_clock)/(double)CLOCKS_PER_SEC); \
	TEST_FAILED( pexception ); \
	return  pexception */

#define TESTINIT_END(iStation) \
	IVI_CHECK_ERR(error); \
	TEST_DESCRIPTION(_test_description, 256); \
	EXCCHECKCVI( SetCtrlVal(gs_TestPanel, panelHelpStation[iStation], _test_description));\
	TEST_USED(); \
	_start_test_clock = _start_clock; \
	if(pexception) TEST_DURATION((double)(clock()-_start_test_clock)/(double)CLOCKS_PER_SEC); \
	TEST_FAILED( pexception ); \
	return  pexception

#define TESTCLEANUP_BEGIN() \
	SElExceptionPtr     pexception = NULL; \
	int32_t				_test_index = 1; \
	int32_t				error = 0

#define TESTCLEANUP_END(station) \
	IVI_CHECK_ERR(error); \
	TEST_DURATION((double)(clock()-_start_test_clock)/(double)CLOCKS_PER_SEC); \
	TEST_FAILED( pexception ); \
	return  pexception
    
#define FCE_HELP( prefix, nb) \
    TEST_API SElExceptionPtr _FUNCC prefix##_##nb##_help(void* pTID, int32_t mode)
    
#define FCE_CHECK( prefix, nb) \
    TEST_API SElExceptionPtr _FUNCC prefix##_##nb##_check(void* pTID, int32_t mode)

#define FCE_INIT( prefix, nb) \
    TEST_API SElExceptionPtr _FUNCC prefix##_##nb##_init(void* pTID, int32_t mode)

#define FCE_TEST( prefix, nb) \
    TEST_API SElExceptionPtr _FUNCC prefix##_##nb##_test(void* pTID, int32_t mode)
    
#define FCE_CLEANUP( prefix, nb) \
    TEST_API SElExceptionPtr _FUNCC prefix##_##nb##_cleanup(void* pTID, int32_t mode)


#define ADD_TI( name, prefix, nb) { name,\
									#prefix"_"#nb"_help",\
									#prefix"_"#nb"_check",\
									#prefix"_"#nb"_init",\
									#prefix"_"#nb"_test",\
									#prefix"_"#nb"_cleanup",\
									NULL,\
									NULL,\
									NULL,\
									NULL,\
									NULL }

#define DEFINE_TEST_FUNCTIONS( prefix, nb) \
	FCE_HELP( prefix, nb);  \
	FCE_CHECK( prefix, nb);  \
	FCE_INIT( prefix, nb);  \
	FCE_TEST( prefix, nb);  \
	FCE_CLEANUP( prefix, nb)

#define TESTSEQUENCEINIT_BEGIN() \
	SElExceptionPtr     pexception = NULL; \
	clock_t				_start_clock = clock(); \
	int32_t				error = 0

#define TESTSEQUENCEINIT_END() \
	IVI_CHECK_ERR(error); \
	_start_testseq_clock = clock(); \
	TESTSEQUENCE_INIT_DURATION((double)(clock()-_start_clock)/(double)CLOCKS_PER_SEC); \
	TESTSEQUENCE_INIT_FAILED(pexception); \
	return  pexception

#define TESTSEQUENCECLEANUP_BEGIN() \
	SElExceptionPtr     pexception = NULL; \
	clock_t				_start_clock = clock(); \
	int32_t				error = 0

#define TESTSEQUENCECLEANUP_END() \
	IVI_CHECK_ERR(error); \
	_kline_init = FALSE; \
	DISPLAY_TESTSEQUENCE_DURATION((double)(clock()-_start_testseq_clock)/(double)CLOCKS_PER_SEC); \
	TESTSEQUENCE_DURATION((double)(clock()-_start_testseq_clock)/(double)CLOCKS_PER_SEC); \
	TESTSEQUENCE_CLEANUP_DURATION((double)(clock()-_start_clock)/(double)CLOCKS_PER_SEC); \
	TESTSEQUENCE_CLEANUP_FAILED(pexception); \
	return  pexception

#define FCE_TESTSEQUENCE_INIT( prefix) \
    TEST_API SElExceptionPtr _FUNCC prefix##_init(void* pTID, int32_t mode)

#define FCE_TESTSEQUENCE_CLEANUP( prefix) \
    TEST_API SElExceptionPtr _FUNCC prefix##_cleanup(void* pTID, int32_t mode)

#define ADD_TSI( prefix) { #prefix"_init",\
						   #prefix"_cleanup",\
							NULL,\
							NULL }

#define DEFINE_TESTSEQUENCE_FUNCTIONS( prefix) \
	FCE_TESTSEQUENCE_INIT(prefix); \
	FCE_TESTSEQUENCE_CLEANUP(prefix)

#define ADD_TI_LAST_ENTRY { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}

#define PARAM_REAL64_REQUIRED( name, var)\
	EXCCHECK( QueryParameterReal64(pTID, ((STestParamPtr)pTID)->step, name, var))
#define PARAM_INT32_REQUIRED( name, var)\
	EXCCHECK( QueryParameterInt32(pTID, ((STestParamPtr)pTID)->step, name, var))
#define PARAM_UINT32_REQUIRED( name, var)\
	EXCCHECK( QueryParameterUInt32(pTID, ((STestParamPtr)pTID)->step, name, var))
#define PARAM_STRING_REQUIRED( name, buffer) \
	EXCCHECK( QueryParameterString(pTID, ((STestParamPtr)pTID)->step, name, buffer, sizeof(buffer)-1))


#define PARAM_UINT32_OPTIONAL( name, var, dvalue) \
	do { \
		*(var) = dvalue;\
		pexception = QueryParameterUInt32(pTID, ((STestParamPtr)pTID)->step, name, var); \
		if (pexception) EXCDELETE( pexception); \
		/* EXCIGNORE( )  */ \
	} while (0);

#define PARAM_INT32_OPTIONAL( name, var, dvalue) \
	do { \
		*(var) = dvalue;\
		pexception = QueryParameterInt32(pTID, ((STestParamPtr)pTID)->step, name, var); \
		if (pexception) EXCDELETE( pexception); \
		/* EXCIGNORE( )  */ \
	} while (0);

#define PARAM_REAL64_OPTIONAL( name, var, dvalue) \
	do { \
		*(var) = dvalue;\
		pexception = QueryParameterReal64(pTID, ((STestParamPtr)pTID)->step, name, var); \
		if (pexception) EXCDELETE( pexception); \
		/* EXCIGNORE( )  */ \
	} while (0);

#define PARAM_STRING_OPTIONAL( name, buffer, dvalue) \
	do { \
		strcpy(buffer, dvalue); \
		pexception = QueryParameterString(pTID, ((STestParamPtr)pTID)->step, name, buffer, sizeof(buffer)-1); \
		if (pexception) EXCDELETE( pexception); \
		/* EXCIGNORE( )  */ \
	} while (0);

#define RESULT_XMLSTRING_GET(type, result, path, value) \
	EXCCHECK( GetResult##type##formXMLString(result, path, value))

#define LIMIT_CREATE(limit_str, description) \
	EXCCHECK( CreateNewLimit(pTID, ((STestParamPtr)pTID)->step, _test_index, limit_str, description))

#define LIMIT_CREATE_EX(min, max, description) \
	EXCCHECK( CreateNewLimitEx(pTID, ((STestParamPtr)pTID)->step, _test_index, min, max, description))

#define LIMIT_CONFIGURATION(buffer) \
	EXCCHECK( GetLimitConfiguration(pTID, ((STestParamPtr)pTID)->step, _test_index, buffer, sizeof(buffer)))

#define LIMIT_EXIST() \
	LimitExist(pTID, ((STestParamPtr)pTID)->step, _test_index)

#define LIMIT_CHECK( type, var) \
	LIMIT_CHECK_EXT(type, var, pexception); \
	if (((STestParamPtr)pTID)->Test_RnR != TRUE && ((STestParamPtr)pTID)->fault_continue != TRUE) EXCCHECK( pexception);


#define CHECK_FOR_LIMIT(type, var) \
	SUBTEST_DURATION(_test_index, (double)(clock()-_clock)/(double)CLOCKS_PER_SEC); \
	pexception = CheckLimit##type(pTID, ((STestParamPtr)pTID)->step, _test_index, var); \
	if (pexception) EXCDELETE(pexception); \
	else {_clock = clock(); _test_index++; break; }

#define LIMIT_CHECK_EXT( type, var, pexception ) \
	SUBTEST_DURATION(_test_index, (double)(clock()-_clock)/(double)CLOCKS_PER_SEC); \
	pexception = CheckLimit##type(pTID, ((STestParamPtr)pTID)->step, _test_index, var); \
	_clock = clock(); \
	_test_index++

#define LIMIT_SET(type, var) \
	do { \
	SElExceptionPtr pexc_tmp = NULL; \
	pexc_tmp = CheckLimit##type(pTID, ((STestParamPtr)pTID)->step, _test_index, var); \
	if (pexc_tmp) EXCDELETE(pexc_tmp); \
	_test_index++; \
	}while(0)

#define TEST_VALUE_SET( name, value) \
	SetTestValue(pTID, ((STestParamPtr)pTID)->step, name, value)

#define TEST_DESCRIPTION( desc, size ) \
	GetTestDescription(pTID, ((STestParamPtr)pTID)->step, desc, size)

#define TEST_USED( ) \
	SetTestUsed(pTID, ((STestParamPtr)pTID)->step)

#define TEST_FAILED( pexception ) \
	SetTestResult(pTID, ((STestParamPtr)pTID)->step, --_test_index, pexception)

#define TESTSEQUENCE_INIT_FAILED( pexception ) \
	SetTestSequenceResult(pTID, 0, pexception)

#define TESTSEQUENCE_CLEANUP_FAILED( pexception ) \
	SetTestSequenceResult(pTID, 1, pexception)

#define TESTSEQUENCE_INIT_DURATION( duration ) \
	SetTestSequenceDuration(pTID, 0, duration)

#define TESTSEQUENCE_CLEANUP_DURATION( duration ) \
	SetTestSequenceDuration(pTID, 1, duration)

#define TEST_DURATION( duration ) \
	SetTestDuration(pTID, ((STestParamPtr)pTID)->step, duration)

#define SUBTEST_DURATION( index, duration ) \
	EXCCHECK( SetSubTestDuration(pTID, ((STestParamPtr)pTID)->step, index, duration))

#define TESTSEQUENCE_DURATION( duration ) \
	SetTestSequenceTime(pTID, duration)

#define TESTSEQUENCE_BARCODE( barcode, size) \
	GetTestSequenceBarcode(pTID, barcode, size)

#endif /* EXCLUDE_TEST_MAKROS */

#ifdef __cplusplus
    }
#endif 

#endif /* __EL_TEST_H__ */
