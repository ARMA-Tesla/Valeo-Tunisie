#include <userint.h>
#include "panel_b2yy_eatc.h"
#include <vline/vline.h>
#include <eventfactory/EventFactory.h> 
#include <utility.h>
#include <guid.h>
#include <multithread/multithread.h>
#include <jig/class/jig_class.h>
#include <IviBridge/ivibridge.h>
#include <test/testseq.h>
#include "panel_b2yy_eatc_plugin.h" 

#include <resistor.h>
#include <IviSwtch.h>
#include <IviDCPwr.h>
#include <eldaq.h>
#include "lang_translation.h"
#include "panel_b2yy_common.h"

#define NUMBER_OF_PANELS 	4
#define MODULE_JIG			MODULE_JIG_B2YY_EATC /* guid.h */
#define COMMAND_LENGHT		1024
#define SUBPANEL_LEFT 		0
#define SUBPANEL_TOP		467

static int					gs_panel[NUMBER_OF_PANELS+1];
static SVLinePtr			gs_pvline = NULL;
static SJigPtr				gs_pjig = NULL;
SLanguagePtr 				gs_pLanguage;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL;
static HANDLE				gs_hEventStop = NULL;
static HANDLE				gs_hThread = NULL;
static SPluginArgsPtr		gs_PluginArgs = NULL;
static STestSequencePtr		gs_TestSequence = NULL;

ViSession	gs_IviSessons[B2YY_IVI_SESSION_COUNT];

static SElExceptionPtr PanelVersion(void);
static SElExceptionPtr ResetPanelCtrl(void);
static SElExceptionPtr DisplayCache(int panel, SVLinePtr pVLine);

static SElExceptionPtr DisplayDumpData(
   int         panel,
   int         control,
   uint8_t      data[],
   uint32_t   dataLength,
   uint32_t   address
);

static DWORD WINAPI Thread_ButtonRead(LPVOID param);

static enum _EMode
{
	EM_IO = 0,
	EM_MEMORY,
	EM_VLINE,
	EM_EB2YY
} EMode;

typedef enum _EPanelSwtch
{
	SWTCH_CONNECT = 0,
	SWTCH_DISCONNECT,
} EPanelSwtch;


