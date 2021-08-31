#include "global.h"
#include "toolbox.h"
#include <utility.h>
#include "visionconfig.h"
#include "visionconfig_fnc.h" 
#include "panel_task.h"

extern int 		gs_panel[];
extern SIMAQPtr	gs_pIMAQ;

static char* gs_math_operations[] = { "subtract", "add", "multiply", "inversion"}; 

static const char* GetActualMathOperation( int panel);
static SElExceptionPtr DoStep(int panel);
static SElExceptionPtr XmlSaveChanges(int panel, int control);
static SElExceptionPtr GetSecondName(int panel, char* buffer);  
static SElExceptionPtr SetActualMathOperation( int panel, const char* operation);
static SElExceptionPtr SetActualImageName( int panel, const char* pImageName);

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "InitializeMathOperationDialog"
SElExceptionPtr InitializeMathOperationDialog(void)
{
	SElExceptionPtr		pexception = NULL;   
	int32_t				error = 0;
	HRESULT         	hr = 0;  
	VBOOL         		is_xml_success = 1;   
	ToolWindowOptions	myToolOptions;	
	SGlobalPtr			pglobal = GetGlobal(NULL);   
	
	EXCCHECK( DisplayAllBuffers( gs_panel[PANMATH], PANMATH_IMAGE_BUFFER));
	EXCCHECKCVI( InsertListItem (gs_panel[PANMATH], PANMATH_IMAGE_BUFFER, 0, "Source Image", -1));
	EXCCHECKCVI( InsertListItem (gs_panel[PANMATH], PANMATH_IMAGE_BUFFER, -1, "Constant", 999)); 

	if( pglobal->step!=0) /* edit step */
	{
		double	dvalue;   
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step, NULL, VFALSE, &pglobal->step_clone));   
		
		XML_NODE_GET_TEXT(pglobal->step, "@name",
		{
			EXCCHECKCVI( SetCtrlVal(gs_panel[PANMATH], PANMATH_SNAME, ptext));
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "operation",
		{
			EXCCHECK( SetActualMathOperation( gs_panel[PANMATH],  ptext));
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "image_name",
		{
			EXCCHECK( SetActualImageName( gs_panel[PANMATH],  ptext));
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANMATH], PANMATH_CONSTANT, 0.0)); 	
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "constant",
		{
			EXCCHECK( SetActualImageName( gs_panel[PANMATH],  "Constant"));
			dvalue = strtod(ptext, NULL);
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANMATH], PANMATH_CONSTANT, (float)dvalue));   	
		});
	}
	else /* create new step */
	{
		VARIANT	type = CA_VariantEmpty ();
		
		EXCCHECKCVI( SetCtrlVal( gs_panel[PANMATH], PANMATH_SNAME, "")); 
		
		CA_VariantSetCString (&type, "Element");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateNode (pglobal->pxml, NULL, type, "step", "", &pglobal->step_clone));
		EXCCHECKMSXML( MSXML_IXMLDOMElementsetAttribute (pglobal->step_clone, NULL, "name", CA_VariantEmpty ()));
	}
	
	/* Setup tools for window interaction */
	memset(&myToolOptions, 0, sizeof(myToolOptions));
	myToolOptions.showSelectionTool 		= FALSE;     
	myToolOptions.showZoomTool      		= TRUE;          
	myToolOptions.showPointTool     		= FALSE;         
	myToolOptions.showLineTool      		= FALSE;           
	myToolOptions.showRectangleTool 		= FALSE;     
	myToolOptions.showOvalTool      		= FALSE;          
	myToolOptions.showPolygonTool   		= FALSE;       
	myToolOptions.showClosedFreehandTool 	= FALSE;
	myToolOptions.showPolyLineTool 			= FALSE;      
	myToolOptions.showFreehandTool 			= TRUE;     
	myToolOptions.showAnnulusTool 			= FALSE;          
   	myToolOptions.showRotatedRectangleTool 	= FALSE;	
   	
	EXCCHECKIMAQ( imaqSetupToolWindow (FALSE, 20, &myToolOptions) );
	EXCCHECKIMAQ( imaqShowToolWindow (TRUE));
	
	EXCCHECKCVI( SetPanelPos (gs_panel[PANMATH], 15, 0));
	EXCCHECKCVI( DisplayPanel (gs_panel[PANMATH]));	
	EXCCHECKCVI( SetActivePanel (gs_panel[PANMATH]));
	
	/* preview */
	EXCCHECKCVI( OnPanelMathPreview( gs_panel[PANMATH], PANMATH_PREVIEW, EVENT_COMMIT, NULL, 0, 0 ));
	
Error:
	EXCRETHROW( pexception);     
}

