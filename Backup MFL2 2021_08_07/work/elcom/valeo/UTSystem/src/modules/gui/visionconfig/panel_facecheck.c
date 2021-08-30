#include "global.h"
#include "toolbox.h"
#include "NIMachineVision.h"
#include <nivision.h>
#include <userint.h>
#include <bklib/bklib2.h>
#include "visionconfig.h" 
#include "visionconfig_fnc.h" 
#include "panel_task.h" 
#include "panel_facecheck.h"

extern int 			gs_panel[];
extern SIMAQPtr		gs_pIMAQ;

typedef struct _SFaceCtrl
{
	int32_t point;
	int32_t point_description;
	int32_t point_target;
	int32_t point_preview;
	int32_t point_delete;
}SFaceCtrl;

#define NUMBER_OF_POINT		5
static SFaceCtrl gs_FaceCtrl[NUMBER_OF_POINT] =
{
	{
		PANFACE_POINT_DEFINITION_1,
		PANFACE_SELECTED_POINT_TXT_1,
		PANFACE_PLERN_1,
		PANFACE_POINT_PREVIEW_1,
		PANFACE_POINT_DELETE_1
	},
	{
		PANFACE_POINT_DEFINITION_2,
		PANFACE_SELECTED_POINT_TXT_2,
		PANFACE_PLERN_2,
		PANFACE_POINT_PREVIEW_2,
		PANFACE_POINT_DELETE_2
	},
	{
		PANFACE_POINT_DEFINITION_3,
		PANFACE_SELECTED_POINT_TXT_3,
		PANFACE_PLERN_3,
		PANFACE_POINT_PREVIEW_3,
		PANFACE_POINT_DELETE_3
	},
	{
		PANFACE_POINT_DEFINITION_4,
		PANFACE_SELECTED_POINT_TXT_4,
		PANFACE_PLERN_4,
		PANFACE_POINT_PREVIEW_4,
		PANFACE_POINT_DELETE_4
	},
	{
		PANFACE_POINT_DEFINITION_5,
		PANFACE_SELECTED_POINT_TXT_5,
		PANFACE_PLERN_5,
		PANFACE_POINT_PREVIEW_5,
		PANFACE_POINT_DELETE_5
	}
};

static SElExceptionPtr DoStep(int panel);
static SElExceptionPtr XmlSaveChanges(int panel,int	control);
static SElExceptionPtr DoPreview(int panel); 

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "InitializeFaceCheckDialog"
SElExceptionPtr InitializeFaceCheckDialog(void)
{
	SElExceptionPtr 	pexception = NULL;   
	int32_t				error = 0;
	SGlobalPtr			pglobal = GetGlobal(NULL);   
	HRESULT         	hr = 0;   
	VBOOL         		is_xml_success = 1;
	Image*				pimage = NULL;
	ToolWindowOptions	myToolOptions;	
	char				full_path[MAX_PATHNAME_LEN+1] = ""; 
	char				file_name[1024] = ""; 
	char				spoint[512];
	int32_t				i32;
	short				AXvalue;
	int32_t				i;
	
	for(i=0; i<NUMBER_OF_POINT; i++)
	{
		EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], gs_FaceCtrl[i].point, "0;0"));  
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PANFACE], gs_FaceCtrl[i].point, ATTR_DIMMED, TRUE));
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PANFACE], gs_FaceCtrl[i].point_description, ATTR_DIMMED, TRUE));
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PANFACE], gs_FaceCtrl[i].point_preview, ATTR_DIMMED, TRUE));
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PANFACE], gs_FaceCtrl[i].point_delete, ATTR_DIMMED, TRUE));
	}
	
	EXCCHECKCVI( DeleteImage (gs_panel[PANFACE], PANFACE_PICTURE));
	
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
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], PANFACE_SNAME, ptext));
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "point1",
		{
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], PANFACE_POINT_DEFINITION_1, ptext));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "point2",
		{
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], PANFACE_POINT_DEFINITION_2, ptext));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "point3",
		{
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], PANFACE_POINT_DEFINITION_3, ptext));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "point4",
		{
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], PANFACE_POINT_DEFINITION_4, ptext));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "point5",
		{
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], PANFACE_POINT_DEFINITION_5, ptext));  
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "area_size",
		{
			i32 = strtol(ptext, NULL, 10); 
			EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], PANFACE_AREA_SIZE, i32));   
		});

		for(i=0; i<NUMBER_OF_POINT; i++)
		{
			EXCCHECKCVI( GetCtrlVal (gs_panel[PANFACE], gs_FaceCtrl[i].point, spoint));
			if(strcmp(spoint, "0;0")!=0)
			{
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[PANFACE], gs_FaceCtrl[i].point, ATTR_DIMMED, FALSE));
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[PANFACE], gs_FaceCtrl[i].point_description, ATTR_DIMMED, FALSE));
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[PANFACE], gs_FaceCtrl[i].point_preview, ATTR_DIMMED, FALSE));
				EXCCHECKCVI( SetCtrlAttribute(gs_panel[PANFACE], gs_FaceCtrl[i].point_delete, ATTR_DIMMED, FALSE));
			}
		}
	} 
	else /* create new step */
	{
		VARIANT	type = CA_VariantEmpty ();
		
		EXCCHECKCVI( SetCtrlVal (gs_panel[PANFACE], PANFACE_SNAME, "")); 
		
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
	myToolOptions.showPointTool = TRUE;         
	myToolOptions.showLineTool = FALSE;           
	myToolOptions.showRectangleTool = FALSE;     
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
	
	EXCCHECKCVI( SetPanelPos (gs_panel[PANFACE], 15, 0)); 
	EXCCHECKCVI( DisplayPanel (gs_panel[PANFACE]));
	EXCCHECKCVI( SetActivePanel (gs_panel[PANFACE])); 
	
Error:
	EXCRETHROW( pexception);  
}

