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

#define  CAMERA                           1
#define  CAMERA_RECEIVE                   2       /* control type: textBox, callback function: (none) */
#define  CAMERA_SEND                      3       /* control type: textBox, callback function: (none) */
#define  CAMERA_COGNEX_DISCONNECT         4       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_SELECT_PRODUCT     5       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_SELECT_PICTO       6       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_OFFLINE            7       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_LOGIN              8       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_SET_PARAM          9       /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_ACT_JOB            10      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_ONLINE             11      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_READ               12      /* control type: command, callback function: OnCamera */
#define  CAMERA_CLEAR                     13      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_TRIGGER            14      /* control type: command, callback function: OnCamera */
#define  CAMERA_OPEN_TCP_SERVER           15      /* control type: command, callback function: OnCamera */
#define  CAMERA_COGNEX_LOGIN_LED          16      /* control type: deco, callback function: (none) */
#define  CAMERA_PICTO_8                   17      /* control type: ring, callback function: (none) */
#define  CAMERA_PICTO_7                   18      /* control type: ring, callback function: (none) */
#define  CAMERA_PICTO_6                   19      /* control type: ring, callback function: (none) */
#define  CAMERA_PICTO_5                   20      /* control type: ring, callback function: (none) */
#define  CAMERA_PICTO_4                   21      /* control type: ring, callback function: (none) */
#define  CAMERA_PICTO_3                   22      /* control type: ring, callback function: (none) */
#define  CAMERA_PICTO_2                   23      /* control type: ring, callback function: (none) */
#define  CAMERA_PICTO_1                   24      /* control type: ring, callback function: (none) */
#define  CAMERA_PRODUCT                   25      /* control type: ring, callback function: OnCamera */
#define  CAMERA_TCP_SERVER_LED            26      /* control type: deco, callback function: (none) */
#define  CAMERA_PICTURE                   27      /* control type: picture, callback function: (none) */
#define  CAMERA_NUMERIC                   28      /* control type: numeric, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnCamera(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
