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
#define  PANEL_CONNECT                   12      /* callback function: OnConnect */
#define  PANEL_TEXTMSG_5                 13
#define  PANEL_TEXTMSG_2                 14
#define  PANEL_DECORATION_7              15
#define  PANEL_MODE                      16      /* callback function: OnMode */
#define  PANEL_LED_FAST                  17      /* callback function: OnLed */
#define  PANEL_TEXTMSG_7                 18
#define  PANEL_LED_AUTO                  19      /* callback function: OnLed */
#define  PANEL_LED_SOFT                  20      /* callback function: OnLed */
#define  PANEL_LED_AC                    21      /* callback function: OnLed */
#define  PANEL_LED_HRW                   22      /* callback function: OnLed */
#define  PANEL_LED_DEFROST               23      /* callback function: OnLed */
#define  PANEL_SEN_READ                  24      /* callback function: OnSenRead */
#define  PANEL_LED_RECY                  25      /* callback function: OnLed */
#define  PANEL_SEN_4                     26
#define  PANEL_SEN_3                     27
#define  PANEL_SEN_2                     28
#define  PANEL_SEN_1                     29
#define  PANEL_LED_FREQ                  30      /* callback function: OnPwm16 */
#define  PANEL_LCD_FREQ                  31      /* callback function: OnPwm16 */
#define  PANEL_SYMBOL_FREQ               32      /* callback function: OnPwm16 */
#define  PANEL_AUTO_FREQ                 33      /* callback function: OnPwm16 */
#define  PANEL_BLOWER_FREQ               34      /* callback function: OnPwm */
#define  PANEL_SEGMENT_5                 35      /* callback function: OnDisplay */
#define  PANEL_SEGMENT_6                 36      /* callback function: OnDisplay */
#define  PANEL_SEGMENT_7                 37      /* callback function: OnDisplay */
#define  PANEL_LED_DUTY                  38      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_8                 39      /* callback function: OnDisplay */
#define  PANEL_LCD_DUTY                  40      /* callback function: OnPwm16 */
#define  PANEL_PWM_LED                   41      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_4                 42      /* callback function: OnDisplay */
#define  PANEL_SYMBOL_DUTY               43      /* callback function: OnPwm16 */
#define  PANEL_PWM_LCD                   44      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_3                 45      /* callback function: OnDisplay */
#define  PANEL_AUTO_DUTY                 46      /* callback function: OnPwm16 */
#define  PANEL_PWM_SYMBOL                47      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_2                 48      /* callback function: OnDisplay */
#define  PANEL_BLOWER_DUTY               49      /* callback function: OnPwm */
#define  PANEL_PWM_AUTO                  50      /* callback function: OnPwm16 */
#define  PANEL_SEGMENT_1                 51      /* callback function: OnDisplay */
#define  PANEL_DISPLAY                   52      /* callback function: OnDisplay */
#define  PANEL_PWM_BLOWER                53      /* callback function: OnPwm */

#define  VLINE                           3
#define  VLINE_TEXTBOX                   2       /* callback function: OnVline */
#define  VLINE_PICTURE                   3


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDisplay(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMemoryRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm16(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSenRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
