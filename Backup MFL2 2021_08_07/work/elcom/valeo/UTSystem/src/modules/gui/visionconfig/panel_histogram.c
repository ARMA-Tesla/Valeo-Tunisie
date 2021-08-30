#include <userint.h>
#include "global.h"
#include "toolbox.h"
#include <bklib/bklib2.h>
#include "visionconfig.h"
#include "visionconfig_fnc.h" 
#include "panel_task.h"

extern int 		gs_panel[];
extern SIMAQPtr	gs_pIMAQ;

static SElExceptionPtr DoStep(int panel);
static SElExceptionPtr XmlSaveChanges(int panel,int	control);
static SElExceptionPtr CreateSearchRegion( int panel);
static SElExceptionPtr StepData(int panel, int control, CAObjHandle pxml, CAObjHandle pstep);
void IMAQ_CALLBACK histogramEventCallback (WindowEventType event, int windowNumber, Tool tool, Rect rect); 

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "InitializeHistogramDialog"
SElExceptionPtr InitializeHistogramDialog(void)
{
	SElExceptionPtr 	pexception = NULL; 
	int32_t				error = 0;
	HRESULT         	hr = 0;   
	VBOOL         		is_xml_success = 1;
	ToolWindowOptions	myToolOptions;	
	SGlobalPtr			pglobal = GetGlobal(NULL);   
	Image*				pimage = NULL;
	short				AXvalue;

	EXCCHECK( CreateSearchRegion(gs_panel[PANHIST]));	
	 
	EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_SREGION, 0));	
	EXCCHECKCVI( DeleteGraphPlot (gs_panel[PANHIST], PANHIST_HISTOGRAM, -1, VAL_DELAYED_DRAW)); 

	EXCCHECKCOM( BKLib2__DBKTreeFromString( pglobal->bktreeH, 
											NULL, 
											BKTREE_RESULT_HEADER
											"|root|Step^i0\n", 
											"$", "\n",
											&AXvalue));
	EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_MIN, 0));
	EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_MAX, 255));
	
	if ( pglobal->step!=0) /* edit step */ 
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step, NULL, VFALSE, &pglobal->step_clone)); 
		
		XML_NODE_GET_TEXT(pglobal->step, "@name",
		{
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_SNAME, ptext));
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "region",
		{
			EXCCHECK( CreateRegion( IMAQ_WINDOW_NUMBER, ptext));
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_REGION_STR, ptext));
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANHIST], PANHIST_SREGION, 1));
		});
		XML_NODE_GET_TEXT(pglobal->step, "min_level",
		{
			int level = atoi(ptext);
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_MIN, level));
		});
		XML_NODE_GET_TEXT(pglobal->step, "max_level",
		{

			int level = atoi(ptext);
			level = (level == 0 ) ? 255 : level;
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_MAX, level));
		});

		XML_NODE_GET_TEXT(pglobal->step, "min_algorithm",
		{

			int alg = atoi(ptext);
			
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], (alg) ? PANHIST_MIN2 : PANHIST_MIN1,  1));
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], (alg) ? PANHIST_MIN1 : PANHIST_MIN2,  0));
			
			EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANHIST], (alg) ? PANHIST_MIN_PRC : PANHIST_MIN, ATTR_DIMMED,  0));
			EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANHIST], (alg) ? PANHIST_MIN : PANHIST_MIN_PRC, ATTR_DIMMED,  1));
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "min_perc",
		{
			double val = atof(ptext);
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_MIN_PRC,  val));
		});
	}
	else /* create new step */
	{
		VARIANT	type = CA_VariantEmpty ();
		
		EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_SNAME, "")); 
		
		CA_VariantSetCString (&type, "Element");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateNode (pglobal->pxml, NULL, type, "step", "", &pglobal->step_clone));
		EXCCHECKMSXML( MSXML_IXMLDOMElementsetAttribute (pglobal->step_clone, NULL, "name", CA_VariantEmpty ()));
	}

	EXCCHECKIMAQ( imaqSetEventCallback (histogramEventCallback, TRUE));
	
	pimage = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME);   
	EXCCHECKIMAQ( imaqClearOverlay(pimage, NULL));	
	EXCCHECKIMAQ( imaqDisplayImage (pimage, IMAQ_WINDOW_NUMBER, FALSE));
	
	/* Setup tools for window interaction */
	memset(&myToolOptions, 0, sizeof(myToolOptions));
	myToolOptions.showSelectionTool 		= TRUE;     
	myToolOptions.showZoomTool      		= TRUE;          
	myToolOptions.showPointTool     		= FALSE;         
	myToolOptions.showLineTool      		= TRUE;           
	myToolOptions.showRectangleTool 		= TRUE;     
	myToolOptions.showOvalTool      		= TRUE;          
	myToolOptions.showPolygonTool   		= TRUE;       
	myToolOptions.showClosedFreehandTool 	= FALSE;
	myToolOptions.showPolyLineTool 			= FALSE;      
	myToolOptions.showFreehandTool 			= TRUE;     
	myToolOptions.showAnnulusTool 			= FALSE;          
   	myToolOptions.showRotatedRectangleTool 	= FALSE;	
   	
	EXCCHECKIMAQ( imaqSetupToolWindow (FALSE, 20, &myToolOptions) );
	EXCCHECKIMAQ( imaqShowToolWindow (TRUE));
	
	EXCCHECKCVI( SetPanelPos (gs_panel[PANHIST], 15, 0));
	EXCCHECKCVI( DisplayPanel (gs_panel[PANHIST]));	
	
