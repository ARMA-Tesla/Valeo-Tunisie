#include "global.h"
#include <toolbox.h>
#include "visionconfig.h"
#include "visionconfig_fnc.h" 
#include "panel_task.h"

extern int 		gs_panel[];
extern SIMAQPtr	gs_pIMAQ;

static SElExceptionPtr	XmlSaveChanges(int panel,int	control);
static SElExceptionPtr	DoStep( int panel);   
static SElExceptionPtr	DoPreview( int panel);

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "InitializeResampleDialog"
SElExceptionPtr InitializeResampleDialog(void)
{
	SElExceptionPtr		pexception = NULL;   
	int32_t				error = 0;
	HRESULT         	hr = 0;   
	VBOOL         		is_xml_success = 1;
	ToolWindowOptions	myToolOptions;	
	SGlobalPtr			pglobal = GetGlobal(NULL); 
	float				fvalue;
	int					ivalue;
	 
	SetCtrlVal( gs_panel[PANRES], PANRES_SNAME, "");
	
	if ( pglobal->step!=0) /* edit step */
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step, NULL, VFALSE, &pglobal->step_clone)); 
		
		XML_NODE_GET_TEXT(pglobal->step, "@name",
		{
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANRES], PANRES_SNAME, ptext));
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "constant",
		{
			fvalue = strtod(ptext, NULL); 
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANRES], PANRES_CONSTANT, fvalue)); 
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "interpolation",
		{
			ivalue = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlIndex( gs_panel[PANRES], PANRES_INTERPOL, ivalue));    
		});
	}
	else /* create new step */
	{
		VARIANT	type = CA_VariantEmpty ();
		
		EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_SNAME, "")); 
		
		CA_VariantSetCString (&type, "Element");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateNode (pglobal->pxml, NULL, type, "step", "", &pglobal->step_clone));
		EXCCHECKMSXML( MSXML_IXMLDOMElementsetAttribute (pglobal->step_clone, NULL, "name", CA_VariantEmpty ()));
	}
	
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
	
	EXCCHECKCVI( SetPanelPos (gs_panel[PANRES], 15, 0));
	EXCCHECKCVI( DisplayPanel (gs_panel[PANRES]));
	EXCCHECKCVI( SetActivePanel (gs_panel[PANRES]));  
	
Error:
	EXCRETHROW( pexception);  
}

/*-------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "OnPanelResamplePreview"
int CVICALLBACK OnPanelResamplePreview (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch(control)
			{
				case PANRES_BPREVIEW:
					EXCCHECK( DoStep(panel));
					break;
				default:
					EXCCHECK( DoPreview(panel));
					break;
			}
			break;
	}
	
Error:
	EXCDISPLAY( pexception);     
	return 0;
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelResampleExit"
int CVICALLBACK OnPanelResampleExit(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);   

	switch (event)
	{
		case EVENT_COMMIT:
			if ( control == PANRES_OK )
			{
				EXCCHECK( CheckStepName( panel, PANRES_SNAME));
				EXCCHECK( XmlSaveChanges( panel, PANRES_SNAME));
				EXCCHECK( InitializeTaskDialog());
			}
			else if( control==PANRES_CANCEL)
			{
				/* clean clone */
				EXCCHECKCVI( CA_DiscardObjHandle(pglobal->step_clone));
				pglobal->step_clone = 0;
			}
			
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
	VBOOL         	is_xml_success = 1; 
	HRESULT         hr = 0; 
	char			name[256];
	char			buffer[256];
	int32_t			ivalue = 0;
	double			dvalue = 0.0;

	/* step name */
	EXCCHECKCVI( GetCtrlVal( panel, control, name));
	RemoveSurroundingWhiteSpace(name);

	XML_NODE_SET_TEXT(pstep, "@name", name);   
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "type", "resample");

	EXCCHECKCVI( GetCtrlVal( panel, PANRES_CONSTANT, &dvalue));
	sprintf(buffer, "%Lf", dvalue); 
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "constant", buffer); 

	EXCCHECKCVI( GetCtrlIndex( panel, PANRES_INTERPOL, &ivalue));
	sprintf(buffer, "%d", ivalue); 
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "interpolation", buffer); 

Error:
	EXCRETHROW( pexception);
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "XmlSaveChanges"
static SElExceptionPtr	XmlSaveChanges(
	int panel,
	int	control
)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
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
/*----------------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DoPreview"
static SElExceptionPtr	DoPreview( int panel)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	int				is_preview = 0;
	
	EXCCHECKCVI( GetCtrlVal (panel, PANRES_PREVIEW, &is_preview));			
	if ( is_preview )
		EXCCHECK( DoStep( panel));

Error:
	EXCRETHROW( pexception);  
}

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "DoStep"
static SElExceptionPtr DoStep( int panel) 
{
	SElExceptionPtr pexception = NULL;    
	int32_t			error = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
	SELStringPtr	result = NULL;
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
	EXCCHECK( StepData(panel, PANRES_SNAME, pglobal->pxml, pstep));

	/* Do Step */
	EXCCHECK( elstring_new("<step>\n", &result));
	EXCCHECK( gs_pIMAQ->DoResampleStep(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, pstep, result));
	EXCCHECK( result->Append(result, "</step>\n"));

	/* display result image */
	EXCCHECK( ResultToView( result));

Error:	
	CA_DiscardObjHandle(pstep);
	elstring_delete( &result);
	EXCRETHROW( pexception);    
}