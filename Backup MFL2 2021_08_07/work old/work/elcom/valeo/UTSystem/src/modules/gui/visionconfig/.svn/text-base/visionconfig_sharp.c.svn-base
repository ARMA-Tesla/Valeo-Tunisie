#include "nivision.h"
#include "global.h" 
#include "visionconfig.h" 
#include "visionconfig_fnc.h"  
#include "visionconfig_sharp.h"

extern int 			gs_panel[];  
extern SIMAQPtr 	gs_pIMAQ;
extern const char* 	gs_cameraName;

static bool_t		gs_sharpRun;   
static Image*		gs_sharpMask = NULL; 

#define NB_PLOT_POINTS			50
static struct _SPlot
{
	double		Max;
	double		plotY[NB_PLOT_POINTS];
	double		plotYmax[NB_PLOT_POINTS];
	int32_t		plotActualPos;
} SPlot;

static SElExceptionPtr SharpFnc(Image* pimage, double frate, double mean);
static SElExceptionPtr ShowSharpTool(void);
static SElExceptionPtr HideSharpTool(void);

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OpenSharpCamera"
SElExceptionPtr	OpenSharpCamera(void)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECK( ShowSharpTool());
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CloseSharpCamera"
SElExceptionPtr	CloseSharpCamera(void)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	Image*			pimage = NULL;
	
	EXCCHECK( HideSharpTool());
	
	EXCCHECKIMAQ( imaqSetWindowROI (IMAQ_WINDOW_NUMBER, NULL));  
	
	pimage = gs_pIMAQ->GetImage(gs_pIMAQ, INTERNAL_IMAGE_NAME);
	if(pimage)
	{
		EXCCHECKIMAQ( imaqClearOverlay(pimage, NULL));
		EXCCHECKIMAQ( imaqDisplayImage (pimage, IMAQ_WINDOW_NUMBER, FALSE));
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ShowSharpTool"
static SElExceptionPtr ShowSharpTool(void)
{
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	ToolWindowOptions	myToolOptions;	  
		
	/* Setup tools for window interaction */
	memset(&myToolOptions, 0, sizeof(myToolOptions));
	myToolOptions.showSelectionTool 		= TRUE;     
	myToolOptions.showZoomTool      		= TRUE;          
	myToolOptions.showPointTool     		= FALSE;         
	myToolOptions.showLineTool      		= FALSE;           
	myToolOptions.showRectangleTool 		= TRUE;     
	myToolOptions.showOvalTool      		= TRUE;          
	myToolOptions.showPolygonTool   		= TRUE;       
	myToolOptions.showClosedFreehandTool 	= FALSE;
	myToolOptions.showPolyLineTool 			= FALSE;      
	myToolOptions.showFreehandTool 			= FALSE;     
	myToolOptions.showAnnulusTool 			= FALSE;          
   	myToolOptions.showRotatedRectangleTool 	= FALSE;	
   	
	EXCCHECKIMAQ( imaqSetupToolWindow (FALSE, 20, &myToolOptions) );
	EXCCHECKIMAQ( imaqShowToolWindow (TRUE));
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "HideSharpTool"
static SElExceptionPtr HideSharpTool(void)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKIMAQ( imaqSetCurrentTool (IMAQ_NO_TOOL));
	EXCCHECKIMAQ( imaqShowToolWindow (FALSE)); 
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CameraSharpStart"
SElExceptionPtr CameraSharpStart(void)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int				average_count = 1;
	double			avarage_gain = 1.0;
	int				count = 0;
	ROI*			userROI = NULL;
	int32_t			tab;
	
	EXCCHECKCVI( GetCtrlVal(gs_panel[CAMCFG], CAMCFG_SNAP_AVERAGE, &average_count));
	EXCCHECKCVI( GetCtrlVal(gs_panel[CAMCFG], CAMCFG_SNAP_AVERAGE_GAIN, &avarage_gain));
	
	userROI = imaqGetWindowROI (IMAQ_WINDOW_NUMBER);
	count = imaqGetContourCount( userROI); 
	
	if(count>0)
	{
		if(gs_sharpMask==NULL)
		{
			gs_sharpMask = imaqCreateImage (IMAQ_IMAGE_U8, 2);
		}
		EXCCHECKIMAQ( imaqROIToMask( gs_sharpMask, userROI, 1, NULL, NULL));
	}
	else
	{
		imaqDispose(gs_sharpMask);
		gs_sharpMask = NULL;
	}
	
	/* set shared data */
	memset(&SPlot, 0, sizeof(SPlot));

	EXCCHECKCVI( GetPanelHandleFromTabPage (gs_panel[CAMCFG], CAMCFG_TAB, 0, &tab));
	EXCCHECKCVI( DeleteGraphPlot (tab, TABPANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW));
	EXCCHECKCVI( GetPanelHandleFromTabPage (gs_panel[CAMCFG], CAMCFG_TAB, 1, &tab));
	EXCCHECKCVI( DeleteGraphPlot (tab, TABPANEL_2_HISTOGRAM, -1, VAL_IMMEDIATE_DRAW));

	EXCCHECK( gs_pIMAQ->SharpStart(gs_pIMAQ, gs_cameraName, average_count, avarage_gain, gs_sharpMask, SharpFnc)); 
		
	EXCCHECK( HideSharpTool());

Error:
	if(userROI) imaqDispose(userROI);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CameraSharpStop"
SElExceptionPtr CameraSharpStop(void)
{
	SElExceptionPtr	pexception = NULL; 
	
	EXCCHECK(gs_pIMAQ->SharpStop(gs_pIMAQ, gs_cameraName)); 
	EXCCHECK( ShowSharpTool());
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SharpFnc"
static SElExceptionPtr SharpFnc(Image* pImage, double fRate, double mean)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
 	SPostDefDataPtr	pdata = NULL;
	double			x1, x2, y1, y2;
	int				index, tab;
		
	EXCCHECKCVI( GetActiveTabPage (gs_panel[CAMCFG], CAMCFG_TAB, &index));
	EXCCHECKCVI( GetPanelHandleFromTabPage (gs_panel[CAMCFG], CAMCFG_TAB, index, &tab));

	/* display image */
	EXCCHECK( MyDisplayFce(pImage, fRate, mean));
	
	if(index==0) /* camera sharp */
	{
		if( SPlot.Max<mean)
		{
			SPlot.Max = mean;
		}

		if(SPlot.plotActualPos>=NB_PLOT_POINTS)
		{
			SPlot.plotActualPos = 0;
			EXCCHECKCVI( DeleteGraphPlot (tab, TABPANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW));
		}
		SPlot.plotYmax[SPlot.plotActualPos] = SPlot.Max;
		SPlot.plotY[SPlot.plotActualPos] = mean;

		if(SPlot.plotActualPos>0)
		{
			x1 = SPlot.plotActualPos;
			x2 = SPlot.plotActualPos-1;

			y1 = SPlot.plotYmax[SPlot.plotActualPos];
			y2 = SPlot.plotYmax[SPlot.plotActualPos-1];
			EXCCHECKCVI( PlotLine (tab, TABPANEL_GRAPH, x1, y1, x2, y2, VAL_RED));

			y1 = SPlot.plotY[SPlot.plotActualPos];
			y2 = SPlot.plotY[SPlot.plotActualPos-1];
			EXCCHECKCVI( PlotLine (tab, TABPANEL_GRAPH, x1, y1, x2, y2, VAL_BLUE));
		}

		SPlot.plotActualPos++;
	}
	else if(index==1) /* picture histogram */
	{
		HistogramReport* histogramRpt_Ptr = NULL;

		histogramRpt_Ptr = imaqHistogram (pImage, 256, 0, 255, gs_sharpMask);

		if ( histogramRpt_Ptr )
		{
			EXCCHECKCVI( DeleteGraphPlot (tab, TABPANEL_2_HISTOGRAM, -1, VAL_DELAYED_DRAW));
			EXCCHECKCVI( PlotY( tab, TABPANEL_2_HISTOGRAM, histogramRpt_Ptr->histogram, 256, VAL_INTEGER,
								VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE));

			EXCCHECKIMAQ( imaqDispose(histogramRpt_Ptr));
		}
	}

Error:
	EXCRETHROW( pexception);
}