Error:
	EXCRETHROW( pexception);    
}

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelHistogramPreview"
int CVICALLBACK OnPanelHistogramPreview (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	Rect 			rect;
	
	switch (event)
	{
		case EVENT_COMMIT:
			memset(&rect, 0, sizeof(Rect));			
			histogramEventCallback(IMAQ_DRAW_EVENT, 0, IMAQ_NO_TOOL, rect);
			break;
	}
	
Error:
	EXCDISPLAY( pexception);    
	return 0;
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelHistogramExit"
int CVICALLBACK OnPanelHistogramExit(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);  

	switch (event)
		{
		case EVENT_COMMIT:
			if ( control == PANHIST_OK )
			{
				EXCCHECK( CheckStepName( panel, PANHIST_SNAME));
				EXCCHECK( XmlSaveChanges( panel, PANHIST_SNAME));
				EXCCHECK( InitializeTaskDialog());
			}
			else if( control==PANHIST_CANCEL)
			{
				/* clean clone */
				EXCCHECKCVI( CA_DiscardObjHandle(pglobal->step_clone));
				pglobal->step_clone = 0;
			}
			
			EXCCHECKIMAQ( imaqSetEventCallback (NULL, TRUE));
			EXCCHECKIMAQ( imaqSetCurrentTool (IMAQ_NO_TOOL));
			EXCCHECKIMAQ( imaqShowToolWindow (FALSE)); 
			EXCCHECKIMAQ( imaqSetWindowROI (IMAQ_WINDOW_NUMBER, NULL));
			EXCCHECKCVI( HidePanel( panel));
			pglobal->step = 0;
			break;
		}
	
Error:
	EXCDISPLAY( pexception);    
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "StepData"
static SElExceptionPtr StepData(
	int				panel,
	int				control,
	CAObjHandle		pxml,
	CAObjHandle		pstep
)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
	char			buffer[256] = "";
	char			name[MAX_STEP_NAME];
	int				ival;
	double			dval;

	/* step name */
	EXCCHECKCVI( GetCtrlVal( panel, control, name));
	RemoveSurroundingWhiteSpace(name);

	XML_NODE_SET_TEXT(pstep, "@name", name);   
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "type", "histogram");

	EXCCHECKCVI( GetCtrlVal( panel, PANHIST_REGION_STR, buffer));
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "region", buffer);   

	EXCCHECKCVI( GetCtrlVal( panel, PANHIST_MIN, &ival));	
	sprintf(buffer, "%d", ival);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "min_level", buffer);   
	
	EXCCHECKCVI( GetCtrlVal( panel, PANHIST_MAX, &ival));	
	sprintf(buffer, "%d", ival);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "max_level", buffer);   

	
	EXCCHECKCVI( GetCtrlVal(panel, PANHIST_MIN2, &ival));
	sprintf(buffer, "%d", ival);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "min_algorithm", buffer);
	
	EXCCHECKCVI( GetCtrlVal(panel, PANHIST_MIN_PRC, &dval));
	sprintf(buffer, "%Lf", dval);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "min_perc", buffer);