#define B1_REL( cid, chi) \
	{ EB2YY_B1_##cid, "com"#chi, "ch"#chi }

#define B2_REL( cid, chi) \
	{ EB2YY_B2_##cid, "com"#chi, "ch"#chi }

typedef struct _SReleyDefinition {
	int				ctrlId;
	const char*		ch1;
	const char*		ch2;
} SReleyDefinition, *SReleyDefinitionPtr;

static SReleyDefinition gs_b1_releys[] = {
	
	B1_REL( CH0, 0), 
	B1_REL( CH10, 10),
	B1_REL( CH20, 20),
	B1_REL( CH30, 30),
	B1_REL( CH40, 40),
	B1_REL( CH28, 28),
	B1_REL( CH50, 50),
	B1_REL( CH60, 60),
	B1_REL( CH70, 70),
	B1_REL( CH80, 80),
	B1_REL( CH90, 90),
	B1_REL( CH78, 78),
//	B1_REL( CH1,  01),
//	B1_REL( CH11, 11),
//	B1_REL( CH21, 21),
//	B1_REL( CH29, 29),
	B1_REL( CH31, 31),
	B1_REL( CH41, 41),
	B1_REL( CH51, 51),
	B1_REL( CH53, 53),
	B1_REL( CH63, 63),
	B1_REL( CH73, 73),
	{0, NULL, NULL}
};


static SReleyDefinition gs_b2_releys[] = {
	
	B2_REL( CH0, 0), 
	B2_REL( CH1, 1),
	B2_REL( CH2, 2),
	B2_REL( CH3, 3),
	{0, NULL, NULL}
};

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PanelSwtch"
static SElExceptionPtr PanelSwtch(EPanelSwtch mode)
{
	SElExceptionPtr	pexception = NULL;

	if(gs_PluginArgs && gs_TestSequence==NULL)
		EXCCHECK( gs_PluginArgs->GetArg(gs_PluginArgs, "TestSequence", ARG_PTR, &gs_TestSequence));

	if(gs_TestSequence)
	{
		switch(mode)
		{
			case SWTCH_CONNECT:
				EXCCHECK( gs_TestSequence->RunSelected(gs_TestSequence, NULL, "00.10"));
				break;
			case SWTCH_DISCONNECT:
				EXCCHECK( gs_TestSequence->RunSelected(gs_TestSequence, NULL, "00.11"));
				break;
		}
	}		

Error:
	EXCRETHROW( pexception);  
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "UpdateReleyStates"
static SElExceptionPtr UpdateReleyStates(int panel)
{
	SElExceptionPtr	pexception = NULL;
	int	i;
	ViStatus	error = VI_SUCCESS;
	ViInt32	state;

	if ( gs_Modules )
	{
		SIviBridgePtr  pIvi = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
		EXCCHECK( pIvi->GetSession( pIvi, "SW4", &gs_IviSessons[SW4]));
		EXCCHECK( pIvi->GetSession( pIvi, "SW5", &gs_IviSessons[SW5]));
		EXCCHECK( pIvi->GetSession( pIvi, "DAQ", &gs_IviSessons[DAQ]));
		EXCCHECK( pIvi->GetSession( pIvi, "DMM", &gs_IviSessons[DMM]));
		EXCCHECK( pIvi->GetSession( pIvi, "PSW", &gs_IviSessons[PSW]));
		EXCCHECK( pIvi->GetSession( pIvi, "PSW1", &gs_IviSessons[PSW1]));
		EXCCHECK( pIvi->GetSession( pIvi, "POWER", &gs_IviSessons[POWER]));
		EXCCHECK( pIvi->GetSession( pIvi, "RESISTOR", &gs_IviSessons[RESISTOR]));
		Resistor_ConfigureOutputResistance( gs_IviSessons[RESISTOR], "1", 5110 );
		Resistor_ConfigureOutputResistance( gs_IviSessons[RESISTOR], "2", 5110 );
		Resistor_ConfigureOutputResistance( gs_IviSessons[RESISTOR], "3", 8200 );
		Resistor_ConfigureOutputResistance( gs_IviSessons[RESISTOR], "4", 8200 );
		Resistor_ConfigureOutputResistance( gs_IviSessons[RESISTOR], "5", 15000 );
		EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_ConfigureOutputRange(gs_IviSessons[POWER], "1", IVIDCPWR_VAL_RANGE_VOLTAGE, 20.0));
		EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_ConfigureVoltageLevel(gs_IviSessons[POWER], "1", 13.5));
		EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_ConfigureCurrentLimit(gs_IviSessons[POWER], "1", IVIDCPWR_VAL_CURRENT_REGULATE, 1.0));
		EXCCHECKIVI( IviDCPwr, gs_IviSessons[POWER], IviDCPwr_ConfigureOutputEnabled(gs_IviSessons[POWER], "1", VI_TRUE));
		EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW], IviSwtch_CanConnect(gs_IviSessons[PSW], "CH1A", "CH1B", &state));
		if (state == IVISWTCH_VAL_PATH_AVAILABLE)
			EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW], IviSwtch_Connect(gs_IviSessons[PSW], "CH1A", "CH1B"));
	}
	
	for (i = 0; gs_b1_releys[i].ch1; i++)
	{
		EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], gs_b1_releys[i].ch1, gs_b1_releys[i].ch2, &state));
		SetCtrlVal( panel, gs_b1_releys[i].ctrlId, (state == IVISWTCH_VAL_PATH_EXISTS) ? 1 : 0);
	}
	
	for (i = 0; gs_b2_releys[i].ch1; i++)
	{
		EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW1], IviSwtch_CanConnect(gs_IviSessons[PSW1], gs_b2_releys[i].ch1, gs_b2_releys[i].ch2, &state));
		SetCtrlVal( panel, gs_b2_releys[i].ctrlId, (state == IVISWTCH_VAL_PATH_EXISTS) ? 1 : 0);
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnB1Reley"
int CVICALLBACK OnB1Reley (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int			value, i;
	ViStatus	error = VI_SUCCESS;
	
	switch (event)
	{
	case EVENT_LEFT_CLICK:
			GetCtrlVal( panel, control, &value);
			for (i = 0; gs_b1_releys[i].ch1; i++)
			{
				if (gs_b1_releys[i].ctrlId == control ) break;	
			}
			if ( gs_b1_releys[i].ch1 )
			{
				if ( value )
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[SW4], gs_b1_releys[i].ch1, gs_b1_releys[i].ch2));
				else
					EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[SW4], gs_b1_releys[i].ch1, gs_b1_releys[i].ch2));
				SetCtrlVal( panel, control, !value);
			}
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTimer"
int CVICALLBACK OnTimer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	double value;
	ViStatus	error = VI_SUCCESS;
	switch (event)
	{
		case EVENT_TIMER_TICK:
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai0", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI0, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai1", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI1, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai2", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI2, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai3", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI3, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai4", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI4, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai5", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI5, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai6", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI6, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai7", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI7, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai8", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI8, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai9", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI9, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai10", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI10, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai11", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI11, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai12", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI12, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai13", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI13, value);
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ReadMeasurement( gs_IviSessons[DAQ], "ai14", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &value));
			SetCtrlVal(panel, EB2YY_AI14, value);
			break;
	}
