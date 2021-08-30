/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2019. All Rights Reserved.          */
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
#define  STATION_8_BARCODE_RIGHT          4       /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_BARCODE                5       /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_BARCODE_LEFT           6       /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_LED_TEST               7       /* control type: deco, callback function: (none) */
#define  STATION_8_RUN                    8       /* control type: command, callback function: OnFncStation1 */
#define  STATION_8_CODE_TOP_2             9       /* control type: command, callback function: OnFncStation1 */
#define  STATION_8_CODE_TOP               10      /* control type: command, callback function: OnFncStation1 */
#define  STATION_8_CODE_STRING_TOP_2      11      /* control type: string, callback function: (none) */
#define  STATION_8_MOVE_BARCODE_REAR      12      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_MOVE_BARCODE_FRONT     13      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_LED_MOVE_REAR          14      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_MOVE_FRONT         15      /* control type: deco, callback function: (none) */
#define  STATION_8_UNLOCK                 16      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_LOCK                   17      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_LASER                  18      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_PICK_PLACE_RESET       19      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_PICK_PLACE             20      /* control type: command, callback function: OnFncStation8 */
#define  STATION_8_LED_UNLOCK             21      /* control type: deco, callback function: (none) */
#define  STATION_8_CODE_STRING_TOP        22      /* control type: string, callback function: (none) */
#define  STATION_8_LED_LOCK               23      /* control type: deco, callback function: (none) */
#define  STATION_8_LED_LASER              24      /* control type: deco, callback function: (none) */
#define  STATION_8_CODE_STRING            25      /* control type: string, callback function: (none) */
#define  STATION_8_LED_PICK_PLACE_RESET   26      /* control type: deco, callback function: (none) */
#define  STATION_8_CODE_STRING_LEFT       27      /* control type: string, callback function: (none) */
#define  STATION_8_CODE_STRING_RIGHT      28      /* control type: string, callback function: (none) */
#define  STATION_8_LED_PICK_PLACE         29      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation1(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFncStation8(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
