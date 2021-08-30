/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2019. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ELDAQ                            1
#define  ELDAQ_PWR_CHANNEL                2       /* control type: ring, callback function: (none) */
#define  ELDAQ_SCALE                      3       /* control type: ring, callback function: OnCalibScale */
#define  ELDAQ_LABEL_AI0                  4       /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI0                5       /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_MEAS_INTERVAL              6       /* control type: numeric, callback function: (none) */
#define  ELDAQ_MODE                       7       /* control type: ring, callback function: OnMode */
#define  ELDAQ_PRECISION                  8       /* control type: numeric, callback function: OnPrecision */
#define  ELDAQ_SAMPLE_RATE                9       /* control type: numeric, callback function: (none) */
#define  ELDAQ_AI0                        10      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_ALL_1                11      /* control type: ring, callback function: OnScaleAll */
#define  ELDAQ_SCALE_AI0                  12      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI1                  13      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI1                14      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI1                        15      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI1                  16      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI2                  17      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI2                18      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI2                        19      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI2                  20      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI3                  21      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI3                22      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI3                        23      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI3                  24      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI4                  25      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI4                26      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI4                        27      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI4                  28      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI5                  29      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI5                30      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI5                        31      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI5                  32      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI6                  33      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI6                34      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI6                        35      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI6                  36      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI7                  37      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI7                38      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI7                        39      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI7                  40      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI8                  41      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI8                42      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI8                        43      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI8                  44      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI9                  45      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI9                46      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI9                        47      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI9                  48      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI10                 49      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI10               50      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI10                       51      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI10                 52      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI11                 53      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI11               54      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI11                       55      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI11                 56      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI12                 57      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI12               58      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI12                       59      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI12                 60      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI13                 61      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI13               62      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI13                       63      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI13                 64      /* control type: ring, callback function: OnScale */
#define  ELDAQ_LABEL_AI14                 65      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_ENABLED_AI14               66      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI14                       67      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI14                 68      /* control type: ring, callback function: OnScale */
#define  ELDAQ_ENABLED_AI15               69      /* control type: radioButton, callback function: OnChannelEnable */
#define  ELDAQ_AI15                       70      /* control type: numeric, callback function: (none) */
#define  ELDAQ_SCALE_AI15                 71      /* control type: ring, callback function: OnScale */
#define  ELDAQ_SAVE_FILE                  72      /* control type: command, callback function: OnSaveFile */
#define  ELDAQ_BROWSE_CF                  73      /* control type: command, callback function: OnBrowseCal */
#define  ELDAQ_TIMER                      74      /* control type: timer, callback function: OnTimer */
#define  ELDAQ_SAVE                       75      /* control type: command, callback function: OnSave */
#define  ELDAQ_STOP                       76      /* control type: command, callback function: OnStop */
#define  ELDAQ_PWR_DISABLE                77      /* control type: command, callback function: OnPwrDisable */
#define  ELDAQ_PWR_CONFIGURE              78      /* control type: command, callback function: OnPwrConfigure */
#define  ELDAQ_START                      79      /* control type: command, callback function: OnStart */
#define  ELDAQ_CALIB_AI_15                80      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_14                81      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_13                82      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_12                83      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_11                84      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_10                85      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_9                 86      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_8                 87      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_7                 88      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_6                 89      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_5                 90      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_4                 91      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_3                 92      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_2                 93      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_CALIB_AI_1                 94      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_TAB_1                      95      /* control type: picture, callback function: (none) */
#define  ELDAQ_CALIB_AI_0                 96      /* control type: radioButton, callback function: OnCalib */
#define  ELDAQ_LABEL_AI15                 97      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_CALIB_FILE                 98      /* control type: string, callback function: (none) */
#define  ELDAQ_DECORATION                 99      /* control type: deco, callback function: (none) */
#define  ELDAQ_LABEL_AI16_3               100     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_CALIB_Y2                   101     /* control type: numeric, callback function: (none) */
#define  ELDAQ_CALIB_X2                   102     /* control type: numeric, callback function: (none) */
#define  ELDAQ_PWR_CURRENT                103     /* control type: numeric, callback function: (none) */
#define  ELDAQ_PWR_VOLTAGE                104     /* control type: numeric, callback function: (none) */
#define  ELDAQ_CALIB_Y1                   105     /* control type: numeric, callback function: (none) */
#define  ELDAQ_CALIB_X1                   106     /* control type: numeric, callback function: (none) */
#define  ELDAQ_MSG_CALIB_CHANNEL          107     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_MSG_CALIB_1                108     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_MSG_PWR                    109     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_PICTURE                    110     /* control type: picture, callback function: (none) */
#define  ELDAQ_MSG_CALIB_2                111     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_DECORATION_7               112     /* control type: deco, callback function: (none) */
#define  ELDAQ_DECORATION_4               113     /* control type: deco, callback function: (none) */
#define  ELDAQ_DECORATION_3               114     /* control type: deco, callback function: (none) */
#define  ELDAQ_DECORATION_5               115     /* control type: deco, callback function: (none) */
#define  ELDAQ_DECORATION_2               116     /* control type: deco, callback function: (none) */
#define  ELDAQ_MSG_CALIB_4                117     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_MSG_CALIB_3                118     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_DECORATION_8               119     /* control type: deco, callback function: (none) */
#define  ELDAQ_LABEL_AI16_2               120     /* control type: textMsg, callback function: (none) */

