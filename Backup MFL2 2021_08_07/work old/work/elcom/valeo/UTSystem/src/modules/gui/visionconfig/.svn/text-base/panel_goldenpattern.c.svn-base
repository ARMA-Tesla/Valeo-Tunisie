#include "global.h"
#include "toolbox.h"
#include "NIMachineVision.h"
#include <nivision.h>
#include <userint.h>
#include <bklib/bklib2.h>
#include "visionconfig.h" 
#include "visionconfig_fnc.h" 
#include "panel_task.h" 
#include "panel_pattern.h"

extern int 			gs_panel[];
extern SIMAQPtr		gs_pIMAQ;

static Image* 		gs_TemplateImage;
static Image*  		gs_defectImage;
static Image*  		gs_defectDisplayImage;

static SElExceptionPtr DoStep( int panel);
static SElExceptionPtr XmlSaveChanges(int panel,int	control);
static SElExceptionPtr DisplayTemplateImage(int panel,int control,const Image*	pImage );
static SElExceptionPtr DoPreview( int panel); 
static SElExceptionPtr CreateSearchRegion( int panel);

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "InitializeGoldenPatternDialog"
SElExceptionPtr InitializeGoldenPatternDialog(void)
{
	SElExceptionPtr 	pexception = NULL;   
	int32_t				error = 0;
	SGlobalPtr			pglobal = GetGlobal(NULL);   
	HRESULT         	hr = 0;   
	VBOOL         		is_xml_success = 1;
	Image*				pimage = NULL;
	ToolWindowOptions	myToolOptions;	
	ImageType			img_type;
	char				full_path[MAX_PATHNAME_LEN+1] = ""; 
	char				file_name[1024] = ""; 
	char				message[1024] = "";
	char				*pmessage = message;
	int32_t				i32;
	double				fvalue;
	short				AXvalue; 
	
	EXCCHECK( CreateSearchRegion(gs_panel[PANGPATT]));	
	
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_SREGION, 0));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_PREVIEW, 0));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_MIN_CONTRAST, 10));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_MIN_SCORE, 50));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_MATCH_MODE, 1));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_BRIGHT_LEVEL, 30.0));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_DARK_LEVEL, 30.0));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_MIN, 30));
	EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_NORMALIZE, 0));
	
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PANGPATT], PANGPATT_RLERN, ATTR_DIMMED, 1));
	
	EXCCHECKCVI( DeleteImage (gs_panel[PANGPATT], PANGPATT_PICTURE));
	
	EXCCHECKCOM( BKLib2__DBKTreeFromString( pglobal->bktreeGT, 
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
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_SNAME, ptext));
		});
		
		
		XML_NODE_GET_TEXT(pglobal->step, "Normalize",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_NORMALIZE, i32));   
		});
		XML_NODE_GET_TEXT(pglobal->step, "MinNormalizeLevel",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_MIN, i32));   
		});

		
		XML_NODE_GET_TEXT(pglobal->step, "score",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_MIN_SCORE, i32));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "contrast",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_MIN_CONTRAST, i32));   
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "mode",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_MATCH_MODE, i32));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "BrightThreshold",
		{
			fvalue = strtod(ptext, NULL); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_BRIGHT_LEVEL, (float)fvalue));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "DarkThreshold",
		{
			fvalue = strtod(ptext, NULL); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_DARK_LEVEL, (float)fvalue));   
		});
		XML_NODE_GET_TEXT(pglobal->step, "EdgeThicknessToIgnore",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_EDGE_TICKNESS, i32));   
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "region",
		{
			EXCCHECK( CreateRegion( IMAQ_WINDOW_NUMBER, ptext));
			EXCCHECKCVI( SetCtrlVal( gs_panel[PANGPATT], PANGPATT_REGION_STR, ptext));
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_SREGION, 1));
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[PANGPATT], PANGPATT_RLERN, ATTR_DIMMED, 0));  
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
			}	
			else
			{
				pmessage += sprintf(pmessage, _("No valid template path defined"));
				pmessage += sprintf(pmessage, "\n%s", full_path);
				MessagePopup ("Error", message);
				goto Error;
			}
		});

		EXCCHECK( DisplayTemplateImage( gs_panel[PANGPATT], PANGPATT_PICTURE, gs_TemplateImage)); 
	} 
	else /* create new step */
	{
		VARIANT	type = CA_VariantEmpty ();
		
		EXCCHECKCVI( SetCtrlVal (gs_panel[PANGPATT], PANGPATT_SNAME, "")); 
		
		CA_VariantSetCString (&type, "Element");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateNode (pglobal->pxml, NULL, type, "step", "", &pglobal->step_clone));
		EXCCHECKMSXML( MSXML_IXMLDOMElementsetAttribute (pglobal->step_clone, NULL, "name", CA_VariantEmpty ()));
	}
	
	pimage = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME);   
	EXCCHECKIMAQ( imaqClearOverlay(pimage, NULL));	
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
	
	EXCCHECKCVI( SetPanelPos (gs_panel[PANGPATT], 15, 0)); 
	EXCCHECKCVI( DisplayPanel (gs_panel[PANGPATT]));
	EXCCHECKCVI( SetActivePanel (gs_panel[PANGPATT])); 
	
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelGPatternLern"
int CVICALLBACK OnPanelGPatternLern(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	ROI*  	  		proi = NULL;
	Image*			pimage = NULL;
	Image*			pmaskimage = NULL;
	PixelValue		PixelValueSet;
	
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
					
					gs_TemplateImage = imaqCreateImage(img_type, 3);
					if( gs_TemplateImage==NULL)
					{
						EXCCHECKIMAQ( 0);
					}
					
					pmaskimage = imaqCreateImage(img_type, 3);
					if( pmaskimage==NULL)
					{
						EXCCHECKIMAQ( 0);
					}
					
					EXCCHECKIMAQ( imaqSetImageSize(gs_TemplateImage, rect.width, rect.height) );
					EXCCHECKIMAQ( imaqSetImageSize(pmaskimage, rect.width, rect.height) ); 
					
					PixelValueSet.rgb.R = 0;
					PixelValueSet.rgb.G = 0;
					PixelValueSet.rgb.B = 0;
					PixelValueSet.rgb.alpha = 0; 
					EXCCHECKIMAQ( imaqFillImage (pmaskimage, PixelValueSet, NULL) );
					
					EXCCHECKIMAQ( imaqCopyRect (gs_TemplateImage, pimage, rect, imaqMakePoint(0,0)) );
					
					EXCCHECK( DisplayTemplateImage( panel, control, gs_TemplateImage)); 
					
					EXCCHECKIMAQ( imaqLearnPattern2 (gs_TemplateImage, IMAQ_LEARN_ALL, NULL) );
					EXCCHECKIMAQ( imaqLearnGoldenTemplate(gs_TemplateImage, IMAQ_NO_OFFSET, pmaskimage) );

					EXCCHECK( DoPreview(panel));
				}
			}
			break;
		}

