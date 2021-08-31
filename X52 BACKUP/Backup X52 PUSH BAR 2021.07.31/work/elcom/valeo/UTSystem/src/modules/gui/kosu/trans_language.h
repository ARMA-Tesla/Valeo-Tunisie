#if !defined(__LANG_TRANSLATION_H__)
#define __LANG_TRANSLATION_H__

#include "kosu.h"

/* language support pattern */



static SLanguageItem KOSUlang[] = {   
 	P(PANEL,				_noop("Kosu")),
    V(PANEL_MSG_GOOD,		_noop("Good")),
	V(PANEL_MSG_BAD,		_noop("Bad")),
	//V(PANEL_MSG_PAUSE,		_noop("Pause")),
//	V(PANEL_MSG_PANEL,		_noop("Panels")), 
	V(PANEL_MSG_OPERATOR,	_noop("Operators")), 
//	V(PANEL_MSG_END,		_noop("End")), 
	{0 , NULL, 0}
};
#endif    
