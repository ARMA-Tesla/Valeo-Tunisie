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
	int     		product,
		            picto_1,
					picto_2,
					picto_3,
					picto_4,
					picto_5,
					picto_6,
					picto_7,
					picto_8;
	char			buffer[4096] = "";
	char    		messageSent[128];
	char    		messageReceived[128];
	const char 		sJob[32];
	int             phase;
	char            data[128];
	
	char *char_array= "BCDEFGH";
	
	const char 		job_1[32]="001";
	const char 		job_2[32]="002";
#if 0    /* formerly excluded lines */
	char *job_1 ="001";
	char *job_2 ="002";
#endif   /* formerly excluded lines */
	
	
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

#if 0    /* formerly excluded lines */
				
#endif   /* formerly excluded lines */
				EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
#if 0    /* formerly excluded lines */
				
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PRODUCT, &product));
				if(product==0)
				{
					EXCCHECK( pcamera->Cognex_Job(pcamera, job_1));
					SetCtrlAttribute (panel, CAMERA_PICTO_1, ATTR_DIMMED, 1);
					SetCtrlAttribute (panel, CAMERA_PICTO_7, ATTR_DIMMED, 1);
					SetCtrlAttribute (panel, CAMERA_PICTO_8, ATTR_DIMMED, 1);
					Sleep(1000);
					EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, 0));
				}
				if(product==1)
				{
					EXCCHECK( pcamera->Cognex_Job(pcamera, job_1));
					SetCtrlAttribute (panel, CAMERA_PICTO_1, ATTR_DIMMED, 0);
					SetCtrlAttribute (panel, CAMERA_PICTO_7, ATTR_DIMMED, 0);
					SetCtrlAttribute (panel, CAMERA_PICTO_8, ATTR_DIMMED, 1);
					Sleep(1000);
					EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, 1));
				}
				
				if(product==2)
				{
					EXCCHECK( pcamera->Cognex_Job(pcamera, job_2));
					SetCtrlAttribute (panel, CAMERA_PICTO_1, ATTR_DIMMED, 0);
					SetCtrlAttribute (panel, CAMERA_PICTO_7, ATTR_DIMMED, 0);
					SetCtrlAttribute (panel, CAMERA_PICTO_8, ATTR_DIMMED, 0);
				}
#endif   /* formerly excluded lines */
				
