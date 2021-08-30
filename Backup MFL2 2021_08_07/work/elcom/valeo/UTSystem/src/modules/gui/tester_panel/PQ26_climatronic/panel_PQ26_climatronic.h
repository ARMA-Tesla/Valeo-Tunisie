/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2013. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  MEMORY                           1
#define  MEMORY_MEMORY                    2       /* control type: textBox, callback function: (none) */
#define  MEMORY_MEMORY_READ               3       /* control type: command, callback function: OnMemoryRead */
#define  MEMORY_PICTURE                   4       /* control type: picture, callback function: (none) */
#define  MEMORY_LENGTH                    5       /* control type: numeric, callback function: (none) */
#define  MEMORY_START_ADDRESS             6       /* control type: numeric, callback function: (none) */

#define  PANEL                            2
#define  PANEL_DECORATION_6               2       /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_9               3       /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_8               4       /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_7               5       /* control type: deco, callback function: (none) */
#define  PANEL_PICTURE_2                  6       /* control type: picture, callback function: (none) */
#define  PANEL_TEXTMSG_29                 7       /* control type: textMsg, callback function: (none) */
#define  PANEL_LED_AUTO                   8       /* control type: toggle, callback function: OnLed */
#define  PANEL_CONNECT_CAN                9       /* control type: textButton, callback function: OnConnect */
#define  PANEL_LED_MAX                    10      /* control type: toggle, callback function: OnLed */
#define  PANEL_TEXTMSG_8                  11      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION_4               12      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_6                  13      /* control type: textMsg, callback function: (none) */
#define  PANEL_LED_ATS                    14      /* control type: toggle, callback function: OnLed */
#define  PANEL_LED_ATF                    15      /* control type: toggle, callback function: OnLed */
#define  PANEL_TEXTMSG_4                  16      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_3                  17      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_7                  18      /* control type: textMsg, callback function: (none) */
#define  PANEL_LED_ATP                    19      /* control type: toggle, callback function: OnLed */
#define  PANEL_LED_RECY                   20      /* control type: toggle, callback function: OnLed */
#define  PANEL_LED_AC                     21      /* control type: toggle, callback function: OnLed */
#define  PANEL_PICT_AC                    22      /* control type: picture, callback function: (none) */
#define  PANEL_PICT_RECY                  23      /* control type: picture, callback function: (none) */
#define  PANEL_PICT_AUTO                  24      /* control type: picture, callback function: (none) */
#define  PANEL_SEGMENT_5                  25      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_PICT_ATF                   26      /* control type: picture, callback function: (none) */
#define  PANEL_PICT_ATP                   27      /* control type: picture, callback function: (none) */
#define  PANEL_SEN_READ                   28      /* control type: command, callback function: OnSenRead */
#define  PANEL_PICT_ATS                   29      /* control type: picture, callback function: (none) */
#define  PANEL_PICT_MAX                   30      /* control type: picture, callback function: (none) */
#define  PANEL_TEMP2_PICT                 31      /* control type: picture, callback function: (none) */
#define  PANEL_BLOW2_PICT                 32      /* control type: picture, callback function: (none) */
#define  PANEL_DC_MOTOR_EN_CSN            33      /* control type: textButton, callback function: OnDcMotorsSignals */
#define  PANEL_SEGMENT_6                  34      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_7                  35      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_8                  36      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_4                  37      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_3                  38      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_TEMP1_PICT                 39      /* control type: picture, callback function: (none) */
#define  PANEL_SEGMENT_2                  40      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_BLOW1_PICT                 41      /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE                    42      /* control type: picture, callback function: (none) */
#define  PANEL_SEGMENT_1                  43      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_TEXTMSG_2                  44      /* control type: textMsg, callback function: (none) */
#define  PANEL_BACKLIGHT_TELL_FREQ        45      /* control type: numeric, callback function: OnPwm */
#define  PANEL_BACKLIGHT_BLUE_FREQ        46      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_BACKLIGHT_TELL         47      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_BACKLIGHT_RED_FREQ         48      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_BACKLIGHT_BLUE         49      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_BACKLIGHT_PUSH_FREQ        50      /* control type: numeric, callback function: OnPwm */
#define  PANEL_BACKLIGHT_TELL_DUTY        51      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_BACKLIGHT_RED          52      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_BACKLIGHT_TEMP_FREQ        53      /* control type: numeric, callback function: OnPwm */
#define  PANEL_BACKLIGHT_BLUE_DUTY        54      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_BACKLIGHT_PUSH         55      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_BACKLIGHT_BLOWER_FREQ      56      /* control type: numeric, callback function: OnPwm */
#define  PANEL_BACKLIGHT_RED_DUTY         57      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_BACKLIGHT_TEMP         58      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_LCD_FREQ                   59      /* control type: numeric, callback function: OnPwm */
#define  PANEL_BACKLIGHT_PUSH_DUTY        60      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_BACKLIGHT_BLOWER       61      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_BACKLIGHT_TEMP_DUTY        62      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_LCD                    63      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_BACKLIGHT_BLOWER_DUTY      64      /* control type: numeric, callback function: OnPwm */
#define  PANEL_BLOWER_FREQ                65      /* control type: numeric, callback function: OnPwm */
#define  PANEL_LCD_DUTY                   66      /* control type: numeric, callback function: OnPwm */
#define  PANEL_COMPRESSOR_FREQ            67      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_BLOWER                 68      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_BUTTON_READ                69      /* control type: radioButton, callback function: OnButtonRead */
#define  PANEL_DISPLAY                    70      /* control type: textButton, callback function: OnDisplay */
#define  PANEL_MODE                       71      /* control type: ring, callback function: OnMode */
#define  PANEL_PWM_COMPRESSOR             72      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_BLOWER_DUTY                73      /* control type: numeric, callback function: OnPwm */
#define  PANEL_COMPRESSOR_DUTY            74      /* control type: numeric, callback function: OnPwm */
#define  PANEL_ANA_FEEDBACK_MOINS         75      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_19                 76      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_FEEDBACK_PLUS          77      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_18                 78      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_PANEL_OUTLET           79      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_17                 80      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_FOOT_OUTLET            81      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_16                 82      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_EVAPORATOR             83      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_15                 84      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_BLOWER                 85      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_31                 86      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_COMPRESSOR_I           87      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_30                 88      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_ZEBRA_TEST             89      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_28                 90      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_5V_POTI                91      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_27                 92      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_VBAT_MEASURE           93      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_26                 94      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_22                 95      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_PCB_TEMP               96      /* control type: numeric, callback function: (none) */
#define  PANEL_ANA_RECI_POTI              97      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_25                 98      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_21                 99      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_INCAR_SENSOR           100     /* control type: numeric, callback function: (none) */
#define  PANEL_ANA_DEFROST_POTI           101     /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_24                 102     /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_20                 103     /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_TEMP_POTI              104     /* control type: numeric, callback function: (none) */
#define  PANEL_ANA_SUN_SENSOR             105     /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_23                 106     /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_14                 107     /* control type: textMsg, callback function: (none) */
#define  PANEL_ANA_DISTRI_POTI            108     /* control type: numeric, callback function: (none) */
#define  PANEL_VER                        109     /* control type: string, callback function: (none) */

#define  VLINE                            3
#define  VLINE_TEXTBOX                    2       /* control type: textBox, callback function: OnVline */
#define  VLINE_PICTURE                    3       /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnButtonRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDcMotorsSignals(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDisplay(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMemoryRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSenRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
