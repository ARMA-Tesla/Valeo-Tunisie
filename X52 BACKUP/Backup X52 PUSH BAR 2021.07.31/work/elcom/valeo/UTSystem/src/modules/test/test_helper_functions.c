#include <windows.h>
#include <formatio.h>
#include "msxmldom.h"
#include "test_helper_functions.h"
#include <ivibridge/ivibridge.h>
#include <test/eltest.h>
#include <IviDcPwr.h>
#include <IviDmm.h>
#include <IviSwtch.h>
#include <guid.h>
#include <toolbox.h>
#include <utility.h>
#include <sdk/mmsystem.h>
#include <multithread/multithread.h>
#include <analysis.h>
#include <testseq.h> 
#include <export.h>

#pragma warning( push)
#pragma warning( disable: 4996)

#define MAX_LENGTH_CHANNEL_NAME		32
#define MAX_NB_OF_CHANNELS			16

/* 
 * BFFA2008  No explicit path exists between the two channels.
 * BFFA200C  The channels are explicitly connected.
 */
#define checkSwtch(fCall) \
	if (error = (fCall), (error = (error<0 && error!=0xBFFA2008 && error!=0xBFFA200C) ? error : VI_SUCCESS)) \
		{ goto Error; }  else error = error

typedef struct _SLogicNames
{
	char		name[MAX_LENGTH_CHANNEL_NAME+1];
	char		physical[MAX_LENGTH_CHANNEL_NAME+1];
	/* switching */
	bool_t		is_active;
	char		psw1[MAX_LENGTH_CHANNEL_NAME+1]; /* channel switch */
	char		psw2[MAX_LENGTH_CHANNEL_NAME+1];
	bool_t		is_low_current;
	char		pswc1[MAX_LENGTH_CHANNEL_NAME+1]; /* low current meas switch */
	char		pswc2[MAX_LENGTH_CHANNEL_NAME+1];
	/* calibration */
	char		scale1[MAX_LENGTH_CHANNEL_NAME+1]; /* hi curent */
	char		scale2[MAX_LENGTH_CHANNEL_NAME+1]; /* low current */
} SLogicNames, *SLogicNamesPtr;

typedef struct _SPowerConfig
{
	SLogicNames		channels[MAX_NB_OF_CHANNELS];
	int32_t			channelsSize;
	char			calibration_file[MAX_LENGTH_CHANNEL_NAME+1];
} SPowerConfig, *SPowerConfigPtr;

typedef enum _ESwitchFnc
{
	ESF_CONNECT = 0,
	ESF_DISCONNECT,
	ESF_DISCONNECT_ALL,
	ESF_CHECK_PATH,
	ESF_SLEEP
} ESwitchFnc;

static SPowerConfig		gs_PowerConfig = {0,0,0};
static DWORD WINAPI Thread_DAQ(LPVOID param);

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadConfiguration"
static SElExceptionPtr LoadConfiguration(
   const char*   fileName,
   CAObjHandle	*xml
)
{
	SElExceptionPtr pexception = NULL;
	static CAObjHandle	pxml  = 0;

	if(pxml==0)
	{
		int32_t			error = 0;
		HRESULT         hr = 0;
		const char*     pfile_name = (fileName) ? fileName : "\\test.xml";
		VBOOL         	is_xml_success;
		VARIANT         pfile_name_var = CA_VariantEmpty();
		char			path[MAX_PATHNAME_LEN];

		EXCCHECKCVI( GetModuleDir (__CVIUserHInst, path)); 
		strcat(path, pfile_name);
		
		EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
		EXCCHECKCVI( CA_VariantSetCString(&pfile_name_var, path));
		EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success)); 
		EXCCHECKCVI( CA_VariantClear(&pfile_name_var));
	}
	
