#include <bklib/BKLib2.h>
#include <dbs/dbs.h>
#include <elstring.h>
#include <stdio.h>
#include <msxmldom.h>
#include "dbs_global.h"
#include "dbs_process.h"
#include "ring_control.h"
#include "resource.h"

#define	_STEP_NEW_WITH_PARAM	1 /* automatically generate parameters with a new step */

#define CONFIG_STEP_RING_XSL	"SeqSteps2Ring"		/* resource name */
#define CONFIG_STEP_STRING_XSL	"SeqSteps2String"	/* resource name */
#define CONFIG_IO_RING_XSL		"IO2Ring"			/* resource name */

extern SGlobalPtr				global;
extern int						gs_panel[];

#define BKTREE_HEADER0			"root|Process/Step/Parameter^w250¦edit|E^w0¦value|Value^w550¦path|Path^w200¦info|I^w200\n"
#define BKTREE_HEADER1			"|0|Processes^i0^r1\n"

#define BKTREE_MENU_ROOT		_("New Process|<Product View>")
#define BKTREE_MENU_PROCESS		_("New Process Before|New Process After|Delete|New Step")
#define BKTREE_MENU_STEP		_("New Step Before|New Step After|Edit Step|Skip Step|Remove Skip|Delete Step|New Parameter")
#define BKTREE_MENU_PARAMETER	_("New Parameter|Delete")

/* menu depth */
typedef enum _EBkTreeLevel
{
	BKTREE_LEVEL_ROOT = -1,
	BKTREE_LEVEL_NONE,
	BKTREE_LEVEL_PROCESS,
	BKTREE_LEVEL_STEP,
	BKTREE_LEVEL_PARAMETER
} EBkTreeLevel;

/* root menu */
typedef enum _EBkTreeMenuRoot
{
	BKTREE_MENU_ROOT_NEW_PROCESS = 0,
	BKTREE_MENU_ROOT_PRODUCT_VIEW
} _EBkTreeMenuProcess;

/* process menu */
typedef enum _EBkTreeMenuProces
{
	BKTREE_MENU_PROCESS_NEW_BEFORE = 0,
	BKTREE_MENU_PROCESS_NEW_AFTER,
	BKTREE_MENU_PROCESS_DEL,
	BKTREE_MENU_PROCESS_NEW_STEP
} _EBkTreeMenuProces;

/* step menu */
typedef enum _EBkTreeMenuStep
{
	BKTREE_MENU_STEP_NEW_BEFORE = 0,
	BKTREE_MENU_STEP_NEW_AFTER,
	BKTREE_MENU_STEP_EDIT,
	BKTREE_MENU_STEP_SKIP,
	BKTREE_MENU_STEP_REMOVE_SKIP,
	BKTREE_MENU_STEP_DEL,
	BKTREE_MENU_STEP_NEW_PARAM
} _EBkTreeMenuStep;

/* parameter menu */
typedef enum _EBkTreeMenuParam
{
	BKTREE_MENU_PARAM_NEW = 0,
	BKTREE_MENU_PARAM_DEL
} _EBkTreeMenuParam;

/* editable items parametrs */
#define TREE_EDIT_NO	0
#define TREE_EDIT_DEL	(1 << 0)
#define TREE_EDIT_SKIP	(1 << 1)

extern SElExceptionPtr ChangeMode (int32_t mode); 
static SElExceptionPtr ProcessTreeString(SELStringPtr pelstring);
static SElExceptionPtr ProcessTreeInfoString(int32_t process_nb, char* info, int32_t infoSize);
static SElExceptionPtr ProcessTreeSkip(int32_t process_nb, int32_t* skip);
static SElExceptionPtr StepTreeInfoString(int32_t step_nb, char* info, int32_t infoSize);
static SElExceptionPtr StepTreeSkip(int32_t step_nb, int32_t* skip);
static SElExceptionPtr ParamTreeInfoString(int32_t step_nb, char* name, char* info, int32_t infoSize);
static SElExceptionPtr ParamTreeVisible(int32_t fnc_nb, char* name, int32_t product_id,	int32_t* visible);
static SElExceptionPtr GetStringFromXMLviaXSL(const char *XMLFileName, const char *ResName, char **string);
static SElExceptionPtr ImportProcces(const char* fileName);

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "{Line Editor Plugin}::OnOpenXmlFile"
int  CVICALLBACK OnOpenXmlFile(int panel, int control, int event, 
   void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception =  NULL;  
	int32_t      	error = 0;
	char			file_name[256] = "";

	switch (event)
	{
		case EVENT_COMMIT:
			if(global->product_id==0)
			{
				EXCTHROW(-1, _("Select valid product for data import!"));
			}

			if(FileSelectPopup ("", "*.xml", "", _("Select Line Sequence File"), VAL_LOAD_BUTTON, 0, 1, 1, 0, file_name))
			{
				EXCCHECKCVI( SetCtrlVal(gs_panel[DBS], DBS_XML_FILE, file_name));
				EXCCHECK( ImportProcces(file_name));
				EXCCHECK( TreeProcessIni());
			}
			break;
	}
   
Error:  
	EXCDISPLAY( pexception);
   	return 0;   
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "{Line Editor Plugin}::OnExportXmlFile"
int  CVICALLBACK OnExportXmlFile(int panel, int control, int event, 
   void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception =  NULL;  
	int32_t      	error = 0;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECK( PLINEEXPORT->GetProcessSeqXML(PLINEEXPORT, global->product_id, NULL));
			MessageBox(hWnd, _("Export file was created c:\\line_seq.xml"), _("Export"), 0);
			break;
	}
   
Error:  
	EXCDISPLAY( pexception);
   	return 0;   
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "BkTreeProcessNodeClick"
SElExceptionPtr BkTreeProcessNodeClick (char *keyNode)
{
	SElExceptionPtr pexception = NULL; 
	HRESULT			hr;
	short int		AXvalue;
	char*			pedit = NULL;
	int32_t 		editable = 0;
	int32_t			level, a, b, c;

	level = sscanf(keyNode, "0_%d_%d_%d", &a, &b, &c);

	if(level==BKTREE_LEVEL_ROOT)
	{
		EXCCHECKCOM( BKLib2__DBKTreeSetPopupMenu(global->treeHandle, NULL, BKTREE_MENU_ROOT, &AXvalue));
	}
	else
	{
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText (global->treeHandle, NULL, keyNode, "edit", &pedit));
		sscanf(pedit, "%d", &editable);
		if(editable)
		{
			if(level==BKTREE_LEVEL_PROCESS)
				EXCCHECKCOM( BKLib2__DBKTreeSetPopupMenu(global->treeHandle, NULL, BKTREE_MENU_PROCESS, &AXvalue));
			else if(level==BKTREE_LEVEL_STEP)
				EXCCHECKCOM( BKLib2__DBKTreeSetPopupMenu(global->treeHandle, NULL, BKTREE_MENU_STEP, &AXvalue));
			else if(level==BKTREE_LEVEL_PARAMETER)
				EXCCHECKCOM( BKLib2__DBKTreeSetPopupMenu(global->treeHandle, NULL, BKTREE_MENU_PARAMETER, &AXvalue));

		}
		else
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetPopupMenu(global->treeHandle, NULL, NULL, &AXvalue));
		}
	}

