        ??  ??                  ?      ?? ??     0 	        ?4   V S _ V E R S I O N _ I N F O     ???                 ?                         ?   S t r i n g F i l e I n f o   ?   0 4 0 9 0 4 b 0        C o m p a n y N a m e     V   F i l e D e s c r i p t i o n     S m a c A x i s   ( R e l e a s e   x 8 6 )     (   F i l e V e r s i o n     1 . 0   2 	  I n t e r n a l N a m e   S m a c A x i s     H   L e g a l C o p y r i g h t   C o p y r i g h t   ?     2 0 2 0   B   O r i g i n a l F i l e n a m e   S m a c A x i s . d l l     4 
  P r o d u c t N a m e       S m a c A x i s   ,   P r o d u c t V e r s i o n   1 . 0   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	??  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        struct _SElException *AlarmReset(struct _axis *me, int ID_NODE);
struct _SElException *CurrentAnalog(struct _axis *me, int ID_NODE, double *analog, double Coefficient, double Offsets);
struct _SElException *CurrentPosition(struct _axis *me, int ID_NODE, double *position);
struct _SElException *EnableAxis(struct _axis *me, int ID_NODE);
struct _SElException *HomeReturn(struct _axis *me, int ID_NODE);
struct _SElException *Initialize(struct _axis *me, int ID_NODE);
struct _SElException *PresentAlarm(struct _axis *me, int ID_NODE, int *status);
struct _SElException *SetPosition(struct _axis *me, int ID_NODE, double position, double speed);
struct _SElException *StopAxis(struct _axis *me, int ID_NODE);
    