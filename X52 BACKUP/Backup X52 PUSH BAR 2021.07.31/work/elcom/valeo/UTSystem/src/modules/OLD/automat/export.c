#include <local.h>
#include <utility.h>
#include <NIDAQmx.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <cvidef.h>
#include "automat_fnc.h"

#define AXIS_Z	 2

double DLLEXPORT data_Analog[32+1];
double DLLEXPORT data_Analog_2[32+1]; 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RealTimeWaiting"
void RealTimeWaiting(double time_ms)
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
#define __FUNC__ "ThreadDAQ"
int CVICALLBACK ThreadDAQ (void *functionData)
{
 	TaskHandle HandleDAQ = (TaskHandle)functionData;
	int read;
	float64 data[32+1];
	
 	//CmtSetCurrentThreadPriority (THREAD_PRIORITY_HIGHEST);
    
    while (Run_DAQ == 1)
    {
		DAQmxReadAnalogF64 (HandleDAQ, DAQmx_Val_Auto,0.005  , DAQmx_Val_GroupByChannel, data, 32, &read, 0);	//0.005
		if (read>0)
		{
		memcpy( data_Analog, data, 32 * sizeof(float64));
		memcpy( _data_Analog, data, 32 * sizeof(float64));
		}
	}
    return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadDAQ_2"
int CVICALLBACK ThreadDAQ_2 (void *functionData)
{
 	TaskHandle HandleDAQ_2 = (TaskHandle)functionData;
	int read;
	float64 data[32+1];
	
 	//CmtSetCurrentThreadPriority (THREAD_PRIORITY_HIGHEST);
    
    while (Run_DAQ_2 == 1)
    {
		DAQmxReadAnalogF64 (HandleDAQ_2, DAQmx_Val_Auto,0.005 , DAQmx_Val_GroupByChannel, data, 32, &read, 0);  //0.005
		if (read>0)
		{
		memcpy( data_Analog_2, data, 32 * sizeof(float64));
		memcpy( _data_Analog_2, data, 32 * sizeof(float64));
		}
	}
    return 0;
}
  
	/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Axis"
int CVICALLBACK Thread_Axis (void *functionData)
{
	StructAxisPtr pthread_data_Axis = (StructAxisPtr)functionData;
	
	while (Run_Axis == 1)
    {

		Sleep(10);
		
	//	if (data_Analog[1] > 7.0)
	//	{
			//printf ("STOP : paxis_right_station_30 \r\n");
	//		pthread_data_Axis->paxis_station_40->StopAxis(pthread_data_Axis->paxis_station_40, AXIS_Z);

	//	}
		
	//	if (data_Analog[2] > 7.0)
	//	{
			//printf ("STOP : paxis_right_station_30 \r\n");
	//		pthread_data_Axis->paxis_station_50->StopAxis(pthread_data_Axis->paxis_station_50, AXIS_Z);
	//	}
	
	}

    return 0;
}
