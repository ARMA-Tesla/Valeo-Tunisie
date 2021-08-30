#include <ansi_c.h>
#include "sercomm.h"
#include <elerror.h>
#include <stdlib.h>

#if 0	
#include <stdio.h>
#define LOG( level, msg) do { printf(msg); printf("\n"); } while(0)
#define LOG_B( level, msg) do { printf(msg
#define LOG_P( param)  ,param
#define LOG_E()		); printf("\n"); } while(0)
#else
#define LOG( level, msg)
#define LOG_B( level, msg)
#define LOG_P( param)
#define LOG_E()
#endif

#if !defined(ARRAY_CNT)
#define ARRAY_CNT( arr) sizeof(arr)/sizeof(*arr)
#endif

static struct {
	BYTE id;
	char *str;
} gs_parity[] = {
	NOPARITY,		"None",
	EVENPARITY,		"Even",
	ODDPARITY,		"Odd",
	MARKPARITY,		"Mark",
	SPACEPARITY,	"Space"
};

static struct {
	BYTE id;
	char *str;
} gs_stopbits[] = {
	ONESTOPBIT,		"1",
	ONE5STOPBITS,	"1.5",
	TWOSTOPBITS,	"2",
};

static struct {
	DWORD id;
	char *str;
} gs_comm_evt[] = {
#define E(id)	EV_##id,	#id
	/* taken from <winbase.h> */
	E(RXCHAR),		// Any Character received
	E(RXFLAG),		// Received certain character
	E(TXEMPTY),		// Transmitt Queue Empty
	E(CTS),			// CTS changed state
	E(DSR),			// DSR changed state
	E(RLSD),		// RLSD (CD) changed state
	E(ERR),			// Line status error occurred
	E(RING),		// Ring signal detected
	E(RX80FULL),	// Receive buffer is 80 percent full
#if 1
	/* Virtual Serial Port from www.mks.zp.ua doesn't support these events: */
	E(BREAK),		// BREAK received
	E(PERR),		// Printer error occured
	E(EVENT1),		// Provider specific event 1
	E(EVENT2),		// Provider specific event 2
#endif
#undef E
};
#define COMM_EVTs	ARRAY_CNT(gs_comm_evt)


static SElExceptionPtr sercommClose(
    struct _SSerComm* pSerComm
);
static SElExceptionPtr sercommOpen(
    struct _SSerComm* pSerComm,
    const char* pResource,
    const char* pOptionString
);
static SElExceptionPtr sercommWrite(
    struct _SSerComm* pSerComm,
    const uint8_t*    pCmd,
    uint32_t          cmdlen
);
static SElExceptionPtr sercommReadAll(
    struct _SSerComm* pSerComm,
    uint32_t bufsize,
    uint8_t *buf,
    uint32_t* rdCount,
    int32_t timeout
);
static SElExceptionPtr sercommRead(
    struct _SSerComm* pSerComm,
    uint32_t bufsize,
    uint8_t *buf,
    uint32_t* rdCount,
    int32_t timeout
);
static SElExceptionPtr sercommSetTerminationCharacter(
    struct _SSerComm* pSerComm,
    int32_t			  typeofbuffer,
    bool_t			  enabled,
    uint8_t			  tch
);

static SElExceptionPtr dcb_set(DCB *dcb, int argc, const char *argv[]);
static SElExceptionPtr ols_wait_for_overlapped(
	struct _SSerComm* pSerComm,
    DWORD *p_len,
    DWORD timeout
);
static SElExceptionPtr ols_wait_comm_event(
    struct _SSerComm* pSerComm,
    DWORD *pmask,
    long timeout
);
static void ols_clear_error(struct _SSerComm* pSerComm);
static SElExceptionPtr ols_io_abort_reset(struct _SSerComm* pSerComm);
static SElExceptionPtr ols_io_abort_pending(struct _SSerComm* pSerComm);

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "sercomm_new"
SElExceptionPtr sercomm_new(
    const char* pName,
    const char* pOptionString,
	SSerCommPtr* pSerComm
)
{
	SElExceptionPtr    	pexception = NULL;
	SSerCommPtr	psercomm = NULL;

	psercomm = calloc( 1, sizeof(SSerComm));
	EXCCHECKALLOC( psercomm);
	
	*pSerComm = psercomm;

	psercomm->Open       = sercommOpen;
	psercomm->Close      = sercommClose;
	psercomm->Write      = sercommWrite;
	psercomm->Read       = sercommReadAll;
	psercomm->SetTerminationCharacter = sercommSetTerminationCharacter;

	/* COM Initialization */
    psercomm->_write_delay         = -1;

Error:
    EXCRETHROW( pexception); 
}/* sercomm_new */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "sercomm_delete"
SElExceptionPtr sercomm_delete(
    SSerCommPtr* pSerComm
)
{
	SElExceptionPtr	pexception = NULL; 
	SSerCommPtr		psercomm = NULL;

    if ( pSerComm && *pSerComm )
    {
        EXCCHECK( (*pSerComm)->Close(*pSerComm));
        free( *pSerComm);
        *pSerComm = NULL;
    }

Error:
	EXCRETHROW( pexception); 
}/* sercomm_new */


