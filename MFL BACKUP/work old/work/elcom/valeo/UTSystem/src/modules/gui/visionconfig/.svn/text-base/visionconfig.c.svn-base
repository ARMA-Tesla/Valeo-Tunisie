#include "msxmldom.h"
#include <exception/Exception.h>
#include <eventfactory/EventFactory.h> 
#include <utility.h>
#include "global.h"
#include "visionconfig.h"
#include "visionconfig_plugin.h" 
#include "toolbox.h"
#include "visionconfig_fnc.h" 
#include "visionconfig_sharp.h"
#include "panel_task.h" 
#include "panel_cfg.h" 
#include "lang_translation.h"

const char* HISTORY = \
"[2007-01-14] rT 1.1\n"
" * Fixed function for camera module.\n";

#define NB_OF_PANELS			14  

static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL;
int 						gs_panel[NB_OF_PANELS+1];
SIMAQPtr 					gs_pIMAQ = NULL;
SPluginInfoPtr				gs_pPI = NULL; 
SLanguagePtr 				LANGUAGE_PTR = NULL; 
const char*					gs_cameraName;
CAObjHandle 				gs_ImaqViewer = 0;

static SElExceptionPtr ListCameras(void);
static SElExceptionPtr CameraStop(void) ;

#if defined(_CVI_EXE_)
/*---------------------------------------------------------------------------*/ 
int main (int argc, char *argv[])
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	int32_t			top;
	int32_t			left;
	int32_t			status, size;
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	char      		pathname[MAX_PATHNAME_LEN];
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;    /* out of memory */
	
	EXCCHECK( imaq_new(&gs_pIMAQ));
	
	/* work directory */
	if(0 == FileExists ("./template", &size)) 
	{
		MakeDir ("./template");
	}
	EXCCHECKCVI( GetDir (pglobal->PatternPath));
	strcat(pglobal->PatternPath, "\\template\\");
	EXCCHECK( XmlOpenDocument(TRUE, NULL));
	
	/* gettext */
	EXCCHECK( language_new(&gs_pLanguage));   
	EXCCHECKCVI( GetProjectDir( pathname));
	strcat(pathname, "\\lang");
	gs_pLanguage->bindtextdomain (LANGUAGE_PTR, PACKAGE, pathname);
	gs_pLanguage->textdomain (LANGUAGE_PTR, PACKAGE);
	
	EXCCHECK( LoadPanelWithLanguage (0, 				"visionconfig.uir", PANEL, PANELlang, &gs_panel[PANEL]) );
	EXCCHECK( LoadPanelWithLanguage (gs_panel[PANEL], 	"visionconfig.uir", MAIN, MAINlang, &gs_panel[MAIN]) );
	EXCCHECK( LoadPanelWithLanguage (gs_panel[MAIN], 	"visionconfig.uir", CAMCFG, CAMCFGlang, &gs_panel[CAMCFG]));
	EXCCHECK( LoadPanelWithLanguage (gs_panel[MAIN], 	"visionconfig.uir", PNLTASKS, PNLTASKSlang,&gs_panel[PNLTASKS]));
	EXCCHECK( LoadPanelWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PNLSTEPS, PNLSTEPSlang,&gs_panel[PNLSTEPS]));
	EXCCHECK( LoadPanelWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANPATT, PANPATTlang,&gs_panel[PANPATT] ));
	EXCCHECK( LoadPanelWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANGPATT, PANGPATTlang,&gs_panel[PANGPATT]));
	EXCCHECK( LoadPanelWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANHIST, PANHISTlang,&gs_panel[PANHIST]));
	EXCCHECK( LoadPanelWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANMATH, PANMATHlang,&gs_panel[PANMATH]));
	EXCCHECK( LoadPanelWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANRES, PANRESlang,&gs_panel[PANRES])); 
	EXCCHECK( LoadPanelWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", IMGBUFFER, IMGBUFFERlang,&gs_panel[IMGBUFFER]));    
	EXCCHECK( LoadPanelWithLanguage (gs_panel[MAIN], 	"visionconfig.uir", CFG, CFGlang,&gs_panel[CFG]));
	EXCCHECK( LoadPanelWithLanguage (gs_panel[MAIN], 	"visionconfig.uir", MAKEBUFF, MAKEBUFFlang,&gs_panel[MAKEBUFF]));
	
	EXCCHECKCVI( GetPanelAttribute (gs_panel[PANEL], ATTR_SYSTEM_WINDOW_HANDLE, &pglobal->HWpanel) );
	
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PANEL], PANEL_EXIT, ATTR_VISIBLE, TRUE));
	
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_MENU_OUTLINE, ATTR_TOP, &top));  
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_MENU_OUTLINE, ATTR_LEFT, &left));
	EXCCHECKCVI( SetPanelPos (gs_panel[MAIN], top+16, left) );
	
	EXCCHECKCVI( DisplayPanel (gs_panel[PANEL]));
	EXCCHECKCVI( DisplayPanel (gs_panel[MAIN]));
	
	pexception = gs_pIMAQ->OpenCamera(gs_pIMAQ, "cam0");
	
	EXCCHECK( ListCameras());
	EXCCHECK( iniDisplayWindow());
	
	RunUserInterface ();
	
	pexception = gs_pIMAQ->CloseCamera(gs_pIMAQ, "cam0");
	
