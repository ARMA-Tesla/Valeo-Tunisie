        ??  ??                  ?      ?? ??     0 	        ?4   V S _ V E R S I O N _ I N F O     ???                 ?                         ?   S t r i n g F i l e I n f o   ?   0 4 0 9 0 4 b 0        C o m p a n y N a m e     P   F i l e D e s c r i p t i o n     m o t o r   ( R e l e a s e   x 8 6 )   (   F i l e V e r s i o n     1 . 0   ,   I n t e r n a l N a m e   m o t o r   H   L e g a l C o p y r i g h t   C o p y r i g h t   ?     2 0 1 8   < 
  O r i g i n a l F i l e n a m e   m o t o r . d l l   .   P r o d u c t N a m e       m o t o r     ,   P r o d u c t V e r s i o n   1 . 0   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	??  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        struct _SElException *__cdecl GetModuleAttribute(unsigned int attrID, unsigned int size, void *value);
struct _SElException *__cdecl InitializeElcomModule(void *pInstance, const char *configuration, struct _SPluginArgs *pParam);
struct _SElException *__cdecl LoadElcomModule(const char *configuration, void **pInstance, struct _SPluginArgs *pParam);
struct _SElException *__cdecl UnloadElcomModule(void *pInstance);
struct _SElException *motor_delete(struct _motor **motor);
struct _SElException *motor_new(struct _motor **motor, const char *name, const char *optionString);
struct _SElException *motors_delete(struct _motors **motors);
struct _SElException *motors_new(struct _motors **motors);
    