        ??  ??                  ?      ?? ??     0 	        ?4   V S _ V E R S I O N _ I N F O     ???                 ?                        8   S t r i n g F i l e I n f o      0 4 0 9 0 4 b 0        C o m p a n y N a m e     j !  F i l e D e s c r i p t i o n     j i g _ P Q 2 6 _ c l i m a t r o n i c   ( D e b u g   x 8 6 )     (   F i l e V e r s i o n     1 . 0   :   I n t e r n a l N a m e   j i g _ b 2 y y _ m c p     H   L e g a l C o p y r i g h t   C o p y r i g h t   ?     2 0 1 7   J   O r i g i n a l F i l e n a m e   j i g _ b 2 y y _ m c p . d l l     L   P r o d u c t N a m e       j i g _ P Q 2 6 _ c l i m a t r o n i c   ,   P r o d u c t V e r s i o n   1 . 0   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	??  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        struct _SElException *ColdMarking(struct _SJig *me, const char *product, int timeout);
struct _SElException *ConnectToItem(struct _SJig *me, const char *product, int timeout);
struct _SElException *Destroy(struct _SJig *me);
struct _SElException *DisconnectFromItem(struct _SJig *me, const char *product, int timeout);
struct _SElException *GetAttributeBoolean(struct _SJig *me, const char *product, int attributeId, signed char *value);
struct _SElException *GetAttributeInt32(struct _SJig *me, const char *product, int attributeId, int *value);
struct _SElException *GetItemCode(struct _SJig *me, const char *product, int index, long bufferSize, char *buffer, int timeout);
struct _SElException *GetItemPosition(struct _SJig *me, const char *product, int *actualPosition);
struct _SElException *GetSensor(struct _SJig *me, const char *sensorId, unsigned int *value);
struct _SElException *GetUUID(struct _SJig *me, char *uuid, unsigned int size);
struct _SElException *Initialize(struct _SJig *me, const char *fileName);
struct _SElException *JigThread(struct _SJig *me);
struct _SElException *LockItem(struct _SJig *me, const char *product, int timeout);
struct _SElException *SetAttributeBoolean(struct _SJig *me, const char *product, int attributeId, signed char value);
struct _SElException *SetAttributeInt32(struct _SJig *me, const char *product, int attributeId, int value);
struct _SElException *SetItemPosition(struct _SJig *me, const char *product, int position, int timeout);
struct _SElException *SetSensor(struct _SJig *me, const char *sensorId, unsigned int value);
struct _SElException *UnlockItem(struct _SJig *me, const char *product, int timeout);
    