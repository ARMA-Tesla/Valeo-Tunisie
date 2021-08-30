#include <bklib/bklib2.h>
#include <camera/camera.h>
#include "global.h"  
#include "visionconfig.h"  
#include "visionconfig_fnc.h"  
#include "hwndutil.h" 
#include "toolbox.h"

extern int		gs_panel[];  
extern SIMAQPtr gs_pIMAQ;

static HWND		gs_HWNDimaq = NULL; 
static HWND		gs_HWNDtool = NULL; 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "XmlOpenDocument"
SElExceptionPtr XmlOpenDocument(bool_t new, const char* fileName)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pparameter  = 0; 
	const char*     pfile_name = (fileName)? fileName : "visionconfig.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	SGlobalPtr		pglobal = GetGlobal(NULL);    

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	
	if(new)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (
				pxml, 
				NULL, 
				"<?xml version=\"1.0\" encoding=\"windows-1250\"?>"
				"<visionconfig>\n"
				"<global>\n"
				"<TemplateDir></TemplateDir>\n"
				"</global>\n"
				"</visionconfig>", 
				&is_xml_success));
	}
	else
	{
		EXCCHECKCOM( CA_VariantSetCString(  &pfile_name_var, pfile_name));
		EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	}
		
	if(is_xml_success)
	{
		if(pglobal->pxml)
		{
			CA_DiscardObjHandle(pglobal->pxml);
			pglobal->pxml = 0;
		}
		
		pglobal->pxml = pxml;
		
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentSetpreserveWhiteSpace (pglobal->pxml, NULL, VTRUE));
		
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pglobal->pxml, NULL, "//visionconfig", &pglobal->root));
		
		if(new && strlen(pglobal->PatternPath)>0)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pglobal->root, NULL, "//global/TemplateDir", &pparameter)); 	
			if ( pparameter )
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, pglobal->PatternPath));
				
				EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
				pparameter = 0;
			}	
		}
	}
	
Error:
	CA_VariantClear(&pfile_name_var);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "XmlSaveDocument"
SElExceptionPtr XmlSaveDocument(const char* fileName)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	const char*     pfile_name = (fileName) ? fileName : "visionconfig.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	SGlobalPtr		pglobal = GetGlobal(NULL);    

	if(pglobal->pxml)
	{
		EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_name));   
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentsave (pglobal->pxml, NULL, pfile_name_var));
	}
	
Error:
	CA_VariantClear(&pfile_name_var);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "XmlLoadGlobal"
SElExceptionPtr XmlLoadGlobal(void)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pnode  = 0;   
	CAObjHandle     pparameter  = 0;
	CAObjHandle     pnode2 = 0;     
	CAObjHandle     pnode2old  = 0;        
	char*         	ptext= NULL;
	VBOOL         	is_xml_success = 1;
	SGlobalPtr		pglobal = GetGlobal(NULL);
	char			name[256];

	if(pglobal->pxml)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pglobal->root, NULL, "global", &pnode));  
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "TemplateDir", &pparameter)); 
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				strcpy(pglobal->PatternPath,  ptext);			
			}
		
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}	
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "BufferImg", &pnode2)); 
		while( pnode2 )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@name", &pparameter));
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				strcpy(name,  ptext);			
			}
			else
			{
				sprintf(name, "");
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
			
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode2, NULL, &ptext));
			if(ptext && strlen(name)>0)
			{
				EXCCHECK( gs_pIMAQ->LoadPicture( gs_pIMAQ, name, ptext)); 		
			}
		
			CA_FreeMemory(ptext);
			ptext=NULL;
			
			pnode2old = pnode2;
			MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2);
			EXCCHECKCVI( CA_DiscardObjHandle(pnode2old));
			pnode2old = 0;  
		}	
	}
	
