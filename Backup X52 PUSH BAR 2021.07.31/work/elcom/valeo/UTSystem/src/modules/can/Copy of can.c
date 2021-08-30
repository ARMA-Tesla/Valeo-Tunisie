/****************************************************************************
 *                              C A N
 *---------------------------------------------------------------------------
 *    Copyright  @ 2005 ELCOM, a.s.  All Rights Reserved.        
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:       can.c                                                    
 * Purpose:     CAN interface (Controled Area Network)
 ****************************************************************************/

#include "can.h"
#include <elerror.h>
#include <stdio.h>
#include <time.h>

#define HIGH_LEVEL_TYPE_UNKNOWN		-1
#define HIGH_LEVEL_TYPE_KWP			0
#define HIGH_LEVEL_TYPE_PL3			1

#define READ_MAX_BLOCK_SIZE			5	/*!< Max block send from panel in one cycle */
#define READ_SEPARATION_TIME		10	/*!< Separation time between send frames from panel */ 

#define TIMEOUT_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( CAN_ERROR_TIMEOUT, "CAN Timeout"); } else

typedef struct _threadParam
{
	SCANPtr		me;
	uint32_t	writeTime;
} SThreadParam, *SThreadParamPtr;

/*****************************************************************************
 * Static no-exported functions declaration **********************************
 *****************************************************************************/
static SElExceptionPtr canLock(struct _SCAN* me);
static SElExceptionPtr canUnlock(struct _SCAN* me);

static SElExceptionPtr canOpen(
    struct _SCAN*	me,
    const char*		channel,
    const char*		options
);
static SElExceptionPtr canClose(struct _SCAN* me);
static SElExceptionPtr canWriteKWP(
	struct _SCAN*	me,
	uint32_t		address,
	uint32_t		dataLength,
	uint8_t*		pData
);
static SElExceptionPtr canWriteKWPPL3(
	struct _SCAN*	me,
	uint32_t		address,
	uint32_t		dataLength,
	uint8_t*		pData
);
static SElExceptionPtr canWriteLowLevel(
	struct _SCAN*	me,
	uint32_t		address,
	uint32_t		frame_type,
	uint32_t		dataLength,
	uint8_t*		pData
);
static SElExceptionPtr canClearCache(struct _SCAN* me);
static const char* canGetCache(struct _SCAN* me);
static SElExceptionPtr canReadKWP(
    struct _SCAN*	me,
    uint32_t*		identifier,
    int32_t*		pDataLength,
    uint8_t*		pData
);
static SElExceptionPtr canReadKWPPL3(
    struct _SCAN*	me,
    uint32_t*		identifier,
    int32_t*		pDataLength,
    uint8_t*		pData
);
static SElExceptionPtr canReadLowLevel(
    struct _SCAN*	me,
    uint32_t*		identifier,
    int32_t*		pDataLength,
    uint8_t*		pData
);
static SElExceptionPtr AppendCacheData(
    struct _SCAN*   me,
    int32_t         dType,
    int32_t         identifier,
    uint8_t*        pData,
    uint32_t        dSize,
	NCTYPE_ABS_TIME	timestamp
);
static SElExceptionPtr canQueryKWP(
	struct _SCAN*	me,
	uint32_t		wIdent,
	uint8_t*		wData,
	uint32_t		wDataDize,
	uint32_t		rIdent,
	int32_t*		prDataSize,
	uint8_t*		rData
);
static SElExceptionPtr canQueryKWP2(
	struct _SCAN*	me,
	uint32_t		wIdent,
	uint8_t*		wData,
	uint32_t		wDataDize,
	uint32_t		rIdent,
	int32_t*		prDataSize,
	uint8_t*		rData
);
static SElExceptionPtr canQueryKWPPL3(
	struct _SCAN*	me,
	uint32_t		wIdent,
	uint8_t*		wData,
	uint32_t		wDataDize,
	uint32_t		rIdent,
	int32_t*		prDataSize,
	uint8_t*		rData
);
static SElExceptionPtr canSetTimeout(struct _SCAN* me, uint32_t timeout);
static SElExceptionPtr canGetTimeout(struct _SCAN* me, uint32_t* timeout);
static SElExceptionPtr canSetAttribute(
	struct _SCAN*	me,
	uint32_t		dataType,
	uint32_t		attrId,
	void*			value
);
static SElExceptionPtr canGetAttribute(
	struct _SCAN*	me,
	uint32_t		dataType,
	uint32_t		attrId,
	void*			value,
	int32_t			valueSize
);
static SElExceptionPtr canStartThread(struct _SCAN* me);
static SElExceptionPtr canStopThread(struct _SCAN* me);
static bool_t canIsThreadRunning(struct _SCAN* me);
static SElExceptionPtr canStartThreadNonCPU(struct _SCAN* me);
static SElExceptionPtr canStopThreadNonCPU(struct _SCAN* me);
static SElExceptionPtr canAddThreadItem(
	struct _SCAN*	me,
	uint32_t		wIdentifier,
	uint32_t		wIdentType,
	uint8_t			data[],
	uint32_t		dataLength,
	uint32_t		writeTime
);
static SElExceptionPtr canClearThreadItems(struct _SCAN* me);
bool_t IsTimedThread(SCANPtr me, uint32_t period);
DWORD WINAPI asyncThread(
  LPVOID lpParameter
);
static SElExceptionPtr waitForFlowControl(
	SCANPtr		me,
	uint32_t	address,
	uint8_t*	blockSize,
	uint8_t*	waitTime,
	uint32_t	timeout
);

#if defined(_CVI_)
int CVICALLBACK SecondThreadFunc (void *pArguments);
#else
unsigned __stdcall SecondThreadFunc( void* pArguments );
#endif

/*---------------------------------------------------------------------------*/
static SElExceptionPtr canlistAddChannel(
	struct _SCANList*	me,
	const char*			channel,
	const char*			options
);
static SElExceptionPtr canlistRemoveChannel(
	struct _SCANList*	me,
	const char*			channel
);

static SElExceptionPtr canlistOpen(
	struct _SCANList*	me,
	const char*			channel,
	const char*			options
);
static SElExceptionPtr canlistClose(
	struct _SCANList*	me,
	const char*			channel
);

static SElExceptionPtr canlistWrite(
	struct _SCANList*	pCAN,
	const char*			channel,
	uint32_t			address,
	uint32_t			dataLength,
	uint8_t*			pData
);
static SElExceptionPtr canlistRead(
	struct _SCANList*	pCAN,
	const char*			channel,
	uint32_t*			identifier,
	int32_t*			pDataLength,
	uint8_t*			pData
);

static SElExceptionPtr canlistWriteLowLevel(
	struct _SCANList*	pCAN,
	const char*			channel,
	uint32_t			address,
	uint32_t			frame_type,
	uint32_t			dataLength,
	uint8_t*			pData
);
static SElExceptionPtr canlistReadLowLevel(
	struct _SCANList*	pCAN,
	const char*			channel,
	uint32_t*			identifier,
	int32_t*			pDataLength,
	uint8_t*			pData
);
	
static SElExceptionPtr canlistQuery(
	struct _SCANList*	pCAN,
	const char*			channel,
	uint32_t			wIdent,
	uint8_t*			wData,
	uint32_t			wDataDize,
	uint32_t			rIdent,
	int32_t*			prDataSize,
	uint8_t*			rData
);
static SElExceptionPtr canlistQueryEx(
	struct _SCANList*	pCAN,
	const char*			channel,
	uint32_t			wIdent,
	uint8_t*			wData,
	uint32_t			wDataDize,
	uint32_t			rIdent,
	int32_t*			prDataSize,
	uint8_t*			rData
);
static SElExceptionPtr canlistClearCache(struct _SCANList* me);
static const char* canlistGetCache(struct _SCANList* me);

static SElExceptionPtr canlistStartThread(
	struct _SCANList*	me,
	const char*			channel
);
static SElExceptionPtr canlistStopThread(
	struct _SCANList*	me,
	const char*			channel
);
static bool_t canlistIsThreadRunning(
	struct _SCANList*	me,
	const char*			channel
);
static SElExceptionPtr canlistExitThread(
	struct _SCANList*	me,
	const char*			channel
);
static SElExceptionPtr canlistAddThreadItem(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			wIdentifier,
	uint32_t			wIdentType,
	uint8_t				data[],
	uint32_t			dataLength,
	uint32_t			writeTime
);
static SElExceptionPtr canlistClearThreadItems(
	struct _SCANList*	me,
	const char*			channel
);
static SElExceptionPtr	canlistSetTimeout(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			ReadTimeout
);
static SElExceptionPtr	canlistGetTimeout(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t*			ReadTimeout
);

static SElExceptionPtr	canlistRegisterLogFunction(
	struct _SCANList*	me,
	const char*			channel,
	PLogFuncPtr			LogFunc,
	PLogFuncPtr*		OldLogFunc
);

static SElExceptionPtr canlistSetAttribute(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			dataType,
	uint32_t			attrId,
	void*				value
);
static SElExceptionPtr canlistGetAttribute(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			dataType,
	uint32_t			attrId,
	void*				value,
	int32_t				valueSize
);

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "can_new"
/*!
 * \brief
 * This function creates a new instance.
 * 
 * \param pError 
 * Returns a value indicating whether an error occurred.
 * A negative error code indicates function failure.	
 * 
 * \return 
 * Pointer to new instance of the CAN object. If the function fails, NULL
 * pointer is returned
 *
 * This function creates a new instance of the CAN object.
 */