Error:
	CA_FreeMemory(pedit); 
	EXCRETHROW( pexception);       
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "BkTreeProcessPopupMenuCommand"
SElExceptionPtr BkTreeProcessPopupMenuCommand (char *keyNode, char *keyCol, long menuItem)
{
	SElExceptionPtr pexception = NULL; 
	int32_t			level, a=0, b=0, c=0, index;
	HRESULT			hr = 0;

	EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, keyNode, "root", &index));

	level = sscanf(keyNode, "0_%d_%d_%d", &a, &b, &c);

	if(level==BKTREE_LEVEL_ROOT)
	{
		switch(menuItem)
		{
			case BKTREE_MENU_ROOT_NEW_PROCESS:
				{
					char nkeyNode[32];
					char firstNode[32];
					short exist;

					sprintf(nkeyNode, "0_1_new");
					EXCCHECKCOM( BKLib2__DBKTreeAddNode(global->treeHandle, NULL, "0", nkeyNode, "?", NULL));

					sprintf(firstNode, "0_1");
					EXCCHECKCOM( BKLib2__DBKTreeIsNodeExist(global->treeHandle, NULL, firstNode, &exist));
					if(exist)
						EXCCHECKCOM( BKLib2__DBKTreeMoveNode(global->treeHandle, NULL, nkeyNode, firstNode, TCTV_PREV, NULL));

					EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, nkeyNode));
				}
				break;
			case BKTREE_MENU_ROOT_PRODUCT_VIEW:
				EXCCHECK( ChangeMode(EDIT_PRODUCT));
				break;
		}
	}
	else if(level==BKTREE_LEVEL_PROCESS)
	{
		SLineProcess process = {0};

		switch(menuItem)
		{
			case BKTREE_MENU_PROCESS_NEW_BEFORE:
			case BKTREE_MENU_PROCESS_NEW_AFTER:
				{
					char nkeyNode[32];

					sprintf(nkeyNode, "0_%d_new", a + (menuItem==BKTREE_MENU_PROCESS_NEW_AFTER));
					EXCCHECKCOM( BKLib2__DBKTreeAddNode(global->treeHandle, NULL, "0", nkeyNode, "?", NULL));
					EXCCHECKCOM( BKLib2__DBKTreeMoveNode(global->treeHandle, NULL, nkeyNode, keyNode, 
												(menuItem==BKTREE_MENU_PROCESS_NEW_BEFORE)? TCTV_PREV:TCTV_NEXT, NULL));
					EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, nkeyNode));
				}
				break;
			case BKTREE_MENU_PROCESS_DEL:
				if(index>0)
				{
					process = PPROCESS->LineProcess[index-1];
				
					if(process.product_id!=global->product_id)
						EXCTHROW(-1, _("Delete process from a definition level of the product!"));

					process.user_id = global->activeUser.user_id;
					EXCCHECK( PPROCESS->LineProcessDel(PPROCESS, process));
				}
				EXCCHECK(TreeProcessIni());
				break;
			case BKTREE_MENU_PROCESS_NEW_STEP:
				{
					char nkeyNode[32];
					char firstNode[32];
					short exist;
					char *fn_ring = NULL;
					
					sprintf(nkeyNode, "0_%d_1_new", a);

					EXCCHECKCOM( BKLib2__DBKTreeAddNode(global->treeHandle, NULL, keyNode, nkeyNode, "?", NULL));
					EXCCHECK( GetStringFromXMLviaXSL (CONFIG_XML, CONFIG_STEP_RING_XSL, &fn_ring));
                    EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemTextEx (global->treeHandle, NULL, nkeyNode, "root", "?^r1", NULL));
                    EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemTextEx (global->treeHandle, NULL, nkeyNode, "value", fn_ring, NULL));
					CA_FreeMemory(fn_ring);

					sprintf(firstNode, "0_%d_1", a);
					EXCCHECKCOM( BKLib2__DBKTreeIsNodeExist(global->treeHandle, NULL, firstNode, &exist));
					if(exist)
						EXCCHECKCOM( BKLib2__DBKTreeMoveNode(global->treeHandle, NULL, nkeyNode, firstNode, TCTV_PREV, NULL));

					EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, nkeyNode));
				}
				break;
		}
	}
	else if(level==BKTREE_LEVEL_STEP)
	{
		SLineStep step = {0};

		switch(menuItem)
		{
			case BKTREE_MENU_STEP_NEW_BEFORE:
			case BKTREE_MENU_STEP_NEW_AFTER:
				{
					char nkeyParent[32];
					char nkeyNode[32];
					char *fn_ring = NULL;

					sprintf(nkeyParent, "0_%d", a);
					sprintf(nkeyNode, "0_%d_%d_new", a, b + (menuItem==BKTREE_MENU_STEP_NEW_AFTER));

					EXCCHECKCOM( BKLib2__DBKTreeAddNode(global->treeHandle, NULL, nkeyParent, nkeyNode, "?", NULL));
					EXCCHECK( GetStringFromXMLviaXSL (CONFIG_XML, CONFIG_STEP_RING_XSL, &fn_ring));
                    EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemTextEx (global->treeHandle, NULL, nkeyNode, "root", "?^r1", NULL));
                    EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemTextEx (global->treeHandle, NULL, nkeyNode, "value", fn_ring, NULL));
					CA_FreeMemory(fn_ring);

					EXCCHECKCOM( BKLib2__DBKTreeMoveNode(global->treeHandle, NULL, nkeyNode, keyNode, 
												(menuItem==BKTREE_MENU_STEP_NEW_BEFORE)? TCTV_PREV:TCTV_NEXT, NULL));
					EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, nkeyNode));
				}
				break;
			case BKTREE_MENU_STEP_EDIT:
				EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemEditable(global->treeHandle, NULL, keyNode, "root", 1));
				break;
			case BKTREE_MENU_STEP_SKIP:
				if(index>0)
				{
					step = PSTEP->LineStep[index-1];

					if(step.product_id>global->product_id)
						EXCTHROW(-1, _("Skip step from a lower level of the product!"));

					step.product_id = global->product_id;
					step.user_id = global->activeUser.user_id;
					EXCCHECK( PSTEP->LineStepSkip(PSTEP, step));
				}
				EXCCHECK(TreeProcessIni());
				break;
			case BKTREE_MENU_STEP_REMOVE_SKIP:
				if(index>0)
				{
					step = PSTEP->LineStep[index-1];

					if(step.product_id>global->product_id)
						EXCTHROW(-1, _("Remove skiped step from a lower level of the product!"));

					step.product_id = global->product_id;
					step.user_id = global->activeUser.user_id;
					EXCCHECK( PSTEP->LineStepSkipRemove(PSTEP, step));
				}
				EXCCHECK(TreeProcessIni());
				break;
			case BKTREE_MENU_STEP_DEL:
				if(index>0)
				{
					step = PSTEP->LineStep[index-1];
				
					if(step.product_id!=global->product_id)
						EXCTHROW(-1, _("Delete step from a definition level of the product!"));

					step.user_id = global->activeUser.user_id;
					EXCCHECK( PSTEP->LineStepDel(PSTEP, step));
				}
				EXCCHECK(TreeProcessIni());
				break;
			case BKTREE_MENU_STEP_NEW_PARAM:
				{
					char nkeyNode[32];
					short exist, i=1;

					do
					{
						sprintf(nkeyNode, "0_%d_%d_%d", a, b, c + i++);
						EXCCHECKCOM( BKLib2__DBKTreeIsNodeExist(global->treeHandle, NULL, nkeyNode, &exist));
					} while(exist);
					
					EXCCHECKCOM( BKLib2__DBKTreeAddNode(global->treeHandle, NULL, keyNode, nkeyNode, "?", NULL));
					EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, nkeyNode));
				}
				break;
		}
	}
	else if(level==BKTREE_LEVEL_PARAMETER)
	{
		SLineParam param = {0};
		
		switch(menuItem)
		{
			case BKTREE_MENU_PARAM_NEW:
				{
					char nkeyParent[32];
					char nkeyNode[32];
					short exist, i=1;

					sprintf(nkeyParent, "0_%d_%d", a, b);
					do
					{
						sprintf(nkeyNode, "0_%d_%d_%d", a, b, c + i++);
						EXCCHECKCOM( BKLib2__DBKTreeIsNodeExist(global->treeHandle, NULL, nkeyNode, &exist));
					} while(exist);
					
					EXCCHECKCOM( BKLib2__DBKTreeAddNode(global->treeHandle, NULL, nkeyParent, nkeyNode, "?", NULL));
					EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, nkeyNode));
				}
				break;
			case BKTREE_MENU_PARAM_DEL:
				if(index>0)
				{
					param = PLINEPARAM->LineParam[index-1];
				
					if(param.product_id!=global->product_id)
						EXCTHROW(-1, _("Delete parameter from a definition level of the product!"));

					param.user_id = global->activeUser.user_id;
					EXCCHECK( PLINEPARAM->LineParamDel(PLINEPARAM, param));
				}
				EXCCHECK(TreeProcessIni());
				break;
		}
	}
	