Error:
		EXCDELETE( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnB2Reley"
int CVICALLBACK OnB2Reley (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int	value, i;
	ViStatus	error = VI_SUCCESS;
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetCtrlVal( panel, control, &value);
			for (i = 0; gs_b2_releys[i].ch1; i++)
			{
				if (gs_b2_releys[i].ctrlId == control ) break;	
			}
			if ( gs_b2_releys[i].ch1 )
			{
				if ( value )
					EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW1], IviSwtch_Disconnect(gs_IviSessons[PSW1], gs_b2_releys[i].ch1, gs_b2_releys[i].ch2));
				else
					EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW1], IviSwtch_Connect(gs_IviSessons[PSW1], gs_b2_releys[i].ch1, gs_b2_releys[i].ch2));
				SetCtrlVal( panel, control, !value);
			}
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnConnect"
int CVICALLBACK OnConnect (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int32_t			value;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, control, &value));
			if(value)
			{
				EXCCHECK( PanelSwtch(SWTCH_CONNECT));
				EXCCHECK( ResetPanelCtrl());

				/* read panel info */
				Sleep(800);
				EXCCHECK( PanelVersion());
			}
			else
			{
				/* stop second thread */
				EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
				EXCCHECKCVI( SetCtrlVal(gs_panel[B2YY], B2YY_BUTTON_READ, FALSE));
				EXCCHECK( PanelSwtch(SWTCH_DISCONNECT));
			}	
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ResetPanelCtrl"
static SElExceptionPtr ResetPanelCtrl(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	SetCtrlVal( gs_panel[B2YY], B2YY_ATS_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_AC_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_AUTO_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_ATF_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_ATP_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_ATS_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_RECIR_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_MAX_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_HFS_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_HRW_LED, 0);

	EXCCHECKCVI( SetCtrlVal(gs_panel[B2YY], B2YY_BUTTON_READ, FALSE));

	SetCtrlVal( gs_panel[B2YY], B2YY_PWM_BLOWER, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_PWM_SYMBOL, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_PWM_LCD, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_PWM_LED, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_DISPLAY, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_STEPPER_DOOR, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_STEPPER_AIR, 0);
	SetCtrlVal( gs_panel[B2YY], B2YY_STEPPER_RECIRC, 0);

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PanelVersion"
static SElExceptionPtr PanelVersion(void)
{
	SElExceptionPtr     pexception = NULL;
	int32_t				error = 0;
	char		read[COMMAND_LENGHT] = "";
	int			count, lenght;
	char		ver[256] = "";
	
	EXCCHECK( gs_pvline->Query( gs_pvline, "\\FAC\\VER", read, COMMAND_LENGHT ) );  
	
	lenght = strlen(read);
	
	if(lenght)
	{
		sscanf(read, "S0%02X0000", &count);
		
		if(count>=8)
		{
			sprintf(ver, "SW:");
			strncat(ver, read+8, 2);
			strcat(ver, ".");
			strncat(ver, read+10, 2);
		
			strcat(ver, "; HW:");
			strncat(ver, read+12, 2);
			strcat(ver, ".");
			strncat(ver, read+14, 2);
		
			strcat(ver, "; EEP:");
			strncat(ver, read+16, 2);
			strcat(ver, ".");
			strncat(ver, read+18, 2);
		}
	
		EXCCHECKCVI( SetCtrlVal(gs_panel[B2YY], B2YY_VER, ver));
	}
	
Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PanelReset"
static SElExceptionPtr PanelReset(void)
{
	SElExceptionPtr     pexception = NULL; 
	char		read[COMMAND_LENGHT];
	int			count;
	
	EXCCHECK( gs_pvline->Write( gs_pvline, "\\FAC\\RSW") );  
	
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPwm"
int CVICALLBACK OnPwm (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int				duty, freq, id, status;
	char			command[COMMAND_LENGHT];
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch(control)
			{
				case B2YY_PWM_BLOWER:
				case B2YY_BLOWER_DUTY:
				case B2YY_BLOWER_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_PWM_BLOWER, &status));
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_BLOWER_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_BLOWER_FREQ, &freq));
					id = 0;
					break;
				case B2YY_PWM_SYMBOL:
				case B2YY_SYMBOL_DUTY:
				case B2YY_SYMBOL_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_PWM_SYMBOL, &status));
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_SYMBOL_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_SYMBOL_FREQ, &freq));
					id = 1;
					break;
				case B2YY_PWM_LCD:
				case B2YY_LCD_DUTY:
				case B2YY_LCD_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_PWM_LCD, &status));
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_LCD_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_LCD_FREQ, &freq));
					id = 2;
					break;
				case B2YY_PWM_LED:
				case B2YY_LED_DUTY:
				case B2YY_LED_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_PWM_LED, &status));
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_LED_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, B2YY_LED_FREQ, &freq));
					id = 3;
					break;
			}
			sprintf(command, "\\HWI\\FIO\\ohzdc8w %d %d %d", id, freq, (status)? duty:0); 
			EXCCHECK( gs_pvline->Write(gs_pvline, command));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;   
}

/*---------------------------------------------------------------------------*/
typedef struct _SLed
{
	int32_t	control_id;
	int32_t	led_id;
	bool_t	state_on;
} SLed;

#define NUMBER_OF_LED	9
SLed gs_Led[NUMBER_OF_LED] =
{
	{B2YY_AC_LED, 0, TRUE},
	{B2YY_AUTO_LED, 1, TRUE},
	{B2YY_ATF_LED, 2, FALSE},
	{B2YY_ATP_LED, 3, FALSE},
	{B2YY_ATS_LED, 4, TRUE},
	{B2YY_RECIR_LED, 5, FALSE},
	{B2YY_MAX_LED, 6, TRUE},
	{B2YY_HFS_LED, 7, TRUE},
	{B2YY_HRW_LED, 8, FALSE}
};

