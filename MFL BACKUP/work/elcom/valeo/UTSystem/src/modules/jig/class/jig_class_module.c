
#include "jig_class.h"
#include <cvirte.h>
#include <modules/module.h>
#include <eventfactory/EventFactory.h>
#include <guid.h>

static SElEventFactoryPtr	gs_EventFactory = NULL;
static SJigPtr				gs_pJig = NULL;

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
#define __FUNC__ "{Jig Class}::GetModuleAttribute"
SElExceptionPtr	_FUNCC GetModuleAttribute( uint32_t  attrID, uint32_t size, void* value)
{
	switch ( attrID )
	{
		case MODULE_ATTR_VERSION_STRING:
			strncpy((char*)value, "1.0", size);
			break;
		case MODULE_ATTR_MODULE_ID:
			strncpy((char*)value, MODULE_JIG_CLASS, size);
			break;
		case MODULE_ATTR_MODULE_NAME:
			strncpy((char*)value, "Jig Class Module", size);
			break;
		case MODULE_ATTR_MODULE_DESCRIPTION:
			strncpy((char*)value, "JIG class module which can use specific JIG dll", size);
			break;
		case MODULE_ATTR_REQUIRED_MODULES:
			strncpy((char*)value, MODULE_SENSORS";"MODULE_DBS, size);
			break;
		case MODULE_ATTR_BUILD_DATETIME:
			strncpy((char*)value, __DATE__" "__TIME__, size);
			break;
	}
	return NULL;
}

#undef __FUNC__
#define __FUNC__ "{Jig Class}::LoadElcomModule"
SElExceptionPtr	_FUNCC LoadElcomModule(
	const char* configuration,
	void** pInstance,
	SPluginArgsPtr	pParam
)
{
	SElExceptionPtr    pexception = NULL;

	EXCCHECK( pParam->GetArg( pParam,
							  "WindowHandle",
							  sizeof(hWnd),
							  &hWnd));

	EXCCHECK( pParam->GetArg( pParam,
							 "EventFactory",
							 sizeof(SElEventFactoryPtr),
							 &gs_EventFactory));

	EXCCHECK( jig_new( &gs_pJig));

	if ( gs_EventFactory )
	{
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_JIG_LOADED));
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_JIG_UNLOADED));
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_JIG_ITEM_INSERTED));
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_JIG_ITEM_RELEASED));
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_JIG_ITEM_LOCK));
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_JIG_ITEM_UNLOCK));
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_JIG_ITEM_CONNECTED));
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_JIG_ITEM_DISCONNECTED));
		EXCCHECK( gs_EventFactory->RegisterEvent(gs_EventFactory, EVNT_BARCODE_READ));
	}
	
	EXCCHECK( gs_pJig->jig_SetEventFactory(gs_pJig, gs_EventFactory));
	
Error:                       
	*pInstance = gs_pJig;
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Jig Class}::InitializeElcomModule"
SElExceptionPtr	_FUNCC InitializeElcomModule( void* pInstance, const char* configuration, SPluginArgsPtr pParam)
{
	SElExceptionPtr	pexception = NULL;
	SJigPtr			p2jig = (SJigPtr) pInstance;
	void*			sensorAPI = NULL;
	void*			databaseAPI = NULL;
	
	EXCCHECK( pParam->GetArg( pParam,
							  "DatabaseModule",
							  sizeof(databaseAPI),
							  &(databaseAPI)));

	EXCCHECK( gs_pJig->jig_SetDatabaseAPI( gs_pJig, databaseAPI));

	EXCCHECK( pParam->GetArg( pParam,
							  "SensorModule",
							  sizeof(sensorAPI),
							  &(sensorAPI)));

	EXCCHECK( gs_pJig->jig_SetSensorAPI( gs_pJig, sensorAPI));

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Jig Class}::UnloadElcomModule"
SElExceptionPtr	_FUNCC UnloadElcomModule( void* pInstance)
{
	SElExceptionPtr    pexception = NULL;
	SJigPtr			p2jig = (SJigPtr) pInstance;
	
	EXCCHECK( jig_delete(&gs_pJig));

Error:
	EXCRETHROW( pexception);
}

