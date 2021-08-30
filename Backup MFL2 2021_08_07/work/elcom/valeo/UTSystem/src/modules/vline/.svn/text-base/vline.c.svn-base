/****************************************************************************
 *                             V L I N E
 *---------------------------------------------------------------------------
 *    Copyright   2005 ELCOM, a.s.  All Rights Reserved.        
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:       vline.c                                                    
 * Purpose:     Vline interface (Valeo debug line interface)
 * Version:     2.0  
 ****************************************************************************/
#include "vline.h"
#include <elerror.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sdk/mmsystem.h>

#define VLINE_WRITE    1
#define VLINE_READ     2

#define VLINELOG( priority, message)	\
	if ( pVLine->_Log ) { \
		pVLine->_Log( priority, message); \
	} else

/*****************************************************************************
 * Static no-exported functions declaration **********************************
 *****************************************************************************/
static	SElExceptionPtr	vlineLock(
		struct _SVLine* me
);
static	SElExceptionPtr	vlineUnlock(
		struct _SVLine* me
);
static SElExceptionPtr	vlineOpen(
	struct _SVLine*		pVLine,
	const char*			pResourceName
);
static SElExceptionPtr	vlineClose(
	struct _SVLine* me
);
static SElExceptionPtr	vlineInvalidate(
	struct _SVLine* me
);
static SElExceptionPtr	vlineWrite(
	struct _SVLine* me,
	const char* cmd
);
static SElExceptionPtr	vlineQuery(
	struct _SVLine* me,
	const char* cmd,
	char*		buffer,
	uint32_t	size
);
static SElExceptionPtr	vlineReset(
	struct _SVLine*	pVLine
);
static bool_t	vlineIsRoot(
	const char*	pPath
);
static SElExceptionPtr	vlineParsePath(
	const char*	pBuffer,
	char*	pPath,
	uint32_t		pathSize
);
static SElExceptionPtr	vlineParseValue(
	const char*	pBuffer,
	const char*	pcmd,
	char*	pValue,
	int32_t		pathSize
);
static SElExceptionPtr vlineGetFirstNode(
	struct _SVLine* pVLine,
	const char* pCmd,
	char*	node,
	const char** pNext
);
static SElExceptionPtr	vlineWriteNode(
	struct _SVLine* pVLine,
	char*	pPath,
	const char* pNode
);
static SElExceptionPtr	vlineWriteString(
	struct _SVLine*		pVLine,
	const char*			pString
);
static SElExceptionPtr vlineWriteRec(
	struct _SVLine* pVLine,
	char*		path,
	const char*	pCmd
);
static	SElExceptionPtr vlineClearCache(
	struct _SVLine* pVLine
);
static	SElExceptionPtr vlineEnableCache(
	struct _SVLine* pVLine,
		bool_t state
);

static const char* vlineGetCachePtr(
	struct _SVLine* pVLine
);
static SElExceptionPtr VLineSetLogCallback(
	struct _SVLine* me,
	int32_t (_FUNCC *Callback)(uint32_t  priority, const char* message)
);
static SElExceptionPtr AppendCacheData(
    struct _SVLine*   pVLine,
    int32_t           dType,
    const char*       pData
);
static const char* vlineGetRoot(const char* pPath);
static SElExceptionPtr TranslateVlineError( const char* pErrorMessage);
static void RemoveSurroundingWhiteSpace(char *string);

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vline_new"
/*!
 * \brief
 * This function creates a new instance.
 * 
 * \param pError 
 * Returns a value indicating whether an error occurred.
 * A negative error code indicates function failure.	
 * 
 * \return 
 * Pointer to new instance of the VLine object. If the function fails, NULL
 * pointer is returned
 *
 * This function creates a new instance of the VLine object.
 */
 SElExceptionPtr vline_new(
	SVLinePtr*	pVLine
)
{
	SElExceptionPtr    	pexception = NULL;
	int32_t				error = 0;

	*pVLine = calloc( 1, sizeof(SVLine));
	EXCCHECKALLOC( *pVLine);

	(*pVLine)->Open       = vlineOpen;
	(*pVLine)->Close      = vlineClose;
	(*pVLine)->Invalidate = vlineInvalidate;
	(*pVLine)->Write = vlineWrite;
	(*pVLine)->Query = vlineQuery;
	(*pVLine)->Reset = vlineReset;
	(*pVLine)->ClearCache = vlineClearCache;
	(*pVLine)->EnableCache = vlineEnableCache;
	(*pVLine)->GetCachePtr = vlineGetCachePtr;
	(*pVLine)->SetLogCallback = VLineSetLogCallback;
	(*pVLine)->_CacheEnabled  = FALSE;

	InitializeCriticalSection( &((*pVLine)->_Lock));
	
	/* COM Initialization */
	EXCCHECK( sercomm_new(NULL, NULL, &((*pVLine)->_pSerComm)));
	EXCCHECK( elstring_new(NULL, &((*pVLine)->_pCache)));
	EXCCHECK( timer_new( &((*pVLine)->_pTimer)));

Error:
	EXCRETHROW( pexception); 
}/* vline_new */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vline_delete"
/*!
 * \brief
 * This function destroys specified instance.
 * 
 * \param pVLine 
 * Pointer to instance of the VLine object cretaed by vline_new() function.
 * 
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n
 *  The general meaning of the status code is as follows:\n
 *  Value                  Meaning\n
 *  -------------------------------\n
 *  0                      Success\n
 *  Positive Values        Warnings\n
 *  Negative Values        Errors\n\n
 *  This driver defines the following status codes:\n\n
 *  Status    Description\n
 *  -------------------------------------------------\n
 *  WARNINGS: \n
 *  None\n\n
 *  ERRORS:\n
 *  None\n 
 * 
 * This function destroys specified instance and release all resources.
 */
