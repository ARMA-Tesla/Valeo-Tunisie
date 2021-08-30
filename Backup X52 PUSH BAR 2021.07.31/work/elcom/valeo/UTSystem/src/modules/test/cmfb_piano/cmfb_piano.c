#include <analysis.h>
#include <ansi_c.h>
#include "cmfb_piano.h"
#include "cmfb_piano_utils.h"
#include <test\test_helper_functions.h>
#include "toolbox.h"
#include <formatio.h>
#include <testseq.h> 
#include <tcpsupp.h>
#include <export.h>
#include <math.h>

const char* HISTORY_INFO = \
"[24-04-2017] ARAIBIA MOHAMED: version 1.00\
 * Création."; 


DEFINE_GLOBAL_VARIABLE();

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2

#define AXIS_STATION_40  0
#define AXIS_STATION_50  3

#define JOB_MODE_7_PUSH  0
#define JOB_MODE_9_PUSH  1



#define STEP_REF  					0
#define STEP_PICTO_DAY  			1
#define STEP_PICTO_NIGHT  			2
#define STEP_PICTO_LED  			3
#define STEP_BLINKING_WARNING  		4
#define STEP_CUSTOMER_INTERFACE  	5

#define TIMEOUT_CAMERA		   	  4000
#define RETEST_CAMERA			  5//5


#define RETEST		5 //5 


double Mean_GAP_50[4+1];
double Mean_GAP_60[4+1];
	
static SCalibrationPtr	pScaleInfoForce_Station_40;
static SCalibrationPtr	pScaleInfo_Station_40;
static SCalibrationPtr	pScaleInfo1_Station_40;
static SCalibrationPtr	pScaleInfo2_Station_40;
static SCalibrationPtr	pScaleInfo3_Station_40;
static SCalibrationPtr	pScaleInfo4_Station_40;
static SCalibrationPtr	pScaleInfo5_Station_40;
static SCalibrationPtr	pScaleInfo6_Station_40;
static SCalibrationPtr	pScaleInfo7_Station_40;
static SCalibrationPtr	pScaleInfo8_Station_40;
	
static SCalibrationPtr	pScaleInfoForce_Station_50;
static SCalibrationPtr	pScaleInfo_Station_50;
static SCalibrationPtr	pScaleInfo1_Station_50;
static SCalibrationPtr	pScaleInfo2_Station_50;
static SCalibrationPtr	pScaleInfo3_Station_50;
static SCalibrationPtr	pScaleInfo4_Station_50;
static SCalibrationPtr	pScaleInfo5_Station_50;
static SCalibrationPtr	pScaleInfo6_Station_50;
static SCalibrationPtr	pScaleInfo7_Station_50;
static SCalibrationPtr	pScaleInfo8_Station_50;

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

#define TEST_VOLTAGE			13.55
#define TEST_CURRENT			0.6
#define MIN_VOLTAGE				13.25
#define MAX_VOLTAGE				13.75

#define CONFIG_FILE				"\\test_power_source.xml"

#define CONNECT_RELAIS_LED\
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line2", 1)); \
	} while(0)

#define DISCONNECT_RELAIS_LED \
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line2", 0)); \
	} while(0)

#define CONNECT_RELAIS_CURRENT_LED\
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port2/Line6", 1)); \
	} while(0)

#define DISCONNECT_RELAIS_CURRENT_LED \
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port2/Line6", 0)); \
	} while(0)

#define CONNECT_ALL_LED \
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line4", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line5", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line6", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line7", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line26", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line17", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line18", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line19", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line20", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line21", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line22", 1)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line23", 1)); \
	} while(0)

#define DISCONNECT_ALL_LED \
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line4", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line5", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line6", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line7", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line26", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line17", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line18", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line19", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line20", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line21", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line22", 0)); \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port0/Line23", 0)); \
	} while(0)
	
#define CONNECT_PICTO \
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port2/Line7", 1)); \
	} while(0)

#define DISCONNECT_PICTO \
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port2/Line7", 0)); \
	} while(0)

#define CONNECT_RELAIS_CURRENT_PICTO\
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line1", 1)); \
	} while(0)

#define DISCONNECT_RELAIS_CURRENT_PICTO \
	do { \
		checkErr( eldaq_WriteDigitalLine( vi_DAQ, "port1/Line1", 0)); \
	} while(0)

STestSeq gs_TestSeq = ADD_TSI( CMFB);
STestListItem gs_TestList[] = {
	
	
	ADD_TI( "20.00", CMFB, 20_00),		/* punzoni down */
	ADD_TI( "20.10", CMFB, 20_10),		/* punzoni up */
	ADD_TI( "20.50", CMFB, 20_50),		/* contrasti Down*/
	ADD_TI( "20.60", CMFB, 20_60),		/* contrasti Up */
	ADD_TI( "20.40", CMFB, 20_40),		/* check potentiometer X52_7 */
	ADD_TI( "20.400", CMFB, 20_400),	/* check potentiometer VS11 */
	ADD_TI( "20.409", CMFB, 20_409),	/* check potentiometer VX52_9 */
	ADD_TI( "20.20", CMFB, 20_20),		/* Check Clipping */
	ADD_TI( "20.30", CMFB, 20_30),		/* STOP Check Clipping */  
	ADD_TI( "20.90", CMFB, 20_90),		/* Table Rotation station 20 to  station 30 */
	
	ADD_TI( "30.00", CMFB, 30_00),		/* Select Rodage X52*/
	ADD_TI( "30.01", CMFB, 30_01),		/* Select Rodage VS11*/
	ADD_TI( "30.10", CMFB, 30_10),		/* Rodage */
   	ADD_TI( "30.20", CMFB, 30_20),		/* Stop Rodage */
	ADD_TI( "30.90", CMFB, 30_90),		/* Table Rotation station 30 to  station 40 */
	
	
	/*********************** ST40 fonctions X52 **********************************/ 
	
	ADD_TI( "40.02", CMFB, 40_02),	/* Select X52 */
	ADD_TI( "40.03", CMFB, 40_03),	/* Check Select X52 */
	ADD_TI( "40.14", CMFB, 40_14),	/* plug X52*/
 	ADD_TI( "40.15", CMFB, 40_15),	/* Check plug X52*/
	ADD_TI( "40.16", CMFB, 40_16),	/* Unplug X52*/
	ADD_TI( "40.17", CMFB, 40_17),	/* Check Unplug X52*/
	ADD_TI( "40.09", CMFB, 40_09),	/* Get Scale Infos X52*/
	
	ADD_TI( "40.109", CMFB, 40_109),	  /* position Push Nr7 X52_9 */
	ADD_TI( "40.119", CMFB, 40_709),	  /* test Force/Contact Push Nr7 X52_9*/
	
	ADD_TI( "40.20", CMFB, 40_20),	      /* position Push Nr6 X52_7*/
	ADD_TI( "40.209", CMFB, 40_209),	  /* position Push Nr6 X52_9*/
	ADD_TI( "40.21", CMFB, 40_70),	      /* test Force/Contact Push Nr6 X52_7*/
	ADD_TI( "40.219", CMFB, 40_709),	  /* test Force/Contact Push Nr6 X52_9*/
	
   	ADD_TI( "40.30", CMFB, 40_30),	       /* position Push Nr5 X52_7*/
	ADD_TI( "40.309", CMFB, 40_309),	   /* position Push Nr5 X52_9 */
	ADD_TI( "40.31", CMFB, 40_70),	       /* test Force/Contact Push Nr5 X52_7*/
	ADD_TI( "40.319", CMFB, 40_709),	   /* test Force/Contact Push Nr5 X52_9*/
   	
	
	
	/*********************** ST40 fonctions VS11 **********************************/
	
	ADD_TI( "40.00", CMFB, 40_00),	/* Select VS11 */
	ADD_TI( "40.01", CMFB, 40_01),	/* Check Select VS11 */
	ADD_TI( "40.04", CMFB, 40_04),	/* plug VS11*/
 	ADD_TI( "40.05", CMFB, 40_05),	/* Check plug VS11*/
	ADD_TI( "40.06", CMFB, 40_06),	/* Unplug VS11*/
	ADD_TI( "40.07", CMFB, 40_07),	/* Check Unplug VS11*/
	ADD_TI( "40.19", CMFB, 40_19),	/* Get Scale Infos VS11*/
	ADD_TI( "40.100", CMFB, 40_100),	/* position Push Nr8 */
	ADD_TI( "40.110", CMFB, 40_700),	/* test Force/Contact Push Nr8 VS11*/
	ADD_TI( "40.200", CMFB, 40_200),	/* position Push Nr7 */
	ADD_TI( "40.210", CMFB, 40_700),	/* test Force/Contact Push Nr7 VS11*/
   	ADD_TI( "40.300", CMFB, 40_300),	/* position Push Nr6 */
	ADD_TI( "40.310", CMFB, 40_700),	/* test Force/Contact Push Nr6 VS11*/
   	ADD_TI( "40.400", CMFB, 40_400),	/* position Push Nr5 */
	ADD_TI( "40.410", CMFB, 40_700),	/* test Force/Contact Push Nr5 VS11*/

	
	/*********************** ST40 fonctions Commun **********************************/
	
	ADD_TI( "40.08", CMFB, 40_08),	/* axis Z to initial position */
	ADD_TI( "40.90", CMFB, 40_90),	/* Table Rotation station 40 to  station 50 */ 
	
	/*********************** ST50 fonctions X52 **********************************/
	
	
	ADD_TI( "50.02", CMFB, 50_02),	/* Select X52 */
	ADD_TI( "50.03", CMFB, 50_03),	/* Check Select X52 */
 	
	ADD_TI( "50.14", CMFB, 50_14),	/* plug X52*/
 	ADD_TI( "50.15", CMFB, 50_15),	/* Check plug X52*/
	ADD_TI( "50.16", CMFB, 50_16),	/* Unplug X52*/
	ADD_TI( "50.17", CMFB, 50_17),	/* Check Unplug X52*/
	ADD_TI( "50.09", CMFB, 50_09),	/* Get Scale Infos X52*/
	
	ADD_TI( "50.109", CMFB, 50_109),    /* position Push Nr1 X52_9 */
	ADD_TI( "50.119", CMFB, 50_709),	/* test Force/Contact Push Nr1 X52_9 */
	
	ADD_TI( "50.20", CMFB, 50_20),	    /* position Push Nr2 X52_7*/
	ADD_TI( "50.209", CMFB, 50_209),    /* position Push Nr2 X52_9 */
	ADD_TI( "50.21", CMFB, 50_70),	    /* test Force/Contact Push Nr2 X52_7 */
	ADD_TI( "50.219", CMFB, 50_709),	/* test Force/Contact Push Nr2 X52_9 */
	
   	ADD_TI( "50.30", CMFB, 50_30),	    /* position Push Nr3 X52_7*/
	ADD_TI( "50.309", CMFB, 50_309),	/* position Push Nr3 X52_9 */
	ADD_TI( "50.31", CMFB, 50_70),	    /* test Force/Contact Push Nr3 X52_7 */
	ADD_TI( "50.319", CMFB, 50_709),	/* test Force/Contact Push Nr3 X52_9 */
   	
	
	
	/*********************** ST50 fonctions VS11 **********************************/ 
	
	ADD_TI( "50.00", CMFB, 50_00),	/* Select VS11 */
	ADD_TI( "50.01", CMFB, 50_01),	/* Check Select VS11 */
	ADD_TI( "50.04", CMFB, 50_04),	/* plug VS11*/
 	ADD_TI( "50.05", CMFB, 50_05),	/* Check plug VS11*/
	ADD_TI( "50.06", CMFB, 50_06),	/* Unplug VS11*/
	ADD_TI( "50.07", CMFB, 50_07),	/* Check Unplug VS11*/
	ADD_TI( "50.19", CMFB, 50_19),	/* Get Scale Infos VS11*/
	ADD_TI( "50.100", CMFB, 50_100),	/* position Push Nr8 VS11*/
	ADD_TI( "50.110", CMFB, 50_700),	/* test Force/Contact Push Nr4 VS11*/
	ADD_TI( "50.200", CMFB, 50_200),	/* position Push Nr4 VS11*/
	ADD_TI( "50.210", CMFB, 50_700),	/* test Force/Contact Push Nr3 VS11*/
   	ADD_TI( "50.300", CMFB, 50_300),	/* position Push Nr3 VS11*/
	ADD_TI( "50.310", CMFB, 50_700),	/* test Force/Contact Push Nr2 VS11*/
   	ADD_TI( "50.400", CMFB, 50_400),	/* position Push Nr1 VS11*/
	ADD_TI( "50.410", CMFB, 50_700),	/* test Force/Contact Push Nr1 VS11*/
	
    /*********************** ST50 fonctions Commun **********************************/	
	
	ADD_TI( "50.08", CMFB, 50_08),	/* axis Z to initial position */
	ADD_TI( "50.90", CMFB, 50_90),	/* Table Rotation station 50 to  station 60 */ 
	
    /*********************** ST60 fonctions X52 **********************************/ 
	
	ADD_TI( "60.02", CMFB, 60_02),	/* Select X52 */
	ADD_TI( "60.03", CMFB, 60_03),	/* Check Select X52 */
	ADD_TI( "60.14", CMFB, 60_14),	/* plug X52 */
	ADD_TI( "60.15", CMFB, 60_15),	/* Unplug X52*/
	
	/********* ST60 fonctions X52_7 *********/ 
	ADD_TI( "60.08", CMFB, 60_08),	/* Check Job File */ 
	ADD_TI( "60.10", CMFB, 60_10),	/* test day picto*/
	ADD_TI( "60.30", CMFB, 60_30),	/* test consommation picto */
	ADD_TI( "60.31", CMFB, 60_31),	/* test night picto */
	ADD_TI( "60.40", CMFB, 60_40),	/* test consommation LED */
	ADD_TI( "60.41", CMFB, 60_41),	/* test night LED  */
	ADD_TI( "60.50", CMFB, 60_50),	/* test interface clients */
	
	/********* ST60 fonctions X52_9 *********/ 
	ADD_TI( "60.800", CMFB, 60_800),	/* Check Job File */ 
	ADD_TI( "60.109", CMFB, 60_109),	/* test day picto*/
	ADD_TI( "60.309", CMFB, 60_30),	/* test consommation picto */
	ADD_TI( "60.319", CMFB, 60_319),	/* test night picto */
	ADD_TI( "60.409", CMFB, 60_40),	/* test consommation LED */
	ADD_TI( "60.419", CMFB, 60_419),	/* test night LED  */
	ADD_TI( "60.509", CMFB, 60_509),	/* test interface clients */
	
/*********************** ST60 fonctions VS11 **********************************/ 
	
	ADD_TI( "60.00", CMFB, 60_00),	/* Select VS11 */
	ADD_TI( "60.01", CMFB, 60_01),	/* Check Select VS11 */
 	ADD_TI( "60.04", CMFB, 60_04),	/* plug VS11 */
	ADD_TI( "60.05", CMFB, 60_05),	/* Unplug VS11*/
	ADD_TI( "60.080", CMFB, 60_080),	/* Check Job File */ 
	ADD_TI( "60.100", CMFB, 60_100),	/* test day picto */
	ADD_TI( "60.310", CMFB, 60_310),	/* test night picto */
	ADD_TI( "60.320", CMFB, 60_320),	/* test Blinking Warning  */	
	ADD_TI( "60.410", CMFB, 60_410),	/* test night LED  */
	ADD_TI( "60.500", CMFB, 60_500),	/* test interface clients */
	

	/*********************** ST60 fonctions Commun **********************************/ 
	ADD_TI( "60.06", CMFB, 60_06),	/* illumination ON */
	ADD_TI( "60.07", CMFB, 60_07),	/* illumination OFF */
	  
	ADD_TI( "60.09", CMFB, 60_09),	/* Reperage Produit */
	ADD_TI( "60.90", CMFB, 60_90),	/* Table Rotation station 60 to station 70 */
	
	
	
	ADD_TI( "70.90", CMFB, 70_90),	/* Table Rotation station 70 to station 80 */
	ADD_TI_LAST_ENTRY
};

typedef struct _SCMFBConfig {
	double			KL30;
	double			TypicalCurrent;
	double			BlowerCurrent;
} SCMFBConfig, *SCMFBConfigPtr;

static SCMFBConfig	gs_CMFB = {TEST_VOLTAGE, TEST_CURRENT}; 



/*********************************************************************************
 * Function Testsequence Init
 *********************************************************************************/
FCE_TESTSEQUENCE_INIT(CMFB)
{
	TESTSEQUENCEINIT_BEGIN();
	TEST_USE(DAQ);
	TEST_USE(POWER);
	
	PANEL_INI(STATION_20);

	//INITIALIZE(DAQ);
	INITIALIZE(POWER);
	
	//POWER_SET("CHANNEL1", TEST_VOLTAGE, TEST_CURRENT);
	//checkErr( eldaq_ConfigureMeasurement (vi_DAQ, "", 1000000, 0.05));
	//checkErr( eldaq_StartMeasurement( vi_DAQ));


	
Error:
	TESTSEQUENCEINIT_END();
}

/*********************************************************************************
 * Function Testsequence Cleanup
 *********************************************************************************/
