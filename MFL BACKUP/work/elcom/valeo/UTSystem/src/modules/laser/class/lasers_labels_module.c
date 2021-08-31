#include "lasers.h"
#include <cvirte.h>
#include <modules/module.h>
#include <guid.h>

static SLasersPtr		gs_pLasers = NULL;

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
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
			_CrtDumpMemoryLeaks();
#endif
		break;
	}
	
	return 1;
}


#undef __FUNC__
#define __FUNC__ "{Lasers}::GetModuleAttribute"
SElExceptionPtr	_FUNCC GetModuleAttribute( uint32_t  attrID, uint32_t size, void* value)
{
	switch ( attrID )
	{
		case MODULE_ATTR_VERSION_STRING:
			strncpy((char*)value, "1.0", size);
			break;
		case MODULE_ATTR_MODULE_ID:
			strncpy((char*)value, MODULE_LASERS, size);
			break;
		case MODULE_ATTR_MODULE_NAME:
			strncpy((char*)value, "Lasers Module", size);
			break;
		case MODULE_ATTR_MODULE_DESCRIPTION:
			strncpy((char*)value, "Lasers module for handling tester lasers", size);
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
#define __FUNC__ "{Lasers}::LoadElcomModule"
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

	EXCCHECK( lasers_new( &gs_pLasers));

	EXCCHECK( pParam->SetArg( pParam, "LasersModule", ARG_PTR, (void*) gs_pLasers));
	
Error:                       
	*pInstance = gs_pLasers;
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Lasers}::InitializeElcomModule"
SElExceptionPtr	_FUNCC InitializeElcomModule( void* pInstance, const char* configuration, SPluginArgsPtr	pParam)
{
	SElExceptionPtr	pexception = NULL;
	SLasersPtr	p2lasers = (SLasersPtr) pInstance;

	EXCCHECK( gs_pLasers->InitLasers(gs_pLasers));

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Lasers}::UnloadElcomModule"
SElExceptionPtr	_FUNCC UnloadElcomModule( void* pInstance)
{
	SElExceptionPtr	pexception = NULL;
	SLasersPtr	p2lasers = (SLasersPtr) pInstance;

	EXCCHECK( lasers_delete(&gs_pLasers));

Error:
	EXCRETHROW( pexception);
}

