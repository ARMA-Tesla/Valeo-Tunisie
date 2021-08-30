#include <rs232.h>
#include <ansi_c.h>
#include <userint.h>
#include "smart_automation_Motor.h"
#include <stdio.h>
#include <utility.h>
#include <formatio.h>

#pragma warning( push)
#pragma warning( disable: 4100 4127)

#define MOTOR_TIMEOUT				200
#define INTER_CMD					100

#define MOTOR_READ_MODE_NORMAL	0
#define MOTOR_READ_MODE_STATUS	1

// Control codes for the Gateway motor

#define SM_ADRESS      129      


#define CR_CHAR			32	// END character

int FindString(char *Buffer, char *DebutTAG, char *FinTAG, char *StrVal , char *Message)
{
	int index1=0,
	    index2=0;
	
	if(DebutTAG != NULL)
		{
			if((index1=FindPattern (Buffer, 0, -1, DebutTAG, 0, 0))<0)
				{
					sprintf(Message,"ERREUR : TAG n'est Pas Trouve \"%s\"\0",DebutTAG);  
						return -100;
					}
			}
	
	else if (DebutTAG == NULL) 
		{
				index1=0;
				 DebutTAG ="\0";
				}
	
	if((index2 = FindPattern (Buffer, index1, -1, FinTAG, 0, 0))<0)
	{
		sprintf(Message,"ERREUR : TAG n'est Pas Trouve \"%s\"\0",FinTAG);  
		return -200;
	}
	
	index2=index2-(index1+strlen(DebutTAG));
	index1=index1+strlen(DebutTAG);
	CopyBytes (StrVal,0,Buffer,index1,index2);
	StrVal[index2]='\0';
	
	return index2;
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::Initialize" 
SElExceptionPtr Initialize(SmotorPtr me)
{
	SElExceptionPtr	pexception = NULL;
	char command[128];
	char Buffer[256];
	int  read;
	int  status;

/* motor initialisation CMD */
memset(command, 0X00, 128);
command[0] = SM_ADRESS;
sprintf (command, "%sZS%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

memset(command, 0X00, 128);
command[0] = SM_ADRESS;
sprintf (command, "%sOFF%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

memset(command, 0X00, 128);
command[0] = SM_ADRESS;	
sprintf (command, "%sEND%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

memset(command, 0X00, 128);
command[0] = SM_ADRESS;
sprintf (command, "%sRUN%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

/* check STARTUP_OK CMD */
read = GetInQLen (me->port);
ComRd (me->port, Buffer, read);
status = FindPattern (Buffer, 0, -1, "STARTUP_OK", 0, 0);
if (status < 0)
	EXCTHROW( MOTOR_ERROR_TIMEOUT, "Motor Timeouted");
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::PresentAlarm"  
SElExceptionPtr PresentAlarm(SmotorPtr me, int32_t motor, int32_t* status)
{
	SElExceptionPtr	pexception = NULL;


Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::ControllerStatus"   
SElExceptionPtr ControllerStatus(SmotorPtr me, int32_t motor, int32_t* status)
{
	SElExceptionPtr	pexception = NULL;
	char 			command[128];
	int  			read;
	char 			Buffer[1024*10] ;
	char            sValue[128];
	int             ret;
	int 			value;
	char            message[128]="";
	clock_t _start_read;
	double   time;
	
FlushInQ (me->port);
FlushOutQ (me->port);
	
/* Reset to sensor */
memset(command, 0X00, 128);
command[0] = SM_ADRESS;	
sprintf (command, "%st=1%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(200);
ReturnRS232Err ();


/* wait for received bytes */
memset(Buffer, 0X00, 1024*4);
_start_read =  clock();
do
{
read = GetInQLen (me->port);
if (read < 1024*5)
	ComRd (me->port, Buffer, read);
else 
	ComRd (me->port, Buffer, 1024*5);

ret = FindPattern (Buffer, 0, -1, "RESET:OK", 0, 0);
time = (double)(clock()-_start_read);
Sleep(200);
}while(ret < 0 && time < 7000);

if (ret < 0)
	EXCTHROW( MOTOR_ERROR_TIMEOUT, "Motor Timeouted");

	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::CurrentPosition"   
SElExceptionPtr CurrentPosition(SmotorPtr me, int32_t motor, double* angle)
{
	SElExceptionPtr	pexception = NULL;
	char 			command[128];
	int  			read;
	char 			Buffer[1024];
	char            sValue[128];
	int             status;
	int 			value;
	char            message[128]="";

FlushInQ (me->port);
FlushOutQ (me->port);
memset(command, 0X00, 128);
command[0] = SM_ADRESS;	
sprintf (command, "%sr=1%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(35);
ReturnRS232Err ();

memset(Buffer, 0X00, 128);
read = GetInQLen (me->port);
if (read < 1024)
ComRd (me->port, Buffer, read);
status = FindString (Buffer, ":", "_", sValue, message);
if (status < 0)
	{
	*angle = 0; 
	EXCTHROW( MOTOR_ERROR_TIMEOUT, message);
	}
*angle = atof(sValue) / 555.56; 

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::RequestAngle"   
SElExceptionPtr RequestAngle(SmotorPtr me, int32_t motor)
{
	SElExceptionPtr	pexception = NULL;
	char 			command[128];
	int  			read;
	char 			Buffer[128];
	char            sValue[128];
	int             status;
	int 			value;
	char            message[128]="";

FlushInQ (me->port);
FlushOutQ (me->port);
memset(command, 0X00, 128);
command[0] = SM_ADRESS;	
sprintf (command, "%sr=1%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(15);
ReturnRS232Err ();


Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::ReadAngle"   
SElExceptionPtr ReadAngle(SmotorPtr me, int32_t motor, double* angle)
{
	SElExceptionPtr	pexception = NULL;
	char 			command[128];
	int  			read;
	char 			Buffer[1024];
	char            sValue[128];
	int             status;
	int 			value;
	char            message[128]="";

memset(Buffer, 0X00, 128);
read = GetInQLen (me->port);
if (read < 1024)
ComRd (me->port, Buffer, read);
status = FindString (Buffer, ":", "_", sValue, message);
if (status < 0)
	{
	*angle = 999.0; 
	EXCTHROW( MOTOR_ERROR_TIMEOUT, message);
	}
*angle = atof(sValue) / 555.56; 

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::EnableMotor" 
SElExceptionPtr EnableMotor(SmotorPtr me, int32_t motor)
{
	SElExceptionPtr	pexception = NULL;
	char 			command[128];
	int  			read;
	char 			Buffer[1024];
	int             status;
	
/* motor start mouvement CMD */
command[0] = SM_ADRESS;	
memset(command, 0X00, 128);
sprintf (command, "%ss=0%c", command, CR_CHAR); 
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();	

memset(Buffer, 0X00, 128);
read = GetInQLen (me->port);
if (read < 1024)
ComRd (me->port, Buffer, read);
status = FindPattern (Buffer, 0, -1, "STOP", 0, 0);
if (status < 0)
	EXCTHROW( MOTOR_ERROR_TIMEOUT, "Motor Timeouted");

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::AlarmReset" 
SElExceptionPtr AlarmReset(SmotorPtr me, int32_t motor)
{
	SElExceptionPtr	pexception = NULL;
	char 			command[128];
	int  			read;
	char 			Buffer[1024];
	int  			status;

	
/* Reset to angle 0° CMD */
memset(command, 0X00, 128);
command[0] = SM_ADRESS;	
sprintf (command, "%sO=0%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

memset(command, 0X00, 128);
command[0] = SM_ADRESS;	
sprintf (command, "%sr=1%c", command, CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

memset(Buffer, 0X00, 128);
read = GetInQLen (me->port);
if (read < 1024)
ComRd (me->port, Buffer, read);
status = FindPattern (Buffer, 0, -1, "ANGLE:0", 0, 0);
if (status < 0)
	EXCTHROW( MOTOR_ERROR_TIMEOUT, "Motor Timeouted");

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::HomeReturn"  
SElExceptionPtr HomeReturn(SmotorPtr me, int32_t motor)
{
	SElExceptionPtr	pexception = NULL;


Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{SmartAutomation}::MoveMotorPosition" 
SElExceptionPtr MoveMotorPosition(SmotorPtr me, int reset, double angle, double speed)
{
	SElExceptionPtr	pexception = NULL;
	char 			command[128];
    char 			readBuf[2048];
	double 			value;
	int 			i,scanned;
	
	
/* motor set angle CMD */
value  = angle * 555.5;  //555.5 = 4000 X 50 / 360°  ==>  4000 = data encoder | 50 = gear 
memset(command, 0X00, 128);
command[0] = SM_ADRESS;	
sprintf (command, "%sp=%d%c", command, (int)floor(value), CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

/* motor set speed CMD */
value  = speed * ((32212 * 50)/60); //speed in RPM
memset(command, 0X00, 128);
command[0] = SM_ADRESS;	
sprintf (command, "%sv=%d%c", command, (int)floor(value), CR_CHAR);
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

/* motor start mouvement CMD */
command[0] = SM_ADRESS;	
memset(command, 0X00, 128);
sprintf (command, "%ss=1%c", command, CR_CHAR); 
ComWrt (me->port, command, StringLength (command));
Sleep(INTER_CMD);
ReturnRS232Err ();

Error:
	EXCRETHROW( pexception);
}

#pragma warning( pop)
