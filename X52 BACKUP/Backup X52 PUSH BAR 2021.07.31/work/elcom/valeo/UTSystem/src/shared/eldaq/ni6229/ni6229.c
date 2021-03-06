/*****************************************************************************
 *  DAQ Measurement Card Instrument Driver                               
 *  LabWindows/CVI Instrument Driver                                     
 *  Original Release: 25. dubna 2007                                  
 *  By: Zdenek Rykala, ELCOM, a.s.                              
 *      PH.    Fax                               
 *                                                                           
 *  Modification History:                                                    
 *                                                                           
 *       25. dubna 2007 - Instrument Driver Created.                  
 *                                                                           
 *****************************************************************************/
#include <msxmldom.h>
#include <utility.h>
#include <ansi_c.h>
#include <analysis.h>
#include <string.h>
#include <stdio.h>  
#include <formatio.h>
#include "ni6229.h"
#include "NIDAQmx.h"
#include <userint.h>

static int CVICALLBACK ThreadDAQ (void *functionData);
int threadDAQId = 0;
int Run_DAQ;
float64 data[32];


#define viCheckDAQ( fCal) if ( derror = (fCal), derror < 0 ) { checkErr( Ivi_SetAttributeViInt32(vi, VI_NULL, NI6229_ATTR_INSTR_DAQ_ERROR, 0, derror)); viCheckErr( IVI_ERROR_INSTR_SPECIFIC); } else derror = derror

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define NI6229_ATTR_OPC_TIMEOUT      				(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)       /* ViInt32 */
#define NI6229_ATTR_USE_SPECIFIC_SIMULATION 			(IVI_INHERENT_ATTR_BASE + 23L)  /* ViBoolean */
#define NI6229_ATTR_VISA_RM_SESSION  				(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 5L)   /* ViSession */
#define NI6229_ATTR_OPC_CALLBACK     				(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 6L)   /* ViAddr */
#define NI6229_ATTR_CHECK_STATUS_CALLBACK    		(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 7L)   /* ViAddr */
#define NI6229_ATTR_USER_INTERCHANGE_CHECK_CALLBACK  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 8L)   /* ViAddr */


#define NI6229_ATTR_INSTR_CARD_NAME					(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 10L)
#define NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING			(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 11L)
#define NI6229_ATTR_INST_DAQ_SESSIONS			(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 12L)
#define NI6229_ATTR_INSTR_CALIBRATION_FILE		(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 13L)
#define NI6229_ATTR_INSTR_DAQ_ERROR				(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 14L)


typedef struct _SCalibrationPoint
{
	double	measValue;
	double	realValue;
} SCalibrationPoint, *SCalibrationPointPtr;
	
typedef struct _SLinearCalibration
{
	int						count;
	SCalibrationPointPtr	pPoints;	
} SLinearCalibration, *SLinearCalibrationPtr;

typedef struct _SChannelInfo
{
	ViChar		drivername[32];		
	ViChar		daqname[32];
} SChannelInfo, *SChannelInfoPtr;
typedef struct _SChannelData
{
	ViReal64*	pData;
	ViReal64*	pChannelData;
	ViInt32		UsedForChannel;
	ViInt32		AllocatedSamples;
	ViInt32		AllocatedChannels;
	uInt64		Samples;
} SChannelData, *SChannelDataPtr;



typedef struct _SCardSessions
{
	ViChar			CardName[32];
	
	TaskHandle		ai;
	SChannelInfo	AI_channels[32];
	SChannelData	AI_data;

	TaskHandle		DO;
	TaskHandle		ao;
} SCardSessions, *SCardSessionsPtr;

enum {
	CAL_TYPE_LINEAR = 1
};
	
	

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);
int CompareChannelInfo( const void* info1, const void* info2);

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L        

    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_ERROR_MASK              0x3C


/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus ni6229_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
static ViStatus ni6229_IviClose (ViSession vi);   

static ViStatus ni6229_InitAttributes (ViSession vi, ViConstString cardName);
static ViStatus ni6229_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus ni6229_CheckStatus (ViSession vi);
static ViStatus ni6229_WaitForOPC (ViSession vi, ViInt32 maxTime);

static ViStatus ni6229_GetChannelScale2(
	ViSession 		vi,
	ViConstString	channelName,
	ViConstString	scaleName,
	SCalibrationPtr*	pScaleInfo
);

static ViStatus ni6229_DiscardScaleInfo(
	ViSession				vi,
	SCalibrationPtr*		pScaleInfo
);
static ViStatus ni6229_ParseLinearScale(
	ViSession				vi,
	MSXMLObj_IXMLDOMNode	node,
	SCalibrationPtr*		pScaleInfo
);


/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/
    
static ViStatus _VI_FUNC ni6229_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC ni6229_WaitForOPCCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC eldaqAttrDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC eldaqAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC eldaqAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC eldaqAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue);
static ViStatus _VI_FUNC eldaqAttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);

/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: ni6229_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           ni6229_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
    ViStatus    error = VI_SUCCESS;

    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    checkErr( ni6229_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));

Error:  
    return error;
}

/*****************************************************************************
 * Function: ni6229_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_InitWithOptions(
    ViRsrc          resourceName,
    ViBoolean       IDQuery,
    ViBoolean       resetDevice,
    ViConstString   optionString,
    ViSession      *newVi
)
{   
    ViStatus    error = VI_SUCCESS;
    ViSession   vi = VI_NULL;
    ViChar      newResourceName[IVI_MAX_MESSAGE_BUF_SIZE];
    ViChar      newOptionString[IVI_MAX_MESSAGE_BUF_SIZE];
    ViBoolean   isLogicalName;
    
    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER5, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    *newVi = VI_NULL;
    
    checkErr( Ivi_GetInfoFromResourceName( resourceName, (ViString)optionString,
                                           newResourceName,
                                           newOptionString,
                                           &isLogicalName));
    
        /* create a new interchangeable driver */
    checkErr( Ivi_SpecificDriverNew ("eldaq", newOptionString, &vi));  
    if (!isLogicalName)
    {
        ViInt32 oldFlag = 0;
        
        checkErr (Ivi_GetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag));
        checkErr (Ivi_SetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010));
        checkErr (Ivi_SetAttributeViString (vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName));
    }
        /* init the driver */
    checkErr( ni6229_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    if (isLogicalName)
        checkErr( Ivi_ApplyDefaultSetup (vi));
    *newVi = vi;
    
Error:
    if (error < VI_SUCCESS) 
        Ivi_Dispose (vi);
        
    return error;
}