Error:
	CA_FreeMemory(ptext);
	CA_DiscardObjHandle(pnode);   
	CA_DiscardObjHandle(pparameter);
	CA_DiscardObjHandle(pnode2old);  
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "XmlLoadTaskTree"
SElExceptionPtr XmlLoadTaskTree(const char* selected_task)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pnode  = 0;   
	CAObjHandle     pparameter  = 0;
	char*         	ptext= NULL;
	VBOOL         	is_xml_success = 1;
	SGlobalPtr		pglobal = GetGlobal(NULL);
	int32_t			count,
					i,
					index = 0;

	/* clear list */
	EXCCHECKCVI( GetNumListItems (gs_panel[MAIN], MAIN_TASKLIST, &count)); 
	for(i=0; i<count; i++)
	{
		EXCCHECKCVI( GetValueFromIndex (gs_panel[MAIN], MAIN_TASKLIST, i, &pnode));
		CA_DiscardObjHandle(pnode);			
	}
	EXCCHECKCVI( ClearListCtrl (gs_panel[MAIN], MAIN_TASKLIST));
		
	if(pglobal->pxml)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pglobal->root, NULL, "task", &pnode));  
		
		while(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@name", &pparameter)); 
			if ( pparameter )
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
				if(ptext)
				{
					EXCCHECKCVI( InsertListItem (gs_panel[MAIN], MAIN_TASKLIST, index, ptext, pnode));
					if( selected_task && 0==strcmp(ptext, selected_task))
					{
						EXCCHECKCVI( SetCtrlIndex(gs_panel[MAIN], MAIN_TASKLIST, index));
						EXCCHECKCVI( SetActiveCtrl(gs_panel[MAIN], MAIN_TASKLIST)); 
					}
					index++;

					CA_FreeMemory(ptext);
					ptext=NULL;
				}
				EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
				pparameter = 0;
				
				MSXML_IXMLDOMNodeGetnextSibling (pnode, NULL, &pnode);
			}
		}
	}
	
Error:
	CA_FreeMemory(ptext);
	CA_DiscardObjHandle(pparameter);   
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "XmlDeleteTask"
SElExceptionPtr XmlDeleteTask(const char* selected_task)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	char         	path[512];
	CAObjHandle     pnode = 0;  
	CAObjHandle     pnode_old = 0; 
	SGlobalPtr		pglobal = GetGlobal(NULL);

	if(pglobal->pxml)
	{
		sprintf(path, "task[@name='%s']", selected_task);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pglobal->root, NULL, path, &pnode));  
		EXCCHECKMSXML( MSXML_IXMLDOMNoderemoveChild (pglobal->root, NULL, pnode, &pnode_old));
	}
	
Error:
	CA_DiscardObjHandle(pnode);   
	CA_DiscardObjHandle(pnode_old);   
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "DisplayImage"
static void CVICALLBACK DisplayImage(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
 	int32_t			error = 0; 	
	int32_t			imaqVisible;
	SGlobalPtr		pglobal = GetGlobal(NULL);   

	EXCCHECKIMAQ( imaqIsWindowVisible (IMAQ_WINDOW_NUMBER, &imaqVisible));
	if(imaqVisible)
	{
		EXCCHECKIMAQ( imaqDisplayImage (pglobal->pImage, IMAQ_WINDOW_NUMBER, FALSE));	
	}

Error:
	EXCDISPLAY( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "MyDisplayFce"
SElExceptionPtr MyDisplayFce(Image* pImage, double fRate, double mean)
{
	SElExceptionPtr	pexception = NULL;
 	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);   

	EXCCHECKIMAQ( imaqDuplicate(pglobal->pImage, pImage));
	EXCCHECKCVI( PostDeferredCall( DisplayImage, NULL));
	EXCCHECKCVI( SetCtrlVal(gs_panel[CAMCFG], CAMCFG_FRATE, fRate));

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "iniDisplayWindow"
SElExceptionPtr iniDisplayWindow(int32_t top, int32_t left)
{
	SElExceptionPtr 	pexception = NULL;   
	int32_t				error = 0;
	SGlobalPtr			pglobal = GetGlobal(NULL);     
	Point				point;
	ToolWindowOptions	myToolOptions;
	int32_t 			window_high;
	int32_t 			window_width;
	int32_t 			window_top;
	int32_t 			window_left;
	int32_t 			panel_top;
	int32_t 			panel_left;
	int32_t 			tool_top;
	int32_t 			tool_left;
	static int 			is_init = 0;

	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_WINDOW_OUTLINE, ATTR_HEIGHT, &window_high));
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_WINDOW_OUTLINE, ATTR_WIDTH, &window_width));
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_WINDOW_OUTLINE, ATTR_TOP, &window_top));  
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_WINDOW_OUTLINE, ATTR_LEFT, &window_left));
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_TOOL_OUTLINE, ATTR_TOP, &tool_top));  
	EXCCHECKCVI( GetCtrlAttribute (gs_panel[PANEL], PANEL_TOOL_OUTLINE, ATTR_LEFT, &tool_left));
	EXCCHECKCVI( GetPanelAttribute (gs_panel[PANEL], ATTR_TOP, &panel_top));
	EXCCHECKCVI( GetPanelAttribute (gs_panel[PANEL], ATTR_LEFT, &panel_left));    
	
#if defined(_CVI_EXE_)     
	point.x=window_left;
	point.y=window_top;	
#endif
#if defined(_CVI_DLL_) || \
	defined(_WINDLL)
	point.x=window_left+panel_left+left;
	point.y=window_top+panel_top+top;
#endif
	
	EXCCHECKIMAQ( imaqMoveWindow (IMAQ_WINDOW_NUMBER, point) );
	
	gs_HWNDimaq = imaqGetSystemWindowHandle(IMAQ_WINDOW_NUMBER);
	if(gs_HWNDimaq==NULL)
	{
		EXCTHROW( -1, "imaqGetSystemWindowHandle() error!");
	}
	SetParent(gs_HWNDimaq,(HWND)pglobal->HWpanel); 

	EXCCHECKCVI( HWNDUTIL_AddWindow(gs_HWNDimaq));
	EXCCHECKCVI( HWNDUTIL_SetAttribute(gs_HWNDimaq, HWNDUTIL_ATTR_MOVABLE, 			FALSE) );
	EXCCHECKCVI( HWNDUTIL_SetAttribute(gs_HWNDimaq, HWNDUTIL_ATTR_TILE_BAR_VISIBLE, 	FALSE) );
	EXCCHECKCVI( HWNDUTIL_SetAttribute(gs_HWNDimaq, HWNDUTIL_ATTR_RESIZABLE, 			FALSE) );
	EXCCHECKCVI( HWNDUTIL_SetAttribute(gs_HWNDimaq, HWNDUTIL_ATTR_FLOATING , 			FALSE) ); 

	EXCCHECKIMAQ( imaqSetWindowSize (IMAQ_WINDOW_NUMBER, window_width, window_high));
	EXCCHECKIMAQ( imaqShowScrollbars (IMAQ_WINDOW_NUMBER, TRUE));
	EXCCHECKIMAQ( imaqShowWindow (IMAQ_WINDOW_NUMBER, FALSE)); 

	/* Setup tools for window interaction */
	memset(&myToolOptions, 0, sizeof(myToolOptions));
	myToolOptions.showSelectionTool 		= TRUE;     
	myToolOptions.showZoomTool      		= TRUE;          
	myToolOptions.showPointTool     		= FALSE;         
	myToolOptions.showLineTool      		= FALSE;           
	myToolOptions.showRectangleTool 		= FALSE;     
	myToolOptions.showOvalTool      		= FALSE;          
	myToolOptions.showPolygonTool   		= FALSE;       
	myToolOptions.showClosedFreehandTool 	= FALSE;
	myToolOptions.showPolyLineTool 			= FALSE;      
	myToolOptions.showFreehandTool 			= FALSE;     
	myToolOptions.showAnnulusTool 			= FALSE;          
	myToolOptions.showRotatedRectangleTool 	= FALSE;

	EXCCHECKIMAQ( imaqSetupToolWindow (FALSE, 3, &myToolOptions));

	gs_HWNDtool = imaqGetToolWindowHandle ();
	if(gs_HWNDtool==NULL)
	{
		EXCTHROW( -1, "imaqGetToolWindowHandle() error!");
	}
	SetParent(gs_HWNDtool,(HWND)pglobal->HWpanel); 

	EXCCHECKCVI( HWNDUTIL_AddWindow(gs_HWNDtool)); 
	EXCCHECKCVI( HWNDUTIL_SetAttribute(gs_HWNDtool, HWNDUTIL_ATTR_TILE_BAR_VISIBLE, FALSE) );

#if defined(_CVI_EXE_)   
	point.x = tool_left;
	point.y = tool_top;	
#endif
#if defined(_CVI_DLL_) || \
	defined(_WINDLL)
	point.x = tool_left+panel_left+left;
	point.y = tool_top+panel_top+top;
#endif

	EXCCHECKIMAQ( imaqMoveToolWindow (point));
	EXCCHECKIMAQ( imaqShowToolWindow (FALSE));
		
Error:
	EXCRETHROW( pexception);       
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "closeDisplayWindow"
SElExceptionPtr closeDisplayWindow(void)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;

	EXCCHECKCVI( HWNDUTIL_RemoveWindow(gs_HWNDimaq));
	EXCCHECKCVI( HWNDUTIL_RemoveWindow(gs_HWNDtool)); 

