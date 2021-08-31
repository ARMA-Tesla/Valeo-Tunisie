#include <userint.h>
#include <vline/vline.h>
#include <eventfactory/EventFactory.h> 
#include <utility.h>
#include <guid.h>
#include <jig/class/jig_class.h>
#include <test/test_helper_functions.h>
#include <multithread/multithread.h>
#include <eldaq/class/eldaq.h>
#include <IviBridge/IviBridge.h> 
#include <IviDmm.h>
#include "lang_translation.h"
#include "panel_E89_plugin.h"

#define NUMBER_OF_PANELS 	6
#define MODULE_JIG			MODULE_JIG_E89_IHKA /* guid.h */
#define COMMAND_LENGHT		1024
#define SUBPANEL_LEFT 		0
#define SUBPANEL_TOP		467
#define CONFIG_FILE			"\\..\\plugins\\test_e89_ihka.xml"

/* error handling */
#define EXCCHECKIVIDAQ( fCall) \
	EXCCHECKIVI(eldaq, gs_DAQ, fCall)

#define EXCCHECKIVIDMM( fCall) \
	EXCCHECKIVI(IviDmm, gs_DMM, fCall)

static int					gs_panel[NUMBER_OF_PANELS+1];
static SVLinePtr			gs_pvline; 
SLanguagePtr 				gs_pLanguage;
static SJigPtr				gs_pjig = NULL;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL;  
static HANDLE				gs_hEventStop = NULL;
static HANDLE				gs_hThread = NULL;
static SPluginArgsPtr		gs_PluginArgs = NULL;
static SIviBridgePtr		gs_IviBridge = NULL;
static ViSession			gs_DAQ = 0;
static ViSession			gs_DMM = 0;

static SElExceptionPtr PanelVersion(void);
static SElExceptionPtr ResetProductCtrl(void);
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
	EM_VLINE
} EMode;

typedef enum _EPanelSwtch
{
	SWTCH_CONNECT_KLINE = 0,
	SWTCH_CONNECT_CAN,
	SWTCH_DISCONNECT,
	SWTCH_MEASUREMENT,
	SWTCH_DMM_PWR_VOLTAGE,
	SWTCH_DMM_SZM_5V,
	SWTCH_DMM_SZM_12V
} EPanelSwtch;

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PanelSwtch"
static SElExceptionPtr PanelSwtch(EPanelSwtch mode)
{
	SElExceptionPtr	pexception = NULL;

	switch(mode)
	{
		case SWTCH_CONNECT_KLINE:
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "KLINE"));
			break;
		case SWTCH_CONNECT_CAN:
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "CAN"));
			break;
		case SWTCH_DISCONNECT:
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "PANEL_DISCONNECT"));
			break;
		case SWTCH_MEASUREMENT:
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "NOZZLE_POTI"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "SOLAR_SENSOR"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "SOLAR_SENSOR_NTC"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "EVAPO_SENSOR"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "OUTLET_SENSOR_FEET"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "OUTLET_SENSOR_PANEL"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "SZM_PUSH"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "SZM_COMM"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "SZM_PWM"));
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "BLOWER_CTR"));
			break;
		case SWTCH_DMM_PWR_VOLTAGE:
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "DMM_PWR_VOLTAGE"));
			break;
		case SWTCH_DMM_SZM_5V:
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "DMM_SZM_5V"));
			break;
		case SWTCH_DMM_SZM_12V:
			EXCCHECK( SwtchSequence(gs_Modules, CONFIG_FILE, "DMM_SZM_12V"));
			break;
	}	

