#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnApply (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnChannelChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnClear (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnCmdChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnLoopEnabled (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnLoopList (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMultEnabled (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnRecChanged (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSend1 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSend2 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSizeChange (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 11;
ExeSymbol __UICallbackSymbols [11] =
{
 {"_OnApply", OnApply, 1, 0},
 {"_OnChannelChanged", OnChannelChanged, 1, 0},
 {"_OnClear", OnClear, 1, 0},
 {"_OnCmdChanged", OnCmdChanged, 1, 0},
 {"_OnLoopEnabled", OnLoopEnabled, 1, 0},
 {"_OnLoopList", OnLoopList, 1, 0},
 {"_OnMultEnabled", OnMultEnabled, 1, 0},
 {"_OnRecChanged", OnRecChanged, 1, 0},
 {"_OnSend1", OnSend1, 1, 0},
 {"_OnSend2", OnSend2, 1, 0},
 {"_OnSizeChange", OnSizeChange, 1, 0}
};
