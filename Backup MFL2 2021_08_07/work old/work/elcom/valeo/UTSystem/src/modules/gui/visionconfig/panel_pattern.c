#include "global.h"
#include "toolbox.h"
#include <nivision.h>
#include <bklib/bklib2.h>
#include "NIMachineVision.h"
#include "visionconfig.h"
#include "visionconfig_fnc.h"  
#include "panel_task.h"  
#include "panel_pattern.h"

extern int 		gs_panel[];
extern SIMAQPtr	gs_pIMAQ;

static Image*	gs_TemplateImage;

static SElExceptionPtr DoStep(int panel);
static SElExceptionPtr XmlSaveChanges(int panel,int	control);
static SElExceptionPtr DisplayTemplateImage(int panel,int control,const Image*	pImage );
static SElExceptionPtr DoPreview( int panel); 
static SElExceptionPtr CreateSearchRegion( int panel);

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "InitializePatternDialog"
SElExceptionPtr InitializePatternDialog(void)
{
	SElExceptionPtr		pexception = NULL;      
	int32_t				error = 0;
	HRESULT         	hr = 0;   
	VBOOL         		is_xml_success = 1;
	Image*				pimage = NULL;
	ToolWindowOptions	myToolOptions;	
	SGlobalPtr			pglobal = GetGlobal(NULL);   
	ImageType			img_type;
	char				full_path[MAX_PATHNAME_LEN+1] = ""; 
	char				file_name[1024] = "";  
	char				message[512];
	char*				pmessage = message;
	int32_t				i32;  
	short				AXvalue; 
	
	EXCCHECK( CreateSearchRegion(gs_panel[PANPATT]));
	
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_SREGION, 0));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_NUM_MATCHES, 1));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_MIN_SCORE, 800));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_MATCH_MODE, 1));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_PREVIEW, 0));
	
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PANPATT], PANPATT_RLERN, ATTR_DIMMED, 1));

	EXCCHECKCVI( DeleteImage (gs_panel[PANPATT], PANPATT_PICTURE));
	
	EXCCHECKCOM( BKLib2__DBKTreeFromString( pglobal->bktreePM, 
											NULL, 
											BKTREE_RESULT_HEADER
											"|root|Step^i0\n", 
											"$", "\n",
											&AXvalue));
	
	if ( pglobal->step!=0) /* edit step */  
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step, NULL, VFALSE, &pglobal->step_clone)); 
		
		XML_NODE_GET_TEXT(pglobal->step, "@name",
		{
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_SNAME, ptext));
		});
	
		XML_NODE_GET_TEXT(pglobal->step, "score",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_MIN_SCORE, i32));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "contrast",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_MIN_CONTRAST, i32));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "mode",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_MATCH_MODE, i32));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "matches",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_NUM_MATCHES, i32));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "region",
		{
			EXCCHECK( CreateRegion( IMAQ_WINDOW_NUMBER, ptext));
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANPATT], PANPATT_REGION_STR, ptext));
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_SREGION, 1));
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[PANPATT], PANPATT_RLERN, ATTR_DIMMED, 0));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "template",
		{
			strcat(full_path, pglobal->PatternPath);
			strcat(full_path, "\\");
			strcat(full_path, ptext);
			if( 0 != imaqGetFileInfo(full_path, NULL, NULL, NULL, NULL, NULL, &img_type))
			{
				gs_TemplateImage = imaqCreateImage( img_type, 0);
				if( gs_TemplateImage==NULL)
				{
					EXCCHECKIMAQ( 0);
				}
				
				EXCCHECKIMAQ( imaqReadVisionFile(gs_TemplateImage, full_path, NULL, NULL));
				EXCCHECKIMAQ( imaqLearnPattern2 (gs_TemplateImage, IMAQ_LEARN_ALL, NULL) );  
			}	
			else
			{
				pmessage += sprintf(pmessage, _("No valid template path defined"));
				pmessage += sprintf(pmessage, "\n%s", full_path);
				MessagePopup ("Error", message);
				goto Error;
			}
		});
		
		EXCCHECK( DisplayTemplateImage( gs_panel[PANPATT], PANPATT_PICTURE, gs_TemplateImage)); 
	} 
	else /* create new step */
	{
		VARIANT	type = CA_VariantEmpty ();
		
		EXCCHECKCVI( SetCtrlVal (gs_panel[PANPATT], PANPATT_SNAME, "")); 
		
		CA_VariantSetCString (&type, "Element");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateNode (pglobal->pxml, NULL, type, "step", "", &pglobal->step_clone));
		EXCCHECKMSXML( MSXML_IXMLDOMElementsetAttribute (pglobal->step_clone, NULL, "name", CA_VariantEmpty ()));
	}
	
	pimage = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME); 	
	EXCCHECKIMAQ( imaqClearOverlay(pimage, NULL) );	
	EXCCHECKIMAQ( imaqDisplayImage (pimage, IMAQ_WINDOW_NUMBER, FALSE));  
	
	/* Setup tools for window interaction */
	myToolOptions.showSelectionTool = TRUE;     
	myToolOptions.showZoomTool = TRUE;          
	myToolOptions.showPointTool = FALSE;         
	myToolOptions.showLineTool = FALSE;           
	myToolOptions.showRectangleTool = TRUE;     
	myToolOptions.showOvalTool = FALSE;          
	myToolOptions.showPolygonTool = FALSE;       
	myToolOptions.showClosedFreehandTool = FALSE;
	myToolOptions.showPolyLineTool = FALSE;      
	myToolOptions.showFreehandTool = FALSE;     
	myToolOptions.showAnnulusTool = FALSE;          
   	myToolOptions.showRotatedRectangleTool = FALSE;	
	
	EXCCHECKIMAQ( imaqSetupToolWindow (FALSE, 20, &myToolOptions));
	EXCCHECKIMAQ( imaqSetCurrentTool (IMAQ_RECTANGLE_TOOL) ); 
	EXCCHECKIMAQ( imaqShowToolWindow (TRUE));

	EXCCHECKCVI( SetPanelPos (gs_panel[PANPATT], 15, 0)); 
	EXCCHECKCVI( DisplayPanel (gs_panel[PANPATT]));
	EXCCHECKCVI( SetActivePanel (gs_panel[PANPATT]));      

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelPatternLern"
int CVICALLBACK OnPanelPatternLern(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	ROI*  	  		proi = NULL;
	Image*			pimage = NULL;
	
	switch (event)
	{
		case EVENT_COMMIT:
		
			proi = imaqGetWindowROI(IMAQ_WINDOW_NUMBER) ;
			if ( proi )
			{
				Rect		rect;
				ImageType 	img_type;
				
				EXCCHECKIMAQ( imaqGetROIBoundingBox( proi, &rect) );
				
				if ( (rect.width == 0) || (rect.height == 0))
				{
					MessagePopup( "Error", _("Region is not defined"));
					return 0;					
				}
				
				pimage = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME);
				if ( pimage )
				{
					EXCCHECKIMAQ( imaqGetImageType(pimage, &img_type) );
					
					gs_TemplateImage = imaqCreateImage( img_type, 0);
					if( gs_TemplateImage==NULL)
					{
						EXCCHECKIMAQ( 0);
					}
					
					EXCCHECKIMAQ( imaqSetImageSize(gs_TemplateImage, rect.width, rect.height) );
					EXCCHECKIMAQ( imaqCopyRect (gs_TemplateImage, pimage, rect, imaqMakePoint(0,0)) );

					EXCCHECK( DisplayTemplateImage( panel, control, gs_TemplateImage)); 
					
					EXCCHECKIMAQ( imaqLearnPattern2 (gs_TemplateImage, IMAQ_LEARN_ALL, NULL) );
					EXCCHECK( DoPreview(panel));
				}
			}
			break;
	}
	
