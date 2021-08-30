#include <msxmldom.h>
#include <utility.h>
#include <users/user.h>
#include <guid.h>
#ifndef _NO_MENU
	#include <zzip/elzip.h>
	#include "resource.h"	
#endif
#include <userint.h>
#include <stdio.h>
#include <language/language.h>
#include "panel.h" 
#include "panel_fnc.h"
#include "panel_plugin.h"

const char* HISTORY = \
"[2008-01-08] zR 1.21\n"
" * Removed toolbox from the project.\n"
"[2008-01-07] rT 1.20\n"
" * Function SetPanelTable was edited because of application freezing.\n"
"[2007-12-13] rT 1.10\n"
" * Functions OnPanel, OnMenu, OnSubmenu were edited because of algorithm speed.\n";


#define PANEL_ITEM_NUMBER		8
#define MENU_ITEM_NUMBER 		9
#define SUBMENU_ITEM_NUMBER 	9 
#define SHORTCUT_ITEM_NUMBER 	12        

/* panels positions */
static int32_t					gv_PanelTop = 70;
static int32_t					gv_PanelLeft = 9;
#define PANEL_TOP				gv_PanelTop
#define PANEL_LEFT				gv_PanelLeft

static int32_t					gv_MenuTop = 70 + 742;
static int32_t					gv_MenuLeft = 9 + 2;
static int32_t					gv_MenuWidth = 467;
static int32_t					gv_MenuHeight = 741;
static int32_t					gv_MenuItemHeight = 82;
#define MENU_TOP				gv_MenuTop
#define MENU_LEFT				gv_MenuLeft
#define MENU_WIDTH				gv_MenuWidth
#define MENU_HEIGHT				gv_MenuHeight
#define MENU_ITEM_HEIGHT		gv_MenuItemHeight

static int32_t					gv_SubMenuLeft = 9 + 2 + 467;
#define SUBMENU_LEFT			gv_SubMenuLeft
#define SUBMENU_ITEM_HEIGHT		60

/* tables offsets */
#define PANEL_CELL_X_OFFSET		80
#define PANEL_CELL_X_SHIFT		140

#define MENU_CELL_Y_OFFSET		40
#define MENU_CELL_Y_SHIFT		80

#define SUBMENU_CELL_Y_OFFSET	30
#define SUBMENU_CELL_Y_SHIFT	60
	
#define COLOR_TEXT  0x00347B 

#define NB_OF_PANEL 		4
static int					gs_panel[NB_OF_PANEL + 1];
static SElEventFactoryPtr	gs_EventFactory; 
static SObjListPtr          gs_Modules;
SLanguagePtr				gs_pLanguage = NULL; 
static Point				gs_lastMenuCell;
static Point				gs_lastSubMenuCell;
static Point				gs_lastPanelCell;
static SPanelItem 			SPanel[PANEL_ITEM_NUMBER]; 
static bool_t				gs_is_locked = FALSE;
HINSTANCE					g_hInstance = NULL;
#ifndef _NO_MENU
	SZipPtr					gs_pZip = NULL;
	static SPanelItem		SMenu[MENU_ITEM_NUMBER]; 
	static SPanelItem		SSubMenu[SUBMENU_ITEM_NUMBER];
	char					g_PanelIcons[512];
#endif
#ifndef _NO_SHORTCUT
#define SHORTCUT_HELP_SHIFT		126
#define SHORTCUT_HELP_OFFSET	102
	static int PanelHelp[] = {
		PANEL_MSG_P1,
		PANEL_MSG_P2,
		PANEL_MSG_P3,
		PANEL_MSG_P4,
		PANEL_MSG_P5,
		PANEL_MSG_P6,
		PANEL_MSG_P7,
		PANEL_MSG_P8};
	static SPanelItem		SShortcut[SHORTCUT_ITEM_NUMBER];
#endif
static SLastPositionPtr		gs_lastSavedPosition;

static SElExceptionPtr SetPanelTableHighlight(int position);
static SElExceptionPtr LoadShortcutConfiguration(const char* fileName);
#ifndef _NO_SHORTCUT
	static SElExceptionPtr FillRings(int panel);
	static SElExceptionPtr KeyPressed(const char* key);
#endif

CRITICAL_SECTION   _Lock;
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "fncLock"
static SElExceptionPtr fncLock(void)
{
	EnterCriticalSection(&_Lock);
	return NULL;
} 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "fncUnlock"
static SElExceptionPtr fncUnlock(void)
{
	LeaveCriticalSection(&_Lock);
	return NULL;
} 

