/****************************************************************************
 *                             I V I B R I D G E
 *---------------------------------------------------------------------------
 *    Copyright   2005-2006 ELCOM, a.s.  All Rights Reserved.        
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:       ivibridge.h                                                    
 * Purpose:     IVI bridge interface
 * Version:     1.0 
 ****************************************************************************/

#if !defined(__IVIBRIDGE_H__)
#define __IVIBRIDGE_H__

#include <eltypes.h>
#include <elstring.h>
#include <exception/Exception.h>
#include <ivi.h>


#if defined(__cplusplus) || defined(__cplusplus__)
	extern "C" {
#endif

/*!
 * Specifies IviBridge Interface
 */ 
typedef struct _SIviBridge
{
	SElExceptionPtr (*InitializeConfiguration)(struct _SIviBridge* me, const char* pConfigStore);
    SElExceptionPtr (*GetSession)(struct _SIviBridge* me, const char* name, ViSession* vi);
	SElExceptionPtr (*Close)(struct _SIviBridge* me);
/*******************************************
 * PRIVATE
 *******************************************/
	int32_t		_alloc_count;
	int32_t		_used;
	void*		_pStorage;
} SIviBridge, *SIviBridgePtr;

SElExceptionPtr	ivibridge_new(SIviBridgePtr* pIviBridge);
SElExceptionPtr	ivibridge_delete(SIviBridgePtr* pIviBridge);


#define IVIBRIDGE_ERROR_CONFIGURATION_MISSING			-121245
#define IVIBRIDGE_ERORR_UNKNOWN_CLASS_DRIVER			-121246
#define IVIBRIDGE_ERORR_INIT							-121247
#define IVIBRIDGE_ERROR_INVALID_SESSION_NAME			-121248


#if defined(__cplusplus) || defined(__cplusplus__)
	}
#endif

#endif /* __IVIBRIDGE_H__*/ 
