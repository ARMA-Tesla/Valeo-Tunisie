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

#define  STATION_8                        1
#define  STATION_8_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_8_PICTURE                3       /* control type: picture, callback function: (none) */
#define  STATION_8_CLOSE_PINCE            4       /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_BARCODE                5       /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_MOVE_X_ARRIERE         6       /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_MOVE_X_AVANT           7       /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_CODE_STRING            8       /* control type: string, callback function: (none) */
#define  STATION_8_LED_MOVE_REAR          9       /* control type: deco, callback function: (none) */
#define  STATION_8_OPEN_PINCE             10      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_LED_MOVE_FRONT         11      /* control type: deco, callback function: (none) */
#define  STATION_8_LOCK                   12      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_MOVE_Z_UP              13      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_UNLOCK                 14      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_MOVE_Z_DOWN            15      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_RESET_PICK_PLACE       16      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_PICK_PLACE             17      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_LED_OPEN_PINCE         18      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_RESET_PICK_PLACE   19      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_PICK_PLACE         20      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_CLOSE_PINCE        21      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_LOCK               22      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_Z_DOWN             23      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_UNLOCK             24      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_Z_UP               25      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation8(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
