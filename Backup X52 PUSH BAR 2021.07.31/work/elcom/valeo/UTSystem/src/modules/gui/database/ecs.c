#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnBKTREE (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnCP (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnCopySourceSelect (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDbDeleteHist (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDbInsert (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDbRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnExportXmlFile (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnGroup (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnHisFillter (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnHisMerge (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnHistList (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnIniLimits (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnOpenXmlFile (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnRightsSelection (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnVtype (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 16;
ExeSymbol __UICallbackSymbols [16] =
{
 {"_OnBKTREE", (void*)OnBKTREE, 1, 0},
 {"_OnCP", (void*)OnCP, 1, 0},
 {"_OnCopySourceSelect", (void*)OnCopySourceSelect, 1, 0},
 {"_OnDbDeleteHist", (void*)OnDbDeleteHist, 1, 0},
 {"_OnDbInsert", (void*)OnDbInsert, 1, 0},
 {"_OnDbRead", (void*)OnDbRead, 1, 0},
 {"_OnExportXmlFile", (void*)OnExportXmlFile, 1, 0},
 {"_OnGroup", (void*)OnGroup, 1, 0},
 {"_OnHisFillter", (void*)OnHisFillter, 1, 0},
 {"_OnHisMerge", (void*)OnHisMerge, 1, 0},
 {"_OnHistList", (void*)OnHistList, 1, 0},
 {"_OnIniLimits", (void*)OnIniLimits, 1, 0},
 {"_OnOpenXmlFile", (void*)OnOpenXmlFile, 1, 0},
 {"_OnRightsSelection", (void*)OnRightsSelection, 1, 0},
 {"_OnTimer", (void*)OnTimer, 1, 0},
 {"_OnVtype", (void*)OnVtype, 1, 0}
};
