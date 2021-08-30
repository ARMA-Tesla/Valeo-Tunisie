#include <userint.h>
#include <stdlib.h>
#include <iviswtch.h>
#include <IviDmm.h>
#include <IviDcPwr.h>
#include <resistor/class/Resistor.h>
#include <eldaq/class/eldaq.h>
#include "panel_x95_common.h"
#include "panel_x95_caman.h" 


extern ViSession gs_IviSessions[X95_IVI_SESSION_COUNT];

int CVICALLBACK OnL38Rele (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = 0;
	int state = 0;
	const char*	pchannel = NULL;
	const char*	pcom = NULL;
	int is_day_night = 0;
	ViSession vi = gs_IviSessions[SW4];
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetCtrlVal( panel, control, &state );
			
			switch ( control )
			{
				case L38_K10:
					pchannel = "ch50";
					pcom     = "com50";
					is_day_night = 1;
					break;
				case L38_K11:
					pchannel = "ch70";
					pcom     = "com70";
					is_day_night = 1;
					break;
				case L38_K13:
					pchannel = "ch60";
					pcom     = "com60";
					is_day_night = 1;
					break;
				case L38_K15:
				case L38_K16:
				case L38_K18:
					pchannel = "ch40";
					pcom     = "com40";
					break;
				case L38_K17:
					pchannel = "ch80";
					pcom     = "com80";
					break;
				case L38_K12:
					pchannel = "ch28";
					pcom     = "com28";
					break;
				case L38_K14:
					pchannel = "CH2A";
					pcom     = "CH2B";
					vi = gs_IviSessions[PSW];
					break;
				default:
					goto Error;
					break;
			}
			if ( is_day_night && !state)
			{
		 		error = IviSwtch_Disconnect ( vi, "ch50","com50");
				error = ( error == IVISWTCH_ERROR_DISCONTINUOUS_PATH ) ? 0 : error;
				error = IviSwtch_Disconnect ( vi, "ch60","com60");
				error = ( error == IVISWTCH_ERROR_DISCONTINUOUS_PATH ) ? 0 : error;
				error = IviSwtch_Disconnect ( vi, "ch70","com70");
				error = ( error == IVISWTCH_ERROR_DISCONTINUOUS_PATH ) ? 0 : error;
				SetCtrlVal( panel, L38_K10, 0 );
				SetCtrlVal( panel, L38_K11, 0 );
				SetCtrlVal( panel, L38_K13, 0 );
			}
							
			if ( state )
			{
				checkErr( IviSwtch_Disconnect (vi, pchannel, pcom));

			}
			else
			{
				checkErr( IviSwtch_Connect (vi,  pchannel, pcom));
			}

			
			if ( control == L38_K15 ||
				 control == L38_K16 ||
				 control == L38_K18 )
			{
				SetCtrlVal( panel, L38_K15, !state );
				SetCtrlVal( panel, L38_K16, !state );
				SetCtrlVal( panel, L38_K18, !state );
			}
			else
			{
				SetCtrlVal( panel, control, !state );
			}
			break;
	}
Error:
	return 0;
}

int CVICALLBACK OnL38Dmm (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK OnL38Timer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = 0;
	double value;
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			checkErr( eldaq_ReadMeasurement( gs_IviSessions[DAQ], "ai0", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal( panel, L38_RECY1, value );
			checkErr( eldaq_ReadMeasurement( gs_IviSessions[DAQ], "ai1", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal( panel, L38_RECY2, value );
			checkErr( eldaq_ReadMeasurement( gs_IviSessions[DAQ], "ai4", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal( panel, L38_RDP, value );
			checkErr( eldaq_ReadMeasurement( gs_IviSessions[DAQ], "ai5", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal( panel, L38_NSF, value );
			checkErr( eldaq_ReadMeasurement( gs_IviSessions[DAQ], "ai7", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal( panel, L38_VCL, value );
			checkErr( eldaq_ReadMeasurement( gs_IviSessions[DAQ], "ai6", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal( panel, L38_AC, value );
			break;
	}
Error:
	if ( error < 0 )
	{
		
	}
	return 0;
}

int CVICALLBACK OnK7Resistance (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = 0;
	int state = 0;
	double resistance;
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetCtrlVal( panel, control, &state);
			resistance = (state) ? 7400 : (7400 + 1500);
			checkErr( Resistor_ConfigureOutputResistance( gs_IviSessions[RESISTOR], "4", resistance ));
			SetCtrlVal( panel, control, !state);
			break;
	}
Error:
	return 0;
}

int CVICALLBACK OnPower (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ViStatus	error = 0;
	int state = 0;
	double voltage, current;
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			if ( control == L38_K3 )
			{
				GetCtrlVal( panel, L38_K3, &state);
				if ( state )
					checkErr( IviSwtch_Disconnect (gs_IviSessions[PSW], "CH1A", "CH1B"));
				else
					checkErr( IviSwtch_Connect (gs_IviSessions[PSW],  "CH1A", "CH1B"));
				SetCtrlVal( panel, L38_K3, !state);
			}
			break;
		case EVENT_COMMIT:
			if ( control == L38_POWER )
			{
				GetCtrlVal( panel, L38_POWER, &voltage);
				checkErr( IviDCPwr_ConfigureVoltageLevel( gs_IviSessions[POWER], "1", voltage));
			}
			break;
	}
Error:
	return 0;
}
