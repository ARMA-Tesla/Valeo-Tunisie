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

#define  CAMCFG                          1
#define  CAMCFG_BRIGHTNESS               2       /* callback function: OnCameraConfig */
#define  CAMCFG_FRAME_RATE               3       /* callback function: OnCameraConfig */
#define  CAMCFG_GAIN                     4       /* callback function: OnCameraConfig */
#define  CAMCFG_DECORATION_2             5
#define  CAMCFG_SNAP_AVERAGE             6
#define  CAMCFG_SNAP_AVERAGE_GAIN        7
#define  CAMCFG_OK                       8       /* callback function: OnSetCam */
#define  CAMCFG_SETTINGS_TXT             9
#define  CAMCFG_FRATE                    10
#define  CAMCFG_SHUTTER                  11      /* callback function: OnCameraConfig */
#define  CAMCFG_TAB                      12

#define  CFG                             2
#define  CFG_SPATH                       2
#define  CFG_DIRECTORY                   3       /* callback function: OnDirectory */
#define  CFG_OK                          4       /* callback function: OnPanelCfgExit */
#define  CFG_CANCEL                      5       /* callback function: OnPanelCfgExit */

#define  IMGBUFFER                       3
#define  IMGBUFFER_DECORATION_4          2
#define  IMGBUFFER_NEW_TXT_2             3
#define  IMGBUFFER_BUFFERS_TXT           4
#define  IMGBUFFER_DECORATION_5          5
#define  IMGBUFFER_ACT_IMAGE_BUFFER      6       /* callback function: OnPanelImgbuffPreview */
#define  IMGBUFFER_NAME_TXT              7
#define  IMGBUFFER_BPREVIEW              8       /* callback function: OnPanelImgbuffPreview */
#define  IMGBUFFER_BUFFER_NAME           9
#define  IMGBUFFER_IMAGE_BUFFER_LIST     10      /* callback function: OnImgBuffList */
#define  IMGBUFFER_OK                    11      /* callback function: OnPanelImgbuffExit */
#define  IMGBUFFER_SNAME                 12
#define  IMGBUFFER_PREVIEW               13
#define  IMGBUFFER_CANCEL                14      /* callback function: OnPanelImgbuffExit */

#define  MAIN                            4
#define  MAIN_CAMERA                     2       /* callback function: OnCamera */
#define  MAIN_SET_CAM                    3       /* callback function: OnSetCam */
#define  MAIN_IMG_SNAP                   4       /* callback function: OnImgSnap */
#define  MAIN_IMG_GRAB                   5       /* callback function: OnImgGrab */
#define  MAIN_IMG_LOAD                   6       /* callback function: OnImgLoad */
#define  MAIN_IMG_SAVE                   7       /* callback function: OnImgSave */
#define  MAIN_IMG_MAKE_BUFFER            8       /* callback function: OnMakeBuffer */
#define  MAIN_CFG_NEW                    9       /* callback function: OnConfig */
#define  MAIN_CFG_LOAD                   10      /* callback function: OnConfig */
#define  MAIN_CFG_SAVE                   11      /* callback function: OnConfig */
#define  MAIN_CFG_SET                    12      /* callback function: OnConfig */
#define  MAIN_TASKLIST                   13      /* callback function: OnTaskList */
#define  MAIN_TASK_ADD                   14      /* callback function: OnTaskAdd */
#define  MAIN_TASK_TXT                   15
#define  MAIN_CONFIG_TXT                 16
#define  MAIN_CAMERA_TXT                 17
#define  MAIN_IMAGE_TXT                  18
#define  MAIN_DECORATION                 19
#define  MAIN_DECORATION_3               20
#define  MAIN_DECORATION_4               21
#define  MAIN_DECORATION_5               22

#define  MAKEBUFF                        5
#define  MAKEBUFF_BUFFER_NAME            2
#define  MAKEBUFF_SNAP_IMAGE_BUFFER      3       /* callback function: OnCreateBuffer */
#define  MAKEBUFF_LOAD_IMAGE_BUFFER      4       /* callback function: OnCreateBuffer */
#define  MAKEBUFF_ACT_IMAGE_BUFFER       5       /* callback function: OnCreateBuffer */
#define  MAKEBUFF_AVG_IMAGE_BUFFER       6
#define  MAKEBUFF_IMAGE_BUFFER_LIST      7       /* callback function: OnBufferImageList */
#define  MAKEBUFF_OK                     8       /* callback function: OnHideChildPanel */
#define  MAKEBUFF_DECORATION_3           9
#define  MAKEBUFF_NEW_TXT                10
#define  MAKEBUFF_BUFFERS_TXT            11
#define  MAKEBUFF_DECORATION_2           12

