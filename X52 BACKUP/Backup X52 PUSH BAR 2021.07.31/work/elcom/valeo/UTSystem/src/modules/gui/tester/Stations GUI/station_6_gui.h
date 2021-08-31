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

#define  STATION_6                        1
#define  STATION_6_BKTREE                 2       /* control type: activeX, callback function: (none) */
#define  STATION_6_PICTURE                3       /* control type: picture, callback function: (none) */
#define  STATION_6_UNPLUG_VS11            4       /* control type: command, callback function: OnFncStation6 */
#define  STATION_6_UNPLUG_X52             5       /* control type: command, callback function: OnFncStation6 */
#define  STATION_6_LED_UNPLUG_VS11        6       /* control type: deco, callback function: (none) */
#define  STATION_6_LED_UNPLUG_X52         7       /* control type: deco, callback function: (none) */
#define  STATION_6_PLUG_VS11              8       /* control type: command, callback function: OnFncStation6 */
#define  STATION_6_PLUG_X52               9       /* control type: command, callback function: OnFncStation6 */
#define  STATION_6_LED_PLUG_VS11          10      /* control type: deco, callback function: (none) */
#define  STATION_6_LED_PLUG_X52           11      /* control type: deco, callback function: (none) */
#define  STATION_6_SELECT_X52             12      /* control type: command, callback function: OnFncStation6 */
#define  STATION_6_LED_SELECT_X52         13      /* control type: deco, callback function: (none) */
#define  STATION_6_RUN                    14      /* control type: command, callback function: OnFncStation6 */
#define  STATION_6_SELECT_VS11            15      /* control type: command, callback function: OnFncStation6 */
#define  STATION_6_LED_SELECT_VS11        16      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncStation6(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
