#include <userint.h>
#include "panel_b2yy_mcp.h"
#include <eventfactory/EventFactory.h>
#include <utility.h>
#include <guid.h>
#include <test/testseq.h>
#include <jig/class/jig_class.h>
#include <IviBridge/ivibridge.h>
#include 	<resistor.h>
#include 	<IviSwtch.h>
#include 	<IviDCPwr.h>
#include 	<IviDmm.h>
#include 	<sensors/sensors.h>
#include 	<eldaq.h>
#include "panel_b2yy_mcp_plugin.h" 
#include "toolbox.h"
#include "lang_translation.h"

#define NUMBER_OF_PANELS 	2
#define MODULE_JIG			MODULE_JIG_B2YY_MCP /* guid.h */


#define SW4                         0
#define SW5                         1
#define DAQ                         2
#define DMM                         3
#define RESISTOR					4
#define POWER						5
#define PSW							6
#define PSW1						7
#define B2YY_IVI_SESSION_COUNT       8

static int					gs_panel[NUMBER_OF_PANELS+1];
static SJigPtr				gs_pjig = NULL;
SLanguagePtr 				gs_pLanguage;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL;  
static SPluginArgsPtr		gs_PluginArgs = NULL;
static STestSequencePtr		gs_TestSequence = NULL;
ViSession	gs_IviSessons[B2YY_IVI_SESSION_COUNT];

typedef enum _EPanelSwtch
{
	SWTCH_CONNECT = 0,
	SWTCH_DISCONNECT,
	SWTCH_HRW_LED_ON,
	SWTCH_HRW_LED_OFF,
	SWTCH_HFS_LED_ON,
	SWTCH_HFS_LED_OFF
} EPanelSwtch;


#define B1_REL( cid, chi) \
	{ EMCP_B1_##cid, "com"#chi, "ch"#chi }

#define B2_REL( cid, chi) \
	{ EMCP_B2_##cid, "com"#chi, "ch"#chi }

typedef struct _SReleyDefinition {
	int				ctrlId;
	const char*		ch1;
	const char*		ch2;
} SReleyDefinition, *SReleyDefinitionPtr;

static SReleyDefinition gs_b1_releys[] = {
	
	B1_REL( CH0, 0), 
	B1_REL( CH10, 10),
	B1_REL( CH20, 20),
	B1_REL( CH30, 30),
	{0, NULL, NULL}
};


static SReleyDefinition gs_b2_releys[] = {
	
	B2_REL( CH0, 0), 
	B2_REL( CH1, 1),
	B2_REL( CH2, 2),
	{0, NULL, NULL}
};

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "UpdateReleyStates"
static SElExceptionPtr UpdateReleyStates(int panel)
{
	SElExceptionPtr	pexception = NULL;
	int	i;
	ViStatus	error = VI_SUCCESS;
	ViInt32	state;

	if ( gs_Modules )
	{
		SIviBridgePtr  pIvi = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
		EXCCHECK( pIvi->GetSession( pIvi, "SW4", &gs_IviSessons[SW4]));
		EXCCHECK( pIvi->GetSession( pIvi, "SW5", &gs_IviSessons[SW5]));
		EXCCHECK( pIvi->GetSession( pIvi, "DAQ", &gs_IviSessons[DAQ]));
		EXCCHECK( pIvi->GetSession( pIvi, "DMM", &gs_IviSessons[DMM]));
		EXCCHECK( pIvi->GetSession( pIvi, "PSW", &gs_IviSessons[PSW]));
		EXCCHECK( pIvi->GetSession( pIvi, "PSW1", &gs_IviSessons[PSW1]));
		EXCCHECK( pIvi->GetSession( pIvi, "POWER", &gs_IviSessons[POWER]));
	}
	
	for (i = 0; gs_b1_releys[i].ch1; i++)
	{
		EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], gs_b1_releys[i].ch1, gs_b1_releys[i].ch2, &state));
		SetCtrlVal( panel, gs_b1_releys[i].ctrlId, (state == IVISWTCH_VAL_PATH_EXISTS));
	}
	
	for (i = 0; gs_b2_releys[i].ch1; i++)
	{
		EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW1], IviSwtch_CanConnect(gs_IviSessons[PSW1], gs_b2_releys[i].ch1, gs_b2_releys[i].ch2, &state));
		SetCtrlVal( panel, gs_b2_releys[i].ctrlId, (state == IVISWTCH_VAL_PATH_EXISTS));
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayPanel"
void CVICALLBACK PostDeferredDisplayPanel (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		DisplayPanel(*panel);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredHidePanel"
void CVICALLBACK PostDeferredHidePanel (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		HidePanel(*panel);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ShowPanel"
static SElExceptionPtr ShowPanel(void)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;

	EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[B2YY])); 
		
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
	
	EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[B2YY]));   
	
Error:      
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
static SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
   
	if(0==strcmp(eventName, "EVNT_JIG_LOADED"))
	{
		if(pData && 0==strcmp(((SElEventDataPtr)pData)->GetStringPtr(pData), MODULE_JIG))
			EXCCHECK( ShowPanel());
	}
	else if(0==strcmp(eventName, "EVNT_JIG_UNLOADED"))
	{
		if(pData && 0==strcmp(((SElEventDataPtr)pData)->GetStringPtr(pData), MODULE_JIG))
			EXCCHECK( ClosePanel());
	}   

Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
   int            error = 0;
   SElExceptionPtr   pexception = NULL;
   
   if ( gs_EventFactory )
   {
	  EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_JIG_LOADED",
                                         EventFunctionCallback));
	  EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_JIG_UNLOADED",
                                         EventFunctionCallback));
   }
   
Error:
   EXCRETHROW(pexception);
}   /* RegisterEventFunction */

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
#define __FUNC__ "{Panel B2YY Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr  	pexception = NULL;
	int32_t         	error = 0;
	int32_t         	main_panel = 0;

	gs_PluginArgs = a_pPluginArgs;

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
                            "ControlPanel",
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

	EXCCHECK( LoadPanelExWithLanguage(main_panel, "panel_b2yy_mcp.uir", B2YY, B2YYlang, &gs_panel[B2YY])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "panel_b2yy_mcp.uir", EMCP, EMCPlang, &gs_panel[EMCP])); 
	   
	EXCCHECKCVI( SetPanelAttribute (gs_panel[B2YY], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[B2YY], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[EMCP], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[EMCP], ATTR_LEFT, 0));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "EventFactory",
	                        sizeof(SElEventFactoryPtr),
	                        &gs_EventFactory));

	if(gs_Modules)
	{
		gs_pjig = (SJigPtr)gs_Modules->Get(gs_Modules, MODULE_JIG_CLASS);

		if(gs_pjig)
		{
			char uuid[256];
			EXCCHECK( gs_pjig->jig_GetUUID(gs_pjig, uuid, 255));

			if( 0==strcmp(uuid, MODULE_JIG))
				EXCCHECK( ShowPanel());
		}
	}

	EXCCHECK( RegisterEventFunction());  

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
   
Error:   
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr   pexception = NULL;
   	int32_t         error = 0;

	if(gs_panel[B2YY])
		EXCCHECKCVI( DiscardPanel(gs_panel[B2YY]));

Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif 