/*****************************************************************************
 * Function: ni6229_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_close (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    checkErr( ni6229_IviClose (vi));

Error:    
    Ivi_UnlockSession (vi, VI_NULL);
    Ivi_Dispose (vi);  

    return error;
}

/*****************************************************************************
 * Function: ni6229_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_reset (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	checkErr( ni6229_DefaultInstrSetup (vi));                                
	
	checkErr( ni6229_CheckStatus (vi));                                      

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ni6229_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( ni6229_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/**************************************************************************** 
 *  Function: ni6229_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC ni6229_Disable (ViSession vi)
{
    return VI_SUCCESS;
}

/*****************************************************************************
 * Function: ni6229_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_self_test (ViSession vi, ViInt16 *testResult, 
                                      ViChar testMessage[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (testResult == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Test Result");
    if (testMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Test Message");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */
	
	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

	
	    }
	else
	    {
	        /* Simulate Self Test */
	    *testResult = 0;
	    strcpy (testMessage, "No error.");
	    }
	
	checkErr( ni6229_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ni6229_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (driverRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
    if (instrRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, NI6229_ATTR_SPECIFIC_DRIVER_REVISION, 
                                        0, 256, driverRev));
    checkErr( Ivi_GetAttributeViString (vi, "", NI6229_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                        0, 256, instrRev));
    checkErr( ni6229_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ni6229_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_error_query (ViSession vi, ViInt32 *errCode, 
                                        ViChar errMessage[])
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    if (errCode == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error Code");
    if (errMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */
	
	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	    checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL,
                                          NI6229_ATTR_INSTR_DAQ_ERROR, 0, errCode));
        DAQmxGetErrorString (*errCode, errMessage, 256);
	
	    }
	else
	    {
	        /* Simulate Error Query */
	    *errCode = 0;
	    strcpy (errMessage, "No error.");
	    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ni6229_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
    ViStatus    error = VI_SUCCESS;
    
    static      IviStringValueTable errorTable = 
        {

            {NI6229_ERROR_MEASUREMENT_IS_RUNNING,   "Measurement isn't running. Call prefix_StartMeasurement()."},   
			{NI6229_ERROR_MEASUREMENT_IS_NOT_CONFIGURED,   "Measurement isn't running. Call prefix_StartMeasurement()."},   
			{NI6229_ERROR_LINEAR_CALIBRATION_CONFIG,   "Wrong daa for linear calibration."},   
			{NI6229_ERROR_NOT_SPECIFIED_CALIBRATION_TYPE,   "Calibration is missing or more calibration scales exist."},   
			{NI6229_ERROR_NOT_SUPPORTED_CALIBRATION_TYPE,   "Specified calibration type is not supported."},   
            {VI_NULL,                               VI_NULL}
        };
        
    if (vi)
        Ivi_LockSession(vi, VI_NULL);

        /* all VISA and IVI error codes are handled as well as codes in the table */
    if (errorMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

    checkErr( Ivi_GetSpecificDriverStatusDesc(vi, errorCode, errorMessage, errorTable));

Error:
    if (vi)
        Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: ni6229_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_InvalidateAllAttributes (ViSession vi)
{
    return Ivi_InvalidateAllAttributes(vi);
}

ViStatus _VI_FUNC ni6229_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                         ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
    return Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);                                
}                                                                                                           
ViStatus _VI_FUNC ni6229_ClearErrorInfo (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: ni6229_GetError and ni6229_ClearError Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_GetError (ViSession vi, 
                                     ViStatus *errorCode, 
                                     ViInt32 bufferSize,
                                     ViChar description[])  
{
    ViStatus error = VI_SUCCESS;
    ViStatus primary = VI_SUCCESS,
             secondary = VI_SUCCESS;
    ViChar   elaboration[256] = "";
    ViChar   errorMessage[1024] = "";
    ViChar  *appendPoint = errorMessage;
    ViInt32  actualSize = 0;
    ViBoolean locked = VI_FALSE;

    /* Lock Session */
    if (vi != 0)
        {
        checkErr( Ivi_LockSession(vi, &locked));
        }

    /* Test for nulls and acquire error data */
    if (bufferSize != 0)
        {
        if (errorCode == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error");
            }
        if (description == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Description");
            }
        checkErr( Ivi_GetErrorInfo (vi, &primary, &secondary, elaboration));
        }

    else
        {
        checkErr( Ivi_GetAttributeViString(vi, VI_NULL, IVI_ATTR_ERROR_ELABORATION, 0, 256, elaboration));
        checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_SECONDARY_ERROR, 0, &secondary));
        checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_PRIMARY_ERROR, 0, &primary));
        }
        
    /* Format data */
    if (primary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( ni6229_error_message (vi, primary, msg));
        appendPoint += sprintf(appendPoint, "Primary Error: (Hex 0x%08X) %s\n", primary, msg);
        }
    
    if (secondary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( ni6229_error_message (vi, secondary, msg));
        appendPoint += sprintf(appendPoint, "Secondary Error: (Hex 0x%08X) %s\n", secondary, msg);
        }
    
    if (elaboration[0])
        {
        sprintf(appendPoint, "Elaboration: %s", elaboration);
        }
    
    actualSize = strlen(errorMessage) + 1;
    
    /* Prepare return values */
    if (bufferSize == 0)
        {
        error = actualSize;
        }
    else 
        {
        if (bufferSize > 0)
            {
            if (actualSize > bufferSize)
                {
                error = actualSize;
                actualSize = bufferSize;
                }
            }
        memcpy(description, errorMessage, actualSize-1);
        description[actualSize-1] = '\0';
        }
    
    if (errorCode) 
        {
        *errorCode = primary;
        }
    
Error:
    /* Unlock Session */
    Ivi_UnlockSession (vi, &locked);
    return error;
}   

