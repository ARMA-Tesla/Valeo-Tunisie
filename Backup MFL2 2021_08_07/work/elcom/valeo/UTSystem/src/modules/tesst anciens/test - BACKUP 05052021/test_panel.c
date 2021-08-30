#include "test_panel.h"
#include <utility.h>
#include <test/testseq.h>
#include "test_plugin.h" /* include path in project/properties/aditional include directories */
#include <ansi_c.h>

#pragma warning( push)
#pragma warning( disable: 4996)

extern STestListItem 		gs_TestList[];
extern STestSeq 			gs_TestSeq;

int							gs_TestPanel = 0;
SObjListPtr 				gs_Modules = NULL;
static SElEventFactoryPtr	gs_EventFactory = NULL;
SLanguagePtr				gs_pLanguage = NULL;

static int panelHandle;




/*==================================================================================
						  	 	  DLL PeakValleysCalc
===================================================================================*/

//Appel de DLL LV  PeakValleysCalc
int LoadLibrary_LV(char *sPath)
{ 
HMODULE hModule;

hModule = (HMODULE)LoadLibrary(sPath);
		if (!hModule)
	 			return -1;
		
return 0;
}

/*==================================================================================
						  	 	  DLL MAIN
===================================================================================*/


int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		
		case DLL_PROCESS_ATTACH:
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)		
				return 0;	//out of memory 
			break;
	
		case DLL_PROCESS_DETACH:
			if (!CVIRTEHasBeenDetached())		//Do not call CVI functions if cvirte.dll has already been detached.
			{
				CloseCVIRTE ();		
			}
			break;
	}

	return 1;
}

int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
//Included for compatibility with Borland 

	 return DllMain (hinstDLL, fdwReason, lpvReserved);
}


//Appel de DLL pour Affichier UIR
void DLLEXPORT RunDllUI (void)
{
    ///Call this function from the appropriate place in your code 
    //to load and display startup panels.                       

    panelHandle = LoadPanelEx (0, "Test.uir", PANEL, __CVIUserHInst);
    DisplayPanel (panelHandle);
    RunUserInterface ();
}

#undef __FUNC__
#define __FUNC__ "{Test Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			main_panel = 0;
	int32_t			monitor_id = 0,
					monitor_id2 = 0;
	int32_t         top, left, 
					width, height;
	
	int             iStatus;
	
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "AutomatPanel",
									 sizeof(main_panel),
									 &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "Modules",
	                        sizeof(SObjListPtr),
	                        &gs_Modules));
	
	/*Gettext*/
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
	
	
#if 0    /* formerly excluded lines */
	iStatus = LoadLibrary_LV("C:\\FlexiTester.OK\\PeaksDetector.dll");
	if (iStatus < 0) 
		EXCTHROW(-1, "Error chargement DLL PeakValleysDetector.dll !!");
