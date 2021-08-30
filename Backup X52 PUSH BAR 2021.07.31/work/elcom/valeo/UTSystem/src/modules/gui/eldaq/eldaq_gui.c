#include <userint.h>
#include "eldaq_gui.h"
#include <IviBridge/IviBridge.h>  
#include <eventfactory/EventFactory.h> 
#include <eldaq/class/eldaq.h>
#include <utility.h>
#include <guid.h>
#include <msxmldom.h>
#include "eldaq_plugin.h" 
#include <analysis.h>
#include <formatio.h>
#include <rs232.h>
#include <ansi_c.h>
#include "lang_translation.h"
#include <IviDCPwr.h>
#include <IviSwtch.h>
#include <IviDmm.h>
#include <DAQmxIOctrl.h>
#include <NIDAQmx.h>
#include <export.h>

#define NUMBER_OF_PANELS 	3

static int plotColors[12] = {VAL_RED, VAL_GREEN, VAL_BLUE, VAL_CYAN, 
VAL_MAGENTA, VAL_YELLOW, VAL_DK_RED, VAL_DK_BLUE, VAL_DK_GREEN, 
VAL_DK_CYAN, VAL_DK_MAGENTA, VAL_DK_YELLOW};

static enum _EMode
{
	EM_ANALOG = 0,
	EM_DIGITAL,
	EM_GRAPH,
} EMode; 

static int					gs_panel[NUMBER_OF_PANELS+1];
SLanguagePtr 				gs_pLanguage;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL;  
static SIviBridgePtr		gs_IviBridge = NULL;
static ViSession			gs_DAQ = 1;
static ViSession			gs_DAQ_2 = 1; 
static ViSession			gs_DMM = 0;
static CAObjHandle			gs_pxml = 0;
static char					gs_pxml_file[512];

static CAObjHandle			gs_pxml2 = 0;
static char					gs_pxml_file2[512];

typedef struct _SCalibration
{
	int		calType;
	void*	pCalibData;
} SCalibration, *SCalibrationPtr;

/* error handling */
#define EXCCHECKIVIDAQ( fCall) \
	EXCCHECKIVI(eldaq, gs_DAQ, fCall)

#define EXCCHECKIVIDMM( fCall) \
	EXCCHECKIVI(IviDmm, gs_DMM, fCall)

#define MAX_NB_AI_CHANNELS 32
#define MAX_NB_AI_CHANNELS_2 16
#define MAX_NB_DI_P0_CHANNELS 32
#define MAX_NB_DI_P1_CHANNELS 8
#define MAX_NB_DI_P2_CHANNELS 8


static int gs_Digital_Port_0[] = 
{
  ELDAQ_2_SWITCH_P0_0,  
  ELDAQ_2_SWITCH_P0_1,
  ELDAQ_2_SWITCH_P0_2,
  ELDAQ_2_SWITCH_P0_3,
  ELDAQ_2_SWITCH_P0_4,
  ELDAQ_2_SWITCH_P0_5,
  ELDAQ_2_SWITCH_P0_6,
  ELDAQ_2_SWITCH_P0_7,
  ELDAQ_2_SWITCH_P0_8,
  ELDAQ_2_SWITCH_P0_9,
  ELDAQ_2_SWITCH_P0_10,
  ELDAQ_2_SWITCH_P0_11,
  ELDAQ_2_SWITCH_P0_12,
  ELDAQ_2_SWITCH_P0_13,
  ELDAQ_2_SWITCH_P0_14,
  ELDAQ_2_SWITCH_P0_15,
  ELDAQ_2_SWITCH_P0_16,
  ELDAQ_2_SWITCH_P0_17,
  ELDAQ_2_SWITCH_P0_18,
  ELDAQ_2_SWITCH_P0_19,
  ELDAQ_2_SWITCH_P0_20,
  ELDAQ_2_SWITCH_P0_21,
  ELDAQ_2_SWITCH_P0_22,
  ELDAQ_2_SWITCH_P0_23,
  ELDAQ_2_SWITCH_P0_24,
  ELDAQ_2_SWITCH_P0_25,
  ELDAQ_2_SWITCH_P0_26,
  ELDAQ_2_SWITCH_P0_27,
  ELDAQ_2_SWITCH_P0_28,
  ELDAQ_2_SWITCH_P0_29,
  ELDAQ_2_SWITCH_P0_30,
  ELDAQ_2_SWITCH_P0_31
};

static int gs_Digital_Port_1[] = 
{
  ELDAQ_2_SWITCH_P1_0,  
  ELDAQ_2_SWITCH_P1_1,
  ELDAQ_2_SWITCH_P1_2,
  ELDAQ_2_SWITCH_P1_3,
  ELDAQ_2_SWITCH_P1_4,
  ELDAQ_2_SWITCH_P1_5,
  ELDAQ_2_SWITCH_P1_6,
  ELDAQ_2_SWITCH_P1_7
};
static int gs_Digital_Port_2[] = 
{
  ELDAQ_2_SWITCH_P2_0,  
  ELDAQ_2_SWITCH_P2_1,
  ELDAQ_2_SWITCH_P2_2,
  ELDAQ_2_SWITCH_P2_3,
  ELDAQ_2_SWITCH_P2_4,
  ELDAQ_2_SWITCH_P2_5,
  ELDAQ_2_SWITCH_P2_6,
  ELDAQ_2_SWITCH_P2_7
};

static int gs_ChannelStates[] = 
{
  ELDAQ_ENABLED_AI0,
  ELDAQ_ENABLED_AI1,
  ELDAQ_ENABLED_AI2,
  ELDAQ_ENABLED_AI3,
  ELDAQ_ENABLED_AI4,
  ELDAQ_ENABLED_AI5,
  ELDAQ_ENABLED_AI6,
  ELDAQ_ENABLED_AI7,
  ELDAQ_ENABLED_AI8,
  ELDAQ_ENABLED_AI9,
  ELDAQ_ENABLED_AI10,
  ELDAQ_ENABLED_AI11,
  ELDAQ_ENABLED_AI12,
  ELDAQ_ENABLED_AI13,
  ELDAQ_ENABLED_AI14,
  ELDAQ_ENABLED_AI15,
  ELDAQ_ENABLED_AI16,
  ELDAQ_ENABLED_AI17,
  ELDAQ_ENABLED_AI18,
  ELDAQ_ENABLED_AI19,
  ELDAQ_ENABLED_AI20,
  ELDAQ_ENABLED_AI21,
  ELDAQ_ENABLED_AI22,
  ELDAQ_ENABLED_AI23,
  ELDAQ_ENABLED_AI24,
  ELDAQ_ENABLED_AI25,
  ELDAQ_ENABLED_AI26,
  ELDAQ_ENABLED_AI27,
  ELDAQ_ENABLED_AI28,
  ELDAQ_ENABLED_AI29,
  ELDAQ_ENABLED_AI30,
  ELDAQ_ENABLED_AI31
};
	
static int gs_MeasIndicators[] = 
{ ELDAQ_AI0,
  ELDAQ_AI1,
  ELDAQ_AI2,
  ELDAQ_AI3,
  ELDAQ_AI4,
  ELDAQ_AI5,
  ELDAQ_AI6,
  ELDAQ_AI7,
  ELDAQ_AI8,
  ELDAQ_AI9,
  ELDAQ_AI10,
  ELDAQ_AI11,
  ELDAQ_AI12,
  ELDAQ_AI13,
  ELDAQ_AI14,
  ELDAQ_AI15,
  ELDAQ_AI16,
  ELDAQ_AI17,
  ELDAQ_AI18,
  ELDAQ_AI19,
  ELDAQ_AI20,
  ELDAQ_AI21,
  ELDAQ_AI22,
  ELDAQ_AI23,
  ELDAQ_AI24,
  ELDAQ_AI25,
  ELDAQ_AI26,
  ELDAQ_AI27,
  ELDAQ_AI28,
  ELDAQ_AI29,
  ELDAQ_AI30,
  ELDAQ_AI31
};
  
static int gs_ScaleControls[] = {
  ELDAQ_SCALE_AI0,
  ELDAQ_SCALE_AI1,
  ELDAQ_SCALE_AI2,
  ELDAQ_SCALE_AI3,
  ELDAQ_SCALE_AI4,
  ELDAQ_SCALE_AI5,
  ELDAQ_SCALE_AI6,
  ELDAQ_SCALE_AI7,
  ELDAQ_SCALE_AI8,
  ELDAQ_SCALE_AI9,
  ELDAQ_SCALE_AI10,
  ELDAQ_SCALE_AI11,
  ELDAQ_SCALE_AI12,
  ELDAQ_SCALE_AI13,
  ELDAQ_SCALE_AI14,
  ELDAQ_SCALE_AI15,
  ELDAQ_SCALE_AI16,
  ELDAQ_SCALE_AI17,
  ELDAQ_SCALE_AI18,
  ELDAQ_SCALE_AI19,
  ELDAQ_SCALE_AI20,
  ELDAQ_SCALE_AI21,
  ELDAQ_SCALE_AI22,
  ELDAQ_SCALE_AI23,
  ELDAQ_SCALE_AI24,
  ELDAQ_SCALE_AI25,
  ELDAQ_SCALE_AI26,
  ELDAQ_SCALE_AI27,
  ELDAQ_SCALE_AI28,
  ELDAQ_SCALE_AI29,
  ELDAQ_SCALE_AI30,
  ELDAQ_SCALE_AI31
}; 

