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

#define  STATION_7                        1
#define  STATION_7_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_7_PICTURE                3       /* control type: picture, callback function: (none) */
#define  STATION_7_PRINT                  4       /* control type: command, callback function: OnFncStation7 */
#define  STATION_7_READY                  5       /* control type: command, callback function: OnFncStation8 */
#define  STATION_7_LED_PRINT              6       /* control type: deco, callback function: (none) */
#define  STATION_7_LED_READY              7       /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation7(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);

#ifdef __cplusplus
    }
#endif
