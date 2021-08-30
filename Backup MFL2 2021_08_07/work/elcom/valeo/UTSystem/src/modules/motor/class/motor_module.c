#include "motor.h"
#include <cvirte.h>
#include <modules/module.h>
#include <guid.h>

static SmotorsPtr		gs_pmotors = NULL;

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
#define __FUNC__ "{motors}::GetModuleAttribute"
SElExceptionPtr	_FUNCC GetModuleAttribute( uint32_t  attrID, uint32_t size, void* value)
{
	switch ( attrID )
	{
		case MODULE_ATTR_VERSION_STRING:
			strncpy((char*)value, "1.0", size);
			break;
		case MODULE_ATTR_MODULE_ID:
			strncpy((char*)value, MODULE_MOTOR, size);
			break;
		case MODULE_ATTR_MODULE_NAME:
			strncpy((char*)value, "motor Module", size);
			break;
		case MODULE_ATTR_MODULE_DESCRIPTION:
			strncpy((char*)value, "motor module for handling tester motors", size);
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
#define __FUNC__ "{motors}::LoadElcomModule"
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

	EXCCHECK( motors_new( &gs_pmotors));

	EXCCHECK( pParam->SetArg( pParam, "motorModule", ARG_PTR, (void*) gs_pmotors));
	
Error:                       
	*pInstance = gs_pmotors;
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{motors}::InitializeElcomModule"
SElExceptionPtr	_FUNCC InitializeElcomModule( void* pInstance, const char* configuration, SPluginArgsPtr	pParam)
{
	SElExceptionPtr	pexception = NULL;
	SmotorsPtr	p2motors = (SmotorsPtr) pInstance;

	EXCCHECK( gs_pmotors->Initmotors(gs_pmotors));

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motors}::UnloadElcomModule"
SElExceptionPtr	_FUNCC UnloadElcomModule( void* pInstance)
{
	SElExceptionPtr	pexception = NULL;
	SmotorsPtr	p2motors = (SmotorsPtr) pInstance;

	EXCCHECK( motors_delete(&gs_pmotors));

Error:
	EXCRETHROW( pexception);
}

