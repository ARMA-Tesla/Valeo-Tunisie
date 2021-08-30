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

#define  INFO                             1
#define  INFO_BKTREE                      2       /* control type: activeX, callback function: (none) */
#define  INFO_EXIT                        3       /* control type: pictButton, callback function: OnInfoExit */
#define  INFO_PICTURE                     4       /* control type: picture, callback function: (none) */

#define  LOG                              2
#define  LOG_PICTURE_3                    2       /* control type: picture, callback function: (none) */
#define  LOG_PICTURE_4                    3       /* control type: picture, callback function: (none) */
#define  LOG_PICTURE_2                    4       /* control type: picture, callback function: (none) */
#define  LOG_MESSAGE                      5       /* control type: string, callback function: (none) */

#define  PANEL                            3       /* callback function: OnAppPanel */
#define  PANEL_BKTREE                     2       /* control type: activeX, callback function: (none) */
#define  PANEL_LINE1                      3       /* control type: picture, callback function: (none) */
#define  PANEL_LINE2                      4       /* control type: picture, callback function: (none) */
#define  PANEL_PICTURERING                5       /* control type: pictRing, callback function: (none) */
#define  PANEL_PICTURE_2                  6       /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnAppPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnInfoExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