Error:
	if(pexception && pxml)
	{
		CA_DiscardObjHandle(pxml);
		pxml = 0;
	}
	if(xml) *xml = pxml;
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadSourceConfiguration"
static SElExceptionPtr LoadSourceConfiguration(
   const char*   fileName
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     plist = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;
	char*         	ptext= NULL;

	EXCCHECK( LoadConfiguration(fileName, &pxml));  
		
	if(pxml)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//calibration_file", &pparameter));
		if(pparameter!=0)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				strncpy(gs_PowerConfig.calibration_file, ptext, MAX_LENGTH_CHANNEL_NAME);	 
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}
		else
			EXCTHROW(-1, "Calibration file not specified!");

		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes (pxml, NULL, "//power/channel", &plist));   
		if(plist)
		{
			SLogicNamesPtr	plogic_names = NULL;
			long			length = 0;	
			int				i;
		
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
			
			if(length==0)
				EXCTHROW(-1, "Source channels are not specified in a config xml!");

			gs_PowerConfig.channelsSize = length;
			plogic_names = gs_PowerConfig.channels;
		
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));
			
			for(i=0; i<length; i++, plogic_names++)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pnode));
				if(pnode)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@logical", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						if(ptext)
						{
							strncpy(plogic_names->name, ptext, MAX_LENGTH_CHANNEL_NAME);	 
							CA_FreeMemory(ptext);
							ptext=NULL;
						}
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@physical", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						if(ptext)
						{
							strncpy(plogic_names->physical, ptext, MAX_LENGTH_CHANNEL_NAME);	 
							CA_FreeMemory(ptext);
							ptext=NULL;
						}
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@psw1", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						if(ptext)
						{
							strncpy(plogic_names->psw1, ptext, MAX_LENGTH_CHANNEL_NAME);	 
							CA_FreeMemory(ptext);
							ptext=NULL;
						}
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@psw2", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						if(ptext)
						{
							strncpy(plogic_names->psw2, ptext, MAX_LENGTH_CHANNEL_NAME);	 
							CA_FreeMemory(ptext);
							ptext=NULL;
						}
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@pswc1", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						if(ptext)
						{
							strncpy(plogic_names->pswc1, ptext, MAX_LENGTH_CHANNEL_NAME);	 
							CA_FreeMemory(ptext);
							ptext=NULL;
						}
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@pswc2", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						if(ptext)
						{
							strncpy(plogic_names->pswc2, ptext, MAX_LENGTH_CHANNEL_NAME);	 
							CA_FreeMemory(ptext);
							ptext=NULL;
						}
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@scale1", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						if(ptext)
						{
							strncpy(plogic_names->scale1, ptext, MAX_LENGTH_CHANNEL_NAME);	 
							CA_FreeMemory(ptext);
							ptext=NULL;
						}
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}

					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "@scale2", &pparameter));
					if(pparameter)
					{
						EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
						if(ptext)
						{
							strncpy(plogic_names->scale2, ptext, MAX_LENGTH_CHANNEL_NAME);	 
							CA_FreeMemory(ptext);
							ptext=NULL;
						}
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
					}
				}
			}
		}
	}

