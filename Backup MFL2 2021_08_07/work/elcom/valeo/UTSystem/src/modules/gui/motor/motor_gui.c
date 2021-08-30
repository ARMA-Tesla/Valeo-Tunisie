#include <exception/exception.h>
#include <motor/class/motor.h>
#include <labels/tester_labels.h>
#include <eventfactory/EventFactory.h>
#include "motor_gui_plugin.h"
#include "toolbox.h"
#include <ansi_c.h>
#include "motor_gui.h"

#define NB_OF_PANELS		1 
#define NAME_LENGTH			128

#define TIMEOUT_MOTOR 6000

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
#define __FUNC__ "{Motor Plugin}::Initialize"
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

	EXCCHECKCVI( gs_panel[MOTOR] = LoadPanelEx(main_panel,"motor_gui.uir", MOTOR, __CVIUserHInst) );

	EXCCHECKCVI( SetPanelAttribute (gs_panel[MOTOR], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MOTOR], ATTR_LEFT, 0));

Error:
   	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Motor Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE(SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	 
	if( 0==strcmp( uuid, PLUGIN_MOTOR))
	{
		EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        			"Modules",
	                        			sizeof(SObjListPtr),
	                        			&gs_Modules));
		
		EXCCHECKCVI( DisplayPanel(gs_panel[MOTOR]));
	}

Error:   
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Motor Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( HidePanel(gs_panel[MOTOR]));

Error:   
	EXCRETHROW( pexception);

}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Motor Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if(gs_panel[MOTOR])
	EXCCHECKCVI( DiscardPanel( gs_panel[MOTOR]));

Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Motor Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
   	SElExceptionPtr	pexception = NULL;  
	
	if (a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Motor Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}


int CVICALLBACK OnMotor (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int     index;
	char	buffer[4096] = "";
	char	text[4096] = "";
	double	angle,speed;
	int32_t	reset = 0;
	double	Currentangle;
	char    messageSent[128];
	char    messageReceived[128];
	int32_t				error = 0;
	double				delay = 0;
	int					status;
	
	switch (event)
	{
		case EVENT_COMMIT:
		if(gs_Modules)
		{
			SmotorsPtr	pmotors = (SmotorsPtr)gs_Modules->Get(gs_Modules, MODULE_MOTOR);  
			SmotorPtr		pmotor = NULL;
			
			if ( pmotors )
			{
				char*	pend = buffer;

				EXCCHECKCVI( GetCtrlVal (panel, MOTOR_INDEX, &index));
				EXCCHECK( pmotors->GetmotorByIndex(pmotors, index, &pmotor));
				
		 			switch (control)
					{
						case MOTOR_INIT:
						EXCCHECKCVI( sprintf(messageSent, "Initialisation motor %d\r\n", index));
						EXCCHECKCVI( SetCtrlVal(panel, MOTOR_SEND, messageSent));
						EXCCHECK( pmotor->_InitializeSpec(pmotor));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case MOTOR_RESET_ANGLE:
						EXCCHECKCVI( sprintf(messageSent, "Reset motor %d to Angle = 0°\r\n", index));
						EXCCHECKCVI( SetCtrlVal(panel, MOTOR_SEND, messageSent));
						EXCCHECK( pmotor->AlarmReset(pmotor, index));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case MOTOR_RESET_SENSOR:
						EXCCHECKCVI( sprintf(messageSent, "Reset sensor motor %d\r\n", index));
						EXCCHECKCVI( SetCtrlVal(panel, MOTOR_SEND, messageSent));
						EXCCHECK( pmotor->ControllerStatus(pmotor, index, &status));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case MOTOR_ROTATE:
						EXCCHECKCVI( GetCtrlVal (panel, MOTOR_RESET, &reset));
						EXCCHECKCVI( GetCtrlVal (panel, MOTOR_ANGLE, &angle));
						EXCCHECKCVI( GetCtrlVal (panel, MOTOR_SPEED, &speed));
						EXCCHECKCVI( sprintf(messageSent, "Rotate motor %d to angle = %f° speed = %f\r\n", index, angle, speed));
						EXCCHECKCVI( SetCtrlVal(panel, MOTOR_SEND, messageSent));
						EXCCHECK( pmotor->MoveMotorPosition(pmotor, reset, angle, speed));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case MOTOR_STOP:
						EXCCHECK( pmotor->EnableMotor(pmotor, index));
						EXCCHECKCVI( sprintf(messageReceived, "Stop motor %d\r\n", index));
						EXCCHECKCVI( SetCtrlVal(panel, MOTOR_RECEIVE, messageReceived));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
				
						case MOTOR_READ_ANGLE:
						int i;
						for(i=0; i<150; i++)
						{
						EXCCHECK( pmotor->CurrentPosition(pmotor, index, &Currentangle));
						EXCCHECKCVI( sprintf(messageReceived, "Current angle of motor %d = %f\r\n", index, Currentangle));
						EXCCHECKCVI( SetCtrlVal(panel, MOTOR_RECEIVE, messageReceived));
						}
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case MOTOR_REQUEST_ANGLE:
						EXCCHECK( pmotor->RequestAngle(pmotor, index));
						EXCCHECKCVI( sprintf(messageReceived, "Request angle of motor %d\r\n", index));
						EXCCHECKCVI( SetCtrlVal(panel, MOTOR_RECEIVE, messageReceived));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case MOTOR_GET_ANGLE:
						EXCCHECK( pmotor->ReadAngle(pmotor, index, &Currentangle));
						EXCCHECKCVI( sprintf(messageReceived, "Requested angle of motor %d = %f\r\n", index, Currentangle));
						EXCCHECKCVI( SetCtrlVal(panel, MOTOR_RECEIVE, messageReceived));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case MOTOR_CLEAR_LOG:
						DeleteTextBoxLines (panel, MOTOR_SEND, 0, -1);
						DeleteTextBoxLines (panel, MOTOR_RECEIVE, 0, -1);
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
#endif
