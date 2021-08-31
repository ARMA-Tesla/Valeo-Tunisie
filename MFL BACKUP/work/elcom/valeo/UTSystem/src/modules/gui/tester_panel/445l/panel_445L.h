/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2007. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  LCDAUTO                         1
#define  LCDAUTO_PICTURE                 2
#define  LCDAUTO_DISPLAYOFF              3       /* callback function: OnAutoDisplay */
#define  LCDAUTO_DISPLAYON               4       /* callback function: OnAutoDisplay */
#define  LCDAUTO_PICTURE_4               5
#define  LCDAUTO_PICTUREBUTTON_2         6
#define  LCDAUTO_S41                     7       /* callback function: OnAutoLCD */
#define  LCDAUTO_S42                     8       /* callback function: OnAutoLCD */
#define  LCDAUTO_S40                     9       /* callback function: OnAutoLCD */
#define  LCDAUTO_S34                     10      /* callback function: OnAutoLCD */
#define  LCDAUTO_S35                     11      /* callback function: OnAutoLCD */
#define  LCDAUTO_S33                     12      /* callback function: OnAutoLCD */
#define  LCDAUTO_S31                     13      /* callback function: OnAutoLCD */
#define  LCDAUTO_S32                     14      /* callback function: OnAutoLCD */
#define  LCDAUTO_S29                     15      /* callback function: OnAutoLCD */
#define  LCDAUTO_S38                     16      /* callback function: OnAutoLCD */
#define  LCDAUTO_S39                     17      /* callback function: OnAutoLCD */
#define  LCDAUTO_S36                     18      /* callback function: OnAutoLCD */
#define  LCDAUTO_S37                     19      /* callback function: OnAutoLCD */
#define  LCDAUTO_S30                     20      /* callback function: OnAutoLCD */
#define  LCDAUTO_S20                     21      /* callback function: OnAutoLCD */
#define  LCDAUTO_S21                     22      /* callback function: OnAutoLCD */
#define  LCDAUTO_S19                     23      /* callback function: OnAutoLCD */
#define  LCDAUTO_S17                     24      /* callback function: OnAutoLCD */
#define  LCDAUTO_S18                     25      /* callback function: OnAutoLCD */
#define  LCDAUTO_S15                     26      /* callback function: OnAutoLCD */
#define  LCDAUTO_S13                     27      /* callback function: OnAutoLCD */
#define  LCDAUTO_S14                     28      /* callback function: OnAutoLCD */
#define  LCDAUTO_S12                     29      /* callback function: OnAutoLCD */
#define  LCDAUTO_S10                     30      /* callback function: OnAutoLCD */
#define  LCDAUTO_S11                     31      /* callback function: OnAutoLCD */
#define  LCDAUTO_S16                     32      /* callback function: OnAutoLCD */
#define  LCDAUTO_S8                      33      /* callback function: OnAutoLCD */
#define  LCDAUTO_S6                      34      /* callback function: OnAutoLCD */
#define  LCDAUTO_S7                      35      /* callback function: OnAutoLCD */
#define  LCDAUTO_S5                      36      /* callback function: OnAutoLCD */
#define  LCDAUTO_S3                      37      /* callback function: OnAutoLCD */
#define  LCDAUTO_S4                      38      /* callback function: OnAutoLCD */
#define  LCDAUTO_S9                      39      /* callback function: OnAutoLCD */
#define  LCDAUTO_S1                      40      /* callback function: OnAutoLCD */
#define  LCDAUTO_S27                     41      /* callback function: OnAutoLCD */
#define  LCDAUTO_S28                     42      /* callback function: OnAutoLCD */
#define  LCDAUTO_S26                     43      /* callback function: OnAutoLCD */
#define  LCDAUTO_S24                     44      /* callback function: OnAutoLCD */
#define  LCDAUTO_S25                     45      /* callback function: OnAutoLCD */
#define  LCDAUTO_S2                      46      /* callback function: OnAutoLCD */
#define  LCDAUTO_S22                     47      /* callback function: OnAutoLCD */
#define  LCDAUTO_HEAD                    48      /* callback function: OnAutoLCD */
#define  LCDAUTO_DOT2                    49      /* callback function: OnAutoLCD */
#define  LCDAUTO_DOT1                    50      /* callback function: OnAutoLCD */
#define  LCDAUTO_B3                      51      /* callback function: OnAutoLCD */
#define  LCDAUTO_B1                      52      /* callback function: OnAutoLCD */
#define  LCDAUTO_B2                      53      /* callback function: OnAutoLCD */
#define  LCDAUTO_FAN0                    54      /* callback function: OnAutoLCD */
#define  LCDAUTO_FAN1                    55      /* callback function: OnAutoLCD */
#define  LCDAUTO_FAN2                    56      /* callback function: OnAutoLCD */
#define  LCDAUTO_FAN3                    57      /* callback function: OnAutoLCD */
#define  LCDAUTO_FAN4                    58      /* callback function: OnAutoLCD */
#define  LCDAUTO_S23                     59      /* callback function: OnAutoLCD */
#define  LCDAUTO_FAN5                    60      /* callback function: OnAutoLCD */
#define  LCDAUTO_FAN                     61      /* callback function: OnAutoLCD */
#define  LCDAUTO_RECY_DOWN               62      /* callback function: OnAutoLCD */
#define  LCDAUTO_RECY_MID                63      /* callback function: OnAutoLCD */
#define  LCDAUTO_DEFROST                 64      /* callback function: OnAutoLCD */
#define  LCDAUTO_MAN                     65      /* callback function: OnAutoLCD */
#define  LCDAUTO_FAN6                    66      /* callback function: OnAutoLCD */