SElExceptionPtr	vline_delete(
	SVLinePtr*	pVLine
)
{
	SElExceptionPtr    pexception = NULL;

	if ( pVLine && *pVLine )
	{
		EXCCHECK( (*pVLine)->Close( *pVLine));
		DeleteCriticalSection( &((*pVLine)->_Lock));

		EXCCHECK( sercomm_delete(&((*pVLine)->_pSerComm)));
		EXCCHECK( elstring_delete( &((*pVLine)->_pCache)));
		EXCCHECK( timer_delete( &((*pVLine)->_pTimer)));

		free( *pVLine);
		*pVLine = NULL;
	}

Error:
	EXCRETHROW( pexception); 
} /* vline_delete */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineOpen"
/*!
 * \brief
 * This function opens VLine (Valeo debug line) interface
 * 
 * \param pVLine
 * Pointer to instance of the VLine object cretaed by vline_new() function.
 * 
 * \patam pResourceName   Resource descriptor.
 *                        Valid resources are:
 *                        "COMxx" where xx means por number for example:"COM3"   
 * 
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n
 *  The general meaning of the status code is as follows:\n
 *  Value                  Meaning\n
 *  -------------------------------\n
 *  0                      Success\n
 *  Positive Values        Warnings\n
 *  Negative Values        Errors\n\n
 *  This driver defines the following status codes:\n\n
 *  Status    Description\n
 *  -------------------------------------------------\n
 *  WARNINGS: \n
 *  None\n\n
 *  ERRORS:\n
 *  VLINE_ERROR_COM_PORT - not valid com port\n 
 *
 */
static SElExceptionPtr	vlineOpen(
	struct _SVLine*		pVLine,
	const char*			pResourceName
)
{
    SElExceptionPtr	pexception = NULL;

	if (strnicmp(pResourceName, "COM", strlen("COM")) == 0 )
	{
		const char*		pres = pResourceName + strlen("COM");
		char*			pend = NULL;
		unsigned long	port = strtoul( pres, &pend, 10);
		
		if ( pend && *pend != '\0' && *pend!=';' || port > UINT16_MAX )
		{
			EXCTHROW( VLINE_ERROR_COM_PORT, "Invalid serial port." );
		}

		EXCCHECK( pVLine->_pSerComm->Open(pVLine->_pSerComm, pResourceName,  "9600/8/n/1/n" ));
		EXCCHECK( pVLine->_pSerComm->SetTerminationCharacter( pVLine->_pSerComm, SERCOMM_BUFFER_READ, TRUE, '>'));
	}
	else
	{
		EXCTHROW( VLINE_ERROR_RESOURCE_NAME, "Invalid resource name." );
	}

Error:
	EXCRETHROW( pexception);
} /* vlineOpen */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vlineClose"
/*!
 * \brief
 * This function close VLine (Valeo debug line) interface
 * 
 * \param pVLine
 * Pointer to instance of the VLine object cretaed by vline_new() function.
 * 
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n
 *  The general meaning of the status code is as follows:\n
 *  Value                  Meaning\n
 *  -------------------------------\n
 *  0                      Success\n
 *  Positive Values        Warnings\n
 *  Negative Values        Errors\n\n
 *  This driver defines the following status codes:\n\n
 *  Status    Description\n
 *  -------------------------------------------------\n
 *  WARNINGS: \n
 *  None\n\n
 *  ERRORS:\n
 *  None\n 
 *
 */
