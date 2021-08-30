#include <vline/vline.h>
#include <eventfactory/EventFactory.h> 
#include <utility.h>
#include <guid.h>
#include <jig/class/jig_class.h>
#include <multithread/multithread.h>
#include <test/testseq.h>
#include "lang_translation.h"
#include "panel_445l_plugin.h"

#define NUMBER_OF_PANELS 	5
#define MODULE_JIG			MODULE_JIG_445L /* guid.h */
#define COMMAND_LENGHT		1024
#define SUBPANEL_LEFT 		0
#define SUBPANEL_TOP		467

static int					gs_panel[NUMBER_OF_PANELS+1];
static SVLinePtr			gs_pvline; 
SLanguagePtr 				gs_pLanguage;
static SJigPtr				gs_pjig = NULL;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL;  
static HANDLE				gs_hEventStop = NULL;
static HANDLE				gs_hThread = NULL;
static SPluginArgsPtr		gs_PluginArgs = NULL;
static STestSequencePtr		gs_TestSequence = NULL;

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
static SElExceptionPtr WriteAutoLCD( int panel );

static enum _EMode
{
	EM_IO = 0,
	EM_MEMORY,
	EM_LCD,
	EM_LCDMAN,
	EM_VLINE
} EMode;

typedef enum _EPanelSwtch
{
	SWTCH_CONNECT = 0,
	SWTCH_DISCONNECT,
	SWTCH_ILLUMINATION_ON,
	SWTCH_ILLUMINATION_OFF
} EPanelSwtch;

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
			case SWTCH_ILLUMINATION_ON:
				EXCCHECK( gs_TestSequence->RunSelected(gs_TestSequence, NULL, "00.20"));
				break;
			case SWTCH_ILLUMINATION_OFF:
				EXCCHECK( gs_TestSequence->RunSelected(gs_TestSequence, NULL, "00.21"));
				break;
		}
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
				EXCCHECK( PanelSwtch(SWTCH_CONNECT));
				EXCCHECK( ResetPanelCtrl());

				/* read panel info */
				pexception = PanelVersion();
				if(pexception)
				{
					EXCDELETE( pexception);
					EXCCHECKCVI( SetCtrlVal(panel, control, 0));
					EXCCHECK( PanelSwtch(SWTCH_DISCONNECT));
					EXCTHROW( -1, "Panel Connect Fault!");
				}
			}
			else
			{
				/* stop second thread */
				EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
				EXCCHECKCVI( SetCtrlVal(gs_panel[P445L], P445L_BUTTON_READ, FALSE));
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
#define __FUNC__ "OnIllumination"
int CVICALLBACK OnIllumination (int panel, int control, int event,
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
				EXCCHECK( PanelSwtch(SWTCH_ILLUMINATION_ON));
			}
			else
			{
				EXCCHECK( PanelSwtch(SWTCH_ILLUMINATION_OFF));
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
	
	SetCtrlVal( gs_panel[P445L], P445L_DUAL_LED, 0);
	SetCtrlVal( gs_panel[P445L], P445L_DUAL_LED, 0);                 
	SetCtrlVal( gs_panel[P445L], P445L_RECIR_LED, 0);                
	SetCtrlVal( gs_panel[P445L], P445L_PICTURE, 0);                   
	SetCtrlVal( gs_panel[P445L], P445L_HRW_LED, 0);                   
	SetCtrlVal( gs_panel[P445L], P445L_ATS_LED, 0);                   
	SetCtrlVal( gs_panel[P445L], P445L_AC_LED, 0);                   
	SetCtrlVal( gs_panel[P445L], P445L_AU_LED, 0);                   
	SetCtrlVal( gs_panel[P445L], P445L_HFS_LED, 0);                   
	SetCtrlVal( gs_panel[P445L], P445L_BUTTON_READ, 0);              
	SetCtrlVal( gs_panel[P445L], P445L_AUTO_LED, 0);       
	SetCtrlVal( gs_panel[P445L], P445L_PWM_LCD, 0);  
	SetCtrlVal( gs_panel[P445L], P445L_PWM_LED, 0);  

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
	
	EXCCHECK( gs_pvline->Write( gs_pvline, "\\FAC") );  
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
	
		EXCCHECKCVI( SetCtrlVal(gs_panel[P445L], P445L_VER, ver));
	}
	
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
				case P445L_PWM_LCD:
					EXCCHECKCVI( GetCtrlVal(panel, P445L_PWM_LCD, &status));
					break;
				case P445L_PWM_LED:
				case P445L_LED_DUTY:
				case P445L_LED_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, P445L_PWM_LED, &status));
					EXCCHECKCVI( GetCtrlVal(panel, P445L_LED_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, P445L_LED_FREQ, &freq));
					
					sprintf(command, "\\HWI\\FIO\\ohzdc8w 0 %d %d", 
							(status==TRUE) ? freq : 0, (status==TRUE) ? duty : 0); 

					EXCCHECK( gs_pvline->Write(gs_pvline, command) );  	
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
#define __FUNC__ "OnLed"
int CVICALLBACK OnLed (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int				id, status, light;
	char			command[COMMAND_LENGHT]; 
	
	switch (event)
	{
		case EVENT_COMMIT:

			EXCCHECKCVI( GetCtrlVal(panel, control, &status));
			
			switch(control)
			{
				case P445L_AUTO_LED:
					id = 0;
					light = TRUE;
					break;
				case P445L_AC_LED:
					id = 1;
					light = TRUE;
					break;
				case P445L_RECIR_LED:
					id = 2;
					light = TRUE;
					break;
				case P445L_HRW_LED:
					id = 3;
					light = TRUE;
					break;
				case P445L_ATS_LED:
					id = 4;
					light = TRUE;
					break;
				case P445L_DUAL_LED:
					id = 5;
					light = TRUE;
					break;
				case P445L_HFS_LED:
					id = 6;
					light = TRUE;
					break;
				case P445L_AU_LED:
					id = 7;
					light = TRUE;
					break;
			}
			
			if(status==TRUE)
				sprintf(command, "\\HWI\\DIO\\o1w %d %d", id, light); 
			else
				sprintf(command, "\\HWI\\DIO\\o1w %d %d", id, !light);  
				
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
	int				id;
	unsigned int 	value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			for(id = 0; id < 2; id++)
			{
				sprintf(command, "\\HWI\\AIO\\i16ar %d", id);
				EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT ) );
				sscanf(read, "%x", &value);
			
				if(id==0)
					EXCCHECKCVI( SetCtrlVal(panel, P445L_BATTERY_VOLTAGE, value));
				if(id==1)
					EXCCHECKCVI( SetCtrlVal(panel, P445L_ZEBRA_COMPRESSION, value));
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

#define BLOCK_SIZE 		1
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMemoryRead"
int CVICALLBACK OnMemoryRead (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	char			command[COMMAND_LENGHT], read[COMMAND_LENGHT], *pread = NULL,bytenbchar[2]; 
	uint32_t   		address;
  	uint32_t   		start_address = 0x0000; 
	uint32_t  		data_lenght = 7,data_lenght2 = 16;
	uint32_t   		stop_address; 
	uint8_t			*data = NULL, *pdata = NULL;
	int				i, value,block_size,bytenb,status,checksum;
	
	switch (event)
	{
		case EVENT_COMMIT:
			/* stop second thread */
			EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
			EXCCHECKCVI( SetCtrlVal(gs_panel[P445L], P445L_BUTTON_READ, FALSE));
			
			/*Read 0x1000 memory*/
			stop_address = start_address + data_lenght;
			data = calloc(data_lenght+data_lenght2, sizeof(uint8_t));
			pdata = data;
			for(address=start_address; address < stop_address; address +=BLOCK_SIZE)
    	 	{
				block_size = (BLOCK_SIZE<(stop_address-address))? BLOCK_SIZE:(stop_address-address);
				checksum=(255-(5+(address/256)+(address&0XFF)+(block_size/256)+(block_size&0XFF)))%256;
			
				sprintf(command, "\\FAC\\R S105%04x%04x%02x", address, block_size,checksum);  
				EXCCHECK( gs_pvline->Reset( gs_pvline ));
				EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT)); 
			
				pread=read;
				bytenbchar[0]=pread[2];
				bytenbchar[1]=pread[3];
				sscanf(bytenbchar, "%X", &bytenb);
				pread[bytenb*2+2]='\0';
				pread=pread+6;
				pread[0]='0';
				pread[1]='x';
				
				
				if(pread==NULL)
						EXCTHROW( -1, "ERROR_NULL_POINTER");
					
				sscanf(pread, "%X", &value);
				*(pdata) = (uint8_t)value;
				pdata++;
				
				
				Sleep(10);
			}
				
			/*Read 0x1000 memory*/	
			start_address=0x1000;
			stop_address = start_address + data_lenght2;
			for(address=start_address; address < stop_address; address +=BLOCK_SIZE)
	  		{
				block_size = (BLOCK_SIZE<(stop_address-address))? BLOCK_SIZE:(stop_address-address);
				checksum=(255-(5+(address/256)+(address&0XFF)+(block_size/256)+(block_size&0XFF)))%256;
			
				sprintf(command, "\\FAC\\R S105%04x%04x%02x", address, block_size,checksum);  
				EXCCHECK( gs_pvline->Reset( gs_pvline ));
				EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT)); 
			
				pread=read;
				bytenbchar[0]=pread[2];
				bytenbchar[1]=pread[3];
				sscanf(bytenbchar, "%X", &bytenb);
				pread[bytenb*2+2]='\0';
				pread=pread+6;
				pread[0]='0';
				pread[1]='x';
				
				
				if(pread==NULL)
						EXCTHROW( -1, "ERROR_NULL_POINTER");
				
				sscanf(pread, "%X", &value);
				*(pdata) = (uint8_t)value;
				pdata++;

				Sleep(10);
			}
			start_address=0;
			
			EXCCHECK( DisplayDumpData( panel, MEMORY_MEMORY, data,data_lenght+ data_lenght2 , start_address));	
				
				
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
	int				i,value, valuerot[2], id;

	while(TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop, 100) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		/*button checking*/
		sprintf(command, "\\FAC\\R S10500000006F4");
		EXCCHECK( gs_pvline->Reset( gs_pvline ));
		EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT ));
		
		read[6]='0'	;read[7]='x';read[14]='\0';
		sscanf((read+6), "%X", &value);
		
		value=value/(256);
		
		/*Byte 1*/
		/* bit 0 : MODE */
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_MODE_PICT, ATTR_VISIBLE, !(value & 1)));
		/* bit 1 : DUAL */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_DUAL_PICT, ATTR_VISIBLE, !(value & 1))); 
		/* bit 2 : HFS */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_HFS_PICT, ATTR_VISIBLE, !(value & 1)));    
		/* bit 3 : AIR UP */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_AU_PICT, ATTR_VISIBLE, !(value & 1)));
		
		value=value/(32);
		
		/*Byte 0*/
		/* bit 0 : AUTO */
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_AUTO_PICT, ATTR_VISIBLE, !(value & 1)));
		/* bit 1 : AC */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_AC_PICT, ATTR_VISIBLE, !(value & 1))); 
		/* bit 2 : RECIRCULATION */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_RECIR_PICT, ATTR_VISIBLE, !(value & 1)));
		/* bit 3 : HRW */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_HRW_PICT, ATTR_VISIBLE, !(value & 1)));
		/* bit 4 : AIR TO SCREEN */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_ATS_PICT, ATTR_VISIBLE, !(value & 1)));    
		/* bit 5 : OFF */
		value=value/2; 
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_OFF_PICT, ATTR_VISIBLE, !(value & 1)));
		/* bit 6 : VENTILATION DOWN */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_VENT_D_PICT, ATTR_VISIBLE, !(value & 1)));
		/* bit 7 : VENTILATION UP */
		value=value/2;
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_VENT_U_PICT, ATTR_VISIBLE, !(value & 1)));
																						
		/*Rotative button checking*/
		
		for(i=0;i<2;i++)
		{
			sprintf(command, "\\FRO\\grs %d",i);
			EXCCHECK( gs_pvline->Reset( gs_pvline ));
			EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT ));
			sscanf(read, "%d", &value);
			valuerot[0]=0;
			valuerot[1]=0;
			if(value>0) valuerot[0]=1;
			if(value<0) valuerot[1]=1;
			
			if(i==0)/*BLOW*/
			{
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_BLOW2_PICT, ATTR_VISIBLE, valuerot[0]));
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_BLOW1_PICT, ATTR_VISIBLE, valuerot[1]));
			}
			if(i==1)/*TEMP*/
			{
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_TEMP1_PICT, ATTR_VISIBLE, valuerot[0]));
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[P445L], P445L_TEMP2_PICT, ATTR_VISIBLE, valuerot[1]));
			}
		}
	
	};
	
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
	stop_address = 0x1015;
	for( index = start_address; index <= stop_address; index++, column++)
	{
		if(index==6)
		{
			index=0x1006;
			address=0x1000;
			
		}
		if (((index % 16 )==0)||( index == 0x1006 ) )
		{
			if ( row != 0 )
			{
				pend += sprintf(pend, " %s\n", text_data);
			}
			row++;
			if(index<0x1000)
				pend += sprintf(pend, "%08x: ", index);
			else
				pend += sprintf(pend, "%08x: ", index-6);
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
			if(address<0x1000)
			{
				pend += sprintf(pend, "%02x ", (int)data[index-address]);
				text_data[column] = ( isprint(data[index-address])) ? data[index-address] : '.';
			}
			else
			{
				pend += sprintf(pend, "%02x ", (int)data[index-address]);
				text_data[column] = ( isprint(data[index-address])) ? data[index-address] : '.';
			}	
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
		case EM_LCD:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[LCDAUTO]));   
			break;
		case EM_LCDMAN:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[LCDMAN]));   
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
		case EM_LCD:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[LCDAUTO]));
			break;
		case EM_LCDMAN:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[LCDMAN]));
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
	
	EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[P445L]));  
		
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
	EXCCHECKCVI( SetCtrlVal(gs_panel[P445L], P445L_BUTTON_READ, FALSE));

	EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[P445L]));   

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
#define __FUNC__ "{Panel 445L Plugin}::Initialize"
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
   
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "panel_445L.uir", P445L, P445Llang, &gs_panel[P445L])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[P445L], "panel_445L.uir", VLINE, VLINElang, &gs_panel[VLINE])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[P445L], "panel_445L.uir", MEMORY, MEMORYlang, &gs_panel[MEMORY])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[P445L], "panel_445L.uir", LCDAUTO, LCDlang, &gs_panel[LCDAUTO])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[P445L], "panel_445L.uir", LCDMAN, LCDMANlang, &gs_panel[LCDMAN])); 
   
	EXCCHECKCVI( SetPanelAttribute (gs_panel[P445L], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[P445L], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[VLINE], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[VLINE], ATTR_TOP, SUBPANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MEMORY], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MEMORY], ATTR_TOP, SUBPANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[LCDAUTO], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[LCDAUTO], ATTR_TOP, SUBPANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[LCDMAN], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[LCDMAN], ATTR_TOP, SUBPANEL_TOP));

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
	}

	EXCCHECK( RegisterEventFunction());  

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel 445L Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
   SElExceptionPtr   pexception = NULL;
      
Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel 445L Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
   	SElExceptionPtr	pexception = NULL;
   	int32_t         error = 0;
   
   	/* stop second thread */
	EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));
	EXCCHECKCVI( SetCtrlVal(gs_panel[P445L], P445L_BUTTON_READ, FALSE));
	