Error:
	EXCRETHROW( pexception);  
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
				EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_CONNECT_CAN, 0));
				
				/* switch */
				EXCCHECK( PanelSwtch(SWTCH_MEASUREMENT));
				EXCCHECK( PanelSwtch(SWTCH_DMM_PWR_VOLTAGE));
				EXCCHECK( PanelSwtch(SWTCH_CONNECT_KLINE));
				
				EXCCHECK( ResetProductCtrl());
				EXCDBGSTR( PanelVersion());
			}
			else
			{
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
#define __FUNC__ "OnConnectCan"
int CVICALLBACK OnConnectCan (int panel, int control, int event,
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
				EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_CONNECT_KLINE, 0));
				
				/* switch */
				EXCCHECK( PanelSwtch(SWTCH_MEASUREMENT));
				EXCCHECK( PanelSwtch(SWTCH_DMM_PWR_VOLTAGE));
				EXCCHECK( PanelSwtch(SWTCH_CONNECT_CAN));
			}
			else
			{
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
#define __FUNC__ "PanelVersion"
static SElExceptionPtr PanelVersion(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			read[COMMAND_LENGHT] = "";
	int				count, lenght;
	char			ver[256] = "";
	
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
	
		EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_VER, ver));
	}
	
Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetProductCtrl"
static SElExceptionPtr SetProductCtrl(int panel_type)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_MAX, ATTR_VISIBLE, !panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_IHKA_RECY1, ATTR_VISIBLE, !panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_IHKA_RECY2, ATTR_VISIBLE, !panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_BLOWER_IHKR1, ATTR_VISIBLE, panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_BLOWER_IHKR7, ATTR_VISIBLE, panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_IHKA_DIST_1, ATTR_VISIBLE, !panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_IHKA_DIST_2, ATTR_VISIBLE, !panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_IHKA_DIST_3, ATTR_VISIBLE, !panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_LED_AC, ATTR_VISIBLE, !panel_type));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_MICROSWITCH, ATTR_VISIBLE, panel_type));

Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ResetProductCtrl"
static SElExceptionPtr ResetProductCtrl(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_MAX, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_IHKA_RECY1, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_IHKA_RECY2, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_BLOWER_IHKR1, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_BLOWER_IHKR7, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_IHKA_DIST_1, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_IHKA_DIST_2, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_IHKA_DIST_3, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LED_AC, 0));

	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PWM_BLUE_BCK, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PWM_RED_BCK, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PWM_GREEN_FNC, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PWM_ORANGE_BCK, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PWM_SZM_BLUE_FNC, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PWM_SZM_RED_FNC, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PWM_ORANGE_FNC, 0));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PWM_BLOWER, 0));

	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_DMM_VALUE, 0));

Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Panel E89 Plugin}::OnProductType"
int CVICALLBACK OnProductType(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int				error = 0;
	int				value;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, control, &value));
			EXCCHECKCVI( SetCtrlIndex(panel, PE89_PRODUCT_TYPE, value));
			EXCCHECK(SetProductCtrl(value));
			break;
	}

Error:
	EXCDISPLAY(pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Panel E89 Plugin}::OnLED"
