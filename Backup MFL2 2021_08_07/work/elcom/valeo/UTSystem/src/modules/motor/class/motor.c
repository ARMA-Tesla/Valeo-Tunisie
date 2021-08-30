#include "motor.h"
#include <string.h>
#include <stdio.h>
#include <cviauto.h>
#include "msxmldom.h"
#include <utility.h>
#include <rs232.h>

#define DISCARD_HANDLE(handle) \
	if (handle) \
	{ \
		CA_DiscardObjHandle(handle); \
		handle = 0; \
	}

#define CA_FREE_MEMORY(mem) \
	if (mem) \
	{ \
		CA_FreeMemory(mem); \
		mem = NULL; \
	}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

static SElExceptionPtr motor_open(
		struct _motor*	me,
		const char*			resource,
		const char*			optionString
);
static SElExceptionPtr motor_close(struct _motor* me);

static SElExceptionPtr motor_write(SmotorPtr me, const char* command);

static SElExceptionPtr motor_PresentAlarm(SmotorPtr me, int32_t motor, int32_t* status);
static SElExceptionPtr motor_ControllerStatus(SmotorPtr me, int32_t motor, int32_t* status);
static SElExceptionPtr motor_EnableMotor(SmotorPtr me, int32_t motor);
static SElExceptionPtr motor_AlarmReset(SmotorPtr me, int32_t motor);
static SElExceptionPtr motor_HomeReturn(SmotorPtr me, int32_t motor);
static SElExceptionPtr motor_MoveMotorPosition(SmotorPtr me, int32_t reset, double angle, double speed);
static SElExceptionPtr motor_CurrentPosition(SmotorPtr me, int32_t motor, double* position);
static SElExceptionPtr motor_RequestAngle(SmotorPtr me, int32_t motor);
static SElExceptionPtr motor_ReadAngle(SmotorPtr me, int32_t motor, double* position);




DWORD WINAPI Thread_motor(
  LPVOID lpParameter
);

/*****************************************************************************/

static SElExceptionPtr motors_GetmotorByName(
	struct _motors*	me,
	const char*			motorName,
	SmotorPtr*		motor
);
static SElExceptionPtr motors_GetmotorByIndex(
	struct _motors*	me,
	int32_t				index,
	SmotorPtr*		motor
);
static SElExceptionPtr motors_GetCurrentmotor(
	struct _motors*	me,
	SmotorPtr*		motor
);
static SElExceptionPtr motors_SetCurrentmotorByName(
	struct _motors*	me,
	const char*			motorName
);
static SElExceptionPtr motors_GetFirstmotor(
	struct _motors*	me,
	SmotorPtr*		motor
);
static SElExceptionPtr motors_GetNextmotor(
	struct _motors*	me,
	SmotorPtr*		motor
);
	