Error:
	EXCRETHROW( pexception);       
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "BkTreeProcessModify"
SElExceptionPtr BkTreeProcessModify (char *keyNode, char *keyCol, long  modifyType)
{
	SElExceptionPtr pexception = NULL; 
	int32_t			level, a=0, b=0, c=0, index;
	HRESULT			hr = 0;
	char*			ptext = NULL;

	EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, keyNode, "root", &index));
	EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(global->treeHandle, NULL, keyNode, keyCol, &ptext));

	level = sscanf(keyNode, "0_%d_%d_%d", &a, &b, &c);

	if(level==BKTREE_LEVEL_PROCESS)
	{
		SLineProcess process = {0};

		if(index>0)
		{
			process = PPROCESS->LineProcess[index-1];
			
			if(ptext && *ptext)
			{
				if(0==strcmp(keyCol, "value"))
				{
					strcpy(process.description, ptext);
					process.user_id = global->activeUser.user_id;
					EXCCHECK( PPROCESS->LineProcessEdit(PPROCESS, process));
				}
			}
			else if(0==strcmp(keyCol, "value")) /* empty field, return old value */
			{
				EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemText(global->treeHandle, NULL, keyNode, keyCol, process.description, NULL));
			}
		}
		else /* new process */
		{
			char *pname = NULL, *nextNode = NULL, *prevNode = NULL;
			bool_t last_node = FALSE;

			EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(global->treeHandle, NULL, keyNode, "root", &pname));

			/* determine actual position - process_nb */
			EXCCHECKCOM( BKLib2__DBKTreeGetNodeNext(global->treeHandle, NULL, keyNode, TCTV_NEXT, &nextNode));
			EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, nextNode, "root", &index));

			if(index<0) /* last node */
			{
				EXCCHECKCOM( BKLib2__DBKTreeGetNodeNext(global->treeHandle, NULL, keyNode, TCTV_PREV, &prevNode));
				EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, prevNode, "root", &index));
				last_node = TRUE;
			}

			strcpy(process.name, pname);
			process.product_id = global->product_id;
			process.user_id = global->activeUser.user_id;
			
			if(index>0)
			{
				process.process_nb = PPROCESS->LineProcess[index-1].process_nb + (last_node);
				EXCCHECK( PPROCESS->LineProcessInsert(PPROCESS, process));
			}
			else if(index==-1)
			{
				process.process_nb = PPROCESS->LineProcessSize;
				EXCCHECK( PPROCESS->LineProcessInsert(PPROCESS, process));
			}

			EXCCHECK( TreeProcessIni());
		}
	}
	else if(level==BKTREE_LEVEL_STEP)
	{
		SLineStep step = {0};

		if(index>0)
		{
			step = PSTEP->LineStep[index-1];

			if(ptext && *ptext)
			{
				if(0==strcmp(keyCol, "root") && 0==strcmp(ptext, "?"))
				{
					char *fn_ring = NULL;

					/* fnc rng */
					EXCCHECK( GetStringFromXMLviaXSL (CONFIG_XML, CONFIG_STEP_RING_XSL, &fn_ring));
					EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemTextEx (global->treeHandle, NULL, keyNode, "value", fn_ring, NULL));
					EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemEditable(global->treeHandle, NULL, keyNode, "value", 1));
					
					CA_FreeMemory(fn_ring);
					fn_ring = NULL;
				}
				else
				{
					strcpy(step.name, ptext);
					EXCCHECK( PSTEP->LineStepEdit(PSTEP, step));
					EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemText(global->treeHandle, NULL, keyNode, "root", ptext, NULL));
					EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemEditable(global->treeHandle, NULL, keyNode, "root", 0));

					EXCCHECK( TreeProcessIni());
				}
			}
			else if(0==strcmp(keyCol, "root")) /* if an empty field, return old value */
			{
				EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemText(global->treeHandle, NULL, keyNode, keyCol, step.name, NULL));
			}
		}
		else /* new step */
		{
			SLineProcess process = {0};
			char *parentNode = NULL, *pname = NULL, *nextNode = NULL, *prevNode = NULL, *fn = NULL, *fn_string = NULL;
			bool_t last_node = FALSE;

			EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(global->treeHandle, NULL, keyNode, &parentNode));
			EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, parentNode, "root", &index));
			if(index>0)
			{
				process = PPROCESS->LineProcess[index-1];
				
				EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(global->treeHandle, NULL, keyNode, "root", &pname));
	
				/* determine actual position - fnc_nb */
				EXCCHECKCOM( BKLib2__DBKTreeGetNodeNext(global->treeHandle, NULL, keyNode, TCTV_NEXT, &nextNode));
				EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, nextNode, "root", &index));

				if(index<0) /* last node */
				{
					EXCCHECKCOM( BKLib2__DBKTreeGetNodeNext(global->treeHandle, NULL, keyNode, TCTV_PREV, &prevNode));
					EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, prevNode, "root", &index));
					last_node = TRUE;
				}
				
				/* selected from ring */
				EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(global->treeHandle, NULL, keyNode, "value", &fn));
				if(fn)
				{
					char *pdata = NULL;
					SLineParam param = {0};
					
					/* set step */
					strcpy(step.name, fn);
					step.product_id = global->product_id;
					step.process_nb = process.process_nb;
					step.fnc_nb = (index>0)? (PSTEP->LineStep[index-1].fnc_nb + (last_node)):PSTEP->LineStepSize;
					step.user_id = global->activeUser.user_id;
					EXCCHECK( PSTEP->LineStepInsert(PSTEP, step));
#if _STEP_NEW_WITH_PARAM
					/* insert parameters */
					EXCCHECK( GetStringFromXMLviaXSL (CONFIG_XML, CONFIG_STEP_STRING_XSL, &fn_string));
					
					/* set param */
					param.fnc_nb = step.fnc_nb;
					param.product_id = step.product_id;
					param.user_id = global->activeUser.user_id;
					strcpy(param.value, "?");

					/* insert parameters */
					pdata = strtok(fn_string, "\n");
					while(pdata)
					{
						if(0==strncmp(fn, pdata, strlen(fn)))
						{
							if((pdata = strstr(pdata, ";"))!=NULL)
							{	
								pdata = strtok(++pdata, ";");
								while(pdata)
								{
									if(strlen(pdata))
									{
										strcpy(param.name, pdata);		
										EXCCHECK( PLINEPARAM->LineParamInsert(PLINEPARAM, param));
									}
									pdata = strtok(NULL, ";");
								}
							}
							break;
						}
						pdata = strtok(NULL, "\n");
					}
#endif
					EXCCHECK( TreeProcessIni());

					CA_FreeMemory(fn_string);
				}
				CA_FreeMemory(pname);
				CA_FreeMemory(nextNode);
			}
			CA_FreeMemory(parentNode); 
		}
	}
	else if(level==BKTREE_LEVEL_PARAMETER)
	{
		SLineParam param = {0};

		if(index>0)
		{
			param = PLINEPARAM->LineParam[index-1];
			
			if(ptext && *ptext)
			{
				if(0==strcmp(keyCol, "value"))
				{
					if(0==strcmp(ptext, "?") && 0==strncmp(param.name, "rsrc", sizeof("rsrc")-1))
					{
						char *io_ring = NULL;

						/* create io ring */
						EXCDISPLAY( GetStringFromXMLviaXSL (global->pathIOxml, CONFIG_IO_RING_XSL, &io_ring));
						EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemTextEx(global->treeHandle, NULL, keyNode, keyCol, (io_ring)? io_ring:"?", NULL));

						CA_FreeMemory(io_ring);
						io_ring = NULL;
					}
					else
					{
						strcpy(param.value, ptext);
						param.product_id = global->product_id;
						param.user_id = global->activeUser.user_id;
						EXCCHECK( PLINEPARAM->LineParamEdit(PLINEPARAM, param));
						EXCCHECK( TreeProcessIni());
					}
				}
			}
			else if(0==strcmp(keyCol, "value")) /* empty field, return old value */
			{
				EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemText(global->treeHandle, NULL, keyNode, keyCol, param.value, NULL));
			}
		}
		else /* new parameter */
		{
			SLineStep step = {0};
			char *parentNode = NULL, *pname = NULL;

			EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(global->treeHandle, NULL, keyNode, &parentNode));
			EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, parentNode, "root", &index));
			if(index>0)
			{
				step = PSTEP->LineStep[index-1];
				
				EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(global->treeHandle, NULL, keyNode, "root", &pname));

				if(pname && *pname)
				{
					strcpy(param.name, pname);
					strcpy(param.value, "?");
					param.product_id = global->product_id;
					param.fnc_nb = step.fnc_nb;
					param.user_id = global->activeUser.user_id;
					EXCCHECK( PLINEPARAM->LineParamInsert(PLINEPARAM, param));
					EXCCHECK( TreeProcessIni());
				}
				CA_FreeMemory(pname); 
			}
			CA_FreeMemory(parentNode); 
		}
	}