#ifndef _NO_MENU
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayMenu"
static void CVICALLBACK PostDeferredDisplayMenu (void *callbackData) 
{ 
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( DisplayPanel(gs_panel[MENU])); 
	EXCCHECKCVI( SetActivePanel( gs_panel[MENU]));
	
Error:
	EXCDISPLAY( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredHideMenu"
static void CVICALLBACK PostDeferredHideMenu (void *callbackData) 
{ 
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECKCVI( HidePanel(gs_panel[SUBMENU])); 
	EXCCHECKCVI( HidePanel(gs_panel[MENU])); 
	
Error:
	EXCDISPLAY( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplaySubMenu"
static void CVICALLBACK PostDeferredDisplaySubMenu (void *callbackData) 
{ 
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				monitor_id, top, height, i;
	
	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP, &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[SUBMENU], 
									ATTR_TOP, 
									top + height - PANEL_TOP - ((MENU_ITEM_NUMBER - gs_lastMenuCell.y + 1)*MENU_ITEM_HEIGHT) - 5 ));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[SUBMENU], ATTR_LEFT, SUBMENU_LEFT));
	
	/* set submenu height */
	for(i=0; i<SUBMENU_ITEM_NUMBER; i++)
	{
		if(SSubMenu[i].valid==FALSE) 
			break;
	}
		
	EXCCHECKCVI( SetPanelAttribute (gs_panel[SUBMENU], ATTR_HEIGHT, i*SUBMENU_ITEM_HEIGHT + 3));
	EXCCHECKCVI( DisplayPanel(gs_panel[SUBMENU]));
	EXCCHECKCVI( SetActivePanel( gs_panel[SUBMENU]));
	
Error:
	EXCDISPLAY( pexception);
}
#endif /* _NO_MENU */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SaveLastPosition"
static SElExceptionPtr SaveLastPosition(
   int	position,
   struct _SPanelItem spanel
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0; 
	bool_t			found = FALSE;
	int				i = 0;
	
	if(spanel.save_position)
	{
		if(gs_lastSavedPosition)
		{
			/* search */
			while(i<gs_lastSavedPosition->allocated && !found)
			{
				if(strlen(gs_lastSavedPosition->pSavedPos[i].panel)==0)
					break;
				
				if(0==strcmp(gs_lastSavedPosition->pSavedPos[i].panel, spanel.panel))
				{
					gs_lastSavedPosition->pSavedPos[i].position = position;
					found = TRUE;	
					continue;
				}
				
				i++;
			}
			
			if(!found && i<gs_lastSavedPosition->allocated)
			{
				gs_lastSavedPosition->pSavedPos[i].position = position;	
				strcpy(gs_lastSavedPosition->pSavedPos[i].panel, spanel.panel);
			}
			else if(!found && i==gs_lastSavedPosition->allocated) 
			{
				gs_lastSavedPosition->pSavedPos = realloc( gs_lastSavedPosition->pSavedPos, 
														   (gs_lastSavedPosition->allocated + 10)*sizeof(SLastPositionItem)); 
				EXCCHECKALLOC(gs_lastSavedPosition->pSavedPos); 
				gs_lastSavedPosition->allocated += 10; 
				
				gs_lastSavedPosition->pSavedPos[i].position = position;	
				strcpy(gs_lastSavedPosition->pSavedPos[i].panel, spanel.panel);  
			}
		}
		else
		{
			gs_lastSavedPosition = calloc(1, sizeof(SLastPosition));
			EXCCHECKALLOC(gs_lastSavedPosition);
			
			gs_lastSavedPosition->pSavedPos = calloc(10, sizeof(SLastPositionItem)); 
			EXCCHECKALLOC(gs_lastSavedPosition->pSavedPos);  
			gs_lastSavedPosition->allocated = 10;
			
			gs_lastSavedPosition->pSavedPos[0].position = position;
			strcpy(gs_lastSavedPosition->pSavedPos[0].panel, spanel.panel);
		}
	}
	
Error:
	EXCRETHROW( pexception);
}	

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetLastPosition"
static SElExceptionPtr GetLastPosition(
   const char* panel,
   int*	position
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0; 
	bool_t			found = FALSE;
	int				i = 0;
	
	if(position) *position = 0;
		
	if(gs_lastSavedPosition)
	{
		/* search */
		while(i<gs_lastSavedPosition->allocated && !found)
		{
			if(strlen(gs_lastSavedPosition->pSavedPos[i].panel)==0)
				break;
			
			if(0==strcmp(gs_lastSavedPosition->pSavedPos[i].panel, panel))
			{
				if(position) *position = gs_lastSavedPosition->pSavedPos[i].position;
				found = TRUE;				
				continue;
			}
			
			i++;
		}
	}

Error:
	EXCRETHROW( pexception);
}	

#ifndef _NO_MENU
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredLostFocus"
static void CVICALLBACK PostDeferredLostFocus(void* data)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				panel_active;
	int				visible;

	panel_active = GetActivePanel ();

	if( panel_active != gs_panel[PANEL]
		&& panel_active != gs_panel[MENU]
		&& panel_active != gs_panel[SUBMENU])
	{
		EXCCHECKCVI( GetPanelAttribute (gs_panel[MENU], ATTR_VISIBLE, &visible));

		if(visible)
		{
			EXCCHECKCVI( HidePanel(gs_panel[SUBMENU])); 
			EXCCHECKCVI( HidePanel(gs_panel[MENU])); 

			EXCCHECK( SetPanelTableHighlight(0));
		}
	}

Error:
	EXCDISPLAY( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanelCallback"
int CVICALLBACK OnPanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	switch (event)
	{
		case EVENT_LOST_FOCUS:
			EXCCHECKCVI( PostDeferredCall( PostDeferredLostFocus, NULL));
			break;		
		case EVENT_GOT_FOCUS:
			break;
	}
	
Error:
	return 0;
}
#else

int CVICALLBACK OnPanelCallback (int panel, int event, 
		void *callbackData, int eventData1, int eventData2)
{
	return 0;
}

#endif /* _NO_MENU */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ProcessEvents"
static SElExceptionPtr ProcessEvents(
   struct _SPanelItem 	spanel
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SElEventDataPtr	pevent_data = NULL; 
	int				i;	           
	
	if ( gs_EventFactory && spanel.valid && !spanel.dimmed)
	{
		EXCCHECK( eleventdata_new(&pevent_data));
		
		for(i=0; i<spanel.event_nb; i++)
		{
			EXCCHECKCVI( ProcessSystemEvents ());
			
			if(strlen(spanel.data[i])>0)
			{
				EXCCHECK( pevent_data->SetStringPtr(pevent_data, spanel.data[i])); 
				
				if(strlen(spanel.event[i])>0)
					EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, spanel.event[i], pevent_data));
			}
			else if(strlen(spanel.event[i])>0)
			{
				EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, spanel.event[i], NULL)); 
			}
		}
	}
	
Error:
	eleventdata_delete(&pevent_data); 
	EXCRETHROW(pexception);
}

#ifndef _NO_MENU
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMenu"
int CVICALLBACK OnMenu (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	bool_t			is_locked = FALSE; 
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			{
				int32_t error = 0;
				Point point, cell;
				bool_t valid_cell; 
			
				EXCCHECK( fncLock()); 
				is_locked = TRUE;

				point.y = eventData1;
				point.x = eventData2;
				
				EXCCHECKCVI( GetTableCellFromPoint (panel, control, point, &cell));
				
				valid_cell = cell.x && cell.y && SMenu[cell.y - 1].valid && !SMenu[cell.y - 1].dimmed;
				if(valid_cell)
				{
					cell.x = 2;
					MENU_HIGHLIGHT_SET( cell);
					EXCCHECK( SaveLastPosition(cell.y, SMenu[cell.y - 1]));
					EXCCHECK( ProcessEvents(SMenu[cell.y - 1]));
				}
			}
			break;
	}
	