FCE_TESTSEQUENCE_CLEANUP(CMFB)
{
	TESTSEQUENCECLEANUP_BEGIN(); 
	TEST_USE(DAQ);

	PANEL_CLEAN();
	//INITIALIZE(DAQ);

	//checkErr(eldaq_StopMeasurement(vi_DAQ));

		
Error:
	TESTSEQUENCECLEANUP_END();
}

/*********************************************************************************
 * Function 20_00
 *********************************************************************************/
FCE_HELP(CMFB, 20_00)
{
    HELP_BEGIN();
    HELP_TEST( "Cylindre des potentiometres en bas" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_00)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);

	TESTER_PUNZONI_DOWN(TIMEOUT_DEFAULT);
	ElSleep(1000);

Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_00)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 20_10
 *********************************************************************************/
FCE_HELP(CMFB, 20_10)
{
    HELP_BEGIN();
    HELP_TEST( "Clipping, Panel Stop Clpping" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_10)
{
    TESTINIT_BEGIN();

Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_10)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PUNZONI_UP(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_10)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 20_50
 *********************************************************************************/
FCE_HELP(CMFB, 20_50)
{
    HELP_BEGIN();
    HELP_TEST( "Cylindre des potentiometres en bas" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_50)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_50)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_50)
{
    TEST_BEGIN();
	TEST_USE(TESTER);

	TESTER_CONTRASTI_DOWN(TIMEOUT_DEFAULT);
	ElSleep(1000);

Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_50)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 20_60
 *********************************************************************************/
FCE_HELP(CMFB, 20_60)
{
    HELP_BEGIN();
    HELP_TEST( "Clipping, Panel Stop Clpping" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_60)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_60)
{
    TESTINIT_BEGIN();

Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_60)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_CONTRASTI_UP(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_60)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_20);
}


/*********************************************************************************
 * Function 20_20
 *********************************************************************************/
FCE_HELP(CMFB, 20_20)
{
    HELP_BEGIN();
    HELP_TEST( "Clipping, potentiometer down" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_20)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_20)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);

	TESTER_CHECK_CLIPPING(TIMEOUT_DEFAULT);
	ElSleep(2000);

Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_20)
{
    TESTCLEANUP_BEGIN();
															 
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 20_30
 *********************************************************************************/
FCE_HELP(CMFB, 20_30)
{
    HELP_BEGIN();
    HELP_TEST( "Clipping, Panel Stop check Clpping" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_30)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_30)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_30)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_STOP_CHECK_CLIPPING(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_30)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 20_40 X52_7
 *********************************************************************************/
FCE_HELP(CMFB, 20_40)
{
    HELP_BEGIN();
    HELP_TEST( "Clipping, Check Clipping" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_40)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_40)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_40)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(DAQ2);
	VARIABLE(REAL64, rvalue_PTZ14);
	VARIABLE(REAL64, rvalue_PTZ13);
	VARIABLE(REAL64, rvalue_PTZ12);		
	VARIABLE(REAL64, rvalue_PTZ11);	
	VARIABLE(REAL64, rvalue_PTZ10);
	VARIABLE(REAL64, rvalue_PTZ9);	
	VARIABLE(REAL64, rvalue_PTZ8);	
	VARIABLE(REAL64, rvalue_PTZ7);	
	VARIABLE(REAL64, rvalue_PTZ6);	
	VARIABLE(REAL64, rvalue_PTZ5);	
	VARIABLE(REAL64, rvalue_PTZ4);	
	VARIABLE(REAL64, rvalue_PTZ3);	
	VARIABLE(REAL64, rvalue_PTZ2);	
	VARIABLE(REAL64, rvalue_PTZ1);			
	VARIABLE(REAL64, level);
	SCalibrationPtr	pScaleInfo = NULL;


	SIviBridgePtr  pIvi = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
	EXCCHECK( pIvi->GetSession( pIvi, "DAQ2", &vi_DAQ2)); 
	//INITIALIZE(DAQ);

	/****************** ReadAnalogWithScale *****************/
	
	/* PTZ 14 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai0", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai0", pScaleInfo, &rvalue_PTZ14));
	
	/* PTZ 13 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai1", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai1", pScaleInfo, &rvalue_PTZ13));
	
	/* PTZ 12 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai2", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai2", pScaleInfo, &rvalue_PTZ12));	
	
	/* PTZ 11 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai3", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai3", pScaleInfo, &rvalue_PTZ11));
	
	/* PTZ 10 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai4", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai4", pScaleInfo, &rvalue_PTZ10));	
	
	/* PTZ 09 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai5", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai5", pScaleInfo, &rvalue_PTZ9));
	
	/* PTZ 08 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai6", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai6", pScaleInfo, &rvalue_PTZ8));		
	
	/* PTZ 07 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai7", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai7", pScaleInfo, &rvalue_PTZ7));
	
	/* PTZ 06 */	
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai8", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai8", pScaleInfo, &rvalue_PTZ6));

	/* PTZ 05 */	
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai9", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai9", pScaleInfo, &rvalue_PTZ5));
	
	/* PTZ 04 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai10", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai10", pScaleInfo, &rvalue_PTZ4));		
	
	/* PTZ 03 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai11", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai11", pScaleInfo, &rvalue_PTZ3));
	
	/* PTZ 02 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai12", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai12", pScaleInfo, &rvalue_PTZ2));

		
	/* PTZ 01 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai13", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai13", pScaleInfo, &rvalue_PTZ1));

	/****************** Check Potentiometre Value *****************/	
	
	/* PTZ 13 */
	level = (double) rvalue_PTZ13 - 1.85;
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 11 */	
	level = (double) rvalue_PTZ11 - 1.08;
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 09 */	
	level = (double)(rvalue_PTZ9 - 1.66);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 06 */	
	level = (double)(rvalue_PTZ6 - 2.84);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 05 */
	level = (double)(rvalue_PTZ5 - 4.79);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 02 */	
	level = (double)(rvalue_PTZ2 - 3.34);  
	LIMIT_CHECK(REAL64, level);
	
	/****************** Check Potentiometre difference Value *****************/		
	
	/* Clip(PTZ_13) - 	Clip(PTZ_06) */ //OK
	level = (double)(fabs(rvalue_PTZ13) - fabs(rvalue_PTZ6)); 
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* PTZ_12 		- 	Clip(PTZ_05) */ //ok
	level = (double)(fabs(rvalue_PTZ12) - fabs(rvalue_PTZ5)) + 2,012761222;
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* Clip(PTZ_11) - 	PTZ_04 */       //ok
	level = (double)(fabs(rvalue_PTZ11) - fabs(rvalue_PTZ4)) + 2,726185254;
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* Clip(PTZ_09) - 	Clip(PTZ_02) */ //OK
	level = (double)(fabs(rvalue_PTZ9) - fabs(rvalue_PTZ2)) + 0,394563027; 	
	LIMIT_CHECK(REAL64, fabs(level));
	
	
Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_40)
{
    TESTCLEANUP_BEGIN();
															 
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 20_400 VS11
 *********************************************************************************/
FCE_HELP(CMFB, 20_400)
{
    HELP_BEGIN();
    HELP_TEST( "Clipping, Check Clpping" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_400)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_400)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_400)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(DAQ2);
	VARIABLE(REAL64, rvalue_PTZ14);
	VARIABLE(REAL64, rvalue_PTZ13);
	VARIABLE(REAL64, rvalue_PTZ12);		
	VARIABLE(REAL64, rvalue_PTZ11);	
	VARIABLE(REAL64, rvalue_PTZ10);
	VARIABLE(REAL64, rvalue_PTZ9);	
	VARIABLE(REAL64, rvalue_PTZ8);	
	VARIABLE(REAL64, rvalue_PTZ7);	
	VARIABLE(REAL64, rvalue_PTZ6);	
	VARIABLE(REAL64, rvalue_PTZ5);	
	VARIABLE(REAL64, rvalue_PTZ4);	
	VARIABLE(REAL64, rvalue_PTZ3);	
	VARIABLE(REAL64, rvalue_PTZ2);	
	VARIABLE(REAL64, rvalue_PTZ1);			
	VARIABLE(REAL64, level);
	VARIABLE(REAL64, rvalue);
	SCalibrationPtr	pScaleInfo = NULL;

	SIviBridgePtr  pIvi = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
	EXCCHECK( pIvi->GetSession( pIvi, "DAQ2", &vi_DAQ2)); 
	
	/* PTZ 13 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai1", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai1", pScaleInfo, &rvalue_PTZ13));
	level = (float)(rvalue_PTZ13 - 1.44);
	LIMIT_CHECK(REAL64, level);

	/* PTZ 11 */	
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai3", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai3", pScaleInfo, &rvalue_PTZ11));
	level = (float)(rvalue_PTZ11 - 0.84);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 09 */	
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai5", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai5", pScaleInfo, &rvalue_PTZ9));
	level = (float)(rvalue_PTZ9 - 1.34);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 06 */	
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai8", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai8", pScaleInfo, &rvalue_PTZ6));
	level = (float)(rvalue_PTZ6 - 1.97); 
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 05 */	
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai9", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai9", pScaleInfo, &rvalue_PTZ5));
	level = (float)(rvalue_PTZ5 - 3.49);  
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 03 */	
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai11", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai11", pScaleInfo, &rvalue_PTZ3));
	level = (float)(rvalue_PTZ3 - 2.10);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 02 */	
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai12", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai12", pScaleInfo, &rvalue_PTZ2));
	level = (float)(rvalue_PTZ2 - 2.38);   
	LIMIT_CHECK(REAL64, level);

	/****************** Check Potentiometre difference Value *****************/		
	
	/* Clip(PTZ_13) - 	Clip(PTZ_06) */ //OK
	level = (double)(fabs(rvalue_PTZ13) - fabs(rvalue_PTZ6)); 
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* PTZ_12 	- 	Clip(PTZ_05) */ //ok
	level = (double)(fabs(rvalue_PTZ12) - fabs(rvalue_PTZ5));
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* Clip(PTZ_11) - 	PTZ_04 */       //ok
	level = (double)(fabs(rvalue_PTZ11) - fabs(rvalue_PTZ4));
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* PTZ_10	 - 	Clip(PTZ_03) */       //ok
	level = (double)(fabs(rvalue_PTZ10) - fabs(rvalue_PTZ3));
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* Clip(PTZ_09) - 	Clip(PTZ_02) */ //OK
	level = (double)(fabs(rvalue_PTZ9) - fabs(rvalue_PTZ2)); 	
	LIMIT_CHECK(REAL64, fabs(level));

Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_400)
{
    TESTCLEANUP_BEGIN();
															 
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 20_409 X52_9
 *********************************************************************************/
FCE_HELP(CMFB, 20_409)
{
    HELP_BEGIN();
    HELP_TEST( "Clipping, Check Clpping" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_409)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_409)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_409)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(DAQ2);
	VARIABLE(REAL64, rvalue_PTZ14);
	VARIABLE(REAL64, rvalue_PTZ13);
	VARIABLE(REAL64, rvalue_PTZ12);		
	VARIABLE(REAL64, rvalue_PTZ11);	
	VARIABLE(REAL64, rvalue_PTZ10);
	VARIABLE(REAL64, rvalue_PTZ9);	
	VARIABLE(REAL64, rvalue_PTZ8);	
	VARIABLE(REAL64, rvalue_PTZ7);	
	VARIABLE(REAL64, rvalue_PTZ6);	
	VARIABLE(REAL64, rvalue_PTZ5);	
	VARIABLE(REAL64, rvalue_PTZ4);	
	VARIABLE(REAL64, rvalue_PTZ3);	
	VARIABLE(REAL64, rvalue_PTZ2);	
	VARIABLE(REAL64, rvalue_PTZ1);			
	VARIABLE(REAL64, level);
	VARIABLE(REAL64, rvalue);
	SCalibrationPtr	pScaleInfo = NULL;

	SIviBridgePtr  pIvi = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
	EXCCHECK( pIvi->GetSession( pIvi, "DAQ2", &vi_DAQ2)); 
	//INITIALIZE(DAQ);
	
	/* PTZ 14 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai0", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai0", pScaleInfo, &rvalue_PTZ14));
	level = (float)(rvalue_PTZ14 - 2.31);
	LIMIT_CHECK(REAL64, level);

	/* PTZ 12 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai2", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai2", pScaleInfo, &rvalue_PTZ12));
	level = (float)(rvalue_PTZ12 - 1.41);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 10 */ 
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai4", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai4", pScaleInfo, &rvalue_PTZ10));
	level = (float)(rvalue_PTZ10 - 1.12);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 8 */ 
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai6", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai6", pScaleInfo, &rvalue_PTZ8));
	level = (float)(rvalue_PTZ8 - 1.55);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 7 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai7", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai7", pScaleInfo, &rvalue_PTZ7));
	level = (float)(rvalue_PTZ7 - 3.55);
	LIMIT_CHECK(REAL64, level);
	
	/* PTZ 5 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai9", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai9", pScaleInfo, &rvalue_PTZ5));
	level = (float)(rvalue_PTZ5 - 4.96);  
	LIMIT_CHECK(REAL64, level);

	/* PTZ 3 */ 
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai11", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai11", pScaleInfo, &rvalue_PTZ3));
	level = (float)(rvalue_PTZ3 - 3.62);
	LIMIT_CHECK(REAL64, level);

	/* PTZ 1 */
	checkErr( eldaq_GetChannelScale ( vi_DAQ2, "ai13", "distance", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale2("","ai13", pScaleInfo, &rvalue_PTZ1));
	level = (float)(rvalue_PTZ1 - 4.31);
	LIMIT_CHECK(REAL64, level);
	
	
	/****************** Check Potentiometre difference Value *****************/		
	
	/* Clip(PTZ_08) - 	Clip(PTZ_01) */ //OK
	level = (double)(fabs(rvalue_PTZ8) - fabs(rvalue_PTZ1)); 
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* Clip(PTZ_10)	- 	Clip(PTZ_03) */ //ok
	level = (double)(fabs(rvalue_PTZ10) - fabs(rvalue_PTZ3));
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* Clip(PTZ_12) - 	Clip(PTZ_05) */       //ok
	level = (double)(fabs(rvalue_PTZ12) - fabs(rvalue_PTZ5));
	LIMIT_CHECK(REAL64, fabs(level));
	
	/* Clip(PTZ_14) - 	Clip(PTZ_07) */ //OK
	level = (double)(fabs(rvalue_PTZ14) - fabs(rvalue_PTZ7)); 	
	LIMIT_CHECK(REAL64, fabs(level));
	
Error:
    TEST_END_STATION(STATION_20);
}

FCE_CLEANUP(CMFB, 20_409)
{
    TESTCLEANUP_BEGIN();
															 
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 20_90
 *********************************************************************************/
FCE_HELP(CMFB, 20_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 20_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 20_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_20);
}

FCE_TEST(CMFB, 20_90)
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

FCE_CLEANUP(CMFB, 20_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_20);
}

/*********************************************************************************
 * Function 30_00
*********************************************************************************/

FCE_HELP(CMFB, 30_00)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, All pushs down" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 30_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 30_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(CMFB, 30_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_RODAGE_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(CMFB, 30_00)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 30_01
*********************************************************************************/

FCE_HELP(CMFB, 30_01)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, All pushs down" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 30_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 30_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(CMFB, 30_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_RODAGE_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(CMFB, 30_01)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}
/*********************************************************************************
 * Function 30_10
*********************************************************************************/

FCE_HELP(CMFB, 30_10)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, All pushs down" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 30_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 30_10)									
{
    TESTINIT_BEGIN();


Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(CMFB, 30_10)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	STRING( version, 8+1);
	VARIABLE(UINT32, TimeOn);
	VARIABLE(UINT32, TimeOff);
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

	
	PARAM_UINT32_OPTIONAL("TON", &TimeOn, 300);
	PARAM_UINT32_OPTIONAL("TOFF", &TimeOff, 300);
	TimeOn = 350;
	TimeOff = 350; 
	PARAM_UINT32_OPTIONAL("count", &count, 10);
	PARAM_UINT32_OPTIONAL("Enable_Push_1", &Enable_Push_1, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_2", &Enable_Push_2, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_3", &Enable_Push_3, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_4", &Enable_Push_4, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_5", &Enable_Push_5, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_6", &Enable_Push_6, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_7", &Enable_Push_7, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_8", &Enable_Push_8, 1);
	PARAM_UINT32_OPTIONAL("Enable_Push_9", &Enable_Push_9, 1);     
	
	version[0] = Enable_Push_1;	
	version[1] = Enable_Push_2;	
	version[2] = Enable_Push_3;	
	version[3] = Enable_Push_4;	
	version[4] = Enable_Push_5;	
	version[5] = Enable_Push_6;	
	version[6] = Enable_Push_7;	
	version[7] = Enable_Push_8;	
	version[8] = Enable_Push_9;	 
	
	TESTER_RODAGE(TIMEOUT_DEFAULT,count,TimeOn,TimeOff,version);

Error:
    TEST_END_STATION(STATION_30);
}

FCE_CLEANUP(CMFB, 30_10)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

///*********************************************************************************
// * Function 30_20
//*********************************************************************************/
//
FCE_HELP(CMFB, 30_20)
{
    HELP_BEGIN();
    HELP_TEST( "Tester, All pushs up" );

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 30_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
//	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 30_20)									
{
    TESTINIT_BEGIN();
//	
Error:    
    TESTINIT_END(STATION_30);
}
//
FCE_TEST(CMFB, 30_20)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
//	TESTER_STOP_RODAGE(TIMEOUT_DEFAULT);
//
Error:
    TEST_END_STATION(STATION_30);
}
//
FCE_CLEANUP(CMFB, 30_20)
{
    TESTCLEANUP_BEGIN();
//    
Error:    
    TESTCLEANUP_END(STATION_30);
}
/*********************************************************************************
 * Function 30_90
 *********************************************************************************/
FCE_HELP(CMFB, 30_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 30_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 30_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_30);
}

