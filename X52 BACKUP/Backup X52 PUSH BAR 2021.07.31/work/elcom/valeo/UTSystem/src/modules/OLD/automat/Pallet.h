#if !defined(__STAGE_H__)
#define __STAGE_H__

#include <exception\exception.h>
#include <windows.h>
#include <line_tools\tool_class.h>
#include <cviauto.h>

#define _STAGE_SIMULATION	0

#define STAGE_ITEM_LENGTH	255

typedef enum _ESledgeState
{
	ESS_NOK,
	ESS_OK
} ESledgeState;

/* GetStatus */
typedef struct _SSledgeStatus
{
	uint32_t		id;
	ESledgeState	State;

	double			progress; /* 0 - 100% */
	char			version[STAGE_ITEM_LENGTH+1];

	char			tool_name[STAGE_ITEM_LENGTH+1];
	char			tool_desc[STAGE_ITEM_LENGTH+1];
	char			tool_msg[STAGE_ITEM_LENGTH+1];
} SSledgeStatus, *SSledgeStatusPtr;

typedef struct _SSledge
{
	/* xml functions */
	SElExceptionPtr (*Load)(struct _SSledge* me, const char* xml_file_path);
	SElExceptionPtr (*Unload)(struct _SSledge* me);

	/* process xml sequence functions */
	SElExceptionPtr	(*Activate)(struct _SSledge* me, const char* step);
	SElExceptionPtr	(*Deactivate)(struct _SSledge* me);
	SElExceptionPtr	(*WaitForState)(struct _SSledge* me, uint32_t timeout, ETooolState *state);

	SElExceptionPtr	(*GetStatus)(struct _SSledge* me, SSledgeStatusPtr pStageSledge);

	/*************************************************************************/
	/* Private
	/*************************************************************************/

	ESledgeState	State;
	uint32_t		id;
	uint8_t			sledge_id[STAGE_ITEM_LENGTH+1]; /* rfid */
	char			xml_file_path[STAGE_ITEM_LENGTH+1];
	CAObjHandle		pxml;
	uint32_t		step_nb; /* number of all steps <tool /> */
	uint32_t		step_actual;
	uint32_t		product_id;
	char			version[STAGE_ITEM_LENGTH+1]; /* ProductPartNumber */
	char			step[STAGE_ITEM_LENGTH+1]; /* xml section <WS1 />, <WS2 />, <WS3 /> */
	char			tool_name[STAGE_ITEM_LENGTH+1]; /* @name */
	char			tool_desc[STAGE_ITEM_LENGTH+1]; /* @desc */
	char			tool_msg[STAGE_ITEM_LENGTH+1];
	bool_t			reset_bck_tool; /* reset all running background tools */

	HANDLE				_hThread;
	HANDLE				_hEventStop;
	CRITICAL_SECTION	_Lock;
	void*				LogApi;
	uint32_t			LogChannel;

} SSledge, *SSledgePtr;

SElExceptionPtr sledge_new(SSledgePtr* pSledge);
SElExceptionPtr sledge_delete(SSledgePtr* pSledge);

#endif /* __STAGE_H__ */
