
#ifndef _LASER_SPECIFIC_KEYENCE_
#define _LASER_SPECIFIC_KEYENCE_

#include <laser/class/lasers.h>

#if defined(WINDOWS)
	#define LASER_API		__declspec( dllexport )
#elif defined(UNIX)
	#define LASER_API		extern "C"
#else
	#define LASER_API
#endif

LASER_API SElExceptionPtr _FUNCC Connect(SLaserPtr me);
LASER_API SElExceptionPtr _FUNCC Disconnect(SLaserPtr me);
LASER_API SElExceptionPtr _FUNCC WriteString(SLaserPtr me, int program, int block, const char* string);
LASER_API SElExceptionPtr _FUNCC ReadString(SLaserPtr me, int program, int block, const char* File_Line_String, int* String_OK,char *buffer);
LASER_API SElExceptionPtr _FUNCC RunProgram(SLaserPtr me, int program, int block);
LASER_API SElExceptionPtr _FUNCC EnableMarking(SLaserPtr me);
LASER_API SElExceptionPtr _FUNCC MarkerReady(SLaserPtr me, int *Ready);
LASER_API SElExceptionPtr _FUNCC StartLasering(SLaserPtr me);
LASER_API SElExceptionPtr _FUNCC ErrorStatus(SLaserPtr me, int *ErrorStatus, char *ErrorCode) ;
LASER_API SElExceptionPtr _FUNCC ErrorClear(SLaserPtr me);

#endif /* _LASER_SPECIFIC_KEYENCE_ */