Error:   
   	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Panel 445L Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
   	int32_t         error = 0;
   
   	/* stop second thread */
	EXCCHECK( ThreadStop( &gs_hThread, &gs_hEventStop));  

	if(gs_panel[P445L])
		EXCCHECKCVI( DiscardPanel(gs_panel[P445L]));

Error:   
   	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel 445L Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel 445L Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif 

#undef __FUNC__
#define __FUNC__ "OnManualLCD"
int CVICALLBACK OnManualLCD (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int				value;

	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetCtrlVal( panel, control, &value);
			value = ( value ) ? 0 : 1;
			SetCtrlVal( panel, control, value);
			
			switch (  control )
			{
			case LCDMAN_FAN:
			case LCDMAN_B1:
			case LCDMAN_B2:
			case LCDMAN_B3:
				SetCtrlVal( panel, LCDMAN_FAN, value);
				SetCtrlVal( panel, LCDMAN_B1, value);
				SetCtrlVal( panel, LCDMAN_B2, value);
				SetCtrlVal( panel, LCDMAN_B3, value);
				break;	
			case LCDMAN_HEAD:
			case LCDMAN_MAN:
				SetCtrlVal( panel, LCDMAN_MAN, value);
				SetCtrlVal( panel, LCDMAN_HEAD, value);
				break;	
			}

			break;
	}
	