static int gs_CalibActual[] = {
  ELDAQ_CALIB_AI_0,
  ELDAQ_CALIB_AI_1,
  ELDAQ_CALIB_AI_2,
  ELDAQ_CALIB_AI_3,
  ELDAQ_CALIB_AI_4,
  ELDAQ_CALIB_AI_5,
  ELDAQ_CALIB_AI_6,
  ELDAQ_CALIB_AI_7,
  ELDAQ_CALIB_AI_8,
  ELDAQ_CALIB_AI_9,
  ELDAQ_CALIB_AI_10,
  ELDAQ_CALIB_AI_11,
  ELDAQ_CALIB_AI_12,
  ELDAQ_CALIB_AI_13,
  ELDAQ_CALIB_AI_14,
  ELDAQ_CALIB_AI_15,
  ELDAQ_CALIB_AI_16,
  ELDAQ_CALIB_AI_17,
  ELDAQ_CALIB_AI_18,
  ELDAQ_CALIB_AI_19,
  ELDAQ_CALIB_AI_20,
  ELDAQ_CALIB_AI_21,
  ELDAQ_CALIB_AI_22,
  ELDAQ_CALIB_AI_23,
  ELDAQ_CALIB_AI_24,
  ELDAQ_CALIB_AI_25,
  ELDAQ_CALIB_AI_26,
  ELDAQ_CALIB_AI_27,
  ELDAQ_CALIB_AI_28,
  ELDAQ_CALIB_AI_29,
  ELDAQ_CALIB_AI_30,
  ELDAQ_CALIB_AI_31
}; 

static int gs_ChannelStates_2[] = 
{
  ELDAQ_3_ENABLED_AI0,
  ELDAQ_3_ENABLED_AI1,
  ELDAQ_3_ENABLED_AI2,
  ELDAQ_3_ENABLED_AI3,
  ELDAQ_3_ENABLED_AI4,
  ELDAQ_3_ENABLED_AI5,
  ELDAQ_3_ENABLED_AI6,
  ELDAQ_3_ENABLED_AI7,
  ELDAQ_3_ENABLED_AI8,
  ELDAQ_3_ENABLED_AI9,
  ELDAQ_3_ENABLED_AI10,
  ELDAQ_3_ENABLED_AI11,
  ELDAQ_3_ENABLED_AI12,
  ELDAQ_3_ENABLED_AI13,
  ELDAQ_3_ENABLED_AI14,
  ELDAQ_3_ENABLED_AI15
};
	
static int gs_MeasIndicators_2[] = 
{ ELDAQ_3_AI0,
  ELDAQ_3_AI1,
  ELDAQ_3_AI2,
  ELDAQ_3_AI3,
  ELDAQ_3_AI4,
  ELDAQ_3_AI5,
  ELDAQ_3_AI6,
  ELDAQ_3_AI7,
  ELDAQ_3_AI8,
  ELDAQ_3_AI9,
  ELDAQ_3_AI10,
  ELDAQ_3_AI11,
  ELDAQ_3_AI12,
  ELDAQ_3_AI13,
  ELDAQ_3_AI14,
  ELDAQ_3_AI15
};
  
static int gs_ScaleControls_2[] = {
  ELDAQ_3_SCALE_AI0,
  ELDAQ_3_SCALE_AI1,
  ELDAQ_3_SCALE_AI2,
  ELDAQ_3_SCALE_AI3,
  ELDAQ_3_SCALE_AI4,
  ELDAQ_3_SCALE_AI5,
  ELDAQ_3_SCALE_AI6,
  ELDAQ_3_SCALE_AI7,
  ELDAQ_3_SCALE_AI8,
  ELDAQ_3_SCALE_AI9,
  ELDAQ_3_SCALE_AI10,
  ELDAQ_3_SCALE_AI11,
  ELDAQ_3_SCALE_AI12,
  ELDAQ_3_SCALE_AI13,
  ELDAQ_3_SCALE_AI14,
  ELDAQ_3_SCALE_AI15
}; 

static int gs_CalibActual_2[] = {
  ELDAQ_3_CALIB_AI_0,
  ELDAQ_3_CALIB_AI_1,
  ELDAQ_3_CALIB_AI_2,
  ELDAQ_3_CALIB_AI_3,
  ELDAQ_3_CALIB_AI_4,
  ELDAQ_3_CALIB_AI_5,
  ELDAQ_3_CALIB_AI_6,
  ELDAQ_3_CALIB_AI_7,
  ELDAQ_3_CALIB_AI_8,
  ELDAQ_3_CALIB_AI_9,
  ELDAQ_3_CALIB_AI_10,
  ELDAQ_3_CALIB_AI_11,
  ELDAQ_3_CALIB_AI_12,
  ELDAQ_3_CALIB_AI_13,
  ELDAQ_3_CALIB_AI_14,
  ELDAQ_3_CALIB_AI_15
}; 


#define   SAMPLES 1
SCalibrationPtr	pScaleInfo[MAX_NB_AI_CHANNELS];
enum {
	CAL_TYPE_LINEAR = 1
};

typedef struct _SCalibrationPoint
{
	double	measValue;
	double	realValue;
} SCalibrationPoint, *SCalibrationPointPtr;

typedef struct _SLinearCalibration
{
	int						count;
	SCalibrationPointPtr	pPoints;	
} SLinearCalibration, *SLinearCalibrationPtr;


static SElExceptionPtr EnabeControls( int panel, int state);
static SElExceptionPtr FillScale( int panel );
static SElExceptionPtr EnabeCalib( int panel, int state);

static SElExceptionPtr EnabeControls2( int panel, int state);
static SElExceptionPtr FillScale2( int panel );
static SElExceptionPtr EnabeCalib2( int panel, int state);


/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalogWithScale"
SElExceptionPtr ReadAnalogWithScale(int channel_index, SCalibrationPtr pScaleInfo, double *Value)
{
SElExceptionPtr pexception = NULL;
double pChannelData[2];
float64*	pdata;

		pdata = data_Analog + channel_index;

		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			/*double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k);
			LinEv1D( pdata, 1, k, q,  pChannelData); */
			
			double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			LinEv1D( pdata, 1, a, b,  pChannelData);
			
			*Value = pChannelData[0]; 
		}
		else 
			*Value = data_Analog[channel_index];
	//	printf("data_analog= %f\r\n", data_Analog[channel_index]);  
Error:
	EXCRETHROW( pexception);
	
}

/*---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalogWithScale2"
SElExceptionPtr ReadAnalogWithScale2(int channel_index, SCalibrationPtr pScaleInfo, double *Value)
{
SElExceptionPtr pexception = NULL;
double pChannelData[2];
float64*	pdata;

		pdata = data_Analog_2 + channel_index;

		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			/*double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k);
			LinEv1D( pdata, 1, k, q,  pChannelData); */
			
			double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			LinEv1D( pdata, 1, a, b,  pChannelData);
			
			*Value = pChannelData[0]; 
		}
		else 
			*Value = data_Analog_2[channel_index];
	//	printf("data_analog_2= %f\r\n", data_Analog_2[channel_index]);   
Error:
	EXCRETHROW( pexception);
	
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Measure_Mean"
static SElExceptionPtr	Measure_Mean( int ichannel, ViReal64 *moyenne)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
   	ViReal64		rvalue;
	ViReal64		somme = 0;

	for ( i = 0; i < SAMPLES; i++)
	{
		EXCCHECK( ReadAnalogWithScale(ichannel, pScaleInfo[ichannel],&rvalue));
		somme = somme + rvalue;
	}
	*moyenne = somme /i;
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Measure_Mean2"
static SElExceptionPtr	Measure_Mean2( int ichannel, ViReal64 *moyenne)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
   	ViReal64		rvalue;
	ViReal64		somme = 0;

	for ( i = 0; i < SAMPLES; i++)
	{
		EXCCHECK( ReadAnalogWithScale2(ichannel, pScaleInfo[ichannel],&rvalue));
		somme = somme + rvalue;
	}
	*moyenne = somme /i;
	
Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EnabeControls"
static SElExceptionPtr	EnabeControls( int panel, int state)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
	
	for ( i = 0; i < MAX_NB_AI_CHANNELS; i++)
	{
		EXCCHECKCVI( SetCtrlAttribute( panel, gs_MeasIndicators[i], ATTR_DIMMED, state));
	}
	
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_SAMPLE_RATE, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_MEAS_INTERVAL, ATTR_DIMMED, !state));

	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_START, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_STOP, ATTR_DIMMED, state));
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/

