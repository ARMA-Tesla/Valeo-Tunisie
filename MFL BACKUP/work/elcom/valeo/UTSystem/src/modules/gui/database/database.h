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

#define  DBS                              1
#define  DBS_IMAGELIST4                   2       /* control type: activeX, callback function: (none) */
#define  DBS_DB_INSERT                    3       /* control type: pictButton, callback function: OnDbInsert */
#define  DBS_DB_READ                      4       /* control type: pictButton, callback function: OnDbRead */
#define  DBS_GROUP1                       5       /* control type: ring, callback function: OnGroup */
#define  DBS_GROUP2                       6       /* control type: ring, callback function: OnGroup */
#define  DBS_GROUP3                       7       /* control type: ring, callback function: OnGroup */
#define  DBS_GROUP4                       8       /* control type: ring, callback function: OnGroup */
#define  DBS_GROUP5                       9       /* control type: ring, callback function: OnGroup */
#define  DBS_PICTURE5                     10      /* control type: picture, callback function: (none) */
#define  DBS_PICTURE4                     11      /* control type: picture, callback function: (none) */
#define  DBS_PICTURE3                     12      /* control type: picture, callback function: (none) */
#define  DBS_PICTURE2                     13      /* control type: picture, callback function: (none) */
#define  DBS_PICTURE1                     14      /* control type: picture, callback function: (none) */
#define  DBS_XML_EXPORT                   15      /* control type: command, callback function: OnExportXmlFile */
#define  DBS_XML_OPEN                     16      /* control type: command, callback function: OnOpenXmlFile */
#define  DBS_XML_FILE                     17      /* control type: string, callback function: (none) */
#define  DBS_MEMORY                       18      /* control type: numeric, callback function: (none) */
#define  DBS_XML_BCK                      19      /* control type: deco, callback function: (none) */
#define  DBS_TIMER                        20      /* control type: numeric, callback function: (none) */
#define  DBS_IMAGELIST1                   21      /* control type: activeX, callback function: (none) */
#define  DBS_IMAGELIST2                   22      /* control type: activeX, callback function: (none) */
#define  DBS_IMAGELIST3                   23      /* control type: activeX, callback function: (none) */
#define  DBS_USERPRODUCT                  24      /* control type: string, callback function: (none) */
#define  DBS_USERNAME                     25      /* control type: string, callback function: (none) */
#define  DBS_RIGHTS_TXT                   26      /* control type: textMsg, callback function: (none) */
#define  DBS_USERRIGHTS                   27      /* control type: textBox, callback function: (none) */
#define  DBS_TIMER_HRUBCA                 28      /* control type: timer, callback function: OnTimer */
#define  DBS_BKTREE                       29      /* control type: activeX, callback function: OnBKTREE */
#define  DBS_OUTLINE                      30      /* control type: deco, callback function: (none) */
#define  DBS_TEXTBOX                      31      /* control type: textBox, callback function: (none) */
#define  DBS_PICTURE                      32      /* control type: picture, callback function: (none) */

#define  DLG_USER                         2
#define  DLG_USER_NAME                    2       /* control type: string, callback function: (none) */
#define  DLG_USER_PASSW1                  3       /* control type: string, callback function: (none) */
#define  DLG_USER_PASSW2                  4       /* control type: string, callback function: (none) */
#define  DLG_USER_IBUTTON                 5       /* control type: string, callback function: (none) */
#define  DLG_USER_RIGHTS                  6       /* control type: ring, callback function: (none) */
#define  DLG_USER_DRIGHTS_1               7       /* control type: radioButton, callback function: (none) */
#define  DLG_USER_DRIGHTS_2               8       /* control type: radioButton, callback function: (none) */
#define  DLG_USER_DRIGHTS_3               9       /* control type: radioButton, callback function: (none) */
#define  DLG_USER_DRIGHTS_4               10      /* control type: radioButton, callback function: (none) */
#define  DLG_USER_DRIGHTS_5               11      /* control type: radioButton, callback function: (none) */
#define  DLG_USER_TRIGHTS_1               12      /* control type: radioButton, callback function: OnRightsSelection */
#define  DLG_USER_TRIGHTS_2               13      /* control type: radioButton, callback function: OnRightsSelection */
#define  DLG_USER_TRIGHTS_3               14      /* control type: radioButton, callback function: OnRightsSelection */
#define  DLG_USER_TRIGHTS_4               15      /* control type: radioButton, callback function: OnRightsSelection */
#define  DLG_USER_TRIGHTS_5               16      /* control type: radioButton, callback function: OnRightsSelection */
#define  DLG_USER_EXIT_OK                 17      /* control type: pictButton, callback function: (none) */
#define  DLG_USER_EXIT_CANCEL             18      /* control type: pictButton, callback function: (none) */
#define  DLG_USER_DBS_TXT                 19      /* control type: textMsg, callback function: (none) */
#define  DLG_USER_TESTER_TXT              20      /* control type: textMsg, callback function: (none) */
#define  DLG_USER_DECORATION_2            21      /* control type: deco, callback function: (none) */
#define  DLG_USER_DECORATION              22      /* control type: deco, callback function: (none) */
#define  DLG_USER_PICTURE                 23      /* control type: picture, callback function: (none) */

#define  HIST                             3
#define  HIST_SWITCH_PROD                 2       /* control type: radioButton, callback function: OnHisFillter */
#define  HIST_SWITCH_PRODPARAM            3       /* control type: radioButton, callback function: OnHisFillter */
#define  HIST_SWITCH_PRODMASTER           4       /* control type: radioButton, callback function: OnHisFillter */
#define  HIST_SWITCH_TEST                 5       /* control type: radioButton, callback function: OnHisFillter */
#define  HIST_SWITCH_TESTPARAM            6       /* control type: radioButton, callback function: OnHisFillter */
#define  HIST_SWITCH_USER                 7       /* control type: radioButton, callback function: OnHisFillter */
#define  HIST_SWITCH_USERRIGHTS           8       /* control type: radioButton, callback function: OnHisFillter */
#define  HIST_MERGE                       9       /* control type: radioButton, callback function: OnHisMerge */
#define  HIST_DTPICKER                    10      /* control type: activeX, callback function: (none) */
#define  HIST_PAGE                        11      /* control type: ring, callback function: OnHistList */
#define  HIST_PICTURE                     12      /* control type: picture, callback function: (none) */
#define  HIST_NEXT                        13      /* control type: command, callback function: OnHistList */
#define  HIST_PREW                        14      /* control type: command, callback function: OnHistList */

#define  INFO                             4
#define  INFO_INI_LIMIT                   2       /* control type: pictButton, callback function: OnIniLimits */
#define  INFO_INI_PARAMETER               3       /* control type: pictButton, callback function: OnIniLimits */
#define  INFO_INI_USER                    4       /* control type: pictButton, callback function: OnIniLimits */
#define  INFO_DTPICKER_2                  5       /* control type: activeX, callback function: (none) */
#define  INFO_DTPICKER_1                  6       /* control type: activeX, callback function: (none) */
#define  INFO_INI_LIMIT_HIST              7       /* control type: pictButton, callback function: OnIniLimits */
#define  INFO_INI_PARAMETER_HIST          8       /* control type: pictButton, callback function: OnIniLimits */
#define  INFO_TO_FILE                     9       /* control type: radioButton, callback function: (none) */
#define  INFO_DB_DELETE_HIST              10      /* control type: pictButton, callback function: OnDbDeleteHist */
#define  INFO_PICTURE                     11      /* control type: picture, callback function: (none) */

#define  MASTER                           5
#define  MASTER_MASTER_ID                 2       /* control type: ring, callback function: (none) */
#define  MASTER_MASTER_CODE               3       /* control type: string, callback function: (none) */
#define  MASTER_EXIT_OK                   4       /* control type: pictButton, callback function: (none) */
#define  MASTER_EXIT_CANCEL               5       /* control type: pictButton, callback function: (none) */
#define  MASTER_PICTURE                   6       /* control type: picture, callback function: (none) */

