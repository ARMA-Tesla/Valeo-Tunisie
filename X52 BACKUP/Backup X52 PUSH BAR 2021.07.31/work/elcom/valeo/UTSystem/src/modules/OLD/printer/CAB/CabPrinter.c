#include <cviauto.h>
#include "toolbox.h"
#include <rs232.h>
#include <cvintwrk.h>
#include <userint.h>
#include <tcpsupp.h>
#include "CabPrinter.h"
#include <stdio.h>
#include <formatio.h>
#include <utility.h>

#pragma warning( push)
#pragma warning( disable: 4100 4127)

#define PRINTER_TIMEOUT 	5000


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckDirectory"
void CheckDirectory(const char* directory)
{
	int					size;

	FileExists (directory, &size);
	if(size==-1)
	{
		/* create directory */	
		 MakeDir(directory);
	}
}

/*-------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CyclePrintStatus"
void CyclePrintStatus(int BitRead, char *buffer)   
{
	SElExceptionPtr	pexception = NULL;
	static HANDLE			hevent;
	DWORD 					result;
	HRESULT         	hr = 0;
	CAObjHandle     	pxml = 0; 
	CAObjHandle     	pnode = 0; 
	const char*     	pfile_form_xml = "test_form.xml";
	VBOOL            	is_xml_success = 1;
	VARIANT         	pfile_name_var = CA_VariantEmpty();
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN],
						fileName[MAX_PATHNAME_LEN],
						csv_dir[MAX_PATHNAME_LEN],
						sdate_time[256];
	SYSTEMTIME			st;

	
	CAObjHandle 		pTicket;
	CAObjHandle 		pTicket_new = 0;
	char*           	ptext = NULL;
    int                 HandleFile;
	char				log[256];
	clock_t 		_start_clock;
	double   		time=0;
	_start_clock =  clock();
	
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\imprimante\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	
	/* enreg des data */
	
	memset(log, 0x00, 256);
	
	sprintf (fileName, "%s/Log_BufferCAB.txt", result_dir);
	
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	
	sprintf (log,"-------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\n BitRead = %d \r\n Buffer = %s\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, BitRead, buffer);  
	
	WriteLine (HandleFile, log, sizeof (log));	
	
	CloseFile (HandleFile);
}

#undef __FUNC__
#define __FUNC__	"{CabPrinter}::Connect"
SElExceptionPtr Connect(SPrinterPtr me)
{

SElExceptionPtr	pexception = NULL;

	if (OpenComConfig (me->port, "", me->baudRate, 0, 8, 1, 512, 512) < 0) 
		EXCTHROW( PRINTER_CONNECTION_FAILED, "Probléme Ouverture COM");
//	printf("PORT =%s , BAUDRATE= %s \n\r, me->port, me->baudRate);

Error:
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__	"{CabPrinter}::Disconnect"
SElExceptionPtr Disconnect(SPrinterPtr me)
{

SElExceptionPtr	pexception = NULL;
CloseCom (me->port);

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{CabPrinter}::Write"
SElExceptionPtr Write(SPrinterPtr me,const char* command)
{
	SElExceptionPtr	pexception = NULL;

	ComWrt (me->port, command, strlen (command));

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{CabPrinter}::Reset"
SElExceptionPtr Reset(SPrinterPtr me)
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
#define __FUNC__	"{CabPrinter}::Status"
SElExceptionPtr Status(SPrinterPtr me, int *StatusOnline, char *Error, int *iAmmountLabels, int *StatusInterpreter)
{
	SElExceptionPtr	pexception = NULL;
	char command[128];
	char BufferRead[128];
	char sAmmountLabel[128];
	int  BitRead;
	clock_t 		_start_clock;
	double   		time=0;
	_start_clock =  clock();
//	printf("Start Status\r\n");
	sprintf (command, "%cs", 27);
	ComWrt (me->port, command, strlen (command));
//	printf("Number of Bits in Buffer= %d\n\r", ComWrt (me->port, command, strlen (command)));
	Delay (0.6);
	BitRead = GetInQLen (me->port);
	time=clock()-_start_clock;
//	printf("Time After Buffer Read=%f\r\n",time);
	if(BitRead >= 9)
	{
	//	BitRead=9;
		ComRd (me->port, BufferRead, BitRead);
	//	printf("BufferRead =%s\r\n", BufferRead);
		BufferRead[BitRead] = 0X00;
	//	printf("BufferRead_2 =%s\r\n", BufferRead); 
		//sprintf (message, "STATUS CAB [%s]", BufferRead);
		time=clock()-_start_clock;
	//	printf("Time Before CyclePrintStatus =%f\r\n",time);
		CyclePrintStatus (BitRead, BufferRead); 
	   time=clock()-_start_clock;
	//	printf("Time After CyclePrintStatus =%f\r\n",time);
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
		
		
		
	//	printf("Status inside =%c\n\r",BufferRead[0]);
	//	printf("Type of error =%c\n\r",BufferRead[1]);
	//	printf("Interpreter inside =%c\n\r",BufferRead[8]);
	//	time=clock()-_start_clock;
	//	printf("Time of End Status=%f\r\n",time);
	}
	
	else 
	{
		ComRd (me->port, BufferRead, BitRead);
		CyclePrintStatus (BitRead, BufferRead);
		*StatusOnline	 = -1;
		sprintf (Error, "Unknown Error");
		*iAmmountLabels = -1;
		*StatusInterpreter = -1;
		
		EXCTHROW( PRINTER_READ_STATUS_FAILED, "Printer Read Status Failed");	
	}
	
Error:
	EXCRETHROW( pexception);
}


#pragma warning( pop)