#undef __FUNC__
#define __FUNC__ "EnabeControls2"
static SElExceptionPtr	EnabeControls2( int panel, int state)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
	
	for ( i = 0; i < MAX_NB_AI_CHANNELS_2; i++)
	{
		EXCCHECKCVI( SetCtrlAttribute( panel, gs_MeasIndicators_2[i], ATTR_DIMMED, state));
	}
	
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_SAMPLE_RATE, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_MEAS_INTERVAL, ATTR_DIMMED, !state));

	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_START, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_STOP, ATTR_DIMMED, state));
	
Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EnabeCalib"
static SElExceptionPtr	EnabeCalib( int panel, int state)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
	
	for ( i = 0; i < MAX_NB_AI_CHANNELS; i++)
	{
		EXCCHECKCVI( SetCtrlAttribute( panel, gs_CalibActual[i], ATTR_DIMMED, !state));
	}

	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_SAVE, ATTR_DIMMED, !state));   
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_SCALE, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_MSG_CALIB_CHANNEL, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_CALIB_X1, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_CALIB_X2, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_CALIB_Y1, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_CALIB_Y2, ATTR_DIMMED, !state));

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EnabeCalib2"
static SElExceptionPtr	EnabeCalib2( int panel, int state)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
	
	for ( i = 0; i < MAX_NB_AI_CHANNELS_2; i++)
	{
		EXCCHECKCVI( SetCtrlAttribute( panel, gs_CalibActual_2[i], ATTR_DIMMED, !state));
	}

	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_SAVE, ATTR_DIMMED, !state));   
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_SCALE, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_MSG_CALIB_CHANNEL, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_CALIB_X1, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_CALIB_X2, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_CALIB_Y1, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_CALIB_Y2, ATTR_DIMMED, !state));

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "FillScale"
static SElExceptionPtr FillScale( int panel )
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
	
	for ( i = 0; i < MAX_NB_AI_CHANNELS; i++)
	{
		EXCCHECKCVI( ClearListCtrl( panel, gs_ScaleControls[i]));
		EXCCHECKCVI( InsertListItem( panel, gs_ScaleControls[i], 0, "None", 0));
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "FillScale2"
static SElExceptionPtr FillScale2( int panel )
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
	
	for ( i = 0; i < MAX_NB_AI_CHANNELS_2; i++)
	{
		EXCCHECKCVI( ClearListCtrl( panel, gs_ScaleControls_2[i]));
		EXCCHECKCVI( InsertListItem( panel, gs_ScaleControls_2[i], 0, "None", 0));
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadCalibData"
static SElExceptionPtr ReadCalibData( const char* channel, const char* scale)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	HRESULT         hr = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     pnode2old = 0;
	CAObjHandle     pparameter = 0;
	VBOOL           is_xml_success = 1;
	char*           ptext = NULL;
	char			path[256];

	if( gs_pxml)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (gs_pxml, NULL, "//scales", &pnode));
		if(pnode)
		{
			sprintf( path, "channel[@name='%s']/scale[@name='%s']/calibration[@type='linear']/measurement", channel, scale);
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pnode2));
			if(pnode2)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "value", &pparameter));
				if ( pparameter )
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_X1, strtod(ptext, NULL)));
					
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					CA_FreeMemory(ptext);
					ptext=NULL;
				}
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "real", &pparameter));
				if ( pparameter )
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_Y1, strtod(ptext, NULL)));
					
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					CA_FreeMemory(ptext);
					ptext=NULL;
				}

				pnode2old = pnode2;
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2));
				EXCCHECKCVI( CA_DiscardObjHandle(pnode2old));
				pnode2old = 0; 
				if(pnode2)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "value", &pparameter));
					if ( pparameter )
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_X2, strtod(ptext, NULL)));
						
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
						CA_FreeMemory(ptext);
						ptext=NULL;
					}
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "real", &pparameter));
					if ( pparameter )
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_Y2, strtod(ptext, NULL)));
						
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
						CA_FreeMemory(ptext);
						ptext=NULL;
					}
				}
			}
			else
			{
				EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ], ELDAQ_CALIB_Y1, 0.0));
				EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ], ELDAQ_CALIB_X1, 0.0));
				EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ], ELDAQ_CALIB_Y2, 0.0));
				EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ], ELDAQ_CALIB_X2, 0.0));
			}
		}
	}
	else
	{
		EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ], ELDAQ_CALIB_Y2, 0.0));
		EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ], ELDAQ_CALIB_X2, 0.0));
		EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ], ELDAQ_CALIB_Y1, 0.0));
		EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ], ELDAQ_CALIB_X1, 0.0));
	}

Error:
	if (pnode2old) CA_DiscardObjHandle(pnode2old);
	if (pnode2) CA_DiscardObjHandle(pnode2);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadCalibData2"
static SElExceptionPtr ReadCalibData2( const char* channel, const char* scale)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	HRESULT         hr = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     pnode2old = 0;
	CAObjHandle     pparameter = 0;
	VBOOL           is_xml_success = 1;
	char*           ptext = NULL;
	char			path[256];

	if( gs_pxml)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (gs_pxml, NULL, "//scales", &pnode));
		if(pnode)
		{
			sprintf( path, "channel[@name='%s']/scale[@name='%s']/calibration[@type='linear']/measurement", channel, scale);
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pnode2));
			if(pnode2)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "value", &pparameter));
				if ( pparameter )
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X1, strtod(ptext, NULL)));
					
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					CA_FreeMemory(ptext);
					ptext=NULL;
				}
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "real", &pparameter));
				if ( pparameter )
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y1, strtod(ptext, NULL)));
					
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					CA_FreeMemory(ptext);
					ptext=NULL;
				}

				pnode2old = pnode2;
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2));
				EXCCHECKCVI( CA_DiscardObjHandle(pnode2old));
				pnode2old = 0; 
				if(pnode2)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "value", &pparameter));
					if ( pparameter )
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X2, strtod(ptext, NULL)));
						
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
						CA_FreeMemory(ptext);
						ptext=NULL;
					}
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "real", &pparameter));
					if ( pparameter )
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y2, strtod(ptext, NULL)));
						
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
						CA_FreeMemory(ptext);
						ptext=NULL;
					}
				}
			}
			else
			{
				EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y1, 0.0));
				EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X1, 0.0));
				EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y2, 0.0));
				EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X2, 0.0));
			}
		}
	}
	else
	{
		EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y2, 0.0));
		EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X2, 0.0));
		EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y1, 0.0));
		EXCCHECKCVI( SetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X1, 0.0));
	}

Error:
	if (pnode2old) CA_DiscardObjHandle(pnode2old);
	if (pnode2) CA_DiscardObjHandle(pnode2);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SaveCalibData"
static SElExceptionPtr SaveCalibData( const char* channel, const char* scale, const char* description)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	HRESULT         hr = 0;
	CAObjHandle		pxml = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     pnode3 = 0;
	CAObjHandle     pnode3old = 0;
	CAObjHandle     pnode_new = 0;
	CAObjHandle     pnode_old = 0;
	CAObjHandle     pparameter = 0;
	VBOOL           is_xml_success = 1;
	char*           ptext = NULL;
	char			path[256];
	VARIANT			var_value = CA_VariantEmpty (),
					var_type = CA_VariantEmpty (),
					pfile_name_var = CA_VariantEmpty ();
	ERRORINFO		errorInfo;
	double			x1, x2, y1, y2;

	if( gs_pxml)
	{
		EXCCHECKCVI( GetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_X1, &x1));
		EXCCHECKCVI( GetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_X2, &x2));
		EXCCHECKCVI( GetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_Y1, &y1));
		EXCCHECKCVI( GetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_Y2, &y2));

		sprintf( path, "//scales/channel[@name='%s']", channel);
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (gs_pxml, NULL, path, &pnode));
		if(pnode)
		{
			sprintf( path, "scale[@name='%s']", scale);
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pnode2));
			if( pnode2) /* rewrite old */
			{
				char	value[64];

				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "calibration[@type='linear']/measurement", &pnode3));
				if ( pnode3 )
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode3, NULL, "value", &pparameter));
					if ( pparameter )
					{
						sprintf(value, "%5.5f", x1);
						EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value));
						
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode3, NULL, "real", &pparameter));
					if ( pparameter )
					{
						sprintf(value, "%5.5f", y1);
						EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value));
						
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					pnode3old = pnode3;
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pnode3, NULL, &pnode3));
					EXCCHECKCVI( CA_DiscardObjHandle(pnode3old));
					pnode3old = 0; 
					if ( pnode3 )
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode3, NULL, "value", &pparameter));
						if ( pparameter )
						{
							sprintf(value, "%5.5f", x2);
							EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value));
							
							EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
							pparameter = 0;
						}
						EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode3, NULL, "real", &pparameter));
						if ( pparameter )
						{
							sprintf(value, "%5.5f", y2);
							EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value));
							
							EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
							pparameter = 0;
						}
					}
				}
			}
			else /* create new */
			{
				char	snode[1024];

				EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
				sprintf( snode, "<scale name='%s' description='%s'>\n"
								"	<calibration type='linear'>\n"
								"		<measurement>\n"
								"			<value>%5.5f</value>\n"
								"			<real>%5.5f</real>\n"
								"		</measurement>\n"
								"		<measurement>\n"
								"			<value>%5.5f</value>\n"
								"			<real>%5.5f</real>\n"
								"		</measurement>\n"
								"	</calibration>\n"
								"</scale>\n",
								scale,
								description,
								x1, y1, x2, y2);

				EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (pxml, &errorInfo, snode, &is_xml_success));
				EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, &errorInfo, "//scale", &pnode_new));

				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, pnode_new, &pnode_old)); \
				EXCCHECKCVI( CA_DiscardObjHandle( pnode_old)); \
				pnode_old = 0;
			}
		}
	}

Error:
	if (pxml) CA_DiscardObjHandle(pxml);
	if (pnode_old) CA_DiscardObjHandle(pnode_old);
	if (pnode_new) CA_DiscardObjHandle(pnode_new);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SaveCalibData2"
