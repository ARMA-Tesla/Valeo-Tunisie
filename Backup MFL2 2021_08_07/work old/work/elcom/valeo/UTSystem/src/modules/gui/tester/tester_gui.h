/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2018. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  TESTER                           1
#define  TESTER_IMAGELIST                 2       /* control type: activeX, callback function: (none) */
#define  TESTER_BKTREE                    3       /* control type: activeX, callback function: (none) */
#define  TESTER_PICTURE                   4       /* control type: picture, callback function: (none) */
#define  TESTER_MASK_CLOSE                5       /* control type: command, callback function: OnFncTester */
#define  TESTER_MASK_CLOSE_LED            6       /* control type: deco, callback function: (none) */
#define  TESTER_LED_8                     7       /* control type: LED, callback function: (none) */
#define  TESTER_LED_7                     8       /* control type: LED, callback function: (none) */
#define  TESTER_LED_6                     9       /* control type: LED, callback function: (none) */
#define  TESTER_LED_5                     10      /* control type: LED, callback function: (none) */
#define  TESTER_LED_4                     11      /* control type: LED, callback function: (none) */
#define  TESTER_LED_3                     12      /* control type: LED, callback function: (none) */
#define  TESTER_LED_2                     13      /* control type: LED, callback function: (none) */
#define  TESTER_LED_1                     14      /* control type: LED, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnFncTester(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
