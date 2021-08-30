#include <exception/Exception.h>
#include <eventfactory/EventFactory.h> 
#include <guid.h>
#include <utility.h>
#include <language/language.h>
#include "info_plugin.h"
#include <ansi_c.h>
#include "trans_language.h"

#define NB_OF_PANEL 		1  
static int					gs_panel[NB_OF_PANEL + 1];
static SElEventFactoryPtr	gs_EventFactory = NULL;  
static SObjListPtr 			gs_Modules = NULL;  
SLanguagePtr				gs_pLanguage = NULL; 

void CVICALLBACK PostDeferredDisplayPanel (void *callbackData) { DisplayPanel(gs_panel[PANEL]);}
void CVICALLBACK PostDeferredHidePanel (void *callbackData) { HidePanel(gs_panel[PANEL]);}
		
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTimerTick"
int CVICALLBACK OnTimerTick (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	int				hour, min, sec;
	char			buffer[256];
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			EXCCHECKCVI( GetSystemTime(&hour, &min, &sec));
			sprintf(buffer, "%02d:%02d:%02d", hour, min, sec);
			EXCCHECKCVI( SetCtrlVal( panel, PANEL_MSG_TIME, buffer)); 
			break;
	}
	
Error:                       
	/* EXCDISPLAY( pexception); */
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	
	if(0==strcmp(eventName, "EVNT_GUI_INFO_SET"))
	{
		if(pData)
		{
			const char* setting = ((SElEventDataPtr)pData)->GetStringPtr(pData);
			char* pdata = NULL;

			if(0==strncmp(setting, "PANEL_MSG_SHIFT", strlen("PANEL_MSG_SHIFT")))
			{
				pdata = (char*)setting + strlen("PANEL_MSG_SHIFT:");
				EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_MSG_SHIFT, gettext(pdata)));
			}
			else if(0==strncmp(setting, "PANEL_MSG_VERSION", strlen("PANEL_MSG_VERSION")))
			{
				pdata = (char*)setting + strlen("PANEL_MSG_VERSION:");
				EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_MSG_VERSION, pdata));
			}
			else if(0==strncmp(setting, "PANEL_MSG_ADMIN", strlen("PANEL_MSG_ADMIN")))
			{
				pdata = (char*)setting + strlen("PANEL_MSG_ADMIN:");
				EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_MSG_ADMIN, gettext(pdata)));
			}
			else if(0==strncmp(setting, "PANEL_MSG_TITLE", strlen("PANEL_MSG_TITLE")))
			{
				pdata = (char*)setting + strlen("PANEL_MSG_TITLE:");
				EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_MSG_TITLE, pdata));
			}
		}
	}

	
Error:                       
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Database Plugin}::RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
   int            error = 0;
   SElExceptionPtr   pexception = NULL;
   
   if ( gs_EventFactory )
   {
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
		                                   "EVNT_GUI_INFO_SET",
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
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)		
				return 0;	/* out of memory */
			break;
		case DLL_PROCESS_DETACH:
			if (!CVIRTEHasBeenDetached())		/* Do not call CVI functions if cvirte.dll has already been detached. */
			{
				DiscardUIObjectsForDLL ();		/* Discard the panels loaded in InitUIForDLL */
				CloseCVIRTE ();		
			}
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
#define __FUNC__ "{Info Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
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

	EXCCHECK( LoadPanelExWithLanguage(main_panel, "info.uir", PANEL, INFOlang, &gs_panel[PANEL]));
	
	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP,    &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_TOP, 100));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_LEFT, -150));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
	EXCCHECK( RegisterEventFunction()); 

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Info Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
		
	EXCCHECKCVI( DisplayPanel(gs_panel[PANEL]));

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Info Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Info Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( DiscardPanel(gs_panel[PANEL]));

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Info Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Info Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 



