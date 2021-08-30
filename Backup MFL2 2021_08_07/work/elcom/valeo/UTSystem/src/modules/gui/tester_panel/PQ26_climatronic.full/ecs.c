#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnB1Reley (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnB2Reley (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnButtonRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnConnectAll (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDMMRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDcMotors (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDcMotorsSignals (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDcMotorsValue (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDisconnectAll (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDisplay (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnEnableCan (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnEnableVline (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnLed (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMemoryRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMode (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPwm (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSenRead (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnVline (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 18;
ExeSymbol __UICallbackSymbols [18] =
{
 {"_OnB1Reley", OnB1Reley, 1, 0},
 {"_OnB2Reley", OnB2Reley, 1, 0},
 {"_OnButtonRead", OnButtonRead, 1, 0},
 {"_OnConnectAll", OnConnectAll, 1, 0},
 {"_OnDMMRead", OnDMMRead, 1, 0},
 {"_OnDcMotors", OnDcMotors, 1, 0},
 {"_OnDcMotorsSignals", OnDcMotorsSignals, 1, 0},
 {"_OnDcMotorsValue", OnDcMotorsValue, 1, 0},
 {"_OnDisconnectAll", OnDisconnectAll, 1, 0},
 {"_OnDisplay", OnDisplay, 1, 0},
 {"_OnEnableCan", OnEnableCan, 1, 0},
 {"_OnEnableVline", OnEnableVline, 1, 0},
 {"_OnLed", OnLed, 1, 0},
 {"_OnMemoryRead", OnMemoryRead, 1, 0},
 {"_OnMode", OnMode, 1, 0},
 {"_OnPwm", OnPwm, 1, 0},
 {"_OnSenRead", OnSenRead, 1, 0},
 {"_OnVline", OnVline, 1, 0}
};
