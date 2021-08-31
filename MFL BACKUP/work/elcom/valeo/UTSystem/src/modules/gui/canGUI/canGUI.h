/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2017. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  CANGUI                           1
#define  CANGUI_START                     2       /* control type: pictButton, callback function: OnRecChanged */
#define  CANGUI_STOP                      3       /* control type: pictButton, callback function: OnRecChanged */
#define  CANGUI_CLEAR                     4       /* control type: pictButton, callback function: OnClear */
#define  CANGUI_PICTURE                   5       /* control type: picture, callback function: (none) */
#define  CANGUI_TAB                       6       /* control type: tab, callback function: (none) */
#define  CANGUI_COMM_LIST                 7       /* control type: textBox, callback function: (none) */

     /* tab page panel controls */
#define  FILTER_EXCLUDE                   2       /* control type: string, callback function: (none) */
#define  FILTER_INCLUDE                   3       /* control type: string, callback function: (none) */
#define  FILTER_EXC_ENABLE                4       /* control type: radioButton, callback function: (none) */
#define  FILTER_INC_ENABLE                5       /* control type: radioButton, callback function: (none) */
#define  FILTER_APPLY                     6       /* control type: command, callback function: OnApply */
#define  FILTER_TEXTMSG                   7       /* control type: textMsg, callback function: (none) */
#define  FILTER_PICTURE                   8       /* control type: picture, callback function: (none) */
#define  FILTER_TEXTMSG_2                 9       /* control type: textMsg, callback function: (none) */

     /* tab page panel controls */
#define  KWPPANEL_CHANNEL                 2       /* control type: ring, callback function: (none) */
#define  KWPPANEL_WRITE_ID                3       /* control type: numeric, callback function: (none) */
#define  KWPPANEL_READ_ID                 4       /* control type: numeric, callback function: (none) */
#define  KWPPANEL_CMD                     5       /* control type: ring, callback function: OnCmdChanged */
#define  KWPPANEL_SIZE                    6       /* control type: numeric, callback function: OnSizeChange */
#define  KWPPANEL_TABLE                   7       /* control type: table, callback function: (none) */
#define  KWPPANEL_SEND                    8       /* control type: command, callback function: OnSend1 */

     /* tab page panel controls */
#define  LOWPANEL_WRITE_SIZE              2       /* control type: numeric, callback function: (none) */
#define  LOWPANEL_WRITE_ID                3       /* control type: numeric, callback function: (none) */
#define  LOWPANEL_TABLE                   4       /* control type: table, callback function: (none) */
#define  LOWPANEL_LIST                    5       /* control type: table, callback function: (none) */
#define  LOWPANEL_ASYNC                   6       /* control type: radioButton, callback function: OnLoopEnabled */
#define  LOWPANEL_ADD                     7       /* control type: command, callback function: OnLoopList */
#define  LOWPANEL_REMOVE                  8       /* control type: command, callback function: OnLoopList */
#define  LOWPANEL_MULT                    9       /* control type: command, callback function: OnMultEnabled */
#define  LOWPANEL_REMOVE_2                10      /* control type: command, callback function: OnLoopList */
#define  LOWPANEL_ADD_2                   11      /* control type: command, callback function: OnLoopList */
#define  LOWPANEL_SEND                    12      /* control type: command, callback function: OnSend2 */
#define  LOWPANEL_LIST_2                  13      /* control type: table, callback function: (none) */
#define  LOWPANEL_CHANNEL                 14      /* control type: ring, callback function: OnChannelChanged */
#define  LOWPANEL_PACKET_TYPE             15      /* control type: ring, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnApply(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnChannelChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnClear(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCmdChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoopEnabled(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoopList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMultEnabled(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnRecChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSend1(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSend2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSizeChange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
