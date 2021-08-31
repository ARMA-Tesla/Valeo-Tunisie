#include "global.h"
#include <toolbox.h>
#include <utility.h> 
#include <elstring.h>
#include <bklib/bklib2.h>
#include "visionconfig.h" 
#include "visionconfig_fnc.h"
#include "panel_task.h"
#include "panel_pattern.h"
#include "panel_histogram.h"
#include "panel_goldenpattern.h"  
#include "panel_mathop.h" 
#include "panel_resample.h"
#include "panel_imgbuff.h"
#include "panel_facecheck.h"

#define MAX_STEPS 		128

#define BKTREE_HEADER "root|Task^w115$result|Result^w115\n"

#define CTRL_HIGHT		60
#define CTRL_WIDTH		253
#define CTRL_STEP		65 

typedef struct	_SStepData
{
	int32_t			controlID1;
	int32_t			controlID2;
	int32_t			controlID3;
	int32_t			controlID4;
	int32_t			controlID5;
	int32_t			controlID6;
	char			type[256];
	CAObjHandle 	pnode;
} StepData, *StepDataPtr;

extern int 			gs_panel[];  
extern SIMAQPtr 	gs_pIMAQ;

static StepData		gs_StepData[MAX_STEPS];
static int32_t		gs_StepDataSize;
static int32_t		gs_StepDataActual;