static SElExceptionPtr motors_Initmotors(struct _motors* me);
static SElExceptionPtr motors_Closemotors(struct _motors* me);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "motor_new"
SElExceptionPtr motor_new(
	SmotorPtr*	motor,
	const char*		name,
	const char*		optionString
)
{
	SElExceptionPtr	pexception = NULL;
	SmotorPtr		pnew_motor = NULL;
	
	pnew_motor = calloc(1, sizeof(Smotor));
	EXCCHECKALLOC( pnew_motor);
	InitializeCriticalSection(&(pnew_motor->lock));
	
	pnew_motor->motorName = calloc(strlen(name) + 1, sizeof(char));
	EXCCHECKALLOC( pnew_motor->motorName);
	strcpy(pnew_motor->motorName, name);
	
	pnew_motor->Open		= motor_open;
	pnew_motor->Close		= motor_close;
	
	pnew_motor->Write		= motor_write;
	pnew_motor->PresentAlarm	= motor_PresentAlarm;
	pnew_motor->ControllerStatus	= motor_ControllerStatus;
	pnew_motor->EnableMotor		= motor_EnableMotor;
	pnew_motor->AlarmReset			= motor_AlarmReset;
	pnew_motor->HomeReturn			= motor_HomeReturn;
	pnew_motor->MoveMotorPosition			= motor_MoveMotorPosition;
	pnew_motor->CurrentPosition			= motor_CurrentPosition;
	pnew_motor->RequestAngle			= motor_RequestAngle;
	pnew_motor->ReadAngle			= motor_ReadAngle;

	pnew_motor->readEvent = CreateEvent(NULL, 1, 0, NULL);
	
	if (motor)
		*motor = pnew_motor;
	
Error:
	if (pexception && pnew_motor)
		free(pnew_motor);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "motor_delete"
SElExceptionPtr motor_delete(SmotorPtr* motor)
{
	SElExceptionPtr	pexception = NULL;
	
	if (motor && *motor)
	{
		(*motor)->Close(*motor);
		
		if ((*motor)->motorName)
			free((*motor)->motorName);
		
		if ((*motor)->optionString)
			free((*motor)->optionString);
		
		DeleteCriticalSection( &((*motor)->lock) );
		
		free(*motor);
		*motor = NULL;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "motors_new"
SElExceptionPtr motors_new(SmotorsPtr*	motors)
{
	SElExceptionPtr	pexception = NULL;
	SmotorsPtr	pnew_motors = NULL;
	
	pnew_motors = calloc(1, sizeof(Smotors));
	EXCCHECKALLOC( pnew_motors);
	
	pnew_motors->motors = NULL;
	pnew_motors->nummotors = 0;
	pnew_motors->currentIndex = -1;
	
	pnew_motors->GetmotorByName			= motors_GetmotorByName;
	pnew_motors->GetmotorByIndex		= motors_GetmotorByIndex;
	pnew_motors->GetCurrentmotor		= motors_GetCurrentmotor;
	pnew_motors->SetCurrentmotorByName	= motors_SetCurrentmotorByName;
	pnew_motors->GetFirstmotor			= motors_GetFirstmotor;
	pnew_motors->GetNextmotor			= motors_GetNextmotor;
	pnew_motors->Initmotors				= motors_Initmotors;
	pnew_motors->Closemotors			= motors_Closemotors;
	
	if (motors)
		*motors = pnew_motors;
	
Error:
	if (pexception && pnew_motors)
		free(pnew_motors);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "motors_delete"
SElExceptionPtr motors_delete(SmotorsPtr* motors)
{
	SElExceptionPtr	pexception = NULL;
	
	if (motors && *motors)
	{
		EXCCHECK( (*motors)->Closemotors(*motors));
		
		free(*motors);
		*motors = NULL;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "{motor}::Open"
static SElExceptionPtr motor_open(
		struct _motor*	me,
		const char*			resource,
		const char*			optionString
)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr = S_OK;
	VARIANT_BOOL	is_xml_success = 1;
	CAObjHandle		document = 0;
	CAObjHandle		xml_configuration = 0;
	CAObjHandle		xml_node = 0;
	char*			xml_type = NULL;
	char*			xml_value = NULL;
	char			message[1024] = "";
	int		        MaxMotor = 0;
	int				i;
		
	if (me->fd)
	{
		EXCTHROW( -11, "motor Allready Open");
	}
	
	me->optionString = calloc(strlen(optionString) + 1, sizeof(char));
	EXCCHECKALLOC( me->optionString);
	strcpy(me->optionString, optionString);
	
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &document));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (document, NULL, optionString, &is_xml_success));
	
	/* get Max Motor */	
	EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(document, NULL,"/Motor/MaxMotor", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			MaxMotor = atoi(xml_value);
			CA_FREE_MEMORY(xml_value);
		}
		
	/* get Configuration */	
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectSingleNode(document, NULL,
													   "/Motor/Configuration",
													   &xml_configuration));
	if (xml_configuration == 0)
		EXCTHROW( -1, "Missing Configuration Node");
	
	/* Load Specific DLL functions */
	EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL, "@specificDll", &xml_node));
	if (xml_node == 0)
		EXCTHROW( -3, "Missing motor Specific Library");
	
	EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
	me->specLib = LoadLibrary(xml_value);
	if (me->specLib == 0)
	{
		sprintf(message, "motor Library (%s) Loading Failed", xml_value);
		EXCTHROW( -4, message);
	}
	
	me->_InitializeSpec	= (SElExceptionPtr (*)(SmotorPtr))
								GetProcAddress(me->specLib, "Initialize");
	me->_PresentAlarmSpec	= (SElExceptionPtr (*)(SmotorPtr, int32_t, int32_t*))
								GetProcAddress(me->specLib, "PresentAlarm");
	me->_ControllerStatusSpec	= (SElExceptionPtr (*)(SmotorPtr, int32_t, int32_t*))
								GetProcAddress(me->specLib, "ControllerStatus");
	me->_EnableMotorSpec= (SElExceptionPtr (*)(SmotorPtr, int32_t))
								GetProcAddress(me->specLib, "EnableMotor");
	me->_AlarmResetSpec= (SElExceptionPtr (*)(SmotorPtr, int32_t))
								GetProcAddress(me->specLib, "AlarmReset");
	me->_HomeReturnSpec= (SElExceptionPtr (*)(SmotorPtr, int32_t))
								GetProcAddress(me->specLib, "HomeReturn");
	me->_MoveMotorPositionSpec = (SElExceptionPtr (*)(SmotorPtr, int32_t, double, double))
								GetProcAddress(me->specLib, "MoveMotorPosition");
	me->_CurrentPositionSpec = (SElExceptionPtr (*)(SmotorPtr, int32_t, double*))
								GetProcAddress(me->specLib, "CurrentPosition");
	me->_RequestAngleSpec = (SElExceptionPtr (*)(SmotorPtr, int32_t))
								GetProcAddress(me->specLib, "RequestAngle");
	me->_ReadAngleSpec = (SElExceptionPtr (*)(SmotorPtr, int32_t, double*))
								GetProcAddress(me->specLib, "ReadAngle");
	me->_WriteSpec		= (SElExceptionPtr (*)(SmotorPtr,const char*))
								GetProcAddress(me->specLib, "Write");
	me->_callbackThreadSpec= (SElExceptionPtr (*)(SmotorPtr))
								GetProcAddress(me->specLib, "CallbackThread");
	CA_FREE_MEMORY(xml_value);
	DISCARD_HANDLE(xml_node);
	
	/* Get Type of communication */
	EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
												   "@type", &xml_node));
	if (xml_node == 0)
		EXCTHROW( -2, "Missing Configuration Type");

	EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_type));
	DISCARD_HANDLE(xml_node);
	
	if (!stricmp(xml_type, "Serial"))
	{
		DCB		dcb;
		
		dcb.BaudRate	= 9600;
		dcb.ByteSize	= 8;
		dcb.Parity		= NOPARITY;
		dcb.StopBits	= ONESTOPBIT;
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "port", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			me->port = atol(xml_value);
			CA_FREE_MEMORY(xml_value);
		}
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "BaudRate", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			dcb.BaudRate = atol(xml_value);
			CA_FREE_MEMORY(xml_value);
		}
		
		DISCARD_HANDLE(xml_node);
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "DataBits", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			dcb.ByteSize = atol(xml_value);
			CA_FREE_MEMORY(xml_value);
		}
		
		DISCARD_HANDLE(xml_node);
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "StopBits", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			if (!strcmp(xml_value, "1"))
				dcb.StopBits = 1;
			else if (!strcmp(xml_value, "2"))
				dcb.StopBits = 2;
			
			CA_FREE_MEMORY(xml_value);
		}
		
		DISCARD_HANDLE(xml_node);
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(xml_configuration, NULL,
													   "Parity", &xml_node));
		if (xml_node)
		{
			EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(xml_node, NULL, &xml_value));
			if (!stricmp(xml_value, "none"))
				dcb.Parity = NOPARITY;
			else if (!strcmp(xml_value, "Mark"))
				dcb.Parity = MARKPARITY;
			else if (!strcmp(xml_value, "Even"))
				dcb.Parity = EVENPARITY;
			else if (!strcmp(xml_value, "Odd"))
				dcb.Parity = ODDPARITY;
			else
				dcb.Parity = NOPARITY;
			
			CA_FREE_MEMORY(xml_value);
		}
		
		/* Open COM port */
		EXCCHECKCOM( OpenComConfig (me->port, "", dcb.BaudRate, dcb.Parity,
                                    dcb.ByteSize, dcb.StopBits, 512, 512));
										 
			
		/*if (!SetCommState (me->fd, &dcb))
		{
		    EXCTHROW( -7, "Serial Port Setting Failed");
		}
		
		SetCommMask(me->fd, EV_RXCHAR);
		me->termEvent = CreateEvent(NULL, 1, 0, NULL);
		
		if (me->termEvent == NULL)
		{
			EXCTHROW( -8, "Terminal Event Creation Failed");
		} */
		
		me->hThread = CreateThread(NULL, 0, Thread_motor,
								   (void*) me, 0, 0);
		
		if (me->hThread == NULL)
		{
			EXCTHROW( -9, "Thread Start Failed");
		}
	}
	else
	{
		EXCTHROW( -10, "Unsupported Configuration Type");
	}
	
