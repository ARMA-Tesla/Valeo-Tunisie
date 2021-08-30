#include <msxmldom.h>
#include <calibration/calibration.h>
#include <language/language.h>
#include <EventFactory/EventFactory.h>
#include <ivibridge/ivibridge.h>
#include <utility.h>
#include "calibration_plugin.h"
#include "calibration_gui.h"
#include <IviDCPwr.h>
#include <IviSwtch.h>
#include <IviDMM.h>

static const char* HISTORY_INFO = \
"[08-05-26] rT\n"\
"+ Changed resolution for all ranges\n"\
"[07-10-23] ZR\n"\
"+ Changed resolution of DMM pro range 2mA (from 2V to 200mV)";

#define NUMBER_OF_PANELS 		1
#define MAX_NB_AI_CHANNELS		8

static int					gs_panel[NUMBER_OF_PANELS+1];
SLanguagePtr 				gs_pLanguage = NULL;
static SObjListPtr          gs_Modules = NULL;
static SElEventFactoryPtr	gs_EventFactory = NULL;
static SCalibrationPtr		gs_calibration = NULL;
static SIviBridgePtr		gs_IviBridge = NULL;
static int32_t				gs_ActiveChannel = 0;
static ViSession			gs_DMM = 0;
static ViSession			gs_POWER = 0;
static ViSession			gs_PSW = 0;
static ViSession			gs_SW5 = 0;

/* language support pattern */
static SLanguageItem gs_ELMEASlang[] = {
   	P(ELMEAS,                	_noop("Current Calibration")),
   	{0 , NULL, 0}
};

int	gs_measSelect[] =
{
	ELMEAS_MEAS_1,
	ELMEAS_MEAS_2,
	ELMEAS_MEAS_3,
	ELMEAS_MEAS_4,
	ELMEAS_MEAS_5,
	ELMEAS_MEAS_6,
	ELMEAS_MEAS_7,
	ELMEAS_MEAS_8
};

int	gs_measValues[] =
{
	ELMEAS_AI0,
	ELMEAS_AI1,
	ELMEAS_AI2,
	ELMEAS_AI3,
	ELMEAS_AI4,
	ELMEAS_AI5,
	ELMEAS_AI6,
	ELMEAS_AI7
};

int	gs_measScales[] =
{
	ELMEAS_SCALE_AI0,
	ELMEAS_SCALE_AI1,
	ELMEAS_SCALE_AI2,
	ELMEAS_SCALE_AI3,
	ELMEAS_SCALE_AI4,
	ELMEAS_SCALE_AI5,
	ELMEAS_SCALE_AI6,
	ELMEAS_SCALE_AI7
};

char* gs_measChannels[] =
{
	"Channel1H",
	"Channel1L",
	"Channel2H",
	"Channel2L",
	"Channel3H",
	"Channel3L",
	"Channel4H",
	"Channel4L"
};

char* gs_measPSWCH[] =
{
	"CH1",
	"CH1",
	"CH2",
	"CH2",
	"CH3",
	"CH3",
	"CH4",
	"CH4"
};

char* gs_measPSWC[] =
{
	"C1",
	"C1",
	"C2",
	"C2",
	"C3",
	"C3",
	"C4",
	"C4"
};

int gs_measPSWCConnect[] =
{
	0,
	1,
	0,
	1,
	0,
	1,
	0,
	1
};

char* gs_measDMMSwtch[] =
{
	"CH1",
	"CH2",
	"CH3",
	"CH4",
	"CH5",
	"CH6",
	"CH7",
	"CH8"
};

/* error handling */
static SElExceptionPtr EnabeControls( int panel, int state);
static SElExceptionPtr FillScale( int panel );
static SElExceptionPtr EnabeCalib( int panel, int state);

int CVICALLBACK OnMeasChanged (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2);

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "FillScale"
static SElExceptionPtr FillScale( int panel )
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int				i;
	
	/* TODO: Skipped temporary. In calibration module must be implemented function Get All Scale for Channel */
/*	for ( i = 0; i < MAX_NB_AI_CHANNELS; i++)
	{
		EXCCHECKCVI( ClearListCtrl( panel, gs_measScales[i]));
		EXCCHECKCVI( InsertListItem( panel, gs_measScales[i], 0, "None", 0));
	} */

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSaveCalib"
int CVICALLBACK OnSaveCalib (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	char			file_name[2048] = "";
	char			xml_file[2048] = "";
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, ELMEAS_CALIB_FILE, xml_file));
			sprintf(file_name, "file://%s", xml_file);
			EXCCHECK( gs_calibration->Save(gs_calibration, file_name));
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSave"
int CVICALLBACK OnSave (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	char			scale[256] = "";
	double			x1, x2, y1, y2;

	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panel, gs_measScales[gs_ActiveChannel], scale);
		GetCtrlVal(panel, ELMEAS_CALIB_X1, &x1);
		GetCtrlVal(panel, ELMEAS_CALIB_X2, &x2);
		GetCtrlVal(panel, ELMEAS_CALIB_Y1, &y1);
		GetCtrlVal(panel, ELMEAS_CALIB_Y2, &y2);
		if (strlen(scale) > 0)
		{
			EXCCHECK( gs_calibration->AddLinearScale(gs_calibration, gs_measChannels[gs_ActiveChannel], scale, x1, y1, x2, y2));
		}
		break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMeasChanged"
