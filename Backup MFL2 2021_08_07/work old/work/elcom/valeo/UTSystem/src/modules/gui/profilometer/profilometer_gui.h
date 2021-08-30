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

#define  PROFILO                          1
#define  PROFILO_PICTURE                  2       /* control type: picture, callback function: (none) */
#define  PROFILO_LOG                      3       /* control type: textBox, callback function: (none) */
#define  PROFILO_CLEAR                    4       /* control type: command, callback function: OnProfilometer */
#define  PROFILO_MEASURE                  5       /* control type: command, callback function: OnProfilometer */
#define  PROFILO_STOP                     6       /* control type: command, callback function: OnProfilometer */
#define  PROFILO_START                    7       /* control type: command, callback function: OnProfilometer */
#define  PROFILO_DEVICE_ID                8       /* control type: ring, callback function: (none) */
#define  PROFILO_NUMERIC_OUT1             9       /* control type: string, callback function: (none) */
#define  PROFILO_LED_LOW_OUT_1            10      /* control type: LED, callback function: (none) */
#define  PROFILO_LED_GO_OUT_1             11      /* control type: LED, callback function: (none) */
#define  PROFILO_LED_HIGH_OUT_1           12      /* control type: LED, callback function: (none) */
#define  PROFILO_NUMERIC_OUT0             13      /* control type: string, callback function: (none) */
#define  PROFILO_LED_LOW_OUT_0            14      /* control type: LED, callback function: (none) */
#define  PROFILO_LED_GO_OUT_0             15      /* control type: LED, callback function: (none) */
#define  PROFILO_LED_HIGH_OUT_0           16      /* control type: LED, callback function: (none) */
#define  PROFILO_LED_ALARM_OUT_1          17      /* control type: LED, callback function: (none) */
#define  PROFILO_LED_ALARM_OUT_0          18      /* control type: LED, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnProfilometer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
