#include <cvintwrk.h>
#include <tcpsupp.h>
#include <rs232.h>
#include <ansi_c.h>
#include <userint.h>
#include "Cognex.h"
#include <stdio.h>
#include <utility.h>
#include <formatio.h>

#pragma warning( push)
#pragma warning( disable: 4100 4127)

#define CAMERA_TIMEOUT				200

#define CAMERA_READ_MODE_NORMAL	0
#define CAMERA_READ_MODE_STATUS	1

/* Temps de réponse COGNEX */
#define COGNEX_ANSWER	0.1
#define COGNEX_TIMEOUT	10.0 

/* Cognex commands */
#define COGNEX_EXEC "SE8"
#define COGNEX_GET "GV"
#define COGNEX_SET_INTEGER "SI"
#define COGNEX_SET_FLOAT "SF"
#define COGNEX_SAVE "TF"
#define COGNEX_OFFLINE "SO0"
#define COGNEX_ONLINE "SO1"
#define COGNEX_SET_JOB "SJ"
#define COGNEX_READ_JOB "GJ"

#define COGNEX_LOGIN "admin" 
#define COGNEX_PASS  "" 

static unsigned int g_hconversation; 
char cameraData[256] = {0};
int cameraDataReady = 0;
int srv_registered = 0; 
static int gTelnetHandle = -1; 



/* Read data from telnet ***************************************/ 

static void ProcessResponse(char *string)
{
    size_t  i, length;

    length = strlen (string);
    for (i = 0; i < length; ++i)
        if (!isprint(string[i]) && !isspace(string[i]))
            string[i] = ' ';
}


static int ReadResponse (int read)
{
    int     error = 0;
    ssize_t bytesRead, totalBytesRead = 0;
    char    response[128];

    // Read response from telnet server 
        bytesRead = 0;
        error = InetTelnetRead (gTelnetHandle, response, read - 1,&bytesRead, 1500);
		//printf ("response TELNET = %s \r\n", response);
        ProcessResponse(response);
		if (response[0] == '1' ) 
		{
		 return 0;
		}
		else
		{
		return -1;	
		}
}

/* Ecriture d'une valeur Entier dans le tableau  ********************/
int Cognex_Set_Value_Integer ( char *Colonne , int Ligne, int Value) 
{
	int Status = -1;
	char Commande[100]; 
	char Chaine[100]; 
	
	/* concatener la trame */
	sprintf (Commande, "%s%s%03d%d", COGNEX_SET_INTEGER, Colonne, Ligne , Value);
	
	/* Envoi de commande au Cognex */
	InetTelnetWrite (gTelnetHandle, Commande, "\r\n", -1, 0, 2000);
	
return Status;
}

/* Lecture d'une valeur dans le tableau  ********************/
int Cognex_Get_Value_Integer ( char *Colonne , int Ligne) 
{
	int Status = -1;
	char Commande[100]; 
	char Chaine[100]; 
	
	/* concatener la trame */
	sprintf (Commande, "%s%s%03d", COGNEX_GET, Colonne, Ligne);
	
	/* Envoi de commande au Cognex */
	InetTelnetWrite (gTelnetHandle, Commande, "\r\n", -1, 0, 2000);

return Status;
}

int ReadResponseTelnet (int Length, char *String_To_Read)
{
    int     error = 0;
    ssize_t bytesRead, totalBytesRead = 0;
    char    response[128];

    // Read response from telnet server 
        bytesRead = 0;
        error = InetTelnetRead (gTelnetHandle, response, Length, &bytesRead, 20000);
        ProcessResponse(response);
		if (FindPattern (response, 0, bytesRead, String_To_Read, 0, 0) >= 0)
		{
			return 0;
		}
		else 
		{
			return -1;
		}
}

