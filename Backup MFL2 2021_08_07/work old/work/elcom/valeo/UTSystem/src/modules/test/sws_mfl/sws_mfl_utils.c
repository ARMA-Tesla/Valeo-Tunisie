#include <time.h> 
#include <test/eltest.h>
#include <sdk/mmsystem.h>
#include <test/test_helper_functions.h>
#include <toolbox.h>
#include "sws_mfl_utils.h" 
#include <multithread/multithread.h>
#include "asynctmr.h"
#include <analysis.h>
#include <rs232.h>
#include <formatio.h>
#include <exception/exception.h>
#include <export.h>


/* RIGHT SIDE */
/*
#define RIGHT_BOTTOM_LEFT_GAIN_F1     0.86
#define RIGHT_BOTTOM_LEFT_OFFSET_F1   0.65
#define RIGHT_BOTTOM_LEFT_GAIN_F2     0.8 //0.86
#define RIGHT_BOTTOM_LEFT_OFFSET_F2   0.0   //0.65
#define RIGHT_BOTTOM_LEFT_GAIN_S3     1.0
#define RIGHT_BOTTOM_LEFT_OFFSET_S3   0.0

#define RIGHT_BOTTOM_MID_GAIN_F1      0.92
#define RIGHT_BOTTOM_MID_OFFSET_F1    0.21
#define RIGHT_BOTTOM_MID_GAIN_F2      0.92
#define RIGHT_BOTTOM_MID_OFFSET_F2    0.21
#define RIGHT_BOTTOM_MID_GAIN_S3      1.0
#define RIGHT_BOTTOM_MID_OFFSET_S3    0.0

#define RIGHT_BOTTOM_RIGHT_GAIN_F1    0.72
#define RIGHT_BOTTOM_RIGHT_OFFSET_F1  1.17
#define RIGHT_BOTTOM_RIGHT_GAIN_F2    0.72
#define RIGHT_BOTTOM_RIGHT_OFFSET_F2  0.65 //1.17 
#define RIGHT_BOTTOM_RIGHT_GAIN_S3    1.0
#define RIGHT_BOTTOM_RIGHT_OFFSET_S3  0.0

#define RIGHT_MID_RIGHT_GAIN_F1       0.73
#define RIGHT_MID_RIGHT_OFFSET_F1     0.88  //-0.70
#define RIGHT_MID_RIGHT_GAIN_F2       0.73
#define RIGHT_MID_RIGHT_OFFSET_F2     0.0  //0.88
#define RIGHT_MID_RIGHT_GAIN_S3       1.0
#define RIGHT_MID_RIGHT_OFFSET_S3     0.0

#define RIGHT_MID_MID_GAIN_F1  	      0.8 //1//1.19
#define RIGHT_MID_MID_OFFSET_F1  	  0.15	 	 //0    -1
#define RIGHT_MID_MID_GAIN_F2  	      0.8   //1.19
#define RIGHT_MID_MID_OFFSET_F2  	  0.0   // -1
#define RIGHT_MID_MID_GAIN_S3  	      1.0
#define RIGHT_MID_MID_OFFSET_S3  	  0.0

#define RIGHT_MID_LEFT_GAIN_F1        0.8  //1.0
#define RIGHT_MID_LEFT_OFFSET_F1      -0.0   //-0.70
#define RIGHT_MID_LEFT_GAIN_F2        0.8
#define RIGHT_MID_LEFT_OFFSET_F2      -0.2
#define RIGHT_MID_LEFT_GAIN_S3        0.8
#define RIGHT_MID_LEFT_OFFSET_S3      0.0

#define RIGHT_TOP_RIGHT_GAIN_F1       1.0  // 1.0
#define RIGHT_TOP_RIGHT_OFFSET_F1     -0.0 //0.6 sute demande production 09.05.2019 et le 10.05.2019 on a remis l'encienne valeur 0.3 (demande prod)//-0.990
										   //ajouter -0.5 sute demande production 15.05.2019  probleme goupillage
#define RIGHT_TOP_RIGHT_GAIN_F2       0.8  //0.8
#define RIGHT_TOP_RIGHT_OFFSET_F2     -0.0
#define RIGHT_TOP_RIGHT_GAIN_S3       1.0
#define RIGHT_TOP_RIGHT_OFFSET_S3     0.0

#define RIGHT_TOP_LEFT_GAIN_F1        0.8 // 0.8
#define RIGHT_TOP_LEFT_OFFSET_F1      -0.0 // -0.5
#define RIGHT_TOP_LEFT_GAIN_F2        0.8
#define RIGHT_TOP_LEFT_OFFSET_F2      -0.0
#define RIGHT_TOP_LEFT_GAIN_S3        1.0
#define RIGHT_TOP_LEFT_OFFSET_S3      0.0
										

/* LEFT SIDE */
/*
#define LEFT_BOTTOM_LEFT_GAIN_F1      0.86  //0.98
#define LEFT_BOTTOM_LEFT_OFFSET_F1    0.2 //   0.25
#define LEFT_BOTTOM_LEFT_GAIN_F2      0.86
#define LEFT_BOTTOM_LEFT_OFFSET_F2    0.2
#define LEFT_BOTTOM_LEFT_GAIN_S3      1.0
#define LEFT_BOTTOM_LEFT_OFFSET_S3    0.0
										
#define LEFT_BOTTOM_MID_GAIN_F1       0.94	  // 0.86
#define LEFT_BOTTOM_MID_OFFSET_F1     0.2		//0.59
#define LEFT_BOTTOM_MID_GAIN_F2       0.86
#define LEFT_BOTTOM_MID_OFFSET_F2     0.2
#define LEFT_BOTTOM_MID_GAIN_S3       1.0
#define LEFT_BOTTOM_MID_OFFSET_S3     0.0

#define LEFT_BOTTOM_RIGHT_GAIN_F1     0.86 //0.98
#define LEFT_BOTTOM_RIGHT_OFFSET_F1   0.2   // 0.32 
#define LEFT_BOTTOM_RIGHT_GAIN_F2     0.86
#define LEFT_BOTTOM_RIGHT_OFFSET_F2   0.2 
#define LEFT_BOTTOM_RIGHT_GAIN_S3     1.0
#define LEFT_BOTTOM_RIGHT_OFFSET_S3   0.0

#define LEFT_MID_RIGHT_GAIN_F1        0.85
#define LEFT_MID_RIGHT_OFFSET_F1      0.32
#define LEFT_MID_RIGHT_GAIN_F2        0.85
#define LEFT_MID_RIGHT_OFFSET_F2      0.32
#define LEFT_MID_RIGHT_GAIN_S3        1.0
#define LEFT_MID_RIGHT_OFFSET_S3      0.0

#define LEFT_MID_MID_GAIN_F1  	      1.0//MA 1.0
#define LEFT_MID_MID_OFFSET_F1  	  0.0
#define LEFT_MID_MID_GAIN_F2  	      1.0
#define LEFT_MID_MID_OFFSET_F2  	  0.0
#define LEFT_MID_MID_GAIN_S3  	      1.0
#define LEFT_MID_MID_OFFSET_S3  	  0.0

#define LEFT_MID_LEFT_GAIN_F1         1.0
#define LEFT_MID_LEFT_OFFSET_F1       -0.0
#define LEFT_MID_LEFT_GAIN_F2         1.0
#define LEFT_MID_LEFT_OFFSET_F2       -0.0
#define LEFT_MID_LEFT_GAIN_S3         1.0
#define LEFT_MID_LEFT_OFFSET_S3       0.0

#define LEFT_TOP_RIGHT_GAIN_F1        0.99 // 0.98
#define LEFT_TOP_RIGHT_OFFSET_F1      0.5 // -1.17	
#define LEFT_TOP_RIGHT_GAIN_F2        1.0
#define LEFT_TOP_RIGHT_OFFSET_F2      -0.0
#define LEFT_TOP_RIGHT_GAIN_S3        0.6
#define LEFT_TOP_RIGHT_OFFSET_S3      0.0

#define LEFT_TOP_LEFT_GAIN_F1         1.00
#define LEFT_TOP_LEFT_OFFSET_F1       0.5    //-0.290
#define LEFT_TOP_LEFT_GAIN_F2         0.98
#define LEFT_TOP_LEFT_OFFSET_F2       -0.0
#define LEFT_TOP_LEFT_GAIN_S3         0.6  //1.0
#define LEFT_TOP_LEFT_OFFSET_S3       0.0
*/ 	//16062020

/* RIGHT SIDE */
#define RIGHT_BOTTOM_LEFT_GAIN_F1     0.9 //0.9
#define RIGHT_BOTTOM_LEFT_OFFSET_F1   0.8 // 0.7
#define RIGHT_BOTTOM_LEFT_GAIN_F2     0.8 //0.86
#define RIGHT_BOTTOM_LEFT_OFFSET_F2   0.65
#define RIGHT_BOTTOM_LEFT_GAIN_S3     1.0
#define RIGHT_BOTTOM_LEFT_OFFSET_S3   0.0

#define RIGHT_BOTTOM_MID_GAIN_F1      1.0  // 0.92
#define RIGHT_BOTTOM_MID_OFFSET_F1    0.41 // 0.31
#define RIGHT_BOTTOM_MID_GAIN_F2      0.85 //0.92
#define RIGHT_BOTTOM_MID_OFFSET_F2    0.21
#define RIGHT_BOTTOM_MID_GAIN_S3      1.0
#define RIGHT_BOTTOM_MID_OFFSET_S3    0.0

#define RIGHT_BOTTOM_RIGHT_GAIN_F1    0.83   // old 0.7 
#define RIGHT_BOTTOM_RIGHT_OFFSET_F1  1.3  // old 1.17
#define RIGHT_BOTTOM_RIGHT_GAIN_F2    0.65  //0.65
#define RIGHT_BOTTOM_RIGHT_OFFSET_F2  0.65 //1.17 
#define RIGHT_BOTTOM_RIGHT_GAIN_S3    1.0
#define RIGHT_BOTTOM_RIGHT_OFFSET_S3  0.0

#define RIGHT_MID_RIGHT_GAIN_F1       0.99
#define RIGHT_MID_RIGHT_OFFSET_F1     0.00  //-0.88 DE?AANDE suite analyse labo 14-07-2021
#define RIGHT_MID_RIGHT_GAIN_F2       0.65   //0.73
#define RIGHT_MID_RIGHT_OFFSET_F2     0.0  //0.88
#define RIGHT_MID_RIGHT_GAIN_S3       1.0
#define RIGHT_MID_RIGHT_OFFSET_S3     0.0

#define RIGHT_MID_MID_GAIN_F1  	      0.95 //0.9
#define RIGHT_MID_MID_OFFSET_F1  	  0.15	 	 //0    -1
#define RIGHT_MID_MID_GAIN_F2  	      0.75   //0.8
#define RIGHT_MID_MID_OFFSET_F2  	  0.0   // -1
#define RIGHT_MID_MID_GAIN_S3  	      1.0
#define RIGHT_MID_MID_OFFSET_S3  	  0.0

#define RIGHT_MID_LEFT_GAIN_F1        1.0  //1     0.8
#define RIGHT_MID_LEFT_OFFSET_F1      0.1   // old -0.2
#define RIGHT_MID_LEFT_GAIN_F2        0.75	 //0.8
#define RIGHT_MID_LEFT_OFFSET_F2      -0.0
#define RIGHT_MID_LEFT_GAIN_S3        0.8
#define RIGHT_MID_LEFT_OFFSET_S3      0.0

#define RIGHT_TOP_RIGHT_GAIN_F1       1.0  // 0.8
#define RIGHT_TOP_RIGHT_OFFSET_F1     0.0 //0.0 ncienne valeur 0.3 (demande prod)//-0.990
										   //ajouter -0.5 sute demande production 15.05.2019  probleme goupillage
#define RIGHT_TOP_RIGHT_GAIN_F2       0.8  //0.75
#define RIGHT_TOP_RIGHT_OFFSET_F2     0.2 // old 0.0
#define RIGHT_TOP_RIGHT_GAIN_S3       1.0
#define RIGHT_TOP_RIGHT_OFFSET_S3     0.0

#define RIGHT_TOP_LEFT_GAIN_F1        1.0 // 1 0.8
#define RIGHT_TOP_LEFT_OFFSET_F1      0.7 //0:5  0.0    -0.5  derniere ,odif 01092020 suite ,ail ,oenes
#define RIGHT_TOP_LEFT_GAIN_F2        0.75 // 0.8
#define RIGHT_TOP_LEFT_OFFSET_F2      -0.0
#define RIGHT_TOP_LEFT_GAIN_S3        1.0
#define RIGHT_TOP_LEFT_OFFSET_S3      0.0
										

/* LEFT SIDE */
#define LEFT_BOTTOM_LEFT_GAIN_F1      0.86  //0.98
#define LEFT_BOTTOM_LEFT_OFFSET_F1    0.7 //   0.5
#define LEFT_BOTTOM_LEFT_GAIN_F2      0.8  //0.86
#define LEFT_BOTTOM_LEFT_OFFSET_F2    0.15   //0.65
#define LEFT_BOTTOM_LEFT_GAIN_S3      1.0
#define LEFT_BOTTOM_LEFT_OFFSET_S3    0.0
										
#define LEFT_BOTTOM_MID_GAIN_F1       0.86	  // 0.94
#define LEFT_BOTTOM_MID_OFFSET_F1     0.7		//0.59 08.03.2120
#define LEFT_BOTTOM_MID_GAIN_F2       0.8	  //0.86
#define LEFT_BOTTOM_MID_OFFSET_F2     0.35	  //0.65
#define LEFT_BOTTOM_MID_GAIN_S3       1.0
#define LEFT_BOTTOM_MID_OFFSET_S3     0.0

#define LEFT_BOTTOM_RIGHT_GAIN_F1     0.86 //0.98
#define LEFT_BOTTOM_RIGHT_OFFSET_F1   0.65   // 0.32 
#define LEFT_BOTTOM_RIGHT_GAIN_F2     0.8   //0.86
#define LEFT_BOTTOM_RIGHT_OFFSET_F2   0.2   //0.5
#define LEFT_BOTTOM_RIGHT_GAIN_S3     1.0
#define LEFT_BOTTOM_RIGHT_OFFSET_S3   0.0

#define LEFT_MID_RIGHT_GAIN_F1        0.85
#define LEFT_MID_RIGHT_OFFSET_F1      0.32
#define LEFT_MID_RIGHT_GAIN_F2        0.8  //0.85
#define LEFT_MID_RIGHT_OFFSET_F2      0.32
#define LEFT_MID_RIGHT_GAIN_S3        1.0
#define LEFT_MID_RIGHT_OFFSET_S3      0.0

#define LEFT_MID_MID_GAIN_F1  	      1.0//MA 1.0
#define LEFT_MID_MID_OFFSET_F1  	  0.0
#define LEFT_MID_MID_GAIN_F2  	      0.95   //1.0
#define LEFT_MID_MID_OFFSET_F2  	  0.0
#define LEFT_MID_MID_GAIN_S3  	      1.0
#define LEFT_MID_MID_OFFSET_S3  	  0.0

#define LEFT_MID_LEFT_GAIN_F1         1.0
#define LEFT_MID_LEFT_OFFSET_F1       -0.0
#define LEFT_MID_LEFT_GAIN_F2         0.95  //1.0
#define LEFT_MID_LEFT_OFFSET_F2       -0.0
#define LEFT_MID_LEFT_GAIN_S3         1.0
#define LEFT_MID_LEFT_OFFSET_S3       0.0

#define LEFT_TOP_RIGHT_GAIN_F1        1.0 // 1.11
#define LEFT_TOP_RIGHT_OFFSET_F1      0.3 // 0:3  -1.17	
#define LEFT_TOP_RIGHT_GAIN_F2        0.9  //0.98
#define LEFT_TOP_RIGHT_OFFSET_F2      -0.0
#define LEFT_TOP_RIGHT_GAIN_S3        0.6
#define LEFT_TOP_RIGHT_OFFSET_S3      0.0

#define LEFT_TOP_LEFT_GAIN_F1         0.86
#define LEFT_TOP_LEFT_OFFSET_F1       1.05    //-0.290
#define LEFT_TOP_LEFT_GAIN_F2         0.9   //0.98
#define LEFT_TOP_LEFT_OFFSET_F2       -0.0
#define LEFT_TOP_LEFT_GAIN_S3         0.6  //1.0
#define LEFT_TOP_LEFT_OFFSET_S3       0.0
 
#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2

#define RATE    	20
#define MOYENNE     10

#define STATUS_IDLE  	  0
#define STATUS_ACTIVE	  1

#define HEADER      0X3A       // Header function
#define LF      	0X0A      // Line feed
#define CR      	0X0D      // Carriage return
#define END      	0X00      // end string 


#define CR_CHAR			'\x0D'	// CR character
#define LF_CHAR			'\x0A'  // LF character

extern SObjListPtr 		gs_Modules;

static int          	g_timerId_TIPP_WIPE;
static int          	g_timerId_SCROLL_WHEEL;
double   				Angle_Tipp_Wheel;
double   				Angle_Scroll_Wheel;


/* register data of status pushs */
uint8_t	data_pushs[MAX_STATIONS][8];
uint8_t All_data_pushs[MAX_STATIONS][2048][8];
uint8_t	ADC_pushs[MAX_STATIONS][8];



#define _DEBUG   1

#define TIMEOUT_MOTOR_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_MOTOR_ANGLE_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( -1, "Motor Angle error"); } else

#define TIMEOUT_AXIS_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_AXIS_PEND_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( -1, "Axis position end error"); } else

#define TIMEOUT_AXIS_POSITION_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( -1, "Axis position error"); } else

#define TIMEOUT_LIN_INIT() 	clock_t	_start_clock = clock();

#define TIMEOUT_LIN_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > (timeout/1000.0) ) { EXCTHROW( -1, "Error communication Lin"); } else


bool_t Get_push_value_from_name (uint8_t data[8], char *push_name);
double Get_ADC_value (uint8_t data[8], char *push_name);

SElExceptionPtr Read_ADC(SCANListPtr pcan, int station);

int crc_table[256] =
  {
    0x00, 0x1d, 0x3a, 0x27, 0x74, 0x69, 0x4e, 0x53,
    0xe8, 0xf5, 0xd2, 0xcf, 0x9c, 0x81, 0xa6, 0xbb,
    0xcd, 0xd0, 0xf7, 0xea, 0xb9, 0xa4, 0x83, 0x9e,
    0x25, 0x38, 0x1f, 0x02, 0x51, 0x4c, 0x6b, 0x76,
    0x87, 0x9a, 0xbd, 0xa0, 0xf3, 0xee, 0xc9, 0xd4,
    0x6f, 0x72, 0x55, 0x48, 0x1b, 0x06, 0x21, 0x3c,
    0x4a, 0x57, 0x70, 0x6d, 0x3e, 0x23, 0x04, 0x19,
    0xa2, 0xbf, 0x98, 0x85, 0xd6, 0xcb, 0xec, 0xf1,
    0x13, 0x0e, 0x29, 0x34, 0x67, 0x7a, 0x5d, 0x40,
    0xfb, 0xe6, 0xc1, 0xdc, 0x8f, 0x92, 0xb5, 0xa8,
    0xde, 0xc3, 0xe4, 0xf9, 0xaa, 0xb7, 0x90, 0x8d,
    0x36, 0x2b, 0x0c, 0x11, 0x42, 0x5f, 0x78, 0x65,
    0x94, 0x89, 0xae, 0xb3, 0xe0, 0xfd, 0xda, 0xc7,
    0x7c, 0x61, 0x46, 0x5b, 0x08, 0x15, 0x32, 0x2f,
    0x59, 0x44, 0x63, 0x7e, 0x2d, 0x30, 0x17, 0x0a,
    0xb1, 0xac, 0x8b, 0x96, 0xc5, 0xd8, 0xff, 0xe2,
    0x26, 0x3b, 0x1c, 0x01, 0x52, 0x4f, 0x68, 0x75,
    0xce, 0xd3, 0xf4, 0xe9, 0xba, 0xa7, 0x80, 0x9d,
    0xeb, 0xf6, 0xd1, 0xcc, 0x9f, 0x82, 0xa5, 0xb8,
    0x03, 0x1e, 0x39, 0x24, 0x77, 0x6a, 0x4d, 0x50,
    0xa1, 0xbc, 0x9b, 0x86, 0xd5, 0xc8, 0xef, 0xf2,
    0x49, 0x54, 0x73, 0x6e, 0x3d, 0x20, 0x07, 0x1a,
    0x6c, 0x71, 0x56, 0x4b, 0x18, 0x05, 0x22, 0x3f,
    0x84, 0x99, 0xbe, 0xa3, 0xf0, 0xed, 0xca, 0xd7,
    0x35, 0x28, 0x0f, 0x12, 0x41, 0x5c, 0x7b, 0x66,
    0xdd, 0xc0, 0xe7, 0xfa, 0xa9, 0xb4, 0x93, 0x8e,
    0xf8, 0xe5, 0xc2, 0xdf, 0x8c, 0x91, 0xb6, 0xab,
    0x10, 0x0d, 0x2a, 0x37, 0x64, 0x79, 0x5e, 0x43,
    0xb2, 0xaf, 0x88, 0x95, 0xc6, 0xdb, 0xfc, 0xe1,
    0x5a, 0x47, 0x60, 0x7d, 0x2e, 0x33, 0x14, 0x09,
    0x7f, 0x62, 0x45, 0x58, 0x0b, 0x16, 0x31, 0x2c,
    0x97, 0x8a, 0xad, 0xb0, 0xe3, 0xfe, 0xd9, 0xc4
  };

typedef enum _EStation
{
	STATION_10 = 0,
	STATION_20,
	STATION_30,
	STATION_40,
	STATION_50,
	STATION_60,
	STATION_70,
	STATION_80,
} EStation;