Error:
	if (pparameter) CA_DiscardObjHandle(pparameter);    
	if (pnode) CA_DiscardObjHandle(pnode);
	if (plist) CA_DiscardObjHandle(plist);
	EXCRETHROW( pexception);
}
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PowerSetPower"
SElExceptionPtr PowerSetPower(
	SObjListPtr		pModules,
	const char*		xml_file,
	const char*		channel,
	double			voltage,
	double			current
)
{
	ViStatus		error = VI_SUCCESS;
	SElExceptionPtr pexception = NULL;
	ViSession		vi_POWER = 0;
	SIviBridgePtr	pIvi = (SIviBridgePtr)pModules->Get(pModules, MODULE_IVIBRIDGE);
	
	EXCCHECK( pIvi->GetSession( pIvi, "POWER", &vi_POWER));
	
	/* source set */
	checkErr( IviDCPwr_ConfigureVoltageLevel(vi_POWER, "CHANNEL1", voltage));
	Sleep(200);
	checkErr( IviDCPwr_ConfigureCurrentLimit(vi_POWER, "CHANNEL1", IVIDCPWR_VAL_CURRENT_REGULATE, current));
	Sleep(200);
	checkErr( IviDCPwr_ConfigureOutputEnabled(vi_POWER, "CHANNEL1", VI_TRUE));

Error:
	IVI_CHECK_ERR(error);
	EXCRETHROW( pexception);
}
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "MeasGapStation50"
SElExceptionPtr MeasGapStation50(
	SObjListPtr		pModules,
	double*			out0,
	double*			out1,
	double*			out2,
	double*			out3 
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SProfilometersPtr pProfilometers = (SProfilometersPtr)pModules->Get(pModules, MODULE_PROFILOMETERS);
	SProfilometerPtr  pProfilometer = NULL;
	MEASURE_DATA 	  pMeasure[16];

	/* read from Profilometer */
	EXCCHECK( pProfilometers->GetProfilometerByIndex(pProfilometers, 0, &pProfilometer));
	//Sleep(20);
	EXCCHECK( pProfilometer->GetMeasurementValue(pProfilometer, pMeasure));

	/* Out 0 */
	if (pMeasure[0].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
		{
			*out0 = (double)pMeasure[0].fValue;
		}
	else
		*out0 = 0;
	
	/* Out 1 */
	if (pMeasure[1].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
		{
			*out1 = (double)pMeasure[1].fValue;
		}
	else
		*out1 = 0;
	
	/* Out 2 */
	if (pMeasure[2].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
		{
			*out2 = (double)pMeasure[2].fValue;
		}
	else
		*out2 = 0;
	
	/* Out 3 */
	if (pMeasure[3].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
		{
			*out3 = (double)pMeasure[3].fValue;
		}
	else
		*out3 = 0;
	

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "MeasGapStation60"
SElExceptionPtr MeasGapStation60(
	SObjListPtr		pModules,
	double*			out0,
	double*			out1, 
	double*			out2,
	double*			out3
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SProfilometersPtr pProfilometers = (SProfilometersPtr)pModules->Get(pModules, MODULE_PROFILOMETERS);
	SProfilometerPtr  pProfilometer = NULL;
	MEASURE_DATA 	  pMeasure[16];

	/* read from Profilometer */
	EXCCHECK( pProfilometers->GetProfilometerByIndex(pProfilometers, 1, &pProfilometer));
	//Sleep(20);
	EXCCHECK( pProfilometer->GetMeasurementValue(pProfilometer, pMeasure));

	/* Out 0 */
	if (pMeasure[0].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
		{
			*out0 = (double)pMeasure[0].fValue;
		}
	else
		*out0 = 0;
	
	/* Out 1 */
	if (pMeasure[1].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
		{
			*out1 = (double)pMeasure[1].fValue;
		}
	else
		*out1 = 0;
	
	/* Out 2 */
	if (pMeasure[2].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
		{
			*out2 = (double)pMeasure[2].fValue;
		}
	else
		*out2 = 0;
	
	/* Out 3 */
	if (pMeasure[3].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
		{
			*out3 = (double)pMeasure[3].fValue;
		}
	else
		*out3 = 0;
	

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Camera_RegisterTCPServer"
SElExceptionPtr Camera_RegisterTCPServer(
	SObjListPtr		pModules,
	int				IndexCamera
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, IndexCamera, &pCamera));
	EXCCHECK( pCamera->openTCPserver(pCamera));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Camera_login"
SElExceptionPtr Camera_login(
	SObjListPtr		pModules,
	int				IndexCamera
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, IndexCamera, &pCamera));
	EXCCHECK( pCamera->Cognex_login(pCamera));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Camera_disconnect"
SElExceptionPtr Camera_disconnect(
	SObjListPtr		pModules,
	int				IndexCamera
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, IndexCamera, &pCamera));
	EXCCHECK( pCamera->Cognex_disconnect(pCamera));

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "Camera_Trigger"
SElExceptionPtr Camera_Trigger(
	SObjListPtr		pModules,
	int				IndexCamera
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, IndexCamera, &pCamera));
	EXCCHECK( pCamera->Cognex_Trigger(pCamera));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Camera_Read"
SElExceptionPtr Camera_Read(
	SObjListPtr		pModules,
	int				IndexCamera,
	char            data[128]
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, 0, &pCamera));
	EXCCHECK( pCamera->Cognex_Read(pCamera, data));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Camera_Online"
SElExceptionPtr Camera_Online(
	SObjListPtr		pModules,
	int				IndexCamera
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, IndexCamera, &pCamera));
	EXCCHECK( pCamera->Cognex_Online(pCamera));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Camera_Offline"
SElExceptionPtr Camera_Offline(
	SObjListPtr		pModules,
	int				IndexCamera
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, IndexCamera, &pCamera));
	EXCCHECK( pCamera->Cognex_Offline(pCamera));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Camera_Set_Job"
SElExceptionPtr Camera_Set_Job(
	SObjListPtr		pModules,
	int				IndexCamera,
	char           *Job
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, IndexCamera, &pCamera));
	EXCCHECK( pCamera->Cognex_Job(pCamera, Job));

Error:
	EXCRETHROW( pexception);
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Camera_Set_Step"
SElExceptionPtr Camera_Set_Step(
	SObjListPtr		pModules,
	int				IndexCamera,
	int             Step
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	SCamerasPtr pCameras = (SCamerasPtr)pModules->Get(pModules, MODULE_CAMERA);
	SCameraPtr  pCamera = NULL;

	EXCCHECK( pCameras->GetCameraByIndex(pCameras, IndexCamera, &pCamera));
	EXCCHECK( pCamera->Cognex_set_param(pCamera, Step));

Error:
	EXCRETHROW( pexception);
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

double parGetFSchwell(void)
{
	return 1.0;
}
void SaveGraphPNG(int PanelHandle, int ControleHandle, char *PathNamePNG)
{
	int BitmapId;
	
	GetCtrlDisplayBitmap ( PanelHandle, ControleHandle, 0, &BitmapId);
	SaveBitmapToPNGFile (BitmapId, PathNamePNG); 
	DiscardBitmap (BitmapId);
}
/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "MathFunction"
void MathFunction(double sdWaveGes[MESS_BUF][BUFFERLEN],
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
				if(((dLetztWeg+DELTA_WEG) < dAktWeg))
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
				if((dLetztWeg+DELTA_WEG) < dAktWeg && (dAktKraft - dmF2V) > DELTA_KRAFT2)
				{
					dLetztWeg = dAktWeg;
					dmF_END = dAktKraft;
					//iCnt = imPosStart;
					state  = SUCHE_END;
					
				}
				break;
	
			case SUCHE_END:
				if(dAktKraft > BET_MAX_KRAFT)
				{ // Fehler: nicht beide SAS betätigt
					dmF_END   = 0.0;
					dmS_END   = 0.0;
					imPosF_END = iCnt;
					dmF2R = dAktKraft;
					state = SUCHE_REV;
				}
				if(  (sdWaveGes[3][iCnt] < BET_SAS_LIMIT) 
				  && (sdWaveGes[2][iCnt] < BET_SAS_LIMIT))
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
				if((dLetztWeg-dAktWeg) > DELTA_WEG)
				//if(dAktKraft > dmF2R + DELTA_KRAFT)
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


double getMaxDoubleArray2(double *Array)
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

double getMinDoubleArray2(double *Array)
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

double parGetFSchwell2(void)
{
	return 1.0;
}
void SaveGraphPNG2(int PanelHandle, int ControleHandle, char *PathNamePNG)
{
	int BitmapId;
	
	GetCtrlDisplayBitmap ( PanelHandle, ControleHandle, 0, &BitmapId);
	SaveBitmapToPNGFile (BitmapId, PathNamePNG); 
	DiscardBitmap (BitmapId);
}
/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "MathFunction2"
void MathFunction2(double sdWaveGes[MESS_BUF][BUFFERLEN],
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
	
	//filter the array
	 /*
		 Bw_LPF (sdWaveGes[MESS_KRAFT], // const double Input_Array[]
	        BUFFERLEN,				// int Number_of_Elements 
	        (double)SCANRATE,		// double _Sampling_Frequency 
	        FILTER_FREQ, 			// double Cutoff_Frequency
	        FELTER_ORDER,			// int Order
	        sdWaveGes[MESS_KRAFT]);	// double Output_Array[]  
			*/
		  
    dKraftLevel = getMaxDoubleArray(sdWaveGes[MESS_KRAFT]);
	
	if(parGetFSchwell2() <= 100)
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
				if(((dLetztWeg+DELTA_WEG) < dAktWeg))
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
				if((dLetztWeg+DELTA_WEG) < dAktWeg)
				{
					dLetztWeg = dAktWeg;
					dmF_END = dAktKraft;
					//iCnt = imPosStart;
					state  = SUCHE_END;
					
				}
				break;
	
			case SUCHE_END:
				if(dAktKraft > BET_MAX_KRAFT)
				{ // Fehler: nicht beide SAS betätigt
					dmF_END   = 0.0;
					dmS_END   = 0.0;
					imPosF_END = iCnt;
					dmF2R = dAktKraft;
					state = SUCHE_REV;
				}
				if(  (sdWaveGes[3][iCnt] < BET_SAS_LIMIT) 
				  && (sdWaveGes[2][iCnt] < BET_SAS_LIMIT))
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
#pragma warning( pop)