/*----------------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "DoPreview"
static SElExceptionPtr DoPreview( int panel)
{
	SElExceptionPtr	pexception = NULL;    
	int32_t			error = 0;
	int				is_preview = 0;
	
	EXCCHECKCVI( GetCtrlVal (panel, PANMATH_PREVIEW, &is_preview));			
	if ( is_preview )
		EXCCHECK( DoStep( panel));

Error:
	EXCRETHROW( pexception);   
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelMathPreview"
int CVICALLBACK OnPanelMathPreview (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	switch (event)
	{
		case EVENT_COMMIT:
									
			switch ( control)
			{
				case PANMATH_BPREVIEW:
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
#define __FUNC__ "OnPanelMathExit"
int CVICALLBACK OnPanelMathExit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;
	int				operation;  
	SGlobalPtr		pglobal = GetGlobal(NULL);     

	switch (event)
		{
		case EVENT_COMMIT:
			if ( control == PANMATH_OK )
			{
				EXCCHECKCVI( GetCtrlVal(panel, PANMATH_MATH_OP, &operation)); 
				
				if ( operation >= 0 )
				{
					EXCCHECK( CheckStepName( panel, PANMATH_SNAME));
					EXCCHECK( XmlSaveChanges( panel, PANMATH_SNAME));
					EXCCHECK( InitializeTaskDialog());
				}
				else
				{
					MessagePopup( "Warning", _("Operation has not been selected. Step has not been created."));
				}
			}
			else if( control==PNLTASKS_CANCEL)
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
	double			fvalue = 0.0;
	const char*		math_op = NULL;

	/* step name */
	EXCCHECKCVI( GetCtrlVal( panel, control, name));
	RemoveSurroundingWhiteSpace(name);

	XML_NODE_SET_TEXT(pstep, "@name", name); 
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "type", "math"); 
	math_op = GetActualMathOperation(panel);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "operation", math_op);  
	
	if(0!=strcmp(math_op, "inversion"))
	{
		EXCCHECK( GetSecondName(panel, name));
		if( 0 == strcmp(name, "Constant") )
		{
			EXCCHECKCVI( GetCtrlVal(panel, PANMATH_CONSTANT, &fvalue));
			sprintf(buffer, "%f", fvalue);
			XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "constant", buffer);   
		}
		else
		{
			XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "image_name", name);   
		}
	}

Error:
	EXCRETHROW( pexception);
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "XmlSaveChanges"
static SElExceptionPtr	XmlSaveChanges(int panel, int control)
{
	SElExceptionPtr	pexception = NULL;         
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL); 
	VBOOL         	is_xml_success = 1; 
	HRESULT         hr = 0;  
	

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
#define __FUNC__ "GetActualMathOperation"
static const char* GetActualMathOperation( int panel)
{
	int		value;
	
	static char* gs_empty = "";
	
	GetCtrlVal( panel, PANMATH_MATH_OP, &value);
	
	if ( value >= (sizeof(gs_math_operations)/sizeof(gs_math_operations[0])) )
	{
		MessagePopup("Error", _("Unknown Math Operation!"));
		return gs_empty;
	}
	else
	if (value < 0 )
	{
		return gs_empty;	
	}
	
	return gs_math_operations[value];
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetActualMathOperation"
static SElExceptionPtr SetActualMathOperation( int panel, const char* operation)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	int				value;
	int				count = (sizeof(gs_math_operations)/sizeof(gs_math_operations[0]));
	int				found = 0;
	
	for ( value = 0 ; value < count; value++)
	{
		if ( stricmp( operation, gs_math_operations[value]) == 0 )
		{
			EXCCHECKCVI( SetCtrlVal( panel, PANMATH_MATH_OP, value)); 
			found = 1; break;
		}
	}
	
	if ( !found )
	{
		char	message[256] = "";
		char*   pmessage = message;
		
		pmessage += sprintf( pmessage, _("Unknown Math Operation!"));
		pmessage += sprintf( pmessage,  "\n%s", operation);
		MessagePopup( "Error", message);	
	}
	
Error:
	EXCRETHROW( pexception);
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetActualImageName"
static SElExceptionPtr SetActualImageName( int panel, const char* pImageName)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	int				count;
	int				index;
	char			name[256];
	
	EXCCHECKCVI( GetNumListItems( panel, PANMATH_IMAGE_BUFFER, &count));
	for( index = 0; index < count; index++)
	{
		EXCCHECKCVI( GetLabelFromIndex( panel, PANMATH_IMAGE_BUFFER, index, name));
		
		if ( index == 0
			&& stricmp( "@self", pImageName) == 0 )
		{
			EXCCHECKCVI( SetCtrlIndex( panel, PANMATH_IMAGE_BUFFER, index)); 	
			break;
		}
		if ( stricmp(name, pImageName) == 0)
		{
			EXCCHECKCVI( SetCtrlIndex( panel, PANMATH_IMAGE_BUFFER, index)); 	
			break;
		}
	}
	
	if ( index >= count )
	{
		char	message[256] = "";
		sprintf(message, _("The specified image name \"%s\" has not befound."), pImageName);
		MessagePopup( "Warning", message); 	
	}

Error:
	EXCRETHROW( pexception);
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetSecondName"
static SElExceptionPtr GetSecondName(int panel, char* buffer)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	int				index;
	
	EXCCHECKCVI( GetCtrlIndex(panel, PANMATH_IMAGE_BUFFER, &index));
	if ( index )
	{
		EXCCHECKCVI( GetLabelFromIndex(panel, PANMATH_IMAGE_BUFFER, index, buffer));
	}
	else
	{
		strcpy(buffer, PICTURE_SELF_IMAGE_NAME);	
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "DoStep"
static SElExceptionPtr DoStep(int panel)
{
	SElExceptionPtr pexception = NULL;    
	int32_t			error = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
	SELStringPtr	result = NULL;
	SGlobalPtr		pglobal = GetGlobal(NULL); 
	CAObjHandle 	pstep = 0;

	/* check if operation was selected */
	if(0==strcmp("", GetActualMathOperation(panel)))
		goto Error;

	/* Set data for the step */
	if(pglobal->step)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step, NULL, VFALSE, &pstep));
	}
	else if(pglobal->step_clone)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step_clone, NULL, VFALSE, &pstep));
	}
	EXCCHECK( StepData(panel, PANMATH_SNAME, pglobal->pxml, pstep));

	/* Do Step */
	EXCCHECK( elstring_new("<step>\n", &result));
	EXCCHECK( gs_pIMAQ->DoMathStep(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, pstep, result));
	EXCCHECK( result->Append(result, "</step>\n"));

	/* display result image */
	EXCCHECK( ResultToView( result));

Error:	
	CA_DiscardObjHandle(pstep);
	elstring_delete( &result);
	EXCRETHROW( pexception);    
}