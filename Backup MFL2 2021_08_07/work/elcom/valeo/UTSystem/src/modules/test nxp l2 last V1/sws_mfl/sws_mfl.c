#include <ansi_c.h>
#include "sws_mfl.h"
#include "sws_mfl_utils.h"
#include <test\test_helper_functions.h>
#include "toolbox.h"
#include <formatio.h>
#include <multithread/multithread.h>
#include <analysis.h>
#include <export.h>
#include <time.h> 
#include "asynctmr.h"
#include "nidmm.h"
#include "niDMMErrorHandler.h"
#include "PeaksDetector.h"

ViChar    resourceName[256]= "Dev1";
ViSession vi_DMM 		   = 0;
ViBoolean idQuery          = VI_TRUE;
ViBoolean reset            = VI_TRUE;
ViStatus  error            = VI_SUCCESS;
ViInt32   measurementType  = NIDMM_VAL_DC_CURRENT;
ViReal64  powerlineFreq    = NIDMM_VAL_60_HERTZ;
ViReal64  range            = 0.20;
ViReal64  resolution       = 5.50;   
ViInt32   ErrorDMM;


/* new*/
ViInt32   	numOfMeas             = 100;
ViInt32   	timeoutInMilliSeconds = 2000;
ViInt32   	triggerSource         = NIDMM_VAL_IMMEDIATE;
ViReal64  	triggerDelay          = 0.00;
ViInt32   	triggerSample         = NIDMM_VAL_IMMEDIATE;
ViInt32    numPointsRead;
/* new*/



const char* HISTORY_INFO = \
"[22-06-2017] ARAIBIA MOHAMED: version 1.00\
 * Création."; 


DEFINE_GLOBAL_VARIABLE();

#define TIPP_WHEEL_DOWN_GAIN_F1     1.0  //0.9 // 0.76 //0.65	
#define TIPP_WHEEL_DOWN_OFFSET_F1   0.0		  // 0.0
#define TIPP_WHEEL_DOWN_GAIN_F2     0.5    // 0.5
#define TIPP_WHEEL_DOWN_OFFSET_F2   0.0 
#define TIPP_WHEEL_DOWN_GAIN_F3      0.7  //0.93	
#define TIPP_WHEEL_DOWN_OFFSET_F3   0.2	// -0.2
#define TIPP_WHEEL_DOWN_GAIN_F4      0.4  //0.7	
#define TIPP_WHEEL_DOWN_OFFSET_F4   -0.0	 // -0.2
#define TIPP_WHEEL_DOWN_GAIN_F5      1.0	
#define TIPP_WHEEL_DOWN_OFFSET_F5   0.00


#define TIPP_WHEEL_DOWN_GAIN_A1     0.45  //0.55  //0.6  //0.69
#define TIPP_WHEEL_DOWN_OFFSET_A1   0.0
#define TIPP_WHEEL_DOWN_GAIN_A2     0.68 // 0.7 // 0.73
#define TIPP_WHEEL_DOWN_OFFSET_A2   0.0
#define TIPP_WHEEL_DOWN_GAIN_A3     0.690  // 0.69
#define TIPP_WHEEL_DOWN_OFFSET_A3   0.0
#define TIPP_WHEEL_DOWN_GAIN_A4     0.6   //0.68
#define TIPP_WHEEL_DOWN_OFFSET_A4   0.0
#define TIPP_WHEEL_DOWN_GAIN_A5     0.650  //0.76
#define TIPP_WHEEL_DOWN_OFFSET_A5   0.0


#define TIPP_WHEEL_UP_GAIN_F1     1.1  // 1.0 // 0.76   // 0.65	
#define TIPP_WHEEL_UP_OFFSET_F1   0.9  //0.7 // 0.82
#define TIPP_WHEEL_UP_GAIN_F2     0.4 // 0.4  // 0.5	
#define TIPP_WHEEL_UP_OFFSET_F2   0.0								    
#define TIPP_WHEEL_UP_GAIN_F3     0.8	//0.9 // 0.75 //0.93
#define TIPP_WHEEL_UP_OFFSET_F3   0.7  //0.4 //0.0	  //0.38
#define TIPP_WHEEL_UP_GAIN_F4     0.75  //0.85  //0.93	
#define TIPP_WHEEL_UP_OFFSET_F4   0.0	   //0.38
#define TIPP_WHEEL_UP_GAIN_F5      1.0	
#define TIPP_WHEEL_UP_OFFSET_F5   0.00

#define TIPP_WHEEL_UP_GAIN_A1     0.45 // 0.55 //0.65  // 0.74
#define TIPP_WHEEL_UP_OFFSET_A1   0.0
#define TIPP_WHEEL_UP_GAIN_A2     0.7  // 0.65 // 0.77
#define TIPP_WHEEL_UP_OFFSET_A2   0.0
#define TIPP_WHEEL_UP_GAIN_A3     0.72  //0.7	   // 0.74
#define TIPP_WHEEL_UP_OFFSET_A3   0.5	  //0.0
#define TIPP_WHEEL_UP_GAIN_A4     0.65	  //0.72
#define TIPP_WHEEL_UP_OFFSET_A4   0.0
#define TIPP_WHEEL_UP_GAIN_A5     0.650   //0.69
#define TIPP_WHEEL_UP_OFFSET_A5   0.0


#define SYSTEM_AXIS_STATION_30_RIGHT  	0
#define SYSTEM_AXIS_STATION_30_LEFT  	1
#define SYSTEM_AXIS_STATION_40_RIGHT  	2
#define SYSTEM_AXIS_STATION_40_LEFT  	3
#define SYSTEM_AXIS_STATION_60_RIGHT  	4
#define SYSTEM_AXIS_STATION_60_LEFT  	5

#define MOTOR_STATION_50_RIGHT		  	0
#define MOTOR_STATION_50_LEFT		  	1
#define MOTOR_STATION_60_RIGHT		  	2
#define MOTOR_STATION_60_LEFT		  	3

#define AXIS_X  						0
#define AXIS_Y  						1
#define AXIS_Z  						2

#define TIMEOUT_CAMERA					4000

#define STEP_DAY  						0
#define STEP_NIGHT						1
#define STEP_STA						2
#define STEP_CHROME						3

#define RETEST							5


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

#define TEST_VOLTAGE			14
#define TEST_CURRENT			0.2
#define MIN_VOLTAGE				14.25
#define MAX_VOLTAGE				13.75

#define SCALE_BIT				5
#define SCALE_BIT_SCROLL		2


#define _CHECK_TRACA_EMS		1
#define _CHECK_TRACA_MMS		1
#define _WRITE_TRACA_MMS		1

	
#define OK				1
#define NOK				0

#define CONFIG_FILE				"\\test_power_source.xml"



/*  Backup positions 05/02/2020
//------------------------  Positions Right pushs STATION 30 : OK --------------------------------------
#define AXIS_MOVE_PUSH_PHONE_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 41.5, 42.6, 42.2, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));//OK

#define AXIS_MOVE_PUSH_LIST_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 42.0, 40.0, 23.0, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));//OK

#define AXIS_MOVE_PUSH_SPEECH_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 42.0, 37.6, 4.3, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));

#define AXIS_MOVE_PUSH_MENU_RIGHT_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 43.0, 25.8, 3.9, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));

#define AXIS_MOVE_PUSH_SCROLL_WHEEL_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 40.5, 25.0, 19.9, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));

#define AXIS_MOVE_PUSH_MENU_LEFT_STATION_30(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 44.0, 25.4, 39.5, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));
																				
#define AXIS_MOVE_PUSH_VOLUME_MINUS_STATION_30(check) \
  	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 44.0, 8.7, 30.6, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));

#define AXIS_MOVE_PUSH_VOLUME_PLUS_STATION_30(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 44.0,12.3,9.9, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));
//----------------------------------------------------------------------------------------------------- 

//------------------------  Positions Left pushs STATION 30 : OK --------------------------------------
#define AXIS_MOVE_PUSH_WHEEL_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 41.5, 8.1, 43.5, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));

#define AXIS_MOVE_PUSH_SPEED_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 41.0, 10.5, 24.8, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));

#define AXIS_MOVE_PUSH_SET_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 41.5, 13.0, 7.8, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));

#define AXIS_MOVE_PUSH_MINUS_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 42.0, 24.9, 6.7, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));

#define AXIS_MOVE_PUSH_PLUS_STATION_30(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 23.5, 25.0, 8.0, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30")); //

#define AXIS_MOVE_PUSH_CANCEL_STATION_30(check) \
  	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 24.5, 41.0, 13.0, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));

#define AXIS_MOVE_PUSH_RESUME_STATION_30(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 24.5, 37.0, 43.0, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));
//------------------------------------------------------------------------------------------------------ 




//------------------------  Positions Right pushs STATION 40 : OK --------------------------------------
#define AXIS_MOVE_PUSH_PHONE_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 20.0, 19.0, 42.0, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));//OK

#define AXIS_MOVE_PUSH_LIST_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 20.0, 18.0, 24.0, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));//OK

#define AXIS_MOVE_PUSH_SPEECH_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 20.5, 17.0, 8.0, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));

#define AXIS_MOVE_PUSH_MENU_RIGHT_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 21.0, 11.0, 6.0, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));

#define AXIS_MOVE_PUSH_SCROLL_WHEEL_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 41.5, 25.0, 19.9, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));

#define AXIS_MOVE_PUSH_MENU_LEFT_STATION_40(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 43.5, 25.7, 42.3, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));

#define AXIS_MOVE_PUSH_VOLUME_MINUS_STATION_40(check) \
  	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 44.5, 9.5, 35.5, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));

#define AXIS_MOVE_PUSH_VOLUME_PLUS_STATION_40(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 44.5,12.3,7.18, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));
//------------------------------------------------------------------------------------------------------ 

//------------------------  Positions Left pushs STATION 40 : OK----------------------------------------
#define AXIS_MOVE_PUSH_WHEEL_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 21.0, 42.0, 42.0, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40"));

#define AXIS_MOVE_PUSH_SPEED_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 21.0, 40.0, 24.0, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40"));

#define AXIS_MOVE_PUSH_SET_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 21.5, 38.0, 8.0, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40"));

#define AXIS_MOVE_PUSH_MINUS_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 20.0, 25.0, 6.0, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40"));

#define AXIS_MOVE_PUSH_PLUS_STATION_40(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 43.0, 25.1, 7.0, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40")); 

#define AXIS_MOVE_PUSH_CANCEL_STATION_40(check) \
  	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 44.5, 8.9, 17.9, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40"));

#define AXIS_MOVE_PUSH_RESUME_STATION_40(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 44.0, 12.3, 39.5, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40"));
//------------------------------------------------------------------------------------------------------
*/

																			 //Z     Y     X
//------------------------  Positions Right pushs STATION 30 : OK --------------------------------------
#define AXIS_MOVE_PUSH_PHONE_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 41.5, 39.5, 42.2, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));//OK

#define AXIS_MOVE_PUSH_LIST_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 42.0, 37.4, 22.5, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));//OK

#define AXIS_MOVE_PUSH_SPEECH_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 42.0, 35.0, 6.5, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));

#define AXIS_MOVE_PUSH_MENU_RIGHT_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_RIGHT], ptester, check, 43.5, 23.0, 5.5, "PEND_Z_RIGHT_STATION_30", "PEND_Y_RIGHT_STATION_30", "PEND_X_RIGHT_STATION_30"));

//------------------------  Positions Left pushs STATION 30 : OK --------------------------------------
#define AXIS_MOVE_PUSH_WHEEL_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 41.5, 7.61, 44.49, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));

#define AXIS_MOVE_PUSH_SPEED_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 41.0, 10.02, 25.59, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));

#define AXIS_MOVE_PUSH_SET_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 41.5, 13.0, 7.8, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));

#define AXIS_MOVE_PUSH_MINUS_STATION_30(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_30_LEFT], ptester, check, 42.5, 24.51, 6.7, "PEND_Z_LEFT_STATION_30", "PEND_Y_LEFT_STATION_30", "PEND_X_LEFT_STATION_30"));


//------------------------  Positions Right pushs STATION 40 : OK --------------------------------------
#define AXIS_MOVE_PUSH_SCROLL_WHEEL_STATION_40(check) \
	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 41.5, 22.0, 19.9, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));   //old y-3

#define AXIS_MOVE_PUSH_MENU_LEFT_STATION_40(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 43.5, 22.0, 40.0, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));  //old y-3	 //old x=39

#define AXIS_MOVE_PUSH_VOLUME_MINUS_STATION_40(check) \
  	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 44.5, 6.0, 35.0, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));   //old y-3   // old x=32

#define AXIS_MOVE_PUSH_VOLUME_PLUS_STATION_40(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_RIGHT], ptester, check, 44.5,9.3,10.0, "PEND_Z_RIGHT_STATION_40", "PEND_Y_RIGHT_STATION_40", "PEND_X_RIGHT_STATION_40"));		 //old y-3

//------------------------  Positions Left pushs STATION 40 : OK----------------------------------------
#define AXIS_MOVE_PUSH_PLUS_STATION_40(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 43.0, 24.8, 9.0, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40")); 

#define AXIS_MOVE_PUSH_CANCEL_STATION_40(check) \
  	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 44.0, 10.82, 15.2, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40"));  // old Z=44.5

#define AXIS_MOVE_PUSH_RESUME_STATION_40(check) \
   	EXCCHECK( MoveToPush(paxis[SYSTEM_AXIS_STATION_40_LEFT], ptester, check, 43.5, 12.9, 36, "PEND_Z_LEFT_STATION_40", "PEND_Y_LEFT_STATION_40", "PEND_X_LEFT_STATION_40"));   // old Z=44




/* Print a description of an LIN error/warning. */
NCTYPE_UINT8  INIT_ALL_FRAME[19][8]={
{0x61 ,0X06 ,0xB1 ,0x02 ,0x00 ,0x02 ,0x32 ,0x85},
{0x61 ,0x06 ,0XB1 ,0X02 ,0X00 ,0X03 ,0X32 ,0X03},
{0x61 ,0x06 ,0xB1 ,0x02 ,0x00 ,0x04 ,0x32 ,0xC4},
{0x2F ,0x06 ,0xB1 ,0x02 ,0x00 ,0x06 ,0x08 ,0x85},
{0x2F ,0x06 ,0xB1 ,0x02 ,0x00 ,0x03 ,0x08 ,0x03},
{0x2F ,0x06 ,0xB1 ,0x02 ,0x00 ,0x02 ,0x08 ,0x2B},
{0x2F ,0x06 ,0xB1 ,0x02 ,0x00 ,0x00 ,0x08 ,0x80},
{0x2F ,0x06 ,0xB1 ,0x02 ,0x00 ,0x05 ,0x08 ,0x14}, 
{0x2F ,0x06 ,0xB1 ,0x02 ,0x00 ,0x01 ,0x08 ,0x42},
{0x2F ,0x06 ,0xB1 ,0x02 ,0x00 ,0x04 ,0x08 ,0x20},
{0x2E ,0x06 ,0xB1 ,0xFF ,0x7F ,0x03 ,0x10 ,0x03},
{0x2E ,0x06 ,0xB1 ,0xFF ,0x7F ,0x00 ,0x10 ,0x2B},
{0x2E ,0x06 ,0xB1 ,0xFF ,0x7F ,0x01 ,0x10 ,0xC1},
{0x2E ,0x06 ,0xB1 ,0xFF ,0x7F ,0x02 ,0x10 ,0xA3},
{0x21 ,0x06 ,0xB1 ,0x02 ,0x00 ,0x00 ,0x13 ,0xE2},
{0x21 ,0x06 ,0xB1 ,0x02 ,0x00 ,0x01 ,0x13 ,0x2B},
{0x21 ,0x06 ,0xB1 ,0x02 ,0x00 ,0x02 ,0x13 ,0x1F},
{0x21 ,0x06 ,0xB1 ,0x02 ,0x00 ,0x03 ,0x13 ,0x61},
{0x21 ,0x06 ,0xB1 ,0x02 ,0x00 ,0x04 ,0x13 ,0x20}
}; 


/* Print a description of an LIN error/warning. */
/*NCTYPE_UINT8  INIT_ALL_FRAME[4][8]={
{0x2E ,0x06 ,0xB1 ,0xFF ,0x7F ,0x03 ,0x10 ,0x03},
{0x2E ,0x06 ,0xB1 ,0xFF ,0x7F ,0x00 ,0x10 ,0x2B},
{0x2E ,0x06 ,0xB1 ,0xFF ,0x7F ,0x01 ,0x10 ,0xC1},
{0x2E ,0x06 ,0xB1 ,0xFF ,0x7F ,0x02 ,0x10 ,0xA3}
}; */

#define CONNECT_RELAIS_CURRENT_100_UA\
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line1", 1)); \
	} while(0)

#define DISCONNECT_RELAIS_CURRENT_100_UA \
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line1", 0)); \
	} while(0)

STestSeq gs_TestSeq = ADD_TSI( MFL);
STestListItem gs_TestList[] = {
	
	ADD_TI( "00.10", MFL, 00_10),	/* Maintenance, Panel Connect */
	ADD_TI( "00.11", MFL, 00_11),	/* Maintenance, Panel Disconnect */
	
	/*-- STATION 20 */
	ADD_TI( "20.00", MFL, 20_00),	/* Cylinder Rodage Down */
	ADD_TI( "20.01", MFL, 20_01),	/* Cylinder Rodage Up */
	ADD_TI( "20.10", MFL, 20_10),	/* Check Dummy pushs */ 
	ADD_TI( "20.20", MFL, 20_20),	/* Rodage pushs */ 
	ADD_TI( "20.90", MFL, 20_90),	/* Table Rotation station 20 to  station 30 */
	
	
	/*-- STATION 30 */
 	ADD_TI( "30.00", MFL, 30_00),	/* Plug Panel */
	ADD_TI( "30.01", MFL, 30_01),	/* Check Plug Panel */
	ADD_TI( "30.02", MFL, 30_02),	/* Unplug Panel */
	ADD_TI( "30.03", MFL, 30_03),	/* Check Unplug Panel */
	ADD_TI( "30.04", MFL, 30_04),	/* Home Return all axis */
	
	
	ADD_TI( "30.10", MFL, 30_10),	/* Position Push RIGHT : BUTTOM_LEFT */
	ADD_TI( "30.11", MFL, 30_11),	/* Position Push RIGHT : BUTTOM_MIDDLE */
	ADD_TI( "30.12", MFL, 30_12),	/* Position Push RIGHT : BUTTOM_RIGHT */
	ADD_TI( "30.13", MFL, 30_13),	/* Position Push RIGHT : MIDDLE_RIGHT */
	
	ADD_TI( "30.20", MFL, 30_20),	/* Position Push LEFT : BUTTOM_RIGHT */
	ADD_TI( "30.21", MFL, 30_21),	/* Position Push LEFT : BUTTOM_MIDDLE */
	ADD_TI( "30.22", MFL, 30_22),	/* Position Push LEFT : BUTTOM_LEFT */
	ADD_TI( "30.23", MFL, 30_23),	/* Position Push LEFT : MIDDLE_LEFT*/
	
	ADD_TI( "30.30", MFL, 30_30),	/* LIN : run normal mode */
	ADD_TI( "30.31", MFL, 30_31),	/* LIN : stop normal mode */
	ADD_TI( "30.32", MFL, 30_32),	/* LIN : check alive byte */
	ADD_TI( "30.33", MFL, 30_33),	/* LIN : Check functions No activated */
	ADD_TI( "30.34", MFL, 30_34),	/* LIN : Write Variant code  F1/
									F2/F3/F4 */
	ADD_TI( "30.35", MFL, 30_35),	/* LIN : Read Variant code  F1/F2/F3/F4 */
	ADD_TI( "30.37", MFL, 30_37),	/* LIN : INIT_ALL Frames */
	ADD_TI( "30.38", MFL, 30_38),	/* LIN : Check DTC */
	
	ADD_TI( "30.50", MFL, 30_50),	/* LIN : check Valeo SW Revision */
	ADD_TI( "30.51", MFL, 30_51),	/* LIN : check SW Build Date */
	ADD_TI( "30.52", MFL, 30_52),	/* LIN : check tracability EMS */
	ADD_TI( "30.53", MFL, 30_53),	/* LIN : check tracability MMS */
	
	ADD_TI( "30.40", MFL, 30_40),	/* Parallel Test 2 Push  */
   	ADD_TI( "30.90", MFL, 30_90),	/* Table Rotation station 30 to  station 40 */

	/*-- STATION 40 */
	ADD_TI( "40.00", MFL, 40_00),	/* Plug Panel */
	ADD_TI( "40.01", MFL, 40_01),	/* Check Plug Panel */
	ADD_TI( "40.02", MFL, 40_02),	/* Unplug Panel */
	ADD_TI( "40.03", MFL, 40_03),	/* Check Unplug Panel */
	ADD_TI( "40.04", MFL, 40_04),	/* Home Return all axis */
	
	
	ADD_TI( "40.10", MFL, 40_10),	/* Position Push RIGHT : BUTTOM_LEFT */
	ADD_TI( "40.11", MFL, 40_11),	/* Position Push RIGHT : BUTTOM_MIDDLE */
	ADD_TI( "40.12", MFL, 40_12),	/* Position Push RIGHT : BUTTOM_RIGHT */
	ADD_TI( "40.13", MFL, 40_13),	/* Position Push RIGHT : MIDDLE_RIGHT */
	
	
	ADD_TI( "40.20", MFL, 40_20),	/* Position Push LEFT : BUTTOM_RIGHT */
	ADD_TI( "40.21", MFL, 40_21),	/* Position Push LEFT : BUTTOM_MIDDLE */
	ADD_TI( "40.22", MFL, 40_22),	/* Position Push LEFT : BUTTOM_LEFT */
	
	
	ADD_TI( "40.30", MFL, 40_30),	/* LIN : run normal mode */
	ADD_TI( "40.31", MFL, 40_31),	/* LIN : stop normal mode */
	ADD_TI( "40.32", MFL, 40_32),	/* LIN : check alive byte */
	ADD_TI( "40.33", MFL, 40_33),	/* LIN : Check functions No activated */
	ADD_TI( "40.34", MFL, 40_34),	/* LIN : Write Variant code  F1/F2/F3/F4 */
	ADD_TI( "40.35", MFL, 40_35),	/* LIN : Read Variant code  F1/F2/F3/F4 */
	ADD_TI( "40.37", MFL, 40_37),	/* LIN : INIT_ALL Frames */
	ADD_TI( "40.38", MFL, 40_38),	/* LIN : Check DTC */
	
	ADD_TI( "40.50", MFL, 40_50),	/* LIN : check Valeo SW Revision */
	ADD_TI( "40.51", MFL, 40_51),	/* LIN : check SW Build Date */
	ADD_TI( "40.52", MFL, 40_52),	/* LIN : check tracability EMS */
	ADD_TI( "40.53", MFL, 40_53),	/* LIN : check tracability MMS */

	ADD_TI( "40.40", MFL, 40_40),	/* Parallel Test 2 Push  */
   	ADD_TI( "40.90", MFL, 40_90),	/* Table Rotation station 30 to  station 40 */
	

	/*-- STATION 50 */
	ADD_TI( "50.00", MFL, 50_00),	/* Cylinder Rodage Down */
	ADD_TI( "50.01", MFL, 50_01),	/* Cylinder Rodage Up */
	ADD_TI( "50.10", MFL, 50_10),	/* Rodage wheels */
	ADD_TI( "50.90", MFL, 50_90),	/* Table Rotation station 50 to  station 60 */

	/*-- STATION 60 */
	ADD_TI( "60.00", MFL, 60_00),	/* Plug Panel */
	ADD_TI( "60.01", MFL, 60_01),	/* Check Plug Panel */
	ADD_TI( "60.02", MFL, 60_02),	/* Unlug Panel */
	ADD_TI( "60.03", MFL, 60_03),	/* Check Unlug Panel */
	ADD_TI( "60.04", MFL, 60_04),	/* Cylinder Test Down */
	ADD_TI( "60.05", MFL, 60_05),	/* Check Cylinder Test Down */
	ADD_TI( "60.06", MFL, 60_06),	/* Cylinder Test Up */
	ADD_TI( "60.07", MFL, 60_07),	/* Check Cylinder Test Up */
	ADD_TI( "60.10", MFL, 60_10),	/* Parallel Test WIPE / SCROLL_WHEEL */
	ADD_TI( "60.30", MFL, 60_30),	/* LIN : run normal mode  */
	ADD_TI( "60.31", MFL, 60_31),	/* LIN : stop normal mode */
	ADD_TI( "60.32", MFL, 60_32),	/* LIN : check alive byte Station 60 */
	ADD_TI( "60.50", MFL, 60_50),	/* reset Sensor motor LEFT */
	ADD_TI( "60.51", MFL, 60_51),	/* reset Sensor motor LEFT no check */
	ADD_TI( "60.90", MFL, 60_90),	/* Table Rotation station 60 to  station 70 */

	/*-- STATION 70 */
 	ADD_TI( "70.00", MFL, 70_00),	/* Plug Panel in Station 70 */
	ADD_TI( "70.01", MFL, 70_01),	/* Unplug Panel in Station 70 */
	ADD_TI( "70.02", MFL, 70_02),	/* illumination ON */
	ADD_TI( "70.03", MFL, 70_03),	/* illumination OFF */
	ADD_TI( "70.04", MFL, 70_04),	/* LIN : run normal mode  */
	ADD_TI( "70.05", MFL, 70_05),	/* LIN : stop normal mode */
	ADD_TI( "70.08", MFL, 70_08),	/* LIN : check alive byte  */
	ADD_TI( "70.09", MFL, 70_09),	/* LIN : Go to Sleep */
	ADD_TI( "70.10", MFL, 70_10),	/* Set Job */
	ADD_TI( "70.11", MFL, 70_11),	/* test day picto */
	ADD_TI( "70.12", MFL, 70_12),	/* test Chrome */
	ADD_TI( "70.13", MFL, 70_13),	/* test night picto */
	ADD_TI( "70.14", MFL, 70_14),	/* test LED STA */
	ADD_TI( "70.15", MFL, 70_15),	/* visal check : live mode */
	ADD_TI( "70.20", MFL, 70_20),	/*  DMM : Read current */
	ADD_TI( "70.21", MFL, 70_21),	/*  DMM : Read current with Retest*/ 
	ADD_TI( "70.50", MFL, 70_50),	/*  LIN : illuminate LED_BG */
	ADD_TI( "70.60", MFL, 70_60),	/*  LIN : illuminate LED_STA */
	ADD_TI( "70.70", MFL, 70_70),	/*  LIN : Check DTC */
	ADD_TI( "70.80", MFL, 70_80),	/* LIN : Write BMW Part Number */
	ADD_TI( "70.81", MFL, 70_81),	/* LIN : Write BMW SW Revision */
	ADD_TI( "70.82", MFL, 70_82),	/* LIN : Write BMW HW Revision */
	ADD_TI( "70.83", MFL, 70_83),	/* LIN : check BMW Part Number */
	ADD_TI( "70.84", MFL, 70_84),	/* LIN : check BMW SW Revision */
	ADD_TI( "70.85", MFL, 70_85),	/* LIN : check BMW HW Revision */
	ADD_TI( "70.86", MFL, 70_86),	/* LIN : Write MAN DATA 2 */
	ADD_TI( "70.87", MFL, 70_87),	/* LIN : Check MAN DATA 2 */
	ADD_TI( "70.90", MFL, 70_90),	/* Table Rotation station 70 to station 80 */

	ADD_TI_LAST_ENTRY
};

typedef struct _SMFLConfig {
	double			KL30;
	double			TypicalCurrent;
	double			BlowerCurrent;
} SMFLConfig, *SMFLConfigPtr;

static SMFLConfig	gs_MFL = {TEST_VOLTAGE, TEST_CURRENT}; 

/*#undef __FUNC__
#define __FUNC__ "CheckDirectory"
void CheckDirectory(const char* directory)
{
	int					size;

	FileExists (directory, &size);
	if(size==-1)
	{
		// create directory 	
		 MakeDir(directory);
	}
}   */

#undef __FUNC__
#define __FUNC__	"LogTest_Steps"
void LogTest_Steps(char *Test_Step)   
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
	char				log[256];
	
	
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\Test_Steps\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	
	/* enreg des data */
	
	memset(log, 0x00, 256);
	
	sprintf (fileName, "%s/Log_Test_Steps.txt", result_dir);
	
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	
//	sprintf (log, "18: 30 -- hello && it is me");
//	sprintf (log, sdate_time, DATE_TIME_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf (log,"---------------------------------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\nTest_Step=%s\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,Test_Step);  
//	sprintf (log, "IS_BARCODE_REAR=%d\r\n", is_barcode_rear); 
//	sprintf (log, "SENSOR_CYLINDER_X_REAR_STATION_80=", is_cylinder_x_rear); 
	
	
	WriteLine (HandleFile, log, sizeof (log));	
	
	CloseFile (HandleFile);
}
/*********************************************************************************
 * Function Testsequence Init
 *********************************************************************************/
FCE_TESTSEQUENCE_INIT(MFL)
{
	TESTSEQUENCEINIT_BEGIN();
	
	PANEL_INI(STATION_20);
	((STestParamPtr)pTID)->Write_Tracability_MMS = FALSE;
	sprintf(((STestParamPtr)pTID)->Production_Serial, "none");


	
Error:
	TESTSEQUENCEINIT_END();
}

/*********************************************************************************
 * Function Testsequence Cleanup
 *********************************************************************************/
