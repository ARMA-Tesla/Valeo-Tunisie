#include <vline/vline.h>
#include <eventfactory/EventFactory.h> 
#include <utility.h>
#include <guid.h>
#include <test/testseq.h>
#include <jig/class/jig_class.h>
#include "panel_x95_caman_plugin.h" 
//#include <ansi_c.h>
#include "lang_translation.h"
#include "panel_x95_common.h"
#include <IviBridge/ivibridge.h>
#include <resistor/class/Resistor.h>
#include <eldaq/class/eldaq.h>
#include <IviDcPwr.h>
#include <IviSwtch.h>

#define NUMBER_OF_PANELS 	4
#define MODULE_JIG			MODULE_JIG_X95_CAMAN /* guid.h */
#define COMMAND_LENGHT		1024
#define SUBPANEL_LEFT 		0
#define SUBPANEL_TOP		467

static int					gs_panel[NUMBER_OF_PANELS+1];
static SVLinePtr			gs_pvline; 
static SJigPtr				gs_pjig = NULL;
SLanguagePtr 				gs_pLanguage;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL;  
static SPluginArgsPtr		gs_PluginArgs = NULL;
static STestSequencePtr		gs_TestSequence = NULL;


ViSession	gs_IviSessions[X95_IVI_SESSION_COUNT];

static enum _EMode
{
	EM_IO = 0,
	EM_MEMORY,
	EM_VLINE
} EMode;

typedef enum _EPanelSwtch
{
	SWTCH_CONNECT = 0,
	SWTCH_DISCONNECT,
} EPanelSwtch;

static SElExceptionPtr DisplayCache(int panel, SVLinePtr pVLine);
void CVICALLBACK PostDeferredDisplayPanel (void *callbackData);
void CVICALLBACK PostDeferredHidePanel (void *callbackData);
SElExceptionPtr InitializeL38( void);

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

			switch(control)
			{
				case PANEL_CONNECT:
					if(value)
						EXCCHECK( PanelSwtch(SWTCH_CONNECT));
					else
						EXCCHECK( PanelSwtch(SWTCH_DISCONNECT));
					break;
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;   
}

int CVICALLBACK OnL38Connect (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;

	switch (event)
	{
		case EVENT_COMMIT:
			if ( control == L38_L38_DISCONNECT )
			{
				EXCCHECKCVI( SetCtrlAttribute (panel, L38_TIMER, ATTR_ENABLED, 0));
				EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[L38])); 
				checkErr( eldaq_StopMeasurement( gs_IviSessions[DAQ]));
			}
			else
			{
				if ( gs_PluginArgs )
				{
					SIviBridgePtr  pIvi = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
					EXCCHECK( pIvi->GetSession( pIvi, "SW4", &gs_IviSessions[SW4]));
					EXCCHECK( pIvi->GetSession( pIvi, "SW5", &gs_IviSessions[SW5]));
					EXCCHECK( pIvi->GetSession( pIvi, "DAQ", &gs_IviSessions[DAQ]));
					EXCCHECK( pIvi->GetSession( pIvi, "DMM", &gs_IviSessions[DMM]));
					EXCCHECK( pIvi->GetSession( pIvi, "PSW", &gs_IviSessions[PSW]));
					EXCCHECK( pIvi->GetSession( pIvi, "POWER", &gs_IviSessions[POWER]));
					EXCCHECK( pIvi->GetSession( pIvi, "RESISTOR", &gs_IviSessions[RESISTOR]));
					Resistor_ConfigureOutputResistance( gs_IviSessions[RESISTOR], "4", 7400 );
					Resistor_ConfigureOutputResistance( gs_IviSessions[RESISTOR], "5", 50 );
					checkErr( eldaq_ConfigureMeasurement (gs_IviSessions[DAQ], "", 2000, 0.02));
					checkErr( eldaq_StartMeasurement( gs_IviSessions[DAQ]));
					EXCCHECK( InitializeL38( ));
					EXCCHECKCVI( SetCtrlAttribute (gs_panel[L38], L38_TIMER, ATTR_ENABLED, 1));
				}
				EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[L38])); 
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
				case PANEL_PWM_LED:
				case PANEL_LED_DUTY:
				case PANEL_LED_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_PWM_LED, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_LED_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_LED_FREQ, &freq));
					id = 0;
					break;
				case PANEL_PWM_KEYBOARD:
				case PANEL_KEYBOARD_DUTY:
				case PANEL_KEYBOARD_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_PWM_KEYBOARD, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_KEYBOARD_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_KEYBOARD_FREQ, &freq));
					id = 1;
					break;
				case PANEL_PWM_BLOWER:
				case PANEL_BLOWER_DUTY:
				case PANEL_BLOWER_FREQ:
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_PWM_BLOWER, &status));
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_BLOWER_DUTY, &duty));
					EXCCHECKCVI( GetCtrlVal(panel, PANEL_BLOWER_FREQ, &freq));
					id = 2;
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
			
			for(id=0; id<3; id++)
			{
				sprintf(command, "\\HWI\\AIO\\i16ar %d", id);
				EXCCHECK( gs_pvline->Query( gs_pvline, command, read, COMMAND_LENGHT));
				sscanf(read, "%x", &value);
			
				if(id==0)
					EXCCHECKCVI( SetCtrlVal(panel, PANEL_SEN_1, value));
				if(id==1)
					EXCCHECKCVI( SetCtrlVal(panel, PANEL_SEN_2, value));
				if(id==2)
					EXCCHECKCVI( SetCtrlVal(panel, PANEL_SEN_3, value));
			}
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