Error:
	return 0;
}

#undef __FUNC__
#define __FUNC__ "OnAutoLCD"
int CVICALLBACK OnAutoLCD (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int				value;

	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetCtrlVal( panel, control, &value);
			value = ( value ) ? 0 : 1;
			SetCtrlVal( panel, control, value);
			
			switch (  control )
			{
			case LCDAUTO_S15:
			case LCDAUTO_S16:
			case LCDAUTO_S21:
			case LCDAUTO_S20:
			case LCDAUTO_DOT1:
				SetCtrlVal( panel, LCDAUTO_S15, value);
				SetCtrlVal( panel, LCDAUTO_S16, value);
				SetCtrlVal( panel, LCDAUTO_S21, value);
				SetCtrlVal( panel, LCDAUTO_S20, value);
				SetCtrlVal( panel, LCDAUTO_DOT1, value);
				break;
			case LCDAUTO_S36:
			case LCDAUTO_S37:
			case LCDAUTO_S42:
			case LCDAUTO_S41:
			case LCDAUTO_DOT2:
				SetCtrlVal( panel, LCDAUTO_S36, value);
				SetCtrlVal( panel, LCDAUTO_S37, value);
				SetCtrlVal( panel, LCDAUTO_S42, value);
				SetCtrlVal( panel, LCDAUTO_S41, value);
				SetCtrlVal( panel, LCDAUTO_DOT2, value);
				break;	
			case LCDAUTO_FAN:
			case LCDAUTO_B1:
			case LCDAUTO_B2:
			case LCDAUTO_B3:
				SetCtrlVal( panel, LCDAUTO_FAN, value);
				SetCtrlVal( panel, LCDAUTO_B1, value);
				SetCtrlVal( panel, LCDAUTO_B2, value);
				SetCtrlVal( panel, LCDAUTO_B3, value);
				break;	
			case LCDAUTO_S39:
			case LCDAUTO_S40:
				SetCtrlVal( panel, LCDAUTO_S36, value);
				SetCtrlVal( panel, LCDAUTO_S37, value);
				break;	
			case LCDAUTO_S18:
			case LCDAUTO_S19:
				SetCtrlVal( panel, LCDAUTO_S18, value);
				SetCtrlVal( panel, LCDAUTO_S19, value);
				break;	
			case LCDAUTO_HEAD:
			case LCDAUTO_MAN:
				SetCtrlVal( panel, LCDAUTO_MAN, value);
				SetCtrlVal( panel, LCDAUTO_HEAD, value);
				break;	
			}
			EXCCHECK( WriteAutoLCD( panel));
			break;
	}

