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

#define  STATION_1                        1
#define  STATION_1_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_1_PICTURE                3       /* control type: picture, callback function: (none) */
#define  STATION_1_UNLOCK                 4       /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_LED_UNLOCK             5       /* control type: deco, callback function: (none) */
#define  STATION_1_LED_TEST               6       /* control type: deco, callback function: (none) */
#define  STATION_1_RUN                    7       /* control type: command, callback function: OnFncStation1 */
#define  STATION_1_LED_LOCK               8       /* control type: deco, callback function: (none) */
#define  STATION_1_LOCK                   9       /* control type: command, callback function: OnFncStation1 */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation1(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
