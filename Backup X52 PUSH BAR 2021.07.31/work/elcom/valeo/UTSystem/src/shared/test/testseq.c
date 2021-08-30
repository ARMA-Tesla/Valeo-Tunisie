#include <test/testseq.h>
#include <formatio.h>
#include <userint.h>
#include <exception/Exception.h>
#include <elstring.h>
#include <stdio.h>
  
#define MAX_RETEST  2

extern HWND		g_hWnd;

static SElExceptionPtr Load(
        struct _STestSequence* pTestSeq,
        const char* path,
		SObjListPtr pModules,
		SElEventFactoryPtr pEventFactory
);
static SElExceptionPtr IniPanel(
        struct _STestSequence* me,
		const char* panelPicture,
		bool_t reset
);
static SElExceptionPtr SetPanelDescription(
        struct _STestSequence* me,
        int32_t     control,
		const char* description
);
static SElExceptionPtr SetPanelWarning(
        struct _STestSequence* me,
        int32_t     control,
		bool_t status
);
static SElExceptionPtr SetPanelStationStatus(
        struct _STestSequence* me,
        int32_t     station,
		bool_t status
);
static SElExceptionPtr SetTestPanelReferences(
        struct _STestSequence* me,
		char* RefValeo,
	    char* RefCustomer
);
static SElExceptionPtr SetPanelMode(
		struct _STestSequence* me, 
		int32_t mode
);
static SElExceptionPtr UnLoad(
        struct _STestSequence* pTestSeq
);
static SElExceptionPtr UsageValidation(
        struct _STestSequence* pTestSeq,
		STestParamPtr pTestParam
);
static SElExceptionPtr RunAll(
        struct _STestSequence* pTestSeq,
		STestParamPtr	pReserved,
		bool_t fault_ignore
);
static SElExceptionPtr RunStation(
        struct _STestSequence* pTestSeq,
		STestParamPtr	pReserved,
		bool_t fault_ignore,
		int32_t	Station_RnR, 
		int32_t Repeat_RnR
);
static SElExceptionPtr RunSelectedStation(
        struct _STestSequence* pTestSeq,
		STestParamPtr	pReserved,
		bool_t fault_ignore,
		int32_t	Station
);
static SElExceptionPtr RunStationMaster(
        struct _STestSequence* pTestSeq,
		STestParamPtr	pReserved,
		bool_t fault_ignore,
		int32_t	Station_Master
);
static SElExceptionPtr RunSelected(
		struct _STestSequence* me, 
		STestParamPtr	pReserved,
		const char* test_name
);
static SElExceptionPtr TestSequenceHelp(
		struct _STestSequence* me, 
		const char* test_name, 
		SELStringPtr pHelp
);
static SElExceptionPtr SetTestActive(
		struct _STestSequence* me,
		const char* TestName,
		int32_t TestStep
);
static SElExceptionPtr StepStart(struct _STestSequence* me, STestParamPtr pTestParam, bool_t fault_ignore);
static SElExceptionPtr StepNext(struct _STestSequence* me, bool_t* last_step); 
static SElExceptionPtr StepRepeat(struct _STestSequence* me); 
static SElExceptionPtr StepBack(struct _STestSequence* me); 
static SElExceptionPtr StepStop(struct _STestSequence* me);
static bool_t StepSequenceActive(struct _STestSequence* me);  

void SaveXmlResultFile(STestParamPtr pTestParam, bool_t failed);

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "testseq_new"
SElExceptionPtr testseq_new(
	STestSequencePtr*  pTestSequence
)
{
    SElExceptionPtr pexception = NULL;

    *pTestSequence = (STestSequencePtr)calloc(1, sizeof(STestSequence));
    EXCCHECKALLOC( *pTestSequence);    
    (*pTestSequence)->Load = Load;
	(*pTestSequence)->UnLoad = UnLoad;
	(*pTestSequence)->UsageValidation = UsageValidation;
	(*pTestSequence)->RunAll = RunAll;
	(*pTestSequence)->RunSelectedStation = RunSelectedStation;
	(*pTestSequence)->RunStationMaster = RunStationMaster;
	(*pTestSequence)->RunStation = RunStation;
	(*pTestSequence)->RunSelected = RunSelected;
	(*pTestSequence)->TestSequenceHelp = TestSequenceHelp;
	(*pTestSequence)->SetTestActive = SetTestActive;
	(*pTestSequence)->IniPanel = IniPanel;
	(*pTestSequence)->SetPanelDescription = SetPanelDescription;
	(*pTestSequence)->SetPanelWarning = SetPanelWarning;
	(*pTestSequence)->SetPanelStationStatus = SetPanelStationStatus;
	(*pTestSequence)->SetTestPanelReferences = SetTestPanelReferences;
	(*pTestSequence)->SetPanelMode = SetPanelMode;
	(*pTestSequence)->StepStart = StepStart;
	(*pTestSequence)->StepNext = StepNext;
	(*pTestSequence)->StepRepeat = StepRepeat;
	(*pTestSequence)->StepBack = StepBack;
	(*pTestSequence)->StepStop = StepStop;
	(*pTestSequence)->StepSequenceActive = StepSequenceActive;
	
Error:
    if ( pexception )
    {
        testseq_delete(pTestSequence);   
    } 
    EXCRETHROW( pexception);
} /* testseq_new */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "testseq_delete"
SElExceptionPtr testseq_delete(
	STestSequencePtr* pTestSeq
)
{
	SElExceptionPtr pexception = NULL;

	if (pTestSeq && *pTestSeq)
	{
        free((*pTestSeq)->_pTests);
        free(*pTestSeq);
        *pTestSeq = NULL;
	} /* testseq_delete */

//Error:
	EXCRETHROW( pexception);
} /* testseq_delete */