Error:
	CA_FreeMemory(ptext); 
	EXCRETHROW( pexception);       
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "TreeProcessIni"
SElExceptionPtr TreeProcessIni(void)
{
	SElExceptionPtr pexception = NULL;   
	int32_t			error = 0;
	SELStringPtr	pelstring = global->pelstring;
	HRESULT			hr;
	short int		AXvalue, exist, isExpanded;
	int32_t			a=0, b=0, c=0, found, i=1, imglistHandle;
	char			*selectedNode = NULL;
	char			actNode[32];

	EXCCHECK( pelstring->Empty(pelstring));

	EXCCHECKCOM( BKLib2__DBKTreeSetRedraw(global->treeHandle, NULL, 0, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeGetSelectedItem(global->treeHandle, NULL, &selectedNode));
	EXCCHECKCOM( BKLib2__DBKTreeIsExpanded(global->treeHandle, NULL, selectedNode, &isExpanded));

	EXCCHECK( ProcessTreeString(pelstring));

	EXCCHECKCOM( BKLib2__DBKTreeFromString (global->treeHandle, NULL, pelstring->GetPtr(pelstring), "¦", "\n", &AXvalue));
	EXCCHECKCOM( BKLib2__DBKTreeSetEditable(global->treeHandle, NULL, TRUE));
	EXCCHECKCOM( BKLib2__DBKTreeSetEnableDragAndDrop (global->treeHandle, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetEnableEventModify (global->treeHandle, NULL, VTRUE));

	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[DBS], DBS_IMAGELIST1, &imglistHandle));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (global->treeHandle, NULL, imglistHandle));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (global->treeHandle, NULL, VFALSE));

	found = sscanf(selectedNode, "0_%d_%d_%d", &a, &b, &c);
	/* collapse all */
	do
	{
		sprintf(actNode, "0_%d", i++);
		EXCCHECKCOM( BKLib2__DBKTreeIsNodeExist(global->treeHandle, NULL, actNode, &exist));
		EXCCHECKCOM( BKLib2__DBKTreeCollapse (global->treeHandle, NULL, actNode, &AXvalue));
	} while(exist);

	/* select old node */
	if(found>=BKTREE_LEVEL_PROCESS)
	{
		sprintf(actNode, "0_%d", a);
		EXCCHECKCOM( BKLib2__DBKTreeIsNodeExist(global->treeHandle, NULL, actNode, &exist));
		if(exist)
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, actNode));
			if(found>BKTREE_LEVEL_PROCESS || (found==BKTREE_LEVEL_PROCESS && isExpanded))
				EXCCHECKCOM( BKLib2__DBKTreeExpand(global->treeHandle, NULL, actNode, &AXvalue));
		}
	}
	if(found>=BKTREE_LEVEL_STEP)
	{
		sprintf(actNode, "0_%d_%d", a, b);
		EXCCHECKCOM( BKLib2__DBKTreeIsNodeExist(global->treeHandle, NULL, actNode, &exist));
		if(exist)
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, actNode));
			if(found>BKTREE_LEVEL_STEP || (found==BKTREE_LEVEL_STEP && isExpanded))
				EXCCHECKCOM( BKLib2__DBKTreeExpand(global->treeHandle, NULL, actNode, &AXvalue));
		}
	}
	if(found>=BKTREE_LEVEL_PARAMETER)
	{
		sprintf(actNode, "0_%d_%d_%d", a, b, c);
		EXCCHECKCOM( BKLib2__DBKTreeIsNodeExist(global->treeHandle, NULL, actNode, &exist));
		if(exist)
			EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(global->treeHandle, NULL, actNode));
	}

	EXCCHECKCOM( BKLib2__DBKTreeSetRedraw(global->treeHandle, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeSetScrollBarVisible(global->treeHandle, NULL, 0, 1));
	EXCCHECKCOM( BKLib2__DBKTreeEnsureVisible(global->treeHandle, NULL, selectedNode));

	EXCCHECKCVI( SetCtrlAttribute(gs_panel[DBS], DBS_XML_BCK, ATTR_VISIBLE, TRUE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[DBS], DBS_XML_OPEN, ATTR_VISIBLE, TRUE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[DBS], DBS_XML_FILE, ATTR_VISIBLE, TRUE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[DBS], DBS_XML_EXPORT, ATTR_VISIBLE, TRUE));

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "TreeProcessClose"
SElExceptionPtr TreeProcessClose(void)
{
	SElExceptionPtr pexception = NULL;   
	int32_t			error = 0;

	EXCCHECKCVI( SetCtrlAttribute(gs_panel[DBS], DBS_XML_BCK, ATTR_VISIBLE, FALSE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[DBS], DBS_XML_OPEN, ATTR_VISIBLE, FALSE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[DBS], DBS_XML_FILE, ATTR_VISIBLE, FALSE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[DBS], DBS_XML_EXPORT, ATTR_VISIBLE, FALSE));

Error:
	EXCRETHROW( pexception);
}

#define TREE_STRUCTURE_SIZE		2048*10
#define TREE_INFO_SIZE			256
#define TREE_PATH_SIZE			256
#define TREE_ROW_SIZE 			2048 + TREE_INFO_SIZE + TREE_PATH_SIZE
/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ParamTreeString"
static SElExceptionPtr ParamTreeString(
	SELStringPtr	pelstring, 
	int32_t 		step_nb, 
	int32_t 		process_index, 
	int32_t			step_index,
	int32_t* 		ProductId, 
	int32_t 		ProductIdSize,
	char*			tool_name,
	char*			tool_desc
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL; 
	int				parameter_index = 1, i, j;
	int32_t			editable = 0;
	char			path[TREE_PATH_SIZE+1] = "";
	char			info[TREE_INFO_SIZE+1] = "";
	char			*row = calloc(TREE_ROW_SIZE+1, sizeof(char));
	bool_t			visible = 0;
	char			*io_ring = NULL;

	/* number of steps */ 
	for(i=0; i<PLINEPARAM->LineParamSize; i++)   			
	{
		if(PLINEPARAM->LineParam[i].fnc_nb==step_nb)
		{
			/* visible only specified steps */
			for(j=ProductIdSize-1; j>=0; j--)   			
			{
				if ( ProductId[j] == PLINEPARAM->LineParam[i].product_id 
					&& (PLINEPARAM->LineParam[i].property&PROPERTY_VALID)>0)
				{
					/* unvisible duplicate parameters, select actual one */
			   		EXCCHECK( ParamTreeVisible(PLINEPARAM->LineParam[i].fnc_nb, 
			   									PLINEPARAM->LineParam[i].name,
			   									PLINEPARAM->LineParam[i].product_id,
			   									&visible));

					/* unvisible duplicate parameters, select actual one */
					if(visible) 
					{
						/* editable: delete step; skip step*/
						if(ProductId[j]==global->product_id)
							editable |= TREE_EDIT_DEL;
						if(ProductId[j]<global->product_id)
							editable |= TREE_EDIT_SKIP;

						EXCCHECK( PPRODUCT->ProductGetIdPath(PPRODUCT, PLINEPARAM->LineParam[i].product_id,path, PRODUCT_PATH_LEVEL_3));
						EXCCHECK( ParamTreeInfoString(PLINEPARAM->LineParam[i].fnc_nb, PLINEPARAM->LineParam[i].name, info, TREE_INFO_SIZE));
						
						if(NULL!=strstr(PLINEPARAM->LineParam[i].name, "rsrc") 
							&& 0==strcmp(PLINEPARAM->LineParam[i].value, "?"))
						{
							EXCDBGSTR( GetStringFromXMLviaXSL (global->pathIOxml, CONFIG_IO_RING_XSL, &io_ring));
							if(io_ring && *io_ring)
							{
								row = (char*)realloc(row, (TREE_ROW_SIZE+strlen(io_ring)+1)*sizeof(char));
								EXCCHECKALLOC(row);
							}
						}

						if(0==strcmp(PLINEPARAM->LineParam[i].name, "name"))
							strncpy(tool_name, PLINEPARAM->LineParam[i].value, DBS_RECORD_LENGHT_DESCRIPTION);
						else if(0==strcmp(PLINEPARAM->LineParam[i].name, "desc"))
							strncpy(tool_desc, PLINEPARAM->LineParam[i].value, DBS_RECORD_LENGHT_DESCRIPTION);

						sprintf(row, "0_%d_%d|0_%d_%d_%d|%s^r1^i2^c0000%s^d%d¦%d¦%s^r%d¦%s^r1¦%s^r1\n", 
								process_index,
								step_index,
								process_index,
								step_index,
								parameter_index,
								PLINEPARAM->LineParam[i].name,
								(editable)? "FF":"00",
								(long)(i+1), 
								editable,
								(io_ring && *io_ring)? io_ring:PLINEPARAM->LineParam[i].value,
								!editable,
								path,
								info);	
					
						EXCCHECK( pelstring->Append(pelstring, row));
						
						CA_FreeMemory(io_ring);
						io_ring = NULL;
						
						parameter_index++;
						break;
					}
				}
			}
			editable = 0;
		}
	}

Error:
	free(row);
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "StepTreeString"
static SElExceptionPtr StepTreeString(
	SELStringPtr	pelstring, 
	int32_t 		process_nb, 
	int32_t 		process_index, 
	int32_t* 		ProductId, 
	int32_t 		ProductIdSize
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL; 
	int				step_index = 1, i, j;
	int32_t			editable = 0;
	int32_t			used_step = -1;
	char			path[TREE_PATH_SIZE+1] = "";
	char			info[TREE_INFO_SIZE+1] = "";
	char			row[TREE_ROW_SIZE+1] = "";
	int32_t			skip = 0;
	char			tool_name[DBS_RECORD_LENGHT_DESCRIPTION+1],
					tool_desc[DBS_RECORD_LENGHT_DESCRIPTION+1];

	/* number of steps */ 
	for(i=0; i<PSTEP->LineStepSize; i++)   			
	{
		if(PSTEP->LineStep[i].process_nb==process_nb)
		{
			/* visible only specified steps */
			for(j=ProductIdSize-1; j>=0; j--)   			
			{
				if(ProductId[j]==PSTEP->LineStep[i].product_id 
					&& (PSTEP->LineStep[i].property&PROPERTY_VALID)>0
					&& (used_step<0 || used_step!=PSTEP->LineStep[i].fnc_nb)) /* unvisible skiped tests */
				{
					/* editable: delete step; skip step*/
					if(ProductId[j]==global->product_id)
						editable |= TREE_EDIT_DEL;
					if(ProductId[j]<global->product_id)
						editable |= TREE_EDIT_SKIP;

					EXCCHECK( PPRODUCT->ProductGetIdPath(PPRODUCT, PSTEP->LineStep[i].product_id, path, PRODUCT_PATH_LEVEL_3));
					EXCCHECK( StepTreeInfoString(PSTEP->LineStep[i].fnc_nb, info, TREE_INFO_SIZE));
					EXCCHECK( StepTreeSkip(PSTEP->LineStep[i].fnc_nb, &skip));

					sprintf(row, "0_%d|0_%d_%d|%s%s^r1^i1^c0000%s^d%d¦%d¦#TOOL_NAME [#TOOL_DESC]^r1¦%s^r1¦%s^r1\n", 
							process_index,
							process_index,
							step_index,
							PSTEP->LineStep[i].name,
							(skip)? " [skip]":"",
							(editable)? "FF":"00",
							(long)(i+1), 
							editable,
							path,
							info);	
				
					EXCCHECK( pelstring->Append(pelstring, row));
					
					tool_name[0] = '\0';
					tool_desc[0] = '\0';

					if(editable)
					{
						/* add parameters */ 
						EXCCHECK( ParamTreeString(pelstring,
												  PSTEP->LineStep[i].fnc_nb, 
												  process_index, 
												  step_index,
												  ProductId, 
												  ProductIdSize,
												  tool_name,
												  tool_desc)); 
					}
				
					EXCCHECK( pelstring->Replace(pelstring, "#TOOL_NAME", tool_name));
					EXCCHECK( pelstring->Replace(pelstring, "#TOOL_DESC", tool_desc));

					step_index++;
					used_step = PSTEP->LineStep[i].fnc_nb;
					break;
				}
			}
			editable = 0;
		}
		else if(PSTEP->LineStep[i].process_nb>process_nb)
		{
			break;
		}
	}

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ProcessTreeString"
static SElExceptionPtr ProcessTreeString(
	SELStringPtr pelstring
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL; 
	int				process_index = 1, i, j;
	int32_t			ProductId[TREE_STRUCTURE_SIZE];
	int32_t			ProductIdSize = TREE_STRUCTURE_SIZE;
	int32_t			editable = 0;
	int32_t			used_process = -1;
	char			path[TREE_PATH_SIZE+1] = "";
	char			info[TREE_INFO_SIZE+1] = "";
	char			row[TREE_ROW_SIZE+1] = "";
	int32_t			skip = 0;
	
	EXCCHECK( pelstring->Append(pelstring, BKTREE_HEADER0));
	EXCCHECK( pelstring->Append(pelstring, BKTREE_HEADER1));
	
	if(global->product_id!=0)
	{
		/* select specified product tree branch */
		EXCCHECK( PPRODUCT->ProductGetIdTreeUpDown(
									PPRODUCT,
									global->product_id, 	
									ProductId, 
									&ProductIdSize));
	}
	else
	{
		ProductIdSize = 0;
	}

	/* number of steps */ 
	for(i=0; i<PPROCESS->LineProcessSize; i++)   			
	{
		/* visible only specified tests */
		for(j=ProductIdSize-1; j>=0; j--)   			
		{
			if(ProductId[j] == PPROCESS->LineProcess[i].product_id 
				&& (PPROCESS->LineProcess[i].property&PROPERTY_VALID)>0
				&& (used_process<0 || used_process!=PPROCESS->LineProcess[i].process_nb)) /* unvisible skiped tests */
			{
				/* editable: delete test; skip test*/
				if(ProductId[j]==global->product_id)
					editable |= TREE_EDIT_DEL;
				if(ProductId[j]<global->product_id)
					editable |= TREE_EDIT_SKIP;

				EXCCHECK( PPRODUCT->ProductGetIdPath(PPRODUCT, PPROCESS->LineProcess[i].product_id,path, PRODUCT_PATH_LEVEL_2));		
				EXCCHECK( ProcessTreeInfoString(PPROCESS->LineProcess[i].process_nb, info, TREE_INFO_SIZE) );
				EXCCHECK( ProcessTreeSkip(PPROCESS->LineProcess[i].process_nb, &skip) );
				
				sprintf(row, "0|0_%d|%s%s^r1^i0^c0000%s^d%d¦%d¦%s^r0¦%s^r1¦%s^r1\n",   
						process_index, 
						PPROCESS->LineProcess[i].name,
						(skip)? " [skip]":"",
						(editable)? "FF":"00",
						(long)(i+1), 
						editable,					
						PPROCESS->LineProcess[i].description,   
						path,
						info);	
			
				EXCCHECK( pelstring->Append(pelstring, row));
				
				if(editable)
				{
					/* add steps */ 
					EXCCHECK( StepTreeString(pelstring, 
											  PPROCESS->LineProcess[i].process_nb, 
											  process_index, 
											  ProductId, 
											  ProductIdSize)); 
				}
			
				process_index++;
				used_process = PPROCESS->LineProcess[i].process_nb;
				break;
			}
		}
		editable = 0;
	}

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/   
#undef __FUNC__
#define __FUNC__ "ProcessTreeInfoString"
static SElExceptionPtr ProcessTreeInfoString(int32_t process_nb, char* info, int32_t infoSize)
{
	SElExceptionPtr pexception = NULL; 
	int32_t			error = 0;
	int				i;
	SLineProcessPtr		pprocess = PPROCESS->LineProcess;
	SProductTypesPtr	pproduct = PPRODUCT->Product;
	int32_t			products_id[128];
	int32_t			products_idSize = 0;
	char			path[128]="";
	
	strcpy(info, "");
	
	/* search tests witch equal test_nb, output product_id*/
	for(i=0; i<PPROCESS->LineProcessSize; i++) 
	{
		if(pprocess[i].process_nb == process_nb 
			&& (pprocess[i].property&PROPERTY_VALID)>0 
			&& 0==strcmp(pprocess[i].name, "@skip"))
		{
			products_id[products_idSize++] = pprocess[i].product_id;		
		}
		
		if(pprocess[i].process_nb > process_nb)
			break;
	}
	
	/* determine product_id name and print to info */
	if(products_idSize>0)
	{
		strcat(info, "Skip: "); 
		
		for(i=0; i<products_idSize; i++)
		{
			EXCCHECK( PPRODUCT->ProductGetIdPath(PPRODUCT, products_id[i], path, PRODUCT_PATH_LEVEL_4));
			
			if( (int)(strlen(info) + strlen(path) + strlen("; ...^i3"))>=infoSize)
			{	
				strcat(info, "...");
				break;
			}
			strcat(info, path);
			strcat(info, "; ");
		}
		strcat(info, "^i3");  /* insert picture index */          
	}

Error:
	EXCRETHROW( pexception);     
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "ProcessTreeSkip"
static SElExceptionPtr ProcessTreeSkip(int32_t process_nb, int32_t* skip)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	int				i, j; 
	SLineProcessPtr		pprocess = PPROCESS->LineProcess;
	SProductTypesPtr	pproduct = PPRODUCT->Product;
	int32_t			products_id[128];
	int32_t			products_idSize = 0;
	int32_t			path[DBS_NUMBER_OF_GROUPS+1];
	int32_t			pathSize = DBS_NUMBER_OF_GROUPS+1;
	
	*skip = 0;
	
	/* determine product id path */
	EXCCHECK( PPRODUCT->ProductGetIdTreeUp(PPRODUCT, global->product_id, path, &pathSize));    
	
	/* search tests witch equal test_nb, output product_id*/
	for(i=0; i<PPROCESS->LineProcessSize; i++) 
	{
		if(pprocess[i].process_nb == process_nb 
			&& (pprocess[i].property&PROPERTY_VALID)>0
			&& 0==strcmp(pprocess[i].name, "@skip"))
		{
			products_id[products_idSize++] = pprocess[i].product_id;		
		}
		
		if(pprocess[i].process_nb > process_nb)
			break;
	}
	
	/* is test skip? */
	for(i=0; i<products_idSize; i++)
	{
		for(j=0; j<pathSize; j++)
		{
			if(path[j] == products_id[i])
			{
				*skip = 1;
				break;
			}
		}
		
		if(*skip)
			break;
	}
	
Error:
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/   
#undef __FUNC__
#define __FUNC__ "StepTreeInfoString"
static SElExceptionPtr StepTreeInfoString(int32_t step_nb, char* info, int32_t infoSize)
{
	SElExceptionPtr pexception = NULL; 
	int32_t			error = 0;
	int				i;
	SLineStepPtr		pstep = PSTEP->LineStep;
	SProductTypesPtr	pproduct = PPRODUCT->Product;
	int32_t			products_id[128];
	int32_t			products_idSize = 0;
	char			path[128]="";
	
	strcpy(info, "");
	
	/* search tests witch equal test_nb, output product_id*/
	for(i=0; i<PSTEP->LineStepSize; i++) 
	{
		if(pstep[i].fnc_nb == step_nb 
			&& (pstep[i].property&PROPERTY_VALID)>0 
			&& 0==strcmp(pstep[i].name, "@skip"))
		{
			products_id[products_idSize++] = pstep[i].product_id;		
		}
	}
	
	/* determine product_id name and print to info */
	if(products_idSize>0)
	{
		strcat(info, "Skip: "); 
		
		for(i=0; i<products_idSize; i++)
		{
			EXCCHECK( PPRODUCT->ProductGetIdPath(PPRODUCT, products_id[i], path, PRODUCT_PATH_LEVEL_4));
			
			if( (int)(strlen(info) + strlen(path) + strlen("; ...^i3"))>=infoSize)
			{	
				strcat(info, "...");
				break;
			}
			strcat(info, path);
			strcat(info, "; ");
		}
		strcat(info, "^i3");  /* insert picture index */          
	}

Error:
	EXCRETHROW( pexception);     
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "StepTreeSkip"
static SElExceptionPtr StepTreeSkip(int32_t step_nb, int32_t* skip)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	int				i, j; 
	SLineStepPtr		pstep = PSTEP->LineStep;
	SProductTypesPtr	pproduct = PPRODUCT->Product;
	int32_t			products_id[128];
	int32_t			products_idSize = 0;
	int32_t			path[DBS_NUMBER_OF_GROUPS+1];
	int32_t			pathSize = DBS_NUMBER_OF_GROUPS+1;
	
	*skip = 0;
	
	/* determine product id path */
	EXCCHECK( PPRODUCT->ProductGetIdTreeUp(PPRODUCT, global->product_id, path, &pathSize));    
	
	/* search tests witch equal test_nb, output product_id*/
	for(i=0; i<PSTEP->LineStepSize; i++) 
	{
		if(pstep[i].fnc_nb == step_nb 
			&& (pstep[i].property&PROPERTY_VALID)>0
			&& 0==strcmp(pstep[i].name, "@skip"))
		{
			products_id[products_idSize++] = pstep[i].product_id;		
		}
	}
	
	/* is test skip? */
	for(i=0; i<products_idSize; i++)
	{
		for(j=0; j<pathSize; j++)
		{
			if(path[j] == products_id[i])
			{
				*skip = 1;
				break;
			}
		}
		
		if(*skip)
			break;
	}
	
Error:
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ParamTreeInfoString"
static SElExceptionPtr ParamTreeInfoString(int32_t step_nb, char* name, char* info, int32_t infoSize)
{
	SElExceptionPtr pexception = NULL; 
	int32_t			error = 0;
	int				i;
	SLineParamPtr		plineparameter = PLINEPARAM->LineParam;
	SProductTypesPtr	pproduct = PPRODUCT->Product;
	int32_t			products_id[128];
	int32_t			products_idSize = 0;
	char			path[128]="";
	char*			pvalue[128];
	
	strcpy(info, "");
	
	/* search parameters witch equal test_nb, output product_id*/
	for(i=0; i<PLINEPARAM->LineParamSize; i++) 
	{
		if(plineparameter[i].fnc_nb == step_nb 
			&& (plineparameter[i].property&PROPERTY_VALID)>0 
			&& 0==strcmp(plineparameter[i].name, name))
		{
			pvalue[products_idSize] = plineparameter[i].value;
			products_id[products_idSize++] = plineparameter[i].product_id;
		}
		
		if(plineparameter[i].fnc_nb > step_nb)
			break;
	}
	
	/* determine product_id name and print to info */
	if(products_idSize>1)
	{
		strcat(info, "Changed: "); 
		
		for(i=1; i<products_idSize; i++)
		{
			EXCCHECK( PPRODUCT->ProductGetIdPath(PPRODUCT, products_id[i], path, PRODUCT_PATH_LEVEL_4));
			
			if( (int)(strlen(info) + strlen(path) + strlen(pvalue[i]) + strlen(":; ...^i3"))>=infoSize)
			{	
				strcat(info, "...");
				break;
			}
			strcat(info, path);
			strcat(info, ":");
			strcat(info, pvalue[i]);  /* visible parametr value */
			strcat(info, "; ");
		}
		strcat(info, "^i3");   /* insert picture index */           
	}

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ParamTreeVisible"
static SElExceptionPtr ParamTreeVisible(
	int32_t fnc_nb, 
	char* name, 
	int32_t product_id,
	int32_t* visible
) 
{
	SElExceptionPtr pexception = NULL;
	int				i, j;
	SProductTypesPtr	pproduct = PPRODUCT->Product; 
	SLineParamPtr		plineparam	= PLINEPARAM->LineParam;
	int32_t			path[DBS_NUMBER_OF_GROUPS+1];
	int32_t			pathSize = DBS_NUMBER_OF_GROUPS+1;
	int32_t			products_id[128];
	int32_t			products_idSize = 0;
	int32_t			actual_id = 0;
	char*			pvalue[128];
	
	if(visible) *visible = 0;
	
	/* determine product id path */ 
	EXCCHECK( PPRODUCT->ProductGetIdTreeUp(PPRODUCT, global->product_id, path, &pathSize));

	/* search parameters witch equal test_nb, output product_id*/
	for(i=0; i<PLINEPARAM->LineParamSize; i++) 
	{
		if(plineparam[i].fnc_nb == fnc_nb 
			&& (plineparam[i].property&PROPERTY_VALID)>0 
			&& 0==strcmp(plineparam[i].name, name))
		{
			pvalue[products_idSize] = plineparam[i].value;
			products_id[products_idSize++] = plineparam[i].product_id;
		}
		
		if(plineparam[i].fnc_nb > fnc_nb)
			break;
	}
	
	/* select actual parametr */
	for(i=0; i<pathSize; i++)
	{
		for(j=0; j<products_idSize; j++)
		{
			if(products_id[j] == path[i])
			{
				actual_id = products_id[j];
				break;
			}
		}
		if(actual_id)
			break;
	}
	
	if(actual_id == product_id)
		if(visible) *visible = 1;
	
Error:
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "TranslateBKTreeBinCode"
static int TranslateBKTreeBinCode (char *buffer)
{
    // Conversion table for "non-printable" characters used in BKTree
    char    *BinaryCodesTbl[] = {"~#01~", "~#02~", "~#03~", "~#04~", "~#05~",
                                 "~#06~", "~#07~", "~#08~", "~#09~", "~#10~",
                                 "~#11~", "~#12~", "~#13~", 0};

    short   idx = 0;
    char    *p2code;

    // Search and replace BKTree binary character tags
    if (buffer)
    {
        for (idx = 0; BinaryCodesTbl[idx]; idx++)
            for (;p2code = strstr (buffer, BinaryCodesTbl[idx]);)
            {
                p2code[0] = idx + 1; // Set bin code
                memmove (p2code + 1, p2code + strlen(BinaryCodesTbl[idx]), strlen(p2code));
            }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetStringFromXMLviaXSL"
static SElExceptionPtr GetStringFromXMLviaXSL (const char *XMLFileName, const char *ResName, char **string)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
    HRESULT			hr = 0;
    char            *output_string;
    CAObjHandle     XSLDOMDoc = 0;
	CAObjHandle     XMLDOMDoc = 0;
	CAObjHandle		XSLDOMElement = 0;
	CAObjHandle		XMLDOMElement = 0;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	CAObjHandle		pnode = 0;
	VBOOL			is_xml_success;
	HRSRC			hresInfo;
	HANDLE			hres;
	char			*ptext = NULL;
	char			*idr = NULL;

	if(0==strcmp(ResName, CONFIG_STEP_STRING_XSL))
		idr = MAKEINTRESOURCE(IDR_SeqSteps2S1);
	else if(0==strcmp(ResName, CONFIG_STEP_RING_XSL))
		idr = MAKEINTRESOURCE(IDR_SeqSteps2R1);
	else if(0==strcmp(ResName, CONFIG_IO_RING_XSL))
		idr = MAKEINTRESOURCE(IDR_IO2Ring1);

	/* load resource */
	hresInfo = FindResource(global->hInstance, idr, ResName);
	if(hresInfo==NULL)
		EXCTHROW(-1, "FindResource error!");

	hres = LoadResource(global->hInstance, hresInfo);
	if(hres==NULL)
		EXCTHROW(-1, "LoadResource error!");
	
	ptext = (char*)LockResource(hres);

	/* xsl */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &XSLDOMDoc));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentloadXML (XSLDOMDoc, NULL, ptext, &is_xml_success));

	/* xml */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &XMLDOMDoc));
	EXCCHECKCVI( CA_VariantSetCString(&pfile_name_var, XMLFileName));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (XMLDOMDoc, NULL, pfile_name_var, &is_xml_success));
	CA_VariantClear(&pfile_name_var);

	if(XSLDOMDoc && XMLDOMDoc)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentGetdocumentElement(XSLDOMDoc, NULL, &XSLDOMElement));
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentGetdocumentElement(XMLDOMDoc, NULL, &XMLDOMElement));

		EXCCHECKMSXML( MSXML_IXMLDOMElementtransformNode(XMLDOMElement, NULL, XSLDOMElement, &output_string));

		// Pre-process transformed data
		TranslateBKTreeBinCode (output_string);

		*string = output_string; // Do not forget to destroy memory when not needed ... CA_FreeMemory ();
	}
	else
		EXCTHROW(-1, "Transformation XML doc error!");

Error:
	CA_DiscardObjHandle (XSLDOMElement);
	CA_DiscardObjHandle (XMLDOMElement);
	CA_DiscardObjHandle (XSLDOMDoc);
	CA_DiscardObjHandle (XMLDOMDoc);
	EXCRETHROW( pexception);    
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ImportProcces"
static SElExceptionPtr ImportProcces(const char* fileName)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pprocess_list = 0;
	CAObjHandle     pprocess = 0;
	CAObjHandle     pstep_list = 0;
	CAObjHandle     pstep = 0;
	CAObjHandle     pparam_list = 0;
	CAObjHandle     pparam = 0;
	CAObjHandle     pparameter = 0;   
	const char*     pfile_name = (fileName) ? fileName : "";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*			ptext = NULL;
	char*			ptext2 = NULL;
	char*			keyNode = NULL;
	int32_t			index = 0, found, a, b, c, pos = 0;

	EXCCHECKCOM( BKLib2__DBKTreeGetSelectedItem(global->treeHandle, NULL, &keyNode)); // 0_x_x_x
	found = sscanf(keyNode, "0_%d_%d_%d", &a, &b, &c);

	if(found>=BKTREE_LEVEL_PROCESS)
	{
		char pkeyNode[32];

		sprintf(pkeyNode, "0_%d", a);
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData (global->treeHandle, NULL, pkeyNode, "root", &index));
	}

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(&pfile_name_var, pfile_name));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes (pxml, NULL, "//assy_line/seq/*", &pprocess_list));
	if(pprocess_list != 0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset(pprocess_list, NULL));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pprocess_list, NULL, &pprocess));
		while(pprocess)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnodeName(pprocess, NULL, &ptext));

			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(pprocess, NULL, "@ws", &pparameter));
			if(pparameter)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pparameter, NULL, &ptext2));
				
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
			}

			if(ptext && 0!=strcmp(ptext, "step"))
			{
				/* process */
				SLineProcess process = {0};
				
				strcpy(process.name, ptext);
				strcpy(process.description, (ptext2)? ptext2:"");
				process.product_id = global->product_id;
				
				/* select process nb */
				if(index>0)
					process.process_nb = PPROCESS->LineProcess[index-1].process_nb + 1 + pos;
				else
					process.process_nb = pos;

				process.user_id = global->activeUser.user_id;
				EXCCHECK( PPROCESS->LineProcessInsert(PPROCESS, process));

				CA_FreeMemory(ptext);
				ptext = NULL;
			}

			if(ptext2)
			{	
				CA_FreeMemory(ptext2);
				ptext2 = NULL;
			}

			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (pprocess, NULL, "step", &pstep_list));
			if(pstep_list != 0)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset(pstep_list, NULL));
				EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pstep_list, NULL, &pstep));

				while(pstep)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(pstep, NULL, "@fn", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pparameter, NULL, &ptext));
						if(ptext)
						{	
							/* step fn */
							SLineStep step = {0};

							strcpy(step.name, ptext);
							step.product_id = global->product_id;
					
							/* select process nb */
							if(index>0)
								step.process_nb = PPROCESS->LineProcess[index-1].process_nb + 1 + pos;
							else
								step.process_nb = pos;

							step.fnc_nb = PSTEP->LineStepSize;
							step.user_id = global->activeUser.user_id;
							EXCCHECK( PSTEP->LineStepInsert(PSTEP, step));

							CA_FreeMemory(ptext);
							ptext = NULL;
						}
						CA_DiscardObjHandle(pparameter);
						pparameter = 0;
					}
					else
					{
						/* step */
						SLineStep step = {0};

						strcpy(step.name, "NOP");
						step.product_id = global->product_id;
	
						/* select process nb */
						if(index>0)
							step.process_nb = PPROCESS->LineProcess[index-1].process_nb + 1 + pos;
						else
							step.process_nb = pos;

						step.fnc_nb = PSTEP->LineStepSize;
						step.user_id = global->activeUser.user_id;
						EXCCHECK( PSTEP->LineStepInsert(PSTEP, step));
					}

					EXCCHECKMSXML( MSXML_IXMLDOMNodeGetattributes (pstep, NULL, &pparam_list));		
					if(pparam_list != 0)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNamedNodeMapreset(pparam_list, NULL));
						EXCCHECKMSXML( MSXML_IXMLDOMNamedNodeMapnextNode(pparam_list, NULL, &pparam));
						while(pparam)
						{
							EXCCHECKMSXML( MSXML_IXMLDOMNodeGetnodeName(pparam, NULL, &ptext));
							if(ptext && 0!=strcmp(ptext, "fn"))
							{
								EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(pparam, NULL, &ptext2));
								if(ptext2)
								{
									/* parameter */
									SLineParam param = {0};

									strcpy(param.name, ptext);
									strcpy(param.value, ptext2);
									param.product_id = global->product_id;
									param.fnc_nb = PSTEP->LineStepSize-1;
									param.user_id = global->activeUser.user_id;
									EXCCHECK( PLINEPARAM->LineParamInsert(PLINEPARAM, param));

									CA_FreeMemory(ptext2);
									ptext2 = NULL;
								}
								CA_FreeMemory(ptext);
								ptext = NULL;
							}

							CA_DiscardObjHandle(pparam); 
							pparam = 0;
							EXCCHECKMSXML( MSXML_IXMLDOMNamedNodeMapnextNode(pparam_list, NULL, &pparam));
						}
					}
					CA_DiscardObjHandle(pstep); 
					pstep = 0;
					EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pstep_list, NULL, &pstep));
				}
			}
			CA_DiscardObjHandle(pstep_list);
			pstep_list = 0;

			CA_DiscardObjHandle(pprocess);
			pprocess = 0;
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(pprocess_list, NULL, &pprocess));

			pos++;
		}
		CA_DiscardObjHandle(pprocess_list); 
		pprocess_list = 0;
	}

Error:
	CA_VariantClear(&pfile_name_var);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pprocess_list) CA_DiscardObjHandle(pprocess_list);
	if (pprocess) CA_DiscardObjHandle(pprocess);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}