SElExceptionPtr can_new(SCANPtr* pCAN, const char* options)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	char*			p2start = (char*) options;
	char*			p2end = NULL;
	char			tmp_string[128] = "";
	int				cpu_used = 0;
	int				high_level_type = HIGH_LEVEL_TYPE_KWP;

	/* Parse Options */
	p2end = strchr(p2start, ';');
	if (p2end)
	{
		strncpy_s(tmp_string, sizeof(tmp_string), p2start, p2end - p2start);
		tmp_string[p2end - p2start] = '\0';
	}
	else
		strcpy_s(tmp_string, sizeof(tmp_string), p2start);

	cpu_used = !strcmp(tmp_string, "CPU");
	if (p2end)
	{
		p2start = p2end + 1;
		p2end = strchr(p2start, ';');
		if (p2end)
		{
			strncpy_s(tmp_string, sizeof(tmp_string), p2start, p2end - p2start);
			tmp_string[p2end - p2start] = '\0';
		}
		else
			strcpy_s(tmp_string, sizeof(tmp_string), p2start);

		if (!strcmp(tmp_string, "PL3"))
			high_level_type = HIGH_LEVEL_TYPE_PL3;
		else
			high_level_type = HIGH_LEVEL_TYPE_KWP;
	}

	pcan = calloc( 1, sizeof(SCAN));
	EXCCHECKALLOC( pcan);

	pcan->Open  = canOpen;
	pcan->Close = canClose;
	switch (high_level_type)
	{
	case HIGH_LEVEL_TYPE_KWP:
		pcan->Write = canWriteKWP;
		pcan->Read  = canReadKWP;
		pcan->Query  = canQueryKWP;
		pcan->QueryEx= canQueryKWP2;
		break;
	case HIGH_LEVEL_TYPE_PL3:
		pcan->Write = canWriteKWPPL3;
		pcan->Read  = canReadKWPPL3;
		pcan->Query  = canQueryKWPPL3;
		pcan->QueryEx= canQueryKWPPL3;
		break;
	}
	pcan->WriteLowLevel = canWriteLowLevel;
	pcan->ReadLowLevel  = canReadLowLevel;
	pcan->SetTimeout  = canSetTimeout;
	pcan->GetTimeout  = canGetTimeout;
	pcan->StartThread = (cpu_used) ? canStartThread : canStartThreadNonCPU;
	pcan->StopThread = (cpu_used) ? canStopThread : canStopThreadNonCPU;
	pcan->IsThreadRunning = canIsThreadRunning;
	pcan->ClearThreadItems = canClearThreadItems;
	pcan->AddThreadItem = canAddThreadItem;
	pcan->ClearCache = canClearCache;
	pcan->GetCache = canGetCache;
	pcan->SetAttribute = canSetAttribute;
	pcan->GetAttribute = canGetAttribute;
	
	InitializeCriticalSection(&(pcan->_Lock));

	pcan->_timeout = 2000;
	pcan->_netObj  = 0;
	pcan->_uutAddress = 0x678;
	pcan->_testerAddress = 0x6F1;

#if 0
	EXCCHECK( elstring_new(NULL, &(pcan->_pCache)));
	EXCCHECK( timer_new( &(pcan->_pTimer)));
#else
	pcan->_pCache = NULL;
	pcan->_pTimer = NULL;
#endif

Error:
	if ( pCAN != NULL )
	{
		*pCAN = pcan;
	}
	
	EXCRETHROW(pexception);
}/* can_new */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "can_delete"
/*!
 * \brief
 * This function destroys specified instance.
 * 
 * \param pCAN 
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
SElExceptionPtr can_delete(SCANPtr* pCAN)
{
	SElExceptionPtr	pexception = NULL;

	if ( pCAN && *pCAN )
	{
		EXCCHECK( (*pCAN)->StopThread(*pCAN));
		EXCCHECK( (*pCAN)->ClearThreadItems(*pCAN));
		EXCCHECK( (*pCAN)->Close(*pCAN));
		DeleteCriticalSection( &((*pCAN)->_Lock));

#if 0
		EXCCHECK( elstring_delete( &((*pCAN)->_pCache)));
		EXCCHECK( timer_delete( &((*pCAN)->_pTimer)));
#endif

		free( *pCAN);
		*pCAN = NULL;
	}

Error:
	EXCRETHROW(pexception);
} /* can_delete */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canOpen"
/*!
 * \brief
 * This function opens CAN interface
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
 * 
 * \patam channel   Reserved for future use, USe NULL or empty string
 * \patam options   Specifies the CAN parameters as Low or High speed
 *                  and baud rate.
 *                  Format is following:\n
 *                  <Port>:<H|L><baudrate>\n
 *                  More ports can be spearated by semicolon\n
 *                  For example:\n
 *                  "CAN1:H500000;CAN2:L100000"
 *                  Can 1 is high can witk speed 500kBauds and CAN2 is low speed can with 100kBauds
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
static SElExceptionPtr canOpen(
	struct _SCAN*	me,
    const char*		channel,
    const char*		options
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	const char*		p2conf = NULL;
	char*			p2next = NULL;
	
	if(0==strncmp(options, "CAN", strlen("CAN")))
	{
#undef ATTR_SIZE
#define	ATTR_SIZE			8

		NCTYPE_ATTRID attr_id_list[ATTR_SIZE];
		NCTYPE_UINT32 attr_value_list[ATTR_SIZE];

		sprintf_s(me->_prefix, sizeof(me->_prefix), "CAN");

		/* Default settings */
		attr_id_list[0]		= NC_ATTR_BAUD_RATE;
		attr_value_list[0]	= 0x8000FAC3; //B2YY: 0x80002387; //PL3:0x8000FAC3
		attr_id_list[1]		= NC_ATTR_START_ON_OPEN;
		attr_value_list[1]	= NC_FALSE;
		attr_id_list[2]		= NC_ATTR_READ_Q_LEN;
		attr_value_list[2]	= 100; //uprava polak 23.4.2010 pro test 15.01 puvodni hodnota 100
		attr_id_list[3]		= NC_ATTR_WRITE_Q_LEN;
		attr_value_list[3]	= 0; //uprava Polak 6.5.2010 pro test 15.01 puvodni hodnota 0
		attr_id_list[4]		= NC_ATTR_CAN_COMP_STD;
		attr_value_list[4]	= 0;
		attr_id_list[5]		= NC_ATTR_CAN_MASK_STD;
		attr_value_list[5]	= NC_CAN_MASK_STD_DONTCARE;
		attr_id_list[6]		= NC_ATTR_CAN_COMP_XTD;
		attr_value_list[6]	= 0;
		attr_id_list[7]		= NC_ATTR_CAN_MASK_XTD;
		attr_value_list[7]	= NC_CAN_MASK_XTD_DONTCARE;

		p2conf = strchr(options, ':');
		if (p2conf)
		{
			/* Get Baud Rate */
			p2conf++;
			if (!strncmp(p2conf, "0x", 2))
			{
				attr_value_list[0] = strtoul(p2conf + 2, &p2next, 16);
			}
			else
			{
				attr_value_list[0] = strtoul(p2conf, &p2next, 10);
			}

			/* TODO: parse other configuration options */
		}

		EXCCHECKCAN( ncConfig((NCTYPE_STRING) channel, ATTR_SIZE, attr_id_list, attr_value_list));
	}
	else if(0==strncmp(options, "LIN", strlen("LIN")))
	{
		sprintf_s(me->_prefix, sizeof(me->_prefix), "LIN");
	}
	else
	{
		EXCTHROW( CAN_ERROR_OPTION_STR_NOT_DEFINED, "Option string not defined for a CAN/LIN open interface");
	}

	EXCCHECKCAN( ncOpenObject((NCTYPE_STRING)channel, &(me->_netObj)));

	if(0==strncmp(options, "LIN", strlen("LIN")))
	{
		int32_t baudrate = 9600;
		int32_t self_rec = 1;
		int32_t termination = 1;
		int32_t bus_err = 1;
		int32_t	crc_type = 0;

		p2conf = strchr(options, ':');
		/* Get LIN configuration */
		if (p2conf)
		{
			p2conf++;
			baudrate = strtol(p2conf, &p2next, 10);

			if (p2next)
			{
				self_rec = strtol(p2next + 1, &p2next, 10);

				if (p2next)
				{
					termination = strtol(p2next + 1, &p2next, 10);

					if (p2next)
					{
						bus_err = strtol(p2next + 1, &p2next, 10);

						if (p2next)
						{
							crc_type = strtol(p2next + 1, &p2next, 10);
						}
					}
				}
			}
		}

		EXCCHECKCAN( ncSetAttribute(me->_netObj, NC_ATTR_BAUD_RATE, sizeof(int32_t), &baudrate));
		EXCCHECKCAN( ncSetAttribute(me->_netObj, NC_ATTR_SELF_RECEPTION, sizeof(int32_t), &self_rec));
		EXCCHECKCAN( ncSetAttribute(me->_netObj, NC_ATTR_TERMINATION, sizeof(int32_t), &termination));
		EXCCHECKCAN( ncSetAttribute(me->_netObj, NC_ATTR_LOG_BUS_ERROR, sizeof(int32_t), &bus_err));
		EXCCHECKCAN( ncSetAttribute(me->_netObj, NC_ATTR_LIN_CHECKSUM_TYPE, sizeof(int32_t), &crc_type));
	}

Error:
	EXCRETHROW(pexception);
} /* canOpen */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canClose"
/*!
 * \brief
 * This function closes CAN interface
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
 */