#undef __FUNC__
#define __FUNC__ "OnLed"
int CVICALLBACK OnLed (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int				id, status, i;
	char			command[COMMAND_LENGHT]; 
	
	switch (event)
	{
		case EVENT_COMMIT:

			EXCCHECKCVI( GetCtrlVal(panel, control, &status));
			
			for(i=0; i<NUMBER_OF_LED; i++)
			{
				if(gs_Led[i].control_id==control) break;
			}

			if(i<NUMBER_OF_LED)
			{
				sprintf(command, "\\HWI\\DIO\\o1w %d %d", gs_Led[i].led_id, (status)? gs_Led[i].state_on:!gs_Led[i].state_on);   	
				EXCCHECK( gs_pvline->Write(gs_pvline, command)); 
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSenRead"
int CVICALLBACK OnSenRead (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	char			command[COMMAND_LENGHT], read[COMMAND_LENGHT];  
	int				id;
	unsigned int 	value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			for(id=0; id<6; id++)
			{
				sprintf(command, "\\HWI\\AIO\\i16ar %d", id);
				EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT ) );
				sscanf(read, "%x", &value);
			
				if(id==0)
					EXCCHECKCVI( SetCtrlVal(panel, B2YY_SEN_ITEMP, value));
				if(id==1)
					EXCCHECKCVI( SetCtrlVal(panel, B2YY_SEN_LSL, value));
				if(id==2)
					EXCCHECKCVI( SetCtrlVal(panel, B2YY_SEN_RSL, value));
				if(id==3)
					EXCCHECKCVI( SetCtrlVal(panel, B2YY_SEN_DF, value));
				if(id==4)
					EXCCHECKCVI( SetCtrlVal(panel, B2YY_SEN_DP, value));
				if(id==5)
					EXCCHECKCVI( SetCtrlVal(panel, B2YY_SEN_VBATT, value));
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnButtonRead"
int CVICALLBACK OnButtonRead (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	int				status;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, control, &status));
			
			if(status==TRUE)
			{
				EXCCHECK( ThreadStart(Thread_ButtonRead, NULL, &gs_hThread, &gs_hEventStop)); 
			}
			else
			{
				EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnStepper"
int CVICALLBACK OnStepper (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t 		error = 0;
	int 			id, state;
	char			command[COMMAND_LENGHT];
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			EXCCHECKCVI( GetCtrlVal(panel, control, &state));
			
			switch(control)
			{
				case B2YY_STEPPER_DOOR:
					id = 0;
					break;
				case B2YY_STEPPER_AIR:
					id = 1;
					break;
				case B2YY_STEPPER_RECIRC:
					id = 2;
					break;
			}
			
			if(state==1) state = 15;

			sprintf(command, "\\HWI\\DIO\\o4w %d %d", id, state);  
			EXCCHECK( gs_pvline->Write(gs_pvline, command) ); 
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

#define BLOCK_SIZE 		32
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMemoryRead"
int CVICALLBACK OnMemoryRead (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	char			command[COMMAND_LENGHT], read[COMMAND_LENGHT], *pread = NULL; 
	uint32_t   		address;
  	uint32_t   		start_address = 0x0000; 
	uint32_t  		data_lenght = 2048;
	uint32_t   		stop_address; 
	uint8_t			*data = NULL, *pdata = NULL;
	int				i, value, block_size, status;
	
	switch (event)
	{
		case EVENT_COMMIT:
			/* stop second thread */
			EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
			EXCCHECKCVI( SetCtrlVal(gs_panel[B2YY], B2YY_BUTTON_READ, FALSE));
			
			stop_address = start_address + data_lenght;
			data = calloc(data_lenght, sizeof(uint8_t));
			pdata = data;
			
			for(address=start_address; address < stop_address; address +=BLOCK_SIZE)
      		{
				block_size = (BLOCK_SIZE<(stop_address-address))? BLOCK_SIZE:(stop_address-address);
				sprintf(command, "\\HWI\\EEP\\r %04d %d", address, block_size);  
		
				EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT)); 
				
				pread = strchr(read, ':');
				pread++;
				pread = strtok( pread, " ");
				for(i=0; i<block_size; i++)
				{
					if(pread==NULL)
						EXCTHROW( -1, "ERROR_NULL_POINTER");
					
					sscanf(pread, "%02x", &value);
					*(pdata++) = (uint8_t)value;
					pread = strtok( NULL, " ");
				}
				Sleep(10);
			}
			
			EXCCHECK( DisplayDumpData( panel, MEMORY_MEMORY, data, data_lenght, start_address));	
			break;
	}
	
Error:
	if(data) free(data);
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnDisplay"
int CVICALLBACK OnDisplay (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error;
	char			command[COMMAND_LENGHT];
	int				state, segment1, segment2, segment3, segment4;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, B2YY_DISPLAY, &state));
			EXCCHECKCVI( GetCtrlVal(panel, B2YY_SEGMENT_1, &segment1));
			EXCCHECKCVI( GetCtrlVal(panel, B2YY_SEGMENT_2, &segment2));
			EXCCHECKCVI( GetCtrlVal(panel, B2YY_SEGMENT_3, &segment3));
			EXCCHECKCVI( GetCtrlVal(panel, B2YY_SEGMENT_4, &segment4));
			
			if(state==TRUE)
				sprintf(command, "\\HWI\\LCD\\w %d %d %d %d", segment1, segment2, segment3, segment4);
			else
				sprintf(command, "\\HWI\\LCD\\w %d %d %d %d", 0, 0, 0, 0);  
			
			EXCCHECK( gs_pvline->Write(gs_pvline, command));  
			
			sprintf(command, "\\HWI\\LCD\\c %d", state);	
			EXCCHECK( gs_pvline->Write(gs_pvline, command)); 
			
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

static int	gs_Offset = 0;
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnVline"
int CVICALLBACK OnVline(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				start;

	switch (event)
	{
		case EVENT_COMMIT:
			break;
		case EVENT_KEYPRESS:
			EXCCHECKCVI( GetCtrlAttribute (panel, VLINE_TEXTBOX, ATTR_TEXT_SELECTION_START, &start));
			switch (eventData1)
			{
				case VAL_HOME_VKEY:
				case VAL_SHIFT_MODIFIER | VAL_HOME_VKEY:
				case VAL_MENUKEY_MODIFIER | VAL_HOME_VKEY:
					EXCCHECKCVI( SetCtrlAttribute (panel, VLINE_TEXTBOX, ATTR_TEXT_SELECTION_START, gs_Offset));
					return 1;
					break;
				case VAL_UP_ARROW_VKEY:		/* arrow UP */
				case VAL_DOWN_ARROW_VKEY:	/* arrow down */
				case VAL_TAB_VKEY:
					return 1;
					break;
				case VAL_LEFT_ARROW_VKEY:      /* arrow left */
				case VAL_BACKSPACE_VKEY:	   /* Backspace */
					if(start<=gs_Offset)
						return 1;
					break;
				case VAL_FWD_DELETE_VKEY:		/* Delete */
					if(start<gs_Offset)
						return 1;
					break;
				default:
				case VAL_ENTER_VKEY:
					{
						char buffer[512];
						char* pbegin = NULL;
						int line, loffset = 0, offset;
						
						if(eventData1 == VAL_ENTER_VKEY || start < gs_Offset)
						{
							EXCCHECKCVI( GetTextBoxLineIndexFromOffset (panel, VLINE_TEXTBOX, gs_Offset, &line));
							GetTextBoxLine (panel, VLINE_TEXTBOX, line, buffer);
							GetTextBoxLineOffset (panel, VLINE_TEXTBOX, line, &loffset);
							
							offset =  gs_Offset-loffset;
							pbegin = (loffset>=0)? &buffer[offset]:NULL;
						
							if (eventData1==VAL_ENTER_VKEY)
							{
								if(pbegin==NULL || strlen(pbegin)==0)
								{
									EXCCHECK( gs_pvline->Reset(gs_pvline)); 

									/* delete last enter */
									if((gs_pvline->_pCache)->_used_size>0 
										&& (gs_pvline->_pCache)->_p_data[strlen((gs_pvline->_pCache)->_p_data)-1]=='\n')
									{
										(gs_pvline->_pCache)->_p_data[strlen((gs_pvline->_pCache)->_p_data)-1] = '\0';
										(gs_pvline->_pCache)->_used_size--;
									}
								}
								else
								{
									char data[COMMAND_LENGHT];
									uint32_t data_length;

									pexception = gs_pvline->Write(gs_pvline, pbegin); 
									EXCDELETE(pexception);

									/* delete last enter */
									if((gs_pvline->_pCache)->_used_size>0
										&& (gs_pvline->_pCache)->_p_data[strlen((gs_pvline->_pCache)->_p_data)-1]=='\n')
									{
										(gs_pvline->_pCache)->_p_data[strlen((gs_pvline->_pCache)->_p_data)-1] = '\0';
										(gs_pvline->_pCache)->_used_size--;
									}

									/* read a rest of data */
									do
									{
										pexception = gs_pvline->_pSerComm->Read(gs_pvline->_pSerComm, COMMAND_LENGHT, data, &data_length, 200);
										if(pexception==NULL)
										{
											data[data_length] = '\0';	
											(gs_pvline->_pCache)->Append(gs_pvline->_pCache, data);
										}
									} while(pexception==NULL);
									EXCDELETE(pexception);
								}
								EXCCHECK( DisplayCache(panel, gs_pvline));
							}
							else
							{
								EXCCHECKCVI( SetCtrlAttribute(panel, 
															  VLINE_TEXTBOX, 
															  ATTR_TEXT_SELECTION_START, 
															  gs_Offset + strlen(pbegin)));
							}
						}
					}
					break;
			}
			break;
		}

Error:
	EXCDISPLAY(pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DisplayCache"
static SElExceptionPtr DisplayCache(int panel, SVLinePtr pVLine)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	const char*		pcache = NULL;
	const char*		pdata  = NULL;
	const char*		pdata2 = NULL;
	char			data[COMMAND_LENGHT];
	int32_t			data_length;

	pcache = pVLine->GetCachePtr(pVLine);
	pdata2  = pcache;
	do 
	{
		pdata2  = strstr(pdata2, "R: ");
		if(pdata2)
		{
			pdata = pdata2;
			pdata2++;
		}
	} while( pdata2 != NULL );

	if (pdata)
	{
		int lines, llchar, offset;

		pdata += strlen("R: ");
		
		EXCCHECKCVI( InsertTextBoxLine(panel, VLINE_TEXTBOX, -1, pdata));
		EXCCHECKCVI( GetNumTextBoxLines(panel, VLINE_TEXTBOX, &lines));
		EXCCHECKCVI( DeleteTextBoxLine(panel, VLINE_TEXTBOX, --lines));
		EXCCHECKCVI( GetTextBoxLineLength(panel, VLINE_TEXTBOX, lines-1, &llchar));
		EXCCHECKCVI( GetTextBoxLineOffset(panel, VLINE_TEXTBOX, lines-1, &offset));

		EXCCHECKCVI( SetCtrlAttribute (panel, VLINE_TEXTBOX, ATTR_TEXT_SELECTION_START, offset+llchar));
		gs_Offset = offset+llchar;

		EXCCHECKCVI( GetNumTextBoxLines(panel, VLINE_TEXTBOX, &lines));
		if((lines-17)>=0)
			EXCCHECKCVI( SetCtrlAttribute (panel, VLINE_TEXTBOX,  ATTR_FIRST_VISIBLE_LINE, lines-17));
	}
	
	EXCCHECK( pVLine->ClearCache( pVLine));
	
Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_ButtonRead"
static DWORD WINAPI Thread_ButtonRead(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	DWORD			state;
	char			command[COMMAND_LENGHT], read[COMMAND_LENGHT];  
	int				value, id;

	while(TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop, 100) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}

		for(id=0; id<14; id++)
		{
			sprintf(command, "\\HWI\\DIO\\i1r %d", id);
			EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT ));
			
			sscanf(read, "%d", &value);
			
			if(id==0) /* OFF */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_OFF_PICT, ATTR_VISIBLE, !value));
			if(id==1) /* AC */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_AC_PICT, ATTR_VISIBLE, !value)); 
			if(id==2) /* AUTO */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_AUTO_PICT, ATTR_VISIBLE, !value)); 
			if(id==3) /* AIR TO FLOOR */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_ATF_PICT, ATTR_VISIBLE, !value)); 
			if(id==4) /* AIR TO PANEL */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_ATP_PICT, ATTR_VISIBLE, !value));
			if(id==5) /* AIR TO SCREEN */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_ATS_PICT, ATTR_VISIBLE, !value));    
			if(id==6) /* RECIRCULATION */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_RECIR_PICT, ATTR_VISIBLE, !value)); 
			if(id==7) /* MAX */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_MAX_PICT, ATTR_VISIBLE, !value)); 
			if(id==8) /* HFS */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_HFS_PICT, ATTR_VISIBLE, !value));    
			if(id==9) /* HRW */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_HRW_PICT, ATTR_VISIBLE, value));  
			if(id==10) /* TEMP 1 */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_TEMP1_PICT, ATTR_VISIBLE, !value));
			if(id==11) /* TEMP 2 */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_TEMP2_PICT, ATTR_VISIBLE, !value));  
			if(id==12) /* BLOWER 1 */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_BLOW1_PICT, ATTR_VISIBLE, !value));    
			if(id==13) /* BLOWER 2 */
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[B2YY], B2YY_BLOW2_PICT, ATTR_VISIBLE, !value));   
		}
	}
	
