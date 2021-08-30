/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2017. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_PICTURE                    2       /* control type: picture, callback function: (none) */
#define  PANEL_SAVE                       3       /* control type: pictButton, callback function: OnButton */
#define  PANEL_CANCEL                     4       /* control type: pictButton, callback function: OnButton */
#define  PANEL_MSG_BARCODE_2              5       /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG_BARCODE                6       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
