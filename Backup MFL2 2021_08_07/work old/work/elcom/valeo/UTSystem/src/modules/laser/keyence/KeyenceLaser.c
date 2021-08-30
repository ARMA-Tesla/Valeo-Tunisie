#include <cviauto.h>
#include "toolbox.h"
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

#undef __FUNC__
#define __FUNC__	"CyclePrintStatus"
void CyclePrintStatus(char *ErrorCode)   
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
						sdate_time[256],
						buffer[2048] = "";
	SYSTEMTIME			st;

	
	CAObjHandle 		pTicket;
	CAObjHandle 		pTicket_new = 0;
	char*           	ptext = NULL;
    int                 HandleFile;
	char				log[256];
	
	
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\laser\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	
	/* enreg des data */
	
	memset(log, 0x00, 256);
	
	sprintf (fileName, "%s/Log_Buffer.txt", result_dir);
	
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	
//	sprintf (log, "18: 30 -- hello && it is me");
//	sprintf (log, sdate_time, DATE_TIME_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf (log,"---------------------------------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\nError Code=%s\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, ErrorCode);  
//	sprintf (log, "IS_BARCODE_REAR=%d\r\n", is_barcode_rear); 
//	sprintf (log, "SENSOR_CYLINDER_X_REAR_STATION_80=", is_cylinder_x_rear); 
	
	
	WriteLine (HandleFile, log, sizeof (log));	
	
	CloseFile (HandleFile);
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
#define __FUNC__	"{KeyenceLaser}::ErrorStatus"
SElExceptionPtr ErrorStatus(SLaserPtr me, int *ErrorStatus, char *ErrorCode)
{
	SElExceptionPtr	pexception = NULL;
	char command[1024];
	char buffer[1024];
	int  received = 0;
	
	TIMEOUT_LASER_INIT();

	*ErrorStatus = -1;	
	do{
	/* Check status marking */
		 FlushInQ (me->port);
		 FlushOutQ (me->port);
		 
		sprintf(command, "RX,Error\r");//RX,Error[CR]	
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
				*ErrorStatus = 0;
					
			}
			else if (FindPattern (buffer, 0, -1, "RX,OK,1", 0, 0) >= 0) 
			{
				*ErrorStatus = 1;
				
				CopyString (ErrorCode, 0, buffer, 8, 1024);
				
			//	ErrorCode= strcat ("Error:",ErrorCode);
				
			//	printf ("Buffer=%s\r\n",buffer);
			//	printf ("ErrorCode=%s\r\n",ErrorCode);
				CyclePrintStatus(ErrorCode);
				
			}
			else 
		       *ErrorStatus = -1;	
			
			break;
		}
		
		TIMEOUT_LASER_CHECK(LASER_TIMEOUT);
			
	}while(TRUE);			

	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{KeyenceLaser}::ErrorClear"
SElExceptionPtr ErrorClear(SLaserPtr me)
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
		 
		sprintf(command, "WX,ErrorClear\r");//WX,ErrorClear[CR]	
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
#define __FUNC__	"{KeyenceLaser}::ReadString"
SElExceptionPtr ReadString(SLaserPtr me, int program, int block, const char* File_Line_String, int* string_OK, char *laser_string)
{
	SElExceptionPtr	pexception = NULL;
	char command[1024];
	char buffer[1024]="";
	int  received = 0;
	
	TIMEOUT_LASER_INIT();

	/* Check status marking */
	do{
		 FlushInQ (me->port);
		 FlushOutQ (me->port);
		 
		sprintf(command, "RX,PRG=%04d,BLK=%03d,CharacterString\r",program, block);//RX,Ready[CR]	sprintf(command, "WX,PRG=%04d,BLK=%03d,CharacterString=%s\r", program, block, string);
		ComWrt (me->port, command, strlen (command));	 
		Sleep(50);
		ReturnRS232Err (); 
		
		Sleep(50);
		
		received  = GetInQLen (me->port);
		if (received > 0)
		{
			ComRd (me->port, buffer, received);
			if (FindPattern (buffer, 0, -1, File_Line_String, 0, 0) >= 0) 
			{
				*string_OK = 1;
					
			}
			else 
				*string_OK = 0;	
	//		printf("buffer=%s\r\n",buffer);
		sprintf(laser_string,"%s",buffer);
	//	printf("laser_string=%s\r\n",laser_string);	
		break;	
		}
	
//		
		TIMEOUT_LASER_CHECK(LASER_TIMEOUT);
			
	}while(TRUE);
	
Error:
	EXCRETHROW( pexception);
}


#pragma warning( pop)
