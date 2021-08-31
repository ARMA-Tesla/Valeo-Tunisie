#include <userint.h>
#include "kosu.h"
#include <dbs/dbs.h> 
#include <objlist/objlist.h>
#include <eventfactory/EventFactory.h> 
#include <language/language.h>
#include <utility.h> 
#include <guid.h>
#include "kosu_plugin.h" 
#include <ansi_c.h>
#include "trans_language.h"

#define NB_OF_PANEL 		1
static int					gs_panel[NB_OF_PANEL + 1];
static bool_t				gs_pause_active;
static SElEventFactoryPtr	gs_EventFactory = NULL; 
static SObjListPtr 			gs_Modules = NULL;  
static SDBSPtr				gs_pDbs = NULL;
static SDBSKOSUPtr			gs_pKosu = NULL;
SLanguagePtr				gs_pLanguage = NULL; 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnResetCounter"
int CVICALLBACK OnResetCounter (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	int32_t			Selected = 0;

	switch (event)
	{																					
		case EVENT_COMMIT:

		
		Selected = ConfirmPopup ("Reset Counter!", "Confirm Reset");
			
		if (Selected == 1)
		{
		/* reset counter */
		if(gs_EventFactory)
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_GUI_RESET_COUNTER", NULL));
		}
		
		break;
	}
Error:
	return 0;
}
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

			if(gs_pause_active)
			{
				EXCCHECKCVI( GetCtrlVal(panel, PANEL_PAUSE_START, &start_time));
		
				time = Timer() - start_time;
		
				sprintf( time_string, "%02d:%02d", (int32_t) (time / 60.0),
						(int32_t) (fmod(time, 60)));
		
				//EXCCHECKCVI( SetCtrlVal(panel, PANEL_PAUSE_TIME, time_string));
			}
		
			break;
	}
	
Error:
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "TimeToString"
static void TimeToString(uint32_t itime, char* stime)
{
   int32_t	error = 0;
   int		h = 0,m = 0,s = 0;  
   
   h =  itime / 3600;
   m = (itime - 3600*h) / 60;
   s = (itime - 3600*h - m*60);
   
   if ( h > 0 )
      sprintf( stime, "%d:%02d:%02d", h, m, s); 
   else
      sprintf( stime, "%02d:%02d", m, s); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "KosuUpdate"
SElExceptionPtr KosuUpdate( void)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
	uint32_t		PANEL_actual_good; 
    uint32_t		PANEL_actual_bad;
    uint32_t		PANEL_actual_retest_good;
    uint32_t		PANEL_actual_retest_bad;
    uint32_t		PANEL_required; 
    uint32_t		PANEL_END_actual;
    uint32_t		PANEL_END_required; 
    uint32_t		KOSU_actual;
    uint32_t		KOSU_required;  
    uint32_t		USER_count;  
    uint32_t		PAUSE_time;
	char      		buffer[128];
	
	if(gs_pKosu)
	{
		
	//	printf("gs_pKosu");
		 pexception = gs_pKosu->GetKOSU( gs_pKosu,
					                    &PANEL_actual_good,
									    &PANEL_actual_bad,
									    &PANEL_actual_retest_good,
									    &PANEL_actual_retest_bad,
									    &PANEL_required, 
									    &PANEL_END_actual,
									    &PANEL_END_required, 
									    &KOSU_actual,
									    &KOSU_required,  
									    &USER_count,  
									    &PAUSE_time);  
		
		if(pexception==NULL)
		{
			sprintf(buffer, "%d", PANEL_actual_good);
		//	printf("PANEL_actual_good %d \n\r", PANEL_actual_good); 
		//	printf("PANEL_NUM_GOOD %d \n\r", PANEL_NUM_GOOD);
			
			EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_GOOD, buffer)); 
			
			sprintf(buffer, "%d", PANEL_actual_bad); 
			
		//	printf("PANEL_actual_bad %d \n\r", PANEL_actual_bad);
		//	printf("PANEL_NUM_BAD %d \n\r", PANEL_NUM_BAD);
			EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_BAD, buffer));  
			
			sprintf(buffer, "%d", PANEL_actual_retest_good);
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_GOOD_RETEST, buffer)); 
			
			sprintf(buffer, "%d", PANEL_actual_retest_bad);        
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_BAD_RETEST, buffer));  
			
		    sprintf(buffer, "%d", USER_count);    
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_OPERATOR, buffer));  
			
		    sprintf(buffer, "%d", PANEL_required);  
	 		//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_PANEL, buffer)); 
			
		    sprintf(buffer, "%d", PANEL_actual_good);   
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_PANEL_2, buffer)); 
			
			sprintf(buffer, "%d", PANEL_END_required);
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_END, buffer)); 
			
		    sprintf(buffer, "%d", PANEL_END_actual);   
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_END_2, buffer)); 
		
			sprintf(buffer, "%d", KOSU_required);
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_KOSU_TIME, buffer)); 
			 
			sprintf(buffer, "%d", KOSU_actual);
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_KOSU_TIME_2, buffer)); 
			
			TimeToString(PAUSE_time, buffer);
			//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_PAUSE_TIME, buffer));
		}
		else
		{
			EXCDBGSTR( pexception);	
		}
	}
	
Error:                       
	EXCRETHROW( pexception);
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
		gs_pause_active = TRUE;
		//EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_PAUSE_TIME, "00:00"));   
		EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_PAUSE_START, Timer()-1));
		EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PANEL_PAUSE_PICT, ATTR_VISIBLE, TRUE)); 

		/* hide operators */
		//EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PANEL_BCKPICT_4, ATTR_VISIBLE, FALSE)); 
		//EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PANEL_NUM_OPERATOR, ATTR_VISIBLE, FALSE)); 
		//EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PANEL_MSG_OPERATOR, ATTR_VISIBLE, FALSE)); 
	}
	else if(0==strcmp(eventName, "EVNT_PAUSE_STOPED"))
	{
		gs_pause_active = FALSE;  
		EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PANEL_PAUSE_PICT, ATTR_VISIBLE, FALSE)); 

		/* visible operators */
		//EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PANEL_BCKPICT_4, ATTR_VISIBLE, TRUE)); 
		//EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PANEL_NUM_OPERATOR, ATTR_VISIBLE, TRUE)); 
		//EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PANEL_MSG_OPERATOR, ATTR_VISIBLE, TRUE)); 
	}
	else if(0==strcmp(eventName, "EVNT_KOSU_UPDATE"))
	{
		EXCCHECK( KosuUpdate());
	}
	
Error:                       
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
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
														 "EVNT_KOSU_UPDATE",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_RESET_COUNTER",
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
#define __FUNC__ "{Kosu Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	const char*		pconfiguration_file = NULL;
	int32_t			main_panel = 0;
	int				monitor_id, top, height;  

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "MainPanel",
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
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel,"kosu.uir", PANEL, KOSUlang, &gs_panel[PANEL]));
	
	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP, &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_TOP, top + height - 718));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_LEFT, 1050-197));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));

	if(gs_Modules)
	{
		gs_pDbs = (SDBSPtr)gs_Modules->Get(gs_Modules, MODULE_DBS); 
		if(gs_pDbs)
		{
			if(gs_pDbs) gs_pKosu = gs_pDbs->Kosu;
		}
	}
	
	EXCCHECK( RegisterEventFunction());

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Kosu Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	EXCCHECK( KosuUpdate());
	EXCCHECKCVI( DisplayPanel(gs_panel[PANEL]));  
		
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Kosu Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Kosu Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if ( gs_panel[PANEL] )
		DiscardPanel(gs_panel[PANEL]);
	
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Kosu Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Kosu Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 