Error:	
	EXCDISPLAY( pexception);
	return 0;
}

#define DUMP_LINE_LENGTH   80 
/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "DisplayDumpData"
static SElExceptionPtr DisplayDumpData(
   int         panel,
   int         control,
   uint8_t      data[],
   uint32_t   dataLength,
   uint32_t   address
)
{
	SElExceptionPtr	pexception = NULL;    
	int32_t      	error = 0;
	uint32_t   		start_address = address;
	uint32_t   		stop_address = address + dataLength; 
	uint32_t   		row, column, row_count, index;
	char      		text_data[17] = "";
	char*      		pdump = NULL;
	char*      		pend = NULL;

	if ( start_address % 16 )
	{
		/* start address must be less as data address */
		start_address -= ( start_address % 16 );
	}

	row_count = ( address - start_address) + dataLength;
	row_count = ( row_count % 16 ) ? row_count / 16 + 1 : row_count / 16;

	pdump = malloc(row_count * ( DUMP_LINE_LENGTH ));
	EXCCHECKALLOC( pdump);
	pend = pdump;

	row = 0;
	column = 0;
	
	for( index = start_address; index < stop_address; index++, column++)
	{
		if (( index % 16 ) == 0 )
		{
			if ( row != 0 )
			{
				pend += sprintf(pend, " %s\n", text_data);
			}
			row++;
			pend += sprintf(pend, "%08x: ", index);
			memset( text_data, 0, sizeof(text_data));
			column = 0;
		}

		if ( index < address )
		{
			pend += sprintf(pend, "?? ");
			text_data[column] = ' ';
		}
		else
		{
			pend += sprintf(pend, "%02x ", (int)data[index-address]);
			text_data[column] = ( isprint(data[index-address])) ? data[index-address] : '.';
		}
	}
	
	if ( text_data[0] )
	{
		pend += sprintf(pend, " %s", text_data);
	}
	
	EXCCHECKCVI( ResetTextBox( panel, control, pdump));

Error:
	free( pdump);
	EXCRETHROW(pexception); 
}

