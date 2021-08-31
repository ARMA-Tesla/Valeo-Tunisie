/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2020. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_GRAPH_2                    2       /* control type: graph, callback function: (none) */
#define  PANEL_GRAPH                      3       /* control type: graph, callback function: (none) */
#define  PANEL_TEXT2                      4       /* control type: textMsg, callback function: (none) */
#define  PANEL_REPEAT                     5       /* control type: pictButton, callback function: OnButton */
#define  PANEL_BACKWARD                   6       /* control type: pictButton, callback function: OnButton */
#define  PANEL_FORWARD                    7       /* control type: pictButton, callback function: OnButton */
#define  PANEL_STOP                       8       /* control type: pictButton, callback function: OnButton */
#define  PANEL_PICTURE_2                  9       /* control type: picture, callback function: (none) */
#define  PANEL_WARNING                    10      /* control type: pictRing, callback function: (none) */
#define  PANEL_ACTION                     11      /* control type: pictRing, callback function: (none) */
#define  PANEL_TABLE                      12      /* control type: table, callback function: (none) */
#define  PANEL_STATION_8                  13      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_7                  14      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_6                  15      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_5                  16      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_4                  17      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_88                 18      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_87                 19      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_86                 20      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_85                 21      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_89                 22      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_84                 23      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_82                 24      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_81                 25      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_3                  26      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_2                  27      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_80                 28      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_70                 29      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_60                 30      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATE_STATION_20           31      /* control type: pictRing, callback function: (none) */
#define  PANEL_STATE_STATION_50           32      /* control type: pictRing, callback function: (none) */
#define  PANEL_STATE_STATION_10           33      /* control type: pictRing, callback function: (none) */
#define  PANEL_STATE_STATION_60           34      /* control type: pictRing, callback function: (none) */
#define  PANEL_STATE_STATION_40           35      /* control type: pictRing, callback function: (none) */
#define  PANEL_STATE_STATION_30           36      /* control type: pictRing, callback function: (none) */
#define  PANEL_STATE_STATION_70           37      /* control type: pictRing, callback function: (none) */
#define  PANEL_STATE_STATION_80           38      /* control type: pictRing, callback function: (none) */
#define  PANEL_STATION_50                 39      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_40                 40      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_30                 41      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_20                 42      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_10                 43      /* control type: textMsg, callback function: (none) */
#define  PANEL_STATION_1                  44      /* control type: textMsg, callback function: (none) */
#define  PANEL_HELP2                      45      /* control type: textMsg, callback function: (none) */
#define  PANEL_HELP1                      46      /* control type: textMsg, callback function: (none) */
#define  PANEL_LABEL_REF_CUSTOMER         47      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXT3                      48      /* control type: textMsg, callback function: (none) */
#define  PANEL_REF_CUSTOMER               49      /* control type: textMsg, callback function: (none) */
#define  PANEL_REF_VALEO                  50      /* control type: textMsg, callback function: (none) */
#define  PANEL_LABEL_REF_VALEO            51      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXT1                      52      /* control type: textMsg, callback function: (none) */
#define  PANEL_RESULT_MSG                 53      /* control type: textMsg, callback function: (none) */
#define  PANEL_CIRCLERING                 54      /* control type: pictRing, callback function: (none) */
#define  PANEL_PANELPICTURE               55      /* control type: picture, callback function: (none) */
#define  PANEL_CANVAS                     56      /* control type: canvas, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