static SElExceptionPtr	vlineClose(
	struct _SVLine*		pVLine
)
{
	SElExceptionPtr	pexception = NULL;

	if ( pVLine->_pSerComm ) 
	{
		EXCCHECK( pVLine->_pSerComm->Close(pVLine->_pSerComm));
    }

Error:
    EXCRETHROW( pexception);
} /* vlineClose */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vlineInvalidate"
/*!
 * \brief
 * This function invalidate vline connection.
 * 
 * \param pVLine
 * Pointer to instance of the VLine object cretaed by vline_new() function.
 * 
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n
 *  The general meaning of the status code is as follows:\n
 *  Value                  Meaning\n
 *  -------------------------------\n
 *  0                      Success\n
 *  Positive Values        Warnings\n
 *  Negative Values        Errors\n\n
 *  This driver defines the following status codes:\n\n
 *  Status    Description\n
 *  -------------------------------------------------\n
 *  WARNINGS: \n
 *  None\n\n
 *  ERRORS:\n
 *  None\n 
 *
 * This function invalidate vline connection. Connection which is invalidated
 * sends Reset before firest write or query.  
 */
static SElExceptionPtr  vlineInvalidate(
	struct _SVLine*		pVLine
)
{
	pVLine->_initialized = FALSE;
	return NULL;
}

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vlineWrite"
/*!
 * \brief
 * This function writes a command to Valeo Debug line
 * 
 * \param pVLine
 * Pointer to instance of the VLine object cretaed by vline_new() function.
 * 
 * \param pCmd  
 * Pointer to command. Command should be defined from the root and every level
 * is separated with '\\'.For examle: '\VER\v' it means "\\VER\\v" 
 *  
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n
 *  The general meaning of the status code is as follows:\n
 *  Value                  Meaning\n
 *  -------------------------------\n
 *  0                      Success\n
 *  Positive Values        Warnings\n
 *  Negative Values        Errors\n\n
 *  This driver defines the following status codes:\n\n
 *  Status    Description\n
 *  -------------------------------------------------\n
 *  WARNINGS: \n
 *  None\n\n
 *  ERRORS:\n
 *  None\n 
 *
 * This function invalidate vline connection. Connection which is invalidated
 * sends Reset before firest write or query.  
 */
