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
/*---------------------------------------------------------------------------*/

void LogPrinterCOMError( char *Error)   
{
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN],
						fileName[MAX_PATHNAME_LEN],
						sdate_time[256],
						buffer[2048] = "";
    int                 HandleFile;
	char				log[256];
	SYSTEMTIME			st;
	
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\printerCOM\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	
	/* enreg des data */
	memset(log, 0x00, 256);
	sprintf (fileName, "%s/Log_PrinterCOM.txt", result_dir);
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	sprintf (log,"---------------------------------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\n %s\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,Error);  
	WriteLine (HandleFile, log, sizeof (log));	
	CloseFile (HandleFile);
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
	char            log[1024]= "";		
	
	/*******************************************************	DOCUMENTATION	*****************************************************
	*********************************************************************************************************************************
			"ESCs" : Printer status query
			Answer : XYNNNNNNZ 
	
	X = Online (Y=Yes, N=No)
	Y = Type of error:				- --------------------------------------------------------------------------------- No error
									a ---- Applicator error- ----- Applicator did not reach the upper position (1) This status request can only be processed on printing systems which are equipped with anattached applicator !
									b ---- Applicator error- ------ Applicator did not reach the lower position (1)
									c ---- Applicator error– ------------------------------ Vacuum plate is empty (1)
									d ---- Applicator error- ------------------------------------- Label not deposit (1)
									e ---- Applicator error- ----------------------------------------- Host stop/error (1)
									f ----- Applicator error- -------------------------- Reflective sensor blocked (1)
									g ---- Applicator error --------------------------------------- Tamp pad 90° error
									h ---- Applicator error ----------------------------------------- Tamp pad 0° error
									i ----- Applicator error --------------------------------Table not in front position
									j ----- Applicator error -------------------------------- Table not in rear position
									k ---- Applicator error --------------------------------------------------- Head liftet
									l ----- Applicator error --------------------------------------------------Head down
									m ------------------------------------------------------------- Scanresult negative(2) Scanresult negative requires an optional barcode scanner. The availability of the optional barcodescanner depends on the printing system.
									n ------------------------------------------------------------- global Network error (3) Network error: Only on printers with the built in network interface. (No print server)
									o -------------------------------------------------------------- Compressed air-error
									r ---------------------------------------------------------------------------- RFID -error
									s ------------------------------------ System fault (immediately after power on)
									u ----------------------------------------------------------------------------- USB error
									x ---- Stacker full - printer goes on Pause (only with a specified cutter)	
		Error types: (continued)
									A------------------------------ Applicator error (only older firmware releases)
									B---------------------------------------------Protocol error/ invalid barcode data
									C------------------------------------------------------------------ Memory card error
									D----------------------------------------------- Printhead open / Pinchroller open
									E ----------------------------------------- Synchronization error (No label found)
									F ------------------------------------------------------------------------ Out of Ribbon
									G --------------------------------------------------------------- PPP reload required
									H-----------------------------------------------------------Heating voltage problem
									M ------------------------------------------------------------------- Cutter jammed (4)
									N----------------------------------------------- Label material too thick (cutter) (4)
									O ---------------------------------------------------------------------- Out of memory
									P -------------------------------------------------------------------------- Out of paper
									R- ----------------------------------- Ribbon dectected in Thermal direct mode
									S ------------------------------------------------------------Ribonsaver malfunction
									V ---------------------------------------------------------------- Input buffer overflow
									W ------------------------------------------------------------ Print head overheated
									X -------------------------------------------------------------------- External I/O error
									Y ---------------------------------------------------------------------- Print head error
									Z ----------------------------------------------------------------- Printhead damaged
	NNNNNN = amount of labels to print		
	Z = Interpreter active (Y=Yes = print job is in process,N=No= printer in Standby mode) 
	
	
 	*******************************************************************************************************************************
	*******************************************************************************************************************************/
	
	_start_clock =  clock();
	sprintf (command, "%cs", 27);
	ComWrt (me->port, command, strlen (command));
	Delay (0.6);
	BitRead = GetInQLen (me->port);
	time=clock()-_start_clock;
	if(BitRead >= 9)
	{

		ComRd (me->port, BufferRead, BitRead);
		BufferRead[BitRead] = 0X00;
		time=clock()-_start_clock;
		CyclePrintStatus (BitRead, BufferRead); 
	 	time=clock()-_start_clock;

		
		sprintf(log,"CabPrinter.c => Status => BufferRead_Debut_Fonction : %s \r \n ",BufferRead);
		LogPrinterCOMError(log);
		
		if (BufferRead[0] == 'N')
			*StatusOnline	 = 0;
		else if (BufferRead[0] == 'Y')
			*StatusOnline	 = 1;
		/* Type of error */
		switch (BufferRead[1])
		{
			case '-' :  sprintf (Error, "No error"); break;
			case 'a' :  sprintf (Error, "Applicator did not reach the upper position (1)");break;
			case 'b' :  sprintf (Error, "Applicator did not reach the lower position (1)");break;
			case 'c' :  sprintf (Error, "Vacuum plate is empty (1)");break;
			case 'd' :  sprintf (Error, "Label not deposit (1)");break;
			case 'e' :  sprintf (Error, "Host stop/error");break;
			case 'f' :  sprintf (Error, "Reflective sensor blocked (1)");break;
			case 'g' :  sprintf (Error, "Tamp pad 90° error");break;
			case 'h' :  sprintf (Error, "Tamp pad 0° error");break;
			case 'i' :  sprintf (Error, "Table not in front position");break;
			case 'j' :  sprintf (Error, "Table not in rear position");break;
			case 'k' :  sprintf (Error, "Head liftet");break;
			case 'l' :  sprintf (Error, "Head down");break;
			case 'm' :  sprintf (Error, "Scanresult negative(2)");break;
			case 'n' :  sprintf (Error, "global Network error (3)");break;
			case 'o' :  sprintf (Error, "Compressed air-error");break;
			case 'r' :  sprintf (Error, "RFID -error");break;
			case 's' :  sprintf (Error, "System fault (immediately after power on");break;
			case 'u' :  sprintf (Error, "USB error");break;
			case 'x' :  sprintf (Error, "Stacker full - printer goes on Pause (only with a specified cutter");break;
			case 'A' :  sprintf (Error, "Applicator error (only older firmware releases");break;
			case 'B' :  sprintf (Error, "Protocol error/ invalid barcode data");break;
			case 'C' :  sprintf (Error, "Memory card error");break;
			case 'D' :  sprintf (Error, "Printhead open / Pinchroller open");break;
			case 'E' :  sprintf (Error, "Synchronization error (No label found");break;
			case 'F' :  sprintf (Error, "Out of Ribbon");break;
			case 'G' :  sprintf (Error, "PPP reload required");break;
			case 'H' :  sprintf (Error, "Heating voltage problem");break;
			case 'M' :  sprintf (Error, "Cutter jammed (4)");break;
			case 'N' :  sprintf (Error, "Label material too thick (cutter) (4)");break;
			case 'O' :  sprintf (Error, "Out of memory");break;
			case 'P' :  sprintf (Error, "Out of paper");break;
			case 'R' :  sprintf (Error, "Ribbon dectected in Thermal direct mode");break;
		    case 'S' :  sprintf (Error, "Ribonsaver malfunction");break;
			case 'V' :  sprintf (Error, "Input buffer overflow");break;
			case 'W' :  sprintf (Error, "Print head overheated");break;
			case 'X' :  sprintf (Error, "External I/O error");break;
			case 'Y' :  sprintf (Error, "Print head error");break;
			case 'Z' :  sprintf (Error, "Printhead damaged");break;
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
		
		sprintf(log,"CabPrinter.c => Status => BufferRead : %s \r \n ",BufferRead);
		LogPrinterCOMError(log);		
		
		
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
	if(pexception)
	{
		sprintf(log,"CabPrinter.c => Status => PRINTER_READ_STATUS_FAILED \r \n ");
		LogPrinterCOMError(log);
	}
	
	EXCRETHROW( pexception);
}


#pragma warning( pop)