static SElExceptionPtr SaveCalibData2( const char* channel, const char* scale, const char* description)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	HRESULT         hr = 0;
	CAObjHandle		pxml = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     pnode3 = 0;
	CAObjHandle     pnode3old = 0;
	CAObjHandle     pnode_new = 0;
	CAObjHandle     pnode_old = 0;
	CAObjHandle     pparameter = 0;
	VBOOL           is_xml_success = 1;
	char*           ptext = NULL;
	char			path[256];
	VARIANT			var_value2 = CA_VariantEmpty (),
					var_type2 = CA_VariantEmpty (),
					pfile_name_var2 = CA_VariantEmpty ();
	ERRORINFO		errorInfo;
	double			x1, x2, y1, y2;

	if( gs_pxml2)
	{
		EXCCHECKCVI( GetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X1, &x1));
		EXCCHECKCVI( GetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X2, &x2));
		EXCCHECKCVI( GetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y1, &y1));
		EXCCHECKCVI( GetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y2, &y2));

		sprintf( path, "//scales/channel[@name='%s']", channel);
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (gs_pxml2, NULL, path, &pnode));
		if(pnode)
		{
			sprintf( path, "scale[@name='%s']", scale);
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pnode2));
			if( pnode2) /* rewrite old */
			{
				char	value[64];

				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "calibration[@type='linear']/measurement", &pnode3));
				if ( pnode3 )
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode3, NULL, "value", &pparameter));
					if ( pparameter )
					{
						sprintf(value, "%5.5f", x1);
						EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value));
						
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode3, NULL, "real", &pparameter));
					if ( pparameter )
					{
						sprintf(value, "%5.5f", y1);
						EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value));
						
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					pnode3old = pnode3;
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pnode3, NULL, &pnode3));
					EXCCHECKCVI( CA_DiscardObjHandle(pnode3old));
					pnode3old = 0; 
					if ( pnode3 )
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode3, NULL, "value", &pparameter));
						if ( pparameter )
						{
							sprintf(value, "%5.5f", x2);
							EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value));
							
							EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
							pparameter = 0;
						}
						EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode3, NULL, "real", &pparameter));
						if ( pparameter )
						{
							sprintf(value, "%5.5f", y2);
							EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value));
							
							EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
							pparameter = 0;
						}
					}
				}
			}
			else /* create new */
			{
				char	snode[1024];

				EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
				sprintf( snode, "<scale name='%s' description='%s'>\n"
								"	<calibration type='linear'>\n"
								"		<measurement>\n"
								"			<value>%5.5f</value>\n"
								"			<real>%5.5f</real>\n"
								"		</measurement>\n"
								"		<measurement>\n"
								"			<value>%5.5f</value>\n"
								"			<real>%5.5f</real>\n"
								"		</measurement>\n"
								"	</calibration>\n"
								"</scale>\n",
								scale,
								description,
								x1, y1, x2, y2);

				EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (pxml, &errorInfo, snode, &is_xml_success));
				EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, &errorInfo, "//scale", &pnode_new));

				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, pnode_new, &pnode_old)); \
				EXCCHECKCVI( CA_DiscardObjHandle( pnode_old)); \
				pnode_old = 0;
			}
		}
	}

Error:
	if (pxml) CA_DiscardObjHandle(pxml);
	if (pnode_old) CA_DiscardObjHandle(pnode_old);
	if (pnode_new) CA_DiscardObjHandle(pnode_new);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSave"
int CVICALLBACK OnSave (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			i;
	char			channel[256], scale[256], description[256];

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, ELDAQ_MSG_CALIB_CHANNEL, channel));	

			EXCCHECKCVI( GetCtrlIndex (panel, ELDAQ_SCALE, &i));
			if( i)
			{
				EXCCHECKCVI( GetValueFromIndex (panel, ELDAQ_SCALE, i, scale));
				EXCCHECKCVI( GetLabelFromIndex (panel, ELDAQ_SCALE, i, description));

				if( scale[0]!='\0' && description[0]!='\0')
					EXCCHECK( SaveCalibData( channel, scale, description));
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSave2"
int CVICALLBACK OnSave2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			i;
	char			channel[256], scale[256], description[256];

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, ELDAQ_3_MSG_CALIB_CHANNEL, channel));	

			EXCCHECKCVI( GetCtrlIndex (panel, ELDAQ_3_SCALE, &i));
			if( i)
			{
				EXCCHECKCVI( GetValueFromIndex (panel, ELDAQ_3_SCALE, i, scale));
				EXCCHECKCVI( GetLabelFromIndex (panel, ELDAQ_3_SCALE, i, description));

				if( scale[0]!='\0' && description[0]!='\0')
					EXCCHECK( SaveCalibData2( channel, scale, description));
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPrecision"
int CVICALLBACK OnPrecision (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			i, precision;

	switch (event)
	{
		case EVENT_VAL_CHANGED:
			
			EXCCHECKCVI( GetCtrlVal(gs_panel[ELDAQ], ELDAQ_PRECISION, &precision));
			for(i=0; i<MAX_NB_AI_CHANNELS; i++)
			{
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ], gs_MeasIndicators[i], ATTR_PRECISION, precision));
			}
			//EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ], ELDAQ_DMM, ATTR_PRECISION, precision));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ], ELDAQ_CALIB_X1, ATTR_PRECISION, precision));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ], ELDAQ_CALIB_X2, ATTR_PRECISION, precision));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ], ELDAQ_CALIB_Y1, ATTR_PRECISION, precision));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ], ELDAQ_CALIB_Y2, ATTR_PRECISION, precision));
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPrecision2"
int CVICALLBACK OnPrecision2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			i, precision;

	switch (event)
	{
		case EVENT_VAL_CHANGED:
			
			EXCCHECKCVI( GetCtrlVal(gs_panel[ELDAQ_3], ELDAQ_3_PRECISION, &precision));
			for(i=0; i<MAX_NB_AI_CHANNELS; i++)
			{
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ_3], gs_MeasIndicators_2[i], ATTR_PRECISION, precision));
			}
			//EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ], ELDAQ_DMM, ATTR_PRECISION, precision));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X1, ATTR_PRECISION, precision));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_X2, ATTR_PRECISION, precision));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y1, ATTR_PRECISION, precision));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[ELDAQ_3], ELDAQ_3_CALIB_Y2, ATTR_PRECISION, precision));
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSaveFile"
int CVICALLBACK OnSaveFile (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	HRESULT         hr = 0;
	VARIANT			pfile_name_var = CA_VariantEmpty ();
	int				state;

	switch (event)
	{
		case EVENT_COMMIT:
			
			/* stop eldaq if is running */
			EXCCHECKCVI( GetCtrlAttribute( panel, ELDAQ_TIMER, ATTR_ENABLED, &state));
			if(state)
			{
				OnStop(panel, control, EVENT_COMMIT, NULL, 0, 0);
			}

			if( gs_pxml_file[0]!='\0')
			{
				EXCCHECKCVI( CA_VariantSetCString( &pfile_name_var, gs_pxml_file));
				EXCCHECKMSXML( MSXML_IXMLDOMDocumentsave (gs_pxml, NULL, pfile_name_var));

				/* reload xml */
				EXCCHECKIVIDAQ( eldaq_LoadCalibration(gs_DAQ, gs_pxml_file));
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSaveFile2"
int CVICALLBACK OnSaveFile2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	HRESULT         hr = 0;
	VARIANT			pfile_name_var2 = CA_VariantEmpty ();
	int				state;

	switch (event)
	{
		case EVENT_COMMIT:
			
			/* stop eldaq if is running */
			EXCCHECKCVI( GetCtrlAttribute( panel, ELDAQ_3_TIMER, ATTR_ENABLED, &state));
			if(state)
			{
				OnStop2(panel, control, EVENT_COMMIT, NULL, 0, 0);
			}

			if( gs_pxml_file2[0]!='\0')
			{
				EXCCHECKCVI( CA_VariantSetCString( &pfile_name_var2, gs_pxml_file2));
				EXCCHECKMSXML( MSXML_IXMLDOMDocumentsave (gs_pxml2, NULL, pfile_name_var2));

				/* reload xml */
				EXCCHECKIVIDAQ( eldaq_LoadCalibration(gs_DAQ_2, gs_pxml_file2));
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
void CVICALLBACK PostDeferredDisplayPanel (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		DisplayPanel(*panel);
}

/*---------------------------------------------------------------------------*/
void CVICALLBACK PostDeferredHidePanel (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		HidePanel(*panel);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ChangeMode"
static SElExceptionPtr ChangeMode (int32_t mode)
{
	SElExceptionPtr 	pexception = NULL;  
	int32_t				error = 0;
	static enum _EMode	last_mode;
	
	switch(last_mode)
	{
		case EM_ANALOG:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[ELDAQ_2]));
			break;
		case EM_DIGITAL:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[ELDAQ]));   
			break;
		case EM_GRAPH:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[ELDAQ_3]));   
			break;
	}
	
	switch(mode)
	{
		case EM_ANALOG:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[ELDAQ]));   
			break;
		case EM_DIGITAL:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[ELDAQ_2]));
			break;
		case EM_GRAPH:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[ELDAQ_3]));   
			break;
	}
	
	last_mode = mode;

Error:      
   EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMode"