static SElExceptionPtr canClose(struct _SCAN* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	EXCCHECK( me->StopThread( me));
	if (me->_netObj)
	{
		EXCCHECKCAN( ncCloseObject(me->_netObj));
		me->_netObj = 0;
	}

Error:
	EXCRETHROW(pexception);
} /* canClose */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canQuery"
/*!
 * This function writes comands and waits for the response
 *
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
 *
 * \param pChanel
 * Name of the channel
 *
 * \param wIdent	CAN Indentifier for write command
 * \param wData		Write Command
 * \param wDataSize	Size of valid bytes in write command
 * \param rIdent	CAN Indentifier of response
 * \param prDataSize Size of the response buffer. This parameter has
 *                   input/output funcionality
 * \param rData`	Read buffer
 *
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n\n
 *  If the current value of the return buffer, is larger than the size
 *  you indicate in the \p prDataSize parameter, the function copies
 *  prDataSize bytes into the buffer, and returns the buffer size
 *  you must pass to get the entire value.
 *  For example, if the value is 0x03 0x7f 0x80 0x02 and the prDataSize is 2,
 *  the function places 0x03 0x7f into the buffer and returns 4.\n\n
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
static SElExceptionPtr canQueryKWP(
	struct _SCAN*  me,
	uint32_t	wIdent,
	uint8_t*	wData,
	uint32_t	wDataSize,
	uint32_t	rIdent,
	int32_t*	prDataSize,
	uint8_t*	rData
)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		identifier = 0;

	me->_testerAddress = (uint16_t)rIdent;
	EXCCHECK( me->Write(me, wIdent, wDataSize, wData));
	identifier = rIdent;
	me->_uutAddress = (uint16_t)wIdent;
	EXCCHECK( me->Read(me, &identifier, prDataSize, rData));

	if (*prDataSize > 1)
	{
		if (rData[0] != wData[0] + 0x40 ||
			memcmp(&rData[1], &wData[1], wDataSize - 1))
		{
			EXCTHROW(CAN_ERROR_INVALID_RESPONSE, "Invalid Response Frame");
		}

		if (*prDataSize > wDataSize)
		{
			memmove(rData, rData + wDataSize, *prDataSize - wDataSize); 
			(*prDataSize) -= wDataSize;
		}
	}
	else
	{
		if (rData[0] != wData[0] + 0x40)
		{
			EXCTHROW(CAN_ERROR_INVALID_RESPONSE, "Invalid Response Frame");
		}

		memmove(rData, rData + 1, 1);
		(*prDataSize) = 0;
	}

Error:
	EXCRETHROW(pexception);
} /* canQueryKWP */

static SElExceptionPtr canQueryKWPPL3(
	struct _SCAN*  me,
	uint32_t	wIdent,
	uint8_t*	wData,
	uint32_t	wDataSize,
	uint32_t	rIdent,
	int32_t*	prDataSize,
	uint8_t*	rData
)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		identifier = 0;

	EXCCHECK( me->Write(me, wIdent, wDataSize, wData));
	identifier = rIdent;
	EXCCHECK( me->Read(me, &identifier, prDataSize, rData));

	if (rData[0] != wData[0] + 0x40)
	{
		EXCTHROW(CAN_ERROR_INVALID_RESPONSE, "Invalid Response Frame");
	}
	
	memmove(rData, rData + 1, *prDataSize - 1); 
	(*prDataSize) -= 1;

Error:
	EXCRETHROW(pexception);
} /* canQueryKWPPL3 */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canQueryEx"
/*!
 * This function writes comands and waits for the response
 *
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
 *
 * \param pChanel
 * Name of the channel
 *
 * \param wIdent	CAN Indentifier for write command
 * \param wData		Write Command
 * \param wDataSize	Size of valid bytes in write command
 * \param rIdent	CAN Indentifier of response
 * \param prDataSize Size of the response buffer. This parameter has
 *                   input/output funcionality
 * \param rData`	Read buffer
 *
 * \return 
 *  Returns the status code of this operation.  The status code either
 *  indicates success or describes an error or warning condition.  You examine
 *  the status code from each call to an log function to determine
 *  if an error occurred.\n\n
 *  If the current value of the return buffer, is larger than the size
 *  you indicate in the \p prDataSize parameter, the function copies
 *  prDataSize bytes into the buffer, and returns the buffer size
 *  you must pass to get the entire value.
 *  For example, if the value is 0x03 0x7f 0x80 0x02 and the prDataSize is 2,
 *  the function places 0x03 0x7f into the buffer and returns 4.\n\n
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
static SElExceptionPtr canQueryKWP2(
	struct _SCAN*  me,
	uint32_t	wIdent,
	uint8_t*	wData,
	uint32_t	wDataSize,
	uint32_t	rIdent,
	int32_t*	prDataSize,
	uint8_t*	rData
)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		identifier = 0;

	EXCCHECK( me->Write(me, wIdent, wDataSize, wData));
	identifier = rIdent;
	me->_uutAddress = (uint16_t)wIdent;
	EXCCHECK( me->Read(me, &identifier, prDataSize, rData));

	if (rData[0] != wData[0] + 0x40)
	{
		EXCTHROW(CAN_ERROR_INVALID_RESPONSE, "Invalid Response Frame");
	}
	
	memmove(rData, rData + 1, *prDataSize-1); 
	(*prDataSize)--;
	
Error:
	EXCRETHROW(pexception);
} /* canQueryKWP2 */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canWrite"
/*!
 * \brief
 * This function writes data to CAN interface
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
static SElExceptionPtr canWriteKWP(
	struct _SCAN*  me,
	uint32_t       address,
	uint32_t       dataLength,
	uint8_t*       pData
)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0,
						data_size = 0,
						i = 0;
	uint8_t				new_data[8];
	uint8_t*			p2send_data = NULL;
	uint8_t				ext_size = 0,
						sn = 0,
						block_size = 0,
						wait_time = 0;
	uint32_t			identifier = 0;
	
	if (dataLength > 7)
	{
		if (dataLength > 4095)
			EXCTHROW( CAN_ERROR_INVALID_SEND_LENGTH, "Write data size cross over maximum send size");
			
		new_data[0] = (uint8_t) 0x10 | (dataLength / 256);
		new_data[1] = (uint8_t) dataLength % 256;
		memcpy(&new_data[2], pData, 6);
		EXCCHECK( canWriteLowLevel(me, address, NC_FRMTYPE_DATA, 8, new_data));
		EXCCHECK( waitForFlowControl(me, address, &block_size, &wait_time, me->_timeout));
		
		i = 0;
		sn = 1;
		p2send_data = pData + 6;
		data_size = dataLength - 6;
		do
		{
			memset(new_data, 0xFF, 8);
			new_data[0] = (uint8_t) 0x20 | sn;
			memcpy(&new_data[1], p2send_data, ((data_size > 7) ? 7 : data_size));
			EXCCHECK( canWriteLowLevel(me, address, NC_FRMTYPE_DATA, 8, new_data));
			if (++sn > 15)
				sn = 0;
			
			p2send_data += 7;
			data_size -= 7;	
			
			if (--block_size <= 0)
			{
				EXCCHECK( waitForFlowControl(me, address, &block_size, &wait_time, me->_timeout));
			}
			else
			{
				if (data_size > 0)
				{
					timeBeginPeriod(1);
					Sleep(wait_time);
					timeEndPeriod(1);
				}
			}
		} while (data_size > 0);
	}
	else
	{
		memset(new_data, 0xFF, 8);
		new_data[0] = (uint8_t) dataLength;
		memcpy(&new_data[1], pData, dataLength);		
		EXCCHECK( canWriteLowLevel(me, address, NC_FRMTYPE_DATA, 8, new_data));
	}

Error:
	EXCRETHROW(pexception);
} /* canWriteKWP */

static SElExceptionPtr canWriteKWPPL3(
	struct _SCAN*  me,
	uint32_t       address,
	uint32_t       dataLength,
	uint8_t*       pData
)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0,
						data_size = 0,
						i = 0;
	uint8_t				new_data[8];
	uint8_t*			p2send_data = NULL;
	uint8_t				ext_size = 0,
						sn = 0,
						block_size = 0,
						wait_time = 0;
	uint32_t			identifier = 0;
	
	if (dataLength > 6)
	{
		if (dataLength > 4095)
			EXCTHROW( CAN_ERROR_INVALID_SEND_LENGTH, "Write data size cross over maximum send size");
		
		new_data[0] = (uint8_t) (me->_uutAddress & 0xFF);
		new_data[1] = (uint8_t) 0x10 | (dataLength / 256);
		new_data[2] = (uint8_t) dataLength % 256;
		memcpy(&new_data[3], pData, 5);
		EXCCHECK( canWriteLowLevel(me, address, NC_FRMTYPE_DATA, 8, new_data));
		EXCCHECK( waitForFlowControl(me, address, &block_size, &wait_time, me->_timeout));
		
		i = 0;
		sn = 1;
		p2send_data = pData + 6;
		data_size = dataLength - 6;
		do
		{
			memset(new_data, 0xFF, 8);
			new_data[0] = (uint8_t) (me->_uutAddress & 0xFF);
			new_data[1] = (uint8_t) 0x20 | sn;
			memcpy(&new_data[2], p2send_data, ((data_size > 6) ? 6 : data_size));
			EXCCHECK( canWriteLowLevel(me, address, NC_FRMTYPE_DATA, 8, new_data));
			if (++sn > 15)
				sn = 0;
			
			p2send_data += 6;
			data_size -= 6;	
			
			if (--block_size <= 0)
			{
				EXCCHECK( waitForFlowControl(me, address, &block_size, &wait_time, me->_timeout));
			}
			else
			{
				if (data_size > 0)
				{
					timeBeginPeriod(1);
					Sleep(wait_time);
					timeEndPeriod(1);
				}
			}
		} while (data_size > 0);
	}
	else
	{
		memset(new_data, 0xFF, 8);
		new_data[0] = (uint8_t) (me->_uutAddress & 0xFF);
		new_data[1] = (uint8_t) dataLength;
		memcpy(&new_data[2], pData, dataLength);		
		EXCCHECK( canWriteLowLevel(me, address, NC_FRMTYPE_DATA, 8, new_data));
	}

