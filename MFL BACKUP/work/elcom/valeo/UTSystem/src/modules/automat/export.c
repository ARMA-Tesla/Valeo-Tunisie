#include <axis/class/axis.h>
#include <utility.h>
#include <NIDAQmx.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <cvidef.h>
#include <local.h>

#define AXIS_Z  	 2

#define RATE 		 20
#define NUM_CHANNALS 7

double DLLEXPORT data_Analog[RATE*NUM_CHANNALS+1];

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
	float64 data[RATE*NUM_CHANNALS+1];
	
    while (Run_DAQ == 1)
    {
	DAQmxReadAnalogF64 (HandleDAQ, RATE, 0.001, DAQmx_Val_GroupByChannel, data, RATE*NUM_CHANNALS, &read, 0);
	
	if (read>0)
		memcpy( data_Analog, data, RATE * NUM_CHANNALS * sizeof(float64));
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

		Sleep(1);
		if (data_Analog[1*RATE]*2 > 10.0)
		{
			//printf ("STOP : paxis_right_station_30 \r\n");
			pthread_data_Axis->paxis_right_station_30->StopAxis(pthread_data_Axis->paxis_right_station_30, AXIS_Z);
		}
		
		if (data_Analog[2*RATE]*2 > 10.0)
		{
			//printf ("STOP : paxis_left_station_30 \r\n");
			pthread_data_Axis->paxis_left_station_30->StopAxis(pthread_data_Axis->paxis_left_station_30, AXIS_Z);
		}
				
		if (data_Analog[3*RATE]*2 > 10.0)
		{
			//printf ("STOP : paxis_right_station_40 \r\n");
			pthread_data_Axis->paxis_right_station_40->StopAxis(pthread_data_Axis->paxis_right_station_40, AXIS_Z);
		}
		
		if (data_Analog[4*RATE]*2 > 10.0)
		{
			//printf ("STOP : paxis_left_station_40 \r\n");
			pthread_data_Axis->paxis_left_station_40->StopAxis(pthread_data_Axis->paxis_left_station_40, AXIS_Z);
		}
	}

    return 0;
}