Error:
	if(proi) imaqDispose(proi);
	EXCDISPLAY( pexception);    
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelPatternSearchRegion"
int CVICALLBACK OnPanelPatternSearchRegion(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int				value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal (panel, PANPATT_SREGION, &value));
			EXCCHECKCVI( SetCtrlAttribute (panel, PANPATT_RLERN, ATTR_DIMMED, !value));
			
			if(!value)
			{
				EXCCHECK( CreateSearchRegion(panel));
			}
			
			EXCCHECK( DoPreview(panel));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);   
	return 0;
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelPatternROI"
int CVICALLBACK OnPanelPatternROI(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	ROI*  	  		proi = NULL;
	char			message[1024] = "";

	switch (event)
	{
		case EVENT_COMMIT:
			
			proi = imaqGetWindowROI(IMAQ_WINDOW_NUMBER) ;
			if ( proi )
			{
				Rect	rect;
				
				EXCCHECKIMAQ( imaqGetROIBoundingBox( proi, &rect) );
				
				if ( (rect.width == 0) || (rect.height == 0))
				{
					MessagePopup( "Error", _("Region is not defined"));
					return 0;					
				}		
				
				EXCCHECK( FormatRegion( 1, 0, message, sizeof(message)));
				EXCCHECKCVI( SetCtrlVal( gs_panel[PANPATT], PANPATT_REGION_STR, message));
				EXCCHECK( DoPreview(panel));
			}
			break;
	}
	
Error:
	if( proi) imaqDispose(proi);
	EXCDISPLAY( pexception);  
	return 0;
}