/*---------------------------------------------------------------------------*/
void CVICALLBACK PostDeferredDisplayPanel (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		DisplayPanel(*panel);
}

/*---------------------------------------------------------------------------*/
void CVICALLBACK PostDeferredHidePanel (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		HidePanel(*panel);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ChangeMode"
static SElExceptionPtr ChangeMode (int32_t mode)
{
	SElExceptionPtr 	pexception = NULL;  
	int32_t				error = 0;
	static enum _EMode	last_mode;
	
	switch(last_mode)
	{
		case EM_IO:
			break;
		case EM_MEMORY:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[MEMORY]));   
			break;
		case EM_VLINE:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[VLINE]));   
			break;
		case EM_EB2YY:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[EB2YY]));
			EXCCHECKCVI(SetCtrlAttribute(gs_panel[EB2YY], EB2YY_TIMER, ATTR_ENABLED, 0));
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_StopMeasurement( gs_IviSessons[DAQ]));
			break;
	}
	
	switch(mode)
	{
		case EM_IO:
			break;
		case EM_MEMORY:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[MEMORY]));
			break;
		case EM_VLINE:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[VLINE]));
			break;
		case EM_EB2YY:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[EB2YY]));
			EXCCHECK( UpdateReleyStates( gs_panel[EB2YY]));
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_ConfigureMeasurement (gs_IviSessons[DAQ], "", 2000, 0.02));
			EXCCHECKIVI( eldaq, gs_IviSessons[DAQ], eldaq_StartMeasurement( gs_IviSessons[DAQ]));
			EXCCHECKCVI(SetCtrlAttribute(gs_panel[EB2YY], EB2YY_TIMER, ATTR_ENABLED, 1));
			break;
	}
	
	last_mode = mode;

Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMode"
int CVICALLBACK OnMode (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL; 
	int32_t				error = 0;
	int					mode;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, control, &mode));
			EXCCHECK( ChangeMode( mode));
			
			if ( panel == gs_panel[EB2YY] )
				EXCCHECKCVI( SetCtrlVal(gs_panel[B2YY], B2YY_MODE, mode));
			else
				EXCCHECKCVI( SetCtrlVal(gs_panel[EB2YY], EB2YY_MODE, mode));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);   
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ShowPanel"
static SElExceptionPtr ShowPanel(void)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	
	EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[B2YY]));
		
Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ClosePanel"
static SElExceptionPtr ClosePanel(void)          
{
	int32_t          error = 0;
   	SElExceptionPtr  pexception = NULL;
	
	/* stop second thread */
	EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
	EXCCHECKCVI( SetCtrlVal(gs_panel[B2YY], B2YY_BUTTON_READ, FALSE));

  	EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[B2YY]));   
	
Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
static SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	SElExceptionPtr  pexception = NULL;
   
	if(0==strcmp(eventName, "EVNT_JIG_LOADED"))
	{
   		if(pData && 0==strcmp(((SElEventDataPtr)pData)->GetStringPtr(pData), MODULE_JIG))
			EXCCHECK( ShowPanel());
	}
	else if(0==strcmp(eventName, "EVNT_JIG_UNLOADED"))
	{
		if(pData && 0==strcmp(((SElEventDataPtr)pData)->GetStringPtr(pData), MODULE_JIG))
			EXCCHECK( ClosePanel());	
	}   
   
Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
	SElExceptionPtr	pexception = NULL;
   
	if ( gs_EventFactory )
	{
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_JIG_LOADED",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_JIG_UNLOADED",
                                         EventFunctionCallback));
	}
   
