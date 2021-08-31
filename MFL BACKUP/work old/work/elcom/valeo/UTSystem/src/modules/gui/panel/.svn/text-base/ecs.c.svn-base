#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnButton (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnF (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMenu (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanel (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnPanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnSubMenu (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 6;
ExeSymbol __UICallbackSymbols [6] =
{
 {"_OnButton", OnButton, 1, 0},
 {"_OnF", OnF, 1, 0},
 {"_OnMenu", OnMenu, 1, 0},
 {"_OnPanel", OnPanel, 1, 0},
 {"_OnPanelCallback", OnPanelCallback, 1, 0},
 {"_OnSubMenu", OnSubMenu, 1, 0}
};