static SElExceptionPtr DisplayStepsForTasks(int panel, CAObjHandle pstep);
static SElExceptionPtr DisplayStep(int	panel, int index, CAObjHandle ptask);
static SElExceptionPtr DeleteStepDisplay(int panel);
static SElExceptionPtr HighlightActiveStep(int32_t step_number);

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "InitializeTaskDialog"
SElExceptionPtr InitializeTaskDialog(void)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	int32_t			left, top;
	int32_t			ActiveStep = gs_StepDataActual;
	HRESULT         hr = 0;  
	VBOOL         	is_xml_success = 1;   
	int				visible;
	short			AXvalue;
	
	EXCCHECKCOM( BKLib2__DBKTreeFromString( pglobal->bktree, 
											NULL, 
											BKTREE_HEADER
											"|root|Task^i0\n", 
											"$", "\n",
											&AXvalue));

	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_BKTREE, ATTR_LEFT, 0)); 
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_BKTREE, ATTR_TOP, CTRL_STEP));
	
	if( pglobal->task!=0 && pglobal->task_clone==0) /* edit step */
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->task, NULL, VTRUE, &pglobal->task_clone));
		
		XML_NODE_GET_TEXT(pglobal->task, "@name",
		{
			EXCCHECKCVI( SetCtrlVal(gs_panel[PNLTASKS], PNLTASKS_TASK_NAME, ptext));  
		});
	}
	else if(pglobal->task==0 && pglobal->task_clone==0) /* create new step */
	{
		VARIANT	type = CA_VariantEmpty ();
		
		EXCCHECKCVI( SetCtrlVal(gs_panel[PNLTASKS], PNLTASKS_TASK_NAME, "")); 
		
		CA_VariantSetCString (&type, "Element");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateNode (pglobal->pxml, NULL, type, "task", "", &pglobal->task_clone));
		EXCCHECKMSXML( MSXML_IXMLDOMElementsetAttribute (pglobal->task_clone, NULL, "name", CA_VariantEmpty ()));
	}
	
	/* reload steps */
	EXCCHECK( DisplayStepsForTasks(gs_panel[PNLSTEPS], pglobal->task_clone));   
	
	EXCCHECKCVI( GetPanelAttribute (gs_panel[PNLSTEPS], ATTR_VISIBLE, &visible));
	if( !visible)
	{
		EXCCHECK( gs_pIMAQ->MakeImageCopy( gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, INTERNAL_IMAGE_NAME));  
	
		EXCCHECKCVI( GetCtrlAttribute (gs_panel[PNLTASKS], PNLTASKS_OUTLINE_STEP, ATTR_LEFT, &left)); 
		EXCCHECKCVI( GetCtrlAttribute (gs_panel[PNLTASKS], PNLTASKS_OUTLINE_STEP, ATTR_TOP, &top));    
		EXCCHECKCVI( SetPanelPos (gs_panel[PNLSTEPS], top + 15, left));
		EXCCHECKCVI( DisplayPanel (gs_panel[PNLSTEPS]));
		EXCCHECKCVI( SetPanelPos (gs_panel[PNLTASKS], 15, 0));
		EXCCHECKCVI( DisplayPanel (gs_panel[PNLTASKS]));
		
		ActiveStep = 0;
		EXCCHECK( HighlightActiveStep(0)); 
	}
	else
	{
		EXCCHECK( HighlightActiveStep(ActiveStep) );  
	}
	
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
	VBOOL         	is_xml_success = 1; 
	HRESULT         hr = 0;    
	char			name[256];
	
	if(pglobal->task_clone) 
	{
		/* task name */
		EXCCHECKCVI( GetCtrlVal( panel, control, name)); 
		RemoveSurroundingWhiteSpace(name);
		
		XML_NODE_SET_TEXT(pglobal->task_clone, "@name", name);  
		
		/* save changes */
		XML_TASK_SAVE_CHANGES();
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnPanelTaskExit"
int CVICALLBACK OnPanelTaskExit(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;       
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);
	char			name[256] = "";

	switch (event)
	{
		case EVENT_COMMIT:
			if( control==PNLTASKS_OK )
			{
				EXCCHECK( CheckTaskName( panel, PNLTASKS_TASK_NAME));
				EXCCHECK( XmlSaveChanges( panel, PNLTASKS_TASK_NAME));
				EXCCHECK( DeleteStepDisplay( panel));
			}
			else if( control==PNLTASKS_CANCEL)
			{
				/* clean clone */
				EXCCHECKCVI( CA_DiscardObjHandle(pglobal->task_clone));
				pglobal->task_clone = 0;
			}
			
			IMAQ_DISPLAY_IMAGE_INTERNAL();

			EXCCHECKCVI( GetCtrlVal( panel, PNLTASKS_TASK_NAME, name));
			EXCCHECK( XmlLoadTaskTree(name));
			EXCCHECKCVI( HidePanel(panel));
			pglobal->task = 0;   
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnDeleteTaskStep"
int CVICALLBACK OnDeleteTaskStep (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;    
	int32_t			error = 0;
	CAObjHandle 	pstep = 0;
	CAObjHandle 	pstepold = 0;  
	VBOOL         	is_xml_success = 1; 
	HRESULT         hr = 0;     
	SGlobalPtr		pglobal = GetGlobal(NULL);   

	switch (event)
	{
		case EVENT_COMMIT:

			pstep = (CAObjHandle)callbackData; 
			
			EXCCHECKMSXML( MSXML_IXMLDOMNoderemoveChild (pglobal->task_clone, NULL, pstep, &pstepold));
			EXCCHECKCVI( CA_DiscardObjHandle(pstepold));
			pstepold = 0;
			
			gs_StepDataActual = 0;
			
			EXCCHECK( DisplayStepsForTasks( panel, pglobal->task_clone));
			break;
	}
	
Error:
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "XmlCreateTmpTask"
SElExceptionPtr XmlCreateTmpTask(const char* task_name, int32_t last_step)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	CAObjHandle		tmp_task = 0, tmp_task_new = 0;
	CAObjHandle		tmp_step = 0, tmp_step_new = 0; 
	HRESULT         hr = 0; 
	VBOOL         	is_xml_success = 1; 
	int				i;
	SGlobalPtr		pglobal = GetGlobal(NULL);  
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (pglobal->task_clone, NULL, VFALSE, &tmp_task));
	if( tmp_task)
	{
		/* task name */
		XML_NODE_SET_TEXT(tmp_task, "@name", task_name);  
	
		for(i=0; i<last_step; i++)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodecloneNode (gs_StepData[i].pnode, NULL, VTRUE, &tmp_step));
			EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (tmp_task, NULL, tmp_step, &tmp_step_new));
			EXCCHECKCVI( CA_DiscardObjHandle(tmp_step));
			tmp_step = 0;
			EXCCHECKCVI( CA_DiscardObjHandle(tmp_step_new));
			tmp_step_new = 0;
		}
	
		EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pglobal->root, NULL, tmp_task, &tmp_task_new));
		EXCCHECKCVI( CA_DiscardObjHandle(tmp_task));
		tmp_task = 0;
		EXCCHECKCVI( CA_DiscardObjHandle(tmp_task_new));
		tmp_task_new = 0;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "XmlDeleteTmpTask"
