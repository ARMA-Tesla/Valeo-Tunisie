#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnBrowseCal (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnCalibScale (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMeasChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPwrConfigure (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPwrDisable (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSave (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSaveCalib (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnScale (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnStart (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnStop (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 11;
ExeSymbol __UICallbackSymbols [11] =
{
 {"_OnBrowseCal", OnBrowseCal, 1, 0},
 {"_OnCalibScale", OnCalibScale, 1, 0},
 {"_OnMeasChanged", OnMeasChanged, 1, 0},
 {"_OnPwrConfigure", OnPwrConfigure, 1, 0},
 {"_OnPwrDisable", OnPwrDisable, 1, 0},
 {"_OnSave", OnSave, 1, 0},
 {"_OnSaveCalib", OnSaveCalib, 1, 0},
 {"_OnScale", OnScale, 1, 0},
 {"_OnStart", OnStart, 1, 0},
 {"_OnStop", OnStop, 1, 0},
 {"_OnTimer", OnTimer, 1, 0}
};