ViStatus _VI_FUNC ni6229_ClearError (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: ni6229_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/**************************************************************************** 
 *  Function: ni6229_GetNextInterchangeWarning,
 *            ni6229_ResetInterchangeCheck, and
 *            ni6229_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC ni6229_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[])
{
    return Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
}

ViStatus _VI_FUNC ni6229_ResetInterchangeCheck (ViSession vi)
{
    return Ivi_ResetInterchangeCheck (vi);
}

ViStatus _VI_FUNC ni6229_ClearInterchangeWarnings (ViSession vi)
{
    return Ivi_ClearInterchangeWarnings (vi);
}

/*****************************************************************************
 * Function: ni6229_LockSession and ni6229_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC ni6229_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   


/*****************************************************************************
 * Function: ni6229_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC ni6229_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC ni6229_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC ni6229_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC ni6229_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: ni6229_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC ni6229_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC ni6229_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC ni6229_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC ni6229_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: ni6229_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the NI6229_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
ViStatus _VI_FUNC ni6229_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC ni6229_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC ni6229_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC ni6229_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC ni6229_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           

/**************************************************************************** 
 *  Function: ni6229_ConfigureMeasurement
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC ni6229_ConfigureMeasurement( 
	ViSession 		vi,
	ViConstString 	channels,
	ViInt32			sampleRate,
	ViReal64		timeInterval
)
{
    ViStatus			error = VI_SUCCESS;
	int32				derror = 0;
	ViChar				channel_name[64];
	ViChar				channel_name2[64];
	ViBoolean			is_running = VI_FALSE;
	SCardSessionsPtr	psessions = NULL;
	ViConstString		pchannels = channels; 
	ViConstString		pend;
	uInt64				samples_per_channel = 1;
	ViInt32				i, count;
	ViConstString		pCoercedChannel;
	TaskHandle			ai_th;
	
    checkErr( Ivi_LockSession (vi, VI_NULL));
	
	checkErr( Ivi_GetAttributeViBoolean( vi,
										 NULL,
										 NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING,
										 0,
										 &is_running));
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));

	
	if ( channels == NULL || strlen(channels) == 0 )
	{
		pchannels = "ai0,ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8,ai9,ai10,"
			        "ai11,ai12,ai13,ai14,ai15,ai16,ai17,ai18,ai19,"
					"ai20,ai21,ai22,ai23,ai24,ai25,ai26,ai27,ai28,"
					"ai29,ai30,ai31";
	}
	
	/* ---------------------------------------------------------------------- */
	if ( psessions->AI_data.pData )
	{
		free( psessions->AI_data.pData);
		free( psessions->AI_data.pChannelData);
		memset( &(psessions->AI_data), 0, sizeof(SChannelData));
		memset( psessions->AI_channels, 0, sizeof(SChannelInfo)*32);
	}
	
	count = 0;
	do 
	{
		pend = strchr( pchannels, ',');
		
		if ( pend )
		{
			strncpy( channel_name, pchannels, pend-pchannels); 
			channel_name[pend-pchannels] = '\0';
			pchannels = pend + 1;
		}
		else
		{
			strncpy( channel_name, pchannels, sizeof(channel_name)); 
			pchannels = pend;
		}
		checkErr(Ivi_CoerceChannelName( vi, channel_name, &pCoercedChannel));
		sprintf( psessions->AI_channels[count].daqname, "%s/%s", psessions->CardName, pCoercedChannel );
		strcpy( psessions->AI_channels[count].drivername, channel_name);  
		count++;
	} while ( pchannels);
	
/*
 * TODO: Use quick sort for channel name, WARNING: empty string must be greater and on the end
 */
	qsort( psessions->AI_channels, count, sizeof(SChannelInfo), CompareChannelInfo);
	if ( psessions->ai )
	{
		viCheckDAQ( DAQmxClearTask( psessions->ai));	
	}
	
	/* ---------------------------------------------------------------------- */
	viCheckDAQ( DAQmxCreateTask( "AnalogInputMeas", &(psessions->ai)));	
		
	psessions->AI_data.pData = calloc( count * samples_per_channel,sizeof(float64));    
	psessions->AI_data.pChannelData = calloc( samples_per_channel, sizeof(float64));
	psessions->AI_data.AllocatedSamples  = samples_per_channel;
	psessions->AI_data.AllocatedChannels = count;

	psessions->AI_data.Samples = samples_per_channel;
	
	for( i = 0; i < count; i++)
	{
		viCheckDAQ( DAQmxCreateAIVoltageChan( 
										  psessions->ai,
										  psessions->AI_channels[i].daqname,
										  "",
										  DAQmx_Val_RSE,
										  0.0,
										  10.0,
										  DAQmx_Val_Volts,
										  ""));	
	}
	

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/* thread DAQ */
static int CVICALLBACK ThreadDAQ (void *functionData)
{
 	TaskHandle HandleDAQ = (TaskHandle)functionData;
	int read;
	
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_HIGHEST);

    while (Run_DAQ == 1)
    {
	DAQmxReadAnalogF64 (HandleDAQ, DAQmx_Val_Auto, 0.01, DAQmx_Val_GroupByChannel, data, 32, &read, 0);
	}
    return 0;
}

ViStatus _VI_FUNC ni6229_StartMeasurement( 
	ViSession 		vi
)
{
	ViStatus			error = VI_SUCCESS;	
	int32				derror = 0;
	SCardSessionsPtr	psessions = NULL;

	checkErr( Ivi_LockSession (vi, VI_NULL)); 
	
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));
	if ( ! (psessions->ai) )
	{
		viCheckErr( NI6229_ERROR_MEASUREMENT_IS_NOT_CONFIGURED);
	}
	viCheckDAQ( DAQmxStartTask( psessions->ai));
	checkErr( Ivi_SetAttributeViBoolean( vi,
										 NULL,
										 NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING,
										 0,
										 VI_TRUE));
	
	//Start Thread Read all channels
	Run_DAQ = 1;
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, ThreadDAQ, (void*)psessions->ai, &threadDAQId);
	

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;	
}