int CVICALLBACK ServerTCPCB (unsigned handle, int event, int error,
                             void *callbackData)
{
	int cameraportNum =  (int)callbackData;
    char receiveBuf[256] = {0};
    ssize_t dataSize        = sizeof (receiveBuf) - 1;
    char addrBuf[31];
	int i = 0;

	//printf ("event TCP = %d \r\n", event);
    switch (event)
        {

        case TCP_CONNECT:
			//printf ("TCP_CONNECT handle = %d \r\n", handle);
			//printf ("TCP_CONNECT g_hconversation = %d \r\n", g_hconversation);
            if (g_hconversation)
                {

                }
            else
                {
                /* Handle this new client connection */
                g_hconversation = handle;
                }
            break;
        case TCP_DATAREADY:
            if ((dataSize = ServerTCPRead (g_hconversation, receiveBuf,
                                           dataSize, 1000))
                < 0)
                {
				//printf ("read TCP NOK = %s \r\n", receiveBuf);
				memset(  receiveBuf, '\0', 256);
				memset(  cameraData, '\0', 256);
                }
            else
                {
				//printf ("read TCP OK = %s \r\n", receiveBuf);
                receiveBuf[dataSize] = '\0';
                	for (i = 0; i < dataSize; ++i) 
					{	
				 	cameraData[i] =  receiveBuf[i];
					} 
				 cameraDataReady = 1;
				 UnregisterTCPServer (cameraportNum); 
				 g_hconversation = 0; 
				 srv_registered = 0;

                }
            break;
        case TCP_DISCONNECT:
			//printf ("TCP_DISCONNECT handle = %d \r\n", handle);
			//printf ("TCP_DISCONNECT g_hconversation = %d \r\n", g_hconversation);
            if (handle == g_hconversation)
                {
                g_hconversation = 0;
				memset(  receiveBuf, '\0', 256);
				memset(  cameraData, '\0', 256);
                }
            break;
    }
    
Done:    
    return 0;
}

