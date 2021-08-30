#if !defined(__LANG_TRANSLATION_H__)
#define __LANG_TRANSLATION_H__

#include "login.h"

/* language support pattern */


static SLanguageItem LOGINlang[] = {   
 	P(PANEL,				_noop("Panel")),
    L(PANEL_LOGINLIST,			_noop("Login Name:")),
	L(PANEL_PASSWORD,		_noop("Password:")),
	{0 , NULL, 0}
};


#endif    
