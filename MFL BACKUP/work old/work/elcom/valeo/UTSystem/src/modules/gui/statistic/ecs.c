#pragma pack(4)
typedef struct {char *name; void *address; unsigned long isFunction:1; unsigned long reserved:31;} ExeSymbol;
int __cdecl OnCalDisplay (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __cdecl OnMonthChange (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int __UICallbackSymbolCount = 2;
ExeSymbol __UICallbackSymbols [2] =
{
 {"_OnCalDisplay", OnCalDisplay, 1, 0},
 {"_OnMonthChange", OnMonthChange, 1, 0}
};
