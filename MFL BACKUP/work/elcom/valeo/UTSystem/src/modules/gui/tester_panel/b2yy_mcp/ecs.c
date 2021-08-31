#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnB1Reley (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnB2Reley (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnBlower (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnEnable (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPower (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 6;
ExeSymbol __UICallbackSymbols [6] =
{
 {"_OnB1Reley", OnB1Reley, 1, 0},
 {"_OnB2Reley", OnB2Reley, 1, 0},
 {"_OnBlower", OnBlower, 1, 0},
 {"_OnEnable", OnEnable, 1, 0},
 {"_OnPower", OnPower, 1, 0},
 {"_OnTimer", OnTimer, 1, 0}
};