SElExceptionPtr XmlDeleteTmpTask(const char* task_name)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	CAObjHandle		tmp_task = 0, tmp_task_old = 0;
	HRESULT         hr = 0; 
	VBOOL         	is_xml_success = 1; 
	SGlobalPtr		pglobal = GetGlobal(NULL); 
	char			path[256];
	
	/* task name */
	sprintf(path, "task[@name='%s']", task_name);
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pglobal->root, NULL, path, &tmp_task)); 
	if( tmp_task)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNoderemoveChild (pglobal->root, NULL, tmp_task, &tmp_task_old));
		EXCCHECKCVI( CA_DiscardObjHandle(tmp_task_old));
		tmp_task_old = 0;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DoSteps"
SElExceptionPtr DoSteps(int32_t step) 
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			error = 0;
	char			result_name[256] = "";
	CAObjHandle		pxml_store = 0; 
	CAObjHandle		pxml = 0; 
	CAObjHandle		plist = 0; 
	CAObjHandle		pchild = 0;
	CAObjHandle		ptask = 0;
	CAObjHandle		pitem = 0;
	CAObjHandle		pparameter = 0;
	char*         	ptext = NULL;
	SGlobalPtr		pglobal = GetGlobal(NULL);  
	SELStringPtr	result = NULL;
	SELStringPtr 	tree = NULL;
	VBOOL			is_xml_success = 1;
	short			AXvalue;
	char			stype[128];
	char			*sname = NULL;
	char			*svalue = NULL;
	
	pxml_store = gs_pIMAQ->pxml;
	gs_pIMAQ->pxml = pglobal->pxml;
	
	EXCCHECK( XmlCreateTmpTask("@TempTest@", step));
	
	sprintf(result_name, END_STEP_IMAGE_NAME"_%d", (int)CmtGetCurrentThreadID ());
	EXCCHECK( gs_pIMAQ->RemoveImage(gs_pIMAQ, result_name));
	
	EXCCHECK( elstring_new( NULL, &result));
	EXCCHECK( gs_pIMAQ->DoTask(gs_pIMAQ, INTERNAL_IMAGE_NAME, "@TempTest@", result));

	/* result */
	EXCCHECK( elstring_new(BKTREE_HEADER, &tree));
	EXCCHECK( tree->Append(tree, "|root|Task^i0\n"));
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (pxml, NULL, result->GetPtr(result), &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "/task", &ptask));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (ptask, NULL, "step", &plist));   
	if(plist)
	{
		int32_t	length, i, j = 0;
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));

		for(i=0; i<length; i++)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pitem));
			if(pitem)
			{
				/* type */
				strcpy(stype, "unknown");
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pitem, NULL, "@type", &pparameter));
				if(pparameter)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext)
					{
						strcpy(stype, ptext);		 
					}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					CA_FreeMemory(ptext);
					ptext=NULL;
				}

				EXCCHECK( tree->Format(tree, "root|step%d|Step^i0$%s\n", i, stype));

				EXCCHECKMSXML( MSXML_IXMLDOMNodeGetfirstChild (pitem, NULL, &pchild)); 
				while(pchild)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnodeName (pchild, NULL, &sname));
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pchild, NULL, &svalue));
					
					EXCCHECK( tree->Format(tree, "step%d|param%d|%s^i0$%s\n", i, j++, sname, svalue));

					CA_FreeMemory(sname);
					sname=NULL;
					CA_FreeMemory(svalue);
					svalue=NULL;

					EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pchild, NULL, &pchild)); 
				}
			}
		}
	}

	EXCCHECKCOM( BKLib2__DBKTreeFromString( pglobal->bktree, 
											NULL, 
											tree->GetPtr(tree),
											"$", "\n",
											&AXvalue));

	EXCCHECK( gs_pIMAQ->MakeImageCopy( gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, result_name));
	
