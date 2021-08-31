#include <exception/exception.h>
#include <profilometer/class/profilometers.h>
#include <labels/tester_labels.h>
#include <eventfactory/EventFactory.h>
#include "Profilometer_gui_plugin.h"
#include <ansi_c.h>
#include "Profilometer_gui.h"

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
#define __FUNC__ "{Profilometer Plugin}::Initialize"
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

	EXCCHECKCVI( gs_panel[PROFILO] = LoadPanelEx(main_panel,"Profilometer_gui.uir", PROFILO, __CVIUserHInst) );

	EXCCHECKCVI( SetPanelAttribute (gs_panel[PROFILO], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PROFILO], ATTR_LEFT, 0));

Error:
   	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Profilometer Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE(SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	 
	if( 0==strcmp( uuid, PLUGIN_PROFILOMETER))
	{
		EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        			"Modules",
	                        			sizeof(SObjListPtr),
	                        			&gs_Modules));
		
		EXCCHECKCVI( DisplayPanel(gs_panel[PROFILO]));
	}

Error:   
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Profilometer Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( HidePanel(gs_panel[PROFILO]));

Error:   
	EXCRETHROW( pexception);

}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Profilometer Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if(gs_panel[PROFILO])
	EXCCHECKCVI( DiscardPanel( gs_panel[PROFILO]));

Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Profilometer Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
   	SElExceptionPtr	pexception = NULL;  
	
	if (a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Profilometer Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif

int CVICALLBACK OnProfilometer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;  
	char	buffer[4096] = "";																				  
	char	text[4096] = "";
    int		deviceID;
	int32_t				error = 0;
	char	Value[32] = "";
	MEASURE_DATA pMeasure[16];

	switch (event)
	{
		case EVENT_COMMIT:
	
		if(gs_Modules)			  
		{
			SProfilometersPtr	pProfilometers = (SProfilometersPtr)gs_Modules->Get(gs_Modules, MODULE_PROFILOMETERS);  
			SProfilometerPtr	pProfilometer = NULL;

			GetCtrlVal (panel, PROFILO_DEVICE_ID, &deviceID);
			
			if ( deviceID >= sizeof(text))
			{
				EXCTHROW( -1, "Small Buffer for Profilometer label");
			}

			if ( pProfilometers )	   
			{
				char*	pend = buffer;

				EXCCHECK( pProfilometers->GetProfilometerByIndex(pProfilometers, deviceID, &pProfilometer));

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
						case PROFILO_START:
						EXCCHECK( pProfilometer->StartMeasure(pProfilometer));
						EXCCHECKCVI( SetCtrlAttribute(panel, PROFILO_START, ATTR_DIMMED, 1));
						break;
			
						case PROFILO_STOP:
						EXCCHECK( pProfilometer->StopMeasure(pProfilometer));
						EXCCHECKCVI( SetCtrlAttribute(panel, PROFILO_START, ATTR_DIMMED, 0));
						break;
						
						case PROFILO_MEASURE:
						 SetCtrlVal(panel, PROFILO_LED_HIGH_OUT_0, 0);
						 SetCtrlVal(panel, PROFILO_LED_GO_OUT_0, 0);
					 	 SetCtrlVal(panel, PROFILO_LED_LOW_OUT_0, 0);
						 
						 SetCtrlVal(panel, PROFILO_LED_HIGH_OUT_1, 0);
						 SetCtrlVal(panel, PROFILO_LED_GO_OUT_1, 0);
					 	 SetCtrlVal(panel, PROFILO_LED_LOW_OUT_1, 0);
						 
						 SetCtrlVal(panel, PROFILO_NUMERIC_OUT0, ""); 
						 SetCtrlVal(panel, PROFILO_NUMERIC_OUT1, ""); 
						 
						SetCtrlVal(panel, PROFILO_LED_ALARM_OUT_1, 0);
						SetCtrlVal(panel, PROFILO_LED_ALARM_OUT_1, 0);

	
						EXCCHECK( pProfilometer->GetMeasurementValue(pProfilometer, pMeasure));

						/* Out 0 */
						if (pMeasure[0].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
						{
						if (pMeasure[0].byJudge == LJV7IF_JUDGE_RESULT_HI)
							{
							 SetCtrlVal(panel, PROFILO_LED_HIGH_OUT_0, 1);
							 SetCtrlVal(panel, PROFILO_LED_GO_OUT_0, 0);
						 	 SetCtrlVal(panel, PROFILO_LED_LOW_OUT_0, 0);
							}
						
						if (pMeasure[0].byJudge == LJV7IF_JUDGE_RESULT_GO)
							{
							 SetCtrlVal(panel, PROFILO_LED_HIGH_OUT_0, 0);
							 SetCtrlVal(panel, PROFILO_LED_GO_OUT_0, 1);
						 	 SetCtrlVal(panel, PROFILO_LED_LOW_OUT_0, 0);
							}
											
						if (pMeasure[0].byJudge == LJV7IF_JUDGE_RESULT_LO)
							{
							 SetCtrlVal(panel, PROFILO_LED_HIGH_OUT_0, 0);
							 SetCtrlVal(panel, PROFILO_LED_GO_OUT_0, 0);
						 	 SetCtrlVal(panel, PROFILO_LED_LOW_OUT_0, 1);
							}
						
						sprintf (Value, "%fmm",pMeasure[0].fValue);
						SetCtrlVal(panel, PROFILO_NUMERIC_OUT0, Value); 
						}
						else 
						SetCtrlVal(panel, PROFILO_LED_ALARM_OUT_0, 1);
						/* Out 1 */
						if (pMeasure[1].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
						{
						if (pMeasure[1].byJudge == LJV7IF_JUDGE_RESULT_HI)
							{
							 SetCtrlVal(panel, PROFILO_LED_HIGH_OUT_1, 1);
							 SetCtrlVal(panel, PROFILO_LED_GO_OUT_1, 0);
						 	 SetCtrlVal(panel, PROFILO_LED_LOW_OUT_1, 0);
							}
						
						if (pMeasure[1].byJudge == LJV7IF_JUDGE_RESULT_GO)
							{
							 SetCtrlVal(panel, PROFILO_LED_HIGH_OUT_1, 0);
							 SetCtrlVal(panel, PROFILO_LED_GO_OUT_1, 1);
						 	 SetCtrlVal(panel, PROFILO_LED_LOW_OUT_1, 0);
							}
											
						if (pMeasure[1].byJudge == LJV7IF_JUDGE_RESULT_LO)
							{
							 SetCtrlVal(panel, PROFILO_LED_HIGH_OUT_1, 0);
							 SetCtrlVal(panel, PROFILO_LED_GO_OUT_1, 0);
						 	 SetCtrlVal(panel, PROFILO_LED_LOW_OUT_1, 1);
							}
						
						sprintf (Value, "%fmm",pMeasure[1].fValue);
						SetCtrlVal(panel, PROFILO_NUMERIC_OUT1, Value); 
						}
						else 
					    SetCtrlVal(panel, PROFILO_LED_ALARM_OUT_1, 1);
						break;
				
						case PROFILO_CLEAR:
						SetCtrlVal(panel, PROFILO_LED_ALARM_OUT_1, 1);
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
