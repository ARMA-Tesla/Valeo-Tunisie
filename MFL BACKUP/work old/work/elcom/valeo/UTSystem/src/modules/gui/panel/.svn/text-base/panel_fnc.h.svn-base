#if !defined(__PANEL_FNC_H__)
#define __PANEL_FNC_H__

#include <eventfactory/EventFactory.h> 

#define PANEL_XML		  	"\\panel.xml" 
#define PANEL_ICONS			"panel.zip"
#define PANEL_PICT		  	"%s.bmp" 

#define PANEL_HIGHLIGHT_RESET() \
	if( gs_lastPanelCell.x && gs_lastPanelCell.y) { \
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[PANEL], PANEL_PANEL, gs_lastPanelCell, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT)); \
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[PANEL], PANEL_PANEL, gs_lastPanelCell, ATTR_TEXT_COLOR, COLOR_TEXT)); }
				
#define PANEL_HIGHLIGHT_SET( cell) \
	PANEL_HIGHLIGHT_RESET(); \
	EXCCHECKCVI( SetTableCellAttribute (gs_panel[PANEL], PANEL_PANEL, cell, ATTR_TEXT_BGCOLOR, 0x08246B)); \
	EXCCHECKCVI( SetTableCellAttribute (gs_panel[PANEL], PANEL_PANEL, cell, ATTR_TEXT_COLOR, VAL_WHITE)); \
	gs_lastPanelCell = cell

#define MENU_HIGHLIGHT_RESET() \
	if( gs_lastMenuCell.x && gs_lastMenuCell.y ) { \
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[MENU], MENU_MENU, gs_lastMenuCell, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT)); \
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[MENU], MENU_MENU, gs_lastMenuCell, ATTR_TEXT_COLOR, COLOR_TEXT)); }
			
#define MENU_HIGHLIGHT_SET( cell) \
	MENU_HIGHLIGHT_RESET(); \
	EXCCHECKCVI( SetTableCellAttribute (gs_panel[MENU], MENU_MENU, cell, ATTR_TEXT_BGCOLOR, 0x08246B)); \
	EXCCHECKCVI( SetTableCellAttribute (gs_panel[MENU], MENU_MENU, cell, ATTR_TEXT_COLOR, VAL_WHITE)); \
	gs_lastMenuCell = cell

#define SUBMENU_HIGHLIGHT_RESET() \
	if( gs_lastSubMenuCell.x && gs_lastSubMenuCell.y ) { \
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[SUBMENU], SUBMENU_SUBMENU, gs_lastSubMenuCell, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT)); \
		EXCCHECKCVI( SetTableCellAttribute (gs_panel[SUBMENU], SUBMENU_SUBMENU, gs_lastSubMenuCell, ATTR_TEXT_COLOR, COLOR_TEXT)); }

#define SUBMENU_HIGHLIGHT_SET( cell) \
	SUBMENU_HIGHLIGHT_RESET(); \
	EXCCHECKCVI( SetTableCellAttribute (gs_panel[SUBMENU], SUBMENU_SUBMENU, cell, ATTR_TEXT_BGCOLOR, 0x08246B)) \
	EXCCHECKCVI( SetTableCellAttribute (gs_panel[SUBMENU], SUBMENU_SUBMENU, cell, ATTR_TEXT_COLOR, VAL_WHITE)) \
	gs_lastSubMenuCell = cell

#define MAX_EVENT_NB			8
#define MAX_EVENT_DATA_LENGTH	64

typedef struct _SPanelItem
{
	char	panel[64];		
	bool_t	valid;
	bool_t	dimmed;
	char	name[64];
	int		pict; 
	bool_t	active;
	char	rights[64];
	bool_t	save_position;
	char	event[MAX_EVENT_NB][ELEVENT_MAX_NAME_LENGTH+1];
	char	data[MAX_EVENT_NB][MAX_EVENT_DATA_LENGTH+1];
	int		event_nb;
} SPanelItem, *SPanelItemPtr;

typedef struct _SLastPositionItem
{
	char	panel[128];	
	int		position;
	
} SLastPositionItem, *SLastPositionItemPtr;

typedef struct _SLastPosition
{
	int						allocated;
	SLastPositionItemPtr	pSavedPos;
	
} SLastPosition, *SLastPositionPtr;

/* export functions */
SElExceptionPtr LoadConfiguration(
   const char*			fileName,
   const char*			spec,
   struct _SPanelItem* 	spanel,
   int 					item_number
);

#endif