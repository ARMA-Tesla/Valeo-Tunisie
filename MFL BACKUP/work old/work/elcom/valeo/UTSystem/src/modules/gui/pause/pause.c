#include <guid.h>
#include <dbs/dbs.h> 
#include <objlist/objlist.h>
#include <eventfactory/EventFactory.h> 
#include <utility.h> 
#include <language/language.h>
#include <math.h>
#include "pause_plugin.h" 
#include <ansi_c.h>
#include "trans_language.h"

#define HIGHLIGHT_TOP 		219
#define HIGHLIGHT_SHIFT		60

#define NB_OF_PANEL 		1
static int					gs_panel[NB_OF_PANEL + 1];  
static int32_t 				gs_control_actual; 
static bool_t 				gs_pause_selected = FALSE; 
static bool_t				gs_pause_active = FALSE;
static SElEventFactoryPtr	gs_EventFactory = NULL; 
static SObjListPtr          gs_Modules = NULL; 
SLanguagePtr				gs_pLanguage = NULL;

static int32_t	gs_control[] = 
{
	PAUSE_BUTTON_1,
	PAUSE_BUTTON_2,
	PAUSE_BUTTON_3,
	PAUSE_BUTTON_4
};

void CVICALLBACK PostDeferredDisplayPanel (void *callbackData) { DisplayPanel(gs_panel[PAUSE]);}
void CVICALLBACK PostDeferredHidePanel (void *callbackData) { HidePanel(gs_panel[PAUSE]);}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTimerTick"
int CVICALLBACK OnTimerTick (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	double			start_time = 0.0,
					time = 0.0;
	char			time_string[10] = "";
	
	switch (event)
	{
		case EVENT_TIMER_TICK:

			EXCCHECKCVI( GetCtrlVal(panel, PAUSE_START, &start_time));
		
			time = Timer() - start_time;
		
			sprintf( time_string, "%02d:%02d", (int32_t) (time / 60.0),
					(int32_t) (fmod(time, 60.0)));
		
			EXCCHECKCVI( SetCtrlVal(panel, PAUSE_TIME, time_string));
		
			break;
	}
	
Error:
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PauseInit"
static SElExceptionPtr PauseInit(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				visible; 
	
	EXCCHECKCVI( GetPanelAttribute(gs_panel[PAUSE], ATTR_VISIBLE, &visible));
	if(!visible)
	{
		gs_control_actual = 0;
		EXCCHECKCVI( SetCtrlVal(gs_panel[PAUSE], PAUSE_TIME, "00:00"));   
		EXCCHECKCVI( SetCtrlVal(gs_panel[PAUSE], PAUSE_START, Timer()-1));

		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PAUSE], PAUSE_BUTTON_1, ATTR_DIMMED, FALSE));
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PAUSE], PAUSE_BUTTON_2, ATTR_DIMMED, FALSE));
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PAUSE], PAUSE_BUTTON_3, ATTR_DIMMED, FALSE));
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PAUSE], PAUSE_BUTTON_4, ATTR_DIMMED, FALSE));

		gs_pause_selected = FALSE;

		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PAUSE], PAUSE_HIGHLIGHT, ATTR_TOP, 
							HIGHLIGHT_TOP + HIGHLIGHT_SHIFT*0)); 

		EXCCHECKCVI( PostDeferredCall(PostDeferredDisplayPanel, NULL));

		gs_pause_active = TRUE;
	}
	
