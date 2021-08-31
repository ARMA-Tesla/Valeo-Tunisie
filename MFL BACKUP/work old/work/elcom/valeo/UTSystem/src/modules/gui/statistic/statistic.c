#include <bklib/bklib2.h>
#include <guid.h>
#include <dbs/dbs.h> 
#include <objlist/objlist.h>
#include <eventfactory/EventFactory.h> 
#include <language/language.h>
#include <utility.h>
#include <time.h>
#include "statistic.h"
#include "statistic_plugin.h"         
#include "test_graph.h"
#include "kosu_graph.h"

const char* HISTORY = \
"[2007-11-23] rT 1.10\n"
" * Added calendar to kosu statistic.";

enum _ESMode
{
	ESM_NONE = 0,
	ESM_TEST_STAT,
	ESM_KOSU
} ESMode;

#define NB_OF_PANEL 		1
int							gs_panel[NB_OF_PANEL + 1];  
CAObjHandle					gs_treeHandle;
SDBSPtr    					gs_pDbs;
SDBSStoragePtr				gs_pStorage; 
SDBSKOSUPtr					gs_pKosu; 
SLanguagePtr				gs_pLanguage = NULL;
static SObjListPtr 			gs_Modules = NULL;     
static SElEventFactoryPtr	gs_EventFactory = NULL; 

void CVICALLBACK PostDeferredDisplayPanel (void *callbackData){DisplayPanel(gs_panel[STAT]);};

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CalendarCallback"
SElExceptionPtr CalendarCallback(int panel, int day, int month, int year)
{
	SElExceptionPtr pexception = NULL; 
	time_t			rawtime;
	struct tm*		timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month - 1;
	timeinfo->tm_mday = day;
	timeinfo->tm_hour = 0;
	timeinfo->tm_min = 0;
	timeinfo->tm_sec = 0;

	rawtime = mktime(timeinfo);

	if(ESMode == ESM_TEST_STAT)
	{
		//EXCCHECK( CalendarCallbackTest(rawtime));
	}
	else if(ESMode == ESM_KOSU) 
	{
		EXCCHECK( CalendarCallbackKosu(rawtime)); 
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClick"
static HRESULT CVICALLBACK BkTreeNodeClick (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode)
{
	SElExceptionPtr	pexception = NULL; 
	
	if(ESMode == ESM_TEST_STAT)
	{
		EXCCHECK( BkTreeTestNodeClick(keyNode));
	}
	else if(ESMode == ESM_KOSU) 
	{
		EXCCHECK( BkTreeKosuNodeClick(keyNode)); 
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
	int32_t				visible;

	if(0==strcmp(eventName, "EVNT_GUI_STATISTIC_TEST"))
	{
		ESMode = ESM_TEST_STAT; 
		EXCCHECK( StatisticTestGraph());  

		EXCCHECKCVI( GetPanelAttribute (gs_panel[STAT], ATTR_VISIBLE, &visible));
		if(!visible)
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, NULL));
	}
	else if(0==strcmp(eventName, "EVNT_GUI_STATISTIC_KOSU"))
	{
		ESMode = ESM_KOSU; 
		EXCCHECK( KosuGraph()); 
		
		EXCCHECKCVI( GetPanelAttribute (gs_panel[STAT], ATTR_VISIBLE, &visible));
		if(!visible)
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, NULL)); 
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
														 "EVNT_GUI_STATISTIC_KOSU",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         				"EVNT_GUI_STATISTIC_TEST",
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
#define __FUNC__ "{Statistic Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	const char*		pconfiguration_file = NULL;
	int32_t			main_panel = 0;
	int				monitor_id, top, height;  
	HRESULT			hr;

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "MainPanel",
									 sizeof(main_panel),
									 &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));
	
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

	EXCCHECKCVI( gs_panel[STAT] = LoadPanelEx (main_panel, "statistic.uir", STAT, __CVIUserHInst));

	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP, &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[STAT], ATTR_TOP, top + height - 80 - 754));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[STAT], ATTR_LEFT, 11));

	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[STAT], STAT_BKTREE, &gs_treeHandle));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_treeHandle, BkTreeNodeClick, NULL, 1, NULL));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "Modules",
									 sizeof(SObjListPtr),
									 &gs_Modules));
	
	if(gs_Modules)
	{
		gs_pDbs = (SDBSPtr)gs_Modules->Get(gs_Modules, MODULE_DBS);
		if(gs_pDbs)
		{
			gs_pStorage = gs_pDbs->Storage;
			gs_pKosu = gs_pDbs->Kosu;
		}
	}

	EXCCHECK( RegisterEventFunction());

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Statistic Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if( 0==strcmp( uuid, PLUGIN_STATISTIC))
	{
		ESMode = ESM_KOSU; 
		EXCCHECK( KosuGraph()); 

		EXCCHECKCVI( DisplayPanel(gs_panel[STAT]));	
	}

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Statistic Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( HidePanel(gs_panel[STAT]));

	if(ESMode==ESM_TEST_STAT)
		EXCCHECK( StatisticTestGraphClean());

	ESMode = ESM_NONE;

Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Statistic Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if ( gs_panel[STAT] )
		EXCCHECKCVI( DiscardPanel(gs_panel[STAT]));
	
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Statistic Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Statistic Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 
