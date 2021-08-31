/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2020. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PRINTER                          1
#define  PRINTER_TEXTBOX                  2       /* control type: textBox, callback function: (none) */
#define  PRINTER_LED_CONNECT              3       /* control type: deco, callback function: (none) */
#define  PRINTER_TABLE                    4       /* control type: table, callback function: (none) */
#define  PRINTER_DISCONNECT2              5       /* control type: command, callback function: OnDisconnect */
#define  PRINTER_PRINT                    6       /* control type: command, callback function: OnPrint */
#define  PRINTER_RESET                    7       /* control type: command, callback function: OnReset */
#define  PRINTER_READ_STATUS2             8       /* control type: command, callback function: OnStatus */
#define  PRINTER_CONNECT2                 9       /* control type: command, callback function: OnConnect */
#define  PRINTER_PICTURE                  10      /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDisconnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrint(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnReset(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStatus(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
