#include <userint.h>
#include <exception/exception.h>
#include <axis/class/axis.h>
#include <labels/tester_labels.h>
#include <eventfactory/EventFactory.h>
#include "axis_gui_plugin.h"
#include "toolbox.h"
#include <ansi_c.h>
#include "axis_gui.h"

#define NB_OF_PANELS		1 
#define NAME_LENGTH			128

#define TIMEOUT_AXIS 6000

/*GLOBALS*/
static SObjListPtr     		gs_Modules = NULL;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static int					gs_panel[NB_OF_PANELS+1];
static SPluginInfoPtr		gs_pPI = NULL; 

#if defined(_CVI_DLL_) || \
	defined(_WINDLL)
/*---------------------------------------------------------------------------*/
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
			//_CrtSetBreakAlloc(61);
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
 /*---------------------------------------------------------------------------*/
int __stdcall DllEntryPoint(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved
)
{
   /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Axis Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr pexception = NULL;
	int32_t         error = 0;
	int      		main_panel;

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
			                        "CardPanel",
			                        sizeof(main_panel),
			                        &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));

	EXCCHECKCVI( gs_panel[AXIS] = LoadPanelEx(main_panel,"axis_gui.uir", AXIS, __CVIUserHInst) );

	EXCCHECKCVI( SetPanelAttribute (gs_panel[AXIS], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[AXIS], ATTR_LEFT, 0));

Error:
   	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Axis Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE(SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	 
	if( 0==strcmp( uuid, PLUGIN_AXIS))
	{
		EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        			"Modules",
	                        			sizeof(SObjListPtr),
	                        			&gs_Modules));
		
		EXCCHECKCVI( DisplayPanel(gs_panel[AXIS]));
	}

Error:   
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Axis Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( HidePanel(gs_panel[AXIS]));

