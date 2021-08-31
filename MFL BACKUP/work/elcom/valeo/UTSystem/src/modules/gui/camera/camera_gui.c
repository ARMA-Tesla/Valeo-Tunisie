#include <exception/exception.h>
#include <Camera/class/cameras.h>
#include <eventfactory/EventFactory.h>
#include "camera_gui_plugin.h"
#include "toolbox.h"
#include <ansi_c.h>
#include "camera_gui.h"

#define NB_OF_PANELS		1 
#define NAME_LENGTH			128

#define TIMEOUT_CAMERA 6000

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

	EXCCHECKCVI( gs_panel[CAMERA] = LoadPanelEx(main_panel,"camera_gui.uir", CAMERA, __CVIUserHInst) );

	EXCCHECKCVI( SetPanelAttribute (gs_panel[CAMERA], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[CAMERA], ATTR_LEFT, 0));

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
	 
	if( 0==strcmp( uuid, PLUGIN_CAMERA))
	{
		EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        			"Modules",
	                        			sizeof(SObjListPtr),
	                        			&gs_Modules));
		
		EXCCHECKCVI( DisplayPanel(gs_panel[CAMERA]));
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

	EXCCHECKCVI( HidePanel(gs_panel[CAMERA]));

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

	if(gs_panel[CAMERA])
	EXCCHECKCVI( DiscardPanel( gs_panel[CAMERA]));

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

int CVICALLBACK OnCamera (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int     		camera;
	char			buffer[4096] = "";
	char    		messageSent[128];
	char    		messageReceived[128];
	const char 		sJob[32];
	int             phase;
	int             live;
	char            data[256];
	clock_t _start_test_clock;
	double   time;
	
	switch (event)
	{
	case EVENT_COMMIT:
		
		if(gs_Modules)
		{
			SCamerasPtr	pcameras = (SCamerasPtr)gs_Modules->Get(gs_Modules, MODULE_CAMERA);  
			SCameraPtr		pcamera = NULL;
			
			if ( pcameras )
			{
				char*	pend = buffer;

				EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
				
		 			switch (control)
					{
						case CAMERA_OPEN_TCP_SERVER :
						EXCCHECKCVI( SetCtrlAttribute(panel, CAMERA_TCP_SERVER_LED, ATTR_FRAME_COLOR, VAL_WHITE));
						EXCCHECK( pcamera->openTCPserver(pcamera));
						EXCCHECKCVI( SetCtrlAttribute(panel, CAMERA_TCP_SERVER_LED, ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case CAMERA_COGNEX_LOGIN :
						EXCCHECKCVI( SetCtrlAttribute(panel, CAMERA_COGNEX_LOGIN_LED, ATTR_FRAME_COLOR, VAL_WHITE));
						EXCCHECK( pcamera->Cognex_login(pcamera));
						EXCCHECKCVI( SetCtrlAttribute(panel, CAMERA_COGNEX_LOGIN_LED, ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
						ProcessDrawEvents ();
						break;
						
						case CAMERA_COGNEX_DISCONNECT :
						EXCCHECK( pcamera->Cognex_disconnect(pcamera));
						break;
						
						case CAMERA_COGNEX_TRIGGER :
						EXCCHECK( pcamera->Cognex_Trigger(pcamera));
						break;
						
						case CAMERA_COGNEX_READ :
						EXCCHECK( pcamera->Cognex_Read(pcamera, data));
						sprintf (messageReceived, "Data Camera = [%s]\r\n", data);
						EXCCHECKCVI( SetCtrlVal(panel, CAMERA_RECEIVE, messageReceived));
						break;
						
						case CAMERA_COGNEX_ONLINE :
						EXCCHECK( pcamera->Cognex_Online(pcamera));
						break;
						
						case CAMERA_COGNEX_OFFLINE :
						EXCCHECK( pcamera->Cognex_Offline(pcamera));
						break;
						
						case CAMERA_COGNEX_JOB :
						GetCtrlVal(panel, CAMERA_STRING, sJob);
						EXCCHECK( pcamera->Cognex_Job(pcamera, sJob));
						break;
						
						case CAMERA_COGNEX_SET_PARAM :
						GetCtrlVal(panel, CAMERA_NUMERIC, &phase);
						EXCCHECK( pcamera->Cognex_set_param(pcamera, phase));
						break;
						
						case CAMERA_NUMERIC :
						GetCtrlVal(panel, CAMERA_NUMERIC, &phase);
						EXCCHECK( pcamera->Cognex_set_param(pcamera, phase));
						break;
						
						case CAMERA_COGNEX_SET_LIVE :
						GetCtrlVal(panel, CAMERA_LIVE, &live);
						EXCCHECK( pcamera->Cognex_set_live(pcamera, live));
						break;
						
						case CAMERA_CLEAR:
						DeleteTextBoxLines (panel, CAMERA_SEND, 0, -1);
						DeleteTextBoxLines (panel, CAMERA_RECEIVE, 0, -1);
						break;
						
						 						
						case CAMERA_COGNEX_TEST:
						GetCtrlVal(panel, CAMERA_NUMERIC, &phase);
						
						EXCCHECK( pcamera->Cognex_set_param(pcamera, phase));
						Sleep(200);
						EXCCHECK( pcamera->Cognex_disconnect(pcamera));
						//Sleep(800);
						EXCCHECK( pcamera->openTCPserver(pcamera));
						//Sleep(800);
						EXCCHECK( pcamera->Cognex_login(pcamera));
						//Sleep(800);
						EXCCHECK( pcamera->Cognex_Trigger(pcamera));
						Sleep(700);
						
						
						EXCCHECK( pcamera->Cognex_Read(pcamera, data));
						if (data[17] == 48+phase)
							sprintf (messageReceived, "Data Camera = [%s]\r\n", data);
						else   
							sprintf (messageReceived, "Timeout => Data Camera = [%s]\r\n", data);

						EXCCHECKCVI( SetCtrlVal(panel, CAMERA_RECEIVE, messageReceived));
	 

						ProcessSystemEvents ();
						ProcessDrawEvents ();
						EXCCHECK( pcamera->Cognex_Read(pcamera, data));
						if (data[17] == 48+phase)
							sprintf (messageReceived, "Data Camera = [%s]\r\n", data);
						else   
							sprintf (messageReceived, "Timeout => Data Camera = [%s]\r\n", data);

						EXCCHECKCVI( SetCtrlVal(panel, CAMERA_RECEIVE, messageReceived));
						
						
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
