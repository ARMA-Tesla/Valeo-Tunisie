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

#define  CAMERA                           1
#define  CAMERA_RECEIVE                   2       /* control type: textBox, callback function: (none) */
#define  CAMERA_SEND                      3       /* control type: textBox, callback function: (none) */
#define  CAMERA_COGNEX_LOGIN_LED          4       /* control type: deco, callback function: (none) */
#define  CAMERA_TCP_SERVER_LED            5       /* control type: deco, callback function: (none) */
#define  CAMERA_PICTURE                   6       /* control type: picture, callback function: (none) */
#define  CAMERA_COGNEX_TEST               7       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_SET_LIVE           8       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_SET_PARAM          9       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_DISCONNECT         10      /* control type: command, callback function: OnCamera */
#define  CAMERA_CLEAR                     11      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_JOB                12      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_OFFLINE            13      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_READ               14      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_LOGIN              15      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_ONLINE             16      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_TRIGGER            17      /* control type: command, callback function: OnCamera */
#define  CAMERA_OPEN_TCP_SERVER           18      /* control type: command, callback function: OnCamera */
#define  CAMERA_STRING                    19      /* control type: string, callback function: (none) */
#define  CAMERA_NUMERIC                   20      /* control type: numeric, callback function: (none) */
#define  CAMERA_LIVE                      21      /* control type: binary, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnCamera(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
