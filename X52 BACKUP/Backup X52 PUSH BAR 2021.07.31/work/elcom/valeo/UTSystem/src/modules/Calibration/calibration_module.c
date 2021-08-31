
#include <modules/module.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "calibration.h"
#include <cvirte.h>
#include <guid.h>

static SCalibrationPtr    	gs_pCalibration = NULL;

int __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
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
SElExceptionPtr	_FUNCC GetModuleAttribute(uint32_t attrID, uint32_t size, void* value)
{
	switch ( attrID )
	{
		case MODULE_ATTR_VERSION_STRING:
			strncpy((char*)value, "1.0", size);
			break;
		case MODULE_ATTR_MODULE_ID:
			strncpy((char*)value, MODULE_CALIBRATION, size);
			break;
		case MODULE_ATTR_MODULE_NAME:
			strncpy((char*)value, "Calibration Module", size);
			break;
		case MODULE_ATTR_MODULE_DESCRIPTION:
			strncpy((char*)value, "Module which recalculate value depends on the scale", size);
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
#define __FUNC__ "LoadElcomModule"
SElExceptionPtr	_FUNCC LoadElcomModule(
	const char* configuration,
	void** pInstance,
	SPluginArgsPtr	pParam
)
{
	SElExceptionPtr    pexception = NULL;

	EXCCHECK( calibration_new( &gs_pCalibration));
	
Error:                       
	*pInstance = gs_pCalibration;
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "InitializeElcomModule"
SElExceptionPtr	_FUNCC InitializeElcomModule( void* pInstance, const char* configuration, SPluginArgsPtr	pParam)
{
	SElExceptionPtr    pexception = NULL; 
	
	EXCCHECK( gs_pCalibration->NewList( gs_pCalibration));
	
Error:
	EXCRETHROW( pexception);   
}

#undef __FUNC__
#define __FUNC__ "UnloadElcomModule"
SElExceptionPtr	_FUNCC UnloadElcomModule( void* pInstance)
{
	SElExceptionPtr    pexception = NULL;        
	
	EXCCHECK( calibration_delete(&gs_pCalibration));
	
Error:
	EXCRETHROW( pexception);   
}
