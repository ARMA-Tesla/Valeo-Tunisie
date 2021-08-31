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

#define  B2YY                            1
#define  B2YY_DECORATION_6               2
#define  B2YY_DECORATION_5               3
#define  B2YY_DECORATION_4               4
#define  B2YY_DECORATION_7               5
#define  B2YY_DECORATION_3               6
#define  B2YY_TEXTMSG_4                  7
#define  B2YY_TEXTMSG_3                  8
#define  B2YY_TEXTMSG_7                  9
#define  B2YY_TEXTMSG_6                  10
#define  B2YY_TEXTMSG_8                  11
#define  B2YY_TEXTMSG_5                  12
#define  B2YY_TEXTMSG_2                  13
#define  B2YY_PWM_SYMBOL                 14      /* callback function: OnPwm */
#define  B2YY_PWM_LCD                    15      /* callback function: OnPwm */
#define  B2YY_PWM_LED                    16      /* callback function: OnPwm */
#define  B2YY_LED_FREQ                   17      /* callback function: OnPwm */
#define  B2YY_LCD_FREQ                   18      /* callback function: OnPwm */
#define  B2YY_SEN_VBATT                  19      /* callback function: OnPwm */
#define  B2YY_SEN_DP                     20      /* callback function: OnPwm */
#define  B2YY_SEN_DF                     21      /* callback function: OnPwm */
#define  B2YY_SEN_RSL                    22      /* callback function: OnPwm */
#define  B2YY_SEN_READ                   23      /* callback function: OnSenRead */
#define  B2YY_SEN_LSL                    24      /* callback function: OnPwm */
#define  B2YY_SEN_ITEMP                  25      /* callback function: OnPwm */
#define  B2YY_BLOWER_FREQ                26      /* callback function: OnPwm */
#define  B2YY_SYMBOL_FREQ                27      /* callback function: OnPwm */
#define  B2YY_SEGMENT_4                  28      /* callback function: OnDisplay */
#define  B2YY_SEGMENT_3                  29      /* callback function: OnDisplay */
#define  B2YY_SEGMENT_2                  30      /* callback function: OnDisplay */
#define  B2YY_SEGMENT_1                  31      /* callback function: OnDisplay */
#define  B2YY_LED_DUTY                   32      /* callback function: OnPwm */
#define  B2YY_LCD_DUTY                   33      /* callback function: OnPwm */
#define  B2YY_BLOWER_DUTY                34      /* callback function: OnPwm */
#define  B2YY_SYMBOL_DUTY                35      /* callback function: OnPwm */
#define  B2YY_STEPPER_AIR                36      /* callback function: OnStepper */
#define  B2YY_CONNECT                    37      /* callback function: OnConnect */
#define  B2YY_STEPPER_DOOR               38      /* callback function: OnStepper */
#define  B2YY_DISPLAY                    39      /* callback function: OnDisplay */
#define  B2YY_STEPPER_RECIRC             40      /* callback function: OnStepper */
#define  B2YY_PWM_BLOWER                 41      /* callback function: OnPwm */
#define  B2YY_AC_LED                     42      /* callback function: OnLed */
#define  B2YY_RECIR_LED                  43      /* callback function: OnLed */
#define  B2YY_PICTURE                    44
#define  B2YY_MAX_LED                    45      /* callback function: OnLed */
#define  B2YY_ATS_LED                    46      /* callback function: OnLed */
#define  B2YY_HFS_LED                    47      /* callback function: OnLed */
#define  B2YY_HRW_LED                    48      /* callback function: OnLed */
#define  B2YY_ATF_LED                    49      /* callback function: OnLed */
#define  B2YY_ATP_LED                    50      /* callback function: OnLed */
#define  B2YY_BUTTON_READ                51      /* callback function: OnButtonRead */
#define  B2YY_AUTO_LED                   52      /* callback function: OnLed */
#define  B2YY_MODE                       53      /* callback function: OnMode */
#define  B2YY_VER                        54
#define  B2YY_TEMP1_PICT                 55
#define  B2YY_TEMP2_PICT                 56
#define  B2YY_BLOW2_PICT                 57
#define  B2YY_AUTO_PICT                  58
#define  B2YY_AC_PICT                    59
#define  B2YY_HRW_PICT                   60
#define  B2YY_RECIR_PICT                 61
#define  B2YY_HFS_PICT                   62
#define  B2YY_MAX_PICT                   63
#define  B2YY_ATS_PICT                   64
#define  B2YY_ATP_PICT                   65
#define  B2YY_ATF_PICT                   66
#define  B2YY_OFF_PICT                   67
#define  B2YY_PICTURE_2                  68
#define  B2YY_BLOW1_PICT                 69