FCE_TESTSEQUENCE_CLEANUP(MFL)
{
	TESTSEQUENCECLEANUP_BEGIN(); 
	TEST_USE(DAQ);

	PANEL_CLEAN();
	INITIALIZE(DAQ);

	//checkErr(eldaq_StopMeasurement(vi_DAQ));

		
Error:
	TESTSEQUENCECLEANUP_END();
}

/*********************************************************************************
 * Function 00_10
 *********************************************************************************/
FCE_HELP(MFL, 00_10)
{
    HELP_BEGIN();
    HELP_TEST( "Maintenance, Panel Connect" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 00_10)
{
    TESTREGUIRE_BEGIN();

Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 00_10)
{
    TESTINIT_BEGIN();

	
Error:    
    TESTINIT_END(STATION_10);
}

FCE_TEST(MFL, 00_10)
{
    TEST_BEGIN();

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 00_10)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_10);
}

/*********************************************************************************
 * Function 00_11
 *********************************************************************************/
FCE_HELP(MFL, 00_11)
{
    HELP_BEGIN();
    HELP_TEST( "Maintenance, Panel Disconnect" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 00_11)
{
    TESTREGUIRE_BEGIN();
    
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 00_11)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_10);
}

FCE_TEST(MFL, 00_11)
{
    TEST_BEGIN();

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 00_11)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_10);
}

/*********************************************************************************
 * Function 00_90
 *********************************************************************************/
FCE_HELP(MFL, 00_90)
{
    HELP_BEGIN();
    HELP_TEST( "Maintenance, Simulation" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 00_90)
{
    TESTREGUIRE_BEGIN();

Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 00_90)
{
    TESTINIT_BEGIN();

	
Error:    
    TESTINIT_END(STATION_10);
}

FCE_TEST(MFL, 00_90)
{
    TEST_BEGIN();
	
	Sleep (5000);

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 00_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_10);
}

/*********************************************************************************
 * Function 20_00
*********************************************************************************/

FCE_HELP(MFL, 20_00)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Cylinder Rodage down" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 20_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 20_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(MFL, 20_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(DAQ);

	INITIALIZE(DAQ);

	TESTER_START_RODAGE_STATION_20(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(MFL, 20_00)
{																	  
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 20_01
*********************************************************************************/

FCE_HELP(MFL, 20_01)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Cylinder Rodage up" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 20_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 20_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(MFL, 20_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	clock_t 					_start_test_clock;
	double   					time;
	double   					timeout = 15000;

	/* Semaphore Rodage */
	_start_test_clock =  clock();
#if 0    /* formerly excluded lines */
	do {
	Sleep(100);
	ProcessDrawEvents ();
	ProcessSystemEvents ();
	
	time = (double)(clock()-_start_test_clock);

	}while ((Inhibit_Rodage_Pushs[STATION_30] == TRUE ||
		     Inhibit_Rodage_Pushs[STATION_40] == TRUE ||
			// Inhibit_Rodage_Pushs[STATION_50] == TRUE ||
			// Inhibit_Rodage_Pushs[STATION_60] == TRUE ||
			 Inhibit_Rodage_Pushs[STATION_70] == TRUE) && time < timeout);
#endif   /* formerly excluded lines */
	/*
	if (time >= timeout)
	{
	printf ("Inhibit_Rodage_Pushs[STATION_30] == %d\r\n", Inhibit_Rodage_Pushs[STATION_30]);
	printf ("Inhibit_Rodage_Pushs[STATION_40] == %d\r\n", Inhibit_Rodage_Pushs[STATION_40]);
	printf ("Inhibit_Rodage_Pushs[STATION_50] == %d\r\n", Inhibit_Rodage_Pushs[STATION_50]);
	printf ("Inhibit_Rodage_Pushs[STATION_60] == %d\r\n", Inhibit_Rodage_Pushs[STATION_60]);
	printf ("Inhibit_Rodage_Pushs[STATION_70] == %d\r\n", Inhibit_Rodage_Pushs[STATION_70]);		
	}  */
	
	TESTER_STOP_RODAGE_STATION_20(TIMEOUT_DEFAULT);

Error:
	//Rodage_Pushs_Run = FALSE;
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(MFL, 20_01)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 20_10
*********************************************************************************/

FCE_HELP(MFL, 20_10)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Check Dummy" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 20_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 20_10)									
{
    TESTINIT_BEGIN();


Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(MFL, 20_10)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	STRING( status, 7+1);
	VARIABLE(UINT32, push);

	
	TESTER_CHECK_DUMMY_STATION_20(TIMEOUT_DEFAULT, status);

	for (push=0; push<7; push++)
	{
	//printf ("status[%d] = %d\r\n", push, status[push]);
	LIMIT_CHECK(INT32, 	status[push]);
	}

	
Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(MFL, 20_10)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 20_20
*********************************************************************************/

FCE_HELP(MFL, 20_20)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Rodage" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 20_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 20_20)									
{
    TESTINIT_BEGIN();


Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(MFL, 20_20)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	STRING( version, 15+1);
	VARIABLE(UINT32, TimeOn);
	VARIABLE(UINT32, count);
	VARIABLE(UINT32, Enable_Push_1);
	VARIABLE(UINT32, Enable_Push_2);
	VARIABLE(UINT32, Enable_Push_3);
	VARIABLE(UINT32, Enable_Push_4);
	VARIABLE(UINT32, Enable_Push_5);
	VARIABLE(UINT32, Enable_Push_6);
	VARIABLE(UINT32, Enable_Push_7);
	VARIABLE(UINT32, Enable_Push_8);
	VARIABLE(UINT32, Enable_Push_9);
	VARIABLE(UINT32, Enable_Push_10);
	VARIABLE(UINT32, Enable_Push_11);
	VARIABLE(UINT32, Enable_Push_12);
	VARIABLE(UINT32, Enable_Push_13);
	VARIABLE(UINT32, Enable_Push_14);
	VARIABLE(UINT32, Enable_Push_15);


	
	PARAM_UINT32_OPTIONAL("TimeOn", &TimeOn, 200);
	PARAM_UINT32_OPTIONAL("count", &count, 4);
	PARAM_UINT32_OPTIONAL("Enable_Push_1", &Enable_Push_1, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_2", &Enable_Push_2, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_3", &Enable_Push_3, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_4", &Enable_Push_4, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_5", &Enable_Push_5, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_6", &Enable_Push_6, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_7", &Enable_Push_7, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_8", &Enable_Push_8, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_9", &Enable_Push_9, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_10", &Enable_Push_10, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_11", &Enable_Push_11, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_12", &Enable_Push_12, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_13", &Enable_Push_13, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_14", &Enable_Push_14, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_15", &Enable_Push_15, 1);
	
	version[0] = Enable_Push_1;	
	version[1] = Enable_Push_2;	
	version[2] = Enable_Push_3;	
	version[3] = Enable_Push_4;	
	version[4] = Enable_Push_5;	
	version[5] = Enable_Push_6;	
	version[6] = Enable_Push_7;	
	version[7] = Enable_Push_8;
	version[8] = Enable_Push_9;	
	version[9] = Enable_Push_10;	
	version[10] = Enable_Push_11;	
	version[12] = Enable_Push_12;	
	version[13] = Enable_Push_13;	
	version[14] = Enable_Push_14;	
	version[15] = Enable_Push_15;	
	
	TimeOn  = 100;
	count = 10;
	
	TESTER_RODAGE_STATION_20(TIMEOUT_DEFAULT,count,TimeOn,0,version);
	
Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(MFL, 20_20)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 20_90
 *********************************************************************************/
FCE_HELP(MFL, 20_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 20_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 20_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(MFL, 20_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	/* wait for table finish rotation */
	DISPLAY_TESTSEQ_RESULT(STATION_20);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_30);

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 20_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 30_00
*********************************************************************************/

FCE_HELP(MFL, 30_00)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	Inhibit_Rodage_Pushs[STATION_30] = TRUE;
	TESTER_PLUG_PANEL_STATION_30(0);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(MFL, 30_00)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}
/*********************************************************************************
 * Function 30_01
*********************************************************************************/

FCE_HELP(MFL, 30_01)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_30(TIMEOUT_DEFAULT);

Error: 
	if (pexception)
		TESTER_UNPLUG_PANEL_STATION_30 (TIMEOUT_DEFAULT);
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(MFL, 30_01)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_02
*********************************************************************************/

FCE_HELP(MFL, 30_02)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_02)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_02)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_02)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_30(0);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_02)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_03
*********************************************************************************/

FCE_HELP(MFL, 30_03)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_03)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_03)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_03)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_30(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_03)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_04
*********************************************************************************/

FCE_HELP(MFL, 30_04)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_04)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_04)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_04)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
																  
	INITIALIZE_AXIS(SYSTEM_AXIS_STATION_30_RIGHT);
	INITIALIZE_AXIS(SYSTEM_AXIS_STATION_30_LEFT);
	
	AXIS_HOME_ALL_STATION_30(FALSE);
	
Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_04)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_40
*********************************************************************************/

FCE_HELP(MFL, 30_40)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_40)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_40)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_40)
{
	TEST_BEGIN();
    SElExceptionPtr     pexception2 = NULL;
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	SAquisitionThreadDataPtr	pthread_data[2];
	HANDLE  					hThreadAquisition[2];
	int  	read[2]= {0,0};
	double 	sdWaveGes[3][MESS_BUF][BUFFERLEN] = {0,0,0,0};
	double  Start_push_position[2]= {0,0};
	char    AnalogChannel[2][32];
	char    name[2][32];
	char    position_ADC[32];
	char 	fileName[512];
	char 	filePath[512];
	int     i,j;
	int 	status;

	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmS3V;
	double dmF_END;
	double dmS_END;
	double dmF1R;
	double dmS1R;
	double dmF2R;
	double dmS2R;
	double TactileRatio, Ce;
	double x_vect[2], y_vect[2]; 
    int IndexContact = 0;
	int station = STATION_30;
	char sValue[128];
	char log_dir[512];
	double Last_position[2];
	
	
	INITIALIZE(DAQ);
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	PARAM_STRING_OPTIONAL("push_name_right", name[0], "none");
	PARAM_STRING_OPTIONAL("push_name_left", name[1], "none");
	PARAM_STRING_OPTIONAL("position_ADC", position_ADC, "none");
								
	iFlag_Delete_Cache [station] = TRUE;

		
	sprintf (AnalogChannel[0], "ai1");
	sprintf (AnalogChannel[1], "ai2");

	paxis[SYSTEM_AXIS_STATION_30_RIGHT]->CurrentPosition(paxis[SYSTEM_AXIS_STATION_30_RIGHT], AXIS_Z, &Last_position[SYSTEM_AXIS_STATION_30_RIGHT]);//Right
	paxis[SYSTEM_AXIS_STATION_30_LEFT]->CurrentPosition(paxis[SYSTEM_AXIS_STATION_30_LEFT], AXIS_Z, &Last_position[SYSTEM_AXIS_STATION_30_LEFT]); //Left

		
	/* Start thread press push direction + */
	for (i=0; i<2; i++)
	{
		if (strcmp (name[i], "none"))
		{
		pthread_data[i] = (SAquisitionThreadDataPtr)calloc(1, sizeof(SAquisitionThreadData)); 
		EXCCHECKALLOC( pthread_data);
		pthread_data[i]->vi_DAQ = vi_DAQ;
		pthread_data[i]->paxis = paxis[i];
		pthread_data[i]->pcan = pcan;
		pthread_data[i]->timeout = 9000;
		pthread_data[i]->index = i;
		pthread_data[i]->station = station;
		pthread_data[i]->position_forward = 46.5;
		pthread_data[i]->position_backward = 40.5;
		sprintf (pthread_data[i]->push_name , name[i]);
		sprintf (pthread_data[i]->Channel, "%s\0",AnalogChannel[i]);
		EXCCHECK(ThreadStart(Thread_Aquisition_Force, pthread_data[i], &hThreadAquisition[i], NULL));
		}
	}
	
	/* Sleep thread to finish */
	for (i=0; i< 2 ; i++)
	{
		if (strcmp (name[i], "none"))
		{
			pexception = ThreadStatus(hThreadAquisition[i], 9000, &status);
#if 0    /* formerly excluded lines */
			if(pexception)
			{
			EXCDELETE(pexception);
			EXCTHROW(-1, "ThreadStatus : Aquisition Timeout");
			}
#endif   /* formerly excluded lines */
	
			read[i] = pthread_data[i]->read;
			for (j=0; j<pthread_data[i]->read; j++)
			{
			 sdWaveGes[i][MESS_KRAFT][j] = pthread_data[i]->data[MESS_KRAFT][j];	
			 sdWaveGes[i][MESS_WEG][j] = pthread_data[i]->data[MESS_WEG][j];
		     sdWaveGes[i][MESS_CONTACT][j] = pthread_data[i]->data[MESS_CONTACT][j];
			}
		}
	}
																									
	
#if 0    /* formerly excluded lines */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
		{
			sprintf (log_dir, "%s\\STEP_%d", ((STestParamPtr)pTID)->csv_dir, ((STestParamPtr)pTID)->step);
			EXCCHECK( CheckDirectory(log_dir));
			/* save array to file right */
			if (strcmp (name[0], "none"))
			{
			sprintf(fileName, "%s\\STATION_30_RIGHT_%s_ESSAI_%d.txt",log_dir, "Stroke", ((STestParamPtr)pTID)->RnR_counter);
			ArrayToFile (fileName, sdWaveGes[0][MESS_WEG], VAL_DOUBLE, pthread_data[0]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
			sprintf(fileName, "%s\\STATION_30_RIGHT_%s_ESSAI_%d.txt",log_dir, "Force", ((STestParamPtr)pTID)->RnR_counter);
			ArrayToFile (fileName, sdWaveGes[0][MESS_KRAFT], VAL_DOUBLE, pthread_data[0]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
			}
	
			/* save array to file left */
			if (strcmp (name[1], "none"))
			{
			sprintf(fileName, "%s\\STATION_30_LEFT_%s_ESSAI_%d.txt",log_dir, "Stroke", ((STestParamPtr)pTID)->RnR_counter);
			ArrayToFile (fileName, sdWaveGes[1][MESS_WEG], VAL_DOUBLE, pthread_data[1]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
			sprintf(fileName, "%s\\STATION_30_LEFT_%s_ESSAI_%d.txt",log_dir, "Force", ((STestParamPtr)pTID)->RnR_counter);
			ArrayToFile (fileName, sdWaveGes[1][MESS_KRAFT], VAL_DOUBLE, pthread_data[1]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
			}
		}
			
	else 
	{
		sprintf (log_dir, "log\\%s", ((STestParamPtr)pTID)->Date);
		EXCCHECK( CheckDirectory(log_dir));
		sprintf (log_dir, "%s\\STEP %d", log_dir, ((STestParamPtr)pTID)->step);
		EXCCHECK( CheckDirectory(log_dir));
		/* save array to file right */
		if (strcmp (name[0], "none"))
		{
		sprintf(fileName, "%s\\STATION_30_RIGHT_%s_%s.txt",log_dir, "Stroke", ((STestParamPtr)pTID)->Time);
		ArrayToFile (fileName, sdWaveGes[0][MESS_WEG], VAL_DOUBLE, pthread_data[0]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
		sprintf(fileName, "%s\\STATION_30_RIGHT_%s_%s.txt",log_dir, "Force", ((STestParamPtr)pTID)->Time);
		ArrayToFile (fileName, sdWaveGes[0][MESS_KRAFT], VAL_DOUBLE, pthread_data[0]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
		}
		
		/* save array to file left */
		if (strcmp (name[1], "none"))
		{
		sprintf(fileName, "%s\\STATION_30_LEFT_%s_%s.txt",log_dir, "Stroke", ((STestParamPtr)pTID)->Time);
		ArrayToFile (fileName, sdWaveGes[1][MESS_WEG], VAL_DOUBLE, pthread_data[1]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
		sprintf(fileName, "%s\\STATION_30_LEFT_%s_%s.txt",log_dir, "Force", ((STestParamPtr)pTID)->Time);
		ArrayToFile (fileName, sdWaveGes[1][MESS_KRAFT], VAL_DOUBLE, pthread_data[1]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
		}

	}
#endif   /* formerly excluded lines */


	/* add index palett */
	SetCtrlVal (gs_TestPanel2, PANEL_2_PALETT_STATION_30, ((STestParamPtr)pTID)->PALETT);

	if (strcmp (name[0], "none"))
	{
		 
		//TEST_GRAPH_RIGHT_STATION_30 (name[0], sdWaveGes[0], read[0]);

		/* Moyenne glissante */
		//Moyenne_Glissante_Courbe_30 (sdWaveGes[0][MESS_KRAFT], read[0]);
		
		/* Analyse Waveform Force/Stroke Right*/
		AnalysisFunction (sdWaveGes[0],
					  	  read[0], 
				  	  	  &dmF1V, &dmS1V, 
					  	  &dmF2V, &dmS2V, 
					  	  &dmF_END, &dmS_END, 
					  	  &dmF1R, &dmS1R, 
					 	  &dmF2R, &dmS2R);
		
		/* Plot Graph & reults */
		for (i=0; i<read[0];i++)
			sdWaveGes[0][MESS_CONTACT][i] = sdWaveGes[0][MESS_CONTACT][i] * SCALE_BIT;
		TEST_GRAPH_RIGHT_STATION_30 (name[0], sdWaveGes[0], read[0]);
		TEST_GRAPH_MESS_RIGHT_STATION_30 (sdWaveGes[0], dmS1V, dmF1V, VAL_RED);
		TEST_GRAPH_MESS_RIGHT_STATION_30 (sdWaveGes[0], dmS2V, dmF2V, VAL_GREEN);
		TEST_GRAPH_MESS_RIGHT_STATION_30 (sdWaveGes[0], dmS_END, dmF_END, VAL_MAGENTA);
		

		/* display result in the Graph */
		sprintf (sValue, "%0.2f N", dmF1V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F1_RIGHT_STATION_30, sValue);
		sprintf (sValue, "%0.2f mm", dmS1V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S1_RIGHT_STATION_30, sValue);
		
		sprintf (sValue, "%0.2f N", dmF2V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F2_RIGHT_STATION_30, sValue);
		sprintf (sValue, "%0.2f mm", dmS2V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S2_RIGHT_STATION_30, sValue);
		
		sprintf (sValue, "%0.2f N", dmF1V+1.0);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F3_RIGHT_STATION_30, sValue);
		sprintf (sValue, "%0.2f mm", dmS_END);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S3_RIGHT_STATION_30, sValue);
		
		/* offset */
	    Ajustement_Gain_Offset_30(name[0], &dmF1V, &dmF2V, &dmS_END);
		
		/* display result in the Graph */
		sprintf (sValue, "%0.2f N", dmF1V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F1_RIGHT_STATION_31, sValue);
		sprintf (sValue, "%0.2f mm", dmS1V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S1_RIGHT_STATION_31, sValue);
		
		sprintf (sValue, "%0.2f N", dmF2V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F2_RIGHT_STATION_31, sValue);
		sprintf (sValue, "%0.2f mm", dmS2V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S2_RIGHT_STATION_31, sValue);
		
		sprintf (sValue, "%0.2f N", dmF1V+1.0);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F3_RIGHT_STATION_31, sValue);
		sprintf (sValue, "%0.2f mm", dmS_END);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S3_RIGHT_STATION_31, sValue);
	}
	
	else 
	{
		DELETE_GRAPH_RIGHT_STATION_30 ("none");
	}

	
	if (strcmp (name[1], "none"))
	{
		
		//TEST_GRAPH_LEFT_STATION_30 (name[1], sdWaveGes[1], read[1]);
		
		/* Moyenne glissante */
		//Moyenne_Glissante_Courbe_30 (sdWaveGes[1][MESS_KRAFT], read[1]);
		
		/* Analyse Waveform Force/Stroke Left*/
		AnalysisFunction (sdWaveGes[1],
					  	  read[1], 
				  	  	  &dmF1V, &dmS1V, 
					  	  &dmF2V, &dmS2V, 
					  	  &dmF_END, &dmS_END, 
					  	  &dmF1R, &dmS1R, 
					 	  &dmF2R, &dmS2R);
		
		/* Plot Graph & reults */
		for (i=0; i<read[1];i++)
			sdWaveGes[1][MESS_CONTACT][i] = sdWaveGes[1][MESS_CONTACT][i] * SCALE_BIT;
		TEST_GRAPH_LEFT_STATION_30 (name[1], sdWaveGes[1], read[1]);
		TEST_GRAPH_MESS_LEFT_STATION_30 (sdWaveGes[1], dmS1V, dmF1V, VAL_RED);
		TEST_GRAPH_MESS_LEFT_STATION_30 (sdWaveGes[1], dmS2V, dmF2V, VAL_GREEN);
		TEST_GRAPH_MESS_LEFT_STATION_30 (sdWaveGes[1], dmS_END, dmF_END, VAL_MAGENTA);
		
		/* display result in the Graph */
	/*	sprintf (sValue, "%0.2f N", dmF1V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F1_LEFT_STATION_30, sValue);
		sprintf (sValue, "%0.2f mm", dmS1V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S1_LEFT_STATION_30, sValue);
		
		sprintf (sValue, "%0.2f N", dmF2V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F2_LEFT_STATION_30, sValue);
		sprintf (sValue, "%0.2f mm", dmS2V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S2_LEFT_STATION_30, sValue);
		
		sprintf (sValue, "%0.2f N", dmF1V+1.0);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F3_LEFT_STATION_30, sValue);
		sprintf (sValue, "%0.2f mm", dmS_END);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S3_LEFT_STATION_30, sValue);
		*/
		/* offset */
	    Ajustement_Gain_Offset_30(position_ADC, &dmF1V, &dmF2V, &dmS_END);
		
		/* display result in the Graph */
		sprintf (sValue, "%0.2f N", dmF1V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F1_LEFT_STATION_31, sValue);
		sprintf (sValue, "%0.2f mm", dmS1V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S1_LEFT_STATION_31, sValue);
		
		sprintf (sValue, "%0.2f N", dmF2V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F2_LEFT_STATION_31, sValue);
		sprintf (sValue, "%0.2f mm", dmS2V);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S2_LEFT_STATION_31, sValue);
		
		sprintf (sValue, "%0.2f N", dmF1V+1.0);
		SetCtrlVal (gs_TestPanel2, PANEL_2_F3_LEFT_STATION_31, sValue);
		sprintf (sValue, "%0.2f mm", dmS_END);
		SetCtrlVal (gs_TestPanel2, PANEL_2_S3_LEFT_STATION_31, sValue);   
	}
	
	else 
	{
		DELETE_GRAPH_LEFT_STATION_30 ("none");
	}

	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	sprintf (log_dir, "%s\\STEP_%d", ((STestParamPtr)pTID)->csv_dir, ((STestParamPtr)pTID)->step);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	//sprintf(filePath, "%s\\%s",((STestParamPtr)pTID)->TicketDirPath, fileName);
	CONVERT_GRAPH_TO_PNG_STATION_30(fileName);
	TEST_VALUE_SET( "image", fileName);
	}
	else  //if (((STestParamPtr)pTID)->fault_retest == FALSE)
	{
	sprintf(fileName, "%s_%s_%d.png", ((STestParamPtr)pTID)->Production_Serial, ((STestParamPtr)pTID)->Time, ((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",((STestParamPtr)pTID)->TicketDirPath, fileName);
	CONVERT_GRAPH_TO_PNG_STATION_30(filePath);
	//SET_VALUE_SET( "image", fileName);
	}

	
	/* Limit check right */
	if (strcmp (name[0], "none"))
	{
		/* Analyse Waveform Force/Stroke Right*/
		AnalysisFunction (sdWaveGes[0],
					  	  read[0], 
				  	  	  &dmF1V, &dmS1V, 
					  	  &dmF2V, &dmS2V, 
					  	  &dmF_END, &dmS_END, 
					  	  &dmF1R, &dmS1R, 
					 	  &dmF2R, &dmS2R);

		/* offset */
	    Ajustement_Gain_Offset_30(name[0], &dmF1V, &dmF2V, &dmS_END);
		
		/* Peak force F1 */
		LIMIT_CHECK(REAL64, dmF1V);
	
		/* Travel to peak Force S3 */
		//LIMIT_CHECK(REAL64, dmS_END);
		LIMIT_CHECK_EXT(REAL64, dmS_END, pexception2);
		
		/* Tactile Ratio (F1-F2)/F1 */
		TactileRatio = ((dmF1V-dmF2V)/dmF1V) * 100;
		LIMIT_CHECK(REAL64, TactileRatio);
	
		/* Travel to contact Ce */
		IndexContact = NAN;
		
		for (i=1; i< read[0]; i++)
			{
			if (sdWaveGes[0][MESS_CONTACT][i] == SCALE_BIT && 
				sdWaveGes[0][MESS_CONTACT][i] != sdWaveGes[0][MESS_CONTACT][i-1]) 
				{
				IndexContact = i;
				break;
				}
			}
	
		if (IndexContact == NAN)
			Ce = NAN;
		else 
			Ce = sdWaveGes[0][MESS_WEG][IndexContact];
		
		LIMIT_CHECK(REAL64, Ce);
	}
	
	else // don't use the limits of right push in the push left (if push right is dummy : none)
	{
	_test_index = _test_index + 4;	
	}
	
	/* Limit check left */
	if (strcmp (name[1], "none"))
	{
		/* Analyse Waveform Force/Stroke Left*/
		AnalysisFunction (sdWaveGes[1],
					  	  read[1], 
				  	  	  &dmF1V, &dmS1V, 
					  	  &dmF2V, &dmS2V, 
					  	  &dmF_END, &dmS_END, 
					  	  &dmF1R, &dmS1R, 
					 	  &dmF2R, &dmS2R);
		
		/* offset */
	    Ajustement_Gain_Offset_30(position_ADC, &dmF1V, &dmF2V, &dmS_END);
		
		/* Peak force F1 */
		LIMIT_CHECK(REAL64, dmF1V);
	
		/* Travel to peak Force S3 */
		//LIMIT_CHECK(REAL64, dmS_END);
		LIMIT_CHECK_EXT(REAL64, dmS_END, pexception2);

		/* Tactile Ratio (F1-F2)/F1 */
		TactileRatio = ((dmF1V-dmF2V)/dmF1V) * 100;
		if (dmF1V == 0) TactileRatio = 0; 
		if (TactileRatio == NAN) TactileRatio = 0; 
		LIMIT_CHECK(REAL64, TactileRatio);
	
		/* Travel to contact Ce */
		IndexContact = NAN;
		
		for (i=1; i< read[1]; i++)
			{
			if (sdWaveGes[1][MESS_CONTACT][i] == SCALE_BIT && 
				sdWaveGes[1][MESS_CONTACT][i] != sdWaveGes[1][MESS_CONTACT][i-1]) 
				{
				IndexContact = i;
				break;
				}
			}
	
		if (IndexContact == NAN)
			Ce = NAN;
		else 
			Ce = sdWaveGes[1][MESS_WEG][IndexContact];
		
		LIMIT_CHECK(REAL64, Ce);


#if 0    /* formerly excluded lines */
		/* ADC check Value */
		if (strcmp (position_ADC, "L_BOTTOM_LEFT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_BOTTOM_LEFT]);
		}
		
		else if (strcmp (position_ADC, "L_BOTTOM_MID") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_BOTTOM_MID]);
		}
		
		else if  (strcmp (position_ADC, "L_BOTTOM_RIGHT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_BOTTOM_RIGHT]);
		}

	    else if  (strcmp (position_ADC, "L_MID_LEFT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_MID_LEFT]);
		}

	    else if  (strcmp (position_ADC, "L_MID_RIGHT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_MID_RIGHT]);
		}

	    else if  (strcmp (position_ADC, "L_TOP_LEFT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_TOP_LEFT]);
		}
		
	    else if  (strcmp (position_ADC, "L_TOP_RIGHT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_TOP_RIGHT]);
		}
		
		else
		{
			EXCTHROW(TEST_ERROR_UNKNOWN_CONFIG_VARIABLE, "TEST_ERROR_UNKNOWN_CONFIG_VARIABLE"); 
		}
#endif   /* formerly excluded lines */
	}

	else // don't use the limits of right push in the push left (if push right is dummy : none)
	{
	_test_index = _test_index + 5;	
	} 
		

	/* Short circuit check right */
	if (strcmp (name[0], "none"))
	{
		if (strcmp (name[0], "PHONE"))
		{
		LIMIT_CREATE_EX ("0","0", "[PHONE_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][PHONE]);
		}
		if (strcmp (name[0], "LIST"))
		{
		LIMIT_CREATE_EX ("0","0", "[LIST_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][LIST]);
		}
		if (strcmp (name[0], "SPEECH"))
		{
		LIMIT_CREATE_EX ("0","0", "[SPEECH_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][SPEECH]);
		}
		if (strcmp (name[0], "MENU_RIGHT"))
		{
		LIMIT_CREATE_EX ("0","0", "[MENU_RIGHT_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][MENU_RIGHT]);
		}
		if (strcmp (name[0], "SCROLL_WHEEL"))
		{
		LIMIT_CREATE_EX ("0","0", "[SCROLL_WHEEL_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][SCROLL_WHEEL]);
		}
		if (strcmp (name[0], "VOLUME_PLUS"))
		{
		LIMIT_CREATE_EX ("0","0", "[VOLUME_PLUS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][VOLUME_PLUS]);
		}
		if (strcmp (name[0], "VOLUME_MINUS"))
		{
		LIMIT_CREATE_EX ("0","0", "[VOLUME_MINUS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][VOLUME_MINUS]);
		}
	}

	/* Short circuit check left */
	if (strcmp (name[1], "none"))
	{
		if (strcmp (name[1], "MODE"))
		{
		LIMIT_CREATE_EX ("0","0", "[MODE_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][MODE]);
		}
		if (strcmp (name[1], "ASS"))
		{
		LIMIT_CREATE_EX ("0","0", "[ASS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][ASS]);
		}
		if (strcmp (name[1], "SET"))
		{
		LIMIT_CREATE_EX ("0","0", "[SET_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][SET]);
		}
		if (strcmp (name[1], "MINUS"))
		{
		LIMIT_CREATE_EX ("0","0", "[MINUS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][MINUS]);
		}
		if (strcmp (name[1], "PLUS"))
		{
		LIMIT_CREATE_EX ("0","0", "[PLUS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][PLUS]);
		}
		if (strcmp (name[1], "LIM"))
		{
		LIMIT_CREATE_EX ("0","0", "[LIM_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][LIM]);
		}
		if (strcmp (name[1], "RES_CANC"))
		{
		LIMIT_CREATE_EX ("0","0", "[RES_CANC_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][RES_CANC]);
		}
		if (strcmp (name[1], "ACC") && strcmp (name[1], "DCC"))
		{
			if (strcmp (name[1], "ACC"))
				LIMIT_CREATE_EX ("0","0", "[ACC_OFF]")
			if (strcmp (name[1], "DCC"))
				LIMIT_CREATE_EX ("0","0", "[DCC_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][ACC]);
		}
		if (strcmp (name[1], "RESUME"))
		{
		LIMIT_CREATE_EX ("0","0", "[RESUME_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][RESUME]);
		}
		if (strcmp (name[1], "CANCEL"))
		{
		LIMIT_CREATE_EX ("0","0", "[CANCEL_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][CANCEL]);
		}
	}
	
Error:
	if (strcmp (name[1], "none"))
	{
		if (pthread_data[1]) free(pthread_data[1]);
	}
	if (strcmp (name[0], "none"))
	{
	if (pthread_data[0]) free(pthread_data[0]);
	}
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[station]); 
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE && ((STestParamPtr)pTID)->fault_retest == FALSE)
	{
		//save log in Ticket PTID
		TEST_VALUE_SET( "image", fileName);
		AXIS_HOME_ALL_STATION_30(FALSE);
		AXIS_HOME_ALL_STATION_30(TRUE);
		TEST_END_STATION(STATION_30);
	}

	else if (pexception && ((STestParamPtr)pTID)->fault_retest == TRUE)
	{
		SYSTEMTIME			st;
		char				pathCopy[MAX_PATHNAME_LEN];

		SaveXmlResultFileCopy(((STestParamPtr)pTID), ((STestParamPtr)pTID)->step, _test_index, 1, pathCopy);

		//save Flag Log + Failed = 1
		SaveXmlResultFileFailed(pathCopy, ((STestParamPtr)pTID)->step, --_test_index, "image", fileName);
		
		GetLocalTime(&st);
		sprintf(((STestParamPtr)pTID)->Time, "%02d.%02d.%02d", st.wHour, st.wMinute, st.wSecond);
		
		/*high speed  ==> mouvement in direction - */
		paxis[SYSTEM_AXIS_STATION_30_RIGHT]->SetPosition(paxis[SYSTEM_AXIS_STATION_30_RIGHT], AXIS_Z, Last_position[SYSTEM_AXIS_STATION_30_RIGHT], 100.0); //Right
		paxis[SYSTEM_AXIS_STATION_30_RIGHT]->EnableAxis(paxis[SYSTEM_AXIS_STATION_30_RIGHT], AXIS_Z);
								
		paxis[SYSTEM_AXIS_STATION_30_LEFT]->SetPosition(paxis[SYSTEM_AXIS_STATION_30_LEFT], AXIS_Z, Last_position[SYSTEM_AXIS_STATION_30_LEFT], 100.0); //LEFT
		paxis[SYSTEM_AXIS_STATION_30_LEFT]->EnableAxis(paxis[SYSTEM_AXIS_STATION_30_LEFT], AXIS_Z);
	}
	
	else 
	{
		//save log in Ticket PTID
		TEST_VALUE_SET( "image", fileName);
		TEST_END_STATION(STATION_30);  //  pexception = NULL
	}
 
return  pexception;

}

FCE_CLEANUP(MFL, 30_40)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_10
*********************************************************************************/

FCE_HELP(MFL, 30_10)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_10)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_10)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_PHONE_STATION_30(check);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_10)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_11
*********************************************************************************/

FCE_HELP(MFL, 30_11)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_11)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_11)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_11)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_LIST_STATION_30(check);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_11)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_12
*********************************************************************************/

FCE_HELP(MFL, 30_12)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_12)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_12)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_12)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_SPEECH_STATION_30(check);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_12)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_13
*********************************************************************************/

FCE_HELP(MFL, 30_13)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_13)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_13)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_13)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_MENU_RIGHT_STATION_30(check);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_13)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_20
*********************************************************************************/

FCE_HELP(MFL, 30_20)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_20)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_20)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_WHEEL_STATION_30(check);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_20)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_21
*********************************************************************************/

FCE_HELP(MFL, 30_21)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_21)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_21)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_21)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_SPEED_STATION_30(check);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_21)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_22
*********************************************************************************/

FCE_HELP(MFL, 30_22)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_22)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_22)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_22)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_SET_STATION_30(check);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_22)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_23
*********************************************************************************/