/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "sercommClose"
static SElExceptionPtr sercommClose(
    struct _SSerComm* pSerComm
)
{
	SElExceptionPtr    	pexception = NULL; 
	int32_t				error = 0;
	SSerCommPtr			psercomm = NULL;

	if ( pSerComm->_fh )
	{
    	EXCCHECKWIN( CloseHandle(pSerComm->_fh));

#if !defined(NO_OVERLAPPED)
		CloseHandle(pSerComm->o.ReadWriteFile.hEvent);
		CloseHandle(pSerComm->o.WaitCommEvent.hEvent);
#endif
    	CloseHandle(pSerComm->abort.hEvent);
    }

Error:
    pSerComm->_fh = 0;
	EXCRETHROW( pexception); 
}/* sercommClose */


/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "sercommOpen"
/*!
 *
 * \param pSerComm 
 * \param pResource 
 * \param pOptionString
 *        Attributes are separated either by '/' or ','. The comma (',') is
 *        used when the user wants to use full specification of hand-shake
 *        mode that contains slash ('/') (e.g. "XON/XOFF" and others):
 *        Example:
 *        <baudrate>,<bytesize>,<parity>,<stopbits>,<hand-shake>,
 * 
 */  
static SElExceptionPtr sercommOpen(
    struct _SSerComm* pSerComm,
    const char* pResource,
    const char* pOptionString
)
{
	SElExceptionPtr	pexception = NULL;
    int32_t       	error = 0;
	HANDLE         	fh = NULL;
	DCB            	dcb;
	COMMTIMEOUTS   	timeouts;
	DWORD          	mask;
	int            	size;
	register int   	i;


    LOG_B( ELOG_DEBUG, __FUNC__"(): resource: '%s', options: '%s'")
        LOG_P(pResource)
        LOG_P(pOptionString)
    LOG_E();
    
	fh = CreateFile(
					pResource,				// LPCTSTR lpFileName
					GENERIC_READ | GENERIC_WRITE,
								// DWORD dwDesiredAccess
#if 1
		0,						// DWORD dwShareMode
#else
		FILE_SHARE_READ | FILE_SHARE_WRITE,
#endif
		NULL,					// LPSECURITY_ATTRIBUTES lpSecurityAttributes
		OPEN_EXISTING,			// DWORD dwCreationDisposition
#if NO_OVERLAPPED
		0,						// DWORD dwFlagsAndAttributes
#else
		FILE_FLAG_OVERLAPPED,	// DWORD dwFlagsAndAttributes
#endif
		NULL					// HANDLE hTemplateFile
	);
	if ( fh == INVALID_HANDLE_VALUE )
	{
		char	serror[256];

		sprintf(serror, "Open port fault: %s", pResource); 
		EXCTHROW( EL_ERROR_OS_SPECIFIC, serror);
	}

	dcb.DCBlength = sizeof(dcb);
	
	EXCCHECKWIN( GetCommState(fh, &dcb));

	if ( pOptionString )	/* user attributes ("device[,attrs]") ? */
	{
		/* brate, chsize, parity, stop-bits, hand-shake, abort-on-error */
		char attrs_str[128+1]; /* ST */
		char *attr[6];
		char *p;
		register int i;

		/*
		 * Copy the attribute string to local variable so that we can modify it
		 * (if the source is constant string literal, it can reside in read-only
		 * segment...):
		 */
		strcpy(attrs_str,pOptionString);
		/*
		 * Attributes are separated either by '/' or ','. The comma (',') is
		 * used when the user wants to use full specification of hand-shake
		 * mode that contains slash ('/') (e.g. "XON/XOFF" and others):
		 */
		for(i = 0, p = attrs_str;
			i < ARRAY_CNT(attr) && (p = strtok(p,"/,")) != NULL;
			i++, p = NULL
		)
		{
			attr[i] = p;
		} /* for (....) */
		EXCCHECK( dcb_set(&dcb,i,attr));
	}
#if 0	
    if ( pSerComm->verbose > 1 )
		dcb_display(&dcb,YES);

	if ( pSerComm->verbose > 1 )
		LogD("SetCommState() ...");
#endif

	EXCCHECKWIN( SetCommState(fh, &dcb));

	/*
	 * Set up I/O timeouts appropriately:
	 *
	 * We need to set up read timeout so that synchronous (non-overlapped)
	 * 'ReadFile()' (after 'WaitCommEvent()') will not block when we will
	 * reach EOF (0 bytes returned) -- no other data available. Without this
	 * timeout, the 'ReadFile()' would block when there would be no other
	 * data available.
	 *
	 * We also need to set up zero write timeout so that synchronous
	 * (non-overlapped) 'WriteFile()' will not block in case output buffer
	 * is full (for whatever reason).
	 */
	 
	EXCCHECKWIN( GetCommTimeouts(fh, &timeouts));
   
   	/*
	 * Read timeout: MAXDWORD, 0, 0
	 * - read operation is to return immediately with the characters that
	 *	 have already been received, even if no characters have been
	 *	 received (i.e. read operation does not block)
	 */
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	/*
	 * Write timeout: 0, n
	 * - write operation will not block
	 */
#if 1
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;		/* no timeout */
#else
	/*
	 * 9600 baud is approximately 1 byte per millisecond. Allow double
	 * the expected time per character:
	 */
	timeouts.WriteTotalTimeoutMultiplier = 2*9600/gs_dev.brate;
	timeouts.WriteTotalTimeoutConstant = 1;
#endif
	EXCCHECKWIN( SetCommTimeouts(fh, &timeouts));

#if 1
	size = 128 * 1024;
#else
	size = 4 * 1024
#endif
	EXCCHECKWIN( SetupComm(fh, size, size));
	
	EXCCHECKWIN( PurgeComm(fh, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) );

#if !defined(NO_OVERLAPPED)
	memset(&pSerComm->o.ReadWriteFile,0,sizeof(pSerComm->o.ReadWriteFile));
	if ( !(pSerComm->o.ReadWriteFile.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)) )
	{
		LOG( ELOG_DEBUG, __FUNC__"(): CreateEvent(ReadWriteFile.hEvent)");
		EXCTHROW( EL_ERROR_OS_SPECIFIC, "Create Event Fault!");
	}
	memset(&pSerComm->o.WaitCommEvent,0,sizeof(pSerComm->o.WaitCommEvent));
	if ( !(pSerComm->o.WaitCommEvent.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)) )
	{
		LOG( ELOG_DEBUG, __FUNC__"(): CreateEvent(WaitCommEvent.hEvent)");
		EXCTHROW( EL_ERROR_OS_SPECIFIC, "Create Event Fault!");
	}
	pSerComm->o.on = TRUE;