#define TEST_CALLOC_RESERVE 	32
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::Load"
static SElExceptionPtr Load(
        struct _STestSequence* pTestSeq,
        const char* path,
		SObjListPtr pModules,
		SElEventFactoryPtr pEventFactory
)
{
	SElExceptionPtr pexception = NULL;
	int32_t         i, count;
	STestListItemPtr   pitem;
	GetTestFunctionFcePtr_t 	GetTestFunction = NULL;
	GetTestSeqFunctionFcePtr_t 	GetTestSeqFunction = NULL;
	TestFce_t				pfce = NULL;
	STestPtr*				ptest = NULL;

    pTestSeq->_hLibrary = LoadLibrary(path);
    if ( pTestSeq->_hLibrary == (HANDLE)NULL )
    {
		char	buffer[1024];
		int		err = GetLastError();
		
		sprintf(buffer, "Load library failed: \"%s\"", path);
        EXCTHROW( EL_ERROR_OS_SPECIFIC, buffer);
    }
	
	pTestSeq->_IniTestPanel = (IniTestPanel_t) GetProcAddress( pTestSeq->_hLibrary, "IniTestPanel"); 
	pTestSeq->_SetTestPanelDescription = (SetTestPanelDescription_t) GetProcAddress( pTestSeq->_hLibrary, "SetTestPanelDescription");
	pTestSeq->_SetPanelWarning = (SetPanelWarning_t) GetProcAddress( pTestSeq->_hLibrary, "SetPanelWarning");
	pTestSeq->_SetPanelStationStatus = (SetPanelStationStatus_t) GetProcAddress( pTestSeq->_hLibrary, "SetPanelStationStatus");
	pTestSeq->_SetTestPanelReferences = (SetTestPanelReferences_t) GetProcAddress( pTestSeq->_hLibrary, "SetTestPanelReferences");
	pTestSeq->_SetTestPanelMode = (SetTestPanelMode_t) GetProcAddress( pTestSeq->_hLibrary, "SetTestPanelMode");
	pTestSeq->_CloseTestPanel = (CloseTestPanel_t) GetProcAddress( pTestSeq->_hLibrary, "CloseTestPanel"); 
	
	/* ini, cleanup testsequence */
	GetTestSeqFunction = (GetTestSeqFunctionFcePtr_t) GetProcAddress( pTestSeq->_hLibrary, "GetTestSeqFunction");  
	EXCCHECK( GetTestSeqFunction(&pTestSeq->_pTestsSeq));
	
	pfce = (TestFce_t) GetProcAddress( pTestSeq->_hLibrary, pTestSeq->_pTestsSeq->InitName);
	pTestSeq->_pTestsSeq->Init = pfce;
	
	pfce = (TestFce_t) GetProcAddress( pTestSeq->_hLibrary, pTestSeq->_pTestsSeq->CleanupName);
	pTestSeq->_pTestsSeq->Cleanup = pfce;
	
    GetTestFunction = (GetTestFunctionFcePtr_t) GetProcAddress( pTestSeq->_hLibrary, "GetTestFunction");
    EXCCHECK( GetTestFunction( -1, NULL, &count));
        
    pTestSeq->_pTests = (STestPtr*)calloc( count + TEST_CALLOC_RESERVE, sizeof(STestPtr));
    EXCCHECKALLOC( pTestSeq->_pTests);
	pTestSeq->_TestAlloc = (uint32_t)count + TEST_CALLOC_RESERVE;
    pTestSeq->_TestCount = (uint32_t)count;
    
    ptest = &(pTestSeq->_pTests[0]);
    
    for ( i = 0; i < count; i++, ptest++ )
    {
        EXCCHECK( GetTestFunction( i, &pitem, NULL));
        EXCCHECK( test_new( ptest));
		
		(*ptest)->test.name = pitem->name;
		(*ptest)->test.HelpName = pitem->HelpName; 
		(*ptest)->test.CheckName = pitem->CheckName; 
		(*ptest)->test.InitName = pitem->InitName; 
		(*ptest)->test.RunName = pitem->RunName; 
		(*ptest)->test.CleanupName = pitem->CleanupName; 
		
        pfce = (TestFce_t) GetProcAddress( pTestSeq->_hLibrary, pitem->HelpName);
        EXCCHECK( (*ptest)->SetFunctionPtr( *ptest,
        								 TEST_FCE_TYPE_HELP,
										 pitem->HelpName,
        								 pfce));
        pfce = (TestFce_t) GetProcAddress( pTestSeq->_hLibrary, pitem->CheckName);
        EXCCHECK( (*ptest)->SetFunctionPtr( *ptest,
        								 TEST_FCE_TYPE_CHECK,
										 pitem->CheckName,
        								 pfce));
        pfce = (TestFce_t) GetProcAddress( pTestSeq->_hLibrary, pitem->InitName);
        EXCCHECK( (*ptest)->SetFunctionPtr( *ptest,
        								 TEST_FCE_TYPE_INIT,
										 pitem->InitName,
        								 pfce));
        pfce = (TestFce_t) GetProcAddress( pTestSeq->_hLibrary, pitem->RunName);
        EXCCHECK( (*ptest)->SetFunctionPtr( *ptest,
        								 TEST_FCE_TYPE_RUN,
										 pitem->RunName,
        								 pfce));
		pfce = (TestFce_t) GetProcAddress( pTestSeq->_hLibrary, pitem->CleanupName);
        EXCCHECK( (*ptest)->SetFunctionPtr( *ptest,
										 TEST_FCE_TYPE_CLEAN,
										 pitem->CleanupName,
										 pfce));
    }

Error:
	EXCRETHROW( pexception);
} /* STestSequence::Load */

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "STestSequence::SetTestActive"
static SElExceptionPtr SetTestActive(
		struct _STestSequence* pTestSeq,
		const char* TestName,
		int32_t TestStep
)
{
	SElExceptionPtr pexception = NULL;
	STestPtr*		ptest = pTestSeq->_pTests; 
	STestPtr		ptest_new = NULL;
	uint32_t        i = 0;  
	
	if ( ptest )
	{
		for( i = 0; i < pTestSeq->_TestCount; i++, ptest++ )
	    {
	        if(0==strcmp((*ptest)->test.name, TestName))
			{
				if((*ptest)->_IsUsed)
				{
					/* create copy */
					EXCCHECK( (*ptest)->CloneSelf(*ptest, &ptest_new));
					ptest_new->_Step = TestStep;
					
					if(pTestSeq->_TestCount<pTestSeq->_TestAlloc)
					{
						pTestSeq->_pTests[pTestSeq->_TestCount] = ptest_new;
						pTestSeq->_TestCount++;
					}
					else
					{
						pTestSeq->_pTests = realloc( pTestSeq->_pTests, 
													(pTestSeq->_TestAlloc + TEST_CALLOC_RESERVE)*sizeof(STestPtr));
						EXCCHECKALLOC( pTestSeq->_pTests);
						pTestSeq->_TestAlloc += TEST_CALLOC_RESERVE;
						
						pTestSeq->_pTests[pTestSeq->_TestCount] = ptest_new;
						pTestSeq->_TestCount++;
					}
				}
				else
				{
					(*ptest)->_IsUsed = TRUE;
					(*ptest)->_Step = TestStep;
				}
				break;
			}
	    }
		if(i == pTestSeq->_TestCount)
		{
			char serror[512];

			sprintf(serror, "Testsequence, test not found: %s", TestName);
			EXCTHROW(ERROR_TESTSEQUENCE_TEST_NOT_FOUND, serror);
		}
	}
	
Error:
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::IniPanel"
static SElExceptionPtr IniPanel(
        struct _STestSequence* pTestSeq,
		const char* panelPicture,
		bool_t reset
)
{
	SElExceptionPtr pexception = NULL;

	if(pTestSeq->_IniTestPanel)
		EXCCHECK( pTestSeq->_IniTestPanel(panelPicture, reset)); 

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::SetPanelDescription"
static SElExceptionPtr SetPanelDescription(
        struct _STestSequence* pTestSeq,
        int32_t     control,
		const char* description
)
{
	SElExceptionPtr pexception = NULL;

	if(pTestSeq->_SetTestPanelDescription)
		EXCCHECK( pTestSeq->_SetTestPanelDescription(control, description)); 

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::SetPanelWarning"
static SElExceptionPtr SetPanelWarning(
        struct _STestSequence* pTestSeq,
        int32_t     control,
		bool_t status
)
{
	SElExceptionPtr pexception = NULL;

	if(pTestSeq->SetPanelWarning)
		EXCCHECK( pTestSeq->_SetPanelWarning(control, status)); 

Error:
	EXCRETHROW( pexception);
}
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::SetPanelStationStatus"
static SElExceptionPtr SetPanelStationStatus(
        struct _STestSequence* pTestSeq,
        int32_t     control,
		bool_t status
)
{
	SElExceptionPtr pexception = NULL;

	if(pTestSeq->SetPanelStationStatus)
		EXCCHECK( pTestSeq->_SetPanelStationStatus(control, status)); 

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::SetTestPanelReferences"
static SElExceptionPtr SetTestPanelReferences(
        struct _STestSequence* pTestSeq,
    char* 	RefValeo,
    char* 	RefCustomer
)
{
	SElExceptionPtr pexception = NULL;

	if(pTestSeq->SetTestPanelReferences)
		EXCCHECK( pTestSeq->_SetTestPanelReferences(RefValeo, RefCustomer)); 

Error:
	EXCRETHROW( pexception);
}
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::SetPanelMode"
static SElExceptionPtr SetPanelMode(
		struct _STestSequence* pTestSeq, 
		int32_t mode
)
{
	SElExceptionPtr pexception = NULL;

	if(pTestSeq->_SetTestPanelMode)
		EXCCHECK( pTestSeq->_SetTestPanelMode(mode)); 

Error:
	EXCRETHROW( pexception);
}
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::UnLoad"
static SElExceptionPtr UnLoad(
        struct _STestSequence* pTestSeq
)
{
	SElExceptionPtr pexception = NULL;
	uint32_t        i = 0;
	STestPtr*		ptest = pTestSeq->_pTests;

    if ( ptest )
	{
		EXCCHECK( pTestSeq->StepStop(pTestSeq));
	
		for(i=0; i<pTestSeq->_TestCount; i++, ptest++ )
	    {
	        EXCCHECK( test_delete(ptest));
	    }
		free( pTestSeq->_pTests);
		pTestSeq->_pTests = NULL;
		pTestSeq->_TestAlloc = 0;
    	pTestSeq->_TestCount = 0; 
		pTestSeq->_pTestsSeq = NULL;
		
		EXCCHECK( pTestSeq->_CloseTestPanel());
		
		if ( pTestSeq->_hLibrary )
		{
			pTestSeq->_hLibrary = NULL;
			pTestSeq->_IniTestPanel = NULL;
			pTestSeq->_SetTestPanelDescription = NULL;
			pTestSeq->_SetPanelWarning = NULL;
			pTestSeq->_SetTestPanelMode = NULL;
			pTestSeq->_CloseTestPanel = NULL; 
		}
	}
	
Error:
	EXCRETHROW( pexception);
} /* STestSequence::UnLoad */

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::UsageValidation"
static SElExceptionPtr UsageValidation(
        struct _STestSequence* pTestSeq,
		STestParamPtr pTestParam
)
{
	SElExceptionPtr pexception = NULL;
	uint32_t        i = 0;
	STestPtr*		ptest = pTestSeq->_pTests;
	SELStringPtr	pstring = NULL;

    if(ptest)
	{
		EXCCHECK( elstring_new(NULL, &pstring)); 

		for(i=0; i<pTestSeq->_TestCount; i++, ptest++)
	    {
			if((*ptest)->_IsUsed)
			{
				pTestParam->step = (*ptest)->_Step;
		        pexception = (*ptest)->test.Check(pTestParam);
				
				(*ptest)->_CanBeUsed = (pexception==NULL);
				
				if(pexception)
				{
					if(pexception->GetErrorCode(pexception)==TEST_ERROR_NOT_POSIBLE_TEST)
					{	
						EXCCHECK(pstring->Format(pstring, "%s: %s\n", (*ptest)->test.name, pexception->GetErrorMsgPtr(pexception)));
						EXCDELETE(pexception);
					}
					/*
					* If the exception is different from TEST_ERROR_NOT_VALID_TESTER_CONFIGURATION
					* function is abborted.
					*/
					EXCCHECK( pexception); 
				}
			}
	    }
	}

	if (pstring!= NULL && pstring->Size(pstring)>0)
	{
		EXCTHROW( TEST_ERROR_NOT_VALID_TESTER_CONFIGURATION, pstring->GetPtr(pstring)); 
	}

Error:
	elstring_delete( &pstring);
	EXCRETHROW( pexception);	
} /* STestSequence::UsageValidation */

int compare(const void *a, const void *b) 
{
	STestPtr	test1 = *(STestPtr*)a;
	STestPtr	test2 = *(STestPtr*)b;
	
	return (int)(test1->_Step - test2->_Step);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::RunAll"
static SElExceptionPtr RunAll(
        struct _STestSequence* pTestSeq,
		STestParamPtr	pTestParam,
		bool_t fault_ignore
)
{
	SElExceptionPtr pexception = NULL;
	SElExceptionPtr pexc_table = NULL;
	uint32_t        i = 0;
	STestPtr*		ptest = pTestSeq->_pTests;
	STestSeqPtr		ptestseq = pTestSeq->_pTestsSeq;
	HANDLE			handle = NULL;
	bool_t			cleanup = FALSE;
	int 			Test, SubTest;
	char 			msg[128];
	  int retest;
	qsort(pTestSeq->_pTests, pTestSeq->_TestCount, sizeof(STestPtr), compare);
	
	if(ptestseq)
		EXCCHECK( (ptestseq->Init)(pTestParam));
	
    if ( ptest )
	{
		for(i=0; i<pTestSeq->_TestCount; i++, ptest++)
	    {

			if  (pexception) /* if panel Fail Run Only wait Table */
			{
				sscanf((*ptest)->test.name, "%d.%d", &Test, &SubTest);
				if (SubTest == 90)
					{
					StatusStations[(Test/10)-1] = 1;
					pexc_table = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
						if (pexc_table)
						{
					  	EXCCHECK(pexc_table);
						}
					}
			}
			else  /* if panel Pass Run next step */
			{
				sscanf((*ptest)->test.name, "%d.%d", &Test, &SubTest); // to change  StatusStations from 0 to 1
				if (SubTest == 90)
				{
				StatusStations[(Test/10)-1] = 1; 
				pexception = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
				if (fault_ignore)
						EXCDELETE(pexception);
				pTestParam->pexception = pexception;
				}
				else
				{
					if (Test == 0) //for test 00.10 & 00.20 because 00/10 -1 = negative array
						StatusStations[1] = 0;
					else 
					{
						if ((*ptest)->_IsUsed)
							StatusStations[(Test/10)-1] = 0;
							
			   /* formerly excluded lines */
						//if (fault_ignore &&   /* don't run Rodage for master parts */
				  		//	   (!strcmp((*ptest)->test.name, "30.00") ||
						//		!strcmp((*ptest)->test.name, "30.10") ||
						//		!strcmp((*ptest)->test.name, "30.20")));
			   /* formerly excluded lines */
													
//#if 0    /* formerly excluded lines */
							//printf("(*ptest)->test.RunName = %s \r\n", (*ptest)->test.RunName);
							 if (!fault_ignore &&  /* retest en prod pas en mode GO */ (!strcmp((*ptest)->test.RunName, "CMFB_40_70_test") ||  
									 													!strcmp((*ptest)->test.RunName, "CMFB_50_70_test") ||  
																						!strcmp((*ptest)->test.RunName, "CMFB_40_709_test") ||  
																						!strcmp((*ptest)->test.RunName, "CMFB_50_709_test") ||  
																						!strcmp((*ptest)->test.RunName, "CMFB_40_700_test") ||  
																						!strcmp((*ptest)->test.RunName, "CMFB_50_700_test")))
							{
								retest = 0;
								pTestParam->fault_retest = TRUE; // Variable pour les steps pour initialisatrion axe z
								
								do {
										if (retest == MAX_RETEST - 1) /* if last init station if NOK */
										{
										  pTestParam->fault_retest = FALSE;
										}
										
										pexception = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
										if(pexception==NULL)
										{
											break;
										}
										else if(pexception)
										{
											Sleep(500);
												
											retest++;

										  if (retest == MAX_RETEST)
											{
												break;
											}
											
											else
											{	
												/* connexion / deconexion plug en mode retest 
												if (Test == 40)
												{
											   		 pexception = pTestSeq->RunSelected( pTestSeq, pTestParam, "40.07"); // UNPLUG
													if (pexception) break;
													Sleep(500);
											   		 pexception = pTestSeq->RunSelected( pTestSeq, pTestParam, "40.05"); // PLUG
													if (pexception) break;
												}
													
												if (Test == 50)
												{
											   		 pexception = pTestSeq->RunSelected( pTestSeq, pTestParam, "50.07");
													if (pexception) break;
													Sleep(500);
											   		 pexception = pTestSeq->RunSelected( pTestSeq, pTestParam, "50.05");
													if (pexception) break;
												}*/
											EXCDELETE(pexception);
											}
										 }	 
									
								}while(TRUE);
							}
//#endif   /* formerly excluded lines */
							
							else
							{
								pexception = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
								if (fault_ignore)
									EXCDELETE(pexception);
							}
							
							pTestParam->pexception = pexception;
					}
				}
			}
			
	    }
	
		if(!fault_ignore)
			{
				EXCCHECK(pexception);
			}
			else
			{
				EXCDELETE(pexception);
			}
	}
	

	if(ptestseq)  
	{
		cleanup = TRUE;
		EXCCHECK( (pTestSeq->_pTestsSeq->Cleanup)(pTestParam));
	}

Error:
	if(ptestseq && !cleanup)  
	{
		SElExceptionPtr pexc_cleanup = NULL;

		pexc_cleanup = (pTestSeq->_pTestsSeq->Cleanup)(pTestParam);
		EXCDELETE( pexc_cleanup);
	}
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::RunSelectedStation"
static SElExceptionPtr RunSelectedStation(
        struct _STestSequence* pTestSeq,
		STestParamPtr	pTestParam,
		bool_t fault_ignore,
		int32_t	Station
)
{
	SElExceptionPtr pexception = NULL;
	SElExceptionPtr pexc_table = NULL;
	uint32_t        i = 0;
	uint32_t        j = 0;
	STestPtr*		ptest = pTestSeq->_pTests;
	STestSeqPtr		ptestseq = pTestSeq->_pTestsSeq;
	HANDLE			handle = NULL;
	bool_t			cleanup = FALSE;
	int 			Test, SubTest;
	STestParam		TestParam = {0};
	
	
	
	qsort(pTestSeq->_pTests, pTestSeq->_TestCount, sizeof(STestPtr), compare);
	
	if(ptestseq)
		EXCCHECK( (ptestseq->Init)(pTestParam));
	
	
    if (ptest)
	{
		for(i=0; i<pTestSeq->_TestCount; i++, ptest++)
	   	{
			
			sscanf((*ptest)->test.name, "%d.%d", &Test, &SubTest);
			
			if  ((*ptest)->_IsUsed)
			{
			 	if (pexception == NULL ) /* if panel Pass Run next step */
				{
					/* Selected station */
					if (Station == Test && SubTest != 90)
						{
							EXCCHECK( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
						}
				}
			}
		}
	
		EXCCHECK(pexception);
	}
	

	if(ptestseq)  
	{
		cleanup = TRUE;
		EXCCHECK( (pTestSeq->_pTestsSeq->Cleanup)(pTestParam));
	}

Error:
	if(ptestseq && !cleanup)  
	{
		SElExceptionPtr pexc_cleanup = NULL;
		pexc_cleanup = (pTestSeq->_pTestsSeq->Cleanup)(pTestParam);
		EXCDELETE( pexc_cleanup);
	}
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::RunStation"
static SElExceptionPtr RunStation(
        struct _STestSequence* pTestSeq,
		STestParamPtr	pTestParam,
		bool_t fault_ignore,
		int32_t	Station_RnR,
		int32_t	Repeat_RnR
)
{
	SElExceptionPtr pexception = NULL;
	SElExceptionPtr pexc_table = NULL;
	uint32_t        i = 0;
	uint32_t        j = 0;
	STestPtr*		ptest = pTestSeq->_pTests;
	STestSeqPtr		ptestseq = pTestSeq->_pTestsSeq;
	HANDLE			handle = NULL;
	bool_t			cleanup = FALSE;
	int 			Test, SubTest;
	int 			First_TestCount_RnR, End_TestCount_RnR;
	
	
	
	qsort(pTestSeq->_pTests, pTestSeq->_TestCount, sizeof(STestPtr), compare);
	
	if(ptestseq)
		EXCCHECK( (ptestseq->Init)(pTestParam));
	
	
	pTestParam->RnR_counter = 0;
	
    if (ptest)
	{
		for(i=0; i<pTestSeq->_TestCount; i++, ptest++)
	   	{
			sscanf((*ptest)->test.name, "%d.%d", &Test, &SubTest);
			
			if  ((*ptest)->_IsUsed)
			{
				/* test RnR */
				if (Station_RnR == Test/10 - 1)
				{
					StatusStations[(Test/10)-1] = BUSY;
					
					if (SubTest == 90) //rotation
						{
							WriteLine (pTestParam->pTicket_CSV, pTestParam->line_CSV, MAX_LENGTH_CSV-1);
							memset(pTestParam->line_CSV, 0X00, MAX_LENGTH_CSV);

							End_TestCount_RnR = i+1; /* decide rotate or not the table when arrived to end test station RnR*/

							if (pTestParam->RnR_counter == Repeat_RnR) /* if all Repeatitions terminated Rotate Table*/
								{
								CloseFile (pTestParam->pTicket_CSV);
								StatusStations[(Test/10)-1] = NOT_BUSY; //station becaome not busy because wait rotation and all test are finished
								pexc_table = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
									if (pexc_table)
										{
					  					EXCCHECK(pexc_table);
										}
								}
					
							else  /* if Repeatitions not terminated back to first test don't rotate table */
								{
					 			for (j=First_TestCount_RnR; j<End_TestCount_RnR;j++)
									{
									ptest--;
										i--;
									}
								pTestParam->RnR_counter++;
								
								/* unplug and unlock panel */
								//pexc_table = pTestSeq->RunSelected(pTestSeq, pTestParam, "60.85");
								}
						}
					
					else if (Station_RnR == 0 && i == pTestSeq->_TestCount-1) //no  SubTest == 90 in Station 10
						{
							/* Run Last Test in station 10 */
							if (pTestParam->RnR_counter == 0 && (*ptest)->_IsUsed)
									{
									pTestParam->RnR_counter += 1; 
									First_TestCount_RnR = i;
									}
					
							if ((*ptest)->_IsUsed)
									StatusStations[(Test/10)-1] = BUSY; 
								pexception = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
								pTestParam->pexception = pexception;	

							WriteLine (pTestParam->pTicket_CSV, pTestParam->line_CSV, MAX_LENGTH_CSV-1);
							memset(pTestParam->line_CSV, 0X00, MAX_LENGTH_CSV);

							End_TestCount_RnR = i+1; /* decide rotate or not the table when arrived to end test station RnR*/

							if (pTestParam->RnR_counter == Repeat_RnR) /* if all Repeatitions terminated Rotate Table*/
								{
								CloseFile (pTestParam->pTicket_CSV);
								StatusStations[(Test/10)-1] = NOT_BUSY; 
								}
					
							else  /* if Repeatitions not terminated back to first test don't rotate table */
								{
					 			for (j=First_TestCount_RnR; j<End_TestCount_RnR;j++)
									{
									ptest--;
										i--;
									}
								pTestParam->RnR_counter++; 	
								}
						}
				
					else /* station to Run R&R*/
							{
							if (pTestParam->RnR_counter == 0 && (*ptest)->_IsUsed)
									{
									pTestParam->RnR_counter += 1; 
									First_TestCount_RnR = i;
									}
								if ((*ptest)->_IsUsed)
									StatusStations[(Test/10)-1] = BUSY; //busy
								pexception = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
								pTestParam->pexception = pexception;	
								}	
	
				}
				
				
				/* NO test RnR */	
				else  
				{
					if (SubTest == 90)
					{
					StatusStations[(Test/10)-1] = NOT_BUSY;
					pexc_table = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
						if (pexc_table)
						{
					  	EXCCHECK(pexc_table);
						}
					}
				}
			}
			
		}
	
		EXCCHECK(pexception);
	}
	

	if(ptestseq)  
	{
		cleanup = TRUE;
		EXCCHECK( (pTestSeq->_pTestsSeq->Cleanup)(pTestParam));
	}

Error:
	if(ptestseq && !cleanup)  
	{
		SElExceptionPtr pexc_cleanup = NULL;
		pexc_cleanup = (pTestSeq->_pTestsSeq->Cleanup)(pTestParam);
		EXCDELETE( pexc_cleanup);
	}
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::RunStationMaster"
static SElExceptionPtr RunStationMaster(
        struct _STestSequence* pTestSeq,
		STestParamPtr	pTestParam,
		bool_t fault_ignore,
		int32_t	Station_Master
)
{
	SElExceptionPtr pexception = NULL;
	SElExceptionPtr pexc_table = NULL;
	uint32_t        i = 0;
	uint32_t        j = 0;
	STestPtr*		ptest = pTestSeq->_pTests;
	STestSeqPtr		ptestseq = pTestSeq->_pTestsSeq;
	HANDLE			handle = NULL;
	bool_t			cleanup = FALSE;
	int 			Test, SubTest; 
	int				actual_station = 0;
	
	
	
	qsort(pTestSeq->_pTests, pTestSeq->_TestCount, sizeof(STestPtr), compare);
	
	if(ptestseq)
		EXCCHECK( (ptestseq->Init)(pTestParam));
	
	
    if (ptest)
	{
		for(i=0; i<pTestSeq->_TestCount; i++, ptest++)
	   	{
			sscanf((*ptest)->test.name, "%d.%d", &Test, &SubTest);
			
			if  ((*ptest)->_IsUsed)
			{
				/* test Master in this Station */
				if (Station_Master == Test/10)
				{
					if (SubTest == 90)
					{
					StatusStations[(Test/10)-1] = 1; 
					pexception = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
					if (fault_ignore)
						EXCDELETE(pexception);
					pTestParam->pexception = pexception;
					}
					
					else 
					{
					StatusStations[(Test/10)-1] = 0; 
					if ((*ptest)->_IsUsed)
						pexception = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
						if (fault_ignore)
							EXCDELETE(pexception);
						pTestParam->pexception = pexception;
					}
			
				}	
			
				/* don't test Master in this Station  Only Run 90 */	
				else  
				{
					if (SubTest == 90)
					{
					Sleep (1000);
					StatusStations[(Test/10)-1] = 1;
					pexc_table = ( (*ptest)->Run( *ptest, pTestParam, &handle, NULL ));
						if (pexc_table)
						{
					  	EXCCHECK(pexc_table);
						}
					}
				}
			}
			
		}
	
		EXCCHECK(pexception);
	}
	

	if(ptestseq)  
	{
		cleanup = TRUE;
		EXCCHECK( (pTestSeq->_pTestsSeq->Cleanup)(pTestParam));
	}

Error:
	if(ptestseq && !cleanup)  
	{
		SElExceptionPtr pexc_cleanup = NULL;
		pexc_cleanup = (pTestSeq->_pTestsSeq->Cleanup)(pTestParam);
		EXCDELETE( pexc_cleanup);
	}
	EXCRETHROW( pexception);
}


/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "STestSequence::RunSelected"
static SElExceptionPtr RunSelected(
        struct _STestSequence* pTestSeq,
		STestParamPtr pTestParam,
		const char* test_name
)
{
	SElExceptionPtr pexception = NULL;
	uint32_t        i = 0;
	STestPtr*		ptest = pTestSeq->_pTests;
	STestSeqPtr		ptestseq = pTestSeq->_pTestsSeq;
	STestParam		TestParam = {0};
	
    if(ptest)
	{
		for(i=0; i<pTestSeq->_TestCount; i++, ptest++)
	    {
			if(0==strcmp(test_name, (*ptest)->test.name))
			{
				STestParamPtr ptestparam = (pTestParam!=NULL)? pTestParam:&TestParam;

				EXCCHECK( (*ptest)->test.Init( ptestparam));
				EXCCHECK( (*ptest)->test.Run( ptestparam));
				EXCCHECK( (*ptest)->test.Cleanup( ptestparam));

				break;
			}
	    }
	}

Error:
	EXCRETHROW( pexception);	
} 

/*----------------------------------------------------------------------------*/ 
/*  <testsequnce>
 *		<test>
 *			<help></help>
 *			<parameter>
 *				<option/>
 *				<name/>
 *				<type/>
 *				<help/>
 *			</parameter>
 *		</test>
 *		...
 *	</testsequence>
 */
#undef __FUNC__
#define __FUNC__ "STestSequence::TestSequenceHelp"
static SElExceptionPtr TestSequenceHelp(
		struct _STestSequence* pTestSeq, 
		const char* test_name, 
		SELStringPtr pHelp
)
{
	SElExceptionPtr pexception = NULL;
	uint32_t        i = 0;
	STestPtr*		ptest = pTestSeq->_pTests;
	STestSeqPtr		ptestseq = pTestSeq->_pTestsSeq;
	STestParam		TestParam = {0};
	
    if(pHelp && ptest)
	{
		TestParam.pHelp = pHelp;

		if(test_name && test_name[0]!='\0') /* help for selected test */
		{
			for(i=0; i<pTestSeq->_TestCount; i++, ptest++)
			{
				if(0==strcmp(test_name, (*ptest)->test.name))
				{
					EXCCHECK( pHelp->Format(pHelp, "<test name='%s'>\n", (*ptest)->test.name));
					EXCCHECK( (*ptest)->test.Help(&TestParam));
					EXCCHECK( pHelp->Append(pHelp, "</test>\n"));
					break;
				}
			}
		}
		else /* help for whole testsequence */
		{
			EXCCHECK( pHelp->Append(pHelp, "<testsequence>\n"));

			for(i=0; i<pTestSeq->_TestCount; i++, ptest++)
			{
				EXCCHECK( pHelp->Format(pHelp, "<test name='%s'>\n", (*ptest)->test.name));
				EXCCHECK( (*ptest)->test.Help(&TestParam));
				EXCCHECK( pHelp->Append(pHelp, "</test>\n"));
			}

			EXCCHECK( pHelp->Append(pHelp, "</testsequence>"));
		}
	}

Error:
	EXCRETHROW( pexception);	
} 

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "STestSequence::StepStart"
static SElExceptionPtr StepStart(struct _STestSequence* pTestSeq, STestParamPtr pTestParam, bool_t fault_ignore)
{
	SElExceptionPtr pexception = NULL;
	STestSeqPtr		ptestseq = pTestSeq->_pTestsSeq;
	
	pTestSeq->_StepTestParameter = pTestParam;
	pTestSeq->_StepTestActive = TRUE;
	pTestSeq->_StepTestActual = 0;
	pTestSeq->_StepIgnoreFault = fault_ignore;
	
	qsort(pTestSeq->_pTests, pTestSeq->_TestCount, sizeof(STestPtr), compare); 
	
	if(ptestseq)
		EXCCHECK( (ptestseq->Init)(pTestParam));
	
Error:
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "STestSequence::StepNext"
static SElExceptionPtr StepNext(struct _STestSequence* pTestSeq, bool_t* last_step)
{
	SElExceptionPtr pexception = NULL;
	STestPtr*		ptest = pTestSeq->_pTests; 
	uint32_t		i;
	HANDLE			handle = NULL;  
	bool_t 			used = FALSE;
	
	if(pTestSeq->_StepTestActive)
	{
		if (ptest)
		{
			for(i=pTestSeq->_StepTestActual, ptest+=pTestSeq->_StepTestActual; i<pTestSeq->_TestCount; i++, ptest++ )
		    {
				pexception = ( (*ptest)->Run( *ptest, pTestSeq->_StepTestParameter, &handle, &used ));
				
				if(!pTestSeq->_StepIgnoreFault)
				{
					EXCCHECK(pexception);
				}
				
				if(used)
				{	
					pTestSeq->_StepTestActual = i + 1;	
					break;
				}
		    }
			if(pTestSeq->_StepTestActual==pTestSeq->_TestCount)
			{
				if(last_step) *last_step = TRUE;
				EXCCHECK(pTestSeq->StepStop(pTestSeq));
			}
		}
	}
	
Error:
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "STestSequence::StepBack"
static SElExceptionPtr StepBack(struct _STestSequence* pTestSeq)
{
	SElExceptionPtr pexception = NULL;
	STestPtr*		ptest = pTestSeq->_pTests; 
	uint32_t		i;
	HANDLE			handle = NULL;  
	bool_t 			used = FALSE;
	
	if(pTestSeq->_StepTestActive)
	{
		if (ptest && pTestSeq->_StepTestActual>1)
		{
			pTestSeq->_StepTestActual -= 2;

			for(i=pTestSeq->_StepTestActual, ptest+=pTestSeq->_StepTestActual; i<pTestSeq->_TestCount; i++, ptest++ )
		    {
				pexception = ( (*ptest)->Run( *ptest, pTestSeq->_StepTestParameter, &handle, &used ));
				
				if(!pTestSeq->_StepIgnoreFault)
				{
					EXCCHECK(pexception);
				}
				
				if(used)
				{	
					pTestSeq->_StepTestActual = i + 1;	
					break;
				}
		    }
		}
	}
	
Error:
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "STestSequence::StepSequenceActive"
static bool_t StepSequenceActive(struct _STestSequence* me) 
{
	return me->_StepTestActive;
}

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "STestSequence::StepRepeat"
static SElExceptionPtr StepRepeat(struct _STestSequence* pTestSeq)
{
	SElExceptionPtr pexception = NULL;
	STestPtr*		ptest = pTestSeq->_pTests; 
	uint32_t		i;
	HANDLE			handle = NULL;  
	bool_t 			used = FALSE;
	
	if(pTestSeq->_StepTestActive && pTestSeq->_StepTestActual>0)
	{
		if (ptest)
		{
			pTestSeq->_StepTestActual--;
				
			for(i=pTestSeq->_StepTestActual, ptest+=pTestSeq->_StepTestActual; i<pTestSeq->_TestCount; i++, ptest++ )
		    {
				pexception = ( (*ptest)->Run( *ptest, pTestSeq->_StepTestParameter, &handle, &used ));
				
				if(!pTestSeq->_StepIgnoreFault)
				{
					EXCCHECK(pexception);
				}
				
				if(used)
				{	
					pTestSeq->_StepTestActual = i + 1;	
					break;
				}
		    }
		}
	}
	
Error:
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "STestSequence::StepStop"
static SElExceptionPtr StepStop(struct _STestSequence* pTestSeq)
{
	SElExceptionPtr pexception = NULL;
	STestSeqPtr		ptestseq = pTestSeq->_pTestsSeq;
	HRESULT         hr = 0;   
	VBOOL         	is_xml_success = 1; 
	
	if(pTestSeq->_StepTestActive)
	{
		if(ptestseq)  
			EXCCHECK((pTestSeq->_pTestsSeq->Cleanup)(pTestSeq->_StepTestParameter));
	
		/* save test file */
		SaveXmlResultFile(pTestSeq->_StepTestParameter, 0);
	
		pTestSeq->_StepTestParameter = NULL;
		pTestSeq->_StepTestActive = FALSE;
		pTestSeq->_StepTestActual = 0;
		pTestSeq->_StepIgnoreFault = FALSE;
	}

Error:
	EXCRETHROW( pexception);	
}

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "SaveXmlResultFile"
void SaveXmlResultFile(STestParamPtr pTestParam, bool_t failed)
{
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	char			pfile_xml[256];
	VARIANT			pfile_name_var = CA_VariantEmpty();

	if(pTestParam && pTestParam->pTicket)
	{
		sprintf(pfile_xml, "%s\\%s_%s_%s.xml",
					pTestParam->TicketDirPath,
					pTestParam->Barcode,
					(failed)? "FAULT":"OK",
					pTestParam->Time);

		strcpy(pTestParam->TicketFilePath, pfile_xml);
		EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, pfile_xml));
		EXCCHECKCOM( MSXML_IXMLDOMDocumentsave (pTestParam->pTicket, NULL, pfile_name_var));
		EXCCHECKCOM( CA_DiscardObjHandle(pTestParam->pTicket));
		pTestParam->pTicket = 0;
	}

Error:
	CA_VariantClear(&pfile_name_var);
	EXCDISPLAY(pexception);
}
