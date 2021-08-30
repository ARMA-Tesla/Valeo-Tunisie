#if !defined(__BATTERYCHECK_H__)
#define __BATTERYCHECK_H__

#include <eltypes.h>
#include <exception/Exception.h>
#include <windows.h>
#include <eventfactory/EventFactory.h>

#if defined(__cplusplus) || defined(__cplusplus__)
	extern "C" {
#endif

#define EVNT_BATTERY_ON_BATTERY			"EVNT_BATTERY_ON_BATTERY"
#define EVNT_BATTERY_ON_POWER			"EVNT_BATTERY_ON_POWER"

typedef struct _SBattery
{
	SElExceptionPtr (*GetStatus)(
			struct _SBattery*	me,
			uint8_t*			online,
			uint8_t*			batteryFlag,
			uint8_t*			lifePercent
		);
	SElExceptionPtr (*GetLifeTime)(
			struct _SBattery*	me,
			int32_t*			lifeRemaining,
			int32_t*			totalRemaining
		);

/*******************************************
 * PRIVATE
 *******************************************/
	uint8_t		_onlineState;		/*!< Online state: 0 - off-line; 1 - on-line; 255 - unknown state */
	uint8_t		_batteryFlag;		/*!< Battery Flag: 1 - High ( > 66% ); 2 - Low ( < 33%); 4 - Critical ( < 5%)
													   8 - Charging
													   128 - no system batery
													   255 - Unknown status */
	uint8_t		_lifePercent;		/*!< Percentage of Full Battery Charge Remaining ( 0 - 100%, 255 - Unknown status ) */
	int32_t		_lifeRemaining;		/*!< The number of seconds of battery life remaining, or –1 if remaining seconds are unknown */
	int32_t		_totalRemaining;	/*!< The number of seconds of battery life when at full charge, or –1 if full battery lifetime is unknown */
	HANDLE		_StopEvent;			/*!< Stop Event for Loop */
} SBattery, *SBatteryPtr;

SElExceptionPtr	battery_new(SBatteryPtr* pBattery);
SElExceptionPtr	battery_delete(SBatteryPtr* pBattery);


#if defined(__cplusplus) || defined(__cplusplus__)
	}
#endif

#endif /* __BATTERYCHECK_H__ */ 
