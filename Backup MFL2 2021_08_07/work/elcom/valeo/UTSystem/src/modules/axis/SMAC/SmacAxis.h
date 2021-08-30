#ifndef _AXIS_SPECIFIC_IAI_
#define _AXIS_SPECIFIC_IAI_

#include <axis/class/axis.h>

#if defined(WINDOWS)
	#define AXIS_API		__declspec( dllexport )
#elif defined(UNIX)
	#define AXIS_API		extern "C"
#else
	#define AXIS_API
#endif

AXIS_API SElExceptionPtr _FUNCC Initialize(SaxisPtr me,/*int32_t ID_SMAC,*/ int32_t axis);
AXIS_API SElExceptionPtr _FUNCC PresentAlarm(SaxisPtr me, int32_t axis, int32_t* status);
AXIS_API SElExceptionPtr _FUNCC AlarmReset(SaxisPtr me, int32_t axis);
AXIS_API SElExceptionPtr _FUNCC HomeReturn(SaxisPtr me, int32_t axis);
AXIS_API SElExceptionPtr _FUNCC EnableAxis(SaxisPtr me, int32_t axis);
AXIS_API SElExceptionPtr _FUNCC StopAxis(SaxisPtr me, int32_t axis);
AXIS_API SElExceptionPtr _FUNCC CurrentPosition(SaxisPtr me, /*int32_t ID_SMAC ,*/int32_t axis,  double *position);
AXIS_API SElExceptionPtr _FUNCC CurrentAnalog(SaxisPtr me, int32_t axis,  double *analog, double Coefficient, double Offsets);
AXIS_API SElExceptionPtr _FUNCC SetPosition(SaxisPtr me, int32_t axis, double position, double speed);


#endif /* _AXIS_SPECIFIC_IAI_ */