int CVICALLBACK OnMode (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL; 
	int32_t				error = 0;
	int					mode;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, control, &mode));
			EXCCHECK( ChangeMode( mode));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);   
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnCalib"
int CVICALLBACK OnCalib (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			i, index = -1;

	switch (event)
	{
		case EVENT_COMMIT:
			for( i=0; i<MAX_NB_AI_CHANNELS; i++)
			{
				if( gs_CalibActual[i]!=control)
				{
					EXCCHECKCVI( SetCtrlVal(panel, gs_CalibActual[i], 0));
				}
				else
				{
					EXCCHECKCVI( SetCtrlVal(panel, gs_CalibActual[i], 1));
					index = i;
				}
			}
			if( index>=0)
			{
				char	channel[32], scale[32];
				int32_t	i;

				sprintf( channel, "ai%d", index);
				EXCCHECKCVI( SetCtrlVal(panel, ELDAQ_MSG_CALIB_CHANNEL, channel));	

				EXCCHECKCVI( GetCtrlIndex (panel, ELDAQ_SCALE, &i));
				if( i)
				{
					EXCCHECKCVI( GetValueFromIndex (panel, ELDAQ_SCALE, i, scale));

					if( scale[0]!='\0')
						EXCCHECK( ReadCalibData( channel, scale));
				}
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnCalib2"
int CVICALLBACK OnCalib2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			i, index = -1;

	switch (event)
	{
		case EVENT_COMMIT:
			for( i=0; i<MAX_NB_AI_CHANNELS_2; i++)
			{
				if( gs_CalibActual_2[i]!=control)
				{
					EXCCHECKCVI( SetCtrlVal(panel, gs_CalibActual_2[i], 0));
				}
				else
				{
					EXCCHECKCVI( SetCtrlVal(panel, gs_CalibActual_2[i], 1));
					index = i;
				}
			}
			if( index>=0)
			{
				char	channel[32], scale[32];
				int32_t	i;

				sprintf( channel, "ai%d", index);
				EXCCHECKCVI( SetCtrlVal(panel, ELDAQ_3_MSG_CALIB_CHANNEL, channel));	

				EXCCHECKCVI( GetCtrlIndex (panel, ELDAQ_3_SCALE, &i));
				if( i)
				{
					EXCCHECKCVI( GetValueFromIndex (panel, ELDAQ_3_SCALE, i, scale));

					if( scale[0]!='\0')
						EXCCHECK( ReadCalibData( channel, scale));
				}
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnCalibScale"
int CVICALLBACK OnCalibScale (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	char			channel[256], scale[256];
	int32_t			i;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, ELDAQ_MSG_CALIB_CHANNEL, channel));	

			EXCCHECKCVI( GetCtrlIndex (panel, ELDAQ_SCALE, &i));
			if( i)
			{
				EXCCHECKCVI( GetValueFromIndex (panel, ELDAQ_SCALE, i, scale));

				if( scale[0]!='\0')
					EXCCHECK( ReadCalibData( channel, scale));
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnCalibScale2"
int CVICALLBACK OnCalibScale2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	char			channel[256], scale[256];
	int32_t			i;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, ELDAQ_3_MSG_CALIB_CHANNEL, channel));	

			EXCCHECKCVI( GetCtrlIndex (panel, ELDAQ_3_SCALE, &i));
			if( i)
			{
				EXCCHECKCVI( GetValueFromIndex (panel, ELDAQ_3_SCALE, i, scale));

				if( scale[0]!='\0')
					EXCCHECK( ReadCalibData( channel, scale));
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnScale"
int CVICALLBACK OnScale (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
   SElExceptionPtr	pexception = NULL;
   int32_t			error = 0;
   int32_t			sample_rate;
   double			interval;
   int32_t			i, index, enabled;
   ViReal64 		offset = 0;
   char			scale[256] = "";
   char			channel[256] = "";
   
	switch (event)
	{
		case EVENT_COMMIT:

		if ( gs_DAQ )
			{
			for( i =0; i < MAX_NB_AI_CHANNELS; i++)
			{
				EXCCHECKCVI( GetCtrlVal (gs_panel[ELDAQ], gs_ChannelStates[i], &enabled));
				if( enabled)
				{
					EXCCHECKCVI( GetCtrlIndex (gs_panel[ELDAQ], gs_ScaleControls[i], &index));
					if( index)
					{
						EXCCHECKCVI( GetValueFromIndex (gs_panel[ELDAQ], gs_ScaleControls[i], index, scale));	
					}
					else
					{
						scale[0] = '\0';
					}
					sprintf( channel, "ai%d", i);
					if ( scale && strlen(scale))
					EXCCHECKIVIDAQ( eldaq_GetChannelScale ( gs_DAQ, channel, scale, (void *)&pScaleInfo[i]));
					else 
					pScaleInfo[i] =NULL;
				}
			}
			}
			
		break;
	}
	
Error:   
	if( pexception)
	{
		EnabeControls( panel, 1);
		SetCtrlAttribute( panel, ELDAQ_TIMER, ATTR_ENABLED, VI_FALSE);
	}
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnScale2"
int CVICALLBACK OnScale2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
   SElExceptionPtr	pexception = NULL;
   int32_t			error = 0;
   int32_t			sample_rate;
   double			interval;
   int32_t			i, index, enabled;
   ViReal64 		offset = 0;
   char			scale[256] = "";
   char			channel[256] = "";
   
	switch (event)
	{
		case EVENT_COMMIT:

		if ( gs_DAQ_2 )
			{
			for( i =0; i < MAX_NB_AI_CHANNELS_2; i++)
			{
				EXCCHECKCVI( GetCtrlVal (gs_panel[ELDAQ_3], gs_ChannelStates_2[i], &enabled));
				if( enabled)
				{
					EXCCHECKCVI( GetCtrlIndex (gs_panel[ELDAQ_3], gs_ScaleControls_2[i], &index));
					if( index)
					{
						EXCCHECKCVI( GetValueFromIndex (gs_panel[ELDAQ_3], gs_ScaleControls_2[i], index, scale));	
					}
					else
					{
						scale[0] = '\0';
					}
					sprintf( channel, "ai%d", i);
					if ( scale && strlen(scale))
					EXCCHECKIVIDAQ( eldaq_GetChannelScale ( gs_DAQ_2, channel, scale, (void *)&pScaleInfo[i]));
					else 
					pScaleInfo[i] =NULL;
				}
			}
			}
			
		break;
	}
	
Error:   
	if( pexception)
	{
		EnabeControls2( panel, 1);
		SetCtrlAttribute( panel, ELDAQ_3_TIMER, ATTR_ENABLED, VI_FALSE);
	}
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnScaleAll"
int CVICALLBACK OnScaleAll (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			value, i;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlIndex (panel, control, &value));
			switch(control)
			{
				case ELDAQ_SCALE_ALL_1:
					for(i=0; i<MAX_NB_AI_CHANNELS/2; i++)
						EXCCHECKCVI( SetCtrlIndex (panel, gs_ScaleControls[i], value));
					
					break;
				case ELDAQ_SCALE_ALL_2:
					for(i=MAX_NB_AI_CHANNELS/2; i<MAX_NB_AI_CHANNELS; i++)
						EXCCHECKCVI( SetCtrlIndex(panel, gs_ScaleControls[i], value));
					break;
			}
			break;
	}

Error:
	EXCDISPLAY(pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnScaleAll2"
int CVICALLBACK OnScaleAll2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			value, i;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlIndex (panel, control, &value));
			switch(control)
			{
				case ELDAQ_3_SCALE_ALL_1:
					for(i=0; i<MAX_NB_AI_CHANNELS_2/2; i++)
						EXCCHECKCVI( SetCtrlIndex (panel, gs_ScaleControls_2[i], value));
					
					break;
			}
			break;
	}

Error:
	EXCDISPLAY(pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnChannelEnable"
int CVICALLBACK OnChannelEnable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	char channel[32];
	int32_t			error = 0;
	int32_t			value, i;
	
	switch (event)
	{
		case EVENT_COMMIT:

			
			break;
	}
	
Error:   
	if( pexception)
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSwitch"
int CVICALLBACK OnSwitch (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	char channel[32];
	int32_t			error = 0;
	int32_t			value, i;
	
	switch (event)
	{
		case EVENT_COMMIT:

			for( i =0; i < MAX_NB_DI_P0_CHANNELS; i++)
				{
				if  (control == gs_Digital_Port_0[i])
					{
					sprintf(channel, "port0/Line%d", i);
					break;
					}
				}
			for( i =0; i < MAX_NB_DI_P1_CHANNELS; i++)
				{
				if  (control == gs_Digital_Port_1[i])
					{
					sprintf(channel, "port1/Line%d", i);
					break;
					}
				}
			for( i =0; i < MAX_NB_DI_P2_CHANNELS; i++)
				{
				if  (control == gs_Digital_Port_2[i])
					{
					sprintf(channel, "port2/Line%d", i);
					break;
					}
				}
					
			GetCtrlVal (panel, control, &value);
			EXCCHECKIVIDAQ( eldaq_WriteDigitalLine( gs_DAQ, channel, value));
			break;
	}
	
Error:   
	if( pexception)
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTimer"
int CVICALLBACK OnTimer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			channel[256] = "";
	ViReal64		value;
	int32_t			i, index, enabled;
	ViReal64 		offset = 0;

	switch (event)
	{
		case EVENT_TIMER_TICK:
			for( i =0; i < MAX_NB_AI_CHANNELS; i++)
			{
				EXCCHECKCVI( GetCtrlVal (gs_panel[ELDAQ], gs_ChannelStates[i], &enabled));
				if( enabled)
				{
					sprintf( channel, "ai%d", i);
					Measure_Mean(i, &value);
					//printf ("channel = %s | data_Analog[i] = %f\r\n", channel, data_Analog[i]);
					EXCCHECKCVI( SetCtrlVal (gs_panel[ELDAQ], gs_MeasIndicators[i], value));
				}
			}
			break;
	}

Error:   
	if(pexception)
	{
		SetCtrlAttribute( panel, ELDAQ_TIMER, ATTR_ENABLED, VI_FALSE);
	}
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTimer2"
int CVICALLBACK OnTimer2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			channel[256] = "";
	ViReal64		value;
	int32_t			i, index, enabled;
	ViReal64 		offset = 0;

	switch (event)
	{
		case EVENT_TIMER_TICK:
			for( i =0; i < MAX_NB_AI_CHANNELS_2; i++)
			{
				EXCCHECKCVI( GetCtrlVal (gs_panel[ELDAQ_3], gs_ChannelStates_2[i], &enabled));
				if( enabled)
				{
					sprintf( channel, "ai%d", i);
					Measure_Mean2(i, &value);
					//printf ("channel = %s | data_Analog[i] = %f\r\n", channel, data_Analog[i]);
					EXCCHECKCVI( SetCtrlVal (gs_panel[ELDAQ_3], gs_MeasIndicators_2[i], value));
				}
			}
			break;
	}

Error:   
	if(pexception)
	{
		SetCtrlAttribute( panel, ELDAQ_3_TIMER, ATTR_ENABLED, VI_FALSE);
	}
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnStart"
int CVICALLBACK OnStart(int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
   SElExceptionPtr	pexception = NULL;
   int32_t			error = 0;
   int32_t			sample_rate;
   double			interval;
   int32_t			i, index, enabled;
   ViReal64 		offset = 0;
   char			scale[256] = "";
   char			channel[256] = "";

	switch (event)
	{
		case EVENT_COMMIT:
			if ( gs_DAQ )
			{
			for( i =0; i < MAX_NB_AI_CHANNELS; i++)
			{
				EXCCHECKCVI( GetCtrlVal (gs_panel[ELDAQ], gs_ChannelStates[i], &enabled));
				if( enabled)
				{
					EXCCHECKCVI( GetCtrlIndex (gs_panel[ELDAQ], gs_ScaleControls[i], &index));
					if( index)
					{
						EXCCHECKCVI( GetValueFromIndex (gs_panel[ELDAQ], gs_ScaleControls[i], index, scale));	
					}
					else
					{
						scale[0] = '\0';
					}
					sprintf( channel, "ai%d", i);
					if ( scale && strlen(scale))
					EXCCHECKIVIDAQ( eldaq_GetChannelScale ( gs_DAQ, channel, scale, (void *)&pScaleInfo[i]));
					}
			}
				EXCCHECKCVI( GetCtrlVal( panel, ELDAQ_SAMPLE_RATE, &sample_rate));
				EXCCHECKCVI( GetCtrlVal( panel, ELDAQ_MEAS_INTERVAL, &interval));
				EXCCHECK( EnabeControls( panel, 0));
				//EXCCHECKIVIDAQ( eldaq_ConfigureMeasurement (gs_DAQ, "", sample_rate, interval));
				//EXCCHECKIVIDAQ( eldaq_StartMeasurement( gs_DAQ));
				EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_TIMER, ATTR_ENABLED, VI_TRUE));
			}
			break;
	}

Error:   
	if( pexception)
	{
		EnabeControls( panel, 1);
		SetCtrlAttribute( panel, ELDAQ_TIMER, ATTR_ENABLED, VI_FALSE);
	}
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnStart2"
int CVICALLBACK OnStart2(int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
   SElExceptionPtr	pexception = NULL;
   int32_t			error = 0;
   int32_t			sample_rate;
   double			interval;
   int32_t			i, index, enabled;
   ViReal64 		offset = 0;
   char			scale[256] = "";
   char			channel[256] = "";

	switch (event)
	{
		case EVENT_COMMIT:
			if ( gs_DAQ_2 )
			{
			for( i =0; i < MAX_NB_AI_CHANNELS_2; i++)
			{
				EXCCHECKCVI( GetCtrlVal (gs_panel[ELDAQ_3], gs_ChannelStates_2[i], &enabled));
				if( enabled)
				{
					EXCCHECKCVI( GetCtrlIndex (gs_panel[ELDAQ_3], gs_ScaleControls_2[i], &index));
					if( index)
					{
						EXCCHECKCVI( GetValueFromIndex (gs_panel[ELDAQ_3], gs_ScaleControls_2[i], index, scale));	
					}
					else
					{
						scale[0] = '\0';
					}
					sprintf( channel, "ai%d", i);
					if ( scale && strlen(scale))
					EXCCHECKIVIDAQ( eldaq_GetChannelScale ( gs_DAQ_2, channel, scale, (void *)&pScaleInfo[i]));
					}
			}
				EXCCHECKCVI( GetCtrlVal( panel, ELDAQ_3_SAMPLE_RATE, &sample_rate));
				EXCCHECKCVI( GetCtrlVal( panel, ELDAQ_3_MEAS_INTERVAL, &interval));
				EXCCHECK( EnabeControls2( panel, 0));
				//EXCCHECKIVIDAQ( eldaq_ConfigureMeasurement (gs_DAQ, "", sample_rate, interval));
				//EXCCHECKIVIDAQ( eldaq_StartMeasurement( gs_DAQ));
				EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_TIMER, ATTR_ENABLED, VI_TRUE));
			}
			break;
	}

Error:   
	if( pexception)
	{
		EnabeControls2( panel, 1);
		SetCtrlAttribute( panel, ELDAQ_3_TIMER, ATTR_ENABLED, VI_FALSE);
	}
	EXCDISPLAY( pexception);
	return 0;
}


//int CVICALLBACK OnAquisition (int panel, int control, int event,
//		void *callbackData, int eventData1, int eventData2)
//{	 
//	SElExceptionPtr	pexception = NULL;
//    int32_t		error = 0;
//	char        chan[256];
//	char        clockSource[256];
//	double      min,max;
//	unsigned int      sampsPerChan;
//	double     *data=NULL;
//	int         log;
//	int         rate;
//
//	switch (event)
//	{
//		case EVENT_COMMIT:
//
//		EnabeControls2(panel, 1);
//		GetCtrlVal(panel,ELDAQ_3_CHANNEL,chan);
//		GetCtrlVal(panel,ELDAQ_3_MIN,&min);
//		GetCtrlVal(panel,ELDAQ_3_MAX,&max);
//		GetCtrlVal(panel,ELDAQ_3_CLKSRC,clockSource);
//		GetCtrlVal(panel,ELDAQ_3_SAMPSPERCHAN,&sampsPerChan);
//		GetCtrlVal(panel,ELDAQ_3_RATE,&rate);
//		
//		if( (data=malloc(sampsPerChan*sizeof(double)))==NULL ) {
//			MessagePopup("Error","Not enough memory");
//			goto Error;
//		}
//		
//		//SetCtrlAttribute(panel,ELDAQ_3_GRAPH,ATTR_XAXIS_GAIN,1.0/rate);
//		//log = (int)log10(rate);
//		//SetCtrlAttribute(panel,ELDAQ_3_GRAPH,ATTR_XPRECISION,log);
//		DeleteGraphPlot(panel,ELDAQ_3_GRAPH,-1,VAL_IMMEDIATE_DRAW);
//
//		EXCCHECKIVIDAQ( eldaq_ReadForce (gs_DAQ, chan, clockSource, min, max, sampsPerChan, rate, data));
//
//		PlotY(panel,ELDAQ_3_GRAPH,&(data[0]),sampsPerChan,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE);
//			
//		break;
//	}
//Error:   
//	if( pexception)
//	EXCDISPLAY( pexception);
//	EnabeControls2(panel,0);
//	return 0;
//}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnReset"
int CVICALLBACK OnReset(int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	char channel[32];
	int32_t			error = 0;
	int32_t			value, i;

	switch (event)
	{
		case EVENT_COMMIT:

			
			for( i =0; i < MAX_NB_DI_P0_CHANNELS; i++)
				{
					sprintf(channel, "port0/Line%d", i);
					EXCCHECKIVIDAQ( eldaq_WriteDigitalLine( gs_DAQ, channel, 0));

				}
			for( i =0; i < MAX_NB_DI_P1_CHANNELS; i++)
				{
			        sprintf(channel, "port1/Line%d", i);
					EXCCHECKIVIDAQ( eldaq_WriteDigitalLine( gs_DAQ, channel, 0));
				}
			for( i =0; i < MAX_NB_DI_P2_CHANNELS; i++)
				{
				    sprintf(channel, "port2/Line%d", i);
					EXCCHECKIVIDAQ( eldaq_WriteDigitalLine( gs_DAQ, channel, 0));
				}
					
			
		break;
	}

Error:   
	if( pexception)
	{
		EnabeControls( panel, 1);
		SetCtrlAttribute( panel, ELDAQ_TIMER, ATTR_ENABLED, VI_FALSE);
	}
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnStop"
int CVICALLBACK OnStop(int panel, int control, int event,
		void *callbackData,	int eventData1, int eventData2)
{
   SElExceptionPtr	pexception = NULL;
   int32_t			error = 0;

	switch (event)
	{
		case EVENT_COMMIT:
			if ( gs_DAQ )
			{
				EXCCHECK( EnabeControls( panel, 1));
				EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_TIMER, ATTR_ENABLED, VI_FALSE));
				//EXCCHECKIVIDAQ( eldaq_StopMeasurement( gs_DAQ));
			}
			break;
	}

Error:   
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnStop2"
int CVICALLBACK OnStop2(int panel, int control, int event,
		void *callbackData,	int eventData1, int eventData2)
{
   SElExceptionPtr	pexception = NULL;
   int32_t			error = 0;

	switch (event)
	{
		case EVENT_COMMIT:
			if ( gs_DAQ_2 )
			{
				EXCCHECK( EnabeControls2( panel, 1));
				EXCCHECKCVI( SetCtrlAttribute( panel, ELDAQ_3_TIMER, ATTR_ENABLED, VI_FALSE));
				//EXCCHECKIVIDAQ( eldaq_StopMeasurement( gs_DAQ));
			}
			break;
	}

Error:   
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadCalibFile"
static SElExceptionPtr ReadCalibFile(const char* fileXml)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     pnode2old = 0;
	CAObjHandle     pparameter = 0;
	const char*     pfile_xml = (fileXml) ? fileXml : "calibration.xml";
	VBOOL           is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL;
	int32_t			i;
	char			channel[256], name[256], description[256];

	EXCCHECK( EnabeCalib( gs_panel[ELDAQ], 1));
	EXCCHECK( FillScale( gs_panel[ELDAQ])); 

	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCOM( CA_VariantSetCString(  &pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//scales", &pnode));

	for( i=0; i<MAX_NB_AI_CHANNELS; i++)
	{
		sprintf( channel, "channel[@name='ai%d']/scale", i);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, channel, &pnode2));
		while( pnode2)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@name", &pparameter));
			if ( pparameter )
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
				strncpy( name, ptext, 255);
				
				EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			else
			{
				name[0] = '\0';
			}

			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@description", &pparameter));
			if ( pparameter )
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
				strncpy( description, ptext, 255);
				
				EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			else
			{
				description[0] = '\0';
			}

			if( name[0]!='\0' && description[0]!='\0')
			{
				EXCCHECKCVI( InsertListItem( gs_panel[ELDAQ], gs_ScaleControls[i], -1, description, name));
			}
		
			pnode2old = pnode2;
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2));
			EXCCHECKCVI( CA_DiscardObjHandle(pnode2old));
			pnode2old = 0; 
		};
	}