FCE_HELP(MFL, 30_23)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_23)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_23)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_23)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_MINUS_STATION_30(check);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_23)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_30
*********************************************************************************/

FCE_HELP(MFL, 30_30)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : run normal mode" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_30)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_30)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_30)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);
	SLinThreadDataPtr	pthread_Lin_data;
	HANDLE				hThreadLin;
    int station = STATION_30;
	clock_t 					_start_test_clock;
	double   					time;
	
	/* Start Loop Thread Read */	 
	iFlag_Thread_Lin_Read[station] = TRUE;

	/* Start thread LIN communication */
	pthread_Lin_data = (SLinThreadDataPtr)calloc(1, sizeof(SLinThreadData)); 
	EXCCHECKALLOC( pthread_Lin_data);
	pthread_Lin_data->pcan = pcan;
	pthread_Lin_data->timeout = 500;
	pthread_Lin_data->station = station;
	sprintf (pthread_Lin_data->lin_channel, LIN_CHANNEL_STATION_30);
	EXCCHECK(ThreadStart(Thread_Lin_Read, pthread_Lin_data, &hThreadLin, NULL));

	/* Start Loop Thread Write */	 
	iFlag_Thread_Lin_Write[station] = TRUE;
	iFlag_Write_Wake_Up[station] = TRUE;
	iFlag_Delete_Cache [station] = TRUE;
		
	/* Start thread 2 LIN communication */
	pthread_Lin_data = (SLinThreadDataPtr)calloc(1, sizeof(SLinThreadData)); 
	EXCCHECKALLOC( pthread_Lin_data);
	pthread_Lin_data->pcan = pcan;
	pthread_Lin_data->timeout = 500;
	pthread_Lin_data->station = station;
	sprintf (pthread_Lin_data->lin_channel, LIN_CHANNEL_STATION_30);
	EXCCHECK(ThreadStart(Thread_Lin_Write, pthread_Lin_data, &hThreadLin, NULL));
	
	Sleep (50);
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[station]); 
	
	/* check run normal mode */
	_start_test_clock =  clock();
	do {
		time = (double)(clock()-_start_test_clock);
	}while (iFlag_Run_Normal_Mode[station] == 0 && time < 500);
	
	LIMIT_CREATE_EX ("1","1", "[COM_LIN]")
	LIMIT_CHECK(UINT32, 	iFlag_Run_Normal_Mode[station]);
	
	/* send Valeo_production_Key */
	iFlag_Write_Valeo_production_Key[station] = TRUE;
	
	Sleep (100);
		
Error:
    TEST_END_STATION(STATION_30);
}
				   
FCE_CLEANUP(MFL, 30_30)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_31
*********************************************************************************/

FCE_HELP(MFL, 30_31)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : stop normal mode" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_31)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_31)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_31)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);

	/* Stop Loop Thread Write */	 
	iFlag_Thread_Lin_Write[STATION_30] = FALSE;
	
	/* Stop Loop Thread Read */	 
	iFlag_Thread_Lin_Read[STATION_30] = FALSE;

	/* wait before Unplug connector to Go to sleep */
	Sleep (150);
	
Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_31)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_32
*********************************************************************************/

FCE_HELP(MFL, 30_32)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Check alive byte" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_32)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_32)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_32)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);

	/* save LIN communication log file */
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 
	LIMIT_CREATE_EX ("0","0", "[check alive byte]")
	LIMIT_CHECK(INT32, Error_Alive_Byte[STATION_30]);

	
Error:
    TEST_END_STATION(STATION_30);
}
				   
FCE_CLEANUP(MFL, 30_32)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_33
*********************************************************************************/

FCE_HELP(MFL, 30_33)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Check functions No activated");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_33)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_33)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_33)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE (INT32, Push_Value);
	VARIABLE (REAL64, Value_ADC);
	
	INITIALIZE(DAQ);
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	Sleep(300);
	iFlag_Delete_Cache [STATION_30] = TRUE;
	Sleep(50);
	
	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = FALSE;
	
	/* Reaquest Read Switch_Raw_Values*/
	iFlag_Read_Switch_Raw_Values [STATION_30] = TRUE;

	Sleep(22);

	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = TRUE;
	
	Sleep(100);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 

#if 0    /* formerly excluded lines */
	/* Left ADC Switch Raw */
	Value_ADC = Get_ADC_push_value("L_BOTTOM_LEFT",STATION_30);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_BOTTOM_MID",STATION_30);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_BOTTOM_RIGHT",STATION_30);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_MID_LEFT",STATION_30);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_MID_RIGHT",STATION_30);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_TOP_LEFT",STATION_30);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_TOP_RIGHT",STATION_30);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("TIP_WHEEL",STATION_30);
	LIMIT_CHECK(REAL64, Value_ADC);
#endif   /* formerly excluded lines */
	
	/* Right */
	Push_Value = Get_push_value("PHONE", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[PHONE_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("LIST", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[LIST_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("SPEECH", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[SPEECH_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("MENU_RIGHT", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[MENU_RIGHT_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("SCROLL_WHEEL", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[SCROLL_WHEEL_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("MENU_LEFT", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[MENU_LEFT_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("VOLUME_PLUS", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[VOLUME_PLUS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("VOLUME_MINUS", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[VOLUME_MINUS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	
	/* Left */
	Push_Value = Get_push_value("MODE", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[MODE_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("ASS", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[ASS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("SET", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[SET_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("MINUS", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[MINUS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("PLUS", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[PLUS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("LIM", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[LIM_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);	

	Push_Value = Get_push_value("RES_CANC", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[RES_CANC_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);	
	
	Push_Value = Get_push_value("ACC", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[ACC_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("DCC", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[DCC_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);	
	
	Push_Value = Get_push_value("RESUME", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[RESUME_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("CANCEL", STATION_30); 
	LIMIT_CREATE_EX ("0","0", "[CANCEL_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_30(TRUE);
	}

    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_33)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_34
*********************************************************************************/

FCE_HELP(MFL, 30_34)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Write Variant code  F1/F2/F3/F4");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_34)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_34)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_34)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( variant_code, 256);

	PARAM_STRING_REQUIRED( "Variant Code", variant_code);

		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	
	/* Write Variant code */
	sprintf (Variant_Code_Write [STATION_30], "%s", variant_code);
	
   	iFlag_Write_Variant_code[STATION_30] = TRUE;
	
	Sleep (100);

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_30(TRUE);
	}

    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_34)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_35
*********************************************************************************/

FCE_HELP(MFL, 30_35)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Read Variant code  F1/F2/F3/F4");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_35)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_35)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_35)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( variant_code, 256);

	PARAM_STRING_REQUIRED( "Variant Code", variant_code);

		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	iFlag_Delete_Cache [STATION_30] = TRUE;
	
	/* Reaquest Variant code */
   	iFlag_Read_Variant_code[STATION_30] = TRUE;
	Sleep (50);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 
	
	LIMIT_CREATE(variant_code, "[Variant Code]");
	LIMIT_SET(STRING, Variant_Code_Read[STATION_30]);
	
	if (strcmp(variant_code, Variant_Code_Read[STATION_30]))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_30(TRUE);
	}

    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_35)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}


/*********************************************************************************
 * Function 30_36
*********************************************************************************/

FCE_HELP(MFL, 30_36)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Earase DTC ");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_36)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_36)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_36)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE (INT32, Push_Value);
	
	INITIALIZE(DAQ);
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	/* send Valeo_production_Key */
	//iFlag_Write_Valeo_production_Key[STATION_30] = TRUE;
	
	iFlag_Delete_Cache [STATION_30] = TRUE;

	/* erase DTC */
	iFlag_Write_Wake_Up[STATION_30] = TRUE;
	Sleep(81);
	iFlag_Write_Wake_Up[STATION_30] = FALSE;
	Sleep(21)
			;
	iFlag_Write_Valeo_production_Key[STATION_30] = TRUE; //20ms
	Sleep(21);
		
	iFlag_Write_Wake_Up[STATION_30] = TRUE;
	Sleep(21);
	iFlag_Write_Wake_Up[STATION_30] = FALSE;
	Sleep(21);
		
	iFlag_Erasing_DTC_Memory_1[STATION_30] = TRUE; //10ms 
	Sleep(21);
		
	iFlag_Write_Wake_Up[STATION_30] = TRUE;
	Sleep(81);
	iFlag_Write_Wake_Up[STATION_30] = FALSE;
	Sleep(21);

	iFlag_Erasing_DTC_Memory_2[STATION_30] = TRUE; //10ms 
	Sleep(21);
		
	Sleep (100);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_30(TRUE);
	}

    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_36)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}


/*********************************************************************************
 * Function 30_37
*********************************************************************************/

FCE_HELP(MFL, 30_37)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_37)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_37)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_37)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE(INT32, i);
	
	for( i=0;i<19;i++)
	{
	pexception = pcan->WriteLowLevel(pcan, LIN_CHANNEL_STATION_30, 0x3C, 18, 8, INIT_ALL_FRAME[i]);
	Sleep(10);
	}
	
Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_37)
{
    TESTCLEANUP_BEGIN();
	
Error:    
    TESTCLEANUP_END(STATION_30);
}
					
/*********************************************************************************
 * Function 30_38
*********************************************************************************/

FCE_HELP(MFL, 30_38)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_38)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_38)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_38)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);

	iFlag_Delete_Cache [STATION_30] = TRUE;
	Number_DTC[STATION_30] = 1;
	
	cnt = 0;
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);

		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_30] = FALSE;
	
		/* Reaquest Number_DTC */
		iFlag_Read_Number_DTC_Memory[STATION_30] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_30] = TRUE;
	
		Sleep(100);
		
		LIMIT_CHECK_EXT(INT32, Number_DTC[STATION_30], pexception);
		if(!pexc_first) pexc_first = pexception;

		if(pexc_first)
		{
		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_30] = FALSE;
	
		/* Reaquest Erasing_DTC_Memory_1 */
		iFlag_Erasing_DTC_Memory_1[STATION_30] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_30] = TRUE;
	
		Sleep(100);
		
		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_30] = FALSE;
	
		/* Reaquest Number_DTC */
		iFlag_Erasing_DTC_Memory_2[STATION_30] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_30] = TRUE;
	
		Sleep(250);
		}
	
	}while(pexc_first && cnt<10);

	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 

   	EXCCHECK(pexc_first);
	
Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_38)
{
    TESTCLEANUP_BEGIN();
	
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_50
*********************************************************************************/

FCE_HELP(MFL, 30_50)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check Valeo SW Revision");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_50)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_50)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_50)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( valeo_sw_revision, 256);

	PARAM_STRING_REQUIRED( "Valeo Sw Revision", valeo_sw_revision);

		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	iFlag_Delete_Cache [STATION_30] = TRUE;
	
	memset(Valeo_Sw_Revision[STATION_30], 0X00,6+1); 

	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_Valeo_Sw_Revision[STATION_30] = TRUE;

	Sleep(22);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = TRUE;
	
	Sleep(150);
	
	
	LIMIT_CREATE(valeo_sw_revision, "[Valeo Sw Revision]");
	LIMIT_SET(STRING, Valeo_Sw_Revision[STATION_30]);
	
	if (strcmp(valeo_sw_revision, Valeo_Sw_Revision[STATION_30]))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 
	AXIS_HOME_ALL_STATION_30(TRUE);
	}

    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_50)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_51
*********************************************************************************/

FCE_HELP(MFL, 30_51)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check SW Build Date");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_51)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_51)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_51)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( sw_build_date, 256);

	PARAM_STRING_REQUIRED( "Sw Build Date", sw_build_date);

		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
	iFlag_Delete_Cache [STATION_30] = TRUE;
	
	memset(Sw_Build_Date[STATION_30], 0X00,12+1); 


	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_Sw_Build_Date[STATION_30] = TRUE;

	Sleep(80);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = TRUE;
	
	
	Sleep(150);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 
	
	LIMIT_CREATE(sw_build_date, "[Sw Build Date}]");
	LIMIT_SET(STRING, Sw_Build_Date[STATION_30]);
	
	if (strcmp(sw_build_date, Sw_Build_Date[STATION_30]))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_30(TRUE);
	}

    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_51)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_52
*********************************************************************************/

FCE_HELP(MFL, 30_52)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check tracability EMS");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_52)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_52)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_52)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	int retest;
	STRING( PcbaPartNumber, 256);
	STRING( PcbaIndex, 4);
	STRING(part_number, 256);
	STRING( BMW_HW_Revision, 256);  
	STRING( BatchNumber, 4); 
	
	PARAM_STRING_REQUIRED( "BMW_HW_Revision", BMW_HW_Revision);
	PARAM_STRING_REQUIRED( "PcbaPartNumber", PcbaPartNumber);
	PARAM_STRING_REQUIRED( "PcbaIndex", PcbaIndex); 
	PARAM_STRING_REQUIRED( "ProductPartNumber", part_number);
	PARAM_STRING_REQUIRED( "BatchNumber", BatchNumber); 
	
	
	
	/*printf("PcbaIndex:%sn",  PcbaIndex);
	printf("BatchNumber:%sn",  BatchNumber);	*/ 
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);
	
#if _CHECK_TRACA_EMS

	iFlag_Read_MAN_DATA_1_READ_OK[STATION_30] = FALSE;
	retest = 0;
		
	do{

	iFlag_Delete_Cache [STATION_30] = TRUE;

	sprintf(MAN_DATA_1_Tracability_EMS_Read[STATION_30], "FF");

	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = FALSE;
	
	/* Reaquest MAN DATA 1 */
   	iFlag_Read_MAN_DATA_1[STATION_30] = TRUE;
					  
	Sleep(80);		
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = TRUE;
	
	Sleep(150);
	
	retest++;
		
	}while (iFlag_Read_MAN_DATA_1_READ_OK[STATION_30] == FALSE  && retest < 3);

	
	SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 
	
	/* ICT / FCT test tracking OK  :  ICT is 0xX1 = Test OK  |  FCT is 0x1X = Test OK */
    if (!strcmp(MAN_DATA_1_Tracability_EMS_Read[STATION_30], "11"))
		{
		LIMIT_CREATE("11", "[Traca_EMS]");
		LIMIT_SET(STRING, MAN_DATA_1_Tracability_EMS_Read[STATION_30]);
		}
	
#if 0    /* formerly excluded lines */
	/* Re-tested OK */
	else if (!strcmp(MAN_DATA_1_Tracability_EMS_Read[STATION_30], "22"))
		{
		LIMIT_CREATE("10", "[Traca_EMS]");
		LIMIT_SET(STRING, MAN_DATA_1_Tracability_EMS_Read[STATION_30]);
		}
#endif   /* formerly excluded lines */
	
	/* Tested NOK */
	else 
		{
		LIMIT_CREATE("11", "[Traca_EMS]");
		LIMIT_SET(STRING, MAN_DATA_1_Tracability_EMS_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
	
/*	
	printf ( PcbaPartNumber	   );
	printf ( PcbaIndex	   );  		*/
# if 0
	if(!strcmp(part_number,"E31080400"))
{		
//if((!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "0E314132"))  ||  (!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "00314132")) )
	if(!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "314132") )         
		{
		LIMIT_CREATE( "314132", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE( "314132", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}  
	if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], "47"))
		{
		LIMIT_CREATE("47", "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE("47", "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
}
	else if(!strcmp(part_number,"E31085800"))
{  
//if((!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "0E314132"))  ||  (!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "00314132")) )
	if(!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "314159") )         
		{
		LIMIT_CREATE( "314159", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE( "314159", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
	if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], "47"))
		{
		LIMIT_CREATE("47", "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE("47", "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
}
	else 	if(!strcmp(part_number,"E31081500"))
{ 
//if((!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "0E314132"))  ||  (!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "00314132")) )
	if(!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "314166") )         
		{
		LIMIT_CREATE( "314166", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE( "314166", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}

		if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], "47"))
		{
		LIMIT_CREATE("47", "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE("47", "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
}
	
// printf	   (BMW_HW_Revision);

/*FOR NXP SBC CHIP*/
		
if (!strcmp(BMW_HW_Revision,"0701"))
{
	//check PCBA index
	if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], "47"))
		{
			LIMIT_CREATE("47", "[PcbaIndex]");
			LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
		else
		{
			LIMIT_CREATE("47", "[PcbaIndex]");
			LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}

		if (!strcmp(MAN_DATA_1_Production_Batch_Number[STATION_30], "01"))
		{
			LIMIT_CREATE("01", "[NXP/ATMEL]");
			LIMIT_SET(STRING, MAN_DATA_1_Production_Batch_Number[STATION_30]);
		}
		else
		{
			LIMIT_CREATE("01", "[NXP/ATMEL]");
			LIMIT_SET(STRING, MAN_DATA_1_Production_Batch_Number[STATION_30]);
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}



	}
	else
	{
		if (!strcmp(MAN_DATA_1_Production_Batch_Number[STATION_30], "00"))
		{
			LIMIT_CREATE("00", "[NXP/ATMEL]");
			LIMIT_SET(STRING, MAN_DATA_1_Production_Batch_Number[STATION_30]);
		}
		else
		{
			LIMIT_CREATE("00", "[NXP/ATMEL]");
			LIMIT_SET(STRING, MAN_DATA_1_Production_Batch_Number[STATION_30]);
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
	}  
#endif
			   

	
	if(!strcmp(part_number,"E31080400"))
{		
	if(!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "314132") )         
		{
		LIMIT_CREATE( "314132", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE( "314132", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}  
	if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], PcbaIndex))
		{
		LIMIT_CREATE(PcbaIndex, "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE(PcbaIndex, "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
}
	else if(!strcmp(part_number,"E31085800"))
{  
	if(!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "314159") )         
		{
		LIMIT_CREATE( "314159", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE( "314159", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
	if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], PcbaIndex))
		{
		LIMIT_CREATE(PcbaIndex, "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE(PcbaIndex, "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
}
	else if(!strcmp(part_number,"E31081500"))

	{	
		if(!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], "314166") )         
		{
		LIMIT_CREATE( "314166", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE( "314166", "[PcbaPartNumber]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}

		if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], PcbaIndex))
		{
		LIMIT_CREATE(PcbaIndex, "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE(PcbaIndex, "[PcbaIndex]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
}
	
// printf	   (BMW_HW_Revision);

/*FOR NXP SBC CHIP*/
if (!strcmp(BMW_HW_Revision,"0700"))
	{
		if (!strcmp(MAN_DATA_1_Production_Batch_Number[STATION_30], "00"))
		{
			LIMIT_CREATE("00", "[NXP/ATMEL]");
			LIMIT_SET(STRING, MAN_DATA_1_Production_Batch_Number[STATION_30]);
		}
		else
		{
			LIMIT_CREATE("00", "[NXP/ATMEL]");
			LIMIT_SET(STRING, MAN_DATA_1_Production_Batch_Number[STATION_30]);
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
	}
else 
{
	//check PCBA index
	if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], PcbaIndex))
		{
			LIMIT_CREATE(PcbaIndex, "[PcbaIndex]");
			LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
		else
		{
			LIMIT_CREATE(PcbaIndex, "[PcbaIndex]");
			LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}

		if (!strcmp(MAN_DATA_1_Production_Batch_Number[STATION_30], BatchNumber))
		{
			LIMIT_CREATE(BatchNumber, "[NXP/ATMEL]");
			LIMIT_SET(STRING, MAN_DATA_1_Production_Batch_Number[STATION_30]);
		}
		else
		{
			LIMIT_CREATE(BatchNumber, "[NXP/ATMEL]");
			LIMIT_SET(STRING, MAN_DATA_1_Production_Batch_Number[STATION_30]);
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}



	}
	


#if 0    /* formerly excluded lines */
/* PCBA INDEX OK */
    if (!strcmp(MAN_DATA_1_Revision_Index_Read[STATION_30], PcbaIndex))
		{
		LIMIT_CREATE(PcbaIndex, "[Traca_EMS]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE(PcbaIndex, "[Traca_EMS]");
		LIMIT_SET(STRING, MAN_DATA_1_Revision_Index_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
	
	
/* PCBA PART NUMBER OK */
    if (!strcmp(MAN_DATA_1_VALEO_Part_Number_Read[STATION_30], PcbaPartNumber))
		{
		LIMIT_CREATE(PcbaPartNumber, "[Traca_EMS]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		}
	else 
		{
		LIMIT_CREATE(PcbaPartNumber, "[Traca_EMS]");
		LIMIT_SET(STRING, MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
#endif   /* formerly excluded lines */
	

	sprintf (((STestParamPtr)pTID)->EMS_PN_PCBA, "%s", MAN_DATA_1_VALEO_Part_Number_Read[STATION_30]); 
	sprintf (((STestParamPtr)pTID)->EMS_SERIAL, "%s", MAN_DATA_1_Serial_Read[STATION_30]);
	sprintf (((STestParamPtr)pTID)->EMS_DATE, "%s/%s/%s", MAN_DATA_1_Production_Day_Read[STATION_30], MAN_DATA_1_Production_Month_Read[STATION_30], MAN_DATA_1_Production_Year_Read[STATION_30]); 	

#endif


Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_30(TRUE);
	}									

    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_52)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_53
*********************************************************************************/

FCE_HELP(MFL, 30_53)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check tracability MMS");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_53)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_53)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_53)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	TEST_USE(DBS);
	int serial;
	int retest;

   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_30_LEFT);

#if _CHECK_TRACA_MMS
	
	iFlag_Read_MAN_DATA_2_READ_OK[STATION_30] = FALSE;
	retest = 0;
	
	do
	{
	iFlag_Delete_Cache [STATION_30] = TRUE;

	sprintf(MAN_DATA_2_Tracability_MMS_Read[STATION_30], "FF");

	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_MAN_DATA_2[STATION_30] = TRUE;
					  
	Sleep(80);		
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_30] = TRUE;
	
	Sleep(150);
	
	retest++;
		
	}while (iFlag_Read_MAN_DATA_2_READ_OK[STATION_30] == FALSE  && retest < 3);
	
	
	SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_30]); 
	
#if 0    /* formerly excluded lines */
	/* check line ID */
	if (!strcmp(MAN_DATA_2_LineID_Read[STATION_30], "01"))
	{
		LIMIT_CREATE("02;00", "[Line ID]");
		LIMIT_SET(STRING, MAN_DATA_2_LineID_Read[STATION_30]);
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
	}
#endif   /* formerly excluded lines */
			
	/* Not Tested */
	if (!strcmp(MAN_DATA_2_Tracability_MMS_Read[STATION_30], "00"))
	{
	LIMIT_CREATE("00", "[Traca_MMS]");
	LIMIT_SET(STRING, MAN_DATA_2_Tracability_MMS_Read[STATION_30]);
	((STestParamPtr)pTID)->Skip_All_Test = FALSE;
	((STestParamPtr)pTID)->Retested = FALSE;
	((STestParamPtr)pTID)->Write_Tracability_MMS = TRUE;
	((STestParamPtr)pTID)->Check_Laser = FALSE;  
	EXCCHECK( GetNewSerial (pdbs, &serial));
	sprintf(((STestParamPtr)pTID)->Production_Serial, "%04d", serial+6000);
	}

	// Tested OK 
	else if (!strcmp(MAN_DATA_2_Tracability_MMS_Read[STATION_30], "10"))
	{
	LIMIT_CREATE("10", "[Traca_MMS]");
	LIMIT_SET(STRING, MAN_DATA_2_Tracability_MMS_Read[STATION_30]);
	((STestParamPtr)pTID)->Skip_All_Test = FALSE;
	//((STestParamPtr)pTID)->Check_DTC = TRUE;
	((STestParamPtr)pTID)->Check_Laser = TRUE;
	((STestParamPtr)pTID)->Retested = TRUE;
	((STestParamPtr)pTID)->Write_Tracability_MMS = TRUE;
	
	// production Serial from MAN_DATA_2 
	CopyString (((STestParamPtr)pTID)->Production_Serial, 0, MAN_DATA_2_Serial_Read[STATION_30], 4, 4); //00000006

	
	
	// production date from MAN_DATA_2 
	sprintf (((STestParamPtr)pTID)->Date, "%04d.%02d.%02d ", atoi(MAN_DATA_2_Production_Year_Read[STATION_30])+2000, 
															 atoi(MAN_DATA_2_Production_Month_Read[STATION_30]), 
															 atoi(MAN_DATA_2_Production_Day_Read[STATION_30]));
	}
	
	// Retested OK 
	else if (!strcmp(MAN_DATA_2_Tracability_MMS_Read[STATION_30], "30"))
	{
	LIMIT_CREATE("30", "[Traca_MMS]");
	LIMIT_SET(STRING, MAN_DATA_2_Tracability_MMS_Read[STATION_30]);
	((STestParamPtr)pTID)->Skip_All_Test = FALSE;
	//((STestParamPtr)pTID)->Check_DTC = TRUE;
	((STestParamPtr)pTID)->Check_Laser = TRUE;
	((STestParamPtr)pTID)->Retested = TRUE;
	((STestParamPtr)pTID)->Write_Tracability_MMS = TRUE;

	// production Serial from MAN_DATA_2 
	CopyString (((STestParamPtr)pTID)->Production_Serial, 0, MAN_DATA_2_Serial_Read[STATION_30], 4, 4); //00000006
	
	// production date from MAN_DATA_2 
	sprintf (((STestParamPtr)pTID)->Date, "%04d.%02d.%02d ", atoi(MAN_DATA_2_Production_Year_Read[STATION_30])+2000, 
															 atoi(MAN_DATA_2_Production_Month_Read[STATION_30]), 
															 atoi(MAN_DATA_2_Production_Day_Read[STATION_30]));
		
	}
	
	// Tested NOK 
	else if (!strcmp(MAN_DATA_2_Tracability_MMS_Read[STATION_30], "20"))
	{

		LIMIT_CREATE("20", "[Traca_MMS]");
		LIMIT_SET(STRING, MAN_DATA_2_Tracability_MMS_Read[STATION_30]);
		((STestParamPtr)pTID)->Skip_All_Test = FALSE;
		((STestParamPtr)pTID)->Retested = TRUE;
		((STestParamPtr)pTID)->Write_Tracability_MMS = TRUE;
		((STestParamPtr)pTID)->Check_Laser = TRUE;
			
	// production Serial from MAN_DATA_2 
	CopyString (((STestParamPtr)pTID)->Production_Serial, 0, MAN_DATA_2_Serial_Read[STATION_30], 4, 4); //00000006
	
	// production date from MAN_DATA_2 
	sprintf (((STestParamPtr)pTID)->Date, "%04d.%02d.%02d ", atoi(MAN_DATA_2_Production_Year_Read[STATION_30])+2000, 
															 atoi(MAN_DATA_2_Production_Month_Read[STATION_30]), 
															 atoi(MAN_DATA_2_Production_Day_Read[STATION_30]));
	
	}
	
	
	/*else if (!strcmp(MAN_DATA_2_Tracability_MMS_Read[STATION_30], "20"))
	{
		if ( ((STestParamPtr)pTID)->Autorisation_retest == TRUE)
		{
		LIMIT_CREATE("0X20", "[Traca_MMS]");
		LIMIT_SET(STRING, MAN_DATA_2_Tracability_MMS_Read[STATION_30]);
		((STestParamPtr)pTID)->Skip_All_Test = FALSE;
		((STestParamPtr)pTID)->Retested = TRUE;
		((STestParamPtr)pTID)->Write_Tracability_MMS = TRUE;
		EXCCHECK( GetNewSerial (pdbs, &serial));
		sprintf(((STestParamPtr)pTID)->Production_Serial, "%05d", serial);
		}
		else if ( ((STestParamPtr)pTID)->Autorisation_retest == FALSE)
		{
		LIMIT_CREATE("00", "[Traca_MMS]");
		LIMIT_SET(STRING, MAN_DATA_2_Tracability_MMS_Read[STATION_30]);
		((STestParamPtr)pTID)->Retested = FALSE;
		((STestParamPtr)pTID)->Write_Tracability_MMS = FALSE;
		CopyString (((STestParamPtr)pTID)->Production_Serial, 0, MAN_DATA_2_Serial_Read[STATION_30], 3, 5); //00000006
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
	}*/
	
	/* no data */
	else 
	{
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
	}

#else
	((STestParamPtr)pTID)->Skip_All_Test = FALSE;
	((STestParamPtr)pTID)->Retested = FALSE;
	((STestParamPtr)pTID)->Write_Tracability_MMS = TRUE;
	EXCCHECK( GetNewSerial (pdbs, &serial));
	sprintf(((STestParamPtr)pTID)->Production_Serial, "%05d", serial);	
#endif
//if ( atoi(((STestParamPtr)pTID)->Production_Serial) < 6000)
//			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "Produit Testeur MFL ligne 1");	
Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_30(TRUE);
	}									

    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(MFL, 30_53)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_90
 *********************************************************************************/
FCE_HELP(MFL, 30_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 30_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 30_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(MFL, 30_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	/* wait for table finish rotation */
	Inhibit_Rodage_Pushs[STATION_30] = FALSE;
	DISPLAY_TESTSEQ_RESULT(STATION_30);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_40);

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 30_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 40_00
*********************************************************************************/

FCE_HELP(MFL, 40_00)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	Inhibit_Rodage_Pushs[STATION_40] = TRUE;
	TESTER_PLUG_PANEL_STATION_40(0);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(MFL, 40_00)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_01
*********************************************************************************/

FCE_HELP(MFL, 40_01)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_40(TIMEOUT_DEFAULT);

Error:
	if (pexception)
		TESTER_UNPLUG_PANEL_STATION_40 (TIMEOUT_DEFAULT);
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(MFL, 40_01)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_02
*********************************************************************************/

FCE_HELP(MFL, 40_02)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_02)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_02)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_02)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_40(0);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_02)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_03
*********************************************************************************/

FCE_HELP(MFL, 40_03)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_03)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_03)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_03)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_40(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_03)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_04
*********************************************************************************/