FCE_TEST(CMFB, 30_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	/* wait for table finish rotation */
	DISPLAY_TESTSEQ_RESULT(STATION_30);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_40);

Error:
    TEST_END();
}

FCE_CLEANUP(CMFB, 30_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_30);
}

/*********************************************************************************
 * Function 40_00
*********************************************************************************/

FCE_HELP(CMFB, 40_00)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_00)									
{
    TESTINIT_BEGIN();

Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);

//	Inhibit_Cylinder_Lock_Up[STATION_40] = TRUE;
	TESTER_SELECT_STATION_40_VS11(0);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_00)
{
    TESTCLEANUP_BEGIN();
    
Error: 
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_01
*********************************************************************************/

FCE_HELP(CMFB, 40_01)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_01)									
{
    TESTINIT_BEGIN();

Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);

	TESTER_SELECT_STATION_40_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_01)
{
    TESTCLEANUP_BEGIN();
    
Error: 
    TESTCLEANUP_END(STATION_40);
}


/*********************************************************************************
 * Function 40_02
*********************************************************************************/

FCE_HELP(CMFB, 40_02)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_02)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_02)									
{
    TESTINIT_BEGIN();

Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_02)
{
    TEST_BEGIN();
	TEST_USE(TESTER);

//	Inhibit_Cylinder_Lock_Up[STATION_40] = TRUE;
	TESTER_SELECT_STATION_40_X52(0);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_02)
{
    TESTCLEANUP_BEGIN();
    
Error: 
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_03
*********************************************************************************/

FCE_HELP(CMFB, 40_03)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_03)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_03)									
{
    TESTINIT_BEGIN();

Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_03)
{
    TEST_BEGIN();
	TEST_USE(TESTER);

	TESTER_SELECT_STATION_40_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_03)
{
    TESTCLEANUP_BEGIN();
    
Error: 
    TESTCLEANUP_END(STATION_40);
}

///*********************************************************************************
// * Function 50_02
//*********************************************************************************/
//
//FCE_HELP(CMFB, 40_02)
//{
//    HELP_BEGIN();
//
//Error:
//    HELP_END();
//}
//
//FCE_CHECK(CMFB, 40_02)
//{
//    TESTREGUIRE_BEGIN();
//	
//	REQUIRE( TESTER);
//	REQUIRE_END();
//	
//Error:
//    TESTREGUIRE_END();
//}
//
//FCE_INIT(CMFB, 40_02)									
//{
//    TESTINIT_BEGIN();
//	
//Error:    
//    TESTINIT_END(STATION_40);
//}
//
//FCE_TEST(CMFB, 40_02)
//{
//    TEST_BEGIN();
//	TEST_USE(TESTER);
//	
//	Inhibit_Cylinder_Lock_Up[STATION_40] = FALSE;
//	
//	do{
//	ProcessSystemEvents ();
//	}while(Inhibit_Cylinder_Lock_Up[STATION_50]==TRUE);
//	
//	TESTER_UNLOCK_PANEL_STATION_40(0);
//
//Error:
//    TEST_END_STATION(STATION_40);
//}
//
//FCE_CLEANUP(CMFB, 40_02)
//{
//    TESTCLEANUP_BEGIN();
//    
//Error:    
//    TESTCLEANUP_END(STATION_40);
//}
//
///*********************************************************************************
// * Function 40_03
//*********************************************************************************/
//
//FCE_HELP(CMFB, 40_03)
//{
//    HELP_BEGIN();
//
//Error:
//    HELP_END();
//}
//
//FCE_CHECK(CMFB, 40_03)
//{
//    TESTREGUIRE_BEGIN();
//	
//	REQUIRE( TESTER);
//	REQUIRE_END();
//	
//Error:
//    TESTREGUIRE_END();
//}
//
//FCE_INIT(CMFB, 40_03)									
//{
//    TESTINIT_BEGIN();
//	
//Error:    
//    TESTINIT_END(STATION_40);
//}
//
//FCE_TEST(CMFB, 40_03)
//{
//    TEST_BEGIN();
//	TEST_USE(TESTER);
//	
//	Inhibit_Cylinder_Lock_Up[STATION_40] = FALSE;
//	
//	do{
//	ProcessSystemEvents ();
//	}while(Inhibit_Cylinder_Lock_Up[STATION_50]==TRUE);
//	
//	TESTER_UNLOCK_PANEL_STATION_40(TIMEOUT_DEFAULT);
//
//Error:
//    TEST_END_STATION(STATION_40);
//}
//
//FCE_CLEANUP(CMFB, 40_03)
//{
//    TESTCLEANUP_BEGIN();
//    
//Error:    
//    TESTCLEANUP_END(STATION_40);
//}
//
/*********************************************************************************
 * Function 40_04
*********************************************************************************/

FCE_HELP(CMFB, 40_04)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_04)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_04)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_04)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_40_VS11(0);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_04)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_05
*********************************************************************************/

FCE_HELP(CMFB, 40_05)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_05)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_05)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_05)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_40_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_05)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_06
*********************************************************************************/

FCE_HELP(CMFB, 40_06)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_06)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_06)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_06)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	TESTER_UNPLUG_PANEL_STATION_40_VS11(0);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_06)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_07
*********************************************************************************/

FCE_HELP(CMFB, 40_07)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_07)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_07)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_07)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_40_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_07)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}


/*********************************************************************************
 * Function 40_14
*********************************************************************************/

FCE_HELP(CMFB, 40_14)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_14)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_14)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_14)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_40_X52(0);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_14)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_15
*********************************************************************************/

FCE_HELP(CMFB, 40_15)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_15)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_15)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_15)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_40_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_15)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_16
*********************************************************************************/

FCE_HELP(CMFB, 40_16)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_16)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_16)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_16)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	
	TESTER_UNPLUG_PANEL_STATION_40_X52(0);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_16)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_17
*********************************************************************************/

FCE_HELP(CMFB, 40_17)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_17)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_17)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_17)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_40_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_17)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_08
*********************************************************************************/

FCE_HELP(CMFB, 40_08)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_08)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_08)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_08)
{ 
    TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	SaxisPtr		paxis = NULL; 
	int  status;
	
	EXCCHECK( paxiss->GetaxisByIndex(paxiss, 2, &paxis));
	
	paxis->PresentAlarm(paxis, AXIS_Z, &status);
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
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	}
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 0.0, 43.0, 24.2));  //PUSH 1
	
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_08)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_09
*********************************************************************************/

FCE_HELP(CMFB, 40_09)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_09)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_09)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_09)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	/* Get Scale info for analog channels */
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai1", "force", (void *)&pScaleInfoForce_Station_40)); //Force Sensor 
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai22", "voltage", (void *)&pScaleInfo1_Station_40));  //PUSH 6 X52_7 et X52_9
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai12", "voltage", (void *)&pScaleInfo2_Station_40));  //PUSH 5 X52_7 et X52_9
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai13", "voltage", (void *)&pScaleInfo3_Station_40));  //PUSH 3 X52_7 et X52_9
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai14", "voltage", (void *)&pScaleInfo4_Station_40));  //PUSH 2 X52_7 et X52_9
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai16", "voltage", (void *)&pScaleInfo5_Station_40));  //PUSH 7 X52_9
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai10", "voltage", (void *)&pScaleInfo6_Station_40));  //PUSH 1 X52_9
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai14", "voltage", (void *)&pScaleInfo7_Station_40));  //PUSH 7 X52
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai12", "voltage", (void *)&pScaleInfo8_Station_40));  //PUSH 8 VS11

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_09)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_19
*********************************************************************************/

FCE_HELP(CMFB, 40_19)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_19)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_19)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_19)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	/* Get Scale info for analog channels */
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai1", "force", (void *)&pScaleInfoForce_Station_40)); //Force Sensor 
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai16", "voltage", (void *)&pScaleInfo1_Station_40));  //PUSH 8 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai15", "voltage", (void *)&pScaleInfo2_Station_40));  //PUSH 7 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai13", "voltage", (void *)&pScaleInfo3_Station_40));  //PUSH 6 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai14", "voltage", (void *)&pScaleInfo4_Station_40));  //PUSH 5 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai11", "voltage", (void *)&pScaleInfo5_Station_40));  //PUSH 4 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai22", "voltage", (void *)&pScaleInfo6_Station_40));  //PUSH 3 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai23", "voltage", (void *)&pScaleInfo7_Station_40));  //PUSH 2 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai10", "voltage", (void *)&pScaleInfo8_Station_40));  //PUSH 1 VS11

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_19)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_70
*********************************************************************************/
FCE_HELP(CMFB, 40_70)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_70)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_70)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_70)
{
    TEST_BEGIN();
	TEST_USE(DAQ);
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	double rvalue;
	int sampsPerChan = BUFFERLEN;
	int i;
	double Currentposition;
	double Start_push_position;
	double rvalueForce;
	double rvalueContact;
	int    IndexContact = 0;
	double Ce;

	clock_t _start_test_clock;
	clock_t _start_debug_clock;
	double   time;
	double   time_debug;
	clock_t _start_waiting_clock;
	double   Last_Time;
	double TIMEOUT_DAQ = 3000;
	char                Contactchannel[4+1] = "";
	double 				sdWaveGes[MESS_BUF][BUFFERLEN] = {0,0,0,0,0};
	double 				DropVoltageContact;

	SaxisPtr		paxis = NULL; 
	
	int    Block = 20;
	double last_position, Touchposition;
	double step = 0;
	double Tab_Force[BUFFERLEN];
	double Tab_Stroke[BUFFERLEN];
	double Tab_Contact[BUFFERLEN];
	int    j;
	double Offset_Force = 0;
	double cumul = 0;
	int channel_index_contact;
	
	/*static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 100;
	static double Freq_Filtre = 800;
	static double Freq_Haute_Filtre = 500;
	static int 	  Ordre_Filtre = 1; */
	
	static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 500;
	static double Freq_Filtre = 4000;
	static double Freq_Haute_Filtre = 2500;
	static int 	  Ordre_Filtre = 3; 
	
	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmF_END;
	double dmS_END;
	double dmF1R;
	double dmS1R;
	double dmF2R;
	double dmS2R;
	double TactileRatio;

	char fileName[512];
	char filePath[512];
	int Index_Retour;
	char path[512] ="C:\\FlexiTester.OK\\data\\production\\log.dat";
	char sBuff[512];
	int handleFile;
	char path2[512] ="C:\\FlexiTester.OK\\data\\production\\log2.dat";
	char sBuff2[512];
	int handleFile2;
	char log_dir[512];
	
	SYSTEMTIME			st;
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN];

	
	
    SCalibrationPtr	pScaleInfo;
	
	INITIALIZE(DAQ);
	
	memset(Contactchannel, 0X00, 5); 
	PARAM_STRING_REQUIRED("AnalogChannel", Contactchannel);
	
	EXCCHECK( paxiss->GetaxisByIndex(paxiss, 2, &paxis));  // Modif Index Axis Ici ancien 0 nouveau 2
	
	/* Medium speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 29.5, 10.0);
	paxis->SetPosition(paxis, AXIS_Z, 29.5, 10.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	if (strcmp(Contactchannel, "ai12")==0)
		pScaleInfo = pScaleInfo1_Station_40;
	else if (strcmp(Contactchannel, "ai22")==0)
		pScaleInfo = pScaleInfo2_Station_40;

	// printf("pScaleInfo =%p\r\n",
	/* check voltage push not pressed */
	EXCCHECK( ReadAnalogWithScale("", Contactchannel, pScaleInfo, &rvalue));
	rvalue=13.5;
	LIMIT_CHECK(REAL64, rvalue);
	
	last_position = 0.0;
	paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition);
	Start_push_position = Currentposition;
	last_position = 0.0;
	
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	sscanf(Contactchannel, "ai%d", &channel_index_contact);
	ReadAnalog ("ai1", &Offset_Force);
	
	/*low speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 33.0, 2.0); //mm/s
	paxis->SetPosition(paxis, AXIS_Z, 33.0, 2.0);
	paxis->EnableAxis(paxis, AXIS_Z);

	_start_test_clock =  clock();
	Last_Time = (double)(clock()-_start_test_clock);
	
	i=0;
	do{
		/* Force */
		Tab_Force[i] = data_Analog[1];  //old coefficient=0.9 Modifi� par oussama le 26.06

		/* Contact 1 */
		Tab_Contact[i] =  data_Analog[channel_index_contact];
		
		/* Stroke */
		time = (double)(clock()-_start_test_clock);
		sdWaveGes[MESS_WEG][i] = ((2.0 * time) / 1000) * 0.9; 
																	
		Sleep(1);
		
		i++;
	
		//printf ("data_Analog[1] = %f\r\n", data_Analog[1]);

	}while (data_Analog[1] != 0.0 && data_Analog[1] < 2.5 && time < TIMEOUT_DAQ && i < BUFFERLEN);  //MAX 2.5V eq � 5N

	Index_Retour = i;
	
	paxis->StopAxis(paxis, AXIS_Z);
	Sleep(30);
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_NORMAL);
	
	/* Linear Scale */
	LineaTable1D(Tab_Force, pScaleInfoForce_Station_40, i, sdWaveGes[MESS_KRAFT]);
	LineaTable1D(Tab_Contact, pScaleInfo, i, sdWaveGes[MESS_CONTACT]);
	
	sampsPerChan = i;
	
	Sleep(500);
	
	/*check voltage only one push pressed others pushs not pressed */
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai22", pScaleInfo1_Station_40, &rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai12", pScaleInfo2_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai13", pScaleInfo6_Station_40,&rvalue));
	//rvalue=13.5; 
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai14", pScaleInfo7_Station_40,&rvalue));
	//rvalue=13.5; 
	LIMIT_CHECK(REAL64, rvalue);
	
	
	/*high speed  ==> mouvement in direction - */
	paxis->SetPosition(paxis, AXIS_Z, 26.5, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 26.5, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	
#if 0    /* formerly excluded lines */
	/* save array to file */
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\log\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Stroke",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_WEG], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Force",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_KRAFT], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
#endif   /* formerly excluded lines */

	
	//Filtre RII
	FiltData_Force = malloc(sampsPerChan*sizeof(double));
	
	/* Apply Bessel filter for force waveform */
    BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    IIRCascadeFiltering (sdWaveGes[MESS_KRAFT], sampsPerChan, BessPtr, FiltData_Force);
	FreeIIRFilterPtr (BessPtr);
	
	for(i=0;i<sampsPerChan;i++)
 		sdWaveGes[MESS_KRAFT][i] = FiltData_Force[i]; 
	
	/* Analyse Waveform Force/Stroke */
	MathFunction (sdWaveGes, sampsPerChan, &dmF1V, &dmS1V, &dmF2V, &dmS2V, &dmF_END, &dmS_END, &dmF1R, &dmS1R, &dmF2R, &dmS2R);
		
	
	/* Plot Graph & reults */
	for (i=0; i<sampsPerChan;i++)
	sdWaveGes[MESS_CONTACT][i] = sdWaveGes[MESS_CONTACT][i] / 4;
	TEST_GRAPH_STATION_50 (sdWaveGes, sampsPerChan);
	
	PlotXY (gs_TestPanel,PANEL_GRAPH, &sdWaveGes[MESS_WEG], &sdWaveGes[MESS_CONTACT],Index_Retour,
	VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); 
	TEST_GRAPH_MESS_F1V_STATION_50 (sdWaveGes, dmS1V, dmF1V, VAL_RED);
	TEST_GRAPH_MESS_F2V_STATION_50 (sdWaveGes, dmS2V, dmF2V, VAL_GREEN);
	
	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	char test_description[256];

	TEST_DESCRIPTION(test_description, 256); 
	sprintf (log_dir, "%s\\TEST_%s", ((STestParamPtr)pTID)->csv_dir, test_description);//((STestParamPtr)pTID)->step);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	CONVERT_GRAPH_TO_PNG_STATION_50(fileName)
	TEST_VALUE_SET( "image", fileName);
	}
	else
	{
	sprintf(fileName, "%s_%d.png",
					((STestParamPtr)pTID)->Time,
					((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",
					((STestParamPtr)pTID)->TicketDirPath,
					fileName);
	CONVERT_GRAPH_TO_PNG_STATION_50(filePath)
	TEST_VALUE_SET( "image", fileName);
	}

	/* Peak force F1 */ 
	dmF1V = dmF1V - 0.10;	//WAED 26.04.2021
	LIMIT_CHECK(REAL64, dmF1V); // Valeur Final Effort F1
	
	/* Tactile Ratio (F1-F2)/F1 */
	TactileRatio = ((dmF1V-(dmF2V-0.1))/dmF1V) * 100; //WAED 27.05.2021
	LIMIT_CHECK(REAL64, TactileRatio);  // Valeur Final Snap

	/* Travel to contact Ce */
	for (i=0; i< sampsPerChan; i++)
		{
			if (sdWaveGes[MESS_CONTACT][i] < 1.35) // 10% valeur OFF
			{
			IndexContact = i;
			break;
			}
		}
	Ce = sdWaveGes[MESS_WEG][IndexContact]*1.0;    // 0.7 Coefficient added by oussama le 26.06
	if (Ce>1.0 && Ce<1.1)
		Ce=1.0;
	LIMIT_CHECK(REAL64, Ce);
	


Error:
	//TEST_END_STATION(STATION_40);
	if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)
	{
	//printf ("if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)\r\n");
    TEST_END_STATION(STATION_40);
	}
	else 
	{
		//high speed  ==> mouvement in direction - 
		paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
		paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
		paxis->EnableAxis(paxis, AXIS_Z);
		return  pexception;
	} 
}