int CVICALLBACK OnLED(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int				error = 0;
	int				value, panel_type;
	int				byte;
	char			message[1024] = "";

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, control, &value));
			EXCCHECKCVI( GetCtrlVal(panel, PE89_PTYPE, &panel_type));
			
			if ( panel_type)
			{
			/*
			 * IHKR
			 */
			switch ( control)
			{
				case PE89_LED_IHKA_AUTO:	byte = 0; break;	
				case PE89_LED_IHKA_DEFROST:	byte = 1; break;
				case PE89_LED_IHKA_HRW:		byte = 2; break;
				
				case PE89_LED_BLOWER_IHKR1:	byte = 3; break;
				case PE89_LED_BLOWER1:		byte = 4; break;
				case PE89_LED_BLOWER2:		byte = 5; break;
				case PE89_LED_BLOWER3:		byte = 6; break;
				case PE89_LED_BLOWER4:		byte = 7; break;
				case PE89_LED_BLOWER5:		byte = 8; break;
				case PE89_LED_BLOWER_IHKR7:	byte = 9; break;

				case PE89_SZM_SUPPLY_CMD:		byte = 31; break;
				case PE89_SZM_STROBE_CMD:		byte = 32; break;
				case PE89_SZM_BCKL_ENABLE_CMD:	byte = 33; break;

				case PE89_LIN_SUPPLY_CMD:	byte = 29; break;
				case PE89_LIN_ENABLE_CMD:	byte = 30; break;

				default:
					EXCTHROW( -1, "Not Valid Output control" );
					break;
			}
			}
			else
			{
			/*
			 * IHKA
			 */
			
			switch ( control)
			{
				case PE89_LED_AC:			byte = 0; break;	
				case PE89_LED_MAX:			byte = 29; break;
				
				case PE89_LED_IHKA_DEFROST:	byte = 4; break;
				case PE89_LED_IHKA_HRW:		byte = 3; break;
				case PE89_LED_IHKA_RECY1:	byte = 1; break;
				case PE89_LED_IHKA_RECY2:	byte = 2; break;
				
				case PE89_LED_IHKA_AUTO:	byte = 5; break;
				case PE89_LED_IHKA_DIST_1:	byte = 28; break;
				case PE89_LED_IHKA_DIST_2:	byte = 27; break;
				case PE89_LED_IHKA_DIST_3:	byte = 26; break;
				
				case PE89_LED_BLOWER1:		byte = 6; break;
				case PE89_LED_BLOWER2:		byte = 7; break;
				case PE89_LED_BLOWER3:		byte = 8; break;
				case PE89_LED_BLOWER4:		byte = 9; break;
				case PE89_LED_BLOWER5:		byte = 10; break;
				
				case PE89_SZM_SUPPLY_CMD:		byte = 36; break;
				case PE89_SZM_STROBE_CMD:		byte = 37; break;
				case PE89_SZM_BCKL_ENABLE_CMD:	byte = 38; break;

				case PE89_LIN_SUPPLY_CMD:	byte = 34; break;
				case PE89_LIN_ENABLE_CMD:	byte = 35; break;

				default:
					EXCTHROW( -1, "Not Valid Output control" );
					break;
			}
			}
			
			sprintf( message, "\\HWI\\DIO\\o1w %d %d", byte, (int)(value != 0 ));
			EXCCHECK( gs_pvline->Write( gs_pvline, message) );  
			break;
	}