#define NUMBER_OF_LED	3
SLed gs_Led[NUMBER_OF_LED] =
{
	{PANEL_LED_RECY, 2, TRUE},
	{PANEL_LED_AC, 3, TRUE},
	{PANEL_LED_HRW, 5, TRUE}
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
				sprintf(command, "\\HWI\\DIO\\o1w %d %d", gs_Led[i].led_id, 
								(status)? gs_Led[i].state_on:!gs_Led[i].state_on);   	
				EXCCHECK( gs_pvline->Write(gs_pvline, command)); 
			}
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
	uint32_t  		data_lenght = 1024;
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
#define __FUNC__ "ShowPanel"
static SElExceptionPtr ShowPanel(void)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;

	EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, &gs_panel[PANEL])); 
		
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
	
	EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, &gs_panel[PANEL]));   
	
Error:      
	EXCRETHROW( pexception);
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
#define __FUNC__ "EventFunctionCallback"
static SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	SElExceptionPtr  pexception = NULL;
   
	if(0==strcmp(eventName, "EVNT_JIG_LOADED"))
	{
		if(pData && 0==strcmp(((SElEventDataPtr)pData)->GetStringPtr(pData), MODULE_JIG))
			EXCCHECK(ShowPanel());
	}
	else if(0==strcmp(eventName, "EVNT_JIG_UNLOADED"))
	{
		if(pData && 0==strcmp(((SElEventDataPtr)pData)->GetStringPtr(pData), MODULE_JIG))
			EXCCHECK(ClosePanel());
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
} /* RegisterEventFunction */

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
#if (!defined(_CVI_)&&defined(_DEBUG))
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
#define __FUNC__ "{Panel X95 caman Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr  	pexception = NULL;
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

	EXCCHECK( LoadPanelExWithLanguage(main_panel, "panel_x95_caman.uir", PANEL, PANELlang, &gs_panel[PANEL])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[PANEL], "panel_x95_caman.uir", MEMORY, MEMORYlang, &gs_panel[MEMORY])); 
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[PANEL], "panel_x95_caman.uir", VLINE, VLINElang, &gs_panel[VLINE]));
	EXCCHECK( LoadPanelExWithLanguage(gs_panel[PANEL], "panel_x95_caman.uir", L38, L38lang, &gs_panel[L38])); 
	   
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MEMORY], ATTR_TOP, SUBPANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MEMORY], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[VLINE], ATTR_TOP, SUBPANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[VLINE], ATTR_LEFT, SUBPANEL_LEFT));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[L38], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[L38], ATTR_LEFT, 0));

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
#define __FUNC__ "{Panel X95 caman Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel X95 caman Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
   