Error:
	elstring_delete( &result);
	elstring_delete( &tree);
	if( strlen(result_name)>0) gs_pIMAQ->RemoveImage(gs_pIMAQ, result_name);
	gs_pIMAQ->pxml = pxml_store;
	XmlDeleteTmpTask("@TempTest@");
	
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnSelectTaskStep"
int CVICALLBACK OnSelectTaskStep (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);       
	int				step = ((int)callbackData);

	switch (event)
	{
		case EVENT_COMMIT:
			
			if(step)
			{
				/* select actual step */
				if( gs_StepDataActual!=0)	
					EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], gs_StepData[gs_StepDataActual-1].controlID1, ATTR_FRAME_COLOR, 0x00C0C0C0));
				if( gs_StepDataActual==0)	
					EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_DECORATION_STEP0, ATTR_FRAME_COLOR, 0x00C0C0C0));
			
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], gs_StepData[step-1].controlID1, ATTR_FRAME_COLOR, 0x00808080));
				gs_StepDataActual = step;
				EXCCHECK( DoSteps(step));
			}
			else
			{	
				EXCCHECK( gs_pIMAQ->MakeImageCopy( gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, INTERNAL_IMAGE_NAME));	
			}
			IMAQ_DISPLAY_IMAGE_BEGIN();
			break;	
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;	
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnStepAdd"
int CVICALLBACK OnStepAdd(int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	int				step_type = 0, index;
	SGlobalPtr		pglobal = GetGlobal(NULL);      
	
	switch (event)
		{
		case EVENT_COMMIT:
			
			EXCCHECKCVI( OnSelectTaskStep( panel, control, event, ((void*)(gs_StepDataSize)), eventData1, eventData2));
			
			EXCCHECKCVI( GetCtrlIndex (panel, PNLTASKS_STEPTYPE, &index));
			EXCCHECKCVI( GetValueFromIndex (panel, PNLTASKS_STEPTYPE, index, &step_type));
			
			pglobal->step = 0;
			
			switch ( step_type )
			{
				case 1:
					EXCCHECK( InitializePatternDialog());
					break;
				case 2:
					EXCCHECK( InitializeHistogramDialog());
					break;
				case 3:
					EXCCHECK( InitializeGoldenPatternDialog());
					break;
				case 4:
					EXCCHECK( InitializeMathOperationDialog());
					break;
				case 5:
					EXCCHECK( InitializeResampleDialog());
					break;
				case 6:
					EXCCHECK( InitializeImgBufferDialog());
					break;
				case 7:
					EXCCHECK( InitializeFaceCheckDialog());
					break;
			}
			break;
		}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "OnEditTaskStep"
int CVICALLBACK OnEditTaskStep (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	SGlobalPtr		pglobal = GetGlobal(NULL);   
	int				index = 0;
	
	switch (event)
		{
		case EVENT_COMMIT:
			
			index = (int)callbackData;
			
			EXCCHECKCVI( OnSelectTaskStep( panel, control, event, (void*)(index-1), eventData1, eventData2));
			
			/* set active step */
			if(gs_StepDataActual!=0)	
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], gs_StepData[gs_StepDataActual-1].controlID1, ATTR_FRAME_COLOR, 0x00C0C0C0));
			else	
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_DECORATION_STEP0, ATTR_FRAME_COLOR, 0x00C0C0C0));
			
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], gs_StepData[index-1].controlID1, ATTR_FRAME_COLOR, 0x00808080));
			
			gs_StepDataActual = index;
			
			pglobal->step = gs_StepData[index-1].pnode;
			
			if ( stricmp(gs_StepData[index-1].type, "histogram" ) == 0 )
			{
				EXCCHECK( InitializeHistogramDialog());
			}
			else if ( stricmp(gs_StepData[index-1].type, "pattern" ) == 0 )
			{
				EXCCHECK( InitializePatternDialog());
			}
			else if ( stricmp(gs_StepData[index-1].type, "math" ) == 0 )
			{
				EXCCHECK( InitializeMathOperationDialog());
			}
			else if ( stricmp(gs_StepData[index-1].type, "golden template" ) == 0 )
			{
				EXCCHECK( InitializeGoldenPatternDialog());
			}
			else if ( stricmp(gs_StepData[index-1].type, "resample" ) == 0 )
			{
				EXCCHECK( InitializeResampleDialog());
			}
			else if ( stricmp(gs_StepData[index-1].type, "image to buffer" ) == 0 )
			{
				EXCCHECK( InitializeImgBufferDialog());
			}
			else if ( stricmp(gs_StepData[index-1].type, "face check" ) == 0 )
			{
				EXCCHECK( InitializeFaceCheckDialog());
			}
			break;
		}