Error:
	EXCDISPLAY(pexception);
	return 0;
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
				case PE89_PWM_BLUE_BCK:
				case PE89_BLUE_BCK_DUTY:
				case PE89_BLUE_BCK_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PE89_PWM_BLUE_BCK, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_BLUE_BCK_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_BLUE_BCK_FREQ, &freq));
					id = 0;
					break;
				case PE89_PWM_RED_BCK:
				case PE89_RED_BCK_DUTY:
				case PE89_RED_BCK_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PE89_PWM_RED_BCK, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_RED_BCK_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_RED_BCK_FREQ, &freq));
					id = 1;
					break;
				case PE89_PWM_GREEN_FNC:
				case PE89_GREEN_FNC_DUTY:
				case PE89_GREEN_FNC_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PE89_PWM_GREEN_FNC, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_GREEN_FNC_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_GREEN_FNC_FREQ, &freq));
					id = 2;
					break;
				case PE89_PWM_ORANGE_BCK:
				case PE89_ORANGE_BCK_DUTY:
				case PE89_ORANGE_BCK_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PE89_PWM_ORANGE_BCK, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_ORANGE_BCK_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_ORANGE_BCK_FREQ, &freq));
					id = 3;
					break;
				case PE89_PWM_SZM_BLUE_FNC:
				case PE89_SZM_BLUE_FNC_DUTY:
				case PE89_SZM_BLUE_FNC_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PE89_PWM_SZM_BLUE_FNC, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_SZM_BLUE_FNC_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_SZM_BLUE_FNC_FREQ, &freq));
					id = 4;
					break;
				case PE89_PWM_SZM_RED_FNC:
				case PE89_SZM_RED_FNC_DUTY:
				case PE89_SZM_RED_FNC_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PE89_PWM_SZM_RED_FNC, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_SZM_RED_FNC_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_SZM_RED_FNC_FREQ, &freq));
					id = 5;
					break;
				case PE89_PWM_ORANGE_FNC:
				case PE89_ORANGE_FNC_DUTY:
				case PE89_ORANGE_FNC_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PE89_PWM_ORANGE_FNC, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_ORANGE_FNC_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_ORANGE_FNC_FREQ, &freq));
					id = 6;
					break;
			}
			sprintf(command, "\\HWI\\FIO\\ohzdc16w %d %d %d", id, freq, (status)? duty:0); 	
			EXCCHECK( gs_pvline->Write(gs_pvline, command) ); 
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPwm2"
int CVICALLBACK OnPwm2 (int panel, int control, int event,
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
				case PE89_PWM_BLOWER:
				case PE89_BLOWER_DUTY:
				case PE89_BLOWER_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PE89_PWM_BLOWER, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_BLOWER_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PE89_BLOWER_FREQ, &freq));
					id = 0;
					break;
			}
			sprintf(command, "\\HWI\\FIO\\ohzdc8w %d %d %d", id, freq, (status)? duty:0); 	
			EXCCHECK( gs_pvline->Write(gs_pvline, command) ); 
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
	int				id, panel_type;
	unsigned int 	value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(gs_panel[PE89], PE89_PTYPE, &panel_type));

			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SOLAR_SENSOR, 0));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_NOZZLE_POTI_FBK, 0));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_ON_SOLAR_SENSOR, 0));

			for(id=0; id<((panel_type)? 13:15); id++)
			{
				sprintf(command, "\\HWI\\AIO\\i16ar %d", id);
				EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT));
				sscanf(read, "%x", &value);
			
				if(!panel_type && id==0)
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SOLAR_SENSOR, value));
				else if((!panel_type && id==1) ||
						(panel_type && id==0))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_TEMP_PCB, value));
				else if(!panel_type && id==2)
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_ON_SOLAR_SENSOR, value));
				else if((!panel_type && id==3) ||
						(panel_type && id==2))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_EVAPO, value));
				else if((!panel_type && id==4) ||
						(panel_type && id==3))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_FOOT_OUTLET, value));
				else if((!panel_type && id==5)||
						(panel_type && id==4))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_PANEL_OUTLET, value));
				else if((!panel_type && id==6)||
						(panel_type && id==5))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_BATTERY, value));
				else if((!panel_type && id==7)||
						(panel_type && id==6))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_POTI_LEFT_FBK, value));
				else if((!panel_type && id==8)||
						(panel_type && id==7))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_POTI_RIGHT_FBK, value));
				else if(!panel_type && id==9)
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_NOZZLE_POTI_FBK, value));
				else if((!panel_type && id==10)||
						(panel_type && id==8))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_PUSH_12, value));
				else if((!panel_type && id==11)||
						(panel_type && id==9))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_PUSH_34, value));
				else if((!panel_type && id==12)||
						(panel_type && id==10))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_PUSH_5, value));
				else if((!panel_type && id==13)||
						(panel_type && id==11))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_VARIANT_1, value));
				else if((!panel_type && id==14)||
						(panel_type && id==12))
					EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_VARIANT_2, value));
			}

			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnDmm"