Error:   
	EXCRETHROW( pexception);

}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Axis Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if(gs_panel[AXIS])
	EXCCHECKCVI( DiscardPanel( gs_panel[AXIS]));

Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Axis Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
   	SElExceptionPtr	pexception = NULL;  
	
	if (a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Axis Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif

int CVICALLBACK OnAxis (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	char	buffer[4096] = "";
	char	text[4096] = "";
	int		station,axis,position;
	double  target_position, speed;
	int     statusAlarm;
	int     status;
	char    messageSent[128];
	char    messageReceived[128];
	int32_t				error = 0;
	double				delay = 0;
	double               Currentposition = 0.0;
	double               Currentanalog = 0.0;
	int i;
	clock_t _start_test_clock;
	double   time;
	bool_t bit;
	
	
	switch (event)
	{
		case EVENT_COMMIT:
		if(gs_Modules)
		{
			SaxissPtr	paxiss = (SaxissPtr)gs_Modules->Get(gs_Modules, MODULE_AXIS);  
			SaxisPtr		paxis = NULL;
			
			if ( paxiss )
			{
				char*	pend = buffer;

				EXCCHECKCVI( GetCtrlVal (panel, AXIS_STATION, &station));
				EXCCHECK( paxiss->GetaxisByIndex(paxiss, station, &paxis));

				pend = strchr( pend, '\n');
				while ( pend != NULL )
				{	
					if ( pend > buffer && *(pend-1)!='\r' )
					{
						memmove(pend+1, pend, strlen(pend)+1);
						*pend = '\r';
						pend += 2;
					}
					else if(pend<buffer+sizeof(buffer))
					{
						pend++;
					}
					pend = strchr( pend, '\n');
				};
				
		 			switch (control)
					{
						case AXIS_ENABLE:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECK( paxis->EnableAxis(paxis, axis));
						EXCCHECKCVI( sprintf(messageSent, "Enable axis %d\r", axis));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						break;
						
						case AXIS_READ_ALARM:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECKCVI( sprintf(messageSent, "Read Alarm Code axis %d\r", axis));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						EXCCHECK( paxis->PresentAlarm(paxis, axis, &statusAlarm));
						EXCCHECKCVI( sprintf(messageReceived, "Alarm Code axis %d : (0X%04X)h\r", axis, statusAlarm));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_RECEIVE, messageReceived));
						break;
	
						case AXIS_RESET_ALARM:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECKCVI( sprintf(messageSent, "Reset Alarm axis %d\r", axis));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						EXCCHECK( paxis->AlarmReset(paxis, axis));
						break;
						
						case AXIS_HOME: 
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECKCVI( sprintf(messageSent, "Home Return axis %d\r", axis));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						EXCCHECK( paxis->HomeReturn(paxis, axis));
						EXCCHECKCVI( SetCtrlAttribute(panel, AXIS_HOME_LED, ATTR_FRAME_COLOR, VAL_GREEN));
						break;
						
						case AXIS_STOP: 
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECKCVI( sprintf(messageSent, "Stop axis %d\r", axis));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						EXCCHECK( paxis->StopAxis(paxis, axis));
						break;
						
						case AXIS_SERVO_ON:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_SERVO_ON, &status));
						
						if (status == 1)
						EXCCHECKCVI( sprintf(messageSent, "SERVO ON axis %d\r", axis));
						if (status == 0)
						EXCCHECKCVI( sprintf(messageSent, "SERVO OFF axis %d\r", axis));
						
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						EXCCHECK( paxis->ControllerStatus(paxis, axis, status));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
								
						case AXIS_SET_POSITION:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_TARGET_POSITION, &target_position));
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_SPEED, &speed));
						if (station == 4 )
						   axis = 32;
						else if  (station == 5)
							 axis = 31;
						EXCCHECKCVI( sprintf(messageSent, "Move axis %d to position %f with speed %f\r", axis, target_position, speed));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						EXCCHECK( paxis->SetPosition(paxis, axis, target_position, speed));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case AXIS_READ_POSITION:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECKCVI( sprintf(messageSent, "Read axis %d current position\r", axis));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						paxis->CurrentPosition(paxis, axis, &Currentposition);
						EXCCHECKCVI( sprintf(messageReceived, "Current Position axis %d : (%lf mm)\r", axis, Currentposition));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_RECEIVE, messageReceived));
						break;
						
						case AXIS_READ_ANALOG:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						EXCCHECKCVI( sprintf(messageSent, "Read axis %d current analog\r", axis));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_SEND, messageSent));
						paxis->CurrentAnalog(paxis, axis, &Currentanalog, 1, 0);
						EXCCHECKCVI( sprintf(messageReceived, "Current Currentanalog axis %d : (%lf N)\r", axis, Currentanalog));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_RECEIVE, messageReceived));
						break;
						
						case AXIS_INDEX:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						paxis->CurrentPosition(paxis, axis, &Currentposition);
						EXCCHECKCVI( SetCtrlVal (panel, AXIS_TARGET_POSITION, Currentposition));
						EXCCHECKCVI( SetCtrlVal(panel, AXIS_RECEIVE, messageReceived));
						break;
				
						case AXIS_DELETE:
						EXCCHECKCVI( GetCtrlVal (panel, AXIS_INDEX, &axis));
						if (axis == 5)
						EXCCHECK( paxiss->GetaxisByIndex(paxiss, 1, &paxis));
						DeleteTextBoxLines (panel, AXIS_SEND, 0, -1);
						DeleteTextBoxLines (panel, AXIS_RECEIVE, 0, -1);
						break;
						
					}
			}
		}			
		break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}

int CVICALLBACK Cb_Station (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	int		station;

	switch (event)
	{
		case EVENT_COMMIT:

			
				GetCtrlVal (panel, AXIS_STATION, &station);
				
				if (station == 4 || station == 5)
				{
					SetCtrlAttribute (panel, AXIS_READ_ANALOG, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panel, AXIS_INDEX, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panel, AXIS_RESET_ALARM, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panel, AXIS_READ_ALARM, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panel, AXIS_STOP, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panel, AXIS_SERVO_ON, ATTR_VISIBLE, 0);
				}
				else 
				{					
					SetCtrlAttribute (panel, AXIS_READ_ANALOG, ATTR_VISIBLE, 0);
					SetCtrlAttribute (panel, AXIS_INDEX, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panel, AXIS_RESET_ALARM, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panel, AXIS_READ_ALARM, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panel, AXIS_STOP, ATTR_VISIBLE, 1);
					SetCtrlAttribute (panel, AXIS_SERVO_ON, ATTR_VISIBLE, 1);
				}
		
		break;
	}
	return 0;
}
