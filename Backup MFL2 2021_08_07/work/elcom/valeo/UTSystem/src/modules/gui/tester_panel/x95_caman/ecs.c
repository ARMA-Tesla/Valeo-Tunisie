#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnConnect (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnK7Resistance (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnL38Connect (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnL38Dmm (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnL38Rele (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnL38Timer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnLed (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMemoryRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMode (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPower (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPwm (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSenRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnVline (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 13;
ExeSymbol __UICallbackSymbols [13] =
{
 {"_OnConnect", OnConnect, 1, 0},
 {"_OnK7Resistance", OnK7Resistance, 1, 0},
 {"_OnL38Connect", OnL38Connect, 1, 0},
 {"_OnL38Dmm", OnL38Dmm, 1, 0},
 {"_OnL38Rele", OnL38Rele, 1, 0},
 {"_OnL38Timer", OnL38Timer, 1, 0},
 {"_OnLed", OnLed, 1, 0},
 {"_OnMemoryRead", OnMemoryRead, 1, 0},
 {"_OnMode", OnMode, 1, 0},
 {"_OnPower", OnPower, 1, 0},
 {"_OnPwm", OnPwm, 1, 0},
 {"_OnSenRead", OnSenRead, 1, 0},
 {"_OnVline", OnVline, 1, 0}
};
