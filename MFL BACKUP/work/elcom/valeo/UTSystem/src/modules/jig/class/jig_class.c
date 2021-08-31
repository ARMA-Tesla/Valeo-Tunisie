
#include "jig_class.h"
#include "msxmldom.h"
#include <userint.h>
#include <stdio.h>
#include <utility.h>
#include <string.h>
#include <stdlib.h>
#include <language/language.h>
#include <multithread/multithread.h>
#include <time.h>
#include <sensors/sensors.h>
#include <dbs/dbs.h>

#define FILE_NAME_SIZE					2048
#define JIG_IDENTIFICATION_FILE			"jig.cfg"
#define	JIG_IDENTIFICATION_TIMEOUT		5000  /* timeout in ms */

SLanguagePtr		LANGUAGE_PTR;

static DWORD WINAPI Thread_Jig(LPVOID param);
SElExceptionPtr initializeJIG( SJigPtr* pJig);

static SElExceptionPtr jigLoad(
	struct _SJig*	me,
	const char*		file
);
static SElExceptionPtr jigUnload(struct _SJig* me);
static SElExceptionPtr jigLockItem(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
static SElExceptionPtr jigUnlockItem(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
static SElExceptionPtr jigConnectToItem(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
static SElExceptionPtr jigDisconnectFromItem(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
static SElExceptionPtr jigColdMarking(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
static SElExceptionPtr jigSetItemPosition(
	struct _SJig*	me,
	const char*		product,
	int32_t			position,
	int32_t			timeout
);
static SElExceptionPtr jigGetItemPosition(
	struct _SJig*	me,
	const char*		product,
	int32_t*		actualPosition
);
static SElExceptionPtr jigGetItemCode(
	struct _SJig*	me,
	const char*		product,
	int32_t			index,
	long			bufferSize,
	char*			buffer,
	int32_t			timeout
);
static SElExceptionPtr jigGetSensor(
	struct _SJig*	me,
	const char*		sensorId,
	uint32_t*		value
);
static SElExceptionPtr jigSetSensor(
	struct _SJig*	me,
	const char*		sensorId,
	uint32_t		value
);
static SElExceptionPtr jigGetAttributeInt32(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int32_t*		value
);
static SElExceptionPtr jigSetAttributeInt32(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int32_t			value
);
static SElExceptionPtr jigGetAttributeBoolean(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int8_t*			value
);
static SElExceptionPtr jigSetAttributeBoolean(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int8_t			value
);
static SElExceptionPtr jigGetUUID(
	struct _SJig* me, 
	char* uuid, 
	uint32_t size
);
static SElExceptionPtr jigSetEventFactory(
	struct _SJig*		me,
	SElEventFactoryPtr	eventAPI
);
static SElExceptionPtr jigSetSensorAPI(
	struct _SJig*		me,
	void*				sensorAPI
);
static SElExceptionPtr jigSetDatabaseAPI(
	struct _SJig*		me,
	void*				sensorAPI
);
static SElExceptionPtr LetterFromDriveLabel(
	const char *Label,
	char drive_letter[],
	int32_t driveSize);
static SElExceptionPtr IDFileCheck(
	const char *root,
	uint32_t	*pID
);

static SElExceptionPtr jigThread(struct _SJig*	me);
static SElExceptionPtr detectJigAndLoad(SJigPtr me, uint8_t jigCode);

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__  "jig_new"
SElExceptionPtr	jig_new( SJigPtr* pJig)
{
	SElExceptionPtr    pexception = NULL;

	*pJig = (SJigPtr)calloc( 1, sizeof(SJig));
	EXCCHECKALLOC( *pJig);
	EXCCHECK( initializeJIG( pJig));

	EXCCHECK( ThreadStart( Thread_Jig, *pJig, &((*pJig)->hThread), &((*pJig)->hEventStop)));

Error:
	if ( pexception )
	{
		jig_delete( pJig);
	}
	EXCRETHROW( pexception);
} /* user_new */


/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__  "jig_delete"
SElExceptionPtr	jig_delete( SJigPtr* pJig)
{
	SElExceptionPtr    pexception = NULL;

	if ( pJig && *pJig )
	{
		EXCCHECK( ThreadStop( &((*pJig)->hThread), &((*pJig)->hEventStop)));

		if( (*pJig)->jigLoaded)
		{
			(*pJig)->eventFactory = NULL;
			EXCCHECK( (*pJig)->jig_Unload(*pJig)); 
		}

		free(*pJig);
		*pJig = NULL;
	}
Error:
	EXCRETHROW( pexception);
} /* user_delete */


/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__  "initializeJIG"
SElExceptionPtr initializeJIG( SJigPtr* pJig)
{
	SElExceptionPtr    pexception = NULL;

	(*pJig)->jig_Load			= jigLoad;
	(*pJig)->jig_Unload			= jigUnload;
	(*pJig)->jig_LockItem		= jigLockItem;
	(*pJig)->jig_UnlockItem		= jigUnlockItem;
	(*pJig)->jig_ConnectToItem	= jigConnectToItem;
	(*pJig)->jig_DisconnectFromItem= jigDisconnectFromItem;
	(*pJig)->jig_ColdMarking	= jigColdMarking;
	(*pJig)->jig_SetItemPosition= jigSetItemPosition;
	(*pJig)->jig_GetItemPosition= jigGetItemPosition;
	(*pJig)->jig_GetItemCode	= jigGetItemCode;
	(*pJig)->jig_GetSensor		= jigGetSensor;
	(*pJig)->jig_SetSensor		= jigSetSensor;
	(*pJig)->jig_SetAttributeInt32= jigSetAttributeInt32;
	(*pJig)->jig_GetAttributeInt32= jigGetAttributeInt32;
	(*pJig)->jig_SetAttributeBoolean= jigSetAttributeBoolean;
	(*pJig)->jig_GetAttributeBoolean= jigGetAttributeBoolean;
	(*pJig)->jig_GetUUID = jigGetUUID;
	(*pJig)->jig_SetEventFactory= jigSetEventFactory;
	(*pJig)->jig_SetSensorAPI = jigSetSensorAPI;
	(*pJig)->jig_SetDatabaseAPI = jigSetDatabaseAPI;
	(*pJig)->_JigThread			= jigThread;
	
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__  "<JigClass>::Load"
static SElExceptionPtr jigLoad(
	struct _SJig*	me,
	const char*		file
)
{
	SElExceptionPtr	pexception = NULL;
	SElEventDataPtr	pevent_data = NULL; 
	int32_t			error = 0;
	char			module_dir[2048] = "";
	char			file_name[2048] = "";
	char			file_cpy[512];
	
	EXCCHECKCVI( GetModuleDir (__CVIUserHInst, module_dir));
	sprintf(file_name, "%s\\%s", module_dir, file);
	
	me->moduleId = LoadLibrary(file_name);
	if (me->moduleId == NULL)
	{
		EXCTHROW( JIG_ERROR_LOAD_LIBRARY, "Unable load specific library");
	}
	
	me->_InitializeSpec = (SElExceptionPtr (*)(SJigPtr,const char*))
						   GetProcAddress( me->moduleId, "Initialize");
	me->_DestroySpec = (SElExceptionPtr (*)(SJigPtr))
						GetProcAddress( me->moduleId, "Destroy");
	me->_GetUUID = (SElExceptionPtr (*)(SJigPtr,char*,uint32_t))
						GetProcAddress( me->moduleId, "GetUUID");
	me->_GetAttributeInt32Spec = 
			(SElExceptionPtr (*)(SJigPtr,const char*,int32_t,int32_t*))
								GetProcAddress( me->moduleId,
												"GetAttributeInt32");
	me->_SetAttributeInt32Spec =
			(SElExceptionPtr (*)(SJigPtr,const char*,int32_t,int32_t))
								GetProcAddress( me->moduleId,
												 "SetAttributeInt32");
	me->_GetAttributeBooleanSpec = 
			(SElExceptionPtr (*)(SJigPtr,const char*,int32_t,int8_t*))
								GetProcAddress( me->moduleId,
												"GetAttributeBoolean");
	me->_SetAttributeBooleanSpec = 
			(SElExceptionPtr (*)(SJigPtr,const char*,int32_t,int8_t))
								GetProcAddress( me->moduleId,
												"SetAttributeBoolean");
	me->_LockItemSpec = (SElExceptionPtr (*)(SJigPtr,const char*,int32_t))
								GetProcAddress( me->moduleId, "LockItem");
	me->_UnlockItemSpec = (SElExceptionPtr (*)(SJigPtr,const char*,int32_t))
								GetProcAddress( me->moduleId, "UnlockItem");
	me->_ConnectToItemSpec = (SElExceptionPtr (*)(SJigPtr,const char*,int32_t))
								GetProcAddress( me->moduleId, "ConnectToItem");
	me->_DisconnectFromItemSpec = (SElExceptionPtr (*)(SJigPtr, const char*,int32_t))
								GetProcAddress( me->moduleId, "DisconnectFromItem");
	me->_ColdMarkingSpec = (SElExceptionPtr (*)(SJigPtr, const char*,int32_t))
								GetProcAddress( me->moduleId, "ColdMarking");
	me->_SetItemPositionSpec = (SElExceptionPtr (*)(SJigPtr,const char*,int32_t,int32_t))
								GetProcAddress( me->moduleId, "SetItemPosition");
	me->_GetItemPositionSpec = (SElExceptionPtr (*)(SJigPtr,const char*,int32_t*))
								GetProcAddress( me->moduleId, "GetItemPosition");
	me->_GetItemCodeSpec = (SElExceptionPtr (*)(SJigPtr,const char*,int32_t,long,char*,int32_t))
								GetProcAddress( me->moduleId, "GetItemCode");
	me->_GetSensorSpec = (SElExceptionPtr (*)(SJigPtr,const char*,uint32_t*))
								GetProcAddress( me->moduleId, "GetSensor");
	me->_SetSensorSpec = (SElExceptionPtr (*)(SJigPtr,const char*,uint32_t))
								GetProcAddress( me->moduleId, "SetSensor");
	me->jigLoaded = 1;
	if (me->_InitializeSpec != NULL)
	{
		char* p2ext = NULL;

		strcpy( file_cpy, file);
		p2ext = strrchr(file_cpy, '.');
		if (p2ext)
			*p2ext = '\0';

		sprintf( me->path, "%s\\%s.xml", module_dir, file_cpy);

		EXCCHECK( me->_InitializeSpec(me, me->path));
		EXCCHECK( me->_SetAttributeInt32Spec(me, "", JIG_ATTR_HWND, (int32_t)hWnd));
	}
	
	me->_JigThreadSpec = (SElExceptionPtr (*)(SJigPtr))
								GetProcAddress( me->moduleId, "JigThread");

	if (me->eventFactory)
	{
		char	uuid[512];

		EXCCHECK( me->_GetUUID(me, uuid, 512));
		EXCCHECK( eleventdata_new(&pevent_data));
		EXCCHECK( pevent_data->SetStringPtr(pevent_data, uuid));
		EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_JIG_LOADED, pevent_data));
	}

	/* set insert position */
	EXCCHECK( me->jig_SetItemPosition(me, "", JIG_POSITION_INSERT, 2000));
	
Error:
	eleventdata_delete(&pevent_data);
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::Unload"
static SElExceptionPtr jigUnload(struct _SJig* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			uuid[512];
	SElEventDataPtr	pevent_data = NULL; 

	EXCCHECK( me->_GetUUID(me, uuid, 512));

	EXCCHECK( me->_DestroySpec(me));
	
	me->_InitializeSpec = NULL;
	me->_DestroySpec = NULL;
	me->_GetUUID = NULL;
	me->_GetAttributeInt32Spec = NULL;
	me->_SetAttributeInt32Spec = NULL;
	me->_GetAttributeBooleanSpec = NULL;
	me->_SetAttributeBooleanSpec = NULL;
	me->_LockItemSpec = NULL;
	me->_UnlockItemSpec = NULL;
	me->_ConnectToItemSpec = NULL;
	me->_DisconnectFromItemSpec = NULL;
	me->_SetItemPositionSpec = NULL;
	me->_GetItemPositionSpec = NULL;
	me->_GetItemCodeSpec = NULL;
	me->_GetSensorSpec = NULL;
	me->_SetSensorSpec = NULL;
	me->_JigThreadSpec = NULL;

	if (me->moduleId)
		EXCCHECKWIN( FreeLibrary(me->moduleId));
	
	me->jigLoaded = 0;

	if (me->eventFactory)
	{		
		EXCCHECK( eleventdata_new(&pevent_data));
		EXCCHECK( pevent_data->SetStringPtr(pevent_data, uuid)); 
		EXCCHECK( me->eventFactory->PulseEvent(me->eventFactory, EVNT_JIG_UNLOADED, pevent_data));
	}

Error:
	eleventdata_delete(&pevent_data);
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::LockItem"
static SElExceptionPtr jigLockItem(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr    pexception = NULL;
	
	if (me->_LockItemSpec)
		EXCCHECK( me->_LockItemSpec(me, product, timeout));

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::UnlockItem"
static SElExceptionPtr jigUnlockItem(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr    pexception = NULL;

	if (me->_UnlockItemSpec)
		EXCCHECK( me->_UnlockItemSpec(me, product, timeout));
	
Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::ConnectToItem"
static SElExceptionPtr jigConnectToItem(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr    pexception = NULL;

	if (me->_ConnectToItemSpec)
		EXCCHECK( me->_ConnectToItemSpec(me, product, timeout));
	
Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::DisconnectFromItem"
static SElExceptionPtr jigDisconnectFromItem(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr    pexception = NULL;

	if (me->_DisconnectFromItemSpec)
		EXCCHECK( me->_DisconnectFromItemSpec(me, product, timeout));
	
Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::jigColdMarking"
static SElExceptionPtr jigColdMarking(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
)
{
	SElExceptionPtr    pexception = NULL;

	if (me->_ColdMarkingSpec)
		EXCCHECK( me->_ColdMarkingSpec(me, product, timeout));
	
Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::SetItemPosition"
static SElExceptionPtr jigSetItemPosition(
	struct _SJig*	me,
	const char*		product,
	int32_t			position,
	int32_t			timeout
)
{
	SElExceptionPtr    pexception = NULL;

	if (me->_SetItemPositionSpec)
		EXCCHECK( me->_SetItemPositionSpec(me, product, position, timeout));
	
Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::GetItemPosition"
static SElExceptionPtr jigGetItemPosition(
	struct _SJig*	me,
	const char*		product,
	int32_t*		actualPosition
)
{
	SElExceptionPtr    pexception = NULL;

	if (me->_GetItemPositionSpec)
		EXCCHECK( me->_GetItemPositionSpec(me, product, actualPosition));
	
Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::GetItemCode"
static SElExceptionPtr jigGetItemCode(
	struct _SJig*	me,
	const char*		product,
	int32_t			index,
	long			bufferSize,
	char*			buffer,
	int32_t			timeout
)
{
	SElExceptionPtr    pexception = NULL;

	memset(buffer, 0, bufferSize);
	
	if (me->_GetItemCodeSpec)
		EXCCHECK( me->_GetItemCodeSpec(me, product, index, bufferSize, buffer, timeout));

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::GetSensor"
static SElExceptionPtr jigGetSensor(
	struct _SJig*	me,
	const char*		sensorId,
	uint32_t*		value
)
{
	SElExceptionPtr    pexception = NULL;

	if (me->_GetSensorSpec)
		EXCCHECK( me->_GetSensorSpec(me, sensorId, value));

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::SetSensor"
static SElExceptionPtr jigSetSensor(
	struct _SJig*	me,
	const char*		sensorId,
	uint32_t		value
)
{
	SElExceptionPtr    pexception = NULL;

	if (me->_SetSensorSpec)
		EXCCHECK( me->_SetSensorSpec(me, sensorId, value));

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::GetAttributeInt32"
static SElExceptionPtr jigGetAttributeInt32(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int32_t*		value
)
{
	SElExceptionPtr	pexception = NULL;
	
	switch (attributeId)
	{
	case JIG_ATTR_JIG_CODE:
		if (value) *value = me->lastJigCode;
		break;
	case JIG_ATTR_IS_JIG_LOADED:
		if (value) *value = me->jigLoaded;
		break;
	default:
		if (me->_GetAttributeInt32Spec)
			EXCCHECK(me->_GetAttributeInt32Spec(me, product, attributeId, value));
	}

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::SetAttributeInt32"
static SElExceptionPtr jigSetAttributeInt32(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int32_t			value
)
{
	SElExceptionPtr	pexception = NULL;
	
	switch (attributeId)
	{
	case JIG_ATTR_JIG_CODE:
		me->lastJigCode = value;
		break;
	case JIG_ATTR_IS_JIG_LOADED:
		EXCTHROW( JIG_ERROR_ATTR_IS_READ_ONLY, "Attribute is read only");
		break;
	default:
		if (me->_SetAttributeInt32Spec)
			EXCCHECK(me->_SetAttributeInt32Spec(me, product, attributeId, value));
	}

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::GetAttributeBoolean"
static SElExceptionPtr jigGetAttributeBoolean(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int8_t*			value
)
{
	SElExceptionPtr	pexception = NULL;
	
	switch (attributeId)
	{
	default:
		if (me->_GetAttributeBooleanSpec)
			EXCCHECK(me->_GetAttributeBooleanSpec(me, product, attributeId, value));
	}

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::SetAttributeBoolean"
static SElExceptionPtr jigSetAttributeBoolean(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int8_t			value
)
{
	SElExceptionPtr	pexception = NULL;
	
	switch (attributeId)
	{
	default:
		if (me->_SetAttributeBooleanSpec)
			EXCCHECK(me->_SetAttributeBooleanSpec(me, product, attributeId, value));
	}

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::jigGetUUID"
static SElExceptionPtr jigGetUUID(
	struct _SJig* me, 
	char* uuid, 
	uint32_t size
)
{
	SElExceptionPtr	pexception = NULL;

	memset(uuid, 0, size);
	
	if (me->_GetUUID)
		EXCCHECK(me->_GetUUID(me, uuid, size));

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::SetEventFactory"
static SElExceptionPtr jigSetEventFactory(
	struct _SJig*		me,
	SElEventFactoryPtr	eventAPI
)
{
	SElExceptionPtr	pexception = NULL;
	
	me->eventFactory = eventAPI;

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"CallbackFncJigCode"
static SElExceptionPtr CallbackFncJigCode(const char* name, uint32_t state, void* pData)
{
	SElExceptionPtr	pexception = NULL;
	SJigPtr			me = (SJigPtr)pData;
	uint32_t		is_jig_inserted = state;
	uint32_t		new_jig_code = 0;
	SDIOSensorsPtr	pSensor = (SDIOSensorsPtr)me->sensorAPI;

	if(pSensor)
	{
		if(is_jig_inserted>0)
		{
			if(pSensor->_IsSimulation==FALSE)
			{
#if 0
				char		root[10];
				time_t		start_clock = clock();

				do 
				{
					Sleep(500);
					EXCCHECK( LetterFromDriveLabel("JIG", root, sizeof(root)));
					if ( ((clock() - start_clock) / (double)CLOCKS_PER_SEC) > (JIG_IDENTIFICATION_TIMEOUT/1000.0) )
					{
						EXCTHROW( -1, "JIG identification timeout");
					}
				} while(root[0] == '\0');

				EXCCHECK( IDFileCheck(root, &new_jig_code));			
#else
				EXCCHECK( pSensor->GetState(pSensor, "JIGCODE", &new_jig_code));
				new_jig_code = 255 - new_jig_code;
#endif
			}
			else
			{
				new_jig_code = state;
			}

			if(!me->jigLoaded)
			{
				EXCCHECK( detectJigAndLoad(me, new_jig_code));
				me->lastJigCode = new_jig_code;
			}
		}
		else
		{
			if(me->jigLoaded)
			{
				EXCCHECK( me->jig_Unload(me));
			}
			me->lastJigCode = 0;
		}
	}

Error:
	EXCRETHROW(pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::jigSetSensorAPI"
static SElExceptionPtr jigSetSensorAPI(
	struct _SJig*	me,
	void*			sensorAPI
)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		state = 0;
	SDIOSensorsPtr	pSensor = (SDIOSensorsPtr)sensorAPI;
	
	me->sensorAPI = sensorAPI;

	if(pSensor)
	{
		/* first ini */
		EXCCHECK( pSensor->GetState(pSensor, "JIGINSERTED", &state));
		EXCCHECK( CallbackFncJigCode("JIGINSERTED", state, (void*)me));

		EXCCHECK( pSensor->RegisterCallback(pSensor, "JIGINSERTED", CallbackFncJigCode, me));
	}

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::jigSetDatabaseAPI"
static SElExceptionPtr jigSetDatabaseAPI(
	struct _SJig*	me,
	void*			databaseAPI
)
{
	SElExceptionPtr	pexception = NULL;
	SDBSPtr			pDbs = (SDBSPtr)databaseAPI;
	
	me->databaseAPI = databaseAPI;

Error:
	EXCRETHROW( pexception);
}

static DWORD WINAPI Thread_Jig(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	SJigPtr			p2jig = (SJigPtr) param;
	
	EXCDISPLAY( p2jig->_JigThread(p2jig));
	
	ExitThread(THREAD_EXIT_CODE);
}

#undef  __FUNC__
#define __FUNC__  "jigThread"
static SElExceptionPtr jigThread(struct _SJig*	me)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		new_jig_code = 0;
	
	while( TRUE)
	{
		if (WaitForSingleObject(me->hEventStop, 100) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		if (me->jigLoaded && me->_JigThreadSpec)
		{
			EXCCHECK( me->_JigThreadSpec(me));
		}
	}

Error:
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "detectJigAndLoad"
static SElExceptionPtr detectJigAndLoad(SJigPtr me, uint8_t jigCode)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			path_string[FILE_NAME_SIZE] = "";
	char*			file_name = NULL,
					*ptext = NULL;
	CAObjHandle		xml_doc = 0;
	CAObjHandle		branch_id = 0;
	CAObjHandle		name_node = 0;
	VBOOL			is_xml_success = 1;
	VARIANT			pfile_name_var = CA_VariantEmpty();
	HRESULT			hr;
	int32_t			branchId = 0;
	SElEventDataPtr	pdata = NULL;
	SDBSPtr			pDbs = (SDBSPtr)me->databaseAPI;
	
	if (jigCode > 0)
	{
		/* TODO: detect correct dll from jig code */
		EXCCHECKCOM(MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &xml_doc));
		EXCCHECKCVI( GetModuleDir (__CVIUserHInst, path_string));
		strcat(path_string, "\\jigConfig.xml");
		EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, path_string));
		EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (xml_doc, NULL, pfile_name_var, &is_xml_success));
	
		sprintf(path_string, "/JigConfig/JigList/jigPair[@jigCode=%d]/@branchId", jigCode);
		EXCCHECKCOM(MSXML_IXMLDOMDocumentselectSingleNode (xml_doc, NULL, path_string, &branch_id));
		if(branch_id)
		{
			EXCCHECKCOM(MSXML_IXMLDOMNodeGettext(branch_id, NULL, &ptext));
			
			branchId = strtol(ptext, NULL, 10);
			if(pDbs && branchId>0)
				EXCCHECK( pDbs->ChangeBranchId(pDbs, branchId, NULL));
			
			EXCCHECKCOM( CA_DiscardObjHandle(branch_id));
			branch_id = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		sprintf(path_string, "/JigConfig/JigList/jigPair[@jigCode=%d]/@fileName", jigCode);
		EXCCHECKCOM(MSXML_IXMLDOMDocumentselectSingleNode (xml_doc, NULL, path_string, &name_node));
		if (name_node == 0)
		{
			/* return error "Unsupported jig" */
			char serror[128];

			sprintf(serror, "JIG not supported: %d", jigCode);
			EXCTHROW( JIG_ERROR_JIG_NOT_SUPPORTED, serror);
		}
		EXCCHECKCOM(MSXML_IXMLDOMNodeGettext(name_node, NULL, &file_name));
		EXCCHECK( me->jig_Load(me, file_name));
	}
	
Error:
	eleventdata_delete(&pdata);
	CA_FreeMemory(file_name);
	CA_VariantClear(&pfile_name_var);
	if(name_node) CA_DiscardObjHandle(name_node);
	if(xml_doc) CA_DiscardObjHandle(xml_doc);
	EXCRETHROW( pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::LetterFromDriveLabel"
static SElExceptionPtr LetterFromDriveLabel(const char *Label, char drive_letter[], int32_t driveSize)
{
	SElExceptionPtr	pexception = NULL;
	DWORD	drive, ConDrive = 0;
	char 	rootname[16] = "";
	char 	volume_name[256] = "";

	drive_letter[0] = '\0';
	ConDrive = GetLogicalDrives();
	
	for( drive = 0; drive < (sizeof(drive)*8); drive++ )
	{
		if ((ConDrive & (1 << drive)) != 0 )
		{
			sprintf(rootname, "%c:\\",'A'+ drive);
			GetVolumeInformation(  rootname,
								  (LPTSTR)volume_name,
								  128,
								  0,
								  0,
								  0,
								  0,
								  0);

			if( strcmp(volume_name,Label)==0)
			{
				strncpy(drive_letter, rootname, driveSize);
				break;
			}
		}
	}
Error:
	EXCRETHROW(pexception);
}

#undef  __FUNC__
#define __FUNC__  "<JigClass>::IDFileCheck"
static SElExceptionPtr IDFileCheck(
	const char *root,
	uint32_t	*pID
)
{
	SElExceptionPtr	pexception = NULL;
	
	char			file_path[360] = "";
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	VBOOL         	is_xml_success = 1;
	char			*ptext= NULL;
	uint32_t		id = 0;
	HRESULT			hr;
	
	if(pID) *pID = 0;
	
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	sprintf(file_path,"%s%s", root, JIG_IDENTIFICATION_FILE);
	EXCCHECKCOM( CA_VariantSetCString(  &pfile_name_var, file_path));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//jig/id", &pnode));
	if(pnode)
	{
		char*	pend = NULL;
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode, NULL, &ptext));
		if(pID) *pID = strtoul(ptext, &pend, 10);
	}

Error:
	if ( ptext ) CA_FreeMemory( ptext );
	if ( pnode ) CA_DiscardObjHandle( pnode);
	if ( pxml ) CA_DiscardObjHandle( pxml);
	EXCRETHROW(pexception);		
}