Error:
	if(is_locked)
		fncUnlock();
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSubMenu"
int CVICALLBACK OnSubMenu (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	bool_t			is_locked = FALSE; 
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			{
				int32_t error = 0;
				Point point, cell;
				bool_t valid_cell; 

				EXCCHECK( fncLock()); 
				is_locked = TRUE;

				point.y = eventData1;
				point.x = eventData2;
				
				EXCCHECKCVI( GetTableCellFromPoint (panel, control, point, &cell));
				
				valid_cell = cell.x && cell.y && SSubMenu[cell.y - 1].valid && !SSubMenu[cell.y - 1].dimmed;
				if(valid_cell)
				{
					SUBMENU_HIGHLIGHT_SET( cell);	
					EXCCHECK( SaveLastPosition(cell.y, SSubMenu[cell.y - 1]));
					EXCCHECK( ProcessEvents(SSubMenu[cell.y - 1]));  
				}
			}
			break;
		}
	
Error:
	if(is_locked)
		fncUnlock();
	EXCDISPLAY( pexception);
	return 0;
}
#else

int CVICALLBACK OnMenu (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	return 0;
}

int CVICALLBACK OnSubMenu (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	return 0;
}

#endif /* _NO_MENU */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnPanel"
int CVICALLBACK OnPanel (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr	pexception = NULL;
	bool_t			is_locked = FALSE;

	if(gs_is_locked)
		goto Error;

	switch (event)
	{
		case EVENT_LEFT_CLICK:
			{
				int32_t error = 0;
				Point point, cell;
				bool_t valid_cell;

				EXCCHECK( fncLock()); 
				is_locked = TRUE;

				point.y = eventData1;
				point.x = eventData2;
				
				EXCCHECKCVI( GetTableCellFromPoint (panel, control, point, &cell));
				
				valid_cell = (cell.x && cell.y && SPanel[cell.x - 1].valid && !SPanel[cell.x - 1].dimmed);
				if(valid_cell)
				{
					PANEL_HIGHLIGHT_SET( cell);	
					EXCCHECK( SaveLastPosition(cell.x, SPanel[cell.x - 1]));
					EXCCHECK( ProcessEvents(SPanel[cell.x - 1]));  
				}
			}
			break;
	}
	
Error:
	if(is_locked)
		fncUnlock();
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckRights"
static SElExceptionPtr CheckRights(
   	struct _SPanelItem*	ppanel,
	int item_number
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	int				i;
	SUserListPtr	puserlist = NULL;
	
	if(gs_Modules)
		puserlist = (SUserListPtr)gs_Modules->Get(gs_Modules, MODULE_USER); 

	for(i=0; i<item_number; i++)
	{
		if(ppanel[i].valid)
		{
			ppanel[i].dimmed = FALSE; 
	
			if(strlen(ppanel[i].rights)>0)
			{
				SUserPtr puser = NULL; 
				
				ppanel[i].dimmed = TRUE;  
				
				if(puserlist)
				{
					EXCCHECK( puserlist->GetLoggedUser(puserlist, &puser)); 
					if(puser && puser->GetTesterRights(puser, ppanel[i].rights))
					{
						ppanel[i].dimmed = FALSE; 
					}
				}
			}
		}
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetPanelTableHighlight"
static SElExceptionPtr SetPanelTableHighlight(
	int position
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	Point			cell;
	
	/* storno selection */
	PANEL_HIGHLIGHT_RESET();
	
	if(position==0)  
		EXCCHECK( GetLastPosition(SPanel[0].panel, &position));	
	
	if( position>0)
	{
		cell.y = 1;
		cell.x = position;
			
		PANEL_HIGHLIGHT_SET( cell);		
		EXCCHECK( SaveLastPosition(cell.x, SPanel[cell.x - 1]));    
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetPanelTable"
static SElExceptionPtr SetPanelTable(
	int position
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	int				i;
	Point			cell;     
#ifndef _NO_SHORTCUT_VISIBLE
	char			help[128] = "";
	int				j;
#endif

	EXCCHECK( CheckRights(SPanel, PANEL_ITEM_NUMBER));
		
	for(i=1; i<PANEL_ITEM_NUMBER+1; i++)
	{
		cell.y = 1;
		cell.x = i;
	
		EXCCHECKCVI( SetTableCellVal (gs_panel[PANEL], PANEL_PANEL, cell, SPanel[i-1].name));
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[PANEL], PANEL_PANEL, cell, ATTR_CELL_DIMMED, SPanel[i-1].dimmed));

#ifndef _NO_SHORTCUT_VISIBLE
		EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PanelHelp[i-1], ATTR_DIMMED, SPanel[i-1].dimmed));
		/* check if shortcut exist */
		for(j=0; j<SHORTCUT_ITEM_NUMBER; j++)
		{
			if( SPanel[i-1].name[0]!='\0' && 0==strcmp(SPanel[i-1].name, SShortcut[j].name))
			{				
				/* application freezes if the msg is visible */			
				EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PanelHelp[i-1], ATTR_VISIBLE, FALSE));

				sprintf(help, "F%d", j+1);
				EXCCHECKCVI( SetCtrlVal( gs_panel[PANEL], PanelHelp[i-1], help));
				EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PanelHelp[i-1], ATTR_VISIBLE, TRUE));
				break;
			}
		}
		if( j==SHORTCUT_ITEM_NUMBER)
		{
			EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PanelHelp[i-1], ATTR_VISIBLE, FALSE));
		}
#endif
	}
	
	EXCCHECK( SetPanelTableHighlight(position));
	
Error:
	EXCRETHROW( pexception);
}

