        ��  ��                  �      �� ��     0 	        �4   V S _ V E R S I O N _ I N F O     ���                 ?                           S t r i n g F i l e I n f o   �   0 4 0 9 0 4 b 0        C o m p a n y N a m e     V   F i l e D e s c r i p t i o n     C a b P r i n t e r   ( D e b u g   x 8 6 )     (   F i l e V e r s i o n     1 . 0   6   I n t e r n a l N a m e   C a b P r i n t e r     H   L e g a l C o p y r i g h t   C o p y r i g h t   �     2 0 2 0   F   O r i g i n a l F i l e n a m e   C a b P r i n t e r . d l l     8   P r o d u c t N a m e       C a b P r i n t e r   ,   P r o d u c t V e r s i o n   1 . 0   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	�h  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        struct _SElException *Connect(struct _Printer *me);
struct _SElException *Disconnect(struct _Printer *me);
struct _SElException *Reset(struct _Printer *me);
struct _SElException *Status(struct _Printer *me, int *StatusOnline, char *Error, int *iAmmountLabels, int *StatusInterpreter);
struct _SElException *Write(struct _Printer *me, const char *command);
    