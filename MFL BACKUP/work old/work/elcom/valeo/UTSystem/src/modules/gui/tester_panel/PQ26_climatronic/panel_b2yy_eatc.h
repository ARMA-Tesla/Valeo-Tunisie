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

#define  PANEL                            2
#define  PANEL_DECORATION_6               2       /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_9               3       /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_8               4       /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_7               5       /* control type: deco, callback function: (none) */
#define  PANEL_PICTURE_2                  6       /* control type: picture, callback function: (none) */
#define  PANEL_TEXTMSG_29                 7       /* control type: textMsg, callback function: (none) */
#define  PANEL_LED_AUTO                   8       /* control type: toggle, callback function: OnLed */
#define  PANEL_CONNECT_CAN                9       /* control type: textButton, callback function: OnConnectCan */
#define  PANEL_LED_DEFROST                10      /* control type: toggle, callback function: OnLed */
#define  PANEL_DECORATION_4               11      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_6                  12      /* control type: textMsg, callback function: (none) */
#define  PANEL_LED_ATS                    13      /* control type: toggle, callback function: OnLed */
#define  PANEL_LED_ATP                    14      /* control type: toggle, callback function: OnLed */
#define  PANEL_TEXTMSG_4                  15      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_3                  16      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_7                  17      /* control type: textMsg, callback function: (none) */
#define  PANEL_LED_ATF                    18      /* control type: toggle, callback function: OnLed */
#define  PANEL_LED_RECY                   19      /* control type: toggle, callback function: OnLed */
#define  PANEL_LED_AC                     20      /* control type: toggle, callback function: OnLed */
#define  PANEL_AC_PICT                    21      /* control type: picture, callback function: (none) */
#define  PANEL_RECY_PICT                  22      /* control type: picture, callback function: (none) */
#define  PANEL_AUTO_PICT22                23      /* control type: picture, callback function: (none) */
#define  PANEL_SEGMENT_5                  24      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_ATF_PICT                   25      /* control type: picture, callback function: (none) */
#define  PANEL_ATP_PICT                   26      /* control type: picture, callback function: (none) */
#define  PANEL_SEN_READ                   27      /* control type: command, callback function: OnSenRead */
#define  PANEL_ATS_PICT                   28      /* control type: picture, callback function: (none) */
#define  PANEL_DEFROST_PICT               29      /* control type: picture, callback function: (none) */
#define  PANEL_BLOW2_PICT_2               30      /* control type: picture, callback function: (none) */
#define  PANEL_BLOW2_PICT                 31      /* control type: picture, callback function: (none) */
#define  PANEL_AUTO_PICT_3                32      /* control type: picture, callback function: (none) */
#define  PANEL_SEGMENT_6                  33      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_7                  34      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_8                  35      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_4                  36      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_3                  37      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_SEGMENT_2                  38      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_AUTO_PICT                  39      /* control type: picture, callback function: (none) */
#define  PANEL_BLOW1_PICT_2               40      /* control type: picture, callback function: (none) */
#define  PANEL_BLOW1_PICT                 41      /* control type: picture, callback function: (none) */
#define  PANEL_BLUE_BCK_FREQ              42      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PICTURE                    43      /* control type: picture, callback function: (none) */
#define  PANEL_SEGMENT_1                  44      /* control type: numeric, callback function: OnDisplay */
#define  PANEL_TEXTMSG_2                  45      /* control type: textMsg, callback function: (none) */
#define  PANEL_SZM_RED_FNC_FREQ           46      /* control type: numeric, callback function: OnPwm */
#define  PANEL_SZM_BLUE_FNC_FREQ          47      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_SZM_RED_FNC            48      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_ORANGE_BCK_FREQ            49      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_SZM_BLUE_FNC           50      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_SZM_RED_FNC_DUTY           51      /* control type: numeric, callback function: OnPwm */
#define  PANEL_GREEN_FNC_FREQ             52      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_ORANGE_BCK             53      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_SZM_BLUE_FNC_DUTY          54      /* control type: numeric, callback function: OnPwm */
#define  PANEL_RED_BCK_FREQ               55      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_GREEN_FNC              56      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_ORANGE_BCK_DUTY            57      /* control type: numeric, callback function: OnPwm */
#define  PANEL_BUTTON_READ                58      /* control type: radioButton, callback function: OnButtonRead */
#define  PANEL_DISPLAY                    59      /* control type: textButton, callback function: OnDisplay */
#define  PANEL_MODE                       60      /* control type: ring, callback function: OnMode */
#define  PANEL_PWM_RED_BCK                61      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_GREEN_FNC_DUTY             62      /* control type: numeric, callback function: OnPwm */
#define  PANEL_BLUE_BCK_DUTY              63      /* control type: numeric, callback function: OnPwm */
#define  PANEL_RED_BCK_DUTY               64      /* control type: numeric, callback function: OnPwm */
#define  PANEL_PWM_BLUE_BCK               65      /* control type: radioButton, callback function: OnPwm */
#define  PANEL_PANEL_OUTLET               66      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_19                 67      /* control type: textMsg, callback function: (none) */
#define  PANEL_FOOT_OUTLET                68      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_18                 69      /* control type: textMsg, callback function: (none) */
#define  PANEL_EVAPO                      70      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_17                 71      /* control type: textMsg, callback function: (none) */
#define  PANEL_ON_SOLAR_SENSOR            72      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_16                 73      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEMP_PCB                   74      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_15                 75      /* control type: textMsg, callback function: (none) */
#define  PANEL_SZM_VARIANT_2              76      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_28                 77      /* control type: textMsg, callback function: (none) */
#define  PANEL_SZM_VARIANT_1              78      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_27                 79      /* control type: textMsg, callback function: (none) */
#define  PANEL_SZM_PUSH_5                 80      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_26                 81      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_22                 82      /* control type: textMsg, callback function: (none) */
#define  PANEL_SZM_PUSH_34                83      /* control type: numeric, callback function: (none) */
#define  PANEL_POTI_LEFT_FBK              84      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_25                 85      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_21                 86      /* control type: textMsg, callback function: (none) */
#define  PANEL_SZM_PUSH_12                87      /* control type: numeric, callback function: (none) */
#define  PANEL_BATTERY                    88      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_24                 89      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_20                 90      /* control type: textMsg, callback function: (none) */
#define  PANEL_NOZZLE_POTI_FBK            91      /* control type: numeric, callback function: (none) */
#define  PANEL_SOLAR_SENSOR               92      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_23                 93      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_14                 94      /* control type: textMsg, callback function: (none) */
#define  PANEL_POTI_RIGHT_FBK             95      /* control type: numeric, callback function: (none) */
#define  PANEL_VER                        96      /* control type: string, callback function: (none) */

