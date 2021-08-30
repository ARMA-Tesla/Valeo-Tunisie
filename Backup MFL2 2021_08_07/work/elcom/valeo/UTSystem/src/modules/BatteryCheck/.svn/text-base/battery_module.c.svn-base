#include <modules/module.h>
#include <eventfactory/EventFactory.h>
#include <guid.h>
#include <batteryCheck/batteryCheck.h>
#include <process.h>

#pragma warning( push)
#pragma warning( disable: 4100)

const char* HISTORY = "[2008-01-24] KP 1.00\n"
					  " * Module Created";

static SElEventFactoryPtr	gs_EventFactory = NULL;
static SBatteryPtr     		gs_pBattery = NULL;
static HANDLE				gs_loopThread = NULL;
void*						hwnd = NULL;

static SElExceptionPtr RegisterEventFunction(void);
uint32_t __stdcall InternalLoop(void* param);

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
#if (!defined(_CVI_)&&defined(_DEBUG))
			//_CrtSetBreakAlloc(78);
#endif
			break;
		case DLL_PROCESS_DETACH:
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
			_CrtDumpMemoryLeaks();
#endif
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
			strncpy_s((char*)value, size, "1.0", size);
			break;
		case MODULE_ATTR_MODULE_ID:
			strncpy_s((char*)value, size, MODULE_BATTERY, size);
			break;
		case MODULE_ATTR_MODULE_NAME:
			strncpy_s((char*)value, size, "Battery Module", size);
			break;
		case MODULE_ATTR_MODULE_DESCRIPTION:
			strncpy_s((char*)value, size, "Module for battery checking", size);
			break;
		case MODULE_ATTR_REQUIRED_MODULES:	
			strncpy_s((char*)value, size, "", size);
			break;
		case MODULE_ATTR_BUILD_DATETIME:
			strncpy_s((char*)value, size, __DATE__" "__TIME__, size);
			break;
	}
	return NULL;
}

#undef __FUNC__
#define __FUNC__ "{Battery}::LoadElcomModule"
SElExceptionPtr	_FUNCC LoadElcomModule(
	const char* configuration,
	void** pInstance,
	SPluginArgsPtr	pParam
)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( pParam->GetArg( pParam,
							  "WindowHandle",
							  sizeof(hwnd),
							  &hwnd));

	EXCCHECK( battery_new( &gs_pBattery));

Error:                       
	*pInstance = gs_pBattery;
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Battery}::InitializeElcomModule"
SElExceptionPtr	_FUNCC InitializeElcomModule( void* pInstance, const char* configuration, SPluginArgsPtr pParam)
{
	SElExceptionPtr	pexception = NULL; 
	
	EXCCHECK( pParam->GetArg( pParam,
							 "EventFactory",
							 sizeof(gs_EventFactory),
							 &gs_EventFactory));

	EXCCHECK( gs_EventFactory->RegisterEvent( gs_EventFactory, EVNT_BATTERY_ON_BATTERY));
	EXCCHECK( gs_EventFactory->RegisterEvent( gs_EventFactory, EVNT_BATTERY_ON_POWER));

	gs_loopThread = (HANDLE) _beginthreadex(NULL, 0, InternalLoop, gs_pBattery, 0, NULL);

Error:
	EXCRETHROW( pexception);   
}

#undef __FUNC__
#define __FUNC__ "{Battery}::UnloadElcomModule"
SElExceptionPtr	_FUNCC UnloadElcomModule( void* pInstance)
{
	SElExceptionPtr    pexception = NULL;        
	
	SetEvent(gs_pBattery->_StopEvent);
	WaitForSingleObject(gs_loopThread, 1000);
	EXCCHECK( battery_delete(&gs_pBattery));

	CloseHandle( gs_loopThread);
	
Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"{Battery}::InternalLoop"
uint32_t __stdcall InternalLoop(void* param)
{
	SElExceptionPtr		pexception = NULL;
	SBatteryPtr			me = (SBatteryPtr) param;
	SYSTEM_POWER_STATUS	power_status;

	while (1)
	{
		if (WaitForSingleObject( me->_StopEvent, 10000) == WAIT_OBJECT_0)
		{
			break;
		}

		GetSystemPowerStatus(&power_status);
		me->_onlineState = power_status.ACLineStatus;
		me->_batteryFlag = power_status.BatteryFlag;
		me->_lifePercent = power_status.BatteryLifePercent;
		me->_lifeRemaining = power_status.BatteryLifeTime;
		me->_totalRemaining = power_status.BatteryFullLifeTime;

		if (power_status.ACLineStatus == 0)
		{
			/* set event EVNT_BATTERY_ON_BATTERY */
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, EVNT_BATTERY_ON_BATTERY, NULL));
		}
		else if (power_status.ACLineStatus == 1)
		{
			/* set event EVNT_BATTERY_ON_POWER */
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, EVNT_BATTERY_ON_POWER, NULL));
		}
	}

Error:
	return 0;
}

#pragma warning( pop)