Error:   
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Panel X95 caman Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr   pexception = NULL;
   	int32_t         error = 0;

	if(gs_panel[PANEL])
		EXCCHECKCVI( DiscardPanel(gs_panel[PANEL]));

Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel X95 Caman Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel X95 Caman Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif 

SElExceptionPtr InitializeL38( void)
{
	SElExceptionPtr   pexception = NULL;
	ViStatus		error;
	ViInt32			path_state;

	checkErr( IviDCPwr_ConfigureOutputRange( gs_IviSessions[POWER], "1", IVIDCPWR_VAL_RANGE_VOLTAGE, 15));
	checkErr( IviDCPwr_ConfigureCurrentLimit( gs_IviSessions[POWER], "1", IVIDCPWR_VAL_CURRENT_REGULATE, 0.5));
	checkErr( IviDCPwr_ConfigureVoltageLevel( gs_IviSessions[POWER], "1", 13.5));
	checkErr( IviDCPwr_ConfigureOutputEnabled( gs_IviSessions[POWER], "1", VI_TRUE));

	checkErr( IviSwtch_CanConnect( gs_IviSessions[PSW], "CH1A", "CH1B", &path_state));
	SetCtrlVal( gs_panel[L38], L38_K3, (path_state == IVISWTCH_VAL_PATH_EXISTS));

	checkErr( IviSwtch_CanConnect( gs_IviSessions[SW4], "ch40", "com40", &path_state));
	SetCtrlVal( gs_panel[L38], L38_K15, (path_state == IVISWTCH_VAL_PATH_EXISTS));
	SetCtrlVal( gs_panel[L38], L38_K16, (path_state == IVISWTCH_VAL_PATH_EXISTS));
	SetCtrlVal( gs_panel[L38], L38_K18, (path_state == IVISWTCH_VAL_PATH_EXISTS));
	checkErr( IviSwtch_CanConnect( gs_IviSessions[SW4], "ch50", "com50", &path_state));
	SetCtrlVal( gs_panel[L38], L38_K10, (path_state == IVISWTCH_VAL_PATH_EXISTS));
	checkErr( IviSwtch_CanConnect( gs_IviSessions[SW4], "ch70", "com70", &path_state));
	SetCtrlVal( gs_panel[L38], L38_K11, (path_state == IVISWTCH_VAL_PATH_EXISTS));
	checkErr( IviSwtch_CanConnect( gs_IviSessions[SW4], "ch60", "com60", &path_state));
	SetCtrlVal( gs_panel[L38], L38_K13, (path_state == IVISWTCH_VAL_PATH_EXISTS));
	checkErr( IviSwtch_CanConnect( gs_IviSessions[SW4], "ch80", "com80", &path_state));
	SetCtrlVal( gs_panel[L38], L38_K17, (path_state == IVISWTCH_VAL_PATH_EXISTS));
	checkErr( IviSwtch_CanConnect( gs_IviSessions[SW4], "ch28", "com28", &path_state));
	SetCtrlVal( gs_panel[L38], L38_K12, (path_state == IVISWTCH_VAL_PATH_EXISTS));
	checkErr( IviSwtch_CanConnect( gs_IviSessions[PSW], "CH2A", "CH2B", &path_state));
	SetCtrlVal( gs_panel[L38], L38_K14, (path_state == IVISWTCH_VAL_PATH_EXISTS));


Error:
	 EXCRETHROW( pexception);
}