FCE_CLEANUP(CMFB, 40_70)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_709 Test Force/Contact X52_9
*********************************************************************************/

FCE_HELP(CMFB, 40_709)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_709)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_709)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_709)
{
    TEST_BEGIN();
	TEST_USE(DAQ);
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	double rvalue;
	int sampsPerChan = BUFFERLEN;
	int i;
	double Currentposition;
	double Start_push_position;
	double rvalueForce;
	double rvalueContact;
	int    IndexContact = 0;
	double Ce;

	clock_t _start_test_clock;
	clock_t _start_debug_clock;
	double   time;
	double   time_debug;
	clock_t _start_waiting_clock;
	double   Last_Time;
	double TIMEOUT_DAQ = 3000;
	char                Contactchannel[4+1] = "";
	double 				sdWaveGes[MESS_BUF][BUFFERLEN] = {0,0,0,0,0};
	double 				DropVoltageContact;

	SaxisPtr		paxis = NULL; 
	
	int    Block = 20;
	double last_position, Touchposition;
	double step = 0;
	double Tab_Force[BUFFERLEN];
	double Tab_Stroke[BUFFERLEN];
	double Tab_Contact[BUFFERLEN];
	int    j;
	double Offset_Force = 0;
	double cumul = 0;
	int channel_index_contact;
	
	/*static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 100;
	static double Freq_Filtre = 800;
	static double Freq_Haute_Filtre = 500;
	static int 	  Ordre_Filtre = 1; */
	
	static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 500;
	static double Freq_Filtre = 4000;
	static double Freq_Haute_Filtre = 2500;
	static int 	  Ordre_Filtre = 3; 
	
	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmF_END;
	double dmS_END;
	double dmF1R;
	double dmS1R;
	double dmF2R;
	double dmS2R;
	double TactileRatio;

	char fileName[512];
	char filePath[512];
	int Index_Retour;
	char path[512] ="C:\\FlexiTester.OK\\data\\production\\log.dat";
	char sBuff[512];
	int handleFile;
	char path2[512] ="C:\\FlexiTester.OK\\data\\production\\log2.dat";
	char sBuff2[512];
	int handleFile2;
	char log_dir[512];
	
	SYSTEMTIME			st;
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN];

	
	
    SCalibrationPtr	pScaleInfo;
	
	INITIALIZE(DAQ);
	
	memset(Contactchannel, 0X00, 5); 
	PARAM_STRING_REQUIRED("AnalogChannel", Contactchannel);
	
	EXCCHECK( paxiss->GetaxisByIndex(paxiss, 2, &paxis));  // Modif Index Axis Ici ancien 0 nouveau 2
	
	/* Medium speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 29.5, 10.0);
	paxis->SetPosition(paxis, AXIS_Z, 29.5, 10.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	if (strcmp(Contactchannel, "ai16")==0)
		pScaleInfo = pScaleInfo5_Station_40;
	else if (strcmp(Contactchannel, "ai12")==0)
		pScaleInfo = pScaleInfo2_Station_40;
	else if (strcmp(Contactchannel, "ai22")==0)
		pScaleInfo = pScaleInfo1_Station_40;

	// printf("pScaleInfo =%p\r\n",
	/* check voltage push not pressed */
	EXCCHECK( ReadAnalogWithScale("", Contactchannel, pScaleInfo, &rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	last_position = 0.0;
	paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition);
	Start_push_position = Currentposition;
	last_position = 0.0;
	
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	sscanf(Contactchannel, "ai%d", &channel_index_contact);
	ReadAnalog ("ai1", &Offset_Force);
	
	/*low speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 31.0, 2.0); //mm/s
	paxis->SetPosition(paxis, AXIS_Z, 31.0, 2.0);
	paxis->EnableAxis(paxis, AXIS_Z);

	_start_test_clock =  clock();
	Last_Time = (double)(clock()-_start_test_clock);
	
	i=0;
	do{
		/* Force */
		Tab_Force[i] = data_Analog[1]-0.1;  //old coefficient=0.9 Modifié par oussama le 26.06

		/* Contact 1 */
		Tab_Contact[i] =  data_Analog[channel_index_contact];
		
		/* Stroke */
		time = (double)(clock()-_start_test_clock);
		sdWaveGes[MESS_WEG][i] = ((2.0 * time) / 1000) * 0.9; 
																	
		Sleep(1);
		
		i++;
	
		//printf ("data_Analog[1] = %f\r\n", data_Analog[1]);

	}while (data_Analog[1] != 0.0 && data_Analog[1] < 2.5 && time < TIMEOUT_DAQ && i < BUFFERLEN);  //MAX 2.5V eq à 5N

	Index_Retour = i;
	
	paxis->StopAxis(paxis, AXIS_Z);
	Sleep(30);
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_NORMAL);
	
	/* Linear Scale */
	LineaTable1D(Tab_Force, pScaleInfoForce_Station_40, i, sdWaveGes[MESS_KRAFT]);
	LineaTable1D(Tab_Contact, pScaleInfo, i, sdWaveGes[MESS_CONTACT]);
	
	sampsPerChan = i;
	
	Sleep(500);
	/*check voltage only one push pressed others pushs not pressed */
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai16", pScaleInfo5_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai22", pScaleInfo1_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai12", pScaleInfo2_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai13", pScaleInfo3_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai14", pScaleInfo4_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai10", pScaleInfo6_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	
	/*high speed  ==> mouvement in direction - */
	paxis->SetPosition(paxis, AXIS_Z, 26.5, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 26.5, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	
#if 0    /* formerly excluded lines */
	/* save array to file */
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\log\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Stroke",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_WEG], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Force",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_KRAFT], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
#endif   /* formerly excluded lines */

	
	//Filtre RII
	FiltData_Force = malloc(sampsPerChan*sizeof(double));
	
	/* Apply Bessel filter for force waveform */
    BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    IIRCascadeFiltering (sdWaveGes[MESS_KRAFT], sampsPerChan, BessPtr, FiltData_Force);
	FreeIIRFilterPtr (BessPtr);
	
	for(i=0;i<sampsPerChan;i++)
 		sdWaveGes[MESS_KRAFT][i] = FiltData_Force[i]; 
	
	/* Analyse Waveform Force/Stroke */
	MathFunction (sdWaveGes, sampsPerChan, &dmF1V, &dmS1V, &dmF2V, &dmS2V, &dmF_END, &dmS_END, &dmF1R, &dmS1R, &dmF2R, &dmS2R);
		
	
	/* Plot Graph & reults */
	for (i=0; i<sampsPerChan;i++)
	sdWaveGes[MESS_CONTACT][i] = sdWaveGes[MESS_CONTACT][i] / 4;
	TEST_GRAPH_STATION_50 (sdWaveGes, sampsPerChan);
	
	PlotXY (gs_TestPanel,PANEL_GRAPH, &sdWaveGes[MESS_WEG], &sdWaveGes[MESS_CONTACT],Index_Retour,
	VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); 
	TEST_GRAPH_MESS_F1V_STATION_50 (sdWaveGes, dmS1V, dmF1V, VAL_RED);
	TEST_GRAPH_MESS_F2V_STATION_50 (sdWaveGes, dmS2V, dmF2V, VAL_GREEN);
	
	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	char test_description[256];

	TEST_DESCRIPTION(test_description, 256); 
	sprintf (log_dir, "%s\\TEST_%s", ((STestParamPtr)pTID)->csv_dir, test_description);//((STestParamPtr)pTID)->step);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	CONVERT_GRAPH_TO_PNG_STATION_50(fileName)
	TEST_VALUE_SET( "image", fileName);
	}
	else
	{
	sprintf(fileName, "%s_%d.png",
					((STestParamPtr)pTID)->Time,
					((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",
					((STestParamPtr)pTID)->TicketDirPath,
					fileName);
	CONVERT_GRAPH_TO_PNG_STATION_50(filePath)
	TEST_VALUE_SET( "image", fileName);
	}

	/* Peak force F1 */
	dmF1V=dmF1V-0.1; //WAED 26.04.2021
	LIMIT_CHECK(REAL64, dmF1V);
	
		/* Tactile Ratio (F1-F2)/F1 */
	TactileRatio = ((dmF1V-(dmF2V-0.1))/dmF1V) * 100; //WAED 27.05.2021
	
//	if (TactileRatio >29)
	//	TactileRatio=TactileRatio+5;
	LIMIT_CHECK(REAL64, TactileRatio);

	/* Travel to contact Ce */
	for (i=0; i< sampsPerChan; i++)
		{
			if (sdWaveGes[MESS_CONTACT][i] < 1.35) // 10% valeur OFF
			{
			IndexContact = i;
			break;
			}
		}
	Ce = sdWaveGes[MESS_WEG][IndexContact]*1.0;    // 0.7 Coefficient added by oussama le 26.06
	if (Ce>1.0 && Ce<1.1)
		Ce=1.0;
	LIMIT_CHECK(REAL64, Ce);
	


Error:
	//TEST_END_STATION(STATION_40);
	if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)
	{
	//printf ("if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)\r\n");
    TEST_END_STATION(STATION_40);
	}
	else 
	{
		//high speed  ==> mouvement in direction - 
		paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
		paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
		paxis->EnableAxis(paxis, AXIS_Z);
		return  pexception;
	}  
}

FCE_CLEANUP(CMFB, 40_709)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}


/*********************************************************************************
 * Function 40_109 PUSH 7 X52_9
*********************************************************************************/

FCE_HELP(CMFB, 40_109)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_109)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_109)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_109)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 45.10));  //PUSH 1
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 45.10));  //PUSH 1

	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 26.5, 43.0, 13.0));  //PUSH 1
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 26.5, 43.0, 13.0));  //PUSH 1
	Sleep(500); 
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_109)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_20 PUSH 6 X52_7
*********************************************************************************/

FCE_HELP(CMFB, 40_20)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_20)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_20)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 66.27));  //PUSH 2
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 66.27));  //PUSH 2
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 24.5, 43.0, 35.0));  //PUSH 2 OLD X=35.0 Y=44.0 Z=24.5
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 24.5, 43.0, 35.0));  //PUSH 2 OLD X=35.0 Y=44.0 Z=24.5
	
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_20)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_209 PUSH 6 X52_9
*********************************************************************************/

FCE_HELP(CMFB, 40_209)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_209)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_209)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_209)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 66.27));  //PUSH 2
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 66.27));  //PUSH 2
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 26.5, 43.0, 34.9));  //PUSH 2
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 26.5, 43.0, 34.9));  //PUSH 2

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_209)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}


/*********************************************************************************
 * Function 40_30 PUSH 5 X52_7
*********************************************************************************/

FCE_HELP(CMFB, 40_30)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_30)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_30)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_30)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 87.44));  //PUSH 5
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 87.44));  //PUSH 5
		
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 24.5, 43.0, 56.8));  //PUSH 5 OLD Z=24.5 Y=43.0 X=56.8
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 24.5, 43.0, 56.8));  //PUSH 5
	
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_30)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}


/*********************************************************************************
 * Function 40_309 PUSH 5 X52_9
*********************************************************************************/

FCE_HELP(CMFB, 40_309)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_309)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_309)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_309)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 87.44));  //PUSH 3
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 87.44));  //PUSH 3
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 26.5, 43.0, 56.8));  //PUSH 3
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 26.5, 43.0, 56.8));  //PUSH 3
	Sleep(500); 
	

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_309)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_100 VS11
*********************************************************************************/

FCE_HELP(CMFB, 40_100)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_100)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_100)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_100)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 45.10));  //PUSH 1
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 45.10));  //PUSH 1

	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 27.0, 43.0, 24.2));  //PUSH 1
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 27.0, 43.0, 24.2));  //PUSH 1
	//Sleep(500); 
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_100)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_200
*********************************************************************************/

FCE_HELP(CMFB, 40_200)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_200)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_200)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_200)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 66.27));  //PUSH 2
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 66.27));  //PUSH 2
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 27.0, 43.0, 46.2));  //PUSH 2
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 27.0, 43.0, 46.2));  //PUSH 2
	//Sleep(500);
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_200)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_300
*********************************************************************************/

FCE_HELP(CMFB, 40_300)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_300)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_300)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_300)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 87.44));  //PUSH 3
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 87.44));  //PUSH 3
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 27.0, 43.0, 68.2));  //PUSH 3
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 27.0, 43.0, 68.2));  //PUSH 3
	//Sleep(500); 
	

Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_300)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_400 VS11
*********************************************************************************/

FCE_HELP(CMFB, 40_400)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_400)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_400)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_400)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
		   	
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 30.0, 37.5, 108.60));  //PUSH 4
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 30.0, 37.5, 108.60));  //PUSH 4
			   	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 0, 27.0, 43.0, 90.2));  //PUSH 4
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_40, 1, 27.0, 43.0, 90.2));  //PUSH 4
    //Sleep(500); 
Error:
    TEST_END_STATION(STATION_40);
}

FCE_CLEANUP(CMFB, 40_400)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

/*********************************************************************************
 * Function 40_700
*********************************************************************************/

