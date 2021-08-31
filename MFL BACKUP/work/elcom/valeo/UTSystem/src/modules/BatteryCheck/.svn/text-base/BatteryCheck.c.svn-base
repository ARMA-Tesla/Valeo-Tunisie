#include "batteryCheck.h"
#include <windows.h>

static SElExceptionPtr battGetStatus(
	struct _SBattery*	me,
	uint8_t*			online,
	uint8_t*			batteryFlag,
	uint8_t*			lifePercent
);
static SElExceptionPtr battGetLifeTime(
	struct _SBattery*	me,
	int32_t*			lifeRemaining,
	int32_t*			totalRemaining
);

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "battery_new"
 SElExceptionPtr battery_new(SBatteryPtr* pBattery)
{
	SElExceptionPtr    	pexception = NULL;
	
	*pBattery = calloc( 1, sizeof(SBattery));
	EXCCHECKALLOC( *pBattery);

	(*pBattery)->GetStatus = battGetStatus;
	(*pBattery)->GetLifeTime = battGetLifeTime;
	
	(*pBattery)->_onlineState = 255;
	(*pBattery)->_batteryFlag = 255;
	(*pBattery)->_lifePercent = 255;
	(*pBattery)->_lifeRemaining = -1;
	(*pBattery)->_totalRemaining = -1;

	(*pBattery)->_StopEvent = CreateEvent(NULL, 1, 0, NULL);

Error:
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "battery_delete"
SElExceptionPtr	battery_delete(SBatteryPtr* pBattery)
{
	SElExceptionPtr	pexception = NULL;

	if(pBattery && *pBattery)
	{
		if ((*pBattery)->_StopEvent)
			CloseHandle( (*pBattery)->_StopEvent);

		free( *pBattery);
		*pBattery = NULL;
	}

	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Battery::GetStatus"
static SElExceptionPtr battGetStatus
(
	struct _SBattery*	me,
	uint8_t*			online,
	uint8_t*			batteryFlag,
	uint8_t*			lifePercent
)
{
	SElExceptionPtr	pexception = NULL;
	
	if (online)
		*online = me->_onlineState;

	if (batteryFlag)
		*batteryFlag = me->_batteryFlag;

	if (lifePercent)
		*lifePercent = me->_lifePercent;
	
Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"Battery::GetLifeTime"
static SElExceptionPtr battGetLifeTime
(
	struct _SBattery*	me,
	int32_t*			lifeRemaining,
	int32_t*			totalRemaining
)
{
	SElExceptionPtr	pexception = NULL;
	
	if (lifeRemaining)
		*lifeRemaining = me->_lifeRemaining;

	if (totalRemaining)
		*totalRemaining = me->_totalRemaining;

Error:
	EXCRETHROW(pexception);
}
