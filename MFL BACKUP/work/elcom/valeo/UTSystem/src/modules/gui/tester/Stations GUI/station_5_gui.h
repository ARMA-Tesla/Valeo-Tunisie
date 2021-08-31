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

#define  STATION_5                        1
#define  STATION_5_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_5_PICTURE                3       /* control type: picture, callback function: (none) */
#define  STATION_5_RODAGE_UP_LEFT         4       /* control type: command, callback function: OnFncStation5 */
#define  STATION_5_RODAGE_DOWN_LEFT       5       /* control type: command, callback function: OnFncStation5 */
#define  STATION_5_RODAGE_DOWN_LED_LEFT   6       /* control type: deco, callback function: (none) */
#define  STATION_5_RODAGE_UP_LED_LEFT     7       /* control type: deco, callback function: (none) */
#define  STATION_5_RODAGE_UP_RIGHT        8       /* control type: command, callback function: OnFncStation5 */
#define  STATION_5_RODAGE_DOWN_RIGHT      9       /* control type: command, callback function: OnFncStation5 */
#define  STATION_5_RUN                    10      /* control type: command, callback function: OnFncStation5 */
#define  STATION_5_LED_TEST               11      /* control type: deco, callback function: (none) */
#define  STATION_5_RODAGE_DOWN_LED_RIGHT  12      /* control type: deco, callback function: (none) */
#define  STATION_5_RODAGE_UP_LED_RIGHT    13      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation5(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