Error:     
	CA_FreeMemory(ptext);
	CA_VariantClear(&pfile_name_var);
	if (pnode2) CA_DiscardObjHandle(pnode2);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	
	if(pexception)
	{
		if (pxml) CA_DiscardObjHandle(pxml);
		if( gs_pxml) CA_DiscardObjHandle(gs_pxml);
		gs_pxml = 0;
		gs_pxml_file[0] = '\0';
	}
	else
	{
		if( gs_pxml) CA_DiscardObjHandle(gs_pxml);
		gs_pxml = pxml;
		strncpy(gs_pxml_file, pfile_xml,511);
	}

	EXCRETHROW( pexception);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadCalibFile2"
static SElExceptionPtr ReadCalibFile2(const char* fileXml2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     pnode2old = 0;
	CAObjHandle     pparameter = 0;
	const char*     pfile_xml2 = (fileXml2) ? fileXml2 : "calibration.xml";
	VBOOL           is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL;
	int32_t			i;
	char			channel[256], name[256], description[256];

	EXCCHECK( EnabeCalib2( gs_panel[ELDAQ_3], 1));
	EXCCHECK( FillScale2( gs_panel[ELDAQ_3])); 

	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCOM( CA_VariantSetCString(  &pfile_name_var, pfile_xml2));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//scales", &pnode));

	for( i=0; i<MAX_NB_AI_CHANNELS_2; i++)
	{
		sprintf( channel, "channel[@name='ai%d']/scale", i);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, channel, &pnode2));
		while( pnode2)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@name", &pparameter));
			if ( pparameter )
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
				strncpy( name, ptext, 255);
				
				EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			else
			{
				name[0] = '\0';
			}

			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@description", &pparameter));
			if ( pparameter )
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
				strncpy( description, ptext, 255);
				
				EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			else
			{
				description[0] = '\0';
			}

			if( name[0]!='\0' && description[0]!='\0')
			{
				EXCCHECKCVI( InsertListItem( gs_panel[ELDAQ_3], gs_ScaleControls_2[i], -1, description, name));
			}
		
			pnode2old = pnode2;
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2));
			EXCCHECKCVI( CA_DiscardObjHandle(pnode2old));
			pnode2old = 0; 
		};
	}

