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

#define  ST_3_FORCE                       1
#define  ST_3_FORCE_PICTURE               2       /* control type: picture, callback function: (none) */
#define  ST_3_FORCE_DISPLAY               3       /* control type: ring, callback function: OnDisplay_St3 */

#define  ST_3_IO                          2
#define  ST_3_IO_BKTREE                   2       /* control type: activeX, callback function: (none) */
#define  ST_3_IO_PICTURE                  3       /* control type: picture, callback function: (none) */
#define  ST_3_IO_DISPLAY                  4       /* control type: ring, callback function: OnDisplay_St3 */
#define  ST_3_IO_UNPLUG                   5       /* control type: command, callback function: OnFncStation3 */
#define  ST_3_IO_LED_TEST                 6       /* control type: deco, callback function: (none) */
#define  ST_3_IO_RUN                      7       /* control type: command, callback function: OnFncStation3 */
#define  ST_3_IO_LED_UNPLUG               8       /* control type: deco, callback function: (none) */
#define  ST_3_IO_PLUG                     9       /* control type: command, callback function: OnFncStation3 */
#define  ST_3_IO_LED_PLUG                 10      /* control type: deco, callback function: (none) */

#define  ST_3_LIN                         3
#define  ST_3_LIN_PICTURE                 2       /* control type: picture, callback function: (none) */
#define  ST_3_LIN_DISPLAY                 3       /* control type: ring, callback function: OnDisplay_St3 */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnDisplay_St3(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFncStation3(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