#endif

	if ( !(pSerComm->abort.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)) )
	{
		LOG( ELOG_DEBUG, __FUNC__"(): CreateEvent(abort.hEvent)");
		EXCTHROW( EL_ERROR_OS_SPECIFIC, "Create Event Fault!");
	}

	/*
	 * Set up event mask:
	 */
	for(i = mask = 0; i < COMM_EVTs; i++)
		mask |= gs_comm_evt[i].id;

	EXCCHECKWIN( SetCommMask(fh, mask));

Error:
    if ( pexception!=NULL )
    {
        pSerComm->Close(pSerComm);
        pSerComm->devname[0] = '\0';
        pSerComm->attrs[0] = '\0';
    }
	else
	{
		pSerComm->_fh  = fh;
		pSerComm->_dcb = dcb;
		strcpy( pSerComm->devname, pResource);
		strcpy( pSerComm->attrs, pOptionString);
	}
    EXCRETHROW( pexception); 
} /* sercommOpen */

/******************************************************************************/
/*
 * 'ols_cmd_buf()':
 *
 * Sends string command to OLS controller.
 *
 * Return values:
 *
 *		>0	- OK, number of bytes received as response, if any
 *		 0	- timeout
 *		-1	- an error
 */
#undef	__FUNC__
#define	__FUNC__	"sercommWrite"
static SElExceptionPtr sercommWrite(
    struct _SSerComm* pSerComm,
    const uint8_t*          pCmd,
    uint32_t          cmdlen
)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
    DWORD			l;
	BOOL			fret;

	/*
	 * Write command:
	 */

	if ( pSerComm->_write_delay == -1 )
	{
		/*
		 * Send command buffer at once as fast as possible:
		 */
		LOG_B( ELOG_DEBUG, __FUNC__"(): WriteFile(%d)")
            LOG_P(cmdlen)
        LOG_E();

		fret = WriteFile(pSerComm->_fh,pCmd,cmdlen,&l,
				pSerComm->o.on ? &(pSerComm->o.ReadWriteFile) : NULL);

        LOG_B( ELOG_DEBUG, __FUNC__"(): WriteFile(%d): done")
            LOG_P(cmdlen)
        LOG_E();

		if ( !fret )
		{
			DWORD le = GetLastError();
			if ( le == ERROR_IO_PENDING )
			{
#if 0
				EXCCHECK( ols_wait_for_overlapped(pSerComm,&l,-1));
#else
				EXCCHECK( ols_wait_for_overlapped(pSerComm,&l, 1000));
#endif
			}
			else
			{
#if defined(_CVI_)
#if 0
				EXCCHECK( ols_wait_for_overlapped(pSerComm,&l,-1));
#else
				EXCCHECK( ols_wait_for_overlapped(pSerComm,&l, 1000));
#endif
#else
				LPVOID buf;

  				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							   NULL,
							   le,
							   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							   (LPTSTR)&buf,
							   0, NULL);

				EXCTHROW( -1, (const char*)buf);
				LocalFree(buf);
#endif
			}
		}
		if ( (int)l != cmdlen )
		{
    		LOG_B( ELOG_DEBUG, __FUNC__"(): WriteFile(): %d/%d")
                LOG_P((int)l)
                LOG_P(cmdlen)
            LOG_E();
		}
	}
	else
	{
		register uint32_t len;

		/*
		 * Send command buffer slowly, byte by byte with small time delay
		 * inbetween individual bytes:
		 *
		 * This is sometimes needed so that ADuC is able to read all data
		 * (not to miss a byte):
		 */
		for(len = 0; len < cmdlen; len += l)
		{
			fret = WriteFile(pSerComm->_fh,pCmd+len,sizeof(*pCmd),&l,
						pSerComm->o.on ? &pSerComm->o.ReadWriteFile : NULL);
			if ( !fret )
			{
				DWORD le = GetLastError();
				if ( le == ERROR_IO_PENDING )
				{
#if 0
					EXCCHECK( ols_wait_for_overlapped(pSerComm,&l,-1));
#else
					EXCCHECK( ols_wait_for_overlapped(pSerComm,&l, 1000));
#endif
				}
				else
				{
					LPVOID buf;

  					FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
								   NULL,
								   le,
								   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
								   (LPTSTR)&buf,
								   0, NULL);

					EXCTHROW( -1, (const char*)buf);
					LocalFree(buf);
				}
			}
			if ( !l )
			{
				LOG(ELOG_DEBUG, __FUNC__"(): WriteFile(): 0");
			}
#if 1
#if 0	/* 0/1: do/don't wait after last character: */
			if ( len < cmdlen - 1 && pSerComm->_write_delay )
#endif
				Sleep(pSerComm->_write_delay);
#endif
		}
	}