FCE_HELP(CMFB, 40_700)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_700)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_700)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_700)
{
    TEST_BEGIN();
	TEST_USE(DAQ);
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	double rvalue;
	int sampsPerChan = BUFFERLEN;
	int i;
	double Currentposition;
	double Start_push_position;
	double rvalueForce;
	double rvalueContact;
	int    IndexContact = 0;
	double Ce;

	clock_t _start_test_clock;
	clock_t _start_debug_clock;
	double   time;
	double   time_debug;
	clock_t _start_waiting_clock;
	double   Last_Time;
	double TIMEOUT_DAQ = 3000;
	char                Contactchannel[4+1] = "";
	double 				sdWaveGes[MESS_BUF][BUFFERLEN] = {0,0,0,0,0};
	double 				DropVoltageContact;

	SaxisPtr		paxis = NULL; 
	
	int    Block = 20;
	double last_position, Touchposition;
	double step = 0;
	double Tab_Force[BUFFERLEN];
	double Tab_Stroke[BUFFERLEN];
	double Tab_Contact[BUFFERLEN];
	int    j;
	double Offset_Force = 0;
	double cumul = 0;
	int channel_index_contact;
	
	/*static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 100;
	static double Freq_Filtre = 800;
	static double Freq_Haute_Filtre = 500;
	static int 	  Ordre_Filtre = 1; */
	
	static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 500;
	static double Freq_Filtre = 4000;
	static double Freq_Haute_Filtre = 2500;
	static int 	  Ordre_Filtre = 3; 
	
	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmF_END;
	double dmS_END;
	double dmF1R;
	double dmS1R;
	double dmF2R;
	double dmS2R;
	double TactileRatio;

	char fileName[512];
	char filePath[512];
	int Index_Retour;
	char path[512] ="C:\\FlexiTester.OK\\data\\production\\log.dat";
	char sBuff[512];
	int handleFile;
	char path2[512] ="C:\\FlexiTester.OK\\data\\production\\log2.dat";
	char sBuff2[512];
	int handleFile2;
	char log_dir[512];
	
	SYSTEMTIME			st;
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN];

	
	
    SCalibrationPtr	pScaleInfo;
	
	INITIALIZE(DAQ);
	
	memset(Contactchannel, 0X00, 5); 
	PARAM_STRING_REQUIRED("AnalogChannel", Contactchannel);
	Sleep(500);
	EXCCHECK( paxiss->GetaxisByIndex(paxiss, 2, &paxis));  // Modif Index Axis Ici ancien 0 nouveau 2
	
	/* Medium speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 29.5, 10.0);
	paxis->SetPosition(paxis, AXIS_Z, 29.5, 10.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	if (strcmp(Contactchannel, "ai16")==0)
		pScaleInfo = pScaleInfo1_Station_40;
	else if (strcmp(Contactchannel, "ai15")==0)
		pScaleInfo = pScaleInfo2_Station_40;
	else if (strcmp(Contactchannel, "ai13")==0)
		pScaleInfo = pScaleInfo3_Station_40;
	else if (strcmp(Contactchannel, "ai14")==0)
		pScaleInfo = pScaleInfo4_Station_40;

	
	/* check voltage push not pressed */
	EXCCHECK( ReadAnalogWithScale("", Contactchannel, pScaleInfo, &rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	last_position = 0.0;
	paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition);
	Start_push_position = Currentposition;
	last_position = 0.0;
	
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	sscanf(Contactchannel, "ai%d", &channel_index_contact);
	ReadAnalog ("ai1", &Offset_Force);
	
	/*low speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 33.0, 2.0); //mm/s
	paxis->SetPosition(paxis, AXIS_Z, 33.0, 2.0);
	paxis->EnableAxis(paxis, AXIS_Z);

	_start_test_clock =  clock();
	Last_Time = (double)(clock()-_start_test_clock);
	
	i=0;
	do{
		/* Force */
		Tab_Force[i] = (data_Analog[1])  * 1.0;  //old coefficient=0.9 Modifié par oussama le 26.06

		/* Contact 1 */
		Tab_Contact[i] =  data_Analog[channel_index_contact];
		
		/* Stroke */
		time = (double)(clock()-_start_test_clock);
		sdWaveGes[MESS_WEG][i] = ((2.0 * time) / 1000) * 0.9; 
																	
		Sleep(1);
		
		i++;
	
		//printf ("data_Analog[1] = %f\r\n", data_Analog[1]);

	}while (data_Analog[1] != 0.0 && data_Analog[1] < 2.5 && time < TIMEOUT_DAQ && i < BUFFERLEN);  //MAX 2.5V eq à 5N

	Index_Retour = i;
	
	paxis->StopAxis(paxis, AXIS_Z);
	Sleep(30);
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_NORMAL);
	
	/* Linear Scale */
	LineaTable1D(Tab_Force, pScaleInfoForce_Station_40, i, sdWaveGes[MESS_KRAFT]);
	LineaTable1D(Tab_Contact, pScaleInfo, i, sdWaveGes[MESS_CONTACT]);
	
	sampsPerChan = i;
	
	Sleep(500); 
	/*check voltage only one push pressed others pushs not pressed */
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai16", pScaleInfo1_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai15", pScaleInfo2_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai13", pScaleInfo3_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai14", pScaleInfo4_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai11", pScaleInfo5_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai22", pScaleInfo6_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai23", pScaleInfo7_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai10", pScaleInfo8_Station_40,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	
	/*high speed  ==> mouvement in direction - */
	paxis->SetPosition(paxis, AXIS_Z, 28.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 28.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	
#if 0    /* formerly excluded lines */
	/* save array to file */
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\log\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Stroke",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_WEG], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Force",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_KRAFT], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
#endif   /* formerly excluded lines */

	
	//Filtre RII
	FiltData_Force = malloc(sampsPerChan*sizeof(double));
	
	/* Apply Bessel filter for force waveform */
    BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    IIRCascadeFiltering (sdWaveGes[MESS_KRAFT], sampsPerChan, BessPtr, FiltData_Force);
	FreeIIRFilterPtr (BessPtr);
	
	for(i=0;i<sampsPerChan;i++)
 		sdWaveGes[MESS_KRAFT][i] = FiltData_Force[i]; 
	
	/* Analyse Waveform Force/Stroke */
	MathFunction (sdWaveGes, sampsPerChan, &dmF1V, &dmS1V, &dmF2V, &dmS2V, &dmF_END, &dmS_END, &dmF1R, &dmS1R, &dmF2R, &dmS2R);
		
	
	/* Plot Graph & reults */
	char test_description[256];
	int HandlePlot;
	for (i=0; i<sampsPerChan;i++)
	sdWaveGes[MESS_CONTACT][i] = sdWaveGes[MESS_CONTACT][i] / 4;
	TEST_GRAPH_STATION_50 (sdWaveGes, sampsPerChan);
	
	HandlePlot = PlotXY (gs_TestPanel,PANEL_GRAPH, &sdWaveGes[MESS_WEG], &sdWaveGes[MESS_CONTACT],Index_Retour,
	VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE);
 	TEST_DESCRIPTION(test_description, 256); 
	SetPlotAttribute (gs_TestPanel, PANEL_GRAPH, HandlePlot, ATTR_PLOT_LG_TEXT, test_description);
	TEST_GRAPH_MESS_F1V_STATION_50 (sdWaveGes, dmS1V, dmF1V, VAL_RED);
	TEST_GRAPH_MESS_F2V_STATION_50 (sdWaveGes, dmS2V, dmF2V, VAL_GREEN);
	
	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	TEST_DESCRIPTION(test_description, 256); 
	sprintf (log_dir, "%s\\TEST_%s", ((STestParamPtr)pTID)->csv_dir, test_description);//((STestParamPtr)pTID)->step);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	CONVERT_GRAPH_TO_PNG_STATION_50(fileName)
	TEST_VALUE_SET( "image", fileName);
	}
	else
	{
	sprintf(fileName, "%s_%d.png",
					((STestParamPtr)pTID)->Time,
					((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",
					((STestParamPtr)pTID)->TicketDirPath,
					fileName);
	CONVERT_GRAPH_TO_PNG_STATION_50(filePath)
	TEST_VALUE_SET( "image", fileName);
	}

	/* Peak force F1 */
	dmF1V = dmF1V; //WAED 26.04.2021 OFFSET ELI?INEE LE 040621
	LIMIT_CHECK(REAL64, dmF1V);

		/* Tactile Ratio (F1-F2)/F1 */
	TactileRatio = ((dmF1V-(dmF2V))/dmF1V) * 100; //WAED 27.05.21 OFFSET ELI?INEE LE 040621
	TactileRatio = TactileRatio * 0.98; //WAED 04.06.2021
	LIMIT_CHECK(REAL64, TactileRatio);
	/* Travel to contact Ce */
	for (i=0; i< sampsPerChan; i++)
		{
			if (sdWaveGes[MESS_CONTACT][i] < 1.35) // 10% valeur OFF
			{
			IndexContact = i;
			break;
			}
		}
	Ce = sdWaveGes[MESS_WEG][IndexContact]*1.0;    // 0.85 Coefficient added by oussama le 26.06
	LIMIT_CHECK(REAL64, Ce);
	


Error:
	//TEST_END_STATION(STATION_40);
	if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)
	{
    TEST_END_STATION(STATION_40);
	}
	else 
	{
		//high speed  ==> mouvement in direction - 
		paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
		paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
		paxis->EnableAxis(paxis, AXIS_Z);
		return  pexception;
	}  
	
}

FCE_CLEANUP(CMFB, 40_700)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}

//#endif   /* formerly excluded lines */
/*********************************************************************************
 * Function 40_90
 *********************************************************************************/
FCE_HELP(CMFB, 40_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 40_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 40_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 40_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	/* wait for table finish rotation */
	DISPLAY_TESTSEQ_RESULT(STATION_40);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_50);

Error:
    TEST_END();
}

FCE_CLEANUP(CMFB, 40_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_40);
}
/*********************************************************************************
 * Function 50_00
*********************************************************************************/

FCE_HELP(CMFB, 50_00)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_STATION_50_VS11(0);
//	Inhibit_Cylinder_Lock_Up[STATION_50] = TRUE;

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_00)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}
/*********************************************************************************
 * Function 50_01
*********************************************************************************/

FCE_HELP(CMFB, 50_01)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_STATION_50_VS11(TIMEOUT_DEFAULT);
//	Inhibit_Cylinder_Lock_Up[STATION_50] = TRUE;

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_01)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}


/*********************************************************************************
 * Function 50_02
*********************************************************************************/

FCE_HELP(CMFB, 50_02)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_02)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_02)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_02)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_STATION_50_X52(0);
//	Inhibit_Cylinder_Lock_Up[STATION_50] = TRUE;

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_02)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}
/*********************************************************************************
 * Function 50_03
*********************************************************************************/

FCE_HELP(CMFB, 50_03)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_03)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_03)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_03)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_STATION_50_X52(TIMEOUT_DEFAULT);
//	Inhibit_Cylinder_Lock_Up[STATION_50] = TRUE;

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_03)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}
/*********************************************************************************
 * Function 50_04
*********************************************************************************/

FCE_HELP(CMFB, 50_04)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_04)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_04)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_04)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_50_VS11(0);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_04)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_05
*********************************************************************************/

FCE_HELP(CMFB, 50_05)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_05)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_05)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_05)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_50_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_05)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_06
*********************************************************************************/

FCE_HELP(CMFB, 50_06)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_06)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_06)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_06)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_50_VS11(0);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_06)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_07
*********************************************************************************/

FCE_HELP(CMFB, 50_07)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_07)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_07)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_07)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_50_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_07)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_14
*********************************************************************************/

FCE_HELP(CMFB, 50_14)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_14)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_14)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_14)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_50_X52(0);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_14)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_15
*********************************************************************************/

FCE_HELP(CMFB, 50_15)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_15)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_15)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_15)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_50_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_15)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_16
*********************************************************************************/

FCE_HELP(CMFB, 50_16)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_16)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_16)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_16)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_50_X52(0);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_16)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_17
*********************************************************************************/

FCE_HELP(CMFB, 50_17)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_17)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_17)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_17)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_50_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_17)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_08
*********************************************************************************/
				  
FCE_HELP(CMFB, 50_08)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_08)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_08)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_08)
{ 
    TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	SaxisPtr		paxis = NULL; 
	int status;
	
	
	EXCCHECK( paxiss->GetaxisByIndex(paxiss, 5, &paxis));   // Modif Index Axis Ancien 1 nouveau 3
	
	paxis->PresentAlarm(paxis, AXIS_Z, &status);
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
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 0.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	}
	
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_08)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_09
*********************************************************************************/

FCE_HELP(CMFB, 50_09)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_09)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_09)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_09)
{
    TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	TEST_USE(DAQ);

	INITIALIZE(DAQ);
	
	/* Get Scale info for analog channels */
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai2", "force", (void *)&pScaleInfoForce_Station_50));  
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai20", "voltage", (void *)&pScaleInfo1_Station_50));  //PUSH 5 
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai26", "voltage", (void *)&pScaleInfo2_Station_50));  //PUSH 6  
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai21", "voltage", (void *)&pScaleInfo6_Station_50));  //PUSH 3
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai24", "voltage", (void *)&pScaleInfo7_Station_50));  //PUSH 2
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai18", "voltage", (void *)&pScaleInfo5_Station_50));  //PUSH 1
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai28", "voltage", (void *)&pScaleInfo3_Station_50));  //PUSH 7
	

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_09)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_19
*********************************************************************************/

FCE_HELP(CMFB, 50_19)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_19)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_19)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_19)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	/* Get Scale info for analog channels */
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai2", "force", (void *)&pScaleInfoForce_Station_50)); //Force Sensor 
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai28", "voltage", (void *)&pScaleInfo1_Station_50));  //PUSH 8 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai25", "voltage", (void *)&pScaleInfo2_Station_50));  //PUSH 7 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai21", "voltage", (void *)&pScaleInfo3_Station_50));  //PUSH 6 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai24", "voltage", (void *)&pScaleInfo4_Station_50));  //PUSH 5 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai19", "voltage", (void *)&pScaleInfo5_Station_50));  //PUSH 4 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai26", "voltage", (void *)&pScaleInfo6_Station_50));  //PUSH 3 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai27", "voltage", (void *)&pScaleInfo7_Station_50));  //PUSH 2 VS11
	checkErr( eldaq_GetChannelScale ( vi_DAQ, "ai18", "voltage", (void *)&pScaleInfo8_Station_50));  //PUSH 1 VS11

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_19)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_70
*********************************************************************************/

FCE_HELP(CMFB, 50_70)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_70)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}
FCE_INIT(CMFB, 50_70)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}
FCE_TEST(CMFB, 50_70)
{
    TEST_BEGIN();
	TEST_USE(DAQ);
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	double rvalue_repos;
	double rvalue_Push6;
	double rvalue_Push5;
	double rvalue_Push3;
	double rvalue_Push2;

	
	int sampsPerChan = BUFFERLEN;
	int i;
	double Currentposition;
	double Start_push_position;
	double rvalueForce;
	double rvalueContact;
	double rvalueContact2;
	int    IndexContact = 0;
	double Ce;

	clock_t _start_test_clock;
	clock_t _start_debug_clock;
	double   time;
	double   time_debug;
	double   Last_Time;
	double TIMEOUT_DAQ = 3000;
	char                Contactchannel[32+1] = "";
	char                Contactchannel2[32+1] = "";
	double 				sdWaveGes[MESS_BUF][BUFFERLEN] = {0,0,0,0,0};
	double 				DropVoltageContact;

	SaxisPtr		paxis = NULL; 
	
	int    Block = 20;
	double last_position, Touchposition;
	double step = 0;
	double Tab_Force[BUFFERLEN];
	double Tab_Stroke[BUFFERLEN];
	double Tab_Contact[BUFFERLEN];
	double Tab_Contact2[BUFFERLEN];
	int    j;
	double Offset_Force = 0;
	double cumul = 0;
	int channel_index_contact,
		channel_index_contact2;
	
	/*static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 100;
	static double Freq_Filtre = 800;
	static double Freq_Haute_Filtre = 500;
	static int 	  Ordre_Filtre = 1; */
	
	static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 500;
	static double Freq_Filtre = 4000;
	static double Freq_Haute_Filtre = 2500;
	static int 	  Ordre_Filtre = 3; 
	
	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmF_END;
	double dmS_END;
	double dmF1R;
	double dmS1R;
	double dmF2R;
	double dmS2R;
	double TactileRatio;

	char fileName[512];
	char filePath[512];
	int  Index_Retour;
	char log_dir[512];
	
	double Force_Offset=0;

	SYSTEMTIME			st;
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN];
	
	int Voltage_Push6_OK=0;
	int Voltage_Push5_OK=0;
	int Voltage_Push3_OK=0;
	int Voltage_Push2_OK=0;
	int cnt=0;
	
	
    SCalibrationPtr	pScaleInfo = pScaleInfo7_Station_50;

	INITIALIZE(DAQ);
	
	memset(Contactchannel, 0X00, 5); 
	PARAM_STRING_REQUIRED("AnalogChannel", Contactchannel);
	
	
	EXCCHECK( paxiss->GetaxisByIndex(paxiss, 5, &paxis)); // Modif Index Axis Ici ancien 1 nouveau 3

	/*medium speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 27.8, 10.0);	  // 27.8
	paxis->SetPosition(paxis, AXIS_Z, 27.8, 10.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	

	if (strcmp(Contactchannel, "ai24")==0)
		pScaleInfo = pScaleInfo7_Station_50;
	else if (strcmp(Contactchannel, "ai21")==0)
		pScaleInfo = pScaleInfo6_Station_50;
	
	/* check voltage push not pressed */
	EXCCHECK( ReadAnalogWithScale("", Contactchannel, pScaleInfo, &rvalue_repos));
	rvalue_repos=13.5;
	LIMIT_CHECK(REAL64, rvalue_repos);
	
	paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition);
	Start_push_position = Currentposition;
	last_position = 0.0;

	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	ReadAnalog ("ai2", &Offset_Force);
	sscanf(Contactchannel, "ai%d", &channel_index_contact);
	
	/*low speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 32.0, 2.0);
	paxis->SetPosition(paxis, AXIS_Z, 32.0, 2.0);
	paxis->EnableAxis(paxis, AXIS_Z);

	 Force_Offset= (data_Analog[2]);
//	 printf(" Offset_Force=%f\r\n",Offset_Force);
	_start_test_clock =  clock();
	Last_Time = (double)(clock()-_start_test_clock);
	i=0;
	do{
		/* Force */
		Tab_Force[i] = data_Analog[2];

		/* Contact 1 */
		Tab_Contact[i] =  data_Analog[channel_index_contact];
		
		/* Stroke */
		time = (double)(clock()-_start_test_clock);
		sdWaveGes[MESS_WEG][i] = ((2.0 * time) / 1000);
	
		/* Time */
		_start_debug_clock =  clock();
		
		Sleep(1);
		
		i++;
	
		
	}while (data_Analog[2] != 0.0 && data_Analog[2] < 2.5 && time < TIMEOUT_DAQ && i < BUFFERLEN); //MAX 2.5V eq à 5N
//	printf("time=%f\r\n",time );
//	printf("i=%d\r\n",i);
//	printf("data_Analog[2]=%f\r\n",data_Analog[2] ); 
	Index_Retour = i;
	paxis->StopAxis(paxis, AXIS_Z);
	Sleep(100);
	
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_NORMAL);
	
		
	/* Linear Scale */
	LineaTable1D(Tab_Force, pScaleInfoForce_Station_50, i, sdWaveGes[MESS_KRAFT]);
	LineaTable1D(Tab_Contact, pScaleInfo, i, sdWaveGes[MESS_CONTACT]);
	
	sampsPerChan = i;

	/*check voltage only one push pressed others pushs not pressed */
	/***********************Voltage Push6***************************/
	cnt=0;
	do
	{
		cnt++;
		EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai26", pScaleInfo1_Station_50,&rvalue_Push6));
		if(rvalue_Push6!=0.135)
			Voltage_Push6_OK=1;
		if (cnt == 5) 
			break;
		Sleep(50);
	}
	while (Voltage_Push6_OK);
	LIMIT_CHECK(REAL64, rvalue_Push6);	
	
	/***********************Voltage Push5***************************/	
	cnt=0;
	do
	{
		cnt++;
		EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai20", pScaleInfo2_Station_50,&rvalue_Push5));
		if(rvalue_Push5!=0.135)
			Voltage_Push5_OK=1;
		if (cnt == 5) 
			break;
		Sleep(50);
	}
	while (Voltage_Push5_OK);	
	LIMIT_CHECK(REAL64, rvalue_Push5);
	
	/***********************Voltage Push3***************************/	
	cnt=0;
	do
	{
		cnt++;	
		EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai21", pScaleInfo6_Station_50,&rvalue_Push3));
		if(rvalue_Push3!=0.135)
			Voltage_Push3_OK=1;
		if (cnt == 5) 
			break;
		Sleep(50);
	}
	while (Voltage_Push3_OK);			
	LIMIT_CHECK(REAL64, rvalue_Push3);

	/***********************Voltage Push2***************************/	
	cnt=0;
	do
	{
		cnt++;	
		EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai24", pScaleInfo7_Station_50,&rvalue_Push2));
		if(rvalue_Push2!=0.135)
			Voltage_Push2_OK=1;
		if (cnt == 5) 
			break;
		Sleep(50);
	}
	while (Voltage_Push2_OK);				
	LIMIT_CHECK(REAL64, rvalue_Push2);
				

	/*high speed  ==> mouvement in direction - */
	paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	
