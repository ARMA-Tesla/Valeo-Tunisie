#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnConnect (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDisplay (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnLed (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMemoryRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMode (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPwm (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPwm16 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSenRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnVline (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 9;
ExeSymbol __UICallbackSymbols [9] =
{
 {"_OnConnect", OnConnect, 1, 0},
 {"_OnDisplay", OnDisplay, 1, 0},
 {"_OnLed", OnLed, 1, 0},
 {"_OnMemoryRead", OnMemoryRead, 1, 0},
 {"_OnMode", OnMode, 1, 0},
 {"_OnPwm", OnPwm, 1, 0},
 {"_OnPwm16", OnPwm16, 1, 0},
 {"_OnSenRead", OnSenRead, 1, 0},
 {"_OnVline", OnVline, 1, 0}
};
