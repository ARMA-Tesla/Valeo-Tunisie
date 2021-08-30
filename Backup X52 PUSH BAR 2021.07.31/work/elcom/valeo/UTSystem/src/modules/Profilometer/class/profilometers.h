#ifndef _PROFILOMETERS_H_
#define _PROFILOMETERS_H_

#include <windows.h>
#include <exception/Exception.h>

#define MAX_PROFILOMETER_NAME			128
#define MAX_BUFFER_SIZE				20

typedef enum {
LJV7IF_JUDGE_RESULT_HI = 0x01, // HI
LJV7IF_JUDGE_RESULT_GO = 0x02, // GO
LJV7IF_JUDGE_RESULT_LO = 0x04 // LO
} LJV7IF_JUDGE_RESULT;

typedef enum {
LJV7IF_MEASURE_DATA_INFO_VALID = 0x00, // Normal measurement data
LJV7IF_MEASURE_DATA_INFO_ALARM = 0x01, // Measurement alarm data
LJV7IF_MEASURE_DATA_INFO_WAIT = 0x02 // Judgment wait data
} LJV7IF_MEASURE_DATA_INFO;

typedef struct {
BYTE byDataInfo;
BYTE byJudge;
BYTE reserve[2];
FLOAT fValue;
} MEASURE_DATA;

typedef struct {
BYTE abyIpAddress[4];
WORD wPortNo;
BYTE reserve[2];
} ETHERNET_CONFIG;

typedef struct _Profilometer
{
	char*				profilometerName;
	char*				optionString;
	char				adressIP[13+1];
	WORD				port;
	int                 Index;
	CRITICAL_SECTION	lock;
	HANDLE				fd;
	HANDLE				termEvent;
	HANDLE				readEvent;
	HANDLE				specLib;
	HANDLE				hThread;
	int32_t				status;
	int32_t				readMode;
	char				readBuffer[MAX_BUFFER_SIZE];
	
	SElExceptionPtr (*Open)(
		struct _Profilometer*	me,
		const char*			resource,
		const char*			optionString
	);
	SElExceptionPtr (*Close)(struct _Profilometer* me);
	SElExceptionPtr (*Write)(struct _Profilometer* me, const char* command);
	SElExceptionPtr (*GetStatus)(struct _Profilometer* me, int32_t* status);
	SElExceptionPtr (*StartMeasure)(struct _Profilometer* me);
	SElExceptionPtr (*StopMeasure)(struct _Profilometer* me);
	SElExceptionPtr (*Trigger)(struct _Profilometer* me);
	SElExceptionPtr (*GetMeasurementValue)(struct _Profilometer* me, MEASURE_DATA* pMeasureData);
	
	/* Private */
	LONG (*_InitializeSpec)(void);
    LONG __stdcall  (*_FinalizeSpec)(void);
	LONG __stdcall (*_EthernetOpenSpec)(LONG lDeviceId, ETHERNET_CONFIG* pEthernetConfig);
	LONG __stdcall (*_StartMeasureSpec)(LONG lDeviceId);
	LONG __stdcall (*_StopMeasureSpec)(LONG lDeviceId);
	LONG __stdcall (*_TriggerSpec)(LONG lDeviceId);
	LONG __stdcall (*_GetMeasurementValueSpec)(LONG lDeviceId, MEASURE_DATA* pMeasureData);
	
	SElExceptionPtr (*_GetStatusSpec)(struct _Profilometer* me, int32_t* status);
	SElExceptionPtr (*_WriteSpec)(struct _Profilometer* me, const char* command);
	SElExceptionPtr (*_SendCommandSpec)(struct _Profilometer* me, const char* commandName);
	SElExceptionPtr (*_callbackThreadSpec)(struct _Profilometer* me);
	

	
} SProfilometer, *SProfilometerPtr;


typedef struct _Profilometers
{
	SProfilometerPtr*	profilometers;
	int32_t			numProfilometers;
	int32_t			currentIndex;
	
	SElExceptionPtr (*GetProfilometerByName)(
		struct _Profilometers*	me,
		const char*			profilometerName,
		SProfilometerPtr*		profilometer
	);
	SElExceptionPtr (*GetProfilometerByIndex)(
		struct _Profilometers*	me,
		int32_t				index,
		SProfilometerPtr*		profilometer
	);
	SElExceptionPtr (*GetCurrentProfilometer)(
		struct _Profilometers*	me,
		SProfilometerPtr*		profilometer
	);
	SElExceptionPtr (*SetCurrentProfilometerByName)(
		struct _Profilometers*	me,
		const char*			profilometerName
	);
	SElExceptionPtr (*GetFirstProfilometer)(
		struct _Profilometers*	me,
		SProfilometerPtr*		profilometer
	);
	SElExceptionPtr (*GetNextProfilometer)(
		struct _Profilometers*	me,
		SProfilometerPtr*		profilometer
	);
	
	SElExceptionPtr (*InitProfilometers)(struct _Profilometers* me);
	SElExceptionPtr (*CloseProfilometers)(struct _Profilometers* me);
	
} SProfilometers, *SProfilometersPtr;

SElExceptionPtr profilometers_new(SProfilometersPtr* profilometers);
SElExceptionPtr profilometers_delete(SProfilometersPtr* profilometers);

SElExceptionPtr profilometer_new(
	SProfilometerPtr*	profilometer,
	const char*		name,
	const char*		optionString
);
SElExceptionPtr profilometer_delete(SProfilometerPtr* profilometer);

#endif /* _PROFILOMETERS_H_ */
