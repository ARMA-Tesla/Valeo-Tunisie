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

#define  LASER                            1
#define  LASER_PICTURE                    2       /* control type: picture, callback function: (none) */
#define  LASER_TEXTBOX                    3       /* control type: textBox, callback function: (none) */
#define  LASER_START_LASER                4       /* control type: command, callback function: OnLaser */
#define  LASER_START_LASER_LED            5       /* control type: deco, callback function: (none) */
#define  LASER_MARKER_READY               6       /* control type: command, callback function: OnLaser */
#define  LASER_ENABLE_MARKING             7       /* control type: command, callback function: OnLaser */
#define  LASER_MARKER_READY_LED           8       /* control type: deco, callback function: (none) */
#define  LASER_PROGRAM_RUN_LED            9       /* control type: deco, callback function: (none) */
#define  LASER_CONNECT_LED                10      /* control type: deco, callback function: (none) */
#define  LASER_ENABLE_MARKING_LED         11      /* control type: deco, callback function: (none) */
#define  LASER_DISCONNECT                 12      /* control type: command, callback function: OnLaser */
#define  LASER_CONNECT                    13      /* control type: command, callback function: OnLaser */
#define  LASER_WRITE_STRING               14      /* control type: command, callback function: OnLaser */
#define  LASER_RUN_PROGRAM                15      /* control type: command, callback function: OnLaser */
#define  LASER_BLOCK                      16      /* control type: ring, callback function: (none) */
#define  LASER_PROGRAM                    17      /* control type: ring, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnLaser(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
