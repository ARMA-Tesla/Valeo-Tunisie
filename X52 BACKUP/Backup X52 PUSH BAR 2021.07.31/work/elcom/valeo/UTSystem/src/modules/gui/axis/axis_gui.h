/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  AXIS                             1
#define  AXIS_RECEIVE                     2       /* control type: textBox, callback function: (none) */
#define  AXIS_SEND                        3       /* control type: textBox, callback function: (none) */
#define  AXIS_SPEED                       4       /* control type: numeric, callback function: (none) */
#define  AXIS_TARGET_POSITION             5       /* control type: numeric, callback function: (none) */
#define  AXIS_POSITION                    6       /* control type: numeric, callback function: (none) */
#define  AXIS_HOME                        7       /* control type: command, callback function: OnAxis */
#define  AXIS_STOP                        8       /* control type: command, callback function: OnAxis */
#define  AXIS_SET_POSITION                9       /* control type: command, callback function: OnAxis */
#define  AXIS_READ_POSITION               10      /* control type: command, callback function: OnAxis */
#define  AXIS_DELETE                      11      /* control type: command, callback function: OnAxis */
#define  AXIS_MOVE                        12      /* control type: command, callback function: OnAxis */
#define  AXIS_READ_ALARM                  13      /* control type: command, callback function: OnAxis */
#define  AXIS_RESET_ALARM                 14      /* control type: command, callback function: OnAxis */
#define  AXIS_ENABLE                      15      /* control type: command, callback function: OnAxis */
#define  AXIS_INDEX                       16      /* control type: ring, callback function: (none) */
#define  AXIS_STATION                     17      /* control type: ring, callback function: (none) */
#define  AXIS_HOME_LED                    18      /* control type: deco, callback function: (none) */
#define  AXIS_PICTURE                     19      /* control type: picture, callback function: (none) */
#define  AXIS_TEXTMSG                     20      /* control type: textMsg, callback function: (none) */
#define  AXIS_TEXTMSG_2                   21      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnAxis(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif