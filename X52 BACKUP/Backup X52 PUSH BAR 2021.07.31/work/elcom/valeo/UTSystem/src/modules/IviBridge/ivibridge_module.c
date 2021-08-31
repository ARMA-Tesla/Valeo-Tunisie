#include <cvirte.h>
#include <modules/module.h>
#include <stdlib.h>
#include <string.h>
#include "ivibridge.h"
#include <userint.h>
#include <guid.h>

static SIviBridgePtr    	gs_pIVI = NULL;

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)
				return 0;	  /* out of memory */
			break;
		case DLL_PROCESS_DETACH:
			CloseCVIRTE ();
			break;
		}
	
	return 1;
}


#undef __FUNC__
#define __FUNC__ "GetModuleAttribute"
SElExceptionPtr	_FUNCC GetModuleAttribute( uint32_t  attrID, uint32_t size, void* value)
{
	switch ( attrID )
	{
		case MODULE_ATTR_VERSION_STRING:
			strncpy((char*)value, "1.0", size);
			break;
		case MODULE_ATTR_MODULE_ID:
			strncpy((char*)value, MODULE_IVIBRIDGE, size);
			break;
		case MODULE_ATTR_MODULE_NAME:
			strncpy((char*)value, "IVI Bridge Module", size);
			break;
		case MODULE_ATTR_MODULE_DESCRIPTION:
			strncpy((char*)value, "Module for unique IVI driver initialization", size);
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
#define __FUNC__ "{IVIBridge}::LoadElcomModule"
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

	EXCCHECK( ivibridge_new( &gs_pIVI));
	
Error:                       
	*pInstance = gs_pIVI;
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{IVIBridge}::InitializeElcomModule"
SElExceptionPtr	_FUNCC InitializeElcomModule( void* pInstance, const char* configuration, SPluginArgsPtr	pParam)
{
	SElExceptionPtr    pexception = NULL; 
	
	EXCCHECK( gs_pIVI->InitializeConfiguration( gs_pIVI, NULL));
	
Error:
	EXCRETHROW( pexception);   
}

#undef __FUNC__
#define __FUNC__ "{IVIBridge}::UnloadElcomModule"
SElExceptionPtr	_FUNCC UnloadElcomModule( void* pInstance)
{
	SElExceptionPtr    pexception = NULL;        
	
	EXCCHECK( ivibridge_delete(&gs_pIVI));
	
Error:
	EXCRETHROW( pexception);   
}
