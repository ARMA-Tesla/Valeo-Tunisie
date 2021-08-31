/****************************************************************************
 *                            I V I B R I D G E
 *---------------------------------------------------------------------------
 *    Copyright   2005 ELCOM, a.s.  All Rights Reserved.        
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:       ivibridge.c                                                    
 * Purpose:     IVI Bridge interface
 * Version:     1.0  
 ****************************************************************************/
#include "ivibridge.h"
#include <elerror.h>
#include <stdio.h> /* should be in elerror.h */
#include <stdlib.h>
#include "msxmldom.h"
#include "IviDCPwr.h"
#include "iviScope.h"
#include "iviSwtch.h"
#include "iviDmm.h"
#include "IviPwrMeter.h"
#include "iviFgen.h"
#include "IviRFSigGen.h"
#include "IviSpecAn.h"
#include <resistor/class/Resistor.h>
#include <eldaq/class/eldaq.h>
#include <guid.h>

#define MAX_LENGTH_IVI_LOGICAL_NAME	64


typedef struct _SIVIStorageEntry
{
	ViSession	vi;	
	ViChar		name[256];
	ViInt32		classType;
} SIVIStorageEntry, *SIVIStorageEntryPtr;

typedef struct _SValueStringEntry
{
	ViChar*		name;
	ViInt32		value;
} SValueStringEntry, *SValueStringEntryPtr;

enum
{
	E_DCPOWER = 0,
	E_SCOPE,
	E_SWITCH,
	E_DMM,
	E_PWRMETER,
	E_FGEN,
	E_RESISTOR,
	E_RFSIGGEN,
	E_SPECAN,
	E_ELDAQ
};

static SValueStringEntry	gs_DriverTypes[] =
{
	{"IviSwtch", 	E_SWITCH},
	{"IviScope", 	E_SCOPE},
	{"IviDmm",   	E_DMM},
	{"IviFgen ", 	E_FGEN},
	{"IviDCPwr", 	E_DCPOWER},
	{"IviPwrMeter", E_PWRMETER},
	{"IviSpecAn", 	E_SPECAN},
	{"IviRFSigGen", E_RFSIGGEN},
	{"IviResistor", E_RESISTOR},
	{"IviELDAQ",    E_ELDAQ},
	{ NULL, 0}
};



/*****************************************************************************
 * Static no-exported functions declaration **********************************
 *****************************************************************************/
static	SElExceptionPtr	ivibridgeLock(
		struct _SIviBridge* pIviBridge
);
static	SElExceptionPtr	ivibridgeUnlock(
		struct _SIviBridge* pIviBridge
);
static SElExceptionPtr	ivibridgeInitializeConfiguration(
	struct _SIviBridge*		pIviBridge,
	const char*				pConfigStore
);
static SElExceptionPtr	ivibridgeClose(
	struct _SIviBridge* pIviBridge
);
static SElExceptionPtr	ivibridgeGetSession(
	struct _SIviBridge* pIviBridge,
	const char*			name,
	ViSession*	vi
);
static SElExceptionPtr InitializeDrivers(
	struct _SIviBridge*	pIVIBridge
);
static int32_t	GetEntryFromString( const char* string);
static const char*	GetEntryFromValue( int32_t value);
static int CompareIviStorageEntry( const void* a, const void* b );


/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "ivibridge_new"
/*!
 * \brief
 * This function creates a new instance.
 * 
 * \param pIviBridge 
 * Returns new instance of the IVI Bridge object. If the function fails, NULL
 * pointer is returned
 * 
 * \return 
 * Pointer to exception object. if the function is successfull, a null pointer is
 * return.
 *
 * This function creates a new instance of the IVI Bridge object.
 */
 SElExceptionPtr ivibridge_new(
	SIviBridgePtr*	pIviBridge
)
{
	SElExceptionPtr    	pexception = NULL;
	int32_t				error = 0;

	*pIviBridge = calloc( 1, sizeof(SIviBridge));
	EXCCHECKALLOC( *pIviBridge);

	(*pIviBridge)->InitializeConfiguration  = ivibridgeInitializeConfiguration;
	(*pIviBridge)->GetSession = ivibridgeGetSession;
	(*pIviBridge)->Close      = ivibridgeClose;

Error:
	EXCRETHROW( pexception); 
}/* vline_new */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "ivibridge_delete"
/*!
 * \brief
 * This function destroys specified instance.
 * 
 * \param pIviBridge 
 * Pointer to instance of the VLine object cretaed by ivibridge_new() function.
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
SElExceptionPtr	ivibridge_delete(
	SIviBridgePtr*	pIviBridge
)
{
	if ( pIviBridge && *pIviBridge )
	{
		(*pIviBridge)->Close( *pIviBridge);
/*		DeleteCriticalSection( &((*pVLine)->_Lock)); */
		free( (*pIviBridge)->_pStorage);
		free( *pIviBridge);
		*pIviBridge = NULL;
	}

