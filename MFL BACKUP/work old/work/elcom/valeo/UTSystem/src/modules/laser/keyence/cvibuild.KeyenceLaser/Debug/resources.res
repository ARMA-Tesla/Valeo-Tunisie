        ??  ??                  ?      ?? ??     0 	        ?4   V S _ V E R S I O N _ I N F O     ???                 ?                           S t r i n g F i l e I n f o   ?   0 4 0 9 0 4 b 0        C o m p a n y N a m e     Z   F i l e D e s c r i p t i o n     K e y e n c e L a s e r   ( D e b u g   x 8 6 )     (   F i l e V e r s i o n     1 . 0   :   I n t e r n a l N a m e   K e y e n c e L a s e r     H   L e g a l C o p y r i g h t   C o p y r i g h t   ?     2 0 2 1   J   O r i g i n a l F i l e n a m e   K e y e n c e L a s e r . d l l     <   P r o d u c t N a m e       K e y e n c e L a s e r   ,   P r o d u c t V e r s i o n   1 . 0   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	??  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        struct _SElException *Connect(struct _Laser *me);
struct _SElException *Disconnect(struct _Laser *me);
struct _SElException *EnableMarking(struct _Laser *me);
struct _SElException *ErrorClear(struct _Laser *me);
struct _SElException *ErrorStatus(struct _Laser *me, int *ErrorStatus, char *ErrorCode);
struct _SElException *MarkerReady(struct _Laser *me, int *Ready);
struct _SElException *ReadString(struct _Laser *me, int program, int block, const char *File_Line_String, int *string_OK, char *laser_string);
struct _SElException *RunProgram(struct _Laser *me, int program, int block);
struct _SElException *StartLasering(struct _Laser *me);
struct _SElException *WriteString(struct _Laser *me, int program, int block, const char *string);
   