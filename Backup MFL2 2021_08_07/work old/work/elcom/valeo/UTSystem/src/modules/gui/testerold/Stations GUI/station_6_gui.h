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

#define  ST_6_FORCE                       1
#define  ST_6_FORCE_PICTURE               2       /* control type: picture, callback function: (none) */
#define  ST_6_FORCE_DISPLAY               3       /* control type: ring, callback function: OnDisplay_St6 */
#define  ST_6_FORCE_UNPLUG                4       /* control type: command, callback function: OnFncStation3 */
#define  ST_6_FORCE_LED_UNPLUG            5       /* control type: deco, callback function: (none) */
#define  ST_6_FORCE_LED_PLUG              6       /* control type: deco, callback function: (none) */
#define  ST_6_FORCE_PLUG                  7       /* control type: command, callback function: OnFncStation3 */

#define  ST_6_IO                          2
#define  ST_6_IO_BKTREE                   2       /* control type: activeX, callback function: (none) */
#define  ST_6_IO_PICTURE                  3       /* control type: picture, callback function: (none) */
#define  ST_6_IO_TEST_DOWN_RIGHT          4       /* control type: command, callback function: OnFncStation6 */
#define  ST_6_IO_UNPLUG                   5       /* control type: command, callback function: OnFncStation6 */
#define  ST_6_IO_LED_UNPLUG               6       /* control type: deco, callback function: (none) */
#define  ST_6_IO_PLUG                     7       /* control type: command, callback function: OnFncStation6 */
#define  ST_6_IO_LED_PLUG                 8       /* control type: deco, callback function: (none) */
#define  ST_6_IO_TEST_DOWN_LED_RIGHT      9       /* control type: deco, callback function: (none) */
#define  ST_6_IO_RUN                      10      /* control type: command, callback function: OnFncStation6 */
#define  ST_6_IO_LED_TEST                 11      /* control type: deco, callback function: (none) */
#define  ST_6_IO_TEST_UP_RIGHT            12      /* control type: command, callback function: OnFncStation6 */
#define  ST_6_IO_TEST_UP_LED_RIGHT        13      /* control type: deco, callback function: (none) */
#define  ST_6_IO_TEST_DOWN_LEFT           14      /* control type: command, callback function: OnFncStation6 */
#define  ST_6_IO_TEST_DOWN_LED_LEFT       15      /* control type: deco, callback function: (none) */
#define  ST_6_IO_TEST_UP_LEFT             16      /* control type: command, callback function: OnFncStation6 */
#define  ST_6_IO_TEST_UP_LED_LEFT         17      /* control type: deco, callback function: (none) */
#define  ST_6_IO_DISPLAY                  18      /* control type: ring, callback function: OnDisplay_St6 */

#define  ST_6_LIN                         3
#define  ST_6_LIN_PICTURE                 2       /* control type: picture, callback function: (none) */
#define  ST_6_LIN_DISPLAY                 3       /* control type: ring, callback function: OnDisplay_St6 */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnDisplay_St6(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFncStation3(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFncStation6(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