#endif   /* formerly excluded lines */
	
	EXCCHECKCVI( gs_TestPanel = LoadPanelEx( main_panel, "test.uir", PANEL, __CVIUserHInst));
	EXCCHECKCVI( SetPanelAttribute( gs_TestPanel, ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute( gs_TestPanel, ATTR_LEFT, 0));

	EXCCHECKCVI( gs_TestPanel2 = LoadPanelEx( main_panel, "test.uir", PANEL_2, __CVIUserHInst));
	EXCCHECKCVI( SetPanelAttribute( gs_TestPanel2, ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute( gs_TestPanel2, ATTR_LEFT, 0));
	
	EXCCHECKCVI( gs_TestPanel3 = LoadPanelEx( main_panel, "test.uir", PANEL_3, __CVIUserHInst));
	EXCCHECKCVI( SetPanelAttribute( gs_TestPanel3, ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute( gs_TestPanel3, ATTR_LEFT, 0));

	EXCCHECKCVI( gs_TestPanel4 = LoadPanelEx( main_panel, "test.uir", PANEL_4, __CVIUserHInst));
	EXCCHECKCVI( SetPanelAttribute( gs_TestPanel4, ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute( gs_TestPanel4, ATTR_LEFT, 0));


	
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Test Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Test Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Test Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	if(gs_TestPanel)
	{
		EXCCHECKCVI( DiscardPanel( gs_TestPanel));
		gs_TestPanel = 0;
	}
	
	if(gs_TestPanel2)
	{
		EXCCHECKCVI( DiscardPanel( gs_TestPanel2));
		gs_TestPanel2 = 0;
	}
	
	if(gs_TestPanel3)
	{
		EXCCHECKCVI( DiscardPanel( gs_TestPanel3));
		gs_TestPanel3 = 0;
	}

	if(gs_TestPanel4)
	{
		EXCCHECKCVI( DiscardPanel( gs_TestPanel4));
		gs_TestPanel4 = 0;
	}
	
	if(gs_TestPanel5)
	{
		EXCCHECKCVI( DiscardPanel( gs_TestPanel5));
		gs_TestPanel5 = 0;
	}

Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Test Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Test Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

//#endif 	 //TEST

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnButton"
int  CVICALLBACK OnButton(int panel, int control, int event, 
   void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception =  NULL;  
	int32_t      	error = 0;

	switch (event)
	{
		case EVENT_COMMIT:

			switch(control)
			{
				case PANEL_FORWARD:
					if(gs_EventFactory)
						EXCCHECK(gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_EXPERTISE_STEP_MODE_NEXT", NULL)); 
					break;
				case PANEL_REPEAT:
					if(gs_EventFactory)
						EXCCHECK(gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_EXPERTISE_STEP_MODE_REPEAT", NULL)); 
					break;
				case PANEL_BACKWARD:
					if(gs_EventFactory)
						EXCCHECK(gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_EXPERTISE_STEP_MODE_BACK", NULL)); 
					break;
				case PANEL_STOP:
					if(gs_EventFactory)
						EXCCHECK(gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_EXPERTISE_STEP_MODE_STOP", NULL)); 
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
#define __FUNC__ "GetTestSeqFunction"
SElExceptionPtr _FUNCC GetTestSeqFunction(
    STestSeqPtr*   pTestSeq 
)
{
	SElExceptionPtr pexception = NULL;   
	
	if( pTestSeq)
		*pTestSeq = &gs_TestSeq;
	
Error:
	EXCRETHROW( pexception); 	
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetTestFunction"
SElExceptionPtr _FUNCC GetTestFunction(
    int32_t         index,
    STestListItemPtr*   pTest,
	int32_t*		test_count
)
{
    static int32_t  count = 0;
    SElExceptionPtr pexception = NULL;
    
	for(; gs_TestList[count].name; count++)
		;

    if( test_count) 
		*test_count = count;
    
    if( pTest && index<count) 
		*pTest = &gs_TestList[index];
    
Error:
    EXCRETHROW( pexception);
};

void CVICALLBACK PostDeferredDisplayPanel (void *callbackData) { DisplayPanel(gs_TestPanel);}
void CVICALLBACK PostDeferredHidePanel (void *callbackData) { HidePanel(gs_TestPanel);}

void CVICALLBACK PostDeferredDisplayPanel2 (void *callbackData) { DisplayPanel(gs_TestPanel2);}
void CVICALLBACK PostDeferredHidePanel2 (void *callbackData) { HidePanel(gs_TestPanel2);}

void CVICALLBACK PostDeferredDisplayPanel3 (void *callbackData) { DisplayPanel(gs_TestPanel3);}
void CVICALLBACK PostDeferredHidePanel3 (void *callbackData) { HidePanel(gs_TestPanel3);}

void CVICALLBACK PostDeferredDisplayPanel4 (void *callbackData) { DisplayPanel(gs_TestPanel4);}
void CVICALLBACK PostDeferredHidePanel4 (void *callbackData) { HidePanel(gs_TestPanel4);}

void CVICALLBACK PostDeferredDisplayPanel5 (void *callbackData) { DisplayPanel(gs_TestPanel5);}
void CVICALLBACK PostDeferredHidePanel5 (void *callbackData) { HidePanel(gs_TestPanel5);}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "IniTestPanel"
SElExceptionPtr _FUNCC IniTestPanel(const char* picture, bool_t reset)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0; 

	if(gs_TestPanel)
	{
		if(reset)
		{
			EXCCHECKCVI( SetCtrlVal( gs_TestPanel, PANEL_RESULT_MSG, _("Test Result")));
			EXCCHECKCVI( SetCtrlVal( gs_TestPanel, PANEL_TEXT1, _("Time:"))); 
			EXCCHECKCVI( SetCtrlVal( gs_TestPanel, PANEL_TEXT2, _("S/N:")));
			EXCCHECKCVI( SetCtrlVal( gs_TestPanel, PANEL_TEXT3, _("Test:")));
			EXCCHECKCVI( SetCtrlIndex( gs_TestPanel, PANEL_ACTION, 3)); 
			EXCCHECKCVI( CanvasClear( gs_TestPanel, PANEL_CANVAS, VAL_ENTIRE_OBJECT)); 
			EXCCHECKCVI( DeleteTableRows( gs_TestPanel, PANEL_TABLE, 1, -1));
		}

		EXCCHECKCVI( DisplayImageFile( gs_TestPanel, PANEL_PANELPICTURE, picture));
		EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel, NULL));
	}

Error:
    EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CloseTestPanel"
SElExceptionPtr _FUNCC CloseTestPanel(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0; 

	if(gs_TestPanel)
		EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel, NULL));

Error:
    EXCRETHROW( pexception);
}

void SetAnnotationAttribute_ALL(int Panel_Handle, int Control_ID, int Annotation_Index)
{
	SetAnnotationAttribute(Panel_Handle, Control_ID, Annotation_Index, ATTR_ANNOTATION_OFFSET_MODE, VAL_TOP_LEFT_OFFSET);
    SetAnnotationAttribute(Panel_Handle, Control_ID, Annotation_Index, ATTR_ANNOTATION_LINE_COLOR, VAL_BLACK);
    SetAnnotationAttribute(Panel_Handle, Control_ID, Annotation_Index, ATTR_ANNOTATION_GLYPH_COLOR, VAL_BLACK);
    SetAnnotationAttribute(Panel_Handle, Control_ID, Annotation_Index, ATTR_ANNOTATION_CAPTION_ALWAYS_IN_VIEW, 1);
    SetAnnotationAttribute(Panel_Handle, Control_ID, Annotation_Index, ATTR_ANNOTATION_CAPTION_BOLD, 1);
    SetAnnotationAttribute(Panel_Handle, Control_ID, Annotation_Index, ATTR_ANNOTATION_CAPTION_BGCOLOR, VAL_WHITE);
	SetAnnotationAttribute(Panel_Handle, Control_ID, Annotation_Index, ATTR_ANNOTATION_CAPTION_COLOR, VAL_BLACK);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestPanelDescription"
SElExceptionPtr _FUNCC SetTestPanelDescription(
    int32_t		control,
    const char*	description
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0; 

	if(gs_TestPanel)
	{
		switch (control)
		{
		case PANEL_RESULT_MSG : 
			
		EXCCHECKCVI( SetCtrlVal( gs_TestPanel, control, description));
		if (strcmp("TEST NOK", description) == 0)
		{
		SetCtrlIndex( gs_TestPanel, panelStatusStation[7], 1);
		EXCCHECKCVI( SetCtrlAttribute (gs_TestPanel, PANEL_RESULT_MSG, ATTR_TEXT_BGCOLOR, VAL_RED));
		}
		else if (strcmp("TEST OK", description) == 0)
		{
		SetCtrlIndex( gs_TestPanel, panelStatusStation[7], 0);
		EXCCHECKCVI( SetCtrlAttribute (gs_TestPanel, PANEL_RESULT_MSG, ATTR_TEXT_BGCOLOR, VAL_GREEN));
		}
		else 
		EXCCHECKCVI( SetCtrlAttribute (gs_TestPanel, PANEL_RESULT_MSG, ATTR_TEXT_BGCOLOR, VAL_WHITE));
			
		break;
		
		default : EXCCHECKCVI( SetCtrlVal( gs_TestPanel, control, description)); break;
		}
	}

Error:
    EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetPanelWarning"
SElExceptionPtr _FUNCC SetPanelWarning(
    int32_t		control,
	bool_t status
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0; 

	if(gs_TestPanel)
		EXCCHECKCVI(SetCtrlAttribute (gs_TestPanel, control, ATTR_VISIBLE, status));

Error:
    EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetPanelStationStatus"
SElExceptionPtr _FUNCC SetPanelStationStatus(
    int32_t		station,
	bool_t status
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0; 

	if(gs_TestPanel)
		EXCCHECKCVI(SetCtrlIndex( gs_TestPanel, panelStatusStation[station], status));

Error:
    EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestPanelMode"
SElExceptionPtr _FUNCC SetTestPanelMode(
    int32_t		mode
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0; 

	if(gs_TestPanel)
	{
		switch(mode)
		{
			case E_TSPM_STEP_BY_STEP_MODE:
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_RESULT_MSG, ATTR_VISIBLE, FALSE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_FORWARD, ATTR_VISIBLE, TRUE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_REPEAT, ATTR_VISIBLE, TRUE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_BACKWARD, ATTR_VISIBLE, TRUE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_STOP, ATTR_VISIBLE, TRUE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_FORWARD, ATTR_DIMMED, TRUE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_BACKWARD, ATTR_DIMMED, TRUE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_REPEAT, ATTR_DIMMED, TRUE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_STOP, ATTR_DIMMED, TRUE));
				break;
			default:
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_RESULT_MSG, ATTR_VISIBLE, TRUE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_FORWARD, ATTR_VISIBLE, FALSE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_REPEAT, ATTR_VISIBLE, FALSE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_BACKWARD, ATTR_VISIBLE, FALSE));
				EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_STOP, ATTR_VISIBLE, FALSE));
				//ARAIBIA ajouter autres pour rest panel
				break;
		}
	}

Error:
    EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestPanelReferences"
SElExceptionPtr _FUNCC SetTestPanelReferences(
    char* 	RefValeo,
    char* 	RefCustomer
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0; 

	if(gs_TestPanel)
	{
		EXCCHECKCVI( DefaultCtrl (gs_TestPanel, PANEL_REF_VALEO));
		EXCCHECKCVI( DefaultCtrl (gs_TestPanel, PANEL_REF_CUSTOMER));
		ProcessSystemEvents ();
		ProcessDrawEvents ();
		EXCCHECKCVI( SetCtrlVal( gs_TestPanel, PANEL_REF_VALEO, RefValeo));
		EXCCHECKCVI( SetCtrlVal( gs_TestPanel, PANEL_REF_CUSTOMER, RefCustomer));
		ProcessSystemEvents ();
		ProcessDrawEvents ();
	}

Error:
    EXCRETHROW( pexception);
}


#pragma warning( pop)
