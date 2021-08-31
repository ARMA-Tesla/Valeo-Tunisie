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

#define  STATION_7                        1
#define  STATION_7_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_7_PICTURE                3       /* control type: picture, callback function: (none) */
#define  STATION_7_UNPLUG                 4       /* control type: command, callback function: OnFncStation7 */
#define  STATION_7_LED_UNPLUG             5       /* control type: deco, callback function: (none) */
#define  STATION_7_LED_STA                6       /* control type: deco, callback function: (none) */
#define  STATION_7_TEST_LED_STA           7       /* control type: command, callback function: OnFncStation7 */
#define  STATION_7_LED_BG                 8       /* control type: deco, callback function: (none) */
#define  STATION_7_TEST_LED_BG            9       /* control type: command, callback function: OnFncStation7 */
#define  STATION_7_LED_TEST               10      /* control type: deco, callback function: (none) */
#define  STATION_7_RUN                    11      /* control type: command, callback function: OnFncStation7 */
#define  STATION_7_PLUG                   12      /* control type: command, callback function: OnFncStation7 */
#define  STATION_7_LED_PLUG               13      /* control type: deco, callback function: (none) */
#define  STATION_7_LIGHT                  14      /* control type: binary, callback function: OnFncStation7 */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation7(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