#ifndef _NO_MENU
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetMenuTableHighlight"
static SElExceptionPtr SetMenuTableHighlight(
	int position
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	Point			cell;
	
	/* storno selection */
	MENU_HIGHLIGHT_RESET();
	
	if(position<0)
	{
		;/* do nothing */
	}
	else
	{
		if(position==0)
			EXCCHECK( GetLastPosition(SMenu[0].panel, &position));	
	
		if( position>0)
		{
			cell.x = 2; 
			cell.y = position;
			MENU_HIGHLIGHT_SET( cell);
		}
	}

	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetMenuTable"
static SElExceptionPtr SetMenuTable(
	int position
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	int				i;
	Point			cell;        
	
	for(i=1; i<MENU_ITEM_NUMBER+1; i++)
	{
		cell.x = 2; 
		cell.y = i;
	
		EXCCHECKCVI( SetTableCellVal (gs_panel[MENU], MENU_MENU, cell, SMenu[i-1].name));
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[MENU], MENU_MENU, cell, ATTR_CELL_DIMMED, SMenu[i-1].dimmed));

		if(SMenu[i-1].pict!=0)
		{
			cell.x = 1;
			cell.y = i;
			EXCCHECKCVI( SetTableCellVal (gs_panel[MENU], MENU_MENU, cell, SMenu[i-1].pict));
		}
		else
		{
			cell.x = 1;
			cell.y = i;
			EXCCHECKCVI( SetTableCellVal (gs_panel[MENU], MENU_MENU, cell, 0));   
		}
	}
	
	EXCCHECK( SetMenuTableHighlight(position));
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetSubMenuTableHighlight"
static SElExceptionPtr SetSubMenuTableHighlight(
	int position 
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	Point			cell;
	
	SUBMENU_HIGHLIGHT_RESET();
	
	if(position==0)  
		EXCCHECK( GetLastPosition(SSubMenu[0].panel, &position));	
	
	if( position>0)
	{
		cell.x = 1;  
		cell.y = position;			
		SUBMENU_HIGHLIGHT_SET( cell);
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetSubMenuTable"
static SElExceptionPtr SetSubMenuTable(
	int position
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	int				i;
	Point			cell;     
	
	for(i=1; i<SUBMENU_ITEM_NUMBER+1; i++)
	{
		cell.x = 1;  
		cell.y = i;
	
		EXCCHECKCVI( SetTableCellVal (gs_panel[SUBMENU], SUBMENU_SUBMENU, cell, SSubMenu[i-1].name));
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[SUBMENU], SUBMENU_SUBMENU, cell, ATTR_CELL_DIMMED, SSubMenu[i-1].dimmed));
	}
	
	EXCCHECK( SetSubMenuTableHighlight(position)); 
	
Error:
	EXCRETHROW( pexception);
}
#endif /* _NO_MENU */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
static SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	bool_t			rights = FALSE;	
	char			mode[64] = "";
	char*			pdata;
	int				position;
	char			path[MAX_PATHNAME_LEN ];
	
	EXCCHECK( fncLock());  
	
	EXCCHECKCVI(GetModuleDir (__CVIUserHInst, path));
	strcat(path, PANEL_XML);

	if(0==strcmp(eventName, "EVNT_GUI_PANEL"))
	{
		if(pData)
		{
			const char* pmode = ((SElEventDataPtr)pData)->GetStringPtr(pData);   
			
			if(strlen(pmode)>0)
			{
				strcpy(mode, pmode);
				pdata = strtok(mode, ";");
				EXCCHECK( LoadConfiguration(path, pdata, SPanel, PANEL_ITEM_NUMBER));
				
				pdata = strtok(NULL, ";");
				if(pdata==NULL)
					position = 0;
				else
					position = strtol(pdata, NULL, 10);
				
				EXCCHECK( SetPanelTable(position));
			}
		}
	}
	else if(0==strcmp(eventName, "EVNT_GUI_PANEL_LOCK"))
	{
		gs_is_locked = TRUE;
	}
	else if(0==strcmp(eventName, "EVNT_GUI_PANEL_UNLOCK"))
	{
		gs_is_locked = FALSE;
	}
#ifndef _NO_MENU
	else if(0==strcmp(eventName, "EVNT_GUI_MENU"))
	{
		if(pData)
		{
			const char* pmode = ((SElEventDataPtr)pData)->GetStringPtr(pData);   
			
			if(strlen(pmode)>0)
			{	
				strcpy(mode, pmode);
				pdata = strtok(mode, ";");
				EXCCHECK( LoadConfiguration(path, pdata, SMenu, MENU_ITEM_NUMBER));
				
				pdata = strtok(NULL, ";");
				if(pdata==NULL)
					position = 0;
				else
					position = strtol(pdata, NULL, 10);
				
				EXCCHECK( SetMenuTable(position)); 
				EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayMenu, NULL));
			}
		}
	}
	else if(0==strcmp(eventName, "EVNT_GUI_SUBMENU"))
	{
		if(pData)
		{
			const char* pmode = ((SElEventDataPtr)pData)->GetStringPtr(pData);   
			
			if(strlen(pmode)>0)
			{	
				strcpy(mode, pmode);
				pdata = strtok(mode, ";");
				EXCCHECK( LoadConfiguration(path, pdata, SSubMenu, SUBMENU_ITEM_NUMBER));
				
				pdata = strtok(NULL, ";");
				if(pdata==NULL)
					position = 0;
				else
					position = strtol(pdata, NULL, 10);
				
				EXCCHECK( SetSubMenuTable(position)); 
				EXCCHECKCVI( PostDeferredCall( PostDeferredDisplaySubMenu, NULL));
			}
		}
	}
	else if(0==strcmp(eventName, "EVNT_GUI_MENU_HIDE"))
	{
		EXCCHECKCVI( PostDeferredCall( PostDeferredHideMenu, NULL));
	}
#endif /* _NO_MENU */
#ifndef _NO_SHORTCUT
	else if(0==strcmp(eventName, "EVNT_USER_LOGIN_VALIDATED"))
	{
		EXCCHECK( SetPanelTable(0));
		
		EXCCHECK( CheckRights(SShortcut, SHORTCUT_ITEM_NUMBER)); 
	}
	else if(0==strcmp(eventName, "EVNT_USER_LOGOUT_VALIDATED"))
	{
		EXCCHECK( SetPanelTable(0));
		
		EXCCHECK( CheckRights(SShortcut, SHORTCUT_ITEM_NUMBER));     
	}
	else if(0==strcmp(eventName, "EVNT_GUI_KEY_PRESSED"))
	{
		if(pData)
		{
			const char* key = ((SElEventDataPtr)pData)->GetStringPtr(pData);   
			
			if(strlen(key)>0 && !gs_is_locked)
			{	
				EXCCHECK( KeyPressed(key));	 
			}
		}
	}
#endif /* _NO_SHORTCUT */
	
Error:  
	fncUnlock();
	EXCRETHROW( pexception);
}

