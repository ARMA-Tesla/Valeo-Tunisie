#include <ansi_c.h>
#include "c:\work\ELCOM\Valeo\UTSystem\src\shared\cvitool\msxmldom2.h"
#include "sledge.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\shared\multithread\multithread.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\tools\tools.h"
#include <log\log.h>

extern SToolsPtr	gs_Tools;
extern void*		gs_LogApi;
static uint32_t		g_id_counter = 1;
static int32_t		g_last_product_id = 0;

static SElExceptionPtr Load(struct _SSledge* me, const char* file_path);
static SElExceptionPtr Unload(struct _SSledge* me);

static SElExceptionPtr Activate(struct _SSledge* me, const char* step);
static SElExceptionPtr Deactivate(struct _SSledge* me);
static SElExceptionPtr WaitForState(struct _SSledge*	me, uint32_t timeout, ETooolState *state);
static SElExceptionPtr GetStatus(struct _SSledge* me, SSledgeStatusPtr pStageSledge);

static SElExceptionPtr ToolActivate(struct _SSledge* me, const char* name, SToolClassPtr* pTool);
static SElExceptionPtr ToolWaitForState(struct _SSledge* me, SToolClassPtr ptool, ETooolState* pStatus);
static SElExceptionPtr ToolDeactivate(struct _SSledge* me, SToolClassPtr ptool);

/* run/stop in the background */
static SElExceptionPtr ToolRun(struct _SSledge* me, const char* name, bool_t run);

static DWORD WINAPI Thread_Sledge(LPVOID param);

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "sledge_new"
SElExceptionPtr	sledge_new(SSledgePtr* pSledge)
{
	SElExceptionPtr	pexception = NULL;
	SSledgePtr		me = NULL;

	me = calloc(1, sizeof(SSledge));
	EXCCHECKALLOC( me);

	me->Load = Load;
	me->Unload = Unload;
	me->Activate = Activate;
	me->Deactivate = Deactivate;
	me->WaitForState = WaitForState;
	me->GetStatus = GetStatus;

	me->State = ESS_OK;
	me->id = g_id_counter++;

	me->LogApi = gs_LogApi;
	me->LogChannel = E_LOG_CHANNEL_SLEDGE;

	InitializeCriticalSection(&me->_Lock);

	if(pSledge) *pSledge = me;

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "sledge_delete"
SElExceptionPtr	sledge_delete(SSledgePtr* pSledge)
{
	SElExceptionPtr	pexception = NULL;

	if(pSledge && *pSledge)
	{
		SSledgePtr	me = *pSledge;

		DeleteCriticalSection(&me->_Lock);

		free(*pSledge);
		*pSledge = NULL;
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Lock"
static SElExceptionPtr Lock(struct _SSledge* me)
{
	EnterCriticalSection( &(me->_Lock));

	return NULL;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Unlock"
static SElExceptionPtr Unlock(struct _SSledge* me)
{
	LeaveCriticalSection( &(me->_Lock));
	return NULL;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetStatus"
static SElExceptionPtr GetStatus(struct _SSledge* me, SSledgeStatusPtr pStageSledge)
{
	pStageSledge->id = me->id;

	if(me->step_nb)
		pStageSledge->progress = ((double)me->step_actual/(double)me->step_nb)*100.0;
	else
		pStageSledge->progress = 0;

	strcpy(pStageSledge->version, me->version);

	Lock(me);
	strcpy(pStageSledge->tool_name, me->tool_name);
	strcpy(pStageSledge->tool_desc, me->tool_desc);
	strcpy(pStageSledge->tool_msg, me->tool_msg);
	Unlock(me);

Error:
	return NULL;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Load"
static SElExceptionPtr Load(
	struct _SSledge* me,
	const char*	file_path
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle		ptool_list = 0;
	char			stmp[STAGE_ITEM_LENGTH+1] = "";

	/* create copy */
	XML_GET_DOC_NODE(file_path, "//testseq", me->pxml);
	strcpy(me->xml_file_path, file_path);

	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (me->pxml, NULL, "//tool", &ptool_list));
	if(ptool_list)
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength(ptool_list, NULL, &me->step_nb));

	XML_GET_NODE_STRING(me->pxml, "//parameter[name='ProductPartNumber']/value", me->version, STAGE_ITEM_LENGTH);
	
	XML_GET_NODE_STRING(me->pxml, "//product/id", stmp, STAGE_ITEM_LENGTH);
	me->product_id = strtoul(stmp, NULL, 10);
	
	if(g_last_product_id!=me->product_id)
	{
		/* reset all background tool threads */
		me->reset_bck_tool = TRUE;

		g_last_product_id = me->product_id;
	}

Error:
	if(ptool_list) CA_DiscardObjHandle(ptool_list);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Unload"
static SElExceptionPtr Unload(struct _SSledge* me)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;

	/* save xml */
	me->xml_file_path[0] = '\0';
	if(me->pxml)
	{
		CA_DiscardObjHandle(me->pxml);
		me->pxml = 0;
	}
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "Activate"
static SElExceptionPtr Activate(struct _SSledge* me, const char* step)
{
	SElExceptionPtr pexception = NULL;

	if (me->_hThread != NULL)
	{
		char	serror[256];

		sprintf(serror, "Sledge was allready activated in position '%s'", me->step);
		EXCTHROW( -1, serror);
	}

	strncpy(me->step, step, STAGE_ITEM_LENGTH);
	EXCCHECK( ThreadStart(Thread_Sledge, me, &me->_hThread, &me->_hEventStop));

Error:   
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "Deactivate"
static SElExceptionPtr Deactivate(struct _SSledge* me)
{
	SElExceptionPtr pexception = NULL;

	EXCCHECK( ThreadStop(&me->_hThread, &me->_hEventStop));

Error:   
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "WaitForState"
static SElExceptionPtr WaitForState(
	struct _SSledge*		me,
	uint32_t			timeout,
	ETooolState			*state
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			status;
	
	pexception = ThreadStatus(me->_hThread, timeout, &status);
	if(pexception) /* timeout */
	{
		EXCDELETE(pexception);
		if(state) *state = ETS_ACTIVE;
	}
	else
	{
		switch(status)
		{
			case THREAD_STATUS_STILL_ACTIVE:
				if(state) *state = ETS_ACTIVE;
				break;
			case THREAD_STATUS_EXIT_CODE:
				if(state) *state = ETS_EXIT_OK;
				break;
			case THREAD_STATUS_ERROR_CODE:
				if(state) *state = ETS_EXIT_ERROR;
				break;
		}
	}

Error:   
	EXCRETHROW( pexception);  
}

/*----------------------------------------------------------------------------*/
typedef enum _ESledgeStep
{
	SS_START,
	SS_TOOL,
	SS_TOOL_RESULT,
	SS_END
} ESledgeStep;

#define LOOP_TIME_MS	20
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Sledge"
static DWORD WINAPI Thread_Sledge(LPVOID param)
{
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	SSledgePtr		me = (SSledgePtr)param;
	bool_t			end_seq = FALSE;
	ESledgeStep		step = SS_START;
	CAObjHandle		ptool_list = 0,
					ptool = 0;
	char			name[STAGE_ITEM_LENGTH+1],
					desc[STAGE_ITEM_LENGTH+1],
					assync[STAGE_ITEM_LENGTH+1],
					cnt[STAGE_ITEM_LENGTH+1];
	uint32_t		icnt;
	ERRORINFO		errorInfo = {0};
	char			path[256];
	SToolClassPtr	pTool[255];
	ETooolState		status[255];
	char			*ToolName[255];
	uint32_t		ToolCnt = 0, i;
	long			list_length = 0;

	//LOG(TRACE, "[%s] Start", me->step);   //ARAIBIA

	/* startup */
	Lock(me);
	me->tool_name[0] = '\0';
	me->tool_desc[0] = '\0';
	me->tool_msg[0] = '\0';
	Unlock(me);

	sprintf(path, "//%s/tool", me->step);
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (me->pxml, &errorInfo, path, &ptool_list));
	if(ptool_list)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset(ptool_list, NULL));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength(ptool_list, NULL, &list_length));
	}

	while(!end_seq)
	{
		if (WaitForSingleObject(me->_hEventStop, LOOP_TIME_MS) == WAIT_OBJECT_0)
		{
			/* cleanup actual tool */
			for(i=0; i<ToolCnt; i++)
			{
				EXCCHECK( ToolDeactivate(me, pTool[i]));
			}

			ExitThread(THREAD_EXIT_CODE);
		}

		/*
		 * Process:
		 */
		switch(step)
		{
			case SS_START:
				if(me->State==ESS_NOK)
				{
					//LOG(TRACE, "[%s] NOK", me->step); //ARAIBIA

					Lock(me);
					strncpy(me->tool_name, me->step, STAGE_ITEM_LENGTH);
					strncpy(me->tool_msg, "NOK", STAGE_ITEM_LENGTH);
					Unlock(me);
				}
				step++;
				break;
			case SS_TOOL:
				if (ptool) 
				{
					CA_DiscardObjHandle(ptool);
					ptool = 0;
				}

				EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(ptool_list, NULL, &ptool));
				if(ptool)
				{
					me->step_actual++;
					XML_GET_NODE_STRING(ptool, "@name", name, STAGE_ITEM_LENGTH);

					*assync = '\0';
					XML_GET_NODE_STRING(ptool, "@assync", assync, STAGE_ITEM_LENGTH);
					if(*assync) /* run or stop tools in the background */
					{
						/* start with new parameters after product change */
						if(me->reset_bck_tool)
						{
							SToolClassPtr p2tool = (SToolClassPtr)gs_Tools->GetTool(gs_Tools, name);

							/* stop */
							EXCCHECK( ToolRun(me, name, FALSE));

							EXCCHECK( p2tool->AppendConfigFromXMLNode(p2tool, ptool));
						}

						if(0==strcmp(assync, "true")) /* run */
							EXCCHECK( ToolRun(me, name, TRUE));
						else /* stop */
							EXCCHECK( ToolRun(me, name, FALSE));

						step = SS_TOOL;
					}
					else if(me->State==ESS_NOK)
					{
						end_seq = TRUE;
					}
					else /* activate tool and wait for result */
					{
						XML_GET_NODE_STRING(ptool, "@desc", desc, STAGE_ITEM_LENGTH);

						*cnt = '\0';
						XML_GET_NODE_STRING(ptool, "@cnt", cnt, STAGE_ITEM_LENGTH);
						if(*cnt)
							icnt = strtoul(cnt, NULL, 10);
						else
							icnt = 1;

						Lock(me);
						strncpy(me->tool_name, name, STAGE_ITEM_LENGTH);
						strncpy(me->tool_desc, desc, STAGE_ITEM_LENGTH);
						*me->tool_msg = '\0';
						Unlock(me);

						/* Activate more then one tool */
						{
							char	*pdata = strtok(name, ";");
							
							ToolCnt = 0;

							while(pdata && *pdata)
							{
								SToolClassPtr p2tool = (SToolClassPtr)gs_Tools->GetTool(gs_Tools, pdata);
								
								if(p2tool)
								{
									/* set tool data */
									EXCCHECK( p2tool->AppendConfigFromXMLNode(p2tool, ptool));

									ToolName[ToolCnt] = pdata;
									EXCCHECK( ToolActivate(me, pdata, &pTool[ToolCnt]));
									ToolCnt++;
								}
								else
								{
									char serror[256];

									sprintf(serror, "Tool '%s' not found!", pdata);
									EXCTHROW(-1, serror);
								}

								pdata = strtok(NULL, ";");
							}
							memset(&status, 0, sizeof(ETooolState)*255); 
						}
						step++;
					}
				}
				else
					end_seq = TRUE;
				break;
			case SS_TOOL_RESULT:
				{
					uint32_t	ToolFinishedCnt = 0;

					for(i=0, ToolFinishedCnt=0; i<ToolCnt; i++)
					{
						if(status[i]==ETS_ACTIVE)
						{
							EXCCHECK( ToolWaitForState(me, pTool[i], &status[i]));
							if(status[i]==ETS_EXIT_OK || status[i]==ETS_EXIT_ERROR)
								EXCCHECK( ToolDeactivate(me, pTool[i]));
						}
						else
							ToolFinishedCnt++;
					}

					if(ToolCnt==ToolFinishedCnt) 
					{	
						for(i=0; i<ToolCnt; i++)
						{
							if(status[i]==ETS_EXIT_ERROR)
							{
								end_seq = TRUE;
								me->State = ESS_NOK;
								break;
							}
						}
						if(i==ToolCnt) /* ETS_EXIT_OK */
						{
							if(icnt>1)
							{
								for(i=0; i<ToolCnt; i++)
									EXCCHECK( ToolActivate(me, ToolName[i], &pTool[i]));

								memset(&status, 0, sizeof(ETooolState)*255);

								icnt--;
							}
							else
								step++;
						}
					}
				}
				break;
			case SS_END:
				step = SS_START;
				break;
		}
	}

	/* cleanup */
	if (ptool) 
		CA_DiscardObjHandle(ptool);
	if (ptool_list) 
		CA_DiscardObjHandle(ptool_list);

Error:
	if(pexception)
	{
		//LOG(ERR, "[%s] %s", me->step, pexception->GetErrorMsgPtr(pexception));  //ARAIBIA 
		EXCDELETE( pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
	{
		//LOG(TRACE, "[%s] Stop", me->step); //ARAIBIA
		ExitThread(THREAD_EXIT_CODE);
	}
}

#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\banner\banner.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\airpump\airpump.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\ionizer\ionizer.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\MPD\mpd.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\barcode\barcode.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\grease\grease.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\grease2\grease2.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\wait\wait.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\screwdriver\screwdriver.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\boxgroup\boxgroup.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\detection\detection.h"
#include "c:\work\ELCOM\Valeo\UTSystem\src\modules\line_tools\pcbprepare\pcbprepare.h"
/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ToolActivate"
static SElExceptionPtr ToolActivate(struct _SSledge* me, const char* name, SToolClassPtr* pTool)
{
	SElExceptionPtr pexception = NULL;
	SToolClassPtr	ptool = NULL;
	EToolType		tool_type = ETT_NONE;

	ptool = (SToolClassPtr)gs_Tools->GetTool(gs_Tools, name);

	if(ptool)
		tool_type = *(EToolType*)ptool;

	switch(tool_type)
	{
		case ETT_BANNER:
			{
				SBannerPtr pbanner = (SBannerPtr)ptool;
				EXCCHECK( pbanner->Activate(pbanner));
			}
			break;
		case ETT_AIRPUMP:
			{
				SAirpumpPtr pairpump = (SAirpumpPtr)ptool;
				EXCCHECK( pairpump->Activate(pairpump));
			}
			break;
		case ETT_IONIZER:
			{
				SIonizerPtr pionizer = (SIonizerPtr)ptool;
				EXCCHECK( pionizer->Activate(pionizer));
			}
			break;
		case ETT_MPD:
			{
				SMpdPtr pmpd = (SMpdPtr)ptool;
				EXCCHECK( pmpd->Activate(pmpd));
			}
			break;
		case ETT_BARCODE:
			{
				SBarcodePtr pbarcode = (SBarcodePtr)ptool;
				EXCCHECK( pbarcode->Activate(pbarcode));
			}
			break;
		case ETT_GREASE:
			{
				SGreasePtr pgrease = (SGreasePtr)ptool;
				EXCCHECK( pgrease->Activate(pgrease));
			}
			break;
		case ETT_GREASE2:
			{
				SGrease2Ptr pgrease = (SGrease2Ptr)ptool;
				EXCCHECK( pgrease->Activate(pgrease));
			}
			break;
		case ETT_WAIT:
			{
				SWaitPtr pwait = (SWaitPtr)ptool;
				EXCCHECK( pwait->Activate(pwait));
			}
			break;
		case ETT_SCREWDRIVER:
			{
				SScrewdriverPtr pscrewdriver = (SScrewdriverPtr)ptool;
				EXCCHECK( pscrewdriver->Activate(pscrewdriver));
			}
			break;
		case ETT_DETECTION:
			{
				SDetectionPtr pdetection = (SDetectionPtr)ptool;
				EXCCHECK( pdetection->Activate(pdetection));
			}
			break;
		case ETT_PCBPREPARE:
			{
				SPCBPreparePtr ppcb = (SPCBPreparePtr)ptool;
				EXCCHECK( ppcb->Activate(ppcb));
			}
			break;
		default:
			{
				char serror[256];

				sprintf(serror, "Tool '%s' not found!", name);
				EXCTHROW(-1, serror);
			}
			break;
	}

Error:   
	if(pTool) *pTool = ptool;
	EXCRETHROW( pexception);
}

#define WAIT_FOR_STATE_TIMEOUT		200
/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ToolWaitForState"
static SElExceptionPtr ToolWaitForState(struct _SSledge* me, SToolClassPtr ptool, ETooolState* pStatus)
{
	SElExceptionPtr pexception = NULL;
	EToolType		tool_type = ETT_NONE;
	ETooolState		status = ETS_EXIT_ERROR;
	char			msg[STAGE_ITEM_LENGTH+1] = "";

	if(ptool)
		tool_type = *(EToolType*)ptool;

	switch(tool_type)
	{
		case ETT_BANNER:
			{
				SBannerPtr pbanner = (SBannerPtr)ptool;
				EXCCHECK( pbanner->WaitForState(pbanner, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pbanner->GetStatus(pbanner, msg));
			}
			break;
		case ETT_AIRPUMP:
			{
				SAirpumpPtr pairpump = (SAirpumpPtr)ptool;
				EXCCHECK( pairpump->WaitForState(pairpump, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pairpump->GetStatus(pairpump, msg));
			}
			break;
		case ETT_IONIZER:
			{
				SIonizerPtr pionizer = (SIonizerPtr)ptool;
				EXCCHECK( pionizer->WaitForState(pionizer, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pionizer->GetStatus(pionizer, msg));
			}
			break;
		case ETT_MPD:
			{
				SMpdPtr pmpd = (SMpdPtr)ptool;
				EXCCHECK( pmpd->WaitForState(pmpd, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pmpd->GetStatus(pmpd, msg));
			}
			break;
		case ETT_BARCODE:
			{
				SBarcodePtr pbarcode = (SBarcodePtr)ptool;
				EXCCHECK( pbarcode->WaitForState(pbarcode, WAIT_FOR_STATE_TIMEOUT, &status));				
				EXCCHECK( pbarcode->GetStatus(pbarcode, msg));
			}
			break;
		case ETT_GREASE:
			{
				SGreasePtr pgrease = (SGreasePtr)ptool;
				EXCCHECK( pgrease->WaitForState(pgrease, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pgrease->GetStatus(pgrease, msg));
			}
			break;
		case ETT_GREASE2:
			{
				SGrease2Ptr pgrease = (SGrease2Ptr)ptool;
				EXCCHECK( pgrease->WaitForState(pgrease, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pgrease->GetStatus(pgrease, msg));
			}
			break;
		case ETT_WAIT:
			{
				SWaitPtr pwait = (SWaitPtr)ptool;
				EXCCHECK( pwait->WaitForState(pwait, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pwait->GetStatus(pwait, msg));
			}
			break;
		case ETT_SCREWDRIVER:
			{
				SScrewdriverPtr pscrewdriver = (SScrewdriverPtr)ptool;
				EXCCHECK( pscrewdriver->WaitForState(pscrewdriver, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pscrewdriver->GetStatus(pscrewdriver, msg));
			}
			break;
		case ETT_DETECTION:
			{
				SDetectionPtr pdetection = (SDetectionPtr)ptool;
				EXCCHECK( pdetection->WaitForState(pdetection, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( pdetection->GetStatus(pdetection, msg));
			}
			break;
		case ETT_PCBPREPARE:
			{
				SPCBPreparePtr ppcb = (SPCBPreparePtr)ptool;
				EXCCHECK( ppcb->WaitForState(ppcb, WAIT_FOR_STATE_TIMEOUT, &status));
				EXCCHECK( ppcb->GetStatus(ppcb, msg));
			}
			break;		
		default:
			EXCTHROW(-1, "Tool not found!");
			break;
	}

	if(*msg)
	{
		Lock(me);
		strcpy(me->tool_msg, msg);
		Unlock(me);
	}

Error:   
	if(pStatus) *pStatus = status;
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ToolDeactivate"
static SElExceptionPtr ToolDeactivate(struct _SSledge* me, SToolClassPtr ptool)
{
	SElExceptionPtr pexception = NULL;
	EToolType		tool_type = ETT_NONE;

	if(ptool)
		tool_type = *(EToolType*)ptool;

	switch(tool_type)
	{
		case ETT_BANNER:
			{
				SBannerPtr pbanner = (SBannerPtr)ptool;
				EXCCHECK( pbanner->Deactivate(pbanner));
			}
			break;
		case ETT_AIRPUMP:
			{
				SAirpumpPtr pairpump = (SAirpumpPtr)ptool;
				EXCCHECK( pairpump->Deactivate(pairpump));
			}
			break;
		case ETT_IONIZER:
			{
				SIonizerPtr pionizer = (SIonizerPtr)ptool;
				EXCCHECK( pionizer->Deactivate(pionizer));
			}
			break;
		case ETT_MPD:
			{
				SMpdPtr pmpd = (SMpdPtr)ptool;
				EXCCHECK( pmpd->Deactivate(pmpd));
			}
			break;
		case ETT_BARCODE:
			{
				SBarcodePtr pbarcode = (SBarcodePtr)ptool;
				EXCCHECK( pbarcode->Deactivate(pbarcode));
			}
			break;
		case ETT_GREASE:
			{
				SGreasePtr pgrease = (SGreasePtr)ptool;
				EXCCHECK( pgrease->Deactivate(pgrease));
			}
			break;
		case ETT_GREASE2:
			{
				SGrease2Ptr pgrease = (SGrease2Ptr)ptool;
				EXCCHECK( pgrease->Deactivate(pgrease));
			}
			break;
		case ETT_WAIT:
			{
				SWaitPtr pwait = (SWaitPtr)ptool;
				EXCCHECK( pwait->Deactivate(pwait));
			}
			break;
		case ETT_SCREWDRIVER:
			{
				SScrewdriverPtr pscrewdriver = (SScrewdriverPtr)ptool;
				EXCCHECK( pscrewdriver->Deactivate(pscrewdriver));
			}
			break;
		case ETT_DETECTION:
			{
				SDetectionPtr pdetection = (SDetectionPtr)ptool;
				EXCCHECK( pdetection->Deactivate(pdetection));
			}
			break;
		case ETT_PCBPREPARE:
			{
				SPCBPreparePtr ppcb = (SPCBPreparePtr)ptool;
				EXCCHECK( ppcb->Deactivate(ppcb));
			}
			break;
		default:
			EXCTHROW(-1, "Tool not found!");
			break;
	}

Error:   
	if(pexception)
	{
		//LOG(ERR, "[%s] %s Deactivate, %s", me->step, SToolType[tool_type], pexception->GetErrorMsgPtr(pexception));  //ARAIBIA
		EXCDELETE( pexception);
	}
	return NULL;
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "ToolRun"
static SElExceptionPtr ToolRun(struct _SSledge* me, const char* name, bool_t run)
{
	SElExceptionPtr pexception = NULL;
	SToolClassPtr	ptool = NULL;
	EToolType		tool_type = ETT_NONE;

	ptool = (SToolClassPtr)gs_Tools->GetTool(gs_Tools, name);

	if(ptool)
		tool_type = *(EToolType*)ptool;

	switch(tool_type)
	{
		case ETT_AIRPUMP:
			{
				SAirpumpPtr pairpump = (SAirpumpPtr)ptool;

				if(run && !pairpump->_ThreadBckg)
					EXCCHECK( pairpump->Run(pairpump));
				else if(!run && pairpump->_ThreadBckg)
					EXCCHECK( pairpump->Stop(pairpump));
			}
			break;
		case ETT_IONIZER:
			{
				SIonizerPtr pionizer = (SIonizerPtr)ptool;

				if(run && !pionizer->_ThreadBckg)
					EXCCHECK( pionizer->Run(pionizer));
				else if(!run && pionizer->_ThreadBckg)
					EXCCHECK( pionizer->Stop(pionizer));
			}
			break;
		case ETT_GREASE:
			{
				SGreasePtr pgrease = (SGreasePtr)ptool;

				if(run && !pgrease->_ThreadBckg)
					EXCCHECK( pgrease->Run(pgrease));
				else if(!run && pgrease->_ThreadBckg)
					EXCCHECK( pgrease->Stop(pgrease));
			}
			break;
		case ETT_GREASE2:
			{
				SGrease2Ptr pgrease = (SGrease2Ptr)ptool;

				if(run && !pgrease->_ThreadBckg)
					EXCCHECK( pgrease->Run(pgrease));
				else if(!run && pgrease->_ThreadBckg)
					EXCCHECK( pgrease->Stop(pgrease));
			}
			break;
		case ETT_PCBPREPARE:
			{
				SPCBPreparePtr pgrease = (SPCBPreparePtr)ptool;

				if(run && !pgrease->_ThreadBckg)
					EXCCHECK( pgrease->Run(pgrease));
				else if(!run && pgrease->_ThreadBckg)
					EXCCHECK( pgrease->Stop(pgrease));
			}
			break;
		case ETT_BOXGROUP:
			{
				SBoxgroupPtr pboxgroup = (SBoxgroupPtr)ptool;

				if(run && !pboxgroup->_ThreadBckg)
					EXCCHECK( pboxgroup->Run(pboxgroup));
				else if(!run && pboxgroup->_ThreadBckg)
					EXCCHECK( pboxgroup->Stop(pboxgroup));
			}
			break;
		default:
			{
				char serror[256];

				sprintf(serror, "Tool '%s' not found!", name);
				EXCTHROW(-1, serror);
			}
			break;
	}

Error:   
	EXCRETHROW( pexception);  
}
