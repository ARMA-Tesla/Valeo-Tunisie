
#if !defined(__SERCOMM_H__)
#define __SERCOMM_H__

#include <eltypes.h>
#include <windows.h>
#include <exception/Exception.h>  

#define SERCOMM_BUFFER_READ		0
#define SERCOMM_BUFFER_WRITE		1

/*!
 * Specifies serial communication interface
 */ 
typedef struct _SSerComm
{
	SElExceptionPtr (*Open)(struct _SSerComm* me, const char* pResourceName, const char* pOptionString);
	SElExceptionPtr (*Write)(struct _SSerComm* me, const uint8_t* buffer, uint32_t bufferSize);
	SElExceptionPtr (*Read)(struct _SSerComm* me,  uint32_t bufferSize, uint8_t* buffer, uint32_t* pReadCount, int32_t timeout);
	SElExceptionPtr (*Close)(struct _SSerComm* me);
	SElExceptionPtr (*SetTerminationCharacter)( struct _SSerComm* me, int32_t typeofbuffer, bool_t enabled, uint8_t tch);
	SElExceptionPtr (*SetLogCallback)(struct _SSerComm* me, int32_t (_FUNCC *Callback)(uint32_t  priority, const char* message));

/* Private */
    HANDLE              _fh;
    int32_t             _write_delay;
    bool_t				_last_read_full;
    struct {
		bool_t on;	/* overlapped I/O enabled/disabled */
		OVERLAPPED ReadWriteFile;
		OVERLAPPED WaitCommEvent;
	} o;
	struct {
		bool_t set;		/* I/O abort flag */
		HANDLE hEvent;	/* I/O abort event */
	} abort;
	
	struct {
		struct {
			bool_t 	set;	/* Read termination character is specified */
			uint8_t ch;		/* Read termination character */
		} read;
		struct {
			bool_t set;		/* Write termination character is specified */
			uint8_t ch;		/* Write termination character */
		} write;
	} tch;
	
	char devname[32];
	char attrs[128];
	DCB            _dcb;
} SSerComm, *SSerCommPtr;

SElExceptionPtr	sercomm_new(const char* pResource, const char* pOptionString, SSerCommPtr* pSerComm);
SElExceptionPtr	sercomm_delete(SSerCommPtr* pSerComm);

#define SERCOMM_ERROR_UNKNOWN_PARITY        	-100001
#define SERCOMM_ERROR_UNKNOWN_HANDSHAKE     	-100005
#define SERCOMM_ERROR_UNKNOWN_OPTION_PARAMETER  -100002
#define SERCOMM_ERROR_UNKNOWN_STOPBITS			-100004
#define SERCOMM_ERROR_TIMEOUT                   -100006
#define SERCOMM_ERROR_ABORT                     -100007

#if defined(_cplusplus)
	}
#endif

#endif /* __SERCOMM_H__*/ 