Error:     
	CA_FreeMemory(ptext);
	CA_VariantClear(&pfile_name_var);
	if (pnode2) CA_DiscardObjHandle(pnode2);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	
	if(pexception)
	{
		if (pxml) CA_DiscardObjHandle(pxml);
		if( gs_pxml2) CA_DiscardObjHandle(gs_pxml2);
		gs_pxml2 = 0;
		gs_pxml_file2[0] = '\0';
	}
	else
	{
		if( gs_pxml2) CA_DiscardObjHandle(gs_pxml2);
		gs_pxml2 = pxml;
		strncpy(gs_pxml_file2, pfile_xml2,511);
	}

	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnBrowseCal"
int CVICALLBACK OnBrowseCal (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	char			file_name[MAX_PATHNAME_LEN] = "";

	switch (event)
	{
		case EVENT_COMMIT:
			if ( FileSelectPopup ("", "*.xml", "*.xml", "Select Calibration File", VAL_LOAD_BUTTON, 0, 1, 1, 0, file_name)
				 == VAL_EXISTING_FILE_SELECTED )
			{
				EXCCHECKCVI( SetCtrlVal( panel, ELDAQ_CALIB_FILE, file_name));
				EXCCHECK( ReadCalibFile( file_name));	
			}
			break;
	}

Error:   
	if( pexception)
	{
		SetCtrlVal( panel, ELDAQ_CALIB_FILE, "");
	}
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnBrowseCal2"
int CVICALLBACK OnBrowseCal2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	char			file_name[MAX_PATHNAME_LEN] = "";

	switch (event)
	{
		case EVENT_COMMIT:
			if ( FileSelectPopup ("", "*.xml", "*.xml", "Select Calibration File", VAL_LOAD_BUTTON, 0, 1, 1, 0, file_name)
				 == VAL_EXISTING_FILE_SELECTED )
			{
				EXCCHECKCVI( SetCtrlVal( panel, ELDAQ_3_CALIB_FILE, file_name));
				EXCCHECK( ReadCalibFile2( file_name));	
			}
			break;
	}

Error:   
	if( pexception)
	{
		SetCtrlVal( panel, ELDAQ_3_CALIB_FILE, "");
	}
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ShowPanel"
static SElExceptionPtr ShowPanel(void)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				visible;
	
	EXCCHECKCVI( GetPanelAttribute (gs_panel[ELDAQ], ATTR_VISIBLE, &visible));	
	if(!visible)
	{
		EXCCHECK( EnabeControls( gs_panel[ELDAQ], 1));
		EXCCHECKCVI( DisplayPanel(gs_panel[ELDAQ]));
	}
		
Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ShowPanel2"
static SElExceptionPtr ShowPanel2(void)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				visible;
	
	EXCCHECKCVI( GetPanelAttribute (gs_panel[ELDAQ_3], ATTR_VISIBLE, &visible));	
	if(!visible)
	{
		EXCCHECK( EnabeControls2( gs_panel[ELDAQ_3], 1));
		EXCCHECKCVI( DisplayPanel(gs_panel[ELDAQ_3]));
	}
		
Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ClosePanel"
static SElExceptionPtr ClosePanel(void)          
{
	int32_t          error = 0;
   	SElExceptionPtr  pexception = NULL;
	int				visible;
	
	EXCCHECKCVI( GetPanelAttribute (gs_panel[ELDAQ], ATTR_VISIBLE, &visible));
	if(visible)
	{
	  	int32_t			enabled = 0; 

		EXCCHECKCVI( GetCtrlAttribute( gs_panel[ELDAQ], ELDAQ_TIMER, ATTR_ENABLED, &enabled));
		if(	enabled)
		{
			//EXCCHECKIVIDAQ( eldaq_StopMeasurement( gs_DAQ));
			EXCCHECKCVI( SetCtrlAttribute( gs_panel[ELDAQ], ELDAQ_TIMER, ATTR_ENABLED, VI_FALSE));
		}

		EXCCHECKCVI( HidePanel(gs_panel[ELDAQ]));
	}
	
Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ClosePanel2"
static SElExceptionPtr ClosePanel2(void)          
{
	int32_t          error = 0;
   	SElExceptionPtr  pexception = NULL;
	int				visible;
	
	EXCCHECKCVI( GetPanelAttribute (gs_panel[ELDAQ_3], ATTR_VISIBLE, &visible));
	if(visible)
	{
	  	int32_t			enabled = 0; 

		EXCCHECKCVI( GetCtrlAttribute( gs_panel[ELDAQ_3], ELDAQ_3_TIMER, ATTR_ENABLED, &enabled));
		if(	enabled)
		{
			//EXCCHECKIVIDAQ( eldaq_StopMeasurement( gs_DAQ));
			EXCCHECKCVI( SetCtrlAttribute( gs_panel[ELDAQ_3], ELDAQ_3_TIMER, ATTR_ENABLED, VI_FALSE));
		}

		EXCCHECKCVI( HidePanel(gs_panel[ELDAQ_3]));
	}
	
Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PwrConfigure"
static SElExceptionPtr PwrConfigure(int panel)          
{
   	SElExceptionPtr	pexception = NULL;
	ViStatus		error = VI_SUCCESS;
	ViSession		vi_PSW = 0;
	ViSession		vi_POWER = 0;
	ViSession		vi_SW5 = 0;
	ViReal64		current = 0.2, voltage = 13.5;
	ViInt32			state = 0;
	char Chaine [50];

	if (gs_IviBridge!=NULL)
	{
		EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "POWER", &vi_POWER));
	
		GetCtrlVal(panel, ELDAQ_PWR_VOLTAGE, &voltage);
		GetCtrlVal(panel, ELDAQ_PWR_CURRENT, &current);
				
		/* source set */
		checkErr( IviDCPwr_ConfigureVoltageLevel(vi_POWER, "CHANNEL1", voltage));
		Sleep(200);
		checkErr( IviDCPwr_ConfigureCurrentLimit(vi_POWER, "CHANNEL1", IVIDCPWR_VAL_CURRENT_REGULATE, current));
		Sleep(200);
		checkErr( IviDCPwr_ConfigureOutputEnabled(vi_POWER, "CHANNEL1", VI_TRUE));
	}

Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PwrConfigure2"
static SElExceptionPtr PwrConfigure2(int panel)          
{
   	SElExceptionPtr	pexception = NULL;
	ViStatus		error = VI_SUCCESS;
	ViSession		vi_PSW = 0;
	ViSession		vi_POWER = 0;
	ViSession		vi_SW5 = 0;
	ViReal64		current = 0.2, voltage = 13.5;
	ViInt32			state = 0;
	char Chaine [50];

	if (gs_IviBridge!=NULL)
	{
		EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "POWER", &vi_POWER));
	
		GetCtrlVal(panel, ELDAQ_3_PWR_VOLTAGE, &voltage);
		GetCtrlVal(panel, ELDAQ_3_PWR_CURRENT, &current);
				
		/* source set */
		checkErr( IviDCPwr_ConfigureVoltageLevel(vi_POWER, "CHANNEL1", voltage));
		Sleep(200);
		checkErr( IviDCPwr_ConfigureCurrentLimit(vi_POWER, "CHANNEL1", IVIDCPWR_VAL_CURRENT_REGULATE, current));
		Sleep(200);
		checkErr( IviDCPwr_ConfigureOutputEnabled(vi_POWER, "CHANNEL1", VI_TRUE));
	}

Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PwrDisable"
static SElExceptionPtr PwrDisable(int panel)          
{
   	SElExceptionPtr	pexception = NULL;
	ViStatus		error = VI_SUCCESS;
	ViSession		vi_PSW = 0;
	ViSession		vi_POWER = 0;
	ViSession		vi_SW5 = 0;
	ViInt32			state = 0;

	if(gs_IviBridge!=NULL)
	{
		EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "POWER", &vi_POWER));

		/* source disconnect */
		checkErr( IviDCPwr_ConfigureOutputEnabled(vi_POWER, "CHANNEL1", VI_FALSE));

	}

Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPwrConfigure"
int CVICALLBACK OnPwrConfigure (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECK( PwrConfigure(panel));
			break;
	}
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPwrConfigure2"
int CVICALLBACK OnPwrConfigure2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECK( PwrConfigure2(panel));
			break;
	}
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPwrDisable"
int CVICALLBACK OnPwrDisable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECK( PwrDisable(panel));
			break;
	}
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#if defined(_CVI_DLL_) || \
	defined(_WINDLL)
