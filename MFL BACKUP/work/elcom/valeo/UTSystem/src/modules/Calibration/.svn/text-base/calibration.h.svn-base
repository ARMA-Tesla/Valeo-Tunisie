/****************************************************************************
 *                       C A L I B R A T I O N
 *---------------------------------------------------------------------------
 *    Copyright   2005-2007 ELCOM, a.s.  All Rights Reserved.        
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:       calibration.h                                                    
 * Purpose:     Calibration module
 * Version:     1.0 
 ****************************************************************************/

#if !defined(__CALIBRATION_H__)
#define __CALIBRATION_H__

#include <eltypes.h>
#include <elstring.h>
#include <exception/Exception.h>


#if defined(__cplusplus) || defined(__cplusplus__)
	extern "C" {
#endif

typedef enum 
{
	E_SCALE_TYPE_LINEAR = 0,
	E_SCALE_TYPE_UNKNOWN = 0xFFFFFFFF
} EScaleType;

/*!
 * Specifies Calibration Channel Scale
 */
typedef struct _SScale
{
	char		scaleName[256];
	EScaleType	scaleType;
	union {
		struct {
			double	x1,y1,x2,y2;
		} linear;
	};
} SScale, *SScalePtr;

/*!
 * Specifies Calibration Channel
 */
typedef struct _SChannel
{
	char		channelName[256];
	SScalePtr	pScaleList;
	int32_t		scaleListCount;
} SChannel, *SChannelPtr;

/*!
 * Specifies Calibration Interface
 */
typedef struct _SCalibration
{
	SElExceptionPtr (*Calculate)(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scale,
		double					value,
		double*					value2
	);
	SElExceptionPtr (*Load)(
		struct _SCalibration*	me,
		const char*				calibSource
	);
	SElExceptionPtr (*Save)(
		struct _SCalibration*	me,
		const char*				calibSource
	);
	
	SElExceptionPtr (*RemoveScale)(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName
	);
	SElExceptionPtr (*AddLinearScale)(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName,
		double					x1,
		double					y1,
		double					x2,
		double					y2
	);
	SElExceptionPtr (*AddLinearScaleEx)(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName,
		double					gain,
		double					offset
	);
	SElExceptionPtr (*GetLinearScale)(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName,
		double*					x1,
		double*					y1,
		double*					x2,
		double*					y2
	);
	SElExceptionPtr (*GetLinearScaleEx)(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName,
		double*					gain,
		double*					offset
	);
	
/*	SElExceptionPtr (*GetChannelScales)(
		struct _SCalibration*	me,
		const char*				channel,
		int32_t*				scalesCount,
		char**					scales
	);
*/	
	SElExceptionPtr (*NewList)(struct _SCalibration* me);

/*******************************************
 * PRIVATE
 *******************************************/
	SChannelPtr	pChannelList;
	int32_t		channelListCount;
} SCalibration, *SCalibrationPtr;

SElExceptionPtr	calibration_new(SCalibrationPtr* pCalibration);
SElExceptionPtr	calibration_delete(SCalibrationPtr* pCalibration);

#define CALIB_ERROR_OFFSET					(-1000L)
#define CALIB_ERROR_UNKNOWN_SOURCE			(CALIB_ERROR_OFFSET - 1L)
#define CALIB_ERROR_UNKNOWN_TYPE			(CALIB_ERROR_OFFSET - 2L)
#define CALIB_ERROR_MISSING_SCALE_VALUES	(CALIB_ERROR_OFFSET - 3L)
#define CALIB_ERROR_UNKNOWN_CHANNEL_NAME	(CALIB_ERROR_OFFSET - 4L)
#define CALIB_ERROR_UNKNOWN_SCALE_NAME		(CALIB_ERROR_OFFSET - 5L)

#if defined(__cplusplus) || defined(__cplusplus__)
	}
#endif

#endif /* __CALIBRATION_H__*/ 
