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

#define  STATION_1                        1
#define  STATION_1_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_1_CODE_INSIDE            3       /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_CODE_OUTSIDE           4       /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_LED_LOCK               5       /* control type: deco, callback function: (none) */
#define  STATION_1_LED_UNLOCK             6       /* control type: deco, callback function: (none) */
#define  STATION_1_CODE_STRING_OUTSIDE    7       /* control type: string, callback function: (none) */
#define  STATION_1_CODE_STRING_INSIDE     8       /* control type: string, callback function: (none) */
#define  STATION_1_UNLOCK                 9       /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_LOCK                   10      /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_PRODUCT_PRESENCE       11      /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_PRODUCT_TYPE           12      /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_RUN                    13      /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_TEXTMSG_4              14      /* control type: textMsg, callback function: (none) */
#define  STATION_1_TEXTMSG_5              15      /* control type: textMsg, callback function: (none) */
#define  STATION_1_LED_PRODUCT_PRESENT    16      /* control type: deco, callback function: (none) */
#define  STATION_1_LED_PRODUCT_ABSENT     17      /* control type: deco, callback function: (none) */
#define  STATION_1_LED_TYPE_UNKNOWN       18      /* control type: deco, callback function: (none) */
#define  STATION_1_TEXTMSG_3              19      /* control type: textMsg, callback function: (none) */
#define  STATION_1_SENSOR_RIGHT           20      /* control type: string, callback function: (none) */
#define  STATION_1_PICTURE                21      /* control type: picture, callback function: (none) */
#define  STATION_1_TEXTMSG                22      /* control type: textMsg, callback function: (none) */
#define  STATION_1_TEXTMSG_6              23      /* control type: textMsg, callback function: (none) */
#define  STATION_1_TEXTMSG_2              24      /* control type: textMsg, callback function: (none) */
#define  STATION_1_LED_TYPE_X52_7         25      /* control type: deco, callback function: (none) */
#define  STATION_1_LED_TYPE_X52_9         26      /* control type: deco, callback function: (none) */
#define  STATION_1_SENSOR_LEFT            27      /* control type: string, callback function: (none) */
#define  STATION_1_LED_TYPE_VS11          28      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation1(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
