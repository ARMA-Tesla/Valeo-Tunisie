#include "eltest.h"
#include <formatio.h>
#include <tcpsupp.h>
#include <userint.h>
#include <userint.h>
#include <userint.h>
#include <userint.h>
#include <userint.h>
#include <exception/exception.h>
#include <elstring.h>
#include <language/language.h>
#include "toolbox.h"

#pragma warning( push)
#pragma warning( disable: 4996)

static SElExceptionPtr SetFunctionPtr(
    struct _STest* pTest,
    int32_t  fceType,
	const char* fceName,
    TestFce_t pFce
);
static SElExceptionPtr QueryParameter(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	char*		buffer,
	int			size
);
static SElExceptionPtr TestRun(
    struct _STest* pTest,
    STestParamPtr pParams,
    HANDLE* pHandle,
	bool_t* used
);
static SElExceptionPtr CloneSelf(
    	struct _STest* pTest,
    	struct _STest** pTestClone 
);

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "SaveXmlResultFileCopy"
void SaveXmlResultFileCopy(STestParamPtr pTestParam, int step, int index, bool_t failed, char* pathCopy)
{
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	char			pfile_xml[256];
	VARIANT			pfile_name_var = CA_VariantEmpty();
	CAObjHandle 	pTicketCopy = 0;
	char			path[256] ="", buffer[256]="";
	CAObjHandle		pnode = 0;
   	int32_t			error = 0;

	
	pTicketCopy = pTestParam->pTicket;
		
	if(pTestParam && pTicketCopy)
	{
		sprintf(pfile_xml, "%s\\%s_%s.xml",
					pTestParam->TicketDirPath,
					"RETEST",
					pTestParam->Time);

		sprintf(pathCopy,pfile_xml);
		EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, pfile_xml));
		EXCCHECKCOM( MSXML_IXMLDOMDocumentsave (pTicketCopy, NULL, pfile_name_var));
		//EXCCHECKCOM( CA_DiscardObjHandle(pTicketCopy));
		//pTicketCopy = 0;
	}

Error:
	//CA_VariantClear(&pfile_name_var);
	EXCDISPLAY(pexception);
}

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "SaveXmlResultFileFailed"
void SaveXmlResultFileFailed(char* pathCopy, int step, int index, char* name, char* value)
{
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	char			pfile_xml[256];
	VARIANT			pfile_name_var = CA_VariantEmpty();
	char			path[256] ="", buffer[256]="";
	CAObjHandle		pnode = 0;
   	int32_t			error = 0;
	CAObjHandle    	pxml = 0; 
   	VBOOL          	is_xml_success = 1;
	CAObjHandle		plog = 0;
	CAObjHandle		plog_new = 0;
	char*         	ptext = NULL;  
   	SELStringPtr	pelstring = NULL;

	//open from copy xml
	MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml);
	CA_VariantSetCString(&pfile_name_var, pathCopy);
	MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success);
	
	//save Failed = 1 
	sprintf(path, "/testseq/product/failed");
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, path, &pnode));
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
	
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}

		sprintf(path, "/testseq/test[@step='%d']/failed", step); 
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode(pxml, NULL, path, &pnode)); 
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
	
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}
	
		sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/failed", step, index); 
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, path, &pnode)); 
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
	
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}
		
		
	//save log 
	if( name && *name && value && *value)
	{
		sprintf(path, "/testseq/test[@step='%d']/%s", step, name);
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode(pxml, NULL, path, &pnode)); 

		if(pnode==0)
		{
			sprintf(path, "/testseq/test[@step='%d']", step);
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode(pxml, NULL, path, &pnode));   
			
			/* create new */
			if(pnode)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (pxml, NULL, name, &plog));
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( plog, NULL, value)); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, plog, &plog_new));

				EXCCHECKCVI( CA_DiscardObjHandle(pnode));
				pnode = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(plog_new));
				plog_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(plog));
				plog = 0;
			}
		}
		else
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
			EXCCHECK( elstring_new(ptext, &pelstring));
			EXCCHECK( pelstring->Append(pelstring, "\n"));
			EXCCHECK( pelstring->Append(pelstring, value));
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, pelstring->GetPtr(pelstring)));

			CA_FreeMemory(ptext);
			ptext=NULL;
		}
	}
			

	CA_VariantSetCString(&pfile_name_var, pathCopy);
	MSXML_IXMLDOMDocumentsave (pxml, NULL, pfile_name_var);
	CA_DiscardObjHandle(pxml);
	CA_VariantClear(&pfile_name_var);
	pxml = 0;


Error:
	CA_VariantClear(&pfile_name_var);
	EXCDISPLAY(pexception);
}