Error:
	EXCRETHROW( pexception);       
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckStepName"
SElExceptionPtr	CheckStepName(
	int panel,
	int	control
)
{
	SElExceptionPtr pexception = NULL;     
	int32_t			error = 0;
	char			name[256];
	
	EXCCHECKCVI( GetCtrlVal( panel, control, name));
	RemoveSurroundingWhiteSpace(name);
	if ( name[0] == '\0' )
	{
		SetActiveCtrl( panel, control);
		EXCTHROW( -1, "Step Name must be specified");
	}

Error:
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "CheckTaskName"
SElExceptionPtr	CheckTaskName(
	int panel,
	int	control
)
{
	SElExceptionPtr pexception = NULL; 
	int32_t			error = 0;
	char			name[256];
	
	EXCCHECKCVI( GetCtrlVal( panel, control, name));
    RemoveSurroundingWhiteSpace(name);
	if ( strlen(name)==0 )
	{
		EXCCHECKCVI( SetActiveCtrl( panel, control));
		EXCTHROW( -1, "Task name must be specified!");
	}
	
Error:
	EXCRETHROW( pexception);
}   

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "CheckStepTemplate"
SElExceptionPtr CheckStepTemplate(int panel, Image* pImage)
{
	SElExceptionPtr pexception = NULL;  
	int32_t			error = 0;
	
	if(pImage==NULL)
	{
		EXCTHROW( -1, "Template must be specified");    
	}

Error:
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMakeBuffer"
SElExceptionPtr DisplayAllBuffers( int panel, int control) 
{
	SElExceptionPtr	pexception = NULL;  
	int				error = 0;     
	uint32_t		i, count = (gs_pIMAQ->images)->GetCount(gs_pIMAQ->images);
	const char*		pname = NULL;
	
	DeleteListItem (panel, control, 0, -1);
	for ( i = 0; i < count; i++)
	{
		pname = (gs_pIMAQ->images)->GetNameOnIndex(gs_pIMAQ->images, i);
		if ( pname && *pname != '@' && stricmp(pname, INTERNAL_IMAGE_NAME) )
		{
			InsertListItem (panel, control, -1, pname, i);
		}
	}
	
Error:
	return 0;	
}

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "CreateRegion"
SElExceptionPtr CreateRegion(int window, const char* buffer)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	ROI*			proi = NULL;
	char*			pend = NULL;

	EXCCHECK( gs_pIMAQ->CreateROIFromString(buffer, &proi, NULL, NULL));
	if(proi)
	{
		EXCCHECKIMAQ( imaqSetWindowROI( IMAQ_WINDOW_NUMBER, proi));
	}

Error:
	if(proi) imaqDispose( proi);
	EXCRETHROW( pexception);      
}

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "FormatRegion"
SElExceptionPtr FormatRegion(int window, int coIndex, char buffer[], uint32_t bufferSize)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	ROI*			puser_ROI = NULL;
	int				count = 0;
	ContourID		contour_ID;
	ContourInfo2*	pcontour_info = NULL;
	char			message[255] = "";
	
	puser_ROI = imaqGetWindowROI (window); 
	count = imaqGetContourCount( puser_ROI);
	
	if(count>0)
	{
		contour_ID = imaqGetContour (puser_ROI, coIndex);
		pcontour_info = imaqGetContourInfo2( puser_ROI, contour_ID);
		
		switch( pcontour_info->type)
		{
			case IMAQ_POINT: /* The contour represents a point. */
				sprintf( message, "point(%d,%d)",
						 (int)pcontour_info->structure.point->x,
						 (int)pcontour_info->structure.point->y
						);
				break;
			case IMAQ_LINE: /* The contour represents a line. */
				sprintf( message, "line(%d,%d,%d,%d)",
						 (int)pcontour_info->structure.line->start.x,
						 (int)pcontour_info->structure.line->start.y,
						 (int)pcontour_info->structure.line->end.x,
						 (int)pcontour_info->structure.line->end.y
						);
				break;
			case IMAQ_RECT: /* The contour represents a rectangle. */
				sprintf( message, "rect(%d,%d,%d,%d)",
						 (int)pcontour_info->structure.rect->left,
						 (int)pcontour_info->structure.rect->top,
						 (int)pcontour_info->structure.rect->width,
						 (int)pcontour_info->structure.rect->height
						);
				break;
			case IMAQ_OVAL: /* The contour represents an oval. */
				sprintf( message, "oval(%d,%d,%d,%d)",
						 (int)pcontour_info->structure.ovalBoundingBox->left,
						 (int)pcontour_info->structure.ovalBoundingBox->top,
						 (int)pcontour_info->structure.ovalBoundingBox->width,
						 (int)pcontour_info->structure.ovalBoundingBox->height
						);
				break;
			case IMAQ_ROTATED_RECT: /* The contour represents a rotated rectangle. */
				sprintf( message, "rect(%d,%d,%d,%d,%.3Lf)",
						 (int)pcontour_info->structure.rotatedRect->left,
						 (int)pcontour_info->structure.rotatedRect->top,
						 (int)pcontour_info->structure.rotatedRect->width,
						 (int)pcontour_info->structure.rotatedRect->height,
						 (double)pcontour_info->structure.rotatedRect->angle
						);
				break;
			case IMAQ_EMPTY_CONTOUR: /* The contour is empty. */
			case IMAQ_CLOSED_CONTOUR: /* The contour represents a series of connected points where the last point connects to the first. */
			case IMAQ_OPEN_CONTOUR: /* The contour represents a series of connected points where the last point does not connect to the first. */
			case IMAQ_ANNULUS: /* The contour represents an annulus. */
				sprintf( message, "not supported");
			break;
		}
	}

	if ( bufferSize <= strlen(message))
	{
			error = strlen( message) + 1;
			if ( bufferSize > 0 )
			{
				memcpy(buffer, message,bufferSize-1);
				buffer[bufferSize-1] = '\0';
			}
	}
	else
	{
		strcpy(buffer, message);
	}

Error:
	if ( puser_ROI ) imaqDispose( puser_ROI);
	if ( pcontour_info ) imaqDispose( pcontour_info);
	EXCRETHROW( pexception);      
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ResultToView"
SElExceptionPtr ResultToView(SELStringPtr result)
{
	SElExceptionPtr pexception = NULL; 
	int32_t			error = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
	Image*			pimage = NULL;
	char			image_name[128] = "";
	CAObjHandle		pxml = 0; 
	CAObjHandle 	pstep = 0;
	char*			sname = NULL;

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (pxml, NULL, result->GetPtr(result), &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "/step/image", &pstep));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pstep, NULL, &sname));

	pimage = gs_pIMAQ->GetImage(gs_pIMAQ, sname);	
	if(pimage)
	{
		EXCCHECKIMAQ( imaqDisplayImage (pimage, IMAQ_WINDOW_NUMBER, FALSE));
	}