void DiscardUIObjectsForDLL (void)
{
   /* Discard the panels loaded in InitUIForDLL */
}

int __stdcall DllMain(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved
)
{
   switch (fdwReason)
      {
      case DLL_PROCESS_ATTACH:
#if (!defined(_CVI_)&&defined(_DEBUG))
		//_CrtSetBreakAlloc(57);
#endif
         if (InitCVIRTE (hinstDLL, 0, 0) == 0)      
            return 0;   /* out of memory */
         break;
      case DLL_PROCESS_DETACH:
         if (!CVIRTEHasBeenDetached())      /* Do not call CVI functions if cvirte.dll has already been detached. */
         {
            DiscardUIObjectsForDLL ();      /* Discard the panels loaded in InitUIForDLL */
            CloseCVIRTE ();      
         }
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
			_CrtDumpMemoryLeaks();
#endif
         break;
      }

   return 1;
}

int __stdcall DllEntryPoint(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved
)
{
   /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}

#undef __FUNC__
#define __FUNC__ "{ElDaq Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	const char*		pconfiguration_file = NULL;
	int32_t			main_panel = 0;

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
                            "CardPanel",
                            sizeof(main_panel),
                            &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "Modules",
	                        sizeof(SObjListPtr),
	                        &gs_Modules));

   	/* gettext */
	if(gs_Modules)
	{
		gs_pLanguage = (SLanguagePtr)gs_Modules->Get(gs_Modules, MODULE_LANGUAGE);  
		if(gs_pLanguage)
		{
			char pathname[MAX_PATHNAME_LEN];
			EXCCHECKCVI( GetProjectDir( pathname));
			strcat(pathname, "\\lang");
			LANGUAGE_PTR->bindtextdomain (LANGUAGE_PTR, PACKAGE, pathname);
			LANGUAGE_PTR->textdomain (LANGUAGE_PTR, PACKAGE);
		}
	}

	EXCCHECK( LoadPanelExWithLanguage(main_panel, "eldaq_gui.uir", ELDAQ, gs_ELDAQlang, &gs_panel[ELDAQ])); 
	EXCCHECK( EnabeControls( gs_panel[ELDAQ], 1));
	EXCCHECK( EnabeCalib( gs_panel[ELDAQ], 0));
	EXCCHECK( FillScale( gs_panel[ELDAQ]));
  
	/* Digital Output DAQ */
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "eldaq_gui.uir", ELDAQ_2, gs_ELDAQlang, &gs_panel[ELDAQ_2])); 
	
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "eldaq_gui.uir", ELDAQ_3, gs_ELDAQlang, &gs_panel[ELDAQ_3]));
	EXCCHECK( EnabeControls2( gs_panel[ELDAQ_3], 1));
	EXCCHECK( EnabeCalib2( gs_panel[ELDAQ_3], 1));
	EXCCHECK( FillScale2( gs_panel[ELDAQ_3]));
	

	EXCCHECKCVI( SetPanelAttribute (gs_panel[ELDAQ], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[ELDAQ], ATTR_LEFT, 0));
	
	/* Digital Output DAQ */
	EXCCHECKCVI( SetPanelAttribute (gs_panel[ELDAQ_2], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[ELDAQ_2], ATTR_LEFT, 0));
	
	/* Graph DAQ */
	EXCCHECKCVI( SetPanelAttribute (gs_panel[ELDAQ_3], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[ELDAQ_3], ATTR_LEFT, 0));
	
//	EXCCHECKIVIDAQ( NIDAQmx_NewPhysChanAICtrl(gs_panel[ELDAQ_3],ELDAQ_3_CHANNEL,1));
//	EXCCHECKIVIDAQ( NIDAQmx_NewTerminalCtrl(gs_panel[ELDAQ_3],ELDAQ_3_CLKSRC,0));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "EventFactory",
	                        sizeof(SElEventFactoryPtr),
	                        &gs_EventFactory));

	if(gs_Modules)
		gs_IviBridge = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
	
	if (gs_IviBridge)
	{
		char file_name[1024] = "";
		char file_name2[1024] = "";

		EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "DAQ", &gs_DAQ));
		
		EXCCHECKIVIDAQ( eldaq_GetAttributeViString(gs_DAQ, NULL, ELDAQ_ATTR_CALIBRATION_FILE_NAME, 1024, file_name));
		if(file_name[0]!='\0')
		{
			EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ], ELDAQ_CALIB_FILE, file_name));
			EXCCHECK( ReadCalibFile( file_name));
		}
		
		EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "DAQ2", &gs_DAQ_2));
		
		EXCCHECKIVIDAQ( eldaq_GetAttributeViString(gs_DAQ_2, NULL, ELDAQ_ATTR_CALIBRATION_FILE_NAME, 1024, file_name2));
		if(file_name2[0]!='\0')
		{
			EXCCHECKCVI( SetCtrlVal( gs_panel[ELDAQ_3], ELDAQ_3_CALIB_FILE, file_name2));
			EXCCHECK( ReadCalibFile2( file_name2));
		}
	}

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{ElDaq Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

	if( 0==strcmp( uuid, PLUGIN_ELDAQ))
	{
		EXCCHECK( ShowPanel());
	}

Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{ElDaq Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
   
	EXCCHECK( ClosePanel());

Error:   
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{ElDaq Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
   
	if(gs_panel[ELDAQ])
		EXCCHECKCVI( DiscardPanel(gs_panel[ELDAQ]));

Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{ElDaq Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{ElDaq Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif 