/*----------------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelFacePreview"
int CVICALLBACK OnPanelFacePreview (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch ( control)
			{
				case PANFACE_BPREVIEW:
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
#define __FUNC__ "OnPanelFaceExit"
int CVICALLBACK OnPanelFaceExit(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	Image*			pImage = NULL;
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	
	switch (event)
	{
		case EVENT_COMMIT:
			if ( control == PANFACE_OK )
			{
				EXCCHECK( CheckStepName( panel, PANFACE_SNAME));
				EXCCHECK( XmlSaveChanges( panel, PANFACE_SNAME));
				EXCCHECK( InitializeTaskDialog());
			}
			else if( control==PANFACE_CANCEL)
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

	EXCCHECKCVI( GetCtrlVal( panel, control, name));
	RemoveSurroundingWhiteSpace(name);

	XML_NODE_SET_TEXT(pstep, "@name", name);   
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "type", "face check");

	EXCCHECKCVI( GetCtrlVal( panel, PANFACE_POINT_DEFINITION_1, buffer));
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "point1", buffer);  
	
	EXCCHECKCVI( GetCtrlVal( panel, PANFACE_POINT_DEFINITION_2, buffer));
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "point2", buffer);  
	
	EXCCHECKCVI( GetCtrlVal( panel, PANFACE_POINT_DEFINITION_3, buffer));
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "point3", buffer);  
	
	EXCCHECKCVI( GetCtrlVal( panel, PANFACE_POINT_DEFINITION_4, buffer));
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "point4", buffer);  
	
	EXCCHECKCVI( GetCtrlVal( panel, PANFACE_POINT_DEFINITION_5, buffer));
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "point5", buffer);  

	EXCCHECKCVI( GetCtrlVal( panel, PANFACE_AREA_SIZE, &v1));
	sprintf( buffer, "%d", v1);
	XML_NODE_ADD_NEW_ELEMENT(pxml, pstep, "area_size", buffer);    

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

/*----------------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "DoPreview"
static SElExceptionPtr DoPreview( int panel)
{
	SElExceptionPtr	pexception = NULL;    
	int32_t			error = 0;
	int				is_preview = 0;
	
	EXCCHECKCVI( GetCtrlVal (panel, PANFACE_PREVIEW, &is_preview));				
	if ( is_preview )
		EXCCHECK( DoStep( panel));

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
	EXCCHECK( StepData(panel, PANFACE_SNAME, pglobal->pxml, pstep));

	/* Do Step */
	EXCCHECK( elstring_new("<step>\n", &result));
	EXCCHECK( gs_pIMAQ->DoFaceCheckStep(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, pstep, result));
	EXCCHECK( result->Append(result, "</step>\n"));

	EXCCHECK( ResultToBktree( pglobal->bktreeFC, result));

Error:	
	CA_DiscardObjHandle(pstep);
	elstring_delete( &result);
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelFacePointDefine"
int CVICALLBACK OnPanelFacePointDefine (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	ROI*  	  		proi = NULL;
	char			spoint[1024] = "";
	int				i;

	switch (event)
	{
		case EVENT_COMMIT:
			proi = imaqGetWindowROI(IMAQ_WINDOW_NUMBER) ;
			if ( proi )
			{
				Rect	rect;
				
				EXCCHECKIMAQ( imaqGetROIBoundingBox (proi, &rect) );
				
				if ( (rect.top == 0) || (rect.left == 0))
				{
					MessagePopup( "Error", _("Point is not defined"));
					return 0;					
				}		
				
				sprintf(spoint, "%d;%d", rect.left, rect.top);

				for(i=0; i<NUMBER_OF_POINT; i++)
				{
					if(control==gs_FaceCtrl[i].point_target)
					{
						EXCCHECKCVI( SetCtrlVal( panel, gs_FaceCtrl[i].point, spoint));
						EXCCHECKCVI( SetCtrlAttribute(panel, gs_FaceCtrl[i].point, ATTR_DIMMED, FALSE));
						EXCCHECKCVI( SetCtrlAttribute(panel, gs_FaceCtrl[i].point_description, ATTR_DIMMED, FALSE));
						EXCCHECKCVI( SetCtrlAttribute(panel, gs_FaceCtrl[i].point_preview, ATTR_DIMMED, FALSE));
						EXCCHECKCVI( SetCtrlAttribute(panel, gs_FaceCtrl[i].point_delete, ATTR_DIMMED, FALSE));
						break;
					}
				}

				EXCCHECK( DoPreview(panel));
			}
			break;
	}
	
Error:
	EXCDISPLAY(pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelFacePointPreview"
int CVICALLBACK OnPanelFacePointPreview (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;    
	int32_t			error = 0;
	int				i;
	char			spoint[256];
	Image*			pimage = NULL;
	Image*			pimage_src = NULL;

	switch (event)
	{
		case EVENT_COMMIT:
			for(i=0; i<NUMBER_OF_POINT; i++)
			{
				if(control==gs_FaceCtrl[i].point_preview)
				{
					EXCCHECKCVI( GetCtrlVal(panel, gs_FaceCtrl[i].point, spoint));

					if(strcmp(spoint, "0;0")!=0)
					{
						Rect		rect;
						int			area;
						ImageType	img_type;

						sscanf(spoint, "%d;%d", &rect.left, &rect.top);

						EXCCHECKCVI( GetCtrlVal(panel, PANFACE_AREA_SIZE, &area));
						rect.height = area;
						rect.width = area;

						pimage_src = gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME);  

						EXCCHECKIMAQ( imaqGetImageType(pimage_src, &img_type) );
						pimage = imaqCreateImage( img_type, 0);
						if( pimage==NULL)
						{
							EXCCHECKIMAQ( 0);
						}
						EXCCHECKIMAQ( imaqSetImageSize(pimage, rect.width, rect.height) );
						EXCCHECKIMAQ( imaqCopyRect (pimage, pimage_src, rect, imaqMakePoint(0,0)));

						if(pimage)
						{
							char	tmp_file[L_tmpnam] = "";

							tmpnam (tmp_file);
							strcat(tmp_file, ".bmp");
							
							EXCCHECKIMAQ( imaqWriteFile(pimage, tmp_file, NULL) );
							EXCCHECKCVI( DisplayImageFile (panel, PANFACE_PICTURE, tmp_file));
							
							remove(tmp_file);
						}
					}
				}
			}
			break;
	}
	
Error:
	if(pimage) imaqDispose(pimage);
	EXCDISPLAY(pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelFacePointDelete"
int CVICALLBACK OnPanelFacePointDelete (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;    
	int32_t			error = 0;
	int32_t			i;

	switch (event)
	{
		case EVENT_COMMIT:
			for(i=0; i<NUMBER_OF_POINT; i++)
			{
				if(control==gs_FaceCtrl[i].point_delete)
				{
					EXCCHECKCVI( SetCtrlVal(panel, gs_FaceCtrl[i].point, "0;0"));
					EXCCHECKCVI( SetCtrlAttribute(panel, gs_FaceCtrl[i].point, ATTR_DIMMED, TRUE));
					EXCCHECKCVI( SetCtrlAttribute(panel, gs_FaceCtrl[i].point_description, ATTR_DIMMED, TRUE));
					EXCCHECKCVI( SetCtrlAttribute(panel, gs_FaceCtrl[i].point_preview, ATTR_DIMMED, TRUE));
					EXCCHECKCVI( SetCtrlAttribute(panel, gs_FaceCtrl[i].point_delete, ATTR_DIMMED, TRUE));
					break;
				}
			}
			break;
	}
	
Error:
	EXCDISPLAY(pexception);
	return 0;
}