/* Numéro jour dans l'année **************************************************/
int NumeroJour (int mois, int jours, int annee)
{
	int nbJours;
  	
	switch (mois) 
	{
   		case 1 : nbJours = jours; break;
   		case 2 : nbJours = 31 + jours; break;
   		case 3 : nbJours = 31 + 28 + jours; break;
   		case 4 : nbJours = 31 + 28 + 31 + jours; break;
   		case 5 : nbJours = 31 + 28 + 31 + 30 + jours; break;
   		case 6 : nbJours = 31 + 28 + 31 + 30 + 31 + jours; break;
   		case 7 : nbJours = 31 + 28 + 31 + 30 + 31 + 30 + jours; break;
   		case 8 : nbJours = 31 + 28 + 31 + 30 + 31 + 30 + 31 + jours; break;
   		case 9 : nbJours = 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + jours; break;
   		case 10: nbJours = 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + jours; break;
   		case 11: nbJours = 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + jours; break;
   		case 12: nbJours = 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + jours;
	};

	/* Si année bissextile */
	if (((annee % 4) == 0) && (mois > 2))
		nbJours ++;

	return nbJours;	
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Ajustement_Gain_Offset_30"
SElExceptionPtr Ajustement_Gain_Offset_30(char *Function, double* dmF1, double* dmF2, double* dmS3)
{
	char *End;
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char            msg[512];
	double Val1=0.0,
	 	   Val2=0.0,
	       Val3=0.0;
	
    char Chaine[1024]="";
	int file;
	char Line[128]="";
	char Val1Ch[10]="";
	char Val2Ch[10]="";
	char Val3Ch[10]="";
	char DirNew[128]="";
	char TimeStrNow[9]="";
	
#if WRITE_FILE
	sprintf(Chaine,"C:\\FlexiTester.OK\\LogNew\\%s.txt",Function);
	
	file=OpenFile (Chaine, VAL_WRITE_ONLY, VAL_APPEND, VAL_ASCII);

#endif   
	
// Right Side 
if (!strcmp(Function, "PHONE"))
{
	Val1  = *dmF1 * RIGHT_BOTTOM_LEFT_GAIN_F1 + RIGHT_BOTTOM_LEFT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_BOTTOM_LEFT_GAIN_F2 + RIGHT_BOTTOM_LEFT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_BOTTOM_LEFT_GAIN_S3 + RIGHT_BOTTOM_LEFT_OFFSET_S3;
}

if (!strcmp(Function, "LIST"))
{
	Val1  = *dmF1 * RIGHT_BOTTOM_MID_GAIN_F1 + RIGHT_BOTTOM_MID_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_BOTTOM_MID_GAIN_F2 + RIGHT_BOTTOM_MID_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_BOTTOM_MID_GAIN_S3 + RIGHT_BOTTOM_MID_OFFSET_S3;
}

if (!strcmp(Function, "SPEECH"))
{
	Val1  = *dmF1 * RIGHT_BOTTOM_RIGHT_GAIN_F1 + RIGHT_BOTTOM_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_BOTTOM_RIGHT_GAIN_F2 + RIGHT_BOTTOM_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_BOTTOM_RIGHT_GAIN_S3 + RIGHT_BOTTOM_RIGHT_OFFSET_S3;
}

if (!strcmp(Function, "MENU_RIGHT"))
{
	Val1  = *dmF1 * RIGHT_MID_RIGHT_GAIN_F1 + RIGHT_MID_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_MID_RIGHT_GAIN_F2 + RIGHT_MID_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_MID_RIGHT_GAIN_S3 + RIGHT_MID_RIGHT_OFFSET_S3;
}

if (!strcmp(Function, "SCROLL_WHEEL"))
{
	Val1  = *dmF1 * RIGHT_MID_MID_GAIN_F1 + RIGHT_MID_MID_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_MID_MID_GAIN_F2 + RIGHT_MID_MID_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_MID_MID_GAIN_S3 + RIGHT_MID_MID_OFFSET_S3;
}

if (!strcmp(Function, "MENU_LEFT"))
{
	Val1  = *dmF1 * RIGHT_MID_LEFT_GAIN_F1 + RIGHT_MID_LEFT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_MID_LEFT_GAIN_F2 + RIGHT_MID_LEFT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_MID_LEFT_GAIN_S3 + RIGHT_MID_LEFT_OFFSET_S3;
}

if (!strcmp(Function, "VOLUME_PLUS"))
{
	Val1  = *dmF1 * RIGHT_TOP_RIGHT_GAIN_F1 + RIGHT_TOP_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_TOP_RIGHT_GAIN_F2 + RIGHT_TOP_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_TOP_RIGHT_GAIN_S3 + RIGHT_TOP_RIGHT_OFFSET_S3;
}

if (!strcmp(Function, "VOLUME_MINUS"))
{
	Val1  = *dmF1 * RIGHT_TOP_LEFT_GAIN_F1 + RIGHT_TOP_LEFT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_TOP_LEFT_GAIN_F2 + RIGHT_TOP_LEFT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_TOP_LEFT_GAIN_S3 + RIGHT_TOP_LEFT_OFFSET_S3;
}


// Left Side
if (strcmp (Function, "L_BOTTOM_LEFT") == 0)
{
	Val1  = *dmF1 * LEFT_BOTTOM_LEFT_GAIN_F1 + LEFT_BOTTOM_LEFT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_BOTTOM_LEFT_GAIN_F2 + LEFT_BOTTOM_LEFT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_BOTTOM_LEFT_GAIN_S3 + LEFT_BOTTOM_LEFT_OFFSET_S3;
}
		
if (strcmp (Function, "L_BOTTOM_MID") == 0)
{
	Val1  = *dmF1 * LEFT_BOTTOM_MID_GAIN_F1 + LEFT_BOTTOM_MID_OFFSET_F1;
	Val2  = *dmF2 * LEFT_BOTTOM_MID_GAIN_F2 + LEFT_BOTTOM_MID_OFFSET_F2;
	Val3  = *dmS3 * LEFT_BOTTOM_MID_GAIN_S3 + LEFT_BOTTOM_MID_OFFSET_S3;
}
		
if  (strcmp (Function, "L_BOTTOM_RIGHT") == 0)
{
	Val1  = *dmF1 * LEFT_BOTTOM_RIGHT_GAIN_F1 + LEFT_BOTTOM_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_BOTTOM_RIGHT_GAIN_F2 + LEFT_BOTTOM_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_BOTTOM_RIGHT_GAIN_S3 + LEFT_BOTTOM_RIGHT_OFFSET_S3;
}

if  (strcmp (Function, "L_MID_LEFT") == 0)
{
 	Val1  = *dmF1 * LEFT_MID_LEFT_GAIN_F1 + LEFT_MID_LEFT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_MID_LEFT_GAIN_F2 + LEFT_MID_LEFT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_MID_LEFT_GAIN_S3 + LEFT_MID_LEFT_OFFSET_S3;
}

if  (strcmp (Function, "L_MID_RIGHT") == 0)
{
 	Val1  = *dmF1 * LEFT_MID_RIGHT_GAIN_F1 + LEFT_MID_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_MID_RIGHT_GAIN_F2 + LEFT_MID_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_MID_RIGHT_GAIN_S3 + LEFT_MID_RIGHT_OFFSET_S3;	
}

if  (strcmp (Function, "L_TOP_LEFT") == 0)
{
 	Val1  = *dmF1 * LEFT_TOP_LEFT_GAIN_F1 + LEFT_TOP_LEFT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_TOP_LEFT_GAIN_F2 + LEFT_TOP_LEFT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_TOP_LEFT_GAIN_S3 + LEFT_TOP_LEFT_OFFSET_S3;	
}
		
if  (strcmp (Function, "L_TOP_RIGHT") == 0)
{
 	Val1 = *dmF1 * LEFT_TOP_RIGHT_GAIN_F1 + LEFT_TOP_RIGHT_OFFSET_F1;
	Val2 = *dmF2 * LEFT_TOP_RIGHT_GAIN_F2 + LEFT_TOP_RIGHT_OFFSET_F2;
    Val3 = *dmS3 * LEFT_TOP_RIGHT_GAIN_S3 + LEFT_TOP_RIGHT_OFFSET_S3;
}

//dmF1  = Val1  ;
//dmF2  = Val2  ;
//dmS3  = Val3  ;

//#if 0    /* formerly excluded lines */
 //sscanf("F1 : %0.00f;F2 : %0.00f;S3 : %0.00f\r",Val1Ch,Val2Ch,Val1Ch);



#if 0    /* formerly excluded lines */
 *dmF1=((int)(Val1*10));
 *dmF2=((int)(Val2*10));
 *dmS3=((int)(Val3*10));
 
 *dmF1  /= 10  ;
 *dmF2  /= 10 ; 
 *dmS3  /= 10;  
#endif   /* formerly excluded lines */
 
 *dmF1=Val1;
 *dmF2=Val2;
 *dmS3=Val3;
 
 
 #if WRITE_FILE
  sprintf(Line,"%s;F1 : %f;F2 : %f;S3 : %f\r",TimeStr(),Val1,Val2,Val3);
  WriteLine (file, Line, -1);
  CloseFile (file);
 #endif   


//else  				   
//	{
//	sprintf (msg, "Function not exist : %s!", Function);
	//MessagePopup ("MFL Function", msg);
//	}

Error:
	EXCRETHROW( pexception);  
}

#undef __FUNC__
#define __FUNC__	"LogAxisError"
void LogAxisError(int ErrorCode, float ForceValue, int channel_index)   
{
	SElExceptionPtr	pexception = NULL;
	static HANDLE			hevent;
	DWORD 					result;
	HRESULT         	hr = 0;
	CAObjHandle     	pxml = 0; 
	CAObjHandle     	pnode = 0; 
	const char*     	pfile_form_xml = "test_form.xml";
	VBOOL            	is_xml_success = 1;
	VARIANT         	pfile_name_var = CA_VariantEmpty();
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN],
						fileName[MAX_PATHNAME_LEN],
						csv_dir[MAX_PATHNAME_LEN],
						sdate_time[256],
						buffer[2048] = "";
	SYSTEMTIME			st;

	
	CAObjHandle 		pTicket;
	CAObjHandle 		pTicket_new = 0;
	char*           	ptext = NULL;
    int                 HandleFile;
	char				log[256];
	char                station[32];
	
	
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\Axis\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	if (channel_index==1)
		sprintf(station,"%s","STATION_30_RIGHT");
	else if (channel_index==2)
		sprintf(station,"%s","STATION_30_LEFT");
	if (channel_index==3)
		sprintf(station,"%s","STATION_40_RIGHT");
	if (channel_index==4)
		sprintf(station,"%s","STATION_30_LEFT");			
				
	
	/* enreg des data */
	
	memset(log, 0x00, 256);
	
	sprintf (fileName, "%s/Log_Buffer.txt", result_dir);
	
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	
//	sprintf (log, "18: 30 -- hello && it is me");
//	sprintf (log, sdate_time, DATE_TIME_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf (log,"---------------------------------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\nError_Code=%d\r\nForce_Value=%f\r\nStation=%s\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, ErrorCode, ForceValue, station);  
//	sprintf (log, "IS_BARCODE_REAR=%d\r\n", is_barcode_rear); 
//	sprintf (log, "SENSOR_CYLINDER_X_REAR_STATION_80=", is_cylinder_x_rear); 
	
	
	WriteLine (HandleFile, log, sizeof (log));	
	
	CloseFile (HandleFile);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Ajustement_Gain_Offset_40"
SElExceptionPtr Ajustement_Gain_Offset_40(char *Function, double* dmF1, double* dmF2, double* dmS3)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char            msg[512];
	double Val1=0.0,Val3=0.0,Val2=0.0;
	 char *End;
	char Chaine[1024]="";
	int file;
	char Line[128]="";
	char Val1Ch[10]="";
	char Val2Ch[10]="";
	char Val3Ch[10]="";
	
	
#if WRITE_FILE
	
	
	sprintf(Chaine,"C:\\FlexiTester.OK\\LogNew\\%s.txt",Function);
	
	file=OpenFile (Chaine, VAL_WRITE_ONLY, VAL_APPEND, VAL_ASCII);
#endif   
	
// Right Side 
if (!strcmp(Function, "PHONE"))
{
	Val1  = *dmF1 * RIGHT_BOTTOM_LEFT_GAIN_F1 + RIGHT_BOTTOM_LEFT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_BOTTOM_LEFT_GAIN_F2 + RIGHT_BOTTOM_LEFT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_BOTTOM_LEFT_GAIN_S3 + RIGHT_BOTTOM_LEFT_OFFSET_S3;
}

if (!strcmp(Function, "LIST"))
{
	Val1  = *dmF1 * RIGHT_BOTTOM_MID_GAIN_F1 + RIGHT_BOTTOM_MID_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_BOTTOM_MID_GAIN_F2 + RIGHT_BOTTOM_MID_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_BOTTOM_MID_GAIN_S3 + RIGHT_BOTTOM_MID_OFFSET_S3;
}

if (!strcmp(Function, "SPEECH"))
{
	Val1  = *dmF1 * RIGHT_BOTTOM_RIGHT_GAIN_F1 + RIGHT_BOTTOM_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_BOTTOM_RIGHT_GAIN_F2 + RIGHT_BOTTOM_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_BOTTOM_RIGHT_GAIN_S3 + RIGHT_BOTTOM_RIGHT_OFFSET_S3;
}

if (!strcmp(Function, "MENU_RIGHT"))
{
	Val1  = *dmF1 * RIGHT_MID_RIGHT_GAIN_F1 + RIGHT_MID_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_MID_RIGHT_GAIN_F2 + RIGHT_MID_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_MID_RIGHT_GAIN_S3 + RIGHT_MID_RIGHT_OFFSET_S3;
}

if (!strcmp(Function, "SCROLL_WHEEL"))
{
	Val1  = *dmF1 * RIGHT_MID_MID_GAIN_F1 + RIGHT_MID_MID_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_MID_MID_GAIN_F2 + RIGHT_MID_MID_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_MID_MID_GAIN_S3 + RIGHT_MID_MID_OFFSET_S3;
}

if (!strcmp(Function, "MENU_LEFT"))
{
	Val1  = *dmF1 * RIGHT_MID_LEFT_GAIN_F1 + RIGHT_MID_LEFT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_MID_LEFT_GAIN_F2 + RIGHT_MID_LEFT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_MID_LEFT_GAIN_S3 + RIGHT_MID_LEFT_OFFSET_S3;
}

if (!strcmp(Function, "VOLUME_PLUS"))
{
	Val1  = *dmF1 * RIGHT_TOP_RIGHT_GAIN_F1 + RIGHT_TOP_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_TOP_RIGHT_GAIN_F2 + RIGHT_TOP_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_TOP_RIGHT_GAIN_S3 + RIGHT_TOP_RIGHT_OFFSET_S3;
}

if (!strcmp(Function, "VOLUME_MINUS"))
{
	Val1  = *dmF1 * RIGHT_TOP_LEFT_GAIN_F1 + RIGHT_TOP_LEFT_OFFSET_F1;
	Val2  = *dmF2 * RIGHT_TOP_LEFT_GAIN_F2 + RIGHT_TOP_LEFT_OFFSET_F2;
	Val3  = *dmS3 * RIGHT_TOP_LEFT_GAIN_S3 + RIGHT_TOP_LEFT_OFFSET_S3;
}


// Left Side
if (strcmp (Function, "L_BOTTOM_LEFT") == 0)
{
Val1  = *dmF1 * LEFT_BOTTOM_LEFT_GAIN_F1 + LEFT_BOTTOM_LEFT_OFFSET_F1;
Val2  = *dmF2 * LEFT_BOTTOM_LEFT_GAIN_F2 + LEFT_BOTTOM_LEFT_OFFSET_F2;
Val3  = *dmS3 * LEFT_BOTTOM_LEFT_GAIN_S3 + LEFT_BOTTOM_LEFT_OFFSET_S3;
}
		
if (strcmp (Function, "L_BOTTOM_MID") == 0)
{
	Val1  = *dmF1 * LEFT_BOTTOM_MID_GAIN_F1 + LEFT_BOTTOM_MID_OFFSET_F1;
	Val2  = *dmF2 * LEFT_BOTTOM_MID_GAIN_F2 + LEFT_BOTTOM_MID_OFFSET_F2;
	Val3  = *dmS3 * LEFT_BOTTOM_MID_GAIN_F2 + LEFT_BOTTOM_MID_OFFSET_S3;
}
		
if  (strcmp (Function, "L_BOTTOM_RIGHT") == 0)
{
	Val1  = *dmF1 * LEFT_BOTTOM_RIGHT_GAIN_F1 + LEFT_BOTTOM_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_BOTTOM_RIGHT_GAIN_F2 + LEFT_BOTTOM_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_BOTTOM_RIGHT_GAIN_S3 + LEFT_BOTTOM_RIGHT_OFFSET_S3;
}

if  (strcmp (Function, "L_MID_LEFT") == 0)
{
 	Val1  = *dmF1 * LEFT_MID_LEFT_GAIN_F1 + LEFT_MID_LEFT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_MID_LEFT_GAIN_F2 + LEFT_MID_LEFT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_MID_LEFT_GAIN_S3 + LEFT_MID_LEFT_OFFSET_S3;
}

if  (strcmp (Function, "L_MID_RIGHT") == 0)
{
 	Val1  = *dmF1 * LEFT_MID_RIGHT_GAIN_F1 + LEFT_MID_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_MID_RIGHT_GAIN_F2 + LEFT_MID_RIGHT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_MID_RIGHT_GAIN_S3 + LEFT_MID_RIGHT_OFFSET_S3;	
}

if  (strcmp (Function, "L_TOP_LEFT") == 0)
{
 	Val1  = *dmF1 * LEFT_TOP_LEFT_GAIN_F1 + LEFT_TOP_LEFT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_TOP_LEFT_GAIN_F2 + LEFT_TOP_LEFT_OFFSET_F2;
	Val3  = *dmS3 * LEFT_TOP_LEFT_GAIN_S3 + LEFT_TOP_LEFT_OFFSET_S3;	
}
		
if  (strcmp (Function, "L_TOP_RIGHT") == 0)
{
 	Val1  = *dmF1 * LEFT_TOP_RIGHT_GAIN_F1 + LEFT_TOP_RIGHT_OFFSET_F1;
	Val2  = *dmF2 * LEFT_TOP_RIGHT_GAIN_F2 + LEFT_TOP_RIGHT_OFFSET_F2;
    Val3  = *dmS3 * LEFT_TOP_RIGHT_GAIN_S3 + LEFT_TOP_RIGHT_OFFSET_S3;
}

*dmF1  = Val1;
*dmF2  = Val2;
*dmS3  = Val3;

#if 0    /* formerly excluded lines */
*dmF1  = ((int)(Val1*10))  ;
*dmF2  = ((int)(Val2*10))  ;
*dmS3  = ((int)(Val3*10))  ;

*dmF1  /= 10;
*dmF2  /= 10;
*dmS3  /= 10;
#endif   /* formerly excluded lines */

#if WRITE_FILE
	sprintf(Line,"%s;F1 : %f;F2 : %f;S3 : %f\r",TimeStr(),Val1,Val2,Val3);
	WriteLine (file, Line, -1);
    CloseFile (file);
#endif
	

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetNewSerial"
SElExceptionPtr GetNewSerial(SDBSPtr pdbs, int* counter)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	EIdShift		shift_id = 0;
	bool_t			isOpen = FALSE;
	

		EXCCHECK( pdbs->Storage->IsShiftOpen(pdbs->Storage, &isOpen, &shift_id));
		
		if(isOpen)
		{
			EXCCHECK( pdbs->Storage->GetProductCounter(pdbs->Storage, pdbs->TesterProductId, counter));
		}

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Write_Traca_MMS_NOK"
int Write_Traca_MMS_NOK (int Station, STestParamPtr pTestParam)
{
	SElExceptionPtr pexception = NULL;
	char 			part_number[128];
	char 			revision_index[128];
	char 			lineID[128];
	int 			Year,Month,Day,iDays;

	void* pTID = pTestParam;

	if (pTestParam->Write_Tracability_MMS == TRUE) //if autorisation write MAN DATA 2
		{
		iFlag_Write_Valeo_production_Key[Station] = TRUE;
	
		Sleep (60);
	
		PARAM_STRING_REQUIRED( "ProductPartNumber", part_number);
		PARAM_STRING_REQUIRED( "Revision_Index", revision_index);
		PARAM_STRING_REQUIRED( "LineID", lineID);

		/* VALEO Part Nember */
		sprintf (MAN_DATA_2_VALEO_Part_Number_Write[Station], "%s", part_number);
	
		/* Revision_Index */
		sprintf (MAN_DATA_2_Revision_Index_Write[Station], "%s", revision_index);
	
		/* LineID */
		sprintf (MAN_DATA_2_LineID_Write[Station], "%s", lineID);
	
		/* Tracability FCT TEST NOK */
		sprintf (MAN_DATA_2_Tracability_MMS_Write[Station], "20");
	
		/* production date*/
		sscanf (((STestParamPtr)pTID)->Date, "%04d.%02d.%02d ", &Year, &Month, &Day);
		sprintf (MAN_DATA_2_Production_Year_Write[Station], "%02d", Year % 100);
		sprintf (MAN_DATA_2_Production_Month_Write[Station], "%02d", Month);
		sprintf (MAN_DATA_2_Production_Day_Write[Station], "%02d", Day);

		/* production_serial 8 digits */
		/* a jjj XXXX 
	    a : année -2017 = (ex 2019  a=2)
	    jjj : jour/année
	    XXXX :compteur */
		sscanf (((STestParamPtr)pTID)->Date, "%04d.%02d.%02d ", &Year, &Month, &Day);
		iDays = NumeroJour (Month, Day, Year);
  		sprintf (MAN_DATA_2_Serial_Write[Station],"%d%03d%s", Year - 2017, iDays, ((STestParamPtr)pTID)->Production_Serial);
		
   		iFlag_Write_MAN_DATA_2[Station] = TRUE;
	
		Sleep (100);
	
		}
Error:
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "StopThreads"
void StopThreads (int Station)
{
	iFlag_Thread_Lin_Write[Station] = FALSE;
	Sleep(50); //wait LIN Bus is free  
	iFlag_Thread_Lin_Read[Station] = FALSE;
}

int Get_Variant_Code (char variant[2+1])
{
	variant[2] = 0X00;
	
	if (strcmp("F0", variant) == 0)
		return 0XF0;
		
	if (strcmp("F1", variant) == 0)
		return 0XF1;
	
	if (strcmp("F2", variant) == 0)
		return 0XF2;
		
	if (strcmp("F3", variant) == 0)
		return 0XF3;
		
	if (strcmp("F4", variant) == 0)
		return 0XF4;
	
	else 
		 return 0XFF;
}

void CalcChecksumV1(unsigned char *Buffer, int *Checksum)
{
int iCompt=0;

int crc_value=0;
crc_value=crc_table[0xE8];
crc_value=crc_table[crc_value];
for(iCompt=0;iCompt<7;iCompt++)
{
crc_value=crc_table[(crc_value^Buffer[iCompt])];
}
*Checksum=crc_value;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ElSleep"
void ElSleep(DWORD time)
{
	timeBeginPeriod(1);
	Sleep( (DWORD)time);
	timeEndPeriod(1);
}

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__  "Timer_Angle_TIPP_WIPE"
int CVICALLBACK Timer_Angle_TIPP_WIPE (int reserved, int theTimerId, int event,
                                 	   void *callbackData, int eventData1,
                                 	   int eventData2)
{                                                   
	SElExceptionPtr	pexception = NULL;
	Smotor* me = (SmotorPtr) callbackData; 
	double Currentangle;
	
	me->CurrentPosition(me, 3, &Currentangle);
	Angle_Tipp_Wheel  = fabs(Currentangle);
	
Error:
	if (pexception)
	{
	EXCDISPLAY(pexception);
	DiscardAsyncTimer (g_timerId_TIPP_WIPE);
	}
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__  "Timer_Angle_SCROLL_WHEEL"
int CVICALLBACK Timer_Angle_SCROLL_WHEEL (int reserved, int theTimerId, int event,
                                 	   void *callbackData, int eventData1,
                                 	   int eventData2)
{                                                   
	SElExceptionPtr	pexception = NULL;
	Smotor* me = (SmotorPtr) callbackData; 
	double Currentangle;
	
	me->CurrentPosition(me, 2, &Currentangle);
	Angle_Scroll_Wheel  = fabs(Currentangle);
	//printf ("Timer_Angle_SCROLL_WHEEL : Currentangle = %f\r\n", Currentangle);
	//Sleep(50);
	
Error:
	if (pexception)
	{
	EXCDISPLAY(pexception);
	DiscardAsyncTimer (g_timerId_SCROLL_WHEEL);
	}
	return 0;
}

#undef __FUNC__
#define __FUNC__ "MoveAxis2"
SElExceptionPtr MoveAxis2( SaxisPtr paxis, int axis, double position, double speed)
{
	SElExceptionPtr pexception = NULL;
	int status;
	
	paxis->SetPosition(paxis, axis, position, speed);
	paxis->SetPosition(paxis, axis, position, speed); 
	paxis->EnableAxis(paxis, axis); 
	paxis->EnableAxis(paxis, axis); 

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "MoveAxis"
SElExceptionPtr MoveAxis( SaxisPtr paxis, int axis, double position)
{
	SElExceptionPtr pexception = NULL;
	double Initial_Position;
	double Currentposition;
	int  status;

	paxis->AlarmReset(paxis, axis);
	paxis->EnableAxis(paxis, axis);
	paxis->ControllerStatus(paxis, axis, 1);  //SERVO ON

	if (axis == 2)
	{
	paxis->EnableAxis(paxis, axis); 
	paxis->SetPosition(paxis, axis, position, 50.0); 
	paxis->SetPosition(paxis, axis, position, 50.0);
	paxis->EnableAxis(paxis, axis);
	}
	
	else 
	{
	paxis->EnableAxis(paxis, axis); 
	paxis->SetPosition(paxis, axis, position, 200.0); 
	paxis->SetPosition(paxis, axis, position, 200.0); 
	paxis->EnableAxis(paxis, axis); 
	}	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "CheckAxis"
SElExceptionPtr CheckAxis( SaxisPtr paxis, int axis, STesterPtr ptester, double position, const char *PEND_AXIS, double timeout)
{
	SElExceptionPtr pexception = NULL;
	double Initial_Position;
	double Currentposition;
	int   status;
	clock_t _start_test_clock;
	double  time;
	int     statusAlarm;
   	char	Error[256] = "";

	
	paxis->CurrentPosition(paxis, axis, &Currentposition);
	
	_start_test_clock =  clock();

    TIMEOUT_AXIS_INIT();
	
	do{
	   	paxis->CurrentPosition(paxis, axis, &Currentposition);

		time = (double)(clock()-_start_test_clock);

		if (Currentposition < position + 0.2 && Currentposition > position - 0.2)
			break;
	
	}while(time < timeout);
	
	if (time>=timeout)
	{
	paxis->PresentAlarm(paxis, axis, &statusAlarm);
	paxis->CurrentPosition(paxis, axis, &Currentposition);
	if (axis == AXIS_X)
	sprintf(Error, "AXE X : Alarm Code:(0X%04X)h\r\n Current Position = %.2fmm\r\n Target = %.2fmm\r\n", statusAlarm, Currentposition, position);
	else if (axis == AXIS_Y)
	sprintf(Error, "AXE Y : Alarm Code:(0X%04X)h\r\n Current Position = %.2fmm\r\n Target = %.2fmm\r\n", statusAlarm, Currentposition, position);
	else if (axis == AXIS_Z)
	sprintf(Error, "AXE Z : Alarm Code:(0X%04X)h\r\n Current Position = %.2fmm\r\n Target = %.2fmm\r\n", statusAlarm, Currentposition, position);
	
	EXCTHROW(-1, Error);
	}
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "MoveToPush"
SElExceptionPtr MoveToPush(SaxisPtr paxis, STesterPtr ptester, int check, 
						   double position_axis_Z, double position_axis_Y, double position_axis_X,
						    const char *PEND_AXIS_Z, const char *PEND_AXIS_Y, const char *PEND_AXIS_X)
{
	SElExceptionPtr pexception = NULL;

	if (check == 1)
	{
	EXCCHECK(CheckAxis (paxis, AXIS_Z, ptester, position_axis_Z, PEND_AXIS_Z, 5000));
	EXCCHECK(CheckAxis (paxis, AXIS_Y, ptester, position_axis_Y, PEND_AXIS_Y, 5000));
	EXCCHECK(CheckAxis (paxis, AXIS_X, ptester, position_axis_X, PEND_AXIS_X, 5000));
	}
	else
	{
	EXCCHECK(MoveAxis (paxis, AXIS_Z, position_axis_Z));
	EXCCHECK(MoveAxis (paxis, AXIS_Y, position_axis_Y));
	EXCCHECK(MoveAxis (paxis, AXIS_X, position_axis_X));
	}

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalog"
SElExceptionPtr ReadAnalog(char *Channel, double *Value)
{
SElExceptionPtr pexception = NULL;
int channel_index;

		sscanf(Channel, "ai%d", &channel_index);
		*Value = data_Analog[channel_index*RATE +1]; 
		
Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalogWithScale"
SElExceptionPtr ReadAnalogWithScale(char *Channel, SCalibrationPtr pScaleInfo, double *Value)
{
SElExceptionPtr pexception = NULL;
int channel_index;
double pChannelData[2];
double*	pdata;

		sscanf(Channel, "ai%d", &channel_index);
	
		pdata = data_Analog + channel_index*RATE +1;

		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			
			//double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			//double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k);
			
			LinEv1D( pdata, 1, a, b,  pChannelData);
			*Value = pChannelData[0]; 
		}
		else 
			*Value = data_Analog[channel_index*RATE +1]; 
Error:
	EXCRETHROW( pexception);
	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LineaTable1D"
SElExceptionPtr LineaTable1D(double *InputTable, SCalibrationPtr pScaleInfo, int size, double *OutputTable)
{
SElExceptionPtr pexception = NULL;


		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			double a = (plincal->pPoints[1].realValue - plincal->pPoints[0].realValue) / (plincal->pPoints[1].measValue - plincal->pPoints[0].measValue);
			double b = a * plincal->pPoints[1].measValue - plincal->pPoints[1].realValue;
			
			//double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			//double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k);
			
			LinEv1D( InputTable, size, a, b,  OutputTable);
		}

Error:
	EXCRETHROW( pexception);
	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Aquisition_Force"
DWORD WINAPI Thread_Aquisition_Force(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	SAquisitionThreadDataPtr	pthread_data = (SAquisitionThreadDataPtr)param;
	ViSession					vi_DAQ = pthread_data->vi_DAQ;
	SaxisPtr					paxis = pthread_data->paxis;
	SCANListPtr					pcan = pthread_data->pcan;
	uint32_t					timeout = pthread_data->timeout;
	char* 						push_name = pthread_data->push_name;
	uint32_t					station = pthread_data->station;
	uint32_t					index = pthread_data->index;
	double						position_forward = pthread_data->position_forward;
	double						position_backward = pthread_data->position_backward;
	char    					Channel[32];
	int                         channel_index;
	clock_t 					_start_test_clock;
	clock_t 					_start_test_clock_2;
	double 						_start_debug_clock;
	double   					time, time2, time_debug;
	SCalibrationPtr				pScaleInfoForce = NULL;
	void* 						pScaleInfo = NULL;;
	int 						sampsPerChan = BUFFERLEN;
	int 						i;
	double 						Currentposition, Touchposition, Initial_Position;
	double 						Start_push_position;
	double 						rvalueForce;
	double 						Offset;
	double 						Tab_Force[BUFFERLEN];
	int    						j;
	int 						First_Read = FALSE;
	double                      Value;
	int k;
	int statusAlarm;
	
	/* LIN variable */
	uint8_t	data[8];
	int32_t dataSize;
	int32_t rid;


	pthread_data->read = 0;

	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	if (index == SIDE_RIGHT)
	{
	Status_Push[station][PHONE]  		= STATUS_ACTIVE; 	
	Status_Push[station][LIST]   		= STATUS_ACTIVE;	
	Status_Push[station][SPEECH] 		= STATUS_ACTIVE;
	Status_Push[station][MENU_RIGHT]  = STATUS_ACTIVE;
	Status_Push[station][SCROLL_WHEEL]= STATUS_ACTIVE;	
	Status_Push[station][MENU_LEFT]  	= STATUS_ACTIVE;
	Status_Push[station][VOLUME_PLUS] = STATUS_ACTIVE;	
	Status_Push[station][VOLUME_MINUS]= STATUS_ACTIVE;
	}
	if (index == SIDE_LEFT)
	{
	Status_Push[station][MODE] = STATUS_ACTIVE; 	
	Status_Push[station][ASS] = STATUS_ACTIVE;	
	Status_Push[station][SET]   = STATUS_ACTIVE;
	Status_Push[station][MINUS] = STATUS_ACTIVE;
	Status_Push[station][PLUS]  = STATUS_ACTIVE;	
	Status_Push[station][LIM]= STATUS_ACTIVE;
	Status_Push[station][RES_CANC]= STATUS_ACTIVE;
	Status_Push[station][ACC] = STATUS_ACTIVE;
	Status_Push[station][DCC] = STATUS_ACTIVE;
	Status_Push[station][RESUME] = STATUS_ACTIVE;	
	}

	sprintf (Channel, "%s\0", pthread_data->Channel);
	sscanf(Channel, "ai%d", &channel_index);
	checkErr( eldaq_GetChannelScale ( vi_DAQ, Channel, "force", (void *)&pScaleInfoForce));

	
	/*-------------------------Forward direction -------------------*/
	
	/* detect start forward point */
	paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition);
	Initial_Position = Currentposition;
	Touchposition =  Currentposition + 0.5;
	
	/* move axis to Touchposition */
	MoveAxis2 (paxis, AXIS_Z, Touchposition, 10.0);
	paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition);
    
	_start_test_clock =  clock();
	do{
		
	  if (Currentposition - Initial_Position < 0.05)
			paxis->SetPosition(paxis, AXIS_Z, Touchposition, 10.0); 

	  if (Currentposition < Touchposition + 0.2 && Currentposition > Touchposition - 0.2)
			break;
	  else 
			paxis->CurrentPosition(paxis, AXIS_Z, &Currentposition); 

     time = (double)(clock()-_start_test_clock);
	 
    }while(time < timeout && (data_Analog[channel_index*RATE]*2) < 0.8);
	
	paxis->StopAxis(paxis, AXIS_Z); 
	
	//Sleep (20);

	paxis->AlarmReset(paxis, AXIS_Z);
	
	
	//Sleep (30);
	MoveAxis2 (paxis, AXIS_Z, position_forward, 2.0);

	/* read start position */
	_start_test_clock =  clock();
		do{
			/* Stroke */
		    time = (double)(clock()-_start_test_clock);
	
			Sleep(1);
		
	}while ((data_Analog[channel_index*RATE]*2) < 0.8 && time < timeout);
	
		
		_start_test_clock =  clock();
		First_Read = FALSE;
	
		k=0;
		do{
			
		for (i=k*RATE; i<RATE*k+RATE; i++)
			{
	    	/* Force */
			Tab_Force[i] = data_Analog[(channel_index*RATE) + (i-(k*RATE))];
		    
			/* Contact 1 */
			pthread_data->data[MESS_CONTACT][i] = Get_push_value_from_name(data_pushs[station], push_name);
			
			/* Stroke */
		    time = (double)(clock()-_start_test_clock);
			pthread_data->data[MESS_WEG][i] = (2.0 * time) / 1000;
		    
			}
	
		Sleep(1);
		
		k++;
		
	}while ((data_Analog[channel_index*RATE]*2) < 7.5 && time < timeout && i < BUFFERLEN-RATE);
		
	paxis->PresentAlarm(paxis, AXIS_Z, &statusAlarm);
	paxis->StopAxis(paxis, AXIS_Z); 
	paxis->StopAxis(paxis, AXIS_Z); 
	if(statusAlarm)
		
	{
	 LogAxisError(statusAlarm, data_Analog[channel_index*RATE]*2,channel_index);
	 paxis->AlarmReset(paxis, AXIS_Z); 
	}

	paxis->StopAxis(paxis, AXIS_Z); 
	paxis->StopAxis(paxis, AXIS_Z); 
	
	_start_test_clock_2 =  clock();
	do{
			
		for (i=k*RATE; i<RATE*k+RATE; i++)
			{
	    	/* Force */
			Tab_Force[i] = data_Analog[(channel_index*RATE) + (i-(k*RATE))];
		    
			/* Stroke */
		    time = (double)(clock()-_start_test_clock);
			time2 = (double)(clock()-_start_test_clock_2);
			pthread_data->data[MESS_WEG][i] = (2.0 * time) / 1000;
		    
			/* Contact 1 */
			pthread_data->data[MESS_CONTACT][i] = Get_push_value_from_name(data_pushs[station], push_name);
			}
	
		Sleep(20);
		k++;
		
	}while (time2 < 3000 && i < BUFFERLEN && pthread_data->data[MESS_CONTACT][i-1]==0);
	
	/* Linear Scale */
	LineaTable1D(Tab_Force, pScaleInfoForce, i, pthread_data->data[MESS_KRAFT]);
	
/*-------------------------LIN Update  -------------------*/
	
	//Sleep(150);
	
	/* DIGITAL SWITCH RAW  */ 
	if (index == SIDE_RIGHT)
	{
	Status_Push[station][PHONE]  		= Get_push_value_from_name(data_pushs[station], "PHONE");	        	
	Status_Push[station][LIST]   		= Get_push_value_from_name(data_pushs[station], "LIST");	       	
	Status_Push[station][SPEECH] 		= Get_push_value_from_name(data_pushs[station], "SPEECH");       
	Status_Push[station][MENU_RIGHT]  	= Get_push_value_from_name(data_pushs[station], "MENU_RIGHT");   
	Status_Push[station][SCROLL_WHEEL]	= Get_push_value_from_name(data_pushs[station], "SCROLL_WHEEL");	
	Status_Push[station][MENU_LEFT]  	= Get_push_value_from_name(data_pushs[station], "MENU_LEFT");    
	Status_Push[station][VOLUME_PLUS] 	= Get_push_value_from_name(data_pushs[station], "VOLUME_PLUS");	
	Status_Push[station][VOLUME_MINUS]	= Get_push_value_from_name(data_pushs[station], "VOLUME_MINUS");
	}
	
	if (index == SIDE_LEFT)
	{
	Status_Push[station][MODE] = Get_push_value_from_name(data_pushs[station], "MODE");
	Status_Push[station][ASS] = Get_push_value_from_name(data_pushs[station], "ASS");	
	Status_Push[station][SET]   = Get_push_value_from_name(data_pushs[station], "SET");
	Status_Push[station][MINUS] = Get_push_value_from_name(data_pushs[station], "MINUS"); 
	Status_Push[station][PLUS]  = Get_push_value_from_name(data_pushs[station], "PLUS");
	Status_Push[station][LIM]= Get_push_value_from_name(data_pushs[station], "LIM");
	Status_Push[station][RES_CANC]= Get_push_value_from_name(data_pushs[station], "RES_CANC");
	Status_Push[station][ACC]= Get_push_value_from_name(data_pushs[station], "DCC");
	Status_Push[station][DCC]= Get_push_value_from_name(data_pushs[station], "DCC");
	Status_Push[station][RESUME]= Get_push_value_from_name(data_pushs[station], "RESUME");
	}							
	
#if 0    /* formerly excluded lines */
	/* ADC SWITCH RAW  */ 
	if (index == SIDE_LEFT)
	{
	/* read value from LIN */
	Value_ADC_Push[station][L_BOTTOM_LEFT] = Get_ADC_value(ADC_pushs[station], "L_BOTTOM_LEFT");
	Value_ADC_Push[station][L_BOTTOM_MID] = Get_ADC_value(ADC_pushs[station], "L_BOTTOM_MID");	        	
	Value_ADC_Push[station][L_BOTTOM_RIGHT] = Get_ADC_value(ADC_pushs[station], "L_BOTTOM_RIGHT");
	
	Value_ADC_Push[station][L_MID_LEFT] = Get_ADC_value(ADC_pushs[station], "L_MID_LEFT");
	Value_ADC_Push[station][L_MID_RIGHT] = Get_ADC_value(ADC_pushs[station], "L_MID_RIGHT");
	
	Value_ADC_Push[station][L_TOP_LEFT] = Get_ADC_value(ADC_pushs[station], "L_TOP_LEFT");
	Value_ADC_Push[station][L_TOP_RIGHT] = Get_ADC_value(ADC_pushs[station], "L_TOP_RIGHT");

	iFlag_Write_Valeo_production_Key[station] = FALSE;
	iFlag_Read_Switch_Raw_Values[station] = FALSE;
	}													  
#endif   /* formerly excluded lines */
	
	pthread_data->read = i;
	
	/* mouvement in direction + high speed */
	MoveAxis2 (paxis, AXIS_Z, position_backward, 100.0);


Error:
	free(pScaleInfoForce);
	pScaleInfoForce = NULL;
	if(pexception)
	{
		paxis->StopAxis(paxis, AXIS_Z); 
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}

#undef __FUNC__
#define __FUNC__ "Thread_Aquisition_Torque_TIPP_WIPE"
DWORD WINAPI Thread_Aquisition_Torque_TIPP_WIPE(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	STorqueThreadDataPtr		pthread_data = (STorqueThreadDataPtr)param;
	ViSession					vi_DAQ = pthread_data->vi_DAQ;
	SaxisPtr					paxis = pthread_data->paxis;
	SCANListPtr					pcan = pthread_data->pcan;
	uint32_t					timeout = pthread_data->timeout;
	uint32_t					station = pthread_data->station;
	double						angle = pthread_data->angle;
	double						speed = pthread_data->speed;
	double						Torque_Max = pthread_data->Torque_Max;
	double						Reduction_Torque = pthread_data->Reduction_Torque;
	double						Reduction_Angle = pthread_data->Reduction_Angle;
	clock_t 					_start_test_clock;
	clock_t 					_start_debug_clock;
	double   					time, time_debug;
	SCalibrationPtr				pScaleInfoTorque = NULL;
	int 						sampsPerChan = BUFFERLEN2;
	int 						i;
	double 						Currentangle;
	double 						Start_angle;
	double 						rvalueTorque;
	double 						Offset_Torque;
	double                      value_Position=8.91;
	double                      smac_Position=8.91;
	double                      value_Force=0.0; 
	double                      position;
	double                      Last_value_Position;
	double                      Total_Value;

	int    Block = 300;
	double CumulTorque;
	double last_angle;
	double step;
	static double Tab_Torque[BUFFERLEN2];
	static double Tab_Torque2[BUFFERLEN2];
	static double Tab_Position[BUFFERLEN2]; 
	static double Tab_Position2[BUFFERLEN2]; 
	int    j;
	
	int 						First_Request_ADC_DOWN_P1 = FALSE;
	int 						First_Request_ADC_DOWN_P10 = FALSE;
	int 						First_Read_ADC_DOWN_P1 = FALSE;
	int 						First_Read_ADC_DOWN_P10 = FALSE;

	int 						First_Request_ADC_UP_P1 = FALSE;
	int 						First_Request_ADC_UP_P10 = FALSE;
	int 						First_Read_ADC_UP_P1 = FALSE;
	int 						First_Read_ADC_UP_P10 = FALSE;

	clock_t 					_start_read_clock;
	double time_ADC;
	
	double                      Position_Speed=0;
		
	/* LIN variable */
	uint8_t	data[8];
	int32_t dataSize;
	int32_t rid;
	

	CmtSetCurrentThreadPriority (THREAD_PRIORITY_ABOVE_NORMAL);
	
	Status_Push[station][TIPP_WIPE]	= STATUS_IDLE; 	
	
	
	/*-------------------------------------------------------------------------------------------*/	
	
	char    					Channel[32]="ai5";
	int                         channel_index=5;
	
	
	sprintf (Channel, "%s\0", "ai5");
	sscanf(Channel, "ai%d", &channel_index);

	
/*-------------------------------------------------------------------------------------------*/	 

	/* ---------------------  TIPP WIPE Direction DOWN --------------------- */

	pthread_data->read = 0;
			   
	paxis->SetPosition(paxis, 32, 23.0, 2.0);  //22
	Sleep(5);
	
	_start_test_clock =  clock();
	i=0;
	Total_Value	= 0;
//	Offset_Torque=data_Analog[(channel_index)*20]; 
	
	do{
		
		paxis->CurrentPosition(paxis, 32, &value_Position);
	//	paxis->CurrentAnalog(paxis, 32, &Offset_Torque,1, 0); 
		if (value_Position!=0)
		   {
			   smac_Position=value_Position;
		   }
		time = (double)(clock()-_start_test_clock);   
		
		Sleep(1);
		Offset_Torque=data_Analog[(channel_index)*20];
		Total_Value=Total_Value+Offset_Torque;
		i++ ;
	//	printf ("Offset_Torque_DOWN = %f\r\n    ## ", Offset_Torque); 
	//	printf ("SMAC_Position_DOWN = %f\r\n    ## ", smac_Position); 
	 
	}while(smac_Position < 13.0 && time<  timeout);
//	
	Offset_Torque=Total_Value/i;
	
//	printf ("Offset_Torque = %f\r\n    ## ", Offset_Torque);
	
	_start_test_clock =  clock();
	do{
		
		paxis->CurrentPosition(paxis, 32, &value_Position);
	//	paxis->CurrentAnalog(paxis, 32, &Offset_Torque,1, 0); 
		if (value_Position!=0)
		   {
			   smac_Position=value_Position;
		   }
		time = (double)(clock()-_start_test_clock);   
		
		Sleep(1);
		value_Force=data_Analog[(channel_index)*20];
	 
	}while(fabs(value_Force)-fabs(Offset_Torque) < 0.5 && time<  timeout);
//	Offset_Torque=0;
	
	/* ---------------------  TIPP WIPE Direction DOWN BOUCLE D'ACQUISITION--------------------- */ 
	_start_test_clock =  clock(); 
	i=0;
	do{
		/* read Torque */
	//	paxis->CurrentAnalog(paxis, 32, &value_Force, 1, 0);
	//	Tab_Torque[i] = fabs(value_Force - Offset_Torque);  
	
		
		/* read contact LIN */
		pthread_data->data[MESS_LIN][i] = Get_push_value_from_name(data_pushs[station], "TIPP_WIPE");
		
		
		/* read angle */
		paxis->CurrentPosition(paxis, 32, &value_Position);
	//	Tab_Position[i] = (smac_Position-12.21) * Reduction_Angle;   // 14.43

		if (value_Position!=0)
		   {smac_Position=value_Position;
		   }
		
		
		 Tab_Torque[i] = fabs(data_Analog[(channel_index)*20])-fabs(Offset_Torque);  

		
	//	printf ("SIDE_LEFT_DOWN : ABSULUTE_FORCE = %f    ## ", value_Force);
	//	printf ("FINAL_FORCE = %f\r\n    ## ", Tab_Torque[i]); 
	//	printf ("Angle = %f\r\n", Tab_Position[i]); 
	//	printf ("SIDE LEFT DOWN Angle = %f\r\n ", Tab_Position[i]);
	//	printf ("Value_Position = %f\r\n", value_Position);   
		 

		i++;
		//printf ("INCREMENT_TIPP_DOWN = %d\r\n", i); 

		 time = (double)(clock()-_start_test_clock);
		 
		  Position_Speed=(time/1000)*2.0;
		  	Tab_Position[i] = Position_Speed * Reduction_Angle;   // 14.43 
	
		 Sleep(1);
		
	}while (//fabs(value_Force) < fabs(Torque_Max) &&
		    fabs(data_Analog[(channel_index)*20]) < fabs(Torque_Max) && 
		 	time < timeout && 
			i < BUFFERLEN2 
		//	&& smac_Position<20.0 //&& smac_Position> 9.25			// 15.2  //21
			);
	
	//printf ("INCREMENT_TIPP_DOWN = %d\r\n", i); 

	if (time >= timeout)	
	{		
		printf ("ERROR TIPP WHEEL DOWN : value_Force = %f\r\n ", value_Force);
		printf ("ERROR TIPP WHEEL DOWN : time = %f\r\n ", time);
		printf ("ERROR TIPP WHEEL DOWN : i = %d\r\n ", i);
		printf ("ERROR TIPP WHEEL DOWN : smac_Position = %f\r\n ", smac_Position);
		EXCTHROW(-1, "Timeout axis");
	}

	pthread_data->read = i;
		
	
		/* ---------------------  TIPP WIPE Direction 0 --------------------- */ 
	
	paxis->SetPosition(paxis, 32,12.4,12.5);
	_start_test_clock =  clock();
	_start_read_clock =  clock();
	 
do{
		
		paxis->CurrentPosition(paxis, 32, &value_Position);
		if (value_Position!=0)
		   {smac_Position=value_Position;
		   }
		
		 Sleep(1);  
		
		time = (double)(clock()-_start_test_clock);  
	 
	}while(smac_Position > 12.7 && time< timeout);// 7.5  //14.7

	paxis->SetPosition(paxis, 32, 3.5, 2.0);   //12
	_start_test_clock =  clock();
	_start_read_clock =  clock();
	i=0;
	Total_Value	= 0;
	 
do{
		
		paxis->CurrentPosition(paxis, 32, &value_Position);
		Offset_Torque=data_Analog[(channel_index)*20];
		if (value_Position!=0)
		   {smac_Position=value_Position;
		   }
		
		 Sleep(1); 
		 i++;
	//	Total_Value=Total_Value+Offset_Torque; 
		time = (double)(clock()-_start_test_clock);
	//	printf ("Offset_Torque_UP = %f\r\n    ## ", Offset_Torque); 
	//	printf ("SMAC_Position_UP = %f\r\n    ## ", smac_Position); 
	 
	}while(smac_Position > 11.8  && time< timeout);// 7.5  //14.3


	if (time >= timeout)	
	{
		printf ("ERROR TIPP WHEEL 0 DOWN : smac_Position = %f\r\n ", smac_Position);
		printf ("ERROR TIPP WHEEL 0 DOWN : time = %f\r\n ", time);
		EXCTHROW(-1, "Timeout Motor");
	}
	
	_start_test_clock =  clock(); 
	do{
		
		paxis->CurrentPosition(paxis, 32, &value_Position);
	//	paxis->CurrentAnalog(paxis, 32, &Offset_Torque,1, 0); 
		if (value_Position!=0)
		   {
			   smac_Position=value_Position;
		   }
		time = (double)(clock()-_start_test_clock);   
		
		Sleep(1);
		value_Force=data_Analog[(channel_index)*20];
	 
	}while(fabs(value_Force)-fabs(Offset_Torque) < 0.5 && time<  timeout);
	
	/* ---------------------  TIPP WIPE Direction UP --------------------- */

	pthread_data->read2 = 0;
	_start_test_clock =  clock();
	_start_read_clock =  clock();
	
	i=0;
	
//	Offset_Torque=0; 
	
	do{
		
	//	paxis->CurrentAnalog(paxis, 32, &value_Force,1,0);
	//	Tab_Torque2[i] = fabs(value_Force - Offset_Torque); 
		
		 Tab_Torque2[i] = fabs(data_Analog[(channel_index)*20])-fabs(Offset_Torque);  
	
		/* read contact LIN */
		pthread_data->data2[MESS_LIN][i] = Get_push_value_from_name(data_pushs[station], "TIPP_WIPE");
		
		/* read angle */
		paxis->CurrentPosition(paxis, 32, &value_Position);
		if (value_Position!=0)
		   {smac_Position=value_Position;
		   }
	    time = (double)(clock()-_start_test_clock);
		Position_Speed=(time/1000)*2.0;
		  	Tab_Position2[i] = Position_Speed * Reduction_Angle;   // 14.43 
		
		
		
	//	Tab_Position2[i] = (11.8-smac_Position) * Reduction_Angle;	//11.8	  
		
		pthread_data->data2[MESS_ANGLE][i] = Tab_Position2[i];
		
		pthread_data->data2[MESS_TORQUE][i] = Tab_Torque2[i];
		
		
	//	printf ("SIDE_LEFT_UP : data2[MESS_ANGLE] = %f\r\n", pthread_data->data2[MESS_ANGLE][i]); 
	//	printf ("SIDE_LEFT_U : data2[MESS_TORQUE] = %f\r\n", pthread_data->data2[MESS_TORQUE][i]);
		
	//	printf ("SIDE_LEFT_UP : time = %f\r\n", time);
	//	printf ("SIDE_LEFT_UP : Tab_Torque2 = %f\r\n", Tab_Torque2[i]);
	//	printf ("SIDE_LEFT_UP : Value_Position = %f\r\n", value_Position);
	//	printf ("SIDE_LEFT_UP : Value_Force = %f\r\n", fabs(value_Force));
		/* read Torque */

		Sleep(1);
		
		i++;
	//	printf ("Tab_Torque2 = %f\r\n    ## ", Tab_Torque2[i]); 
	//	printf ("SMAC_Position_UP_FINAL = %f\r\n    ## ", smac_Position); 
		
		
	}while (  time < timeout &&
			  i < BUFFERLEN2 &&
			 // && fabs(value_Force) < fabs(Torque_Max) &&
			  fabs(data_Analog[(channel_index)*20]) < fabs(Torque_Max) 
			//  && smac_Position>5.3 //&& smac_Position<8.5					 // 2.3
			  );
	
//	printf ("INCREMENT_TIPP_UP = %d\r\n", i); 
	
	
	if (time >= timeout)	
	{
		printf ("ERROR TIPP WHEEL DOWN : value_Force = %f\r\n ", value_Force);
		printf ("ERROR TIPP WHEEL DOWN : time = %f\r\n ", time);
		printf ("ERROR TIPP WHEEL DOWN : i = %d\r\n ", i);
		printf ("ERROR TIPP WHEEL DOWN : smac_Position = %f\r\n ", smac_Position);
		EXCTHROW(-1, "Timeout Motor");
	}
	_start_test_clock =  clock();
	_start_read_clock =  clock();
	
	paxis->SetPosition(paxis, 32, 12.21 ,12.0); // 14.43
	
	
	do{
	   	paxis->CurrentPosition(paxis, 32, &value_Position); 

		time = (double)(clock()-_start_test_clock);
	//	printf ("ERROR TIPP WHEEL 0 UP : value_Position = %f\r\n ", value_Position); 

		if (value_Position < 12.21 + 0.8 && value_Position > 12.21 - 0.8)  // 8.54
			break;
		Sleep(5);
	
	}while(time < timeout);
	
	if (time >= timeout)	
	{
		printf ("ERROR TIPP WHEEL 0 UP : value_Position = %f\r\n ", value_Position);
		printf ("ERROR TIPP WHEEL 0 UP : time = %f\r\n ", time);
		EXCTHROW(-1, "Timeout Motor");
	}
		
	for (j=0;j<pthread_data->read; j++)
	{	pthread_data->data[MESS_TORQUE][j] = Tab_Torque[j];
	   // printf ("SIDE_LEFT_DOWN : data[MESS_TORQUE] = %f\r\n", pthread_data->data[MESS_TORQUE][j]);  
	}
	
	for (j=0;j<pthread_data->read; j++)
	{	pthread_data->data[MESS_ANGLE][j] = Tab_Position[j];   
	  // printf ("SIDE_LEFT_DOWN : data[MESS_ANGLE] = %f\r\n", pthread_data->data[MESS_ANGLE][j]); 
	}
	
	pthread_data->read2 = i;	
Error:
	paxis->SetPosition(paxis, 32,12.21,12.0);
	free(pScaleInfoTorque);
	pScaleInfoTorque = NULL;
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Aquisition_Torque_SCROLL_WHEEL"
DWORD WINAPI Thread_Aquisition_Torque_SCROLL_WHEEL(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	STorqueThreadDataPtr		pthread_data = (STorqueThreadDataPtr)param;
	ViSession					vi_DAQ = pthread_data->vi_DAQ;
	SaxisPtr					paxis = pthread_data->paxis;
	SCANListPtr					pcan = pthread_data->pcan;
	uint32_t					timeout = pthread_data->timeout;
	uint32_t					station = pthread_data->station;
//	double						target_position = pthread_data->target_position;
//	double						target_speed = pthread_data->target_speed;
	double						Torque_Max = pthread_data->Torque_Max;
	double						Reduction_Torque = pthread_data->Reduction_Torque;
	double						Reduction_Angle = pthread_data->Reduction_Angle;
	clock_t 					_start_test_clock;
	clock_t 					_start_debug_clock;
	double   					time, time_debug,old_time=0;
	SCalibrationPtr				pScaleInfoTorque = NULL;
	int 						sampsPerChan = BUFFERLEN2;
	int 						i=0;
	double 						Currentangle;
	double 						Start_angle;
	double 						rvalueTorque;
	double 						Offset_Torque=0;
	double                      Offset_Position;
	double                      value_Position;
	double                      value_Force, Total_Value, Position_Speed; 

	int    Block = 100;
	double CumulTorque;
	double last_angle;
	double step;
	static double Tab_Torque[BUFFERLEN2];
	static double Tab_Torque_2[BUFFERLEN2];
	int    j=0,n=0;
	int Pulse_Down = TRUE;
	
	/* LIN variable */
	uint8_t	data[8];
	int32_t dataSize;
	int32_t rid;

	char sTimeDate[128];

	int overload = 0;
	double test_speed = 11.0; //old 9.0 //ARAIBIA 17.06.2021
		
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	pthread_data->read = 0;
	
	
	char    					Channel[32]="ai0";
	int                         channel_index=0;
	
	
	sprintf (Channel, "%s\0", "ai0");
	sscanf(Channel, "ai%d", &channel_index);

	
/*------------------------------ Boucle d'acquisition sens 1 UP--------------------------------------------*/	

	 //Offset_Torque = data_Analog[(channel_index)*20];
 	 //printf ("UP : Offset_Torque = %f \r\n", Offset_Torque);
	 Offset_Torque = 0.4; 
	
	 paxis->SetPosition(paxis, 1, 0.5, 13.5);   // old position 0.5
		
	_start_test_clock =  clock(); 
	i=0;
	do
	{	 	
		paxis->CurrentPosition(paxis, 1, &value_Position);
		Sleep(1);
		i++;
		
	    time = (double)(clock()-_start_test_clock);
		
	}  while (//fabs(value_Force) < fabs(Torque_Max) && 
		 	time < timeout && 
			i < BUFFERLEN2 && 
			value_Position > 50.0);   // 59

		

	 paxis->SetPosition(paxis, 1, 0.5, test_speed);   // old position 0.5 	
	
	_start_test_clock =  clock();
	i=0;
	do{
		time = (double)(clock()-_start_test_clock); 
		
		Position_Speed=(time/1000) * test_speed; 
		/* Read Force with DAQmx */
		Tab_Torque[i] = data_Analog[(channel_index)*20] - Offset_Torque; //-(Offset_Torque-0.0275*Position_Speed);

		/* read contact LIN */
		pthread_data->data[MESS_LIN][i] = Get_push_value_from_name(data_pushs[station], "SCROLL");

		/* read angle with speed*time */
		
		pthread_data->data[MESS_ANGLE][i] = Position_Speed * Reduction_Angle;

		/* Read Smac Position */
		paxis->CurrentPosition(paxis, 1, &value_Position);
		
		/* Sleep*/
		i++;
		Sleep(1);

	}while (time < timeout && i < BUFFERLEN2 && Position_Speed < 50.0  /*&& value_Position > 5.0*/);		   // old=49
	
	
	pthread_data->read = i;
	pthread_data->NumPulse=n;	
	for (j=0;j<pthread_data->read; j++)
	{	pthread_data->data[MESS_TORQUE][j] = Tab_Torque[j]; //frottement + force radial
	}

    
	
/*------------------------------ Boucle d'acquisition sens DOWN--------------------------------------------*/	

	//Sleep(1000); //pour arret axe SMAC
	pthread_data->read2 = 0;
	
	//Offset_Torque = data_Analog[(channel_index)*20]; 
	//printf ("DOWN : Offset_Torque = %f \r\n", Offset_Torque);
	Offset_Torque = 0.4;  //old 0.4

	paxis->SetPosition(paxis, 1, 52.0, 13.5); 	 // old 59.5
	
	_start_test_clock =  clock(); 
	 i=0;
	do
	{	 	
		paxis->CurrentPosition(paxis, 1, &value_Position);
		Sleep(1);
		i++;
		
	    time = (double)(clock()-_start_test_clock);
	
		}  while (//fabs(value_Force) < fabs(Torque_Max) && 
		 	time < timeout && 
			i < BUFFERLEN2 && 
			value_Position < 2.0);   // 5
		

	paxis->SetPosition(paxis, 1, 52.0, test_speed); 	  // old 59.5
	paxis->CurrentPosition(paxis, 1, &Offset_Position);

	_start_test_clock =  clock();
	i=0;
	Position_Speed=0;
	time=0;
	do{
		
		time = (double)(clock()-_start_test_clock); 
	
		Position_Speed = (time/1000) * test_speed; 
		
		/* Read Force with DAQmx */
		Tab_Torque_2[i] = data_Analog[(channel_index)*20] + Offset_Torque;
		//printf ("DOWN : Tab_Torque[%d] = %f \r\n", i, Tab_Torque[i]);
	
		/* read contact LIN */
		pthread_data->data2[MESS_LIN][i] = Get_push_value_from_name(data_pushs[station], "SCROLL");

		
		pthread_data->data2[MESS_ANGLE][i] = Position_Speed* Reduction_Angle; 
	
		/* Read Smac Position */
		paxis->CurrentPosition(paxis, 1, &value_Position);
		
		/* Sleep*/
		i++;
		Sleep(1);

	}while (time < timeout && i < BUFFERLEN2 && Position_Speed < 50.0 /* && value_Position < 53.0 */);  // old =49
 
	
	_start_test_clock =  clock(); 
	
	paxis->SetPosition(paxis, 1, 52.0, 45.45);  // old 59.5

	if (time >= timeout)	
	{		
		EXCTHROW(-1, "Timeout axis");
	}

	
	pthread_data->read2 = i;
	pthread_data->NumPulse=n;
	
	for (j=0;j<pthread_data->read2; j++)
	{	pthread_data->data2[MESS_TORQUE][j] = Tab_Torque_2[j]; //frottement + force radial
	}
	
	//printf ("FIN ------------- \r\n");

Error:
	free(pScaleInfoTorque);
	pScaleInfoTorque = NULL;
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}

#undef __FUNC__
#define __FUNC__ "Thread_Lin_Read"
DWORD WINAPI Thread_Lin_Read(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	SLinThreadDataPtr			pthread_data = (SLinThreadDataPtr)param;
	SCANListPtr					pcan = pthread_data->pcan;
	uint32_t					station = pthread_data->station;
  	uint32_t					timeout = pthread_data->timeout;
   	uint32_t					i,j;
    char CharBuff[256];
	double DataSize=0x00;
 	int DiagResp=0;
	char length[3]="";

		
	/* timeout variable */
	clock_t 					_start_test_clock;
	clock_t 					_start_debug_clock;
	double   					time, time_debug;
 	unsigned char Go_To_Sleep[8]={0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; 
	unsigned char Frame[8]; 
	/* LIN variable */
	uint8_t	cmd[] = "";
	int32_t dataSize;
	int32_t rid;
    uint8_t	data[8][8];
	uint8_t find_frame  = 0;
	
	
	/* initialisation */
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);

	TIMEOUT_LIN_INIT();
	Last_Alive_Byte[station] = 0X00;
    first_LIN_Frame[station] = TRUE;
    iFlag_Run_Normal_Mode[station] = 0;
    iFlag_Delete_Cache [station] = TRUE;
	Number_DTC[station] = 0XFF; 
	iFlag_Thread_Lin_Read[station]  = TRUE;
	//iFlag_Store_All_data_Pushs[station] == FALSE
	//k=0;	
	for (i=0; i<8; i++)
	data_pushs[station][i]  = 0XFF;
	
	while (iFlag_Thread_Lin_Read[station])
	  {
		if (iFlag_Delete_Cache [station] == TRUE)
		{
			/* Header of Log */
			memset (CharBuff , 0X00, 256);
			memset (Log_Lin[station] , 0X00, 1024*1000);

			sprintf (Log_Lin[station], "Type	");
			sprintf (Log_Lin[station], "Time	");
   			strcat (Log_Lin[station], "ID	");
   			strcat (Log_Lin[station], "dataSize		");
   			strcat (Log_Lin[station], "Data		");
   			strcat (Log_Lin[station], "\r\n");
			iFlag_Delete_Cache [station] = FALSE;
		}
	
		memset (data[station] , 0X00, 8);
		pexception = pcan->ReadLowLevel(pcan, pthread_data->lin_channel, &rid, &dataSize, data[station]);
				
			
		if (rid == 0X3D && dataSize == 8) //refresh the register of pushs ADC
		{
		//printf ("Read : rid = 0X3D\r\n");

		time = (clock() - _start_clock) / (double)CLOCKS_PER_SEC;
		strcat (Log_Lin[station], "RX	");
		sprintf (&CharBuff[0], "%f	", time);
		strcat (Log_Lin[station], CharBuff);
		sprintf (&CharBuff[0], "0X%2.2X	", rid);
		strcat (Log_Lin[station], CharBuff);
		sprintf (&CharBuff[0], "%1d		",dataSize);
		strcat (Log_Lin[station], CharBuff);
	
			
			/* check DID */
			if (data[station][4] == 0XFD && data[station][5] == 0X51)  // Switch_Raw_Values
			{
				iFlag_Write_Wake_Up[station] = FALSE;
				//printf ("Switch_Raw_Values Received -----OK\r\n");
				
				if (data[station][1] == 0x10) // check if we must send again another request to read more data
				{
					for (i=0; i<dataSize; i++)
					{
						if (i >= 6)// read data from the byte 3 (byte 1= 2E | byte 2= 10 : frame size >8 | byte 3= 09 : data size)
						{
						ADC_pushs[station][i-6] = data[station][i];
						//printf ("ADC_pushs[%d][%d]  = %02X\r\n", station, i-6, data[station][i]);
						}
						sprintf (&CharBuff[0], "%02X ", data[station][i]);  //2E 10 09 62 FD 51 [00 D1]
						strcat (Log_Lin[station], CharBuff);
					}
					strcat (Log_Lin[station], "\r\n");
					
					
					
					find_frame = 0;
					data[station][0] = 0x2E;
					do{
					EXCCHECK(pcan->WriteLowLevel(pcan,  pthread_data->lin_channel, 0x3D, 17, 1, data[station]));
					pexception = pcan->ReadLowLevel(pcan, pthread_data->lin_channel, &rid, &dataSize, data[station]);
					}while (find_frame<10 && rid != 0X3D);
					
					iFlag_Write_Wake_Up[station] = TRUE;

					time = (clock() - _start_clock) / (double)CLOCKS_PER_SEC;
					strcat (Log_Lin[station], "RX	");
					sprintf (&CharBuff[0], "%f	", time);
					strcat (Log_Lin[station], CharBuff);
					sprintf (&CharBuff[0], "0X%2.2X	", rid);
					strcat (Log_Lin[station], CharBuff);
					sprintf (&CharBuff[0], "%1d		",dataSize);
					strcat (Log_Lin[station], CharBuff);
					
					if (data[station][1] == 0x21) // rest of frame 
					{
						for (i=0; i<dataSize; i++)
						{
							if (i >= 2)// read data from the byte 3 (byte 1= 2E | byte 2= 10 : frame size >8 | byte 3= 09 : data size)
							{
							ADC_pushs[station][i]  = data[station][i];  //2E 21 [D1 D5 D1 D1] FF FF
							//printf ("ADC_pushs[%d][%d]  = %02X\r\n", station, i, data[station][i]);
							}
							sprintf (&CharBuff[0], "%02X ", data[station][i]);
							strcat (Log_Lin[station], CharBuff);
						}
					strcat (Log_Lin[station], "\r\n");
					ADC_Received[station] = TRUE; //all frame received
					strcat (Log_Lin[station], "---------------- Read : Switch_Raw_Values\r\n");
					}
				}
			}
			
			if (data[station][4] == 0XFD && data[station][5] == 0X34)  // Sw Build Date
			{
				iFlag_Write_Wake_Up[station] = FALSE;
				
				if (data[station][1] == 0x10) // check if we must send again another request to read more data
				{
					for (i=0; i<dataSize; i++)
					{
						if (i >= 6)// read data from the byte 3 (byte 1= 2E | byte 2= 10 : frame size >8 | byte 3= 09 : data size)
						{
						Sw_Build_Date[station][i-6] = data[station][i];
						}
						sprintf (&CharBuff[0], "%02X ", data[station][i]);  //2E 10 09 62 FD 34 [17 07]
						strcat (Log_Lin[station], CharBuff);
					}
					strcat (Log_Lin[station], "\r\n");
					
					
					find_frame = 0;
					data[station][0] = 0x2E;
					do{
					EXCCHECK(pcan->WriteLowLevel(pcan,  pthread_data->lin_channel, 0x3D, 17, 1, data[station]));
					pexception = pcan->ReadLowLevel(pcan, pthread_data->lin_channel, &rid, &dataSize, data[station]);
					}while (find_frame<10 && rid != 0X3D);
					
					iFlag_Write_Wake_Up[station] = TRUE;

					time = (clock() - _start_clock) / (double)CLOCKS_PER_SEC;
					strcat (Log_Lin[station], "RX	");
					sprintf (&CharBuff[0], "%f	", time);
					strcat (Log_Lin[station], CharBuff);
					sprintf (&CharBuff[0], "0X%2.2X	", rid);
					strcat (Log_Lin[station], CharBuff);
					sprintf (&CharBuff[0], "%1d		",dataSize);
					strcat (Log_Lin[station], CharBuff);
					
					if (data[station][1] == 0x21) // rest of frame 
					{
						for (i=0; i<dataSize; i++)
						{
							if (i >= 2)// read data from the byte 3 (byte 1= 2E | byte 2= 10 : frame size >8 | byte 3= 09 : data size)
							{
							Sw_Build_Date[station][i]  = data[station][i];  //2E 21 [14 15 11 45] FF FF
							}
							sprintf (&CharBuff[0], "%02X ", data[station][i]);
							strcat (Log_Lin[station], CharBuff);
						}
					strcat (Log_Lin[station], "\r\n");
					sprintf(Sw_Build_Date[station],"%02X%02X%02X%02X%02X%02X",
													Sw_Build_Date[station][0], 
													Sw_Build_Date[station][1], 
													Sw_Build_Date[station][2],
													Sw_Build_Date[station][3], 
													Sw_Build_Date[station][4], 
													Sw_Build_Date[station][5]); 
					strcat (Log_Lin[station], "---------------- Read : SW Build Date\r\n");
					}
				}
			}
			
			if (data[station][4] == 0X00 && data[station][5] == 0X00 && data[station][6] == 0X10)  // Read : Variant Code 2E 06 F2 66 00 00 10 [F4]
			{
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
					
				sprintf(Variant_Code_Read[station],"%02X",data[station][7]); 
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "--------- Read : Variant_code\r\n");
			}
			
			if (data[station][3] == 0XFD && data[station][4] == 0X33)  // Read : Valeo Sw Revision 2E 06 62 FD 33 [17 29 00]
			{
			//printf ("Read : Valeo_Sw_Revision\r\n");
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
					
				sprintf(Valeo_Sw_Revision[station],"%02X%02X%02X",data[station][5], 
																  data[station][6], 
																  data[station][7]); 
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "--------- Read : Valeo_Sw_Revision\r\n");
			}
			
			if (data[station][1] == 0X06 && data[station][2] == 0XF2)  // Read : BMW Part Number
			{
			//printf ("Read : BMW Part Number\r\n");
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
					
				sprintf(BMW_Part_Number_Read[station],"%02X%02X%02X%02X",data[station][6], 
																	 data[station][5], 
																	 data[station][4], 
																	 data[station][3]); 
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "--------- Read : BMW_Part_Number\r\n");
			}
			
			if (data[station][1] == 0X03 && data[station][2] == 0XF2)  // Read : BMW SW Revision
			{
			//printf ("Read : BMW SW Revision\r\n");
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
					
				sprintf(BMW_SW_Revision_Read[station],"%02X%02X",data[station][4], 
																 data[station][3]); 
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "--------- Read : BMW SW Revision\r\n");
			}
			if (data[station][1] == 0X03 && data[station][2] == 0XF2)  // Read : BMW HW Revision
			{
			//printf ("Read : BMW HW Revision\r\n");
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
					
				sprintf(BMW_HW_Revision_Read[station],"%02X%02X",data[station][4], 
																 data[station][3]); 
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "--------- Read : BMW HW Revision\r\n");
			}
			
			if (data[station][4] == 0XFD && data[station][5] == 0X38)  // Manufacturing_Data_1
			{
				
				if (data[station][1] == 0x10) // check if we must send again another request to read more data
				{
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
					strcat (Log_Lin[station], "\r\n");
											

					memset (MAN_DATA_1_LineID_Read[station], 0X00, 10);
					
					// 2E 10 11 62 FD 38 [01] 00 
					MAN_DATA_1_LineID_Read[station][0] = data[station][6];  
		
					// 2E 10 11 62 FD 38 01 [10] 
					MAN_DATA_1_VALEO_Part_Number_Read[station][0] = data[station][7]; 

					for (j=0; j<2; j++)
						{
							pexception = pcan->ReadLowLevel(pcan, pthread_data->lin_channel, &rid, &dataSize, data[station]);
							
							time = (clock() - _start_clock) / (double)CLOCKS_PER_SEC;
							strcat (Log_Lin[station], "RX	");
							sprintf (&CharBuff[0], "%f	", time);
							strcat (Log_Lin[station], CharBuff);
							sprintf (&CharBuff[0], "0X%2.2X	", rid);
							strcat (Log_Lin[station], CharBuff);
							sprintf (&CharBuff[0], "%1d		",dataSize);
							strcat (Log_Lin[station], CharBuff);
		
							if (data[station][1] == 0x21) // rest of frame   // 2E 21 [05 42 01 01 17 11]
							{
								for (i=0; i<dataSize; i++)
								{
								sprintf (&CharBuff[0], "%02X ", data[station][i]);
								strcat (Log_Lin[station], CharBuff);
								}
							
							MAN_DATA_1_VALEO_Part_Number_Read[station][1] = data[station][2]; 
							MAN_DATA_1_VALEO_Part_Number_Read[station][2] = data[station][3]; 
							MAN_DATA_1_VALEO_Part_Number_Read[station][3] = data[station][4]; 

							sprintf (MAN_DATA_1_VALEO_Part_Number_Read[station], "%02X%02X%02X%02X", 
																				 MAN_DATA_1_VALEO_Part_Number_Read[station][0],
																				 MAN_DATA_1_VALEO_Part_Number_Read[station][1],
																				 MAN_DATA_1_VALEO_Part_Number_Read[station][2],
																				 MAN_DATA_1_VALEO_Part_Number_Read[station][3]); 
							
							//printf ("MAN_DATA_1_VALEO_Part_Number_Read[station] = %s\r\n", MAN_DATA_1_VALEO_Part_Number_Read[station]);
							
							
							sprintf (MAN_DATA_1_Revision_Index_Read[station], "%02X", data[station][4]);
							//printf ("MAN_DATA_1_Revision_Index_Read[station] = %s\r\n", MAN_DATA_1_Revision_Index_Read[station]);
							
							////sprintf (MAN_DATA_1_LineID_Read[station], "%02X", data[station][5]);
							//printf ("MAN_DATA_1_LineID_Read[station] = %s\r\n", MAN_DATA_1_LineID_Read[station]);
							
							sprintf (MAN_DATA_1_Production_Year_Read[station], "%02X", data[station][6]);
							//printf ("MAN_DATA_1_Production_Year_Read[station] = %s\r\n", MAN_DATA_1_Production_Year_Read[station]);
							
							sprintf (MAN_DATA_1_Production_Month_Read[station], "%02X", data[station][7]);
							//printf ("MAN_DATA_1_Production_Month_Read[station] = %s\r\n", MAN_DATA_1_Production_Month_Read[station]);
							
							strcat (Log_Lin[station], "\r\n");
							}
							
							if (data[station][1] == 0x22) // rest of frame   // 2E 22 [29 00 99 99 09 10]
							{
								for (i=0; i<dataSize; i++)
								{
								sprintf (&CharBuff[0], "%02X ", data[station][i]);
								strcat (Log_Lin[station], CharBuff);
								}
							
							sprintf (MAN_DATA_1_Production_Day_Read[station], "%02X", data[station][2]);
							//printf ("MAN_DATA_1_Production_Day_Read[station] = %s\r\n", MAN_DATA_1_Production_Day_Read[station]);
							
							sprintf (MAN_DATA_1_Serial_Read[station], "%02X%02X", data[station][3],
																				  data[station][4]);
							
							//printf ("MAN_DATA_1_Serial_Read[station] = %s\r\n", MAN_DATA_1_Serial_Read[station]);
							
							sprintf (MAN_DATA_1_Tracability_EMS_Read[station], "%02X", data[station][5]);
							
							//printf ("MAN_DATA_1_Tracability_MMS_Read[station] = %s\r\n", MAN_DATA_1_Tracability_MMS_Read[station]);
						
							iFlag_Read_MAN_DATA_1_READ_OK[station] = TRUE; 
							
							strcat (Log_Lin[station], "\r\n");
							}
						
						}
			
				strcat (Log_Lin[station], "--------- Read : Manufacturing data 1\r\n");
				}
		
			}
		
			if (data[station][4] == 0XFD && data[station][5] == 0X39)  // Manufacturing_Data_2
			{
				if (data[station][1] == 0x10) // check if we must send again another request to read more data
				{
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
					strcat (Log_Lin[station], "\r\n");
					
					memset (MAN_DATA_2_VALEO_Part_Number_Read[station], 0X00, 10);
					
					// 2E 10 11 62 FD 39[00 59] EXPLE : 42055900
					MAN_DATA_2_VALEO_Part_Number_Read[station][0] = data[station][6];  
					MAN_DATA_2_VALEO_Part_Number_Read[station][1] = data[station][7];  // 2E 10 11 62 FD 39[59 05]
				   	//printf ("data[station][7] = %02X\r\n", data[station][7]);
					//printf ("MAN_DATA_2_VALEO_Part_Number_Read[station][1] = %02X\r\n", MAN_DATA_2_VALEO_Part_Number_Read[station][1]);

					for (j=0; j<2; j++)
						{
							pexception = pcan->ReadLowLevel(pcan, pthread_data->lin_channel, &rid, &dataSize, data[station]);
							
							time = (clock() - _start_clock) / (double)CLOCKS_PER_SEC;
							strcat (Log_Lin[station], "RX	");
							sprintf (&CharBuff[0], "%f	", time);
							strcat (Log_Lin[station], CharBuff);
							sprintf (&CharBuff[0], "0X%2.2X	", rid);
							strcat (Log_Lin[station], CharBuff);
							sprintf (&CharBuff[0], "%1d		",dataSize);
							strcat (Log_Lin[station], CharBuff);
		
							if (data[station][1] == 0x21) // rest of frame   // 2E 21 [05 42 01 01 17 11]
							{
								for (i=0; i<dataSize; i++)
								{
								sprintf (&CharBuff[0], "%02X ", data[station][i]);
								strcat (Log_Lin[station], CharBuff);
								}
							
							MAN_DATA_2_VALEO_Part_Number_Read[station][2] = data[station][2]; 
							MAN_DATA_2_VALEO_Part_Number_Read[station][3] = data[station][3]; 
							
							sprintf (MAN_DATA_2_VALEO_Part_Number_Read[station], "%02X%02X%02X%02X", 
																				 MAN_DATA_2_VALEO_Part_Number_Read[station][0],
																				 MAN_DATA_2_VALEO_Part_Number_Read[station][1],
																				 MAN_DATA_2_VALEO_Part_Number_Read[station][2],
																				 MAN_DATA_2_VALEO_Part_Number_Read[station][3]); 
							
							//printf ("MAN_DATA_2_VALEO_Part_Number_Read[station] = %s\r\n", MAN_DATA_2_VALEO_Part_Number_Read[station]);
							
							
							sprintf (MAN_DATA_2_Revision_Index_Read[station], "%02X", data[station][4] - 48);
							//printf ("MAN_DATA_2_Revision_Index_Read[station] = %s\r\n", MAN_DATA_2_Revision_Index_Read[station]);
							
							sprintf (MAN_DATA_2_LineID_Read[station], "%02X", data[station][5]);
							//printf ("MAN_DATA_2_LineID_Read[station] = %s\r\n", MAN_DATA_2_LineID_Read[station]);
							
							sprintf (MAN_DATA_2_Production_Year_Read[station], "%02X", data[station][6]);
							//printf ("MAN_DATA_2_Production_Year_Read[station] = %s\r\n", MAN_DATA_2_Production_Year_Read[station]);
							
							sprintf (MAN_DATA_2_Production_Month_Read[station], "%02X", data[station][7]);
							//printf ("MAN_DATA_2_Production_Month_Read[station] = %s\r\n", MAN_DATA_2_Production_Month_Read[station]);
							
							
							strcat (Log_Lin[station], "\r\n");
							}
							
							if (data[station][1] == 0x22) // rest of frame   // 2E 22 [29 00 99 99 09 10]
							{
								for (i=0; i<dataSize; i++)
								{
								sprintf (&CharBuff[0], "%02X ", data[station][i]);
								strcat (Log_Lin[station], CharBuff);
								}
							
							sprintf (MAN_DATA_2_Production_Day_Read[station], "%02X", data[station][2]);
							//printf ("MAN_DATA_2_Production_Day_Read[station] = %s\r\n", MAN_DATA_2_Production_Day_Read[station]);
							
							sprintf (MAN_DATA_2_Serial_Read[station], "%02X%02X%02X%02X", data[station][3],
																						  data[station][4],
																						  data[station][5],
																						  data[station][6]);
							//printf ("MAN_DATA_2_Serial_Read[station] = %s\r\n", MAN_DATA_2_Serial_Read[station]);
							
							sprintf (MAN_DATA_2_Tracability_MMS_Read[station], "%02X", data[station][7]);
							
							//printf ("MAN_DATA_2_Tracability_MMS_Read[station] = %s\r\n", MAN_DATA_2_Tracability_MMS_Read[station]);
						
							iFlag_Read_MAN_DATA_2_READ_OK[station] = TRUE; 
							
							strcat (Log_Lin[station], "\r\n");
							}
							
							
						}
				strcat (Log_Lin[station], "--------- Read : Manufacturing data 2\r\n");
				}
		
			}
	
			if (data[station][4] == 0XFD && data[station][5] == 0X32  && data[station][6] == 0X01)  // Erasing_DTC_Memory1  2E 05 71 01 FD 32 01 FF
			{
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "---------Erasing_DTC_Memory_1\r\n");
			}
		
			if (data[station][4] == 0XFD && data[station][5] == 0X32  && data[station][6] == 0X03)  // Erasing_DTC_Memory2  2E 05 71 03 FD 32 03 FF
			{
					for (i=0; i<dataSize; i++)
					{																									
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "---------Erasing_DTC_Memory_2\r\n");
			}
			
			if (data[station][3] == 0XFD && data[station][4] == 0X30)  // Number of DTC  2E 04 62 FD 30 [01] FF FF
			{
					for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
					
				Number_DTC[station] = data[station][5];
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "---------Number of DTC\r\n");
				if (Number_DTC[station] == 0)
					break;
			}
			if (data[station][3] == 0XFD && data[station][4] == 0X81 )  // valeo production key
			{
					for (i=0; i<dataSize; i++)
					{																									
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
				strcat (Log_Lin[station], "\r\n");
				strcat (Log_Lin[station], "---------Valeo_production_key\r\n");
			}
		
			else
			{
				for (i=0; i<dataSize; i++)
					{
						sprintf (&CharBuff[0], "%02X ", data[station][i]); 
						strcat (Log_Lin[station], CharBuff);
					}
				strcat (Log_Lin[station], "\r\n");
			}
			
		}
		   
		if (rid == 0X01 && dataSize == 8) //refresh the register of pushs status
		{
		iFlag_Run_Normal_Mode[station] = 1;
		time = (clock() - _start_clock) / (double)CLOCKS_PER_SEC;
		strcat (Log_Lin[station], "RX	");
		sprintf (&CharBuff[0], "%f	", time);
		strcat (Log_Lin[station], CharBuff);
		sprintf (&CharBuff[0], "0X%2.2X	", rid);
		strcat (Log_Lin[station], CharBuff);
		sprintf (&CharBuff[0], "%1d		",dataSize);
		strcat (Log_Lin[station], CharBuff);
		for (i=0; i<dataSize; i++)
		{
		if (data[station][i] >= 0 && data[station][i] <= 255)
			{
			//if (i=6)
				//printf("%02X[%fs]\r\n", data_pushs[station][6], time);

			data_pushs[station][i]  = data[station][i];
			//if (iFlag_Store_All_data_Pushs[station] == TRUE)
				//All_data_pushs[station][k++][i] = data[station][i];
			sprintf (&CharBuff[0], "%02X ", data[station][i]);
			strcat (Log_Lin[station], CharBuff);
			}
		}
		strcat (Log_Lin[station], "\r\n");
		
		/* check Alive byte 0X10 -> 0XE0 -> 0X10 */
		
			if ( (data[station][6] & 0XF0) == 0X00 && first_LIN_Frame[station] == TRUE)
				{
				Last_Alive_Byte[station] = data[station][6] & 0XF0;
				first_LIN_Frame[station] = FALSE;
				}
			
			else if (( data[station][6]  & 0XF0 == 0X00) && first_LIN_Frame[station] == FALSE) //Last alive should be 0X0E
				{
					if (Last_Alive_Byte[station] & 0XF0 != 0XE0)
						{
							if (data[station][6] & 0XF0 != 0X00) //not same frame
							{
							Error_Alive_Byte[station] = 1;
							strcat (Log_Lin[station], "------------- Error Alive Byte\r\n");
							}
						}
					Last_Alive_Byte[station] = data[station][6] & 0XF0;
				}
			else if (first_LIN_Frame == FALSE)
				{
					if (data[station][6] & 0XF0 != Last_Alive_Byte[station] + 16)
						{
							if (data[station][6] & 0XF0 != Last_Alive_Byte[station]) //not same frame
							{
							Error_Alive_Byte[station] = 1;
							strcat (Log_Lin[station], "------------- Error Alive Byte\r\n");
							}
						}
					Last_Alive_Byte[station] = data[station][6] & 0XF0;
				}
		}
	
		if (rid == 0X03 && dataSize == 8) 
		{
		time = (clock() - _start_clock) / (double)CLOCKS_PER_SEC;
		strcat (Log_Lin[station], "RX	");
		sprintf (&CharBuff[0], "%f	", time);
		strcat (Log_Lin[station], CharBuff);
		sprintf (&CharBuff[0], "0X%2.2X	", rid);
		strcat (Log_Lin[station], CharBuff);
		sprintf (&CharBuff[0], "%1d		",dataSize);
		strcat (Log_Lin[station], CharBuff);
		for (i=0; i<dataSize; i++)
		{
			if (data[station][i] >= 0 && data[station][i] <= 255)
			{
			sprintf (&CharBuff[0], "%02X ", data[station][i]);
			strcat (Log_Lin[station], CharBuff);
			}
		}
		strcat (Log_Lin[station], "\r\n");
		}
	  }

	/* Wait 50ms after stop communication to Go to Sleep */
	Sleep (50);

	/* Go to sleep */
	pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Go_To_Sleep);
	
Error:
	free(pthread_data);
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Lin_Write"
DWORD WINAPI Thread_Lin_Write(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	SLinThreadDataPtr			pthread_data = (SLinThreadDataPtr)param;
	SCANListPtr					pcan = pthread_data->pcan;
	uint32_t					station = pthread_data->station;
  	uint32_t					timeout = pthread_data->timeout;
   	uint32_t					i;
    char CharBuff[256];
	
	/* timeout variable */
	clock_t 					_start_test_clock;
	clock_t 					_start_debug_clock;
	double   					time, time_debug;
	
	
	
	/* LIN commands */
	unsigned char LRE_VRFD[8][8]={0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0xFF,0x00};
	unsigned char Read_Switch_Raw_Values[8]={0x2E,0x03,0x22,0xFD,0x51,0xFF,0xFF,0xFF};
	unsigned char DIM_PWM[8]={0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
	
	/* LIN Production_Key commands */
	unsigned char Production_Key1[8]={0x2E,0x10,0x08,0x2E,0xFD,0x81,0x02,0x2B}; 
	unsigned char Production_Key2[8]={0x2E,0x21,0x65,0x76,0x8B,0xFF,0xFF,0xFF};
	
	/* LIN diagnostic commands */
	unsigned char Write_Variant_Code[8]={0x2E,0x04,0x2E,0xFD,0x3C,0xF4,0xFF,0xFF};//   2E 04 2E FD 3C F4 FF FF
	unsigned char Read_Variant_Code[8]={0x2E,0x06,0xB2,0x00,0xFF,0x7F,0x00,0x10};//B2 00 FF 7F 00 10
	
	unsigned char Read_Valeo_Sw_Revision[8]={0x2E,0x03,0x22,0xFD,0x33,0xFF,0xFF,0xFF};//22 FD 33  
	unsigned char Read_Sw_Build_Date[8]={0x2E,0x03,0x22,0xFD,0x34,0xFF,0xFF,0xFF};//22 FD 34
	
	unsigned char Read_Manufacturing_Data_1[8]={0x2E,0x03,0x22,0xFD,0x38,0xFF,0xFF,0xFF};//22 FD 38  
	unsigned char Read_Manufacturing_Data_2[8]={0x2E,0x03,0x22,0xFD,0x39,0xFF,0xFF,0xFF};//22 FD 39
	
	unsigned char Erasing_DTC_Memory_1[8]={0x2E,0x04,0x31,0x01,0xFD,0x32,0xFF,0xFF}; //2E 04 31 01 FD 32 FF FF
	unsigned char Erasing_DTC_Memory_2[8]={0x2E,0x04,0x31,0x03,0xFD,0x32,0xFF,0xFF}; //2E 04 31 03 FD 32 FF FF
	unsigned char Read_Number_DTC_Memory[8]={0x2E,0x03,0x22,0xFD,0x30,0xFF,0xFF,0xFF}; //2E 03 22 FD 30 FF FF FF
	
	/* LIN customer commands */
	unsigned char Write_BMW_Part_Number1[8]={0x2E,0x10,0x08,0x2E,0xFD,0x3B,0x24,0x95};
	unsigned char Write_BMW_Part_Number2[8]={0x2E,0x21,0x94,0x07,0x00,0xFF,0xFF,0xFF};
	unsigned char Read_BMW_Part_Number[8]={0x2E,0x06,0xB2,0x29,0xFF,0x7F,0x00,0x10};
	
	unsigned char Write_BMW_SW_Revision[8]={0x2E,0x05,0x2E,0xFD,0x3D,0x00,0x09,0xFF};
	unsigned char Read_BMW_SW_Revision[8]={0x2E,0x06,0xB2,0x2A,0xFF,0x7F,0x00,0x10};
	
	unsigned char Write_BMW_HW_Revision[8]={0x2E,0x05,0x2E,0xFD,0x3E,0x00,0x03,0xFF};
	unsigned char Read_BMW_HW_Revision[8]={0x2E,0x06,0xB2,0x2B,0xFF,0x7F,0x00,0x10};
	
	unsigned char Read_Man_Data_1[8]={0x2E,0x03,0x22,0xFD,0x38,0xFF,0xFF,0xFF};
	
	unsigned char Write_Man_Data2_1[8]={0x2E,0x10,0x11,0x2E,0xFD,0x39,0x00,0x00};
	unsigned char Write_Man_Data2_2[8]={0x2E,0x21,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char Write_Man_Data2_3[8]={0x2E,0x22,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char Read_Man_Data_2[8]={0x2E,0x03,0x22,0xFD,0x39,0xFF,0xFF,0xFF};
	
	
	unsigned char data[8];
	int32_t dataSize  = 8;
	int32_t rid;

	
	CmtSetCurrentThreadPriority (THREAD_PRIORITY_TIME_CRITICAL);
	
	iFlag_Write_Valeo_production_Key[station] = FALSE;
	iFlag_Read_Switch_Raw_Values[station] = FALSE;
	iFlag_Write_Variant_code[station] = FALSE;
	iFlag_Read_Variant_code[station] = FALSE;
	iFlag_Write_MAN_DATA_2[station] = FALSE;
	iFlag_Read_MAN_DATA_1[station] = FALSE;
	iFlag_Read_MAN_DATA_2[station] = FALSE;
	iFlag_Erasing_DTC_Memory_1[station] = FALSE;
	iFlag_Erasing_DTC_Memory_2[station] = FALSE;
	iFlag_Read_Number_DTC_Memory[station] = FALSE;
	
	static int ALIV[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	int Intensity_STA;
	int Intensity_BG;
	int ColorByte = 0x2F;
	int crc = 0x00;
	int Color = 3;
    int Mode = 5;

		

	if (iFlag_LED_STA_RED[station] == TRUE)
		 Color = 1;
	else if (iFlag_LED_STA_GREEN[station] == TRUE) 
		 Color = 2;
	else if (iFlag_LED_STA_YELLOW[station] == TRUE) 
		 Color = 3;
	
	switch(Color)
	{
	case 1:
       ColorByte= 0x4F;  //RED
       break;
   
	case 2:
	ColorByte= 0x1F;	 //GREEN
       break;
	   
	case 3:
	ColorByte= 0x2F;	//YELLOW
       break;
	}
	
	LRE_VRFD[station][4]= ColorByte;
	ALIV[station]=Mode;
	iFlag_Write_Wake_Up[station] = TRUE;
	iFlag_Thread_Lin_Write[station] = TRUE;
		
	TIMEOUT_LIN_INIT();
	
	while (iFlag_Thread_Lin_Write[station])
	{
	
		if (iFlag_Write_Wake_Up[station] == TRUE)
		{
		/* send 0x10 */
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x01, 17, 0, "");
		Sleep (10);
	
		/* send 0x03 */
		ALIV[station]+=0x10;
		
		if(ALIV[station]==0xF0||ALIV[station]==0xF1||ALIV[station]==0xF2||ALIV[station]==0xF3||ALIV[station]==0xF4||ALIV[station]==0xF5)
		{
		ALIV[station]=Mode;
		}
    	LRE_VRFD[station][6]= ALIV[station];
    	CalcChecksumV1(LRE_VRFD[station], &crc);
        LRE_VRFD[station][7]=crc; 
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x03, 18, dataSize, LRE_VRFD[station]);
		Sleep (10);
		}
		
		/* send 0x2B LED_BG*/
		if (iFlag_LED_BG[station] == TRUE)
		{
		DIM_PWM[1]=0XFE;
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x2B, 18, 4, DIM_PWM);
		Sleep (10);
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x2B, 18, 4, DIM_PWM);
		Sleep (10);
		iFlag_LED_BG[station] = FALSE;	
		}
		
		/* send 0x2B LED_STA_RED */
		if (iFlag_LED_STA_RED[station] == TRUE)
		{
		DIM_PWM[2]=0X10;
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x2B, 18, 4, DIM_PWM);
		Sleep (10);
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x2B, 18, 4, DIM_PWM);
		Sleep (10);
		iFlag_LED_STA_RED[station] = FALSE;	
		}
		
		/* send 0x2B LED_STA_GREEN */
		if (iFlag_LED_STA_GREEN[station] == TRUE)
		{
		DIM_PWM[2]=0X10;
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x2B, 18, 4, DIM_PWM);
		Sleep (10);
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x2B, 18, 4, DIM_PWM);
		Sleep (10);
		iFlag_LED_STA_GREEN[station] = FALSE;	
		}
		
		/* send 0x2B LED_STA_YELLOW */
		if (iFlag_LED_STA_YELLOW[station] == TRUE)
		{
		DIM_PWM[2]=0XFE;;
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x2B, 18, 4, DIM_PWM);
		Sleep (10);
		pexception = pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x2B, 18, 4, DIM_PWM);
		Sleep (10);
		iFlag_LED_STA_YELLOW[station] = FALSE;	
		}
		
		/* Valeo_production_Key */
		if (iFlag_Write_Valeo_production_Key[station] == TRUE)
		{
		/* send Production_Key1 */
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Production_Key1));
		Sleep (10);
		/* send Production_Key2 */
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Production_Key2));
		Sleep (10);
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		Sleep (10);
		iFlag_Write_Valeo_production_Key[station] = FALSE;
		}
		
		/* Switch_Raw_Values */
	  	if (iFlag_Read_Switch_Raw_Values[station] == TRUE)
		{
		iFlag_Write_Wake_Up[station] = FALSE;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_Switch_Raw_Values))
		Sleep (20);
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan,  pthread_data->lin_channel, 0x3D, 17, 1, data));
		Sleep (10);
		EXCCHECK(pcan->WriteLowLevel(pcan,  pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Write_Valeo_production_Key[station] = FALSE;
		iFlag_Read_Switch_Raw_Values[station] = FALSE;
		}
		
		/* Write : Variant code */
		if (iFlag_Write_Variant_code[station] == TRUE)
		{
		Write_Variant_Code [5] = Get_Variant_Code(Variant_Code_Write[station]);
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Write_Variant_Code));
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Write_Variant_code[station] = FALSE;
		Sleep (10);
		}
		
		/* Read : Variant code */
		if (iFlag_Read_Variant_code[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_Variant_Code));
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Read_Variant_code[station] = FALSE;
		Sleep (10);
		}
		
		/* Read : Valeo Sw Revision */
		if (iFlag_Read_Valeo_Sw_Revision[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_Valeo_Sw_Revision));
		Sleep (20);
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Read_Valeo_Sw_Revision[station] = FALSE;
		Sleep (10);
		}
	
		/* Read : Sw Build Date */
		if (iFlag_Read_Sw_Build_Date[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_Sw_Build_Date));
		Sleep (20);
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan,  pthread_data->lin_channel, 0x3D, 17, 1, data));
		Sleep (10);
		EXCCHECK(pcan->WriteLowLevel(pcan,  pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Read_Sw_Build_Date[station] = FALSE;
		}
		
		/* Write : BMW_Part_Number */
		if (iFlag_Write_BMW_Part_Number[station] == TRUE)
		{
		char byte[2+1];
		memset (byte, 0X00, 3);
		//Exemple : BMW_Part_Number = 7949524
		
		/* copy 24 */
		CopyString (byte, 0, BMW_Part_Number_Write[station], 5, 2);
		Write_BMW_Part_Number1[6] = (int)strtol(byte, NULL, 16);
	
		/* copy 95 */
		CopyString (byte, 0, BMW_Part_Number_Write[station], 3, 2);
		Write_BMW_Part_Number1[7] = (int)strtol(byte, NULL, 16);	
		
		/* copy 94 */
		CopyString (byte, 0, BMW_Part_Number_Write[station], 1, 2);
		Write_BMW_Part_Number2[2] = (int)strtol(byte, NULL, 16);
	
		/* copy 7 */
		CopyString (byte, 0, BMW_Part_Number_Write[station], 0, 1);
		Write_BMW_Part_Number2[3] = (int)strtol(byte, NULL, 16);
		
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Write_BMW_Part_Number1));
		//Sleep (10);
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Write_BMW_Part_Number2));
		iFlag_Write_BMW_Part_Number[station] = FALSE;
		Sleep (10);
		}
		
		/* Read : BMW_Part_Number */
		if (iFlag_Read_BMW_Part_Number[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_BMW_Part_Number));
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Read_BMW_Part_Number[station] = FALSE;
		Sleep (10);
		}
		
		/* Write : BMW_SW_Revision */
		if (iFlag_Write_BMW_SW_Revision[station] == TRUE)
		{
		char byte[2+1];
		memset (byte, 0X00, 3);
		//Exemple : BMW_SW_Revision = 0900
		
		/* copy 00 */
		CopyString (byte, 0, BMW_SW_Revision_Write[station], 2, 2);
		Write_BMW_SW_Revision[5] = (int)strtol(byte, NULL, 16);
	
		/* copy 09 */
		CopyString (byte, 0, BMW_SW_Revision_Write[station], 0, 2);
		Write_BMW_SW_Revision[6] = (int)strtol(byte, NULL, 16);	
		
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Write_BMW_SW_Revision));
		iFlag_Write_BMW_SW_Revision[station] = FALSE;
		Sleep (10);
		}
		
		/* Read : BMW_SW_Revision */
		if (iFlag_Read_BMW_SW_Revision[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_BMW_SW_Revision));
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Read_BMW_SW_Revision[station] = FALSE;
		Sleep (10);
		}	
		
		/* Write : BMW_HW_Revision */
		if (iFlag_Write_BMW_HW_Revision[station] == TRUE)
		{
		char byte[2+1];
		memset (byte, 0X00, 3);
		//Exemple : BMW_SW_Revision = 0300
		
		/* copy 00 */
		CopyString (byte, 0, BMW_HW_Revision_Write[station], 2, 2);
		Write_BMW_HW_Revision[5] = (int)strtol(byte, NULL, 16);
	
		/* copy 09 */
		CopyString (byte, 0, BMW_HW_Revision_Write[station], 0, 2);
		Write_BMW_HW_Revision[6] = (int)strtol(byte, NULL, 16);	
		
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Write_BMW_HW_Revision));
		iFlag_Write_BMW_HW_Revision[station] = FALSE;
		Sleep (10);
		}
		
		/* Read : BMW_SW_Revision */
		if (iFlag_Read_BMW_HW_Revision[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_BMW_HW_Revision));
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Read_BMW_HW_Revision[station] = FALSE;
		Sleep (10);
		}
		
		/* DTC : Erasing_DTC_Memory1 */
		if (iFlag_Erasing_DTC_Memory_1[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Erasing_DTC_Memory_1));
		Sleep (10);
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Erasing_DTC_Memory_1[station] = FALSE;
		}
		
		/* DTC : Erasing_DTC_Memory2 */
		if (iFlag_Erasing_DTC_Memory_2[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Erasing_DTC_Memory_2));
		Sleep (10);
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Erasing_DTC_Memory_2[station] = FALSE;
		}
		 
		/* DTC : Number_DTC_Memory */
		if (iFlag_Read_Number_DTC_Memory[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_Number_DTC_Memory));
		Sleep (20);
		data[0]=0x2E;
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3D, 17, 1, data));
		iFlag_Read_Number_DTC_Memory[station] = FALSE;
		}
		
		/* Read : Manufacturing_Data_1 */
		if (iFlag_Read_MAN_DATA_1[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_Man_Data_1));
		Sleep (10);
			data[0]=0x2E;
			for (i=0;i<4; i++)
			{
			EXCCHECK(pcan->WriteLowLevel(pcan,  pthread_data->lin_channel, 0x3D, 17, 1, data));
			Sleep (10);
			}
		iFlag_Read_MAN_DATA_1[station] = FALSE;
		}
		
		/* Write : Manufacturing Data_2  */
		if (iFlag_Write_MAN_DATA_2[station] == TRUE)
		{
		char byte[2+1];
		memset (byte, 0X00, 3);
		
		//Exemple : VALEO_Part_Number = [42055901] ==> 01590542 in Man data 2
		if (strlen(MAN_DATA_2_VALEO_Part_Number_Write[station]) == 8)
		{
		/* copy 42 */
		CopyString (byte, 0, MAN_DATA_2_VALEO_Part_Number_Write[station], 0, 2);
		Write_Man_Data2_1[6] = (int)strtol(byte, NULL, 16);
			
		/* copy 05 */
		CopyString (byte, 0, MAN_DATA_2_VALEO_Part_Number_Write[station], 2, 2);
		Write_Man_Data2_1[7] = (int)strtol(byte, NULL, 16);
	
		/* copy 59 */
		CopyString (byte, 0, MAN_DATA_2_VALEO_Part_Number_Write[station], 4, 2);
		Write_Man_Data2_2[2] = (int)strtol(byte, NULL, 16);
		
		/* copy 01 */
		CopyString (byte, 0, MAN_DATA_2_VALEO_Part_Number_Write[station], 6, 2);
		Write_Man_Data2_2[3] = (int)strtol(byte, NULL, 16);
		}
		
		//Exemple : VALEO_Part_Number = E[10904801]
		else if(strlen (MAN_DATA_2_VALEO_Part_Number_Write[station]) == 9) 
		{
		/* copy 10 */
		CopyString (byte, 0, MAN_DATA_2_VALEO_Part_Number_Write[station], 1, 2);
		Write_Man_Data2_1[6] = (int)strtol(byte, NULL, 16);
			
		/* copy 90 */
		CopyString (byte, 0, MAN_DATA_2_VALEO_Part_Number_Write[station], 3, 2);
		Write_Man_Data2_1[7] = (int)strtol(byte, NULL, 16);
	
		/* copy 48 */
		CopyString (byte, 0, MAN_DATA_2_VALEO_Part_Number_Write[station], 5, 2);
		Write_Man_Data2_2[2] = (int)strtol(byte, NULL, 16);
		
		/* copy 01 */
		CopyString (byte, 0, MAN_DATA_2_VALEO_Part_Number_Write[station], 7, 2);
		Write_Man_Data2_2[3] = (int)strtol(byte, NULL, 16);
		}
		
		//Exemple : Revision_Index = 01
		/* copy 01 */
		CopyString (byte, 0, MAN_DATA_2_Revision_Index_Write[station], 0, 2);
		Write_Man_Data2_2[4] = 48 + (int)strtol(byte, NULL, 16);
		
		//Exemple : LineID = 01
		/* copy 01 */
		CopyString (byte, 0, MAN_DATA_2_LineID_Write[station], 0, 2);
		Write_Man_Data2_2[5] = (int)strtol(byte, NULL, 16);
		
		//Exemple : Year = 18
		/* copy 18 */
		CopyString (byte, 0, MAN_DATA_2_Production_Year_Write[station], 0, 2);
		Write_Man_Data2_2[6] = (int)strtol(byte, NULL, 16);
		
		//Exemple : Month = 01
		/* copy 01 */
		CopyString (byte, 0, MAN_DATA_2_Production_Month_Write[station], 0, 2);
		Write_Man_Data2_2[7] = (int)strtol(byte, NULL, 16);
		
		//Exemple : Day = 29
		/* copy 29 */
		CopyString (byte, 0, MAN_DATA_2_Production_Day_Write[station], 0, 2);
		Write_Man_Data2_3[2] = (int)strtol(byte, NULL, 16);
		
		//Exemple : Serial = 20 55 00 62 in Man data 2
		/* copy aj */
		CopyString (byte, 0, MAN_DATA_2_Serial_Write[station], 0, 2);
		Write_Man_Data2_3[3] = (int)strtol(byte, NULL, 16);
		
		/* copy jj */
		CopyString (byte, 0, MAN_DATA_2_Serial_Write[station], 2, 2);
		Write_Man_Data2_3[4] = (int)strtol(byte, NULL, 16);
	
		/* copy 99 */
		CopyString (byte, 0, MAN_DATA_2_Serial_Write[station], 4, 2);
		Write_Man_Data2_3[5] = (int)strtol(byte, NULL, 16);
		
		/* copy 09 */
		CopyString (byte, 0, MAN_DATA_2_Serial_Write[station], 6, 2);
		Write_Man_Data2_3[6] = (int)strtol(byte, NULL, 16);
		
		
		//Exemple : Tracability_MMS = 10 
		/* copy 10 */
		CopyString (byte, 0, MAN_DATA_2_Tracability_MMS_Write[station], 0, 2);
		Write_Man_Data2_3[7] = (int)strtol(byte, NULL, 16);
		
#if 0    /* formerly excluded lines */
		printf ("Write_Man_Data2_1 = %02X %02X %02X %02X %02X %02X %02X %02X\r\n", Write_Man_Data2_1[0],
																			Write_Man_Data2_1[1],
																			Write_Man_Data2_1[2],
																			Write_Man_Data2_1[3],
																			Write_Man_Data2_1[4],
																			Write_Man_Data2_1[5],
																			Write_Man_Data2_1[6],
																			Write_Man_Data2_1[7]);
		
		printf ("Write_Man_Data2_2 = %02X %02X %02X %02X %02X %02X %02X %02X\r\n", Write_Man_Data2_2[0],
																			Write_Man_Data2_2[1],
																			Write_Man_Data2_2[2],
																			Write_Man_Data2_2[3],
																			Write_Man_Data2_2[4],
																			Write_Man_Data2_2[5],
																			Write_Man_Data2_2[6],
																			Write_Man_Data2_2[7]);
		
		printf ("Write_Man_Data2_3 = %02X %02X %02X %02X %02X %02X %02X %02X\r\n", Write_Man_Data2_3[0],
																			Write_Man_Data2_3[1],
																			Write_Man_Data2_3[2],
																			Write_Man_Data2_3[3],
																			Write_Man_Data2_3[4],
																			Write_Man_Data2_3[5],
																			Write_Man_Data2_3[6],
																			Write_Man_Data2_3[7]);
#endif   /* formerly excluded lines */

		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Write_Man_Data2_1));
		//Sleep (10);
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Write_Man_Data2_2));
		//Sleep (10);
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Write_Man_Data2_3));
		//Sleep (10);
		iFlag_Write_MAN_DATA_2[station] = FALSE;
		}
	
		/* Read : Manufacturing_Data_2 */
		if (iFlag_Read_MAN_DATA_2[station] == TRUE)
		{
		EXCCHECK(pcan->WriteLowLevel(pcan, pthread_data->lin_channel, 0x3C, 18, 8, Read_Man_Data_2));
		Sleep (15);																					  
			data[0]=0x2E;
			for (i=0;i<4; i++)
			{
			EXCCHECK(pcan->WriteLowLevel(pcan,  pthread_data->lin_channel, 0x3D, 17, 1, data));
			Sleep (10);
			}
		iFlag_Read_MAN_DATA_2[station] = FALSE;
		}
	}
			
