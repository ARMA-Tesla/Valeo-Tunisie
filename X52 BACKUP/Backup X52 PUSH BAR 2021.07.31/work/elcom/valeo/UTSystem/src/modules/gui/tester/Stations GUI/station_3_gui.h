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

#define  STATION_3                        1
#define  STATION_3_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_3_PICTURE                3       /* control type: picture, callback function: (none) */
#define  STATION_3_RODAGE_UP              4       /* control type: command, callback function: OnFncStation3 */
#define  STATION_3_RUN                    5       /* control type: command, callback function: OnFncStation3 */
#define  STATION_3_RODAGE_DOWN            6       /* control type: command, callback function: OnFncStation3 */
#define  STATION_3_START_RODAGE           7       /* control type: command, callback function: OnFncStation3 */
#define  STATION_3_TIME_OFF               8       /* control type: numeric, callback function: (none) */
#define  STATION_3_TIME_ON                9       /* control type: numeric, callback function: (none) */
#define  STATION_3_CYCLE                  10      /* control type: numeric, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_9          11      /* control type: radioButton, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_8          12      /* control type: radioButton, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_7          13      /* control type: radioButton, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_6          14      /* control type: radioButton, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_5          15      /* control type: radioButton, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_4          16      /* control type: radioButton, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_3          17      /* control type: radioButton, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_2          18      /* control type: radioButton, callback function: (none) */
#define  STATION_3_ENABLE_PUSH_1          19      /* control type: radioButton, callback function: (none) */
#define  STATION_3_RODAGE_UP_LED          20      /* control type: deco, callback function: (none) */
#define  STATION_3_LED_RODAGE             21      /* control type: deco, callback function: (none) */
#define  STATION_3_RODAGE_DOWN_LED        22      /* control type: deco, callback function: (none) */
#define  STATION_3_TEXTMSG                23      /* control type: textMsg, callback function: (none) */
#define  STATION_3_TEXTMSG_2              24      /* control type: textMsg, callback function: (none) */
#define  STATION_3_TEXTMSG_3              25      /* control type: textMsg, callback function: (none) */
#define  STATION_3_TEXTMSG_4              26      /* control type: textMsg, callback function: (none) */
#define  STATION_3_TEXTMSG_8              27      /* control type: textMsg, callback function: (none) */
#define  STATION_3_TEXTMSG_7              28      /* control type: textMsg, callback function: (none) */
#define  STATION_3_TEXTMSG_6              29      /* control type: textMsg, callback function: (none) */
#define  STATION_3_TEXTMSG_9              30      /* control type: textMsg, callback function: (none) */
#define  STATION_3_TEXTMSG_5              31      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation3(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
