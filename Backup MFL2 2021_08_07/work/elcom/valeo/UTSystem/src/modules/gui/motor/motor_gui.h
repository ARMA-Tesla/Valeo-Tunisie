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

#define  MOTOR                            1
#define  MOTOR_RECEIVE                    2       /* control type: textBox, callback function: (none) */
#define  MOTOR_SEND                       3       /* control type: textBox, callback function: (none) */
#define  MOTOR_SPEED                      4       /* control type: numeric, callback function: (none) */
#define  MOTOR_ANGLE                      5       /* control type: numeric, callback function: (none) */
#define  MOTOR_CLEAR_LOG                  6       /* control type: command, callback function: OnMotor */
#define  MOTOR_INIT                       7       /* control type: command, callback function: OnMotor */
#define  MOTOR_RESET_SENSOR               8       /* control type: command, callback function: OnMotor */
#define  MOTOR_GET_ANGLE                  9       /* control type: command, callback function: OnMotor */
#define  MOTOR_REQUEST_ANGLE              10      /* control type: command, callback function: OnMotor */
#define  MOTOR_READ_ANGLE                 11      /* control type: command, callback function: OnMotor */
#define  MOTOR_RESET_ANGLE                12      /* control type: command, callback function: OnMotor */
#define  MOTOR_STOP                       13      /* control type: command, callback function: OnMotor */
#define  MOTOR_ROTATE                     14      /* control type: command, callback function: OnMotor */
#define  MOTOR_PICTURE                    15      /* control type: picture, callback function: (none) */
#define  MOTOR_RESET                      16      /* control type: binary, callback function: (none) */
#define  MOTOR_BINARYSWITCH               17      /* control type: binary, callback function: (none) */
#define  MOTOR_INDEX                      18      /* control type: ring, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnMotor(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