#define  PARAM                            6
#define  PARAM_NB                         2       /* control type: string, callback function: (none) */
#define  PARAM_NAME                       3       /* control type: string, callback function: (none) */
#define  PARAM_DESCRIPT                   4       /* control type: string, callback function: (none) */
#define  PARAM_VALUE                      5       /* control type: string, callback function: (none) */
#define  PARAM_VTYPE                      6       /* control type: ring, callback function: OnVtype */
#define  PARAM_PICTURE                    7       /* control type: picture, callback function: (none) */
#define  PARAM_TESTER                     8       /* control type: radioButton, callback function: (none) */
#define  PARAM_EXIT_OK                    9       /* control type: pictButton, callback function: (none) */
#define  PARAM_EXIT_CANCEL                10      /* control type: pictButton, callback function: (none) */
#define  PARAM_TEXTBOX                    11      /* control type: textBox, callback function: (none) */

#define  PRODUCT                          7
#define  PRODUCT_NAME                     2       /* control type: string, callback function: (none) */
#define  PRODUCT_DESCRIPT                 3       /* control type: string, callback function: (none) */
#define  PRODUCT_EXIT_OK                  4       /* control type: pictButton, callback function: (none) */
#define  PRODUCT_EXIT_CANCEL              5       /* control type: pictButton, callback function: (none) */
#define  PRODUCT_PARENT                   6       /* control type: string, callback function: (none) */
#define  PRODUCT_PICTURE                  7       /* control type: picture, callback function: (none) */
#define  PRODUCT_SETTINGS                 8       /* control type: radioButton, callback function: (none) */

#define  PRODUCT_CP                       8
#define  PRODUCT_CP_SOURCE                2       /* control type: ring, callback function: OnCopySourceSelect */
#define  PRODUCT_CP_SET_PROD_PARAM        3       /* control type: radioButton, callback function: OnCP */
#define  PRODUCT_CP_SET_PROD_MASTER       4       /* control type: radioButton, callback function: OnCP */
#define  PRODUCT_CP_SET_TEST              5       /* control type: radioButton, callback function: OnCP */
#define  PRODUCT_CP_SET_LINE_PARAM        6       /* control type: radioButton, callback function: OnCP */
#define  PRODUCT_CP_SET_LINE_STEP         7       /* control type: radioButton, callback function: OnCP */
#define  PRODUCT_CP_SET_LINE_PROCESS      8       /* control type: radioButton, callback function: OnCP */
#define  PRODUCT_CP_SET_TEST_PARAM        9       /* control type: radioButton, callback function: OnCP */
#define  PRODUCT_CP_EXIT_OK               10      /* control type: pictButton, callback function: (none) */
#define  PRODUCT_CP_EXIT_CANCEL           11      /* control type: pictButton, callback function: (none) */
#define  PRODUCT_CP_DECORATION_2          12      /* control type: deco, callback function: (none) */
#define  PRODUCT_CP_LISTBOX               13      /* control type: textBox, callback function: (none) */
#define  PRODUCT_CP_SETTING_TXT           14      /* control type: textMsg, callback function: (none) */
#define  PRODUCT_CP_PICTURE               15      /* control type: picture, callback function: (none) */

#define  TEST                             9
#define  TEST_STEP                        2       /* control type: string, callback function: (none) */
#define  TEST_NAME                        3       /* control type: string, callback function: (none) */
#define  TEST_DESCRIPT                    4       /* control type: string, callback function: (none) */
#define  TEST_EXIT_OK                     5       /* control type: pictButton, callback function: (none) */
#define  TEST_EXIT_CANCEL                 6       /* control type: pictButton, callback function: (none) */
#define  TEST_PICTURE                     7       /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnBKTREE(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCopySourceSelect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCP(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDbDeleteHist(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDbInsert(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDbRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnExportXmlFile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnGroup(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnHisFillter(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnHisMerge(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnHistList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnIniLimits(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnOpenXmlFile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnRightsSelection(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVtype(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