FCE_HELP(MFL, 40_04)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_04)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_04)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_04)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
																  
	INITIALIZE_AXIS(SYSTEM_AXIS_STATION_40_RIGHT);
	INITIALIZE_AXIS(SYSTEM_AXIS_STATION_40_LEFT);
	
	AXIS_HOME_ALL_STATION_40(FALSE);
	
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_04)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_40
*********************************************************************************/

FCE_HELP(MFL, 40_40)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_40)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_40)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_40)				  
{
	TEST_BEGIN();
	SElExceptionPtr     pexception2 = NULL;
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	SAquisitionThreadDataPtr	pthread_data[2];
	HANDLE  					hThreadAquisition[2];
	int  	read[2];
	double 	sdWaveGes[3][MESS_BUF][BUFFERLEN] = {0,0,0,0};
	double  Start_push_position[2]= {0,0};
	char    AnalogChannel[2][32];
	char    position_ADC[32];
	char    name[2][32];
	char 	fileName[512];
	char 	filePath[512];
	int     i,j;
	int 	status;

	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmS3V;
	double dmF_END;
	double dmS_END;
	double dmF1R;
	double dmS1R;
	double dmF2R;
	double dmS2R;
	double TactileRatio, Ce;
	double x_vect[2], y_vect[2]; 
    int IndexContact = 0;
	int station = STATION_40;
	char sValue[128];
	char log_dir[512];

	static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 500;
	static double Freq_Filtre = 2000;
	static double Freq_Haute_Filtre = 1000;
	static int 	  Ordre_Filtre = 3;
	static int 	  Use_Filter;
		
	double Last_position[4];
	
	INITIALIZE(DAQ);
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	PARAM_STRING_OPTIONAL("push_name_right", name[0], "none");
	PARAM_STRING_OPTIONAL("push_name_left", name[1], "none");
	PARAM_STRING_OPTIONAL("position_ADC", position_ADC, "none");
	
	iFlag_Delete_Cache [STATION_40] = TRUE;

	//sprintf (AnalogChannel[0], "ai3");
	sprintf (AnalogChannel[0], "ai3");
	sprintf (AnalogChannel[1], "ai4");

	paxis[SYSTEM_AXIS_STATION_40_RIGHT]->CurrentPosition(paxis[SYSTEM_AXIS_STATION_40_RIGHT], AXIS_Z, &Last_position[SYSTEM_AXIS_STATION_40_RIGHT]);//Right
	paxis[SYSTEM_AXIS_STATION_40_LEFT]->CurrentPosition(paxis[SYSTEM_AXIS_STATION_40_LEFT], AXIS_Z, &Last_position[SYSTEM_AXIS_STATION_40_LEFT]); //Left
	
	/* Start thread press push direction + */
	for (i=0; i<2; i++)
	{
		if (strcmp (name[i], "none"))
		{
		pthread_data[i] = (SAquisitionThreadDataPtr)calloc(1, sizeof(SAquisitionThreadData)); 
		EXCCHECKALLOC( pthread_data);
		pthread_data[i]->vi_DAQ = vi_DAQ;
		pthread_data[i]->paxis = paxis[i+2];
		pthread_data[i]->pcan = pcan;
		pthread_data[i]->timeout = 9000;
		pthread_data[i]->index = i;
		pthread_data[i]->station = station;
		pthread_data[i]->position_forward = 48.0;
		pthread_data[i]->position_backward = 40.0;
		sprintf (pthread_data[i]->push_name , name[i]);
		sprintf (pthread_data[i]->Channel, "%s\0",AnalogChannel[i]);
		EXCCHECK(ThreadStart(Thread_Aquisition_Force, pthread_data[i], &hThreadAquisition[i], NULL));
		}
	}
	
	/* Sleep thread to finish */
	for (i=0; i< 2 ; i++)
	{
		if (strcmp (name[i], "none"))
		{
			pexception = ThreadStatus(hThreadAquisition[i], 9000, &status);
#if 0    /* formerly excluded lines */
			if(pexception)
			{
			LIMIT_CREATE_EX ("1","1", "[Aquisition]");
			LIMIT_CHECK(INT32, 0);
			EXCDELETE(pexception);
			EXCTHROW(-1, "ThreadStatus : Aquisition Timeout");
			}
#endif   /* formerly excluded lines */
		
			read[i] = pthread_data[i]->read;
		
			for (j=0; j<pthread_data[i]->read; j++)
			{
			 sdWaveGes[i][MESS_KRAFT][j] = pthread_data[i]->data[MESS_KRAFT][j];	
			 sdWaveGes[i][MESS_WEG][j] = pthread_data[i]->data[MESS_WEG][j];
		     sdWaveGes[i][MESS_CONTACT][j] = pthread_data[i]->data[MESS_CONTACT][j];
			}
			
#if 0    /* formerly excluded lines */
			/* Apply Bessel filter for force waveform */
    		BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    		Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    		IIRCascadeFiltering (pthread_data[i]->data[MESS_KRAFT], read[i], BessPtr, sdWaveGes[i][MESS_KRAFT]);
			FreeIIRFilterPtr (BessPtr);
#endif   /* formerly excluded lines */
		}
			
	}
																									
	
#if 0    /* formerly excluded lines */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
		{
			sprintf (log_dir, "%s\\STEP_%d", ((STestParamPtr)pTID)->csv_dir, ((STestParamPtr)pTID)->step);
			EXCCHECK( CheckDirectory(log_dir));
			/* save array to file right */
			if (strcmp (name[0], "none"))
			{
			sprintf(fileName, "%s\\STATION_40_RIGHT_%s_ESSAI_%d.txt",log_dir, "Stroke", ((STestParamPtr)pTID)->RnR_counter);
			ArrayToFile (fileName, sdWaveGes[0][MESS_WEG], VAL_DOUBLE, pthread_data[0]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
			sprintf(fileName, "%s\\STATION_40_RIGHT_%s_ESSAI_%d.txt",log_dir, "Force", ((STestParamPtr)pTID)->RnR_counter);
			ArrayToFile (fileName, pthread_data[0]->data[MESS_KRAFT], VAL_DOUBLE, pthread_data[0]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
			}
	
			/* save array to file left */
			if (strcmp (name[1], "none"))
			{
			sprintf(fileName, "%s\\STATION_40_LEFT_%s_ESSAI_%d.txt",log_dir, "Stroke", ((STestParamPtr)pTID)->RnR_counter);
			ArrayToFile (fileName, sdWaveGes[1][MESS_WEG], VAL_DOUBLE, pthread_data[1]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
			sprintf(fileName, "%s\\STATION_40_LEFT_%s_ESSAI_%d.txt",log_dir, "Force", ((STestParamPtr)pTID)->RnR_counter);
			ArrayToFile (fileName, pthread_data[1]->data[MESS_KRAFT], VAL_DOUBLE, pthread_data[1]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
			}
		}
			
	else 
	{
		sprintf (log_dir, "log\\%s", ((STestParamPtr)pTID)->Date);
		EXCCHECK( CheckDirectory(log_dir));
		sprintf (log_dir, "%s\\STEP %d", log_dir, ((STestParamPtr)pTID)->step);
		EXCCHECK( CheckDirectory(log_dir)); 
		
		/* save array to file right */
		if (strcmp (name[0], "none"))
		{
		sprintf(fileName, "%s\\STATION_40_RIGHT_%s_%s.txt",log_dir, "Stroke", ((STestParamPtr)pTID)->Time);
		ArrayToFile (fileName, sdWaveGes[0][MESS_WEG], VAL_DOUBLE, pthread_data[0]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
		sprintf(fileName, "%s\\STATION_40_RIGHT_%s_%s.txt",log_dir, "Force", ((STestParamPtr)pTID)->Time);
		ArrayToFile (fileName, sdWaveGes[0][MESS_KRAFT], VAL_DOUBLE, pthread_data[0]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
		}
		
		/* save array to file left */
		if (strcmp (name[1], "none"))
		{
		sprintf(fileName, "%s\\STATION_40_LEFT_%s_%s.txt",log_dir, "Stroke", ((STestParamPtr)pTID)->Time);
		ArrayToFile (fileName, sdWaveGes[1][MESS_WEG], VAL_DOUBLE, pthread_data[1]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
		sprintf(fileName, "%s\\STATION_40_LEFT_%s_%s.txt",log_dir, "Force", ((STestParamPtr)pTID)->Time);
		ArrayToFile (fileName, sdWaveGes[1][MESS_KRAFT], VAL_DOUBLE, pthread_data[1]->read, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
		}

	}
#endif   /* formerly excluded lines */
	

	/* add index palett */
	SetCtrlVal (gs_TestPanel3, PANEL_3_PALETT_STATION_40, ((STestParamPtr)pTID)->PALETT);

	
	if (strcmp (name[0], "none"))
	{
			
		//TEST_GRAPH_RIGHT_STATION_40 (name[0], sdWaveGes[0], read[0]);

		/* Moyenne glissante */
		//Moyenne_Glissante_Courbe_40 (sdWaveGes[0][MESS_KRAFT], read[0]);
		
		/* Analyse Waveform Force/Stroke Right*/
		AnalysisFunction (sdWaveGes[0],
					  	  read[0], 
				  	  	  &dmF1V, &dmS1V, 
					  	  &dmF2V, &dmS2V, 
					  	  &dmF_END, &dmS_END, 
					  	  &dmF1R, &dmS1R, 
					 	  &dmF2R, &dmS2R);

		
		/* Plot Graph & reults */
		for (i=0; i<read[0];i++)
			sdWaveGes[0][MESS_CONTACT][i] = sdWaveGes[0][MESS_CONTACT][i] * SCALE_BIT;
		TEST_GRAPH_RIGHT_STATION_40 (name[0], sdWaveGes[0], pthread_data[0]->read);
		TEST_GRAPH_MESS_RIGHT_STATION_40 (sdWaveGes[0], dmS1V, dmF1V, VAL_RED);
		TEST_GRAPH_MESS_RIGHT_STATION_40 (sdWaveGes[0], dmS2V, dmF2V, VAL_GREEN);
		TEST_GRAPH_MESS_RIGHT_STATION_40 (sdWaveGes[0], dmS_END, dmF_END, VAL_MAGENTA);
		
		/* display result in the Graph */
		sprintf (sValue, "%0.2f N", dmF1V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F1_RIGHT_STATION_40, sValue);
		sprintf (sValue, "%0.2f mm", dmS1V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S1_RIGHT_STATION_40, sValue);
		
		sprintf (sValue, "%0.2f N", dmF2V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F2_RIGHT_STATION_40, sValue);
		sprintf (sValue, "%0.2f mm", dmS2V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S2_RIGHT_STATION_40, sValue);
		
		sprintf (sValue, "%0.2f N", dmF1V+1.0);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F3_RIGHT_STATION_40, sValue);
		sprintf (sValue, "%0.2f mm", dmS_END);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S3_RIGHT_STATION_40, sValue);
		
		/* Ajustement Gain offset F = aX+b */
	    Ajustement_Gain_Offset_40(name[0], &dmF1V, &dmF2V, &dmS_END);
		
		/* display result in the Graph */
		sprintf (sValue, "%0.2f N", dmF1V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F1_RIGHT_STATION_41, sValue);
		sprintf (sValue, "%0.2f mm", dmS1V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S1_RIGHT_STATION_41, sValue);
		
		sprintf (sValue, "%0.2f N", dmF2V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F2_RIGHT_STATION_41, sValue);
		sprintf (sValue, "%0.2f mm", dmS2V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S2_RIGHT_STATION_41, sValue);
		
		sprintf (sValue, "%0.2f N", dmF1V+1.0);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F3_RIGHT_STATION_41, sValue);
		sprintf (sValue, "%0.2f mm", dmS_END);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S3_RIGHT_STATION_41, sValue);
		
	}
	
	else 
	{
		DELETE_GRAPH_RIGHT_STATION_40 ("none");
	}

	if (strcmp (name[1], "none"))
	{
	
		//TEST_GRAPH_LEFT_STATION_40 (name[1], sdWaveGes[1], read[1]);

		/* Moyenne glissante */
		//Moyenne_Glissante_Courbe_40 (sdWaveGes[1][MESS_KRAFT], read[1]);

		/* Analyse Waveform Force/Stroke Left*/
		AnalysisFunction (sdWaveGes[1],
					  	  read[1], 
				  	  	  &dmF1V, &dmS1V, 
					  	  &dmF2V, &dmS2V, 
					  	  &dmF_END, &dmS_END, 
					  	  &dmF1R, &dmS1R, 
					 	  &dmF2R, &dmS2R);
		
		/* Plot Graph & reults */
		for (i=0; i<read[1];i++)
			sdWaveGes[1][MESS_CONTACT][i] = sdWaveGes[1][MESS_CONTACT][i] * SCALE_BIT;
		TEST_GRAPH_LEFT_STATION_40 (name[1], sdWaveGes[1], pthread_data[1]->read);
		TEST_GRAPH_MESS_LEFT_STATION_40 (sdWaveGes[1], dmS1V, dmF1V, VAL_RED);
		TEST_GRAPH_MESS_LEFT_STATION_40 (sdWaveGes[1], dmS2V, dmF2V, VAL_GREEN);
		TEST_GRAPH_MESS_LEFT_STATION_40 (sdWaveGes[1], dmS_END, dmF_END, VAL_MAGENTA);
		
		/* display result in the Graph */
	/*	sprintf (sValue, "%0.2f N", dmF1V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F1_LEFT_STATION_40, sValue);
		sprintf (sValue, "%0.2f mm", dmS1V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S1_LEFT_STATION_40, sValue);
		
		sprintf (sValue, "%0.2f N", dmF2V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F2_LEFT_STATION_40, sValue);
		sprintf (sValue, "%0.2f mm", dmS2V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S2_LEFT_STATION_40, sValue);
		
		sprintf (sValue, "%0.2f N", dmF1V+1.0);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F3_LEFT_STATION_40, sValue);
		sprintf (sValue, "%0.2f mm", dmS_END);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S3_LEFT_STATION_40, sValue);	   */
		
		/* offset */
	    Ajustement_Gain_Offset_40(position_ADC, &dmF1V, &dmF2V, &dmS_END);
		
		/* display result in the Graph */
		sprintf (sValue, "%0.2f N", dmF1V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F1_LEFT_STATION_41, sValue);
		sprintf (sValue, "%0.2f mm", dmS1V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S1_LEFT_STATION_41, sValue);
		
		sprintf (sValue, "%0.2f N", dmF2V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F2_LEFT_STATION_41, sValue);
		sprintf (sValue, "%0.2f mm", dmS2V);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S2_LEFT_STATION_41, sValue);
		
		sprintf (sValue, "%0.2f N", dmF1V+1.0);
		SetCtrlVal (gs_TestPanel3, PANEL_3_F3_LEFT_STATION_41, sValue);
		sprintf (sValue, "%0.2f mm", dmS_END);
		SetCtrlVal (gs_TestPanel3, PANEL_3_S3_LEFT_STATION_41, sValue);
	}
	
	else 
	{
		DELETE_GRAPH_LEFT_STATION_40 ("none");
	}
	
	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	sprintf (log_dir, "%s\\STEP_%d", ((STestParamPtr)pTID)->csv_dir, ((STestParamPtr)pTID)->step);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	//sprintf(filePath, "%s\\%s",((STestParamPtr)pTID)->TicketDirPath, fileName);
	CONVERT_GRAPH_TO_PNG_STATION_40(fileName);
	TEST_VALUE_SET( "image", fileName);
	}
	else  //if (((STestParamPtr)pTID)->fault_retest == FALSE)
	{
	sprintf(fileName, "%s_%s_%d.png", ((STestParamPtr)pTID)->Production_Serial, ((STestParamPtr)pTID)->Time, ((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",((STestParamPtr)pTID)->TicketDirPath, fileName);
	CONVERT_GRAPH_TO_PNG_STATION_40(filePath);
	//TEST_VALUE_SET( "image", fileName);
	}

	
	/* Limit check right */
	if (strcmp (name[0], "none"))
	{
		/* Analyse Waveform Force/Stroke Right*/
		AnalysisFunction (sdWaveGes[0],
					  	  read[0], 
				  	  	  &dmF1V, &dmS1V, 
					  	  &dmF2V, &dmS2V, 
					  	  &dmF_END, &dmS_END, 
					  	  &dmF1R, &dmS1R, 
					 	  &dmF2R, &dmS2R);
	
		/* Ajustement Gain offset F = aX+b */
	    Ajustement_Gain_Offset_40(name[0], &dmF1V, &dmF2V, &dmS_END);
		
		/* Peak force F1 */
		LIMIT_CHECK(REAL64, dmF1V);
	
		/* Travel to peak Force S3 */
		//LIMIT_CHECK(REAL64, dmS_END);
		LIMIT_CHECK_EXT(REAL64, dmS_END, pexception);
	
		/* Tactile Ratio (F1-F2)/F1 */
		TactileRatio = ((dmF1V-dmF2V)/dmF1V) * 100;
		if (dmF1V == 0) TactileRatio = 0; 
		if (TactileRatio == NAN) TactileRatio = 0; 
		LIMIT_CHECK(REAL64, TactileRatio);
	
		/* Travel to contact Ce */
		IndexContact = NAN;
		
		for (i=1; i< read[0]; i++)
			{
			if (sdWaveGes[0][MESS_CONTACT][i] == SCALE_BIT && 
				sdWaveGes[0][MESS_CONTACT][i] != sdWaveGes[0][MESS_CONTACT][i-1]) 
				{		
				IndexContact = i;
				break;
				}
			}
	
		if (IndexContact == NAN)
			Ce = NAN;
		else 
			Ce = sdWaveGes[0][MESS_WEG][IndexContact];
		
		LIMIT_CHECK(REAL64, Ce);
	}
	
	else // don't use the limits of right push in the push left (if push right is dummy : none)
	{
	_test_index = _test_index + 4;	
	}
	
	/* Limit check left */
	if (strcmp (name[1], "none"))
	{
		/* Analyse Waveform Force/Stroke Left*/
		AnalysisFunction (sdWaveGes[1],
					  	  read[1], 
				  	  	  &dmF1V, &dmS1V, 
					  	  &dmF2V, &dmS2V, 
					  	  &dmF_END, &dmS_END, 
					  	  &dmF1R, &dmS1R, 
					 	  &dmF2R, &dmS2R);

		/* offset */
	    Ajustement_Gain_Offset_40(position_ADC, &dmF1V, &dmF2V, &dmS_END);
		
		/* Peak force F1 */
		LIMIT_CHECK(REAL64, dmF1V);
	
		/* Travel to peak Force S3 */
		//LIMIT_CHECK(REAL64, dmS_END);
		LIMIT_CHECK_EXT(REAL64, dmS_END, pexception);

		/* Tactile Ratio (F1-F2)/F1 */
		TactileRatio = ((dmF1V-dmF2V)/dmF1V) * 100;
		LIMIT_CHECK(REAL64, TactileRatio);
	
		/* Travel to contact Ce */
		IndexContact = NAN;
		
		for (i=1; i< read[1]; i++)
			{
			if (sdWaveGes[1][MESS_CONTACT][i] == SCALE_BIT && 
				sdWaveGes[1][MESS_CONTACT][i] != sdWaveGes[1][MESS_CONTACT][i-1]) 
				{
				IndexContact = i;
				break;
				}
			}
	
		if (IndexContact == NAN)
			Ce = NAN;
		else 
			Ce = sdWaveGes[1][MESS_WEG][IndexContact];
		
		LIMIT_CHECK(REAL64, Ce);
		

#if 0    /* formerly excluded lines */
		/* ADC check Value */
		if (strcmp (position_ADC, "L_BOTTOM_LEFT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_BOTTOM_LEFT]);
		}
		
		else if (strcmp (position_ADC, "L_BOTTOM_MID") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_BOTTOM_MID]);
		}
		
		else if  (strcmp (position_ADC, "L_BOTTOM_RIGHT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_BOTTOM_RIGHT]);
		}

		else if  (strcmp (position_ADC, "L_MID_LEFT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_MID_LEFT]);
		}

		else if  (strcmp (position_ADC, "L_MID_RIGHT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_MID_RIGHT]);
		}

		else if  (strcmp (position_ADC, "L_TOP_LEFT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_TOP_LEFT]);
		}
		
		else if  (strcmp (position_ADC, "L_TOP_RIGHT") == 0)
		{
			LIMIT_CHECK(REAL64, Value_ADC_Push[station][L_TOP_RIGHT]);
		}
		
		else 
		{
			EXCTHROW(TEST_ERROR_UNKNOWN_CONFIG_VARIABLE, "TEST_ERROR_UNKNOWN_CONFIG_VARIABLE");
		}
#endif   /* formerly excluded lines */
	}

	else // don't use the limits of right push in the push left (if push right is dummy : none)
	{
	_test_index = _test_index + 5;	
	} 

	
		/* Short circuit check right */
	if (strcmp (name[0], "none"))
	{
		if (strcmp (name[0], "PHONE"))
		{
		LIMIT_CREATE_EX ("0","0", "PHONE_OFF")
		LIMIT_CHECK(UINT32, Status_Push[station][PHONE]);
		}
		if (strcmp (name[0], "LIST"))
		{
		LIMIT_CREATE_EX ("0","0", "LIST_OFF")
		LIMIT_CHECK(UINT32, Status_Push[station][LIST]);
		}
		if (strcmp (name[0], "SPEECH"))
		{
		LIMIT_CREATE_EX ("0","0", "SPEECH_OFF")
		LIMIT_CHECK(UINT32, Status_Push[station][SPEECH]);
		}
		if (strcmp (name[0], "MENU_RIGHT"))
		{
		LIMIT_CREATE_EX ("0","0", "MENU_RIGHT_OFF")
		LIMIT_CHECK(UINT32, Status_Push[station][MENU_RIGHT]);
		}
		if (strcmp (name[0], "SCROLL_WHEEL"))
		{
		LIMIT_CREATE_EX ("0","0", "SCROLL_WHEEL_OFF")
		LIMIT_CHECK(UINT32, Status_Push[station][SCROLL_WHEEL]);
		}
		if (strcmp (name[0], "VOLUME_PLUS"))
		{
		LIMIT_CREATE_EX ("0","0", "VOLUME_PLUS_OFF")
		LIMIT_CHECK(UINT32, Status_Push[station][VOLUME_PLUS]);
		}
		if (strcmp (name[0], "VOLUME_MINUS"))
		{
		LIMIT_CREATE_EX ("0","0", "VOLUME_MINUS_OFF")
		LIMIT_CHECK(UINT32, Status_Push[station][VOLUME_MINUS]);
		}
	}

	/* Short circuit check right */
	if (strcmp (name[0], "none"))
	{
		if (strcmp (name[0], "PHONE"))
		{
		LIMIT_CREATE_EX ("0","0", "[PHONE_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][PHONE]);
		}
		if (strcmp (name[0], "LIST"))
		{
		LIMIT_CREATE_EX ("0","0", "[LIST_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][LIST]);
		}
		if (strcmp (name[0], "SPEECH"))
		{
		LIMIT_CREATE_EX ("0","0", "[SPEECH_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][SPEECH]);
		}
		if (strcmp (name[0], "MENU_RIGHT"))
		{
		LIMIT_CREATE_EX ("0","0", "[MENU_RIGHT_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][MENU_RIGHT]);
		}
		if (strcmp (name[0], "SCROLL_WHEEL"))
		{
		LIMIT_CREATE_EX ("0","0", "[SCROLL_WHEEL_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][SCROLL_WHEEL]);
		}
		if (strcmp (name[0], "VOLUME_PLUS"))
		{
		LIMIT_CREATE_EX ("0","0", "[VOLUME_PLUS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][VOLUME_PLUS]);
		}
		if (strcmp (name[0], "VOLUME_MINUS"))
		{
		LIMIT_CREATE_EX ("0","0", "[VOLUME_MINUS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][VOLUME_MINUS]);
		}
	}

	/* Short circuit check left */
	if (strcmp (name[1], "none"))
	{
		if (strcmp (name[1], "MODE"))
		{
		LIMIT_CREATE_EX ("0","0", "[MODE_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][MODE]);
		}
		if (strcmp (name[1], "ASS"))
		{
		LIMIT_CREATE_EX ("0","0", "[ASS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][ASS]);
		}
		if (strcmp (name[1], "SET"))
		{
		LIMIT_CREATE_EX ("0","0", "[SET_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][SET]);
		}
		if (strcmp (name[1], "MINUS"))
		{
		LIMIT_CREATE_EX ("0","0", "[MINUS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][MINUS]);
		}
		if (strcmp (name[1], "PLUS"))
		{
		LIMIT_CREATE_EX ("0","0", "[PLUS_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][PLUS]);
		}
		if (strcmp (name[1], "LIM"))
		{
		LIMIT_CREATE_EX ("0","0", "[LIM_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][LIM]);
		}
		if (strcmp (name[1], "RES_CANC"))
		{
		LIMIT_CREATE_EX ("0","0", "[RES_CANC_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][RES_CANC]);
		}
		if (strcmp (name[1], "ACC") && strcmp (name[1], "DCC"))
		{
			if (strcmp (name[1], "ACC"))
				LIMIT_CREATE_EX ("0","0", "[ACC_OFF]")
			if (strcmp (name[1], "DCC"))
				LIMIT_CREATE_EX ("0","0", "[DCC_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][ACC]);
		}
		if (strcmp (name[1], "RESUME"))
		{
		LIMIT_CREATE_EX ("0","0", "[RESUME_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][RESUME]);
		}
		if (strcmp (name[1], "CANCEL"))
		{
		LIMIT_CREATE_EX ("0","0", "[CANCEL_OFF]")
		LIMIT_CHECK(UINT32, Status_Push[station][CANCEL]);
		}
	}
	
Error:
	if (strcmp (name[1], "none"))
	{
		if (pthread_data[1]) free(pthread_data[1]);
	}
	if (strcmp (name[0], "none"))
	{
	if (pthread_data[0]) free(pthread_data[0]);
	}
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[station]); 
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE && ((STestParamPtr)pTID)->fault_retest == FALSE)
	{
		//save log in Ticket PTID
		TEST_VALUE_SET( "image", fileName);
		AXIS_HOME_ALL_STATION_40(FALSE);
		AXIS_HOME_ALL_STATION_40(TRUE);
		TEST_END_STATION(STATION_40);
	}

	else if (pexception && ((STestParamPtr)pTID)->fault_retest == TRUE)
	{
		SYSTEMTIME			st;
		char				pathCopy[MAX_PATHNAME_LEN];

		SaveXmlResultFileCopy(((STestParamPtr)pTID), ((STestParamPtr)pTID)->step, _test_index, 1, pathCopy);

		//save Flag Log + Failed = 1
		SaveXmlResultFileFailed(pathCopy, ((STestParamPtr)pTID)->step, --_test_index, "image", fileName);
		
		GetLocalTime(&st);
		sprintf(((STestParamPtr)pTID)->Time, "%02d.%02d.%02d", st.wHour, st.wMinute, st.wSecond);
		
		/*high speed  ==> mouvement in direction - */
		paxis[SYSTEM_AXIS_STATION_40_RIGHT]->SetPosition(paxis[SYSTEM_AXIS_STATION_40_RIGHT], AXIS_Z, Last_position[SYSTEM_AXIS_STATION_40_RIGHT], 100.0); //Right
		paxis[SYSTEM_AXIS_STATION_40_RIGHT]->EnableAxis(paxis[SYSTEM_AXIS_STATION_40_RIGHT], AXIS_Z);
								
		paxis[SYSTEM_AXIS_STATION_40_LEFT]->SetPosition(paxis[SYSTEM_AXIS_STATION_40_LEFT], AXIS_Z, Last_position[SYSTEM_AXIS_STATION_40_LEFT], 100.0); //LEFT
		paxis[SYSTEM_AXIS_STATION_40_LEFT]->EnableAxis(paxis[SYSTEM_AXIS_STATION_40_LEFT], AXIS_Z);
	}
	
	else 
	{
		//save log in Ticket PTID
		TEST_VALUE_SET( "image", fileName);
		TEST_END_STATION(STATION_40);  //  pexception = NULL
	}
 
return  pexception;
}


FCE_CLEANUP(MFL, 40_40)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_10
*********************************************************************************/

FCE_HELP(MFL, 40_10)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_10)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_10)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_VOLUME_PLUS_STATION_40(check);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_10)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_11
*********************************************************************************/

FCE_HELP(MFL, 40_11)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_11)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_11)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_11)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_VOLUME_MINUS_STATION_40(check);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_11)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_12
*********************************************************************************/

FCE_HELP(MFL, 40_12)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_12)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_12)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_12)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_MENU_LEFT_STATION_40(check);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_12)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_13
*********************************************************************************/

FCE_HELP(MFL, 40_13)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_13)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_13)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_13)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_SCROLL_WHEEL_STATION_40(check);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_13)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_20
*********************************************************************************/

FCE_HELP(MFL, 40_20)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_20)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_20)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_RESUME_STATION_40(check);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_20)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_21
*********************************************************************************/

FCE_HELP(MFL, 40_21)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_21)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_21)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_21)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_CANCEL_STATION_40(check);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_21)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_22
*********************************************************************************/