#ifndef _NO_MENU
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ButtonEventFunctionCallback"
static SElExceptionPtr GetIconsFromResource(void)
{
	SElExceptionPtr pexception = NULL;
	DWORD			size;
	HRSRC			hresInfo;
	HANDLE			hres;
	char*			pres = NULL;
	FILE*			pfile = NULL;
	int32_t			index;

	/* load resource */
	/*hresInfo = FindResource(g_hInstance, MAKEINTRESOURCE(IDR_PanelIcon), "ICONFILETYPE");
	if(hresInfo==NULL)
		EXCTHROW(-1, "FindResource error!");

	hres = LoadResource(g_hInstance, hresInfo);
	if(hres==NULL)
		EXCTHROW(-1, "LoadResource error!");
	
	size = SizeofResource(g_hInstance, hresInfo);
	pres = (char*)LockResource(hres);*/

	GetTempPath(255, g_PanelIcons);
	strcat(g_PanelIcons, PANEL_ICONS);

	//pfile = fopen(g_PanelIcons, "wb+");;

	/*for(index=0; index<size; index++)
		fputc(pres[index], pfile); */

Error:
	if(pfile)
		fclose(pfile);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ButtonEventFunctionCallback"
static SElExceptionPtr ButtonEventFunctionCallback( const char* eventName, void* pData)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	int32_t			i, j;
	int				visible, visible_submenu;
	bool_t			bReset = FALSE;
	Point			cell;

	EXCCHECK( fncLock());  

	EXCCHECKCVI( GetPanelAttribute( gs_panel[MENU], ATTR_VISIBLE, &visible));
	EXCCHECKCVI( GetPanelAttribute( gs_panel[SUBMENU], ATTR_VISIBLE, &visible_submenu));

	if(0==strcmp(eventName, "EVNT_TESTER_BUTTON_PRESSED_OK"))
	{
		if( visible && !visible_submenu)
		{
			for(i=0; i<MENU_ITEM_NUMBER; i++)
			{
				if( SMenu[i].active == TRUE)
				{
					OnMenu(gs_panel[MENU], MENU_MENU, EVENT_LEFT_CLICK, NULL, MENU_CELL_Y_OFFSET + i*MENU_CELL_Y_SHIFT, 40);
					break;
				}
			}
		}
		else if( visible_submenu)
		{
			for(i=0; i<SUBMENU_ITEM_NUMBER; i++)
			{
				if( SSubMenu[i].active == TRUE)
				{
					OnSubMenu(gs_panel[SUBMENU], SUBMENU_SUBMENU, EVENT_LEFT_CLICK, NULL, SUBMENU_CELL_Y_OFFSET + i*SUBMENU_CELL_Y_SHIFT, 40);
					break;
				}
			}
		}
	}
	else if(0==strcmp(eventName, "EVNT_TESTER_BUTTON_PRESSED_NOK"))
	{
		if( visible)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideMenu, NULL));
			EXCCHECK( SetPanelTableHighlight(0));
		}

	}
	else if(0==strcmp(eventName, "EVNT_TESTER_BUTTON_PRESSED_MENU"))
	{
		if( !visible)
		{
			for(i=0; i<PANEL_ITEM_NUMBER; i++)
			{
				for(j=0; j<SPanel[i].event_nb; j++)
				{
					if( 0==strcmp(SPanel[i].event[i], "EVNT_GUI_MENU"))
					{
						OnPanel(gs_panel[PANEL], PANEL_PANEL, EVENT_LEFT_CLICK, NULL, 40, PANEL_CELL_X_OFFSET + i*PANEL_CELL_X_SHIFT);
						bReset = TRUE;
						break;
					}
				}
			}

			if( bReset)
			{
				for(i=0; i<MENU_ITEM_NUMBER; i++)
				{
					SMenu[i].active = FALSE;
				}
				for(i=0; i<SUBMENU_ITEM_NUMBER; i++)
				{
					SSubMenu[i].active = FALSE;
				}
			}
		}
		else if( visible && !visible_submenu)
		{
			for(i=0; i<MENU_ITEM_NUMBER; i++)
			{
				if( SMenu[i].active == TRUE) /* highlight next item */
				{
					/* find next valid cell */
					for( j=i+1; j<MENU_ITEM_NUMBER; j++)
					{
						if( SMenu[j].valid == TRUE)
						{
							cell.y = j+1;
							break;
						}
					}
					if( j==MENU_ITEM_NUMBER)
					{
						cell.y = 1;
					}
					SMenu[i].active = FALSE;
					SMenu[cell.y-1].active = TRUE;
					break;
				}
			}
			if( i==MENU_ITEM_NUMBER) /* highlight first item if there is not saved position */
			{
				int	position;

				/* look for saved position */
				EXCCHECK( GetLastPosition(SMenu[0].panel, &position));

				if(position==0) /* start with first item */
				{
					cell.y = 1;
					SMenu[0].active = TRUE;
				}
				else /* start with saved position */ 
				{
					/* find next valid cell */
					for( j=position; j<MENU_ITEM_NUMBER; j++)
					{
						if( SMenu[j].valid == TRUE)
						{
							cell.y = j+1;
							break;
						}
					}
					if( j==MENU_ITEM_NUMBER)
					{
						cell.y = 1;
					}	
					SMenu[cell.y-1].active = TRUE;
				}
			}

			cell.x = 2;
			MENU_HIGHLIGHT_SET( cell);
		}
		else if( visible_submenu)
		{
			for(i=0; i<SUBMENU_ITEM_NUMBER; i++)
			{
				if( SSubMenu[i].active == TRUE) /* highlight next item */
				{
					/* find next valid cell */
					for( j=i+1; j<SUBMENU_ITEM_NUMBER; j++)
					{
						if( SSubMenu[j].valid == TRUE)
						{
							cell.y = j+1;
							break;
						}
					}
					if( j==SUBMENU_ITEM_NUMBER)
					{
						cell.y = 1;
					}
					SSubMenu[i].active = FALSE;
					SSubMenu[cell.y-1].active = TRUE;
					break;
				}
			}
			if( i==SUBMENU_ITEM_NUMBER) /* highlight first item if there is not saved position */
			{
				int	position;

				/* look for saved position */
				EXCCHECK( GetLastPosition(SSubMenu[0].panel, &position));

				if(position==0) /* start with first item */
				{
					cell.y = 1;
					SSubMenu[0].active = TRUE;
				}
				else /* start with saved position */
				{
					/* find next valid cell */
					for( j=position; j<SUBMENU_ITEM_NUMBER; j++)
					{
						if( SSubMenu[j].valid == TRUE)
						{
							cell.y = j+1;
							break;
						}
					}
					if( j==SUBMENU_ITEM_NUMBER)
					{
						cell.y = 1;
					}	
					SSubMenu[cell.y-1].active = TRUE;
				}
			}
			cell.x = 1;
			SUBMENU_HIGHLIGHT_SET( cell);	
		}
	}

Error:  
	fncUnlock();
	EXCRETHROW( pexception);
}
#else

static SElExceptionPtr ButtonEventFunctionCallback( const char* eventName, void* pData)
{
	return NULL;
}