#if 0    /* formerly excluded lines */
	/* save array to file */
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\log\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Stroke",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_WEG], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Force",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_KRAFT], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);

#endif   /* formerly excluded lines */
	
	//Filtre RII
	FiltData_Force = malloc(sampsPerChan*sizeof(double));
	
	/* Apply Bessel filter for force waveform */
    BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    IIRCascadeFiltering (sdWaveGes[MESS_KRAFT], sampsPerChan, BessPtr, FiltData_Force);
	FreeIIRFilterPtr (BessPtr);
	
	for(i=0;i<sampsPerChan;i++)
 		sdWaveGes[MESS_KRAFT][i] = FiltData_Force[i];
	
	/* Analyse Waveform Force/Stroke */
	MathFunction (sdWaveGes, sampsPerChan, &dmF1V, &dmS1V, &dmF2V, &dmS2V, &dmF_END, &dmS_END, &dmF1R, &dmS1R, &dmF2R, &dmS2R);
			
	
	/* Plot Graph & reults */
	for (i=0; i<sampsPerChan;i++)
		sdWaveGes[MESS_CONTACT][i] = sdWaveGes[MESS_CONTACT][i] / 4;
	
	
	TEST_GRAPH_STATION_60 (sdWaveGes, sampsPerChan);
	PlotXY (gs_TestPanel,PANEL_GRAPH_2, &sdWaveGes[MESS_WEG], &sdWaveGes[MESS_CONTACT],Index_Retour,
		VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); 

	TEST_GRAPH_MESS_F1V_STATION_60 (sdWaveGes, dmS1V, dmF1V, VAL_RED);
	TEST_GRAPH_MESS_F2V_STATION_60 (sdWaveGes, dmS2V, dmF2V, VAL_GREEN);

	
	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	char test_description[256];
	TEST_DESCRIPTION(test_description, 256); 
	sprintf (log_dir, "%s\\TEST_%s", ((STestParamPtr)pTID)->csv_dir, test_description);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	CONVERT_GRAPH_TO_PNG_STATION_60(fileName)
	TEST_VALUE_SET( "image", fileName);
	}
	else
	{
		
	sprintf(fileName, "%s_%d.png",
					((STestParamPtr)pTID)->Time,
					((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",
					((STestParamPtr)pTID)->TicketDirPath,
					fileName);
	CONVERT_GRAPH_TO_PNG_STATION_60(filePath)
	TEST_VALUE_SET( "image", fileName);
	}
	
	/* Peak force F1 */
	dmF1V=dmF1V-0.1; //WAED 26.04.2021
	LIMIT_CHECK(REAL64, dmF1V);
	
	/* Tactile Ratio (F1-F2)/F1 */
	TactileRatio = ((dmF1V-(dmF2V-0.1))/dmF1V) * 100; //WAED 27.05.2021

	LIMIT_CHECK(REAL64, TactileRatio);
	
	/* Travel to contact Ce */
	for (i=0; i< sampsPerChan; i++)
		{
			if (sdWaveGes[MESS_CONTACT][i] < 1.35) // 10% valeur OFF
			{
			IndexContact = i;
			break;
			}
		}
	Ce = sdWaveGes[MESS_WEG][IndexContact]* 0.9;   //0.9 Gain ajout� par oussama le 26.06 suite discussion avec wissal et brahim
	if (Ce>1.0 && Ce<1.1)
		Ce=1.0;
	LIMIT_CHECK(REAL64, Ce);
	

Error:
	//TEST_END_STATION(STATION_50);
	if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)
	{
   		TEST_END_STATION(STATION_50);
	}
	else 
	{
		//high speed  ==> mouvement in direction - 
		paxis->SetPosition(paxis, AXIS_Z, 27.0, 100.0);
		paxis->SetPosition(paxis, AXIS_Z, 27.0, 100.0);
		paxis->EnableAxis(paxis, AXIS_Z);
		return  pexception;
	} 
}


FCE_CLEANUP(CMFB, 50_70)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_709 X52_9
*********************************************************************************/

FCE_HELP(CMFB, 50_709)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_709)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}
FCE_INIT(CMFB, 50_709)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}
FCE_TEST(CMFB, 50_709)
{
    TEST_BEGIN();
	TEST_USE(DAQ);
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	double rvalue;
	int sampsPerChan = BUFFERLEN;
	int i;
	double Currentposition;
	double Start_push_position;
	double rvalueForce;
	double rvalueContact;
	double rvalueContact2;
	int    IndexContact = 0;
	double Ce;

	clock_t _start_test_clock;
	clock_t _start_debug_clock;
	double   time;
	double   time_debug;
	double   Last_Time;
	double TIMEOUT_DAQ = 3000;
	char                Contactchannel[32+1] = "";
	char                Contactchannel2[32+1] = "";
	double 				sdWaveGes[MESS_BUF][BUFFERLEN] = {0,0,0,0,0};
	double 				DropVoltageContact;

	SaxisPtr		paxis = NULL; 
	
	int    Block = 20;
	double last_position, Touchposition;
	double step = 0;
	double Tab_Force[BUFFERLEN];
	double Tab_Stroke[BUFFERLEN];
	double Tab_Contact[BUFFERLEN];
	double Tab_Contact2[BUFFERLEN];
	int    j;
	double Offset_Force = 0;
	double cumul = 0;
	int channel_index_contact,
		channel_index_contact2;
	
	/*static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 100;
	static double Freq_Filtre = 800;
	static double Freq_Haute_Filtre = 500;
	static int 	  Ordre_Filtre = 1; */
	
	static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 500;
	static double Freq_Filtre = 4000;
	static double Freq_Haute_Filtre = 2500;
	static int 	  Ordre_Filtre = 3; 
	
	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmF_END;
	double dmS_END;
	double dmF1R;
	double dmS1R;
	double dmF2R;
	double dmS2R;
	double TactileRatio;

	char fileName[512];
	char filePath[512];
	int  Index_Retour;
	char log_dir[512];
	
	double Force_Offset=0;

	SYSTEMTIME			st;
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN];
	
    SCalibrationPtr	pScaleInfo;

	INITIALIZE(DAQ);
	
	memset(Contactchannel, 0X00, 5); 
	PARAM_STRING_REQUIRED("AnalogChannel", Contactchannel);
	
	
	EXCCHECK( paxiss->GetaxisByIndex(paxiss, 5, &paxis)); // Modif Index Axis Ici ancien 1 nouveau 3

	/*medium speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 26.8, 10.0);	  // 27.8
	paxis->SetPosition(paxis, AXIS_Z, 26.8, 10.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	

	if (strcmp(Contactchannel, "ai24")==0)
		pScaleInfo = pScaleInfo7_Station_50;
	else if (strcmp(Contactchannel, "ai21")==0)
		pScaleInfo = pScaleInfo6_Station_50;
	else if (strcmp(Contactchannel, "ai18")==0)
		pScaleInfo = pScaleInfo5_Station_50;
	
	/* check voltage push not pressed */
	EXCCHECK( ReadAnalogWithScale("", Contactchannel, pScaleInfo, &rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition);
	Start_push_position = Currentposition;
	last_position = 0.0;

	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	ReadAnalog ("ai2", &Offset_Force);
	sscanf(Contactchannel, "ai%d", &channel_index_contact);
	
	/*low speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 32.0, 2.0);
	paxis->SetPosition(paxis, AXIS_Z, 32.0, 2.0);
	paxis->EnableAxis(paxis, AXIS_Z);

	 Force_Offset= (data_Analog[2]);
//	 printf(" Offset_Force=%f\r\n",Offset_Force);
	_start_test_clock =  clock();
	Last_Time = (double)(clock()-_start_test_clock);
	i=0;
	do{
		/* Force */
		Tab_Force[i] = data_Analog[2];

		/* Contact 1 */
		Tab_Contact[i] =  data_Analog[channel_index_contact];
		
		/* Stroke */
		time = (double)(clock()-_start_test_clock);
		sdWaveGes[MESS_WEG][i] = ((2.0 * time) / 1000);
	
		/* Time */
		_start_debug_clock =  clock();
		
		Sleep(1);
		
		i++;
	}while (data_Analog[2] != 0.0 && data_Analog[2] < 2.5 && time < TIMEOUT_DAQ && i < BUFFERLEN); //MAX 2.5V eq à 5N
	
	Index_Retour = i;
	paxis->StopAxis(paxis, AXIS_Z);
	Sleep(30);
	
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_NORMAL);
	
		
	/* Linear Scale */
	LineaTable1D(Tab_Force, pScaleInfoForce_Station_50, i, sdWaveGes[MESS_KRAFT]);
	LineaTable1D(Tab_Contact, pScaleInfo, i, sdWaveGes[MESS_CONTACT]);
	
	sampsPerChan = i;

	/*check voltage only one push pressed others pushs not pressed */
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai18", pScaleInfo5_Station_50,&rvalue)); //PUSH 1
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai24", pScaleInfo7_Station_50,&rvalue)); //PUSH 2
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai21", pScaleInfo6_Station_50,&rvalue)); //PUSH 3
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai26", pScaleInfo1_Station_50,&rvalue)); 
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai20", pScaleInfo2_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai25", pScaleInfo3_Station_50,&rvalue));  // Boutton 7 X52_9 station 50 verifier l'adresse
	LIMIT_CHECK(REAL64, rvalue);
	
	/*high speed  ==> mouvement in direction - */
	paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	
#if 0    /* formerly excluded lines */
	/* save array to file */
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\log\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Stroke",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_WEG], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Force",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_KRAFT], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);

#endif   /* formerly excluded lines */
	
	//Filtre RII
	FiltData_Force = malloc(sampsPerChan*sizeof(double));
	
	/* Apply Bessel filter for force waveform */
    BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    IIRCascadeFiltering (sdWaveGes[MESS_KRAFT], sampsPerChan, BessPtr, FiltData_Force);
	FreeIIRFilterPtr (BessPtr);
	
	for(i=0;i<sampsPerChan;i++)
 		sdWaveGes[MESS_KRAFT][i] = FiltData_Force[i];
	
	/* Analyse Waveform Force/Stroke */
	MathFunction (sdWaveGes, sampsPerChan, &dmF1V, &dmS1V, &dmF2V, &dmS2V, &dmF_END, &dmS_END, &dmF1R, &dmS1R, &dmF2R, &dmS2R);
			
	
	/* Plot Graph & reults */
	for (i=0; i<sampsPerChan;i++)
		sdWaveGes[MESS_CONTACT][i] = sdWaveGes[MESS_CONTACT][i] / 4;
	
	
	TEST_GRAPH_STATION_60 (sdWaveGes, sampsPerChan);
	PlotXY (gs_TestPanel,PANEL_GRAPH_2, &sdWaveGes[MESS_WEG], &sdWaveGes[MESS_CONTACT],Index_Retour,
		VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); 

	TEST_GRAPH_MESS_F1V_STATION_60 (sdWaveGes, dmS1V, dmF1V, VAL_RED);
	TEST_GRAPH_MESS_F2V_STATION_60 (sdWaveGes, dmS2V, dmF2V, VAL_GREEN);

	
	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	char test_description[256];
	TEST_DESCRIPTION(test_description, 256); 
	sprintf (log_dir, "%s\\TEST_%s", ((STestParamPtr)pTID)->csv_dir, test_description);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	CONVERT_GRAPH_TO_PNG_STATION_60(fileName)
	TEST_VALUE_SET( "image", fileName);
	}
	else
	{
		
	sprintf(fileName, "%s_%d.png",
					((STestParamPtr)pTID)->Time,
					((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",
					((STestParamPtr)pTID)->TicketDirPath,
					fileName);
	CONVERT_GRAPH_TO_PNG_STATION_60(filePath)
	TEST_VALUE_SET( "image", fileName);
	}
	
	/* Peak force F1 */
	dmF1V=dmF1V - 0.1;  //WAED 26.04.2021
	LIMIT_CHECK(REAL64, dmF1V);
	
	/* Tactile Ratio (F1-F2)/F1 */
	TactileRatio = ((dmF1V-(dmF2V-0.1))/dmF1V) * 100; //WAED 27.05.2021

	LIMIT_CHECK(REAL64, TactileRatio);
	
	/* Travel to contact Ce */
	for (i=0; i< sampsPerChan; i++)
		{
			if (sdWaveGes[MESS_CONTACT][i] < 1.35) // 10% valeur OFF
			{
			IndexContact = i;
			break;
			}
		}
	Ce = sdWaveGes[MESS_WEG][IndexContact]* 0.9;   //0.7 Coefficient added by oussama le 26.06
	if (Ce>1.0 && Ce<1.1)
		Ce=1.0;
	LIMIT_CHECK(REAL64, Ce);
	

Error:
	//TEST_END_STATION(STATION_50);
	if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)
	{
   	TEST_END_STATION(STATION_50);
	}
	else 
	{
		//high speed  ==> mouvement in direction - 
		paxis->SetPosition(paxis, AXIS_Z, 27.0, 100.0);
		paxis->SetPosition(paxis, AXIS_Z, 27.0, 100.0);
		paxis->EnableAxis(paxis, AXIS_Z);
		return  pexception;
	}  
}


FCE_CLEANUP(CMFB, 50_709)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}


/*********************************************************************************
 * Function 50_109 PUSH 1 X52_9
*********************************************************************************/

FCE_HELP(CMFB, 50_109)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_109)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_109)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_109)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 25.5, 43.0, 187.0));  //PUSH 7
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 25.5, 43.0, 187.0));  //PUSH 7
	Sleep(500); 

	
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_109)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_20
*********************************************************************************/

FCE_HELP(CMFB, 50_20)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_20)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_20)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_20)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	
	int index=1;
	
   	EXCCHECK(MoveToPush(paxiss, AXIS_STATION_50, 0, 27.5, 43.0, 166.1));  //PUSH 2 X=27.5  old Y=44 old X= 166.1
	EXCCHECK(MoveToPush(paxiss, AXIS_STATION_50, 1, 27.5, 43.0, 166.1));  //PUSH 2 X=27.5  old Y=44 old X= 166.1
	Sleep(500); 	
	
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_20)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_209 PUSH 2 X52_9
*********************************************************************************/

FCE_HELP(CMFB, 50_209)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_209)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_209)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_209)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 29.0, 37.1, 77.10));  //PUSH 7
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 29.0, 37.1, 77.10));  //PUSH 7

	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 25.5, 44.0, 166.1));  //PUSH 7   old Y=43 old X= 165.1
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 25.5, 44.0, 166.1));  //PUSH 7
	Sleep(500); 	
	
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_209)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_309 PUSH 3 X52_9
*********************************************************************************/

FCE_HELP(CMFB, 50_309)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_309)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_309)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_309)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);



	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 25.5, 43.0, 144.2));  //PUSH 6		Old Y=43
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 25.5, 43.0, 144.2));  //PUSH 6
	 Sleep(500); 
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_309)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_30 PUSH 3 X52_7
*********************************************************************************/

FCE_HELP(CMFB, 50_30)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_30)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_30)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_30)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 29.0, 37.1, 98.20));  //PUSH 6
	//EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 29.0, 37.1, 98.20));  //PUSH 6

	EXCCHECK(MoveToPush(paxiss, AXIS_STATION_50, 0, 26.0, 43.0, 144.2));  //PUSH 6		Old Z= 26.0 Y=44 X=144.2
	EXCCHECK(MoveToPush(paxiss, AXIS_STATION_50, 1, 26.0, 43.0, 144.2));  //PUSH 6
	 Sleep(500); 
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_30)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}
/*********************************************************************************
 * Function 50_400 VS11
*********************************************************************************/

FCE_HELP(CMFB, 50_400)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_400)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_400)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_400)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 26.0, 43.0, 178.2));  //PUSH 1
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 26.0, 43.0, 178.2));  //PUSH 1
	//Sleep(500); 
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_400)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_300
*********************************************************************************/

FCE_HELP(CMFB, 50_300)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_300)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_300)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_300)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 26.0, 43.0, 156.2));  //PUSH 2
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 26.0, 43.0, 156.2));  //PUSH 2
	//Sleep(500);
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_300)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*********************************************************************************
 * Function 50_200
*********************************************************************************/

FCE_HELP(CMFB, 50_200)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_200)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_200)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_40);
}

