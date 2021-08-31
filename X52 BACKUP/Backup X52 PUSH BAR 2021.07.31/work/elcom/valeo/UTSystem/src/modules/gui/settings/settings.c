#include "msxmldom.h"
#include <utility.h>
#include <eventfactory/EventFactory.h> 
#include <guid.h>
#include <bklib/bklib2.h>
#include "settings.h"
#include "settings_plugin.h"

#define BKTREE_HEADER0			"root|Modules^w250¦value|Value^w500\n"
#define BKTREE_HEADER1			"|0|Modules^i0^r1\n"

#define NB_OF_PANEL 		1

typedef enum _ENbOfPanel
{
	ENOP_FIRST = NB_OF_PANEL,
	TABPANEL_1,
	TABPANEL_2,
	NB_OF_PANELS
} ENbOfPanel;

static int					gs_panel[NB_OF_PANELS];  
static SElEventFactoryPtr	gs_EventFactory = NULL;  
static SObjListPtr 			gs_Modules = NULL;  
static CAObjHandle 			gv_TreeHandle;

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnButton"
int CVICALLBACK OnButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	switch (event)
	{
		case EVENT_COMMIT:
			switch(control)
			{
				case PANEL_SAVE:
				case PANEL_DEFAULT:
				case PANEL_CANCEL:
					RemovePopup(0);
					break;
			}
			break;
	}
	
Error:
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "AddConfigFile"
static SElExceptionPtr AddConfigFile(
   SELStringPtr	pelstring,
   const char*	fileName,
   const char*	mode
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml = 0;
	const char*     pfile_name = (fileName) ? fileName : "\\settings.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char			path[MAX_PATHNAME_LEN]; 
	char*			ptext = NULL;

	EXCCHECKCVI( GetProjectDir (path));  
	strcat(path, pfile_name);

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(&pfile_name_var, path));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));

Error:
	CA_VariantClear(&pfile_name_var);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadWithConfiguration"
static SElExceptionPtr LoadWithConfiguration(
   const char*	fileName
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pconf_list = 0;
	CAObjHandle     pconfiguration = 0;
	CAObjHandle     pparameter = 0;
	const char*     pfile_name = (fileName) ? fileName : "\\settings.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char			path[MAX_PATHNAME_LEN]; 
	char*			ptext = NULL;
	SELStringPtr	pelstring = NULL;
	char			mode[256];
	
	EXCCHECK( elstring_new("", &pelstring));
	EXCCHECK( pelstring->Append(pelstring, BKTREE_HEADER0));
	EXCCHECK( pelstring->Append(pelstring, BKTREE_HEADER1));

	EXCCHECKCVI( GetModuleDir (__CVIUserHInst, path));  
	strcat(path, pfile_name);

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(&pfile_name_var, path));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes (pxml, NULL,
												     "//plugin[@id='"PLUGIN_SETTINGS"']/configs/config",
													 &pconf_list));
	
	if(pconf_list != 0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset(pconf_list, NULL));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pconf_list, NULL, &pconfiguration));
		
		while (pconfiguration)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(pconfiguration, NULL, "@type", &pparameter));
			if(pparameter)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pparameter, NULL, &ptext));
				if(ptext)
				{
					strncpy(mode, ptext, 255);
					CA_FreeMemory(ptext); 
					ptext = NULL;
				}
			}

			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pconfiguration, NULL, &ptext));
			if(ptext)
			{
				EXCCHECK( AddConfigFile(pelstring, ptext, mode));
				CA_FreeMemory(ptext); 
				ptext = NULL;
			}

			CA_DiscardObjHandle(pconfiguration); 
			pconfiguration = 0;
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pconf_list, NULL, &pconfiguration));
		}

		CA_DiscardObjHandle(pconf_list);
		pconf_list = 0;
	}

	EXCCHECKCOM( BKLib2__DBKTreeFromString (gv_TreeHandle, NULL, pelstring->GetPtr(pelstring), "¦", "\n", NULL));

Error:
	elstring_delete(&pelstring);
	CA_VariantClear(&pfile_name_var);
	if (pconf_list) CA_DiscardObjHandle(pconf_list);
	if (pconfiguration) CA_DiscardObjHandle(pconfiguration);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
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
#define __FUNC__ "{Settings Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			main_panel = 0, tab = 0; 
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "MainPanel",
									 sizeof(main_panel),
									 &main_panel));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));

	EXCCHECKCVI( gs_panel[PANEL] = LoadPanelEx (main_panel, "settings.uir", PANEL, __CVIUserHInst));  

	EXCCHECKCVI( GetPanelHandleFromTabPage (gs_panel[PANEL], PANEL_TAB, 0, &gs_panel[TABPANEL_1]));
	EXCCHECKCVI( GetPanelHandleFromTabPage (gs_panel[PANEL], PANEL_TAB, 1, &gs_panel[TABPANEL_2]));
	
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[TABPANEL_1], TABPANEL_1_BKTREE, &gv_TreeHandle));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "Modules",
									 sizeof(SObjListPtr),
									 &gs_Modules)); 

	EXCCHECK(LoadWithConfiguration(NULL));

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Settings Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
		
	if( 0==strcmp( uuid, PLUGIN_SETTINGS))
	{
		EXCCHECKCVI( InstallPopup(gs_panel[PANEL]));
	}

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Settings Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( HidePanel(gs_panel[PANEL]));

Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Settings Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( DiscardPanel(gs_panel[PANEL]));

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Settings Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Settings Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 