FCE_HELP(MFL, 40_22)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_22)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_22)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_22)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	VARIABLE(UINT32, check);
															  
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	PARAM_UINT32_OPTIONAL("check", &check, FALSE);

	AXIS_MOVE_PUSH_PLUS_STATION_40(check);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_22)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_30
*********************************************************************************/

FCE_HELP(MFL, 40_30)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : run normal mode" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_30)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_30)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_30)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);
	SLinThreadDataPtr	pthread_Lin_data;
	HANDLE				hThreadLin;
    int station = STATION_40;
	clock_t 					_start_test_clock;
	double   					time;
	
	/* Start Loop Thread Read */	 
	iFlag_Thread_Lin_Read[station] = TRUE;

	/* Start thread LIN communication */
	pthread_Lin_data = (SLinThreadDataPtr)calloc(1, sizeof(SLinThreadData)); 
	EXCCHECKALLOC( pthread_Lin_data);
	pthread_Lin_data->pcan = pcan;
	pthread_Lin_data->timeout = 500;
	pthread_Lin_data->station = station;
	sprintf (pthread_Lin_data->lin_channel, LIN_CHANNEL_STATION_40);
	EXCCHECK(ThreadStart(Thread_Lin_Read, pthread_Lin_data, &hThreadLin, NULL));

	/* Start Loop Thread Write */	 
	iFlag_Thread_Lin_Write[station] = TRUE;
	iFlag_Write_Wake_Up[station] = TRUE;
	iFlag_Delete_Cache [station] = TRUE;
		
	/* Start thread 2 LIN communication */
	pthread_Lin_data = (SLinThreadDataPtr)calloc(1, sizeof(SLinThreadData)); 
	EXCCHECKALLOC( pthread_Lin_data);
	pthread_Lin_data->pcan = pcan;
	pthread_Lin_data->timeout = 500;
	pthread_Lin_data->station = station;
	sprintf (pthread_Lin_data->lin_channel, LIN_CHANNEL_STATION_40);
	EXCCHECK(ThreadStart(Thread_Lin_Write, pthread_Lin_data, &hThreadLin, NULL));
	
	Sleep (50);
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[station]); 
	
	/* check run normal mode */
	_start_test_clock =  clock();
	do {
		time = (double)(clock()-_start_test_clock);
	}while (iFlag_Run_Normal_Mode[station] == 0 && time < 500);
	
	LIMIT_CREATE_EX ("1","1", "[COM_LIN]")
	LIMIT_CHECK(UINT32, 	iFlag_Run_Normal_Mode[station]);

	/* send Valeo_production_Key */
	iFlag_Write_Valeo_production_Key[station] = TRUE;
	
	Sleep (100);
	
Error:
    TEST_END_STATION(STATION_40);
}
				   
FCE_CLEANUP(MFL, 40_30)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_31
*********************************************************************************/

FCE_HELP(MFL, 40_31)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : stop normal mode" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_31)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_31)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_31)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);

	/* Stop Loop Thread Write */	 
	iFlag_Thread_Lin_Write[STATION_40] = FALSE;
	
	/* Stop Loop Thread Read */	 
	iFlag_Thread_Lin_Read[STATION_40] = FALSE;

	/* wait before Unplug connector to Go to sleep */
	Sleep (150);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_31)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_32
*********************************************************************************/

FCE_HELP(MFL, 40_32)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Check alive byte" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_32)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_32)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_32)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);

	/* save LIN communication log file */
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_40]); 
	LIMIT_CREATE_EX ("0","0", "check alive byte")
	LIMIT_CHECK(INT32, Error_Alive_Byte[STATION_40]);

	
Error:
    TEST_END_STATION(STATION_40);
}
				   
FCE_CLEANUP(MFL, 40_32)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_33
*********************************************************************************/

FCE_HELP(MFL, 40_33)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Check functions No activated");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_33)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_33)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_33)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE (INT32, Push_Value);
	VARIABLE (REAL64, Value_ADC);

	INITIALIZE(DAQ);
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	Sleep(300);
	iFlag_Delete_Cache [STATION_40] = TRUE;
	Sleep(50);
	
	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_40] = FALSE;
	
	/* Reaquest Read Switch_Raw_Values*/
	iFlag_Read_Switch_Raw_Values [STATION_40] = TRUE;

	Sleep(22);

	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_40] = TRUE;
	
	Sleep(100);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_40]); 

#if 0    /* formerly excluded lines */
	/* Left ADC Switch Raw */
	Value_ADC = Get_ADC_push_value("L_BOTTOM_LEFT",STATION_40);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_BOTTOM_MID",STATION_40);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_BOTTOM_RIGHT",STATION_40);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_MID_LEFT",STATION_40);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_MID_RIGHT",STATION_40);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_TOP_LEFT",STATION_40);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("L_TOP_RIGHT",STATION_40);
	LIMIT_CHECK(REAL64, Value_ADC);
	
	Value_ADC = Get_ADC_push_value("TIP_WHEEL",STATION_40);
	LIMIT_CHECK(REAL64, Value_ADC);
#endif   /* formerly excluded lines */

	
	/* Right */
	Push_Value = Get_push_value("PHONE", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[PHONE_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("LIST", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[LIST_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("SPEECH", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[SPEECH_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("MENU_RIGHT", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[MENU_RIGHT_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("SCROLL_WHEEL", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[SCROLL_WHEEL_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("MENU_LEFT", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[MENU_LEFT_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("VOLUME_PLUS", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[VOLUME_PLUS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("VOLUME_MINUS", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[VOLUME_MINUS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	
	/* Left */
	Push_Value = Get_push_value("MODE", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[MODE_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("ASS", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[ASS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("SET", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[SET_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("MINUS", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[MINUS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("PLUS", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[PLUS_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("LIM", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[LIM_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);	

	Push_Value = Get_push_value("RES_CANC", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[RES_CANC_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);	
	
	Push_Value = Get_push_value("ACC", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[ACC_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("DCC", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[DCC_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);	
	
	Push_Value = Get_push_value("RESUME", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[RESUME_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	
	Push_Value = Get_push_value("CANCEL", STATION_40); 
	LIMIT_CREATE_EX ("0","0", "[CANCEL_OFF]")
	LIMIT_CHECK(UINT32, Push_Value);
	

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_40(TRUE);
	}

    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_33)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_34
*********************************************************************************/

FCE_HELP(MFL, 40_34)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Write Variant code  F1/F2/F3/F4");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_34)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_34)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_34)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( variant_code, 256);

	PARAM_STRING_REQUIRED( "Variant Code", variant_code);

		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	
	/* Write Variant code */
	sprintf (Variant_Code_Write [STATION_40], "%s", variant_code);
	
   	iFlag_Write_Variant_code[STATION_40] = TRUE;
	
	Sleep (100);

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_40(TRUE);
	}

    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_34)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_35
*********************************************************************************/

FCE_HELP(MFL, 40_35)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Read Variant code  F1/F2/F3/F4");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_35)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_35)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_35)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( variant_code, 256);

	PARAM_STRING_REQUIRED( "Variant Code", variant_code);

		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	iFlag_Delete_Cache [STATION_40] = TRUE;
	
	/* Reaquest Variant code */
   	iFlag_Read_Variant_code[STATION_40] = TRUE;
	Sleep (50);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_40]); 
	
	LIMIT_CREATE(variant_code, "[Variant Code]");
	LIMIT_SET(STRING, Variant_Code_Read[STATION_40]);
	
	if (strcmp(variant_code, Variant_Code_Read[STATION_40]))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_40(TRUE);
	}

    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_35)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}


/*********************************************************************************
 * Function 40_36
*********************************************************************************/

FCE_HELP(MFL, 40_36)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Earase DTC ");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_36)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_36)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_36)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE (INT32, Push_Value);
	
	INITIALIZE(DAQ);
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	/* send Valeo_production_Key */
	//iFlag_Write_Valeo_production_Key[STATION_40] = TRUE;
	
	iFlag_Delete_Cache [STATION_40] = TRUE;

	/* erase DTC */
	iFlag_Write_Wake_Up[STATION_40] = TRUE;
	Sleep(81);
	iFlag_Write_Wake_Up[STATION_40] = FALSE;
	Sleep(21)
			;
	iFlag_Write_Valeo_production_Key[STATION_40] = TRUE; //20ms
	Sleep(21);
		
	iFlag_Write_Wake_Up[STATION_40] = TRUE;
	Sleep(21);
	iFlag_Write_Wake_Up[STATION_40] = FALSE;
	Sleep(21);
		
	iFlag_Erasing_DTC_Memory_1[STATION_40] = TRUE; //10ms 
	Sleep(21);
		
	iFlag_Write_Wake_Up[STATION_40] = TRUE;
	Sleep(81);
	iFlag_Write_Wake_Up[STATION_40] = FALSE;
	Sleep(21);

	iFlag_Erasing_DTC_Memory_2[STATION_40] = TRUE; //10ms 
	Sleep(21);
		
	Sleep (100);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_40]); 

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_40(TRUE);
	}

    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_36)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}


/*********************************************************************************
 * Function 40_37
*********************************************************************************/

FCE_HELP(MFL, 40_37)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_37)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_37)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_37)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE(INT32, i);
	
	for( i=0;i<19;i++)
	{
	pexception = pcan->WriteLowLevel(pcan, LIN_CHANNEL_STATION_40, 0x3C, 18, 8, INIT_ALL_FRAME[i]);
	Sleep(10);
	}
	
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_37)
{
    TESTCLEANUP_BEGIN();
	
Error:    
    TESTCLEANUP_END(STATION_40);
}
					
/*********************************************************************************
 * Function 40_38
*********************************************************************************/

FCE_HELP(MFL, 40_38)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_38)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_38)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_38)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);

	iFlag_Delete_Cache [STATION_40] = TRUE;
	Number_DTC[STATION_40] = 1;
	
	cnt = 0;
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);

		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_40] = FALSE;
	
		/* Reaquest Number_DTC */
		iFlag_Read_Number_DTC_Memory[STATION_40] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_40] = TRUE;
	
		Sleep(100);
		
		LIMIT_CHECK_EXT(INT32, Number_DTC[STATION_40], pexception);
		if(!pexc_first) pexc_first = pexception;

		if(pexc_first)
		{
		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_40] = FALSE;
	
		/* Reaquest Erasing_DTC_Memory_1 */
		iFlag_Erasing_DTC_Memory_1[STATION_40] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_40] = TRUE;
	
		Sleep(100);
		
		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_40] = FALSE;
	
		/* Reaquest Number_DTC */
		iFlag_Erasing_DTC_Memory_2[STATION_40] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_40] = TRUE;
	
		Sleep(250);
		}
	
	}while(pexc_first && cnt<10);

	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_40]); 

   	EXCCHECK(pexc_first);
	
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_38)
{
    TESTCLEANUP_BEGIN();
	
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_50
*********************************************************************************/

FCE_HELP(MFL, 40_50)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check Valeo SW Revision");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_50)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_50)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_50)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( valeo_sw_revision, 256);

	PARAM_STRING_REQUIRED( "Valeo Sw Revision", valeo_sw_revision);

		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	iFlag_Delete_Cache [STATION_40] = TRUE;
	
	memset(Valeo_Sw_Revision[STATION_40], 0X00,6+1); 

	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_40] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_Valeo_Sw_Revision[STATION_40] = TRUE;

	Sleep(22);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_40] = TRUE;
	
	Sleep(100);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_40]); 
	
	LIMIT_CREATE(valeo_sw_revision, "[Valeo Sw Revision]");
	LIMIT_SET(STRING, Valeo_Sw_Revision[STATION_40]);
	
	if (strcmp(valeo_sw_revision, Valeo_Sw_Revision[STATION_40]))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_40(TRUE);
	}

    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_50)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_51
*********************************************************************************/

FCE_HELP(MFL, 40_51)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check SW Build Date");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_51)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_51)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_51)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( sw_build_date, 256);

	PARAM_STRING_REQUIRED( "Sw Build Date", sw_build_date);

		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
	iFlag_Delete_Cache [STATION_40] = TRUE;
	
	memset(Sw_Build_Date[STATION_40], 0X00,12+1); 


	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_40] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_Sw_Build_Date[STATION_40] = TRUE;

	Sleep(80);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_40] = TRUE;
	
	
	Sleep(150);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_40]); 
	
	LIMIT_CREATE(sw_build_date, "[Sw Build Date}]");
	LIMIT_SET(STRING, Sw_Build_Date[STATION_40]);
	
	if (strcmp(sw_build_date, Sw_Build_Date[STATION_40]))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_40(TRUE);
	}

    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_51)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_52
*********************************************************************************/

FCE_HELP(MFL, 40_52)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check tracability EMS");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_52)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_52)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_52)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);
		
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_40_LEFT);
	
#if _CHECK_TRACA_EMS
	
	iFlag_Delete_Cache [STATION_40] = TRUE;

	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_40] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_MAN_DATA_1[STATION_40] = TRUE;

	Sleep(80);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_40] = TRUE;
	
	Sleep(150);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_40]);
	
#endif
	
Error:
	if(pexception && ((STestParamPtr)pTID)->fault_continue == FALSE)
	{
	AXIS_HOME_ALL_STATION_40(TRUE);
	}

    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_52)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_53
*********************************************************************************/

FCE_HELP(MFL, 40_53)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check tracability MMS");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_53)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_53)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_53)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(CAN);
	TEST_USE(TESTER);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(MFL, 40_53)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_90
 *********************************************************************************/
FCE_HELP(MFL, 40_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 40_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 40_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(MFL, 40_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	/* wait for table finish rotation */
	Inhibit_Rodage_Pushs[STATION_40] = FALSE;
	DISPLAY_TESTSEQ_RESULT(STATION_40);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_50);

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 40_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}


/*********************************************************************************
 * Function 50_00
*********************************************************************************/

FCE_HELP(MFL, 50_00)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Cylinder Rodage down" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 50_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 50_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(MFL, 50_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(MOTOR);
	INITIALIZE_MOTOR (MOTOR_STATION_50_LEFT);
	int 	status;

	/* reset sensor rodage 50 */	
	pmotor[MOTOR_STATION_50_LEFT]->ControllerStatus(pmotor[MOTOR_STATION_50_LEFT], MOTOR_STATION_50_LEFT, &status);

	Inhibit_Rodage_Pushs[STATION_50] = TRUE;
	
	TESTER_START_RODAGE_RIGHT_STATION_50(0);
	TESTER_START_RODAGE_LEFT_STATION_50(0);
	
	Rodage_Wheels_Run = TRUE;
	TESTER_START_RODAGE_RIGHT_STATION_50(TIMEOUT_DEFAULT);
	TESTER_START_RODAGE_LEFT_STATION_50(TIMEOUT_DEFAULT);

Error:
	Rodage_Wheels_Run = FALSE;
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(MFL, 50_00)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 50_01
*********************************************************************************/

FCE_HELP(MFL, 50_01)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Cylinder Rodage up" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 50_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 50_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(MFL, 50_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	clock_t 					_start_test_clock;
	double   					time;
	double   					timeout = 15000;

	/* Semaphore Rodage */
	_start_test_clock =  clock();
#if 0    /* formerly excluded lines */
	do {
	Sleep(100);
	ProcessDrawEvents ();
	ProcessSystemEvents ();
	
	time = (double)(clock()-_start_test_clock);

	}while ((Inhibit_Rodage_Pushs[STATION_30] == TRUE ||
		     Inhibit_Rodage_Pushs[STATION_40] == TRUE ||
			// Inhibit_Rodage_Pushs[STATION_50] == TRUE ||
		//	 Inhibit_Rodage_Pushs[STATION_60] == TRUE ||
			 Inhibit_Rodage_Pushs[STATION_70] == TRUE) && time < timeout);
#endif   /* formerly excluded lines */
	
	
	TESTER_STOP_RODAGE_RIGHT_STATION_50(0);
   	TESTER_STOP_RODAGE_LEFT_STATION_50(0);
	
	TESTER_STOP_RODAGE_RIGHT_STATION_50(TIMEOUT_DEFAULT);
   	TESTER_STOP_RODAGE_LEFT_STATION_50(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(MFL, 50_01)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 50_10
*********************************************************************************/

FCE_HELP(MFL, 50_10)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 50_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 50_10)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(MFL, 50_10)
{
	TEST_BEGIN();
	TEST_USE(MOTOR);
	TEST_USE(TESTER);
	SRodageThreadDataPtr	pthread_data[2];
	HANDLE  				hThreadRodageWheel[2];
	double 					angle[2]= {0,0};
	double 					speed[2]= {0,0};
	int 					count[2]= {0,0};
	int 					i;
	int  					status;
	
	
	INITIALIZE_MOTOR (MOTOR_STATION_50_RIGHT);
	INITIALIZE_MOTOR (MOTOR_STATION_50_LEFT);

	MOTOR_INIT(MOTOR_STATION_50_RIGHT);
   	MOTOR_INIT(MOTOR_STATION_50_LEFT);

	PARAM_REAL64_REQUIRED("Angle_Right", &angle[0]);
	PARAM_REAL64_REQUIRED("Speed_Right", &speed[0]);
	PARAM_INT32_REQUIRED("Cycle_Right", &count[0]);

	PARAM_REAL64_REQUIRED("Angle_Left", &angle[1]);
	PARAM_REAL64_REQUIRED("Speed_Left", &speed[1]);
	PARAM_INT32_REQUIRED("Cycle_Left", &count[1]);
	
	/* Start thread rodage for wheel right */
		pthread_data[0] = (SRodageThreadDataPtr)calloc(1, sizeof(SRodageThreadData)); 
		EXCCHECKALLOC( pthread_data);
		pthread_data[0]->pmotor = pmotor[0];
		pthread_data[0]->angle = angle[0];
		//pthread_data[0]->speed = speed[0];
		pthread_data[0]->speed = 50.0;
		pthread_data[0]->count = count[0];
		pthread_data[0]->timeout = 25000;
		EXCCHECK(ThreadStart(Thread_Rodage_Wheel_Right, pthread_data[0], &hThreadRodageWheel[0], NULL));
	
	/* Start thread rodage for wheel left */
		pthread_data[1] = (SRodageThreadDataPtr)calloc(1, sizeof(SRodageThreadData)); 
		EXCCHECKALLOC(pthread_data);
		pthread_data[1]->pmotor = pmotor[1];
		//pthread_data[1]->angle = angle[1];
		//pthread_data[1]->speed = speed[1];
		pthread_data[1]->angle = 15.0;
		pthread_data[1]->speed = 10.0;
		pthread_data[1]->count = count[1];
		pthread_data[1]->timeout = 25000;
		EXCCHECK(ThreadStart(Thread_Rodage_Wheel_Left, pthread_data[1], &hThreadRodageWheel[1], NULL));
		
	/* Sleep thread to finish press push direction - */
    for (i=0; i<2; i++)
	{	
		pexception = ThreadStatus(hThreadRodageWheel[i], 17000, &status);   //old =20000
		if(pexception)
		{
			EXCDELETE(pexception);
			EXCTHROW(-1, "Rodage Wheel Timeout");
		}
		else
		{
			switch(status)
			{
				case THREAD_STATUS_STILL_ACTIVE:
					EXCTHROW( TESTER_ERROR_STILL_ACTIVE, "Thread Handle Status: STILL_ACTIVE");
				break;
				case THREAD_STATUS_ERROR_CODE:
					EXCTHROW( TESTER_ERROR_TIMEOUT, "Rotation Timeout");
				break;
			}
		}
	}
	
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(MFL, 50_10)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_90
 *********************************************************************************/
FCE_HELP(MFL, 50_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 50_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 50_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(MFL, 50_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	Inhibit_Rodage_Pushs[STATION_50] = FALSE;
	
	/* wait for table finish rotation */
	DISPLAY_TESTSEQ_RESULT(STATION_50);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_60);

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 50_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 60_00
*********************************************************************************/

FCE_HELP(MFL, 60_00)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	Inhibit_Rodage_Pushs[STATION_60] = TRUE;


	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);

	
	SElExceptionPtr	pexc = NULL;
	STorqueThreadDataPtr		pthread_data_Phase_Detection_Left;
   	HANDLE  					hThreadTorque_Phase_Detection_Left;
	
	STorqueThreadDataPtr		pthread_data_Phase_Detection_Right;
   	HANDLE  					hThreadTorque_Phase_Detection_Right;
	
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_LEFT);
	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_RIGHT);
	
	
	/*
	pthread_data_Phase_Detection_Right = (STorqueThreadDataPtr)calloc(1, sizeof(STorqueThreadData));
	EXCCHECKALLOC(pthread_data_Phase_Detection_Right);
	pthread_data_Phase_Detection_Right->angle = 25.0;
	pthread_data_Phase_Detection_Right->paxis = paxis[SYSTEM_AXIS_STATION_60_RIGHT];
	EXCCHECK(ThreadStart(Thread_Phase_Detection_Right, pthread_data_Phase_Detection_Right, &hThreadTorque_Phase_Detection_Right, NULL));
	


	pthread_data_Phase_Detection_Left = (STorqueThreadDataPtr)calloc(1, sizeof(STorqueThreadData));
    EXCCHECKALLOC(pthread_data_Phase_Detection_Left);
	pthread_data_Phase_Detection_Left->angle = 35.0;
	pthread_data_Phase_Detection_Left->paxis = paxis[SYSTEM_AXIS_STATION_60_LEFT];
	EXCCHECK(ThreadStart(Thread_Phase_Detection_Left, pthread_data_Phase_Detection_Left, &hThreadTorque_Phase_Detection_Left, NULL)); 
	
    Sleep(200);
	
	if (pthread_data_Phase_Detection_Right->angle<51||pthread_data_Phase_Detection_Left->angle<11 || pthread_data_Phase_Detection_Left->angle>13.5)
	{
		Sleep(9000);
		
	//	printf("Sleep 1");
	}
	*/

	
	
	TESTER_PLUG_PANEL_STATION_60(0);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_00)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_01
*********************************************************************************/

FCE_HELP(MFL, 60_01)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_60(TIMEOUT_DEFAULT);
//	printf("60_01\n\r");

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_01)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_02
*********************************************************************************/

FCE_HELP(MFL, 60_02)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_02)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_02)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_02)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_60(0);
//	printf("60_02");

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_02)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_03
*********************************************************************************/

FCE_HELP(MFL, 60_03)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_03)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_03)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_03)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_60(TIMEOUT_DEFAULT);
//	printf("60_03");

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_03)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}
/*********************************************************************************
 * Function 60_04
*********************************************************************************/

FCE_HELP(MFL, 60_04)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Cylinder test down" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_04)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_04)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_04)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	Offset_Torque_TIPP_WHEEL = data_Analog[6*RATE+1];
	
	/*Set cylinder down */
	TESTER_START_TEST_RIGHT_STATION_60(0);
	TESTER_START_TEST_LEFT_STATION_60(0);
//	printf("60_04");
	
Error:
	Test_Torque_Run = FALSE;
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_04)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 60_05
*********************************************************************************/

FCE_HELP(MFL, 60_05)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Cylinder test down" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_05)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_05)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_05)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	/*check down cylinder down */
	TESTER_START_TEST_RIGHT_STATION_60(TIMEOUT_DEFAULT);
	TESTER_START_TEST_LEFT_STATION_60(TIMEOUT_DEFAULT);
//	printf("60_05");
	
Error:
	Test_Torque_Run = FALSE;
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_05)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 60_06
*********************************************************************************/

FCE_HELP(MFL, 60_06)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Cylinder test up" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_06)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_06)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_06)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	TESTER_STOP_TEST_RIGHT_STATION_60(0);
   	TESTER_STOP_TEST_LEFT_STATION_60(0);
////	TEST_BEGIN();
//	TEST_USE(AXIS);
//	TEST_USE(DAQ);
//	TEST_USE(CAN);
////	TEST_USE(TESTER);
//	SElExceptionPtr	pexc = NULL;
//	STorqueThreadDataPtr		pthread_data_Phase_Detection_Left;
//   	HANDLE  					hThreadTorque_Phase_Detection_Left;
//	
//	STorqueThreadDataPtr		pthread_data_Phase_Detection_Right;
//   	HANDLE  					hThreadTorque_Phase_Detection_Right;
//	
//	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_LEFT);
//	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_RIGHT);
//	
//	pthread_data_Phase_Detection_Left = (STorqueThreadDataPtr)calloc(1, sizeof(STorqueThreadData));
//	EXCCHECKALLOC(pthread_data_Phase_Detection_Left);
//	pthread_data_Phase_Detection_Left->paxis = paxis[SYSTEM_AXIS_STATION_60_LEFT];
//	EXCCHECK(ThreadStart(Thread_Phase_Detection_Left, pthread_data_Phase_Detection_Left, &hThreadTorque_Phase_Detection_Left, NULL)); 
//	
//	
//	pthread_data_Phase_Detection_Right = (STorqueThreadDataPtr)calloc(1, sizeof(STorqueThreadData));
//	EXCCHECKALLOC(pthread_data_Phase_Detection_Right);
//	pthread_data_Phase_Detection_Right->paxis = paxis[SYSTEM_AXIS_STATION_60_RIGHT];
//	EXCCHECK(ThreadStart(Thread_Phase_Detection_Right, pthread_data_Phase_Detection_Right, &hThreadTorque_Phase_Detection_Right, NULL)); 
//	printf("check cylinder test up");
	
