#if !defined(__EL_JIG_CLASS_H__)
#define __EL_JIG_CLASS_H__

#include <exception/Exception.h>
#include <objlist/objlist.h>
#include <windows.h>
#include <tester/barcodereader/BarCodeReader.h>
#include <eventfactory/EventFactory.h>
#include <multithread/multithread.h>


#define EVNT_JIG_RESET_ENABLE		"EVNT_JIG_RESET_ENABLE"
#define EVNT_JIG_RESET_DISABLE		"EVNT_JIG_RESET_DISABLE"
#define EVNT_JIG_LOADED				"EVNT_JIG_LOADED"
#define EVNT_JIG_UNLOADED			"EVNT_JIG_UNLOADED"
#define EVNT_JIG_ITEM_INSERTED		"EVNT_JIG_ITEM_INSERTED"
#define EVNT_JIG_ITEM_RELEASED		"EVNT_JIG_ITEM_RELEASED"
#define EVNT_JIG_ITEM_LOCK			"EVNT_JIG_ITEM_LOCKED"
#define EVNT_JIG_ITEM_UNLOCK		"EVNT_JIG_ITEM_UNLOCKED"
#define EVNT_JIG_ITEM_CONNECTED		"EVNT_JIG_ITEM_CONNECTED"
#define EVNT_JIG_ITEM_DISCONNECTED	"EVNT_JIG_ITEM_DISCONNECTED"

#define JIG_ATTR_IS_PANEL_LOCKED			0	/* boolean */
#define JIG_ATTR_IS_PANEL_UNLOCKED			1	/* boolean */
#define JIG_ATTR_IS_PANEL_INSERTED			2	/* boolean */
#define JIG_ATTR_IS_PANEL_RELEASED			3	/* boolean */
#define JIG_ATTR_IS_CONNECTORS_UP			4	/* boolean */
#define JIG_ATTR_IS_CONNECTORS_DOWN			5	/* boolean */
#define JIG_ATTR_IS_JIG_LOADED				6	/* boolean */
#define JIG_ATTR_JIG_CODE					7	/* byte */
#define JIG_ATTR_IS_JIG_SETTINGS_OK			8	/* boolean */
#define JIG_ATTR_HWND						9	/* int */

#define JIG_POSITION_HORIZONTAL			0
#define JIG_POSITION_INSERT				-1


#define JIG_ERROR_OFFSET						-1000L
#define JIG_WARN_OFFSET							1000L

#define JIG_WARN_JIG_NOT_LOADED					(JIG_WARN_OFFSET + 0L)

#define JIG_ERROR_ATTR_IS_READ_ONLY				(JIG_ERROR_OFFSET - 0L)
#define JIG_ERROR_THREAD_START_FAILED			(JIG_ERROR_OFFSET - 1L)
#define JIG_ERROR_JIG_NOT_SUPPORTED				(JIG_ERROR_OFFSET - 2L)
#define JIG_ERROR_UNKNOWN_PRODUCT_ID			(JIG_ERROR_OFFSET - 3L)
#define JIG_ERROR_LOAD_LIBRARY					(JIG_ERROR_OFFSET - 4L)
#define JIG_ERROR_ATTR_UNKNOWN					(JIG_ERROR_OFFSET - 5L)
#define JIG_ERROR_ATTR_INVALID_TYPE				(JIG_ERROR_OFFSET - 6L)
#define JIG_ERROR_INVALID_PARAMETER				(JIG_ERROR_OFFSET - 7L)
#define JIG_ERROR_TIMEOUT						(JIG_ERROR_OFFSET - 8L)



typedef struct _SJig
{
	/***************************************************************************
     * Public Members
     **************************************************************************/      

	SElExceptionPtr (*jig_Load)(
		struct _SJig*	me,
		const char*		file
	);
	SElExceptionPtr (*jig_Unload)(struct _SJig* me);
	
	SElExceptionPtr (*jig_LockItem)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	SElExceptionPtr (*jig_UnlockItem)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	SElExceptionPtr (*jig_ConnectToItem)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	SElExceptionPtr (*jig_DisconnectFromItem)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	SElExceptionPtr (*jig_ColdMarking)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	SElExceptionPtr (*jig_SetItemPosition)(
		struct _SJig*	me,
		const char*		product,
		int32_t			position,
		int32_t			timeout
	);
	SElExceptionPtr (*jig_GetItemPosition)(
		struct _SJig*	me,
		const char*		product,
		int32_t*		actualPosition
	);
	SElExceptionPtr (*jig_GetItemCode)(
		struct _SJig*	me,
		const char*		product,
		int32_t			index,
		long			bufferSize,
		char*			buffer,
		int32_t			timeout
	);
	SElExceptionPtr (*jig_GetSensor)(
		struct _SJig*	me,
		const char*		sensorId,
		uint32_t*		value
	);
	SElExceptionPtr (*jig_SetSensor)(
		struct _SJig*	me,
		const char*		sensorId,
		uint32_t		value
	);
	SElExceptionPtr (*jig_GetAttributeInt32)(
		struct _SJig*	me,
		const char*		product,
		int32_t			attributeId,
		int32_t*		value
	);
	SElExceptionPtr (*jig_SetAttributeInt32)(
		struct _SJig*	me,
		const char*		product,
		int32_t			attributeId,
		int32_t			value
	);
	SElExceptionPtr (*jig_GetAttributeBoolean)(
		struct _SJig*	me,
		const char*		product,
		int32_t			attributeId,
		int8_t*			value
	);
	SElExceptionPtr (*jig_SetAttributeBoolean)(
		struct _SJig*	me,
		const char*		product,
		int32_t			attributeId,
		int8_t			value
	);
	SElExceptionPtr (*jig_GetUUID)(
		struct _SJig* me, 
		char* uuid, 
		uint32_t size
	);
	SElExceptionPtr (*jig_SetEventFactory)(
		struct _SJig*		me,
		SElEventFactoryPtr	event
	);
	SElExceptionPtr (*jig_SetSensorAPI)(
		struct _SJig*		me,
		void*				sensorAPI
	);
	SElExceptionPtr (*jig_SetDatabaseAPI)(
		struct _SJig*		me,
		void*				databaseAPI
	);
	
	
	/**************************************************************************
     * Private Members
     **************************************************************************/
	uint8_t			jigLoaded;
	uint8_t			lastJigCode;
	HANDLE			hThread;
	HANDLE			hEventStop;
	void*			sensorAPI;
	void*			databaseAPI;
	HMODULE			moduleId;
	void*			pLogicalNames;
	int32_t			LogicalCounts;
	SBCRPtr			pBarCodeReader;
	int32_t			BCRCounts;
	SElEventFactoryPtr	eventFactory;
	char			path[512];
	
	
	/* Initialize specific JIG */
	SElExceptionPtr (*_InitializeSpec)(
		struct _SJig*	me,
		const char*		fileName
	);
	/* Destroy specific JIG */
	SElExceptionPtr (*_DestroySpec)(
		struct _SJig*	me
	);
	SElExceptionPtr (*_GetUUID)(
		struct _SJig* me, 
		char* uuid, 
		uint32_t size
	);
	/* Lock product */
	SElExceptionPtr (*_LockItemSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	/* Unlock product */
	SElExceptionPtr (*_UnlockItemSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	/* Connectors connect to product */
	SElExceptionPtr (*_ConnectToItemSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	/* Connectors disconnect from product */
	SElExceptionPtr (*_DisconnectFromItemSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	/* Cold marking product */
	SElExceptionPtr (*_ColdMarkingSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			timeout
	);
	/* Move JIG to specified position */
	SElExceptionPtr (*_SetItemPositionSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			position,
		int32_t			timeout
	);
	/* Get JIG position */
	SElExceptionPtr (*_GetItemPositionSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t*		actualPosition
	);
	/* Get bar code indentification of panel */
	SElExceptionPtr (*_GetItemCodeSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			index,
		long			bufferSize,
		char*			buffer,
		int32_t			timeout
	);
	/* Low-level function get sensor state directly */
	SElExceptionPtr (*_GetSensorSpec)(
		struct _SJig*	me,
		const char*		sensorId,
		uint32_t*		value
	);
	/* Low-level function set sensor state directly */
	SElExceptionPtr (*_SetSensorSpec)(
		struct _SJig*	me,
		const char*		sensorId,
		uint32_t		value
	);
	/* Get Attribute Int32 */
	SElExceptionPtr (*_GetAttributeInt32Spec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			attributeId,
		int32_t*		value
	);
	/* Set Attribute Int32 */
	SElExceptionPtr (*_SetAttributeInt32Spec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			attributeId,
		int32_t			value
	);
	/* Get Attribute Boolean */
	SElExceptionPtr (*_GetAttributeBooleanSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			attributeId,
		int8_t*			value
	);
	/* Set Attribute Boolean */
	SElExceptionPtr (*_SetAttributeBooleanSpec)(
		struct _SJig*	me,
		const char*		product,
		int32_t			attributeId,
		int8_t			value
	);
	SElExceptionPtr (*_JigThreadSpec)(struct _SJig* me);
	SElExceptionPtr (*_JigThread)(struct _SJig*	me);

} SJig, *SJigPtr;


SElExceptionPtr	jig_new( SJigPtr* pJig);	
SElExceptionPtr	jig_delete( SJigPtr* pJig);


#endif /* __EL_JIG_CLASS_H__ */

