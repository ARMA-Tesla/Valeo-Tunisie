#include <time.h> 
#include <test/eltest.h>
#include <formatio.h>
#include <analysis.h>
#include <sdk/mmsystem.h>
#include <test/test_helper_functions.h>
#include <toolbox.h>
#include "cmfb_piano_utils.h" 
#include <export.h>

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2

#define RATE  1000

extern SObjListPtr 		gs_Modules;

#define TIMEOUT_AXIS_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_AXIS_PEND_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( -1, "Axis position end error"); } else

#define TIMEOUT_AXIS_POSITION_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( -1, "Axis position error"); } else

#define TIMEOUT_LIN_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_LIN_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( -1, "Error communication Lin"); } else

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckDirectory"
SElExceptionPtr CheckDirectory(const char* directory)
{
	int32_t			   	error = 0;
	SElExceptionPtr    	pexception = NULL; 
	int					size;

	EXCCHECKCVI( FileExists (directory, &size));
	if(size==-1)
	{
		/* create directory */	
		EXCCHECKCVI( MakeDir(directory));
	}
	
Error:
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__	"LogAxisError"
void LogAxisError( char *Error)   
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
	sprintf(result_dir, "%s\\axis\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	
	/* enreg des data */
	
	memset(log, 0x00, 256);
	
	sprintf (fileName, "%s/Log_Buffer.txt", result_dir);
	
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	
//	sprintf (log, "18: 30 -- hello && it is me");
//	sprintf (log, sdate_time, DATE_TIME_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf (log,"---------------------------------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\nError:%s\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,Error);  
//	sprintf (log, "IS_BARCODE_REAR=%d\r\n", is_barcode_rear); 
//	sprintf (log, "SENSOR_CYLINDER_X_REAR_STATION_80=", is_cylinder_x_rear); 
	
	
	WriteLine (HandleFile, log, sizeof (log));	
	
	CloseFile (HandleFile);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RealTimeWaiting50"
void RealTimeWaiting50(double time_ms)
{
	clock_t _start_clock;
	double   time;
	
	_start_clock = clock();
	do{
		time = (double)(clock()-_start_clock);
	}while (time <= time_ms);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RealTimeWaiting60"
void RealTimeWaiting60(double time_ms)
{
	clock_t _start_clock;
	double   time;
	
	_start_clock = clock();
	do{
		time = (double)(clock()-_start_clock);
	}while (time <= time_ms);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ElSleep"
void ElSleep(DWORD time)
{
	timeBeginPeriod(1);
	Sleep( (DWORD)time);
	timeEndPeriod(1);
}

/*----------------------------------------------------------------------------*/

#undef __FUNC__
#define __FUNC__ "MoveAxis"
SElExceptionPtr MoveAxis( SaxisPtr paxis, int axis, double position)
{
	SElExceptionPtr pexception = NULL;
	
	paxis->AlarmReset(paxis, axis);
    paxis->EnableAxis(paxis, axis);
	paxis->ControllerStatus(paxis, axis,1);  //SERVO ON
	
	if (axis == 2)
	{
	paxis->EnableAxis(paxis, axis); 
    paxis->SetPosition(paxis, axis, position, 100.0); 			// paxis->SetPosition(paxis, axis, position, 100.0);
	paxis->SetPosition(paxis, axis, position, 100.0);
	paxis->EnableAxis(paxis, axis);
	}
	else 
	{
	paxis->EnableAxis(paxis, axis); 
    paxis->SetPosition(paxis, axis, position, 200.0); 			//    paxis->SetPosition(paxis, axis, position, 200.0); 
	paxis->SetPosition(paxis, axis, position, 200.0);
	paxis->EnableAxis(paxis, axis);
	}	
	
Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/

#undef __FUNC__
#define __FUNC__ "CheckAxis"
SElExceptionPtr CheckAxis( SaxisPtr paxis, int axis, double position, double timeout)
{
	SElExceptionPtr pexception = NULL;
	double Currentposition;
	int   status;
	clock_t _start_test_clock;
	double  time=0;
	int     statusAlarm;
   	char	Error[256] = "";

	_start_test_clock =  clock();
		do{
			paxis->CurrentPosition(paxis, axis, &Currentposition); 
			if (Currentposition < position + 0.2 && Currentposition > position - 0.2)
				break;
   		    time = (double)(clock()-_start_test_clock);    
  	  } while(time < timeout);
	  
	if (time >= timeout)
	{
		paxis->PresentAlarm(paxis, axis, &statusAlarm);
		paxis->CurrentPosition(paxis, axis, &Currentposition);
		if (axis == AXIS_X)
		{
			sprintf(Error, "AXE X : Alarm Code:(0X%04X)h\r\n Current Position = %.2fmm\r\n Target = %.2fmm\r\n", statusAlarm, Currentposition, position);
	   		LogAxisError(Error); 
		}
		else if (axis == AXIS_Y)
		{
			sprintf(Error, "AXE Y : Alarm Code:(0X%04X)h\r\n Current Position = %.2fmm\r\n Target = %.2fmm\r\n", statusAlarm, Currentposition, position);
			LogAxisError(Error); 
		}
		else if (axis == AXIS_Z)
		{	
			sprintf(Error, "AXE Z : Alarm Code:(0X%04X)h\r\n Current Position = %.2fmm\r\n Target = %.2fmm\r\n", statusAlarm, Currentposition, position);
			LogAxisError(Error); 
		}
		
		EXCTHROW(-1, Error);
	}
	
Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "MoveToPush"
SElExceptionPtr MoveToPush(SaxissPtr paxiss,
						   int index,
						   int check, 
						   double position_axis_Z, 
						   double position_axis_Y, 
						   double position_axis_X)
{
	SElExceptionPtr pexception = NULL;
	SaxisPtr		paxis = NULL; 

	if (check == 1)
	{
	    EXCCHECK( paxiss->GetaxisByIndex(paxiss, index+2, &paxis));
		EXCCHECK(CheckAxis (paxis, AXIS_Z, position_axis_Z, 5000));
		
		EXCCHECK( paxiss->GetaxisByIndex(paxiss, index+1, &paxis));
		EXCCHECK(CheckAxis(paxis, AXIS_Y, position_axis_Y, 5000));
		
		EXCCHECK( paxiss->GetaxisByIndex(paxiss, index, &paxis));
		EXCCHECK(CheckAxis (paxis, AXIS_X, position_axis_X, 5000));
		
	
	}
	else
	{
		EXCCHECK( paxiss->GetaxisByIndex(paxiss, index+1, &paxis));
		EXCCHECK(MoveAxis (paxis, AXIS_Y, position_axis_Y));
		EXCCHECK(CheckAxis(paxis, AXIS_Y, position_axis_Y, 6000));
			
		EXCCHECK( paxiss->GetaxisByIndex(paxiss, index, &paxis));
	    EXCCHECK(MoveAxis (paxis, AXIS_X, position_axis_X));
		EXCCHECK(CheckAxis (paxis, AXIS_X, position_axis_X, 6000));
		
		//Sleep(100);
				
	    EXCCHECK( paxiss->GetaxisByIndex(paxiss, index+2, &paxis));		
		EXCCHECK(MoveAxis (paxis, AXIS_Z, position_axis_Z));	
	
	}

Error:
	EXCRETHROW( pexception);
	EXCDISPLAY( pexception);
}


/*----------------------------------------------------------------------------*/

#undef __FUNC__
#define __FUNC__ "MoveAxis2"
SElExceptionPtr MoveAxis2( SaxisPtr paxis, int axis, double position)
{
	SElExceptionPtr pexception = NULL;
	
	EXCCHECK( paxis->SetPosition(paxis, axis, position, 500.0)); 
	EXCCHECK( paxis->SetPosition(paxis, axis, position, 500.0)); 
	EXCCHECK( paxis->EnableAxis(paxis, axis));
	
Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/

#undef __FUNC__
#define __FUNC__ "CheckAxis2"
SElExceptionPtr CheckAxis2( SaxisPtr paxis, int axis, double position, double timeout)
{
	SElExceptionPtr pexception = NULL;
	double Currentposition;
	int   status;

	
    TIMEOUT_AXIS_INIT();
	
	do{
		paxis->CurrentPosition(paxis, axis, &Currentposition); 
	if (Currentposition < position + 0.1 && Currentposition > position - 0.1)
			break;
        TIMEOUT_AXIS_POSITION_CHECK(timeout);
    }
    while(TRUE);
	
	
Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalog"
SElExceptionPtr ReadAnalog(char *Channel, double *Value)
{
SElExceptionPtr pexception = NULL;
int channel_index;

		sscanf(Channel, "ai%d", &channel_index);
		*Value = data_Analog[channel_index]; 
		
Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalog2"
SElExceptionPtr ReadAnalog2(char *Channel, double *Value)
{
SElExceptionPtr pexception = NULL;
int channel_index;

		sscanf(Channel, "ai%d", &channel_index);
		*Value = data_Analog_2[channel_index]; 
		
Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalogWithScale"
SElExceptionPtr ReadAnalogWithScale(char *Used_Channel, char *Channel, SCalibrationPtr pScaleInfo, double *Value)
{
	SElExceptionPtr pexception = NULL;
	int i, channel_index;
	char fileName[512];

	
		sscanf(Channel, "ai%d", &channel_index);
	
		if (strcmp(Used_Channel, Channel))
		{
			
			static double pChannelData[RATE];
			static double pdata[RATE];
	
			for(i=0;i<RATE;i++)
				pdata[i] = 	data_Analog [channel_index];
			
			if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
			{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			
			/*changement formule ARAIBIA 08/03/2021
			  double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			  double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			  LinEv1D( pdata, RATE, a, b,  pChannelData);*/
			
			  //changement formule ARAIBIA 08/03/2021
			  double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			  double b =  plincal->pPoints[0].realValue - a * plincal->pPoints[0].measValue;
			  LinEv1D( pdata, RATE, a, b,  pChannelData);
			
			
			 /*double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			  double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k); 
			  LinEv1D( pdata, RATE, k, q,  pChannelData); */
			
			Mean (pChannelData, RATE, Value);
			}
			else 
				*Value = data_Analog[channel_index]; 
					
		}
		else //Lecture plus des valeurs push appyé
		{
			
			static double pChannelData[RATE*100];
			static double pdata[RATE*100];
	
			for(i=0;i<RATE*100;i++)
				pdata[i] = 	data_Analog [channel_index];
			
			if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
			{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			
			/*changement formule ARAIBIA 08/03/2021
			  double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			  double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			  LinEv1D( pdata, RATE*100, a, b,  pChannelData); */
			
			  //changement formule ARAIBIA 08/03/2021
			  double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			  double b =  plincal->pPoints[0].realValue - a * plincal->pPoints[0].measValue;
			  LinEv1D( pdata, RATE, a, b,  pChannelData);
			
			/*double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k); 
			LinEv1D( pdata, RATE, k, q,  pChannelData); */
			
			Mean (pChannelData, RATE*100, Value);
			}
			else 
				*Value = data_Analog[channel_index]; 
			
			
		}
	

Error:
	EXCRETHROW( pexception);
	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalogWithScale2"
SElExceptionPtr ReadAnalogWithScale2(char *Used_Channel, char *Channel, SCalibrationPtr pScaleInfo, double *Value)
{
	SElExceptionPtr pexception = NULL;
	int i, channel_index;
	char fileName[512];

	
		sscanf(Channel, "ai%d", &channel_index);
	
		if (strcmp(Used_Channel, Channel))
		{
			
			static double pChannelData[RATE];
			static double pdata[RATE];
	
			for(i=0;i<RATE;i++)
				pdata[i] = 	data_Analog_2 [channel_index];
			
			if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
			{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			
			double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			LinEv1D( pdata, RATE, a, b,  pChannelData);
			
			
			/*double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k); 
			LinEv1D( pdata, RATE, k, q,  pChannelData); */
			
			Mean (pChannelData, RATE, Value);
			}
			else 
				*Value = data_Analog_2[channel_index]; 
					
		}
		else //Lecture plus des valeurs push appyé
		{
			
			static double pChannelData[RATE*100];
			static double pdata[RATE*100];
	
			for(i=0;i<RATE*100;i++)
				pdata[i] = 	data_Analog_2 [channel_index];
			
			if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
			{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			
			double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			LinEv1D( pdata, RATE*100, a, b,  pChannelData);
			
			
			/*double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k); 
			LinEv1D( pdata, RATE, k, q,  pChannelData); */
			
			Mean (pChannelData, RATE*100, Value);
			}
			else 
				*Value = data_Analog_2[channel_index]; 
			
			
		}
	

Error:
	EXCRETHROW( pexception);
	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LineaTable1D"
SElExceptionPtr LineaTable1D(double *InputTable, SCalibrationPtr pScaleInfo, int size, double *OutputTable)
{
SElExceptionPtr pexception = NULL;


		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			
			double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			LinEv1D( InputTable, size, a, b,  OutputTable);
			
			/*double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k); 
			LinEv1D( InputTable, size, k, q,  OutputTable); */ 
		}

Error:
	EXCRETHROW( pexception);
}