Error:
	EXCDISPLAY( pexception);
	DiscardPanel(gs_panel[PANEL]);
	imaq_delete(&gs_pIMAQ);
	language_delete(&gs_pLanguage);
	return 0;
}
#endif

#if defined(_CVI_DLL_) || \
	defined(_WINDLL)

static char		gs_dir_current[MAX_PATHNAME_LEN];
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ShowPanel"
static SElExceptionPtr ShowPanel(void)
{
	int32_t         error = 0;
	SElExceptionPtr pexception = NULL;
	int32_t			visible;
	
	EXCCHECKCVI( GetPanelAttribute( gs_panel[PANEL], ATTR_VISIBLE, &visible));
	if(!visible)
	{
		EXCCHECKCVI( DisplayPanel (gs_panel[PANEL]));
		EXCCHECKCVI( DisplayPanel (gs_panel[MAIN]));
		
		EXCCHECK( ListCameras());
		EXCCHECKIMAQ( imaqShowWindow (IMAQ_WINDOW_NUMBER, TRUE)); 

		/* save actual directory */
		EXCCHECKCVI( GetDir (gs_dir_current));
	}
	
Error:      
   	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ClosePanel"
static SElExceptionPtr ClosePanel(void)
{
	int32_t         error = 0;
	SElExceptionPtr pexception = NULL;
	int32_t			visible;
	
	EXCCHECKCVI( GetPanelAttribute( gs_panel[PANEL], ATTR_VISIBLE, &visible));
	if(visible)
	{
		/* stop grap or sharp process */     
		EXCCHECK( CameraStop());   	
			
		EXCCHECKIMAQ( imaqShowWindow (IMAQ_WINDOW_NUMBER, FALSE));
		EXCCHECKIMAQ( imaqShowToolWindow (FALSE));  
		EXCCHECKCVI( HidePanel (gs_panel[PANEL])); 
		EXCCHECKCVI( SetDir (gs_dir_current));
	}
	
Error:      
   	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	int32_t         error = 0;
	SElExceptionPtr pexception = NULL;
	
	if(0==strcmp(eventName, "EVNT_GUI_MENU"))
	{
		if(pData)
		{
			const char* mode = ((SElEventDataPtr)pData)->GetStringPtr(pData);   
			
			if(0==strcmp(mode, "menu_maintenance"))
			{
				EXCCHECKIMAQ( imaqShowWindow (IMAQ_WINDOW_NUMBER, FALSE));
				EXCCHECKIMAQ( imaqShowToolWindow (FALSE));	
			}
		}	
	}
	
Error:      
   	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
   int            	error = 0;
   SElExceptionPtr 	pexception = NULL;
   
   if ( gs_EventFactory )
   {
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
		                                   "EVNT_GUI_MENU",
		                                   EventFunctionCallback));
   }
   