ViStatus _VI_FUNC ni6229_StopMeasurement( 
	ViSession 		vi
)
{
	
	ViStatus			error = VI_SUCCESS;	
	SCardSessionsPtr	psessions = NULL;
	int32				derror = 0;

	checkErr( Ivi_LockSession (vi, VI_NULL)); 
	
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));
	
	Run_DAQ = 0;
	
	/* Wait for the thread functions to finish executing */
    CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE,
                                            threadDAQId,
                                            OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
    
    /* Release thread functions */
    CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, threadDAQId);
	
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));
	
	if ( ! (psessions->ai) )
	{
		viCheckErr( NI6229_ERROR_MEASUREMENT_IS_NOT_CONFIGURED);
	}

	viCheckDAQ( DAQmxStopTask( psessions->ai));
	checkErr( Ivi_SetAttributeViBoolean( vi,
										 NULL,
										 NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING,
										 0,
										 VI_FALSE));

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************/
ViStatus _VI_FUNC ni6229_ReadMeasurement (ViSession vi,
                                          ViChar channelName[], void *scaleInfo,
                                          ViInt32 measType,
                                          ViReal64 *value)
{	 
	ViStatus			error = VI_SUCCESS;	
	SCardSessionsPtr	psessions = NULL;
	ViConstString		pcoerced = NULL;
	uInt32				channel_index = 0;
	SChannelInfo		channel_info;
	SChannelInfoPtr		psearch;
	double				dvalue;
	int					ivalue;
	int32				derror = 0;
	SCalibrationPtr		pScaleInfo = (SCalibrationPtr)scaleInfo;
   	uInt64				samples_per_channel = 1;
	clock_t _start_debug_clock;
	double   time_debug,
			 time_Ivi_LockSession,
			 time_Ivi_CoerceChannelName,
			 time_Ivi_GetAttributeViAddr,
			 time_LinEv1D,
			 time_NI6229_VAL_MEASUREMENT_MEAN;
			 
	
	

	_start_debug_clock =  clock();
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	checkErr( Ivi_CoerceChannelName( vi, channelName, &pcoerced));
	
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));

	
	psessions->AI_data.UsedForChannel = 1;
	
	strcpy(channel_info.drivername, pcoerced);
	psearch = bsearch( &channel_info, psessions->AI_channels, 32, sizeof(SChannelInfo), CompareChannelInfo);
	

	channel_index = (psearch - (psessions->AI_channels));
	{
		float64*	pdata = data +  psessions->AI_data.UsedForChannel * channel_index;

		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k);
			LinEv1D( pdata, psessions->AI_data.UsedForChannel, k, q,  psessions->AI_data.pChannelData);
		}
		else
		{
			memcpy( psessions->AI_data.pChannelData, pdata, psessions->AI_data.UsedForChannel * sizeof(float64));
		}
	}
	
	switch( measType )
	{
		case NI6229_VAL_MEASUREMENT_RMS:
			RMS (psessions->AI_data.pChannelData, psessions->AI_data.UsedForChannel, value);
			break;
		case NI6229_VAL_MEASUREMENT_MIN:
			MaxMin1D (psessions->AI_data.pChannelData, psessions->AI_data.UsedForChannel, &dvalue, &ivalue, value, &ivalue);
			break;
		case NI6229_VAL_MEASUREMENT_MAX:
			MaxMin1D (psessions->AI_data.pChannelData, psessions->AI_data.UsedForChannel, value, &ivalue, &dvalue, &ivalue);
			break;
		case NI6229_VAL_MEASUREMENT_MEAN:
			Mean (psessions->AI_data.pChannelData, psessions->AI_data.UsedForChannel, value);
			break;
	}
	
Error:
    Ivi_UnlockSession (vi, VI_NULL);
	time_debug = (double)(clock()-_start_debug_clock);
	if (time_debug > 10.0)
	{
		printf ("ni6229_ReadMeasurement : time_debug = %f\r\n", time_debug);
		printf ("time_Ivi_LockSession = %f\r\n", time_Ivi_LockSession);
		printf ("time_Ivi_CoerceChannelName = %f\r\n", time_Ivi_CoerceChannelName);
		printf ("time_Ivi_GetAttributeViAddr = %f\r\n", time_Ivi_GetAttributeViAddr);
		printf ("time_LinEv1D = %f\r\n", time_LinEv1D);
		printf ("time_NI6229_VAL_MEASUREMENT_MEAN = %f\r\n", time_NI6229_VAL_MEASUREMENT_MEAN);
		printf ("-----------------------------------------------\r\n");
	}
	return error;      
}

/*****************************************************************************/
ViStatus _VI_FUNC ni6229_ReadWaveform( 
	ViSession 		vi,
	ViConstString	channelName,
	ViConstString	scaleName,
	ViInt32			arraySize,
	ViReal64		data[],
	ViReal64		*startX,
	ViReal64		*xInc
)
{
	ViStatus			error = VI_SUCCESS;	
	SCardSessionsPtr	psessions = NULL;
	ViConstString		pcoerced = NULL;
	uInt32				channel_index = 0;
	SChannelInfo		channel_info;
	SChannelInfoPtr		psearch;
	SCalibrationPtr		pScaleInfo = NULL;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	

	checkErr( Ivi_CoerceChannelName( vi, channelName, &pcoerced));
	
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));

	strcpy(channel_info.drivername, pcoerced);
	psearch = bsearch( &channel_info, psessions->AI_channels, 32, sizeof(SChannelInfo), CompareChannelInfo);   
	channel_index = (psearch - (psessions->AI_channels));

	{
		float64*	pdata = psessions->AI_data.pData +  psessions->AI_data.UsedForChannel * channel_index;
		ViInt32		count = (psessions->AI_data.UsedForChannel > arraySize ) ? arraySize : psessions->AI_data.UsedForChannel;
		
		checkErr( ni6229_GetChannelScale2( vi, channelName, scaleName, &pScaleInfo));
		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			double k = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			double q = k * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			
			LinEv1D( pdata, count, k, q,  data);
		}
		else
		{
			memcpy( psessions->AI_data.pChannelData, data, count * sizeof(float64));
		}
	}

Error:
	ni6229_DiscardScaleInfo(vi, &pScaleInfo);
    Ivi_UnlockSession (vi, VI_NULL);		
	return error;  
	
}

/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/
/*****************************************************************************
 * Function: ni6229_IviInit                                                       
 * Purpose:  This function is called by ni6229_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
static ViStatus ni6229_IviInit(
	ViRsrc		resourceName,
	ViBoolean	IDQuery,
	ViBoolean	reset,
	ViSession	vi
)
{
    ViStatus    error = VI_SUCCESS;
	ViChar		setup[1024] = "";
	ViChar*		pfile = NULL;
	

    checkErr( Ivi_BuildChannelTable (vi, "ai0,ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8,ai9,ai10,"
										 "ai11,ai12,ai13,ai14,ai15,ai16,ai17,ai18,ai19,ai20,"
										 "ai21,ai22,ai23,ai24,ai25,ai26,ai27,ai28,ai29,ai30,"
										 "ai31,ao1,ao2,ao3,ao4", VI_FALSE, VI_NULL));
	/* Add attributes */
    checkErr( ni6229_InitAttributes(vi, resourceName));

	checkErr( Ivi_GetAttributeViString (vi,
										VI_NULL,
										IVI_ATTR_DRIVER_SETUP,
										0, sizeof(setup),
										setup));
	pfile  = strstr( setup, "CalibrationFile:");
	if ( pfile )
	{
		pfile += strlen("CalibrationFile:");
		for ( ; *pfile != '\0' && isspace(*pfile); pfile++)
			; /* do nothing */
	}
	
    checkErr( ni6229_LoadCalibration( vi, pfile));
	
	/*- Reset instrument ----------------------------------------------------*/
    if (reset) 
        checkErr( ni6229_reset (vi));
    else  /*- Send Default Instrument Setup ---------------------------------*/
        checkErr( ni6229_DefaultInstrSetup (vi));

	
	
	/*- Identification Query ------------------------------------------------*/
	if (IDQuery)                               
	{
		ViChar rdBuffer[BUFFER_SIZE];
	
	    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, NI6229_ATTR_ID_QUERY_RESPONSE, 
	                                        0, BUFFER_SIZE, rdBuffer));
	    