Error:
	CA_FreeMemory(sname);
	CA_DiscardObjHandle(pstep);
	CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);      
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ResultToBktree"
SElExceptionPtr ResultToBktree(CAObjHandle bktree, SELStringPtr result)
{
	SElExceptionPtr pexception = NULL; 
	SELStringPtr 	tree = NULL;
	CAObjHandle		pxml = 0; 
	CAObjHandle 	pstep = 0;
	CAObjHandle 	pchild = 0;
	CAObjHandle 	pchild_old = 0;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1;
	char			*sname = NULL, *svalue = NULL;
	short			AXvalue;
	int32_t			i = 0;

	/* result */
	EXCCHECK( elstring_new(BKTREE_RESULT_HEADER, &tree));
	EXCCHECK( tree->Append(tree, "|root|Step^i0\n"));
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (pxml, NULL, result->GetPtr(result), &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "/step", &pstep));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeGetfirstChild (pstep, NULL, &pchild)); 
	while(pchild)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnodeName (pchild, NULL, &sname));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pchild, NULL, &svalue));
		
		EXCCHECK( tree->Format(tree, "root|param%d|%s^i0$%s\n", i++, sname, svalue));

		CA_FreeMemory(sname);
		sname=NULL;
		CA_FreeMemory(svalue);
		svalue=NULL;

		pchild_old = pchild;
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pchild_old, NULL, &pchild)); 
		EXCCHECKCOM( CA_DiscardObjHandle(pchild_old));
		pchild_old = 0;
	}

	EXCCHECKCOM( BKLib2__DBKTreeFromString( bktree, 
											NULL, 
											tree->GetPtr(tree),
											"$", "\n",
											&AXvalue));

Error:
	CA_DiscardObjHandle(pstep);
	CA_DiscardObjHandle(pchild);
	CA_DiscardObjHandle(pchild_old);
	CA_DiscardObjHandle(pxml);
	elstring_delete( &tree);
	EXCRETHROW( pexception);
}