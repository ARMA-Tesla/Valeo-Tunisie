#include <msxmldom.h>
#include <guid.h>
#include <dbs/dbs.h> 
#include <objlist/objlist.h>
#include <eventfactory/EventFactory.h> 
#include <utility.h> 
#include <language/language.h>
#include "rnr_plugin.h" 
#include "trans_language.h"

#define BARCODE_SIZE	128

static struct _SRnr
{
	int32_t		operator;
	int32_t 	panel;
	int32_t		round;
	
	int32_t		a_operator;
	int32_t 	a_panel;
	int32_t		a_round;
	
	char**		barcodes;	
	int			_barcodes_size;
	
	char		rnr_dir[MAX_PATHNAME_LEN];
} SRnr;

#define RESET_RNR() \
	EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_OPERATOR, "1")); \
	EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_PANEL, "1")); \
	EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_ROUND, "1")); \
	SRnr.a_operator = 1; \
	SRnr.a_panel = 1; \
	SRnr.a_round = 1

#define NB_OF_PANEL 		1
static int					gs_panel[NB_OF_PANEL + 1];
static SElEventFactoryPtr	gs_EventFactory = NULL; 
static SObjListPtr 			gs_Modules = NULL;  
SLanguagePtr				gs_pLanguage = NULL;

static SElExceptionPtr RnRGetTextResult(const char* xml_path, bool_t* failed, char* barcode, uint32_t barcode_size);

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RnRInitialize"
static SElExceptionPtr RnRInitialize(bool_t dialog)
{
	SElExceptionPtr    	pexception = NULL;  
	int32_t			   	error = 0; 
	int32_t				result;
	
	EXCCHECKCVI( DisplayPanel(gs_panel[PANEL])); 

	if(dialog 
		&& (SRnr.a_operator != 1
		|| SRnr.a_panel != 1
		|| SRnr.a_round != 1))
	{
		if(IDNO == MessageBox((HWND)hWnd, "Continue last RnR studie?", "RnR", MB_YESNO))
		{
			RESET_RNR();
		}
	}
	else
	{
		RESET_RNR();
	}
	
Error:                       
	EXCRETHROW( pexception);
}

#define PATH_EXPERTISE_RNR		"%s\\data\\expertise\\%04d.%02d.%02d\\rnr_%02d.%02d.%02d"
#define PATH_OPERATOR			"%s\\operator%02d"
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RnRUpdate"
static SElExceptionPtr RnRUpdate(const char* xml_path)
{
	SElExceptionPtr pexception = NULL;  
	int32_t			error = 0;   
	char			buffer[128];
	bool_t			failed = FALSE;
	char			barcode[128] = "";
	char			project_dir[MAX_PATHNAME_LEN] = "";
	char			operator_dir[MAX_PATHNAME_LEN] = "";
	int32_t			i;
	
	EXCCHECK( RnRGetTextResult(xml_path, &failed, barcode, 128));
	
	if(!failed)
	{
		/* rnr start */
		if(SRnr.a_round==1 && SRnr.a_panel==1 && SRnr.a_operator==1)
		{
			SYSTEMTIME st;

			GetLocalTime(&st);
			EXCCHECKCVI(GetProjectDir(project_dir));

			/* create rnr directory */
			sprintf(SRnr.rnr_dir, PATH_EXPERTISE_RNR, project_dir, 
								st.wYear, st.wMonth, st.wDay,
								st.wHour, st.wMinute, st.wSecond);
			
			EXCCHECKCVI( MakeDir(SRnr.rnr_dir));

			for(i=0; i<SRnr.operator; i++)
			{
				sprintf(operator_dir, PATH_OPERATOR, SRnr.rnr_dir, i+1);
				EXCCHECKCVI( MakeDir(operator_dir));
			}
		}

		/* copy xml file */
		sprintf(operator_dir, PATH_OPERATOR, SRnr.rnr_dir, SRnr.a_operator);
		CopyFile(xml_path, operator_dir);

		SRnr.a_panel++;
		if( SRnr.a_panel>SRnr.panel)
		{
			SRnr.a_panel = 1;
			SRnr.a_operator++;
			if( SRnr.a_operator>SRnr.operator)
			{
				SRnr.a_operator = 1;
				SRnr.a_round++;
				if( SRnr.a_round>SRnr.round)
				{
					/* end RnR study */
					SRnr.a_round = 1;
				
				}
			}
		}
	}
	
	sprintf(buffer, "%d", SRnr.a_operator); 
	EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_OPERATOR, buffer));
	sprintf(buffer, "%d", SRnr.a_panel);
	EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_PANEL, buffer));  
	sprintf(buffer, "%d", SRnr.a_round);
	EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_ROUND, buffer));  
	
