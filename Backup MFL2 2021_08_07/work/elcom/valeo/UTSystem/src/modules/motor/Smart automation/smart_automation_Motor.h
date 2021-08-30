#ifndef _MOTOR_SPECIFIC_IAI_
#define _MOTOR_SPECIFIC_IAI_

#include <motor/class/motor.h>

#if defined(WINDOWS)
	#define MOTOR_API		__declspec( dllexport )
#elif defined(UNIX)
	#define MOTOR_API		extern "C"
#else
	#define MOTOR_API
#endif

MOTOR_API SElExceptionPtr _FUNCC Initialize(SmotorPtr me);

MOTOR_API SElExceptionPtr _FUNCC PresentAlarm(SmotorPtr me, int32_t motor, int32_t* status);
MOTOR_API SElExceptionPtr _FUNCC ControllerStatus(SmotorPtr me, int32_t motor, int32_t* status);

MOTOR_API SElExceptionPtr _FUNCC EnableMotor(SmotorPtr me, int32_t motor);
MOTOR_API SElExceptionPtr _FUNCC AlarmReset(SmotorPtr me, int32_t motor);
MOTOR_API SElExceptionPtr _FUNCC HomeReturn(SmotorPtr me, int32_t motor);
MOTOR_API SElExceptionPtr _FUNCC MoveMotorPosition(SmotorPtr me, int reset, double angle, double speed);
MOTOR_API SElExceptionPtr _FUNCC CurrentPosition(SmotorPtr me, int32_t motor,  double *position);
MOTOR_API SElExceptionPtr _FUNCC RequestAngle(SmotorPtr me, int32_t motor);
MOTOR_API SElExceptionPtr _FUNCC ReadAngle(SmotorPtr me, int32_t motor,  double *position);

#endif /* _MOTOR_SPECIFIC_IAI_ */
