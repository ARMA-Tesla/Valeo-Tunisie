#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnError (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnErrorExit (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSAP (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSAPErrorTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnTimerBox (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 6;
ExeSymbol __UICallbackSymbols [6] =
{
 {"_OnError", OnError, 1, 0},
 {"_OnErrorExit", OnErrorExit, 1, 0},
 {"_OnSAP", OnSAP, 1, 0},
 {"_OnSAPErrorTimer", OnSAPErrorTimer, 1, 0},
 {"_OnTimer", OnTimer, 1, 0},
 {"_OnTimerBox", OnTimerBox, 1, 0}
};