Error:
	free(pthread_data);
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Get_push_value"
bool_t Get_push_value (char *push_name, int station)
{
return Get_push_value_from_name(data_pushs[station], push_name);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Get_ADC_push_value"
double Get_ADC_push_value (char *push_name, int station)
{
return Get_ADC_value(ADC_pushs[station], push_name);
}
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Read_ADC"
SElExceptionPtr Read_ADC(SCANListPtr pcan, int station)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	
    TIMEOUT_LIN_INIT();
	
	/* check if all data received */
	ADC_Received[station] = FALSE;
	
  	do{
		if (ADC_Received[station] == TRUE)
			break;
        TIMEOUT_LIN_CHECK(5000);
    }while(TRUE);

Error:
	EXCRETHROW( pexception);
}

/*
bool_t Get_push_value_from_name (uint8_t data[8], char *push_name)
{
bool_t status_push;
char msg[128];
int byte;


// Right Side 
if (!strcmp(push_name, "PHONE"))
status_push = (data[0] >> 0) & 1; 	

else if (!strcmp(push_name, "LIST"))
status_push = (data[1] >> 4) & 1;

else if (!strcmp(push_name, "SPEECH"))
status_push = (data[1] >> 0) & 1;

else if (!strcmp(push_name, "MENU_RIGHT"))
status_push = (data[5] >> 5) & 1;

else if (!strcmp(push_name, "SCROLL_WHEEL"))
status_push = (data[1] >> 6) & 1;

else if (!strcmp(push_name, "MENU_LEFT"))
status_push = (data[5] >> 4) & 1;

else if (!strcmp(push_name, "VOLUME_PLUS"))
status_push = (data[1] >> 3) & 1;

else if (!strcmp(push_name, "VOLUME_MINUS"))
status_push = (data[1] >> 2) & 1;

else if (!strcmp(push_name, "SCROLL"))
status_push = data[6] & 0X0F;


// Left Side
else if (!strcmp(push_name, "MODE"))
status_push = (data[4] >> 1) & 1;

else if (!strcmp(push_name, "ASS"))
status_push = (data[4] >> 2) & 1;

else if (!strcmp(push_name, "SET"))
status_push = (data[3] >> 6) & 1;

else if (!strcmp(push_name, "MINUS"))
status_push = (data[4] >> 5) & 1;

else if (!strcmp(push_name, "PLUS"))
status_push = (data[4] >> 6) & 1;

else if (!strcmp(push_name, "LIM"))
status_push = (data[3] >> 5) & 1;

else if (!strcmp(push_name, "RES_CANC"))
status_push = (data[2] >> 7) & 1;

else if (!strcmp(push_name, "DCC") || !strcmp(push_name, "ACC"))
status_push = (data[2] >> 0) & 1;

else if (!strcmp(push_name, "RESUME"))
status_push = (data[2] >> 6) & 1;

else if (!strcmp(push_name, "CANCEL"))
status_push = (data[2] >> 5) & 1;

else if (!strcmp(push_name, "TIPP_WIPE"))
status_push = data[3];

else  
	{
	sprintf (msg, "Function not exist : %s!", push_name);
	MessagePopup ("MFL Function", msg);
	status_push = 0;
	}
return status_push;
//return status_push? 0 : 1;
}*/