Error:
	EXCRETHROW(pexception);
}   /* RegisterEventFunction */

/*---------------------------------------------------------------------------*/
#if defined(_CVI_DLL_) || \
	defined(_WINDLL)
void DiscardUIObjectsForDLL (void)
{
   /* Discard the panels loaded in InitUIForDLL */
}

int __stdcall DllMain(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved
)
{
   switch (fdwReason)
      {
      case DLL_PROCESS_ATTACH:
#if (!defined(_CVI_)&&defined(_DEBUG))
		//_CrtSetBreakAlloc(57);
#endif
         if (InitCVIRTE (hinstDLL, 0, 0) == 0)      
            return 0;   /* out of memory */
         break;
      case DLL_PROCESS_DETACH:
         if (!CVIRTEHasBeenDetached())      /* Do not call CVI functions if cvirte.dll has already been detached. */
         {
            DiscardUIObjectsForDLL ();      /* Discard the panels loaded in InitUIForDLL */
            CloseCVIRTE ();      
         }
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
			_CrtDumpMemoryLeaks();
#endif
         break;
      }

   return 1;
}

int __stdcall DllEntryPoint(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved
)
{
   /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}

#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr  	pexception = NULL;
	char        		pathname[MAX_PATHNAME_LEN];      
	int32_t         	error = 0;
    int32_t         	main_panel = 0;

	gs_PluginArgs = a_pPluginArgs;

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
                            "ControlPanel",
                            sizeof(main_panel),
                            &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "Modules",
	                        sizeof(SObjListPtr),
	                        &gs_Modules));

   	/* gettext */
	if(gs_Modules)
	{
		gs_pLanguage = (SLanguagePtr)gs_Modules->Get(gs_Modules, MODULE_LANGUAGE);  
		if(gs_pLanguage)
		{
			char pathname[MAX_PATHNAME_LEN];
			EXCCHECKCVI( GetProjectDir( pathname));
			strcat(pathname, "\\lang");
			LANGUAGE_PTR->bindtextdomain (LANGUAGE_PTR, PACKAGE, pathname);
			LANGUAGE_PTR->textdomain (LANGUAGE_PTR, PACKAGE);
		}
	}
   
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "panel_b2yy_eatc.uir", B2YY, B2YYlang, &gs_panel[B2YY])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[B2YY], "panel_b2yy_eatc.uir", VLINE, VLINElang, &gs_panel[VLINE])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[B2YY], "panel_b2yy_eatc.uir", MEMORY, MEMORYlang, &gs_panel[MEMORY])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[B2YY], "panel_b2yy_eatc.uir", EB2YY, EB2YYlang, &gs_panel[EB2YY])); 
   
	EXCCHECKCVI( SetPanelAttribute (gs_panel[B2YY], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[B2YY], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[VLINE], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[VLINE], ATTR_TOP, SUBPANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MEMORY], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MEMORY], ATTR_TOP, SUBPANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[EB2YY], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[EB2YY], ATTR_LEFT, 0));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "EventFactory",
	                        sizeof(SElEventFactoryPtr),
	                        &gs_EventFactory));

	if(gs_Modules)
	{
		gs_pvline = (SVLinePtr)gs_Modules->Get(gs_Modules, MODULE_VLINE);
		gs_pjig = (SJigPtr)gs_Modules->Get(gs_Modules, MODULE_JIG_CLASS);

		if(gs_pjig)
		{
			char uuid[256];
			EXCCHECK( gs_pjig->jig_GetUUID(gs_pjig, uuid, 255));

			if( 0==strcmp(uuid, MODULE_JIG))
				EXCCHECK( ShowPanel());
		}
	}

	EXCCHECK( RegisterEventFunction());  

Error:   
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	/* stop second thread */
	EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
	EXCCHECKCVI( SetCtrlVal(gs_panel[B2YY], B2YY_BUTTON_READ, FALSE));