Error:
	EXCDISPLAY( pexception); 
	return 0;
}

#undef __FUNC__
#define __FUNC__ "WriteAutoLCD"
static SElExceptionPtr WriteAutoLCD( int panel )
{
	SElExceptionPtr	pexception = NULL;
	char			command[COMMAND_LENGHT] = "";
	uint8_t			lcd[8];
	int				state;
	
	memset( lcd, 0, sizeof( lcd));
	
	GetCtrlVal( panel, LCDAUTO_FAN0, &state);
	if ( state ) lcd[0] |= (uint8_t)(1 << 1);
	
	GetCtrlVal( panel, LCDAUTO_FAN1, &state);
	if ( state ) lcd[2] |= (uint8_t)(1 << 3);
	
	GetCtrlVal( panel, LCDAUTO_FAN2, &state);
	if ( state ) lcd[1] |= (uint8_t)(1 << 0);
	
	GetCtrlVal( panel, LCDAUTO_FAN3, &state);
	if ( state ) lcd[2] |= (uint8_t)(1 << 4);
	
	GetCtrlVal( panel, LCDAUTO_FAN4, &state);
	if ( state ) lcd[2] |= (uint8_t)(1 << 5);
					 
	GetCtrlVal( panel, LCDAUTO_FAN5, &state);
	if ( state ) lcd[2] |= (uint8_t)(1 << 6);
	
	GetCtrlVal( panel, LCDAUTO_FAN6, &state);
	if ( state ) lcd[6] |= (uint8_t)(1 << 6);
	
	GetCtrlVal( panel, LCDAUTO_FAN, &state);
	if ( state ) lcd[2] |= (uint8_t)(1 << 7);
	
	GetCtrlVal( panel, LCDAUTO_DEFROST, &state);
	if ( state ) lcd[6] |= (uint8_t)(1 << 5);
	
	GetCtrlVal( panel, LCDAUTO_RECY_DOWN, &state);
	if ( state ) lcd[6] |= (uint8_t)(1 << 4);
	
	GetCtrlVal( panel, LCDAUTO_RECY_MID, &state);
	if ( state ) lcd[5] |= (uint8_t)(1 << 0);
	
	GetCtrlVal( panel, LCDAUTO_MAN, &state);
	if ( state ) lcd[6] |= (uint8_t)(1 << 3);
	
	
	/* first digit */
	GetCtrlVal( panel, LCDAUTO_S1, &state);
	if ( state ) lcd[6] |= (uint8_t)(1 << 7);
	
	GetCtrlVal( panel, LCDAUTO_S4, &state);
	if ( state ) lcd[3] |= (uint8_t)(1 << 5);
	
	GetCtrlVal( panel, LCDAUTO_S7, &state);
	if ( state ) lcd[7] |= (uint8_t)(1 << 5);
	
	GetCtrlVal( panel, LCDAUTO_S6, &state);
	if ( state ) lcd[7] |= (uint8_t)(1 << 6);
	
	GetCtrlVal( panel, LCDAUTO_S5, &state);
	if ( state ) lcd[7] |= (uint8_t)(1 << 7);
	
	GetCtrlVal( panel, LCDAUTO_S2, &state);
	if ( state ) lcd[3] |= (uint8_t)(1 << 7);
	
	GetCtrlVal( panel, LCDAUTO_S3, &state);
	if ( state ) lcd[3] |= (uint8_t)(1 << 6);
	
	
	/* second digit */
	GetCtrlVal( panel, LCDAUTO_S8, &state);
	if ( state ) lcd[3] |= (uint8_t)(1 << 0);
	
	GetCtrlVal( panel, LCDAUTO_S11, &state);
	if ( state ) lcd[3] |= (uint8_t)(1 << 2);
	
	GetCtrlVal( panel, LCDAUTO_S14, &state);
	if ( state ) lcd[7] |= (uint8_t)(1 << 2);
	
	GetCtrlVal( panel, LCDAUTO_S13, &state);
	if ( state ) lcd[7] |= (uint8_t)(1 << 3);
	
	GetCtrlVal( panel, LCDAUTO_S12, &state);
	if ( state ) lcd[7] |= (uint8_t)(1 << 4);
	
	GetCtrlVal( panel, LCDAUTO_S9, &state);
	if ( state ) lcd[3] |= (uint8_t)(1 << 4);
	
	GetCtrlVal( panel, LCDAUTO_S10, &state);
	if ( state ) lcd[3] |= (uint8_t)(1 << 3);
	
	/* 3th digit */
	GetCtrlVal( panel, LCDAUTO_S15, &state);
	if ( state ) lcd[7] |= (uint8_t)(1 << 1);
	
	GetCtrlVal( panel, LCDAUTO_S18, &state);
	if ( state ) lcd[7] |= (uint8_t)(1 << 0);
	
	GetCtrlVal( panel, LCDAUTO_S17, &state);
	if ( state ) lcd[3] |= (uint8_t)(1 << 1);
	
	/* 4th digit */
	GetCtrlVal( panel, LCDAUTO_S22, &state);
	if ( state ) lcd[4] |= (uint8_t)(1 << 1);
	
	GetCtrlVal( panel, LCDAUTO_S25, &state);
	if ( state ) lcd[1] |= (uint8_t)(1 << 1);
	
	GetCtrlVal( panel, LCDAUTO_S28, &state);
	if ( state ) lcd[4] |= (uint8_t)(1 << 7);
	
	GetCtrlVal( panel, LCDAUTO_S27, &state);
	if ( state ) lcd[5] |= (uint8_t)(1 << 1);
	
	GetCtrlVal( panel, LCDAUTO_S26, &state);
	if ( state ) lcd[6] |= (uint8_t)(1 << 2);
	
	GetCtrlVal( panel, LCDAUTO_S23, &state);
	if ( state ) lcd[2] |= (uint8_t)(1 << 2);
	
	GetCtrlVal( panel, LCDAUTO_S24, &state);
	if ( state ) lcd[0] |= (uint8_t)(1 << 7);
	
	/* 5th digit */
	GetCtrlVal( panel, LCDAUTO_S29, &state);
	if ( state ) lcd[0] |= (uint8_t)(1 << 2);
	
	GetCtrlVal( panel, LCDAUTO_S32, &state);
	if ( state ) lcd[0] |= (uint8_t)(1 << 4);
	
	GetCtrlVal( panel, LCDAUTO_S35, &state);
	if ( state ) lcd[4] |= (uint8_t)(1 << 4);
	
	GetCtrlVal( panel, LCDAUTO_S34, &state);
	if ( state ) lcd[4] |= (uint8_t)(1 << 5);
	
	GetCtrlVal( panel, LCDAUTO_S33, &state);
	if ( state ) lcd[4] |= (uint8_t)(1 << 6);
	
	GetCtrlVal( panel, LCDAUTO_S30, &state);
	if ( state ) lcd[0] |= (uint8_t)(1 << 6);
	
	GetCtrlVal( panel, LCDAUTO_S31, &state);
	if ( state ) lcd[0] |= (uint8_t)(1 << 5);
	
	
	/* 3th digit */
	GetCtrlVal( panel, LCDAUTO_S36, &state);
	if ( state ) lcd[4] |= (uint8_t)(1 << 3);
	
	GetCtrlVal( panel, LCDAUTO_S38, &state);
	if ( state ) lcd[0] |= (uint8_t)(1 << 3);
	
	GetCtrlVal( panel, LCDAUTO_S39, &state);
	if ( state ) lcd[4] |= (uint8_t)(1 << 2);
	
	sprintf(command, "\\HWI\\LCD\\w %d %d %d %d %d %d %d %d", (int)lcd[0],
															  (int)lcd[1],
															  (int)lcd[2],
															  (int)lcd[3],
															  (int)lcd[4],
															  (int)lcd[5],
															  (int)lcd[6],
															  (int)lcd[7]);

	EXCCHECK( gs_pvline->Write(gs_pvline, "\\HWI\\LCD\\c 1") ); 
	EXCCHECK( gs_pvline->Write(gs_pvline, command) ); 

Error:
	EXCRETHROW( pexception);  
}