bool_t Get_push_value_from_name (uint8_t data[8], char *push_name)
{
bool_t status_push;
char msg[128];
int byte;


// Right Side 
if (!strcmp(push_name, "PHONE"))
{
	 data[0]&=0x03;
	 
	if (data[0] == 0X01)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "LIST"))
{
	 data[1]&=0x30;
	 
	if (data[1] == 0X10)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "SPEECH"))
{
	data[1]&=0x03;
	
	if (data[1] == 0X01)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "MENU_RIGHT"))
{
	  data[5]&=0x30;
	  
	if (data[5] == 0X20)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "SCROLL_WHEEL"))
	{
	
		data[1]&=0xC0;
		
	if (data[1] == 0X40)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "MENU_LEFT"))
{
	data[5]&=0x30;
	
	if (data[5] == 0X10)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "VOLUME_PLUS"))
{
	data[1]&=0x0C;
	
	if (data[1] == 0X08)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "VOLUME_MINUS"))
{
	data[1]&=0x0C;
	
	if (data[1] == 0X04)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "SCROLL"))
status_push = data[6] & 0X0F;


// Left Side
else if (!strcmp(push_name, "MODE"))
{
	
	data[4]&=0x0F;
	
	if (data[4] == 0X03)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "ASS"))
{
	data[4]&=0x0F;
	
	if (data[4] == 0X04)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "SET"))
{
	data[3]&=0xF0;
	
	if (data[3] == 0X40)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "MINUS"))
{
	data[4]&=0xF0;
	
	if (data[4] == 0X20)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "PLUS"))
{
	if (data[4] == 0X40)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "LIM"))
{
	data[3]&=0xF0;
	
	if (data[3] == 0X20)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "RES_CANC"))
{
	data[2]&=0xF0;
	
	if (data[2] == 0X80)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "DCC") || !strcmp(push_name, "ACC"))
{
	data[2]&=0x0F;
	
	if (data[2] == 0X01)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "RESUME"))
{
	data[2]&=0xF0;
	
	if (data[2] == 0X40)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "CANCEL"))
{
	data[2]&=0xF0;
	
	if (data[2] == 0X20)
		status_push = 1; 
	else 
		status_push = 0;
}

