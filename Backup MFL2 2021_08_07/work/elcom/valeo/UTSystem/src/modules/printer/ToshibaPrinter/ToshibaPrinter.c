#include <cvintwrk.h>
#include <userint.h>
#include <tcpsupp.h>
#include "ToshibaPrinter.h"
#include <stdio.h>
#include <formatio.h>
#include <utility.h>

#pragma warning( push)
#pragma warning( disable: 4100 4127)

#define PRINTER_TIMEOUT 	5000


int uiHandleConnection = -1;


#undef __FUNC__
#define __FUNC__	"{ToshibaPrinter}::Connect"
SElExceptionPtr Connect(SPrinterPtr me)
{
	SElExceptionPtr	pexception = NULL;
	
	
uiHandleConnection = InetTelnetOpen (me->adressIP, me->port, 0); 
	if(uiHandleConnection < 0)	
		EXCTHROW( PRINTER_CONNECTION_FAILED, "Printer Connection Failed");

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{ToshibaPrinter}::Disconnect"
SElExceptionPtr Disconnect(SPrinterPtr me)
{
	SElExceptionPtr	pexception = NULL;

if (uiHandleConnection>=0)
{
InetTelnetClose (uiHandleConnection);
uiHandleConnection = -1;
}
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{ToshibaPrinter}::Write"
SElExceptionPtr Write(SPrinterPtr me, const char* command)
{
	SElExceptionPtr	pexception = NULL;
    int BitWrite=0; 

	if (uiHandleConnection>=0)
		InetTelnetWrite(uiHandleConnection, command, "\r\n", strlen(command), &BitWrite, 3000);

	if (BitWrite != strlen(command))
			EXCTHROW( PRINTER_SEND_FAILED, "Printer Send Failed");

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{ToshibaPrinter}::Reset"
SElExceptionPtr Reset(SPrinterPtr me)
{
	SElExceptionPtr	pexception = NULL;
	int BitWrite=0; 

	if (uiHandleConnection>=0)
		InetTelnetWrite(uiHandleConnection, "{WR|}", "\r\n", strlen("{WR|}"), &BitWrite, 2000);
	
	if (BitWrite != strlen("{WR|}"))
		EXCTHROW( PRINTER_RESET_FAILED, "Printer Reset Failed");	
	
Error:
	EXCRETHROW( pexception);
}
#undef __FUNC__
#define __FUNC__	"{ToshibaPrinter}::Status"
SElExceptionPtr Status(SPrinterPtr me, int *Status)
{
	SElExceptionPtr	pexception = NULL;
	int Result;
	int BitWrite=0;
	int BitRead=0;
	
	static int HandleFile;
	char sBuff[128] = "";
	
	char BufferRead[13+1];
	char sStatus[2+1];

	if (uiHandleConnection>=0)
	{
	InetTelnetWrite(uiHandleConnection, "{WS|}", "\r\n", strlen("{WS|}"), &BitWrite, 2000);
	Sleep(500);
	InetTelnetRead (uiHandleConnection, BufferRead, 13, &BitRead, 3000);
	}
	
	if(BitRead>=13)
	{
		if ((BufferRead[0] == 0x01) //SOH
			&& (BufferRead[1] == 0x02)//STX
			&& (BufferRead[4] == 0x31)//Status Request Command
			&& (BufferRead[9] == 0x03) //ETX
			&& (BufferRead[10] == 0x04)) //EOT
			{
			HandleFile = OpenFile ("log.dat", VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
			sprintf (sBuff, "%s - %s : Status Printer : %s", DateStr (), TimeStr (), BufferRead);
			WriteLine (HandleFile, sBuff, -1);
			
			CopyBytes (sStatus, 0, BufferRead, 2, 2);
			sStatus[2] = 0X00;
		    *Status = atoi(sStatus);
			
			CloseFile (HandleFile);
			}
	}
	else 
	{
		printf ("ToshibaPrinter.dll : Printer Read Status Failed\r\n");
		EXCTHROW( PRINTER_READ_STATUS_FAILED, "Printer Read Status Failed");	
	}
Error:
	EXCRETHROW( pexception);
}

#pragma warning( pop)
