        ??  ??                  ?      ?? ??     0 	        ?4   V S _ V E R S I O N _ I N F O     ???                 ?                        8   S t r i n g F i l e I n f o      0 4 0 9 0 4 b 0        C o m p a n y N a m e     b   F i l e D e s c r i p t i o n     s m a r t _ a u t o m a t i o n   ( D e b u g   x 8 6 )     (   F i l e V e r s i o n     1 . 0   B   I n t e r n a l N a m e   s m a r t _ a u t o m a t i o n     H   L e g a l C o p y r i g h t   C o p y r i g h t   ?     2 0 1 8   R   O r i g i n a l F i l e n a m e   s m a r t _ a u t o m a t i o n . d l l     D   P r o d u c t N a m e       s m a r t _ a u t o m a t i o n   ,   P r o d u c t V e r s i o n   1 . 0   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	??  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        struct _SElException *AlarmReset(struct _motor *me, int motor);
struct _SElException *ControllerStatus(struct _motor *me, int motor, int *status);
struct _SElException *CurrentPosition(struct _motor *me, int motor, double *angle);
struct _SElException *EnableMotor(struct _motor *me, int motor);
struct _SElException *HomeReturn(struct _motor *me, int motor);
struct _SElException *Initialize(struct _motor *me);
struct _SElException *MoveMotorPosition(struct _motor *me, int reset, double angle, double speed);
struct _SElException *PresentAlarm(struct _motor *me, int motor, int *status);
struct _SElException *ReadAngle(struct _motor *me, int motor, double *angle);
struct _SElException *RequestAngle(struct _motor *me, int motor);
 