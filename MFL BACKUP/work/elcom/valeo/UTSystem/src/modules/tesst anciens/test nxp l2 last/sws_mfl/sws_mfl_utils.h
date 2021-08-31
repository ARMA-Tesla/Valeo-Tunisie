#if !defined(__CMFB_PIANO_UTILS_H__)
#define __CMFB_PIANO_UTILS_H__

#include <test/eltest.h>
#include <eltypes.h>
#include <elerror.h>
#include <utility.h>

#ifdef __cplusplus
    extern "C" {
#endif 
 
#define WRITE_FILE	0

		
		
//#define DELTA_WEG		0.007
//#define DELTA_KRAFT		0.9
#define DELTA_KRAFT2	0.5
#define DELTA_KRAFT3	2.7
//#define BET_MAX_KRAFT	10
#define DELTA_WEG		0.03
#define DELTA_KRAFT		0.3
#define BET_MAX_KRAFT	10

#define DELTA_ANGLE		1.5 //OLD 1.5
#define DELTA_TORQUE	0.1
#define DELTA_TORQUE2	0.4 //OLD 0.5
#define BET_MAX_TORQUE	10

#define BUFFERLEN 		12000 //OLD  12000
#define BUFFERLEN2 		12000 //OLD  12000
		
#define COLOR_FORCE	VAL_BLUE
#define COLOR_CONTACT VAL_BLACK
#define COLOR_CONTACT2 VAL_YELLOW
#define COLOR_MESSW VAL_DK_BLUE
#define COLOR_LEVEL	VAL_MAGENTA		

#define COLOR_TORQUE	VAL_BLUE
#define COLOR_CONTACT3 	VAL_RED
		
#define MAX_STATIONS				8
#define SIDE_RIGHT					0
#define SIDE_LEFT					1
		
#define TIMEOUT_DEFAULT			15000	 //10000
#define TIMEOUT_TABLE			INFINITE
#define TIMEOUT_AXIS			10000

#define LIN_CHANNEL_STATION_30				"LIN0"
#define LIN_CHANNEL_STATION_40				"LIN1"
#define LIN_CHANNEL_STATION_60				"LIN3"
#define LIN_CHANNEL_STATION_70				"LIN2"


		
/* BFFA2008  No explicit path exists between the two channels.
 * BFFA200C  The channels are explicitly connected.
 */

#define TIMEOUT_INIT() 	clock_t	_start_clock = clock()

#define TIMEOUT_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > ((double)timeout/1000.0) ) { EXCTHROW( TEST_ERR_TIMEOUT, "Button Timeout"); } else


enum
{
	MESS_KRAFT = 0,
	MESS_WEG,
	MESS_CONTACT,
	MESS_BUF
};

enum
{
	MESS_TORQUE = 0,
	MESS_ANGLE,
	MESS_LIN,
	MESS_BUF2
};

enum
{
	PHONE = 0,
	LIST,
	SPEECH,
	MENU_RIGHT,
	SCROLL_WHEEL,
	MENU_LEFT,
	VOLUME_PLUS,
	VOLUME_MINUS,
	MODE,
	ASS,
	SET,
	MINUS,
	PLUS,
	CANCEL,
	RESUME,
	LIM,
	RES_CANC,
	ACC,
	DCC,
	TIPP_WIPE,
	MAX_FUNCTIONS
};

enum
{
	R_BUTTOM_LEFT = 0,
	R_BUTTOM_MID,
	R_BUTTOM_RIHGT,
	R_MID_LEFT,
	R_MID_MID,
	R_MID_RIGHT,
	R_TOP_LEFT,
	R_TOP_RIGHT,
	L_BOTTOM_LEFT,
	L_BOTTOM_MID,
	L_BOTTOM_RIGHT,
	L_MID_LEFT,
	L_MID_RIGHT,
	L_TOP_LEFT,
	L_TOP_RIGHT,
	TIP_WHEEL,
	MAX_POSITIONS
};

int iFlag_Thread_Lin_Read[MAX_STATIONS], 
	iFlag_Thread_Lin_Write[MAX_STATIONS], 
	iFlag_Run_Normal_Mode[MAX_STATIONS],
	ADC_Received[MAX_STATIONS], 
	iFlag_Write_Wake_Up[MAX_STATIONS],
	iFlag_Write_Valeo_production_Key[MAX_STATIONS],
	iFlag_Read_Switch_Raw_Values[MAX_STATIONS],
	iFlag_Write_Variant_code[MAX_STATIONS],
	iFlag_Read_Variant_code[MAX_STATIONS],
	iFlag_Read_Valeo_Sw_Revision[MAX_STATIONS],
	iFlag_Write_BMW_Part_Number[MAX_STATIONS],
	iFlag_Write_BMW_SW_Revision[MAX_STATIONS],
	iFlag_Write_BMW_HW_Revision[MAX_STATIONS],
	iFlag_Read_BMW_Part_Number[MAX_STATIONS],
	iFlag_Read_BMW_SW_Revision[MAX_STATIONS],
	iFlag_Read_BMW_HW_Revision[MAX_STATIONS],
	iFlag_Read_Sw_Build_Date[MAX_STATIONS],
	iFlag_Read_MAN_DATA_1[MAX_STATIONS],
	iFlag_Read_MAN_DATA_1_READ_OK[MAX_STATIONS],
	iFlag_Write_MAN_DATA_2[MAX_STATIONS],
	iFlag_Read_MAN_DATA_2[MAX_STATIONS],
	iFlag_Read_MAN_DATA_2_READ_OK[MAX_STATIONS],
	iFlag_Erasing_DTC_Memory_1[MAX_STATIONS],
	iFlag_Erasing_DTC_Memory_2[MAX_STATIONS],
	iFlag_Read_Number_DTC_Memory[MAX_STATIONS],
	iFlag_LED_BG[MAX_STATIONS],
	iFlag_LED_STA_RED[MAX_STATIONS],
	iFlag_LED_STA_GREEN[MAX_STATIONS],
	iFlag_LED_STA_YELLOW[MAX_STATIONS],
	Error_Alive_Byte[MAX_STATIONS],
	iFlag_Delete_Cache[MAX_STATIONS],
	Number_DTC[MAX_STATIONS];
   
int Status_Push[MAX_STATIONS][MAX_FUNCTIONS];
double Value_ADC_Push[MAX_STATIONS][MAX_FUNCTIONS];

double Value_ADC_TIP_WHEEL_DOWN_P1[MAX_STATIONS];
double Value_ADC_TIP_WHEEL_DOWN_P10[MAX_STATIONS];
double Value_ADC_TIP_WHEEL_UP_P1[MAX_STATIONS];
double Value_ADC_TIP_WHEEL_UP_P10[MAX_STATIONS];


unsigned char Log_Lin[MAX_STATIONS][1024*1000];
uint8_t Last_Alive_Byte[MAX_STATIONS];
uint8_t first_LIN_Frame[MAX_STATIONS];

unsigned char Variant_Code_Read[MAX_STATIONS][2+1];
unsigned char Variant_Code_Write[MAX_STATIONS][2+1];
unsigned char Valeo_Sw_Revision[MAX_STATIONS][6+1];
unsigned char Sw_Build_Date[MAX_STATIONS][12+1];

unsigned char BMW_Part_Number_Write[MAX_STATIONS][7+1];
unsigned char BMW_SW_Revision_Write[MAX_STATIONS][4+1];
unsigned char BMW_HW_Revision_Write[MAX_STATIONS][4+1];
unsigned char BMW_Part_Number_Read[MAX_STATIONS][7+1];
unsigned char BMW_SW_Revision_Read[MAX_STATIONS][4+1];
unsigned char BMW_HW_Revision_Read[MAX_STATIONS][4+1];

unsigned char MAN_DATA_1_Tracability_EMS_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_1_Serial_Read[MAX_STATIONS][4+1];
unsigned char MAN_DATA_1_VALEO_Part_Number_Read[MAX_STATIONS][9+1];
unsigned char MAN_DATA_1_Revision_Index_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_1_LineID_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_1_Production_Year_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_1_Production_Month_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_1_Production_Day_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_1_Production_Batch_Number[MAX_STATIONS][2+1]; 


unsigned char MAN_DATA_2_VALEO_Part_Number_Write[MAX_STATIONS][9+1];
unsigned char MAN_DATA_2_Revision_Index_Write[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_LineID_Write[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_Production_Year_Write[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_Production_Month_Write[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_Production_Day_Write[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_Serial_Write[MAX_STATIONS][5+1];
unsigned char MAN_DATA_2_Tracability_MMS_Write[MAX_STATIONS][2+1];

unsigned char MAN_DATA_2_VALEO_Part_Number_Read[MAX_STATIONS][9+1];
unsigned char MAN_DATA_2_Revision_Index_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_LineID_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_Production_Year_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_Production_Month_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_Production_Day_Read[MAX_STATIONS][2+1];
unsigned char MAN_DATA_2_Serial_Read[MAX_STATIONS][5+1];
unsigned char MAN_DATA_2_Tracability_MMS_Read[MAX_STATIONS][2+1];


static double position_scroll=10;
static double position_Tip=10;

static int Scroll= 10;
static int Tipp= 10;  

double Offset_Torque_TIPP_WHEEL;


int Rodage_Pushs_Run;
int Rodage_Wheels_Run;
int Test_Torque_Run;

int Inhibit_Rodage_Pushs[MAX_STATIONS];

enum {
	CAL_TYPE_LINEAR = 1
};

typedef struct _SCalibrationPoint
{
	double	measValue;
	double	realValue;
} SCalibrationPoint, *SCalibrationPointPtr;

typedef struct _SLinearCalibration
{
	int						count;
	SCalibrationPointPtr	pPoints;	
} SLinearCalibration, *SLinearCalibrationPtr;

typedef struct _SCalibration
{
	int		calType;
	void*	pCalibData;
} SCalibration, *SCalibrationPtr;


typedef struct MaxMinScrollWheel{
	
	double *PeaksAmps;
	double *PeaksLocs;
	int *PeakNum;
	
	double *ValleysAmps;
	double *ValleysLocs;
	int *ValleyNum;
	
}MaxMinScrollWheel;

typedef struct MaxMinScrollWheel_up{
	
	double *PeaksAmps_up;
	double *PeaksLocs_up;
	int *PeakNum_up;
	
	double *ValleysAmps_up;
	double *ValleysLocs_up;
	int *ValleyNum_up;
	
}MaxMinScrollWheel_up;

typedef struct LesPoints{
	
	double *Amp;
	double *Loc;
	int *Label;
	
}LesPoints;

typedef struct LesPoints_up{
	
	double *Amp_up;
	double *Loc_up;
	int *Label_up;
	
}LesPoints_up;

typedef struct _SRodageThreadData{
	/* input data */
	SmotorPtr			  pmotor; 
	double			 	  angle;
	double			 	  speed;
	uint32_t			  count;
	int32_t				  timeout;
} SRodageThreadData, *SRodageThreadDataPtr;


typedef struct _SSensorThreadData{
	SaxisPtr			  paxis; 
} SSensorThreadData, *SSensorThreadDataPtr;

typedef struct _SLinThreadData{
	/* input data */
	SCANListPtr			  pcan;
	int32_t				  timeout;
	uint32_t			  station;
	char 				  lin_channel[32];
	/* returned data */
} SLinThreadData, *SLinThreadDataPtr;

typedef struct _SAquisitionThreadData{
	/* input data */
	ViSession		      vi_DAQ;
	SaxisPtr			  paxis; 
	SCANListPtr			  pcan;
	char				  Channel[32];
	uint32_t			  index;
	uint32_t			  station;
	int32_t				  timeout;
	double                position_forward;
	double                position_backward;
	char				  push_name[32];
	/* returned data */
	double 				  data[MESS_BUF][BUFFERLEN];
	int					  read;
	double				  Start_push_position;
} SAquisitionThreadData, *SAquisitionThreadDataPtr;


typedef struct _STorqueThreadData{
	/* input data */
	ViSession		      vi_DAQ;
	SaxisPtr			  paxis; 
	SCANListPtr			  pcan;
	char				  Channel[32];
	char				  Scale[32];
	uint32_t			  index;
	uint32_t			  station;
	int32_t				  timeout;
	double                angle;
	double                speed;
	double                Torque_Max;
	double			      Reduction_Torque;
	double				  Reduction_Angle;
	char				  wheel_name[32];
	/* returned data */
	double 				  data[MESS_BUF2][BUFFERLEN2];
	double 				  data2[MESS_BUF2][BUFFERLEN2];
	int					  read;
	int					  read2;
	int                   NumPulse;

} STorqueThreadData, *STorqueThreadDataPtr;

CmtThreadLockHandle gLock;

int NumeroJour (int mois, int jours, int annee);

SElExceptionPtr MoveToPush(SaxisPtr paxis, STesterPtr ptester, int check, 
						   double position_axis_Z, double position_axis_Y, double position_axis_X,
						    const char *PEND_AXIS_Z, const char *PEND_AXIS_Y, const char *PEND_AXIS_X);
SElExceptionPtr MoveAxis2( SaxisPtr paxis, int axis, double position, double speed);

void ElSleep(DWORD time);
void StopThreads (int Station);
SElExceptionPtr GetNewSerial(SDBSPtr pdbs, int* counter);
int Write_Traca_MMS_NOK (int Station, STestParamPtr pTestParam);

DWORD WINAPI Thread_Aquisition_Force(LPVOID param);
DWORD WINAPI Thread_Aquisition_Torque_TIPP_WIPE(LPVOID param);
DWORD WINAPI Thread_Aquisition_Torque_SCROLL_WHEEL(LPVOID param);
DWORD WINAPI Thread_Phase_Detection_Left(LPVOID param); 
DWORD WINAPI Thread_Phase_Detection_Right(LPVOID param); 

DWORD WINAPI Thread_Lin_Read(LPVOID param);
DWORD WINAPI Thread_Lin_Write(LPVOID param);

DWORD WINAPI Thread_Reset_Sensor(LPVOID param);

	
bool_t Get_push_value (char *push_name, int station);
double Get_ADC_push_value (char *push_name, int station);

DWORD WINAPI Thread_Rodage_Wheel_Right(LPVOID param);
DWORD WINAPI Thread_Rodage_Wheel_Left(LPVOID param);

double getMinDoubleArray(double *Array);
double getMaxDoubleArray(double *Array);

double getMinDoubleArray2(double *Array);
double getMaxDoubleArray2(double *Array);

void SaveGraphPNG(int PanelHandle, int ControleHandle, char *PathNamePNG);
void MathFunction30(double sdWaveGes[MESS_BUF][BUFFERLEN],
				   int 	  length,
				   double *_dmF1V, double *_dmS1V, 
				   double *_dmF2V, double *_dmS2V, 
				   double *_dmF_END, double *_dmS_END, 
				   double *_dmF1R, double *_dmS1R, 
				   double *_dmF2R, double *_dmS2R);


void Moyenne_Glissante_Courbe_30(double sdWaveGes[BUFFERLEN],int length);
SElExceptionPtr Ajustement_Gain_Offset_30(char *Function, double* dmF1, double* dmF2, double* dmS3);

void MathFunction40(double sdWaveGes[MESS_BUF][BUFFERLEN],
				   int 	  length,
				   double *_dmF1V, double *_dmS1V, 
				   double *_dmF2V, double *_dmS2V, 
				   double *_dmF_END, double *_dmS_END, 
				   double *_dmF1R, double *_dmS1R, 
				   double *_dmF2R, double *_dmS2R);


void Moyenne_Glissante_Courbe_40(double sdWaveGes[BUFFERLEN],int length);


void AnalysisFunction(double sdWaveGes[MESS_BUF][BUFFERLEN],
				   		  int 	  length,
				   		  double *_dmF1V, double *_dmS1V, 
				   		  double *_dmF2V, double *_dmS2V, 
				   		  double *_dmF_END, double *_dmS_END, 
				   		  double *_dmF1R, double *_dmS1R, 
				   		  double *_dmF2R, double *_dmS2R);
						  
SElExceptionPtr Ajustement_Gain_Offset_40(char *Function, double* dmF1, double* dmF2, double* dmS3);

void MathFunction2(double sdWaveGes[MESS_BUF2][BUFFERLEN2],
				   int 	  length,
				   double *_dmF1V, double *_dmS1V, 
				   double *_dmF2V, double *_dmS2V, 
				   double *_dmF3V, double *_dmS3V, 
				   double *_dmF4V, double *_dmS4V,
				   double *_dmF5V, double *_dmS5V);

void Filter_Table (double *data, int size);

SElExceptionPtr CheckDirectory(const char* directory);


#if defined(__cplusplus)
    } /* extern "C" */
#endif

#endif /* __CMFB_PIANO_UTILS_H__ */