Error:
	EXCRETHROW( pexception);   
}

/******************************************************************************/
#undef	__FUNC__
#define	__FUNC__	"sercommReadAll"
static SElExceptionPtr sercommReadAll(
    struct _SSerComm* pSerComm,
    uint32_t bufsize,
    uint8_t *buf,
    uint32_t* rdCount,
    int32_t timeout
)
{
	SElExceptionPtr    	pexception = NULL; 
	int32_t		error = 0;  
	uint32_t	buflen = 0, ret = 0;

	LOG_B( ELOG_DEBUG, __FUNC__"(): bufsize: %d")
        LOG_P((int)bufsize)
    LOG_E();
    
    for(buflen = 0; buflen < bufsize; buflen += ret, bufsize -= ret)
	{
		EXCCHECK( sercommRead(pSerComm,bufsize,buf+buflen,&ret,timeout));
		if ( pSerComm->tch.read.set ) 
		{
			if ( buf[ buflen + ret -1 ] == pSerComm->tch.read.ch )
			{
				buflen += ret;
				break;
			}
		} /* if ( pSerComm->tch.read.set )  */
	}

Error:
	if ( pexception!=NULL )
	{
		LOG_B( ELOG_DEBUG, __FUNC__"(): ols_io_read(): %d (failed), %d bytes read so far")
        	LOG_P((int)ret)
        	LOG_P((int)buflen)
    	LOG_E();
	}
	if ( rdCount ) *rdCount = buflen;
	EXCRETHROW( pexception);  
}

/******************************************************************************/
#undef	__FUNC__
#define	__FUNC__	"sercommRead"
static SElExceptionPtr sercommRead(
    struct _SSerComm* pSerComm,
    uint32_t bufsize,
    uint8_t *buf,
    uint32_t* rdCount,
    int32_t timeout
)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	DWORD			mask;
	DWORD			l, len = 0;

	/*
	 * If necessary, wait for data to arrive:
	 *
	 * If the last 'ReadFile()' (usually from previous call) did not return
	 * all requested bytes (i.e. it returned either partial data or no data
	 * at all), then we are going to wait via 'WaitCommEvent()' for new data
	 * to arrive.
	 *
	 * If the last 'ReadFile()' from previous call returned all requested
	 * bytes, then we have to try another 'ReadFile()' without waiting,
	 * because there still might be some data in the input buffer that have
	 * been already indicated by last 'WaitCommEvent()' (but have not been
	 * read yet) and would not be reported by next 'WaitCommEvent()', which
	 * would block even though there would be these data in the input buffer
	 * waiting to be read. This happens when data are received in large
	 * blocks that are greater than I/O buffer/cache (max. requested number
	 * of bytes to read per 'ReadFile()') or when the data are being
	 * received faster than caller is retrieving the data.
	 */
	if ( !pSerComm->_last_read_full )
	{
	wait_comm_event:
		do
		{
			EXCCHECK( ols_wait_comm_event(pSerComm,&mask,timeout));

			if ( mask & EV_ERR )
			{
				COMSTAT comstat;
				DWORD errflags;

				if ( !ClearCommError(pSerComm->_fh,&errflags,&comstat) )
				{
					/* Log("%s(): ClearCommError(): %W",__FUNC__); */
				}
				else
				{
#if 0
					if ( pSerComm->verbose )
					{
						Log("%s(): --- EV_ERR ---",__FUNC__);
						err_flags_logd(errflags);
						comstat_logd(&comstat);
					}
#endif
					ols_clear_error(pSerComm);
				}
			}
		} while( !(mask & EV_RXCHAR) );

	}	/* if ( !pSerComm->_last_read_full ) */