int CVICALLBACK OnBlower (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	ViStatus	error = VI_SUCCESS;
	SDIOSensorsPtr	psensors = NULL;
	
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			SetCtrlVal( panel, EMCP_BLOWER0, 0);
			SetCtrlVal( panel, EMCP_BLOWER1, 0);
			SetCtrlVal( panel, EMCP_BLOWER2, 0);
			SetCtrlVal( panel, EMCP_BLOWER3, 0);
			SetCtrlVal( panel, EMCP_BLOWER4, 0);
			SetCtrlVal( panel, control, 1);
			

			
			if ( gs_Modules )
				psensors = (SDIOSensorsPtr)gs_Modules->Get(gs_Modules, MODULE_SENSORS);
			if ( psensors )
			{
				EXCCHECK(psensors->SetState(psensors, "JIG_50", FALSE));
				EXCCHECK(psensors->SetState(psensors, "JIG_51", FALSE));
				EXCCHECK(psensors->SetState(psensors, "JIG_52", FALSE));
				EXCCHECK(psensors->SetState(psensors, "JIG_53", FALSE));
			
				switch ( control )
				{
				case EMCP_BLOWER0:	
					break;
				case EMCP_BLOWER1:
					EXCCHECK(psensors->SetState(psensors, "JIG_50", TRUE));
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW5], IviSwtch_DisconnectAll( gs_IviSessons[SW5]));
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW5], IviSwtch_Connect( gs_IviSessons[SW5], "ch4", "com0"));
					EXCCHECKIVI( IviDmm, gs_IviSessons[DMM], IviDmm_ConfigureMeasurement(gs_IviSessons[DMM], IVIDMM_VAL_DC_VOLTS, 200.0e-3, 0.001));
					break;
				case EMCP_BLOWER2:
					EXCCHECK(psensors->SetState(psensors, "JIG_51", TRUE));
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW5], IviSwtch_DisconnectAll( gs_IviSessons[SW5]));
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW5], IviSwtch_Connect( gs_IviSessons[SW5], "ch5", "com0"));
					EXCCHECKIVI( IviDmm, gs_IviSessons[DMM], IviDmm_ConfigureMeasurement(gs_IviSessons[DMM], IVIDMM_VAL_DC_VOLTS, 200.0e-3, 0.001));
					break;
				case EMCP_BLOWER3:
					EXCCHECK(psensors->SetState(psensors, "JIG_52", TRUE));
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW5], IviSwtch_DisconnectAll( gs_IviSessons[SW5]));
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW5], IviSwtch_Connect( gs_IviSessons[SW5], "ch6", "com0"));
					EXCCHECKIVI( IviDmm, gs_IviSessons[DMM], IviDmm_ConfigureMeasurement(gs_IviSessons[DMM], IVIDMM_VAL_DC_VOLTS, 200.0e-3, 0.001));
					break;
				case EMCP_BLOWER4:
					EXCCHECK(psensors->SetState(psensors, "JIG_53", TRUE));
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW5], IviSwtch_DisconnectAll( gs_IviSessons[SW5]));
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW5], IviSwtch_Connect( gs_IviSessons[SW5], "ch7", "com0"));
					EXCCHECKIVI( IviDmm, gs_IviSessons[DMM], IviDmm_ConfigureMeasurement(gs_IviSessons[DMM], IVIDMM_VAL_DC_VOLTS, 200.0e-3, 0.001));
					break;
				}
			}
			if ( control != EMCP_BLOWER_OFF)
				SetCtrlVal( panel, EMCP_BLOWER_OFF, 0);
			
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPower"
int CVICALLBACK OnPower (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnB2Reley"
int CVICALLBACK OnB2Reley (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int			value, i;
	ViStatus	error = VI_SUCCESS;
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetCtrlVal( panel, control, &value);
			for (i = 0; gs_b2_releys[i].ch1; i++)
			{
				if (gs_b2_releys[i].ctrlId == control ) break;	
			}
			if ( gs_b2_releys[i].ch1 )
			{
				if ( value )
					EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW1], IviSwtch_Disconnect(gs_IviSessons[PSW1], gs_b2_releys[i].ch1, gs_b2_releys[i].ch2));
				else
					EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW1], IviSwtch_Connect(gs_IviSessons[PSW1], gs_b2_releys[i].ch1, gs_b2_releys[i].ch2));
				SetCtrlVal( panel, control, !value);
			}
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnB1Reley"
int CVICALLBACK OnB1Reley (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int			value, i;
	ViStatus	error = VI_SUCCESS;
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetCtrlVal( panel, control, &value);
			for (i = 0; gs_b1_releys[i].ch1; i++)
			{
				if (gs_b1_releys[i].ctrlId == control ) break;	
			}
			if ( gs_b1_releys[i].ch1 )
			{
				if ( value )
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[SW4], gs_b1_releys[i].ch1, gs_b1_releys[i].ch2));
				else
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[SW4], gs_b1_releys[i].ch1, gs_b1_releys[i].ch2));
				SetCtrlVal( panel, control, !value);
			
				if ( control == EMCP_B1_CH30 ) SetCtrlVal( panel, EMCP_LED_HRW, !value);
				if ( control == EMCP_B1_CH10 ) SetCtrlVal( panel, EMCP_LED_HFS, !value);
			}

			break;
	}
