/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2017. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  STAT                             1
#define  STAT_CAL_DISPLAY                 2       /* control type: table, callback function: OnCalDisplay */
#define  STAT_MONTH_PREV                  3       /* control type: command, callback function: OnMonthChange */
#define  STAT_MONTH_NEXT                  4       /* control type: command, callback function: OnMonthChange */
#define  STAT_DECORATION                  5       /* control type: deco, callback function: (none) */
#define  STAT_MONTH_DISPLAY               6       /* control type: textMsg, callback function: (none) */
#define  STAT_GRAPH                       7       /* control type: graph, callback function: (none) */
#define  STAT_BKTREE                      8       /* control type: activeX, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnCalDisplay(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMonthChange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