Error:
	EXCRETHROW( pexception);
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "XmlSaveChanges"
static SElExceptionPtr	XmlSaveChanges(int panel,int control)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
	char			name[MAX_STEP_NAME] = "";
	char			buffer[256] = "";
	SGlobalPtr		pglobal = GetGlobal(NULL);   

	if(pglobal->task_clone && pglobal->step_clone)
	{
		/* step data */
		EXCCHECK( StepData(panel, control, pglobal->pxml, pglobal->step_clone));
		
		/* save changes */
		XML_STEP_SAVE_CHANGES();
	}

Error:
	EXCRETHROW( pexception);    
}

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "histogramEventCallback"
void IMAQ_CALLBACK histogramEventCallback(WindowEventType event, int windowNumber, Tool tool, Rect rect)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	static 			s_plot_handle = -1;
	ROI*			userROI = NULL;
	Image*			histogramMask = NULL;
	Image*			pimage = NULL;
	char			message[1024] = "";
	HistogramReport*	histogramRpt_Ptr = NULL;

	switch (event)
	{
  		case IMAQ_DRAW_EVENT:
					
			histogramMask = imaqCreateImage (IMAQ_IMAGE_U8, 2);
			if( histogramMask==NULL)
			{
				EXCCHECKIMAQ(0);
			}

			userROI = imaqGetWindowROI (IMAQ_WINDOW_NUMBER); 
			if( userROI==NULL)
			{
				EXCCHECKIMAQ(0);
			}
			EXCCHECKIMAQ( imaqROIToMask( histogramMask, userROI, 1, NULL, NULL));

			pimage = gs_pIMAQ->GetImage(gs_pIMAQ, INTERNAL_IMAGE_NAME);
			if(pimage)
			{
				int min, max;
				int alg;
				
				EXCCHECKCVI( GetCtrlVal( gs_panel[PANHIST], PANHIST_MIN2, &alg));
				EXCCHECKCVI( GetCtrlVal( gs_panel[PANHIST], PANHIST_MIN, &min));
				EXCCHECKCVI( GetCtrlVal( gs_panel[PANHIST], PANHIST_MAX, &max));
				
				if ( alg )
				{
					min = 0;
				}

				histogramRpt_Ptr = imaqHistogram (pimage, (max + 1 - min), min, max, histogramMask);
				if( histogramRpt_Ptr==NULL)
				{
					EXCCHECKIMAQ(0);
				}

				if ( alg )
				{
					double	min_perc;
					
					EXCCHECKCVI( GetCtrlVal( gs_panel[PANHIST], PANHIST_MIN_PRC, &min_perc));
					
					min = histogramRpt_Ptr->max / 100.0 * min_perc;

					imaqDispose (histogramMask); histogramMask = NULL;
				histogramRpt_Ptr = imaqHistogram (pimage, (max + 1 - min), min, max, histogramMask);
				if( histogramRpt_Ptr==NULL)
				{
					EXCCHECKIMAQ(0);
				}
				}
			
				EXCCHECKCVI( DeleteGraphPlot (gs_panel[PANHIST], PANHIST_HISTOGRAM, -1, VAL_DELAYED_DRAW));

				EXCCHECKCVI( SetAxisScalingMode ( gs_panel[PANHIST], PANHIST_HISTOGRAM, VAL_BOTTOM_XAXIS, VAL_MANUAL, min, max));
				
				s_plot_handle = PlotY( gs_panel[PANHIST],
									   PANHIST_HISTOGRAM,
									   histogramRpt_Ptr->histogram,
									   histogramRpt_Ptr->histogramCount,
									   VAL_INTEGER,
									   VAL_THIN_LINE,
									   VAL_EMPTY_SQUARE,
									   VAL_SOLID, 1,
									   VAL_BLUE);
			
				EXCCHECK( FormatRegion( 1, 0, message, sizeof(message)));
				EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_REGION_STR, message));
				EXCCHECKCVI( SetCtrlVal( gs_panel[PANHIST], PANHIST_SREGION, 1));
			}
			EXCCHECK( DoStep(gs_panel[PANHIST]));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	if(userROI) imaqDispose (userROI);
	if(histogramMask) imaqDispose (histogramMask);
	if(histogramRpt_Ptr) imaqDispose (histogramRpt_Ptr);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CreateSearchRegion"