Error:   
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
   
	/* stop second thread */
	EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));

	if(gs_panel[B2YY])
		EXCCHECKCVI( DiscardPanel(gs_panel[B2YY]));

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel B2YY Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnDisconnectAll"
int CVICALLBACK OnDisconnectAll (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int				error = 0;
	int				state;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH80, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH80, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH60, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH60, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH28, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH28, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH30, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH30, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B2_CH2, &state));
			if ( state ) OnB2Reley( panel, EB2YY_B2_CH2, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH10, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH10, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH78, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH78, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH0, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH0, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH20, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH20, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B2_CH1, &state));
			if ( state ) OnB2Reley( panel, EB2YY_B2_CH1, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH40, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH40, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH50, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH50, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH70, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH70, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH90, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH90, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B2_CH3, &state));
			if ( state ) OnB2Reley( panel, EB2YY_B2_CH3, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH41, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH41, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH51, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH51, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH31, &state));
			if ( state ) OnB1Reley( panel, EB2YY_B1_CH31, EVENT_LEFT_CLICK, NULL, 0, 0); 
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnConnectAll"
int CVICALLBACK OnConnectAll (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int				error = 0;
	int				state;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH80, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH80, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH60, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH60, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH28, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH28, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH30, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH30, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B2_CH2, &state));
			if ( !state ) OnB2Reley( panel, EB2YY_B2_CH2, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH10, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH10, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH78, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH78, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH0, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH0, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH20, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH20, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B2_CH1, &state));
			if ( !state ) OnB2Reley( panel, EB2YY_B2_CH1, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH40, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH40, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH50, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH50, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH70, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH70, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH90, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH90, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B2_CH3, &state));
			if ( !state ) OnB2Reley( panel, EB2YY_B2_CH3, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH41, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH41, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH51, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH51, EVENT_LEFT_CLICK, NULL, 0, 0); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_B1_CH31, &state));
			if ( !state ) OnB1Reley( panel, EB2YY_B1_CH31, EVENT_LEFT_CLICK, NULL, 0, 0); 
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnEnableVline"
int CVICALLBACK OnEnableVline (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	ViStatus				error = 0;
	ViInt32				state;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], "ch73","com73", &state));
			if ( state == IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[SW4], "ch73","com73"));
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], "ch63","com63", &state));
			if ( state == IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[SW4], "ch63","com63"));
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], "ch53","com53", &state));
			if ( state == IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[SW4], "ch53","com53"));
			EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW1], IviSwtch_CanConnect(gs_IviSessons[PSW1], "ch0","com0", &state));
			if ( state == IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[PSW1], "ch0","com0"));

			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], "ch83","com83", &state));
			if ( state != IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[SW4], "ch83","com83"));
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[SW4], "ch53","com53"));
			Delay( 0.2);
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[PSW1], "ch0","com0"));
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnEnableCan"
int CVICALLBACK OnEnableCan (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	ViStatus				error = 0;
	ViInt32				state;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], "ch73","com73", &state));
			if ( state == IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[SW4], "ch73","com73"));
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], "ch63","com63", &state));
			if ( state == IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[SW4], "ch63","com63"));
			
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], "ch53","com53", &state));
			if ( state == IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[SW4], "ch53","com53"));
			EXCCHECKIVI( IviSwtch, gs_IviSessons[PSW1], IviSwtch_CanConnect(gs_IviSessons[PSW1], "ch0","com0", &state));
			if ( state == IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Disconnect(gs_IviSessons[PSW1], "ch0","com0"));

			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_CanConnect(gs_IviSessons[SW4], "ch93","com93", &state));
			if ( state != IVISWTCH_VAL_PATH_EXISTS )
				EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[SW4], "ch93","com93"));
			
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[SW4], "ch73","com73"));
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[SW4], "ch63","com63"));
			Delay( 0.2);
			EXCCHECKIVI( IviSwtch, gs_IviSessons[SW4], IviSwtch_Connect(gs_IviSessons[PSW1], "ch0","com0"));
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnReciStepper"
int CVICALLBACK OnReciStepper (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t 		error = 0;
	int 			a,b,c,d, state;
	char			command[COMMAND_LENGHT];
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECK( gs_pvline->Write(gs_pvline, "\\HWI\\DIO\\o1w 10 1") ); 
			
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_RECI_A, &a)); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_RECI_B, &b)); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_RECI_C, &c));
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_RECI_D, &d)); 
			
			state = 0;
			if (a) state |= (1<<0);
			if (b) state |= (1<<1);
			if (c) state |= (1<<2);
			if (d) state |= (1<<3);
			
			sprintf(command, "\\HWI\\DIO\\o4w 2 %d", state);  
			EXCCHECK( gs_pvline->Write(gs_pvline, command) ); 
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTempStepper"
int CVICALLBACK OnTempStepper (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t 		error = 0;
	int 			a,b,c,d, state;
	char			command[COMMAND_LENGHT];
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_TEMP_A, &a)); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_TEMP_B, &b)); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_TEMP_C, &c));
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_TEMP_D, &d)); 
			
			state = 0;
			if (a) state |= (1<<0);
			if (b) state |= (1<<1);
			if (c) state |= (1<<2);
			if (d) state |= (1<<3);
			sprintf(command, "\\HWI\\DIO\\o4w 0 %d", state);  
			EXCCHECK( gs_pvline->Write(gs_pvline, command) ); 
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnAirStepper"
int CVICALLBACK OnAirStepper (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t 		error = 0;
	int 			a,b,c,d, state;
	char			command[COMMAND_LENGHT];
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_AIR_A, &a)); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_AIR_B, &b)); 
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_AIR_C, &c));
			EXCCHECKCVI( GetCtrlVal( panel, EB2YY_DBG_AIR_D, &d)); 
			
			state = 0;
			if (a) state |= (1<<0);
			if (b) state |= (1<<1);
			if (c) state |= (1<<2);
			if (d) state |= (1<<3);
			sprintf(command, "\\HWI\\DIO\\o4w 1 %d", state);  
			EXCCHECK( gs_pvline->Write(gs_pvline, command) ); 
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

int CVICALLBACK OnSensor (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t 		error = 0;

	char*	ch = NULL;
	int			value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control)
			{
				case EB2YY_R1:	
					ch = "1";
				break;
				case EB2YY_R2:
					ch = "2";
				break;
				case EB2YY_R3:
					ch = "3";
				break;
				case EB2YY_R4:
					ch = "4";
				break;
				case EB2YY_R5:
					ch = "5";
				break;
			}
			
			if ( ch )
			{
				EXCCHECKCVI( GetCtrlVal( panel, control, &value));
				EXCCHECKIVI( Resistor, gs_IviSessons[RESISTOR], Resistor_ConfigureOutputResistance( gs_IviSessons[RESISTOR], ch, value ));
			}

			break;
	}
Error:
	EXCDISPLAY( pexception); 
	return 0;
}
