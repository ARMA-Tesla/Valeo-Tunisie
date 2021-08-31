#include <axis/class/axis.h>


extern int CVICALLBACK ThreadDAQ (void *functionData);
int threadDAQId;
int Run_DAQ;

extern int CVICALLBACK Thread_Axis (void *functionData);

typedef struct _StructAxis{

	SaxisPtr 	paxis_right_station_30;
	SaxisPtr 	paxis_left_station_30;
	SaxisPtr 	paxis_right_station_40;
	SaxisPtr 	paxis_left_station_40;

} StructAxis, *StructAxisPtr;

int thread_Id_Axis;
int Run_Axis;
												 
