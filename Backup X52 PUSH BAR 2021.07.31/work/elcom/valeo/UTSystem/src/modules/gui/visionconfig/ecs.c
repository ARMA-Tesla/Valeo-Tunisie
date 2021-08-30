#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnBufferImageList (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnCamera (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnCameraConfig (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnConfig (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnCreateBuffer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDirectory (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFirstImage (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnHideChildPanel (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnImgBuffList (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnImgGrab (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnImgLoad (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnImgSave (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnImgSnap (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMakeBuffer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanel (int panel, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelCfgExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelFaceExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelFacePointDefine (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelFacePointDelete (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelFacePointPreview (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelFacePreview (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelGPatternExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelGPatternLern (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelGPatternPreview (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelGPatternROI (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelGPatternSearchRegion (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelHistogramExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelHistogramPreview (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelHistogramRange (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelImgbuffExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelImgbuffPreview (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelMathExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelMathPreview (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelPatternExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelPatternLern (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelPatternPreview (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelPatternROI (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelPatternSearchRegion (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelResampleExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelResamplePreview (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelTaskExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSelectedMin (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSetCam (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnStepAdd (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnTaskAdd (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnTaskList (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnZoom (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 48;
ExeSymbol __UICallbackSymbols [48] =
{
 {"_OnBufferImageList", OnBufferImageList, 1, 0},
 {"_OnCamera", OnCamera, 1, 0},
 {"_OnCameraConfig", OnCameraConfig, 1, 0},
 {"_OnConfig", OnConfig, 1, 0},
 {"_OnCreateBuffer", OnCreateBuffer, 1, 0},
 {"_OnDirectory", OnDirectory, 1, 0},
 {"_OnExit", OnExit, 1, 0},
 {"_OnFirstImage", OnFirstImage, 1, 0},
 {"_OnHideChildPanel", OnHideChildPanel, 1, 0},
 {"_OnImgBuffList", OnImgBuffList, 1, 0},
 {"_OnImgGrab", OnImgGrab, 1, 0},
 {"_OnImgLoad", OnImgLoad, 1, 0},
 {"_OnImgSave", OnImgSave, 1, 0},
 {"_OnImgSnap", OnImgSnap, 1, 0},
 {"_OnMakeBuffer", OnMakeBuffer, 1, 0},
 {"_OnPanel", OnPanel, 1, 0},
 {"_OnPanelCfgExit", OnPanelCfgExit, 1, 0},
 {"_OnPanelFaceExit", OnPanelFaceExit, 1, 0},
 {"_OnPanelFacePointDefine", OnPanelFacePointDefine, 1, 0},
 {"_OnPanelFacePointDelete", OnPanelFacePointDelete, 1, 0},
 {"_OnPanelFacePointPreview", OnPanelFacePointPreview, 1, 0},
 {"_OnPanelFacePreview", OnPanelFacePreview, 1, 0},
 {"_OnPanelGPatternExit", OnPanelGPatternExit, 1, 0},
 {"_OnPanelGPatternLern", OnPanelGPatternLern, 1, 0},
 {"_OnPanelGPatternPreview", OnPanelGPatternPreview, 1, 0},
 {"_OnPanelGPatternROI", OnPanelGPatternROI, 1, 0},
 {"_OnPanelGPatternSearchRegion", OnPanelGPatternSearchRegion, 1, 0},
 {"_OnPanelHistogramExit", OnPanelHistogramExit, 1, 0},
 {"_OnPanelHistogramPreview", OnPanelHistogramPreview, 1, 0},
 {"_OnPanelHistogramRange", OnPanelHistogramRange, 1, 0},
 {"_OnPanelImgbuffExit", OnPanelImgbuffExit, 1, 0},
 {"_OnPanelImgbuffPreview", OnPanelImgbuffPreview, 1, 0},
 {"_OnPanelMathExit", OnPanelMathExit, 1, 0},
 {"_OnPanelMathPreview", OnPanelMathPreview, 1, 0},
 {"_OnPanelPatternExit", OnPanelPatternExit, 1, 0},
 {"_OnPanelPatternLern", OnPanelPatternLern, 1, 0},
 {"_OnPanelPatternPreview", OnPanelPatternPreview, 1, 0},
 {"_OnPanelPatternROI", OnPanelPatternROI, 1, 0},
 {"_OnPanelPatternSearchRegion", OnPanelPatternSearchRegion, 1, 0},
 {"_OnPanelResampleExit", OnPanelResampleExit, 1, 0},
 {"_OnPanelResamplePreview", OnPanelResamplePreview, 1, 0},
 {"_OnPanelTaskExit", OnPanelTaskExit, 1, 0},
 {"_OnSelectedMin", OnSelectedMin, 1, 0},
 {"_OnSetCam", OnSetCam, 1, 0},
 {"_OnStepAdd", OnStepAdd, 1, 0},
 {"_OnTaskAdd", OnTaskAdd, 1, 0},
 {"_OnTaskList", OnTaskList, 1, 0},
 {"_OnZoom", OnZoom, 1, 0}
};
