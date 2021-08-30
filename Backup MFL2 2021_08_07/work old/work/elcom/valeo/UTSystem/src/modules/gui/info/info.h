/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2018. All Rights Reserved.          */
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
#define  PANEL_PICTURE_5                  2       /* control type: picture, callback function: (none) */
#define  PANEL_TEXT_ADMIN                 3       /* control type: textMsg, callback function: (none) */
#define  PANEL_PICTURE_3                  4       /* control type: picture, callback function: (none) */
#define  PANEL_TEXT_TIME                  5       /* control type: textMsg, callback function: (none) */
#define  PANEL_PICTURE_2                  6       /* control type: picture, callback function: (none) */
#define  PANEL_TEXT_VERSION               7       /* control type: textMsg, callback function: (none) */
#define  PANEL_PICTURE                    8       /* control type: picture, callback function: (none) */
#define  PANEL_TEXT_SHIFT                 9       /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_ADMIN                  10      /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_SHIFT                  11      /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_VERSION                12      /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_TIME                   13      /* control type: textMsg, callback function: (none) */
#define  PANEL_TIMER                      14      /* control type: timer, callback function: OnTimerTick */
#define  PANEL_PICTURE_4                  15      /* control type: picture, callback function: (none) */
#define  PANEL_MSG_TITLE                  16      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnTimerTick(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
