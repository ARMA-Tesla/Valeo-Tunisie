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
#define  PANEL_PAUSE_PICT                 2       /* control type: picture, callback function: (none) */
#define  PANEL_PAUSE_START                3       /* control type: numeric, callback function: (none) */
#define  PANEL_TIMER                      4       /* control type: timer, callback function: OnTimerTick */
#define  PANEL_BCKPICT_4                  5       /* control type: picture, callback function: (none) */
#define  PANEL_BCKPICT_1                  6       /* control type: picture, callback function: (none) */
#define  PANEL_BCKPICT_2                  7       /* control type: picture, callback function: (none) */
#define  PANEL_NUM_GOOD                   8       /* control type: textMsg, callback function: (none) */
#define  PANEL_NUM_BAD                    9       /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_BAD                    10      /* control type: textMsg, callback function: (none) */
#define  PANEL_NUM_GOOD_RETEST            11      /* control type: textMsg, callback function: (none) */
#define  PANEL_NUM_BAD_RETEST             12      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXT_MSG_1                 13      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXT_MSG_2                 14      /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_OPERATOR               15      /* control type: textMsg, callback function: (none) */
#define  PANEL_BCKPICT_5                  16      /* control type: picture, callback function: (none) */
#define  PANEL_BCKPICT_6                  17      /* control type: picture, callback function: (none) */
#define  PANEL_KOSU_TIME_2                18      /* control type: textMsg, callback function: (none) */
#define  PANEL_KOSU_TIME                  19      /* control type: textMsg, callback function: (none) */
#define  PANEL_NUM_OPERATOR               20      /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_GOOD                   21      /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_KOSU                   22      /* control type: textMsg, callback function: (none) */
#define  PANEL_PICTURERING                23      /* control type: pictRing, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnTimerTick(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
