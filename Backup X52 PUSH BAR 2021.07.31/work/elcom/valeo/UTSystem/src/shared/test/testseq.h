/*****************************************************************************
 *
 *  testseq.h
 *
 *  Author: $Author: rykalaz $
 *
 *  $Id: testseq.h,v 1.6 2006/04/28 07:14:51 rykalaz Exp $
 *
 *****************************************************************************/
#if !defined(__EL_TESTSEQ_H__)
#define __EL_TESTSEQ_H__

#include <windows.h>
#include <eltypes.h>
#include <objlist/objlist.h> 
#include <eventfactory/EventFactory.h>
#define EXCLUDE_TEST_MAKROS
#include <test/eltest.h>
#include <NIDAQmx.h>

#ifdef __cplusplus
    extern "C" {
#endif 

/*!
 * Specifies test information
 */ 

#define NB_OF_STATIONS 		8
#define NOT_BUSY 			1
#define BUSY 				0
		
int32_t StatusStations[NB_OF_STATIONS];
 

typedef enum _ETestSequencePanelMode
{
	E_TSPM_DEFAULT_MODE = 0,
	E_TSPM_STEP_BY_STEP_MODE
} ETestSequencePanelMode;

typedef struct _STestSequence {

    SElExceptionPtr (*Load)(struct _STestSequence* me, const char* path, SObjListPtr pModules, SElEventFactoryPtr pEventFactory);
	SElExceptionPtr (*UnLoad)(struct _STestSequence* me); 
	
	/* test panel fnc*/
	SElExceptionPtr (*IniPanel)(struct _STestSequence* me, const char* panelPicture, bool_t reset);
	SElExceptionPtr (*SetPanelDescription)(struct _STestSequence* me, int32_t control, const char* description);
	SElExceptionPtr (*SetPanelWarning)(struct _STestSequence* me, int32_t control, bool_t status);
	SElExceptionPtr (*SetPanelStationStatus)(struct _STestSequence* me, int32_t station, bool_t status);
	SElExceptionPtr (*SetTestPanelReferences)(struct _STestSequence* me, char* RefValeo, char* RefCustomer);
	SElExceptionPtr (*SetPanelMode)(struct _STestSequence* me, int32_t mode);
   
	/* testsequence */
	SElExceptionPtr (*RunAll)(struct _STestSequence* me, STestParamPtr pTestParam, bool_t fault_ignore);
	SElExceptionPtr (*RunSelectedStation)(struct _STestSequence* me, STestParamPtr pTestParam, bool_t fault_ignore, int32_t	Station);
	SElExceptionPtr (*RunStationMaster)(struct _STestSequence* me, STestParamPtr pTestParam, bool_t fault_ignore, int32_t Station_Master);
	SElExceptionPtr (*RunStation)(struct _STestSequence* me, STestParamPtr pTestParam, bool_t fault_ignore, int32_t	Station_RnR, int32_t Repeat_RnR);
	SElExceptionPtr (*StepStart)(struct _STestSequence* me, STestParamPtr pTestParam, bool_t fault_ignore);
	SElExceptionPtr (*StepNext)(struct _STestSequence* me, bool_t* last_step); 
	SElExceptionPtr (*StepRepeat)(struct _STestSequence* me); 
	SElExceptionPtr (*StepBack)(struct _STestSequence* me); 
	SElExceptionPtr (*StepStop)(struct _STestSequence* me);
	bool_t (*StepSequenceActive)(struct _STestSequence* me);

	/* maintenance */
	SElExceptionPtr (*RunSelected)(struct _STestSequence* me, STestParamPtr pTestParam, const char* test_name);
	SElExceptionPtr (*TestSequenceHelp)(struct _STestSequence* me, const char* test_name, SELStringPtr pHelp);

	/* validation */
	SElExceptionPtr (*UsageValidation)(struct _STestSequence* me, STestParamPtr pTestParam);     
	SElExceptionPtr (*SetTestActive)(struct _STestSequence* me, const char* TestName, int32_t TestStep);

    HANDLE      _hLibrary;   /*!< Handle to Library */
	STestSeqPtr _pTestsSeq;  /*!< Ini, Cleanup fnc*/ 
    STestPtr*   _pTests;     /*!< All loaded tests */
	uint32_t	_TestAlloc;
    uint32_t    _TestCount;  /*!< Number of Test in _pTest array */
	CRITICAL_SECTION LockTest; /*!< Lock shared variable */

	/* step by step mode */
	uint32_t			_StepTestActual;
	STestParamPtr		_StepTestParameter;
	bool_t				_StepTestActive;
	bool_t				_StepIgnoreFault;
	char				ProductPartNumber[256+1];
	char				ProductCustomerRef[256+1];
	char				JobCameraPicto[32+1];
	char				JobCameraLed[32+1];
	char				ProductType[32+1];
	char				PICTOS[32+1];
	int				    Picto_Boutton_1;
	int				    Picto_Boutton_2;
	int				    Picto_Boutton_3;
	int				    Picto_Boutton_4;
	int				    Picto_Boutton_5;
	int				    Picto_Boutton_6;
	int				    Picto_Boutton_7;
	int				    Picto_Boutton_8;
	int                 CameraPicto [9+1];
	int                 number_boutton;
	int                 mauvaise;

	IniTestPanel_t 				_IniTestPanel;
	SetTestPanelDescription_t	_SetTestPanelDescription;
	SetPanelWarning_t			_SetPanelWarning;
	SetPanelStationStatus_t		_SetPanelStationStatus;
	SetTestPanelReferences_t	_SetTestPanelReferences;
	SetTestPanelMode_t			_SetTestPanelMode;
	CloseTestPanel_t 			_CloseTestPanel;

} STestSequence, *STestSequencePtr;

SElExceptionPtr testseq_new(
	STestSequencePtr*  pTestSequence
);
SElExceptionPtr testseq_delete(
    STestSequencePtr* pTestSeq
);

void SaveXmlResultFile(STestParamPtr pTestParam, bool_t failed);

#define ERROR_TESTSEQUENCE_TEST_NOT_FOUND        	-100001     

#ifdef __cplusplus
    }
#endif 

#endif /* __EL_TESTSEQ_H__ */