//	printf("60_06");

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_06)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 60_07
*********************************************************************************/

FCE_HELP(MFL, 60_07)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, Cylinder test up" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_07)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_07)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_07)
{
    TEST_BEGIN();
//	TEST_USE(AXIS);
//	TEST_USE(DAQ);
//	TEST_USE(CAN);
	TEST_USE(TESTER);
//	SElExceptionPtr	pexc = NULL;
//	STorqueThreadDataPtr		pthread_data_Phase_Detection_Left;
//   	HANDLE  					hThreadTorque_Phase_Detection_Left;
//	
//	STorqueThreadDataPtr		pthread_data_Phase_Detection_Right;
//   	HANDLE  					hThreadTorque_Phase_Detection_Right;
//	
//	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_LEFT);
//	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_RIGHT);
//	
//	pthread_data_Phase_Detection_Left = (STorqueThreadDataPtr)calloc(1, sizeof(STorqueThreadData));
//	EXCCHECKALLOC(pthread_data_Phase_Detection_Left);
//	pthread_data_Phase_Detection_Left->paxis = paxis[SYSTEM_AXIS_STATION_60_RIGHT];
//	EXCCHECK(ThreadStart(Thread_Phase_Detection_Left, pthread_data_Phase_Detection_Left, &hThreadTorque_Phase_Detection_Left, NULL)); 
//	
//	
//	pthread_data_Phase_Detection_Right = (STorqueThreadDataPtr)calloc(1, sizeof(STorqueThreadData));
//	EXCCHECKALLOC(pthread_data_Phase_Detection_Right);
//	pthread_data_Phase_Detection_Right->paxis = paxis[SYSTEM_AXIS_STATION_60_LEFT];
//	EXCCHECK(ThreadStart(Thread_Phase_Detection_Right, pthread_data_Phase_Detection_Right, &hThreadTorque_Phase_Detection_Right, NULL)); 
//	printf("60_07");
//

	TESTER_STOP_TEST_RIGHT_STATION_60(TIMEOUT_DEFAULT);
   	TESTER_STOP_TEST_LEFT_STATION_60(TIMEOUT_DEFAULT);
	
	

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_07)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END();
}

/*********************************************************************************
 * Function 60_30
*********************************************************************************/

FCE_HELP(MFL, 60_30)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Start normal mode" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_30)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_30)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_30)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);
	SLinThreadDataPtr	pthread_Lin_data;
	HANDLE				hThreadLin;
    int station = STATION_60;
	clock_t 					_start_test_clock;
	double   					time;
	char sTimeDate[128];
	
	/* Start Loop Thread Read */	 
	iFlag_Thread_Lin_Read[station] = TRUE;

	/* Start thread LIN communication */
	pthread_Lin_data = (SLinThreadDataPtr)calloc(1, sizeof(SLinThreadData)); 
	EXCCHECKALLOC( pthread_Lin_data);
	pthread_Lin_data->pcan = pcan;
	pthread_Lin_data->timeout = 500;
	pthread_Lin_data->station = station;
	sprintf (pthread_Lin_data->lin_channel, LIN_CHANNEL_STATION_60);
	EXCCHECK(ThreadStart(Thread_Lin_Read, pthread_Lin_data, &hThreadLin, NULL));

	/* Start Loop Thread Write */	 
	iFlag_Thread_Lin_Write[station] = TRUE;
	iFlag_Write_Wake_Up[station] = TRUE;
	iFlag_Delete_Cache [station] = TRUE;
		
	/* Start thread 2 LIN communication */
	pthread_Lin_data = (SLinThreadDataPtr)calloc(1, sizeof(SLinThreadData)); 
	EXCCHECKALLOC( pthread_Lin_data);
	pthread_Lin_data->pcan = pcan;
	pthread_Lin_data->timeout = 500;
	pthread_Lin_data->station = station;
	sprintf (pthread_Lin_data->lin_channel, LIN_CHANNEL_STATION_60);
	EXCCHECK(ThreadStart(Thread_Lin_Write, pthread_Lin_data, &hThreadLin, NULL));
	
	Sleep (50);
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[station]); 
	
	/* check run normal mode */
	_start_test_clock =  clock();
	do {
		time = (double)(clock()-_start_test_clock);
	}while (iFlag_Run_Normal_Mode[station] == 0 && time < 500);
	
	LIMIT_CREATE_EX ("1","1", "[COM_LIN]")
	LIMIT_CHECK(UINT32, 	iFlag_Run_Normal_Mode[station]);
	
	/* send Valeo_production_Key */
	iFlag_Write_Valeo_production_Key[station] = TRUE;
	
	Sleep (100);
		
Error:
    TEST_END_STATION(STATION_60);
}
				   
FCE_CLEANUP(MFL, 60_30)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_31
*********************************************************************************/

FCE_HELP(MFL, 60_31)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : stop normal mode" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_31)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_31)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_31)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);

	/* Stop Loop Thread Write */	 
	iFlag_Thread_Lin_Write[STATION_60] = FALSE;
	
	/* Stop Loop Thread Read */	 
	iFlag_Thread_Lin_Read[STATION_60] = FALSE;

	/* wait before Unplug connector to Go to sleep */
	Sleep (150);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_31)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_32
*********************************************************************************/

FCE_HELP(MFL, 60_32)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Check alive byte" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_32)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_32)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_32)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);

	/* save LIN communication log file */
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_60]); 
	LIMIT_CREATE_EX ("0","0", "[check alive byte]")
	LIMIT_CHECK(INT32, Error_Alive_Byte[STATION_60]);

	
Error:
    TEST_END_STATION(STATION_60);
}
				   
FCE_CLEANUP(MFL, 60_32)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_09
*********************************************************************************/

FCE_HELP(MFL, 60_09)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_09)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_09)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_09)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE(INT32, BG_PWM);
	VARIABLE(INT32, i);

	/* LIN variable */
	unsigned char Go_To_Sleep[8]={0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  
	int32_t 	  dataSize = 8;
	
	pexception = pcan->WriteLowLevel(pcan, LIN_CHANNEL_STATION_60, 0x3C, 18, dataSize, Go_To_Sleep);
	Sleep (150);
	
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_09)
{
    TESTCLEANUP_BEGIN();
	
Error:    
    TESTCLEANUP_END(STATION_60);
}


/*********************************************************************************
 * Function 60_10
*********************************************************************************/

FCE_HELP(MFL, 60_10)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(MOTOR);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_10)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_10)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	SElExceptionPtr	pexc = NULL;
	STorqueThreadDataPtr		pthread_data_SCROLL_WHEEL;
	STorqueThreadDataPtr		pthread_data_TIPP_WIPE;
   	HANDLE  					hThreadTorque_SCROLL_WHEEL;
	HANDLE  					hThreadTorque_TIPP_WIPE;
	int  	read[4];
	static double 	sdWaveGes[4][MESS_BUF2][BUFFERLEN2] = {0,0,0,0};
	double Tab_LIN[BUFFERLEN2];
	double Tab_LIN_UP[BUFFERLEN2];

	char    AnalogChannel[2][32];
	char 	fileName[512];
	char 	filePath[512];
	int     i,j;
	int 	status; 
	int     Index_Down_P1=0;
	int     Index_Up_P1=0; 
	
	int     NumPulse_UP=0; 
	int     NumPulse_DOWN=0; 
	int 	Value;
	
	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmF3V;
	double dmS3V;
	double dmF4V;
	double dmS4V;
	double dmF5V;
	double dmS5V;
	
	double F1_DOWN,F2_DOWN,F3_DOWN,F4_DOWN,F5_DOWN;
	double A1_DOWN,A2_DOWN,A3_DOWN,A4_DOWN,A5_DOWN;
	
	double F1_UP,F2_UP,F3_UP,F4_UP,F5_UP;
	double A1_UP,A2_UP,A3_UP,A4_UP,A5_UP;
	
	double Tab_Angle[5+1];
	double Tab_Force[5+1];
	char sValue[128];
	
	static IIRFilterPtr BessPtr;
    static double  *FiltData_Torque;
	static double Freq_Basse_Filtre = 100;
	static double Freq_Filtre = 2000;
	static double Freq_Haute_Filtre = 500;
	static int 	  Ordre_Filtre = 1;
    int Width;
	int result;
   	static double *locations;
	static double *amplitudes;
	static double *derivatives;
	
	static int numPeaks_down=0;
	static int numValleys_down=0;
	static int numPeaks_up=0;
	static int numValleys_up=0;

	int iCmpt,n,m;
	
	MaxMinScrollWheel SW;
	MaxMinScrollWheel_up SW_up;
	
	double *BufferTorqueScrollWheelFiltred_UP;
	double *BufferAngleScrollWheelFiltred_UP;
	
	double *BufferTorqueScrollWheelFiltred_DOWN;
	double *BufferAngleScrollWheelFiltred_DOWN;

	double *SWPeaks2ndDeriv;
	double *SWPeaksAmp;
	double *SWPeaksLocs;
   	ssize_t SWPeaks = 0;
	
	double *SWPeaks2ndDeriv_up;
	double *SWPeaksAmp_up;
	double *SWPeaksLocs_up;
   	ssize_t SWPeaks_up = 0;

	double *SWValleysAmp;
	double *SWValleyss2ndDeriv;
	double *SWValleysLocs;
	ssize_t SWValleys = 0;
	
	double *SWValleysAmp_up;
	double *SWValleyss2ndDeriv_up;
	double *SWValleysLocs_up;
	ssize_t SWValleys_up = 0;
	
	LesPoints PointsSW;
	LesPoints T_PointsSW;
	LesPoints T_F_PointsSW;
	
	LesPoints_up PointsSW_up;
	LesPoints_up T_PointsSW_up;
	LesPoints_up T_F_PointsSW_up;
	
	double *BufferDist;
	double *BufferLoc;
	
	double *BufferDist_up;
	double *BufferLoc_up;
	
	double *PointBas;
	double *PointHaut;
	double *PointBasX;
	double *PointHautX;
	
	double *PointBas_up;
	double *PointHaut_up;
	double *PointBasX_up;
	double *PointHautX_up;

	int NbrPtsBas;
	int NbrPtsHaut;
	
	int NbrPtsBas_up;
	int NbrPtsHaut_up;

	int *BufferX;
	
	int *BufferX_up; 

	int sortieFiltre1;

	char Chaine[256]="";
	char Line[1024]="";
	int file;
	char log_dir[1024]="";
	
		
	INITIALIZE(DAQ);
   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_LEFT);

	pthread_data_SCROLL_WHEEL = (STorqueThreadDataPtr)calloc(1, sizeof(STorqueThreadData));
	EXCCHECKALLOC(pthread_data_SCROLL_WHEEL);
	pthread_data_SCROLL_WHEEL->station = STATION_60;
	pthread_data_SCROLL_WHEEL->paxis = paxis[SYSTEM_AXIS_STATION_60_RIGHT];
	pthread_data_SCROLL_WHEEL->pcan = pcan;
	pthread_data_SCROLL_WHEEL->vi_DAQ = vi_DAQ;
	pthread_data_SCROLL_WHEEL->timeout = 15000;
	pthread_data_SCROLL_WHEEL->angle = 180.0;
	pthread_data_SCROLL_WHEEL->speed = 5.0; //OLD 5.5
	pthread_data_SCROLL_WHEEL->Torque_Max = 0.5; //20Nmm
	pthread_data_SCROLL_WHEEL->Reduction_Torque = 1.0;
	pthread_data_SCROLL_WHEEL->Reduction_Angle = 6.85; //7.6
	pthread_data_SCROLL_WHEEL->NumPulse = 0;
	EXCCHECK(ThreadStart(Thread_Aquisition_Torque_SCROLL_WHEEL, pthread_data_SCROLL_WHEEL, &hThreadTorque_SCROLL_WHEEL, NULL));

	pthread_data_TIPP_WIPE = (STorqueThreadDataPtr)calloc(1, sizeof(STorqueThreadData));
	EXCCHECKALLOC(pthread_data_TIPP_WIPE);
	pthread_data_TIPP_WIPE->station = STATION_60;
	pthread_data_TIPP_WIPE->paxis = paxis[SYSTEM_AXIS_STATION_60_LEFT];											
	pthread_data_TIPP_WIPE->pcan = pcan;
	pthread_data_TIPP_WIPE->vi_DAQ = vi_DAQ;
	pthread_data_TIPP_WIPE->timeout = 15000;
	pthread_data_TIPP_WIPE->angle = 23.0; //OLD 12.0
	pthread_data_TIPP_WIPE->speed = 2;  //OLD 2.5
	pthread_data_TIPP_WIPE->Torque_Max = 9.0; //0.1Nm  0.13
	//pthread_data_TIPP_WIPE->Reduction_Torque = 0.004882;		
	//pthread_data_TIPP_WIPE->Reduction_Angle = 0.03;
	pthread_data_TIPP_WIPE->Reduction_Torque = 1;		
	pthread_data_TIPP_WIPE->Reduction_Angle = 6;
	EXCCHECK(ThreadStart(Thread_Aquisition_Torque_TIPP_WIPE, pthread_data_TIPP_WIPE, &hThreadTorque_TIPP_WIPE, NULL));
	
	

	/* Wait thread to finish : hThreadTorque_SCROLL_WHEEL*/
	pexception = ThreadStatus(hThreadTorque_SCROLL_WHEEL, 40000, &status);
		if(pexception)
			{
			//printf("ThreadStatus : Motor Timeout\r\n");
			EXCDELETE(pexception);
			EXCTHROW(-1, "Motor Timeout SCROLL_WHEEL");
			}
		
		else
			{
				switch(status)
				{
				case THREAD_STATUS_STILL_ACTIVE:
					EXCTHROW( TESTER_ERROR_STILL_ACTIVE, "Thread Handle Status: STILL_ACTIVE");
				break;
				case THREAD_STATUS_ERROR_CODE:
					EXCTHROW( TESTER_ERROR_TIMEOUT, "Motor Timeout SCROLL_WHEEL 1");
				break;
				}
			}
	   
		read[0] = pthread_data_SCROLL_WHEEL->read;
		
		for (j=0; j<pthread_data_SCROLL_WHEEL->read; j++)
		{
		sdWaveGes[0][MESS_TORQUE][j] = pthread_data_SCROLL_WHEEL->data[MESS_TORQUE][j];
		sdWaveGes[0][MESS_ANGLE][j] = pthread_data_SCROLL_WHEEL->data[MESS_ANGLE][j];
		sdWaveGes[0][MESS_LIN][j] = pthread_data_SCROLL_WHEEL->data[MESS_LIN][j];
		}
		
		read[3] = pthread_data_SCROLL_WHEEL->read2;
		
		for (j=0; j<pthread_data_SCROLL_WHEEL->read2; j++)
		{
		sdWaveGes[3][MESS_TORQUE][j] = pthread_data_SCROLL_WHEEL->data2[MESS_TORQUE][j] * -1;
		sdWaveGes[3][MESS_ANGLE][j] = pthread_data_SCROLL_WHEEL->data2[MESS_ANGLE][j];
		sdWaveGes[3][MESS_LIN][j] = pthread_data_SCROLL_WHEEL->data2[MESS_LIN][j];
		}

		/* log values in text file*/
	   	SYSTEMTIME			st;
	    char                Date[128];
		
		GetLocalTime(&st);														 
		sprintf(Date, "%02d.%02d.%02d", st.wYear, st.wMonth, st.wDay);
		sprintf (log_dir, "log\\%s", Date);
		EXCCHECK( CheckDirectory(log_dir));
		
		/* save array to file SCROLL WHEEL UP*/
		sprintf(fileName, "%s\\%s_%s_%s_%d.txt", log_dir, "SCROLL_Angle_UP", ((STestParamPtr)pTID)->Production_Serial, ((STestParamPtr)pTID)->Time, ((STestParamPtr)pTID)->step);
		ArrayToFile (fileName, sdWaveGes[0][MESS_ANGLE], VAL_DOUBLE, read[0], 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_TRUNCATE);
		
		sprintf(fileName, "%s\\%s_%s_%s_%d.txt", log_dir, "SCROLL_Torque_UP", ((STestParamPtr)pTID)->Production_Serial, ((STestParamPtr)pTID)->Time, ((STestParamPtr)pTID)->step);
		ArrayToFile (fileName, sdWaveGes[0][MESS_TORQUE], VAL_DOUBLE, read[0], 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
	
		/* save array to file SCROLL WHEEL DOWN*/
		sprintf(fileName, "%s\\%s_%s_%s_%d.txt", log_dir, "SCROLL_Angle_DOWN", ((STestParamPtr)pTID)->Production_Serial, ((STestParamPtr)pTID)->Time, ((STestParamPtr)pTID)->step);
		ArrayToFile (fileName, sdWaveGes[3][MESS_ANGLE], VAL_DOUBLE, read[3], 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_TRUNCATE);
		
		sprintf(fileName, "%s\\%s_%s_%s_%d.txt", log_dir, "SCROLL_Torque_DOWN", ((STestParamPtr)pTID)->Production_Serial, ((STestParamPtr)pTID)->Time, ((STestParamPtr)pTID)->step);
		ArrayToFile (fileName, sdWaveGes[3][MESS_TORQUE], VAL_DOUBLE, read[3], 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
	
		
		
	/* Wait thread to finish : hThreadTorque_TIPP_WIPE*/
	pexception = ThreadStatus(hThreadTorque_TIPP_WIPE, 40000, &status);
		if(pexception)
			{
			EXCDELETE(pexception);
			EXCTHROW(-1, "Motor Timeout TIPE_WHEEL");
			}
		
		else
			{
				switch(status)
				{
				case THREAD_STATUS_STILL_ACTIVE:
					EXCTHROW( TESTER_ERROR_STILL_ACTIVE, "Thread Handle Status: STILL_ACTIVE");
				break;
				case THREAD_STATUS_ERROR_CODE:
					EXCTHROW( TESTER_ERROR_TIMEOUT, "Motor Timeout TIPE_WHEEL 1");
				break;
				}
			}
	    read[1] = pthread_data_TIPP_WIPE->read;
		for (j=0; j<pthread_data_TIPP_WIPE->read; j++)
		{
		sdWaveGes[1][MESS_TORQUE][j] = pthread_data_TIPP_WIPE->data[MESS_TORQUE][j];	
		sdWaveGes[1][MESS_ANGLE][j] = pthread_data_TIPP_WIPE->data[MESS_ANGLE][j];
		sdWaveGes[1][MESS_LIN][j] = pthread_data_TIPP_WIPE->data[MESS_LIN][j];
		}
		
		read[2] = pthread_data_TIPP_WIPE->read2;								
		for (j=0; j<pthread_data_TIPP_WIPE->read2; j++)						
		{
		sdWaveGes[2][MESS_TORQUE][j] = pthread_data_TIPP_WIPE->data2[MESS_TORQUE][j];	
		sdWaveGes[2][MESS_ANGLE][j] = pthread_data_TIPP_WIPE->data2[MESS_ANGLE][j];
		sdWaveGes[2][MESS_LIN][j] = pthread_data_TIPP_WIPE->data2[MESS_LIN][j];
		}
		

	
	/* add index palett and serial part */
	DefaultCtrl (gs_TestPanel4, PANEL_4_PALETT_STATION_60);
	DefaultCtrl (gs_TestPanel4, PANEL_4_SERIAL_STATION_60);
	SetCtrlVal (gs_TestPanel4, PANEL_4_PALETT_STATION_60, ((STestParamPtr)pTID)->PALETT);
	SetCtrlVal (gs_TestPanel4, PANEL_4_SERIAL_STATION_60, ((STestParamPtr)pTID)->Production_Serial);
	


	/*-------------------- Analyse Waveform Torque/Angle SCROLL_WHEEL UP */
	DeleteGraphPlot (gs_TestPanel4, PANEL_4_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	
	double TorqueDetactedOUT_peaks[200];
	double AngleDetactedOUT_peaks[200];
	int    Locations_peaks[200];
	int	   len_TorqueDetactedOUT_peaks,
		   len_AngleDetactedOUT_peaks,
		   len_Locations_peaks;
		   
	double TorqueDetactedOUT_valleys[200];
	double AngleDetactedOUT_valleys[200];
	int	   Locations_valleys[200];
	int	   len_TorqueDetactedOUT_valleys,
		   len_AngleDetactedOUT_valleys,
		   len_Locations_valleys;
	
	double mean;	
	int    Pulse_Up;
	
	double  Moyenne_Couple_M1_UP = 0;
	double  Moyenne_Couple_M1_DOWN = 0;
	
	/* allocations */
	BufferTorqueScrollWheelFiltred_UP=calloc(read[0]*2,sizeof(double));	
	BufferAngleScrollWheelFiltred_UP =calloc(read[0]*2,sizeof(double));
	
	/* Filter bessel */
	Filtre(sdWaveGes[0][MESS_TORQUE], read[0], Enum_BesselFilter, Enum1_Lowpass, 3, 0.45, 0.125, 23, BufferTorqueScrollWheelFiltred_UP, &n);
	
	/* conversion N en Nmm */
	for (i=0; i<n;i++)
	{
		BufferTorqueScrollWheelFiltred_UP[i] = BufferTorqueScrollWheelFiltred_UP[i] * 13.1;//13.1;
		BufferAngleScrollWheelFiltred_UP[i] =sdWaveGes[0][MESS_ANGLE][i];
	}
	
	/*PLOT Filtered graph */
	PlotXY (gs_TestPanel4, PANEL_4_GRAPH, BufferAngleScrollWheelFiltred_UP, BufferTorqueScrollWheelFiltred_UP, n ,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE);
	
	for (i=0; i<n;i++)
		Tab_LIN[i] = sdWaveGes[0][MESS_LIN][i] * SCALE_BIT_SCROLL;

	PlotXY (gs_TestPanel4, PANEL_4_GRAPH, BufferAngleScrollWheelFiltred_UP, Tab_LIN, n ,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLACK);
	
	Mean (BufferTorqueScrollWheelFiltred_UP, n, &mean);
	
	/* peak detector */	
	//Peaks_Detactor(BufferTorqueScrollWheelFiltred_UP, n, Enum2_Peaks, 3 , (int)ceil( mean), &numPeaks_up, Locations_peaks, &len_Locations_peaks); //ARAIBIA 17.06.2021
	Peaks_Detactor(BufferTorqueScrollWheelFiltred_UP, n, Enum2_Peaks, 3 , -30, &numPeaks_up, Locations_peaks, &len_Locations_peaks);
	
	for(i=0; i<numPeaks_up; i++)
	{
		AngleDetactedOUT_peaks[i] = BufferAngleScrollWheelFiltred_UP[Locations_peaks[i]];
		TorqueDetactedOUT_peaks[i] = BufferTorqueScrollWheelFiltred_UP[Locations_peaks[i]];
	}
	
	for(i=0; i<numPeaks_up; i++)
	{
		PlotPoint (gs_TestPanel4, PANEL_4_GRAPH, AngleDetactedOUT_peaks[i], TorqueDetactedOUT_peaks[i], VAL_SOLID_SQUARE, VAL_GREEN);   
	}
	
	Mean (TorqueDetactedOUT_peaks, numPeaks_up, &Moyenne_Couple_M1_UP);
	PlotLine (gs_TestPanel4, PANEL_4_GRAPH, 0, Moyenne_Couple_M1_UP, BufferAngleScrollWheelFiltred_UP[n-1], Moyenne_Couple_M1_UP, VAL_MAGENTA);

	
	/* valley detector */
	//Peaks_Detactor(BufferTorqueScrollWheelFiltred_UP, n, Enum2_Valleys, 3 , (int)ceil( mean), &numValleys_up, Locations_valleys, &len_Locations_valleys);  //ARAIBIA 17.06.2021
	Peaks_Detactor(BufferTorqueScrollWheelFiltred_UP, n, Enum2_Valleys, 3 , 30, &numValleys_up, Locations_valleys, &len_Locations_valleys);  
	
	for(i=0; i<numValleys_up; i++)
	{
		AngleDetactedOUT_valleys[i] = BufferAngleScrollWheelFiltred_UP[Locations_valleys[i]];
		TorqueDetactedOUT_valleys[i] = BufferTorqueScrollWheelFiltred_UP[Locations_valleys[i]];
	}
	
	for(i=0; i<numValleys_up; i++)
	{
		PlotPoint (gs_TestPanel4, PANEL_4_GRAPH, AngleDetactedOUT_valleys[i], TorqueDetactedOUT_valleys[i], VAL_SOLID_SQUARE, VAL_RED);   
	}
		
#if 0    /* formerly excluded lines */
	Mean (TorqueDetactedOUT_valleys, numValleys_up, &Moyenne_Couple_M2_UP);
	PlotLine (gs_TestPanel4, PANEL_4_GRAPH, 0, Moyenne_Couple_M2_UP, BufferAngleScrollWheelFiltred_UP[n-1], Moyenne_Couple_M2_UP, VAL_MAGENTA);
#endif   /* formerly excluded lines */
	
	/* NumPulse LIN */
	m=0;
	for (i=0; i <read[0]; i++)
	{
		Value = (int)sdWaveGes[0][MESS_LIN][i];
		if (Value == 1 && Pulse_Up == TRUE)
			{
				NumPulse_UP++;
				Pulse_Up = FALSE;
				 m=i;
			}	
			else if (Value == 0)
			{
				Pulse_Up = TRUE;
			}
	}
		
	/*-------------------- Analyse Waveform Torque/Angle SCROLL_WHEEL DOWN */
	
	DeleteGraphPlot (gs_TestPanel4, PANEL_4_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
	
	/* allocations */
	BufferTorqueScrollWheelFiltred_DOWN=calloc(read[3]*2,sizeof(double));	
	BufferAngleScrollWheelFiltred_DOWN =calloc(read[3]*2,sizeof(double)); 
  
		/* Filter bessel */
	Filtre(sdWaveGes[3][MESS_TORQUE], read[3], Enum_BesselFilter, Enum1_Lowpass, 3, 0.45, 0.25, 15, BufferTorqueScrollWheelFiltred_DOWN, &n);
	
	/* conversion N en Nmm */
	for (i=0; i<n;i++)
	{
		BufferTorqueScrollWheelFiltred_DOWN[i] = BufferTorqueScrollWheelFiltred_DOWN[i] * 13.1;//13.1;
		BufferAngleScrollWheelFiltred_DOWN[i] =sdWaveGes[3][MESS_ANGLE][i];
	}
	
	/*PLOT Filtered graph */
	PlotXY (gs_TestPanel4, PANEL_4_GRAPH_3, BufferAngleScrollWheelFiltred_DOWN, BufferTorqueScrollWheelFiltred_DOWN, n ,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE);
	
	for (i=0; i<n;i++)
		Tab_LIN[i] = sdWaveGes[3][MESS_LIN][i] * SCALE_BIT_SCROLL;

	PlotXY (gs_TestPanel4, PANEL_4_GRAPH_3, BufferAngleScrollWheelFiltred_DOWN, Tab_LIN, n ,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLACK);
	
	Mean (BufferTorqueScrollWheelFiltred_DOWN, n, &mean);
	
	/* peak detector */	
	Peaks_Detactor(BufferTorqueScrollWheelFiltred_DOWN, n, Enum2_Peaks, 3 , (int)ceil( mean), &numPeaks_down, Locations_peaks, &len_Locations_peaks);

	for(i=0; i<numPeaks_down; i++)
	{
		AngleDetactedOUT_peaks[i] = BufferAngleScrollWheelFiltred_DOWN[Locations_peaks[i]];
		TorqueDetactedOUT_peaks[i] = BufferTorqueScrollWheelFiltred_DOWN[Locations_peaks[i]];
	}
	
	for(i=0; i<numPeaks_down; i++)
	{
		PlotPoint (gs_TestPanel4, PANEL_4_GRAPH_3, AngleDetactedOUT_peaks[i], TorqueDetactedOUT_peaks[i], VAL_SOLID_SQUARE, VAL_GREEN);   
	}
	
	Mean (TorqueDetactedOUT_peaks, numPeaks_down, &Moyenne_Couple_M1_DOWN);
	PlotLine (gs_TestPanel4, PANEL_4_GRAPH_3, 0, Moyenne_Couple_M1_DOWN, BufferAngleScrollWheelFiltred_DOWN[n-1], Moyenne_Couple_M1_DOWN, VAL_MAGENTA);

	
	/* valley detector */
	Peaks_Detactor(BufferTorqueScrollWheelFiltred_DOWN, n, Enum2_Valleys, 3 , (int)ceil( mean), &numValleys_down, Locations_valleys, &len_Locations_valleys);
	
	for(i=0; i<numValleys_down; i++)
	{
		AngleDetactedOUT_valleys[i] = BufferAngleScrollWheelFiltred_DOWN[Locations_valleys[i]];
		TorqueDetactedOUT_valleys[i] = BufferTorqueScrollWheelFiltred_DOWN[Locations_valleys[i]];
	}
	
	for(i=0; i<numValleys_down; i++)
	{
		PlotPoint (gs_TestPanel4, PANEL_4_GRAPH_3, AngleDetactedOUT_valleys[i], TorqueDetactedOUT_valleys[i], VAL_SOLID_SQUARE, VAL_RED);   
	}
		
#if 0    /* formerly excluded lines */
	Mean (TorqueDetactedOUT_valleys, numValleys_down, &Moyenne_Couple_M2_DOWN);
	PlotLine (gs_TestPanel4, PANEL_4_GRAPH_3, 0, Moyenne_Couple_M2_DOWN, BufferAngleScrollWheelFiltred_DOWN[n-1], Moyenne_Couple_M2_DOWN, VAL_MAGENTA);
#endif   /* formerly excluded lines */
	
	/* NumPulse LIN */
	m=0;
	for (i=0; i <read[0]; i++)
	{
		Value = (int)sdWaveGes[0][MESS_LIN][i];
		if (Value == 1 && Pulse_Up == TRUE)
			{
				NumPulse_DOWN++;
				Pulse_Up = FALSE;
				 m=i;
			}	
			else if (Value == 0)
			{
				Pulse_Up = TRUE;
			}
	}
	

	
	/*---------------------  Analyse Waveform Torque/Angle TIP WHEEL DOWN */
	
	//Filter_Table (sdWaveGes[1][MESS_TORQUE], read[1]);
	
	MathFunction2 (sdWaveGes[1], read[1], 
				  	  &dmF1V, &dmS1V, 
					  &dmF2V, &dmS2V, 
					  &dmF3V, &dmS3V, 
					  &dmF4V, &dmS4V,
					  &dmF5V, &dmS5V);
		
	DeleteGraphPlot (gs_TestPanel4, PANEL_4_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
	DeleteGraphAnnotation(gs_TestPanel4, PANEL_4_GRAPH_2, -1);
	//SetAxisRange (gs_TestPanel4, PANEL_4_GRAPH_2, VAL_MANUAL, getMinDoubleArray2(sdWaveGes[1][MESS_ANGLE]), 28.0, VAL_MANUAL, 0, 0.1); 
	//SetAxisRange (gs_TestPanel4, PANEL_4_GRAPH_2, VAL_MANUAL, 0, 32.0, VAL_MANUAL, 0, 0.1);
	
	
	PlotXY (gs_TestPanel4, PANEL_4_GRAPH_2, &sdWaveGes[1][MESS_ANGLE], &sdWaveGes[1][MESS_TORQUE], read[1],VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); 
	PlotXY (gs_TestPanel4, PANEL_4_GRAPH_2, &sdWaveGes[1][MESS_ANGLE], &sdWaveGes[1][MESS_LIN], read[1],VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); 
	
	Tab_Angle[0] = dmS1V;
	Tab_Angle[1] = dmS2V; 
	Tab_Angle[2] = dmS3V; 
	Tab_Angle[3] = dmS4V;
	Tab_Angle[4] = dmS5V;

	Tab_Force[0] = dmF1V;
	Tab_Force[1] = dmF2V; 
	Tab_Force[2] = dmF3V; 
	Tab_Force[3] = dmF4V;
	Tab_Force[4] = dmF5V;

	PlotXY (gs_TestPanel4, PANEL_4_GRAPH_2, Tab_Angle, Tab_Force, 5, VAL_DOUBLE, VAL_DOUBLE, VAL_SCATTER, VAL_SOLID_CIRCLE, VAL_SOLID, 1, VAL_RED);
  
	
	/* offset */

	//diamater of wheel = 13mm =>0.013m
	F1_DOWN = dmF1V/1; 
//	printf ("F1_DOWN_0, %0.2f N", F1_DOWN); 
	F1_DOWN = F1_DOWN * TIPP_WHEEL_DOWN_GAIN_F1 + TIPP_WHEEL_DOWN_OFFSET_F1;
//	printf ("F1_DOWN_1, %0.2f N", F1_DOWN);
	sprintf (sValue, "%0.2f N", F1_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F1_DOWN_2, sValue);

	A1_DOWN = dmS1V;
	A1_DOWN = A1_DOWN * TIPP_WHEEL_DOWN_GAIN_A1 + TIPP_WHEEL_DOWN_OFFSET_A1;
	sprintf (sValue, "%0.2f °", A1_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A1_DOWN_2, sValue);
	
	F2_DOWN = dmF2V /1;
	F2_DOWN = F2_DOWN * TIPP_WHEEL_DOWN_GAIN_F2 + TIPP_WHEEL_DOWN_OFFSET_F2; 
	sprintf (sValue, "%0.2f N", F2_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F2_DOWN_2, sValue);

	A2_DOWN = dmS2V; 
	A2_DOWN = A2_DOWN * TIPP_WHEEL_DOWN_GAIN_A2 + TIPP_WHEEL_DOWN_OFFSET_A2; 
	sprintf (sValue, "%0.2f °", A2_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A2_DOWN_2, sValue);

	F3_DOWN = dmF3V /1;
	F3_DOWN = F3_DOWN * TIPP_WHEEL_DOWN_GAIN_F3 + TIPP_WHEEL_DOWN_OFFSET_F3;
	sprintf (sValue, "%0.2f N", F3_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F3_DOWN_2, sValue);

	A3_DOWN = dmS3V; 
	A3_DOWN = A3_DOWN * TIPP_WHEEL_DOWN_GAIN_A3 + TIPP_WHEEL_DOWN_OFFSET_A3; 
	sprintf (sValue, "%0.2f °", A3_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A3_DOWN_2, sValue);
	
	F4_DOWN = dmF4V /1;
	F4_DOWN = F4_DOWN * TIPP_WHEEL_DOWN_GAIN_F4 + TIPP_WHEEL_DOWN_OFFSET_F4;
	sprintf (sValue, "%0.2f N", F4_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F4_DOWN_2, sValue);

	A4_DOWN = dmS4V;
	A4_DOWN = A4_DOWN * TIPP_WHEEL_DOWN_GAIN_A4 + TIPP_WHEEL_DOWN_OFFSET_A4; 
	sprintf (sValue, "%0.2f °", A4_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A4_DOWN_2, sValue);
	
	F5_DOWN = dmF5V /1;
//	F5_DOWN = F5_DOWN * TIPP_WHEEL_DOWN_GAIN_F5 + TIPP_WHEEL_DOWN_OFFSET_F5;
	sprintf (sValue, "%0.2f N", F3_DOWN+1.0);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F5_DOWN_2, sValue);

	A5_DOWN = dmS5V; 
	A5_DOWN = A5_DOWN * TIPP_WHEEL_DOWN_GAIN_A5 + TIPP_WHEEL_DOWN_OFFSET_A5; 
	sprintf (sValue, "%0.2f °", A5_DOWN);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A5_DOWN_2, sValue);

	/*---------------------  Analyse Waveform Torque/Angle TIP WHEEL UP */
	
	//Filter_Table (sdWaveGes[2][MESS_TORQUE], read[2]);

	MathFunction2 (sdWaveGes[2], read[2], 
				  	  &dmF1V, &dmS1V, 
					  &dmF2V, &dmS2V, 
					  &dmF3V, &dmS3V, 
					  &dmF4V, &dmS4V,
					  &dmF5V, &dmS5V);
		
	//SetAxisRange (gs_TestPanel4, PANEL_4_GRAPH_2, VAL_MANUAL, getMinDoubleArray2(sdWaveGes[2][MESS_ANGLE]), 30.0, VAL_MANUAL, 0, 0.1); 
	//SetAxisRange (gs_TestPanel4, PANEL_4_GRAPH_2, VAL_MANUAL, 0, 32.0, VAL_MANUAL, 0, 0.1); 
	PlotXY (gs_TestPanel4, PANEL_4_GRAPH_2, &sdWaveGes[2][MESS_ANGLE], &sdWaveGes[2][MESS_TORQUE], read[2],VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_RED); 
	PlotXY (gs_TestPanel4, PANEL_4_GRAPH_2, &sdWaveGes[2][MESS_ANGLE], &sdWaveGes[2][MESS_LIN], read[2],VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_RED); 
	
	Tab_Angle[0] = dmS1V;
	Tab_Angle[1] = dmS2V; 
	Tab_Angle[2] = dmS3V; 
	Tab_Angle[3] = dmS4V;
	Tab_Angle[4] = dmS5V;

	Tab_Force[0] = dmF1V;
	Tab_Force[1] = dmF2V; 
	Tab_Force[2] = dmF3V; 
	Tab_Force[3] = dmF4V;
	Tab_Force[4] = dmF5V;
	
	PlotXY (gs_TestPanel4, PANEL_4_GRAPH_2, Tab_Angle, Tab_Force, 5, VAL_DOUBLE, VAL_DOUBLE, VAL_SCATTER, VAL_SOLID_CIRCLE, VAL_SOLID, 1, VAL_BLUE);
	
	/* offset */
	F1_UP = dmF1V; //diamater of wheel = 13mm =>0.013m
	F1_UP = F1_UP * TIPP_WHEEL_UP_GAIN_F1 + TIPP_WHEEL_UP_OFFSET_F1;
	sprintf (sValue, "%0.2f N", F1_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F1_UP_2, sValue);

	A1_UP = dmS1V; 
	A1_UP = A1_UP * TIPP_WHEEL_UP_GAIN_A1 + TIPP_WHEEL_UP_OFFSET_A1;
	sprintf (sValue, "%0.2f °", A1_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A1_UP_2, sValue);

	F2_UP  = dmF2V;
	F2_UP = F2_UP * TIPP_WHEEL_UP_GAIN_F2 + TIPP_WHEEL_UP_OFFSET_F2;
	sprintf (sValue, "%0.2f N", F2_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F2_UP_2, sValue);

	A2_UP = dmS2V; 
	A2_UP = A2_UP * TIPP_WHEEL_UP_GAIN_A2 + TIPP_WHEEL_UP_OFFSET_A2;
	sprintf (sValue, "%0.2f °", A2_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A2_UP_2, sValue);

	F3_UP  = dmF3V;
	F3_UP = F3_UP * TIPP_WHEEL_UP_GAIN_F3 + TIPP_WHEEL_UP_OFFSET_F3;
	sprintf (sValue, "%0.2f N", F3_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F3_UP_2, sValue);

	A3_UP = dmS3V;
	A3_UP = A3_UP * TIPP_WHEEL_UP_GAIN_A3 + TIPP_WHEEL_UP_OFFSET_A3;
	sprintf (sValue, "%0.2f °", A3_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A3_UP_2, sValue);

	F4_UP  = dmF4V;
	F4_UP = F4_UP * TIPP_WHEEL_UP_GAIN_F4 + TIPP_WHEEL_UP_OFFSET_F4;
	sprintf (sValue, "%0.2f N", F4_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F4_UP_2, sValue);

	A4_UP = dmS4V;
	A4_UP = A4_UP * TIPP_WHEEL_UP_GAIN_A4 + TIPP_WHEEL_UP_OFFSET_A4;
	sprintf (sValue, "%0.2f °", A4_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A4_UP_2, sValue);
	
	F5_UP  = dmF5V;
//	F5_UP = F5_UP * TIPP_WHEEL_UP_GAIN_F5 + TIPP_WHEEL_UP_OFFSET_F5;
	sprintf (sValue, "%0.2f N", F3_UP+1.0);
	SetCtrlVal (gs_TestPanel4, PANEL_4_F5_UP_2, sValue);

	A5_UP = dmS5V; 
	A5_UP = A5_UP * TIPP_WHEEL_UP_GAIN_A5 + TIPP_WHEEL_UP_OFFSET_A5;
	sprintf (sValue, "%0.2f °", A5_UP);
	SetCtrlVal (gs_TestPanel4, PANEL_4_A5_UP_2, sValue);
//	
//	
	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	sprintf (log_dir, "%s\\STEP_%d", ((STestParamPtr)pTID)->csv_dir, ((STestParamPtr)pTID)->step);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	//sprintf(filePath, "%s\\%s",((STestParamPtr)pTID)->TicketDirPath, fileName);
	CONVERT_GRAPH_TO_PNG_STATION_60(fileName);
	TEST_VALUE_SET( "image", fileName);
	}
	else  //if (((STestParamPtr)pTID)->fault_retest == FALSE)
	{
	sprintf(fileName, "%s_%s_%d.png", ((STestParamPtr)pTID)->Production_Serial, ((STestParamPtr)pTID)->Time, ((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",((STestParamPtr)pTID)->TicketDirPath, fileName);
	CONVERT_GRAPH_TO_PNG_STATION_60(filePath);
	TEST_VALUE_SET( "image", fileName);
	}
//	
//	
	/* check limit TIPP WHEEL DOWN Force/Angle */
	
	double TactileRatio;
	
	/*---- DOWN P1 ---- */ 
	LIMIT_CHECK(REAL64, F1_DOWN); //audit
//	LIMIT_CHECK_EXT(REAL64, F1_DOWN,pexception);
	//LIMIT_CHECK(REAL64, A1_DOWN);
	LIMIT_CHECK_EXT(REAL64, A1_DOWN, pexception);	

	
	/* Tactile Ratio (F1-F2)/F1 of P1 */
	TactileRatio = ((F1_DOWN-F2_DOWN)/F1_DOWN) * 100;
	if (F1_DOWN == 0) TactileRatio = 0; 
	else if (TactileRatio == NAN) TactileRatio = 0; 
	LIMIT_CHECK(REAL64, TactileRatio); //audit
//	LIMIT_CHECK_EXT(REAL64, TactileRatio,pexception);
	//LIMIT_CHECK(REAL64, A2_DOWN);
	LIMIT_CHECK_EXT(REAL64, A2_DOWN, pexception);
	
	
	
	/*---- DOWN P10 ---- */
	LIMIT_CHECK(REAL64, F3_DOWN);	// audit
//	LIMIT_CHECK_EXT(REAL64, F3_DOWN,pexception); 
	//LIMIT_CHECK(REAL64, A3_DOWN);
	LIMIT_CHECK_EXT(REAL64, A3_DOWN, pexception);
	
	/* Tactile Ratio (F3-F4)/F3 of P10 */
	TactileRatio = ((F3_DOWN-F4_DOWN)/F3_DOWN) * 100;
	if (F3_DOWN == 0) TactileRatio = 0; 
	else if (TactileRatio == NAN) TactileRatio = 0;
	LIMIT_CHECK(REAL64, TactileRatio);  // audit
//	LIMIT_CHECK(REAL64, A4_DOWN);
//	LIMIT_CHECK_EXT(REAL64, TactileRatio, pexception);
	LIMIT_CHECK_EXT(REAL64, A4_DOWN, pexception);
	
	//LIMIT_CHECK(REAL64, A5_DOWN);
	LIMIT_CHECK_EXT(REAL64, A5_DOWN, pexception);

	
	/* check limit TIPP WHEEL UP Force/Angle */
	
	/*---- UP P1 ---- */ 
	LIMIT_CHECK(REAL64, F1_UP); // audit
//	LIMIT_CHECK_EXT(REAL64, F1_UP,pexception);
	//LIMIT_CHECK(REAL64, A1_UP);
	LIMIT_CHECK_EXT(REAL64, A1_UP, pexception);
	
	/* Tactile Ratio (F1-F2)/F1 of P1 */
	TactileRatio = ((F1_UP-F2_UP)/F1_UP) * 100;
	if (F1_UP == 0) TactileRatio = 0; 
	else if (TactileRatio == NAN) TactileRatio = 0; 
	LIMIT_CHECK(REAL64, TactileRatio); // audit
//	LIMIT_CHECK_EXT(REAL64, TactileRatio, pexception);      
	//LIMIT_CHECK(REAL64, A2_UP);
	LIMIT_CHECK_EXT(REAL64, A2_UP, pexception);
	
	
	/*---- UP P10 ---- */ 
	LIMIT_CHECK(REAL64, F3_UP);  // audit
//	LIMIT_CHECK_EXT(REAL64, F3_UP, pexception);      
	//LIMIT_CHECK(REAL64, A3_UP);
	LIMIT_CHECK_EXT(REAL64, A3_UP, pexception);
	
	/* Tactile Ratio (F3-F4)/F3 of P10 */
	TactileRatio = ((F3_UP-F4_UP)/F3_UP) * 100;
	if (F1_UP == 0) TactileRatio = 0; 
	else if (TactileRatio == NAN) TactileRatio = 0; 
	LIMIT_CHECK(REAL64, TactileRatio); // audit
	//LIMIT_CHECK(REAL64, A4_UP);
//	LIMIT_CHECK_EXT(REAL64, TactileRatio, pexception);
	LIMIT_CHECK_EXT(REAL64, A4_UP, pexception);
	
	//LIMIT_CHECK(REAL64, A5_UP);
	LIMIT_CHECK_EXT(REAL64, A5_UP, pexception);

	/* check LIN limit TIPP WHEEL DOWN P1 */
	Value = 0;
	for (i=0; i <read[1]; i++)
	{
		Value = (int)sdWaveGes[1][MESS_LIN][i];
		if (Value == 5)
		{
			Index_Down_P1 = i;
			break;
		}
		else if (Value != 0)
			break;
	}
	LIMIT_CHECK(INT32, Value);
//	LIMIT_CHECK_EXT(INT32, Value,pexception);  // audit
	
	/* check LIN limit TIPP WHEEL DOWN P10 */
	Value = 0;
	for (i=Index_Down_P1; i<read[1]; i++)
	{
		Value = (int)sdWaveGes[1][MESS_LIN][i];
		if (Value == 6)
			break;
		else if (Value != 5)
			break;
	}
//	LIMIT_CHECK(INT32, Value);
	LIMIT_CHECK_EXT(INT32, Value,pexception); // audit
	
	
	/* check LIN limit TIPP WHEEL UP P1 */
	Value = 0;
	for (i=0; i<read[2]; i++)
	{
		Value = (int)sdWaveGes[2][MESS_LIN][i];
		if (Value == 1)
		{
			Index_Up_P1 = i;
			break;
		}
		else if (Value != 0)
			break;
	}
	LIMIT_CHECK(INT32, Value);
//	LIMIT_CHECK_EXT(INT32, Value, pexception);   // audit
	
	/* check LIN limit TIPP WHEEL UP P10 */
	Value = 0;
	for (i=Index_Up_P1; i <read[2]; i++)
	{
		Value = (int)sdWaveGes[2][MESS_LIN][i];
		if (Value == 2)
			break;
		else if (Value != 1)
			break;
	}
	LIMIT_CHECK(INT32, Value);
//	LIMIT_CHECK_EXT(INT32, Value, pexception); 
//	
	

/* -------------- check Nbr Detents LIN Scroll Wheel DOWN*/
	 LIMIT_CHECK(INT32, NumPulse_DOWN);
	 
	 LIMIT_CHECK_EXT(INT32, numPeaks_down,pexception);
	 EXCDELETE(pexception);
	 LIMIT_CHECK_EXT(INT32, numValleys_down,pexception);
	 EXCDELETE(pexception);
	 LIMIT_CHECK(REAL64, Moyenne_Couple_M1_DOWN); 
	 //LIMIT_CHECK_EXT(REAL64, Moyenne_Couple_M1_DOWN,pexception); RABIE 01072021
	 //EXCDELETE(pexception);
	 
	/* -------------- check Nbr Detents LIN Scroll Wheel UP*/
	 LIMIT_CHECK(INT32, NumPulse_UP);
	 
	 LIMIT_CHECK_EXT(INT32, numPeaks_up,pexception);
	 EXCDELETE(pexception);
	 LIMIT_CHECK_EXT(INT32, numValleys_up,pexception);
	 EXCDELETE(pexception);
	 LIMIT_CHECK(REAL64, Moyenne_Couple_M1_UP); 
	// LIMIT_CHECK_EXT(REAL64, Moyenne_Couple_M1_UP,pexception); 
	// EXCDELETE(pexception);
Error:
	
	if (pexception)
	{
	ThreadStatus(hThreadTorque_SCROLL_WHEEL, 20000, &status);	
	ThreadStatus(hThreadTorque_TIPP_WIPE, 20000, &status);	
	}
	if (pthread_data_SCROLL_WHEEL) free(pthread_data_SCROLL_WHEEL);
	if (pthread_data_TIPP_WIPE) free(pthread_data_TIPP_WIPE);
	Inhibit_Rodage_Pushs[STATION_60] = FALSE;
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_60]); 
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_10)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_50
*********************************************************************************/

FCE_HELP(MFL, 60_50)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_50)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_50)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_50)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	int 	status;
	double 	Currentangle;

	
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_LEFT);
	
	Test_Torque_Run = TRUE;
	
