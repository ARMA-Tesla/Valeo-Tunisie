
#if !defined(__EL_JIG_SPECIFIC_H__)
#define __EL_JIG_SPECIFIC_H__

#include <exception/Exception.h>
#include <jig/class/jig_class.h>

#if defined(WINDOWS)
	#define JIG_API		__declspec( dllexport )
#elif defined(UNIX)
	#define JIG_API		extern "C"
#else
	#define JIG_API
#endif

/* Initialize specific jig */
JIG_API SElExceptionPtr _FUNCC Initialize
(
	struct _SJig*	me,
	const char*		fileName
);
/* Destroy specific jig */
JIG_API SElExceptionPtr _FUNCC Destroy(struct _SJig* me);

/* give uuid jig */
JIG_API SElExceptionPtr _FUNCC GetUUID(struct _SJig* me, char* uuid, uint32_t size);

/* Lock product */
JIG_API SElExceptionPtr _FUNCC LockItem
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
/* Unlock product */
JIG_API SElExceptionPtr _FUNCC UnlockItem
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
/* Connectors connect to product */
JIG_API SElExceptionPtr _FUNCC ConnectToItem
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
/* Connectors disconnect from product */
JIG_API SElExceptionPtr _FUNCC DisconnectFromItem
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
/* Cold marking product */
JIG_API SElExceptionPtr _FUNCC ColdMarking
(
	struct _SJig*	me,
	const char*		product,
	int32_t			timeout
);
/* Move JIG to specified position */
JIG_API SElExceptionPtr _FUNCC SetItemPosition
(
	struct _SJig*	me,
	const char*		product,
	int32_t			position,
	int32_t			timeout
);
/* Get JIG position */
JIG_API SElExceptionPtr _FUNCC GetItemPosition
(
	struct _SJig*	me,
	const char*		product,
	int32_t*		actualPosition
);
/* Get bar code indentification of panel */
JIG_API SElExceptionPtr _FUNCC GetItemCode
(
	struct _SJig*	me,
	const char*		product,
	int32_t			index,
	long			bufferSize,
	char*			buffer,
	int32_t			timeout
);
/* Low-level function get sensor state directly */
JIG_API SElExceptionPtr _FUNCC GetSensor
(
	struct _SJig*	me,
	const char*		sensorId,
	uint32_t*		value
);
/* Low-level function set sensor state directly */
JIG_API SElExceptionPtr _FUNCC SetSensor
(
	struct _SJig*	me,
	const char*		sensorId,
	uint32_t		value
);
/* Get Attribute Int32 */
JIG_API SElExceptionPtr _FUNCC GetAttributeInt32
(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int32_t*		value
);
/* Set Attribute Int32 */
JIG_API SElExceptionPtr _FUNCC SetAttributeInt32
(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int32_t			value
);
/* Get Attribute Boolean */
JIG_API SElExceptionPtr _FUNCC GetAttributeBoolean
(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int8_t*			value
);
/* Set Attribute Boolean */
JIG_API SElExceptionPtr _FUNCC SetAttributeBoolean
(
	struct _SJig*	me,
	const char*		product,
	int32_t			attributeId,
	int8_t			value
);
/* Specific sensor test */
JIG_API SElExceptionPtr _FUNCC JigThread
(
	struct _SJig*	me
);

#endif /* __EL_JIG_SPECIFIC_H__ */
