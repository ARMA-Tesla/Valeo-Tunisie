#include <msxmldom.h>
#include <exception/Exception.h>
#include <eventfactory/EventFactory.h> 
#include <guid.h>
#include <dbs/dbs.h>
#include <users/user.h>
#include "quality.h"
#include "quality_plugin.h"
#include <ansi_c.h>

const char* HISTORY = \
"[2007-11-23] rT 1.10\n"
" * Added master save to dbs.";

#define NB_OF_PANEL 		1  
static int					gs_panel[NB_OF_PANEL + 1];  
static SElEventFactoryPtr	gs_EventFactory = NULL;  
static SObjListPtr 			gs_Modules = NULL; 
static SDBSPtr    			gs_pDbs;
static SDBSMasterListPtr 	gs_pMasterList = NULL;
static SDBSProductListPtr 	gs_pProductList = NULL;

int32_t						product_id;

#define BARCODE_PREFIX		"S/N: "

void CVICALLBACK PostDeferredDisplayPanel (void *callbackData) { InstallPopup(gs_panel[PANEL]);}
void CVICALLBACK PostDeferredHidePanel (void *callbackData) { RemovePopup(0);}
	
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetBarcodeString"
static SElExceptionPtr GetBarcodeString(const char* fileXml, char* barcode, uint32_t size, bool_t* failed)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
	HRESULT         hr = 0;
	char            _message[512];
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;
	const char*     pfile_xml = (fileXml) ? fileXml : "test_form.xml";
	VBOOL           is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL;
	
	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//testseq", &pnode));
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "product/id", &pparameter));
	
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		
		product_id = atoi(ptext);
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "product/barcode", &pparameter));
	
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		
		strncpy(barcode, ptext, size);
					
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "product/failed", &pparameter));
	
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		
		if(failed) *failed = strtol(ptext, NULL, 10);
					
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

Error:
	CA_FreeMemory(ptext);
	CA_VariantClear(&pfile_name_var);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);  
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PanelIni"
static SElExceptionPtr PanelIni(const char* xml_file)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	char			barcode[256];
	char			buffer[512];
	char**			codes = NULL;
	int32_t			codesSize = 512;
	int				i;
	bool_t			failed = FALSE;

	EXCCHECK( GetBarcodeString(xml_file, barcode, sizeof(barcode)-1, &failed));

	if( !failed)
	{
		sprintf( buffer, BARCODE_PREFIX"%s", barcode);
		EXCCHECKCVI( SetCtrlVal( gs_panel[PANEL], PANEL_MSG_BARCODE, buffer));

		EXCCHECKCVI( PostDeferredCall(PostDeferredDisplayPanel, NULL) );    
	}

Error:
	free(codes);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PanelClose"
static SElExceptionPtr PanelClose(void)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	
	EXCCHECKCVI( PostDeferredCall(PostDeferredHidePanel, NULL) );       

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnButton"
int CVICALLBACK OnButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	char			master[512];
	char*			pmaster = master;
	
	switch (event)
		{
		case EVENT_COMMIT:

			switch(control)
			{
				case PANEL_SAVE:
					EXCCHECKCVI( GetCtrlVal( gs_panel[PANEL], PANEL_MSG_BARCODE, master));
					pmaster += strlen(BARCODE_PREFIX);
					EXCCHECK( gs_pProductList->ProductRename(gs_pProductList, (((SDBSPtr)gs_pMasterList->pdbs)->_branch_id), product_id, pmaster));
					((SDBSPtr)gs_pMasterList->pdbs)->data_changed = FALSE;
					EXCCHECK( gs_pProductList->ProductRead(gs_pProductList, (((SDBSPtr)gs_pMasterList->pdbs)->_branch_id)));
					break;
				case PANEL_CANCEL:
					break;
			}
			EXCCHECK( PanelClose());
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
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	
	if(0==strcmp(eventName, "EVNT_TEST_SEQUENCE_END"))
	{
		if(pData)
		{	// printf("Executing EventFunctionCallback\r\n");
			const char* buffer = ((SElEventDataPtr)pData)->GetStringPtr(pData);
			
			if(buffer && buffer[0] != '\0')
			{
				char	data[1024];
				char*	pdata = NULL;
				char*	pdata2 = NULL;
				
				strcpy(data, buffer);
				
				pdata = strtok(data, ";");  /* tester mode */
				pdata2 = strtok(NULL, ";"); /* xml path */

				if( pdata && 0==strcmp(pdata, "QUALITY_MASTER") && pdata2)
				{
					EXCCHECK( PanelIni(pdata2));
				}
			}
		}
	}
  // printf("End of Executing EventFunctionCallback\r\n");
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
		                                   "EVNT_TEST_SEQUENCE_END",
		                                   EventFunctionCallback));
   	}
   
Error:
   	EXCRETHROW(pexception);
} /* RegisterEventFunction */

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
#define __FUNC__ "{Quality Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			main_panel = 0; 
	
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
	
	EXCCHECKCVI( gs_panel[PANEL] = LoadPanelEx (main_panel, "quality.uir", PANEL, __CVIUserHInst));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
	if(gs_Modules)
	{
		gs_pDbs = (SDBSPtr)gs_Modules->Get(gs_Modules, MODULE_DBS);
		if(gs_pDbs)
		{
			gs_pMasterList = gs_pDbs->ProductMasterList;
			gs_pProductList = gs_pDbs->ProductList;
		}
	}
		
	EXCCHECK( RegisterEventFunction()); 

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Quality Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Quality Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( HidePanel(gs_panel[PANEL]));

Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Quality Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( DiscardPanel(gs_panel[PANEL]));

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Quality Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Quality Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 



