/****************************************************************************
 *                             V L I N E
 *---------------------------------------------------------------------------
 *    Copyright   2005-2006 ELCOM, a.s.  All Rights Reserved.        
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:       vline.h                                                    
 * Purpose:     Vline interface (Valeo debug line interface)
 * Version:     2.0 
 ****************************************************************************/

#if !defined(__VLINE_H__)
#define __VLINE_H__

#include <eltypes.h>
#include <elstring.h>
#include <exception/Exception.h>
#include <windows.h>
#include <timer/eltimer.h>
#include <sercomm/sercomm.h>

#if defined(__cplusplus) || defined(__cplusplus__)
	extern "C" {
#endif

#define VLINE_STATIC_PATH		256
#define VLINE_STATIC_VALUE		128

#define FLOW_CTRL_NONE			0
#define FLOW_CTRL_XON_XOFF		1
#define FLOW_CTRL_RTS_CTS		2
#define FLOW_CTRL_DTR_DSR		3


/*!
 * Specifies VALEO Line interface
 */ 
typedef struct _SVLine
{
	SElExceptionPtr (*Open)(struct _SVLine* me, const char* pResourceName);
	SElExceptionPtr (*Reset)(struct _SVLine* me);
	SElExceptionPtr (*Write)(struct _SVLine* me, const char* cmd);
	SElExceptionPtr (*Query)(struct _SVLine* me, const char* cmd, char* buffer, uint32_t bufferSize);
	SElExceptionPtr (*Close)(struct _SVLine* me);
	SElExceptionPtr (*Invalidate)(struct _SVLine* me);
	SElExceptionPtr (*SetLogCallback)(struct _SVLine* me, int32_t (_FUNCC *Callback)(uint32_t  priority, const char* message));
    SElExceptionPtr (*ClearCache)(struct _SVLine* me);
	SElExceptionPtr (*EnableCache)(struct _SVLine* me, bool_t state);
    const char* (*GetCachePtr)(struct _SVLine* me);

/*******************************************
 * PRIVATE
 *******************************************/
    SSerCommPtr			_pSerComm;    /*!< Pointer to serial interface */
	CRITICAL_SECTION	_Lock; 
	uint8_t             _CMDPath[VLINE_STATIC_PATH+1];	/*!< Cache for commands */
	uint8_t             _CMDValue[VLINE_STATIC_VALUE+1];	/*!< Cache for values */
	uint32_t			_write_delay;

	int32_t (_FUNCC *_Log)(uint32_t  priority, const char* message);
	bool_t			_initialized; /*!< Is communication iniialized?
                                 When value is FALSE, write or query
                                 function first call reset function before
                                 send any data */ 
	SELStringPtr	_pCache;      /*!< Cache data, all write and read data */
	STimerPtr		_pTimer;      /*!< Timer used in cache logs */
	bool_t			_CacheEnabled; /*!< Is cache enabled */
} SVLine, *SVLinePtr;

SElExceptionPtr	vline_new(SVLinePtr* pVLine);
SElExceptionPtr	vline_delete(SVLinePtr* pVLine);


#define VLINE_ERROR_COM_PORT				-1000000
#define VLINE_ERROR_RESOURCE_NAME			-1000001
#define VLINE_ERR_LONG_COMMAND				-1000002
#define VLINE_ERR_NOT_VALID_PATH			-1000003
#define VLINE_ERR_INVALID_INTERNAL_PATH		-1000004
#define VLINE_ERR_BUFFER_OVERFLOW			-1000005

#define VLINE_ERR_UNKNOWN_SPECIFIC_ERROR	-1000010
#define VLINE_ERR_UNKNOWN_COMMAND			-1000011
#define VLINE_ERR_ARG_NUMBER_OVERFLOW		-1000012
#define VLINE_ERR_NO_MEM_FOR_NEW_LEVEL		-1000013
#define VLINE_ERR_PARAMETER_NB_ERROR		-1000014
#define VLINE_ERR_PARAMETER_ERROR			-1000015
#define VLINE_ERR_FORMAT_ERROR				-1000016

#if defined(__cplusplus) || defined(__cplusplus__)
	}
#endif

#endif /* __VLINE_H__*/ 