#undef __FUNC__
#define __FUNC__ "OnAutoDisplay"
int CVICALLBACK OnAutoDisplay (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int				value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			value = ( control == LCDAUTO_DISPLAYON ) ? 1 : 0;

			SetCtrlVal( panel, LCDAUTO_S1, value);
			SetCtrlVal( panel, LCDAUTO_S2, value);
			SetCtrlVal( panel, LCDAUTO_S3, value);
			SetCtrlVal( panel, LCDAUTO_S4, value);
			SetCtrlVal( panel, LCDAUTO_S5, value);
			SetCtrlVal( panel, LCDAUTO_S6, value);
			SetCtrlVal( panel, LCDAUTO_S7, value);
			SetCtrlVal( panel, LCDAUTO_S8, value);
			SetCtrlVal( panel, LCDAUTO_S9, value);
			SetCtrlVal( panel, LCDAUTO_S10, value);
			SetCtrlVal( panel, LCDAUTO_S11, value);
			SetCtrlVal( panel, LCDAUTO_S12, value);
			SetCtrlVal( panel, LCDAUTO_S13, value);
			SetCtrlVal( panel, LCDAUTO_S14, value);
			SetCtrlVal( panel, LCDAUTO_S15, value);
			SetCtrlVal( panel, LCDAUTO_S16, value);
			SetCtrlVal( panel, LCDAUTO_S17, value);
			SetCtrlVal( panel, LCDAUTO_S18, value);
			SetCtrlVal( panel, LCDAUTO_S19, value); 
			SetCtrlVal( panel, LCDAUTO_S20, value); 
			SetCtrlVal( panel, LCDAUTO_S21, value);
			SetCtrlVal( panel, LCDAUTO_S22, value);
			SetCtrlVal( panel, LCDAUTO_S23, value);
			SetCtrlVal( panel, LCDAUTO_S24, value);
			SetCtrlVal( panel, LCDAUTO_S25, value);
			SetCtrlVal( panel, LCDAUTO_S26, value);
			SetCtrlVal( panel, LCDAUTO_S27, value);
			SetCtrlVal( panel, LCDAUTO_S28, value);
			SetCtrlVal( panel, LCDAUTO_S29, value);
			SetCtrlVal( panel, LCDAUTO_S30, value);
			SetCtrlVal( panel, LCDAUTO_S31, value);
			SetCtrlVal( panel, LCDAUTO_S32, value);
			SetCtrlVal( panel, LCDAUTO_S33, value);
			SetCtrlVal( panel, LCDAUTO_S34, value);
			SetCtrlVal( panel, LCDAUTO_S35, value);
			SetCtrlVal( panel, LCDAUTO_S36, value);
			SetCtrlVal( panel, LCDAUTO_S37, value);
			SetCtrlVal( panel, LCDAUTO_S38, value);
			SetCtrlVal( panel, LCDAUTO_S39, value);
			SetCtrlVal( panel, LCDAUTO_S40, value);			
			SetCtrlVal( panel, LCDAUTO_S41, value);
			SetCtrlVal( panel, LCDAUTO_S42, value);
			
			SetCtrlVal( panel, LCDAUTO_FAN, value);
			SetCtrlVal( panel, LCDAUTO_FAN0, value);
			SetCtrlVal( panel, LCDAUTO_FAN1, value);
			SetCtrlVal( panel, LCDAUTO_FAN2, value);
			SetCtrlVal( panel, LCDAUTO_FAN3, value);
			SetCtrlVal( panel, LCDAUTO_FAN4, value);
			SetCtrlVal( panel, LCDAUTO_FAN5, value);
			SetCtrlVal( panel, LCDAUTO_FAN6, value);
			
			SetCtrlVal( panel, LCDAUTO_B1, value);
			SetCtrlVal( panel, LCDAUTO_B2, value);
			SetCtrlVal( panel, LCDAUTO_B3, value);
			
			SetCtrlVal( panel, LCDAUTO_DOT1, value);
			SetCtrlVal( panel, LCDAUTO_DOT2, value);
			
			SetCtrlVal( panel, LCDAUTO_DEFROST, value);
			SetCtrlVal( panel, LCDAUTO_RECY_DOWN, value);
			SetCtrlVal( panel, LCDAUTO_RECY_MID, value);
			SetCtrlVal( panel, LCDAUTO_MAN, value);
			SetCtrlVal( panel, LCDAUTO_HEAD, value);
			
			EXCCHECK( WriteAutoLCD( panel));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

#undef __FUNC__
#define __FUNC__ "OnManDisplay"
int CVICALLBACK OnManDisplay (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int				value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			value = ( control == LCDMAN_DISPLAYON ) ? 1 : 0;
			
			SetCtrlVal( panel, LCDMAN_FAN, value);
			SetCtrlVal( panel, LCDMAN_FAN0, value);
			SetCtrlVal( panel, LCDMAN_FAN1, value);
			SetCtrlVal( panel, LCDMAN_FAN2, value);
			SetCtrlVal( panel, LCDMAN_FAN3, value);
			SetCtrlVal( panel, LCDMAN_FAN4, value);
			SetCtrlVal( panel, LCDMAN_FAN5, value);
			SetCtrlVal( panel, LCDMAN_FAN6, value);
			
			SetCtrlVal( panel, LCDMAN_B1, value);
			SetCtrlVal( panel, LCDMAN_B2, value);
			SetCtrlVal( panel, LCDMAN_B3, value);
			
			SetCtrlVal( panel, LCDMAN_DEFROST, value);
			SetCtrlVal( panel, LCDMAN_RECY_DOWN, value);
			SetCtrlVal( panel, LCDMAN_RECY_MID, value);
			SetCtrlVal( panel, LCDMAN_MAN, value);
			SetCtrlVal( panel, LCDMAN_HEAD, value);

			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}