#if 0
	    #define VALID_RESPONSE_STRING_START        "XXX"

		if (strncmp (rdBuffer, VALID_RESPONSE_STRING_START, 
	                 strlen(VALID_RESPONSE_STRING_START)) != 0)
	        {             
	        viCheckErr( VI_ERROR_FAIL_ID_QUERY);
	        }
#endif
	 }

    checkErr( ni6229_CheckStatus (vi));

Error:
    if (error < VI_SUCCESS)
        {

		}
    return error;
}

/*****************************************************************************
 * Function: ni6229_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           ni6229_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
static ViStatus ni6229_IviClose (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
	CAObjHandle	objHandle;
	SCardSessionsPtr	psessions = NULL;

        /* Do not lock here.  The caller manages the lock. */
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));
	DAQmxClearTask( psessions->ai);
	
	free( psessions->AI_data.pData);
	free( psessions->AI_data.pChannelData);
	psessions->AI_data.AllocatedSamples = 0;
	psessions->AI_data.AllocatedChannels = 0;

	
	checkErr( Ivi_GetAttributeViAddr( vi, "", NI6229_ATTR_INSTR_CALIBRATION_FILE, 0, (ViAddr*)&objHandle));

Error:
	if ( objHandle )
	{
		CA_DiscardObjHandle(objHandle);	
	}
    Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, VI_NULL);
    return error;   
}

/*****************************************************************************
 * Function: ni6229_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTRUMENT_SPECIFIC.  The user can set the 
 *           NI6229_ATTR_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus ni6229_CheckStatus (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
        {
        checkErr( ni6229_CheckStatusCallback (vi, Ivi_IOSession(vi)));
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
        }
        
Error:
    return error;
}

/*****************************************************************************
 * Function: ni6229_WaitForOPC                                                  
 * Purpose:  This function waits for the instrument to complete the      
 *           execution of all pending operations.  This function is a        
 *           wrapper for the WaitForOPCCallback.  It can be called from 
 *           other instrument driver functions. 
 *
 *           The maxTime parameter specifies the maximum time to wait for
 *           operation complete in milliseconds.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus ni6229_WaitForOPC (ViSession vi, ViInt32 maxTime) 
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        {
        ViInt32  oldOPCTimeout; 
        
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, NI6229_ATTR_OPC_TIMEOUT, 
                                           0, &oldOPCTimeout));
        Ivi_SetAttributeViInt32 (vi, VI_NULL, NI6229_ATTR_OPC_TIMEOUT,        
                                 0, maxTime);

        error = ni6229_WaitForOPCCallback (vi, Ivi_IOSession(vi));

        Ivi_SetAttributeViInt32 (vi, VI_NULL, NI6229_ATTR_OPC_TIMEOUT, 
                                 0, oldOPCTimeout);
        viCheckErr( error);
        }
Error:
    return error;
}

/*****************************************************************************
 * Function: ni6229_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           ni6229_reset function calls this function.  The 
 *           ni6229_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus ni6229_DefaultInstrSetup (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
        
    /* Invalidate all attributes */
    checkErr( Ivi_InvalidateAllAttributes (vi));
    
    if (!Ivi_Simulating(vi))
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		
        }
Error:
    return error;
}



/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: ni6229_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           ni6229_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           NI6229_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC ni6229_CheckStatusCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;
	ViInt32		value;
	
	/* Query instrument status */
	checkErr( Ivi_GetAttributeViInt32( vi, "", NI6229_ATTR_INSTR_DAQ_ERROR, 0, &value));
	
    if ( value )
    {
        viCheckErr( IVI_ERROR_INSTR_SPECIFIC);
    }
            
Error:
    return error;
}

/*****************************************************************************
 * Function: ni6229_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           ni6229_WaitForOPC utility function.  The IVI engine invokes
 *           this function in the following two cases:
 *           - Before invoking the read callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_BEFORE_READS flag is set.
 *           - After invoking the write callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_AFTER_WRITES flag is set.
 *****************************************************************************/
static ViStatus _VI_FUNC ni6229_WaitForOPCCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;

    return error;
}

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*- NI6229_ATTR_ID_QUERY_RESPONSE -*/
static ViStatus _VI_FUNC eldaqAttrIdQueryResponse_ReadCallback(
	ViSession vi,
	ViSession io,
	ViConstString channelName, 
	ViAttr attributeId,
	const ViConstString cacheValue
)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
	SCardSessionsPtr	psessions = NULL;
	int32				derror = 0;
	
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));
	
	viCheckDAQ(	DAQmxGetDeviceAttribute (psessions->CardName, DAQmx_Dev_ProductType, rdBuffer, sizeof(rdBuffer)));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}
    
/*- NI6229_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC eldaqAttrDriverRevision_ReadCallback(
	ViSession vi, 
	ViSession io,
	ViConstString channelName, 
	ViAttr attributeId,
	const ViConstString cacheValue
)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      driverRevision[256];
    
    
    sprintf (driverRevision, 
             "Driver: eldaq %.2f, Compiler: %s %3.2f, "
             "Components: IVIEngine %.2f, VISA-Spec %.2f",
             NI6229_MAJOR_VERSION + NI6229_MINOR_VERSION/1000.0, 
             IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
             IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
             Ivi_ConvertVISAVer(VI_SPEC_VERSION));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
    return error;
}


/*- NI6229_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/
static ViStatus _VI_FUNC eldaqAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, ""));
    
Error:
    return error;
}

/*- NI6229_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC eldaqAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, "National Instruments"));
    
Error:
    return error;
}

    

/*- NI6229_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC eldaqAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
	SCardSessionsPtr	psessions = NULL;
	int32				derror = 0;
	
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));
	
	viCheckDAQ( DAQmxGetDeviceAttribute (psessions->CardName, DAQmx_Dev_ProductType, rdBuffer, sizeof(rdBuffer)));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}
    


/*****************************************************************************
 * Function: ni6229_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/
static ViStatus ni6229_InitAttributes (ViSession vi, ViConstString cardName)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     flags = 0;
	SCardSessionsPtr	pcard = NULL;
	
	
	pcard = calloc( 1, sizeof( SCardSessions));
	
	strcpy( pcard->CardName, cardName);
    
        /*- Initialize instrument attributes --------------------------------*/            