Error:
	if (pexception)
	{
		if (me->specLib)
		{
			FreeLibrary(me->specLib);
			me->specLib = 0;
		}
		
		me->_InitializeSpec			= NULL;
		me->_PresentAlarmSpec		= NULL;
		me->_ControllerStatusSpec	= NULL;
		me->_EnableMotorSpec			= NULL;
		me->_AlarmResetSpec			= NULL;
		me->_HomeReturnSpec			= NULL;
		me->_MoveMotorPositionSpec	= NULL;
		me->_CurrentPositionSpec	= NULL;
		me->_RequestAngleSpec		= NULL;
		me->_ReadAngleSpec			= NULL;
		me->_WriteSpec				= NULL;
		me->_callbackThreadSpec		= NULL;
		
		if (me->termEvent)
		{
			CloseHandle(me->termEvent);
			me->termEvent = 0;
		}
		
		if (me->fd)
		{
			CloseHandle(me->fd);
			me->fd = 0;
		}
	}
		
	DISCARD_HANDLE(xml_node);
	CA_FREE_MEMORY(xml_value);
	CA_FREE_MEMORY(xml_type);
	DISCARD_HANDLE(xml_configuration);
	DISCARD_HANDLE(document);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::Close"
static SElExceptionPtr motor_close(struct _motor* me)
{
	SElExceptionPtr	pexception = NULL;
	
	if (me->fd)
	{
		SetEvent(me->termEvent);
		/* todo: wait for thread end */
		Sleep(500);
		
		me->_InitializeSpec			= NULL;
		me->_PresentAlarmSpec		= NULL;
		me->_ControllerStatusSpec	= NULL;
		me->_EnableMotorSpec			= NULL;
		me->_AlarmResetSpec			= NULL;
		me->_HomeReturnSpec			= NULL;
		me->_MoveMotorPositionSpec	= NULL;
		me->_CurrentPositionSpec	= NULL;
		me->_RequestAngleSpec		= NULL;
		me->_ReadAngleSpec			= NULL;
		me->_WriteSpec				= NULL;
		me->_callbackThreadSpec		= NULL;
		if (me->specLib)
		{
			FreeLibrary(me->specLib);
			me->specLib = 0;
		}
		
		CloseHandle(me->fd);
		me->fd = 0;
		
		CloseHandle(me->hThread);
		me->hThread = 0;
		
		CloseHandle(me->termEvent);
		me->termEvent = 0;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "fncLock"
static SElExceptionPtr motorLock(SmotorPtr me)
{
	EnterCriticalSection( &me->lock);    
	return NULL;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "motorUnLock"
static SElExceptionPtr motorUnLock(SmotorPtr me)
{
	LeaveCriticalSection( &me->lock);   
	return NULL;
}

#undef __FUNC__
#define __FUNC__ "{motor}::Write"
static SElExceptionPtr motor_write(SmotorPtr me, const char* command)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( motorLock(me));

	if (me->_WriteSpec)
		EXCCHECK( me->_WriteSpec(me, command));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::PresentAlarm"
static SElExceptionPtr motor_PresentAlarm(SmotorPtr me, int32_t motor, int32_t* status)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( motorLock(me));

	if (me->_PresentAlarmSpec)
		EXCCHECK( me->_PresentAlarmSpec(me, motor, status));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::ControllerStatus"
static SElExceptionPtr motor_ControllerStatus(SmotorPtr me, int32_t motor, int32_t* status)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( motorLock(me));

	if (me->_ControllerStatusSpec)
		EXCCHECK( me->_ControllerStatusSpec(me, motor, status));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::EnableMotor"
static SElExceptionPtr motor_EnableMotor(SmotorPtr me, int32_t motor)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( motorLock(me));
	
	if (me->_EnableMotorSpec)
		EXCCHECK( me->_EnableMotorSpec(me, motor));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::AlarmReset"
static SElExceptionPtr motor_AlarmReset(SmotorPtr me, int32_t motor)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( motorLock(me));

	if (me->_AlarmResetSpec)
		EXCCHECK( me->_AlarmResetSpec(me, motor));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::HomeReturn"
static SElExceptionPtr motor_HomeReturn(SmotorPtr me, int32_t motor)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( motorLock(me));

	if (me->_HomeReturnSpec)
		EXCCHECK( me->_HomeReturnSpec(me,motor));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{motor}::MoveMotorPosition"
static SElExceptionPtr motor_MoveMotorPosition(SmotorPtr me, int reset, double angle, double speed)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( motorLock(me));

	if (me->_MoveMotorPositionSpec)
		EXCCHECK( me->_MoveMotorPositionSpec(me, reset, angle, speed));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::CurrentPosition"
static SElExceptionPtr motor_CurrentPosition(SmotorPtr me, int32_t motor, double* position)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( motorLock(me));

	if (me->_CurrentPositionSpec)
		EXCCHECK( me->_CurrentPositionSpec(me, motor, position));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::RequestAngle"
static SElExceptionPtr motor_RequestAngle(SmotorPtr me, int32_t motor)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( motorLock(me));

	if (me->_RequestAngleSpec)
		EXCCHECK( me->_RequestAngleSpec(me, motor));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::ReadAngle"
static SElExceptionPtr motor_ReadAngle(SmotorPtr me, int32_t motor, double* position)
{
	SElExceptionPtr	pexception = NULL;
	
	EXCCHECK( motorLock(me));

	if (me->_ReadAngleSpec)
		EXCCHECK( me->_ReadAngleSpec(me, motor, position));
	
Error:
	motorUnLock(me);
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motor}::Thread_motor"
DWORD WINAPI Thread_motor(
  LPVOID lpParameter
)
{
	SElExceptionPtr	pexception = NULL;
	SmotorPtr	me = (SmotorPtr) lpParameter;
	
	if (me->_callbackThreadSpec)
		EXCCHECK( me->_callbackThreadSpec(me));
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "{motors}::GetmotorByName"
static SElExceptionPtr motors_GetmotorByName(
	struct _motors*	me,
	const char*			motorName,
	SmotorPtr*		motor
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	SmotorPtr		found_motor = NULL;
	char			message[1024] = "";
	
	for (i = 0; i < me->nummotors; i++)
	{
		if (!strcmp(me->motors[i]->motorName, motorName))
		{
			found_motor = me->motors[i];
			break;
		}
	}
	
	if (motor)
		*motor = found_motor;
	
	if (found_motor == NULL)
	{
		sprintf(message, "motor Name (%s) Not Found", motorName);
		EXCTHROW( -12, message);
	}
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motors}::GetmotorByIndex"
static SElExceptionPtr motors_GetmotorByIndex(
	struct _motors*	me,
	int32_t				index,
	SmotorPtr*		motor
)
{
	SElExceptionPtr	pexception = NULL;
	
	if (index < me->nummotors && index >= 0)
	{
		if (motor)
			*motor = me->motors[index];
	}
	else
	{
		if (motor)
			*motor = NULL;
		
		EXCTHROW( -13, "Index Out of Range");
	}
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motors}::GetCurrentmotor"
static SElExceptionPtr motors_GetCurrentmotor(
	struct _motors*	me,
	SmotorPtr*		motor
)
{
	SElExceptionPtr	pexception = NULL;
	
	if (me->currentIndex >= 0 && me->nummotors > 0)
	{
		if (motor)
			*motor = me->motors[me->currentIndex];
	}
	else
	{
		if (motor)
			*motor = NULL;
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motors}::SetCurrentmotorByName"
static SElExceptionPtr motors_SetCurrentmotorByName(
	struct _motors*	me,
	const char*			motorName
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	SmotorPtr		motor = NULL;
	char			message[1024] = "";

	for (i = 0; i < me->nummotors; i++)
	{
		if (!strcmp(me->motors[i]->motorName, motorName))
		{
			me->currentIndex = i;
			break;
		}
	}
	
	if (i >= me->nummotors)
	{
		sprintf(message, "motor Name (%s) Not Found", motorName);
		EXCTHROW( -12, message);
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motors}::GetFirstmotor"
static SElExceptionPtr motors_GetFirstmotor(
	struct _motors*	me,
	SmotorPtr*		motor
)
{
	SElExceptionPtr	pexception = NULL;
	
	me->currentIndex = 0;

	if(me->nummotors > 0)
	{
		if (motor)
			*motor = me->motors[0];
	}
	else
	{
		if (motor)
			*motor = NULL;
	}

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motors}::GetNextmotor"
static SElExceptionPtr motors_GetNextmotor(
	struct _motors*	me,
	SmotorPtr*		motor
)
{
	SElExceptionPtr	pexception = NULL;

	if (++(me->currentIndex) >= me->nummotors)
	{
		me->currentIndex = me->nummotors - 1;
		if (motor)
			*motor = NULL;
	}
	else
	{
		if (motor)
			*motor = me->motors[me->currentIndex];
	}
	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motors}::Initmotors"
static SElExceptionPtr motors_Initmotors(struct _motors* me)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr = S_OK;
	VARIANT_BOOL	is_xml_success = 1;
	VARIANT			file;
	CAObjHandle		document = 0;
	CAObjHandle		motor = 0;
	CAObjHandle		motor_name_node = 0;
	CAObjHandle		motor_list = 0;
	char*			xml_value = NULL;
	char*			motor_xml = NULL;
	
	EXCCHECKCOM( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &document));
	
	CA_VariantSetCString (&file, "modules/motor.xml");
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentload(document, NULL, file, &is_xml_success));
	
	EXCCHECKCOM( MSXML_IXMLDOMDocumentselectNodes(document, NULL, "/Motors/Motor", &motor_list));
	EXCCHECKCOM( MSXML_IXMLDOMNodeListreset (motor_list, NULL));
	
	EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode(motor_list, NULL, &motor));
	while (motor)
	{
		EXCCHECKCOM( MSXML_IXMLDOMNodeselectSingleNode(motor, NULL, "@name", &motor_name_node));
		EXCCHECKCOM( MSXML_IXMLDOMNodeGettext(motor_name_node, NULL, &xml_value));
		EXCCHECKCOM( MSXML_IXMLDOMNodeGetxml(motor, NULL, &motor_xml));
		
		me->motors = realloc( me->motors, (me->nummotors + 1) * sizeof(SmotorPtr));
		EXCCHECKALLOC( me->motors);
		
		EXCCHECK( motor_new( &me->motors[me->nummotors], xml_value, NULL));
		EXCCHECK( me->motors[me->nummotors]->Open(me->motors[me->nummotors], NULL, motor_xml));
		me->nummotors++;
		
		CA_FREE_MEMORY(xml_value);
		CA_FREE_MEMORY(motor_xml);
		DISCARD_HANDLE(motor_name_node);
		DISCARD_HANDLE(motor);
		
		EXCCHECKCOM( MSXML_IXMLDOMNodeListnextNode(motor_list, NULL, &motor));
	}
	
Error:
	CA_FREE_MEMORY(xml_value);
	CA_FREE_MEMORY(motor_xml);
	DISCARD_HANDLE(motor_name_node);
	DISCARD_HANDLE(motor);
	DISCARD_HANDLE(motor_list);
	DISCARD_HANDLE(document);
	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{motors}::Closemotors"
static SElExceptionPtr motors_Closemotors(struct _motors* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;
	
	for (i = 0; i < me->nummotors; i++)
	{
		EXCCHECK( motor_delete(&(me->motors[i])));
	}
	free(me->motors);
	me->motors = NULL;
	me->nummotors = 0;
	
Error:
	EXCRETHROW( pexception);
}