static SElExceptionPtr	vlineWrite(
	struct _SVLine*	pVLine,
	const char*	pCmd
)
{
    SElExceptionPtr	pexception = NULL;
	char			path[VLINE_STATIC_PATH+1] = "";

	EXCCHECK( vlineLock(pVLine)); 
	
	/* initialize */
	if ( pVLine->_initialized == FALSE)
	{
		memset(pVLine->_CMDValue, 0, VLINE_STATIC_VALUE);
		memset(pVLine->_CMDPath, 0, VLINE_STATIC_VALUE);
		EXCCHECK( vlineWriteString( pVLine, "\n"));
		pVLine->_initialized = TRUE;
	}

	if ( VLINE_STATIC_PATH <= strlen(pCmd))
	{
		EXCTHROW( VLINE_ERR_LONG_COMMAND, "VLine command too long" );
	}

	EXCCHECK( vlineWriteRec( pVLine, path, pCmd));

Error:
	vlineUnlock(pVLine);
#if 0
	if ( pexception && pexception->GetErrorCode( pexception) == VLINE_ERR_UNKNOWN_COMMAND )
	{
		vlineReset( pVLine);
	}
#endif
	EXCRETHROW( pexception);
} /* vlineWrite */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vlineQuery"
static SElExceptionPtr	vlineQuery(
	struct _SVLine*	pVLine,
	const char*	pCmd,
	char*		pValue,
	uint32_t	size
	
)
{
	SElExceptionPtr	pexception = NULL;
	char			path[VLINE_STATIC_PATH+1] = "";
	uint32_t		length = 0;

	EXCCHECK( vlineLock(pVLine)); 

	/* initialize */
	if ( pVLine->_initialized == FALSE)
	{
		memset(pVLine->_CMDValue, 0, VLINE_STATIC_VALUE);
		memset(pVLine->_CMDPath, 0, VLINE_STATIC_VALUE);
		EXCCHECK( vlineWriteString( pVLine, "\n"));
		pVLine->_initialized = TRUE;
	}

	if ( VLINE_STATIC_PATH <= strlen(pCmd))
	{
		EXCTHROW( VLINE_ERR_LONG_COMMAND, "VLine command too long" );
	}

	EXCCHECK( vlineWriteRec( pVLine, path, pCmd));

	length = (uint32_t)strlen( pVLine->_CMDValue);
	if ( length > 0 && size <= length )
	{
		EXCTHROW(VLINE_ERR_BUFFER_OVERFLOW, "VLine response buffer too small");
	}
	else if ( size > 0 )
	{
		length = ( size < length ) ? size - 1: length;
		memcpy(pValue, pVLine->_CMDValue, length);
		*(pValue + length) = '\0';
	}

Error:
	vlineUnlock(pVLine);
#if 0
	if ( pexception && pexception->GetErrorCode( pexception) == VLINE_ERR_UNKNOWN_COMMAND )
	{
		vlineReset( pVLine);
	}
#endif
	EXCRETHROW( pexception);
} /* vlineQuery */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vlineReset"
static SElExceptionPtr	vlineReset(
	struct _SVLine*	pVLine
)
{
	SElExceptionPtr	pexception = NULL;
	bool_t			is_init = FALSE;
	int32_t			count = 0;
	
	EXCCHECK( vlineLock(pVLine)); 
	
	do
	{
		memset(pVLine->_CMDValue, 0, VLINE_STATIC_VALUE);
		memset(pVLine->_CMDPath, 0, VLINE_STATIC_VALUE);
		EXCCHECK( vlineWriteString( pVLine, "\n"));
		is_init = (NULL!=strstr(pVLine->_CMDPath, ">"));
	}while(!is_init && ++count<5);

	if(count==5)
		EXCTHROW(-1, "Invalid path, response does not contain \">\"!");

	pVLine->_initialized = TRUE;

Error:
	vlineUnlock(pVLine); 
	EXCRETHROW( pexception);
} /* vlineReset */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineClearCache"
/*!
 * \brief
 * This function clears allocated cache.
 * 
 * \param pVLine
 * Pointer to instance of the Vline object cretaed by vline_new() function.
 * 
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n
 *  The general meaning of the status code is as follows:\n
 *  Value                  Meaning\n
 *  -------------------------------\n
 *  0                      Success\n
 *  Positive Values        Warnings\n
 *  Negative Values        Errors\n\n
 *  This driver defines the following status codes:\n\n
 *  Status    Description\n
 *  -------------------------------------------------\n
 *  WARNINGS: \n
 *  None\n\n
 *  ERRORS:\n
 *  None\n 
 *
 * This function clears cache.
 */
static SElExceptionPtr vlineClearCache(
	struct _SVLine* pVLine
)
{
	SElExceptionPtr    pexception = NULL;
     
    EXCCHECK( vlineLock(pVLine));

    if ( pVLine->_pCache != NULL )
    {
        EXCCHECK( (pVLine->_pCache)->Empty(pVLine->_pCache));
    }

Error:
	vlineUnlock(pVLine);
	EXCRETHROW( pexception);
} /* vlineClearCache */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineEnableCache"
/*!
 * \brief
 * This function enables/disables cache for vline communication.
 * 
 * \param pVLine
 * Pointer to instance of the Vline object cretaed by vline_new() function.
 *
 * \param state
 * State of the chache. true - cache enabled
 * 
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n
 *  The general meaning of the status code is as follows:\n
 *  Value                  Meaning\n
 *  -------------------------------\n
 *  0                      Success\n
 *  Positive Values        Warnings\n
 *  Negative Values        Errors\n\n
 *  This driver defines the following status codes:\n\n
 *  Status    Description\n
 *  -------------------------------------------------\n
 *  WARNINGS: \n
 *  None\n\n
 *  ERRORS:\n
 *  None\n 
 *
 * This function clears cache.
 */