Error:
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
	ViInt32	error = 0;
	ViReal64	value = 0.0;
	int			boff;
	ViReal64	voltage, current, resistance;
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai0", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EMCP_AI0, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai1", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EMCP_AI1, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai2", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EMCP_AI2, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai3", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EMCP_AI3, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai4", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EMCP_AI4, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai5", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EMCP_AI5, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai6", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EMCP_AI6, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai7", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EMCP_AI7, value);
			GetCtrlVal(panel, EMCP_BLOWER_OFF, &boff);
			
			if ( !boff )
			{
				EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_Measure( gs_IviSessons[POWER], "CHANNEL1", IVIDCPWR_VAL_MEASURE_CURRENT, &current));
				if ( current < 2.0e-3) current = 0.0;
				EXCCHECKIVI( IviDmm, gs_IviSessons[DMM], IviDmm_Read (gs_IviSessons[DMM], 2000, &voltage));
				if ( IsNotANumber(voltage) ) 
					EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_Measure( gs_IviSessons[POWER], "CHANNEL1", IVIDCPWR_VAL_MEASURE_VOLTAGE, &voltage));
				resistance = (current < 1.0e-3) ? 9.9e39 : voltage / current;
				SetCtrlVal( panel, EMCP_BLOWER_CURRENT, current);
				SetCtrlVal( panel, EMCP_BLOWER_VOLTAGE, voltage);
				SetCtrlVal( panel, EMCP_BLOWER_RESISTANCE, resistance);
			}
			break;
	}
Error:
	EXCDELETE( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnEnable"
int CVICALLBACK OnEnable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	ViInt32			state;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[EMCP]));
			EXCCHECK( UpdateReleyStates( gs_panel[EMCP]));
			EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_ConfigureOutputRange(gs_IviSessons[POWER], "1", IVIDCPWR_VAL_RANGE_VOLTAGE, 20.0));
			EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_ConfigureVoltageLevel(gs_IviSessons[POWER], "1", 13.5));
			EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_ConfigureCurrentLimit(gs_IviSessons[POWER], "1", IVIDCPWR_VAL_CURRENT_REGULATE, 1.0));
			EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_ConfigureOutputEnabled(gs_IviSessons[POWER], "1", VI_TRUE));
			EXCCHECKCVI( SetCtrlVal( gs_panel[EMCP], EMCP_POWER, 13.5));
			EXCCHECKCVI( SetCtrlVal( gs_panel[EMCP], EMCP_CURRENT_LIMT, 3.0));
			EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW], IviSwtch_CanConnect(gs_IviSessons[PSW], "CH1A", "CH1B", &state));
			if (state == IVISWTCH_VAL_PATH_AVAILABLE)
				EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW], IviSwtch_Connect(gs_IviSessons[PSW], "CH1A", "CH1B"));
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ConfigureMeasurement (gs_IviSessons[DAQ], "", 2000, 0.02));
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_StartMeasurement( gs_IviSessons[DAQ]));
			EXCCHECKCVI(SetCtrlAttribute(gs_panel[EMCP], EMCP_TIMER, ATTR_ENABLED, 1));
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnDisable"
int CVICALLBACK OnDisable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0, i;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI(SetCtrlAttribute(gs_panel[EMCP], EMCP_TIMER, ATTR_ENABLED, 0));
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_StopMeasurement( gs_IviSessons[DAQ]));
			for (i = 0; gs_b1_releys[i].ch1; i++)
			{
				error = IviSwtch_Disconnect(gs_IviSessons[SW4], gs_b1_releys[i].ch1, gs_b1_releys[i].ch2);
				if ( error && error != 0xBFFA2011 )
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], error);
				SetCtrlVal( panel, gs_b1_releys[i].ctrlId, 0);
			}

			for (i = 0; gs_b2_releys[i].ch1; i++)
			{
				error = IviSwtch_Disconnect(gs_IviSessons[PSW1], gs_b2_releys[i].ch1, gs_b2_releys[i].ch2);
				if ( error && error != 0xBFFA2011 )
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], error);
				SetCtrlVal( panel, gs_b2_releys[i].ctrlId, 0);
			}

			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[EMCP]));
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}