#define  LCDMAN                          2
#define  LCDMAN_PICTURE                  2
#define  LCDMAN_DISPLAYOFF               3       /* callback function: OnManDisplay */
#define  LCDMAN_DISPLAYON                4       /* callback function: OnManDisplay */
#define  LCDMAN_PICTURE_3                5
#define  LCDMAN_FAN0                     6       /* callback function: OnManualLCD */
#define  LCDMAN_FAN1                     7       /* callback function: OnManualLCD */
#define  LCDMAN_FAN2                     8       /* callback function: OnManualLCD */
#define  LCDMAN_FAN3                     9       /* callback function: OnManualLCD */
#define  LCDMAN_FAN4                     10      /* callback function: OnManualLCD */
#define  LCDMAN_FAN5                     11      /* callback function: OnManualLCD */
#define  LCDMAN_FAN                      12      /* callback function: OnManualLCD */
#define  LCDMAN_FAN6                     13      /* callback function: OnManualLCD */
#define  LCDMAN_HEAD                     14      /* callback function: OnManualLCD */
#define  LCDMAN_RECY_DOWN                15      /* callback function: OnManualLCD */
#define  LCDMAN_RECY_MID                 16      /* callback function: OnManualLCD */
#define  LCDMAN_DEFROST                  17      /* callback function: OnManualLCD */
#define  LCDMAN_MAN                      18      /* callback function: OnManualLCD */
#define  LCDMAN_B3                       19      /* callback function: OnManualLCD */
#define  LCDMAN_B1                       20      /* callback function: OnManualLCD */
#define  LCDMAN_B2                       21      /* callback function: OnManualLCD */

#define  MEMORY                          3
#define  MEMORY_MEMORY                   2
#define  MEMORY_MEMORY_READ              3       /* callback function: OnMemoryRead */
#define  MEMORY_PICTURE                  4

#define  P445L                           4
#define  P445L_DECORATION_5              2
#define  P445L_DECORATION_7              3
#define  P445L_DECORATION_3              4
#define  P445L_TEXTMSG_4                 5
#define  P445L_TEXTMSG_3                 6
#define  P445L_TEXTMSG_8                 7
#define  P445L_TEXTMSG_5                 8
#define  P445L_TEXTMSG_2                 9
#define  P445L_PWM_LCD                   10      /* callback function: OnPwm */
#define  P445L_PWM_LED                   11      /* callback function: OnPwm */
#define  P445L_LED_FREQ                  12      /* callback function: OnPwm */
#define  P445L_SEN_READ                  13      /* callback function: OnSenRead */
#define  P445L_ZEBRA_COMPRESSION         14
#define  P445L_BATTERY_VOLTAGE           15
#define  P445L_LED_DUTY                  16      /* callback function: OnPwm */
#define  P445L_ILLUMINATION              17      /* callback function: OnIllumination */
#define  P445L_CONNECT                   18      /* callback function: OnConnect */
#define  P445L_DUAL_LED                  19      /* callback function: OnLed */
#define  P445L_RECIR_LED                 20      /* callback function: OnLed */
#define  P445L_PICTURE                   21
#define  P445L_HRW_LED                   22      /* callback function: OnLed */
#define  P445L_ATS_LED                   23      /* callback function: OnLed */
#define  P445L_AC_LED                    24      /* callback function: OnLed */
#define  P445L_AU_LED                    25      /* callback function: OnLed */
#define  P445L_HFS_LED                   26      /* callback function: OnLed */
#define  P445L_BUTTON_READ               27      /* callback function: OnButtonRead */
#define  P445L_AUTO_LED                  28      /* callback function: OnLed */
#define  P445L_MODE                      29      /* callback function: OnMode */
#define  P445L_VER                       30
#define  P445L_CANVAS                    31
#define  P445L_TEMP1_PICT                32
#define  P445L_TEMP2_PICT                33
#define  P445L_BLOW2_PICT                34
#define  P445L_BLOW1_PICT                35
#define  P445L_AUTO_PICT                 36
#define  P445L_AC_PICT                   37
#define  P445L_HRW_PICT                  38
#define  P445L_RECIR_PICT                39
#define  P445L_DUAL_PICT                 40
#define  P445L_MODE_PICT                 41
#define  P445L_ATS_PICT                  42
#define  P445L_VENT_U_PICT               43
#define  P445L_VENT_D_PICT               44
#define  P445L_HFS_PICT                  45
#define  P445L_AU_PICT                   46
#define  P445L_OFF_PICT                  47

#define  VLINE                           5
#define  VLINE_TEXTBOX                   2       /* callback function: OnVline */
#define  VLINE_PICTURE                   3


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnAutoDisplay(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAutoLCD(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnButtonRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnIllumination(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnManDisplay(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnManualLCD(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMemoryRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSenRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVline(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
