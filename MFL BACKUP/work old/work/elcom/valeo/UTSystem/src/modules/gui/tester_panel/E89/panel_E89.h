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

#define  PE89                            2
#define  PE89_DECORATION_10              2
#define  PE89_DECORATION_9               3
#define  PE89_DECORATION_8               4
#define  PE89_DECORATION_7               5
#define  PE89_TEXTMSG_30                 6
#define  PE89_TEXTMSG_29                 7
#define  PE89_TEXTMSG_8                  8
#define  PE89_CONNECT_CAN                9       /* callback function: OnConnectCan */
#define  PE89_CONNECT_KLINE              10      /* callback function: OnConnect */
#define  PE89_BUTTON_READ                11      /* callback function: OnButtonRead */
#define  PE89_PTYPE                      12      /* callback function: OnProductType */
#define  PE89_MODE                       13      /* callback function: OnMode */
#define  PE89_VER                        14
#define  PE89_RR_TURN_RIGHT              15
#define  PE89_TEXTMSG_4                  16
#define  PE89_TEXTMSG_3                  17
#define  PE89_RR_TURN_LEFT               18
#define  PE89_LR_TURN_RIGHT              19
#define  PE89_BTN_MAX                    20
#define  PE89_BTN_RECI                   21
#define  PE89_BTN_DIST                   22
#define  PE89_BTN_AC                     23
#define  PE89_BTN_HFS                    24
#define  PE89_BTN_FAN_DOWN               25
#define  PE89_BTN_FAN_UP                 26
#define  PE89_VOLTAGE_READ_2             27      /* callback function: OnVoltageRead */
#define  PE89_SEN_READ                   28      /* callback function: OnSenRead */
#define  PE89_BTN_AUTO                   29
#define  PE89_BTN_HRW                    30
#define  PE89_LR_TURN_LEFT               31
#define  PE89_PICTURE                    32
#define  PE89_LED_IHKA_DIST_3            33      /* callback function: OnLED */
#define  PE89_LED_IHKA_DIST_2            34      /* callback function: OnLED */
#define  PE89_LED_BLOWER5                35      /* callback function: OnLED */
#define  PE89_LED_BLOWER4                36      /* callback function: OnLED */
#define  PE89_LED_BLOWER3                37      /* callback function: OnLED */
#define  PE89_LED_BLOWER2                38      /* callback function: OnLED */
#define  PE89_LED_BLOWER_IHKR7           39      /* callback function: OnLED */
#define  PE89_LED_BLOWER_IHKR1           40      /* callback function: OnLED */
#define  PE89_LED_BLOWER1                41      /* callback function: OnLED */
#define  PE89_LED_AC                     42      /* callback function: OnLED */
#define  PE89_BLUE_BCK_FREQ              43      /* callback function: OnPwm */
#define  PE89_LED_IHKA_AUTO              44      /* callback function: OnLED */
#define  PE89_LED_IHKA_DIST_1            45      /* callback function: OnLED */
#define  PE89_TEXTMSG_2                  46
#define  PE89_BLOWER_FREQ                47      /* callback function: OnPwm2 */
#define  PE89_ORANGE_FNC_FREQ            48      /* callback function: OnPwm */
#define  PE89_PWM_BLOWER                 49      /* callback function: OnPwm2 */
#define  PE89_SZM_RED_FNC_FREQ           50      /* callback function: OnPwm */
#define  PE89_PWM_ORANGE_FNC             51      /* callback function: OnPwm */
#define  PE89_BLOWER_DUTY                52      /* callback function: OnPwm2 */
#define  PE89_SZM_BLUE_FNC_FREQ          53      /* callback function: OnPwm */
#define  PE89_PWM_SZM_RED_FNC            54      /* callback function: OnPwm */
#define  PE89_ORANGE_FNC_DUTY            55      /* callback function: OnPwm */
#define  PE89_ORANGE_BCK_FREQ            56      /* callback function: OnPwm */
#define  PE89_PWM_SZM_BLUE_FNC           57      /* callback function: OnPwm */
#define  PE89_SZM_RED_FNC_DUTY           58      /* callback function: OnPwm */
#define  PE89_GREEN_FNC_FREQ             59      /* callback function: OnPwm */
#define  PE89_PWM_ORANGE_BCK             60      /* callback function: OnPwm */
#define  PE89_SZM_BLUE_FNC_DUTY          61      /* callback function: OnPwm */
#define  PE89_RED_BCK_FREQ               62      /* callback function: OnPwm */
#define  PE89_PWM_GREEN_FNC              63      /* callback function: OnPwm */
#define  PE89_ORANGE_BCK_DUTY            64      /* callback function: OnPwm */
#define  PE89_LIN_ENABLE_CMD             65      /* callback function: OnLED */
#define  PE89_LIN_SUPPLY_CMD             66      /* callback function: OnLED */
#define  PE89_SZM_BCKL_ENABLE_CMD        67      /* callback function: OnLED */
#define  PE89_SZM_STROBE_CMD             68      /* callback function: OnLED */
#define  PE89_SZM_SUPPLY_CMD             69      /* callback function: OnLED */
#define  PE89_LED_MAX                    70      /* callback function: OnLED */
#define  PE89_PWM_RED_BCK                71      /* callback function: OnPwm */
#define  PE89_GREEN_FNC_DUTY             72      /* callback function: OnPwm */
#define  PE89_BLUE_BCK_DUTY              73      /* callback function: OnPwm */
#define  PE89_LED_IHKA_HRW               74      /* callback function: OnLED */
#define  PE89_RED_BCK_DUTY               75      /* callback function: OnPwm */
#define  PE89_PWM_BLUE_BCK               76      /* callback function: OnPwm */
#define  PE89_LED_IHKA_DEFROST           77      /* callback function: OnLED */
#define  PE89_LED_IHKA_RECY2             78      /* callback function: OnLED */
#define  PE89_LED_IHKA_RECY1             79      /* callback function: OnLED */
#define  PE89_PANEL_OUTLET               80
#define  PE89_TEXTMSG_19                 81
#define  PE89_FOOT_OUTLET                82
#define  PE89_TEXTMSG_18                 83
#define  PE89_EVAPO                      84
#define  PE89_TEXTMSG_17                 85
#define  PE89_ON_SOLAR_SENSOR            86
#define  PE89_TEXTMSG_16                 87
#define  PE89_TEMP_PCB                   88
#define  PE89_TEXTMSG_15                 89
#define  PE89_SZM_VARIANT_2              90
#define  PE89_TEXTMSG_28                 91
#define  PE89_SZM_VARIANT_1              92
#define  PE89_TEXTMSG_27                 93
#define  PE89_SZM_PUSH_5                 94
#define  PE89_TEXTMSG_26                 95
#define  PE89_SZM_PUSH_34                96
#define  PE89_TEXTMSG_25                 97
#define  PE89_SZM_PUSH_12                98
#define  PE89_TEXTMSG_24                 99
#define  PE89_NOZZLE_POTI_FBK            100
#define  PE89_TEXTMSG_23                 101
#define  PE89_POTI_RIGHT_FBK             102
#define  PE89_TEXTMSG_22                 103
#define  PE89_POTI_LEFT_FBK              104
#define  PE89_TEXTMSG_21                 105
#define  PE89_BATTERY                    106
#define  PE89_TEXTMSG_20                 107
#define  PE89_DMM                        108
#define  PE89_LIN_SUPPLY                 109
#define  PE89_OUTLET_SENSOR              110
#define  PE89_5V_EXTERNAL                111
#define  PE89_BLOWER_PWM                 112
#define  PE89_SZM_VARIANT2               113
#define  PE89_SZM_VARIANT1               114
#define  PE89_SZM_PUSH5                  115
#define  PE89_SZM_PUSH34                 116
#define  PE89_SZM_PUSH12                 117
#define  PE89_SZM_PWM_RED                118
#define  PE89_SZM_PWM_BACKLIGHT          119
#define  PE89_SZM_PWM_GREEN              120
#define  PE89_SZM_STROBE                 121
#define  PE89_SZM_SCK                    122
#define  PE89_SZM_DATA                   123
#define  PE89_SOLAR_SENSOR               124
#define  PE89_TEXTMSG_32                 125
#define  PE89_TEXTMSG_31                 126
#define  PE89_TEXTMSG_14                 127
#define  PE89_PRODUCT_TYPE               128
#define  PE89_DMM_VALUE                  129     /* callback function: OnDmm */
#define  PE89_LIN_SUPPLY_DIAG            130
#define  PE89_MICROSWITCH                131
#define  PE89_BACKLIGHT_DIAG             132
#define  PE89_5V_SZM_DIAG                133
#define  PE89_SZM_SUPPLY_DIAG            134
#define  PE89_SZM_5_PUSH                 135
#define  PE89_SZM_4_PUSH                 136
#define  PE89_SZM_3_PUSH                 137
#define  PE89_SZM_2_PUSH                 138
#define  PE89_SZM_1_PUSH                 139

#define  VLINE                           3
#define  VLINE_TEXTBOX                   2       /* callback function: OnVline */
#define  VLINE_PICTURE                   3


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnButtonRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConnectCan(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDmm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLED(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMemoryRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnProductType(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSenRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVoltageRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