#if 0    /* formerly excluded lines */
	pmotor[MOTOR_STATION_60_LEFT]->CurrentPosition(pmotor[MOTOR_STATION_60_LEFT], MOTOR_STATION_60_LEFT, &Currentangle);

	//printf ("Currentangle = %f\r\n", Currentangle);

	if (Currentangle < -7.0 || Currentangle > 1.0)
	{
	 //printf ("Currentangle = %f \r\n", Currentangle);
	 
	/* reset Sensor motor LEFT */
#endif   /* formerly excluded lines */
//	paxis[AXIS_STATION_60_LEFT]->ControllerStatus(pmotor[AXIS_STATION_60_LEFT], AXIS_STATION_60_LEFT, &status);
#if 0    /* formerly excluded lines */
	}
#endif   /* formerly excluded lines */
		
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_50)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_51
*********************************************************************************/

FCE_HELP(MFL, 60_51)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_51)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_51)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_51)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(DAQ);
	TEST_USE(CAN);
	TEST_USE(TESTER);
	int 	status;
	double 	Currentangle;

	
		
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(MFL, 60_51)
{
    TESTCLEANUP_BEGIN();
	
	
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_90
 *********************************************************************************/
FCE_HELP(MFL, 60_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 60_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 60_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(MFL, 60_90)
{
    TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	SSensorThreadDataPtr	pthread_data;
	HANDLE  				hThreadSensor;
	
	Inhibit_Rodage_Pushs[STATION_60] = FALSE;

   	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_RIGHT);
 	INITIALIZE_AXIS (SYSTEM_AXIS_STATION_60_LEFT);
	
	DISPLAY_TESTSEQ_RESULT(STATION_60);
	
	/* Start thread rodage for wheel right */
	pthread_data = (SSensorThreadDataPtr)calloc(1, sizeof(SSensorThreadData)); 
	EXCCHECKALLOC( pthread_data);
	pthread_data->paxis = paxis[SYSTEM_AXIS_STATION_60_LEFT];
	
	EXCCHECK(ThreadStart(Thread_Reset_Sensor, pthread_data, &hThreadSensor, NULL));

	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	
	PANEL_INI(STATION_70);

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 60_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);											 
}

/*********************************************************************************
 * Function 70_00
*********************************************************************************/

FCE_HELP(MFL, 70_00)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	Inhibit_Rodage_Pushs[STATION_70] = TRUE;
	TESTER_PLUG_PANEL_STATION_70(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_00)
{
    TESTCLEANUP_BEGIN();
    
Error:  
	LogTest_Steps("70_00"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_01
*********************************************************************************/

FCE_HELP(MFL, 70_01)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_70(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_01)
{
    TESTCLEANUP_BEGIN();
    
Error:
	LogTest_Steps("70_01"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_02
*********************************************************************************/

FCE_HELP(MFL, 70_02)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_02)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_02)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_02)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	TESTER_ILLUMINATION_STATION_70(TRUE);
	Sleep(50);

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_02)
{
    TESTCLEANUP_BEGIN();
    
Error:  
	LogTest_Steps("70_02"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_03
*********************************************************************************/

FCE_HELP(MFL, 70_03)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_03)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_03)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_03)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_ILLUMINATION_STATION_70(FALSE);

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_03)
{
    TESTCLEANUP_BEGIN();
    
Error:  
	LogTest_Steps("70_03"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_04
*********************************************************************************/

FCE_HELP(MFL, 70_04)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Start Thread Writing" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_04)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_04)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_04)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);
	SLinThreadDataPtr	pthread_Lin_data;
	HANDLE				hThreadLin;
    int station = STATION_70;
	clock_t 					_start_test_clock;
	double   					time;
	
	/* Start Loop Thread Read */	 
	iFlag_Thread_Lin_Read[station] = TRUE;

	/* Start thread LIN communication */
	pthread_Lin_data = (SLinThreadDataPtr)calloc(1, sizeof(SLinThreadData)); 
	EXCCHECKALLOC( pthread_Lin_data);
	pthread_Lin_data->pcan = pcan;
	pthread_Lin_data->timeout = 500;
	pthread_Lin_data->station = station;
	sprintf (pthread_Lin_data->lin_channel, LIN_CHANNEL_STATION_70);
	EXCCHECK(ThreadStart(Thread_Lin_Read, pthread_Lin_data, &hThreadLin, NULL));

	/* Start Loop Thread Write */	 
	iFlag_Thread_Lin_Write[station] = TRUE;
	iFlag_Write_Wake_Up[station] = TRUE;
	iFlag_Delete_Cache [station] = TRUE;
		
	/* Start thread 2 LIN communication */
	pthread_Lin_data = (SLinThreadDataPtr)calloc(1, sizeof(SLinThreadData)); 
	EXCCHECKALLOC( pthread_Lin_data);
	pthread_Lin_data->pcan = pcan;
	pthread_Lin_data->timeout = 500;
	pthread_Lin_data->station = station;
	sprintf (pthread_Lin_data->lin_channel, LIN_CHANNEL_STATION_70);
	EXCCHECK(ThreadStart(Thread_Lin_Write, pthread_Lin_data, &hThreadLin, NULL));
	
	Sleep (50);
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[station]); 

	/* send Valeo_production_Key */
	iFlag_Write_Valeo_production_Key[station] = TRUE;
	
	/* check run normal mode */
	_start_test_clock =  clock();
	do {
		time = (double)(clock()-_start_test_clock);
	}while (iFlag_Run_Normal_Mode[station] == 0 && time < 500);
	
	LIMIT_CREATE_EX ("1","1", "[COM_LIN]")
	LIMIT_CHECK(UINT32, 	iFlag_Run_Normal_Mode[station]);
	
	/* send Valeo_production_Key */
	iFlag_Write_Valeo_production_Key[station] = TRUE;
	
	Sleep (100);
		
Error:
    TEST_END_STATION(STATION_70);
}
				   
FCE_CLEANUP(MFL, 70_04)
{
    TESTCLEANUP_BEGIN();
    
Error:
	LogTest_Steps("70_04"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_05
*********************************************************************************/

FCE_HELP(MFL, 70_05)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : stop normal mode" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_05)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_05)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_05)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);

	/* Stop Loop Thread Write */	 
	iFlag_Thread_Lin_Write[STATION_70] = FALSE;
	
	/* Stop Loop Thread Read */	 
	iFlag_Thread_Lin_Read[STATION_70] = FALSE;

	/* wait before Unplug connector to Go to sleep */
	Sleep (500);

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_05)
{
    TESTCLEANUP_BEGIN();
    
Error: 
	LogTest_Steps("70_05"); 
    TESTCLEANUP_END(STATION_70);
}
/*********************************************************************************
 * Function 70_08
*********************************************************************************/

FCE_HELP(MFL, 70_08)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Check alive byte" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_08)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_08)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_08)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAN);

	/* save LIN communication log file */
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_70]); 
	LIMIT_CREATE_EX ("0","0", "[check alive byte]")
	LIMIT_CHECK(INT32, Error_Alive_Byte[STATION_70]);

	
Error:
    TEST_END_STATION(STATION_70);
}
				   
FCE_CLEANUP(MFL, 70_08)
{
    TESTCLEANUP_BEGIN();
    
Error: 
	LogTest_Steps("70_08"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_09
*********************************************************************************/

FCE_HELP(MFL, 70_09)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_09)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_09)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_09)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE(INT32, BG_PWM);
	VARIABLE(INT32, i);

	/* LIN variable */
	unsigned char Go_To_Sleep[8]={0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  
	int32_t 	  dataSize = 8;
	
	pexception = pcan->WriteLowLevel(pcan, LIN_CHANNEL_STATION_70, 0x3C, 18, dataSize, Go_To_Sleep);
	Sleep (150);
	
Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_09)
{
    TESTCLEANUP_BEGIN();
	
Error:
	LogTest_Steps("70_09"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_10
*********************************************************************************/

FCE_HELP(MFL, 70_10)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_10)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_10)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[128] = "";
	char value[8+1];
	
	//CameraSetLiveOff(pTID, pcameras);

	EXCCHECK( CameraSetJob(pTID, pcameras));
	
Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_10)
{
    TESTCLEANUP_BEGIN();
    
Error: 
	LogTest_Steps("70_10"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_11
*********************************************************************************/

FCE_HELP(MFL, 70_11)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_11)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_11)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_11)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char Log_Camera[128] = "";
	char data[256] = "";
	char value[1+1] = "";;
	int i;
	double time;
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	
	/* check STA LED */
	iFlag_LED_STA_RED[STATION_70] = TRUE;
	Sleep(150);  //wait illumination of STA MFL start
	
	cnt = 0;
	_start_test_clock =  clock();
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
		pexception = CameraGetdata(pcameras, STEP_DAY,TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
	
		
		if(!pexc_first) pexc_first = pexception;
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<8;i++)
		{
		value [0] = data[i];
		LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
		if(!pexc_first) pexc_first = pexception;
		}
		// Right
		for (i=8; i<15;i++)
		{
		value [0] = data[i];
		LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
		if(!pexc_first) pexc_first = pexception;
		}
		time = (double)(clock()-_start_test_clock);
	//	printf ("70_11: Test day Picto time = %f\r\n", time); 

	
	}while(pexc_first && cnt<RETEST);
	
	
	
Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_11)
{
    TESTCLEANUP_BEGIN();
    
Error:
//	LogTest_Steps("70_11"); 
    TESTCLEANUP_END(STATION_70);
}


/*********************************************************************************
 * Function 70_12
*********************************************************************************/