#define  PQ26                             3
#define  PQ26_CONNECT_ALL                 2       /* control type: command, callback function: OnConnectAll */
#define  PQ26_DISCONNECT_ALL              3       /* control type: command, callback function: OnDisconnectAll */
#define  PQ26_B1_CH80                     4       /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH60                     5       /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH28                     6       /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH30                     7       /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B2_CH2                      8       /* control type: pictRing, callback function: OnB2Reley */
#define  PQ26_B1_CH10                     9       /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH78                     10      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH0                      11      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH20                     12      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B2_CH1                      13      /* control type: pictRing, callback function: OnB2Reley */
#define  PQ26_B1_CH40                     14      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH50                     15      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH70                     16      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH90                     17      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B2_CH3                      18      /* control type: pictRing, callback function: OnB2Reley */
#define  PQ26_B2_CH0                      19      /* control type: pictRing, callback function: OnB2Reley */
#define  PQ26_B1_CH51                     20      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH41                     21      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH31                     22      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH73                     23      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH63                     24      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_B1_CH53                     25      /* control type: pictRing, callback function: OnB1Reley */
#define  PQ26_AI9                         26      /* control type: numeric, callback function: (none) */
#define  PQ26_AI7                         27      /* control type: numeric, callback function: (none) */
#define  PQ26_AI5                         28      /* control type: numeric, callback function: (none) */
#define  PQ26_AI3                         29      /* control type: numeric, callback function: (none) */
#define  PQ26_AI13                        30      /* control type: numeric, callback function: (none) */
#define  PQ26_AI1                         31      /* control type: numeric, callback function: (none) */
#define  PQ26_AI11                        32      /* control type: numeric, callback function: (none) */
#define  PQ26_AI0                         33      /* control type: numeric, callback function: (none) */
#define  PQ26_AI2                         34      /* control type: numeric, callback function: (none) */
#define  PQ26_AI12                        35      /* control type: numeric, callback function: (none) */
#define  PQ26_AI4                         36      /* control type: numeric, callback function: (none) */
#define  PQ26_AI6                         37      /* control type: numeric, callback function: (none) */
#define  PQ26_AI8                         38      /* control type: numeric, callback function: (none) */
#define  PQ26_AI10                        39      /* control type: numeric, callback function: (none) */
#define  PQ26_VOLTAGE                     40      /* control type: numeric, callback function: (none) */
#define  PQ26_CURRENT                     41      /* control type: numeric, callback function: (none) */
#define  PQ26_AI14                        42      /* control type: numeric, callback function: (none) */
#define  PQ26_R2                          43      /* control type: numeric, callback function: OnSensor */
#define  PQ26_R1                          44      /* control type: numeric, callback function: OnSensor */
#define  PQ26_R5                          45      /* control type: numeric, callback function: OnSensor */
#define  PQ26_R4                          46      /* control type: numeric, callback function: OnSensor */
#define  PQ26_R3                          47      /* control type: numeric, callback function: OnSensor */
#define  PQ26_PWR_VOLTAGE                 48      /* control type: numeric, callback function: (none) */
#define  PQ26_PICTURE                     49      /* control type: picture, callback function: (none) */
#define  PQ26_MODE                        50      /* control type: ring, callback function: OnMode */
#define  PQ26_TAB                         51      /* control type: tab, callback function: (none) */
#define  PQ26_VOLTAGE_BCK                 52      /* control type: deco, callback function: (none) */
#define  PQ26_TIMER                       53      /* control type: timer, callback function: OnTimer */
#define  PQ26_CURRENT_BCK                 54      /* control type: deco, callback function: (none) */