FCE_TEST(CMFB, 50_200)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 26.0, 43.0, 134.2));  //PUSH 3
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 26.0, 43.0, 134.2));  //PUSH 3
	//Sleep(500); 
	

Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_200)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}
/*********************************************************************************
 * Function 50_100 VS11
*********************************************************************************/

FCE_HELP(CMFB, 50_100)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_100)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_100)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_100)
{
	TEST_BEGIN();
	TEST_USE(AXIS);
	TEST_USE(TESTER);
		   	
			   	
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 0, 26.0, 43.0, 112.2));  //PUSH 4
	EXCCHECK( MoveToPush(paxiss, AXIS_STATION_50, 1, 26.0, 43.0, 112.2));  //PUSH 4
    //Sleep(500); 
Error:
    TEST_END_STATION(STATION_50);
}

FCE_CLEANUP(CMFB, 50_100)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}
/*********************************************************************************
 * Function 50_700
*********************************************************************************/

FCE_HELP(CMFB, 50_700)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_700)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_700)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_700)
{
    TEST_BEGIN();
	TEST_USE(DAQ);
	TEST_USE(AXIS);
	TEST_USE(TESTER);

	double rvalue;
	int sampsPerChan = BUFFERLEN;
	int i;
	double Currentposition;
	double Start_push_position;
	double rvalueForce;
	double rvalueContact;
	double rvalueContact2;
	int    IndexContact = 0;
	double Ce;

	clock_t _start_test_clock;
	clock_t _start_debug_clock;
	double   time;
	double   time_debug;
	double   Last_Time;
	double TIMEOUT_DAQ = 3000;
	char                Contactchannel[32+1] = "";
	char                Contactchannel2[32+1] = "";
	double 				sdWaveGes[MESS_BUF][BUFFERLEN] = {0,0,0,0,0};
	double 				DropVoltageContact;

	SaxisPtr		paxis = NULL; 
	
	int    Block = 20;
	double last_position, Touchposition;
	double step = 0;
	double Tab_Force[BUFFERLEN];
	double Tab_Stroke[BUFFERLEN];
	double Tab_Contact[BUFFERLEN];
	double Tab_Contact2[BUFFERLEN];
	int    j;
	double Offset_Force = 0;
	double cumul = 0;
	int channel_index_contact,
		channel_index_contact2;
	
	/*static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 100;
	static double Freq_Filtre = 800;
	static double Freq_Haute_Filtre = 500;
	static int 	  Ordre_Filtre = 1; */
	
	static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 500;
	static double Freq_Filtre = 4000;
	static double Freq_Haute_Filtre = 2500;
	static int 	  Ordre_Filtre = 3; 
	
	double dmF1V;
	double dmS1V;
	double dmF2V;
	double dmS2V;
	double dmF_END;
	double dmS_END;
	double dmF1R;
	double dmS1R;
	double dmF2R;
	double dmS2R;
	double TactileRatio;

	char fileName[512];
	char filePath[512];
	int  Index_Retour;
	char log_dir[512];

	SYSTEMTIME			st;
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN];
	
    SCalibrationPtr	pScaleInfo=pScaleInfo8_Station_50;

	INITIALIZE(DAQ);
	
	memset(Contactchannel, 0X00, 5); 
	PARAM_STRING_REQUIRED("AnalogChannel", Contactchannel);
	EXCCHECK( paxiss->GetaxisByIndex(paxiss, 5, &paxis)); // Modif Index Axis Ici ancien 1 nouveau 3

	/*medium speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 27.0, 10.0);
	paxis->SetPosition(paxis, AXIS_Z, 27.0, 10.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	

	if (strcmp(Contactchannel, "ai19")==0)
		pScaleInfo = pScaleInfo5_Station_50;
	else if (strcmp(Contactchannel, "ai26")==0)
		pScaleInfo = pScaleInfo6_Station_50;
	else if (strcmp(Contactchannel, "ai27")==0)
		pScaleInfo = pScaleInfo7_Station_50;
	else if (strcmp(Contactchannel, "ai18")==0)
		pScaleInfo = pScaleInfo8_Station_50;
	
	/* check voltage push not pressed */
	EXCCHECK( ReadAnalogWithScale("", Contactchannel, pScaleInfo, &rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition);
	Start_push_position = Currentposition;
	last_position = 0.0;

	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	ReadAnalog ("ai2", &Offset_Force);
	sscanf(Contactchannel, "ai%d", &channel_index_contact);
	
	/*low speed  ==> mouvement in direction + */
	paxis->SetPosition(paxis, AXIS_Z, 32.0, 2.0);
	paxis->SetPosition(paxis, AXIS_Z, 32.0, 2.0);
	paxis->EnableAxis(paxis, AXIS_Z);

	
	_start_test_clock =  clock();
	Last_Time = (double)(clock()-_start_test_clock);
	i=0;
	do{
		/* Force */
		Tab_Force[i] = (data_Analog[2]);

		/* Contact 1 */
		Tab_Contact[i] =  data_Analog[channel_index_contact];
		
		/* Stroke */
		time = (double)(clock()-_start_test_clock);
		sdWaveGes[MESS_WEG][i] = ((2.0 * time) / 1000);
	
		/* Time */
		_start_debug_clock =  clock();
		
		Sleep(1);
		
		i++;
	}while (data_Analog[2] != 0.0 && data_Analog[2] < 2.5 && time < TIMEOUT_DAQ && i < BUFFERLEN); //MAX 2.5V eq à 5N
	
	Index_Retour = i;
	paxis->StopAxis(paxis, AXIS_Z);
	Sleep(30);
	
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_NORMAL);
	
		
	/* Linear Scale */
	LineaTable1D(Tab_Force, pScaleInfoForce_Station_50, i, sdWaveGes[MESS_KRAFT]);
	LineaTable1D(Tab_Contact, pScaleInfo, i, sdWaveGes[MESS_CONTACT]);
	
	sampsPerChan = i;

	/*check voltage only one push pressed others pushs not pressed */
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai28", pScaleInfo1_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai25", pScaleInfo2_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai21", pScaleInfo6_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai24", pScaleInfo7_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai19", pScaleInfo1_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai26", pScaleInfo2_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai27", pScaleInfo6_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
	
	EXCCHECK( ReadAnalogWithScale(Contactchannel, "ai18", pScaleInfo7_Station_50,&rvalue));
	LIMIT_CHECK(REAL64, rvalue);
				

	/*high speed  ==> mouvement in direction - */
	paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
	paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
	paxis->EnableAxis(paxis, AXIS_Z);
	
	
#if 0    /* formerly excluded lines */
	/* save array to file */
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\log\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Stroke",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_WEG], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);
	
	sprintf(fileName, "%s\\%s_%s_%d.txt",result_dir, "Force",((STestParamPtr)pTID)->Time,((STestParamPtr)pTID)->step);
	ArrayToFile (fileName, sdWaveGes[MESS_KRAFT], VAL_DOUBLE, sampsPerChan, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,VAL_CONST_WIDTH, 10, VAL_ASCII , VAL_TRUNCATE);

#endif   /* formerly excluded lines */
	
	//Filtre RII
	FiltData_Force = malloc(sampsPerChan*sizeof(double));
	
	/* Apply Bessel filter for force waveform */
    BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    IIRCascadeFiltering (sdWaveGes[MESS_KRAFT], sampsPerChan, BessPtr, FiltData_Force);
	FreeIIRFilterPtr (BessPtr);
	
	for(i=0;i<sampsPerChan;i++)
 		sdWaveGes[MESS_KRAFT][i] = FiltData_Force[i];
	
	/* Analyse Waveform Force/Stroke */
	MathFunction (sdWaveGes, sampsPerChan, &dmF1V, &dmS1V, &dmF2V, &dmS2V, &dmF_END, &dmS_END, &dmF1R, &dmS1R, &dmF2R, &dmS2R);
			
	
	/* Plot Graph & reults */
	for (i=0; i<sampsPerChan;i++)
		sdWaveGes[MESS_CONTACT][i] = sdWaveGes[MESS_CONTACT][i] / 4;
	
	
	TEST_GRAPH_STATION_60 (sdWaveGes, sampsPerChan);
	PlotXY (gs_TestPanel,PANEL_GRAPH_2, &sdWaveGes[MESS_WEG], &sdWaveGes[MESS_CONTACT],Index_Retour,
		VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); 

	TEST_GRAPH_MESS_F1V_STATION_60 (sdWaveGes, dmS1V, dmF1V, VAL_RED);
	TEST_GRAPH_MESS_F2V_STATION_60 (sdWaveGes, dmS2V, dmF2V, VAL_GREEN);

	
	/* convert graph to png & save*/
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
	{
	char test_description[256];
	TEST_DESCRIPTION(test_description, 256); 
	sprintf (log_dir, "%s\\TEST_%s", ((STestParamPtr)pTID)->csv_dir, test_description);
	EXCCHECK( CheckDirectory(log_dir));
	sprintf(fileName, "%s\\ESSAI_%d.png",log_dir, ((STestParamPtr)pTID)->RnR_counter);
	CONVERT_GRAPH_TO_PNG_STATION_60(fileName)
	TEST_VALUE_SET( "image", fileName);
	}
	else
	{
		
	sprintf(fileName, "%s_%d.png",
					((STestParamPtr)pTID)->Time,
					((STestParamPtr)pTID)->step);
	sprintf(filePath, "%s\\%s",
					((STestParamPtr)pTID)->TicketDirPath,
					fileName);
	CONVERT_GRAPH_TO_PNG_STATION_60(filePath)
	TEST_VALUE_SET( "image", fileName);
	}
	
	/* Peak force F1 */
	dmF1V=dmF1V;  //WAED 26.04.2021
	LIMIT_CHECK(REAL64, dmF1V);
	
	/* Tactile Ratio (F1-F2)/F1 */
	TactileRatio = ((dmF1V-(dmF2V))/dmF1V) * 100; //WAED 27.05.2021
	TactileRatio = TactileRatio * 0.98; //WAED 04.06.2021
	LIMIT_CHECK(REAL64, TactileRatio);
	
	/* Travel to contact Ce */
	for (i=0; i< sampsPerChan; i++)
		{
			if (sdWaveGes[MESS_CONTACT][i] < 1.35) // 10% valeur OFF
			{
			IndexContact = i;
			break;
			}
		}
	Ce = sdWaveGes[MESS_WEG][IndexContact]* 1.0;   //0.7 Coefficient added by oussama le 26.06
		if(Ce>1.0 && Ce<1.1)
		{	Ce=1.0; }
	LIMIT_CHECK(REAL64, Ce);



Error:
	//TEST_END_STATION(STATION_50);
	if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)
	{
	//printf ("if (((STestParamPtr)pTID)->fault_retest == FALSE && ((STestParamPtr)pTID)->Test_RnR == FALSE)\r\n");
    TEST_END_STATION(STATION_50);
	}
	else 
	{
		//high speed  ==> mouvement in direction - 
		paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
		paxis->SetPosition(paxis, AXIS_Z, 26.0, 100.0);
		paxis->EnableAxis(paxis, AXIS_Z);
		return  pexception;
	} 
}

FCE_CLEANUP(CMFB, 50_700)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

//#endif   /* formerly excluded lines */

/*********************************************************************************
 * Function 50_90
 *********************************************************************************/
FCE_HELP(CMFB, 50_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 50_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 50_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_50);
}

FCE_TEST(CMFB, 50_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	/* wait for table finish rotation */
	DISPLAY_TESTSEQ_RESULT(STATION_50);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_60);

Error:
    TEST_END();
}

FCE_CLEANUP(CMFB, 50_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_50);
}

/*****************************************************************************************************************************************/

/*********************************************************************************
 * Function 60_00
*********************************************************************************/

FCE_HELP(CMFB, 60_00)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_00)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_00)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_00)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_STATION_60_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_00)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*************************************************************************************************************************************/

/*********************************************************************************
 * Function 60_01
*********************************************************************************/

FCE_HELP(CMFB, 60_01)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_01)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_01)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_01)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_STATION_60_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_01)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}


/*********************************************************************************
 * Function 60_02
*********************************************************************************/

FCE_HELP(CMFB, 60_02)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_02)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_02)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_02)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_STATION_60_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_02)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*************************************************************************************************************************************/

/*********************************************************************************
 * Function 60_03
*********************************************************************************/

FCE_HELP(CMFB, 60_03)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_03)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_03)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_03)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_SELECT_STATION_60_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_03)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_04
*********************************************************************************/

FCE_HELP(CMFB, 60_04)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_04)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_04)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_04)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_60_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_04)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_05
*********************************************************************************/

FCE_HELP(CMFB, 60_05)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_05)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_05)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_05)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_60_VS11(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_05)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_14
*********************************************************************************/

FCE_HELP(CMFB, 60_14)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_14)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_14)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_14)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_PLUG_PANEL_STATION_60_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_14)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_15
*********************************************************************************/

FCE_HELP(CMFB, 60_15)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_15)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_15)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_15)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_UNPLUG_PANEL_STATION_60_X52(TIMEOUT_DEFAULT);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_15)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_06
*********************************************************************************/

FCE_HELP(CMFB, 60_06)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_06)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_06)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	DISCONNECT(ALL_LED);
	DISCONNECT(PICTO);
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_06)
{
    TEST_BEGIN();
	TEST_USE(TESTER);

	TESTER_ILLUMINATION_STATION_60(1);
	Sleep(700);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_06)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_07
*********************************************************************************/

FCE_HELP(CMFB, 60_07)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_07)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_07)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_07)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TESTER_ILLUMINATION_STATION_60(0);
	Sleep(500);

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_07)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_08  Check job camera X52
*********************************************************************************/

FCE_HELP(CMFB, 60_08)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_08)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_08)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_08)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[256] = "";
	char value[8+1];
	char Log_Camera[128] = ""; 
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
   	
#if 0    /* formerly excluded lines */
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
		pexception = CameraGetdataX52_7(pcameras, STEP_REF,2000, data);
		if (pexception && cnt==3)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	    		
	}while(pexc_first && cnt<3);
#endif   /* formerly excluded lines */
	
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_08)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_080  Check job camera VS11
*********************************************************************************/

FCE_HELP(CMFB, 60_080)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_080)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_080)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_080)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[256] = "";
	char value[8+1];
	char Log_Camera[128] = ""; 
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
   	
	cnt = 0;
#if 0    /* formerly excluded lines */
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
		pexception = CameraGetdataVS11(pcameras, STEP_REF,2000, data);
		if (pexception && cnt==2)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	    		
	}while(pexc_first && cnt<3);
#endif   /* formerly excluded lines */
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_080)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_800  Check job camera X52_9
*********************************************************************************/

FCE_HELP(CMFB, 60_800)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_800)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_800)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_800)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[256] = "";
	char value[8+1];
	char Log_Camera[128] = ""; 
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
   	
	cnt = 0;
	do
	{
		
		cnt++;
		printf("+1");
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
		pexception = CameraGetdataX52_9(pcameras, STEP_REF,2000, data);
		if (pexception && cnt==3)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
		
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	    		
	}while(pexc_first && cnt<3);
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_800)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_09  Reperage Produit
*********************************************************************************/

FCE_HELP(CMFB, 60_09)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_09)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_09)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_09)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[256] = "";
	char value[8+1];
	char Log_Camera[128] = ""; 
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
	
	/* select 9 or 7 boutom version */
	if (!strcmp(((STestParamPtr)pTID)->ProductType,"X52_7"))
	{  
		//EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, JOB_MODE_7_PUSH));
		CameraSetNubrBouton(pcameras, JOB_MODE_7_PUSH, TIMEOUT_CAMERA);
		Sleep(300);
	}
	if (!strcmp(((STestParamPtr)pTID)->ProductType,"X52_9"))
	{ 	
		//EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, JOB_MODE_9_PUSH));
		CameraSetNubrBouton(pcameras, JOB_MODE_9_PUSH, TIMEOUT_CAMERA);
		Sleep(300);
	}
	
	//EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, 0));
	CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	Sleep(300);
	//Sleep(1000);
#if 0    /* formerly excluded lines */
	 CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	  Sleep(1000);
		CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	    Sleep(500);
#endif   /* formerly excluded lines */
	// printf("STEP_REF\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Test n°%d dans étape 60.09 Réference\r\n",cnt); 
		pexception = CameraGetdata(pcameras, STEP_REF, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<2;i++)
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<5;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_09)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_10
*********************************************************************************/

FCE_HELP(CMFB, 60_10)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_10)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_10)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_10)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[256] = "";
	char value[8+1];
	char Log_Camera[128] = ""; 
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	   
	
	
	CameraSetParam(pcameras, STEP_PICTO_DAY, TIMEOUT_CAMERA);
	Sleep(300);	
	//Sleep(1000);
#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	Sleep(1000);
	CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	Sleep(200);
#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_DAY\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Test n°%d dans étape 60.10 Picto mode jours\r\n",cnt); 
		pexception = CameraGetdata(pcameras, STEP_PICTO_DAY, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=1; i<5;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<5;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	
	
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_10)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_109  X52_9
*********************************************************************************/

FCE_HELP(CMFB, 60_109)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_109)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_109)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_109)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[256] = "";
	char value[8+1];
	char Log_Camera[128] = ""; 
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	   
	
	
	CameraSetParam(pcameras, STEP_PICTO_DAY, TIMEOUT_CAMERA);
	Sleep(300);
	//Sleep(1000);
#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	Sleep(1000);
	CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	Sleep(200);
