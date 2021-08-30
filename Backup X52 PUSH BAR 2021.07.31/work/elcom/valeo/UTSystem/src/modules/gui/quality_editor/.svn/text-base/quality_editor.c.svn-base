#include <msxmldom.h>
#include <exception/Exception.h>
#include <eventfactory/EventFactory.h> 
#include <guid.h>
#include <dbs/dbs.h>
#include <users/user.h>
#include <bklib/bklib2.h>
#include <utility.h>
#include <toolbox.h>
#include "quality_editor.h"
#include "quality_editor_plugin.h"

#define NB_OF_PANEL 		1  
static int					gs_panel[NB_OF_PANEL + 1];
static SElEventFactoryPtr	gs_EventFactory = NULL;  
static SObjListPtr 			gs_Modules = NULL; 
static SDBSPtr    			gs_pDbs = NULL;
static SDBSParamListPtr		gs_pParam = NULL;
static SDBSProductListPtr	gs_pProduct = NULL;
static SUserListPtr			gs_pUserList = NULL;
static CAObjHandle			gs_bktree = 0;
static int32_t				gs_user_id = 0;

#define BKTREE_HEADER "root|Parameter^w115$value|Value^w300\n" //width 438

typedef struct _SGlobal
{
	char**		ParameterList;
	int32_t		ParameterListSize;

	SProductTypesPtr*	pParam;
	int32_t				pParamSize;

} SGlobal, *SGlobalPtr;

static SGlobal				gs_global = {NULL, 0, NULL, 0};

#define CHECK_VALUE( value) \
	RemoveSurroundingWhiteSpace(value); \
    if(0==strlen(value)) { \
		MessagePopup("Warning", "Empty string value!"); \
		CHECKERR( -1); }

static void CVICALLBACK PostDeferredHidePanel (void *callbackData) { RemovePopup(0);}

static HRESULT CVICALLBACK BkTreeModify (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol,
                              long  modifyType);

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ParametersListIni"
static SElExceptionPtr ParametersListIni(const char* fileName)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pparameter_list = 0;
	CAObjHandle     pparameter = 0;   
	CAObjHandle     pnode = 0;   
	const char*     pfile_name = (fileName) ? fileName : "\\quality_editor.xml";
	VBOOL         	is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*         	ptext= NULL;
	char			path[MAX_PATHNAME_LEN];  
	int32_t			i = 0;

	EXCCHECKCVI( GetModuleDir(__CVIUserHInst, path));  
	strcat(path, pfile_name);
	
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument(NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(&pfile_name_var, path));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload(pxml, NULL, pfile_name_var, &is_xml_success)); 
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes(pxml, NULL, "//plugin[@id='"PLUGIN_QUALITY_EDITOR"']/parameters/parameter", &pparameter_list));
	
	if(pparameter_list != 0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset(pparameter_list, NULL));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength(pparameter_list, NULL, &gs_global.ParameterListSize));
		
		gs_global.ParameterList = (char**)calloc(gs_global.ParameterListSize, sizeof(char*));
		EXCCHECKALLOC(gs_global.ParameterList);
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pparameter_list, NULL, &pparameter));
		
		while(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(pparameter, NULL, "@name", &pnode));
			if(pnode)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pnode, NULL, &ptext));
				if(ptext)
				{
					gs_global.ParameterList[i] = (char*)calloc(strlen(ptext)+1, sizeof(char));
					strcpy(gs_global.ParameterList[i], ptext);
					i++;
					CA_FreeMemory(ptext); 
					ptext = NULL;
				}
				CA_DiscardObjHandle(pnode);
				pnode = 0;
			}
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pparameter_list, NULL, &pparameter));
		}

		CA_DiscardObjHandle(pparameter_list); 
		pparameter_list = 0;
	}

Error:
	CA_VariantClear(&pfile_name_var);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pparameter_list) CA_DiscardObjHandle(pparameter_list);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Clean"
static SElExceptionPtr Clean(void)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	int32_t			i;

	for(i=0; i<gs_global.ParameterListSize; i++)
	{
		free(gs_global.ParameterList[i]);
		gs_global.ParameterList[i] = NULL;
	}

	free(gs_global.ParameterList);
	gs_global.ParameterList = NULL;

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ProductList"
static SElExceptionPtr ProductList(void)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	int32_t			i, j, k, count = 0, pid[512];
	char			product_path[512] = "";
	
	EXCCHECKCVI( DeleteListItem(gs_panel[PANEL], PANEL_PRODUCT_RING, 0, -1));

	for(i=0; i<gs_global.ParameterListSize; i++)
	{
		for(j=0; j<gs_pParam->ProductParameterSize; j++)
		{
			if(0==strcmp(gs_global.ParameterList[i], gs_pParam->ProductParameter[j].name)
				&& (gs_pParam->ProductParameter[j].property&PROPERTY_VALID)>0)
			{
				/* check if pid was used */
				for(k=0; k<count; k++)
				{
					if(pid[k] == gs_pParam->ProductParameter[j].product_id)
						break;
				}
				if(k==count)
				{
					pid[count] = gs_pParam->ProductParameter[j].product_id;
					EXCCHECK( gs_pProduct->ProductGetIdPath(gs_pProduct, pid[count], product_path, 2)) 
					EXCCHECKCVI( InsertListItem (gs_panel[PANEL], PANEL_PRODUCT_RING, -1, product_path, pid[count]));
					count++;
				}
			}
		}
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ParameterList"
static SElExceptionPtr ParameterList(void)
{
	int32_t			error = 0;
	HRESULT         hr = 0; 
	SElExceptionPtr pexception = NULL;
	int32_t			pid, i, j, k=0, index;
	SELStringPtr 	tree = NULL;
	short			AXvalue;

	EXCCHECK( elstring_new(BKTREE_HEADER, &tree));
	EXCCHECK( tree->Append(tree, "|root|Parameters^i0^r1$^r1\n"));

	EXCCHECKCVI( GetCtrlIndex (gs_panel[PANEL], PANEL_PRODUCT_RING, &index));
	if(index>=0)
	{
		EXCCHECKCVI( GetValueFromIndex (gs_panel[PANEL], PANEL_PRODUCT_RING, index, &pid));
		if(pid>0)
		{
			for(i=0; i<gs_global.ParameterListSize; i++)
			{
				for(j=0; j<gs_pParam->ProductParameterSize; j++)
				{
					if(0==strcmp(gs_global.ParameterList[i], gs_pParam->ProductParameter[j].name)
						&& (gs_pParam->ProductParameter[j].property&PROPERTY_VALID)>0
						&& gs_pParam->ProductParameter[j].product_id==pid)
					{
						EXCCHECK( tree->Format(tree, "root|param%d|%s^i0^r1$%s^d%d\n", 
														k++,
														gs_pParam->ProductParameter[j].name,
														gs_pParam->ProductParameter[j].value,
														j+1));
					}
				}
			}
		}
	}

	EXCCHECKCOM( BKLib2__DBKTreeFromString( gs_bktree, 
											NULL, 
											tree->GetPtr(tree),
											"$", "\n",
											&AXvalue));
	EXCCHECKCOM( BKLib2__DBKTreeSetEditable (gs_bktree, NULL, VTRUE));  
	EXCCHECKCOM( BKLib2__DBKTreeSetEnableDragAndDrop (gs_bktree, NULL, VFALSE)); 
	EXCCHECKCOM( BKLib2__DBKTreeSetEnableEventModify (gs_bktree, NULL, VTRUE));

Error:
	elstring_delete( &tree);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PanelIni"
static SElExceptionPtr PanelIni(void)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	SUserPtr		puser = NULL; 

	EXCCHECK( gs_pUserList->GetLoggedUser(gs_pUserList, &puser)); 

	if(puser)
	{
		gs_user_id = puser->_user_id;
		EXCCHECK( ProductList());
		EXCCHECK( ParameterList());
		EXCCHECKCVI( InstallPopup(gs_panel[PANEL]));
	}
	else
	{
		EXCTHROW(-1, "No User Logged!");
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Quality Editor Plugin}::BkTreeModify"
static HRESULT CVICALLBACK BkTreeModify (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol,
                              long  modifyType
)
{
	SElExceptionPtr pexception =  NULL;      
	int32_t      	error = 0; 
	char*      		value = NULL;
	HRESULT			hr;
	long 			index;
	SProductParameter	parameter = {0};  

	EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText (gs_bktree, 
												 NULL, 
												 keyNode, 
												 keyCol, 
												 &value));
	CHECK_VALUE( value);

	EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (gs_bktree, NULL, keyNode, keyCol, &index));
	if(index)
	{
		index -= 1;

		parameter = gs_pParam->ProductParameter[index]; 
		strncpy(parameter.value, value, DBS_RECORD_LENGHT_VALUE_EXT);
		parameter.description = gs_pDbs->CharCopy(parameter.description);
		parameter.user_id = gs_user_id;
		EXCCHECK( gs_pParam->ProductParamEdit(gs_pParam, parameter)); 

		/* reload table */
		EXCCHECK( ParameterList());
	}

Error:
	EXCDISPLAY( pexception);
   	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Quality Editor Plugin}::OnButton"
int CVICALLBACK OnButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	
	switch (event)
	{
		case EVENT_COMMIT:

			switch(control)
			{
				case PANEL_SAVE:
					EXCCHECK( gs_pParam->ProductParamSave(gs_pParam, ((SDBSPtr)gs_pParam->pdbs)->_branch_id));
					((SDBSPtr)gs_pParam->pdbs)->data_changed = FALSE;
					break;
				case PANEL_CANCEL:
					break;
			}
			EXCCHECKCVI( PostDeferredCall(PostDeferredHidePanel, NULL));    
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Quality Editor Plugin}::OnProduct"
int CVICALLBACK OnProduct (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	
	switch (event)
	{
		case EVENT_VAL_CHANGED:
			EXCCHECK( ParameterList());
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
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
#define __FUNC__ "{Quality Editor Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			main_panel = 0; 
	HRESULT			hr;
	
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
	
	EXCCHECKCVI( gs_panel[PANEL] = LoadPanelEx (main_panel, "quality_editor.uir", PANEL, __CVIUserHInst));
	
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[PANEL], PANEL_BKTREE, &gs_bktree));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnModify (gs_bktree, BkTreeModify, NULL, 1, NULL));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
	if(gs_Modules)
	{
		gs_pDbs = (SDBSPtr)gs_Modules->Get(gs_Modules, MODULE_DBS);
		if(gs_pDbs)
		{
			gs_pParam = gs_pDbs->ProductParameterList;
			gs_pProduct = gs_pDbs->ProductList;
		}
		
		gs_pUserList = (SUserListPtr)gs_Modules->Get(gs_Modules, MODULE_USER);
	}

	EXCCHECK( ParametersListIni(NULL));		

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Quality Editor Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	if( 0==strcmp( uuid, PLUGIN_QUALITY_EDITOR))
	{
		EXCCHECK( PanelIni());
	}

Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Quality Editor Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
Error:	
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Quality Editor Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	if(gs_panel[PANEL])
		EXCCHECKCVI( DiscardPanel(gs_panel[PANEL]));

	EXCCHECK( Clean());

Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Quality Editor Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Quality Editor Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 