else if (!strcmp(push_name, "TIPP_WIPE"))
status_push = data[3];

else  				   
	{
	sprintf (msg, "Function not exist : %s!", push_name);
	//MessagePopup ("MFL Function", msg);
	status_push = 0;
	}
return status_push;
//return status_push? 0 : 1;
}

double Get_ADC_value (uint8_t data[8], char *push_name)
{
double value;

	if (!strcmp(push_name, "L_BOTTOM_LEFT"))
		{
		value = data[4] * 0.3922;
		return value;
		}
	else if (!strcmp(push_name, "L_BOTTOM_MID"))
		{
		value = data[5] * 0.3922;
		return value;
		}
	else if (!strcmp(push_name, "L_BOTTOM_RIGHT"))
		{
		value = data[4] * 0.3922;
		return value;
		}
	else if (!strcmp(push_name, "L_MID_LEFT"))
		{
		value = data[2] * 0.3922;
		return value;
		}
	else if (!strcmp(push_name, "L_MID_RIGHT"))
		{
		value = data[2] * 0.3922;
		return value;
		}
	
	else if (!strcmp(push_name, "L_TOP_LEFT"))
		{
		value = data[1] * 0.3922;
		return value;
		}
	else if (!strcmp(push_name, "L_TOP_RIGHT"))
		{
		value = data[1] * 0.3922;
		return value;
		}
	
	else if (!strcmp(push_name, "TIP_WHEEL"))
		{
		value = data[3] * 0.3922;
		return value;
		}

return 0.0;
}
	
