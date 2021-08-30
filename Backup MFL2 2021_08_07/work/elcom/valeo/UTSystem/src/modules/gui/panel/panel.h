/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2007. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  MENU                            1       /* callback function: OnPanelCallback */
#define  MENU_MENU                       2       /* callback function: OnMenu */
#define  MENU_RING_PICT                  3
#define  MENU_PICTURERING                4

#define  PANEL                           2       /* callback function: OnPanelCallback */
#define  PANEL_PANEL                     2       /* callback function: OnPanel */
#define  PANEL_PICTURERING               3
#define  PANEL_MSG_P1                    4
#define  PANEL_MSG_P2                    5
#define  PANEL_MSG_P3                    6
#define  PANEL_MSG_P4                    7
#define  PANEL_MSG_P5                    8
#define  PANEL_MSG_P6                    9
#define  PANEL_MSG_P7                    10
#define  PANEL_MSG_P8                    11

#define  SHORTCUT                        3
#define  SHORTCUT_F_1                    2       /* callback function: OnF */
#define  SHORTCUT_F_2                    3       /* callback function: OnF */
#define  SHORTCUT_F_3                    4       /* callback function: OnF */
#define  SHORTCUT_F_4                    5       /* callback function: OnF */
#define  SHORTCUT_F_5                    6       /* callback function: OnF */
#define  SHORTCUT_F_6                    7       /* callback function: OnF */
#define  SHORTCUT_F_7                    8       /* callback function: OnF */
#define  SHORTCUT_F_8                    9       /* callback function: OnF */
#define  SHORTCUT_F_9                    10      /* callback function: OnF */
#define  SHORTCUT_F_10                   11      /* callback function: OnF */
#define  SHORTCUT_F_11                   12      /* callback function: OnF */
#define  SHORTCUT_F_12                   13      /* callback function: OnF */
#define  SHORTCUT_SAVE                   14      /* callback function: OnButton */
#define  SHORTCUT_DEFAULT                15      /* callback function: OnButton */
#define  SHORTCUT_CANCEL                 16      /* callback function: OnButton */
#define  SHORTCUT_PICTURE                17

#define  SUBMENU                         4       /* callback function: OnPanelCallback */
#define  SUBMENU_SUBMENU                 2       /* callback function: OnSubMenu */
#define  SUBMENU_PICTURE                 3


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnF(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMenu(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSubMenu(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
