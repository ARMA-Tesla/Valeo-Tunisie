#if !defined(__TEST_HELPER_FUNCTIONS_H__)
#define __TEST_HELPER_FUNCTIONS_H__

#include <exception/exception.h>
#include <objlist/objlist.h>


#define BUFFERLEN 		10000

#define DELTA_WEG		0.14
#define DELTA_KRAFT		0.44
#define DELTA_KRAFT2	0.7


#define BET_MAX_KRAFT	10
#define BET_GS_LIMIT	0.8
#define BET_SAS_LIMIT	2.5

#define COLOR_FORCE	VAL_BLUE
#define COLOR_CONTACT VAL_DK_RED
#define COLOR_CONTACT2 VAL_YELLOW
#define COLOR_MESSW VAL_DK_BLUE
#define COLOR_LEVEL	VAL_MAGENTA

unsigned int uiPosWaveGes;

enum
{
	MESS_KRAFT = 0,
	MESS_WEG,
	MESS_CONTACT,
	MESS_CONTACT2,
	MESS_TIME,
	MESS_BUF
};

typedef enum
{
	Q_I = 0,
	Q_II, 
	Q_III,
	Q_IV
}TeQuadrand;

enum
{
	SUCHE_ANSTIEG = 0,
	SUCHE_F1V,
	SUCHE_F2V,
	SUCHE_END,
	SUCHE_REV,
	SUCHE_F2R,
	SUCHE_F1R,
	SUCHE_ENDE
};

double getMinDoubleArray(double *Array);
double getMaxDoubleArray(double *Array);
void SaveGraphPNG(int PanelHandle, int ControleHandle, char *PathNamePNG);
void MathFunction(double sdWaveGes[MESS_BUF][BUFFERLEN],
				   int 	  length,
				   double *_dmF1V, double *_dmS1V, 
				   double *_dmF2V, double *_dmS2V, 
				   double *_dmF_END, double *_dmS_END, 
				   double *_dmF1R, double *_dmS1R, 
				   double *_dmF2R, double *_dmS2R);

double getMinDoubleArray2(double *Array);
double getMaxDoubleArray2(double *Array);
void SaveGraphPNG2(int PanelHandle, int ControleHandle, char *PathNamePNG);
void MathFunction2(double sdWaveGes[MESS_BUF][BUFFERLEN],
				   int 	  length,
				   double *_dmF1V, double *_dmS1V, 
				   double *_dmF2V, double *_dmS2V, 
				   double *_dmF_END, double *_dmS_END, 
				   double *_dmF1R, double *_dmS1R, 
				   double *_dmF2R, double *_dmS2R);

SElExceptionPtr Camera_RegisterTCPServer(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_login(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_disconnect(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_Trigger(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_Read(
	SObjListPtr		pModules,
	int				IndexCamera,
	char            data[8]
);
SElExceptionPtr Camera_Online(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_Offline(
	SObjListPtr		pModules,
	int				IndexCamera
);
SElExceptionPtr Camera_Set_Job(
	SObjListPtr		pModules,
	int				IndexCamera,
	char           *Job
);
SElExceptionPtr Camera_Set_Step(
	SObjListPtr		pModules,
	int				IndexCamera,
	int             Step
);
SElExceptionPtr PowerSetPower(
	SObjListPtr		pModules,
	const char*		xml_file,
	const char*		channel,
	double			voltage,
	double			current
);

SElExceptionPtr MeasGapStation50(
	SObjListPtr		pModules,
	double*			out0,
	double*			out1,
	double*			out2,
	double*			out3 
);
SElExceptionPtr MeasGapStation60(
	SObjListPtr		pModules,
	double*			out0,
	double*			out1,
	double*			out2,
	double*			out3 
);

#endif /* __TEST_HELPER_FUNCTIONS_H__ */
