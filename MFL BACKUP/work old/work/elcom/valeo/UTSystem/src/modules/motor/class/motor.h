#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <windows.h>
#include <exception/Exception.h>

#define MAX_MOTOR_NAME				128
#define MAX_BUFFER_SIZE				20

#define MOTOR_ERROR_TIMEOUT				-50001

typedef struct _motor
{
	char*				motorName;
	char*				optionString;
	CRITICAL_SECTION	lock;
	HANDLE				fd;
	int 				port;
	HANDLE				termEvent;
	HANDLE				readEvent;
	HANDLE				specLib;
	HANDLE				hThread;
	double				CurrentAngle;
	int32_t				status;
	int32_t				readMode;
	char				readBuffer[MAX_BUFFER_SIZE];
	
	SElExceptionPtr (*Open)(
		struct _motor*	me,
		const char*			resource,
		const char*			optionString
	);
	SElExceptionPtr (*Close)(struct _motor* me);
	SElExceptionPtr (*Write)(struct _motor* me, const char* command);
	
	SElExceptionPtr (*PresentAlarm)(struct _motor* me, int32_t motor, int32_t* status);
	SElExceptionPtr (*ControllerStatus)(struct _motor* me, int32_t motor, int32_t* status);
	SElExceptionPtr (*EnableMotor)(struct _motor* me, int32_t motor);
	SElExceptionPtr (*AlarmReset)(struct _motor* me, int32_t motor);
	SElExceptionPtr (*HomeReturn)(struct _motor* me, int32_t motor);
	SElExceptionPtr (*MoveMotorPosition)(struct _motor* me, int32_t reset, double angle, double speed);
	SElExceptionPtr (*CurrentPosition)(struct _motor* me, int32_t motor, double *position);
	SElExceptionPtr (*RequestAngle)(struct _motor* me, int32_t motor);
	SElExceptionPtr (*ReadAngle)(struct _motor* me, int32_t motor, double *position);
	
	/* Private */
	SElExceptionPtr (*_InitializeSpec)(struct _motor* me);
	SElExceptionPtr (*_PresentAlarmSpec)(struct _motor* me, int32_t motor, int32_t* status);
	SElExceptionPtr (*_ControllerStatusSpec)(struct _motor* me,  int32_t motor, int32_t* status);
	SElExceptionPtr (*_EnableMotorSpec)(struct _motor* me, int32_t motor);
	SElExceptionPtr (*_AlarmResetSpec)(struct _motor* me, int32_t motor);
	SElExceptionPtr (*_HomeReturnSpec)(struct _motor* me, int32_t motor);
	SElExceptionPtr (*_MoveMotorPositionSpec)(struct _motor* me, int32_t reset, double angle, double speed);
	SElExceptionPtr (*_CurrentPositionSpec)(struct _motor* me, int32_t motor, double* position);
	SElExceptionPtr (*_RequestAngleSpec)(struct _motor* me, int32_t motor);
	SElExceptionPtr (*_ReadAngleSpec)(struct _motor* me, int32_t motor, double* position);
	SElExceptionPtr (*_WriteSpec)(struct _motor* me, const char* command);
	SElExceptionPtr (*_callbackThreadSpec)(struct _motor* me);
	
} Smotor, *SmotorPtr;


typedef struct _motors
{
	SmotorPtr*		motors;
	int32_t			nummotors;
	int32_t			currentIndex;
	
	SElExceptionPtr (*GetmotorByName)(
		struct _motors*	me,
		const char*			motorName,
		SmotorPtr*		motor
	);
	SElExceptionPtr (*GetmotorByIndex)(
		struct _motors*	me,
		int32_t				index,
		SmotorPtr*		motor
	);
	SElExceptionPtr (*GetCurrentmotor)(
		struct _motors*	me,
		SmotorPtr*		motor
	);
	SElExceptionPtr (*SetCurrentmotorByName)(
		struct _motors*	me,
		const char*			motorName
	);
	SElExceptionPtr (*GetFirstmotor)(
		struct _motors*	me,
		SmotorPtr*		motor
	);
	SElExceptionPtr (*GetNextmotor)(
		struct _motors*	me,
		SmotorPtr*		motor
	);
	
	SElExceptionPtr (*Initmotors)(struct _motors* me);
	SElExceptionPtr (*Closemotors)(struct _motors* me);
	
} Smotors, *SmotorsPtr;

SElExceptionPtr motors_new(SmotorsPtr* motors);
SElExceptionPtr motors_delete(SmotorsPtr* motors);

SElExceptionPtr motor_new(
	SmotorPtr*	motor,
	const char*		name,
	const char*		optionString
);
SElExceptionPtr motor_delete(SmotorPtr* motor);

#endif /* _MOTOR_H_ */
