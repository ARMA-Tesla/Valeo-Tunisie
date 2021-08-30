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

#define  MSG_ERROR                       1
#define  MSG_ERROR_TABLE                 2
#define  MSG_ERROR_EXIT                  3       /* callback function: OnErrorExit */
#define  MSG_ERROR_PICTURE               4
#define  MSG_ERROR_DECORATION            5

#define  MSG_SAP                         2
#define  MSG_SAP_PICTURE                 2
#define  MSG_SAP_TRANSLATED_BARCODE      3
#define  MSG_SAP_EXIT                    4       /* callback function: OnErrorExit */
#define  MSG_SAP_STRING_4                5
#define  MSG_SAP_STRING_5                6
#define  MSG_SAP_STRING_3                7
#define  MSG_SAP_BARCODE                 8
#define  MSG_SAP_DECORATION              9
#define  MSG_SAP_DECORATION_2            10

#define  PANEL                           3
#define  PANEL_PICTURE                   2
#define  PANEL_SAP                       3       /* callback function: OnSAP */
#define  PANEL_PRINTER                   4
#define  PANEL_BOX                       5
#define  PANEL_ERROR                     6       /* callback function: OnError */
#define  PANEL_TIMER                     7       /* callback function: OnTimer */
#define  PANEL_TEXTMSG                   8
#define  PANEL_BOX_TXT                   9
#define  PANEL_TIMER_BOX                 10      /* callback function: OnTimerBox */
#define  PANEL_SAP_ERROR                 11
#define  PANEL_PICTURE_2                 12
#define  PANEL_SAP_ERR_LABEL             13

#define  SAP_ERROR                       4
#define  SAP_ERROR_TIME                  2
#define  SAP_ERROR_STRING                3
#define  SAP_ERROR_TIMER                 4       /* callback function: OnSAPErrorTimer */
#define  SAP_ERROR_PICTURE               5


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnError(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnErrorExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSAP(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSAPErrorTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimerBox(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