#if 0
    checkErr( Ivi_SetAttributeViInt32 (vi, "", NI6229_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION,
                                       0, NI6229_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", NI6229_ATTR_SPECIFIC_DRIVER_MINOR_VERSION,
                                       0, NI6229_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", NI6229_ATTR_IO_SESSION_TYPE,
                                        0, NI6229_IO_SESSION_TYPE));
#endif
    checkErr( Ivi_SetAttrReadCallbackViString (vi, NI6229_ATTR_SPECIFIC_DRIVER_REVISION,
                                               eldaqAttrDriverRevision_ReadCallback));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", NI6229_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                       0, NI6229_CLASS_SPEC_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", NI6229_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                       0, NI6229_CLASS_SPEC_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", NI6229_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                        0, NI6229_SUPPORTED_INSTRUMENT_MODELS));


    checkErr( Ivi_GetAttributeFlags (vi, NI6229_ATTR_INSTRUMENT_FIRMWARE_REVISION, &flags));
    checkErr( Ivi_SetAttributeFlags (vi, NI6229_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                     flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, NI6229_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                               eldaqAttrFirmwareRevision_ReadCallback));

	checkErr( Ivi_GetAttributeFlags (vi, NI6229_ATTR_INSTRUMENT_MANUFACTURER, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, NI6229_ATTR_INSTRUMENT_MANUFACTURER, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, NI6229_ATTR_INSTRUMENT_MANUFACTURER,
	                                           eldaqAttrInstrumentManufacturer_ReadCallback));

	checkErr( Ivi_GetAttributeFlags (vi, NI6229_ATTR_INSTRUMENT_MODEL, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, NI6229_ATTR_INSTRUMENT_MODEL, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, NI6229_ATTR_INSTRUMENT_MODEL,
	                                           eldaqAttrInstrumentModel_ReadCallback));

    checkErr( Ivi_SetAttributeViString (vi, "", NI6229_ATTR_SPECIFIC_DRIVER_VENDOR,
                                        0, NI6229_DRIVER_VENDOR));
    checkErr( Ivi_SetAttributeViString (vi, "", NI6229_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                        0, NI6229_DRIVER_DESCRIPTION));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
                                      ni6229_WaitForOPCCallback));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                      ni6229_CheckStatusCallback));


        
	checkErr( Ivi_SetAttributeViString (vi, "", NI6229_ATTR_GROUP_CAPABILITIES, 0,
	                                    "None"));
    
        /*- Add instrument-specific attributes ------------------------------*/            
	
	checkErr( Ivi_AddAttributeViString (vi, NI6229_ATTR_ID_QUERY_RESPONSE,
	                                    "NI6229_ATTR_ID_QUERY_RESPONSE", 
	                                    "XXX",
	                                    IVI_VAL_NOT_USER_WRITABLE,
	                                    eldaqAttrIdQueryResponse_ReadCallback,
	                                    VI_NULL));
	                                           
    checkErr( Ivi_AddAttributeViInt32 (vi, NI6229_ATTR_OPC_TIMEOUT,
                                       "NI6229_ATTR_OPC_TIMEOUT",
                                       5000, IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL)); 
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       NI6229_ATTR_VISA_RM_SESSION,
                                       "NI6229_ATTR_VISA_RM_SESSION",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi, NI6229_ATTR_OPC_CALLBACK,
                                       "NI6229_ATTR_OPC_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       NI6229_ATTR_CHECK_STATUS_CALLBACK,
                                       "NI6229_ATTR_CHECK_STATUS_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       NI6229_ATTR_USER_INTERCHANGE_CHECK_CALLBACK,
                                       "NI6229_ATTR_USER_INTERCHANGE_CHECK_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
	checkErr( Ivi_AddAttributeViString (vi,
                                       NI6229_ATTR_INSTR_CARD_NAME,
                                       "NI6229_ATTR_INSTR_CARD_NAME",
                                       cardName,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL));
	/*- NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING -*/
	    checkErr( Ivi_AddAttributeViBoolean (vi,
                                       NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING,
                                       "NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING",
                                       VI_FALSE,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL));
	
	/*- NI6229_ATTR_INST_DAQ_SESSIONS -*/	
	checkErr( Ivi_AddAttributeViAddr (vi,
                                       NI6229_ATTR_INST_DAQ_SESSIONS,
                                       "NI6229_ATTR_INST_DAQ_SESSIONS",
                                       (ViAddr)pcard,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL));
	/*- NI6229_ATTR_INSTR_CALIBRATION_FILE -*/
	checkErr( Ivi_AddAttributeViAddr (vi,
                                       NI6229_ATTR_INSTR_CALIBRATION_FILE,
                                       "NI6229_ATTR_INSTR_CALIBRATION_FILE",
                                       (ViAddr)pcard,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL));
	/*- NI6229_ATTR_CALIBRATION_FILE_NAME -*/
	checkErr( Ivi_AddAttributeViString (vi, NI6229_ATTR_CALIBRATION_FILE_NAME,
	                                    "NI6229_ATTR_CALIBRATION_FILE_NAME", 
	                                    "",
	                                    IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_ALWAYS_CACHE,
	                                    VI_NULL,
	                                    VI_NULL));
	
	checkErr( Ivi_AddAttributeViInt32 (vi,
                                       NI6229_ATTR_INSTR_DAQ_ERROR,
                                       "NI6229_ATTR_INSTR_DAQ_ERROR",
                                       0,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
                                       
        /*- Setup attribute invalidations -----------------------------------*/            

    /*=CHANGE:===============================================================*
       
       Set attribute dependencies by calling the additional 
       Ivi_AddAttributeInvalidation functions here.  Remove the dependencies
       that do not apply to your instrument by deleting the calls to
       Ivi_AddAttributeInvalidation.
       When you initially add an attribute, it applies to all channels / repeated
       capability instances.  If you want it to apply to only a subset, call the 
       Ivi_RestrictAttrToChannels or Ivi_RestrictAttrToInstances function.

     *============================================================END=CHANGE=*/

Error:
    return error;
}

/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/

int32 CVICALLBACK EveryNCallback(
		TaskHandle	taskHandle,
		int32		everyNsamplesEventType,
		uInt32		nSamples,
		void 		*callbackData
)
{
	int32   			error = 0;
	ViSession			vi = (ViSession)callbackData;
	SCardSessionsPtr	psessions = NULL; 
	int32				derror = 0;
	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));

	viCheckDAQ( DAQmxReadAnalogF64( psessions->ai, 
									psessions->AI_data.Samples,
									1.0e-3,
									DAQmx_Val_GroupByChannel,
									psessions->AI_data.pData,
									psessions->AI_data.AllocatedSamples*psessions->AI_data.AllocatedChannels,
									&psessions->AI_data.UsedForChannel, NULL));
	

Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   	error=0;
	ViSession	vi = (ViSession)callbackData;

	checkErr( Ivi_SetAttributeViBoolean( vi,
										 NULL,
										 NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING,
										 0,
										 VI_FALSE));