/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "test_new"
SElExceptionPtr test_new( STestPtr* pTest)
{
    SElExceptionPtr pexception = NULL;

    *pTest = (STestPtr)calloc(1, sizeof(STestObj));
    EXCCHECKALLOC( *pTest);
	
    (*pTest)->SetFunctionPtr 	= SetFunctionPtr;
    (*pTest)->Run            	= TestRun;
	(*pTest)->CloneSelf   		= CloneSelf; 
	
    (*pTest)->_LoopCount = 1;
    (*pTest)->_IsUsed    = FALSE;    

Error:
    if ( pexception )
    {
        test_delete(pTest);   
    } 
    EXCRETHROW( pexception);
} /* test_new */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "test_delete"
SElExceptionPtr test_delete( STestPtr* pTest)
{
	SElExceptionPtr pexception = NULL;

	if (pTest && *pTest)
	{
        free(*pTest);
        *pTest = NULL;
	} /* testseq_delete */

//Error:
	EXCRETHROW( pexception);
} /* testseq_delete */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STest::SetFunctionPtr"
static SElExceptionPtr SetFunctionPtr(
    struct _STest* pTest,
    int32_t  fceType,
	const char* fceName,
    TestFce_t pFce
)
{
    SElExceptionPtr pexception = NULL;

	if(pFce==NULL)
	{
		char buffer[256];

		sprintf(buffer, "Unknown Export Function Definition: %s", fceName);
		EXCTHROW(TEST_ERR_UNKNOWN_FCE_PTR, buffer);
	}

    switch ( fceType )
    {
        case TEST_FCE_TYPE_HELP:
            pTest->test.Help = pFce;
            break; 
        case TEST_FCE_TYPE_CHECK:
            pTest->test.Check= pFce;
            break;
        case TEST_FCE_TYPE_INIT:
            pTest->test.Init = pFce;
            break;
        case TEST_FCE_TYPE_RUN:
            pTest->test.Run = pFce;
            break;
        case TEST_FCE_TYPE_CLEAN:
            pTest->test.Cleanup = pFce;
            break;
        default:
            EXCTHROW( TEST_ERR_UNKNOWN_FCE_TYPE, "Unknown Function Type");
            break;
    }

Error:
    EXCRETHROW( pexception);
} /* STest::SetFunctionPtr */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STest::Run"
static SElExceptionPtr TestRun(
    struct _STest* pTest,
    STestParamPtr pParams,
    HANDLE* pHandle,
	bool_t* used 
)
{
    SElExceptionPtr pexception = NULL;
	bool_t			is_cleaned = FALSE;
	
	if(used) *used = FALSE;
    
    if ( pTest->_IsUsed )
    {
		pParams->step = pTest->_Step;
		
		if(used) *used = TRUE;
		
        EXCCHECK( pTest->test.Init( pParams));
        EXCCHECK( pTest->test.Run( pParams));
		is_cleaned = TRUE;
        EXCCHECK( pTest->test.Cleanup( pParams));
    }

Error:
#if 0
	if(is_cleaned==FALSE) 
	{
		SElExceptionPtr pexc_tmp = NULL;
		pexc_tmp = pTest->test.Cleanup( pParams);
		EXCDELETE(pexc_tmp);
	}
#endif
    EXCRETHROW( pexception);
} /* STest::Run */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STest::CloneSelf"
static SElExceptionPtr CloneSelf(
    	struct _STest* pTest,
    	struct _STest** pTestClone 
)
{
	SElExceptionPtr pexception = NULL;  	
	
	EXCCHECK( test_new(&(*pTestClone)));
	
	memcpy(*pTestClone, pTest, sizeof(STestObj));
	
Error:
    EXCRETHROW( pexception);
} /* STest::CloneSelf */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "QueryParameter"
static SElExceptionPtr QueryParameter(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	char*		buffer,
	int			size
)
{												 
	HRESULT 		hr;
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	CAObjHandle		pnode = 0;
	char			path[256] ="";
	char*         	ptext = NULL; 

	/* test parameter */
	sprintf(path, "/testseq/test[@step='%d']/parameter[name='%s']/value", step, name);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext));
		
		if(ptext)  
		{
			strncpy(buffer, ptext, size);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;	
	}
	else
	{
		/* global parameter */
		sprintf(path, "/testseq/global/parameter[name='%s']/value", name); 
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
		
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext));

			if(ptext)
			{
				strncpy(buffer, ptext, size);
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}
	}
	
#if 0
	if ( !pnode )
	{
		char	message[512];	
		sprintf( message, "Varaible '%s' is missing in configuration file!", name); 
		EXCTHROW( TEST_ERROR_UNKNOWN_CONFIG_VARIABLE, message);
	}
#endif
	
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "QueryParameterInt32"
SElExceptionPtr QueryParameterInt32(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	int32_t*	pValue
)
{
	SElExceptionPtr pexception = NULL;
	char	buffer[32] = "";
	char*	pend = NULL;

	EXCCHECK( QueryParameter( ppar, step, name, buffer, sizeof(buffer)));
	RemoveSurroundingWhiteSpace(buffer);
	if (pValue && buffer[0]) *pValue = strtol( buffer, &pend, 10);
	
	if ( pend && *pend!='\0' )
	{
		EXCTHROW( EL_ERROR_NOT_NUMERIC_VALUE, "Value is not Numeric");
	}

Error:
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "QueryParameterString"
SElExceptionPtr QueryParameterString(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	char		buffer[],
	int32_t		bufferSize
)
{
	return QueryParameter( ppar, step, name, buffer, bufferSize);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "QueryParameterUInt32"
SElExceptionPtr QueryParameterUInt32(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	uint32_t*	pValue
)
{
	SElExceptionPtr pexception = NULL;
	unsigned long	ul;
	char	buffer[32] = "";
	char*	pend = NULL;
	int		is_hex = 0;

	if ( pValue ) ul = *pValue;

	EXCCHECK( QueryParameter( ppar, step, name, buffer, sizeof(buffer)));
	RemoveSurroundingWhiteSpace(buffer);
	if( buffer && buffer[0]) ul = strtoul( buffer, &pend, 10);

	/*
	 * TODO: Fynction rewrite value when step and name parameters does not exisit
	 */
	if ( pend )
	{
		switch(*pend)
		{
		case	'x':	/* value 0xASX */
		case	'X':
			if ( buffer[0] == '0' && (buffer + 1) == pend )
			{
				ul = strtoul( ++pend, &pend, 16);
			}
			break;
		case	'a':
		case	'A':
		case	'b':
		case	'B':
		case	'c':
		case	'C':
		case	'd':
		case	'D':
		case	'e':
		case	'E':
		case	'f':
		case	'F':
			is_hex = 1;
		case	'h':  /* 56351h */
		case	'H':
			if ( is_hex || ( *(pend+1) == '\0' ))
			{
				ul = strtoul( buffer, &pend, 16);
				if ( pend && (*pend=='h' || *pend=='H') ) pend++;
			}
			break;
		}
	}
	
	if ( pend && *pend!='\0' )
	{
		EXCTHROW( EL_ERROR_NOT_NUMERIC_VALUE, "Value is not unsigned numeric."
											  "Allowed formats 121,0x1A2,412h.");
	}

Error:
	if ( pValue ) *pValue = ul;
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "QueryParameterReal64"
SElExceptionPtr QueryParameterReal64(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	double*	pValue
)
{
	SElExceptionPtr pexception = NULL;
	char	buffer[32] = "";
	char*	pend = NULL;

	EXCCHECK( QueryParameter( ppar, step, name, buffer, sizeof(buffer)));
	RemoveSurroundingWhiteSpace(buffer);
	if (pValue && buffer[0]) *pValue = strtod( buffer, &pend);
	
	if ( pend && *pend!='\0' )
	{
		EXCTHROW( EL_ERROR_NOT_NUMERIC_VALUE, "Value is not Numeric");
	}

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetParameterInt32"
SElExceptionPtr SetParameterInt32(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	int32_t		value
)
{
	SElExceptionPtr pexception = NULL;
	char			buffer[12] = ""; /* -2147483648 */
	sprintf( buffer, "%d", (int)value);

	EXCCHECK( SetParameterString( ppar, step, name, buffer));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetParameterUInt32"
SElExceptionPtr SetParameterUInt32(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	uint32_t		value
)
{
	SElExceptionPtr pexception = NULL;
	char			buffer[12] = ""; /* 4147483648 */
	sprintf( buffer, "%u", (unsigned int)value);
	EXCCHECK( SetParameterString( ppar, step, name, buffer));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetParameterReal64"
SElExceptionPtr SetParameterReal64(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	double		value
)
{
	SElExceptionPtr pexception = NULL;
	char			buffer[32] = ""; /* depends on the number */
	const char*		fmt = "%f";
	
	if ( fabs(value) < 1.0e-3 || fabs(value) > 1.0e10) fmt = "%e";

	sprintf( buffer, fmt, value);
	EXCCHECK( SetParameterString( ppar, step, name, buffer));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetParameterString"
SElExceptionPtr SetParameterString(
	STestParamPtr	ppar,
	int			step,
	const char* name,
	const char* buffer
)
{
	int32_t			error = 0;	
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			path[256] =""; 

	sprintf(path, "/testseq/test[@step='%d']/record[name='%s']/value", step, name);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));  

	if ( pnode ) /* exist */
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, buffer)); 	
	}
	else /* does not exist */
	{
		;
	}
	
Error:
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CreateNewLimit"
SElExceptionPtr CreateNewLimit(
	STestParamPtr	ppar,
	int			step,
	int			index,
	char*		limit_str,
	const char*	description
)
{
	HRESULT 		hr;
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	char			buffer[1024];
	CAObjHandle		pxml = 0; 
	CAObjHandle		pmeas = 0; 
	CAObjHandle		pmeas_new = 0; 
	CAObjHandle		pnode = 0; 
	VBOOL			is_xml_success = 1;
	
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	
	sprintf( buffer, 
			"<measure step=\"%d\">\n"
			"	<config>%s</config>\n"
			"	<value type=\"\"></value>\n"
			"	<failed>0</failed>\n"
			"	<duration></duration>\n"
			"	<unit></unit>\n"
			"	<used>0</used>\n"
			"	<limit>\n"
			"		<id>0</id>\n"
			"		<name>Limit</name>\n"
			"		<value1>%s</value1>\n"
			"		<value2></value2>\n"
			"	</limit>\n"
			"</measure>\n",
			index,
			description,
			limit_str);

	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (pxml, NULL, buffer, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "/measure", &pmeas));

	sprintf(buffer, "/testseq/test[@step='%d']", step);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, buffer, &pnode)); 
	EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, pmeas, &pmeas_new));

Error:
	CA_DiscardObjHandle(pxml);  
	CA_DiscardObjHandle(pmeas); 
	CA_DiscardObjHandle(pmeas_new); 
	CA_DiscardObjHandle(pnode);  
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CreateNewLimitEx"
SElExceptionPtr CreateNewLimitEx(
	STestParamPtr	ppar,
	int			step,
	int			index,
	char*		min,
	char*		max,
	const char*	description
)
{
	HRESULT 		hr;
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	char			buffer[1024];
	CAObjHandle		pxml = 0; 
	CAObjHandle		pmeas = 0; 
	CAObjHandle		pmeas_new = 0; 
	CAObjHandle		pnode = 0; 
	VBOOL			is_xml_success = 1;
	
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	
	sprintf( buffer, 
			"<measure step=\"%d\">\n"
			"	<config>%s</config>\n"
			"	<value type=\"\"></value>\n"
			"	<failed>0</failed>\n"
			"	<duration></duration>\n"
			"	<unit></unit>\n"
			"	<used>0</used>\n"
			"	<limit>\n"
			"		<id>0</id>\n"
			"		<name>Limit</name>\n"
			"		<value1>%s</value1>\n"
			"		<value2>%s</value2>\n"
			"	</limit>\n"
			"</measure>\n",
			index,
			description,
			min, max);

	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (pxml, NULL, buffer, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "/measure", &pmeas));

	sprintf(buffer, "/testseq/test[@step='%d']", step);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, buffer, &pnode)); 
	EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, pmeas, &pmeas_new));

Error:
	CA_DiscardObjHandle(pxml);  
	CA_DiscardObjHandle(pmeas); 
	CA_DiscardObjHandle(pmeas_new); 
	CA_DiscardObjHandle(pnode);  
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetLimitConfiguration"
SElExceptionPtr GetLimitConfiguration(
	STestParamPtr	ppar,
	int			step,
	int32_t		index,
	char		buffer[],
	int32_t		bufferSize
)
{
	HRESULT 		hr;
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	CAObjHandle		pnode = 0;
	char			path[256] ="";
	char*         	ptext = NULL;

	memset(buffer, 0, bufferSize*sizeof(char));

	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/config", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));    
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
		if(ptext && *ptext)
		{
			strncpy(buffer, ptext, bufferSize);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
	
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LimitExist"
bool_t LimitExist(
	STestParamPtr	ppar,
	int			step,
	int32_t		index
)
{
	HRESULT 		hr;
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	CAObjHandle		pnode = 0;
	char			path[256] ="";

	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));    
	if(pnode)
	{
		CA_DiscardObjHandle(pnode);
		return TRUE;
	}
	
Error:  
	return FALSE;
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckLimitSTRING"
SElExceptionPtr CheckLimitSTRING(
	STestParamPtr	ppar,
	int			step,
	int			index,
	char*		value
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			path[256] ="", limit[256]="";
	char*         	ptext = NULL; 
	int32_t			min = 0, max = 0;
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/used", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/value", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, value));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/value/@type", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "string"));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/limit/value1", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));    
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
		strncpy(limit, ptext, 255);
	
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

	/* check range */
	if(0!=strncmp(value, limit, strlen(limit)))
	{
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
	}
	
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW_NO_DISP( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckLimitINT32"
SElExceptionPtr CheckLimitINT32(
	STestParamPtr	ppar,
	int			step,
	int			index, 
	int			value
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			path[256] ="", buffer[256]="";
	char*         	ptext = NULL; 
	int32_t			min = 0, max = 0;
	char 			sValeurCSV[128];

	if (ppar->Test_RnR == TRUE)
		{
		sprintf (sValeurCSV, "%d", value);
		strcat(ppar->line_CSV, sValeurCSV);
		strcat(ppar->line_CSV, ";");
		}
		
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/used", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/value", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		sprintf(buffer, "%d", value);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, buffer));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/value/@type", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "int32"));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/limit/value1", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));    
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 

		min = strtol(ptext, NULL, 10);
	
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/limit/value2", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));  

	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext));   

		max = strtol(ptext, NULL, 10);
	
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

	/* check range */
	if(value<min || value>max)
	{
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
	}
	
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW_NO_DISP( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckLimitUINT32"
SElExceptionPtr CheckLimitUINT32(
	STestParamPtr	ppar,
	int			step,
	int			index, 
	uint32_t	value
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			path[256] ="", buffer[256]="";
	char*         	ptext = NULL; 
	uint32_t		min = 0, max = 0;
	bool_t			is_max = FALSE;
	char 			sValeurCSV[128];

#if 0    /* formerly excluded lines */
	if (ppar->Test_RnR == TRUE)
		{
		sprintf (sValeurCSV, "%d", value);
		strcat(ppar->line_CSV, sValeurCSV);
		strcat(ppar->line_CSV, ";");
		}
#endif   /* formerly excluded lines */
		
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/used", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/value", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		sprintf(buffer, "%d", value);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, buffer));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/value/@type", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "int32")); /* TODO: define uint32 in xsl template */
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/limit/value1", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));    
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 

		min = strtoul(ptext, NULL, 10);
	
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/limit/value2", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));  

	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext));   

		if(ptext && ptext[0])
		{
			max = strtoul(ptext, NULL, 10);
			is_max = TRUE;
		}
	
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

	/* check range */
	if(is_max)
	{
		if(min<=max && (value<min || value>max))
		{
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
		else if(min>max && (value<min && value>max))
		{
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
		}
	}
	else
	{
		if(value!=min)
			EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
	}

Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW_NO_DISP( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckLimitREAL64"
SElExceptionPtr CheckLimitREAL64(
	STestParamPtr	ppar,
	int			step,
	int			index, 
	double		value
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			path[256] ="", buffer[256]="";
	char            sValeurCSV[256]="";
	char*         	ptext = NULL; 
	double			min = 0, max = 0;
	char msg[128];
    char sValeur[128];
    int  valeur1, valeur2;
	
	//if(value<0.0) value *= -1.0;
	if (ppar->Test_RnR == TRUE)
		{
		sprintf (sValeurCSV, "%.6f", value);
		strcat(ppar->line_CSV, sValeurCSV);
		strcat(ppar->line_CSV, ";");
		}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/used", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/value", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		
		sprintf(buffer, "%.6f", value);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, buffer));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/value/@type", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "real64"));
		
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/limit/value1", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));    
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 

		min = strtod(ptext, NULL);
	
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}

	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/limit/value2", step, index);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));  

	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext));   

		max = strtod(ptext, NULL);
	
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
		CA_FreeMemory(ptext);
		ptext=NULL;
	}


	/* check range */
	if(value<min || value>max)
	{
		EXCTHROW(TEST_ERR_VALUE_OUT_OF_LIMIT, "TEST_ERR_VALUE_OUT_OF_LIMIT");
	}
	
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW_NO_DISP( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestSequenceResult"
SElExceptionPtr SetTestSequenceResult(
	STestParamPtr	ppar,
	int					mode,
	SElExceptionPtr		pException
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	CAObjHandle		perror = 0;
	CAObjHandle		perror_new = 0;
	CAObjHandle		perror_sub = 0;
	CAObjHandle		perror_sub_new = 0;
	char			path[256] = "";
	char*         	ptext = NULL;  
	int32_t			value = 0;
	char			svalue[32] = "";
	const char*		svalue_ptr = NULL;

	if(ppar->pTicket && pException!=NULL)
	{
		sprintf(path, "/testseq/product/failed");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
	
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}

		if(!mode)
			sprintf(path, "/testseq/testseq_init/failed"); 
		else
			sprintf(path, "/testseq/testseq_cleanup/failed"); 

		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode)); 
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
	
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}

		/* log Error */
		if(!mode)
			sprintf(path, "/testseq/testseq_init"); 
		else
			sprintf(path, "/testseq/testseq_cleanup"); 

		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));   		
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, "Error", &perror));

			/* Code */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, "Code", &perror_sub));
			if(perror_sub)
			{
				sprintf(svalue, "%d", pException->GetErrorCode(pException));
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( perror_sub, NULL, svalue)); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (perror, NULL, perror_sub, &perror_sub_new));

				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub_new));
				perror_sub_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub));
				perror_sub = 0;
			}

			/* CallPath */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, "CallPath", &perror_sub));
			if(perror_sub)
			{
				svalue_ptr = pException->GetCallPathPtr(pException);
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( perror_sub, NULL, (svalue_ptr)? svalue_ptr:"")); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (perror, NULL, perror_sub, &perror_sub_new));

				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub_new));
				perror_sub_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub));
				perror_sub = 0;
			}

			/* Msg */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, "Msg", &perror_sub));
			if(perror_sub)
			{
				svalue_ptr = pException->GetErrorMsgPtr(pException);
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( perror_sub, NULL, (svalue_ptr)? svalue_ptr:"")); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (perror, NULL, perror_sub, &perror_sub_new));

				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub_new));
				perror_sub_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub));
				perror_sub = 0;
			}

			EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, perror, &perror_new));

			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
			EXCCHECKCVI( CA_DiscardObjHandle(perror_new));
			perror_new = 0;
			EXCCHECKCVI( CA_DiscardObjHandle(perror));
			perror = 0;
		}
	}

Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetTestResult"
SElExceptionPtr GetTestResult(
	STestParamPtr	ppar,
	int32_t*		result
)
{
	int32_t			error = 0,
					test_failed = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char*         	ptext = NULL;
	char			path[256] = "";
	
	if(ppar->pTicket)
	{
		sprintf(path, "/testseq/product/failed");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext));
			if (ptext != NULL)
			{
				test_failed = atoi(ptext);
				CA_FreeMemory(ptext);
				ptext = NULL;
			}
			
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}
	}
	
Error:
	if (result)
		*result = test_failed;

	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestResult"
SElExceptionPtr SetTestResult(
	STestParamPtr	ppar,
	int			step,
	int			index, 
	SElExceptionPtr	pException
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	CAObjHandle		perror = 0;
	CAObjHandle		perror_new = 0;
	CAObjHandle		perror_sub = 0;
	CAObjHandle		perror_sub_new = 0;
	char			path[256] = "";
	char*         	ptext = NULL;
	int32_t			value = 0;
	char			svalue[32] = "";
	const char*		svalue_ptr = NULL;
	
	if(ppar->pTicket && pException!=NULL)
	{
		sprintf(path, "/testseq/product/failed");
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
	
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}

		sprintf(path, "/testseq/test[@step='%d']/failed", step); 
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode)); 
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
	
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}
	
		sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/failed", step, index); 
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode)); 
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
	
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}

		/* log Error */
		sprintf(path, "/testseq/test[@step='%d']", step); 
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));   		
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, "Error", &perror));

			/* Code */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, "Code", &perror_sub));
			if(perror_sub)
			{
				sprintf(svalue, "%d", pException->GetErrorCode(pException));
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( perror_sub, NULL, svalue)); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (perror, NULL, perror_sub, &perror_sub_new));

				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub_new));
				perror_sub_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub));
				perror_sub = 0;
			}

			/* CallPath */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, "CallPath", &perror_sub));
			if(perror_sub)
			{
				svalue_ptr = pException->GetCallPathPtr(pException);
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( perror_sub, NULL, (svalue_ptr)? svalue_ptr:"")); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (perror, NULL, perror_sub, &perror_sub_new));

				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub_new));
				perror_sub_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub));
				perror_sub = 0;
			}

			/* Msg */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, "Msg", &perror_sub));
			if(perror_sub)
			{
				svalue_ptr = pException->GetErrorMsgPtr(pException);
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( perror_sub, NULL, (svalue_ptr)? svalue_ptr:"")); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (perror, NULL, perror_sub, &perror_sub_new));

				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub_new));
				perror_sub_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(perror_sub));
				perror_sub = 0;
			}

			EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, perror, &perror_new));

			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
			EXCCHECKCVI( CA_DiscardObjHandle(perror_new));
			perror_new = 0;
			EXCCHECKCVI( CA_DiscardObjHandle(perror));
			perror = 0;
		}
	}
	
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestUsed"
SElExceptionPtr SetTestUsed(
	STestParamPtr	ppar,
	int			step
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			path[256] ="";

	if(ppar->pTicket)
	{
		sprintf(path, "/testseq/test[@step='%d']/used", step);
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "1"));
			
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}
	}
	
