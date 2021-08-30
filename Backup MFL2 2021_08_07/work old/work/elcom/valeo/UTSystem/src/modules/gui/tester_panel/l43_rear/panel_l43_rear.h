/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2008. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  MEMORY                          1
#define  MEMORY_MEMORY                   2
#define  MEMORY_MEMORY_READ              3       /* callback function: OnMemoryRead */
#define  MEMORY_PICTURE                  4

#define  PANEL                           2
#define  PANEL_DECORATION_6              2
#define  PANEL_DECORATION_5              3
#define  PANEL_PICTURE_2                 4
#define  PANEL_PICTURE                   5
#define  PANEL_TEXTMSG_8                 6
#define  PANEL_DECORATION_3              7
#define  PANEL_TEXTMSG_4                 8
#define  PANEL_TEXTMSG_3                 9
#define  PANEL_DECORATION_4              10
#define  PANEL_TEXTMSG_6                 11
#define  PANEL_CONNECT_2                 12      /* callback function: OnConnect */
#define  PANEL_CONNECT                   13      /* callback function: OnConnect */
#define  PANEL_TEXTMSG_5                 14
#define  PANEL_TEXTMSG_2                 15
#define  PANEL_DECORATION_7              16
#define  PANEL_MODE                      17      /* callback function: OnMode */
#define  PANEL_TEXTMSG_7                 18
#define  PANEL_SEN_READ                  19      /* callback function: OnSenRead */
#define  PANEL_SEN_4                     20
#define  PANEL_LCD_FREQ                  21      /* callback function: OnPwm16 */
#define  PANEL_RED_FREQ                  22      /* callback function: OnPwm16 */
#define  PANEL_BLUE_FREQ                 23      /* callback function: OnPwm16 */
#define  PANEL_LCD_DUTY                  24      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_4                 25      /* callback function: OnDisplay */
#define  PANEL_PWM_LCD                   26      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_3                 27      /* callback function: OnDisplay */
#define  PANEL_PWM_RED                   28      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_2                 29      /* callback function: OnDisplay */
#define  PANEL_PWM_BLUE                  30      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_1                 31      /* callback function: OnDisplay */
#define  PANEL_DISPLAY                   32      /* callback function: OnDisplay */
#define  PANEL_RED_DUTY                  33      /* callback function: OnPwm16 */
#define  PANEL_BLUE_DUTY                 34      /* callback function: OnPwm16 */

#define  VLINE                           3
#define  VLINE_TEXTBOX                   2       /* callback function: OnVline */
#define  VLINE_PICTURE                   3


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDisplay(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMemoryRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm16(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSenRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