Error:
	EXCDISPLAY( pexception); 
	return 0;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "DisplayStepsForTasks"
static SElExceptionPtr DisplayStepsForTasks(
	int 		panel,
	CAObjHandle ptask
)
{
	SElExceptionPtr	pexception = NULL;   
	int32_t			error = 0;
	CAObjHandle     pparameter  = 0; 
	CAObjHandle		pstep = 0;
	int				index = 0;
	char*         	ptext= NULL;     
	HRESULT         hr = 0; 
	VBOOL         	is_xml_success = 1;   
	
	if(ptask)
	{
		EXCCHECK( DeleteStepDisplay( panel));
	
		for(index=0; index<MAX_STEPS; index++)
		{	
			CA_DiscardObjHandle(gs_StepData[index].pnode);
			gs_StepData[index].pnode = 0;
		}

		index = 0;
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (ptask, NULL, "step", &pstep)); 
		while(pstep)
		{
			EXCCHECK( DisplayStep( panel, index++, pstep));    
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnextSibling (pstep, NULL, &pstep));  
		}
	
		if( index==0)
		{
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_BKTREE, ATTR_LEFT, 0)); 
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_BKTREE, ATTR_TOP, CTRL_STEP));
		}
	}

Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DisplayStep"
static SElExceptionPtr DisplayStep(
	 int panel,
	 int index,
	 CAObjHandle pstep
)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	int				top;
	int				id;
	char			message[256]="";
	int				bitmapID0, bitmapID1, bitmapID2;
	HRESULT         hr = 0;    
	VBOOL         	is_xml_success = 1;     
	
	EXCCHECKCVI( GetCtrlBitmap (panel, PNLSTEPS_PICTURES, 0, &bitmapID0));
	EXCCHECKCVI( GetCtrlBitmap (panel, PNLSTEPS_PICTURES, 1, &bitmapID1));
	EXCCHECKCVI( GetCtrlBitmap (panel, PNLSTEPS_PICTURES, 2, &bitmapID2));
	
	top = CTRL_STEP + (index*CTRL_STEP);
	id = NewCtrl (panel, CTRL_FLAT_BOX, "", top, 0);
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_WIDTH, CTRL_WIDTH));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_HEIGHT, CTRL_HIGHT));
	gs_StepData[index].pnode = pstep;
	gs_StepData[index].controlID1 = id;

	id = NewCtrl (panel, CTRL_PICTURE_COMMAND_BUTTON_LS, "", top + 2, 210);
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_WIDTH, 40));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_HEIGHT, 40));
	EXCCHECKCVI( SetCtrlBitmap (panel, id, 0, bitmapID0));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CMD_BUTTON_COLOR, VAL_TRANSPARENT));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CALLBACK_FUNCTION_POINTER, OnDeleteTaskStep));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CALLBACK_DATA, (void*)(pstep)));
	gs_StepData[index].controlID2 = id; 
	
	id = NewCtrl (panel, CTRL_PICTURE_COMMAND_BUTTON_LS, "", top + 2, 178);
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_WIDTH, 40));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_HEIGHT, 40));
	EXCCHECKCVI( SetCtrlBitmap (panel, id, 0, bitmapID2));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CMD_BUTTON_COLOR, VAL_TRANSPARENT));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CALLBACK_FUNCTION_POINTER, OnSelectTaskStep));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CALLBACK_DATA, (void*)(index+1)));
	gs_StepData[index].controlID3 = id; 
	
	id = NewCtrl (panel, CTRL_PICTURE_COMMAND_BUTTON_LS, "", top + 2, 145);
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_WIDTH, 40));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_HEIGHT, 40));
	EXCCHECKCVI( SetCtrlBitmap (panel, id, 0, bitmapID1));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CMD_BUTTON_COLOR, VAL_TRANSPARENT));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CALLBACK_FUNCTION_POINTER, OnEditTaskStep));
	EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_CALLBACK_DATA, (void*)(index+1)));
	gs_StepData[index].controlID4 = id; 

	XML_NODE_GET_TEXT( pstep, "type",
	{
		id = NewCtrl (panel, CTRL_TEXT_MSG, "", top + 5, 10);
		EXCCHECKCVI( SetCtrlVal (panel, id, ptext));
		EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT));
		EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_TEXT_BOLD, VAL_TRANSPARENT));
		gs_StepData[index].controlID5 = id;	 
		strcpy(gs_StepData[index].type, ptext);
	});

	XML_NODE_GET_TEXT( pstep, "@name",
	{
		id = NewCtrl (panel, CTRL_TEXT_MSG, "", top + 24, 10);
		sprintf( message, "Name: %s", ptext);
		EXCCHECKCVI( SetCtrlVal (panel, id, message));
		EXCCHECKCVI( SetCtrlAttribute (panel, id, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT)); 
		gs_StepData[index].controlID6 = id;	
	});
	
	/* result text box */
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_BKTREE, ATTR_LEFT, 0)); 
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_BKTREE, ATTR_TOP, top + CTRL_STEP));
	