#define  PANEL                           6       /* callback function: OnPanel */
#define  PANEL_MENU_OUTLINE              2
#define  PANEL_EXIT                      3       /* callback function: OnExit */
#define  PANEL_TOOL_OUTLINE              4
#define  PANEL_WINDOW_OUTLINE            5
#define  PANEL_ZOOM_PLUS                 6       /* callback function: OnZoom */
#define  PANEL_ZOOM                      7       /* callback function: OnZoom */
#define  PANEL_ZOOM_MINUS                8       /* callback function: OnZoom */
#define  PANEL_PICTURE                   9

#define  PANFACE                         7
#define  PANFACE_SELECTED_POINT_TXT_5    2
#define  PANFACE_SELECTED_POINT_TXT_4    3
#define  PANFACE_SELECTED_POINT_TXT_3    4
#define  PANFACE_SELECTED_POINT_TXT_2    5
#define  PANFACE_BKTREE                  6
#define  PANFACE_SELECTED_POINT_TXT_1    7
#define  PANFACE_SELECTION_TXT           8
#define  PANFACE_POINT_PREVIEW_5         9       /* callback function: OnPanelFacePointPreview */
#define  PANFACE_NAME_TXT                10
#define  PANFACE_POINT_PREVIEW_4         11      /* callback function: OnPanelFacePointPreview */
#define  PANFACE_PATTERN_TXT             12
#define  PANFACE_PLERN_5                 13      /* callback function: OnPanelFacePointDefine */
#define  PANFACE_POINT_DELETE_5          14      /* callback function: OnPanelFacePointDelete */
#define  PANFACE_POINT_PREVIEW_3         15      /* callback function: OnPanelFacePointPreview */
#define  PANFACE_DECORATION_3            16
#define  PANFACE_PLERN_4                 17      /* callback function: OnPanelFacePointDefine */
#define  PANFACE_POINT_DELETE_4          18      /* callback function: OnPanelFacePointDelete */
#define  PANFACE_POINT_DEFINITION_5      19
#define  PANFACE_POINT_PREVIEW_2         20      /* callback function: OnPanelFacePointPreview */
#define  PANFACE_DECORATION_6            21
#define  PANFACE_PLERN_3                 22      /* callback function: OnPanelFacePointDefine */
#define  PANFACE_POINT_DELETE_3          23      /* callback function: OnPanelFacePointDelete */
#define  PANFACE_POINT_DEFINITION_4      24
#define  PANFACE_POINT_PREVIEW_1         25      /* callback function: OnPanelFacePointPreview */
#define  PANFACE_BPREVIEW                26      /* callback function: OnPanelFacePreview */
#define  PANFACE_PLERN_2                 27      /* callback function: OnPanelFacePointDefine */
#define  PANFACE_POINT_DELETE_2          28      /* callback function: OnPanelFacePointDelete */
#define  PANFACE_POINT_DEFINITION_3      29
#define  PANFACE_AREA_SIZE               30      /* callback function: OnPanelFacePreview */
#define  PANFACE_SNAME                   31
#define  PANFACE_PLERN_1                 32      /* callback function: OnPanelFacePointDefine */
#define  PANFACE_POINT_DELETE_1          33      /* callback function: OnPanelFacePointDelete */
#define  PANFACE_POINT_DEFINITION_2      34
#define  PANFACE_CANCEL                  35      /* callback function: OnPanelFaceExit */
#define  PANFACE_DECORATION_2            36
#define  PANFACE_OK                      37      /* callback function: OnPanelFaceExit */
#define  PANFACE_PREVIEW                 38
#define  PANFACE_POINT_DEFINITION_1      39
#define  PANFACE_PICTURE                 40
#define  PANFACE_DECORATION_4            41

