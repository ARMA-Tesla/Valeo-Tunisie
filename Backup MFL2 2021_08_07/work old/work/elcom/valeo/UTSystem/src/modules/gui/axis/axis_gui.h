/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2019. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  AXIS                             1
#define  AXIS_RECEIVE                     2       /* control type: textBox, callback function: (none) */
#define  AXIS_SEND                        3       /* control type: textBox, callback function: (none) */
#define  AXIS_SPEED                       4       /* control type: numeric, callback function: (none) */
#define  AXIS_TARGET_POSITION             5       /* control type: numeric, callback function: (none) */
#define  AXIS_HOME                        6       /* control type: command, callback function: OnAxis */
#define  AXIS_STOP                        7       /* control type: command, callback function: OnAxis */
#define  AXIS_SET_POSITION                8       /* control type: command, callback function: OnAxis */
#define  AXIS_READ_ANALOG                 9       /* control type: command, callback function: OnAxis */
#define  AXIS_READ_POSITION               10      /* control type: command, callback function: OnAxis */
#define  AXIS_DELETE                      11      /* control type: command, callback function: OnAxis */
#define  AXIS_READ_ALARM                  12      /* control type: command, callback function: OnAxis */
#define  AXIS_RESET_ALARM                 13      /* control type: command, callback function: OnAxis */
#define  AXIS_ENABLE                      14      /* control type: command, callback function: OnAxis */
#define  AXIS_INDEX                       15      /* control type: ring, callback function: OnAxis */
#define  AXIS_STATION                     16      /* control type: ring, callback function: Cb_Station */
#define  AXIS_HOME_LED                    17      /* control type: deco, callback function: (none) */
#define  AXIS_PICTURE                     18      /* control type: picture, callback function: (none) */
#define  AXIS_TEXTMSG                     19      /* control type: textMsg, callback function: (none) */
#define  AXIS_SERVO_ON                    20      /* control type: textButton, callback function: OnAxis */
#define  AXIS_TEXTMSG_2                   21      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Cb_Station(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAxis(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