Error:                       
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RnRGetTextResult"
static SElExceptionPtr RnRGetTextResult(const char* xml_path, bool_t* failed, char* barcode, uint32_t barcode_size)
{
	SElExceptionPtr pexception = NULL;  
	int32_t			error = 0;   	
	HRESULT         hr = 0;
	char         	_message[512];
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;   
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*         	ptext= NULL;
	char			path[MAX_PATHNAME_LEN];  

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, xml_path));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//product", &pnode));   
		
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "failed", &pparameter));
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(failed) *failed = (1==strtol(ptext, NULL, 10));
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "barcode", &pparameter));
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(barcode) strncpy(barcode, ptext, barcode_size);
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
	const char*     pfile_name = (fileName) ? fileName : "\\rnr.xml";
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
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//plugin[@id='"PLUGIN_RNR"']", &pnode));   
		
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "operator", &pparameter));
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			SRnr.operator = strtol(ptext, NULL, 10);
			EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_OPERATORS, ptext));
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "panel", &pparameter));
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			SRnr.panel = strtol(ptext, NULL, 10);
			EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_PANELS, ptext));
			
			SRnr._barcodes_size = BARCODE_SIZE;
			SRnr.barcodes = calloc(SRnr.panel, sizeof(char*));
			for(i=0; i<SRnr.panel; i++)
			{
				SRnr.barcodes[i] = calloc(1, sizeof(char)*SRnr._barcodes_size);	
			}
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "round", &pparameter));
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			SRnr.round = strtol(ptext, NULL, 10);
			EXCCHECKCVI( SetCtrlVal(gs_panel[PANEL], PANEL_NUM_ROUNDS, ptext));
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

	/* reset values */
	SRnr.a_operator = 1;
	SRnr.a_panel = 1;
	SRnr.a_round = 1;

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
static SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	SElExceptionPtr	pexception = NULL;

	if(0==strcmp(eventName, "EVNT_TEST_SEQUENCE_END"))
	{
		const char* buffer = ((SElEventDataPtr)pData)->GetStringPtr(pData);   
		
		if(strlen(buffer)>0)
		{
			char	data[2048];
			char*	pdata = NULL;
			char*	pdata2 = NULL;
			char*	pdata3 = NULL;
			
			strcpy(data, buffer);
			
			pdata = strtok(data, ";");  /* tester mode */
			pdata2 = strtok(NULL, ";"); /* xml path */
			pdata3 = strtok(NULL, ";"); /* result */
			
			if(0==strcmp(pdata, "EXPERTISE_RNR_STUDY"))
			{
				EXCCHECK( RnRUpdate(pdata2));
			}
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
	SElExceptionPtr	pexception = NULL;
	
	if ( gs_EventFactory )
	{
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
		                                   				"EVNT_TEST_SEQUENCE_END",
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
#define __FUNC__ "{RnR Plugin}::Initialize"
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
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel,    "rnr.uir", PANEL, RNRlang, &gs_panel[PANEL]));
	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP,    &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_TOP, top + height - 80 - 466 - 288));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_LEFT, 11));
	
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
#define __FUNC__ "{RnR Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	if( 0==strcmp( uuid, PLUGIN_RNR))
	{
		EXCCHECK( RnRInitialize(TRUE)); 
	}

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{RnR Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( HidePanel(gs_panel[PANEL]));

Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{RnR Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				i;

	for(i=0; i<SRnr.panel; i++)
	{
		free(SRnr.barcodes[i]);
		SRnr.barcodes[i] = NULL;
	}
	free( SRnr.barcodes);
	SRnr.barcodes = NULL;

	if ( gs_panel[PANEL] )
		EXCCHECKCVI( DiscardPanel(gs_panel[PANEL]));
	
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{RnR Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{RnR Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 