#endif /* _NO_MENU */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
	int				error = 0;
	SElExceptionPtr	pexception = NULL;
	
	if ( gs_EventFactory )
	{
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_PANEL",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_PANEL_LOCK",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_PANEL_UNLOCK",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_MENU",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_SUBMENU",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_MENU_HIDE",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_USER_LOGIN_VALIDATED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_USER_LOGOUT_VALIDATED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_KEY_PRESSED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_TESTER_BUTTON_PRESSED_OK",
														 ButtonEventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_TESTER_BUTTON_PRESSED_NOK",
														 ButtonEventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_TESTER_BUTTON_PRESSED_MENU",
														 ButtonEventFunctionCallback));
	}

Error:
	EXCRETHROW(pexception);
}	/* RegisterEventFunction */

/*---------------------------------------------------------------------------*/
#if defined(_CVI_DLL_) || \
	defined(_WINDLL)
void DiscardUIObjectsForDLL (void)
{
	/* Discard the panels loaded in InitUIForDLL */
}

int __stdcall DllMain(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved
)
{
	switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
#if (!defined(_CVI_)&&defined(_DEBUG))
			//_CrtSetBreakAlloc(79);
#endif
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)		
				return 0;	/* out of memory */
			g_hInstance = hinstDLL;
			break;
		case DLL_PROCESS_DETACH:
			if (!CVIRTEHasBeenDetached())		/* Do not call CVI functions if cvirte.dll has already been detached. */
			{
				DiscardUIObjectsForDLL ();		/* Discard the panels loaded in InitUIForDLL */
				CloseCVIRTE ();		
			}
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
			_CrtDumpMemoryLeaks();
#endif
			break;
		}

	return 1;
}

int __stdcall DllEntryPoint(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved
)
{
	/* Included for compatibility with Borland */

	 return DllMain (hinstDLL, fdwReason, lpvReserved);
}

#undef __FUNC__
#define __FUNC__ "{Panel Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				main_panel = 0;
	const char*		pconfiguration_file = NULL;
	int				monitor_id, top, height, width;  
	
	InitializeCriticalSection( &_Lock);
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "MainPanel",
									 sizeof(main_panel),
									 &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "Modules",
	                        sizeof(SObjListPtr),
	                        &gs_Modules));

	/* gettext */
	if(gs_Modules)
	{
		gs_pLanguage = (SLanguagePtr)gs_Modules->Get(gs_Modules, MODULE_LANGUAGE);  
		if(gs_pLanguage)
		{
			char pathname[MAX_PATHNAME_LEN];
			EXCCHECKCVI( GetProjectDir( pathname));
			strcat(pathname, "\\lang");
			LANGUAGE_PTR->bindtextdomain (LANGUAGE_PTR, PACKAGE, pathname);
			LANGUAGE_PTR->textdomain (LANGUAGE_PTR, PACKAGE);
		}
	}
	
	EXCCHECKCVI( gs_panel[PANEL] = LoadPanelEx (main_panel, "panel.uir", PANEL, __CVIUserHInst));
#ifndef _NO_MENU
	EXCCHECKCVI( gs_panel[MENU] = LoadPanelEx (main_panel, "panel.uir", MENU, __CVIUserHInst)); 
	EXCCHECKCVI( gs_panel[SUBMENU] = LoadPanelEx (main_panel, "panel.uir", SUBMENU, __CVIUserHInst));
#endif
#ifndef _NO_SHORTCUT
	EXCCHECKCVI( gs_panel[SHORTCUT] = LoadPanelEx (main_panel, "panel.uir", SHORTCUT, __CVIUserHInst));
#endif

	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP, &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_WIDTH, &width));   

	/* adjust to resolution 1024x768 */
	if(	width==1024)
	{
		int32_t		i;
		/* set panels constatnt */		
		PANEL_TOP = 60;
		PANEL_LEFT = 5;

		MENU_TOP = PANEL_TOP + 598;
		MENU_LEFT = 6;
		MENU_WIDTH = 374;
		MENU_HEIGHT	= 597;
		MENU_ITEM_HEIGHT = 66;

		SUBMENU_LEFT = MENU_LEFT + MENU_WIDTH;

		EXCCHECKCVI( SetCtrlVal( gs_panel[PANEL], PANEL_PICTURERING, 1));
		
		EXCCHECKCVI( SetTableRowAttribute (gs_panel[PANEL], PANEL_PANEL, -1, ATTR_ROW_HEIGHT, 48));
		EXCCHECKCVI( SetTableColumnAttribute (gs_panel[PANEL], PANEL_PANEL, -1, ATTR_COLUMN_WIDTH, 126));
		EXCCHECKCVI( SetTableCellRangeAttribute (gs_panel[PANEL], PANEL_PANEL, MakeRect (1, 1, 1, PANEL_ITEM_NUMBER),
                            ATTR_TEXT_POINT_SIZE, 20));
#ifndef _NO_SHORTCUT
		/* shift shortcut help */
		for(i=0; i<PANEL_ITEM_NUMBER;i++)
		{
			EXCCHECKCVI( SetCtrlAttribute( gs_panel[PANEL], PanelHelp[i], ATTR_LEFT, SHORTCUT_HELP_OFFSET+i*SHORTCUT_HELP_SHIFT));
		}
#endif
#ifndef _NO_MENU
		EXCCHECKCVI( SetCtrlVal( gs_panel[MENU], MENU_PICTURERING, 1));
		EXCCHECKCVI( SetTableRowAttribute (gs_panel[MENU], MENU_MENU, -1, ATTR_ROW_HEIGHT, MENU_ITEM_HEIGHT));
		EXCCHECKCVI( SetTableColumnAttribute (gs_panel[MENU], MENU_MENU, 1, ATTR_COLUMN_WIDTH, MENU_ITEM_HEIGHT));

		EXCCHECKCVI( SetPanelAttribute (gs_panel[MENU], ATTR_WIDTH, MENU_WIDTH));
		EXCCHECKCVI( SetPanelAttribute (gs_panel[MENU], ATTR_HEIGHT, MENU_HEIGHT));
#endif
	}
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_TOP, top + height - PANEL_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PANEL], ATTR_LEFT, PANEL_LEFT)); 

#ifndef _NO_MENU
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MENU], ATTR_TOP, top + height - MENU_TOP));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[MENU], ATTR_LEFT, MENU_LEFT));   
#endif

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
#ifndef _NO_SHORTCUT
	/* shortcuts */
	EXCCHECK( LoadConfiguration(NULL, "shortcut", SShortcut, SHORTCUT_ITEM_NUMBER)); 
	EXCCHECK( CheckRights(SShortcut, SHORTCUT_ITEM_NUMBER)); 
	
	/* shortcuts config dialog */
	EXCCHECK( LoadShortcutConfiguration(NULL));
	EXCCHECK( FillRings(gs_panel[SHORTCUT]));
#endif
	
	EXCCHECK( LoadConfiguration(NULL, "panel", SPanel, PANEL_ITEM_NUMBER));
#ifndef _NO_MENU
	EXCCHECK( SetPanelTable(2));
#else
	EXCCHECK( SetPanelTable(1));
#endif
	
	EXCCHECK( RegisterEventFunction());

#ifndef _NO_MENU	
	EXCCHECK( elzip_new(&gs_pZip));
	EXCCHECK( GetIconsFromResource());
#endif

	EXCCHECKCVI( DisplayPanel(gs_panel[PANEL]));

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
#ifndef _NO_MENU
	int32_t			i;
#endif

	if(gs_lastSavedPosition)
	{
		if(gs_lastSavedPosition->pSavedPos)
		{
			free(gs_lastSavedPosition->pSavedPos);
			gs_lastSavedPosition->pSavedPos = NULL;
		}
		
		free(gs_lastSavedPosition);
		gs_lastSavedPosition = NULL;
	}

#ifndef _NO_MENU
	/* clear menu pictures */
	for(i=0; i<MENU_ITEM_NUMBER; i++)
	{
		if(SMenu[i].pict!=0)
		{
			DiscardBitmap(SMenu[i].pict);
			SMenu[i].pict = 0;
		}
	}

	EXCCHECK( elzip_delete(&gs_pZip));
#endif
	
	if(gs_panel[PANEL])
		DiscardPanel(gs_panel[PANEL]);
	
	if(gs_panel[MENU])
		DiscardPanel(gs_panel[MENU]);
	
	if(gs_panel[SUBMENU])
		DiscardPanel(gs_panel[SUBMENU]);
	
	DeleteCriticalSection( &_Lock);  
	
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Panel Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Panel Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 

/*---------------------------------------------------------------------------*/
/* SHORTCUT ******************************************************************/
/*---------------------------------------------------------------------------*/
#ifndef _NO_SHORTCUT

#define SHORTCUT_BUFFER_ITEM_NUMBER 	64

static struct _SShortcutItem
{
	int		position;
	char	name[256];
	
} SShortcutItem, SShortcutBuff[SHORTCUT_BUFFER_ITEM_NUMBER];

#define SHORTCUT_ITEM_NUMBER 	12 

static struct _SFcontrol
{
	const int	control;
	int			value;
	
} gs_Fcontrol[SHORTCUT_ITEM_NUMBER] =  	{	
										{SHORTCUT_F_1,0},
										{SHORTCUT_F_2,0}, 
										{SHORTCUT_F_3,0}, 
										{SHORTCUT_F_4,0}, 
										{SHORTCUT_F_5,0}, 
										{SHORTCUT_F_6,0}, 
										{SHORTCUT_F_7,0}, 
										{SHORTCUT_F_8,0}, 
										{SHORTCUT_F_9,0}, 
										{SHORTCUT_F_10,0}, 
										{SHORTCUT_F_11,0},
										{SHORTCUT_F_12,0} 
										};