Error:
	EXCRETHROW(pexception);
} /* canWriteKWPPL3 */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canWriteLowLevel"
/*!
 * \brief
 * This function writes data to CAN interface
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
static SElExceptionPtr canWriteLowLevel(
	struct _SCAN*  me,
	uint32_t       address,
	uint32_t	   frame_type,
	uint32_t       dataLength,
	uint8_t*       pData
)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0,
						data_size = 0;
	NCTYPE_CAN_FRAME	data_packet;
	NCTYPE_ABS_TIME		timestamp = {0,0};
	NCTYPE_STATE		state;
	
	data_size = (dataLength > 8) ? 8 : dataLength;

	EXCCHECKCAN( ncAction (me->_netObj, NC_OP_START, 0));

	data_packet.ArbitrationId	= address;
	data_packet.IsRemote		= frame_type;
	data_packet.DataLength		= data_size;
	memcpy(data_packet.Data, pData, data_size);

	EXCCHECK( AppendCacheData(me, CAN_WRITE, address, pData, data_size, timestamp));
	EXCCHECKCAN( ncWrite(me->_netObj, sizeof(data_packet), &data_packet));

	/* TODO: check state for CAN communication: problem with nonCPU mode */

	/* check state for LIN communication */
	if(frame_type==NC_FRMTYPE_LIN_RESPONSE_ENTRY || frame_type==NC_FRMTYPE_LIN_HEADER)
	{
		error = ncWaitForState(me->_netObj, (NC_ST_WRITE_SUCCESS | NC_ST_ERROR), me->_timeout, &state);
		EXCCHECKCAN( error);
	}

Error:
	EXCRETHROW(pexception);
} /* canWriteLowLevel */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canSetTimeout"
/*!
 * \brief
 * This function writes data to CAN interface
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
 *
 * \param timeout
 * Specifies Timeout for Query Function
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
static SElExceptionPtr canSetTimeout(
	struct _SCAN*	me,
	uint32_t		timeout
)
{
	SElExceptionPtr		pexception = NULL;
	
	me->_timeout = timeout;

//Error:
	EXCRETHROW(pexception);
} /* canSetTimeout */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canGetTimeout"
/*!
 * \brief
 * This function writes data to CAN interface
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
 *
 * \param timeout
 * Specifies Timeout for Query Function
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
static SElExceptionPtr canGetTimeout(
	struct _SCAN*	me,
	uint32_t*		timeout
)
{
	SElExceptionPtr		pexception = NULL;
	
	if (timeout)
		*timeout = me->_timeout;

//Error:
	EXCRETHROW(pexception);
} /* canGetTimeout */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canSetAttribute"
static SElExceptionPtr canSetAttribute(
	struct _SCAN*	me,
	uint32_t		dataType,
	uint32_t		attrId,
	void*			value
)
{
	SElExceptionPtr		pexception = NULL;
	
	switch (attrId)
	{
	case CAN_ATTR_TIMEOUT:
		if (dataType != DATA_TYPE_UINT32)
			EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_TIMEOUT");

		me->_timeout = *((uint32_t*) value);
		break;

	case CAN_ATTR_CACHE_ENABLED:
		if (dataType != DATA_TYPE_INT32)
			EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_CACHE_ENABLED");

		me->_CacheEnabled = *((bool_t*) value);
		break;

	case CAN_ATTR_UUT_ADDRESS:
		if (dataType != DATA_TYPE_UINT16)
			EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_UUT_ADDRESS");

		me->_uutAddress = *((uint16_t*) value);
		break;
	
	case CAN_ATTR_TESTER_ADDRESS:
		if (dataType != DATA_TYPE_UINT16)
			EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_TESTER_ADDRESS");

		me->_testerAddress = *((uint16_t*) value);
		break;

	default:
		EXCTHROW(-1, "Unknown Attribute");
	}

Error:
	EXCRETHROW(pexception);
} /* canSetAttribute */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canGetAttribute"
static SElExceptionPtr canGetAttribute(
	struct _SCAN*	me,
	uint32_t		dataType,
	uint32_t		attrId,
	void*			value,
	int32_t			valueSize
)
{
	SElExceptionPtr		pexception = NULL;
	
	switch (attrId)	
	{
	case CAN_ATTR_TIMEOUT:
		if (dataType == DATA_TYPE_UINT32)
			EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_TIMEOUT");

		*((uint32_t*) value) = me->_timeout;
		break;

	case CAN_ATTR_CACHE_ENABLED:
		if (dataType == DATA_TYPE_INT32)
			EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_CACHE_ENABLED");

		*((bool_t*) value) = me->_CacheEnabled;
		break;

	case CAN_ATTR_UUT_ADDRESS:
		if (dataType == DATA_TYPE_UINT16)
			EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_UUT_ADDRESS");

		*((uint16_t*) value) = me->_uutAddress;
		break;
	
	case CAN_ATTR_TESTER_ADDRESS:
		if (dataType == DATA_TYPE_UINT16)
			EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_TESTER_ADDRESS");

		*((uint16_t*) value) = me->_testerAddress;
		break;

	default:
		EXCTHROW(-1, "Unknown Attribute");
	}

Error:
	EXCRETHROW(pexception);
} /* canGetAttribute */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canRead"
/*!
 * \brief
 * This function reads data from CAN interface
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
static SElExceptionPtr canReadKWP(
	struct _SCAN*	me,
	uint32_t*		identifier,
	int32_t*		pDataLength,
	uint8_t*		pData
)
{
	SElExceptionPtr		pexception = NULL;
	uint8_t				read_data[8];
	uint8_t*			p2full_buffer = NULL;
	uint8_t*			p2end = NULL;
	int32_t				read_size = 0,
						full_data_size = 0;
	char				message[256] = "";
	
	if (pDataLength == NULL ||
		(*pDataLength == 0) ||
		pData == NULL)
	{
		EXCTHROW( CAN_ERROR_PARAMETERS, "Buffer size not defined or buffer is NULL");
	}

	do
	{
		read_size = sizeof(read_data);
		EXCCHECK( canReadLowLevel( me, identifier, &read_size, read_data));
		
		if (read_data[1] == 0x7F)
		{
			if (read_data[3] != 0x78)
			{
				sprintf_s(message, sizeof(message), "Negative Response (Service: %02x) Error Code: %02x",
						read_data[2], read_data[3]);
				memcpy(pData, &read_data[1], (*pDataLength < (read_size - 1)) ? *pDataLength : read_size - 1);
				EXCTHROW(CAN_ERROR_NEGATIVE_RESPONSE, message);
			}
			else
			{
				Sleep(50);
				continue;
			}
		}
		
		if ((read_data[0] & 0xF0) == 0x00)
		{
			/* Single Frame */
			read_size = read_data[0] & 0x0F;
			memcpy(pData, &read_data[1], (*pDataLength < read_size) ? *pDataLength : read_size);
			*pDataLength = read_size;
			if (*pDataLength < read_size)
				EXCTHROW( CAN_WARN_INVALID_BUFFER_SIZE, "Buffer size is small. Data was truncate");
			break;
		}
		else if ((read_data[0] & 0xF0) == 0x10)
		{
			uint8_t		send_data[8];
			int32_t		rest2read = 0,
						block2read = 0,
						sn = 0;
			uint32_t	write_address = me->_uutAddress; //(*identifier) - 8;
			
			/* First Frame */
			full_data_size = (read_data[0] & 0x0F) << 8;
			full_data_size += read_data[1];
			
			p2full_buffer = calloc(full_data_size + 8, sizeof(uint8_t));
			p2end = p2full_buffer;
			memcpy(p2end, &read_data[2], 6);
			p2end += 6;
			rest2read = full_data_size - 6;
			
			/* Send Flow Control Frame */
			send_data[0] = 0x30;
			send_data[1] = READ_MAX_BLOCK_SIZE;
			send_data[2] = READ_SEPARATION_TIME;
			send_data[3] = 0xFF;
			send_data[4] = 0xFF;
			send_data[5] = 0xFF;
			send_data[6] = 0xFF;
			send_data[7] = 0xFF;
			EXCCHECK( canWriteLowLevel(me, write_address, NC_FRMTYPE_DATA, 8, send_data));
			block2read = READ_MAX_BLOCK_SIZE;
			
			do
			{
				read_size = sizeof(read_data);
				EXCCHECK( canReadLowLevel( me, identifier, &read_size, read_data));
				
				if (read_data[1] == 0x7F)
				{
					if (read_data[3] != 0x78)
					{
						sprintf_s(message, sizeof(message), "Negative Response (Service: %02x) Error Code: %02x",
								read_data[2], read_data[3]);
						memcpy(pData, &read_data[1], (*pDataLength < (read_size - 1)) ? *pDataLength : read_size - 1);
						EXCTHROW(CAN_ERROR_NEGATIVE_RESPONSE, message);
					}
					else
					{
						Sleep(50);
						continue;
					}
				}
				
				if ((read_data[0] & 0xF0) != 0x20)
					EXCTHROW( CAN_ERROR_INVALID_CONTINUE_RESPONSE, "Invalid Consecutive Frame Read");

				sn = read_data[0] & 0x0F;
				/* TODO: check if sn is incremented from previous read */
				memcpy( p2end, &read_data[1], 7);
				p2end += 7;
				rest2read -= 7;
				
				if (--block2read <= 0 && rest2read > 0)
				{
					EXCCHECK( canWriteLowLevel(me, write_address, NC_FRMTYPE_DATA, 8, send_data));
					block2read = READ_MAX_BLOCK_SIZE;
				}
			} while (rest2read > 0);
			
			memcpy(pData, p2full_buffer, (*pDataLength < full_data_size) ? *pDataLength : full_data_size);
			free(p2full_buffer);
			p2full_buffer = NULL;

			*pDataLength = full_data_size;
			if (*pDataLength < full_data_size)
				EXCTHROW( CAN_WARN_INVALID_BUFFER_SIZE, "Buffer size is small. Data was truncate");
			break;
		}
		else
		{
			/* Invalid frame response */
			EXCTHROW( CAN_ERROR_INVALID_CONTINUE_RESPONSE, "Invalid data frame read");
		}
	} while (1);