#if 0
	{	/* inner block */
	COMSTAT comstat;
	DWORD errflags;

	/*
	 * Number of bytes in I/O buffers:
	 */
	if ( !ClearCommError(pSerComm->_fh,&errflags,&comstat) )
	{
		Log("%s(): ClearCommError(): %W",__FUNC__);
		return OLS_ERR;
	}
	}	/* inner block */
#endif

	/*
	 * Read from serial port until no data are available:
	 */
	for(len = 0, l = 1; l && (int)len < bufsize; len += l)
	{
	read:
		LOG_B( ELOG_DEBUG, __FUNC__"(): ReadFile(%d): ...")
            LOG_P(bufsize-len)
        LOG_E();

		if ( !ReadFile(pSerComm->_fh,buf+len,bufsize-len,&l,
				pSerComm->o.on ? &pSerComm->o.ReadWriteFile : NULL) )
		{
			switch( GetLastError() )
			{
			case ERROR_OPERATION_ABORTED:
				ols_clear_error(pSerComm);
				goto read;
				break;
			case ERROR_IO_PENDING:
#if 0
				EXCCHECK( ols_wait_for_overlapped(pSerComm,&l,-1)); 
#else
				EXCCHECK( ols_wait_for_overlapped(pSerComm,&l, 1000)); 
#endif
				break;
			default:
                LOG_B( ELOG_DEBUG, __FUNC__"(): ReadFile(%d): ...")
                    LOG_P(bufsize-len)
                LOG_E();
				EXCTHROW( EL_ERROR_OS_SPECIFIC, "ReadFile Fault!");
				break;
			}
		}
		    LOG_B( ELOG_DEBUG, __FUNC__"(): ReadFile(): %ld")
                LOG_P( l)
            LOG_E();

		pSerComm->_last_read_full = (bool_t) (l == bufsize - len);

		if ( !l )
		{
			/*
			 * Note:
			 *
			 * It can happen, that no data are read by 'ReadFile()' even if
			 * we have waited via 'WaitCommEvent()' for event 'EV_RXCHAR'
			 * (character received) and this event has occurred. This is
			 * not a bug and no work around can be used to avoid this
			 * situation. The following pseudocode explains how this can
			 * happen:
			 *
			 *      for(;;) {
			 *          WaitCommEvent(EV_RXCHAR);
			 *          puts("EV_RXCHAR received");
			 *          //
			 *          // At this time, new data arrive and 'EV_RXCHAR' is
			 *          // set internally to be indicated by next
			 *          // 'WaitCommEvent()'.
			 *          //
			 *          // The following 'ReadFile()' reads all available
			 *          // data including new data that have just arrived,
			 *          // thus "swallowing" data that will be indicated by
			 *          // next 'WaitCommEvent()'. The next 'ReadFile()'
			 *          // will return zero bytes, because the data have
			 *          // been already read by previous 'ReadFile()'...
			 *          //
			 *          ReadFile(bufsize); [or] while( ReadFile(bufsize) );
			 *      }
			 *
			 * We cannot avoid this situation with 'SetCommMask()' being
			 * called just before 'WaitCommEvent()' in the above code,
			 * because then the 'WaitCommEvent()' would not indicate data
			 * that happened to arrive between last 'ReadFile()' and
			 * subsequent 'SetCommMask()'.
			 *
			 * 'EV_RXCHAR' event does not necessarily indicates, that there
			 * are some data available in the input buffer, it just
			 * indicates, that there were some data put in the input buffer
			 * since last call to either 'WaitCommEvent()' or
			 * 'SetCommMask()'.
			 */

			if ( len )  /* have we already gathered some data ? */
				goto Error;     /* yes, return them to caller */

			/*
			 * There are no data available and so far we haven't read anything.
			 * We need to wait for some data to arrive:
			 */
			goto wait_comm_event;

		}	/* if ( !l ) */

	}	/* for(len = 0, l = 1; l && (int)len < bufsize; len += l) */

	/*
	 * Return all data that have been read so far:
	 */