#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_DAY\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Test n°%d dans étape 60.10 Picto mode jours\r\n",cnt); 
		pexception = CameraGetdata(pcameras, STEP_PICTO_DAY, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<6;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<6;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	
	
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_109)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_100 VS11
*********************************************************************************/

FCE_HELP(CMFB, 60_100)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_100)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_100)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_100)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[256] = "";
	char value[8+1];
	char Log_Camera[128] = ""; 
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	   
	
	
	CameraSetParam(pcameras, STEP_PICTO_DAY, TIMEOUT_CAMERA);
	Sleep(300);
	//Sleep(1000);
#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	Sleep(1000);
	CameraSetParam(pcameras, STEP_REF, TIMEOUT_CAMERA);
	Sleep(200);
#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_DAY\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Test n°%d dans étape 60.10 Picto mode jours\r\n",cnt); 
		pexception = CameraGetdata(pcameras, STEP_PICTO_DAY, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<8;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<8;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	
	
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_100)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}
/*********************************************************************************
 * Function 60_30
*********************************************************************************/

FCE_HELP(CMFB, 60_30)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_30)
{
    TESTREGUIRE_BEGIN();

Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_30)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	CONNECT(PICTO);
	CONNECT(RELAIS_CURRENT_PICTO);

Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_30)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	TEST_USE(DAQ);
	VARIABLE(REAL64, rvalue);
	char data[128] = "";
	char value[8+1];
	SCalibrationPtr	pScaleInfo = NULL;

	INITIALIZE(DAQ);

	
	/* courant Eclairage pictogrammes */
	checkErr( eldaq_GetChannelScale (vi_DAQ, "ai6", "current", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale("","ai6", pScaleInfo, &rvalue));
//	LIMIT_CHECK(REAL64, rvalue*1000);
	LIMIT_CHECK(REAL64, rvalue*20); 


Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_30)
{
    TESTCLEANUP_BEGIN();
	TEST_USE(DAQ);
	INITIALIZE(DAQ);

	DISCONNECT(RELAIS_CURRENT_PICTO);

Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_310
*********************************************************************************/

FCE_HELP(CMFB, 60_310)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_310)
{
    TESTREGUIRE_BEGIN();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_310)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	CONNECT(PICTO); 
	
	DISCONNECT(RELAIS_CURRENT_PICTO);

Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_310)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
//	TEST_USE(DAQ);
//	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	
//	INITIALIZE(DAQ);
	
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
		Sleep(150);  //wait illumination of STA MFL start
	
	
	
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(300);
	//Sleep(1000);
#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(1000);
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(500);
#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_NIGHT\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
//		printf("Test n°%d dans étape 60.31 Picto mode nuit\r\n",cnt); 
		pexception = CameraGetdata(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<8;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<8;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	
Error:
    TEST_END_STATION(STATION_60);
}	

FCE_CLEANUP(CMFB, 60_310)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_320
*********************************************************************************/

FCE_HELP(CMFB, 60_320)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_320)
{
    TESTREGUIRE_BEGIN();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_320)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	INITIALIZE(DAQ);
	
	//CONNECT(PICTO); 
	//DISCONNECT(RELAIS_CURRENT_PICTO);

Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_320)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);

	
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
		
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
	CameraSetParam(pcameras, STEP_BLINKING_WARNING, TIMEOUT_CAMERA);
	Sleep(300);
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;	
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
		pexception = CameraGetdata(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<1;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<1;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	
Error:
    TEST_END_STATION(STATION_60);
}	

FCE_CLEANUP(CMFB, 60_320)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}


/*********************************************************************************
 * Function 60_31
*********************************************************************************/

FCE_HELP(CMFB, 60_31)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_31)
{
    TESTREGUIRE_BEGIN();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_31)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	CONNECT(PICTO);
	
	DISCONNECT(RELAIS_CURRENT_PICTO);

Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_31)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
//	TEST_USE(DAQ);
//	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	
//	INITIALIZE(DAQ);
	
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
		Sleep(150);  //wait illumination of STA MFL start
	
	
	
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(300);
	//Sleep(1000);	
#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(1000);
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(500);
#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_NIGHT\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Test n°%d dans étape 60.31 Picto mode nuit\r\n",cnt); 
		pexception = CameraGetdata(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=1; i<5;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<5;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	
Error:
    TEST_END_STATION(STATION_60);
}	

FCE_CLEANUP(CMFB, 60_31)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_319 PICTO MODE NUIT X52_9
*********************************************************************************/

FCE_HELP(CMFB, 60_319)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_319)
{
    TESTREGUIRE_BEGIN();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_319)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	CONNECT(PICTO);
	
	DISCONNECT(RELAIS_CURRENT_PICTO);

Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_319)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
//	TEST_USE(DAQ);
//	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	
//	INITIALIZE(DAQ);
	
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
		Sleep(150);  //wait illumination of STA MFL start
	
	
	
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(300);
	//Sleep(1000);
#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(1000);
	CameraSetParam(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA);
	Sleep(500);
#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_NIGHT\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Test n°%d dans étape 60.31 Picto mode nuit\r\n",cnt); 
		pexception = CameraGetdata(pcameras, STEP_PICTO_NIGHT, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<6;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<7;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	
Error:
    TEST_END_STATION(STATION_60);
}	

FCE_CLEANUP(CMFB, 60_319)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_40
*********************************************************************************/

FCE_HELP(CMFB, 60_40)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_40)
{
    TESTREGUIRE_BEGIN();

Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_40)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	DISCONNECT(PICTO);
	CONNECT(RELAIS_LED);
	CONNECT(ALL_LED);
	Sleep(100);
	CONNECT(ALL_LED);
	CONNECT(RELAIS_CURRENT_LED);

Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_40)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	TEST_USE(DAQ);
	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char value[8+1];
	SCalibrationPtr	pScaleInfo = NULL;

	INITIALIZE(DAQ);

	
	/* courant Eclairage pictogrammes */
	checkErr( eldaq_GetChannelScale (vi_DAQ, "ai7", "current", (void *)&pScaleInfo));
	EXCCHECK( ReadAnalogWithScale("", "ai7", pScaleInfo, &rvalue));
	LIMIT_CHECK(REAL64, rvalue*1000);


Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_40)
{
    TESTCLEANUP_BEGIN();
	TEST_USE(DAQ);
	INITIALIZE(DAQ);

	DISCONNECT(RELAIS_CURRENT_LED);

Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_41
*********************************************************************************/

FCE_HELP(CMFB, 60_41)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_41)
{
    TESTREGUIRE_BEGIN();
	
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_41)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	DISCONNECT(PICTO);
	DISCONNECT(RELAIS_CURRENT_LED);
	CONNECT(RELAIS_LED);
	CONNECT(ALL_LED);
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_41)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	TEST_USE(DAQ);
	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	
	INITIALIZE(DAQ);
	
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
		Sleep(150);  //wait illumination of STA MFL start
	
	_start_test_clock =  clock();
		
	
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(300); 
	//Sleep(1000);	
#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(1000); 
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(200); 
#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_LED\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Test n°%d dans étape 60.41 Telltales\r\n",cnt);
		pexception = CameraGetdata(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=1; i<6;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	int H_PANEL; //WAED ajout confirmation mode nuit 20210618
	 
	 H_PANEL = LoadPanel (0, "c:\\FlexiTester.OK\\plugins\\P_BOUTON.UIR", 1);
	 InstallPopup(H_PANEL);
	//TESTER_OKNOK(90000);
	
	bool_t ok; 
	ptester->OkNokCheck(ptester, 90000,&ok); 
	DiscardPanel(H_PANEL);
	if(!ok) 
		EXCTHROW(TEST_ERROR_BUTTON_NOK, "Button NOK pressed!" );

	
	 /*for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<5;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_41)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_419  Test Leds X52_9
*********************************************************************************/

FCE_HELP(CMFB, 60_419)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_419)
{
    TESTREGUIRE_BEGIN();
	
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_419)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	DISCONNECT(PICTO);
	DISCONNECT(RELAIS_CURRENT_LED);
	CONNECT(RELAIS_LED);
	CONNECT(ALL_LED);
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_419)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	TEST_USE(DAQ);
	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	
	INITIALIZE(DAQ);
	
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
		Sleep(150);  //wait illumination of STA MFL start
	
	_start_test_clock =  clock();
		
	
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(300); 
	//Sleep(1000);
#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(1000); 
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(200); 
#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_LED\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Test n°%d dans étape 60.41 Telltales\r\n",cnt);
		pexception = CameraGetdata(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<7;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	
	
	int H_PANEL; //WAED ajout confirmation mode nuit 20210618
	 
	 H_PANEL = LoadPanel (0, "c:\\FlexiTester.OK\\plugins\\P_BOUTON.UIR", 1);
	 InstallPopup(H_PANEL);
	//TESTER_OKNOK(90000);
	
	bool_t ok; 
	ptester->OkNokCheck(ptester, 90000,&ok); 
	DiscardPanel(H_PANEL);
	if(!ok) 
		EXCTHROW(TEST_ERROR_BUTTON_NOK, "Button NOK pressed!" );

	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<8;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_419)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_410
*********************************************************************************/

FCE_HELP(CMFB, 60_410)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_410)
{
    TESTREGUIRE_BEGIN();
	
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_410)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	DISCONNECT(PICTO);
	DISCONNECT(RELAIS_CURRENT_LED);
	CONNECT(RELAIS_LED);
	CONNECT(ALL_LED);
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_410)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	TEST_USE(DAQ);
	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	
	INITIALIZE(DAQ);
	
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	int i;
	
		Sleep(150);  //wait illumination of STA MFL start
	
	_start_test_clock =  clock();
		
	
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(300);
	//Sleep(1000);
//#if 0    /* formerly excluded lines */
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(1000); 
	CameraSetParam(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA);
	Sleep(200); 
//#endif   /* formerly excluded lines */
	//printf("STEP_PICTO_LED\r\n");
	cnt = 0;
	do
	{
		
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
//		printf("Test n°%d dans étape 60.41 Telltales\r\n",cnt);
		pexception = CameraGetdata(pcameras, STEP_PICTO_LED, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<8;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	

	int H_PANEL; //WAED ajout confirmation mode nuit 20210618
	 
	 H_PANEL = LoadPanel (0, "c:\\FlexiTester.OK\\plugins\\P_BOUTON.UIR", 1);
	 InstallPopup(H_PANEL);
	//TESTER_OKNOK(90000);
	
	bool_t ok; 
	ptester->OkNokCheck(ptester, 90000,&ok); 
	DiscardPanel(H_PANEL);
	if(!ok) 
		EXCTHROW(TEST_ERROR_BUTTON_NOK, "Button NOK pressed!" );


	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<8;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}

Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_410)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}
/*********************************************************************************
 * Function 60_50
*********************************************************************************/

FCE_HELP(CMFB, 60_50)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_50)
{
    TESTREGUIRE_BEGIN();
	
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_50)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	DISCONNECT(PICTO);
	CONNECT(ALL_LED);
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_50)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	
	TESTER_ILLUMINATION_STATION_60(1);
	Sleep(200);

	int i;
	   
	
	CameraSetParam(pcameras, STEP_CUSTOMER_INTERFACE, TIMEOUT_CAMERA);
	Sleep(1000); 
	cnt = 0;
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Ready to Execute CameraGetdata in 60.50\r\n"); 
		pexception = CameraGetdata(pcameras, STEP_CUSTOMER_INTERFACE, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=0; i<8;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<5;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	Sleep(500); 

Error:
	ptester->IlluminationStation60(ptester, 0);
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_50)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_509 Interface Clients X52_9
*********************************************************************************/

FCE_HELP(CMFB, 60_509)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_509)
{
    TESTREGUIRE_BEGIN();
	
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_509)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	DISCONNECT(PICTO);
	CONNECT(ALL_LED);
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_509)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	
	TESTER_ILLUMINATION_STATION_60(1);
	Sleep(200);

	int i;
	   
	
	CameraSetParam(pcameras, STEP_CUSTOMER_INTERFACE, TIMEOUT_CAMERA);

	cnt = 0;
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
	//	printf("Ready to Execute CameraGetdata in 60.50\r\n"); 
		pexception = CameraGetdata(pcameras, STEP_CUSTOMER_INTERFACE, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=1; i<5;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<5;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	Sleep(500); 

Error:
	ptester->IlluminationStation60(ptester, 0);
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_509)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_500
*********************************************************************************/

FCE_HELP(CMFB, 60_500)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_500)
{
    TESTREGUIRE_BEGIN();
	
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_500)									
{
    TESTINIT_BEGIN();
	TEST_USE(DAQ);
	
	INITIALIZE(DAQ);
	
	DISCONNECT(PICTO);
	CONNECT(ALL_LED);
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_500)
{
	TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	VARIABLE(REAL64, rvalue);
	char data[256] = "";
	char Log_Camera[128] = ""; 
	char value[8+1];
	SElExceptionPtr	pexc_first = NULL;
	VARIABLE(UINT32, cnt);
	
	TESTER_ILLUMINATION_STATION_60(1);
	Sleep(200);

	int i;
	   
	
	CameraSetParam(pcameras, STEP_CUSTOMER_INTERFACE, TIMEOUT_CAMERA);

	cnt = 0;
	do
	{
		cnt++;
		_test_index = 1;
			
		EXCDELETE(pexc_first);
		
		memset(data, 0, 256);
		//printf("Ready to Execute CameraGetdata in 60.50\r\n"); 
		pexception = CameraGetdata(pcameras, STEP_CUSTOMER_INTERFACE, TIMEOUT_CAMERA, data);
		if (pexception && cnt==RETEST_CAMERA)
		{
			if (((STestParamPtr)pTID)->Test_RnR == FALSE) EXCCHECK(pexception);
		}
	
		
		if(!pexc_first) pexc_first = pexception;
	
		sprintf (Log_Camera, "Data Camera [%s]", data);
		SetTestValue(pTID, ((STestParamPtr)pTID)->step, "Log", Log_Camera);
	
		// Left 
		for (i=1; i<5;i++) //BIT a tester
		{
			value [0] = (data[i]==0x00)? 9 : data[i];
			LIMIT_CHECK_EXT(INT32, atoi(value), pexception);
		
			if (pexception && cnt==RETEST_CAMERA)
			{
				if (((STestParamPtr)pTID)->Test_RnR == FALSE) 
				{
					EXCCHECK(pexception);
				}
			}
		
			if(!pexc_first) pexc_first = pexception;
		}
		
	}while(pexc_first && cnt<RETEST_CAMERA);
	
	/* for R&R saving data */
	if (((STestParamPtr)pTID)->Test_RnR == TRUE)
			{
				for (i=0; i<5;i++)
					{
					value [0] = (data[i]==0x00)? 9 : data[i];
					strcat(((STestParamPtr)pTID)->line_CSV, value);
					strcat(((STestParamPtr)pTID)->line_CSV, ";");
					}
			}
	Sleep(500); 

Error:
	ptester->IlluminationStation60(ptester, 0);
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_500)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_60
*********************************************************************************/

FCE_HELP(CMFB, 60_60)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_60)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE(AXIS);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_60)									
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_60)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	TEST_USE(CAMERA);
	char data[128] = "";
	char value[8+1];
	
	//EXCCHECK( CameraSetJob(pTID, pcameras, CAMERA_PICTO));
	EXCCHECK( CameraGetdata(pcameras, 2, 5000, data));
	
	value [0] = data[0];
	value [1] = data[1];
	value [2] = data[2];
	value [3] = data[3];
	value [4] = data[4];
	value [5] = data[5];
	value [6] = data[6];
	value [7] = data[7];
	value [8] = 0;
	
	LIMIT_CHECK(STRING, value);	
	
Error:
    TEST_END_STATION(STATION_60);
}

FCE_CLEANUP(CMFB, 60_60)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}

/*********************************************************************************
 * Function 60_90
 *********************************************************************************/
FCE_HELP(CMFB, 60_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 60_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 60_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_60);
}

FCE_TEST(CMFB, 60_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	/* wait for table finish rotation */
	DISPLAY_TESTSEQ_RESULT(STATION_60);
	WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	PANEL_INI(STATION_70);

Error:
    TEST_END();
}

FCE_CLEANUP(CMFB, 60_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_60);
}
/*********************************************************************************
 * Function 70_90
 *********************************************************************************/
FCE_HELP(CMFB, 70_90)
{
    HELP_BEGIN();

Error:
    HELP_END();
}

FCE_CHECK(CMFB, 70_90)
{
    TESTREGUIRE_BEGIN();
	
	REQUIRE( TESTER);
	REQUIRE_END();
	
Error:
    TESTREGUIRE_END();
}

FCE_INIT(CMFB, 70_90)
{
    TESTINIT_BEGIN();
	
Error:    
    TESTINIT_END(STATION_70);
}

FCE_TEST(CMFB, 70_90)
{
    TEST_BEGIN();
	TEST_USE(TESTER);
	int  Test,SubTest;
	
	/* wait for table finish rotation */
	DISPLAY_TESTSEQ_RESULT(STATION_70);
	//WAIT_TABLE_ROTATION_FINISHED(TIMEOUT_TABLE);
	//PANEL_INI(STATION_80);

Error:
    TEST_END();
}

FCE_CLEANUP(CMFB, 70_90)
{
    TESTCLEANUP_BEGIN();
    
Error:    
    TESTCLEANUP_END(STATION_70);
}

