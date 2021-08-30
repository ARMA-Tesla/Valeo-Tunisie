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

#define  STATION_2                        1
#define  STATION_2_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_2_PICTURE                3       /* control type: picture, callback function: (none) */
#define  STATION_2_POTENT_OFF             4       /* control type: command, callback function: OnFncStation2 */
#define  STATION_2_POTENT_ON              5       /* control type: command, callback function: OnFncStation2 */
#define  STATION_2_CLIPPAGE_CONTRAST_ON   6       /* control type: command, callback function: OnFncStation2 */
#define  STATION_2_LED_POTENT_ON          7       /* control type: deco, callback function: (none) */
#define  STATION_2_LED_CLIPP_CONTRAS_ON   8       /* control type: deco, callback function: (none) */
#define  STATION_2_CLIPPAGE_CONTRAST_OFF  9       /* control type: command, callback function: OnFncStation2 */
#define  STATION_2_LED_POTENT_OFF         10      /* control type: deco, callback function: (none) */
#define  STATION_2_LED_CLIPP_CONTRAS_OFF  11      /* control type: deco, callback function: (none) */
#define  STATION_2_RUN                    12      /* control type: command, callback function: OnFncStation2 */
#define  STATION_2_READ                   13      /* control type: command, callback function: OnFncStation2 */
#define  STATION_2_CLIPPAGE_PUNZONI_OFF   14      /* control type: command, callback function: OnFncStation2 */
#define  STATION_2_LED_CLIPP_PUNZONI_OFF  15      /* control type: deco, callback function: (none) */
#define  STATION_2_CLIPPAGE_PUNZONI_ON    16      /* control type: command, callback function: OnFncStation2 */
#define  STATION_2_LED_CLIPP_PUNZONI_ON   17      /* control type: deco, callback function: (none) */
#define  STATION_2_INT_7                  18      /* control type: string, callback function: (none) */
#define  STATION_2_INT_6                  19      /* control type: string, callback function: (none) */
#define  STATION_2_INT_5                  20      /* control type: string, callback function: (none) */
#define  STATION_2_INT_4                  21      /* control type: string, callback function: (none) */
#define  STATION_2_INT_3                  22      /* control type: string, callback function: (none) */
#define  STATION_2_EXT_7                  23      /* control type: string, callback function: (none) */
#define  STATION_2_EXT_6                  24      /* control type: string, callback function: (none) */
#define  STATION_2_EXT_5                  25      /* control type: string, callback function: (none) */
#define  STATION_2_EXT_4                  26      /* control type: string, callback function: (none) */
#define  STATION_2_EXT_3                  27      /* control type: string, callback function: (none) */
#define  STATION_2_EXT_2                  28      /* control type: string, callback function: (none) */
#define  STATION_2_EXT_1                  29      /* control type: string, callback function: (none) */
#define  STATION_2_INT_2                  30      /* control type: string, callback function: (none) */
#define  STATION_2_INT_1                  31      /* control type: string, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