Error:
	return error;
}


int CompareChannelInfo( const void* info1, const void* info2)
{
	SChannelInfoPtr pinfo1	 = (SChannelInfoPtr)info1;
	SChannelInfoPtr pinfo2	 = (SChannelInfoPtr)info2;
	size_t	l1 = strlen(pinfo1->drivername);
	size_t	l2 = strlen(pinfo2->drivername);
	
	if ( pinfo1->drivername[0]	 == '\0' || pinfo2->drivername[0]	 == '\0')
	{
		if ( pinfo1->drivername[0]	 != '\0' )
		{
			return (-1);	
		}
		if ( pinfo2->drivername[0]	 != '\0' )
		{
			return (1);	
		}	
		return 0;
	}
#if 1
	if ( l1 != l2 )
	{
		if ( l1 > l2 )
			return (1);
		return -1;
	}
#endif	
	return strcmp( pinfo1->drivername, pinfo2->drivername); 
}



ViStatus _VI_FUNC ni6229_GetChannelScale (ViSession vi,
                                          ViChar channelName[],
                                          ViConstString scaleName,
                                          void* _pScaleInfo)
{
	
	ViStatus					error = VI_SUCCESS;
	CAObjHandle					objHandle;
	char						query_string[1024] = "";
	MSXMLObj_IXMLDOMNodeList	nodes = 0;
	MSXMLObj_IXMLDOMNode		attr = 0,node = 0;
	MSXMLObj_IXMLDOMNamedNodeMap	attrs = 0;
	long						length;
	char						*ptype = NULL;
		
	checkErr( Ivi_GetAttributeViAddr( vi, "", NI6229_ATTR_INSTR_CALIBRATION_FILE, 0, (ViAddr*)&objHandle));

	sprintf(query_string, "//channel[@name='%s']/scale[@name='%s']/calibration", channelName, scaleName);  
	MSXML_IXMLDOMDocumentselectNodes (objHandle, NULL, query_string, &nodes);

	MSXML_IXMLDOMNodeListGetlength (nodes, NULL, &length);
	if ( length == 1 )
	{
		MSXML_IXMLDOMNodeListreset (nodes, NULL);	
		MSXML_IXMLDOMNodeListnextNode (nodes, NULL, &node);
		MSXML_IXMLDOMNodeGetattributes (node, NULL, &attrs);
		MSXML_IXMLDOMNamedNodeMapgetNamedItem (attrs, NULL, "type", &attr);
		MSXML_IXMLDOMNodeGettext(attr, NULL,  &ptype);
		
		if ( strcmp( ptype, "linear") == 0 )
		{
			checkErr( ni6229_ParseLinearScale( vi, node, (SCalibrationPtr*)_pScaleInfo)); 
		}
		else
		{
			viCheckErr( NI6229_ERROR_NOT_SUPPORTED_CALIBRATION_TYPE);	
		}
	}
	else
	{
		viCheckErr( NI6229_ERROR_NOT_SPECIFIED_CALIBRATION_TYPE);	
	}
	
	
Error:
	if ( ptype ) CA_FreeMemory( ptype);
	if ( attr ) CA_DiscardObjHandle( attr);
	if ( attrs ) CA_DiscardObjHandle( attrs);
	if ( node ) CA_DiscardObjHandle( node);
	if ( nodes ) CA_DiscardObjHandle( nodes);
	return error;	
}

static ViStatus ni6229_GetChannelScale2( ViSession 		vi,
									   ViConstString	channelName,
									   ViConstString	scaleName,
									   SCalibrationPtr*	pScaleInfo
)
{
	ViStatus					error = VI_SUCCESS;
	CAObjHandle					objHandle;
	char						query_string[1024] = "";
	MSXMLObj_IXMLDOMNodeList	nodes = 0;
	MSXMLObj_IXMLDOMNode		attr = 0,node = 0;
	MSXMLObj_IXMLDOMNamedNodeMap	attrs = 0;
	long						length;
	char						*ptype = NULL;
	
	checkErr( Ivi_GetAttributeViAddr( vi, "", NI6229_ATTR_INSTR_CALIBRATION_FILE, 0, (ViAddr*)&objHandle));

	sprintf(query_string, "//channel[@name='%s']/scale[@name='%s']/calibration", channelName, scaleName);  
	MSXML_IXMLDOMDocumentselectNodes (objHandle, NULL, query_string, &nodes);

	MSXML_IXMLDOMNodeListGetlength (nodes, NULL, &length);
	if ( length == 1 )
	{
		MSXML_IXMLDOMNodeListreset (nodes, NULL);	
		MSXML_IXMLDOMNodeListnextNode (nodes, NULL, &node);
		MSXML_IXMLDOMNodeGetattributes (node, NULL, &attrs);
		MSXML_IXMLDOMNamedNodeMapgetNamedItem (attrs, NULL, "type", &attr);
		MSXML_IXMLDOMNodeGettext(attr, NULL,  &ptype);
		
		if ( strcmp( ptype, "linear") == 0 )
		{
			checkErr( ni6229_ParseLinearScale( vi, node, pScaleInfo)); 
		}
		else
		{
			viCheckErr( NI6229_ERROR_NOT_SUPPORTED_CALIBRATION_TYPE);	
		}
	}
	else
	{
		viCheckErr( NI6229_ERROR_NOT_SPECIFIED_CALIBRATION_TYPE);	
	}
	
	

Error:
	if ( ptype ) CA_FreeMemory( ptype);
	if ( attr ) CA_DiscardObjHandle( attr);
	if ( attrs ) CA_DiscardObjHandle( attrs);
	if ( node ) CA_DiscardObjHandle( node);
	if ( nodes ) CA_DiscardObjHandle( nodes);
	return error;	
}

