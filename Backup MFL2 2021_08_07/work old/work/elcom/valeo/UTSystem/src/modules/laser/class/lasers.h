#ifndef _LASERS_H_
#define _LASERS_H_

#include <windows.h>
#include <exception/Exception.h>

#define MAX_LASER_NAME			128
#define MAX_BUFFER_SIZE				20

#define LASER_CONNECTION_FAILED				-1
#define LASER_SEND_FAILED						-1
#define LASER_RESET_FAILED					-1
#define LASER_READ_STATUS_FAILED				-1



 
typedef struct _Laser
{
	char*				laserName;
	char*				optionString;
	WORD				baudRate;
	WORD				port;
	CRITICAL_SECTION	lock;
	HANDLE				fd;
	HANDLE				termEvent;
	HANDLE				readEvent;
	HANDLE				specLib;
	HANDLE				hThread;
	int32_t				status;
	int32_t				readMode;
	char				readBuffer[MAX_BUFFER_SIZE];
	
	SElExceptionPtr (*Open)(struct _Laser* me,const char*	resource, const char* optionString);
	SElExceptionPtr (*Close)(struct _Laser* me);
	
	SElExceptionPtr (*Connect)(struct _Laser* me);
	SElExceptionPtr (*Disconnect)(struct _Laser* me);
	
	SElExceptionPtr (*WriteString)(struct _Laser* me, int program, int block, const char* string);
	SElExceptionPtr (*ReadString)(struct _Laser* me, int program, int block, const char* File_Line_String, int* String_OK,char *buffer);
	SElExceptionPtr (*RunProgram)(struct _Laser* me, int program, int block);
 	SElExceptionPtr (*EnableMarking)(struct _Laser* me);
	SElExceptionPtr (*MarkerReady)(struct _Laser* me, int *Ready);
	SElExceptionPtr (*StartLasering)(struct _Laser* me);
	SElExceptionPtr (*ErrorStatus)(struct _Laser* me, int* ErrorStatus, char* ErrorCode);  
	SElExceptionPtr (*ErrorClear)(struct _Laser* me);

	/* Private */
	SElExceptionPtr (*_ConnectSpec)(struct _Laser* me);
	SElExceptionPtr (*_DisconnectSpec)(struct _Laser* me);
	SElExceptionPtr (*_WriteStringSpec)(struct _Laser* me, int program, int block, const char* string);
	SElExceptionPtr (*_ReadStringSpec)(struct _Laser* me, int program, int block, const char* File_Line_String, int* String_OK,char *buffer);
	SElExceptionPtr (*_RunProgramSpec)(struct _Laser* me, int program, int block);
	SElExceptionPtr (*_EnableMarkingSpec)(struct _Laser* me);
	SElExceptionPtr (*_MarkerReadySpec)(struct _Laser* me, int *Ready);
	SElExceptionPtr (*_StartLaseringSpec)(struct _Laser* me);
	SElExceptionPtr (*_ErrorStatusSpec)(struct _Laser* me, int* ErrorStatus, char* ErrorCode);  
	SElExceptionPtr (*_ErrorClearSpec)(struct _Laser* me); 
	
	
} SLaser, *SLaserPtr;


typedef struct _Lasers
{
	SLaserPtr*	lasers;
	int32_t			numLasers;
	int32_t			currentIndex;
	
	SElExceptionPtr (*GetLaserByName)(
		struct _Lasers*	me,
		const char*			laserName,
		SLaserPtr*		laser
	);
	SElExceptionPtr (*GetLaserByIndex)(
		struct _Lasers*	me,
		int32_t				index,
		SLaserPtr*		laser
	);
	SElExceptionPtr (*GetCurrentLaser)(
		struct _Lasers*	me,
		SLaserPtr*		laser
	);
	SElExceptionPtr (*SetCurrentLaserByName)(
		struct _Lasers*	me,
		const char*			laserName
	);
	SElExceptionPtr (*GetFirstLaser)(
		struct _Lasers*	me,
		SLaserPtr*		laser
	);
	SElExceptionPtr (*GetNextLaser)(
		struct _Lasers*	me,
		SLaserPtr*		laser
	);
	
	SElExceptionPtr (*InitLasers)(struct _Lasers* me);
	SElExceptionPtr (*CloseLasers)(struct _Lasers* me);
	
} SLasers, *SLasersPtr;

SElExceptionPtr lasers_new(SLasersPtr* lasers);
SElExceptionPtr lasers_delete(SLasersPtr* lasers);

SElExceptionPtr laser_new(
	SLaserPtr*	laser,
	const char*		name,
	const char*		optionString
);
SElExceptionPtr laser_delete(SLaserPtr* laser);

#endif /* _LASERS_H_ */