static SElExceptionPtr vlineEnableCache(
	struct _SVLine* pVLine,
	bool_t	state
)
{
	pVLine->_CacheEnabled = state;
    return  vlineClearCache( pVLine);
} /* vlineClearCache */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineGetCachePtr"
/*!
 * \brief
 * This function returns pointer to allocated string.
 * 
 * \param pVLine
 * Pointer to instance of the VLine object cretaed by vline_new() function.
 * 
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n
 *  The general meaning of the status code is as follows:\n
 *  Value                  Meaning\n
 *  -------------------------------\n
 *  0                      Success\n
 *  Positive Values        Warnings\n
 *  Negative Values        Errors\n\n
 *  This driver defines the following status codes:\n\n
 *  Status    Description\n
 *  -------------------------------------------------\n
 *  WARNINGS: \n
 *  None\n\n
 *  ERRORS:\n
 *  None\n 
 *
 * This function returns pointer to string witch cache data.
 */
static const char* vlineGetCachePtr(
	struct _SVLine* pVLine
)
{
    static char*  pempty_cahce = "";
    if ( pVLine->_pCache == NULL )
    {
        return pempty_cahce;
    }
    return  (pVLine->_pCache)->GetPtr(pVLine->_pCache);
} /* vlineGetCachePtr */