/* Error: */
	return NULL;
} /* vline_delete */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ivibridgeInitializeConfiguration"
/*!
 * \brief
 * This function opens all driver sessions specified in configuration store
 * 
 * \param pIVIBridge
 * Pointer to instance of the IVI Bridge object cretaed by ivibridge_new() function.
 * 
 * \param pConfigStore path to configuration file of IVI Bridge module. If the 
 *                     path is empty default file is "ivibridge.xml"
 * 
 * \return 
 *  Returns an exception. If the function pass NULL pointer is returned.
 */
static SElExceptionPtr	ivibridgeInitializeConfiguration(
	struct _SIviBridge*		pIVIBridge,
	const char*				pConfigStore
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	char         	_message[512];
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0, plist = 0, pnode2old = 0;
	CAObjHandle     pparameter = 0;
	CAObjHandle     pitem = 0;
	const char*     pfile_name = (pConfigStore) ? pConfigStore : ".\\modules\\ivibridge.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*         	ptext= NULL;
	
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCOM( CA_VariantSetCString(  &pfile_name_var, pfile_name));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//module[@id='"MODULE_IVIBRIDGE"']", &pnode) );   	
	if(pnode == 0)
	{
		char		message[2048]="";
		sprintf( message, "Configuration for module '"MODULE_IVIBRIDGE"' in file %s is missing.",
				 pfile_name);
		EXCTHROW( IVIBRIDGE_ERROR_CONFIGURATION_MISSING, message);
	}


	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (pnode, NULL, "driver", &plist)); 
	if(plist)
	{
		long			length = 0;
		char			class_name[MAX_LENGTH_IVI_LOGICAL_NAME];
		SIVIStorageEntryPtr	pentry = NULL;

		EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));  
	
		pentry = calloc( length, sizeof(SIVIStorageEntry));
		pIVIBridge->_pStorage =  (void*)pentry;
		pIVIBridge->_alloc_count = length;
		pIVIBridge->_used = 0;
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));  
	
		do
		{
			if ( pitem ) 
			{ 
				CA_DiscardObjHandle( pitem ); 
				pitem = 0; 
			} 
		
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pitem));   
			if ( pitem )
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pitem, NULL, "name", &pparameter));
				if(pparameter)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext)
					{
						strncpy(pentry->name, ptext, MAX_LENGTH_IVI_LOGICAL_NAME);	
					}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					CA_FreeMemory(ptext);
					ptext=NULL;
				}
				
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pitem, NULL, "class", &pparameter));
				if(pparameter)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && strlen(pentry->name)>0)
					{
						strncpy(class_name, ptext, MAX_LENGTH_IVI_LOGICAL_NAME);
						pentry->classType = GetEntryFromString( class_name);
						
						pentry++;
						pIVIBridge->_used += 1;
					}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					CA_FreeMemory(ptext);
					ptext=NULL;
				}
			}
		} while ( pitem );
	
		qsort( pIVIBridge->_pStorage, pIVIBridge->_used, sizeof(SIVIStorageEntry), CompareIviStorageEntry); 
	}
	EXCCHECK( InitializeDrivers( pIVIBridge)); 
	
Error:
	CA_FreeMemory(ptext);
	CA_VariantClear(&pfile_name_var);
	if (plist) CA_DiscardObjHandle(plist);
	if (pitem) CA_DiscardObjHandle(pitem);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
} /* ivibridgeInitializeConfiguration */


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ivibridgeClose"
/*!
 * \brief
 * This function close all driver sessions
 * 
 * \param pIVIBridge
 * Pointer to instance of the IVI Bridge object cretaed by ivibridge_new() function.
 * 
 * \return 
 *  Returns an exception. If the function pass NULL pointer is returned.
 */