double getMaxDoubleArray(double *Array)
{  
	int i=0;
	double Max=0.0;

	 
	while(i<BUFFERLEN)
	{
	    if(Array[i]>Max) Max=Array[i];
		i++;
	}
	return Max;
}

double getMinDoubleArray(double *Array)
{  
	int i=0;
	double Min=0.0;

	 
	while(i<BUFFERLEN)
	{
	    if(Array[i]<Min) Min=Array[i];
		i++;
	}
	return Min;
}

double getMaxDoubleArray2(double *Array)
{  
	int i=0;
	double Max=0.0;

	 
	while(i<BUFFERLEN2)
	{
	    if(Array[i]>Max) Max=Array[i];
		i++;
	}
	return Max;
}

double getMinDoubleArray2(double *Array)
{  
	int i=0;
	double Min=0.0;

	 
	while(i<BUFFERLEN2)
	{
	    if(Array[i]<Min) Min=Array[i];
		i++;
	}
	return Min;
}

double parGetFSchwell(void)
{
	return 1.0;
}

void SaveGraphPNG(int PanelHandle, int ControleHandle, char *PathNamePNG)
{
	int BitmapId;
	
	GetPanelDisplayBitmap (PanelHandle, VAL_FULL_PANEL, VAL_ENTIRE_OBJECT, &BitmapId);
	SaveBitmapToPNGFile (BitmapId, PathNamePNG); 
	DiscardBitmap (BitmapId);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "MathFunction30"
void MathFunction30(double sdWaveGes[MESS_BUF][BUFFERLEN],
				   int 	  length,
				   double *_dmF1V, double *_dmS1V, 
				   double *_dmF2V, double *_dmS2V, 
				   double *_dmF_END, double *_dmS_END, 
				   double *_dmF1R, double *_dmS1R, 
				   double *_dmF2R, double *_dmS2R)
{
	int iErrorCase = 0;
	int iError     = 0;
	
	int state;
			  
	TeQuadrand eQuadrant, eQuadrantNew;   
	
	int iIndex;
	int iCnt;
	double dAktKraft, dAktWeg, dLetztWeg, dWegNP;
	double dMaxS1, dMinS1, dMitS1, dMaxS2, dMinS2, dMitS2;
	double w1, w2;
	int idiff;
	double *pdKraft, *pdWeg;
	double dKraftLevel = 32;

	static int    imPosStart;

	static double dmF1V, dmS1V;
	static int    imPosF1V;

	static double dmF2V, dmS2V;
	static int    imPosF2V;

	static double dmF_END, dmS_END;
	static int    imPosF_END;

	static double dmF1R, dmS1R;
	static int    imPosF1R;

	static double dmF2R, dmS2R;
	static int    imPosF2R;
	
	static double dmS_REV;

	char buff[128];

		
	pdKraft	= sdWaveGes[MESS_KRAFT];
	pdWeg	= sdWaveGes[MESS_WEG];
	
	// Initialisation des variable
	state = SUCHE_ANSTIEG;
	
	iIndex = 0;
	dmF1V = 0;
	dmS1V = 0;
	dmF2V = 0;
	dmS2V = 0;
	dmF1R = 0;  
	dmS1R = 0;
	dmF2R = 0;
	dmS1R = 0;
	dmF_END	= 0.0;
	dmS_END = 0.0;
	dWegNP = 0.0;
	
		  
    dKraftLevel = getMaxDoubleArray(sdWaveGes[MESS_KRAFT]);
	
	if(parGetFSchwell() <= 100)
	{
		//dKraftLevel *= parGetFSchwell();	//facteur de gonflement = 1 actuellement
		dKraftLevel *=2;
		dKraftLevel /= 100;
	}
	
	
	for(iCnt = 0; 
		(iCnt < length) && (state != SUCHE_ENDE); 
		iCnt++)
	{
		
		dAktKraft = pdKraft[iCnt];
		dAktWeg   = pdWeg[iCnt];
		
		switch(state)
		{
			case SUCHE_ANSTIEG:
				if(dAktKraft < dKraftLevel)
				{
					dWegNP = dAktWeg;
				}
				if(  (dAktKraft > DELTA_KRAFT) 
				  && (dAktKraft > dKraftLevel))
				{
					dLetztWeg = dAktWeg;
					imPosStart = iCnt;
					dWegNP = dAktWeg;
					state  = SUCHE_F1V;
				}
				break;
				
			case SUCHE_F1V:
				// das Maximum suchen
				if(dAktKraft > dmF1V)
				{
					dmF1V  = dAktKraft;
					dmS1V = dAktWeg - dWegNP;

					dLetztWeg = dAktWeg;
					imPosF1V = iCnt;
				}
				// Ist das Maximum schon mehr als 0,2mm weg
				if(((dLetztWeg+DELTA_WEG) < dAktWeg) && ((dmF1V - dAktKraft) > DELTA_KRAFT2) && dAktKraft > DELTA_KRAFT3)
				{
					dmF2V = dmF1V;
					dLetztWeg = dAktWeg;
					state  = SUCHE_F2V;
				}
				break;
	
			case SUCHE_F2V:
				if(dAktKraft < dmF2V)
				{
					dmF2V  = dAktKraft;
					dmS2V = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					imPosF2V = iCnt;
				}
				// Ist das Minimum schon mehr als 0,2mm weg
				if(((dLetztWeg+DELTA_WEG) < dAktWeg) && ((dAktKraft - dmF2V) > DELTA_KRAFT2))
				{
					dLetztWeg = dAktWeg;
					dmF_END = dAktKraft;
					//iCnt = imPosStart;
					state  = SUCHE_END;
				}
				break;
				
			case SUCHE_END:
				if(dAktKraft < dmF1V+1.0)
				//if(dAktKraft > BET_MAX_KRAFT)
				{ // Fehler: nicht beide SAS betätigt
					dmF_END   = 0.0;
					dmS_END   = 0.0;
					imPosF_END = iCnt;
					dmF2R = dAktKraft;
				}
				else 
				{
					dmF_END   = dAktKraft;
					dmS_END   = dAktWeg - dWegNP;
					dmF2R = dAktKraft;
					state = SUCHE_REV;
				}
				break;
	
	        // Suche Minimum bei Rücklauf
			case SUCHE_F2R:
				if(dAktKraft < dmF2R)
				{
					dmF2R  = dAktKraft;
					dmS2R  = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					imPosF2R = iCnt;
				}
				if(dAktKraft > dmF2R + DELTA_KRAFT)
				{
					dLetztWeg = dAktWeg;
					dmF1R = 0.0;
					state  = SUCHE_F1R;
				}
				break;
				
			// Suche Maximum bei Rücklauf
			case SUCHE_F1R:
				if(dAktKraft > dmF1R)
				{
					dmF1R  = dAktKraft;
					dmS1R  = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					imPosF1R = iCnt;
				}
				if(dAktWeg < (dLetztWeg-DELTA_WEG))
				{
					state = SUCHE_ENDE;
				}
				break;
				
			default:
				break;
	
		}
	
	}

for(iCnt = 1; (iCnt < length); iCnt++)
	{
	pdWeg[iCnt] -= dWegNP;
	}

	
	*_dmF1V = dmF1V;
	*_dmS1V = dmS1V;
	*_dmF2V = dmF2V;
	*_dmS2V = dmS2V;
	*_dmF1R = dmF1R;  
	*_dmS1R = dmS1R;
	*_dmF2R = dmF2R;
	*_dmS2R = dmS2R;
	*_dmF_END = dmF_END;
	*_dmS_END = dmS_END;
	
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "AnalysisFunction"
void AnalysisFunction(double sdWaveGes[MESS_BUF][BUFFERLEN],
				   		  int 	  length,
				   		  double *_dmF1V, double *_dmS1V, 
				   		  double *_dmF2V, double *_dmS2V, 
				   		  double *_dmF_END, double *_dmS_END, 
				   		  double *_dmF1R, double *_dmS1R, 
				   		  double *_dmF2R, double *_dmS2R)
{
	int num;
	int iCmpt;
	int iValleys;
	int iPeaks;
	
	int iVal=0;
	double *pdKraft, *pdWeg;

	
	double F1;
	double S1;
	double F2;
	double S2;
	
	double F1_First=0;
	double S1_First=0;
	double F2_First=10;
	double S2_First=0;
	int    iF1_First=0;
	int    iF2_First=0;
	
	double *_F1;
	double *_S1;
	double *_F2;
	double *_S2;
		   
	double dS1=0.0;
	double dS2=0.0;
	
	 *_dmF1V = 0;
	 *_dmS1V = 0;
	 *_dmF2V = 0;
	 *_dmS2V = 0;
	 
	double minF2=18.0;
	double minS2;
	int iCmptF;

	
		 _F1=(double*)calloc(length,sizeof(double));
		 _S1=(double*)calloc(length,sizeof(double));
	     _F2 =(double*)calloc(length,sizeof(double));
		 _S2=(double*)calloc(length,sizeof(double));
		
		
		
		pdKraft	= sdWaveGes[MESS_KRAFT];
		pdWeg	= sdWaveGes[MESS_WEG];
	
		iVal=0; 
	
		F1_First=0; 
		S1_First=0; 
		F2_First=10;
		S2_First=0; 
		iF1_First=0;
		iF2_First=0;
		dS1=0.0;
		dS2=0.0;
		
		for(iCmpt=0;iCmpt<length;iCmpt++)
		{

			 if(pdKraft[iCmpt]>F1_First)
			 {
				 F1_First=pdKraft[iCmpt];
				_F1[iF1_First]=F1_First;
				_S1[iF1_First]=pdWeg[iCmpt];
				iF1_First++;
				//_F1=realloc(_F1,(iF1_First+1)*sizeof(double));
				//_S1=realloc(_S1,(iF1_First+1)*sizeof(double));
				//_F1[iF1_First]=0.0;
				//_S1[iF1_First]=0.0;
				
				if(iF1_First>2)
				{
					   if((_S1[iF1_First-1]-_S1[iF1_First-2])>dS1&&_F1[iF1_First-1]>2.5&&_F1[iF1_First-1]<8.5 &&_S1[iF1_First-1]>0.2)
			           {
				            dS1=_S1[iF1_First-1]-_S1[iF1_First-2];
				
							*_dmF1V=_F1[iF1_First-2];
							*_dmS1V=_S1[iF1_First-2];
				            iVal=iCmpt;
				       }
					   
		    	}
				
	  	    }
		
			 if(pdKraft[length-iCmpt-1]<F2_First) 
			 {
				 F2_First=pdKraft[length-iCmpt-1];
				_F2[iF2_First]=F2_First;
				_S2[iF2_First]=pdWeg[length-iCmpt-1];
				iF2_First++;
				//_F2=realloc(_F2,(iF2_First+1)*sizeof(double));
				//_S2=realloc(_S2,(iF2_First+1)*sizeof(double));
				//_F2[iF2_First]=0.0;
				//_S2[iF2_First]=0.0;
			
				if(iF2_First>2)
				{	  if(minF2>_F2[iF2_First-2]) {minF2=_F2[iF2_First-2];minS2=_S2[iF2_First-2];}
				
					   if((_S2[iF2_First-2]-_S2[iF2_First-1])>dS2&&_F2[iF2_First-2]>1.0&&_F2[iF2_First-2]<7.5&&_S2[iF2_First-2]>*_dmS1V&&_S2[iF2_First-2]<1)
						{
							dS2=_S2[iF2_First-2]-_S2[iF2_First-1];
							*_dmF2V=_F2[iF2_First-2];
							*_dmS2V=_S2[iF2_First-2];
			            }
					   
		    	}
				
			}
			 if(pdKraft[0]>minF2 &&fabs(pdWeg[0]-minS2)>0.1 && pdKraft[0]>1.4)
			 {
				 *_dmF2V=minF2;	
		         *_dmS2V=minS2;
			 }
		}
		
		
		if (*_dmF2V > *_dmF1V && *_dmF2V > 0)
		{
		*_dmF2V= _F2[iF2_First];	
		*_dmS2V = _S2[iF2_First];	
		}
			
		
		*_dmF_END=*_dmF1V+1;
		for(iCmpt=iVal;iCmpt<length;iCmpt++)
		{
			  if(pdKraft[iCmpt]==*_dmF_END)
				{
					 *_dmS_END=pdWeg[iCmpt];
					 break;
				}
				else if(pdKraft[iCmpt]>*_dmF_END)
				{
					 *_dmS_END=(pdWeg[iCmpt-1]+pdWeg[iCmpt])/2;
					 break;
				}
		}
		free(_F1 );
		free(_S1 );
			    
		free(_F2 );
		free(_S2 );
	
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "Moyenne_Glissante_Courbe_30"
void Moyenne_Glissante_Courbe_30(double sdWaveGes[BUFFERLEN],int length)
{

	static double New_Tab_Force[BUFFERLEN];
	static double tab_Step[MOYENNE];
	int i,j;
	double Moyenne;
	
	
	for (i=0; i<length-MOYENNE;i++)
	{
		for (j=0; j<MOYENNE; j++)	
		  	tab_Step[j] = sdWaveGes[i+j]; 
	
		Mean (tab_Step, MOYENNE, &Moyenne);

	New_Tab_Force[i] = Moyenne;
		
	}
	
for (i=0; i<length-MOYENNE;i++)
	sdWaveGes[i] = New_Tab_Force[i];

}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "MathFunction40"
void MathFunction40(double sdWaveGes[MESS_BUF][BUFFERLEN],
				   int 	  length,
				   double *_dmF1V, double *_dmS1V, 
				   double *_dmF2V, double *_dmS2V, 
				   double *_dmF_END, double *_dmS_END, 
				   double *_dmF1R, double *_dmS1R, 
				   double *_dmF2R, double *_dmS2R)
{
	int iErrorCase = 0;
	int iError     = 0;
	
	int state;
			  
	TeQuadrand eQuadrant, eQuadrantNew;   
	
	int iIndex;
	int iCnt;
	double dAktKraft, dAktWeg, dLetztWeg, dWegNP;
	double dMaxS1, dMinS1, dMitS1, dMaxS2, dMinS2, dMitS2;
	double w1, w2;
	int idiff;
	double *pdKraft, *pdWeg;
	double dKraftLevel = 32;

	static int    imPosStart;

	static double dmF1V, dmS1V;
	static int    imPosF1V;

	static double dmF2V, dmS2V;
	static int    imPosF2V;

	static double dmF_END, dmS_END;
	static int    imPosF_END;

	static double dmF1R, dmS1R;
	static int    imPosF1R;

	static double dmF2R, dmS2R;
	static int    imPosF2R;
	
	static double dmS_REV;

	char buff[128];

		
	pdKraft	= sdWaveGes[MESS_KRAFT];
	pdWeg	= sdWaveGes[MESS_WEG];
	
	// Initialisation des variable
	state = SUCHE_ANSTIEG;
	
	iIndex = 0;
	dmF1V = 0;
	dmS1V = 0;
	dmF2V = 0;
	dmS2V = 0;
	dmF1R = 0;  
	dmS1R = 0;
	dmF2R = 0;
	dmS1R = 0;
	dmF_END	= 0.0;
	dmS_END = 0.0;
	dWegNP = 0.0;
	
		  
    dKraftLevel = getMaxDoubleArray(sdWaveGes[MESS_KRAFT]);
	
	if(parGetFSchwell() <= 100)
	{
		//dKraftLevel *= parGetFSchwell();	//facteur de gonflement = 1 actuellement
		dKraftLevel *=2;
		dKraftLevel /= 100;
	}
	
	
	for(iCnt = 0; 
		(iCnt < length) && (state != SUCHE_ENDE); 
		iCnt++)
	{
		
		dAktKraft = pdKraft[iCnt];
		dAktWeg   = pdWeg[iCnt];
		
		switch(state)
		{
			case SUCHE_ANSTIEG:
				if(dAktKraft < dKraftLevel)
				{
					dWegNP = dAktWeg;
				}
				if(  (dAktKraft > DELTA_KRAFT) 
				  && (dAktKraft > dKraftLevel))
				{
					dLetztWeg = dAktWeg;
					imPosStart = iCnt;
					dWegNP = dAktWeg;
					state  = SUCHE_F1V;
				}
				break;
				
			case SUCHE_F1V:
				// das Maximum suchen
				if(dAktKraft > dmF1V)
				{
					dmF1V  = dAktKraft;
					dmS1V = dAktWeg - dWegNP;

					dLetztWeg = dAktWeg;
					imPosF1V = iCnt;
				}
				// Ist das Maximum schon mehr als 0,2mm weg
				if(((dLetztWeg+DELTA_WEG) < dAktWeg) && ((dmF1V - dAktKraft) > DELTA_KRAFT2) && dAktKraft > DELTA_KRAFT3)
				{
					dmF2V = dmF1V;
					dLetztWeg = dAktWeg;
					state  = SUCHE_F2V;
				}
				break;
	
			case SUCHE_F2V:
				if(dAktKraft < dmF2V)
				{
					dmF2V  = dAktKraft;
					dmS2V = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					imPosF2V = iCnt;
				}
				// Ist das Minimum schon mehr als 0,2mm weg
				if(((dLetztWeg+DELTA_WEG) < dAktWeg) && ((dAktKraft - dmF2V) > DELTA_KRAFT2))
				{
					dLetztWeg = dAktWeg;
					dmF_END = dAktKraft;
					//iCnt = imPosStart;
					state  = SUCHE_END;
					
				}
				break;
	
			case SUCHE_END:
				if(dAktKraft < dmF1V+1.0)
				//if(dAktKraft > BET_MAX_KRAFT)
				{ // Fehler: nicht beide SAS betätigt
					dmF_END   = 0.0;
					dmS_END   = 0.0;
					imPosF_END = iCnt;
					dmF2R = dAktKraft;
				}
				else 
				{
					dmF_END   = dAktKraft;
					dmS_END   = dAktWeg - dWegNP;
					dmF2R = dAktKraft;
					state = SUCHE_REV;
				}
				break;
				
			case SUCHE_REV:
				if(dAktKraft < (dmF2R - DELTA_KRAFT))
				{
					dmF2R  = dAktKraft;
					dmS2R  = dAktWeg - dWegNP;
					state = SUCHE_F2R;
				}
				break;
	
	        // Suche Minimum bei Rücklauf
			case SUCHE_F2R:
				if(dAktKraft < dmF2R)
				{
					dmF2R  = dAktKraft;
					dmS2R  = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					imPosF2R = iCnt;
				}
				if(dAktKraft > dmF2R + DELTA_KRAFT)
				{
					dLetztWeg = dAktWeg;
					dmF1R = 0.0;
					state  = SUCHE_F1R;
				}
				break;
				
			// Suche Maximum bei Rücklauf
			case SUCHE_F1R:
				if(dAktKraft > dmF1R)
				{
					dmF1R  = dAktKraft;
					dmS1R  = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					imPosF1R = iCnt;
				}
				if(dAktWeg < (dLetztWeg-DELTA_WEG))
				{
					state = SUCHE_ENDE;
				}
				break;
				
			default:
				break;
	
		}
	
	}

for(iCnt = 1; (iCnt < length); iCnt++)
	{
	pdWeg[iCnt] -= dWegNP;
	}

	
	*_dmF1V = dmF1V;
	*_dmS1V = dmS1V;
	*_dmF2V = dmF2V;
	*_dmS2V = dmS2V;
	*_dmF1R = dmF1R;  
	*_dmS1R = dmS1R;
	*_dmF2R = dmF2R;
	*_dmS2R = dmS2R;
	*_dmF_END = dmF_END;
	*_dmS_END = dmS_END;
	
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "Moyenne_Glissante_Courbe_40"
void Moyenne_Glissante_Courbe_40(double sdWaveGes[BUFFERLEN],int length)
{

	static double New_Tab_Force[BUFFERLEN];
	static double tab_Step[MOYENNE];
	int i,j;
	double Moyenne;
	
	
	for (i=0; i<length-MOYENNE;i++)
	{
		for (j=0; j<MOYENNE; j++)	
		  	tab_Step[j] = sdWaveGes[i+j]; 
	
		Mean (tab_Step, MOYENNE, &Moyenne);

	New_Tab_Force[i] = Moyenne;
		
	}
	
for (i=0; i<length-MOYENNE;i++)
	sdWaveGes[i] = New_Tab_Force[i];

}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "MathFunction2"
void MathFunction2(double sdWaveGes[MESS_BUF2][BUFFERLEN2],
				   int 	  length,
				   double *_dmF1V, double *_dmS1V, 
				   double *_dmF2V, double *_dmS2V, 
				   double *_dmF3V, double *_dmS3V, 
				   double *_dmF4V, double *_dmS4V,
				   double *_dmF5V, double *_dmS5V)
{
	int iErrorCase = 0;
	int iError     = 0;
	
	int state;
			  
	TeQuadrand eQuadrant, eQuadrantNew;   
	
	int iIndex;
	int iCnt;
	double dAktKraft, dAktWeg, dLetztWeg, dWegNP;
	double dMaxS1, dMinS1, dMitS1, dMaxS2, dMinS2, dMitS2;
	double w1, w2;
	int idiff;
	double *pdKraft, *pdWeg;
	double dKraftLevel = 32;

	static int    imPosStart;

	static double dmF1V, dmS1V;
	static int    imPosF1V;

	static double dmF2V, dmS2V;
	static int    imPosF2V;

	static double dmF3V, dmS3V;
	static int    imPosF3V;

	static double dmF4V, dmS4V;
	static int    imPosF4V;

	static double dmF5V, dmS5V;
	static int    imPosF5V;
	
	static double dmF_END, dmS_END;
	static int    imPosF_END;

	char buff[128];

	static IIRFilterPtr BessPtr;
    static double  *FiltData_Force;
	static double Freq_Basse_Filtre = 100;
	static double Freq_Filtre = 800;
	static double Freq_Haute_Filtre = 500;
	static int 	  Ordre_Filtre = 3;
	int i;
	
	//Filtre RII
	FiltData_Force = malloc(length*sizeof(double));
	
	/* Apply Bessel filter for force waveform */
    BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    IIRCascadeFiltering (sdWaveGes[MESS_TORQUE], length, BessPtr, FiltData_Force);
	FreeIIRFilterPtr (BessPtr);

	for(i=0;i<length;i++)
		sdWaveGes[MESS_TORQUE][i] = FiltData_Force[i]; 
	
	pdKraft	= sdWaveGes[MESS_TORQUE];
	pdWeg	= sdWaveGes[MESS_ANGLE];
	
	// Initialisation des variable
	state = SUCHE_INIT;
	
	iIndex = 0;
	dmF1V = 0;
	dmS1V = 0;
	dmF2V = 0;
	dmS2V = 0;
	dmF3V = 0;  
	dmS3V = 0;
	dmF4V = 0;
	dmS4V = 0;
	dmF5V = 0;
	dmS5V = 0;
	dmF_END	= 0.0;
	dmS_END = 0.0;
	dWegNP = 0.0;
	
	
    dKraftLevel = getMaxDoubleArray2(sdWaveGes[MESS_TORQUE]);
	
	if(parGetFSchwell() <= 100)
	{
		//dKraftLevel *= parGetFSchwell();	//facteur de gonflement = 1 actuellement
		dKraftLevel *=2;
		dKraftLevel /= 100;
	}
	
	for(iCnt = 0; 
		(iCnt < length) && (state != SUCHE_FIN); 
		iCnt++)
	{
		
		dAktKraft = pdKraft[iCnt];
		dAktWeg   = pdWeg[iCnt];
		
		switch(state)
		{
			case SUCHE_INIT:
				if(dAktKraft < dKraftLevel)
				{
					dWegNP = dAktWeg;
				}
				if(  (dAktKraft > DELTA_TORQUE) 
				  && (dAktKraft > dKraftLevel))
				{
					dLetztWeg = dAktWeg;
					imPosStart = iCnt;
					state  = SUCHE_F1;
				}
				break;
				
			case SUCHE_F1:
				// das Maximum suchen
				if(dAktKraft > dmF1V)
				{
					dmF1V  = dAktKraft;
					dmS1V = dAktWeg - dWegNP;

					dLetztWeg = dAktWeg;
					imPosF1V = iCnt;
				}
				// Ist das Maximum schon mehr als 0,2mm weg
				if(((dLetztWeg+DELTA_ANGLE) < dAktWeg) && ((dmF1V - dAktKraft) > DELTA_TORQUE2))
				{
					dmF2V = dmF1V;
					dLetztWeg = dAktWeg;
					state  = SUCHE_F2;
				}
				break;
	
			case SUCHE_F2:
				if(dAktKraft < dmF2V)
				{
					dmF2V  = dAktKraft;
					dmS2V = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					imPosF2V = iCnt;
				}
				// Ist das Minimum schon mehr als 0,2mm weg
				if(((dLetztWeg+DELTA_ANGLE) < dAktWeg) && ((dAktKraft - dmF2V) > DELTA_TORQUE2))
				{
					dLetztWeg = dAktWeg;
					dmF_END = dAktKraft;
					//iCnt = imPosStart;
					state  = SUCHE_F3;
					
				}
				break;
				
			case SUCHE_F3:
				// das Maximum suchen
				if(dAktKraft > dmF3V)
				{
					dmF3V  = dAktKraft;
					dmS3V = dAktWeg - dWegNP;

					dLetztWeg = dAktWeg;
					imPosF3V = iCnt;      //  imPosF1V
				}
				// Ist das Maximum schon mehr als 0,2mm weg
				if(((dLetztWeg+DELTA_ANGLE) < dAktWeg))
				{
					dmF4V = dmF3V;
					dLetztWeg = dAktWeg;
					state  = SUCHE_F4;
				}
				break;
	
			case SUCHE_F4:
				if(dAktKraft < dmF4V)
				{
					dmF4V  = dAktKraft;
					dmS4V = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					imPosF4V = iCnt;	//  imPosF2V
				}
				// Ist das Minimum schon mehr als 0,2mm weg
				if((dLetztWeg+DELTA_ANGLE) < dAktWeg)
				{
					dLetztWeg = dAktWeg;
					dmF_END = dAktKraft;
					//iCnt = imPosStart;
					state  = SUCHE_F5;
					
				}
				break;
				
				case SUCHE_F5:
				if(dAktKraft < dmF3V+0.013) //F5 = F3 + 1 => Force = Torque / 0.013 (conversion Nm to N) ==> 1.0 N = Torque / 0.013  => 1.0 N =  0.013 Nm
				{
					dmF5V  = 0;
					dmS5V = 0;
					dLetztWeg = dAktWeg;
					imPosF5V = iCnt;	   //  imPosF1V
				}
				// Ist das Minimum schon mehr als 0,2mm weg
				else 
				{
					dmF5V  = dAktKraft;
					dmS5V = dAktWeg - dWegNP;
					dLetztWeg = dAktWeg;
					dmF_END = dAktKraft;
					//iCnt = imPosStart;
					state  = SUCHE_FIN;
					
				}
				break;
				
			default:
				break;
	
		}
	
	}

for(iCnt = 1; (iCnt < length); iCnt++)
	{
	pdWeg[iCnt] -= dWegNP;
	}

	
	*_dmF1V = dmF1V;
	*_dmS1V = dmS1V;
	*_dmF2V = dmF2V;
	*_dmS2V = dmS2V;
	*_dmF3V = dmF3V;  
	*_dmS3V = dmS3V;
	*_dmF4V = dmF4V;
	*_dmS4V = dmS4V;
	*_dmF5V = dmF5V;
	*_dmS5V = dmS5V;
}

void Filter_Table (double *data, int size)
{
	int i;
	static IIRFilterPtr BessPtr;
    static double  *FiltData;
	static double Freq_Basse_Filtre = 400;
	static double Freq_Filtre = 800;
	static double Freq_Haute_Filtre = 1000;
	static int 	  Ordre_Filtre = 2;
	
	//Filtre RII
	FiltData = malloc(size*sizeof(double));
			
	/* Apply Bessel filter for force waveform */
    BessPtr = AllocIIRFilterPtr (LOWPASS, Ordre_Filtre);
    Bessel_CascadeCoef (Freq_Filtre, Freq_Basse_Filtre, Freq_Haute_Filtre, BessPtr);
    IIRCascadeFiltering (data, size, BessPtr, FiltData);
	FreeIIRFilterPtr (BessPtr);
 
	for (i=0; i<size; i++)
	{
	data[i] = FiltData[i];	
	}
}


/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Rodage_Wheel_Right"
DWORD WINAPI Thread_Rodage_Wheel_Right(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	SRodageThreadDataPtr		pthread_data = (SRodageThreadDataPtr)param;
	SmotorPtr					pmotor = pthread_data->pmotor;
	uint32_t					timeout = pthread_data->timeout;
	double						angle = pthread_data->angle;
	double						speed = pthread_data->speed;
	uint32_t					count = pthread_data->count;
	clock_t 					_start_test_clock;
	double   					time;
	int 						i;
	double   					Currentangle;
	double   					angle_reduction;


	for (i=0; i<count; i++)
	{
	/* angle initialisation */
	pmotor->AlarmReset(pmotor, 0);
	pmotor->AlarmReset(pmotor, 0);
	pmotor->AlarmReset(pmotor, 0);
	
	/* wait initialisation */
	Sleep (500);
	
	/* move clockwise  */
	angle_reduction = angle / 1.0;  //4.5 
	EXCCHECK( pmotor->MoveMotorPosition(pmotor, 0, angle_reduction, speed)); 
	
	/* wait until rotation finished */
	_start_test_clock =  clock();
	do{
	pmotor->CurrentPosition(pmotor, AXIS_Z, &Currentangle);
	//read status I/O
	time = (double)(clock()-_start_test_clock);	
	}while (fabs(Currentangle) <= (fabs(angle_reduction) - 0.2) && time < timeout);
	
	if (time >= timeout)	
	{
		//printf("Motor rotation Timeout clockwise Currentangle = %f | angle = %f\r\n", Currentangle, angle_reduction);
		EXCTHROW(-1, "Motor rotation Timeout");
	}
	
	/* wait initialisation */
	Sleep (500);
	
	/* angle initialisation */
	pmotor->AlarmReset(pmotor, 0);
	pmotor->AlarmReset(pmotor, 0);
	pmotor->AlarmReset(pmotor, 0);

	
	/* move to counterclockwise  */
	angle_reduction = -1 * (angle / 1.0); //4.5
	EXCCHECK( pmotor->MoveMotorPosition(pmotor, 0, angle_reduction, speed));

	/* wait until rotation finished */
	_start_test_clock =  clock();
	do{
	pmotor->CurrentPosition(pmotor, AXIS_Z, &Currentangle);
	//read status I/O
	time = (double)(clock()-_start_test_clock);	
	}while (fabs(Currentangle) <= (fabs(angle_reduction) - 0.2) && time < timeout);
	
	if (time >= timeout)	
	{
	//	printf("Thread_Rodage_Wheel_Right Currentangle = %f | angle = %f\r\n", Currentangle, angle_reduction);
		EXCTHROW(-1, "Motor rotation Timeout");
	}
	
	/* wait initialisation */
	Sleep (500);
	
	}

Error:
	free(pthread_data);
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Rodage_Wheel_Left"
DWORD WINAPI Thread_Rodage_Wheel_Left(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	SRodageThreadDataPtr		pthread_data = (SRodageThreadDataPtr)param;
	SmotorPtr					pmotor = pthread_data->pmotor;
	uint32_t					timeout = pthread_data->timeout;
	double						angle = pthread_data->angle;
	double						speed = pthread_data->speed;
	uint32_t					count = pthread_data->count;
	clock_t 					_start_test_clock;
	double   					time;
	int 						i;
	double   					Currentangle;


	
	/*Init Motor */
	pmotor->_InitializeSpec(pmotor);
	
	/*Reset Angle = 0°*/
	pmotor->AlarmReset(pmotor, 1);
	

	for (i=0; i<count; i++)
	{
	/* move CW  */
	EXCCHECK( pmotor->MoveMotorPosition(pmotor, 1, angle, speed)); 
	
		/* wait until rotation finished */
		_start_test_clock =  clock();
		do{
		pmotor->CurrentPosition(pmotor, AXIS_Z, &Currentangle);
		//read status I/O
		time = (double)(clock()-_start_test_clock);	
		}while (fabs(Currentangle) <= (fabs(angle)-0.2) && time < timeout);
	
		if (time >= timeout)	
		{
	//	printf("Motor rotation LEFT Timeout CW Currentangle = %f | angle = %f\r\n", Currentangle, angle);
		EXCTHROW(-1, "Motor rotation Timeout");
		}
	
	/* move CCW  */
	EXCCHECK( pmotor->MoveMotorPosition(pmotor, 1, angle*-1, speed));

		/* wait until rotation finished */
		_start_test_clock =  clock();
		do{
		pmotor->CurrentPosition(pmotor, AXIS_Z, &Currentangle);
		//read status I/O
		time = (double)(clock()-_start_test_clock);	
		}while (fabs(Currentangle) <= (fabs(angle)-0.2) && time < timeout);
	
		if (time >= timeout)	
		{
	//	printf("Motor rotation LEFT Timeout CCW Currentangle = %f | angle = %f\r\n", Currentangle, angle);
		EXCTHROW(-1, "Motor rotation Timeout");
		}
	
	}

Error:
	free(pthread_data);
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Reset_Sensor"
DWORD WINAPI Thread_Reset_Sensor(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	SRodageThreadDataPtr		pthread_data = (SRodageThreadDataPtr)param;
	SmotorPtr					pmotor = pthread_data->pmotor;
	int  	status;

	
	pmotor->ControllerStatus(pmotor, 3, &status);

Error:
	free(pthread_data);
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckDirectory"
SElExceptionPtr CheckDirectory(const char* directory)
{
	int32_t			   	error = 0;
	SElExceptionPtr    	pexception = NULL; 
	int					size;

	EXCCHECKCVI( FileExists (directory, &size));
	if(size==-1)
	{
		/* create directory */	
		EXCCHECKCVI( MakeDir(directory));
	}
	
Error:
	EXCRETHROW( pexception);
}

/*--------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Phase_Detection_Left"
DWORD WINAPI Thread_Phase_Detection_Left(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	STorqueThreadDataPtr		pthread_data = (STorqueThreadDataPtr)param;
	SaxisPtr					paxis = pthread_data->paxis;
	double                      value_Position=8.91;
	double                      time=0;
	clock_t 					_start_test_clock;  
	
	uint8_t	data[8];   


	/*Execute phase detection°*/
	

	
	
	
	
		paxis->CurrentPosition(paxis, 32, &value_Position);
	
	pthread_data->angle= value_Position; 
	
//	printf("angle_Tip= %f\n\r",pthread_data->angle );
	
//	printf("Position_Tip= %f\n\r",position_Tip );
	
	
	if(pthread_data->angle < 11 ||  pthread_data->angle> 13.5)
	{
		paxis->EnableAxis(paxis, 32);
	
	}


	

Error:
	
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}


/*--------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Phase_Detection_Right"
DWORD WINAPI Thread_Phase_Detection_Right(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	STorqueThreadDataPtr		pthread_data = (STorqueThreadDataPtr)param;
	SaxisPtr					paxis = pthread_data->paxis;
	double                      value_Position=8.91;
	double                      time=0;
	clock_t 					_start_test_clock; 
	
	uint8_t	data[8]; 


	/*Execute phase detection°*/
	
		paxis->CurrentPosition(paxis, 1, &value_Position);
	//	printf ("value_Position=%f\r\n",value_Position);
	
	pthread_data->angle= value_Position;
	
  //  printf("angle_Scroll= %f\n\r",pthread_data->angle ); 
	
//	printf("Position_Scroll= %f\n\r",position_scroll ); 
	
	if(pthread_data->angle < 49)
	{
		paxis->EnableAxis(paxis, 1);	  //51
//		Sleep(2000);
//        paxis->SetPosition(paxis, 1, 52.0, 25.5); 
		
//	Sleep(5000);
//	 paxis->CurrentPosition(paxis, 1, &value_Position);
	// printf ("value_Position_2=%f\r\n",value_Position);
	
	}

	

Error:
	
	if(pexception)
	{
		EXCDBGSTR(pexception);
		ExitThread(THREAD_ERROR_CODE);
	}
	else
		ExitThread(THREAD_EXIT_CODE);
return 0;
}