#define MAX_LEVEL_DEPTH	10
/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vlineWriteRec"
static SElExceptionPtr vlineWriteRec(
	struct _SVLine* pVLine,
	char*		path,
	const char*	pCmd
)
{
	SElExceptionPtr	pexception = NULL;
	const char*		pNext = pCmd;
	char			node[VLINE_STATIC_PATH+1] = "";
	char			required_path[VLINE_STATIC_PATH+1] = "";
	char*			pend = required_path;
	const char*		proot = NULL;
	bool_t			is_full_path = (*path=='\0' && pCmd && *pCmd == '\\');

	do
	{
		EXCCHECK( vlineGetFirstNode( pVLine, pNext, node, &pNext));
		if ( pNext )
		{
			EXCCHECK( vlineWriteNode( pVLine, path, node));
			pend += sprintf(pend, "\\%s", node);
		}
	} while ( pNext != NULL );
	
	if ( is_full_path )
	{
		int32_t count = 0;

    	pend += sprintf(pend, ">");
    	proot = vlineGetRoot( pVLine->_CMDPath);
    
    	while( stricmp(required_path, proot)) 
    	{
			if(++count>MAX_LEVEL_DEPTH)
				EXCTHROW( -1, "Maximal level depth excited!");

    		EXCCHECK( vlineWriteString( pVLine, "x"));
    		proot = vlineGetRoot( pVLine->_CMDPath);
    	}
	}

	EXCCHECK( vlineWriteString( pVLine, node));

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vlineGetFirstNode"
static SElExceptionPtr vlineGetFirstNode(
	struct _SVLine* pVLine,
	const char* pCmd,
	char*	node,
	const char** pNext
)
{
	SElExceptionPtr    pexception = NULL;
	const char*	pend   = NULL;
	const char*	pbegin = pCmd;
	
	pend = strchr(pbegin+1, '\\');
	if ( *pbegin != '\\' && pend != NULL )
	{
		EXCTHROW( VLINE_ERR_NOT_VALID_PATH, "VLine not valid path!");
	}
	else
	{
		if ( *pbegin == '\\' )
		{
			pbegin += 1;
		}
		if ( pend == NULL )
		{
			strcpy( node, pbegin);
		}
		else
		{
			memcpy( node, pbegin, pend - pbegin);
			*(node + (pend - pbegin)) = '\0';
		}
	}

Error:
	*pNext = pend;
    EXCRETHROW( pexception);
} /* vlineGetFirstNode */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "vlineWriteNode"
static SElExceptionPtr	vlineWriteNode(
	struct _SVLine* pVLine,
	char*	pPath,
	const char* pNode
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	const char*		pcurrent_path = vlineGetRoot(pVLine->_CMDPath);
	uint32_t		path_size = (uint32_t)strlen( pPath);
	char			path[VLINE_STATIC_PATH+1] = "";
	size_t			cmp_length;

	if ( path_size > 0 )
	{
		if ( 0 != strnicmp( pcurrent_path, pPath, path_size))
		{
            EXCTHROW( VLINE_ERR_INVALID_INTERNAL_PATH, "VLine invalid internal path!");
		} 
	}

	sprintf( path, "%s\\%s", pPath, pNode);
	cmp_length = strlen(path);
	if ( 0 != strnicmp( pcurrent_path, path, cmp_length) )
	{
		if ( ( vlineIsRoot(pcurrent_path) == FALSE ) &&
		     (cmp_length < strlen( pcurrent_path )) )
		{
			do
			{
				EXCCHECK( vlineWriteString( pVLine, "x\n"));

				pcurrent_path = vlineGetRoot(pVLine->_CMDPath);
				path_size = (uint32_t)(strlen(pcurrent_path)-1);
			} while ( ( strnicmp( pcurrent_path, pPath, path_size) != 0 ) 
					   && vlineIsRoot(pcurrent_path) == FALSE);
		}	
		EXCCHECK( vlineWriteString( pVLine, pNode));
	}

Error:
	if(pexception == NULL)
	{
		sprintf( pPath, "%s\\%s", pPath, pNode);
	}
    EXCRETHROW( pexception);
}/* vlineWriteNode */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineWriteString"
static SElExceptionPtr	vlineWriteString(
	struct _SVLine*		pVLine,
	const char*			pString
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	DWORD			size = (DWORD)(( pString ) ? strlen( pString ) : 0);
	char			buffer[1024];
	const char*		pcommand = pString;
	char			command[256] = "";
	DWORD			rd_count = 0, wrt_count = 0;
	int				index = 0;
	char			message[256] = "";
	static int				is_initialized = 0;
	static LARGE_INTEGER	last_write;
	static LARGE_INTEGER	_qw_freq;

	if(size > 0)
	{
        if ( *(pString + (size -1)) != '\n' )
		{
			sprintf( command, "%s\n", pString);
			pcommand = command;
		}

		if ( !is_initialized )
		{
			EXCCHECKWIN( QueryPerformanceCounter(&last_write));
			EXCCHECKWIN( QueryPerformanceFrequency(&_qw_freq));
			is_initialized = 1;
			Sleep( pVLine->_write_delay);
		}
		else
		{
			LARGE_INTEGER	ctime;
			double			time_diff;

			EXCCHECKWIN( QueryPerformanceCounter(&ctime));

			time_diff = (((double)(ctime.QuadPart - last_write.QuadPart)) / _qw_freq.QuadPart) * 1000.0;
			time_diff =  (double)pVLine->_write_delay - time_diff;
			if ( time_diff >= 1.0 && time_diff < pVLine->_write_delay )
			{
				timeBeginPeriod(1);
				Sleep( (DWORD)time_diff);
				timeEndPeriod(1);
			}
		}

		{
			int count = 0;
			do {
				if (count) 
				{
					timeBeginPeriod(1);
					Sleep(pVLine->_write_delay);
					timeEndPeriod(1);
				}
				if ( pexception ) {
					memset( buffer, 0, sizeof(buffer));
					EXCDELETE( pexception); 
				}
				EXCCHECK( pVLine->_pSerComm->Write(pVLine->_pSerComm, pcommand, (uint32_t)strlen(pcommand)));
				EXCCHECK( AppendCacheData( pVLine, VLINE_WRITE, pString));
				pexception = pVLine->_pSerComm->Read(pVLine->_pSerComm, sizeof(buffer), buffer, &rd_count, 400);
			} while ( pexception && ++count < 2);
			EXCCHECK( pexception);
		}

		EXCCHECKWIN( QueryPerformanceCounter(&last_write));

		buffer[rd_count]='\0';
        EXCCHECK( AppendCacheData( pVLine, VLINE_READ, buffer));		
		EXCCHECK( vlineParsePath( buffer, pVLine->_CMDPath, VLINE_STATIC_PATH));
		EXCCHECK( vlineParseValue( buffer, pcommand, pVLine->_CMDValue, VLINE_STATIC_VALUE));
	}

Error:
    EXCRETHROW( pexception);
} /* vlineWriteString */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineParsePath"
static SElExceptionPtr	vlineParsePath(
	const char*	pBuffer,
	char*		pPath,
	uint32_t	pathSize
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	const char		*pend = pBuffer + strlen(pBuffer);
	const char		*pbegin = pend;

	while (pbegin > pBuffer && *pbegin != '\n')
	{
		pbegin--;
	}
	if ( *pbegin == '\n' )
	{
		uint32_t	size = (uint32_t)(pend-pbegin);
		if ( size > pathSize )
		{
			error = size;
			size = pathSize-1; 
		}
		if ( size > 0 )
		{
			memcpy( pPath, pbegin+1, size); 
		}
	}

/* Error: */
	EXCRETHROW( pexception);  
} /* vlineParsePath */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineParseValue"
static SElExceptionPtr	vlineParseValue(
	const char*	pBuffer,
	const char*	pcmd,
	char*		pValue,
	int32_t		valueSize
)
{
	SElExceptionPtr	pexception = NULL;

#if 1
		/* Hot Fix */
	for ( ; *pBuffer == '\\' ; pBuffer++ )
		;
#endif

	if ( 0 == strnicmp(pBuffer, pcmd, strlen(pcmd)) )
	{
		const char* pvalue = pBuffer + strlen(pcmd);
		const char* pend   = strchr(pvalue, '\n');

		if ( pend != NULL )
		{
			int32_t	count = (int32_t)((pend - pvalue)-1);

			if( count>=0)
			{
				count = (count >= valueSize)? valueSize : count;
				memcpy( pValue, pvalue, count);
				pValue[count] = '\0';
			}
		}
		else
		{
			/*
			 *  TODO: Add error checking
			 */			 
			pValue[0] = '\0';
		}
	}
	
	RemoveSurroundingWhiteSpace(pValue);
	if (pValue[0] == '!')
	{
		EXCCHECK( TranslateVlineError(pValue));
	}

Error:
	EXCRETHROW( pexception); 
} /* vlineParseValue */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Checks if the specified path is root or not.
 *
 * \param pPath		current path in Valeo debug line.
 * \return	TRUE if the specifed path is root, otherwise FALSE
 *
 * Checks if the specified path is root. Root path does not contain '\'
 * character.
 * For example  ">" is root
 *              "FLP3MAIN>" is root
 *              "FLP3AUX\HWI>" is NOT root
 */
#undef __FUNC__
#define __FUNC__ "vlineIsRoot"
static bool_t	vlineIsRoot(
	const char*	pPath
)
{
	bool_t status = FALSE;
	if ( pPath )
	{
		status = (strchr(pPath, '\\') == NULL);
	}
	return status;
} /* vlineIsRoot */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "VLineSetLogCallback"
static SElExceptionPtr VLineSetLogCallback(
	struct _SVLine* pVLine,
	int32_t (_FUNCC *Callback)(uint32_t  priority, const char* message)
)
{
	pVLine->_Log = Callback;
	
	return NULL;
} /* VLineSetLogCallback */


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "AppendCacheData"
static SElExceptionPtr AppendCacheData(
    struct _SVLine*   pVLine,
    int32_t           dType,
    const char*       pData
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	static bool_t	was_reset = FALSE;

	if ( pVLine->_CacheEnabled != TRUE )
	{
		goto Error;
	}

	{
		const char* prefix = (dType == VLINE_WRITE ) ? "[%0.3Lf ms] W: " : "[%0.3Lf ms] R: "; 
		EXCCHECK( (pVLine->_pCache)->Format(pVLine->_pCache,
											prefix,
											(pVLine->_pTimer)->GetTimeDiff( pVLine->_pTimer)
											));
	}
	
	if (*pData == '\n' && dType == VLINE_WRITE)
	{
		was_reset = TRUE;
		EXCCHECK( (pVLine->_pCache)->Append(pVLine->_pCache, "RESET\n"));
	}
	else
	{
		size_t	size = (pData) ? strlen(pData) : 0;
		const char*	pbegin = pData;
		
		if ( was_reset && dType == VLINE_READ )
		{
			for ( ; *pbegin && *pbegin != '\n'; pbegin++ )
				;
			was_reset = FALSE;
			EXCCHECK( (pVLine->_pCache)->Append(pVLine->_pCache, pbegin));
		}
		else
		{
			was_reset = FALSE;
			EXCCHECK( (pVLine->_pCache)->Append(pVLine->_pCache, pData));
		}

		
		if ( size && pData[size] != '\n' )
		{
			EXCCHECK( (pVLine->_pCache)->Append(pVLine->_pCache, "\n"));
		}
	}

Error:
	EXCRETHROW( pexception); 
} /* AppendCacheData */


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineGetRoot"
/*!
 * \brief Returns pointer to root of vline debug path.
 *
 * \param pPath Specifies path of valeo debug line.
 * \return pointer to begin of the path without prefix.
 * 
 * This function skip the prefix and returns pointer to character which specifies
 * root of the path.\n
 * For example:\n
 *             ">" returns ">"
 *             "FLP3MAIN>" returns ">"
 *             "FLP3AUX\HWI>" returns "\HWI>"
 */
static const char* vlineGetRoot(const char* pPath)
{
	const char*		proot = strchr(pPath, '\\');
	if ( proot == NULL )
	{
		proot = strchr(pPath, '>');
	}
	return ( proot ) ? proot : pPath; 
} /* vlineGetRoot */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineLock"
static	SElExceptionPtr	vlineLock(
		struct _SVLine*   pVLine
)
{
	EnterCriticalSection( &(pVLine->_Lock));

	return NULL;
} /* vlineLock */
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "vlineUnlock"
static	SElExceptionPtr	vlineUnlock(
		struct _SVLine*   pVLine
)
{
	LeaveCriticalSection( &(pVLine->_Lock));
	return NULL;
} /* vlineUnlock */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "TranslateVlineError"
/*!
 * \brief Returns numeric representation of VALEO debug error message. 
 *
 * \param pErrorMessage Valeo debug error message
 * 
 * \return Returns numeric representation of VALEO debug error message.\n
 * Valid values are: \n
 * * VLINE_ERR_UNKNOWN_SPECIFIC_ERROR
 * * VLINE_ERR_UNKNOWN_COMMAND
 * * VLINE_ERR_ARG_NUMBER_OVERFLOW
 * * VLINE_ERR_NO_MEM_FOR_NEW_LEVEL
 * * VLINE_ERR_PARAMETER_NB_ERROR
 * * VLINE_ERR_PARAMETER_ERROR
 */
static SElExceptionPtr TranslateVlineError( const char* pErrorMessage)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i;
	
	static struct 
	{ 
		int32_t		error; 
		const char*	message;
	} s_error_messages[] = {
							{VLINE_ERR_UNKNOWN_COMMAND, 	 "! Unknown Command" },
							{VLINE_ERR_UNKNOWN_COMMAND, 	 "! Unknown cmd" },
							{VLINE_ERR_ARG_NUMBER_OVERFLOW,  "! Arg number overflow" },
							{VLINE_ERR_NO_MEM_FOR_NEW_LEVEL, "! No mem for new level" },
							{VLINE_ERR_PARAMETER_NB_ERROR, 	 "! Incorrect nb. of par." },
							{VLINE_ERR_PARAMETER_ERROR, 	 "! Error in par." },
							{VLINE_ERR_FORMAT_ERROR, 		 "! Format Error" },
							{0, NULL}
						   };

	for(i=0; s_error_messages[i].message; i++ )
	{
		if (strcmp(s_error_messages[i].message, pErrorMessage) == 0)
		{
			EXCTHROW( s_error_messages[i].error, s_error_messages[i].message);
			break;
		}
	}
	
	if ( pexception==NULL )
	{
		char	serror[256];

		sprintf(serror, "Vline unknown specific error: %s", pErrorMessage);
		EXCTHROW( VLINE_ERR_UNKNOWN_SPECIFIC_ERROR, serror);
	}

Error:
	EXCRETHROW( pexception);
} /* TranslateVlineError */

/*---------------------------------------------------------------------------*/
static char* SkipWhiteSpace(const char *s)
{
    if (s)
        while (*s && isspace((unsigned char)*s))
            s++;
    return (char*)s;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RemoveSurroundingWhiteSpace"
static void RemoveSurroundingWhiteSpace(char *string)
{
    char *start, *end, *lastNonWS;
    size_t lengthWithoutLeadingWS;

    if (string)
	{
        start = SkipWhiteSpace(string);
        lengthWithoutLeadingWS = strlen(start);
        memmove (string, start, lengthWithoutLeadingWS);

        lastNonWS = end = string;
        while (end < string+lengthWithoutLeadingWS)
        {
            if (!isspace(*end))
                lastNonWS  = end;
			end++;
        }

        if (end != string)
		{
			lastNonWS++;
		}
        *lastNonWS = '\0';
	}
}