int CVICALLBACK OnDmm (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int				value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlIndex(panel, control, &value));
			switch(value)
			{
				case 0:
					EXCCHECK( PanelSwtch(SWTCH_DMM_PWR_VOLTAGE));
					break;
				case 1:
					EXCCHECK( PanelSwtch(SWTCH_DMM_SZM_5V));
					break;
				case 2:
					EXCCHECK( PanelSwtch(SWTCH_DMM_SZM_12V));
					break;
			}
			break;
	}


Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnVoltageRead"
int CVICALLBACK OnVoltageRead (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	double			rvalue;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKIVIDAQ( eldaq_ConfigureMeasurement (gs_DAQ, "", 2000, 0.02));
			checkErr( eldaq_StartMeasurement(gs_DAQ));

			Sleep(40);

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai2", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_5V_EXTERNAL, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai3", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_PWM_GREEN, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai4", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_STROBE, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai5", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_DATA, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai6", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_SCK, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai7", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_PWM_BACKLIGHT, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai8", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_PWM_RED, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai9", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_PUSH5, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai10", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_PUSH34, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai11", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_PUSH12, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai12", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_BLOWER_PWM, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai13", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_VARIANT2, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai14", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_SZM_VARIANT1, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai17", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_LIN_SUPPLY, rvalue));

			EXCCHECKIVIDAQ( eldaq_ReadMeasurement(gs_DAQ, "ai18", "voltage", ELDAQ_VAL_MEASUREMENT_MEAN, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_OUTLET_SENSOR, rvalue));

			EXCCHECKIVIDAQ(eldaq_StopMeasurement(gs_DAQ));

			EXCCHECKIVIDMM( IviDmm_ConfigureMeasurement(gs_DMM, IVIDMM_VAL_DC_VOLTS, 25.0, 0.001));
			EXCCHECKIVIDMM( IviDmm_Read (gs_DMM, 2000, &rvalue));
			EXCCHECKCVI( SetCtrlVal(panel, PE89_DMM, rvalue));
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
	uint32_t  		data_lenght = 2032;
	uint32_t   		stop_address; 
	uint8_t			*data = NULL, *pdata = NULL;
	int				i, value, block_size, status;
	
	switch (event)
	{
		case EVENT_COMMIT:
				
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
#define __FUNC__ "Thread_ButtonRead"
static DWORD WINAPI Thread_ButtonRead(LPVOID param)  
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	DWORD			state;
	char			command[COMMAND_LENGHT], read[COMMAND_LENGHT];  
	int				i,value, valuerot[2], id, panel_type;
	char*			pbyte = NULL;
	uint8_t			data[64];
	
	EXCCHECKCVI( GetCtrlVal(gs_panel[PE89], PE89_PTYPE, &panel_type));
	
	while(TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop, 100) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		/*button checking*/
		EXCCHECK( gs_pvline->Reset( gs_pvline ));
		EXCCHECK( gs_pvline->Query( gs_pvline, "\\FAC\\R S1050000001CDE", read, COMMAND_LENGHT ));
		memset( data, 0, sizeof(data));
		pbyte = read + 8;
		i = 0;	

#define _CB( p ) ((*(p) > '9' ) ? (10 + ((*(p) > 'Z' ) ? (*(p) - 'a') : (*(p) - 'A'))) : (*(p) - '0'))
#define CB(p) ((_CB(p) << 4) | ((*(p+1)) ? _CB(p+1) : 0 )) 			

		while ( *pbyte )
		{
			data[i++] = CB(pbyte);
			if (*++pbyte ) pbyte++;
		}
	
		if ( panel_type )
		{
			/******************************************************************************
			 * IHKR ***********************************************************************
			 ******************************************************************************/
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_AC, ATTR_VISIBLE, 0));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_RECI, ATTR_VISIBLE, 0));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_DIST, ATTR_VISIBLE, 0));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_MAX, ATTR_VISIBLE, 0));

			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_AUTO, ATTR_VISIBLE,		(((~(data[0])) & (1 << 0)) !=0 ) )); /* AC */
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_HFS, ATTR_VISIBLE,		(((~(data[0])) & (1 << 1)) !=0 ) )); /* RECI */
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_HRW, ATTR_VISIBLE,		(((~(data[0])) & (1 << 2)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_FAN_UP, ATTR_VISIBLE,	(((~(data[0])) & (1 << 3)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_FAN_DOWN, ATTR_VISIBLE,	(((~(data[0])) & (1 << 4)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_1_PUSH,		((data[0] & (1 << 5)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_2_PUSH,		((data[0] & (1 << 6)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_3_PUSH,		((data[0] & (1 << 7)) !=0 ) ));

			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_4_PUSH,		((data[1] & (1 << 0)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_5_PUSH,		((data[1] & (1 << 1)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LIN_SUPPLY_DIAG,   ((data[1] & (1 << 4)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_SUPPLY_DIAG,   ((data[1] & (1 << 5)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_5V_SZM_DIAG,		((data[1] & (1 << 6)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_MICROSWITCH,		((data[1] & (1 << 7)) !=0 ) ));

			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_BACKLIGHT_DIAG,	((data[2] & (1 << 0)) !=0 ) ));
		}
		else
		{
			/******************************************************************************
			 * IHKA ***********************************************************************
			 ******************************************************************************/
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_AC, ATTR_VISIBLE,       (((~(data[0])) & (1 << 0)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_RECI, ATTR_VISIBLE,     (((~(data[0])) & (1 << 1)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_HFS, ATTR_VISIBLE,      (((~(data[0])) & (1 << 2)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_HRW, ATTR_VISIBLE,      (((~(data[0])) & (1 << 3)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_FAN_UP, ATTR_VISIBLE,   (((~(data[0])) & (1 << 4)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_FAN_DOWN, ATTR_VISIBLE, (((~(data[0])) & (1 << 5)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_AUTO, ATTR_VISIBLE, 	   (((~(data[0])) & (1 << 6)) !=0 ) ));
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_DIST, ATTR_VISIBLE,     (((~(data[0])) & (1 << 7)) !=0 ) ));
		
			EXCCHECKCVI( SetCtrlAttribute(gs_panel[PE89], PE89_BTN_MAX, ATTR_VISIBLE, 	   (((~(data[1])) & (1 << 0)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_1_PUSH,   ((data[1] & (1 << 1)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_2_PUSH,   ((data[1] & (1 << 2)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_3_PUSH,   ((data[1] & (1 << 3)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_4_PUSH,   ((data[1] & (1 << 4)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_5_PUSH,   ((data[1] & (1 << 5)) !=0 ) ));

			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_LIN_SUPPLY_DIAG,   ((data[2] & (1 << 0)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_SZM_SUPPLY_DIAG,   ((data[2] & (1 << 1)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_5V_SZM_DIAG,		((data[2] & (1 << 2)) !=0 ) ));
			EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_BACKLIGHT_DIAG,	((data[2] & (1 << 3)) !=0 ) ));
		}
	};

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
	int					mode,index;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, control, &mode));
			EXCCHECK( ChangeMode( mode));
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
	
	EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[PE89]));  
		
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
	
	EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_BUTTON_READ, FALSE));

	EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[PE89]));   

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
#define __FUNC__ "{Panel E89 Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr  	pexception = NULL;
	char        		pathname[MAX_PATHNAME_LEN];      
	int32_t         	error = 0;
	const char*      	pconfiguration_file = NULL;
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
   
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "panel_E89.uir", PE89, PE89lang, &gs_panel[PE89])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[PE89], "panel_E89.uir", VLINE, VLINElang, &gs_panel[VLINE])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[PE89], "panel_E89.uir", MEMORY, MEMORYlang, &gs_panel[MEMORY])); 
   
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PE89], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PE89], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[VLINE], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[VLINE], ATTR_TOP, SUBPANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MEMORY], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MEMORY], ATTR_TOP, SUBPANEL_TOP));

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
			{
				EXCCHECK( ShowPanel());
			}
		}

		gs_IviBridge = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
		if(gs_IviBridge)
		{
			EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "DAQ", &gs_DAQ));
			EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "DMM", &gs_DMM));
		}
	}

	EXCCHECK( RegisterEventFunction());  

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel E89 Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
   SElExceptionPtr   pexception = NULL;
      
Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel E89 Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
   	SElExceptionPtr	pexception = NULL;
   	int32_t         error = 0;
   
   	/* stop second thread */
	EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PE89], PE89_BUTTON_READ, FALSE));
	
Error:   
   	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Panel E89 Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
   	int32_t         error = 0;
   
   	/* stop second thread */
	EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));  

	if(gs_panel[PE89])
		EXCCHECKCVI( DiscardPanel(gs_panel[PE89]));

Error:   
   	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel E89 Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel E89 Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif
