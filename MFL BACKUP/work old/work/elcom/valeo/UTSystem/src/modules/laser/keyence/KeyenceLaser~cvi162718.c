#include <ansi_c.h>
#include <rs232.h>
#include <cvintwrk.h>
#include <userint.h>
#include <tcpsupp.h>
#include "KeyenceLaser.h"
#include <stdio.h>
#include <formatio.h>
#include <utility.h>

#pragma warning( push)
#pragma warning( disable: 4100 4127)

#define LASER_TIMEOUT 		25000

#define TIMEOUT_LASER_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_LASER_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( -1, "Timeout Run Program"); } else

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::Connect"
SElExceptionPtr Connect(SLaserPtr me)
{

SElExceptionPtr	pexception = NULL;

if (OpenComConfig (me->port, "", me->baudRate, 0, 8, 1, 512, 512) < 0) 
	EXCTHROW( LASER_CONNECTION_FAILED, "Probléme Ouverture COM");

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::Disconnect"
SElExceptionPtr Disconnect(SLaserPtr me)
{

SElExceptionPtr	pexception = NULL;

	CloseCom (me->port);

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::RunProgram"
SElExceptionPtr RunProgram(SLaserPtr me, int program, int block)
{
	SElExceptionPtr	pexception = NULL;
	char command[1024];
	char buffer[1024];
	int  received = 0;
	
	TIMEOUT_LASER_INIT();
	
	/* Run program XXXX */
	do{
		 FlushInQ (me->port);
		 FlushOutQ (me->port);
		 
		sprintf(command, "WX,ProgramNo=%04d\r", program);//WX,ProgramNo=0000[CR]
		//printf("WX,ProgramNo=%04d\r", program);//WX,ProgramNo=0000[CR]
		ComWrt (me->port, command, strlen (command));	 
		Sleep(50);
		ReturnRS232Err (); 
		
		Sleep(50);
		
		received  = GetInQLen (me->port);
		if (received > 0)
		{
			ComRd (me->port, buffer, received);
			if (FindPattern (buffer, 0, -1, "WX,OK", 0, 0) >= 0)   
				break;	
		}
		
		TIMEOUT_LASER_CHECK(LASER_TIMEOUT);
			
	}while(TRUE);


Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::WriteString"
SElExceptionPtr WriteString(SLaserPtr me, int program, int block, const char* string)
{
	SElExceptionPtr	pexception = NULL;
	char command[1024];
	char buffer[1024];
	int  received = 0;
	
	TIMEOUT_LASER_INIT();
	
	/* Ecrire string dans program XXXX , block=000 */
	do{
		 FlushInQ (me->port);
		 FlushOutQ (me->port);
		 
		sprintf(command, "WX,PRG=%04d,BLK=%03d,CharacterString=%s\r", program, block, string);//WX,PRG=0000,BLK=000,CharacterString=ABC[CR]	
		//printf("WX,PRG=%04d,BLK=%03d,CharacterString=%s\r", program, block, string);
		ComWrt (me->port, command, strlen (command));	 
		Sleep(50);
		ReturnRS232Err (); 
		
		Sleep(50);
		
		received  = GetInQLen (me->port);
		if (received > 0)
		{
			ComRd (me->port, buffer, received);
			if (FindPattern (buffer, 0, -1, "WX,OK", 0, 0) >= 0)   
				break;	
		}
		
		TIMEOUT_LASER_CHECK(LASER_TIMEOUT);
			
	}while(TRUE);
	

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::EnableMarking"
SElExceptionPtr EnableMarking(SLaserPtr me)
{
	SElExceptionPtr	pexception = NULL;
	char command[1024];
	char buffer[1024];
	int  received = 0;
	
	TIMEOUT_LASER_INIT();
	
	/* Enable marking */
	do{
		 FlushInQ (me->port);
		 FlushOutQ (me->port);
		 
		sprintf(command, "WX,TriggerLock=0\r");//WX,TriggerLock=0[CR]	
		ComWrt (me->port, command, strlen (command));	 
		Sleep(50);
		ReturnRS232Err (); 
		
		Sleep(50);
		
		received  = GetInQLen (me->port);
		if (received > 0)
		{
			ComRd (me->port, buffer, received);
			if (FindPattern (buffer, 0, -1, "WX,OK", 0, 0) >= 0)   
				break;	
		}
		
		TIMEOUT_LASER_CHECK(LASER_TIMEOUT);
			
	}while(TRUE);

   	/* Check Enable marking */
	do{
		 FlushInQ (me->port);
		 FlushOutQ (me->port);
		 
		sprintf(command, "RX,TriggerLock\r");//RX,TriggerLock[CR]	
		ComWrt (me->port, command, strlen (command));	 
		Sleep(50);
		ReturnRS232Err (); 
		
		Sleep(50);
		
		received  = GetInQLen (me->port);
		if (received > 0)
		{
			ComRd (me->port, buffer, received);
			if (FindPattern (buffer, 0, -1, "RX,OK,0", 0, 0) >= 0)   
				break;	
		}
		
		TIMEOUT_LASER_CHECK(LASER_TIMEOUT);
			
	}while(TRUE);
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::MarkerReady"
SElExceptionPtr MarkerReady(SLaserPtr me, int *Ready)
{
	SElExceptionPtr	pexception = NULL;
	char command[1024];
	char buffer[1024];
	int  received = 0;
	
	TIMEOUT_LASER_INIT();

	*Ready = -1;	

	/* Check status marking */
//	do{
		 FlushInQ (me->port);
		 FlushOutQ (me->port);
		 
		sprintf(command, "RX,Ready\r");//RX,Ready[CR]	
		ComWrt (me->port, command, strlen (command));	 
		Sleep(50);
		ReturnRS232Err (); 
		
		Sleep(50);
		
		received  = GetInQLen (me->port);
		if (received > 0)
		{
			ComRd (me->port, buffer, received);
			if (FindPattern (buffer, 0, -1, "RX,OK,0", 0, 0) >= 0) 
			{
				*Ready = 0;
				//break;	
			}
			else if (FindPattern (buffer, 0, -1, "RX,OK,1", 0, 0) >= 0) 
			{
				*Ready = 1;
				//break;	
			}
			else if (FindPattern (buffer, 0, -1, "RX,OK,2", 0, 0) >= 0) 
			{
				*Ready = 2;
				//break;	
			}
			else 
				*Ready = -1;	
		}
//		
		TIMEOUT_LASER_CHECK(LASER_TIMEOUT);
			
//	}while(TRUE);
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::StartLasering"
SElExceptionPtr StartLasering(SLaserPtr me)
{
	SElExceptionPtr	pexception = NULL;
	char command[1024];
	char buffer[1024];
	int  received = 0;
	clock_t 		_start_test_clock;
	double          time;
	
	TIMEOUT_LASER_INIT();
	_start_test_clock =  clock(); 
	
	sprintf(command, "WX,StartMarking\r");//WX,StartMarking[CR]	
	
	ComWrt (me->port, command, strlen (command));
	
	Sleep(50);
	
	ReturnRS232Err (); 
		
	/* Run program XXXX */
		 FlushInQ (me->port);
		 
		 FlushOutQ (me->port);
		 
		sprintf(command, "WX,GuideLaser=2\r");//WX,GuideLaser=2[CR]	
		
		ComWrt (me->port, command, strlen (command));
		
		Sleep(50);
		
		ReturnRS232Err (); 
		



Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::Reset"
SElExceptionPtr Reset(SLaserPtr me)
{
	SElExceptionPtr	pexception = NULL;
	char command[128];

	sprintf (command, "%ct", 27);
	ComWrt (me->port, command, strlen (command));
	Delay (0.5);
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::Status"
SElExceptionPtr Status(SLaserPtr me, int *StatusOnline, char *Error, int *iAmmountLabels, int *StatusInterpreter)
{
	SElExceptionPtr	pexception = NULL;
	char command[128];
	char BufferRead[128];
	char sAmmountLabel[128];
	int  BitRead;
	;
	
	
	sprintf (command, "%cs", 27);
	ComWrt (me->port, command, strlen (command));
	Delay (0.5);
	BitRead = GetInQLen (me->port);
	
	if(BitRead == 9)
	{
		ComRd (me->port, BufferRead, BitRead);
		BufferRead[BitRead] = 0X00;
		//sprintf (message, "STATUS CAB [%s]", BufferRead);
		
		/* Online (Y=Yes, N=No) */
		if (BufferRead[0] == 'N')
			*StatusOnline	 = 0;
		else if (BufferRead[0] == 'Y')
			*StatusOnline	 = 1;
		
		/* Type of error */
		switch (BufferRead[1])
		{
		case '-' :  sprintf (Error, "No error"); break;
		case 'o' :  sprintf (Error, "Compressed air-error"); break;
		case 'B' :  sprintf (Error, "Protocol error/ invalid barcode data");break;
		default :  sprintf (Error, "Unknown Error");
		}
	
		/* amount of labels to print */
		CopyBytes (sAmmountLabel, 0, BufferRead, 2, 6);
		*iAmmountLabels = atoi(sAmmountLabel);
		
			
		/* Interpreter active (Y=Yes = print job is in process, N=No= printer in Standby mode) */
		if (BufferRead[8] == 'N')
			*StatusInterpreter = 0;
		else if (BufferRead[8] == 'Y')
			*StatusInterpreter = 1;
		
	}
	
	else 
	{
		*StatusOnline	 = -1;
		sprintf (Error, "Unknown Error");
		*iAmmountLabels = -1;
		*StatusInterpreter = -1;
		EXCTHROW( LASER_READ_STATUS_FAILED, "Printer Read Status Failed");	
	}
	
Error:
	EXCRETHROW( pexception);
}


#pragma warning( pop)
