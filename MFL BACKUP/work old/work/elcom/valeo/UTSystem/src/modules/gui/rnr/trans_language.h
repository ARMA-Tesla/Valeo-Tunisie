#if !defined(__LANG_TRANSLATION_H__)
#define __LANG_TRANSLATION_H__

#include "rnr.h"

/* language support pattern */



static SLanguageItem RNRlang[] = {   
 	P(PANEL,					_noop("Rnr")),
    V(PANEL_MSG_PANEL,			_noop("Panel")),
	V(PANEL_MSG_OPERATOR,		_noop("Operator")),
	V(PANEL_MSG_ROUND,			_noop("Round")),
	{0 , NULL, 0}
};
#endif    