Error:
	if( proi) imaqDispose(proi);
	if( pmaskimage) imaqDispose(pmaskimage); 
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelGPatternSearchRegion"
int CVICALLBACK OnPanelGPatternSearchRegion(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	int				value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_SREGION, &value));
			EXCCHECKCVI( SetCtrlAttribute (panel, PANGPATT_RLERN, ATTR_DIMMED, !value));
					
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
#define __FUNC__ "OnPanelGPatternROI"
int CVICALLBACK OnPanelGPatternROI(int panel, int control, int event,
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
				
				EXCCHECKIMAQ( imaqGetROIBoundingBox (proi, &rect) );
				
				if ( (rect.width == 0) || (rect.height == 0))
				{
					MessagePopup( "Error", _("Region is not defined"));
					return 0;					
				}		
				
				EXCCHECK( FormatRegion( 1, 0, message, sizeof(message)));
				EXCCHECKCVI( SetCtrlVal( panel, PANGPATT_REGION_STR, message));
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
#define __FUNC__ "OnPanelGPatternPreview"
int CVICALLBACK OnPanelGPatternPreview (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch ( control)
			{
				case PANGPATT_BPREVIEW:
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
#define __FUNC__ "OnPanelGPatternExit"
int CVICALLBACK OnPanelGPatternExit(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	Image*			pImage = NULL;
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	
	switch (event)
	{
		case EVENT_COMMIT:
			if ( control == PANGPATT_OK )
			{
				EXCCHECK( CheckStepName( panel, PANGPATT_SNAME));
				EXCCHECK( XmlSaveChanges( panel, PANGPATT_SNAME));
				EXCCHECK( CheckStepTemplate( panel, gs_TemplateImage )); 
				EXCCHECK( InitializeTaskDialog());
			}
			else if( control==PANGPATT_CANCEL)
			{
				/* clean clone */
				EXCCHECKCVI( CA_DiscardObjHandle(pglobal->step_clone));
				pglobal->step_clone = 0;
			}
			
			EXCCHECKIMAQ( imaqShowToolWindow (FALSE)); 
			EXCCHECKIMAQ( imaqSetCurrentTool (IMAQ_NO_TOOL));
			EXCCHECKIMAQ( imaqSetWindowROI (IMAQ_WINDOW_NUMBER, NULL));
			EXCCHECKIMAQ( imaqSetWindowPalette (IMAQ_WINDOW_NUMBER, IMAQ_PALETTE_GRAY, NULL, 0));
			
			pImage = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME);
			EXCCHECKIMAQ( imaqClearOverlay(pImage, NULL));
			EXCCHECKIMAQ( imaqDisplayImage (pImage, IMAQ_WINDOW_NUMBER, FALSE));
			
			if ( gs_TemplateImage )
			{
				imaqDispose( gs_TemplateImage);
				gs_TemplateImage = NULL;
			}
			
			if ( gs_defectImage )
			{
				imaqDispose( gs_defectImage);
				gs_defectImage = NULL;   
			}
			
			if ( gs_defectDisplayImage )
			{
				imaqDispose( gs_defectDisplayImage);
				gs_defectDisplayImage = NULL;  
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
	float			f1;

	EXCCHECKCVI( GetCtrlVal( panel, control, name));
	RemoveSurroundingWhiteSpace(name);

	XML_NODE_SET_TEXT(pstep, "@name", name);   
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "type", "golden template");

	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_REGION_STR, buffer));
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "region", buffer);  

	if( gs_TemplateImage)
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
	
		EXCCHECKIMAQ(  imaqWriteVisionFile( gs_TemplateImage, full_path, NULL));
		XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "template", file_name);  
		EXCCHECK( gs_pIMAQ->LoadPicture2(gs_pIMAQ, file_name, full_path));
	}

	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_MIN_SCORE, &v1));
	sprintf( buffer, "%d", v1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "score", buffer);    

	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_MIN_CONTRAST, &v1));
	sprintf( buffer, "%d", v1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "contrast", buffer); 

	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_MATCH_MODE, &v1));
	sprintf( buffer, "%d", v1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "mode", buffer);       

	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_BRIGHT_LEVEL, &f1));
	sprintf( buffer, "%Lf", (double)f1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "BrightThreshold", buffer);   

	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_DARK_LEVEL, &f1));
	sprintf( buffer, "%Lf", (double)f1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "DarkThreshold", buffer);   

	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_EDGE_TICKNESS, &v1));
	sprintf( buffer, "%d", v1);	
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "EdgeThicknessToIgnore", buffer);
	
	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_NORMALIZE, &v1));
	sprintf( buffer, "%d", v1);	
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "Normalize", buffer);
	
	EXCCHECKCVI( GetCtrlVal( panel, PANGPATT_MIN, &v1));
	sprintf( buffer, "%d", v1);	
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "MinNormalizeLevel", buffer);
	
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "NormalizationMethod", "0");     
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "RegistrationMethod", "0");     

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "XmlSaveChanges"
static SElExceptionPtr XmlSaveChanges(
	int panel,
	int	control
)
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
static SElExceptionPtr DisplayTemplateImage(
	int panel,
	int control,
	const Image*	pImage
)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	char	  		tmp_file[L_tmpnam] = "";

	tmpnam (tmp_file);
	strcat(tmp_file, ".bmp");
	
	EXCCHECKIMAQ( imaqWriteFile(gs_TemplateImage, tmp_file, NULL) );
	
	EXCCHECKCVI( DisplayImageFile (panel, PANGPATT_PICTURE, tmp_file));
	
	remove(tmp_file);

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
	int				left = 0, width, top = 0, height;
	char			pregionnew[50];  
	
	pimage = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME);
	
	EXCCHECKIMAQ( imaqGetImageSize( pimage, &width, &height) );	
	sprintf(pregionnew, "rect(%d,%d,%d,%d)", left, top, width, height);
	
	EXCCHECKCVI( SetCtrlVal( panel, PANGPATT_REGION_STR, pregionnew)); 

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
	
	EXCCHECKCVI( GetCtrlVal (panel, PANGPATT_PREVIEW, &is_preview));				
	if ( is_preview )
		EXCCHECK( DoStep( panel));

Error:
	EXCRETHROW( pexception);   
}

/*----------------------------------------------------------------------------------*/   
#undef __FUNC__
#define __FUNC__ "OnThresholdEnabled"
int CVICALLBACK OnThresholdEnabled (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;    
	int32_t			error = 0;
	int				state = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, control, &state));
			EXCCHECKCVI( SetCtrlAttribute( panel, PANGPATT_BRIGHT_LEVEL, ATTR_DIMMED, !state));
			EXCCHECKCVI( SetCtrlAttribute( panel, PANGPATT_DARK_LEVEL, ATTR_DIMMED,  !state));
			EXCCHECK( DoPreview(panel));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
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
	EXCCHECK( gs_pIMAQ->DoGoldenTemplateStep(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, pstep, result));
	EXCCHECK( result->Append(result, "</step>\n"));

	EXCCHECK( ResultToBktree( pglobal->bktreeGT, result));
	EXCCHECK( ResultToView( result));

Error:	
	CA_DiscardObjHandle(pstep);
	elstring_delete( &result);
	EXCRETHROW( pexception);    
}