Error:
	free(p2full_buffer);
	EXCRETHROW(pexception);
} /* canReadKWP */

static SElExceptionPtr canReadKWPPL3(
	struct _SCAN*	me,
	uint32_t*		identifier,
	int32_t*		pDataLength,
	uint8_t*		pData
)
{
	SElExceptionPtr		pexception = NULL;
	uint8_t				read_data[8];
	uint8_t*			p2full_buffer = NULL;
	uint8_t*			p2end = NULL;
	int32_t				read_size = 0,
						full_data_size = 0;
	char				message[256] = "";
	
	if (pDataLength == NULL ||
		(*pDataLength == 0) ||
		pData == NULL)
	{
		EXCTHROW( CAN_ERROR_PARAMETERS, "Buffer size not defined or buffer is NULL");
	}

	do
	{
		read_size = sizeof(read_data);
		EXCCHECK( canReadLowLevel( me, identifier, &read_size, read_data));
		
		if (read_data[2] == 0x7F)
		{
			if (read_data[4] != 0x78)
			{
				sprintf_s(message, sizeof(message), "Negative Response (Service: %02x) Error Code: %02x",
						read_data[3], read_data[4]);
				memcpy(pData, &read_data[2], (*pDataLength < (read_size - 1)) ? *pDataLength : read_size - 2);
				EXCTHROW(CAN_ERROR_NEGATIVE_RESPONSE, message);
			}
			else
			{
				Sleep(50);
				continue;
			}
		}
		
		if (read_data[0] != (uint8_t)(me->_testerAddress & 0xFF))
		{
			sprintf_s(message, sizeof(message), "Invalid Address Response ( Expected: %02E, Read: %02E )",
					(int32_t) me->_testerAddress & 0xFF, (int32_t) read_data[0]);
			EXCTHROW( CAN_ERROR_INVALID_RESPONSE, message);
		}

		if ((read_data[1] & 0xF0) == 0x00)
		{
			/* Single Frame */
			read_size = read_data[1] & 0x0F;
			memcpy(pData, &read_data[2], (*pDataLength < read_size) ? *pDataLength : read_size);
			*pDataLength = read_size;
			if (*pDataLength < read_size)
				EXCTHROW( CAN_WARN_INVALID_BUFFER_SIZE, "Buffer size is small. Data was truncate");
			break;
		}
		else if ((read_data[1] & 0xF0) == 0x10)
		{
			uint8_t		send_data[8];
			int32_t		rest2read = 0,
						block2read = 0,
						sn = 0;
			uint32_t	write_address = me->_testerAddress;
			
			/* First Frame */
			full_data_size = (read_data[1] & 0x0F) << 8;
			full_data_size += read_data[2];
			
			p2full_buffer = calloc(full_data_size + 8, sizeof(uint8_t));
			p2end = p2full_buffer;
			memcpy(p2end, &read_data[3], 5);
			p2end += 5;
			rest2read = full_data_size - 5;
			
			/* Send Flow Control Frame */
			send_data[0] = (uint8_t) (me->_uutAddress & 0xFF);
			send_data[1] = 0x30;
			send_data[2] = 0x00;
			send_data[3] = 0x00;
			send_data[4] = 0xFF;
			send_data[5] = 0xFF;
			send_data[6] = 0xFF;
			send_data[7] = 0xFF;
			EXCCHECK( canWriteLowLevel(me, write_address, NC_FRMTYPE_DATA, 8, send_data));
			block2read = READ_MAX_BLOCK_SIZE;
			
			do
			{
				read_size = sizeof(read_data);
				EXCCHECK( canReadLowLevel( me, identifier, &read_size, read_data));
				
				if (read_data[2] == 0x7F)
				{
					if (read_data[4] != 0x78)
					{
						sprintf_s(message, sizeof(message), "Negative Response (Service: %02x) Error Code: %02x",
								read_data[3], read_data[4]);
						memcpy(pData, &read_data[2], (*pDataLength < (read_size - 1)) ? *pDataLength : read_size - 2);
						EXCTHROW(CAN_ERROR_NEGATIVE_RESPONSE, message);
					}
					else
					{
						Sleep(50);
						continue;
					}
				}

				if (read_data[0] != (uint8_t)(me->_testerAddress & 0xFF))
				{
					sprintf_s(message, sizeof(message), "Invalid Address Response ( Expected: %02E, Read: %02E )",
							(int32_t) me->_testerAddress & 0xFF, (int32_t) read_data[0]);
					EXCTHROW( CAN_ERROR_INVALID_RESPONSE, message);
				}
				
				if ((read_data[1] & 0xF0) != 0x20)
					EXCTHROW( CAN_ERROR_INVALID_CONTINUE_RESPONSE, "Invalid Consecutive Frame Read");

				sn = read_data[1] & 0x0F;
				/* TODO: check if sn is incremented from previous read */
				memcpy( p2end, &read_data[2], 6);
				p2end += 6;
				rest2read -= 6;
				
				if (--block2read <= 0 && rest2read > 0)
				{
					EXCCHECK( canWriteLowLevel(me, write_address, NC_FRMTYPE_DATA, 8, send_data));
					block2read = READ_MAX_BLOCK_SIZE;
				}
			} while (rest2read > 0);
			
			memcpy(pData, p2full_buffer, (*pDataLength < full_data_size) ? *pDataLength : full_data_size);
			free(p2full_buffer);
			p2full_buffer = NULL;

			*pDataLength = full_data_size;
			if (*pDataLength < full_data_size)
				EXCTHROW( CAN_WARN_INVALID_BUFFER_SIZE, "Buffer size is small. Data was truncate");
			break;
		}
		else
		{
			/* Invalid frame response */
			EXCTHROW( CAN_ERROR_INVALID_CONTINUE_RESPONSE, "Invalid data frame read");
		}
	} while (1);

Error:
	free(p2full_buffer);
	EXCRETHROW(pexception);
} /* canReadKWPPL3 */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canReadLowLevel"
/*!
 * \brief
 * This function reads data from CAN interface
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
static SElExceptionPtr canReadLowLevel(
	struct _SCAN*	me,
	uint32_t*		identifier,
	int32_t*		pDataLength,
	uint8_t*		pData
)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	NCTYPE_CAN_STRUCT	data_packet;
	NCTYPE_STATE		state;
	TIMEOUT_INIT();
	
	ncAction(me->_netObj, NC_OP_START, 0); //rt

	if (pDataLength == NULL ||
		(*pDataLength > 0 && pData == NULL))
	{
		EXCTHROW( CAN_ERROR_PARAMETERS, "Buffer size not defined or buffer is NULL");
	}

	do
	{
		error = ncWaitForState(me->_netObj, (NC_ST_READ_AVAIL | NC_ST_ERROR), me->_timeout, &state);
		EXCCHECKCAN( error);
		
		TIMEOUT_CHECK(me->_timeout);
		
		if (state & NC_ST_ERROR)
		{
			EXCTHROW( CAN_ERROR_COMMUNICATION, "CAN communication error");
		}
		EXCCHECKCAN( ncRead(me->_netObj, sizeof(data_packet), &data_packet));
		EXCCHECK( AppendCacheData(me, CAN_READ, data_packet.ArbitrationId, data_packet.Data, data_packet.DataLength, data_packet.Timestamp));
		if (data_packet.ArbitrationId == 0x00 && 
			(data_packet.FrameType == NC_FRMTYPE_LIN_BUS_ERR || data_packet.FrameType == NC_FRMTYPE_BUS_ERR)
			)
		{
			int32_t	size = 0;

			*identifier = data_packet.ArbitrationId;
			if (*pDataLength == 0 && pData == NULL)
			{
				*pDataLength = data_packet.DataLength;
				break;
			}
			size = (*pDataLength < data_packet.DataLength) ? *pDataLength : data_packet.DataLength;
			memcpy(pData, data_packet.Data, size);
			*pDataLength = data_packet.DataLength;
			EXCTHROW( CAN_ERROR_BUS_ERROR_FRAME, "Error Frame");
			break;
		}

		if (*identifier == READ_ANY || data_packet.ArbitrationId == *identifier)
		{
			int32_t	size = 0;

			*identifier = data_packet.ArbitrationId;

			if (*pDataLength == 0 && pData == NULL)
			{
				*pDataLength = data_packet.DataLength;
				break;
			}
			size = (*pDataLength < data_packet.DataLength) ? *pDataLength : data_packet.DataLength;
			memcpy(pData, data_packet.Data, size);
			*pDataLength = data_packet.DataLength;
			break;
		}
	} while (1);

Error:
	ncAction(me->_netObj, NC_OP_STOP, 0); //rt
	EXCRETHROW(pexception);
} /* canReadLowLevel */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canClearCache"
/*!
 * \brief
 * This function clears allocated cache.
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
static SElExceptionPtr canClearCache(struct _SCAN* me)
{
	SElExceptionPtr		pexception = NULL;

	if ( me->_pCache != NULL )
    {
        EXCCHECK( (me->_pCache)->Empty(me->_pCache));
    }

Error:
	EXCRETHROW(pexception);
} /* canClearCache */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canGetCache"
/*!
 * \brief
 * This function returns pointer to allocated string.
 * 
 * \param pCAN
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
static const char* canGetCache(struct _SCAN* me)
{
    static char*  pempty_cache = "";
    if ( me->_pCache == NULL )
    {
        return pempty_cache;
    }
    return  (me->_pCache)->GetPtr(me->_pCache);
} /* canGetCache */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canLock"
static SElExceptionPtr canLock(struct _SCAN* me)
{
	EnterCriticalSection( &(me->_Lock));

	return NULL;
} /* canLock */
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canUnlock"
static SElExceptionPtr canUnlock(struct _SCAN* me)
{
	LeaveCriticalSection( &(me->_Lock));
	return NULL;
} /* canUnlock */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canStartThread"
static SElExceptionPtr canStartThread(struct _SCAN* me)
{
#undef ATTR_SIZE
#define	ATTR_SIZE			5
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	SCANThreadItemPtr	pitem = me->_pItemsList;
	uint32_t			i = 0;
	NCTYPE_ATTRID		attr_id_list[ATTR_SIZE];
	NCTYPE_UINT32		attr_value_list[ATTR_SIZE];
	NCTYPE_CAN_DATA		send_data;
	
	EXCCHECKCAN( ncAction(me->_netObj, NC_OP_STOP, 0));
	for (i = 0; i < me->_ItemsCount; i++, pitem++)
	{
		int32_t	channel = 0;
		char	obj_name[128] = "";

		EXCCHECKCAN( ncGetAttribute(me->_netObj, NC_ATTR_INTERFACE_NUM, sizeof(int32_t), &channel));
		switch (pitem->wIdentType)
		{
		case CAN_ADDRESS_TYPE_STD:
			sprintf_s(obj_name, sizeof(obj_name), "CAN%d::STD%d", channel, pitem->wIdentifier);
			break;

		case CAN_ADDRESS_TYPE_XTD:
			sprintf_s(obj_name, sizeof(obj_name), "CAN%d::XTD%d", channel, pitem->wIdentifier);
			break;
		}
		me->_hThreadList = (NCTYPE_OBJH_P) realloc(me->_hThreadList, (me->_hThreadCount + 1) * sizeof(NCTYPE_OBJH));
		EXCCHECKALLOC( me->_hThreadList );

		/* Default settings */
		attr_id_list[0]		= NC_ATTR_COMM_TYPE;
		attr_value_list[0]	= NC_CAN_COMM_TX_PERIODIC;
		attr_id_list[1]		= NC_ATTR_PERIOD;
		attr_value_list[1]	= pitem->writeTime;
		attr_id_list[2]		= NC_ATTR_DATA_LEN;
		attr_value_list[2]	= pitem->length;
		attr_id_list[3]		= NC_ATTR_WRITE_Q_LEN;
		attr_value_list[3]	= 1;
		attr_id_list[4]		= NC_ATTR_READ_Q_LEN;
		attr_value_list[4]	= 0;

		EXCCHECKCAN( ncConfig(obj_name, ATTR_SIZE, attr_id_list, attr_value_list));

		EXCCHECKCAN( ncOpenObject(obj_name, &(me->_hThreadList[me->_hThreadCount])));
		memcpy(send_data.Data, pitem->data, pitem->length);
		EXCCHECKCAN( ncWrite(me->_hThreadList[me->_hThreadCount], sizeof(send_data), &send_data));
		me->_hThreadCount++;
	}
	
Error:
	ncAction(me->_netObj, NC_OP_START, 0);
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canStartThreadNonCPU"
static SElExceptionPtr canStartThreadNonCPU(struct _SCAN* me)
{
#undef ATTR_SIZE
#define	ATTR_SIZE			5
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	SCANThreadItemPtr	pitem = me->_pItemsList;
	uint32_t			i = 0;
	
	if (me->_hStopEvent == NULL)
	{
		me->_hStopEvent = CreateEvent(NULL, TRUE, FALSE, "CAN module Stop Event");
		if (me->_hStopEvent == NULL)
			EXCTHROW( EL_ERROR_OS_SPECIFIC, "CreateEvent fault!");
	}

	EXCCHECKWIN( ResetEvent(me->_hStopEvent));

	for (i = 0; i < me->_ItemsCount; i++, pitem++)
	{
		/* Find timed thread or create new */
 		if (!IsTimedThread(me, pitem->writeTime)) /* ToDo: PADLO */
		{
			/* Create new thread and add to list */
			SThreadParamPtr	param = calloc(1, sizeof(SThreadParam));

			param->me = me;
			param->writeTime = pitem->writeTime;

			me->_hHandleList = (SCANHandleItemPtr) realloc(me->_hHandleList, (me->_hThreadCount + 1) * sizeof(SCANHandleItem));
			EXCCHECKALLOC( me->_hHandleList);
			
			/* Create new thread */
			me->_hHandleList[me->_hThreadCount].writeTime = pitem->writeTime;
			me->_hHandleList[me->_hThreadCount].pHandle = CreateThread(NULL, 0, asyncThread, param, 0, NULL);
			EXCCHECKALLOC( me->_hHandleList[me->_hThreadCount].pHandle);

			me->_hThreadCount++;
		}
	}
	
Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canStopThread"
static SElExceptionPtr canStopThread(struct _SCAN* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	uint32_t		i = 0;
	NCTYPE_OBJH_P	pthread = me->_hThreadList;

	for (i = 0; i < me->_hThreadCount; i++, pthread++)
	{
		if (*pthread)
			EXCCHECKCAN( ncCloseObject(*pthread));
	}
	free(me->_hThreadList);
	me->_hThreadList = NULL;
	me->_hThreadCount = 0;

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canIsThreadRunning"
static bool_t canIsThreadRunning(struct _SCAN* me)
{
	return (me->_hThreadCount>0);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canStopThreadNonCPU"
static SElExceptionPtr canStopThreadNonCPU(struct _SCAN* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	uint32_t		i = 0;
	DWORD			result, exitCode;
	
	if (me->_hStopEvent)
		EXCCHECKWIN( SetEvent(me->_hStopEvent));

	for (i = 0; i < me->_hThreadCount; i++)
	{
		result = WaitForSingleObject( me->_hHandleList[i].pHandle, 2000);
		switch(result)
		{
			case WAIT_TIMEOUT:
				EXCTHROW( -1, "Thread Handle Status: WAIT_TIMEOUT");
				break;
			case WAIT_OBJECT_0:
				EXCCHECKWIN( GetExitCodeThread( me->_hHandleList[i].pHandle, &exitCode));
				switch(exitCode)
				{
					case STILL_ACTIVE:
						/* fault */
						break;
					default:
						/* success */
						break;
				}
				break;
		}

		if (me->_hHandleList[i].pHandle)
			EXCCHECKWIN( CloseHandle(me->_hHandleList[i].pHandle));
	}
	free(me->_hHandleList);
	me->_hHandleList = NULL;
	me->_hThreadCount = 0;

Error:
	EXCRETHROW(pexception);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canAddThreadItem"
static SElExceptionPtr canAddThreadItem(
	struct _SCAN*	me,
	uint32_t		wIdentifier,
	uint32_t		wIdentType,
	uint8_t			data[],
	uint32_t		dataLength,
	uint32_t		writeTime
)
{
	SElExceptionPtr	pexception = NULL;
	
	me->_pItemsList = (SCANThreadItemPtr)realloc(me->_pItemsList, (me->_ItemsCount + 1)*sizeof(SCANThreadItem));
	EXCCHECKALLOC( me->_pItemsList);

	me->_pItemsList[me->_ItemsCount].wIdentifier= wIdentifier;
	me->_pItemsList[me->_ItemsCount].wIdentType	= wIdentType;
	me->_pItemsList[me->_ItemsCount].writeTime	= writeTime;
	me->_pItemsList[me->_ItemsCount].length		= dataLength;
	memcpy(me->_pItemsList[me->_ItemsCount].data, data, dataLength);

	me->_ItemsCount++;

Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "canClearThreadItems"
static SElExceptionPtr canClearThreadItems(struct _SCAN* me)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				i = 0;
	
	free(me->_pItemsList);
	me->_pItemsList = NULL;
	me->_ItemsCount = 0;

//Error:
	EXCRETHROW(pexception);
}

/*****************************************************************************/
#undef  __FUNC__
#define __FUNC__ "canlist_new"
/*!
 * \brief
 * This function creates a new instance.
 * 
 * \param pError 
 * Returns a value indicating whether an error occurred.
 * A negative error code indicates function failure.	
 * 
 * \return 
 * Pointer to new instance of the CAN object. If the function fails, NULL
 * pointer is returned
 *
 * This function creates a new instance of the CAN object.
 */
SElExceptionPtr canlist_new(SCANListPtr* pCAN, const char* options)
{
	SElExceptionPtr	pexception = NULL;
	SCANListPtr		pcan_list = NULL;
	
	pcan_list = calloc( 1, sizeof(SCANList));
	EXCCHECKALLOC( pcan_list);

	pcan_list->AddChannel  = canlistAddChannel;
	pcan_list->RemoveChannel = canlistRemoveChannel;
	pcan_list->Open  = canlistOpen;
	pcan_list->Close = canlistClose;
	pcan_list->Write = canlistWrite;
	pcan_list->Read  = canlistRead;
	pcan_list->Query  = canlistQuery;
	pcan_list->QueryEx= canlistQueryEx;
	pcan_list->WriteLowLevel = canlistWriteLowLevel;
	pcan_list->ReadLowLevel  = canlistReadLowLevel;
	pcan_list->SetTimeout  = canlistSetTimeout;
	pcan_list->GetTimeout  = canlistGetTimeout;
	pcan_list->StartThread = canlistStartThread;
	pcan_list->StopThread = canlistStopThread;
	pcan_list->IsThreadRunning = canlistIsThreadRunning;
	pcan_list->ClearThreadItems = canlistClearThreadItems;
	pcan_list->AddThreadItem = canlistAddThreadItem;
	pcan_list->ClearCache = canlistClearCache;
	pcan_list->GetCache = canlistGetCache;
	pcan_list->RegisterLogFunction = canlistRegisterLogFunction;
	pcan_list->SetAttribute = canlistSetAttribute;
	pcan_list->GetAttribute = canlistGetAttribute;

	pcan_list->_logFunction = NULL;
	EXCCHECK( objlist_new(&(pcan_list->_channelList)));
	EXCCHECK( (pcan_list->_channelList)->RegisterDestroy(pcan_list->_channelList, can_delete));

	EXCCHECK( elstring_new(NULL, &(pcan_list->_pCache)));
	EXCCHECK( timer_new( &(pcan_list->_pTimer)));

Error:
	if ( pCAN != NULL )
	{
		*pCAN = pcan_list;
	}
	
	EXCRETHROW(pexception);
}/* canlist_new */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "canlist_delete"
/*!
 * \brief
 * This function destroys specified instance.
 * 
 * \param pCAN 
 * Pointer to instance of the CAN object cretaed by can_new() function.
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
SElExceptionPtr canlist_delete(SCANListPtr* pCAN)
{
	SElExceptionPtr	pexception = NULL;

	if ( pCAN && *pCAN )
	{
		EXCCHECK( objlist_delete( &((*pCAN)->_channelList)));
		EXCCHECK( elstring_delete( &((*pCAN)->_pCache)));
		EXCCHECK( timer_delete( &((*pCAN)->_pTimer)));

		free( *pCAN);
		*pCAN = NULL;
	}

Error:
	EXCRETHROW(pexception);
} /* canlist_delete */


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#undef __FUNC__
#define __FUNC__ "AppendCacheData"
static SElExceptionPtr AppendCacheData(
    struct _SCAN*   me,
    int32_t         dType,
    int32_t         identifier,
    uint8_t*        pData,
    uint32_t        dSize,
	NCTYPE_ABS_TIME	timestamp
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0,
					channel = 0;
	uint32_t		i = 0;
	char			channel_string[128] = "";

	canLock(me);
	
	EXCCHECKCAN( ncGetAttribute(me->_netObj, NC_ATTR_INTERFACE_NUM, sizeof(int32_t), &channel));

	if (me->_logFunction)
	{
		sprintf_s(channel_string, sizeof(channel_string), "%s%d", me->_prefix, channel);
		EXCCHECK( me->_logFunction(channel_string, dType, identifier, pData, dSize, timestamp));
	}

	if ( me->_CacheEnabled != TRUE )
	{
		goto Error;
	}

	{
		const char* prefix = (dType == CAN_WRITE ) ? "[%0.3Lf ms] W " : "[%0.3Lf ms] R "; 
		EXCCHECK( (me->_pCache)->Format(me->_pCache, prefix,
										(me->_pTimer)->GetTimeDiff( me->_pTimer)
										));
	}

	(me->_pCache)->Format(me->_pCache, "%s%d::%04x", me->_prefix, channel, identifier);
	(me->_pCache)->Append(me->_pCache, ": ");
	
	for (i = 0; i < dSize; i++)
	{
		(me->_pCache)->Format(me->_pCache, "%02x ", pData[i]);
	}
#if 0
	if (dType == CAN_READ)
	{
		(me->_pCache)->Format(me->_pCache, " timestamp = %d", timestamp);
	}
#endif
	(me->_pCache)->Append(me->_pCache, "\n");

Error:
	canUnlock(me);
	EXCRETHROW(pexception);
} /* AppendCacheData */

bool_t IsTimedThread(SCANPtr me, uint32_t period)
{
	bool_t		ret = 0;
	uint32_t	i = 0;

	for (i = 0; i < me->_hThreadCount; i++)
	{
		if (me->_hHandleList[i].writeTime == period)
		{
			ret = 1;
			break;
		}
	}

	return ret;
}

DWORD WINAPI asyncThread(
  LPVOID lpParameter
)
{
	SElExceptionPtr	pexception = NULL;
	SThreadParamPtr	param = (SThreadParamPtr) lpParameter;
	uint32_t		time = param->writeTime;
	SCANPtr			me = param->me;
	uint32_t		i = 0;

	while (1)
	{
		if (WaitForSingleObject(me->_hStopEvent, time) == WAIT_OBJECT_0)
			break;

		/* Write all packet with same time */

		for (i = 0; i < me->_ItemsCount; i++)
		{
			if (me->_pItemsList[i].writeTime == time)
			{
				pexception = me->WriteLowLevel(me, me->_pItemsList[i].wIdentifier, NC_FRMTYPE_DATA, me->_pItemsList[i].length, me->_pItemsList[i].data);
				EXCDBGSTR(pexception);
			}
		}
	}

	free(param);
	ExitThread(0);
}

#undef __FUNC__
#define __FUNC__ "waitForFlowControl"
static SElExceptionPtr waitForFlowControl(
	SCANPtr		me,
	uint32_t	address,
	uint8_t*	blockSize,
	uint8_t*	waitTime,
	uint32_t	timeout
)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		exp_address = me->_testerAddress;
	int32_t			data_length = 0;
	uint8_t			read_data[8];
	TIMEOUT_INIT();
	
	do
	{
		data_length = sizeof(read_data);
		EXCCHECK( canReadLowLevel(me, &exp_address, &data_length, read_data));
		
		if (read_data[1] == 0x7F)
		{
			char	message[256] = "";
			
			if (read_data[3] != 0x78)
			{
				sprintf_s(message, sizeof(message), "Negative Response (Service: %02x) Error Code: %02x",
						read_data[2], read_data[3]);
				EXCTHROW(CAN_ERROR_NEGATIVE_RESPONSE, message);
			}
			else
			{
				Sleep(50);
				continue;
			}
		}
		
		if ((read_data[0] & 0xF0) != 0x30)
		{
			EXCTHROW(CAN_ERROR_INVALID_CONTINUE_RESPONSE, "Invalid Response");
		}
		if ((read_data[0] & 0x0F) == 0)
			break;
			
		TIMEOUT_CHECK(timeout);
	} while (1);
	
	if (blockSize)
	{
		*blockSize = (read_data[1]>0)? read_data[1]:8;
	}

	if (waitTime)
		*waitTime = read_data[2];
	
Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistAddChannel"
static SElExceptionPtr canlistAddChannel(
	struct _SCANList*	me,
	const char*			channel,
	const char*			options
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;

	if (me->_channelList->Get(me->_channelList, channel) != NULL)
	{
		char	message[256] = "";

		sprintf_s(message, sizeof(message), "Channel %s Allready Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_ALLREADY_EXISTS, message);
	}

	EXCCHECK( can_new(&pcan, options));
	EXCCHECK( me->_channelList->Add(me->_channelList, channel, pcan));
	pcan->_pCache = me->_pCache;
	pcan->_pTimer = me->_pTimer;
	pcan->_logFunction = me->_logFunction;

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistRemoveChannel"
static SElExceptionPtr canlistRemoveChannel(
	struct _SCANList*	me,
	const char*			channel
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;

	if (channel == NULL || strlen(channel) == 0)
	{
		EXCCHECK( me->_channelList->Clear(me->_channelList));
	}
	else
	{
		if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
		{
			char	message[256] = "";

			sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
		}

		EXCCHECK( can_delete(&pcan));
	}

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistOpen"
static SElExceptionPtr canlistOpen(
	struct _SCANList*	me,
	const char*			channel,
	const char*			options
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	char			message[256] = "";

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	if (pcan->_netObj != 0)
	{
		sprintf_s(message, sizeof(message), "Channel %s Allready Open", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_ALLREADY_OPENED, message);
	}

	EXCCHECK( pcan->Open(pcan, channel, options));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistClose"
static SElExceptionPtr canlistClose(
	struct _SCANList*	me,
	const char*			channel
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	int32_t			channel_count = 0,
					i = 0;
	const char*		p2name = NULL;
	char			message[256] = "";

	if (channel == NULL || strlen(channel) == 0)
	{
		channel_count = me->_channelList->GetCount(me->_channelList);

		for (i = 0; i < channel_count; i++)
		{
			p2name = me->_channelList->GetNameOnIndex(me->_channelList, i);
			pcan = me->_channelList->Get(me->_channelList, p2name);

			EXCCHECK( pcan->Close(pcan));
		}
	}
	else
	{
		if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
		{
			sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
		}

		if (pcan->_netObj == 0)
		{
			sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
		}

		EXCCHECK( pcan->Close(pcan));
	}

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistWrite"
static SElExceptionPtr canlistWrite(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			address,
	uint32_t			dataLength,
	uint8_t*			pData
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	char			message[256] = "";

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	if (pcan->_netObj == 0)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
	}

	EXCCHECK( pcan->Write(pcan, address, dataLength, pData));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistRead"
static SElExceptionPtr canlistRead(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t*			identifier,
	int32_t*			pDataLength,
	uint8_t*			pData
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	char			message[256] = "";

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	if (pcan->_netObj == 0)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
	}

	EXCCHECK( pcan->Read(pcan, identifier, pDataLength, pData));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistWriteLowLevel"
static SElExceptionPtr canlistWriteLowLevel(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			address,
	uint32_t			frame_type,
	uint32_t			dataLength,
	uint8_t*			pData
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	char			message[256] = "";

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	if (pcan->_netObj == 0)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
	}

	EXCCHECK( pcan->WriteLowLevel(pcan, address, frame_type, dataLength, pData));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistReadLowLevel"
static SElExceptionPtr canlistReadLowLevel(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t*			identifier,
	int32_t*			pDataLength,
	uint8_t*			pData
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	char			message[256] = "";

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	if (pcan->_netObj == 0)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
	}

	EXCCHECK( pcan->ReadLowLevel(pcan, identifier, pDataLength, pData));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistQuery"
static SElExceptionPtr canlistQuery(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			wIdent,
	uint8_t*			wData,
	uint32_t			wDataDize,
	uint32_t			rIdent,
	int32_t*			prDataSize,
	uint8_t*			rData
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	char			message[256] = "";

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	if (pcan->_netObj == 0)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
	}

	EXCCHECK( pcan->Query(pcan, wIdent, wData, wDataDize, rIdent, prDataSize, rData));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistQueryEx"
static SElExceptionPtr canlistQueryEx(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			wIdent,
	uint8_t*			wData,
	uint32_t			wDataDize,
	uint32_t			rIdent,
	int32_t*			prDataSize,
	uint8_t*			rData
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	char			message[256] = "";

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	if (pcan->_netObj == 0)
	{
		sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
	}

	EXCCHECK( pcan->QueryEx(pcan, wIdent, wData, wDataDize, rIdent, prDataSize, rData));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistClearCache"
static SElExceptionPtr canlistClearCache(struct _SCANList* me)
{
	SElExceptionPtr	pexception = NULL;

	if ( me->_pCache != NULL )
    {
        EXCCHECK( (me->_pCache)->Empty(me->_pCache));
    }

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistGetCache"
static const char* canlistGetCache(struct _SCANList* me)
{
	static char*  pempty_cache = "";
    if ( me->_pCache == NULL )
    {
        return pempty_cache;
    }
    return  (me->_pCache)->GetPtr(me->_pCache);
}

#undef __FUNC__
#define __FUNC__ "canlistStartThread"
static SElExceptionPtr canlistStartThread(
	struct _SCANList*	me,
	const char*			channel
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	int32_t			channel_count = 0,
					i = 0;
	const char*		p2name = NULL;
	char			message[256] = "";

	if (channel == NULL || strlen(channel) == 0)
	{
		channel_count = me->_channelList->GetCount(me->_channelList);

		for (i = 0; i < channel_count; i++)
		{
			p2name = me->_channelList->GetNameOnIndex(me->_channelList, i);
			pcan = me->_channelList->Get(me->_channelList, p2name);

			EXCCHECK( pcan->StartThread(pcan));
		}
	}
	else
	{
		if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
		{
			sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
		}

		if (pcan->_netObj == 0)
		{
			sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
		}

		EXCCHECK( pcan->StartThread(pcan));
	}

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistStopThread"
static SElExceptionPtr canlistStopThread(
	struct _SCANList*	me,
	const char*			channel
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	int32_t			channel_count = 0,
					i = 0;
	const char*		p2name = NULL;
	char			message[256] = "";

	if (channel == NULL || strlen(channel) == 0)
	{
		channel_count = me->_channelList->GetCount(me->_channelList);

		for (i = 0; i < channel_count; i++)
		{
			p2name = me->_channelList->GetNameOnIndex(me->_channelList, i);
			pcan = me->_channelList->Get(me->_channelList, p2name);

			EXCCHECK( pcan->StopThread(pcan));
		}
	}
	else
	{
		if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
		{
			sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
		}

		if (pcan->_netObj == 0)
		{
			sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
		}

		EXCCHECK( pcan->StopThread(pcan));
	}

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistIsThreadRunning"
static bool_t canlistIsThreadRunning(
	struct _SCANList*	me,
	const char*			channel
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) != NULL)
	{
		return pcan->IsThreadRunning(pcan);
	}

	return FALSE;
}

#undef __FUNC__
#define __FUNC__ "canlistExitThread"
static SElExceptionPtr canlistExitThread(
	struct _SCANList*	me,
	const char*			channel
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	int32_t			channel_count = 0,
					i = 0;
	const char*		p2name = NULL;
	char			message[256] = "";

	if (channel == NULL || strlen(channel) == 0)
	{
		channel_count = me->_channelList->GetCount(me->_channelList);

		for (i = 0; i < channel_count; i++)
		{
			p2name = me->_channelList->GetNameOnIndex(me->_channelList, i);
			pcan = me->_channelList->Get(me->_channelList, p2name);

			EXCCHECK( pcan->ExitThread(pcan));
		}
	}
	else
	{
		if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
		{
			sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
		}

		if (pcan->_netObj == 0)
		{
			sprintf_s(message, sizeof(message), "Channel %s Not Open", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_OPENED, message);
		}

		EXCCHECK( pcan->ExitThread(pcan));
	}

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistAddThreadItem"
static SElExceptionPtr canlistAddThreadItem(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			wIdentifier,
	uint32_t			wIdentType,
	uint8_t				data[],
	uint32_t			dataLength,
	uint32_t			writeTime
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		char	message[256] = "";

		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	EXCCHECK( pcan->AddThreadItem(pcan, wIdentifier, wIdentType, data, dataLength, writeTime));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistClearThreadItems"
static SElExceptionPtr canlistClearThreadItems(
	struct _SCANList*	me,
	const char*			channel
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	int32_t			channel_count = 0,
					i = 0;
	const char*		p2name = NULL;

	if (channel == NULL || strlen(channel) == 0)
	{
		channel_count = me->_channelList->GetCount(me->_channelList);

		for (i = 0; i < channel_count; i++)
		{
			p2name = me->_channelList->GetNameOnIndex(me->_channelList, i);
			pcan = me->_channelList->Get(me->_channelList, p2name);

			EXCCHECK( pcan->ClearThreadItems(pcan));
		}
	}
	else
	{
		if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
		{
			char	message[256] = "";

			sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
		}

		EXCCHECK( pcan->ClearThreadItems(pcan));
	}

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistSetTimeout"
static SElExceptionPtr	canlistSetTimeout(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			ReadTimeout
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		char	message[256] = "";

		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	EXCCHECK( pcan->SetTimeout(pcan, ReadTimeout));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistGetTimeout"
static SElExceptionPtr	canlistGetTimeout(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t*			ReadTimeout
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;

	if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
	{
		char	message[256] = "";

		sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
		EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
	}

	EXCCHECK( pcan->GetTimeout(pcan, ReadTimeout));

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistRegisterLogFunction"
static SElExceptionPtr	canlistRegisterLogFunction(
	struct _SCANList*	me,
	const char*			channel,
	PLogFuncPtr			LogFunc,
	PLogFuncPtr*		OldLogFunc
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			channel_count = 0,
					i;
	const char*		p2name = NULL;
	SCANPtr			pcan = NULL;

	if (OldLogFunc != NULL)
		*OldLogFunc = me->_logFunction;

	channel_count = me->_channelList->GetCount(me->_channelList);

	for (i = 0; i < channel_count; i++)
	{
		p2name = me->_channelList->GetNameOnIndex(me->_channelList, i);
		pcan = me->_channelList->Get(me->_channelList, p2name);

		pcan->_logFunction = LogFunc;
	}

	me->_logFunction = LogFunc;

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistSetAttribute"
static SElExceptionPtr canlistSetAttribute(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			dataType,
	uint32_t			attrId,
	void*				value
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
	int32_t			channel_count = 0,
					i = 0;
	const char*		p2name = NULL;
	
	if (channel == NULL || strlen(channel) == 0)
	{
		/* set global attributes */
		switch (attrId)
		{
		case CAN_ATTR_CACHE_ENABLED:
			if (dataType != DATA_TYPE_INT32)
				EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_CACHE_ENABLED");

			me->_CacheEnabled = *((int32_t*) value);
			
			channel_count = me->_channelList->GetCount(me->_channelList);

			for (i = 0; i < channel_count; i++)
			{
				p2name = me->_channelList->GetNameOnIndex(me->_channelList, i);
				pcan = me->_channelList->Get(me->_channelList, p2name);

				EXCCHECK( pcan->SetAttribute(pcan, dataType, attrId, value));
			}
			break;

		default:
			EXCTHROW(-1, "Unknown Attribute");
			break;
		}
	}
	else
	{
		if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
		{
			char	message[256] = "";

			sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
		}

		EXCCHECK( pcan->SetAttribute(pcan, dataType, attrId, value));
	}

Error:
	EXCRETHROW(pexception);
}

#undef __FUNC__
#define __FUNC__ "canlistGetAttribute"
static SElExceptionPtr canlistGetAttribute(
	struct _SCANList*	me,
	const char*			channel,
	uint32_t			dataType,
	uint32_t			attrId,
	void*				value,
	int32_t				valueSize
)
{
	SElExceptionPtr	pexception = NULL;
	SCANPtr			pcan = NULL;
		
	if (channel == NULL || strlen(channel) == 0)
	{
		/* get global attributes */
		switch (attrId)
		{
		case CAN_ATTR_CACHE_ENABLED:
			if (dataType != DATA_TYPE_INT32)
				EXCTHROW(-1, "Invalid Data Type for CAN_ATTR_CACHE_ENABLED");

			*((int32_t*) value) = me->_CacheEnabled;
			break;

		default:
			EXCTHROW(-1, "Unknown Attribute");
			break;
		}
	}
	else
	{
		if ((pcan = me->_channelList->Get(me->_channelList, channel)) == NULL)
		{
			char	message[256] = "";

			sprintf_s(message, sizeof(message), "Channel %s Not Exists", channel);
			EXCTHROW( CAN_ERROR_CHANNEL_NOT_EXISTS, message);
		}

		EXCCHECK( pcan->GetAttribute(pcan, dataType, attrId, value, valueSize));
	}

Error:
	EXCRETHROW(pexception);
}