#define  PANGPATT                        8
#define  PANGPATT_SEARCH_TXT             2
#define  PANGPATT_PATTERN_TXT            3
#define  PANGPATT_DECORATION_6           4
#define  PANGPATT_DECORATION_3           5
#define  PANGPATT_BKTREE                 6
#define  PANGPATT_DECORATION_2           7
#define  PANGPATT_NAME_TXT               8
#define  PANGPATT_MIN_SCORE              9       /* callback function: OnPanelGPatternPreview */
#define  PANGPATT_EDGE_TICKNESS          10      /* callback function: OnPanelGPatternPreview */
#define  PANGPATT_DARK_LEVEL             11      /* callback function: OnPanelGPatternPreview */
#define  PANGPATT_BRIGHT_LEVEL           12      /* callback function: OnPanelGPatternPreview */
#define  PANGPATT_MIN_CONTRAST           13      /* callback function: OnPanelGPatternPreview */
#define  PANGPATT_PLERN                  14      /* callback function: OnPanelGPatternLern */
#define  PANGPATT_RLERN                  15      /* callback function: OnPanelGPatternROI */
#define  PANGPATT_REGION_STR             16
#define  PANGPATT_MIN                    17
#define  PANGPATT_SNAME                  18
#define  PANGPATT_MATCH_MODE             19      /* callback function: OnPanelGPatternPreview */
#define  PANGPATT_DECORATION_5           20
#define  PANGPATT_SREGION                21      /* callback function: OnPanelGPatternSearchRegion */
#define  PANGPATT_BPREVIEW               22      /* callback function: OnPanelGPatternPreview */
#define  PANGPATT_CANCEL                 23      /* callback function: OnPanelGPatternExit */
#define  PANGPATT_OK                     24      /* callback function: OnPanelGPatternExit */
#define  PANGPATT_PICTURE                25
#define  PANGPATT_PREVIEW                26
#define  PANGPATT_DECORATION_4           27
#define  PANGPATT_NORMALIZE              28

#define  PANHIST                         9
#define  PANHIST_HISTOGRAM_TXT           2
#define  PANHIST_DECORATION_3            3
#define  PANHIST_DECORATION_2            4
#define  PANHIST_NAME_TXT                5
#define  PANHIST_BKTREE                  6
#define  PANHIST_BPREVIEW                7       /* callback function: OnPanelHistogramPreview */
#define  PANHIST_OK                      8       /* callback function: OnPanelHistogramExit */
#define  PANHIST_SNAME                   9
#define  PANHIST_REGION_STR              10
#define  PANHIST_PREVIEW                 11
#define  PANHIST_CANCEL                  12      /* callback function: OnPanelHistogramExit */
#define  PANHIST_DECORATION_5            13
#define  PANHIST_SREGION                 14
#define  PANHIST_HISTOGRAM               15
#define  PANHIST_MAX                     16      /* callback function: OnPanelHistogramRange */
#define  PANHIST_MIN                     17      /* callback function: OnPanelHistogramRange */
#define  PANHIST_MIN2                    18      /* callback function: OnSelectedMin */
#define  PANHIST_MIN1                    19      /* callback function: OnSelectedMin */
#define  PANHIST_MIN_PRC                 20

#define  PANMATH                         10
#define  PANMATH_SETTINGS_TXT            2
#define  PANMATH_IMAGE_TXT               3
#define  PANMATH_NAME_TXT                4
#define  PANMATH_BPREVIEW                5       /* callback function: OnPanelMathPreview */
#define  PANMATH_IMAGE_BUFFER            6       /* callback function: OnPanelMathPreview */
#define  PANMATH_MATH_OP                 7       /* callback function: OnPanelMathPreview */
#define  PANMATH_SNAME                   8
#define  PANMATH_DECORATION_6            9
#define  PANMATH_CANCEL                  10      /* callback function: OnPanelMathExit */
#define  PANMATH_OK                      11      /* callback function: OnPanelMathExit */
#define  PANMATH_IMAGE_TXT_4             12
#define  PANMATH_IMAGE_TXT_3             13
#define  PANMATH_CONSTANT                14      /* callback function: OnPanelMathPreview */
#define  PANMATH_PREVIEW                 15
#define  PANMATH_DECORATION              16