Error:
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PauseOpen"
static SElExceptionPtr PauseOpen(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				visible;
	
	if(gs_pause_active)
	{
		EXCCHECKCVI( GetPanelAttribute(gs_panel[PAUSE], ATTR_VISIBLE, &visible));
		if(!visible)
		{
			EXCCHECKCVI( PostDeferredCall(PostDeferredDisplayPanel, NULL)); 
		}
	}
	
Error:
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PauseClose"
static SElExceptionPtr PauseClose(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				visible;
	
	EXCCHECKCVI( GetPanelAttribute(gs_panel[PAUSE], ATTR_VISIBLE, &visible));
	if(visible)
	{
		EXCCHECKCVI( PostDeferredCall(PostDeferredHidePanel, NULL)); 
	}
	
Error:
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPauseSelect"
int CVICALLBACK OnPauseSelect (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;    
	int32_t			error = 0;
	int				dimmed = 0;
	int				pause_id = 0;
	SElEventDataPtr	pevent_data = NULL;  
	char			buffer[256];
	
	switch (event)
	{
		case EVENT_COMMIT:
		case EVENT_LEFT_CLICK:
			if( !gs_pause_selected)
			{
				switch (control)
				{
					case PAUSE_BUTTON_1: /* operator pause */
						EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_HIGHLIGHT, ATTR_TOP, 
										 HIGHLIGHT_TOP + HIGHLIGHT_SHIFT*0)); 
						dimmed = 1<<0;
						pause_id = ID_PAUSE_OPERATOR;
						break;
					case PAUSE_BUTTON_2: /* planned downtime */
						EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_HIGHLIGHT, ATTR_TOP, 
										 HIGHLIGHT_TOP + HIGHLIGHT_SHIFT*1));
						dimmed = 1<<1;
						pause_id = ID_PAUSE_DOWNTIME;
						break;
					case PAUSE_BUTTON_3: /* quality */
						EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_HIGHLIGHT, ATTR_TOP, 
										 HIGHLIGHT_TOP + HIGHLIGHT_SHIFT*2));
						dimmed = 1<<2;
						pause_id = ID_PAUSE_QUALITY;
						break;
					case PAUSE_BUTTON_4: /* maintenance */
						EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_HIGHLIGHT, ATTR_TOP, 
										 HIGHLIGHT_TOP + HIGHLIGHT_SHIFT*3));
						dimmed = 1<<3;
						pause_id = ID_PAUSE_MAINTENANCE;
						break;
				}
				
				EXCCHECK( eleventdata_new(&pevent_data)); 
				sprintf(buffer, "%d", pause_id);
				EXCCHECK( pevent_data->SetStringPtr(pevent_data, buffer));
				EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_PAUSE_SELECTED", pevent_data));
				
				EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_BUTTON_1, ATTR_DIMMED, (dimmed&(1<<0))==0));
				EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_BUTTON_2, ATTR_DIMMED, (dimmed&(1<<1))==0));
				EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_BUTTON_3, ATTR_DIMMED, (dimmed&(1<<2))==0));
				EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_BUTTON_4, ATTR_DIMMED, (dimmed&(1<<3))==0));
				gs_pause_selected = TRUE;  
			}
			break;     
	}