#undef __FUNC__
#define __FUNC__	"{cognex}::openTCPserver" 
SElExceptionPtr openTCPserver(SCameraPtr me)
{
	SElExceptionPtr	pexception = NULL;
	int Status;
		

		// open TCP server to read response
		Status = RegisterTCPServer (me->port, ServerTCPCB, (void*)me->port);
		//printf ("Status  RegisterTCPServer = %d \r\n", Status);
		if (Status == -3)
		{
			g_hconversation = 0;
			Status = UnregisterTCPServer ((me->port));
			//printf ("Status  UnregisterTCPServer = %d \r\n", Status);
			Sleep (1000);
		}
		
		// open TCP server to read response
		Status = RegisterTCPServer (me->port, ServerTCPCB, (void*)me->port);
		//printf ("Status  RegisterTCPServer = %d \r\n", Status);
		srv_registered = 1;
		
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_login"  
SElExceptionPtr Cognex_login(SCameraPtr me)
{
	SElExceptionPtr	pexception = NULL;
	int Status;	
	int i=0;
	
	 gTelnetHandle = InetTelnetOpen (me->adressIP, 23, 0); 
	 Sleep(50);
	 if(gTelnetHandle<0)
	 { gTelnetHandle = InetTelnetOpen (me->adressIP, 23, 0);  }
	 //printf("gTelnetHandle=%d\r\n",gTelnetHandle);
     if (gTelnetHandle >= 0)
     	{
		// login
		// 	printf("Ready for 
		InetTelnetWrite (gTelnetHandle, COGNEX_LOGIN, "\r\n", -1, 0, 2000);	
		ReadResponse (sizeof("Password:"));
		// pass
		InetTelnetWrite (gTelnetHandle, COGNEX_PASS, "\r\n", -1, 0, 2000);
		ReadResponse (sizeof("User Logged In"));
		Status = 0;
		}
	 else
	 	{
		EXCTHROW( CAMERA_ERROR_TIMEOUT, "InetTelnetOpen Failed");
	 	}
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_disconnect"  
SElExceptionPtr Cognex_disconnect(SCameraPtr me)
{
	SElExceptionPtr	pexception = NULL;
	
	// disconnect
	if(gTelnetHandle >= 0)
		InetTelnetClose(gTelnetHandle);	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_Trigger"  
SElExceptionPtr Cognex_Trigger(SCameraPtr me)
{
	SElExceptionPtr	pexception = NULL;
	
	InetTelnetWrite (gTelnetHandle, COGNEX_EXEC, "\r\n", -1, 0, 2000);
	ReadResponse (sizeof("1"));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_Read"  
SElExceptionPtr Cognex_Read(SCameraPtr me, char *data)
{
	SElExceptionPtr	pexception = NULL;
    char    response[128];
    ssize_t bytesRead, totalBytesRead = 0;
	clock_t _start_test_clock;
	double   time;
	
	_start_test_clock =  clock();

	do{
	time = (double)(clock()-_start_test_clock);	
	}while (!cameraDataReady && time < CAMERA_TIMEOUT);
	
	if (cameraDataReady == 1)
	{
		cameraDataReady = 0;
		sprintf(data, cameraData);
	}
	

Error:
	EXCRETHROW( pexception);
}
//#undef __FUNC__
//#define __FUNC__	"{cognex}::Cognex_Read"  
//SElExceptionPtr Cognex_Read(SCameraPtr me, char *data)
//{
//	SElExceptionPtr	pexception = NULL;
//    char    response[128];
//    ssize_t bytesRead, totalBytesRead = 0;
//	clock_t _start_test_clock;
//	double   time;
//		
//	memset(  response, '\0', 128);	

//	/* Get cell GVC014 */
//	Cognex_Get_Value_Integer ( "C" , 14);   // read C014
//	if (ReadResponseTelnet (4, "1") < 0)
//		EXCTHROW( CAMERA_ERROR_TIMEOUT, "Cognex_Get_Value_Integer");
//		
//	/* Read cell GVC014 */
//	InetTelnetRead (gTelnetHandle, response, 9, &bytesRead, 2000);
//	printf("InetTelnetRead response = %s \n",response);

//    ProcessResponse(response);

//	if (FindPattern (response, 0, bytesRead, ",", 0, 0) < 0)
//		{
//		memset(  cameraData, '\0', 256);	
//		}
//    else
//        {
//		CopyBytes (data, 0, response, 0, 18);
//		}
//	
//Error:
//	EXCRETHROW( pexception);
//}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_Online"  
SElExceptionPtr Cognex_Online(SCameraPtr me)
{
	SElExceptionPtr	pexception = NULL;
	
	InetTelnetWrite (gTelnetHandle, COGNEX_ONLINE, "\r\n", -1, 0, 2000); 
	ReadResponse (sizeof("1"));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_Offline"  
SElExceptionPtr Cognex_Offline(SCameraPtr me)
{
	SElExceptionPtr	pexception = NULL;

	InetTelnetWrite (gTelnetHandle, COGNEX_OFFLINE, "\r\n", -1, 0, 2000);
	ReadResponse (sizeof("1"));
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_act_job"  
SElExceptionPtr Cognex_act_job(SCameraPtr me, int *actual_job)
{
	SElExceptionPtr	pexception = NULL;
	int Status = -1;
	int     error = 0;
    ssize_t bytesRead, totalBytesRead = 0;
	size_t  i, length; 
    char    response[128] = ""; 
	char 	job_ID[128] = "";
	
	/* Envoi de commande au Cognex */
	InetTelnetWrite (gTelnetHandle, COGNEX_READ_JOB, "\r\n", -1, 0, 2000);	

    // Read response from telnet server 
	// response OK: "1/r200"
    bytesRead = 0;
    error = InetTelnetRead (gTelnetHandle, response, sizeof (response) - 1, &bytesRead, 200);
	if (response[0] == '1' ) // ok 
		{
 		length = strlen (response);
    	for (i = 0; i < length; ++i)
		job_ID[i] = response[i+2];
		// char to int conversion
		 *actual_job = atoi(job_ID);
		}
		else
		{
		EXCTHROW( CAMERA_ERROR_TIMEOUT, "InetTelnetWrite Failed");
		}
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_Job"  
SElExceptionPtr Cognex_Job(SCameraPtr me, const char *JobID)
{
	SElExceptionPtr	pexception = NULL;
	int Status = 0;
	char Commande[100];
	
	// job 
	sprintf (Commande, "%s%s", COGNEX_SET_JOB, JobID); 
	InetTelnetWrite (gTelnetHandle, Commande, "\r\n", -1, 0, 2000); 
	ReadResponse (sizeof("1"));	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_set_param"  
SElExceptionPtr Cognex_set_param(SCameraPtr me, int phase)
{
	SElExceptionPtr	pexception = NULL;
	
	Cognex_Set_Value_Integer ( "B" , 1, phase);   // write B002 + phase
//	//printf ("Executing Cognex_set_param\r\n");
//	printf("phase =%d\r\n",phase);
	ReadResponse (sizeof("1"));	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_set_number_boutton"  
SElExceptionPtr Cognex_set_number_boutton(SCameraPtr me, int phase)
{
	SElExceptionPtr	pexception = NULL;
	
	Cognex_Set_Value_Integer ( "D" , 1, phase);   // write B002 + phase
	ReadResponse (sizeof("1"));	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_set_parameters"  
SElExceptionPtr Cognex_set_parameters(SCameraPtr me, char* Colonne, int phase)
{
	SElExceptionPtr	pexception = NULL;
	
	Cognex_Set_Value_Integer ( Colonne , 17, phase);   // write B002 + phase
	ReadResponse (sizeof("1"));	
	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{cognex}::Cognex_set_parameters_VS11"  
SElExceptionPtr Cognex_set_parameters_VS11(SCameraPtr me, char* Colonne, int phase)
{
	SElExceptionPtr	pexception = NULL;
	
	Cognex_Set_Value_Integer ( Colonne , 16, phase);   // write B002 + phase
	ReadResponse (sizeof("1"));	
	
	
Error:
	EXCRETHROW( pexception);
}


#pragma warning( pop)
