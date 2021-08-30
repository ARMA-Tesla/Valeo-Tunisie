#include <exception/exception.h>
#include <laser/class/lasers.h>
#include <labels/tester_labels.h>
#include <eventfactory/EventFactory.h>
#include "laser_gui_plugin.h"
#include "laser_gui.h"

#define NB_OF_PANELS		1 
#define NAME_LENGTH			128

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
#define __FUNC__ "{Laser Plugin}::Initialize"
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

	EXCCHECKCVI( gs_panel[LASER] = LoadPanelEx(main_panel,"laser_gui.uir",LASER, __CVIUserHInst) );

	EXCCHECKCVI( SetPanelAttribute (gs_panel[LASER], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[LASER], ATTR_LEFT, 0));

Error:
   	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Laser Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE(SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	 
	if( 0==strcmp( uuid, PLUGIN_LASER))
	{
		EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        			"Modules",
	                        			sizeof(SObjListPtr),
	                        			&gs_Modules));
		
		EXCCHECKCVI( DisplayPanel(gs_panel[LASER]));
	}

Error:   
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Laser Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( HidePanel(gs_panel[LASER]));

Error:   
	EXCRETHROW( pexception);

}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Laser Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if(gs_panel[LASER])
	EXCCHECKCVI( DiscardPanel( gs_panel[LASER]));

Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Laser Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
   	SElExceptionPtr	pexception = NULL;  
	
	if (a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Laser Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif
/*---------------------------------------------------------------------------*/
int CVICALLBACK OnLaser (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	char	buffer[4096] = "";
	char	text[4096] = "";
	int		count;
	int     block, program;
	int     Ready;
	
	switch (event)
	{
		case EVENT_COMMIT:
		if(gs_Modules)
		{
			SLasersPtr	plasers = (SLasersPtr)gs_Modules->Get(gs_Modules, MODULE_LASERS);  
			SLaserPtr		plaser = NULL;
			
			GetCtrlVal(panel, LASER_TEXTBOX, text);
		   	GetCtrlVal(panel, LASER_BLOCK, &block);
			GetCtrlVal(panel, LASER_PROGRAM, &program);
			
		 if ( plasers )
			{

				EXCCHECK( plasers->GetLaserByIndex(plasers, 0, &plaser));
				
				switch (control)
				{		
					case LASER_CONNECT:
						
						pexception = plaser->_ConnectSpec(plaser);
						if ( pexception )
							 SetCtrlAttribute(panel, LASER_CONNECT_LED, ATTR_FRAME_COLOR, VAL_RED);
						else 
							SetCtrlAttribute(panel, LASER_CONNECT_LED, ATTR_FRAME_COLOR, VAL_GREEN);
						
					break;
					
					
					
					case LASER_DISCONNECT:
						
						EXCCHECK( plaser->_DisconnectSpec(plaser));
						
					break;
					
					
					case LASER_RUN_PROGRAM:
						
						pexception = plaser->_RunProgramSpec(plaser, program, block);
						if ( pexception )
							SetCtrlAttribute(panel, LASER_PROGRAM_RUN_LED, ATTR_FRAME_COLOR, VAL_RED);
						else 
							SetCtrlAttribute(panel, LASER_PROGRAM_RUN_LED, ATTR_FRAME_COLOR, VAL_GREEN);
						
					break;

					
					case LASER_WRITE_STRING:
						
						EXCCHECK( plaser->_WriteStringSpec(plaser, program, block, text));
						
					break;
					
					
					case LASER_ENABLE_MARKING:
						
						pexception = plaser->_EnableMarkingSpec(plaser);
						if ( pexception )
							SetCtrlAttribute(panel, LASER_ENABLE_MARKING_LED, ATTR_FRAME_COLOR, VAL_RED);
						else 
							SetCtrlAttribute(panel, LASER_ENABLE_MARKING_LED, ATTR_FRAME_COLOR, VAL_GREEN);
						
					break;
				 		
					
					case LASER_MARKER_READY:
						
						EXCCHECK( plaser->_MarkerReadySpec(plaser, &Ready));
						if (Ready)
							SetCtrlAttribute(panel, LASER_MARKER_READY_LED, ATTR_FRAME_COLOR, VAL_GREEN);
						else
							SetCtrlAttribute(panel, LASER_MARKER_READY_LED, ATTR_FRAME_COLOR, VAL_RED);
					break;
					
					
					case LASER_START_LASER:
						
						pexception = plaser->_StartLaseringSpec(plaser);
						if (pexception)
							SetCtrlAttribute(panel, LASER_START_LASER_LED, ATTR_FRAME_COLOR, VAL_GREEN);
						else
							SetCtrlAttribute(panel, LASER_START_LASER_LED, ATTR_FRAME_COLOR, VAL_RED);
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
