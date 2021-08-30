#include "global.h"

static SGlobal		gs_Global;
static SGlobalPtr	gs_GlobalPtr = NULL;


SGlobalPtr GetGlobal( SGlobalPtr* pGlobal)
{
	if ( gs_GlobalPtr == NULL )
	{
		memset(&gs_Global, 0 , sizeof(SGlobal));
		gs_GlobalPtr = &gs_Global;
	}
	
	if ( pGlobal ) *pGlobal = gs_GlobalPtr;

	return gs_GlobalPtr;
}


