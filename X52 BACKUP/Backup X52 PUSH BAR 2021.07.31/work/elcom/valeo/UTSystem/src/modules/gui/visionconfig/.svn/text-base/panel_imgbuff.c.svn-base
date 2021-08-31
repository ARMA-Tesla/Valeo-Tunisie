#include "global.h"
#include "toolbox.h"
#include "visionconfig.h"
#include "visionconfig_fnc.h" 
#include "panel_task.h"

extern int 		gs_panel[];
extern SIMAQPtr	gs_pIMAQ;

static SElExceptionPtr XmlSaveChanges(int panel, int	control, int control2);

/*-------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "InitializeImgBufferDialog"
SElExceptionPtr InitializeImgBufferDialog(void)     
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	VBOOL         	is_xml_success = 1; 
	HRESULT         hr = 0;  
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	 
	EXCCHECK( DisplayAllBuffers(gs_panel[IMGBUFFER], IMGBUFFER_IMAGE_BUFFER_LIST)); 
	
	if ( pglobal->step!=0) /* edit step */     
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->step, NULL, VFALSE, &pglobal->step_clone)); 
		
		XML_NODE_GET_TEXT(pglobal->step, "@name",
		{
			EXCCHECKCVI( SetCtrlVal( gs_panel[IMGBUFFER], IMGBUFFER_SNAME, ptext));
		});
		
		XML_NODE_GET_TEXT(pglobal->step, "buffer_name",
		{
			EXCCHECKCVI( SetCtrlVal( gs_panel[IMGBUFFER], IMGBUFFER_BUFFER_NAME, ptext));  
		});
	}
	else /* create new step */
	{
		VARIANT	type = CA_VariantEmpty ();
		
		EXCCHECKCVI( SetCtrlVal( gs_panel[IMGBUFFER], IMGBUFFER_SNAME, "")); 
		
		CA_VariantSetCString (&type, "Element");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateNode (pglobal->pxml, NULL, type, "step", "", &pglobal->step_clone));
		EXCCHECKMSXML( MSXML_IXMLDOMElementsetAttribute (pglobal->step_clone, NULL, "name", CA_VariantEmpty ()));
	}
	
	EXCCHECKIMAQ( imaqSetCurrentTool (IMAQ_NO_TOOL));
	EXCCHECKIMAQ( imaqShowToolWindow (TRUE));
	
	EXCCHECKCVI( SetPanelPos (gs_panel[IMGBUFFER], 15, 0));
	EXCCHECKCVI( DisplayPanel (gs_panel[IMGBUFFER]));	
	EXCCHECKCVI( SetActivePanel (gs_panel[IMGBUFFER]));  
	
Error:
	EXCRETHROW( pexception);    
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelImgbuffExit"
int CVICALLBACK OnPanelImgbuffExit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	char			pbuffer_name[256];
	SGlobalPtr		pglobal = GetGlobal(NULL);   

	switch (event)
	{
		case EVENT_COMMIT:
			if ( control == IMGBUFFER_OK )
			{
				EXCCHECKCVI( GetCtrlVal( gs_panel[IMGBUFFER], IMGBUFFER_BUFFER_NAME, pbuffer_name));  
				RemoveSurroundingWhiteSpace(pbuffer_name);
				
				if(0!=strlen(pbuffer_name))
				{
					EXCCHECK( CheckStepName(panel, IMGBUFFER_SNAME));
					EXCCHECK( XmlSaveChanges(panel, IMGBUFFER_SNAME, IMGBUFFER_BUFFER_NAME));
					EXCCHECK( InitializeTaskDialog());
				}
				else
				{
					MessagePopup(_("Error"),_("You must specify buffer name!"));
					goto Error;
				}
			}
			else if( control==IMGBUFFER_CANCEL)
			{
				/* clean clone */
				EXCCHECKCVI( CA_DiscardObjHandle(pglobal->step_clone));
				pglobal->step_clone = 0;
			}
			
			EXCCHECKIMAQ( imaqSetCurrentTool (IMAQ_NO_TOOL));
			EXCCHECKIMAQ( imaqShowToolWindow (FALSE));
			EXCCHECKCVI( HidePanel( panel));
			pglobal->step = 0; 
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "XmlSaveChanges"
static SElExceptionPtr	XmlSaveChanges(int panel, int	control, int control2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	VBOOL         	is_xml_success = 1; 
	HRESULT         hr = 0;  
	char			name[MAX_STEP_NAME];
	char			buff_name[256];
	SGlobalPtr		pglobal = GetGlobal(NULL);   

	if(pglobal->task_clone && pglobal->step_clone)
	{
		/* step name */
		EXCCHECKCVI( GetCtrlVal( panel, control, name));
		RemoveSurroundingWhiteSpace(name);

		XML_NODE_SET_TEXT(pglobal->step_clone, "@name", name); 
		XML_NODE_ADD_NEW_ELEMENT(pglobal->pxml, pglobal->step_clone, "type", "image to buffer"); 
		
		EXCCHECKCVI( GetCtrlVal( panel, control2, buff_name));
		RemoveSurroundingWhiteSpace(buff_name);
		XML_NODE_ADD_NEW_ELEMENT(pglobal->pxml, pglobal->step_clone, "buffer_name", buff_name); 
		
		/* save changes */
		XML_STEP_SAVE_CHANGES();
	}

Error:
	EXCRETHROW( pexception);      
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelImgbuffPreview"
int CVICALLBACK OnPanelImgbuffPreview (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;
	char			buffer[256] = "";    
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, IMGBUFFER_BUFFER_NAME, buffer));
			RemoveSurroundingWhiteSpace(buffer);
			if ( strlen(buffer)==0 )
			{
				MessagePopup(_("Error"), _("You must specify buffer name!"));
				goto Error;
			}
			EXCCHECK( gs_pIMAQ->MakeImageCopy( gs_pIMAQ, buffer, BEGIN_STEP_IMAGE_NAME)); 
			EXCCHECK( DisplayAllBuffers( panel, IMGBUFFER_IMAGE_BUFFER_LIST));    
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*-------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnImgBuffList"
int CVICALLBACK OnImgBuffList (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;      
	int32_t			error = 0;
	int				index;  
	char			buffer[256];
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlIndex(panel, control, &index));
			EXCCHECKCVI( GetLabelFromIndex (panel, control, index, buffer));
			EXCCHECKCVI( SetCtrlVal(panel, IMGBUFFER_BUFFER_NAME, buffer));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}