Error:
   EXCRETHROW(pexception);
}   /* RegisterEventFunction */

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
#define __FUNC__ "{Visionconfig Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr pexception = NULL;
	int32_t         error = 0;
	int      		maintenance_panel, main_panel;
	int				monitor_id, top, height, left, size, panel_top, panel_left;
	SGlobalPtr		pglobal = GetGlobal(NULL);     

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "MaintenancePanel",
	                        sizeof(maintenance_panel),
	                        &maintenance_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "MainPanel",
	                        sizeof(main_panel),
	                        &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "Modules",
	                        sizeof(SObjListPtr),
	                        &gs_Modules));

	/* work directory */
	if(0 == FileExists ("./template", &size)) 
	{
		MakeDir ("./template");
	}
	EXCCHECKCVI( GetDir (pglobal->PatternPath));
	strcat(pglobal->PatternPath, "\\template\\");
	EXCCHECK( XmlOpenDocument(TRUE, NULL));
	
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

	EXCCHECK( LoadPanelExWithLanguage (maintenance_panel, "visionconfig.uir", PANEL, PANELlang, &gs_panel[PANEL]) );
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PANEL], "visionconfig.uir", MAIN, MAINlang, &gs_panel[MAIN]) );
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[MAIN], 	"visionconfig.uir", CAMCFG, CAMCFGlang, &gs_panel[CAMCFG]));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[MAIN], 	"visionconfig.uir", PNLTASKS, PNLTASKSlang,&gs_panel[PNLTASKS]));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PNLSTEPS, PNLSTEPSlang,&gs_panel[PNLSTEPS]));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANPATT, PANPATTlang,&gs_panel[PANPATT] ));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANGPATT, PANGPATTlang,&gs_panel[PANGPATT]));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANHIST, PANHISTlang,&gs_panel[PANHIST]));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANMATH, PANMATHlang,&gs_panel[PANMATH]));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANRES, PANRESlang,&gs_panel[PANRES])); 
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", IMGBUFFER, IMGBUFFERlang,&gs_panel[IMGBUFFER]));    
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[PNLTASKS],"visionconfig.uir", PANFACE, PANFACElang,&gs_panel[PANFACE]));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[MAIN], 	"visionconfig.uir", CFG, CFGlang,&gs_panel[CFG]));
	EXCCHECK( LoadPanelExWithLanguage (gs_panel[MAIN], 	"visionconfig.uir", MAKEBUFF, MAKEBUFFlang,&gs_panel[MAKEBUFF]));
	
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[PNLSTEPS], PNLSTEPS_BKTREE, &pglobal->bktree));
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[PANGPATT], PANGPATT_BKTREE, &pglobal->bktreeGT));
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[PANPATT], PANPATT_BKTREE, &pglobal->bktreePM));
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[PANHIST], PANHIST_BKTREE, &pglobal->bktreeH));
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[PANFACE], PANFACE_BKTREE, &pglobal->bktreeFC));

	EXCCHECKCVI( GetPanelAttribute (main_panel, ATTR_SYSTEM_WINDOW_HANDLE, &pglobal->HWpanel) );  
	
	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
	EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP,    &top));
	EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));

	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_LEFT, 0));
	
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_MENU_OUTLINE, ATTR_TOP, &top));  
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_MENU_OUTLINE, ATTR_LEFT, &left));
	EXCCHECKCVI( SetPanelPos (gs_panel[MAIN], top+16, left) );

	EXCCHECKCVI( GetPanelAttribute (maintenance_panel, ATTR_TOP, &panel_top));
	EXCCHECKCVI( GetPanelAttribute (maintenance_panel, ATTR_LEFT, &panel_left));    
	EXCCHECK( iniDisplayWindow(panel_top, panel_left));        
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "EventFactory",
	                        sizeof(SElEventFactoryPtr),
	                        &gs_EventFactory));

	if(gs_Modules)
		gs_pIMAQ = (SIMAQPtr)gs_Modules->Get(gs_Modules, MODULE_CAMERA); 
	
	EXCCHECK( RegisterEventFunction());

	pglobal->pImage = imaqCreateImage (IMAQ_IMAGE_U8, 3);
	if(pglobal->pImage==NULL)
	{
		EXCCHECKIMAQ(0);
	}
   
Error:
   	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Visionconfig Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE(SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
   
	if( 0==strcmp( uuid, PLUGIN_VISIONCONFIG))
	{
		EXCCHECK( ShowPanel());
	}

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Visionconfig Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( ClosePanel());

Error:   
	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Visionconfig Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
   	int32_t         error = 0;

	/* stop grap or sharp process */     
	EXCCHECK( CameraStop());  

	EXCCHECKIMAQ( imaqCloseWindow (IMAQ_ALL_WINDOWS));
	
	if ( gs_panel[PANEL] )
		EXCCHECKCVI( DiscardPanel(gs_panel[PANEL]));  

	EXCCHECK( closeDisplayWindow());
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Visionconfig Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
   	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Visionconfig Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif

/*---------------------------------------------------------------------------*/   
#undef __FUNC__
#define __FUNC__ "OnPanel"
int CVICALLBACK OnPanel (int panel, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;   
	
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			EXCCHECKIMAQ( imaqShowWindow (IMAQ_WINDOW_NUMBER, TRUE));  
			break;
	}
	
Error:	
	EXCDISPLAY( pexception); 
	return 0;	
}

/*---------------------------------------------------------------------------*/   
#undef __FUNC__
#define __FUNC__ "OnExit"
int CVICALLBACK OnExit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;   
	
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface(0);
			break;
	}
	
Error:	
	EXCDISPLAY( pexception); 
	return 0;	
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnImgLoad"
int CVICALLBACK OnImgLoad (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	char**			path = NULL;
	int				cancelled = 0;
	
	
	switch (event)
		{
		case EVENT_COMMIT:
			path = imaqLoadImagePopup ("", "*.png", "",
										"Select an Image", FALSE,
										IMAQ_BUTTON_LOAD, 0, 0, 1, 0, &cancelled,
										NULL);
			if (!cancelled)
			{
				EXCCHECK( gs_pIMAQ->LoadPicture( gs_pIMAQ, INTERNAL_IMAGE_NAME, path[0]));
				IMAQ_DISPLAY_IMAGE_INTERNAL();
			}
			break;
		}

Error:
	imaqDispose(path); 
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnImgSnap"
int CVICALLBACK OnImgSnap (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	int				average_count;
	double			average_gain;      
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(gs_panel[CAMCFG], CAMCFG_SNAP_AVERAGE, &average_count));
			if(average_count>1)
			{
				EXCCHECKCVI( GetCtrlVal(gs_panel[CAMCFG], CAMCFG_SNAP_AVERAGE_GAIN, &average_gain));	
				EXCCHECK( gs_pIMAQ->SnapAverage(gs_pIMAQ, gs_cameraName, average_count, average_gain, INTERNAL_IMAGE_NAME));
				IMAQ_DISPLAY_IMAGE_INTERNAL(); 
			}
			else
			{
				EXCCHECK( gs_pIMAQ->Snap(gs_pIMAQ, gs_cameraName, INTERNAL_IMAGE_NAME));
				IMAQ_DISPLAY_IMAGE_INTERNAL(); 
			}
			break;
	}

Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CameraStop"
static SElExceptionPtr CameraStop(void)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	int32_t			dimmed,
					dimmed_cam;
 
	EXCCHECKCVI( GetCtrlAttribute(gs_panel[MAIN], MAIN_IMG_SNAP, ATTR_DIMMED, &dimmed));
	EXCCHECKCVI( GetCtrlAttribute(gs_panel[MAIN], MAIN_SET_CAM, ATTR_DIMMED, &dimmed_cam));
	if( dimmed && !dimmed_cam)
	{
		EXCCHECKCVI( OnImgGrab(0, 0, EVENT_COMMIT, NULL, 0, 0)); 	
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "OnImgGrab"
int CVICALLBACK OnImgGrab (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	static bool_t	cameraRun = FALSE;
	int32_t			visible;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetPanelAttribute (gs_panel[CAMCFG], ATTR_VISIBLE, &visible));

			if(!visible) /* sharp */
			{
				if(!cameraRun)
					EXCCHECK( gs_pIMAQ->GrabStart(gs_pIMAQ, gs_cameraName, MyDisplayFce));
				else
					EXCCHECK( gs_pIMAQ->GrabStop(gs_pIMAQ, gs_cameraName));  
			}
			else /* grab */
			{
				if(!cameraRun)
					EXCCHECK( CameraSharpStart());
				else
					EXCCHECK( CameraSharpStop());
			}
			cameraRun = (!cameraRun);
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_IMG_SNAP, ATTR_DIMMED, cameraRun));
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_IMG_LOAD, ATTR_DIMMED, cameraRun)); 
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_IMG_SAVE, ATTR_DIMMED, cameraRun));
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_TASKLIST, ATTR_DIMMED, cameraRun)); 
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_TASK_ADD, ATTR_DIMMED, cameraRun));
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_CAMERA, ATTR_DIMMED, cameraRun));
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_IMG_MAKE_BUFFER, ATTR_DIMMED, cameraRun)); 
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_SET_CAM, ATTR_DIMMED, cameraRun)); 
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_OK, ATTR_DIMMED, cameraRun)); 
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_SNAP_AVERAGE, ATTR_DIMMED, cameraRun)); 
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_SNAP_AVERAGE_GAIN, ATTR_DIMMED, cameraRun)); 
			break;
	}

Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnImgSave"
int CVICALLBACK OnImgSave (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int				error = 0;  
	char			path[MAX_PATHNAME_LEN] = "";
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			  error = FileSelectPopup ("", "*.png", "*.jpg;*.png;*.bmp;*.tiff", "", 
				  						VAL_SAVE_BUTTON, 0, 0, 1, 1, path);
			  
			  if ( error != VAL_NO_FILE_SELECTED && error >= 0 )
			  {
				EXCCHECK( gs_pIMAQ->SaveImage( gs_pIMAQ, INTERNAL_IMAGE_NAME, path));
			  }

			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMakeBuffer"
int CVICALLBACK OnMakeBuffer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int32_t			visible;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			EXCCHECKCVI( GetPanelAttribute (gs_panel[MAKEBUFF], ATTR_VISIBLE, &visible));
			if(!visible)
			{
				EXCCHECKCVI( SetPanelPos( gs_panel[MAKEBUFF], 165, 14));
				EXCCHECKCVI( DisplayPanel( gs_panel[MAKEBUFF]));
				EXCCHECK( DisplayAllBuffers( gs_panel[MAKEBUFF], MAKEBUFF_IMAGE_BUFFER_LIST));
			}
			else
			{
				EXCCHECKCVI( HidePanel( gs_panel[MAKEBUFF] ));
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnConfig"
int CVICALLBACK OnConfig(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int				error = 0;
	char			path[MAX_PATHNAME_LEN] = "";
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch(control)
			{
				case MAIN_CFG_NEW:
					EXCCHECK( XmlOpenDocument(TRUE, NULL));
					EXCCHECK( XmlLoadTaskTree(NULL));	   
					break;
				case MAIN_CFG_LOAD:
					error = FileSelectPopup ("", "*.xml", "*.xml", "", VAL_SELECT_BUTTON, 0, 1, 1, 0, path);
					if ( error != VAL_NO_FILE_SELECTED && error >= 0 )
					{
						EXCCHECK( XmlOpenDocument(FALSE, path));
						EXCCHECK( XmlLoadGlobal());  
						EXCCHECK( XmlLoadTaskTree(NULL));				
					}
					break;
				case MAIN_CFG_SAVE:
					error = FileSelectPopup ("", "*.xml", "*.xml", "", VAL_SAVE_BUTTON, 0, 1, 1, 1, path);
					if ( error != VAL_NO_FILE_SELECTED && error >= 0 )
					{
						EXCCHECK( XmlSaveDocument(path));  
					}
					break;
				case MAIN_CFG_SET:
					EXCCHECK( InitializeConfigDialog());      
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
#define __FUNC__ "DisplayTaskEditor"
static SElExceptionPtr DisplayTaskEditor(CAObjHandle ptask)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL); 
			
	if( gs_pIMAQ->GetImage(gs_pIMAQ, INTERNAL_IMAGE_NAME) )
	{
		pglobal->task = ptask;
		
		EXCCHECK( InitializeTaskDialog());
	}
	else
	{
		MessagePopup ("Error", _("Snap or Load Image"));
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTaskAdd"
int CVICALLBACK OnTaskAdd(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;     
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECK( DisplayTaskEditor(0));	
			break;
	}

Error:	
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnTaskList"
int CVICALLBACK OnTaskList(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;
   	int            	index, area, column;
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	CAObjHandle     pnode;
	char			label[256];
	
	switch (event)
	{
		case EVENT_KEYPRESS:
			if(eventData1 == VAL_FWD_DELETE_VKEY)
			{
				EXCCHECKCVI( GetCtrlIndex (panel, control, &index));
				if ( index >= 0 )
				{
					if( ConfirmPopup ("Delete Dialog", "Delete selected task with the steps?") )
					{
						EXCCHECKCVI( GetLabelFromIndex (panel, control, index, label));
						EXCCHECK( XmlDeleteTask(label));
						EXCCHECK( XmlLoadTaskTree(NULL));	
					}
				} 
			}
			break;
		case EVENT_LEFT_DOUBLE_CLICK:
		
	        EXCCHECKCVI( GetIndexFromPoint (panel, control, MakePoint (eventData2, eventData1), &index, &area, &column));
	        if (index != -1)
	        {  
				EXCCHECKCVI( GetValueFromIndex (panel, control, index, &pnode));	
				EXCCHECK( DisplayTaskEditor(pnode));
			}	
			break;
	}

Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnSetCam"
int CVICALLBACK OnSetCam (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	int32_t 		value = -1;
	int32_t			visible;
	uint32_t		min, max;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			EXCCHECKCVI( GetPanelAttribute (gs_panel[CAMCFG], ATTR_VISIBLE, &visible));
			
			if(!visible)
			{
				/*****************brightness*************/
				pexception = gs_pIMAQ->GetCameraAttribute(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_BRIGHTNESS, &value);
				if(pexception)
				{
					EXCDBGSTR(pexception);
					min = 0;
					max = 0;
					value = 0;
				}
				else
					EXCCHECK( gs_pIMAQ->GetCameraAttributeRange(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_BRIGHTNESS, &min, &max)); 

				EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_BRIGHTNESS, ATTR_MAX_VALUE, max));
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_BRIGHTNESS, ATTR_MIN_VALUE, min));
				EXCCHECKCVI( SetCtrlVal(gs_panel[CAMCFG], CAMCFG_BRIGHTNESS, value));  

				/*****************frame rate*************/
				pexception = gs_pIMAQ->GetCameraAttribute(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_FRAME_RATE, &value);
				if(pexception)
				{
					EXCDBGSTR(pexception);
					min = 0;
					max = 0;
					value = 0;
				}
				else
					EXCCHECK( gs_pIMAQ->GetCameraAttributeRange(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_FRAME_RATE, &min, &max)); 
				
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_FRAME_RATE, ATTR_MAX_VALUE, max));
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_FRAME_RATE, ATTR_MIN_VALUE, min));
				EXCCHECKCVI( SetCtrlVal(gs_panel[CAMCFG], CAMCFG_FRAME_RATE, value));  

				/*****************gain******************/
				pexception = gs_pIMAQ->GetCameraAttribute(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_GAIN, &value);
				if(pexception)
				{
					EXCDBGSTR(pexception);
					min = 0;
					max = 0;
					value = 0;
				}
				else
					EXCCHECK( gs_pIMAQ->GetCameraAttributeRange(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_GAIN, &min, &max)); 
				
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_GAIN, ATTR_MAX_VALUE, max));
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_GAIN, ATTR_MIN_VALUE, min));
				EXCCHECKCVI( SetCtrlVal(gs_panel[CAMCFG], CAMCFG_GAIN, value));  

				/*****************shutter******************/
				pexception = gs_pIMAQ->GetCameraAttribute(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_SHUTTER_SPEED, &value);
				if(pexception)
				{
					EXCDBGSTR(pexception);
					min = 0;
					max = 0;
					value = 0;
				}
				else
					EXCCHECK( gs_pIMAQ->GetCameraAttributeRange(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_SHUTTER_SPEED, &min, &max)); 
				
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_SHUTTER, ATTR_MAX_VALUE, max));
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[CAMCFG], CAMCFG_SHUTTER, ATTR_MIN_VALUE, min));
				EXCCHECKCVI( SetCtrlVal(gs_panel[CAMCFG], CAMCFG_SHUTTER, value));  
				
				/* camera sharp */
				EXCCHECK( OpenSharpCamera());
				EXCCHECKCVI( SetPanelPos( gs_panel[CAMCFG], 165, 14));
				EXCCHECKCVI( DisplayPanel( gs_panel[CAMCFG]));
				EXCCHECKCVI( SetActivePanel( gs_panel[CAMCFG])); 
			}
			else
			{
				EXCCHECK( CloseSharpCamera());
				EXCCHECKCVI( HidePanel( gs_panel[CAMCFG]));
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnCameraConfig"
int CVICALLBACK OnCameraConfig (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   
	SElExceptionPtr	pexception = NULL; 
	int32_t			error; 
	int32_t 		value = -1;
	int32_t 		dimmed = -1;

	switch (event)
	{
		case EVENT_COMMIT:
			
			EXCCHECKCVI( GetCtrlVal(panel, control, &value));
			switch(control)
			{
				case CAMCFG_BRIGHTNESS:
					EXCCHECK( gs_pIMAQ->SetCameraAttribute(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_BRIGHTNESS, value));
					break;
				case CAMCFG_FRAME_RATE:
					EXCCHECK( gs_pIMAQ->SetCameraAttribute(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_FRAME_RATE, value));
					break;
				case CAMCFG_GAIN:
					EXCCHECK( gs_pIMAQ->SetCameraAttribute(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_GAIN, value));
					break;
				case CAMCFG_SHUTTER:
					EXCCHECK( gs_pIMAQ->SetCameraAttribute(gs_pIMAQ, gs_cameraName, CAMERA_ATTR_SHUTTER_SPEED, value));
					break;
			}
			
			EXCCHECKCVI( GetCtrlAttribute (gs_panel[MAIN], MAIN_IMG_SNAP, ATTR_DIMMED, &dimmed));
			if(!dimmed)
		    {
		    	OnImgSnap (0, 0, EVENT_COMMIT, NULL, 0, 0);	
		    }
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnCamera"
int CVICALLBACK OnCamera (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	uint32_t		index = 0;
	uint32_t		visible = 0;	
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(panel, control, &index));
			
			gs_cameraName = (gs_pIMAQ->cameras)->GetNameOnIndex(gs_pIMAQ->cameras, index);
			
			EXCCHECKCVI( GetPanelAttribute (gs_panel[CAMCFG], ATTR_VISIBLE, &visible));
			if (visible)
			{
				EXCCHECKCVI( OnSetCam (0, 0, EVENT_COMMIT, NULL, 0, 0));
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ListCameras"
static SElExceptionPtr ListCameras(void)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	uint32_t		index = 0;
	uint32_t		cam_count = 0;
	const char*		name; 
	
	EXCCHECKCVI( ClearListCtrl( gs_panel[MAIN], MAIN_CAMERA));    
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[MAIN], MAIN_CAMERA, ATTR_DIMMED, TRUE));	

	gs_cameraName = NULL;   
	
	cam_count = (gs_pIMAQ->cameras)->GetCount(gs_pIMAQ->cameras);
	if ( cam_count > 0 )
	{
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_IMG_SNAP, ATTR_DIMMED, FALSE));
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_IMG_GRAB, ATTR_DIMMED, FALSE)); 
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_SET_CAM, ATTR_DIMMED, FALSE)); 
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[MAIN], MAIN_CAMERA, ATTR_DIMMED, FALSE));

		gs_cameraName = (gs_pIMAQ->cameras)->GetNameOnIndex(gs_pIMAQ->cameras, 0); 
		
		for ( index = 0; index < cam_count; index++)
		{
			name = (gs_pIMAQ->cameras)->GetNameOnIndex(gs_pIMAQ->cameras, index);
			EXCCHECKCVI( InsertListItem (gs_panel[MAIN], MAIN_CAMERA, index, name, index));
		}	
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnHideChildPanel"
int CVICALLBACK OnHideChildPanel (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
 	int32_t			error = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( HidePanel( panel));
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadImageWithPopup"
static SElExceptionPtr LoadImageWithPopup(const char* dir, const char* imageName)
{
	SElExceptionPtr	pexception = NULL;    
	int32_t			error = 0;
	int				cancelled;
	char**			path = NULL;
	
	path = imaqLoadImagePopup (dir, "*.png", "",
							   "Select an Image", FALSE,
							   IMAQ_BUTTON_LOAD, 0, 0, 1, 0, &cancelled,
							   NULL);
	if (!cancelled)
	{
		if ( path == NULL )
		{
			EXCCHECKIMAQ( imaqGetLastError());
		}
		
		EXCCHECK( gs_pIMAQ->LoadPicture( gs_pIMAQ, imageName, path[0])); 
		IMAQ_DISPLAY_IMAGE_INTERNAL();
	}

Error:
	imaqDispose(path); 
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnCreateBuffer"
int CVICALLBACK OnCreateBuffer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	char			buffer[256] = "";
	Image*			pbuffer_image = NULL;
	int				avg_count = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, MAKEBUFF_BUFFER_NAME, buffer));
			RemoveSurroundingWhiteSpace(buffer);
			if ( strlen(buffer)==0 )
			{
				MessagePopup(_("Error"), _("You must specify buffer name!"));
				goto Error;
			}
			
			if ( stricmp(buffer, INTERNAL_IMAGE_NAME)==0 || buffer[0] =='@')
			{
				MessagePopup(_("Error"), _("Name \"image\" or names with @ character on begin are reserved for internal use"));
				goto Error;
			}
			
			pbuffer_image = gs_pIMAQ->GetImage(gs_pIMAQ, buffer);
			if ( pbuffer_image != NULL )
			{
				char	message[512] = "";
				sprintf( message, _("Buffer with name \"%s\" already exist. Do you want replace it?"), buffer);
				if ( !ConfirmPopup (_("Warning"), message))
				{
					goto Error;
				}
			}
			
			switch (control)
			{
				case MAKEBUFF_ACT_IMAGE_BUFFER:
					EXCCHECK( gs_pIMAQ->MakeImageCopy( gs_pIMAQ, buffer, INTERNAL_IMAGE_NAME));
					break;
				case MAKEBUFF_SNAP_IMAGE_BUFFER:
					SetWaitCursor (1);
					EXCCHECKCVI( GetCtrlVal( panel, MAKEBUFF_AVG_IMAGE_BUFFER, &avg_count));
					EXCCHECK( gs_pIMAQ->SnapAverage(gs_pIMAQ, gs_cameraName, avg_count, 1.0, buffer));
					break;
				case MAKEBUFF_LOAD_IMAGE_BUFFER:
					EXCCHECK( LoadImageWithPopup("", buffer));
					break;
			}
			EXCCHECK( DisplayAllBuffers( panel, MAKEBUFF_IMAGE_BUFFER_LIST));
			break;
	}
	
Error:
	SetWaitCursor (0);
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnBufferImageList"
int CVICALLBACK OnBufferImageList (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	char			name[256];
	int				index;
	
	switch (event)
	{
		case EVENT_KEYPRESS:
			if(eventData1 == VAL_FWD_DELETE_VKEY)
			{
				EXCCHECKCVI( GetCtrlIndex(panel, control, &index));
				if ( index >= 0 )
				{
					EXCCHECKCVI( GetLabelFromIndex( panel, control, index, name));
					if( ConfirmPopup ("Delete Dialog", "Delete selected image from buffer?") )
					{
						EXCCHECK( gs_pIMAQ->RemoveImage(gs_pIMAQ, name));
						EXCCHECK( DisplayAllBuffers( panel, MAKEBUFF_IMAGE_BUFFER_LIST));
						EXCCHECKCVI( SetCtrlVal(panel, MAKEBUFF_BUFFER_NAME, ""));
					}
 				}
			}
			break;
			
		case EVENT_LEFT_DOUBLE_CLICK:
			EXCCHECKCVI( GetCtrlIndex(panel, control, &index));
			if ( index >= 0 )
			{
				EXCCHECKCVI( GetLabelFromIndex( panel, control, index, name));
				EXCCHECK( gs_pIMAQ->MakeImageCopy( gs_pIMAQ, INTERNAL_IMAGE_NAME, name));
				EXCCHECKCVI( SetCtrlVal(panel, MAKEBUFF_BUFFER_NAME, name));
				IMAQ_DISPLAY_IMAGE_INTERNAL();
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnZoom"
int CVICALLBACK OnZoom (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	static int		xZoom = 1;
	static int		yZoom = 1;
	Point			point = {0, 0};
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			EXCCHECKIMAQ(imaqGetWindowCenterPos(IMAQ_WINDOW_NUMBER, &point));  
			
			switch(control)
			{
				case PANEL_ZOOM:
					xZoom = 1;
					yZoom = 1;
					EXCCHECKIMAQ(imaqZoomWindow(IMAQ_WINDOW_NUMBER,xZoom,yZoom,point));
					break;
				case PANEL_ZOOM_PLUS:
					EXCCHECKIMAQ(imaqGetWindowZoom(IMAQ_WINDOW_NUMBER, &xZoom, &yZoom));
					EXCCHECKIMAQ(imaqZoomWindow(IMAQ_WINDOW_NUMBER,++xZoom,++yZoom,point)); 
					break;
				case PANEL_ZOOM_MINUS:
					EXCCHECKIMAQ(imaqGetWindowZoom(IMAQ_WINDOW_NUMBER, &xZoom, &yZoom)); 
					if(xZoom == 1 && yZoom == 1)
					{
						xZoom = -1;
						yZoom = -1;
					}
					EXCCHECKIMAQ(imaqZoomWindow(IMAQ_WINDOW_NUMBER,--xZoom,--yZoom,point));
					break;
				
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception); 
	return 0;
}

