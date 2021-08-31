
#include "profilometers.h"
#include <cvirte.h>
#include <modules/module.h>
#include <guid.h>

static SProfilometersPtr		gs_pProfilometers = NULL;

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
#define __FUNC__ "{Profilometers}::GetModuleAttribute"
SElExceptionPtr	_FUNCC GetModuleAttribute( uint32_t  attrID, uint32_t size, void* value)
{
	switch ( attrID )
	{
		case MODULE_ATTR_VERSION_STRING:
			strncpy((char*)value, "1.0", size);
			break;
		case MODULE_ATTR_MODULE_ID:
			strncpy((char*)value, MODULE_PROFILOMETERS, size);
			break;
		case MODULE_ATTR_MODULE_NAME:
			strncpy((char*)value, "Profilometers Module", size);
			break;
		case MODULE_ATTR_MODULE_DESCRIPTION:
			strncpy((char*)value, "Profilometers module for handling tester profilometers", size);
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
#define __FUNC__ "{Profilometers}::LoadElcomModule"
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

	EXCCHECK( profilometers_new( &gs_pProfilometers));

	EXCCHECK( pParam->SetArg( pParam, "ProfilometersModule", ARG_PTR, (void*) gs_pProfilometers));
	
Error:                       
	*pInstance = gs_pProfilometers;
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Profilometers}::InitializeElcomModule"
SElExceptionPtr	_FUNCC InitializeElcomModule( void* pInstance, const char* configuration, SPluginArgsPtr	pParam)
{
	SElExceptionPtr	pexception = NULL;
	SProfilometersPtr	p2profilometers = (SProfilometersPtr) pInstance;

	EXCCHECK( gs_pProfilometers->InitProfilometers(gs_pProfilometers));

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Profilometers}::UnloadElcomModule"
SElExceptionPtr	_FUNCC UnloadElcomModule( void* pInstance)
{
	SElExceptionPtr	pexception = NULL;
	SProfilometersPtr	p2profilometers = (SProfilometersPtr) pInstance;

	EXCCHECK( profilometers_delete(&gs_pProfilometers));

Error:
	EXCRETHROW( pexception);
}