Error:
	eleventdata_delete(&pevent_data); 
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPausePanel"
int  CVICALLBACK OnPausePanel(int panel, int event, 
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;  
	int32_t			control[ID_PAUSE_COUNT-1] = {PAUSE_BUTTON_1,
							  	  PAUSE_BUTTON_2,
							  	  PAUSE_BUTTON_3,
							  	  PAUSE_BUTTON_4};
		
	switch (event)
	{
		case EVENT_KEYPRESS:
			
			switch(eventData1)
			{
				case VAL_TAB_VKEY:				
				case VAL_DOWN_ARROW_VKEY:
					if(!gs_pause_selected)
					{
						gs_control_actual++;   
					
						if(gs_control_actual>=ID_PAUSE_COUNT-1)
							gs_control_actual = 0;
					
						EXCCHECKCVI( SetActiveCtrl (panel, control[gs_control_actual]));
					
						EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_HIGHLIGHT, ATTR_TOP, 
										 HIGHLIGHT_TOP + HIGHLIGHT_SHIFT*gs_control_actual));
					}
					break;
				case VAL_UP_ARROW_VKEY:
					if(!gs_pause_selected)
					{
						gs_control_actual--;
					
						if(gs_control_actual<0)
							gs_control_actual = ID_PAUSE_COUNT-1;
					
						EXCCHECKCVI( SetActiveCtrl (panel, control[gs_control_actual]));
					
						EXCCHECKCVI( SetCtrlAttribute(panel, PAUSE_HIGHLIGHT, ATTR_TOP, 
										 HIGHLIGHT_TOP + HIGHLIGHT_SHIFT*gs_control_actual));
					}
					break;
				case VAL_ESC_VKEY:
					break;
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	int32_t			   	error = 0;
	SElExceptionPtr    	pexception = NULL;

	if(0==strcmp(eventName, "EVNT_PAUSE_STARTED"))
	{
		EXCCHECK( PauseInit());
	}
	else if( 0==strcmp(eventName, "EVNT_PAUSE_STOPED"))
	{
		gs_pause_active = FALSE;
		EXCCHECK( PauseClose());
	}
	else if( 0==strcmp(eventName, "EVNT_GUI_PRODUCTION"))
	{
		EXCCHECK( PauseOpen());
	}
	else if( 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_STEP_MODE")
		  || 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_GO_MODE")
		  || 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_RNR")
		  || 0==strcmp(eventName, "EVNT_GUI_QUALITY_MASTER")
		  || 0==strcmp(eventName, "EVNT_GUI_QUALITY_RETEST"))
	{
		EXCCHECK( PauseClose());
	}
	else if( 0==strcmp(eventName, "EVNT_TESTER_BUTTON_PRESSED_OK"))
	{
		if(gs_pause_active && !gs_pause_selected)
			OnPauseSelect( gs_panel[PAUSE], gs_control[gs_control_actual], EVENT_LEFT_CLICK, NULL, 0, 0);
	}
	else if( 0==strcmp(eventName, "EVNT_TESTER_BUTTON_PRESSED_DOWNTIME"))
	{
		if(gs_pause_active)
			OnPausePanel( gs_panel[PAUSE], EVENT_KEYPRESS, NULL, VAL_DOWN_ARROW_VKEY, 0);
	}
	
Error:                       
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
	int				error = 0;
	SElExceptionPtr	pexception = NULL;
	
	if ( gs_EventFactory )
	{
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_PAUSE_STARTED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_PAUSE_STOPED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_EXPERTISE_STEP_MODE",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_EXPERTISE_GO_MODE",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_EXPERTISE_RNR",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_QUALITY_MASTER",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_QUALITY_RETEST",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_PRODUCTION",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
		                                   "EVNT_TESTER_BUTTON_PRESSED_OK",
		                                   EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
		                                   "EVNT_TESTER_BUTTON_PRESSED_DOWNTIME",
		                                   EventFunctionCallback));
	}

Error:
	EXCRETHROW(pexception);
}	/* RegisterEventFunction */

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
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)		
				return 0;	/* out of memory */
			break;
		case DLL_PROCESS_DETACH:
			if (!CVIRTEHasBeenDetached())		/* Do not call CVI functions if cvirte.dll has already been detached. */
			{
				DiscardUIObjectsForDLL ();		/* Discard the panels loaded in InitUIForDLL */
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
#define __FUNC__ "{Pause Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	const char*		pconfiguration_file = NULL;
	int32_t			main_panel = 0;
	int				monitor_id, top, height;  	

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "Modules",
	                        sizeof(SObjListPtr),
	                        &gs_Modules));

	/*Gettext*/
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
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "MainPanel",
									 sizeof(main_panel),
									 &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel,    "pause.uir", PAUSE, PAUSElang, &gs_panel[PAUSE]));
	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP,    &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	
	//EXCCHECKCVI( SetPanelAttribute (gs_panel[PAUSE], ATTR_TOP, top + height - 80 - 754));
	  EXCCHECKCVI( SetPanelAttribute (gs_panel[PAUSE], ATTR_TOP, 100));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PAUSE], ATTR_LEFT, 11));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
	EXCCHECK( RegisterEventFunction());

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Pause Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	
//	EXCCHECKCVI( DisplayPanel(gs_panel[PAUSE]));
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Pause Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Pause Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if ( gs_panel[PAUSE] )
		EXCCHECKCVI( DiscardPanel(gs_panel[PAUSE]));
	
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Pause Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Pause Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 