/*----------------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelPatternPreview"
int CVICALLBACK OnPanelPatternPreview (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch ( control)
			{
				case PANPATT_BPREVIEW:
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

/*----------------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelPatternExit"
int CVICALLBACK OnPanelPatternExit(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;
	Image*			pimage = NULL;
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	
	switch (event)
	{
		case EVENT_COMMIT:
			if ( control == PANPATT_OK )
			{
				EXCCHECK( CheckStepName( panel, PANPATT_SNAME));
				EXCCHECK( CheckStepTemplate( panel, gs_TemplateImage ));
				EXCCHECK( XmlSaveChanges( panel, PANPATT_SNAME));
				EXCCHECK( InitializeTaskDialog());
			}
			else if( control==PANPATT_CANCEL)
			{
				/* clean clone */
				EXCCHECKCVI( CA_DiscardObjHandle(pglobal->step_clone));
				pglobal->step_clone = 0;
			}
			
			EXCCHECKIMAQ( imaqShowToolWindow (FALSE)); 
			EXCCHECKIMAQ( imaqSetCurrentTool (IMAQ_NO_TOOL));
			EXCCHECKIMAQ( imaqSetWindowROI (IMAQ_WINDOW_NUMBER, NULL));
			
			pimage = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME);
			EXCCHECKIMAQ( imaqClearOverlay(pimage, NULL));
			EXCCHECKIMAQ( imaqDisplayImage (pimage, IMAQ_WINDOW_NUMBER, FALSE));             
			
			if ( gs_TemplateImage )
			{
				imaqDispose( gs_TemplateImage);
				gs_TemplateImage = NULL;  
			}
				
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
	char			name[MAX_STEP_NAME];
	char			full_path[MAX_PATHNAME_LEN]="";
	char			file_name[MAX_STEP_NAME]="";
	char			buffer[256] = "";
	SGlobalPtr		pglobal = GetGlobal(NULL);
	int				v1;

	/* step name */
	EXCCHECKCVI( GetCtrlVal( panel, control, name));
	RemoveSurroundingWhiteSpace(name);

	XML_NODE_SET_TEXT(pstep, "@name", name);   
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "type", "pattern");

	EXCCHECKCVI( GetCtrlVal( panel, PANPATT_REGION_STR, buffer));
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "region", buffer); 

	if(gs_TemplateImage)
	{
		if( strlen(name)>0)
		{
			strcat(file_name, name);
			strcat(file_name, ".png");
		}
		else
		{
			sprintf(file_name, "template.png");			
		}		
	
		strcat(full_path, pglobal->PatternPath);
		strcat(full_path, "\\");
		strcat(full_path, file_name);
	
		EXCCHECKIMAQ( imaqWriteVisionFile( gs_TemplateImage, full_path, NULL));
		XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "template", file_name);  
		EXCCHECK( gs_pIMAQ->LoadPicture2(gs_pIMAQ, file_name, full_path));
	}

	EXCCHECKCVI( GetCtrlVal( panel, PANPATT_NUM_MATCHES, &v1));
	sprintf( buffer, "%d", v1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "matches", buffer); 

	EXCCHECKCVI( GetCtrlVal( panel, PANPATT_MIN_SCORE, &v1));
	sprintf( buffer, "%d", v1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "score", buffer); 

	EXCCHECKCVI( GetCtrlVal( panel, PANPATT_MIN_CONTRAST, &v1));
	sprintf( buffer, "%d", v1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "contrast", buffer);   

	EXCCHECKCVI( GetCtrlVal( panel, PANPATT_MATCH_MODE, &v1));
	sprintf( buffer, "%d", v1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "mode", buffer);  