Error:
    if (rdCount) *rdCount = len;
    EXCRETHROW( pexception);
}

/******************************************************************************/
#undef	__FUNC__
#define	__FUNC__	"sercommSetTerminationCharacter"
static SElExceptionPtr sercommSetTerminationCharacter(
    struct _SSerComm* pSerComm,
    int32_t			  typeofbuffer,
    bool_t			  enabled,
    uint8_t			  tch
)
{
	SElExceptionPtr	pexception = NULL;
	
	if ( typeofbuffer == SERCOMM_BUFFER_WRITE )
	{
		pSerComm->tch.write.set = enabled;
		pSerComm->tch.write.ch  = tch;
	}
	else
	{
		pSerComm->tch.read.set = enabled;
		pSerComm->tch.read.ch  = tch;
	}
	
/* Error: */
	EXCRETHROW( pexception);
} /* sercommSetTerminationCharacter */

/******************************************************************************
 * STATIC
 ******************************************************************************/ 
#undef	__FUNC__
#define	__FUNC__	"dcb_set"
static SElExceptionPtr dcb_set(DCB *dcb, int argc, const char *argv[])
{
	SElExceptionPtr	pexception = NULL; 
    int32_t			error = 0;
    bool_t			rts_cts,
					dtr_dsr,
					xon_xoff;
	register int	i;

    if ( argc >= 1 )
		dcb->BaudRate = atoi(argv[0]);		/* Baud Rate */
	if ( argc >= 2 )
		dcb->ByteSize = atoi(argv[1]);		/* Character Size */

	if ( argc >= 3 )
	{
		/*
		 * Parity:
		 */
		for(i = 0;
			i < ARRAY_CNT(gs_parity) && *gs_parity[i].str != toupper(*argv[2]);
			i++
		);
		if ( i == ARRAY_CNT(gs_parity) )
		{
		    LOG_B( ELOG_DEBUG, __FUNC__"(): '%s': unknown parity")
                LOG_P(argv[2])
            LOG_E();
            EXCTHROW( SERCOMM_ERROR_UNKNOWN_PARITY, "Sercomm error unknown parity!");
		}
		dcb->Parity = gs_parity[i].id;
		dcb->fParity = TRUE;
	}
	
	if ( argc >= 4 )
	{
		/*
		 * Stop Bits:
		 */
		for(i = 0;
			i < ARRAY_CNT(gs_stopbits) && strcmp(gs_stopbits[i].str,argv[3]);
			i++
		);
		if ( i == ARRAY_CNT(gs_stopbits) )
		{
			LOG_B( ELOG_DEBUG, __FUNC__"(): '%s': unknown number of stop-bits")
                LOG_P(argv[2])
            LOG_E();
			EXCTHROW( SERCOMM_ERROR_UNKNOWN_STOPBITS, "Sercomm error unknown stopbit!");
		}
		dcb->StopBits = gs_stopbits[i].id;
	}

	if ( argc >= 5 )
	{
		/*
		 * Hand-shake:
		 */
		rts_cts = dtr_dsr = xon_xoff = FALSE;
		switch( toupper(*argv[4]) )
		{
		case 'N':	/* None */
			break;
		case 'R':	/* RTS/CTS */
			rts_cts = TRUE;
			dcb->fRtsControl		= RTS_CONTROL_HANDSHAKE;
			dcb->fOutxCtsFlow		= TRUE;
			break;
		case 'D':	/* DTR/DSR */
			dtr_dsr = TRUE;
			dcb->fDtrControl		= DTR_CONTROL_HANDSHAKE;
			dcb->fOutxDsrFlow		= TRUE;
			dcb->fDsrSensitivity	= FALSE;
			break;
		case 'X':	/* XON/XOFF */
			xon_xoff = TRUE;
			dcb->fOutX				= TRUE;
			dcb->fInX				= TRUE;
			dcb->fTXContinueOnXoff	= FALSE;
			dcb->XonChar			= 0x11;
			dcb->XoffChar			= 0x13;
			dcb->XonLim				= 0;
			dcb->XoffLim			= 0;
			break;
		default:
			LOG_B( ELOG_DEBUG, __FUNC__"(): '%s': unknown hand-shake")
                LOG_P(argv[4])
            LOG_E();
			EXCTHROW( SERCOMM_ERROR_UNKNOWN_HANDSHAKE, "Sercomm error unknown handshake!");
		}
		if ( !rts_cts )
		{
			dcb->fRtsControl		= RTS_CONTROL_ENABLE;
			dcb->fOutxCtsFlow		= FALSE;
		}
		if ( !dtr_dsr )
		{
			dcb->fDtrControl		= DTR_CONTROL_ENABLE;
			dcb->fOutxDsrFlow		= FALSE;
			dcb->fDsrSensitivity	= FALSE;
		}
		if ( !xon_xoff )
		{
			dcb->fOutX				= FALSE;
			dcb->fInX				= FALSE;
			dcb->fTXContinueOnXoff	= FALSE;
		}
	}	/* if ( argc >= 5 ) */

	if ( argc >= 6 )
	{
		/*
		 * Abort-on-error:
		 */
        EXCTHROW( SERCOMM_ERROR_UNKNOWN_OPTION_PARAMETER, "Sercomm error unknown option parameter!");
		//dcb->fAbortOnError = atoi(argv[5]);
	}

Error:
	EXCRETHROW( pexception);      
} 