int CVICALLBACK OnMeasChanged (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int				error = 0,
					i = 0;
	char			channel[128] = "CHANNEL1";
	char			chxa[20] = "";
	char			chxb[20] = "";
	char			cxa[20] = "";
	char			cxb[20] = "";
	ViBoolean		enabled;
	
	switch (event)
	{
		case EVENT_COMMIT:
			for (i = 0; i < MAX_NB_AI_CHANNELS; i++)
			{
				EXCCHECKCVI( SetCtrlVal(panel, gs_measSelect[i], 0));
				if (control == gs_measSelect[i])
					gs_ActiveChannel = i;
			}
			EXCCHECKCVI( SetCtrlVal(panel, control, 1));
			EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_MSG_CALIB_CHANNEL, gs_measChannels[gs_ActiveChannel]));
			
			checkErr( IviDCPwr_GetAttributeViBoolean(gs_POWER, channel, IVIDCPWR_ATTR_OUTPUT_ENABLED, &enabled));
			if(enabled)
				checkErr( IviDCPwr_ConfigureOutputEnabled(gs_POWER, channel, 0));
			
			checkErr( IviSwtch_DisconnectAll(gs_PSW));
			checkErr( IviSwtch_DisconnectAll(gs_SW5));		
			checkErr( IviSwtch_Connect(gs_SW5, gs_measDMMSwtch[gs_ActiveChannel], "com0"));

			sprintf(chxa, "%sA", gs_measPSWCH[gs_ActiveChannel]);
			sprintf(chxb, "%sB", gs_measPSWCH[gs_ActiveChannel]);
			checkErr( IviSwtch_Connect(gs_PSW, chxa, chxb));

			sprintf(cxa, "%sA", gs_measPSWC[gs_ActiveChannel]);
			sprintf(cxb, "%sB", gs_measPSWC[gs_ActiveChannel]);
			if (gs_measPSWCConnect[gs_ActiveChannel])
				checkErr( IviSwtch_Connect(gs_PSW, cxa, cxb));
			
			if(enabled)
				checkErr( IviDCPwr_ConfigureOutputEnabled(gs_POWER, channel, 1));

			break;
	}

Error:
	IVI_CHECK_ERR(error);
	EXCDISPLAY(pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnScale"
int CVICALLBACK OnScale (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int				index = 0,
					i = 0,
					error = 0;
	char			scale[256] = "";
	double			x1 = 0.0,
					y1 = 0.0,
					x2 = 0.0,
					y2 = 0.0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_CALIB_X1, x1));
			EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_CALIB_Y1, y1));
			EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_CALIB_X2, x2));
			EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_CALIB_Y2, y2));

			EXCCHECKCVI( GetCtrlIndex(panel, control, &index));
			EXCCHECKCVI( GetCtrlVal(panel, control, scale));

			for (i = 0; i < MAX_NB_AI_CHANNELS; i++)
			{
				if (control == gs_measScales[i])
					break;
			}

			switch (index)
			{
				case 1: /* 2mA => 0.002 * 100 => 200mV */
						/* 200 mA => 0.2 * 0.1 => 20mV */
					if ( control == ELMEAS_SCALE_AI1 || control == ELMEAS_SCALE_AI3 || control == ELMEAS_SCALE_AI5 || control == ELMEAS_SCALE_AI7)
					{
						checkErr( IviDmm_ConfigureMeasurement(gs_DMM, IVIDMM_VAL_DC_VOLTS, 0.2, 0.00001));
					}
					else
					{
						checkErr( IviDmm_ConfigureMeasurement(gs_DMM, IVIDMM_VAL_DC_VOLTS, 0.02, 0.000001));
					}
					break;
				case 2: /* 2A => 2 * 0.1 => 200mV */
					checkErr( IviDmm_ConfigureMeasurement(gs_DMM, IVIDMM_VAL_DC_VOLTS, 0.2, 0.00001));
					break;
				default:
					EXCCHECKCVI( IviDmm_ConfigureMeasurement(gs_DMM, IVIDMM_VAL_DC_VOLTS, 25.0, 0.001));
					break;
			}

			if(strlen(scale)>0 && i<MAX_NB_AI_CHANNELS)
			{
				pexception = gs_calibration->GetLinearScale(gs_calibration, gs_measChannels[i], scale, &x1, &y1, &x2, &y2);
				if(pexception==NULL)
				{
					EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_CALIB_X1, x1));
					EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_CALIB_Y1, y1));
					EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_CALIB_X2, x2));
					EXCCHECKCVI( SetCtrlVal(panel, ELMEAS_CALIB_Y2, y2));
				}
				else
				{
					EXCIGNORE(CALIB_ERROR_UNKNOWN_SCALE_NAME);
				}
			}
			break;
	}
	