#define  ELDAQ_2                          2
#define  ELDAQ_2_LABEL_AI0_38             2       /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_LABEL_P0_31              3       /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_31             4       /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_30              5       /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_30             6       /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_29              7       /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_29             8       /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_28              9       /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_28             10      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_27              11      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_27             12      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_26              13      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_26             14      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_25              15      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_25             16      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_24              17      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_24             18      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_23              19      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_23             20      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_22              21      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_22             22      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_21              23      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_21             24      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_20              25      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_20             26      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_19              27      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_19             28      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_18              29      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_18             30      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_17              31      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_17             32      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_16              33      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_16             34      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_TAB_5                    35      /* control type: picture, callback function: (none) */
#define  ELDAQ_2_TAB_6                    36      /* control type: picture, callback function: (none) */
#define  ELDAQ_2_LABEL_AI0_40             37      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_LABEL_AI0_39             38      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_LABEL_AI0_7              39      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_LABEL_P0_15              40      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_15             41      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_14              42      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_14             43      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_13              44      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_13             45      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_12              46      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_12             47      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_11              48      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_11             49      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_10              50      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_10             51      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_9               52      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_9              53      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P2_7               54      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P2_7              55      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P2_6               56      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P2_6              57      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P2_5               58      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P2_5              59      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P2_4               60      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P2_4              61      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P2_3               62      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P2_3              63      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P2_2               64      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P2_2              65      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P2_1               66      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P2_1              67      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P2_0               68      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P2_0              69      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_8               70      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_TAB_8                    71      /* control type: picture, callback function: (none) */
#define  ELDAQ_2_LABEL_P1_7               72      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P1_7              73      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P1_6               74      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P1_6              75      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P1_5               76      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P1_5              77      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P1_4               78      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P1_4              79      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P1_3               80      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P1_3              81      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P1_2               82      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P1_2              83      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P1_1               84      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P1_1              85      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P1_0               86      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P1_0              87      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_SWITCH_P0_8              88      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_TAB_7                    89      /* control type: picture, callback function: (none) */
#define  ELDAQ_2_LABEL_P0_7               90      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_7              91      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_6               92      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_6              93      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_5               94      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_5              95      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_4               96      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_4              97      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_3               98      /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_3              99      /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_2               100     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_2              101     /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_1               102     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_1              103     /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_LABEL_P0_0               104     /* control type: textMsg, callback function: (none) */
#define  ELDAQ_2_SWITCH_P0_0              105     /* control type: binary, callback function: OnSwitch */
#define  ELDAQ_2_TAB_4                    106     /* control type: picture, callback function: (none) */
#define  ELDAQ_2_TAB_3                    107     /* control type: picture, callback function: (none) */
#define  ELDAQ_2_MODE                     108     /* control type: ring, callback function: OnMode */
#define  ELDAQ_2_RESET                    109     /* control type: command, callback function: OnReset */
#define  ELDAQ_2_PICTURE                  110     /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnBrowseCal(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCalib(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCalibScale(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnChannelEnable(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrecision(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwrConfigure(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPwrDisable(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnReset(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSave(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSaveFile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnScale(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnScaleAll(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSwitch(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