#define  VLINE                            4
#define  VLINE_TEXTBOX                    2       /* control type: textBox, callback function: OnVline */
#define  VLINE_PICTURE                    3       /* control type: picture, callback function: (none) */

     /* tab page panel controls */
#define  EB2YY_CAN_CAN                    2       /* control type: command, callback function: OnEnableCan */

     /* tab page panel controls */
#define  EB2YY_DBG_VLINE                  2       /* control type: command, callback function: OnEnableVline */
#define  EB2YY_DBG_RECI_A                 3       /* control type: radioButton, callback function: OnReciStepper */
#define  EB2YY_DBG_RECI_B                 4       /* control type: radioButton, callback function: OnReciStepper */
#define  EB2YY_DBG_RECI_C                 5       /* control type: radioButton, callback function: OnReciStepper */
#define  EB2YY_DBG_RECI_D                 6       /* control type: radioButton, callback function: OnReciStepper */
#define  EB2YY_DBG_TEMP_A                 7       /* control type: radioButton, callback function: OnTempStepper */
#define  EB2YY_DBG_TEMP_B                 8       /* control type: radioButton, callback function: OnTempStepper */
#define  EB2YY_DBG_TEMP_C                 9       /* control type: radioButton, callback function: OnTempStepper */
#define  EB2YY_DBG_TEMP_D                 10      /* control type: radioButton, callback function: OnTempStepper */
#define  EB2YY_DBG_AIR_A                  11      /* control type: radioButton, callback function: OnAirStepper */
#define  EB2YY_DBG_AIR_B                  12      /* control type: radioButton, callback function: OnAirStepper */
#define  EB2YY_DBG_AIR_C                  13      /* control type: radioButton, callback function: OnAirStepper */
#define  EB2YY_DBG_AIR_D                  14      /* control type: radioButton, callback function: OnAirStepper */
#define  EB2YY_DBG_DECORATION_3           15      /* control type: deco, callback function: (none) */
#define  EB2YY_DBG_DECORATION_2           16      /* control type: deco, callback function: (none) */
#define  EB2YY_DBG_DECORATION             17      /* control type: deco, callback function: (none) */
#define  EB2YY_DBG_TEXTMSG                18      /* control type: textMsg, callback function: (none) */
#define  EB2YY_DBG_TEXTMSG_2              19      /* control type: textMsg, callback function: (none) */
#define  EB2YY_DBG_TEXTMSG_3              20      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnAirStepper(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnB1Reley(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnB2Reley(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnButtonRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConnectAll(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConnectCan(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDisconnectAll(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDisplay(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnEnableCan(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnEnableVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMemoryRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnReciStepper(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSenRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSensor(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTempStepper(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