/******************************************************************************/
/*
 * ols_wait_for_overlapped() - Wait for overlapped operation to complete.
 */
#undef	__FUNC__
#define	__FUNC__	"ols_wait_for_overlapped"
static SElExceptionPtr ols_wait_for_overlapped(
	struct _SSerComm* pSerComm,
    DWORD *p_len,
    DWORD timeout
)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
    HANDLE			h[2];
	DWORD			ret;

#if 0
	if ( pSerComm->verbose > 1 )
		LogD("%s(): WaitForMultipleObjects(timeout=%ld) ...",__FUNC__,timeout);
#endif
	h[0] = pSerComm->o.ReadWriteFile.hEvent;
	h[1] = pSerComm->abort.hEvent;
	ret = WaitForMultipleObjects(2,h,FALSE, timeout == -1 ? INFINITE : timeout);

	switch( ret )
	{
		case WAIT_OBJECT_0 + 0:
			EXCCHECKWIN( GetOverlappedResult(pSerComm->_fh,&pSerComm->o.ReadWriteFile,p_len,FALSE));
			break;
		case WAIT_OBJECT_0 + 1:
			ols_io_abort_reset(pSerComm);
			EXCTHROW( SERCOMM_ERROR_ABORT, "Sercomm error abort!");
			break;
		case WAIT_TIMEOUT:
			ols_io_abort_pending(pSerComm);
			EXCTHROW( SERCOMM_ERROR_TIMEOUT, "Sercomm error timeout!");
			break;
		default:
			EXCTHROW( EL_ERROR_OS_SPECIFIC, "Sercomm error!");
			break;
	}

Error:
    EXCRETHROW( pexception);   
}

/******************************************************************************/
#undef	__FUNC__
#define	__FUNC__	"ols_io_abort_reset"
static SElExceptionPtr ols_io_abort_reset(struct _SSerComm* pSerComm)
{
    pSerComm->abort.set = FALSE;
    ResetEvent(pSerComm->abort.hEvent);
	return ols_io_abort_pending(pSerComm);
}

/******************************************************************************/
#undef	__FUNC__
#define	__FUNC__	"ols_io_abort_pending"
static SElExceptionPtr ols_io_abort_pending(struct _SSerComm* pSerComm)
{
	/*
	 * After an I/O timeout or I/O aborted by user, the overlapped I/O is still
	 * in progress (all subsequent I/O calls return 'ERROR_INVALID_PARAMETER')
	 * until finished or cancelled. To cancel overlapped I/O, we could call
	 * 'CancelIo()', but this routine is not available on Win95 or WinNT 3.1.
	 * Another way how to cancel the pending overlapped I/O is to close
	 * the I/O handle. This works on all Windows platforms. To make this I/O
	 * timeout handling transparent to the caller, we will close and reopen
	 * the serial I/O right here:
	 */
	pSerComm->Close( pSerComm);
	return pSerComm->Open(pSerComm,pSerComm->devname,pSerComm->attrs);
}