Error:
	gs_StepDataSize = index+1;
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "DeleteStepDisplay"
static SElExceptionPtr DeleteStepDisplay( int panel)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	int32_t			i;
	
	for(i=0; i<gs_StepDataSize; i++)
	{
		EXCCHECKCVI( DiscardCtrl (gs_panel[PNLSTEPS], gs_StepData[i].controlID1));
		EXCCHECKCVI( DiscardCtrl (gs_panel[PNLSTEPS], gs_StepData[i].controlID2));
		EXCCHECKCVI( DiscardCtrl (gs_panel[PNLSTEPS], gs_StepData[i].controlID3));
		EXCCHECKCVI( DiscardCtrl (gs_panel[PNLSTEPS], gs_StepData[i].controlID4));
		EXCCHECKCVI( DiscardCtrl (gs_panel[PNLSTEPS], gs_StepData[i].controlID5));
		EXCCHECKCVI( DiscardCtrl (gs_panel[PNLSTEPS], gs_StepData[i].controlID6));
	}
	gs_StepDataSize = 0;

Error:
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnFirstImage"
int CVICALLBACK OnFirstImage (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;       
	int32_t			error = 0;
	HRESULT			hr;
	SGlobalPtr		pglobal = GetGlobal(NULL);  
	short			AXvalue;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECK( gs_pIMAQ->MakeImageCopy( gs_pIMAQ, BEGIN_STEP_IMAGE_NAME, INTERNAL_IMAGE_NAME));
			
			IMAQ_DISPLAY_IMAGE_BEGIN();
			
			/* select actual step */ 
			if(gs_StepDataActual!=0)
				EXCCHECKCVI( SetCtrlAttribute (panel, gs_StepData[gs_StepDataActual-1].controlID1, ATTR_FRAME_COLOR, 0x00C0C0C0));
			
			EXCCHECKCVI( SetCtrlAttribute (panel, PNLSTEPS_DECORATION_STEP0, ATTR_FRAME_COLOR, 0x00808080));
			
			gs_StepDataActual = 0;

			EXCCHECKCOM( BKLib2__DBKTreeFromString( pglobal->bktree, 
											NULL, 
											BKTREE_HEADER
											"|root|Task^i0\n", 
											"$", "\n",
											&AXvalue));
			break;
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "HighlightActiveStep"
static SElExceptionPtr HighlightActiveStep(int32_t step_number)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int				i;
	
	if( step_number == 0 )
	{
		for(i=0; i<gs_StepDataSize; i++)
		{
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], gs_StepData[i].controlID1, ATTR_FRAME_COLOR, 0x00C0C0C0));
		}
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_DECORATION_STEP0, ATTR_FRAME_COLOR, 0x00808080));
		
		gs_StepDataActual = step_number;
	}
	if( step_number > 0 )
	{
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], PNLSTEPS_DECORATION_STEP0, ATTR_FRAME_COLOR, 0x00C0C0C0));
		for(i=0; i<gs_StepDataSize; i++)
		{
			EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], gs_StepData[i].controlID1, ATTR_FRAME_COLOR, 0x00C0C0C0));
		}
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[PNLSTEPS], gs_StepData[step_number-1].controlID1, ATTR_FRAME_COLOR, 0x00808080));
		
		gs_StepDataActual = step_number;
	}
	
Error:
	EXCRETHROW( pexception); 
}