static SElExceptionPtr	ivibridgeClose(
	struct _SIviBridge* pIVIBridge
)
{
	ViStatus	error = VI_SUCCESS;
	SElExceptionPtr     pexception = NULL;
	SIVIStorageEntryPtr	pentry = (SIVIStorageEntryPtr) pIVIBridge->_pStorage;
	int32_t		i;
	ViChar		message1[256] = "";
	
	for( i = 0; i < pIVIBridge->_used; i++, pentry++)
	{
		switch ( pentry->classType)
		{
			case E_DCPOWER:
				if ( pentry->vi) error = IviDCPwr_close( pentry->vi);
				if ( error < VI_SUCCESS ) IviDCPwr_error_message( pentry->vi, error, message1);
				break;
			case E_SCOPE:
				if ( pentry->vi) error = IviScope_close( pentry->vi);
				if ( error < VI_SUCCESS ) IviScope_error_message( pentry->vi, error, message1);
				break;
			case E_SWITCH:
				if ( pentry->vi) error = IviSwtch_close( pentry->vi);
				if ( error < VI_SUCCESS ) IviSwtch_error_message( pentry->vi, error, message1);
				break;
			case E_DMM:
				if ( pentry->vi) error = IviDmm_close( pentry->vi);
				if ( error < VI_SUCCESS ) IviDmm_error_message( pentry->vi, error, message1);
				break;
			case E_PWRMETER:
				if ( pentry->vi) error = IviPwrMeter_close( pentry->vi);
				if ( error < VI_SUCCESS ) IviPwrMeter_error_message( pentry->vi, error, message1);
				break;
			case E_FGEN:
				if ( pentry->vi) error = IviFgen_close( pentry->vi);
				if ( error < VI_SUCCESS ) IviFgen_error_message( pentry->vi, error, message1);
				break;
			case E_RFSIGGEN:
				if ( pentry->vi) error = IviRFSigGen_close( pentry->vi);
				if ( error < VI_SUCCESS ) IviRFSigGen_error_message( pentry->vi, error, message1);
				break;
			case E_SPECAN:
				if ( pentry->vi) error = IviSpecAn_close( pentry->vi);
				if ( error < VI_SUCCESS ) IviSpecAn_error_message( pentry->vi, error, message1);
				break;
			case E_RESISTOR:
				if ( pentry->vi) error = Resistor_close( pentry->vi);
				if ( error < VI_SUCCESS ) Resistor_error_message( pentry->vi, error, message1);
				break;
			case E_ELDAQ:
				if ( pentry->vi) error = eldaq_close( pentry->vi);
				if ( error < VI_SUCCESS ) eldaq_error_message( pentry->vi, error, message1);
				break;
			default:
				EXCTHROW( IVIBRIDGE_ERORR_UNKNOWN_CLASS_DRIVER, "Unknown IVI Class");
				break;
		}
		checkErr( error);
	}

Error:
	if ( error < 0 )
	{
		char message[1024] = "";
		sprintf( message, "IVI Driver Initialization fails.\n%s", message1);
		error = 0;
		EXCTHROW( IVIBRIDGE_ERORR_INIT, message);
	}
	EXCRETHROW( pexception);	
} /* ivibridgeClose */


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ivibridgeGetSession"
/*!
 * \brief
 * This function close all driver sessions
 * 
 * \param pIVIBridge
 * Pointer to instance of the IVI Bridge object cretaed by ivibridge_new() function.
 * 
 * \return 
 *  Returns an exception. If the function pass NULL pointer is returned.
 */
static SElExceptionPtr	ivibridgeGetSession(
	struct _SIviBridge* pIviBridge,
	const char*			name,
	ViSession*	vi
)
{
	SElExceptionPtr     pexception = NULL;
	SIVIStorageEntry	entry;
	SIVIStorageEntryPtr	pentry = (SIVIStorageEntryPtr) pIviBridge->_pStorage;	
	
	strcpy( entry.name , name);
	pentry = (SIVIStorageEntryPtr) bsearch( &entry,
											pIviBridge->_pStorage,
											pIviBridge->_used,
											sizeof(SIVIStorageEntry),
											CompareIviStorageEntry);
	if ( pentry == NULL )
	{
		char	message[256] = "";
		sprintf(message, "Unknown IVI session name: '%s'", name);
		EXCTHROW( IVIBRIDGE_ERROR_INVALID_SESSION_NAME, message);
	}
	*vi = pentry->vi;	

Error:
	EXCRETHROW( pexception);	
} /* ivibridgeGetSession */