Error:
	IVI_CHECK_ERR(error);
	EXCDISPLAY(pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnCalibScale"
int CVICALLBACK OnCalibScale (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:

		break;
	}
	return 0;
}

#define	FILTER_AVG_SIZE		20

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTimer"
int CVICALLBACK OnTimer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				i;
	char			scale[256] = "";
	ViReal64		value = 0.0,
					valueAvg = 0.0;

	switch (event)
	{
		case EVENT_TIMER_TICK:
			valueAvg = 0.0;
			i = 0;
			do
			{
				checkErr( IviDmm_Read(gs_DMM, 2000, &value));
				valueAvg += value;
			}while(++i<FILTER_AVG_SIZE);
	
			valueAvg /= FILTER_AVG_SIZE;

			EXCCHECKCVI( GetCtrlVal(panel, gs_measScales[gs_ActiveChannel], scale));
			if(strlen(scale)>0)
			{
				pexception = gs_calibration->Calculate(gs_calibration, gs_measChannels[gs_ActiveChannel], scale, valueAvg, &value);
				EXCIGNORE(CALIB_ERROR_UNKNOWN_SCALE_NAME);
			}
			EXCCHECKCVI( SetCtrlVal(panel, gs_measValues[gs_ActiveChannel], value));

			break;
	}

Error:   
	IVI_CHECK_ERR(error);
	if(pexception)
	{
		SetCtrlAttribute( panel, ELMEAS_TIMER, ATTR_ENABLED, VI_FALSE);
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

	switch (event)
	{
		case EVENT_COMMIT:
			if(gs_DMM)
			{
				EXCCHECK( EnabeControls( panel, FALSE));
				EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_TIMER, ATTR_ENABLED, TRUE));
			}
			break;
	}

Error:   
	if(pexception)
	{
		EnabeControls( panel, TRUE);
		SetCtrlAttribute( panel, ELMEAS_TIMER, ATTR_ENABLED, FALSE);
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
			if(gs_DMM)
			{
				EXCCHECK( EnabeControls( panel, TRUE));
				EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_TIMER, ATTR_ENABLED, VI_FALSE));
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
	int32_t         error = 0,
					i = 0;
   	SElExceptionPtr pexception = NULL;
	char			file_name[2048] = "";
	
	sprintf(file_name, "file://%s", fileXml);
	EXCCHECK( gs_calibration->Load(gs_calibration, file_name));
	
	for (i = 0; i < MAX_NB_AI_CHANNELS; i++)
	{
		/* TODO: Set Scale Names for channel */
	}

	EXCCHECK( EnabeCalib( gs_panel[ELMEAS], 1));

Error:     
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
				EXCCHECKCVI( SetCtrlVal( panel, ELMEAS_CALIB_FILE, file_name));
				EXCCHECK( ReadCalibFile( file_name));	
			}
			break;
	}