FCE_HELP(MFL, 70_12)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_12)
{
    TESTREGUIRE_BEGIN();
	
    REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_12)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_12)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char Log_Camera[128] = "";
	char data[256] = "";
	char value[1+1] = "";;
	int i;
	double time;
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	clock_t 		_start_test_clock; 

	/* check STA LED */
	iFlag_LED_STA_GREEN[STATION_70] = TRUE;
	Sleep(150);  //wait illumination of STA MFL start
	
	/* check STA LED */
	iFlag_LED_STA_RED[STATION_70] = TRUE;
	Sleep(150);  //wait illumination of STA MFL start
	
	
	cnt = 0;
_start_test_clock =  clock();
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
		pexception = CameraGetdata(pcameras, STEP_CHROME,TIMEOUT_CAMERA, data);
	//	printf("data=%s\r\n",data);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);

		if(!pexc_first) pexc_first = pexception;
		
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera); 

		// Left 
		for (i=0; i<6;i++)
		{
		value [0] = data[i];
		LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
		if(!pexc_first) pexc_first = pexception;
		}
		// Right
		for (i=8; i<13;i++)
		{
		value [0] = data[i];
		LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
		if(!pexc_first) pexc_first = pexception;
		}
		
		

	
	}while(pexc_first && cnt<RETEST);
	
	
Error:
	//time = (double)(clock()-_start_test_clock);
	//printf ("70_12: Test Chrome time = %f\r\n", time);
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_12)
{
    TESTCLEANUP_BEGIN();
    
Error: 
	LogTest_Steps("70_12"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_13
*********************************************************************************/

FCE_HELP(MFL, 70_13)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_13)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_13)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_13)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char Log_Camera[128] = "";
	char data[256] = "";
	char value[1+1]= "";
	int i;
	
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	double time;
	clock_t 		_start_test_clock;
	
	/* check STA LED */
	iFlag_LED_STA_GREEN[STATION_70] = TRUE;
	Sleep(150);  //wait illumination of STA MFL start

	cnt = 0;
	_start_test_clock =  clock();
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
		pexception = CameraGetdata(pcameras, STEP_NIGHT,TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);

		if(!pexc_first) pexc_first = pexception;

		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera); 

		// Left 
		for (i=0; i<8;i++)
		{
		value [0] = data[i];
		LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
		if(!pexc_first) pexc_first = pexception;
		}
		// Right
		for (i=8; i<15;i++)
		{
		value [0] = data[i];
		LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
		if(!pexc_first) pexc_first = pexception;
		}
		
		 

	
	}while(pexc_first && cnt<RETEST);
	
Error:
	time = (double)(clock()-_start_test_clock);
//	printf ("70_13: Test night Picto : time = %f\r\n", time);
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_13)
{
    TESTCLEANUP_BEGIN();
    
Error: 
	LogTest_Steps("70_13"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_14
*********************************************************************************/

FCE_HELP(MFL, 70_14)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_14)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_14)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_14)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char Log_Camera[128] = "";
	char data[256] = "";
	char value[1+1]= ""; 
	int i;
   	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	double time;
	clock_t 		_start_test_clock;

	cnt = 0;
	_start_test_clock =  clock();
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);

		memset(data, 0, 256);
		pexception = CameraGetdata(pcameras, STEP_STA,TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);

		if(!pexc_first) pexc_first = pexception;

		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<3;i++)
		{
		value [0] = data[i];
		LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
		if(!pexc_first) pexc_first = pexception;
		}
		// Right
		for (i=8; i<11;i++)
		{
		value [0] = data[i];
		LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		if (pexception && cnt==RETEST)
			EXCCHECK(pexception);
		if(!pexc_first) pexc_first = pexception;
		}

	
	}while(pexc_first && cnt<RETEST);
	
	
Error:
		 
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_14)
{
    TESTCLEANUP_BEGIN();
    
Error: 
	LogTest_Steps("70_14"); 
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_15
*********************************************************************************/

FCE_HELP(MFL, 70_15)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_15)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_15)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_15)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
   	TEST_USE(CAMERA);
	char data[128] = "";
	SCameraPtr		pcamera = NULL;

	TEST_HELP(_("Visual Check"));

	/* activate live mode */
	EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
	EXCCHECK( pcamera->Cognex_set_param(pcamera, STEP_NIGHT));
	EXCCHECK( pcamera->Cognex_Offline(pcamera));
	EXCCHECK( pcamera->Cognex_set_live(pcamera, 1));
	
	//DisplayPanel(gs_TestPanel5);
	
	
#if 0    /* formerly excluded lines */
	TESTER_ILLUMINATION_STATION_70(TRUE);
	Delay(0.2);
	TESTER_ILLUMINATION_STATION_70(FALSE);
	Delay(0.2);
	TESTER_ILLUMINATION_STATION_70(TRUE);
	Delay(0.2);
	TESTER_ILLUMINATION_STATION_70(FALSE);
	Delay(0.2);
#endif   /* formerly excluded lines */
	
	TESTER_OKNOK(TIMEOUT_DEFAULT);

Error:
	TEST_HELP(_("Push Start Cycle"));
	pcamera->Cognex_set_live(pcamera, 0);
	pcamera->Cognex_Online(pcamera);
	//HidePanel(gs_TestPanel5);
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_15)
{

    TESTCLEANUP_BEGIN();
    
Error:
	LogTest_Steps("70_15"); 
    TESTCLEANUP_END(STATION_70);
}


#if 0    /* formerly excluded lines */
/*********************************************************************************
 * Function 70_20
*********************************************************************************/

FCE_HELP(MFL, 70_20)
{
    HELP_BEGIN();
	HELP_TEST( "DMM : mesure courant" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_20)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_20)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
   	//TEST_USE(DMM);
	
	
    ViReal64  current = 0.000;

	/*- Initiate the DMM and create a session ---------------------------*/
	ErrorDMM = niDMM_init(resourceName, idQuery, reset, &vi_DMM);
	if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_init! ");	

    /*- Configure Powerline Frequency -----------------------------------*/
	ErrorDMM = niDMM_ConfigurePowerLineFrequency(vi_DMM, powerlineFreq);
	if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_ConfigurePowerLineFrequency! ");	
	            
	/*- Configure DC Measurement ----------------------------------------*/
   	ErrorDMM = niDMM_ConfigureMeasurementDigits(vi_DMM, measurementType, range, resolution);
	if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_ConfigureMeasurementDigits! ");
	
    /*- Read DC current ---------------------------------------------------*/ 
 	ErrorDMM = niDMM_Read(vi_DMM, NIDMM_VAL_TIME_LIMIT_AUTO, &current);
	if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_Read! ");
	
	
	LIMIT_CHECK(REAL64, current, pexception);
//	if (pexception ) EXCDELETE (pexception)
		
Error:
	if (pexception && (ErrorDMM < VI_SUCCESS))
		ErrorHandler(vi_DMM);
	
	
	niDMM_close(vi_DMM);
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_20)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_70);
}
#endif   /* formerly excluded lines */



/*********************************************************************************
 * Function 70_20
*********************************************************************************/

FCE_HELP(MFL, 70_20)
{
    HELP_BEGIN();
	HELP_TEST( "DMM : mesure courant" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_20)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_20)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
   	//TEST_USE(DMM);
    ViReal64  mean_Current = 0.000;
 	static   ViReal64  	*measurements = VI_NULL;
	
	double time;
	clock_t 		_start_test_clock;
	_start_test_clock =  clock();
	
	/*- Initiate the DMM and create a session ---------------------------*/
	ErrorDMM = niDMM_init(resourceName, idQuery, reset, &vi_DMM);
	if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_init! ");	

    /*- Configure Powerline Frequency -----------------------------------*/
	ErrorDMM = niDMM_ConfigurePowerLineFrequency(vi_DMM, powerlineFreq);
	if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_ConfigurePowerLineFrequency! ");	
	            
	/*- Configure DC Measurement ----------------------------------------*/
   	ErrorDMM = niDMM_ConfigureMeasurementDigits(vi_DMM, measurementType, range, resolution);
	if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_ConfigureMeasurementDigits! ");
	
	/*- Configure Trigger -----------------------------------------------*/
    ErrorDMM = niDMM_ConfigureTrigger (vi_DMM, triggerSource, triggerDelay);
	   if (ErrorDMM<0)
			EXCTHROW( -1, "Erreur niDMM_ConfigureMeasurementDigits! ");

    /*- Configure Multipoint acquisition --------------------------------*/
    ErrorDMM = niDMM_ConfigureMultiPoint (vi_DMM, 1, numOfMeas, triggerSample, triggerDelay);
	if (ErrorDMM<0)
			EXCTHROW( -1, "Erreur niDMM_ConfigureMultiPoint! ");
	
	
	/*- Fetch the data -----------------------------------------------*/
	measurements = (ViReal64 *) malloc(numOfMeas * sizeof(ViReal64));
	
	ErrorDMM = niDMM_ReadMultiPoint (vi_DMM, NIDMM_VAL_TIME_LIMIT_AUTO, numOfMeas, measurements, &numPointsRead);
	if (ErrorDMM<0)
			EXCTHROW( -1, "Erreur niDMM_FetchMultiPoint! ");
	
	Mean (measurements, numPointsRead, &mean_Current);
	
    /*- Read DC current ---------------------------------------------------*/ 
 	/*ErrorDMM = niDMM_Read(vi_DMM, NIDMM_VAL_TIME_LIMIT_AUTO, &current);
	if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_Read! ");*/
	
	LIMIT_CHECK(REAL64, mean_Current);
//	LIMIT_CHECK_EXT(REAL64, mean_Current, pexception); 
//	if (pexception ) EXCDELETE (pexception)	   // sans jugement: pour activer le jugement collentez cette ligne
		
Error:
	time = (double)(clock()-_start_test_clock);
//	printf ("70_20:time = %f\r\n", time);
	LogTest_Steps("70_20");  
	if (pexception && (ErrorDMM < VI_SUCCESS))
		ErrorHandler(vi_DMM);
	
	if (measurements) free(measurements);
	niDMM_close(vi_DMM);
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_20)
{
    TESTCLEANUP_BEGIN();
    
Error:
	LogTest_Steps("70_20");
    TESTCLEANUP_END(STATION_70);
}


 /*********************************************************************************
 * Function 70_21
*********************************************************************************/

FCE_HELP(MFL, 70_21)
{
    HELP_BEGIN();
	HELP_TEST( "DMM : mesure courant Faible" );

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_21)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_21)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_21)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
    ViReal64  current = 0.000;
	double time;
	clock_t 		_start_test_clock;
	_start_test_clock =  clock();
	
	cnt = 0;
	do 
	{	
	    _test_index=1;		
		cnt++;
		EXCDELETE(pexc_first);
		
		/*- Initiate the DMM and create a session ---------------------------*/
		ErrorDMM = niDMM_init(resourceName, idQuery, reset, &vi_DMM);
		if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_init! ");	

    	/*- Configure Powerline Frequency -----------------------------------*/
		ErrorDMM = niDMM_ConfigurePowerLineFrequency(vi_DMM, powerlineFreq);
		if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_ConfigurePowerLineFrequency! ");	
	            
		/*- Configure DC Measurement ----------------------------------------*/
   		ErrorDMM = niDMM_ConfigureMeasurementDigits(vi_DMM, measurementType, 0.02, resolution);
		if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_ConfigureMeasurementDigits! ");
	
    	/*- Read DC current ---------------------------------------------------*/ 
 		ErrorDMM = niDMM_Read(vi_DMM, NIDMM_VAL_TIME_LIMIT_AUTO, &current);
		if (ErrorDMM < VI_SUCCESS)
			EXCTHROW( -1, "Erreur niDMM_Read! ");
	
		LIMIT_CHECK_EXT(REAL64, current, pexception);
		if(!pexc_first) pexc_first = pexception;
	
		if(pexc_first)
			Sleep(50);
		
	}
	
	while(pexc_first && cnt< 15);

//	if (pexc_first ) EXCDELETE (pexception)
	if (pexc_first ) EXCCHECK (pexception) 
		
Error:
	//time = (double)(clock()-_start_test_clock);
	//printf ("70_20:time = %f\r\n", time);
	if (pexception && (ErrorDMM < VI_SUCCESS))
		ErrorHandler(vi_DMM);
	
	
	niDMM_close(vi_DMM);
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_21)
{
    TESTCLEANUP_BEGIN();
    
Error:
//	time = (double)(clock()-_start_test_clock);
//	printf ("70_21: time = %f\r\n", time);
	LogTest_Steps("70_21");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_50
*********************************************************************************/

FCE_HELP(MFL, 70_50)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_50)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_50)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_50)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE(INT32, BG_PWM);
	VARIABLE(INT32, BG_STA);
	
	PARAM_INT32_OPTIONAL("BG_PWM", &BG_PWM, 128);
	PARAM_INT32_OPTIONAL("BG_STA", &BG_STA, 128);

	
	iFlag_Delete_Cache [STATION_70] = TRUE;
	iFlag_LED_BG[STATION_70] = TRUE;
		
	Sleep(200);  //wait illumination of MFL start

	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_70]); 

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_50)
{
    TESTCLEANUP_BEGIN();
	
Error:  
	LogTest_Steps("70_50");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_60
*********************************************************************************/

FCE_HELP(MFL, 70_60)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_60)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_60)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_60)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	VARIABLE(INT32, BG_PWM);
	VARIABLE(INT32, BG_STA);
	
	PARAM_INT32_OPTIONAL("BG_PWM", &BG_PWM, 254);
	PARAM_INT32_OPTIONAL("BG_STA", &BG_STA, 254);

	iFlag_Delete_Cache [STATION_70] = TRUE;

	iFlag_LED_STA_YELLOW[STATION_70] = TRUE;
	Sleep(200);  //wait illumination of STA MFL start

	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_70]); 
	
Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_60)
{
    TESTCLEANUP_BEGIN();
	
Error:
	LogTest_Steps("70_60");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_70
*********************************************************************************/

FCE_HELP(MFL, 70_70)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_70)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_70)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_70)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);

	iFlag_Delete_Cache [STATION_70] = TRUE;
	Number_DTC[STATION_70] = 1;
	
		
	cnt = 0;
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);

		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_70] = FALSE;
	
		/* Reaquest Number_DTC */
		iFlag_Read_Number_DTC_Memory[STATION_70] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_70] = TRUE;
	
		Sleep(100);
		
		LIMIT_CHECK_EXT(INT32, Number_DTC[STATION_70], pexception);
		if(!pexc_first) pexc_first = pexception;

		if(pexc_first)
		{
		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_70] = FALSE;
	
		/* Reaquest Erasing_DTC_Memory_1 */
		iFlag_Erasing_DTC_Memory_1[STATION_70] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_70] = TRUE;
	
		Sleep(100);
		
		/* stop wake up frames */
		iFlag_Write_Wake_Up[STATION_70] = FALSE;
	
		/* Reaquest Number_DTC */
		iFlag_Erasing_DTC_Memory_2[STATION_70] = TRUE; //10ms 

		Sleep(22);
	
		/* run wake up frmaes */
		iFlag_Write_Wake_Up[STATION_70] = TRUE;
	
		Sleep(250);
		}
	
	}while(pexc_first && cnt<10);

	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_70]); 

   	EXCCHECK(pexc_first);
	
Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_70)
{
    TESTCLEANUP_BEGIN();
	
Error:    
	LogTest_Steps("70_70");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_80
*********************************************************************************/

FCE_HELP(MFL, 70_80)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Write BMW Part Number");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_80)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_80)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_80)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( BMWPartNumber, 256);

	PARAM_STRING_REQUIRED( "BMWPartNumber", BMWPartNumber);

	
	/* Write Variant code */
	sprintf (BMW_Part_Number_Write[STATION_70], "%s", BMWPartNumber);
	
   	iFlag_Write_BMW_Part_Number[STATION_70] = TRUE;
	Sleep (40);

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_80)
{
    TESTCLEANUP_BEGIN();
	
	
Error:
	LogTest_Steps("70_80");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_81
*********************************************************************************/

FCE_HELP(MFL, 70_81)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Write BMW SW Revision");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_81)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_81)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_81)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( BMW_SW_Revision, 256);

	PARAM_STRING_REQUIRED( "BMW_SW_Revision", BMW_SW_Revision);

	/* Write Variant code */
	sprintf (BMW_SW_Revision_Write[STATION_70], "%s", BMW_SW_Revision);
	
   	iFlag_Write_BMW_SW_Revision[STATION_70] = TRUE;
	Sleep (40);

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_81)
{
    TESTCLEANUP_BEGIN();
	
	
Error:  
	LogTest_Steps("70_81");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_82
*********************************************************************************/

FCE_HELP(MFL, 70_82)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Write BMW HW Revision");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_82)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_82)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_82)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( BMW_HW_Revision, 256);

	PARAM_STRING_REQUIRED( "BMW_HW_Revision", BMW_HW_Revision);

	
	/* Write Variant code */
	sprintf (BMW_HW_Revision_Write[STATION_70], "%s", BMW_HW_Revision);
	
   	iFlag_Write_BMW_HW_Revision[STATION_70] = TRUE;
	Sleep (40);

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_82)
{
    TESTCLEANUP_BEGIN();
	
	
Error:
	LogTest_Steps("70_82");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_83
*********************************************************************************/

FCE_HELP(MFL, 70_83)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check BMW Part Number");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_83)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_83)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_83)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( BMWPartNumber, 256);
	STRING( Respense_BMW_Part_Number, 7+1);

	char BMW_Part_Number_Write[MAX_STATIONS][7+1];

	PARAM_STRING_REQUIRED( "BMWPartNumber", BMWPartNumber);
	
	iFlag_Delete_Cache [STATION_70] = TRUE;
	
	memset(BMW_Part_Number_Read[STATION_70], 0X00,7+1); 


	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_70] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_BMW_Part_Number[STATION_70] = TRUE;

	Sleep(80);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_70] = TRUE;
	
	Sleep(150);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_70]); 
	
	CopyString (Respense_BMW_Part_Number, 0, BMW_Part_Number_Read[STATION_70], 1, -1);

	LIMIT_CREATE(BMWPartNumber, "[BMW Part Number]");
	LIMIT_SET(STRING, Respense_BMW_Part_Number);
	
	if (strcmp(BMWPartNumber, Respense_BMW_Part_Number))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_83)
{
    TESTCLEANUP_BEGIN();
	
	
Error:
	LogTest_Steps("70_83");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_84
*********************************************************************************/

FCE_HELP(MFL, 70_84)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check BMW SW Revision");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_84)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_84)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_84)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( BMW_sw_revision, 256);

	PARAM_STRING_REQUIRED( "BMW_SW_Revision", BMW_sw_revision);

	iFlag_Delete_Cache [STATION_70] = TRUE;
	
	memset(BMW_SW_Revision_Read[STATION_70], 0X00,7+1); 


	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_70] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_BMW_SW_Revision[STATION_70] = TRUE;

	Sleep(80);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_70] = TRUE;
	
	Sleep(150);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_70]); 
	
	LIMIT_CREATE(BMW_sw_revision, "[BMW SW Revivion]");
	LIMIT_SET(STRING, BMW_SW_Revision_Read[STATION_70]);
	
	if (strcmp(BMW_sw_revision, BMW_SW_Revision_Read[STATION_70]))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_84)
{
    TESTCLEANUP_BEGIN();
	
	
Error: 
	LogTest_Steps("70_84");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_85
*********************************************************************************/

FCE_HELP(MFL, 70_85)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check BMW SW Revision");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_85)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_85)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_85)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING( BMW_hw_revision, 256);

	PARAM_STRING_REQUIRED( "BMW_HW_Revision", BMW_hw_revision);

	iFlag_Delete_Cache [STATION_70] = TRUE;
	
	memset(BMW_SW_Revision_Read[STATION_70], 0X00,7+1); 


	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_70] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_BMW_HW_Revision[STATION_70] = TRUE;

	Sleep(80);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_70] = TRUE;
	
	Sleep(150);
	
	//SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_70]); 
	
	LIMIT_CREATE(BMW_hw_revision, "[BMW HW Revivion]");
	LIMIT_SET(STRING, BMW_HW_Revision_Read[STATION_70]);
	
	if (strcmp(BMW_hw_revision, BMW_HW_Revision_Read[STATION_70]))
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_85)
{
    TESTCLEANUP_BEGIN();
	
	
Error: 
	LogTest_Steps("70_85");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_86
*********************************************************************************/

FCE_HELP(MFL, 70_86)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : Write MAN DATA 2");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_86)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_86)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_86)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	STRING(part_number, 256);
	STRING(revision_index, 256);
	STRING(lineID, 256);
	int Year,Month,Day, iDays;

#if _WRITE_TRACA_MMS
	
	iFlag_Write_Valeo_production_Key[STATION_70] = TRUE;
	
	Sleep (60);
	
	PARAM_STRING_REQUIRED( "ProductPartNumber", part_number);
	PARAM_STRING_REQUIRED( "Revision_Index", revision_index);
	PARAM_STRING_REQUIRED( "LineID", lineID);

	/* VALEO Part Nember */
	sprintf (MAN_DATA_2_VALEO_Part_Number_Write[STATION_70], "%s", part_number);
	
	/* Revision_Index */
	sprintf (MAN_DATA_2_Revision_Index_Write[STATION_70], "%s", revision_index);
	
	/* LineID */
	sprintf (MAN_DATA_2_LineID_Write[STATION_70], "%s", lineID);
	
	/* Tracability FCT TEST OK */
	if (((STestParamPtr)pTID)->pexception == NULL)
	{
		if (((STestParamPtr)pTID)->Retested == FALSE)
			sprintf (MAN_DATA_2_Tracability_MMS_Write[STATION_70], "10");
		else if (((STestParamPtr)pTID)->Retested == TRUE)
			sprintf (MAN_DATA_2_Tracability_MMS_Write[STATION_70], "30");
	}		
	/* Tracability FCT TEST NOK */
	else if (((STestParamPtr)pTID)->pexception)
	{
		sprintf (MAN_DATA_2_Tracability_MMS_Write[STATION_70], "20");
	}
	
	/* production date*/
	sscanf (((STestParamPtr)pTID)->Date, "%04d.%02d.%02d ", &Year, &Month, &Day);
	sprintf (MAN_DATA_2_Production_Year_Write[STATION_70], "%02d", Year % 100);
	sprintf (MAN_DATA_2_Production_Month_Write[STATION_70], "%02d", Month);
	sprintf (MAN_DATA_2_Production_Day_Write[STATION_70], "%02d", Day);

	/* production_serial 8 digits */
	/* a jjj XXXX 
	   a : année -2017 = (ex 2019  a=2)
	   jjj : jour/année
	   XXXX :compteur */
	sscanf (((STestParamPtr)pTID)->Date, "%04d.%02d.%02d ", &Year, &Month, &Day);
	iDays = NumeroJour (Month, Day, Year);
  	sprintf (MAN_DATA_2_Serial_Write[STATION_70],"%d%03d%s", Year - 2017, iDays, ((STestParamPtr)pTID)->Production_Serial);
	
   	iFlag_Write_MAN_DATA_2[STATION_70] = TRUE;
	
	Sleep (100);
	
#endif 

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_86)
{
    TESTCLEANUP_BEGIN();
	
	
Error: 
	LogTest_Steps("70_86");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_87
*********************************************************************************/

FCE_HELP(MFL, 70_87)
{
    HELP_BEGIN();
	HELP_TEST( "LIN : check MAN DATA 2");

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_87)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_87)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_87)
{
	TEST_BEGIN();
	TEST_USE(CAN);
	TEST_USE(TESTER);
	char VALEO_part_number_Read[9+1];
	char Line_ID_Write[2+1];
	int retest, Num_Check_Data_2;
	int check_Man_Data_2 = OK;
	
#if _WRITE_TRACA_MMS
	
	Num_Check_Data_2 = 0;

	do{

	iFlag_Read_MAN_DATA_2_READ_OK[STATION_70] = FALSE;
	retest = 0;

	do{
		
	iFlag_Delete_Cache [STATION_70] = TRUE;
	
	/* stop wake up frmaes */
	iFlag_Write_Wake_Up[STATION_70] = FALSE;
	
	/* Reaquest Valeo_Sw_Revision */
   	iFlag_Read_MAN_DATA_2[STATION_70] = TRUE;

	Sleep(80);
	
	/* run wake up frmaes */
	iFlag_Write_Wake_Up[STATION_70] = TRUE;
	
	Sleep(150);
	
	retest++;
		
	}while (iFlag_Read_MAN_DATA_2_READ_OK[STATION_70] == FALSE  && retest < 3);
	
	SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Lin[STATION_70]); 
	  	
	check_Man_Data_2 = OK;
	
	if (strlen(MAN_DATA_2_VALEO_Part_Number_Write[STATION_70]) == 9) //E[10904801]
	{
		sprintf (VALEO_part_number_Read, "E%s\0", MAN_DATA_2_VALEO_Part_Number_Read[STATION_70]);
	}
	else if (strlen(MAN_DATA_2_VALEO_Part_Number_Write[STATION_70]) == 8) //[42055701]
	{
		sprintf (VALEO_part_number_Read, "%s\0", MAN_DATA_2_VALEO_Part_Number_Read[STATION_70]);
	}

	if (strcmp(VALEO_part_number_Read, MAN_DATA_2_VALEO_Part_Number_Write[STATION_70]))
	{
		LIMIT_CREATE(MAN_DATA_2_VALEO_Part_Number_Write[STATION_70], "[MD_2_VALEO_PN]");
		LIMIT_SET(STRING, VALEO_part_number_Read);
		check_Man_Data_2 = NOK;
	}
	if (strcmp(MAN_DATA_2_Revision_Index_Read[STATION_70], MAN_DATA_2_Revision_Index_Write[STATION_70]))
	{
		LIMIT_CREATE(MAN_DATA_2_Revision_Index_Write[STATION_70], "[MD_2_REV_IND]");
		LIMIT_SET(STRING, MAN_DATA_2_Revision_Index_Read[STATION_70]);
		check_Man_Data_2 = NOK;
	}
	sprintf (Line_ID_Write, "0%s\0", MAN_DATA_2_LineID_Write[STATION_70]);
	if (strcmp(MAN_DATA_2_LineID_Read[STATION_70], Line_ID_Write))
	{
		LIMIT_CREATE(Line_ID_Write, "[MD_2_LineID]");
		LIMIT_SET(STRING, MAN_DATA_2_LineID_Read[STATION_70]);
		check_Man_Data_2 = NOK;
	}
	if (strcmp(MAN_DATA_2_Production_Year_Read[STATION_70], MAN_DATA_2_Production_Year_Write[STATION_70]))
	{
		LIMIT_CREATE(MAN_DATA_2_Production_Year_Write[STATION_70], "[MD_2_Year]");
		LIMIT_SET(STRING, MAN_DATA_2_Production_Year_Read[STATION_70]);
		check_Man_Data_2 = NOK;
	}
	if (strcmp(MAN_DATA_2_Production_Month_Read[STATION_70], MAN_DATA_2_Production_Month_Write[STATION_70]))
	{
		LIMIT_CREATE(MAN_DATA_2_Production_Month_Write[STATION_70], "[MD_2_Month]");
		LIMIT_SET(STRING, MAN_DATA_2_Production_Month_Read[STATION_70]);
		check_Man_Data_2 = NOK;
	}
	if (strcmp(MAN_DATA_2_Production_Day_Read[STATION_70], MAN_DATA_2_Production_Day_Write[STATION_70]))
	{
		LIMIT_CREATE(MAN_DATA_2_Production_Day_Write[STATION_70], "[MD_2_Day]");
		LIMIT_SET(STRING, MAN_DATA_2_Production_Day_Read[STATION_70]);
		check_Man_Data_2 = NOK;
	}
	if (strcmp(MAN_DATA_2_Serial_Read[STATION_70], MAN_DATA_2_Serial_Write[STATION_70]))
	{
		LIMIT_CREATE(MAN_DATA_2_Serial_Write[STATION_70], "[MD_2_Serial]");
		LIMIT_SET(STRING, MAN_DATA_2_Serial_Read[STATION_70]);
		check_Man_Data_2 = NOK;
	}
	if (strcmp(MAN_DATA_2_Tracability_MMS_Read[STATION_70], MAN_DATA_2_Tracability_MMS_Write[STATION_70]))
	{
		LIMIT_CREATE(MAN_DATA_2_Tracability_MMS_Write[STATION_70], "[MD_2_Trac_MMS]");
		LIMIT_SET(STRING, MAN_DATA_2_Tracability_MMS_Read[STATION_70]);
		check_Man_Data_2 = NOK;
	}
	
	Num_Check_Data_2 ++;
		
	}while (check_Man_Data_2 == NOK && Num_Check_Data_2 < 5);
	
	if (check_Man_Data_2 == NOK)
	  	EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");

#endif   

Error:
    TEST_END_STATION(STATION_70);
}

FCE_CLEANUP(MFL, 70_87)
{
    TESTCLEANUP_BEGIN();
	
	
Error:  
	LogTest_Steps("70_87");
    TESTCLEANUP_END(STATION_70);
}

/*********************************************************************************
 * Function 70_90
 *********************************************************************************/
FCE_HELP(MFL, 70_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(MFL, 70_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(MFL, 70_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(MFL, 70_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
   	TEST_USE(CAMERA);
	SCameraPtr	pcamera = NULL;
	
	Inhibit_Rodage_Pushs[STATION_70] = FALSE;
	
	/* wait for table finish rotation */
	DISPLAY_TESTSEQ_RESULT(STATION_70);
	//pcameras->GetCameraByIndex(pcameras, 0, &pcamera);
	//pcamera->Cognex_Trigger(pcamera);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_80);

Error:
    TEST_END();
}

FCE_CLEANUP(MFL, 70_90)
{
    TESTCLEANUP_BEGIN();
    
Error: 
	LogTest_Steps("70_90");
    TESTCLEANUP_END(STATION_70);
}