Error:
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestDuration"
SElExceptionPtr SetTestDuration(
	STestParamPtr	ppar,
	int			step,
	double 		duration
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			sduration[256] = ""; 
	char			path[256] =""; 
	
	if(ppar->pTicket)
	{
		sprintf(path, "/testseq/test[@step='%d']/duration", step);  
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
		if(pnode)
		{
			sprintf(sduration, "%4.2f", duration);
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, sduration));

			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
		}
	}
		
Error:
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}	

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetSubTestDuration"
SElExceptionPtr SetSubTestDuration(
	STestParamPtr	ppar,
	int			step,
	int			index,
	double 		duration
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			sduration[256] = ""; 
	char			path[256] =""; 
	
	sprintf(path, "/testseq/test[@step='%d']/measure[@step='%d']/duration", step, index);  
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		sprintf(sduration, "%4.2f", duration);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, sduration));

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
		
Error:
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}	

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestSequenceDuration"
SElExceptionPtr SetTestSequenceDuration(
	STestParamPtr	ppar,
	int			mode,
	double 		duration
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			sduration[256] = "";
	char*         	ptext = NULL;  
	char			path[128];
	
	if(!mode)
		sprintf(path, "/testseq/testseq_init/duration"); 
	else
		sprintf(path, "/testseq/testseq_cleanup/duration"); 

	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));
	if(pnode)
	{
		sprintf(sduration, "%4.2f", duration);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, sduration));

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
		
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}	

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestSequenceTime"
SElExceptionPtr SetTestSequenceTime(
	STestParamPtr	ppar,
	double duration 
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			sduration[256] = "";
	char*         	ptext = NULL;  
	
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, "/testseq/product/duration", &pnode));
	if(pnode)
	{
		sprintf(sduration, "%4.2f", duration);
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, sduration));

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
		
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}	

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetTestSequenceBarcode"
SElExceptionPtr GetTestSequenceBarcode(
	STestParamPtr	ppar,
	char* barcode,
	uint32_t barcode_size
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char*         	ptext = NULL;
	
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, "/testseq/product/barcode", &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext));
		if(ptext)
		{
			strncpy(barcode, ptext, barcode_size);
		}
		
		CA_FreeMemory(ptext);
		ptext=NULL;
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
		
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}	

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetTestDescription"
SElExceptionPtr GetTestDescription(
	STestParamPtr	ppar,
	int			step,
	char*		desc,
	uint32_t	descSize
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	char			path[256] ="";
	char*         	ptext = NULL; 

	if( ppar->pTicket)
	{
		sprintf(path, "/testseq/test[@step='%d']/name", step);
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));   
		
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
			if(ptext)
			{
				strcpy(desc, ptext);
			}
				
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		sprintf(path, "/testseq/test[@step='%d']/description", step);
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));   
		
		if(pnode)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
			if(ptext)
			{
				strcat(desc, " - ");
				strncat(desc, gettext(ptext), descSize - strlen(desc) - strlen(" - "));
			}
				
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
	}
	
Error:
	CA_FreeMemory(ptext);     
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestValue"
SElExceptionPtr SetTestValue(
	STestParamPtr	ppar,
	int					step,
	const char*			name,
	const char*			value
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	CAObjHandle		plog = 0;
	CAObjHandle		plog_new = 0;
	char			path[256] =""; 
	char*			ptext = NULL;
	SELStringPtr	pelstring = NULL;

	if( name && *name && value && *value)
	{
		sprintf(path, "/testseq/test[@step='%d']/%s", step, name);
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode)); 

		if(pnode==0)
		{
			sprintf(path, "/testseq/test[@step='%d']", step);
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));   
			
			/* create new */
			if(pnode)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, name, &plog));
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( plog, NULL, value)); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, plog, &plog_new));

				EXCCHECKCVI( CA_DiscardObjHandle(pnode));
				pnode = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(plog_new));
				plog_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(plog));
				plog = 0;
			}
		}
		else
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
			EXCCHECK( elstring_new(ptext, &pelstring));
			EXCCHECK( pelstring->Append(pelstring, "\n"));
			EXCCHECK( pelstring->Append(pelstring, value));
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, pelstring->GetPtr(pelstring)));

			CA_FreeMemory(ptext);
			ptext=NULL;
		}
	}
	
Error:     
	elstring_delete(&pelstring);
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTestSequenceValue"
SElExceptionPtr SetTestSequenceValue(
	STestParamPtr	ppar,
	int					mode,
	const char*			name,
	const char*			value
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;   
	CAObjHandle		pnode = 0;
	CAObjHandle		plog = 0;
	CAObjHandle		plog_new = 0;
	char			path[256] =""; 
	char*			ptext = NULL;
	SELStringPtr	pelstring = NULL;

	if( name && *name && value && *value)
	{
		if(!mode)
			sprintf(path, "/testseq/testseq_init/%s", name);	
		else
			sprintf(path, "/testseq/testseq_cleanup/%s", name);

		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode)); 

		if(pnode==0)
		{
			if(!mode)
				sprintf(path, "/testseq/testseq_init");	
			else
				sprintf(path, "/testseq/testseq_cleanup");	

			EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, path, &pnode));   
			
			/* create new */
			if(pnode)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (ppar->pTicket, NULL, name, &plog));
				EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( plog, NULL, value)); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pnode, NULL, plog, &plog_new));

				EXCCHECKCVI( CA_DiscardObjHandle(pnode));
				pnode = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(plog_new));
				plog_new = 0;
				EXCCHECKCVI( CA_DiscardObjHandle(plog));
				plog = 0;
			}
		}
		else
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
			EXCCHECK( elstring_new(ptext, &pelstring));
			EXCCHECK( pelstring->Append(pelstring, "\n"));
			EXCCHECK( pelstring->Append(pelstring, value));
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, pelstring->GetPtr(pelstring)));

			CA_FreeMemory(ptext);
			ptext=NULL;
		}
	}
	
Error:     
	elstring_delete(&pelstring);
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Get_Parameter_Variant_Code"
SElExceptionPtr Get_Parameter_Variant_Code(
	STestParamPtr		ppar,
	int					VariantCode
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	CAObjHandle		pnode = 0;
	HRESULT			hr;
	char			*ptext = NULL;
	SCameraPtr		pcamera = NULL;

	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, "/testseq/global/parameter[name='Variant Code']/value", &pnode));
	if(pnode==0)
	EXCTHROW(TEST_ERROR_CAMERA_SET_PARAMETER, "Parameter Variant Code not found!");
	

	EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
	if(ptext && *ptext)
	{
	VariantCode = atoi(ptext);
	CA_FreeMemory(ptext);
	ptext = NULL;
	}

Error: 
	pcamera->Cognex_disconnect(pcamera);
	CA_FreeMemory(ptext);
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CameraSetJob"
SElExceptionPtr CameraSetJob(
	STestParamPtr		ppar,
	SCamerasPtr			pcameras
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	CAObjHandle		pnode = 0;
	HRESULT			hr;
	char			*ptext = NULL;
	SCameraPtr		pcamera = NULL;

	EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( ppar->pTicket, NULL, "/testseq/global/parameter[name='JobCamera']/value", &pnode));
	if(pnode==0)
	EXCTHROW(TEST_ERROR_CAMERA_SET_PARAMETER, "Parameter JobCamera not found!");
	

	EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
	if(ptext && *ptext)
	{
	EXCCHECK( pcamera->Cognex_disconnect(pcamera));
	EXCCHECK( pcamera->openTCPserver(pcamera));
	EXCCHECK( pcamera->Cognex_login(pcamera));
	EXCCHECK( pcamera->Cognex_Offline(pcamera));
	EXCCHECK( pcamera->Cognex_Job(pcamera,ptext));
	EXCCHECK( pcamera->Cognex_Online(pcamera));
	Sleep (3800);
	CA_FreeMemory(ptext);
	ptext = NULL;
	}

Error: 
	pcamera->Cognex_disconnect(pcamera);
	CA_FreeMemory(ptext);
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CameraSetLiveOn"
SElExceptionPtr CameraSetLiveOn(
	STestParamPtr		ppar,
	SCamerasPtr			pcameras
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	CAObjHandle		pnode = 0;
	HRESULT			hr;
	char			*ptext = NULL;
	SCameraPtr		pcamera = NULL;
	
	EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
	
	EXCCHECK( pcamera->Cognex_disconnect(pcamera));
	Sleep(100);
	EXCCHECK( pcamera->openTCPserver(pcamera));
	Sleep(100);
	EXCCHECK( pcamera->Cognex_set_live(pcamera, 1));
	Sleep(100);

Error: 
	CA_FreeMemory(ptext);
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CameraSetLiveOff"
SElExceptionPtr CameraSetLiveOff(
	STestParamPtr		ppar,
	SCamerasPtr			pcameras
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	CAObjHandle		pnode = 0;
	HRESULT			hr;
	char			*ptext = NULL;
	SCameraPtr		pcamera = NULL;
	
	
	EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
	
	EXCCHECK( pcamera->Cognex_disconnect(pcamera));
	Sleep(100);
	EXCCHECK( pcamera->openTCPserver(pcamera));
	Sleep(100);
	EXCCHECK( pcamera->Cognex_login(pcamera));
	Sleep(100);
	EXCCHECK( pcamera->Cognex_Offline(pcamera));
	Sleep(100);
	EXCCHECK( pcamera->Cognex_set_live(pcamera, FALSE));
	ProcessSystemEvents ();
	ProcessDrawEvents ();
	Sleep(100);
   	EXCCHECK( pcamera->Cognex_Online(pcamera));

Error: 
	CA_FreeMemory(ptext);
	CA_DiscardObjHandle(pnode);
	EXCRETHROW( pexception);	
}

#undef __FUNC__
#define __FUNC__ "CameraSetParam"
SElExceptionPtr CameraSetParam(
	SCamerasPtr			pcameras,
	int32_t 			step,
	int32_t				timeout
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	CAObjHandle		pnode = 0;
	HRESULT			hr;
	char			*ptext = NULL;
	SCameraPtr		pcamera = NULL;
	clock_t _start_test_clock;
	double   time;
	
	EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
	EXCCHECK( pcamera->Cognex_set_param(pcamera, step));
 	Sleep(300);

Error: 
	EXCRETHROW( pexception);	
}

#undef __FUNC__
#define __FUNC__ "CameraGetdata"
SElExceptionPtr CameraGetdata(
	SCamerasPtr			pcameras,
	int32_t 			step,
	int32_t				timeout,
	char				*data
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	CAObjHandle		pnode = 0;
	HRESULT			hr;
	char			*ptext = NULL;
	SCameraPtr		pcamera = NULL;
	clock_t _start_test_clock;
	double   time;

	  	EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
		EXCCHECK( pcamera->openTCPserver(pcamera));
		DelayWithEventProcessing (0.01);
		ProcessTCPEvents ();
			
		_start_test_clock = clock();
		do{
			
			EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
			EXCCHECK( pcamera->Cognex_Trigger(pcamera));
			DelayWithEventProcessing (0.6);	 // old =0.15
			ProcessTCPEvents ();
			
			EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));
			DelayWithEventProcessing (0.01);	// old= 0.01
			ProcessTCPEvents ();
			EXCCHECK( pcamera->Cognex_Read(pcamera, data));
	//		DelayWithEventProcessing (0.1);
			time = (double)(clock()-_start_test_clock);
		
		}while ((data[17] != 48+step) && (time < timeout));

		if (data[17] != 48+step)
		{
			EXCTHROW(TEST_ERROR_CAMERA_RESPONSE_TIMEOUT, "Camera Response Timeout!");
		}
Error: 
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#if 0    /* formerly excluded lines */
#undef __FUNC__
#define __FUNC__ "CameraGetdata"
SElExceptionPtr CameraGetdata(
	SCamerasPtr			pcameras,
	int32_t 			step,
	int32_t				timeout,
	char				*data
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	CAObjHandle		pnode = 0;
	HRESULT			hr;
	char			*ptext = NULL;
	SCameraPtr		pcamera = NULL;
	clock_t _start_test_clock;
	double   time;
	
	
	_start_test_clock =  clock();  
	
		EXCCHECK( pcameras->GetCameraByIndex(pcameras, 0, &pcamera));

 
		EXCCHECK( pcamera->Cognex_disconnect(pcamera));
		EXCCHECK( pcamera->openTCPserver(pcamera));
		EXCCHECK( pcamera->Cognex_login(pcamera));
		EXCCHECK( pcamera->Cognex_set_param(pcamera, step));
		Sleep(150);
		EXCCHECK( pcamera->Cognex_Trigger(pcamera));
	    Sleep(250);
	
		_start_test_clock =  clock();
		do{
		EXCCHECK( pcamera->Cognex_Read(pcamera, data));
		time = (double)(clock()-_start_test_clock);
	//	if (FindPattern (data, 0, -1, "User: Password:", 0, 0) >= 0)
	//	{
		EXCCHECK( pcamera->Cognex_disconnect(pcamera));
		time = (double)(clock()-_start_test_clock);
		EXCCHECK( pcamera->openTCPserver(pcamera));	
		time = (double)(clock()-_start_test_clock);
		EXCCHECK( pcamera->Cognex_login(pcamera));
		time = (double)(clock()-_start_test_clock);
		EXCCHECK( pcamera->Cognex_set_param(pcamera, step));
		Sleep(150);
		time = (double)(clock()-_start_test_clock);
		EXCCHECK( pcamera->Cognex_Trigger(pcamera));
	    Sleep(250);
		time = (double)(clock()-_start_test_clock);
	//	}
	
		//ProcessSystemEvents ();
		DelayWithEventProcessing (0.2);
		//ProcessDrawEvents ();
		ProcessTCPEvents ();
		time = (double)(clock()-_start_test_clock);
//		printf ("END COGNEX READ : time = %f\r\n", time);
		}while ((data[17] != 48+step) && (time < timeout));

		if (data[17] != 48+step)
		{
			EXCTHROW(TEST_ERROR_CAMERA_RESPONSE_TIMEOUT, "Camera Response Timeout!");
		}

Error: 
	EXCRETHROW( pexception);	
}
#endif   /* formerly excluded lines */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetResultREAL64formXMLString"
SElExceptionPtr GetResultREAL64formXMLString(
	SELStringPtr	result, 
	const char*		path, 
	double*			value
)
{
	SElExceptionPtr pexception = NULL;
	char			buffer[256] = "";
	
	if(value) *value = 0.0;

	EXCCHECK( GetResultSTRINGformXMLString(result, path, buffer));

	if(value) *value = strtod(buffer, NULL);

Error:
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetResultSTRINGformXMLString"
SElExceptionPtr GetResultSTRINGformXMLString(
	SELStringPtr		result, 
	const char*			path,
	char*				buffer
)
{
	SElExceptionPtr pexception = NULL;
	HRESULT 		hr;
	int32_t			error = 0;
	CAObjHandle		pxml = 0;
	CAObjHandle		pnode = 0;
	char*         	ptext = NULL;
	VBOOL         	is_xml_success;

	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentloadXML (pxml, NULL, result->GetPtr(result), &is_xml_success));	
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, path, &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &ptext)); 
		if(ptext && *ptext)
		{
			strcpy(buffer, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}

Error:
	if(ptext) CA_FreeMemory(ptext);
	if(pnode) CA_DiscardObjHandle(pnode);
	if(pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);	
}

#pragma warning( pop)