static SElExceptionPtr CreateSearchRegion( int panel)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	Image*			pimage = NULL;
	int				left = 0, width, top = 0, height;
	char			pregionnew[50];  
	
	pimage = gs_pIMAQ->GetImage(gs_pIMAQ, INTERNAL_IMAGE_NAME);
	
	EXCCHECKIMAQ( imaqGetImageSize( pimage, &width, &height));
	sprintf(pregionnew, "rect(%d,%d,%d,%d)", left, top, width, height);
	
	EXCCHECKCVI( SetCtrlVal( panel, PANHIST_REGION_STR, pregionnew)); 
	EXCCHECK( CreateRegion( IMAQ_WINDOW_NUMBER, pregionnew));

Error:
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "DoStep"
static SElExceptionPtr DoStep( int panel)
{
	SElExceptionPtr pexception = NULL;    
	int32_t			error = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
	SELStringPtr	result = NULL;
	Image*			pimage = NULL;
	SGlobalPtr		pglobal = GetGlobal(NULL); 
	CAObjHandle 	pstep = 0;

	/* Set data for the step */
	if(pglobal->step)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step, NULL, VFALSE, &pstep));
	}
	else if(pglobal->step_clone)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step_clone, NULL, VFALSE, &pstep));
	}
	EXCCHECK( StepData(panel, PANHIST_SNAME, pglobal->pxml, pstep));

	/* Do Step */
	EXCCHECK( elstring_new("<step>\n", &result));
	EXCCHECK( gs_pIMAQ->DoHistogramStep(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, pstep, result));
	EXCCHECK( result->Append(result, "</step>\n"));

	EXCCHECK( ResultToBktree( pglobal->bktreeH, result));

Error:	
	CA_DiscardObjHandle(pstep);
	elstring_delete( &result);
	EXCRETHROW( pexception);    
}

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelHistogramRange"
int CVICALLBACK OnPanelHistogramRange (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int				min, max;
	int	preview = 0;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, PANHIST_MIN, &min));
			EXCCHECKCVI( GetCtrlVal( panel, PANHIST_MAX, &max));
			if ( min >= max )
			{
				if ( control == PANHIST_MIN ) /* MIN has been changed */
				{
					EXCCHECKCVI( SetCtrlVal( panel, PANHIST_MAX, min));
					if ( min == 255 ) SetCtrlVal( panel, PANHIST_MIN, 254);
				}
				else
				{
					EXCCHECKCVI( SetCtrlVal( panel, PANHIST_MIN, max));
					if ( max == 0 ) SetCtrlVal( panel, PANHIST_MAX, 1);
				}
			}
			EXCCHECKCVI( GetCtrlVal( panel, PANHIST_PREVIEW, &preview));
			if ( preview )
			{
				Rect 			rect;
				memset(&rect, 0, sizeof(Rect));			
				histogramEventCallback(IMAQ_DRAW_EVENT, 0, IMAQ_NO_TOOL, rect);
			}
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}


/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnSelectedMin"
int CVICALLBACK OnSelectedMin(
	int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( SetCtrlVal(panel, control, 1));
			
			if ( control == PANHIST_MIN1 )
			{
				EXCCHECKCVI( SetCtrlVal(panel, PANHIST_MIN2, 0));
				EXCCHECKCVI( SetCtrlAttribute(panel, PANHIST_MIN, ATTR_DIMMED, 0));
				EXCCHECKCVI( SetCtrlAttribute(panel, PANHIST_MIN_PRC, ATTR_DIMMED, 1));
			}
			else
			{
				EXCCHECKCVI( SetCtrlVal(panel, PANHIST_MIN1, 0));
				EXCCHECKCVI( SetCtrlAttribute(panel, PANHIST_MIN, ATTR_DIMMED, 1));
				EXCCHECKCVI( SetCtrlAttribute(panel, PANHIST_MIN_PRC, ATTR_DIMMED, 0));
			}	
			
			break;
	}
Error:
	EXCDISPLAY( pexception);
	return 0;
}