/******************************************************************************/
#undef	__FUNC__
#define	__FUNC__	"ols_wait_comm_event"
static SElExceptionPtr ols_wait_comm_event(
    struct _SSerComm* pSerComm,
    DWORD *pmask,
    long timeout
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
    /*
	 * Start up overlapped (asynchronous) 'WaitCommEvent()':
	 */
wait:
	if ( WaitCommEvent(pSerComm->_fh,pmask,pSerComm->o.on ? &pSerComm->o.WaitCommEvent : NULL) )
	{
        /* LogD("\n - completed synchronously"); */
	}
	else
	{
		HANDLE h[2];
		DWORD ret;

		if ( GetLastError() == ERROR_OPERATION_ABORTED )
		{
			ols_clear_error(pSerComm);
			goto wait;
		}
		else if ( GetLastError() != ERROR_IO_PENDING )
		{
            EXCTHROW( EL_ERROR_OS_SPECIFIC, "Sercomm error!");
		}

		/*
		 * 'WaitCommEvent()' is in progress (executed asynchronously):
		 */
		LOG(ELOG_DEBUG, __FUNC__"(): - launched asynchronously (overlapped)");
        LOG_B( ELOG_DEBUG, __FUNC__"(): MsgWaitForMultipleObjects(timeout=%ld) ...")
            LOG_P( timeout )
        LOG_E();

		h[0] = pSerComm->o.WaitCommEvent.hEvent;
		h[1] = pSerComm->abort.hEvent;
		ret = WaitForMultipleObjects(2,h,FALSE,
			timeout == -1 ? INFINITE : timeout);

		
		LOG_B( ELOG_DEBUG, __FUNC__"(): MsgWaitForMultipleObjects(): %lu")
            LOG_P( ret )
        LOG_E();

		switch( ret )
		{
		case WAIT_FAILED:
			EXCTHROW( EL_ERROR_OS_SPECIFIC, "Sercomm error wait failed!");
			break;
		case WAIT_TIMEOUT:
			LOG(ELOG_DEBUG, __FUNC__"(): MsgWaitForMultipleObjects(): timeout!");
			ols_io_abort_pending(pSerComm);
			EXCTHROW( SERCOMM_ERROR_TIMEOUT, "Sercomm error timeout!");
			break;

		case WAIT_OBJECT_0 + 0:		/* pSerComm->o.WaitCommEvent.hEvent */
		{	/* inner block */
			/* overlapped 'WaitCommEvent()' has completed: */
			/*
			 * 'cnt' - dummy variable, just for the sake of
			 *		   'GetOverlappedResult()', because 'WaitCommEvent()'
			 *		   doesn't have anything to do with number of bytes
			 *		   transferred ('{Read|Write}File()' does)
			 */
			DWORD cnt;

            LOG(ELOG_DEBUG, __FUNC__"(): WaitCommEvent(): completed");
            EXCCHECKWIN( GetOverlappedResult(pSerComm->_fh,&pSerComm->o.WaitCommEvent, &cnt,FALSE));
			break;
		}	/* inner block */

		case WAIT_OBJECT_0 + 1:		/* pSerComm->abort.hEvent */
			ols_io_abort_reset(pSerComm);
			EXCTHROW( SERCOMM_ERROR_ABORT, "Sercomm error abort!");
			break;

		/*
		 * Any event in the thread's input queue ?
		 */
		case WAIT_OBJECT_0 + ARRAY_CNT(h):
		{	/* inner block */
			MSG msg;
			bool_t f;

			/*
			 * Remove all thread's messages now, because messages that would
			 * be left over, would NOT wake up next 'MsgWaitFor...()' !
			 */
			for(f = FALSE; PeekMessage(&msg,NULL,0,0,PM_REMOVE); f = TRUE)
			{
#if 0
				LOG_B( ELOG_DEBUG, __FUNC__"(): PeekMessage(): hwnd: 0x%lX, message: %s (%u), wParam: %u, lParam: %lu, time: %lu")
                    LOG_P(msg.hwnd)
                    LOG_P(winmsg_idstr(msg.message))
                    LOG_P(msg.message)
                    LOG_P(msg.wParam)
                    LOG_P(msg.lParam)
                    LOG_P(msg.time)
                LOG_E();
#else
				LOG_B( ELOG_DEBUG, __FUNC__"(): PeekMessage(): hwnd: 0x%lX, message: %s (%u), wParam: %u, lParam: %lu, time: %lu")
                    LOG_P(msg.hwnd)
                    LOG_P(msg.message)
                    LOG_P(msg.message)
                    LOG_P(msg.wParam)
                    LOG_P(msg.lParam)
                    LOG_P(msg.time)
                LOG_E();
#endif
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if ( f )
            {
                LOG(ELOG_DEBUG, __FUNC__"(): PeekMessage(): 0");
			}
			break;
		}	/* inner block */

		default:
				LOG_B( ELOG_DEBUG, __FUNC__"(): MsgWaitForMultipleObjects(): %lu")
                    LOG_P(ret)
                LOG_E();
				EXCTHROW( EL_ERROR_OS_SPECIFIC, "Sercomm error!");
			break;

		}	/* switch( ret ) */
	}	/* asynchronous 'WaitCommEvent()' */

#if 0
	if ( pSerComm->verbose > 1 )
	   evt_mask_logd(__FUNC__,*pmask);
#endif
		
Error:
	EXCRETHROW( pexception); 
}


/******************************************************************************/
#undef	__FUNC__
#define	__FUNC__	"ols_clear_error"
static void ols_clear_error(struct _SSerComm* pSerComm)
{
	int32_t     error = 0;
	BOOL        win_error;
	COMSTAT		comstat;
	DWORD		errflags;

	CHECKWIN( ClearCommError(pSerComm->_fh,&errflags,&comstat));

Error:
/*
    if ( io->verbose )
	{
		err_flags_logd(errflags);
		comstat_logd(&comstat);
	}
*/
    ;
}
