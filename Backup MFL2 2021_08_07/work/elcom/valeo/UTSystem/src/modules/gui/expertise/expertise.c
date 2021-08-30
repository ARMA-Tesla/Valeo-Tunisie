#include <msxmldom.h>
#include <eventfactory/EventFactory.h> 
#include <guid.h>
#include <utility.h>
#include <language/language.h>
#include "expertise_plugin.h"
#include <ansi_c.h>
#include "trans_language.h"

const char* HISTORY = \
"[2018-08-27] M.A 1.00\n"
" * creation \n";


#define NB_OF_PANEL 		1 
#define NB_OF_STATIONS 		8 

static int					gs_panel[NB_OF_PANEL + 1];
static SElEventFactoryPtr	gs_EventFactory = NULL;  
static SObjListPtr 			gs_Modules = NULL;  
SLanguagePtr				gs_pLanguage = NULL;

static int gs_CheckStation[NB_OF_STATIONS] = 
{ EXPERTISE_CHECKBOX_STATION_10,
  EXPERTISE_CHECKBOX_STATION_20,
  EXPERTISE_CHECKBOX_STATION_30,
  EXPERTISE_CHECKBOX_STATION_40,
  EXPERTISE_CHECKBOX_STATION_50,
  EXPERTISE_CHECKBOX_STATION_60,
  EXPERTISE_CHECKBOX_STATION_70,
  EXPERTISE_CHECKBOX_STATION_80
};

 static int gs_RepeatStation[NB_OF_STATIONS] = 
{ EXPERTISE_REPEAT_STATION_10,
  EXPERTISE_REPEAT_STATION_20,
  EXPERTISE_REPEAT_STATION_30,
  EXPERTISE_REPEAT_STATION_40,
  EXPERTISE_REPEAT_STATION_50,
  EXPERTISE_REPEAT_STATION_60,
  EXPERTISE_REPEAT_STATION_70,
  EXPERTISE_REPEAT_STATION_80
};

int gs_repeat;

static SElExceptionPtr LoadTestSequence(const char* fileXml);

void CVICALLBACK PostDeferredHidePanel (void *callbackData) { HidePanel(gs_panel[EXPERTISE]);}



/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnButton"
int CVICALLBACK OnButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
   	int32_t			i = 0;
 	char Msg[32];
	
	switch (event)
	{
		case EVENT_COMMIT:

			for (i=0; i<NB_OF_STATIONS; i++)
			{
			if (gs_CheckStation[i] != control)
				EXCCHECKCVI( SetCtrlVal (panel, gs_CheckStation[i], 0));  
			}
			
			for (i=0; i<NB_OF_STATIONS; i++)
			{
				if (gs_CheckStation[i] == control)
				{
					sprintf (Msg, "%d", gs_repeat);
					EXCCHECKCVI( SetCtrlVal (panel, gs_RepeatStation[i], Msg));  
				}
				else 
				{
					EXCCHECKCVI( SetCtrlVal (panel, gs_RepeatStation[i], "00"));  
				}
			}
			break;
	}
	
Error:
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadConfiguration"
static SElExceptionPtr LoadConfiguration(
   const char*   fileName
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	char         	_message[512];
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;   
	const char*     pfile_name = (fileName) ? fileName : "\\expertise.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*         	ptext= NULL;
	char			path[MAX_PATHNAME_LEN];
	int				i;

	EXCCHECKCVI( GetModuleDir (__CVIUserHInst, path)); 
	strcat(path, pfile_name);
	
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, path));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//plugin[@id='"PLUGIN_EXPERTISE"']", &pnode));   
		
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "repeat", &pparameter));
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			gs_repeat = strtol(ptext, NULL, 10);
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

Error:
	CA_VariantClear(&pfile_name_var);
	if (pparameter) CA_DiscardObjHandle(pparameter);    
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
static SElExceptionPtr ShowPanel(void) 
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	char			path[MAX_PATHNAME_LEN] = "";

	EXCCHECKCVI( DisplayPanel(gs_panel[EXPERTISE]));

Error:                       
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
static SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	char			path[MAX_PATHNAME_LEN] = "", 
					path_t[MAX_PATHNAME_LEN] = "";   

	int i, Selected;
	SElEventDataPtr 	_pData; 
   	char				buffer[2048] = "";

	if(0==strcmp(eventName, "EVNT_GUI_EXPERTISE_SEQUENCE_NEW"))
	{
		// Get Selected station
		for (i=0; i<NB_OF_STATIONS; i++)
			{
				EXCCHECKCVI( GetCtrlVal (gs_panel[EXPERTISE], gs_CheckStation[i], &Selected));  
				if (Selected == 1)
					break;
			}
		
		EXCCHECKCVI( PostDeferredCall(PostDeferredHidePanel, NULL) ); 
		
		if ( gs_EventFactory )
   		{
			/* Set data [Station+Repeat] for R&R  */ 
			sprintf(buffer, "%d;%d", i, gs_repeat);
			EXCCHECK( eleventdata_new(&(_pData)));
			EXCCHECK( _pData->SetStringPtr(_pData, buffer));
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_GUI_EXPERTISE_SEQUENCE_CREATED", _pData));   //NULL devient la station selectionné

		}
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
		                                   "EVNT_GUI_EXPERTISE_SEQUENCE_NEW",
		                                   EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
										  "EVNT_GUI_PRODUCT_SELECTED",
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
#define __FUNC__ "{Expertise Plugin}::Initialize"
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
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel,    "expertise.uir", EXPERTISE, EXPERTISElang, &gs_panel[EXPERTISE]));
	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP,    &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[EXPERTISE], ATTR_TOP, top + height - 560));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[EXPERTISE], ATTR_LEFT, 11));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
	
	EXCCHECK( LoadConfiguration(NULL));
	EXCCHECK( RegisterEventFunction()); 

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Expertise Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
		
	if( 0==strcmp( uuid, PLUGIN_EXPERTISE))
	{
		EXCCHECK( ShowPanel());
	}

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Expertise Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( HidePanel(gs_panel[EXPERTISE]));

Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Expertise Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( DiscardPanel(gs_panel[EXPERTISE]));

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Expertise Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Expertise Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 