#undef __FUNC__
#define __FUNC__ "InitializeDrivers"
static SElExceptionPtr InitializeDrivers(
	struct _SIviBridge*	pIVIBridge
)
{
	ViStatus	error = VI_SUCCESS;
	SElExceptionPtr     pexception = NULL;
	SIVIStorageEntryPtr	pentry = (SIVIStorageEntryPtr) pIVIBridge->_pStorage;
	int32_t		i;
	ViChar		message1[256] = "";
	
	for( i = 0; i < pIVIBridge->_used; i++, pentry++)
	{
		switch ( pentry->classType)
		{
			case E_DCPOWER:
				error = IviDCPwr_init( pentry->name, VI_TRUE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) IviDCPwr_error_message( pentry->vi, error, message1);
				break;
			case E_SCOPE:
				error = IviScope_init( pentry->name, VI_TRUE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) IviScope_error_message( pentry->vi, error, message1);
				break;
			case E_SWITCH:
				error = IviSwtch_init( pentry->name, VI_TRUE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) 
					IviSwtch_error_message( pentry->vi, error, message1);
				else
					IviSwtch_DisconnectAll( pentry->vi);
				break;
			case E_DMM:
				error = IviDmm_init( pentry->name, VI_TRUE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) IviDmm_error_message( pentry->vi, error, message1);
				break;
			case E_PWRMETER:
				error = IviPwrMeter_init( pentry->name, VI_TRUE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) IviPwrMeter_error_message( pentry->vi, error, message1);
				break;
			case E_FGEN:
				error = IviFgen_init( pentry->name, VI_TRUE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) IviFgen_error_message( pentry->vi, error, message1);
				break;
			case E_RFSIGGEN:
				error = IviRFSigGen_init( pentry->name, VI_TRUE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) IviRFSigGen_error_message( pentry->vi, error, message1);
				break;
			case E_SPECAN:
				error = IviSpecAn_init( pentry->name, VI_TRUE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) IviSpecAn_error_message( pentry->vi, error, message1);
				break;
			case E_RESISTOR:
				error = Resistor_init( pentry->name, VI_FALSE, VI_TRUE, &(pentry->vi));
				if ( error < VI_SUCCESS ) Resistor_error_message( pentry->vi, error, message1);
				break;
			case E_ELDAQ:
				error = eldaq_init( pentry->name, VI_FALSE, VI_FALSE, &(pentry->vi));
				if ( error < VI_SUCCESS ) eldaq_error_message( pentry->vi, error, message1);
				break;
			default:
				EXCTHROW( IVIBRIDGE_ERORR_UNKNOWN_CLASS_DRIVER, "Unknown IVI Class");
				break;
		}
		checkErr( error);
	}

Error:
	if ( error < 0 )
	{
		char message[1024] = "";
		sprintf( message, "IVI Driver Initialization fails \"%s\".\n%s", (pentry)? pentry->name:"none", message1);
		error = 0;
		EXCTHROW( IVIBRIDGE_ERORR_INIT, message);
	}
	EXCRETHROW( pexception);
} /* InitializeDrivers */
	
	
	
	
#undef  __FUNC__
#define __FUNC__ "GetEntryFromString"
static int32_t	GetEntryFromString( const char* string)
{
	int i = 0;
	for ( ; gs_DriverTypes[i].name; i++)
	{
		if ( !strcmp(gs_DriverTypes[i].name, string) )
		{
			return gs_DriverTypes[i].value;
		}
	}	
	
	return (~0);
} /* GetEntryFromString */

#undef  __FUNC__
#define __FUNC__ "GetEntryFromString"
static const char*	GetEntryFromValue( int32_t value)
{
	int i = 0;
	for ( ; gs_DriverTypes[i].name; i++)
	{
		if ( gs_DriverTypes[i].value == value)
		{
			return gs_DriverTypes[i].name;
		}
	}	
	
	return NULL;
}

#undef  __FUNC__
#define __FUNC__ "CompareIviStorageEntry"
static int CompareIviStorageEntry( const void* a, const void* b )
{
	return strcmp( ((SIVIStorageEntryPtr)a)->name, ((SIVIStorageEntryPtr)b)->name);	
}