#define  PANPATT                         11
#define  PANPATT_SEARCH_TXT              2
#define  PANPATT_PATTERN_TXT             3
#define  PANPATT_DECORATION_6            4
#define  PANPATT_DECORATION_3            5
#define  PANPATT_BKTREE                  6
#define  PANPATT_DECORATION_2            7
#define  PANPATT_NUM_MATCHES             8       /* callback function: OnPanelPatternPreview */
#define  PANPATT_MIN_SCORE               9       /* callback function: OnPanelPatternPreview */
#define  PANPATT_MIN_CONTRAST            10      /* callback function: OnPanelPatternPreview */
#define  PANPATT_NAME_TXT                11
#define  PANPATT_PLERN                   12      /* callback function: OnPanelPatternLern */
#define  PANPATT_RLERN                   13      /* callback function: OnPanelPatternROI */
#define  PANPATT_REGION_STR              14
#define  PANPATT_MATCH_MODE              15      /* callback function: OnPanelPatternPreview */
#define  PANPATT_SNAME                   16
#define  PANPATT_BPREVIEW                17      /* callback function: OnPanelPatternPreview */
#define  PANPATT_DECORATION_5            18
#define  PANPATT_SREGION                 19      /* callback function: OnPanelPatternSearchRegion */
#define  PANPATT_CANCEL                  20      /* callback function: OnPanelPatternExit */
#define  PANPATT_OK                      21      /* callback function: OnPanelPatternExit */
#define  PANPATT_PREVIEW                 22
#define  PANPATT_PICTURE                 23
#define  PANPATT_DECORATION_4            24

#define  PANRES                          12
#define  PANRES_SETTINGS_TXT             2
#define  PANRES_NAME_TXT                 3
#define  PANRES_BPREVIEW                 4       /* callback function: OnPanelResamplePreview */
#define  PANRES_OK                       5       /* callback function: OnPanelResampleExit */
#define  PANRES_PREVIEW                  6
#define  PANRES_CONSTANT                 7       /* callback function: OnPanelResamplePreview */
#define  PANRES_SNAME                    8
#define  PANRES_DECORATION_6             9
#define  PANRES_CANCEL                   10      /* callback function: OnPanelResampleExit */
#define  PANRES_INTERPOL                 11      /* callback function: OnPanelResamplePreview */

#define  PNLSTEPS                        13
#define  PNLSTEPS_PICTURES               2
#define  PNLSTEPS_DECORATION_STEP0       3
#define  PNLSTEPS_IMAGE_TXT              4
#define  PNLSTEPS_TASK_FIRST_IMAGE       5       /* callback function: OnFirstImage */
#define  PNLSTEPS_BKTREE                 6

#define  PNLTASKS                        14
#define  PNLTASKS_TASK_NAME              2
#define  PNLTASKS_STEPTYPE               3
#define  PNLTASKS_STEP_ADD               4       /* callback function: OnStepAdd */
#define  PNLTASKS_OK                     5       /* callback function: OnPanelTaskExit */
#define  PNLTASKS_CANCEL                 6       /* callback function: OnPanelTaskExit */
#define  PNLTASKS_NAME_TXT               7
#define  PNLTASKS_STEP_TXT               8
#define  PNLTASKS_OUTLINE_STEP           9
#define  PNLTASKS_DECORATION_3           10

#define  TABPANEL_GRAPH                  2
#define  TABPANEL_DECORATION             3

#define  TABPANEL_2_HISTOGRAM            2


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK OnBufferImageList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCamera(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCameraConfig(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnConfig(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnCreateBuffer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDirectory(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFirstImage(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnHideChildPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnImgBuffList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnImgGrab(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnImgLoad(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnImgSave(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnImgSnap(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMakeBuffer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelCfgExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelFaceExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelFacePointDefine(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelFacePointDelete(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelFacePointPreview(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelFacePreview(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelGPatternExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelGPatternLern(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelGPatternPreview(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelGPatternROI(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelGPatternSearchRegion(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelHistogramExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelHistogramPreview(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelHistogramRange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelImgbuffExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelImgbuffPreview(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelMathExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelMathPreview(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelPatternExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelPatternLern(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelPatternPreview(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelPatternROI(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelPatternSearchRegion(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelResampleExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelResamplePreview(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanelTaskExit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSelectedMin(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSetCam(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStepAdd(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTaskAdd(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTaskList(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnZoom(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
