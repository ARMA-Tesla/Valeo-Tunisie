#include "msxmldom.h"
#include <modules/module.h>
#include <eventfactory/EventFactory.h>
#include <guid.h>
#include <utility.h>
#include "can.h"
#include <stdio.h>

const char* HISTORY = "[2007-11-19] KP 2.10\n"
					  " * Fixed checking of data type for attribute functions.";

static SElEventFactoryPtr	gs_EventFactory = NULL;
static SCANListPtr			gs_pCAN = NULL;

static SElExceptionPtr LoadWithConfiguration(const char* fileName, SCANListPtr* pCAN);

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
#if (!defined(_CVI_)&&defined(_DEBUG))
			//_CrtSetBreakAlloc(235);
#endif
		if (InitCVIRTE (hinstDLL, 0, 0) == 0)
			return 0;	  /* out of memory */
		break;
	case DLL_PROCESS_DETACH:
		CloseCVIRTE ();
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
		_CrtDumpMemoryLeaks();
#endif
		break;
	}
	
	return 1;
}


#undef __FUNC__
#define __FUNC__ "{CAN}::GetModuleAttribute"
SElExceptionPtr	_FUNCC GetModuleAttribute( uint32_t  attrID, uint32_t size, void* value)
{
	switch ( attrID )
	{
		case MODULE_ATTR_VERSION_STRING:
			strncpy((char*)value, "2.10", size);
			break;
		case MODULE_ATTR_MODULE_ID:
			strncpy((char*)value, MODULE_CAN, size);
			break;
		case MODULE_ATTR_MODULE_NAME:
			strncpy((char*)value, "CAN Multi Module", size);
			break;
		case MODULE_ATTR_MODULE_DESCRIPTION:
			strncpy((char*)value, "Communication module for CAN, LIN", size);
			break;
		case MODULE_ATTR_REQUIRED_MODULES:
			strncpy((char*)value, "", size);
			break;
		case MODULE_ATTR_BUILD_DATETIME:
			strncpy((char*)value, __DATE__" "__TIME__, size);
			break;
	}
	return NULL;
}

#undef __FUNC__
#define __FUNC__ "{CAN}::LoadElcomModule"
SElExceptionPtr	_FUNCC LoadElcomModule(
	const char* configuration,
	void** pInstance,
	SPluginArgsPtr	pParam
)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( pParam->GetArg( pParam,
							  "WindowHandle",
							  sizeof(hWnd),
							  &hWnd));

	EXCCHECK( LoadWithConfiguration(NULL, &gs_pCAN));

Error:                       
	*pInstance = gs_pCAN;
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{CAN}::InitializeElcomModule"
SElExceptionPtr	_FUNCC InitializeElcomModule( void* pInstance, const char* configuration, SPluginArgsPtr pParam)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = (SCANPtr) pInstance;

	EXCCHECK( pParam->GetArg( pParam,
							  "EventFactory",
							  sizeof(SElEventFactoryPtr),
							  &gs_EventFactory));

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{CAN}::UnloadElcomModule"
SElExceptionPtr	_FUNCC UnloadElcomModule( void* pInstance)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pCAN = (SCANPtr) pInstance;
	
	EXCCHECK( gs_pCAN->Close(gs_pCAN, NULL));
	EXCCHECK( canlist_delete(&gs_pCAN));

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadWithConfiguration"
static SElExceptionPtr LoadWithConfiguration(
   const char*	fileName,
   SCANListPtr*	pCAN
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pconf_list = 0;
	CAObjHandle     pconfiguration = 0;
	CAObjHandle     pparameter = 0;   
	const char*     pfile_name = (fileName) ? fileName : "\\can.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char			path[MAX_PATHNAME_LEN]; 
	char*			options = NULL;
	char*			channel_name = NULL;
	bool_t			enable = TRUE;

	EXCCHECKCVI( GetModuleDir (__CVIUserHInst, path));  
	strcat(path, pfile_name);

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(&pfile_name_var, path));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	
	EXCCHECK( canlist_new(pCAN, NULL));
	
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes (pxml, NULL,
												     "//module[@id='"MODULE_CAN"']/settings/configure",
													 &pconf_list));
	
	if(pconf_list != 0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset(pconf_list, NULL));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pconf_list, NULL, &pconfiguration));
		
		while (pconfiguration)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(pconfiguration, NULL, "@channel", &pparameter));
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pparameter, NULL, &channel_name));
			CA_DiscardObjHandle(pparameter);
			pparameter = 0;

			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(pconfiguration, NULL, "cardOption", &pparameter));
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pparameter, NULL, &options));
			CA_DiscardObjHandle(pparameter);
			pparameter = 0;

			EXCCHECK( (*pCAN)->AddChannel(*pCAN, channel_name, options));
			if (options)
			{
				CA_FreeMemory(options); options = NULL;
			}

			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(pconfiguration, NULL, "openOption", &pparameter));
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pparameter, NULL, &options));
			CA_DiscardObjHandle(pparameter);
			pparameter = 0;

			EXCCHECK( (*pCAN)->Open(*pCAN, channel_name, options));
			EXCCHECK( (*pCAN)->SetAttribute(*pCAN, channel_name, DATA_TYPE_INT32, CAN_ATTR_CACHE_ENABLED, (void*)&enable));
			if (options)
			{
				CA_FreeMemory(options); options = NULL;
			}

			if (channel_name)
			{
				CA_FreeMemory(channel_name); channel_name = NULL;
			}
			CA_DiscardObjHandle(pconfiguration); pconfiguration = 0;
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pconf_list, NULL, &pconfiguration));
		}

		CA_DiscardObjHandle(pconf_list); pconf_list = 0;
	}

Error:
	CA_VariantClear(&pfile_name_var);
	if (options) CA_FreeMemory(options);
	if (channel_name) CA_FreeMemory(channel_name);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pconf_list) CA_DiscardObjHandle(pconf_list);
	if (pconfiguration) CA_DiscardObjHandle(pconfiguration);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}