Error:   
	if( pexception)
	{
		SetCtrlVal( panel, ELMEAS_CALIB_FILE, "");
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
	
	EXCCHECKCVI( GetPanelAttribute (gs_panel[ELMEAS], ATTR_VISIBLE, &visible));	
	if(!visible)
	{
		EXCCHECK( EnabeControls( gs_panel[ELMEAS], 1));
		EXCCHECKCVI( DisplayPanel(gs_panel[ELMEAS]));
		OnMeasChanged(gs_panel[ELMEAS], ELMEAS_MEAS_1, EVENT_COMMIT, 0, 0, 0);
		checkErr( IviDmm_ConfigureMeasurement(gs_DMM, IVIDMM_VAL_DC_VOLTS, 25.0, 0.001));
		checkErr( IviDmm_SetAttributeViReal64(gs_DMM, "", IVIDMM_ATTR_POWERLINE_FREQ, 50.0));
	}
		
Error:      
	IVI_CHECK_ERR(error);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ClosePanel"
static SElExceptionPtr ClosePanel(void)          
{
	int32_t			error = 0;
   	SElExceptionPtr	pexception = NULL;
	int				visible;
	int32_t			enabled = 0; 
	
	EXCCHECKCVI( GetPanelAttribute (gs_panel[ELMEAS], ATTR_VISIBLE, &visible));
	if(visible)
	{
	 	EXCCHECKCVI( GetCtrlAttribute( gs_panel[ELMEAS], ELMEAS_TIMER, ATTR_ENABLED, &enabled));
		if(	enabled)
			EXCCHECKCVI( SetCtrlAttribute( gs_panel[ELMEAS], ELMEAS_TIMER, ATTR_ENABLED, VI_FALSE));

		EXCCHECKCVI( HidePanel(gs_panel[ELMEAS]));
		checkErr( IviDCPwr_ConfigureOutputEnabled(gs_POWER, "CHANNEL1", 0));
		checkErr( IviSwtch_DisconnectAll(gs_PSW));
		checkErr( IviSwtch_DisconnectAll(gs_SW5));		
	}
	
Error:     
	IVI_CHECK_ERR(error);
	EXCRETHROW( pexception);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPwrConfigure"
int CVICALLBACK OnPwrConfigure (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr  pexception = NULL;
	ViStatus		error = VI_SUCCESS;
	ViConstString	pchannel = "CHANNEL1";
	ViReal64		current = 0.5, voltage = 5.0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, ELMEAS_PWR_VOLTAGE, &voltage));
			EXCCHECKCVI( GetCtrlVal(panel, ELMEAS_PWR_CURRENT, &current));
				
			checkErr( IviDCPwr_ConfigureVoltageLevel (gs_POWER, pchannel, voltage));
			checkErr( IviDCPwr_ConfigureCurrentLimit (gs_POWER, pchannel, IVIDCPWR_VAL_CURRENT_REGULATE, current));
			checkErr( IviDCPwr_ConfigureOutputEnabled (gs_POWER, pchannel, VI_TRUE));
			break;
	}
Error:
	IVI_CHECK_ERR(error);
	EXCDISPLAY( pexception); 
	return 0;
}

/*--------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPwrDisable"
int CVICALLBACK OnPwrDisable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	ViStatus		error = VI_SUCCESS;
	ViConstString	pchannel = "CHANNEL1";
		
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( IviDCPwr_ConfigureOutputEnabled (gs_POWER, pchannel, VI_FALSE));
			break;
	}
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EnabeControls"
static SElExceptionPtr	EnabeControls( int panel, int state)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_START, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_STOP, ATTR_DIMMED, state));

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
	
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_SAVE, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_SAVE_CALIB, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_SCALE, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_MSG_CALIB_CHANNEL, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_CALIB_X1, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_CALIB_X2, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_CALIB_Y1, ATTR_DIMMED, !state));
	EXCCHECKCVI( SetCtrlAttribute( panel, ELMEAS_CALIB_Y2, ATTR_DIMMED, !state));

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
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
#define __FUNC__ "{Calibration Plugin}::Initialize"
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

	EXCCHECK( LoadPanelExWithLanguage(main_panel, "calibration_gui.uir", ELMEAS, gs_ELMEASlang, &gs_panel[ELMEAS])); 
	EXCCHECK( EnabeControls( gs_panel[ELMEAS], 1));
	EXCCHECK( EnabeCalib( gs_panel[ELMEAS], 0));
	//EXCCHECK( FillScale( gs_panel[ELMEAS]));
  
	EXCCHECKCVI( SetPanelAttribute (gs_panel[ELMEAS], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[ELMEAS], ATTR_LEFT, 0));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "EventFactory",
	                        sizeof(SElEventFactoryPtr),
	                        &gs_EventFactory));

	if(gs_Modules)
	{
		gs_IviBridge = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
		gs_calibration = (SCalibrationPtr)gs_Modules->Get(gs_Modules, MODULE_CALIBRATION);
	}
	
	if(gs_calibration==NULL)
		EXCTHROW( -1, _("Calibration Module Failed"));
	
	if(gs_IviBridge==NULL)
		EXCTHROW( -1, _("IVI-Bridge Module Failed"));
	
	EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "POWER", &gs_POWER));
	EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "PSW", &gs_PSW));
	EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "SW5", &gs_SW5));
	EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "DMM", &gs_DMM));
		
Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Calibration Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

	if( 0==strcmp( uuid, PLUGIN_CALIBRATION))
	{
		EXCCHECK( ShowPanel());
	}

Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Calibration Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
   
	EXCCHECK( ClosePanel());

Error:   
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Calibration Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
   
	if(gs_panel[ELMEAS])
		EXCCHECKCVI( DiscardPanel(gs_panel[ELMEAS]));

Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Calibration Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Calibration Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}