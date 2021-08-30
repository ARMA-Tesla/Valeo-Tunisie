#include "nican.h" 


#define MAX_SMAC 6

#define TYPE_ASCII	  1
#define TYPE_CHAR_HEX 0

 
NCTYPE_OBJH TXRXHandle[MAX_SMAC];
NCTYPE_CAN_FRAME Transmit[MAX_SMAC];




void PrintStat(NCTYPE_STATUS Status, char *source);

int CAN_Write_Frame(int IndiceSMAC, int id,char *data, char *nom);
int CAN_Write_FrameV2(int IndiceSMAC, int NODE, int ID, char *data, int data_length);
int CAN_SendRecive(int IndiceSMAC, int NODE, int ID_ToSend, int ID_ToFind, char *FrameToSend, int data_length, int TimeOutMs, int Type,  char *FrameToFind, unsigned char *Buffer);
int CAN_Read_TIMEOUT(int IndiceSMAC, int Id_To_FIND, unsigned char *Buffer, int TimeOutMs, int Type);
void GetValFromDataHex(unsigned char *Data, double *Val, double Coefficient, double Offsets);




int SMAC_GetAllPosition(double *TabForce,int ValRead, int *Position1, int *Position2, int *Position3, int *Position4);

int SMAC_CAN_Open(int IndiceSMAC, char *Interface);
int SMAC_CAN_Close(int IndiceSMAC);


#define CMD_SPECIF_StartRemoteNode			1		
#define CMD_SPECIF_StopRemoteNode			2		
#define CMD_SPECIF_EnterPre_Operational		3
#define CMD_SPECIF_ResetNode				4			
#define CMD_SPECIF_ResetCommunication		5	

int SMAC_CAN_NMT_Services_Commands(int IndiceSMAC, int NODE, int CommandSpecifier);


int SMAC_CAN_Set_Max_Profile_Velocity(int IndiceSMAC, int NODE, int Profile_Velocity);  
int SMAC_CAN_Start_Macro(int IndiceSMAC, int NODE, int macro);
int SMAC_CAN_Set_Target_Position(int IndiceSMAC, int NODE, int Position);
int SMAC_CAN_Set_Max_Motor_Speed(int IndiceSMAC, int NODE, int Speed);
int SMAC_CAN_Set_Min_Position_Limit(int IndiceSMAC, int NODE, int limit);
int SMAC_CAN_Set_Max_Position_Limit(int IndiceSMAC, int NODE, int limit);
int SMAC_CAN_Set_Max_Torque(int IndiceSMAC, int NODE, int Torque);

int SMAC_CAN_Set_Target_Velocity(int IndiceSMAC, int NODE, int Speed);
int SMAC_CAN_Set_Profile_Velocity(int IndiceSMAC, int NODE, int Speed);

int SMAC_CAN_READ_ANALOG1(int IndiceSMAC, int NODE, double *Val,double Coefficient, double Offsets);
int SMAC_CAN_READ_ECHANTILLON_ANALOG1(int IndiceSMAC, int NODE, int NumbEchantillon ,double *Val,double Coefficient, double Offsets);

int SMAC_CAN_READ_ANALOG2(int IndiceSMAC, int NODE, double *Val,double Coefficient, double Offsets);

int SMAC_CAN_READ_ACTUAL_POSITION(int IndiceSMAC, int NODE, double *Val,double Coefficient, double Offsets);
int SMAC_CAN_READ_Min_Position_Limit(int IndiceSMAC, int NODE, double *Min_Position);

int ValReadSmac[MAX_SMAC];
double SMAC_CAN_TabValPosition[MAX_SMAC][99999];
double SMAC_CAN_TabValForce[MAX_SMAC][99999];
int SMAC_CAN_TabValTemps[MAX_SMAC][99999];

double SMAC_CAN_TabValForceRII[MAX_SMAC][99999];

int SMAC_CAN_READ_Force_Position(int IndiceSMAC, int NODE, double ForceCoefficient, double ForceOffsets, double PositionCoefficient, double PositionOffsets, int TimeOut_ms);


int SMAC_CAN_WAIT_POSITION(int IndiceSMAC, int NODE, double Position, int Tolerance, double Coefficient, double Offsets, int TimeOut_ms);

int SMAC_GetAllPositionContact(int *TabEtat,int ValRead, int *Position_On, int *Position_Off);