#if 0    /* formerly excluded lines */
				/* Select Picto 1*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_1, &picto_1));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "B", picto_1));
				
				/* Select Picto 2*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_2, &picto_2));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "C", picto_1));
				
				
				/* Select Picto 3*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_3, &picto_3));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "D", picto_3));
				
				
				/* Select Picto 4*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_4, &picto_4));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "E", picto_4));
				
				
				/* Select Picto 5*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_5, &picto_5));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "F", picto_5));
				
				
				/* Select Picto 6*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_6, &picto_6));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "G", picto_6));
				
				
				/* Select Picto 7*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_7, &picto_7));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "H", picto_7));
				
				
				/* Select Picto 8*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_8, &picto_8));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "I", picto_8));
#endif   /* formerly excluded lines */
				
				

				

				
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
						sprintf (messageReceived, "Data Camera = [%s]\r", data);
						EXCCHECKCVI( SetCtrlVal(panel, CAMERA_RECEIVE, messageReceived));
						break;
						
						case CAMERA_COGNEX_ONLINE :
						EXCCHECK( pcamera->Cognex_Online(pcamera));
						break;
						
						case CAMERA_COGNEX_OFFLINE :
						EXCCHECK( pcamera->Cognex_Offline(pcamera));
						break;
						
						case CAMERA_COGNEX_SELECT_PICTO :
				
							EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
							EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PRODUCT, &product)); 
				if(product==0 || product==1)			
							
				{
					/* Select Picto 1*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_1, &picto_1));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "B", picto_1));
				
				/* Select Picto 2*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_2, &picto_2));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "C", picto_1));
				
				
				/* Select Picto 3*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_3, &picto_3));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "D", picto_3));
				
				
				/* Select Picto 4*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_4, &picto_4));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "E", picto_4));
				
				
				/* Select Picto 5*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_5, &picto_5));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "F", picto_5));
				
				
				/* Select Picto 6*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_6, &picto_6));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "G", picto_6));
				
				
				/* Select Picto 7*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_7, &picto_7));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "H", picto_7));
				
				
				/* Select Picto 8*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_8, &picto_8));
				EXCCHECK (pcamera->Cognex_set_parameters(pcamera, "I", picto_8)); }
				
				else
				{
							/* Select Picto 1*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_1, &picto_1));
				EXCCHECK (pcamera->Cognex_set_parameters_VS11(pcamera, "B", picto_1));
				
				/* Select Picto 2*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_2, &picto_2));
				EXCCHECK (pcamera->Cognex_set_parameters_VS11(pcamera, "C", picto_1));
				
				
				/* Select Picto 3*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_3, &picto_3));
				EXCCHECK (pcamera->Cognex_set_parameters_VS11(pcamera, "D", picto_3));
				
				
				/* Select Picto 4*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_4, &picto_4));
				EXCCHECK (pcamera->Cognex_set_parameters_VS11(pcamera, "E", picto_4));
				
				
				/* Select Picto 5*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_5, &picto_5));
				EXCCHECK (pcamera->Cognex_set_parameters_VS11(pcamera, "F", picto_5));
				
				
				/* Select Picto 6*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_6, &picto_6));
				EXCCHECK (pcamera->Cognex_set_parameters_VS11(pcamera, "G", picto_6));
				
				
				/* Select Picto 7*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_7, &picto_7));
				EXCCHECK (pcamera->Cognex_set_parameters_VS11(pcamera, "H", picto_7));
				
				
				/* Select Picto 8*/
				EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PICTO_8, &picto_8));
				EXCCHECK (pcamera->Cognex_set_parameters_VS11(pcamera, "I", picto_8));
				}
						break;

						
						case CAMERA_COGNEX_SET_PARAM :
						GetCtrlVal(panel, CAMERA_NUMERIC, &phase);
						EXCCHECK( pcamera->Cognex_set_param(pcamera, phase));
						break;
						
						case CAMERA_CLEAR:
						DeleteTextBoxLines (panel, CAMERA_SEND, 0, -1);
						DeleteTextBoxLines (panel, CAMERA_RECEIVE, 0, -1);
						break;
						
						
				case CAMERA_COGNEX_SELECT_PRODUCT :		
					EXCCHECKCVI( GetCtrlVal (panel, CAMERA_PRODUCT, &product));
				if(product==0)
				{
					printf("case product =0\r\n");
					EXCCHECK( pcamera->Cognex_Job(pcamera, job_1));
					printf("job_1=%s\r\n",job_1);
					SetCtrlAttribute (panel, CAMERA_PICTO_1, ATTR_DIMMED, 1);
					SetCtrlAttribute (panel, CAMERA_PICTO_7, ATTR_DIMMED, 1);
					SetCtrlAttribute (panel, CAMERA_PICTO_8, ATTR_DIMMED, 1);
					Sleep(1000);
					EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, 0));
				}
				if(product==1)
				{
					printf("case product =1\r\n");
					EXCCHECK( pcamera->Cognex_Job(pcamera, job_1));
					printf("job_1=%s\r\n",job_1);
					SetCtrlAttribute (panel, CAMERA_PICTO_1, ATTR_DIMMED, 0);
					SetCtrlAttribute (panel, CAMERA_PICTO_7, ATTR_DIMMED, 0);
					SetCtrlAttribute (panel, CAMERA_PICTO_8, ATTR_DIMMED, 1);
					Sleep(1000);
					EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, 1));
				}
				
				if(product==2)
				{
					printf("case product =2\r\n");
					EXCCHECK( pcamera->Cognex_Job(pcamera, job_2));
					printf("job_2=%s\r\n",job_2);
					SetCtrlAttribute (panel, CAMERA_PICTO_1, ATTR_DIMMED, 0);
					SetCtrlAttribute (panel, CAMERA_PICTO_7, ATTR_DIMMED, 0);
					SetCtrlAttribute (panel, CAMERA_PICTO_8, ATTR_DIMMED, 0);
				}
				
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
