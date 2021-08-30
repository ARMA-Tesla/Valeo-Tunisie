#include <axis/class/axis.h>

extern int CVICALLBACK ThreadDAQ (void *functionData);
extern int CVICALLBACK ThreadDAQ_2 (void *functionData);
int threadDAQId;
int threadDAQId_2; 
int Run_DAQ;
int Run_DAQ_2;

extern int CVICALLBACK Thread_Axis (void *functionData);

typedef struct _StructAxis{

	SaxisPtr 	paxis_station_40;
	SaxisPtr 	paxis_station_50;

} StructAxis, *StructAxisPtr;

int thread_Id_Axis;
int Run_Axis;
		
