#if !defined(__TEST_HELPER_FUNCTIONS_H__)
#define __TEST_HELPER_FUNCTIONS_H__

#include <exception/exception.h>
#include <objlist/objlist.h>


typedef enum
{
	Q_I = 0,
	Q_II, 
	Q_III,
	Q_IV
}TeQuadrand;

enum
{
	SUCHE_ANSTIEG = 0,
	SUCHE_F1V,
	SUCHE_F2V,
	SUCHE_END,
	SUCHE_REV,
	SUCHE_F2R,
	SUCHE_F1R,
	SUCHE_ENDE
};

enum
{
	SUCHE_INIT = 0,
	SUCHE_F1,
	SUCHE_F2,
	SUCHE_F3,
	SUCHE_F4,
	SUCHE_F5,
	SUCHE_FIN
};

#if 0    /* formerly excluded lines */
typedef struct _SCalibration
{
	int		calType;
	void*	pCalibData;
} SCalibration, *SCalibrationPtr;
#endif   /* formerly excluded lines */


SElExceptionPtr Camera_RegisterTCPServer(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_login(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_disconnect(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_Trigger(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_Read(
	SObjListPtr		pModules,
	int				IndexCamera,
	char            data[8]
);
SElExceptionPtr Camera_Online(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_Offline(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_Set_Job(
	SObjListPtr		pModules,
	int				IndexCamera,
	char           *Job
);
SElExceptionPtr Camera_Set_Step(
	SObjListPtr		pModules,
	int				IndexCamera,
	int             Step
);



SElExceptionPtr PowerDisconnect(
	SObjListPtr		pModules,
	const char*		xml_file,
	const char*		channel
);
SElExceptionPtr PowerSetPower(
	SObjListPtr		pModules,
	const char*		xml_file,
	const char*		channel,
	double			voltage,
	double			current
);

SElExceptionPtr MeasCurrent(
	SObjListPtr		pModules,
	const char*		xml_file,
	const char*		channel, /* power source channel */
	const char*		channelDmm, /* ch0 - ch32; L: range 2mA; H: range 200mA, 2A */
	double			valueExpected, /* posible ranges: 2mA; 200mA; 2A */
	double*			value, /* measured current value A */
	uint32_t		delay
);

SElExceptionPtr MeasGapStation50(
	SObjListPtr		pModules,
	double*			out0,
	double*			out1 
);

SElExceptionPtr MeasGapStation60(
	SObjListPtr		pModules,
	double*			out0,
	double*			out1 
);
/* switch configuration from file */
SElExceptionPtr SwtchConnect(
	SObjListPtr		pModules, 
	const char*		xml_file,
	const char*		id /* entry id: KLINE, CAN, STEPPERS ... */
);

SElExceptionPtr SwtchDisconnect(
	SObjListPtr		pModules,
	const char*		xml_file,
	const char*		id /* entry id: KLINE, CAN, STEPPERS ... */
);

SElExceptionPtr SwtchCheckPath(
	SObjListPtr		pModules, 
	const char*		xml_file,
	const char*		id, /* entry id: KLINE, CAN, STEPPERS ... */
	bool_t*			exist
);

SElExceptionPtr SwtchSequence(
	SObjListPtr		pModules, 
	const char*		xml_file,
	const char*		id /* sequence id: KLINE, CAN */
);

#endif /* __TEST_HELPER_FUNCTIONS_H__ */
