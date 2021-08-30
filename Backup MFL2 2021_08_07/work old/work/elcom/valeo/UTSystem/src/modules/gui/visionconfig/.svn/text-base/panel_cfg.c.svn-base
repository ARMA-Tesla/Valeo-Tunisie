#include "global.h"
#include <userint.h>
#include "panel_cfg.h"
#include "visionconfig.h" 
#include "toolbox.h"

extern int 	gs_panel[];  

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "InitializeConfigDialog"
SElExceptionPtr InitializeConfigDialog(void)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);

	EXCCHECKCVI( SetCtrlVal(gs_panel[CFG], CFG_SPATH, pglobal->PatternPath));
	EXCCHECKCVI( SetPanelPos (gs_panel[CFG], 15, 0));
	EXCCHECKCVI( DisplayPanel (gs_panel[CFG]));
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "InitializeConfigDialog"
static SElExceptionPtr XmlSetTemplatePath(void)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);
	HRESULT         hr = 0;       
	CAObjHandle     pparameter  = 0;    
	VBOOL         	is_xml_success = 1;  

	if(pglobal->pxml)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pglobal->pxml, NULL, "//global/TemplateDir", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, pglobal->PatternPath));
			
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}	
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelCfgExit"
int CVICALLBACK OnPanelCfgExit(int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t		error = 0;
	SGlobalPtr	pglobal = GetGlobal(NULL);   
	char		new_path[MAX_PATHNAME_LEN+1];
	int			size;

	switch (event)
		{
		case EVENT_COMMIT:
			if ( control == CFG_OK )
			{
				EXCCHECKCVI( GetCtrlVal(panel, CFG_SPATH, new_path));
				strcpy(pglobal->PatternPath, new_path);
				
				if(0 != FileExists (new_path, &size))
				{
					EXCCHECK( XmlSetTemplatePath());
				}
				else
				{
					MessagePopup("Error", "Not valid directory!");
					break;
				}
			}
			EXCCHECKCVI( HidePanel( panel));
			EXCCHECKCVI( DisplayPanel( gs_panel[MAIN]));
			break;
		}
Error:
	return 0;
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "OnDirectory"
int CVICALLBACK OnDirectory (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;        
	int32_t			error = 0; 
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	char			new_path[MAX_PATHNAME_LEN+1];
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			if( DirSelectPopup (pglobal->PatternPath, "Select Directory", 1, 1, new_path) );
			{		
				if(strlen(new_path)>0)
				{
					strcat(new_path, "\\");
					EXCCHECKCVI( SetCtrlVal(panel, CFG_SPATH, new_path));
				}
			}
			break;
	}

Error:
	return 0;
}