static ViStatus ni6229_ParseLinearScale(
	ViSession				vi,
	MSXMLObj_IXMLDOMNode	node,
	SCalibrationPtr*		pScaleInfo
)
{
	ViStatus					error = VI_SUCCESS;	
	MSXMLObj_IXMLDOMNodeList	nodes = 0;
	MSXMLObj_IXMLDOMNode		value_node = 0, real_node = 0, cal_node = 0;
	char						*ptext = NULL;
	char						*pend = NULL;
	SLinearCalibrationPtr		plincal = NULL;
	SCalibrationPointPtr		ppoint = NULL;
	long						length = 0;

	MSXML_IXMLDOMNodeselectNodes (node, NULL, "measurement", &nodes);
	MSXML_IXMLDOMNodeListGetlength (nodes, NULL, &length);
	
	if ( length < 2 )
	{
		viCheckErr( NI6229_ERROR_LINEAR_CALIBRATION_CONFIG);
	}
	MSXML_IXMLDOMNodeListreset( nodes, NULL);
	
	(*pScaleInfo) = calloc( 1, sizeof(SCalibration));
	(*pScaleInfo)->calType  = CAL_TYPE_LINEAR;
	(*pScaleInfo)->pCalibData = calloc( 1, sizeof(SLinearCalibration));
	plincal = (SLinearCalibrationPtr)((*pScaleInfo)->pCalibData);
	plincal->count   = length;
	plincal->pPoints = calloc(length, sizeof(SCalibrationPoint));
	ppoint = plincal->pPoints;
	for( ; length > 0; length--, ppoint++)
	{
		if ( ptext ) CA_FreeMemory( ptext);
		if ( real_node ) CA_DiscardObjHandle( real_node);
		if ( value_node ) CA_DiscardObjHandle( value_node);
		if ( cal_node ) CA_DiscardObjHandle( cal_node);	
		MSXML_IXMLDOMNodeListnextNode (nodes, NULL, &cal_node);
		
		MSXML_IXMLDOMNodeselectSingleNode (cal_node, NULL, "value", &value_node);
		MSXML_IXMLDOMNodeselectSingleNode (cal_node, NULL, "real", &real_node);	
		MSXML_IXMLDOMNodeGettext(value_node, NULL,  &ptext);
		ppoint->measValue = strtod( ptext, &pend);
		MSXML_IXMLDOMNodeGettext(real_node, NULL,  &ptext);
		ppoint->realValue = strtod( ptext, &pend);
	}

Error:
	if ( ptext ) CA_FreeMemory( ptext);
	if ( real_node ) CA_DiscardObjHandle( real_node);
	if ( value_node ) CA_DiscardObjHandle( value_node);
	if ( cal_node ) CA_DiscardObjHandle( cal_node);
	if ( nodes ) CA_DiscardObjHandle( nodes);
	return error;	
}

static ViStatus ni6229_DiscardScaleInfo(
	ViSession				vi,
	SCalibrationPtr*		pScaleInfo
)
{
	ViStatus					error = VI_SUCCESS;	
	
	if (pScaleInfo && (*pScaleInfo) && (*pScaleInfo)->calType == CAL_TYPE_LINEAR)
	{
		SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)((*pScaleInfo)->pCalibData);
		free( plincal->pPoints);
		plincal->pPoints = 0;
		plincal->count = 0;
		free((*pScaleInfo)->pCalibData);
		(*pScaleInfo)->pCalibData = NULL;
	}
	free( (*pScaleInfo));
	(*pScaleInfo) = NULL;
	
	
	return error;	
}

ViStatus _VI_FUNC ni6229_WriteDigitalLine (ViSession vi,
                                           ViChar digitalChannels[],
                                           ViInt32 status)
{
	
	
 	ViStatus			error = VI_SUCCESS;
	int32				derror = 0;
	ViChar				channel_name[64];
	ViBoolean			is_running = VI_FALSE;
	SCardSessionsPtr	psessions = NULL;
	uInt8       data[8];

	
    checkErr( Ivi_LockSession (vi, VI_NULL));
	
	checkErr( Ivi_GetAttributeViBoolean( vi,
										 NULL,
										 NI6229_ATTR_INSTR_MEASUREMENT_IS_RUNNING,
										 0,
										 &is_running));
	checkErr( Ivi_GetAttributeViAddr( vi,
								      NULL,
									  NI6229_ATTR_INST_DAQ_SESSIONS,
									  0,
									  (ViAddr*)&psessions));
	
/*
 * TODO: Use quick sort for channel name, WARNING: empty string must be greater and on the end
 */
	if ( psessions->DO )
	{
		viCheckDAQ( DAQmxClearTask( psessions->DO));	
	}
	
	/* ---------------------------------------------------------------------- */
	viCheckDAQ( DAQmxCreateTask( "", &(psessions->DO)));
	sprintf(channel_name, "%s/%s", psessions->CardName, digitalChannels);
	viCheckDAQ( DAQmxCreateDOChan(psessions->DO,channel_name,"",DAQmx_Val_ChanForAllLines));	
	viCheckDAQ( DAQmxStartTask(psessions->DO));	
	data[0] = status;
	data[1] = 0X00;
	data[2] = 0X00;
	data[3] = 0X00;
	data[4] = 0X00;
	data[5] = 0X00;
	data[6] = 0X00;
	data[7] = 0X00;

	viCheckDAQ (DAQmxWriteDigitalLines(psessions->DO,1,1,10.0,DAQmx_Val_GroupByChannel,data,NULL,NULL));


Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
    
}


ViStatus _VI_FUNC ni6229_LoadCalibration(
	ViSession		vi,
	ViConstString	calibrationFile
)
{
	ViStatus	error = VI_SUCCESS;
	VARIANT		vFile = CA_VariantEmpty();
	CAObjHandle	objHandle = 0;
	CAObjHandle	objHandle2 = 0;
	VBOOL		is_success;
	const char* file_name = (calibrationFile) ? calibrationFile : "calibration.xml";
	
	
	CA_VariantSetCString( &vFile, file_name);
	MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &objHandle);
	MSXML_IXMLDOMDocumentload (objHandle, NULL, vFile, &is_success);	
	
	checkErr( Ivi_GetAttributeViAddr( vi, "", NI6229_ATTR_INSTR_CALIBRATION_FILE, 0, (ViAddr*)&objHandle2)); 
	if ( objHandle2 )
	{
		//CA_DiscardObjHandle(objHandle2);	
		objHandle2 = 0;
		checkErr( Ivi_SetAttributeViString( vi, VI_NULL, NI6229_ATTR_CALIBRATION_FILE_NAME,  0, ""));
	}

	checkErr( Ivi_SetAttributeViAddr( vi, "", NI6229_ATTR_INSTR_CALIBRATION_FILE, 0, (ViAddr)objHandle));
	checkErr( Ivi_SetAttributeViString( vi, VI_NULL, NI6229_ATTR_CALIBRATION_FILE_NAME,  0, calibrationFile));

Error:
	CA_VariantClear(&vFile);
	return error;
}