Error:	
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "XmlSaveChanges"
static SElExceptionPtr XmlSaveChanges(int panel, int control)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;

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

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "DisplayTemplateImage"
static SElExceptionPtr DisplayTemplateImage(int panel, int control, const Image* pImage)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	char	  		tmp_file[L_tmpnam] = "";

	tmpnam (tmp_file);
	strcat(tmp_file, ".bmp");
	
	EXCCHECKIMAQ( imaqWriteFile( gs_TemplateImage, tmp_file, NULL) );
	
	EXCCHECKCVI( DisplayImageFile (panel, PANPATT_PICTURE, tmp_file));
	remove( tmp_file);

Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CreateSearchRegion"
static SElExceptionPtr CreateSearchRegion( int panel)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	Image*			pimage = NULL;
	int				left, width, top, height;
	char			pregionnew[50];  
	
	pimage = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME);
	top = 0;
	left = 0;
	EXCCHECKIMAQ( imaqGetImageSize( pimage, &width, &height) );
	
	sprintf(pregionnew, "rect(%d,%d,%d,%d)", left, top, width, height);
	
	EXCCHECKCVI( SetCtrlVal( panel, PANPATT_REGION_STR, pregionnew)); 

Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DoPreview"
static SElExceptionPtr DoPreview( int panel)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	int				is_preview = 0;
	
	EXCCHECKCVI( GetCtrlVal (panel, PANPATT_PREVIEW, &is_preview));			
	
	if(is_preview)
	{
		EXCCHECK( DoStep(panel) );
	}

Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "DoStep"
static SElExceptionPtr DoStep( int panel)
{
	SElExceptionPtr	pexception = NULL;          
	int32_t			error = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
	SELStringPtr	result = NULL;
	Image*			pimage = NULL;
	SGlobalPtr		pglobal = GetGlobal(NULL); 
	CAObjHandle 	pstep = 0;

	/* Check template image */
	if( gs_TemplateImage==NULL)
	{
		EXCTHROW(-1, "Template image not defined!");
	}

	/* Set data for the step */
	if(pglobal->step)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step, NULL, VFALSE, &pstep));
	}
	else if(pglobal->step_clone)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step_clone, NULL, VFALSE, &pstep));
	}
	EXCCHECK( StepData(panel, PANGPATT_SNAME, pglobal->pxml, pstep));

	/* Do Step */
	EXCCHECK( elstring_new("<step>\n", &result));
	EXCCHECK( gs_pIMAQ->DoPatternStep(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, pstep, result));
	EXCCHECK( result->Append(result, "</step>\n"));

	EXCCHECK( ResultToBktree( pglobal->bktreePM, result));
	EXCCHECK( ResultToView( result));

Error:	
	EXCRETHROW( pexception);    
}