static SElExceptionPtr SaveShortcutConfiguration(
   const char*	fileName,
   int			panel,
   bool_t		def
);

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayShortcut"
static void CVICALLBACK PostDeferredDisplayShortcut (void *callbackData) 
{ 
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			visible;

	EXCCHECKCVI( GetPanelAttribute(gs_panel[SHORTCUT], ATTR_VISIBLE, &visible));
	if(!visible)
		EXCCHECKCVI( InstallPopup( gs_panel[SHORTCUT]));

Error:
	EXCDISPLAY( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredHideShortcut"
static void CVICALLBACK PostDeferredHideShortcut (void *callbackData) 
{	
	RemovePopup(0); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnButton"
int CVICALLBACK OnButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	char			path[MAX_PATHNAME_LEN];

	switch (event)
	{
		case EVENT_COMMIT:

			EXCCHECKCVI(GetModuleDir (__CVIUserHInst, path));
			strcat(path, PANEL_XML);

			switch(control)
			{
				case SHORTCUT_SAVE:
					EXCCHECK( SaveShortcutConfiguration(path, panel, FALSE));
					EXCCHECKCVI( PostDeferredCall( PostDeferredHideShortcut, NULL)); 
					break;
				case SHORTCUT_DEFAULT:
					EXCCHECK( SaveShortcutConfiguration(path, panel, TRUE));        
					EXCCHECK( LoadShortcutConfiguration(path));
					EXCCHECK( FillRings(panel));
					break;
				case SHORTCUT_CANCEL:
					EXCCHECKCVI( PostDeferredCall( PostDeferredHideShortcut, NULL));
					break;
			}
			EXCCHECK( LoadConfiguration(path, "shortcut", SShortcut, SHORTCUT_ITEM_NUMBER));  
			break;
	}
	
Error:
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnF"
int CVICALLBACK OnF (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	int				pos = -1, index, i;
	char			label[256];
	
	switch (event)
	{
		case EVENT_COMMIT:

			switch(control)
			{
				case SHORTCUT_F_1:
					pos = 0;
					break;
				case SHORTCUT_F_2:
					pos = 1;
					break;
				case SHORTCUT_F_3:
					pos = 2;
					break;
				case SHORTCUT_F_4:
					pos = 3;
					break;
				case SHORTCUT_F_5:
					pos = 4;
					break;
				case SHORTCUT_F_6:
					pos = 5;
					break;
				case SHORTCUT_F_7:
					pos = 6;
					break;
				case SHORTCUT_F_8:
					pos = 7;
					break;
				case SHORTCUT_F_9:
					pos = 8;
					break;
				case SHORTCUT_F_10:
					pos = 9;
					break;
				case SHORTCUT_F_11:
					pos = 10;
					break;
				case SHORTCUT_F_12:
					pos = 11;
					break;
			}
			GetCtrlIndex(panel, control, &index);

			if(index)
			{
				GetLabelFromIndex (panel, control, index, label);
				
				if(gs_Fcontrol[pos].value)
					SShortcutBuff[gs_Fcontrol[pos].value-1].position = 99;
				
				for(i=0; i<SHORTCUT_BUFFER_ITEM_NUMBER; i++)
				{
					if(0==strcmp(SShortcutBuff[i].name, label))
					{
						SShortcutBuff[i].position = pos+1;	
						gs_Fcontrol[pos].value = i+1;
						break;
					}
				}
				
				FillRings(panel); 
			}
			else if(gs_Fcontrol[pos].value)
			{
				SShortcutBuff[gs_Fcontrol[pos].value-1].position = 99;
				gs_Fcontrol[pos].value = 0;
				
				FillRings(panel); 
			}
			
			break;
	}
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "KeyPressed"
static SElExceptionPtr KeyPressed(
	const char* key
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	
	if(0==strcmp(key, "F1_KEY"))
	{
		EXCCHECK( ProcessEvents(SShortcut[0]));   		
	}
	else if(0==strcmp(key, "F2_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[1]));  
	}
	else if(0==strcmp(key, "F3_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[2]));  
	}
	else if(0==strcmp(key, "F4_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[3]));  
	}
	else if(0==strcmp(key, "F5_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[4]));  
	}
	else if(0==strcmp(key, "F6_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[5]));  
	}
	else if(0==strcmp(key, "F7_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[6]));  
	}
	else if(0==strcmp(key, "F8_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[7]));  
	}
	else if(0==strcmp(key, "F9_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[8]));  
	}
	else if(0==strcmp(key, "F10_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[9]));  
	}
	else if(0==strcmp(key, "F11_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[10]));  
	}
	else if(0==strcmp(key, "F12_KEY")) 
	{
		EXCCHECK( ProcessEvents(SShortcut[11]));  
	}
	else if(0==strcmp(key, "CTRL_S_KEY")) 
	{
		EXCCHECKCVI( PostDeferredCall(PostDeferredDisplayShortcut,NULL));  
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "FillRings"
static SElExceptionPtr FillRings(
   	int		panel
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	int				i, j, control, pos;
	
	for(i=0; i<SHORTCUT_ITEM_NUMBER; i++)
	{
		control = gs_Fcontrol[i].control;
		
		EXCCHECKCVI( DeleteListItem (panel, control, 0, -1));
		
		pos = 0;
		
		EXCCHECKCVI( InsertListItem (panel, control, pos++, "none", 0));  
		
		for(j=0; j<SHORTCUT_BUFFER_ITEM_NUMBER; j++)
		{
			if(SShortcutBuff[j].position==99 || SShortcutBuff[j].position==(i+1))
			{
				EXCCHECKCVI( InsertListItem (panel, control, pos, SShortcutBuff[j].name, SShortcutBuff[j].position));
				
				if(SShortcutBuff[j].position==(i+1))
				{
					EXCCHECKCVI( SetCtrlIndex (panel, control, pos));
					gs_Fcontrol[i].value = j+1;
				}
				pos++;
			}
		}
	}
	
Error:
	EXCRETHROW( pexception);     
}
	
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadShortcutConfiguration"
static SElExceptionPtr LoadShortcutConfiguration(
   const char*		fileName
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0, pnode2 = 0, pnode2old = 0;
	CAObjHandle     pparameter = 0;
	const char*     pfile_name = (fileName) ? fileName : "plugins/panel.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*         	ptext= NULL;
	int				i = 0;
	
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_name));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//plugin[@id='"PLUGIN_PANEL"']", &pnode));   	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "shortcut/item", &pnode2));
	
	while(pnode2!=0 && i<SHORTCUT_BUFFER_ITEM_NUMBER)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@position", &pparameter)); 
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				SShortcutBuff[i].position = strtol(ptext, NULL, 10);
			}
		
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "name", &pparameter)); 
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				strcpy(SShortcutBuff[i].name, ptext);;
			}
		
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		pnode2old = pnode2;
		MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2);
		EXCCHECKCVI( CA_DiscardObjHandle(pnode2old));
		pnode2old = 0; 
		
		i++;
	}
		
Error:
	CA_FreeMemory(ptext);
	CA_VariantClear(&pfile_name_var);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
} 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetNewPosition"
static SElExceptionPtr GetNewPosition(
	int			panel,
	const char* ptext, 
	int* 		pos
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;	
	int				i, index, control, count;
	char			label[256];
	
	for(i=0; i<SHORTCUT_ITEM_NUMBER; i++)
	{
		control = gs_Fcontrol[i].control;
		
		EXCCHECKCVI( GetNumListItems (panel, control, &count));

		EXCCHECKCVI( GetCtrlIndex (panel, control, &index));  	
		
		if(index)
		{
			EXCCHECKCVI( GetLabelFromIndex (panel, control, index, label));
			
			if(0==strcmp(label, ptext))
			{
				if(pos) *pos = i+1;
				break;
			}
		}
	}
	
	if(i==SHORTCUT_ITEM_NUMBER)
	{
		if(pos) *pos = 99; 
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SaveShortcutConfiguration"
static SElExceptionPtr SaveShortcutConfiguration(
   const char*		fileName,
   int				panel,
   bool_t			def
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0, pnode2 = 0, pnode2old = 0;
	CAObjHandle     pparameter = 0;
	const char*     pfile_name = (fileName) ? fileName : "plugins/panel.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*         	ptext= NULL;
	int				i = 0, pos;
	char			buffer[128];

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(&pfile_name_var, pfile_name));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//plugin[@id='"PLUGIN_PANEL"']", &pnode));   
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "shortcut/item", &pnode2));
		
	while(pnode2!=0 && i<SHORTCUT_BUFFER_ITEM_NUMBER)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, (def)? "@default":"name", &pparameter)); 
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext && def)
			{
				pos = strtol(ptext, NULL, 10);	
			}
			else if(ptext)
			{
				EXCCHECK( GetNewPosition(panel, ptext, &pos));
			}
		
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@position", &pparameter)); 
		if ( pparameter )
		{
			sprintf(buffer, "%d", pos);
			
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, buffer));
		
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}
		
		pnode2old = pnode2;
		MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2);
		EXCCHECKCVI( CA_DiscardObjHandle(pnode2old));
		pnode2old = 0; 
		
		i++;
	}
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentsave (pxml, NULL, pfile_name_var));    
		
Error:
	CA_FreeMemory(ptext);
	CA_VariantClear(&pfile_name_var);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
} 

#else

int CVICALLBACK OnButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	return 0;
}

int CVICALLBACK OnF (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	return 0;
}

#endif /* _NO_SHORTCUT */
