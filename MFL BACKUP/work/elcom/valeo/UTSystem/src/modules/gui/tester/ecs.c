#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnDisplay_St3 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDisplay_St4 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnDisplay_St6 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncStation1 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncStation2 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncStation3 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncStation4 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncStation5 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncStation6 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncStation7 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncStation8 (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnFncTester (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 12;
ExeSymbol __UICallbackSymbols [12] =
{
 {"_OnDisplay_St3", OnDisplay_St3, 1, 0},
 {"_OnDisplay_St4", OnDisplay_St4, 1, 0},
 {"_OnDisplay_St6", OnDisplay_St6, 1, 0},
 {"_OnFncStation1", OnFncStation1, 1, 0},
 {"_OnFncStation2", OnFncStation2, 1, 0},
 {"_OnFncStation3", OnFncStation3, 1, 0},
 {"_OnFncStation4", OnFncStation4, 1, 0},
 {"_OnFncStation5", OnFncStation5, 1, 0},
 {"_OnFncStation6", OnFncStation6, 1, 0},
 {"_OnFncStation7", OnFncStation7, 1, 0},
 {"_OnFncStation8", OnFncStation8, 1, 0},
 {"_OnFncTester", OnFncTester, 1, 0}
};