#define  EB2YY                           2
#define  EB2YY_CONNECT_ALL               2       /* callback function: OnConnectAll */
#define  EB2YY_DISCONNECT_ALL            3       /* callback function: OnDisconnectAll */
#define  EB2YY_B1_CH80                   4       /* callback function: OnB1Reley */
#define  EB2YY_B1_CH60                   5       /* callback function: OnB1Reley */
#define  EB2YY_B1_CH28                   6       /* callback function: OnB1Reley */
#define  EB2YY_B1_CH30                   7       /* callback function: OnB1Reley */
#define  EB2YY_B2_CH2                    8       /* callback function: OnB2Reley */
#define  EB2YY_B1_CH10                   9       /* callback function: OnB1Reley */
#define  EB2YY_B1_CH78                   10      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH0                    11      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH20                   12      /* callback function: OnB1Reley */
#define  EB2YY_B2_CH1                    13      /* callback function: OnB2Reley */
#define  EB2YY_B1_CH40                   14      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH50                   15      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH70                   16      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH90                   17      /* callback function: OnB1Reley */
#define  EB2YY_B2_CH3                    18      /* callback function: OnB2Reley */
#define  EB2YY_B2_CH0                    19      /* callback function: OnB2Reley */
#define  EB2YY_B1_CH51                   20      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH41                   21      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH31                   22      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH73                   23      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH63                   24      /* callback function: OnB1Reley */
#define  EB2YY_B1_CH53                   25      /* callback function: OnB1Reley */
#define  EB2YY_AI9                       26
#define  EB2YY_AI7                       27
#define  EB2YY_AI5                       28
#define  EB2YY_AI3                       29
#define  EB2YY_AI13                      30
#define  EB2YY_AI1                       31
#define  EB2YY_AI11                      32
#define  EB2YY_AI0                       33
#define  EB2YY_AI2                       34
#define  EB2YY_AI12                      35
#define  EB2YY_AI4                       36
#define  EB2YY_AI6                       37
#define  EB2YY_AI8                       38
#define  EB2YY_AI10                      39
#define  EB2YY_VOLTAGE                   40
#define  EB2YY_CURRENT                   41
#define  EB2YY_AI14                      42
#define  EB2YY_R2                        43      /* callback function: OnSensor */
#define  EB2YY_R1                        44      /* callback function: OnSensor */
#define  EB2YY_R5                        45      /* callback function: OnSensor */
#define  EB2YY_R4                        46      /* callback function: OnSensor */
#define  EB2YY_R3                        47      /* callback function: OnSensor */
#define  EB2YY_PWR_VOLTAGE               48
#define  EB2YY_PICTURE                   49
#define  EB2YY_MODE                      50      /* callback function: OnMode */
#define  EB2YY_TAB                       51
#define  EB2YY_VOLTAGE_BCK               52
#define  EB2YY_TIMER                     53      /* callback function: OnTimer */
#define  EB2YY_CURRENT_BCK               54

#define  MEMORY                          3
#define  MEMORY_MEMORY                   2
#define  MEMORY_MEMORY_READ              3       /* callback function: OnMemoryRead */
#define  MEMORY_PICTURE                  4

#define  VLINE                           4
#define  VLINE_TEXTBOX                   2       /* callback function: OnVline */
#define  VLINE_PICTURE                   3

     /* tab page panel controls */
#define  EB2YY_CAN_CAN                   2       /* callback function: OnEnableCan */

     /* tab page panel controls */
#define  EB2YY_DBG_VLINE                 2       /* callback function: OnEnableVline */
#define  EB2YY_DBG_RECI_A                3       /* callback function: OnReciStepper */
#define  EB2YY_DBG_RECI_B                4       /* callback function: OnReciStepper */
#define  EB2YY_DBG_RECI_C                5       /* callback function: OnReciStepper */
#define  EB2YY_DBG_RECI_D                6       /* callback function: OnReciStepper */
#define  EB2YY_DBG_TEMP_A                7       /* callback function: OnTempStepper */
#define  EB2YY_DBG_TEMP_B                8       /* callback function: OnTempStepper */
#define  EB2YY_DBG_TEMP_C                9       /* callback function: OnTempStepper */
#define  EB2YY_DBG_TEMP_D                10      /* callback function: OnTempStepper */
#define  EB2YY_DBG_AIR_A                 11      /* callback function: OnAirStepper */
#define  EB2YY_DBG_AIR_B                 12      /* callback function: OnAirStepper */
#define  EB2YY_DBG_AIR_C                 13      /* callback function: OnAirStepper */
#define  EB2YY_DBG_AIR_D                 14      /* callback function: OnAirStepper */
#define  EB2YY_DBG_DECORATION_3          15
#define  EB2YY_DBG_DECORATION_2          16
#define  EB2YY_DBG_DECORATION            17
#define  EB2YY_DBG_TEXTMSG               18
#define  EB2YY_DBG_TEXTMSG_2             19
#define  EB2YY_DBG_TEXTMSG_3             20


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnAirStepper(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnB1Reley(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnB2Reley(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnButtonRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConnectAll(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
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
int  CVICALLBACK OnStepper(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTempStepper(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
