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

#define  L38                             1
#define  L38_PICTURE                     2
#define  L38_C1                          3       /* callback function: OnL38Dmm */
#define  L38_C2                          4       /* callback function: OnL38Dmm */
#define  L38_VBAT                        5       /* callback function: OnL38Dmm */
#define  L38_AC                          6
#define  L38_VCL                         7
#define  L38_RECY2                       8
#define  L38_RECY1                       9
#define  L38_NSF                         10
#define  L38_RDP                         11
#define  L38_K14                         12      /* callback function: OnL38Rele */
#define  L38_K12                         13      /* callback function: OnL38Rele */
#define  L38_K13                         14      /* callback function: OnL38Rele */
#define  L38_K15                         15      /* callback function: OnL38Rele */
#define  L38_K16                         16      /* callback function: OnL38Rele */
#define  L38_K17                         17      /* callback function: OnL38Rele */
#define  L38_K18                         18      /* callback function: OnL38Rele */
#define  L38_K11                         19      /* callback function: OnL38Rele */
#define  L38_K10                         20      /* callback function: OnL38Rele */
#define  L38_K3                          21      /* callback function: OnPower */
#define  L38_K7                          22      /* callback function: OnK7Resistance */
#define  L38_L38_DISCONNECT              23      /* callback function: OnL38Connect */
#define  L38_TIMER                       24      /* callback function: OnL38Timer */
#define  L38_POWER                       25      /* callback function: OnPower */

#define  MEMORY                          2
#define  MEMORY_MEMORY                   2
#define  MEMORY_MEMORY_READ              3       /* callback function: OnMemoryRead */
#define  MEMORY_PICTURE                  4

#define  PANEL                           3
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
#define  PANEL_TEXTMSG_7                 17
#define  PANEL_SEN_READ                  18      /* callback function: OnSenRead */
#define  PANEL_SEN_3                     19
#define  PANEL_SEN_2                     20
#define  PANEL_SEN_1                     21
#define  PANEL_BLOWER_FREQ               22      /* callback function: OnPwm */
#define  PANEL_BLOWER_DUTY               23      /* callback function: OnPwm */
#define  PANEL_PWM_BLOWER                24      /* callback function: OnPwm */
#define  PANEL_KEYBOARD_FREQ             25      /* callback function: OnPwm */
#define  PANEL_KEYBOARD_DUTY             26      /* callback function: OnPwm */
#define  PANEL_PWM_KEYBOARD              27      /* callback function: OnPwm */
#define  PANEL_LED_FREQ                  28      /* callback function: OnPwm */
#define  PANEL_LED_DUTY                  29      /* callback function: OnPwm */
#define  PANEL_PWM_LED                   30      /* callback function: OnPwm */
#define  PANEL_LED_RECY                  31      /* callback function: OnLed */
#define  PANEL_LED_HRW                   32      /* callback function: OnLed */
#define  PANEL_LED_AC                    33      /* callback function: OnLed */
#define  PANEL_L38_CONNECT               34      /* callback function: OnL38Connect */

#define  VLINE                           4
#define  VLINE_TEXTBOX                   2       /* callback function: OnVline */
#define  VLINE_PICTURE                   3


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnK7Resistance(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnL38Connect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnL38Dmm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnL38Rele(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnL38Timer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMemoryRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPower(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSenRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
