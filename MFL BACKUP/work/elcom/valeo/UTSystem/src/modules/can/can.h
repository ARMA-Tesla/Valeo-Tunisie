/****************************************************************************
 *                              C A N
 *---------------------------------------------------------------------------
 *    Copyright  © 2007 ELCOM, a.s.  All Rights Reserved.        
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:       can.h                                                    
 * Purpose:     CAN interface (Controled Area Network)
 ****************************************************************************/

#if !defined(__CAN_H__)
#define __CAN_H__

#include <eltypes.h>
#include <elstring.h>
#include <timer/eltimer.h>
#include <exception/exception.h>
#include <objlist/objlist.h>
#include <windows.h>
#include "Nican.h"

#if defined(__cplusplus) || defined(__cplusplus__)
	extern "C" {
#endif

#define READ_ANY		0xFFFFFFFF

#define CAN_READ		0
#define CAN_WRITE		1

#define CAN_ADDRESS_TYPE_STD		0
#define CAN_ADDRESS_TYPE_XTD		1
#define LIN_ADDRESS_TYPE_STD		18
#define LIN_ADDRESS_TYPE_XTD		17

/*! Data types for GetAttribute, SetAttribute methods */
typedef enum _DataTypes
{
	DATA_TYPE_UNKNOWN = 0xFFFFFFFF,
	DATA_TYPE_UINT8	= 0,
	DATA_TYPE_INT8,
	DATA_TYPE_UINT16,
	DATA_TYPE_INT16,
	DATA_TYPE_UINT32,
	DATA_TYPE_INT32,
	DATA_TYPE_REAL32,
	DATA_TYPE_REAL64,
	DATA_TYPE_DYNSTRING
} EDataTypes;

/*! Attributes list for GetAttribute, SetAttribute methods */
typedef enum _CanAttributes
{
	CAN_ATTR_UNKNOWN = 0xFFFFFFFF,
	CAN_ATTR_UUT_ADDRESS = 0,
	CAN_ATTR_TESTER_ADDRESS,
	CAN_ATTR_TIMEOUT,
	CAN_ATTR_CACHE_ENABLED,
	CAN_ATTR_SKIP_ERROR_FRAME
} ECanAttributes;

typedef SElExceptionPtr (*PLogFuncPtr)(
			char*			channel,
			int32_t			type,
			int32_t			indentifier,
			uint8_t*		pData,
			uint32_t		dataSize,
			NCTYPE_ABS_TIME	timestamp
		);

/*! Specifies data, which are write in asynchronous loop */
typedef struct _SCANThreadItem
{
	uint32_t	wIdentifier;	/*!< CAN write identifier */
	uint32_t	wIdentType;		/*!< Address type (STD, XTD) */
	uint8_t		data[8];		/*!< data */
	uint8_t		length;			/*!< Number of valid bytes in data */
	uint32_t	writeTime;		/*!< Time between two writes [ms] */
} SCANThreadItem, *SCANThreadItemPtr;

typedef struct _SCANHandleItem
{
	HANDLE		pHandle;
	uint32_t	writeTime;
} SCANHandleItem, *SCANHandleItemPtr;

/*!
 * Specifies CAN (Controled Area Network) interface
 */ 
typedef struct _SCAN
{
	SElExceptionPtr (*Open)(
			struct _SCAN*	me,
			const char*		channel,
			const char*		options
		);  /*!< \see canOpen */
    SElExceptionPtr (*Close)(struct _SCAN*	me); /*!< \see canClose */
	SElExceptionPtr (*Stop)(struct _SCAN* me, bool_t reset);
	
	SElExceptionPtr (*Write)(
			struct _SCAN*	pCAN,
			uint32_t		address,
			uint32_t		dataLength,
			uint8_t*		pData
		); /*!< \see canWrite */
	SElExceptionPtr (*Read)(
			struct _SCAN*	pCAN,
			uint32_t*		identifier,
			int32_t*		pDataLength,
			uint8_t*		pData
		); /*!< \see canRead */

	SElExceptionPtr (*WriteLowLevel)(
			struct _SCAN*	pCAN,
			uint32_t		address,
			uint32_t	    frame_type,
			uint32_t		dataLength,
			uint8_t*		pData
		); /*!< \see canWrite */
	SElExceptionPtr (*ReadLowLevel)(
			struct _SCAN*	pCAN,
			uint32_t*		identifier,
			int32_t*		pDataLength,
			uint8_t*		pData
		); /*!< \see canRead */
	
	SElExceptionPtr (*Query)(
			struct _SCAN*	pCAN,
			uint32_t		wIdent,
			uint8_t*		wData,
			uint32_t		wDataDize,
			uint32_t		rIdent,
			int32_t*		prDataSize,
			uint8_t*		rData
		); /*!< \see canQuery */
	SElExceptionPtr (*QueryEx)(
			struct _SCAN*	pCAN,
			uint32_t		wIdent,
			uint8_t*		wData,
			uint32_t		wDataDize,
			uint32_t		rIdent,
			int32_t*		prDataSize,
			uint8_t*		rData
		); /*!< \see canQueryEx */
    SElExceptionPtr (*ClearCache)(struct _SCAN* me);
    const char* (*GetCache)(struct _SCAN* me);

	SElExceptionPtr (*StartThread)(struct _SCAN* me);
	SElExceptionPtr (*StopThread)(struct _SCAN* me);
	bool_t (*IsThreadRunning)(struct _SCAN* me);
	SElExceptionPtr (*ExitThread)(struct _SCAN* me);
	SElExceptionPtr (*AddThreadItem)(
			struct _SCAN*	me,
			uint32_t		wIdentifier,
			uint32_t		wIdentType,
			uint8_t			data[],
			uint32_t		dataLength,
			uint32_t		writeTime
		);
	SElExceptionPtr (*ClearThreadItems)(struct _SCAN* me);
	SElExceptionPtr	(*SetTimeout)(struct _SCAN* me, uint32_t ReadTimeout);
	SElExceptionPtr	(*GetTimeout)(struct _SCAN* me, uint32_t* ReadTimeout);

	SElExceptionPtr (*SetAttribute)(
		struct _SCAN*	me,
		uint32_t		dataType,
		uint32_t		attrId,
		void*			value
	);
	SElExceptionPtr (*GetAttribute)(
		struct _SCAN*	me,
		uint32_t		dataType,
		uint32_t		attrId,
		void*			value,
		int32_t			valueSize
	);

/*******************************************
 * PRIVATE
 *******************************************/ 
	NCTYPE_OBJH			_netObj;		/*!< Object handle of CAN Network */
	
	SCANThreadItemPtr	_pItemsList;	/*!< List of periodic CAN Frames */
	uint32_t			_ItemsCount;	/*!< Number of periodic CAN Frames */

	SELStringPtr		_pCache;		/*!< Cache data, all write and read data */
	STimerPtr			_pTimer;		/*!< Timer used in cache logs */
	bool_t				_CacheEnabled;	/*!< Is cache enabled */
	char				_prefix[32];	/*!< Channel prefix */
	uint16_t			_uutAddress;	/*!< UUT address for PL3 KWP2000 protocol */
	uint16_t			_testerAddress;	/*!< Tester address for PL3 KWP2000 protocol */
	
	CRITICAL_SECTION	_Lock; /* AppendCacheData() */
	CRITICAL_SECTION	_LockId; /* write data */
	SObjListPtr			_LockIds;

	PLogFuncPtr			_logFunction;	/*!< Custom Log Function */

	/* for CPU CAN cards */
	NCTYPE_OBJH_P		_hThreadList;	/*!< List of runing CAN object (Periodic Frames) */
	/* for Non-CPU CAN cards */
	SCANHandleItemPtr	_hHandleList;	/*!< List of running threads (Periodic Frames) */
	HANDLE				_hStopEvent;	/*!< Stop Event for all threads */

	uint32_t			_hThreadCount;	/*!< Number of runing CAN objects or threads (Periodic Frames) */

	uint32_t			_timeout;		/*!< CAN timeout */
	uint32_t			_add8bytes;		/*!< Add 0xFF to rest of 8B for send in KWP commands */
	uint32_t			_skipErrorFrames; /*!< Ignore error frame during communication */
} SCAN, *SCANPtr;

/*! Creates a new instance of CAN interface */
SElExceptionPtr	can_new(SCANPtr* pCAN, const char* options);
/*! Destroys an instance of CAN interface */
SElExceptionPtr can_delete(SCANPtr* pCAN);

/**
 *  Specified list of Communication objects
 */
typedef struct _SCANList
{
	SElExceptionPtr (*AddChannel)(
			struct _SCANList*	me,
			const char*			channel,
			const char*			options
		);  /*!< \see canOpen */
	SElExceptionPtr (*RemoveChannel)(
			struct _SCANList*	me,
			const char*			channel
		);  /*!< \see canOpen */

	SElExceptionPtr (*Open)(
			struct _SCANList*	me,
			const char*			channel,
			const char*			options
		);  /*!< \see canOpen */
    SElExceptionPtr (*Close)(
			struct _SCANList*	me,
			const char*			channel
		); /*!< \see canClose */

	SElExceptionPtr (*Stop)(
			struct _SCANList*	me,
			const char*			channel,
			bool_t				reset
		); /*!< \see canStop */

	SElExceptionPtr (*Write)(
			struct _SCANList*	pCAN,
			const char*			channel,
			uint32_t			address,
			uint32_t			dataLength,
			uint8_t*			pData
		); /*!< \see canWrite */
	SElExceptionPtr (*Read)(
			struct _SCANList*	pCAN,
			const char*			channel,
			uint32_t*			identifier,
			int32_t*			pDataLength,
			uint8_t*			pData
		); /*!< \see canRead */

	SElExceptionPtr (*WriteLowLevel)(
			struct _SCANList*	pCAN,
			const char*			channel,
			uint32_t			address,
			uint32_t			frame_type,
			uint32_t			dataLength,
			uint8_t*			pData
		); /*!< \see canWriteLowLevel */
	SElExceptionPtr (*ReadLowLevel)(
			struct _SCANList*	pCAN,
			const char*			channel,
			uint32_t*			identifier,
			int32_t*			pDataLength,
			uint8_t*			pData
		); /*!< \see canReadLowLevel */
	
	SElExceptionPtr (*Query)(
			struct _SCANList*	pCAN,
			const char*			channel,
			uint32_t			wIdent,
			uint8_t*			wData,
			uint32_t			wDataDize,
			uint32_t			rIdent,
			int32_t*			prDataSize,
			uint8_t*			rData
		); /*!< \see canQuery */
	SElExceptionPtr (*QueryEx)(
			struct _SCANList*	pCAN,
			const char*			channel,
			uint32_t			wIdent,
			uint8_t*			wData,
			uint32_t			wDataDize,
			uint32_t			rIdent,
			int32_t*			prDataSize,
			uint8_t*			rData
		); /*!< \see canQueryEx */
    SElExceptionPtr (*ClearCache)(struct _SCANList* me);
    const char* (*GetCache)(struct _SCANList* me);

	SElExceptionPtr (*StartThread)(
			struct _SCANList*	me,
			const char*			channel
		);
	SElExceptionPtr (*StopThread)(
			struct _SCANList*	me,
			const char*			channel
		);
	bool_t (*IsThreadRunning)(
			struct _SCANList*	me,
			const char*			channel
		);
	SElExceptionPtr (*ExitThread)(
			struct _SCANList*	me,
			const char*			channel
		);
	SElExceptionPtr (*AddThreadItem)(
			struct _SCANList*	me,
			const char*			channel,
			uint32_t			wIdentifier,
			uint32_t			wIdentType,
			uint8_t				data[],
			uint32_t			dataLength,
			uint32_t			writeTime
		);
	SElExceptionPtr (*ClearThreadItems)(
			struct _SCANList*	me,
			const char*			channel
		);
	SElExceptionPtr	(*SetTimeout)(
			struct _SCANList*	me,
			const char*			channel,
			uint32_t			ReadTimeout
		);
	SElExceptionPtr	(*GetTimeout)(
			struct _SCANList*	me,
			const char*			channel,
			uint32_t*			ReadTimeout
		);

	SElExceptionPtr	(*RegisterLogFunction)(
			struct _SCANList*	me,
			const char*			channel,
			PLogFuncPtr			LogFunc,
			PLogFuncPtr*		OldLogFunc
		);

	SElExceptionPtr (*SetAttribute)(
		struct _SCANList*	me,
		const char*			channel,
		uint32_t			dataType,
		uint32_t			attrId,
		void*				value
	);
	SElExceptionPtr (*GetAttribute)(
		struct _SCANList*	me,
		const char*			channel,
		uint32_t			dataType,
		uint32_t			attrId,
		void*				value,
		int32_t				valueSize
	);

/*******************************************
 * PRIVATE
 *******************************************/
	PLogFuncPtr			_logFunction;	/*!< Custom Log Function */
	SObjListPtr			_channelList;	/*!< List of opened channels */

	SELStringPtr		_pCache;		/*!< Cache data, all write and read data */
	STimerPtr			_pTimer;		/*!< Timer used in cache logs */
	bool_t				_CacheEnabled;	/*!< Is cache enabled */
} SCANList, *SCANListPtr;

/*! Creates a new instance of CAN interface */
SElExceptionPtr	canlist_new(SCANListPtr* pCAN, const char* options);
/*! Destroys an instance of CAN interface */
SElExceptionPtr canlist_delete(SCANListPtr* pCAN);

#define CAN_ERROR_COMMUNICATION				-1000000
#define CAN_ERROR_PARAMETERS				-1000001
#define CAN_ERROR_LONG_COMMAND				-1000002
#define CAN_ERROR_NOT_VALID_PATH			-1000003
#define CAN_ERROR_INVALID_INTERNAL_PATH		-1000004
#define CAN_ERROR_INVALID_SEND_LENGTH		-1000005
#define CAN_ERROR_TIMEOUT					-1000006
#define CAN_ERROR_INVALID_CONTINUE_RESPONSE	-1000007
#define CAN_ERROR_NEGATIVE_RESPONSE			-1000008
#define CAN_ERROR_NOT_SUPPORTED_BRATE		-1000009
#define CAN_ERROR_THREAD_RUNNING			-1000010
#define CAN_ERROR_INVALID_RESPONSE			-1000011
#define CAN_ERROR_CHANNEL_NOT_OPENED		-1000012
#define CAN_ERROR_CHANNEL_ALLREADY_OPENED	-1000013
#define CAN_ERROR_CHANNEL_ALLREADY_EXISTS	-1000014
#define CAN_ERROR_CHANNEL_NOT_EXISTS		-1000015
#define CAN_ERROR_BUS_ERROR_FRAME			-1000016
#define CAN_ERROR_OPTION_STR_NOT_DEFINED	-1000100

#define CAN_WARN_INVALID_BUFFER_SIZE		100001
#define CAN_WARN_DEFAULT_CHIP_CONFIGURATION	100002


#if defined(__cplusplus) || defined(__cplusplus__)
	}
#endif

#endif /* __CAN_H__*/ 
