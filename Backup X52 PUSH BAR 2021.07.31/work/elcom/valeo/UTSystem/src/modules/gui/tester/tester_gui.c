#include "station_8_gui.h"
#include "station_4_gui.h"
#include <userint.h>
#include "tester_gui.h"
#include <msxmldom.h>
#include <eventfactory/EventFactory.h> 
#include <utility.h>
#include <guid.h>
#include <tester/tester.h>
#include <jig/class/jig_class.h>
#include <axis/class/axis.h>
#include <profilometer/class/profilometers.h>
#include <language/language.h>
#include <bklib/bklib2.h>
#include "tester_gui_plugin.h"
#include <analysis.h>
#include <ansi_c.h>
#include "trans_language.h"
#include <IviBridge/IviBridge.h>  
#include <eldaq/class/eldaq.h>
#include <test/testseq.h>
#include "toolbox.h"
#include <NIDAQmx.h>
#include <export.h>


#define CHECKER_RIGHT  		0
#define CHECKER_LEFT  		1
#define CHECKER_CENTRAL  	2

int Axis_Station_40_In_Home;

typedef enum _EDisplay
{
	IO = 0,
	PANEL,
	FORCE,
	GAP,
} EDisplay;

typedef enum _EMode
{
	MODE_TESTER = 0,
	MODE_STATION_1,
	MODE_STATION_2,
	MODE_STATION_3,
	MODE_STATION_4,
	MODE_STATION_5,
	MODE_STATION_6,
	MODE_STATION_7,
	MODE_STATION_8,
} EMode;

enum EThreadId
{
	THREAD_TESTER = 0,
	THREAD_STATION_1,
	THREAD_STATION_2,
	THREAD_STATION_3,
	THREAD_STATION_4,
	THREAD_STATION_5,
	THREAD_STATION_6,
	THREAD_STATION_7,
	THREAD_STATION_8,
	NB_OF_THREAD
};

typedef struct _SCalibration
{
	int		calType;
	void*	pCalibData;
} SCalibration, *SCalibrationPtr;


#define NUMBER_OF_PANELS 	1+1
#define NUMBER_OF_PANELS_STATION_1  1+1
#define NUMBER_OF_PANELS_STATION_2  1+1  
#define NUMBER_OF_PANELS_STATION_7  1+1  
#define NUMBER_OF_PANELS_STATION_3  1+1  
#define NUMBER_OF_PANELS_STATION_4  4+1  
#define NUMBER_OF_PANELS_STATION_5  4+1  
#define NUMBER_OF_PANELS_STATION_6  4+1  
#define NUMBER_OF_PANELS_STATION_8  1+1  

static int gs_status[] = {
	TESTER_LED_1,
	TESTER_LED_2,
	TESTER_LED_3,
	TESTER_LED_4,
	TESTER_LED_5,
	TESTER_LED_6,
	TESTER_LED_7,
	TESTER_LED_8};

static EMode				gs_mode;
static int					gs_panel[NUMBER_OF_PANELS+1];
static int					gs_panel_1[NUMBER_OF_PANELS_STATION_1+1];
static int					gs_panel_2[NUMBER_OF_PANELS_STATION_2+1]; 
static int					gs_panel_7[NUMBER_OF_PANELS_STATION_7+1]; 
static int					gs_panel_3[NUMBER_OF_PANELS_STATION_3+1]; 
static int					gs_panel_4[NUMBER_OF_PANELS_STATION_4+1]; 
static int					gs_panel_5[NUMBER_OF_PANELS_STATION_5+1]; 
static int					gs_panel_6[NUMBER_OF_PANELS_STATION_6+1]; 
static int					gs_panel_8[NUMBER_OF_PANELS_STATION_8+1]; 

SLanguagePtr 				gs_pLanguage;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL; 
static STesterPtr			gs_pTester = NULL;
static SDIOSensorsPtr		gs_pSensors = NULL;
static SaxissPtr			gs_paxiss = NULL;
static SProfilometersPtr    gs_pProfilometers = NULL; 
static CAObjHandle			gs_hTreeTester;
static SIviBridgePtr		gs_IviBridge = NULL;
static ViSession			gs_DAQ = 1;
static ViSession			gs_DAQ_2 =1; 
static SPluginArgsPtr		gs_PluginArgs = NULL;
static STestSequencePtr		gs_TestSequence = NULL;

static CAObjHandle			gs_hTreeStation1;
static CAObjHandle			gs_hTreeStation2;
static CAObjHandle			gs_hTreeStation3;
static CAObjHandle			gs_hTreeStation4;
static CAObjHandle			gs_hTreeStation5;
static CAObjHandle			gs_hTreeStation6;
static CAObjHandle			gs_hTreeStation7;
static CAObjHandle			gs_hTreeStation8;
static CAObjHandle			gs_hImageList;
static HANDLE				gs_hThread[NB_OF_THREAD];
static HANDLE				gs_hEventStop[NB_OF_THREAD];

#define LINE_NAME_LENGTH	32

typedef struct _SLine
{
	char		channel[LINE_NAME_LENGTH+1];
	char		keyNode[LINE_NAME_LENGTH+1];
	uint32_t	state;
} SLine, *SLinePtr;

static SLinePtr				gs_pInputTester = NULL;
static SLinePtr				gs_pOutputTester = NULL;

static SLinePtr				gs_pInputStation1 = NULL;
static SLinePtr				gs_pOutputStation1 = NULL;

static SLinePtr				gs_pInputStation2 = NULL;
static SLinePtr				gs_pOutputStation2 = NULL;

static SLinePtr				gs_pInputStation3 = NULL;
static SLinePtr				gs_pOutputStation3 = NULL;

static SLinePtr				gs_pInputStation4 = NULL;
static SLinePtr				gs_pOutputStation4 = NULL;

static SLinePtr				gs_pInputStation5 = NULL;
static SLinePtr				gs_pOutputStation5 = NULL;

static SLinePtr				gs_pInputStation6 = NULL;
static SLinePtr				gs_pOutputStation6 = NULL;

static SLinePtr				gs_pInputStation7 = NULL;
static SLinePtr				gs_pOutputStation7 = NULL;

static SLinePtr				gs_pInputStation8 = NULL;
static SLinePtr				gs_pOutputStation8 = NULL;



static DWORD WINAPI ThreadProcTester(LPVOID param);
static DWORD WINAPI ThreadProcStation1(LPVOID param);
static DWORD WINAPI ThreadProcStation2(LPVOID param);
static DWORD WINAPI ThreadProcStation3(LPVOID param);
static DWORD WINAPI ThreadProcStation4(LPVOID param);
static DWORD WINAPI ThreadProcStation5(LPVOID param);
static DWORD WINAPI ThreadProcStation6(LPVOID param);
static DWORD WINAPI ThreadProcStation7(LPVOID param);
static DWORD WINAPI ThreadProcStation8(LPVOID param);



#define TIMEOUT 			2000

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


typedef struct _STestThreadData{
	STestParamPtr pTestParam;
	int station;
} STestThreadData, *STestThreadDataPtr;

/*----------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "SaveXmlResultFile_Manual"
void SaveXmlResultFile_Manual(STestParamPtr pTestParam, int station, bool_t failed)
{
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	char			pfile_xml[256];
	VARIANT			pfile_name_var = CA_VariantEmpty();

	if(pTestParam && pTestParam->pTicket)
	{
		
		sprintf(pfile_xml, "%s\\%s_%s_%s_station_%02d.xml",
					pTestParam->TicketDirPath,
					pTestParam->Barcode,
					(failed)? "FAULT":"OK",
					pTestParam->Time,
					station);
		
		strcpy(pTestParam->TicketFilePath, pfile_xml);
		EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, pfile_xml));
		EXCCHECKCOM( MSXML_IXMLDOMDocumentsave (pTestParam->pTicket, NULL, pfile_name_var));
		EXCCHECKCOM( CA_DiscardObjHandle(pTestParam->pTicket));
		pTestParam->pTicket = 0;
	}

Error:
	CA_VariantClear(&pfile_name_var);
	EXCDISPLAY(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetTestParameters"
static SElExceptionPtr GetTestParameters(
	SObjListPtr pModules,
	STestSequencePtr pTestSeq, 
	STestParamPtr pTestParam,
	int station
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml = 0; 
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;
	CAObjHandle     pparameter2 = 0;
	char			pfile_form_xml[MAX_PATHNAME_LEN];        
	VBOOL           is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL;
	char*           ptext2 = NULL;
	char			project_dir[MAX_PATHNAME_LEN],
					result_dir[MAX_PATHNAME_LEN],
					sdate_time[256],
					buffer[2048] = "";
	char			testdll_path[MAX_PATHNAME_LEN] = "";
	SYSTEMTIME		st;
	bool_t			is_locked = FALSE;
	int             serial;
	int				size;
	char         	path[256];
	int            	i = 0;
	int 			Test, SubTest;
	
	/* check destination directory */
	GetLocalTime(&st);
	EXCCHECKCVI(GetProjectDir(project_dir));

	sprintf(result_dir, "%s\\data\\production\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);

	EXCCHECKCVI( FileExists (result_dir, &size));
	
	if(size==-1)
		{
		/* create directory */	
		EXCCHECKCVI( MakeDir(result_dir));
		}
	
	sprintf(pfile_form_xml, "%s\\plugins\\test_form.xml", project_dir);

	/* open xml file */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, pfile_form_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//testseq", &pnode));
	
	/* test dll name */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='ProductTestFile']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			EXCCHECKCVI( GetProjectDir(testdll_path)); 
			strcat(testdll_path, "\\plugins\\");
			strcat(testdll_path, ptext);
			
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	else
	{
		EXCTHROW(-1, _("Library name does not specified in xml file!"));
	}
	
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='JobCamera:Picto']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			sprintf(pTestSeq->JobCameraPicto, ptext);
			CA_FreeMemory(ptext);
			ptext = NULL;
		}
		else 
			EXCTHROW(TEST_ERROR_CAMERA_SET_PARAMETER, "Parameter JobCamera:Picto not found!");
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='JobCamera:LED']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			sprintf(pTestSeq->JobCameraLed, ptext);
			CA_FreeMemory(ptext);
			ptext = NULL;
		}
		else 
			EXCTHROW(TEST_ERROR_CAMERA_SET_PARAMETER, "Parameter JobCamera:LED not found!");
	}
	
	/* load new test sequence from DLL */ 
	EXCCHECK( pTestSeq->Load(pTestSeq, testdll_path, pModules, gs_EventFactory));

		
	do
	{
		sprintf(path, "test[@step='%d']/name", ++i); 
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext && *ptext)
			{
				sscanf(ptext, "%d.%d", &Test, &SubTest);
		
				if (station == Test && SubTest != 90)
				{
				EXCCHECK(pTestSeq->SetTestActive(pTestSeq, ptext, i));
				}
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}
		else
		{
			break;
		}
		
	}while(TRUE);

	/* check validation */
	pTestParam->pTicket = pxml;
	
	EXCCHECK( pTestSeq->UsageValidation(pTestSeq, pTestParam));  
	

	/* date_time */
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/mode", &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, "Manual"));
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	/* date_time */
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/date_time", &pnode));
	if(pnode)
	{
		sprintf(sdate_time, "%02d.%02d.%02d %02d.%02d.%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);      
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, sdate_time));
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	/* prepare test parameter data */
	pTestParam->pTicket = pxml;
	pTestParam->shift_id = 1;
	sprintf(pTestParam->Time, "%02d.%02d.%02d", st.wHour, st.wMinute, st.wSecond);
	sprintf(pTestParam->Date, "%04d.%02d.%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(pTestParam->TicketDirPath, result_dir);
	sprintf(pTestParam->Barcode, "%s\0", "none");
			
Error:
	EXCRETHROW( pexception);  
}
/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Test"
DWORD WINAPI Thread_Test(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
	int							station = (int)param;
	int                         i=0;
	static STestParamPtr		pTestParam = NULL;
	SYSTEMTIME			st;
	//EXCCHECKCVI( SetCtrlAttribute(panel, Led_Test[station/10],ATTR_FRAME_COLOR, VAL_WHITE));

	pTestParam = calloc(1, sizeof(STestParam));
	EXCCHECKALLOC( pTestParam);
	GetLocalTime(&st);
	
	for(i=0; i<1; i++)
	{	// printf("test number %d\r\n",i+1);
		if(gs_TestSequence==NULL)
		   	EXCCHECK( gs_PluginArgs->GetArg(gs_PluginArgs, "TestSequence", ARG_PTR, &gs_TestSequence));

		if(gs_PluginArgs)
		{
			EXCCHECK( GetTestParameters(gs_Modules, gs_TestSequence, pTestParam, station));
			sprintf(pTestParam->Time, "%02d.%02d.%02d", st.wHour, st.wMinute, st.wSecond);
		}
							
		if(gs_TestSequence && pTestParam)
		{				
			pexception = gs_TestSequence->RunSelectedStation(gs_TestSequence, pTestParam, FALSE, station);
			if (pexception == NULL)	
			{
			SaveXmlResultFile_Manual(pTestParam, station, 0);
			//EXCCHECKCVI( SetCtrlAttribute(panel, Led_Test[station/10],ATTR_FRAME_COLOR, VAL_GREEN));
			}
			else  
			{   																
			//EXCCHECKCVI( SetCtrlAttribute(panel, Led_Test[station/10],ATTR_FRAME_COLOR, VAL_RED));
			SaveXmlResultFile_Manual(pTestParam, station, 1);
			}
			/* unload old test sequence */
			//EXCCHECK( gs_TestSequence->UnLoad(gs_TestSequence));
		}
	
	
	Delay (2); //delay 5s attente avant next test
		
		
		
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

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalogWithScale"
SElExceptionPtr ReadAnalogWithScale(char *channel, SCalibrationPtr pScaleInfo, double *Value)
{
SElExceptionPtr pexception = NULL;
int channel_index;
double pChannelData[2];
float64*	pdata;


		sscanf(channel, "ai%d",&channel_index);
		pdata = data_Analog + channel_index;

		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k);
			
			LinEv1D( pdata, 1, k, q,  pChannelData);
			*Value = pChannelData[0]; 
		}
		else 
			*Value = data_Analog[channel_index];
		
	//	printf("AI%d Value = %.2f/n/r", channel_index, *Value); 
Error:
	EXCRETHROW( pexception);
	
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadAnalogWithScale2"
SElExceptionPtr ReadAnalogWithScale2(char *channel, SCalibrationPtr pScaleInfo, double *Value)
{
SElExceptionPtr pexception = NULL;
int channel_index;
double pChannelData[2];
float64*	pdata;


		sscanf(channel, "ai%d",&channel_index);
		pdata = data_Analog_2 + channel_index;

		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k);
			
			LinEv1D( pdata, 1, k, q,  pChannelData);
			*Value = pChannelData[0]; 
		}
		else 
			*Value = data_Analog_2[channel_index];
		
	//	printf("AI%d Value = %.2f\n\r", channel_index, *Value); 
Error:
	EXCRETHROW( pexception);
	
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnFncStation1"
int CVICALLBACK OnFncStation1 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
	int                 microCam, right, left;
	double rvalue, value_1, value_2, value_3, value_4, somme_1, somme_2;   
 	HANDLE  			hThread;
	SCalibrationPtr	pScaleInfo = NULL; 

	
		
	switch (event)
		{
		case EVENT_COMMIT:

			switch(control)
			{
				case STATION_1_LOCK:
				
					pexception = gs_pTester->LockPanelStation10(gs_pTester, 7000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_LOCK,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_LOCK,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_UNLOCK, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
					
				case STATION_1_UNLOCK:
				
					pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 7000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_UNLOCK,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_UNLOCK,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_LOCK, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				
				
				case STATION_1_CODE_INSIDE:  
					
					pexception = gs_pTester->GetItemCode(gs_pTester, 0, 1, 512, buffer, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlVal(panel, STATION_1_CODE_STRING_INSIDE, buffer));
					}
					else
					{
						EXCDISPLAY( pexception); 
						EXCCHECKCVI( SetCtrlVal(panel, STATION_1_CODE_STRING_INSIDE, "#error"));
					}
				
				break;
				
				case STATION_1_CODE_OUTSIDE:  
					
					pexception = gs_pTester->GetItemCode(gs_pTester, 0, 0, 512, buffer, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlVal(panel, STATION_1_CODE_STRING_OUTSIDE, buffer));
					}
					else
					{
						EXCDISPLAY( pexception); 
						EXCCHECKCVI( SetCtrlVal(panel, STATION_1_CODE_STRING_OUTSIDE, "#error"));
					}
				
				break;
				
				case STATION_1_PRODUCT_PRESENCE:
					
					checkErr( eldaq_GetChannelScale ( gs_DAQ, "ai5", "voltage", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale("ai5", pScaleInfo, &rvalue));
			//	distance = rvalue;
				value_3=rvalue;
			//	printf("Hello\n\r"); 
				sprintf(buffer, "%.02fV",value_3);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_1_SENSOR_RIGHT, buffer));
			//	printf("AI5 Value %.02fV \n\r", value_3);
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ, "ai31", "voltage", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale("ai31", pScaleInfo,&rvalue));
				
			//	distance = rvalue;
				value_4 = rvalue;
				sprintf(buffer, "%.02fV",value_4);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_1_SENSOR_LEFT, buffer));
			//	printf("AI31 Value %.02fV \n\r", value_4); 
				
					if(value_3<0.8 && value_3>0.67 && value_4<0.8 && value_4>0.67) 
					{
					    EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_PRODUCT_PRESENT,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_PRODUCT_ABSENT, ATTR_FRAME_COLOR, VAL_WHITE)); 
						ProcessSystemEvents ();
					}
				else
					{
					//	EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_PRODUCT_ABSENT,ATTR_FRAME_COLOR, VAL_RED));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_PRODUCT_PRESENT,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents (); 
					}
					
				break;
				
				
				case STATION_1_PRODUCT_TYPE:
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ, "ai3", "voltage", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale("ai3", pScaleInfo, &rvalue));
				value_1=rvalue;
				sprintf(buffer, "%.02fV",value_1);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_1_SENSOR_LEFT, buffer));
			
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ, "ai4", "voltage", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale("ai4", pScaleInfo,&rvalue));
				value_2 = rvalue;
				sprintf(buffer, "%.02fV",value_2);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_1_SENSOR_RIGHT, buffer));
	 
				
				if(value_1<3.75 && value_1>3.40 && value_2<3.75 && value_2>3.40)
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_X52_7, ATTR_FRAME_COLOR, VAL_WHITE));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_UNKNOWN, ATTR_FRAME_COLOR, VAL_WHITE));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_X52_9,ATTR_FRAME_COLOR, VAL_WHITE)); 
						ProcessSystemEvents ();
					}
				else if(value_1<3.35 && value_1>3.05 && value_2<3.35 && value_2>3.05)
				
					
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_X52_7,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_UNKNOWN, ATTR_FRAME_COLOR, VAL_WHITE));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_X52_9,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					
				else if(value_1<4.1 && value_1>3.80 && value_2<4.1 && value_2>3.80)
				    
					    
					  {
						   EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_VS11,ATTR_FRAME_COLOR, VAL_WHITE));
						   EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_X52_7, ATTR_FRAME_COLOR, VAL_WHITE));
						   EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_UNKNOWN, ATTR_FRAME_COLOR, VAL_WHITE));
						   EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_X52_9,ATTR_FRAME_COLOR, VAL_GREEN));
						   ProcessSystemEvents ();
					  }
					
				 else
						
					  {
						    EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_UNKNOWN,ATTR_FRAME_COLOR, VAL_RED));
						    EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_X52_7, ATTR_FRAME_COLOR, VAL_WHITE));
						    EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
							EXCCHECKCVI( SetCtrlAttribute(panel, STATION_1_LED_TYPE_X52_9,ATTR_FRAME_COLOR, VAL_WHITE));
						    ProcessSystemEvents ();
					   }

				
				
				
				
				
				break;
				
				
				case STATION_1_RUN:
					
					EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &microCam));
					
				  	if (microCam)
						EXCCHECK(ThreadStart(Thread_Test, (void*)00, &hThread, NULL)); 
				  
					else 
						EXCTHROW( -1, "Plateau pas en position!");
				
				break;

			}
			
			break;
		}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnFncStation2"
int CVICALLBACK OnFncStation2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
	double rvalue,distance;
	SCalibrationPtr	pScaleInfo = NULL;
	int                 microCam;
 	HANDLE  			hThread;
	
	switch (event)
		{
		case EVENT_COMMIT:

			switch(control)
			{
				case STATION_2_CLIPPAGE_PUNZONI_OFF:
				
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_PUNZONI_OFF,ATTR_FRAME_COLOR, VAL_WHITE));  
					pexception = gs_pTester->StopClipping_punzoni(gs_pTester, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_PUNZONI_OFF,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_PUNZONI_OFF,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_PUNZONI_ON,ATTR_FRAME_COLOR, VAL_WHITE));  

				break;
				
				case STATION_2_CLIPPAGE_PUNZONI_ON:
				
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_PUNZONI_ON,ATTR_FRAME_COLOR, VAL_WHITE));  
					pexception = gs_pTester->Clipping_punzoni(gs_pTester, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_PUNZONI_ON,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_PUNZONI_ON,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_PUNZONI_OFF,ATTR_FRAME_COLOR, VAL_WHITE));  
				break;
				
				case STATION_2_CLIPPAGE_CONTRAST_OFF:
				
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_CONTRAS_OFF,ATTR_FRAME_COLOR, VAL_WHITE));  
					pexception = gs_pTester->StopClipping_contrasti(gs_pTester, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_CONTRAS_OFF,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_CONTRAS_OFF,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_CONTRAS_ON,ATTR_FRAME_COLOR, VAL_WHITE));  

				break;
				
				case STATION_2_CLIPPAGE_CONTRAST_ON:
				
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_CONTRAS_ON,ATTR_FRAME_COLOR, VAL_WHITE));  
					pexception = gs_pTester->Clipping_contrasti(gs_pTester, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_CONTRAS_ON,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_CONTRAS_ON,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_CLIPP_CONTRAS_OFF,ATTR_FRAME_COLOR, VAL_WHITE));  
				break;	
				
				case STATION_2_POTENT_OFF:
				
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_POTENT_OFF,ATTR_FRAME_COLOR, VAL_WHITE));  
					pexception = gs_pTester->StopCheckClipping(gs_pTester, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_POTENT_OFF,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_POTENT_OFF,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_POTENT_ON,ATTR_FRAME_COLOR, VAL_WHITE));  

				break;
				
				case STATION_2_POTENT_ON:
				
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_POTENT_ON,ATTR_FRAME_COLOR, VAL_WHITE));  
					pexception = gs_pTester->CheckClipping(gs_pTester, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_POTENT_ON,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_POTENT_ON,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_2_LED_POTENT_OFF,ATTR_FRAME_COLOR, VAL_WHITE));  
				break;	
				
				
				case STATION_2_READ:
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai0", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai0", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_INT_1, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai1", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai1", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_INT_2, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai2", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai2", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_INT_3, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai3", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai3", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_INT_4, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai4", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai4", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_INT_5, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai5", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai5", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_INT_6, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai6", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai6", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_INT_7, buffer));
				
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai7", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai7", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_EXT_1, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai8", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai8", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_EXT_2, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai9", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai9", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_EXT_3, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai10", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai10", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_EXT_4, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai11", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai11", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_EXT_5, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai12", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai12", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_EXT_6, buffer));
				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ_2, "ai13", "distance", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale2("ai13", pScaleInfo,&rvalue));
				distance = rvalue;
				sprintf(buffer, "%.02fmm",distance);
				EXCCHECKCVI( SetCtrlVal( panel, STATION_2_EXT_7, buffer));
				
				
			//	}
					
					
				break;
				
			case STATION_2_RUN:
					
					EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &microCam));
					
				  	if (microCam)
						EXCCHECK(ThreadStart(Thread_Test, (void*)20, &hThread, NULL)); 
				  
					else 
						EXCTHROW( -1, "Plateau pas en position!");
				
				break;	
				
			}
			
			break;
		}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

#undef __FUNC__
#define __FUNC__ "OnFncStation3"
int CVICALLBACK OnFncStation3 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
	int i,cycle;
	double timeOn,timeOff;
	int EnablePush1;
	int EnablePush2;
	int EnablePush3;
	int EnablePush4;
	int EnablePush5;
	int EnablePush6;
	int EnablePush7;
	int EnablePush8;
	int EnablePush9; 
	char Combinaison[9+1];
	
	SCalibrationPtr	pScaleInfo = NULL;
	int                 microCam;
 	HANDLE  			hThread;
	
	
	switch (event)
		{
		case EVENT_COMMIT:
	
		switch(control)
			{
			case STATION_3_RODAGE_DOWN :  // deja changé
					pexception = gs_pTester->SelectRodageX52(gs_pTester, 10000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_RODAGE_DOWN_LED,ATTR_FRAME_COLOR, VAL_GREEN));		// deja changé
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_RODAGE_DOWN_LED,ATTR_FRAME_COLOR, VAL_RED));		   // deja chnagé
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_RODAGE_UP_LED,ATTR_FRAME_COLOR, VAL_WHITE));

			break;
			
			case STATION_3_RODAGE_UP :
					pexception = gs_pTester->SelectRodageVS11(gs_pTester, 10000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_RODAGE_UP_LED, ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_RODAGE_UP_LED,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_RODAGE_DOWN_LED,ATTR_FRAME_COLOR, VAL_WHITE));  // deja changé

			break;
			
			case STATION_3_START_RODAGE :
				
					GetCtrlVal(panel, STATION_3_CYCLE, &cycle);
					GetCtrlVal(panel, STATION_3_TIME_ON, &timeOn);
					GetCtrlVal(panel, STATION_3_TIME_OFF, &timeOff);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_1, &EnablePush1);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_2, &EnablePush2);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_3, &EnablePush3);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_4, &EnablePush4);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_5, &EnablePush5);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_6, &EnablePush6);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_7, &EnablePush7);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_8, &EnablePush8);
					GetCtrlVal(panel, STATION_3_ENABLE_PUSH_9, &EnablePush9);
					
					Combinaison[0] = EnablePush1;
					Combinaison[1] = EnablePush2;
					Combinaison[2] = EnablePush3;
					Combinaison[3] = EnablePush4;
					Combinaison[4] = EnablePush5;
					Combinaison[5] = EnablePush6;
					Combinaison[6] = EnablePush7;
					Combinaison[7] = EnablePush8;
					Combinaison[8] = EnablePush9; 

					
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_LED_RODAGE,ATTR_FRAME_COLOR, VAL_WHITE));

					pexception = gs_pTester->Rodage(gs_pTester, 10000, cycle, timeOn, timeOff, Combinaison);
					
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_LED_RODAGE, ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_3_LED_RODAGE,ATTR_FRAME_COLOR, VAL_RED));
					}


					
				break;
				
				case STATION_3_RUN:
					
					EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &microCam));
					
				  	if (microCam)
						EXCCHECK(ThreadStart(Thread_Test, (void*)30, &hThread, NULL)); 
				  
					else 
						EXCTHROW( -1, "Plateau pas en position!");
				
				break;	
			
			}
			break;
		}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnFncTester"
int CVICALLBACK OnFncTester (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
	char                TableStatus[8+1];
	bool_t				is_table_ready = FALSE;
	bool_t				is_station_10_ready = FALSE,
		                is_station_20_ready = FALSE,
						is_station_30_ready = FALSE,
						is_station_40_ready = FALSE,
						is_station_50_ready = FALSE,
						is_station_60_ready = FALSE;
	
	switch (event)
		{
		case EVENT_COMMIT:

			switch(control)
			{
				case TESTER_MASK_CLOSE:
					EXCCHECKCVI( SetCtrlAttribute(panel, TESTER_MASK_CLOSE_LED,ATTR_FRAME_COLOR, VAL_WHITE));
					ProcessSystemEvents ();
			
					pexception = gs_pTester->GetStationStatus(gs_pTester, TableStatus);
					is_station_10_ready = TableStatus[0];
					is_station_20_ready = TableStatus[1];
					is_station_30_ready = TableStatus[3];
					is_station_40_ready = TableStatus[4];
					is_station_50_ready = TableStatus[5];
					is_station_60_ready = TableStatus[6];
					
				//	printf(" is_station_10_ready = %d \n\r", is_station_10_ready);

					if(is_station_10_ready &&
					   is_station_20_ready && 
		   			   is_station_30_ready &&
		   			   is_station_40_ready &&
		   			   is_station_50_ready &&
		   			   is_station_60_ready)
						{
						is_table_ready = TRUE;
						}
			
					if	(is_table_ready)
					   {
						pexception = gs_pTester->TableRotate(gs_pTester, 3000, TRUE);
						if(pexception==NULL) 
							{
							EXCCHECKCVI( SetCtrlAttribute(panel, TESTER_MASK_CLOSE_LED,ATTR_FRAME_COLOR, VAL_GREEN));
							}
						else
							{
							EXCDISPLAY( pexception);   
							EXCCHECKCVI( SetCtrlAttribute(panel, TESTER_MASK_CLOSE_LED,ATTR_FRAME_COLOR, VAL_RED));
							}
					   }
					break;
			}
			
			break;
		}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

#define BKTREE_HEADER "root|DIO^w300$id|ID^w300$desc|Description^w500$state|^w24\n"
#define OUTPUT_STR			"output"
#define INPUT_STR			"input"

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ClearTree"
static SElExceptionPtr ClearTree(CAObjHandle hTree)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
	HRESULT         hr = 0;
	short			AXvalue;

	EXCCHECKCOM( BKLib2__DBKTreeFromString ( hTree, NULL, BKTREE_HEADER, "$", "\n", &AXvalue));
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTree"
static SElExceptionPtr SetTree(
	const char* fileXml, 
	const char* GUID, 
	CAObjHandle hTree, 
	SLinePtr* pInput,
	SLinePtr* pOutput
)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
	HRESULT         hr = 0;
	char            _message[512];
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     plist = 0; 
	CAObjHandle     pparameter = 0;
	const char*     pfile_xml = (fileXml) ? fileXml : "../modules/tester.xml";
	VBOOL           is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL;
	int32_t			length, i, j;
	SELStringPtr 	pelstring = NULL;
	short			AXvalue;
	char			path[512];
	ERRORINFO		errorInfo;

	EXCCHECK( elstring_new( BKTREE_HEADER, &pelstring));
	
	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, &errorInfo, pfile_name_var, &is_xml_success));

	sprintf(path, "//module[@id='%s']/Sensors", GUID);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, path, &pnode));	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (pnode, NULL, "Sensor[@type='DI']", &plist));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));

	if(pInput && *pInput)
	{
		free( *pInput);
		*pInput = NULL;
	}
	if(length>0)
	{
		if(pInput) *pInput = calloc(length+1, sizeof(SLine));
	}

	EXCCHECK( pelstring->Append(pelstring, "|"INPUT_STR"|Inputs\n"));

	for( i=0; i<length; i++)
	{
		EXCCHECK( pelstring->Format(pelstring, ""INPUT_STR"|%d|", i));    
		if(pInput) sprintf((*pInput)[i].keyNode, "%d", i);

		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pnode2));
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@name", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
				
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		if ( pnode2 )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode2, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
		
			if(pInput) strncpy((*pInput)[i].channel, ptext, LINE_NAME_LENGTH);

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@description", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
			
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECK( pelstring->Append(pelstring, "^i0\n"));
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (pnode, NULL, "Sensor[@type='DO']", &plist));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));
	
	if(pOutput && *pOutput)
	{
		free( *pOutput);
		*pOutput = NULL;
	}
	if(length>0)
	{
		if(pOutput) *pOutput = calloc(length+1, sizeof(SLine));
	}

	EXCCHECK( pelstring->Append(pelstring, "|"OUTPUT_STR"|Outputs\n"));

	for( j=0; j<length; j++)
	{
		EXCCHECK( pelstring->Format(pelstring, ""OUTPUT_STR"|%d|", i+j)); 
		if(pOutput) sprintf((*pOutput)[j].keyNode, "%d", i+j);
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pnode2));
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@name", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
				
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		if ( pnode2 )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode2, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
		
			if(pOutput) strncpy((*pOutput)[j].channel, ptext, LINE_NAME_LENGTH);

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@description", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
			
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		EXCCHECK( pelstring->Append(pelstring, "^i0^d0\n"));
	}

	EXCCHECKCOM( BKLib2__DBKTreeFromString ( hTree, NULL, pelstring->GetPtr(pelstring), "$", "\n", &AXvalue));
	EXCCHECKCOM( BKLib2__DBKTreeSetEnableDragAndDrop ( hTree, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeExpandLevel ( hTree, NULL, 2, &AXvalue));
	
Error:
	elstring_delete( &pelstring);
	CA_FreeMemory(ptext);
	CA_VariantClear(&pfile_name_var);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);  
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SetTreeStations"
static SElExceptionPtr SetTreeStations(
	const char* fileXml, 
	const char* GUID, 
	CAObjHandle hTree, 
	SLinePtr* pInput,
	SLinePtr* pOutput,
	int Station
)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
	HRESULT         hr = 0;
	char            _message[512];
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     plist = 0; 
	CAObjHandle     pparameter = 0;
	const char*     pfile_xml = (fileXml) ? fileXml : "../modules/tester.xml";
	VBOOL           is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL;
	int32_t			length, i, j;
	SELStringPtr 	pelstring = NULL;
	short			AXvalue;
	char			path[512];
	ERRORINFO		errorInfo;

	EXCCHECK( elstring_new( BKTREE_HEADER, &pelstring));
	
	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, &errorInfo, pfile_name_var, &is_xml_success));

	sprintf(path, "//module[@id='%s']/Station%d", GUID,Station);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, path, &pnode));	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (pnode, NULL, "Sensor[@type='DI']", &plist));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));

	if(pInput && *pInput)
	{
		free( *pInput);
		*pInput = NULL;
	}
	if(length>0)
	{
		if(pInput) *pInput = calloc(length+1, sizeof(SLine));
	}

	EXCCHECK( pelstring->Append(pelstring, "|"INPUT_STR"|Inputs\n"));

	for( i=0; i<length; i++)
	{
		EXCCHECK( pelstring->Format(pelstring, ""INPUT_STR"|%d|", i));    
		if(pInput) sprintf((*pInput)[i].keyNode, "%d", i);

		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pnode2));
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@name", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
				
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		if ( pnode2 )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode2, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
		
			if(pInput) strncpy((*pInput)[i].channel, ptext, LINE_NAME_LENGTH);

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@description", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
			
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECK( pelstring->Append(pelstring, "^i0\n"));
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes (pnode, NULL, "Sensor[@type='DO']", &plist));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));
	
	if(pOutput && *pOutput)
	{
		free( *pOutput);
		*pOutput = NULL;
	}
	if(length>0)
	{
		if(pOutput) *pOutput = calloc(length+1, sizeof(SLine));
	}

	EXCCHECK( pelstring->Append(pelstring, "|"OUTPUT_STR"|Outputs\n"));

	for( j=0; j<length; j++)
	{
		EXCCHECK( pelstring->Format(pelstring, ""OUTPUT_STR"|%d|", i+j)); 
		if(pOutput) sprintf((*pOutput)[j].keyNode, "%d", i+j);
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pnode2));
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@name", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
				
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		if ( pnode2 )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode2, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
		
			if(pOutput) strncpy((*pOutput)[j].channel, ptext, LINE_NAME_LENGTH);

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@description", &pparameter));  
		if ( pparameter )
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			
			EXCCHECK( pelstring->Append(pelstring, ptext));
			EXCCHECK( pelstring->Append(pelstring, "$"));
			
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		EXCCHECK( pelstring->Append(pelstring, "^i0^d0\n"));
	}

	EXCCHECKCOM( BKLib2__DBKTreeFromString ( hTree, NULL, pelstring->GetPtr(pelstring), "$", "\n", &AXvalue));
	EXCCHECKCOM( BKLib2__DBKTreeSetEnableDragAndDrop ( hTree, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeExpandLevel ( hTree, NULL, 2, &AXvalue));
	
Error:
	elstring_delete( &pelstring);
	CA_FreeMemory(ptext);
	CA_VariantClear(&pfile_name_var);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);  
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetOutputStates"
static SElExceptionPtr GetOutputStates(SLinePtr pOutput)
{
	HRESULT			hr;
	SElExceptionPtr	pexception = NULL;
	uint32_t		state = 0;
	int32_t			i;

	if(pOutput)
	{
		i = 0;
		while(0 != strlen(pOutput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pOutput[i].channel, &state));
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeTester, 
														 NULL,  
														 pOutput[i].keyNode, 
														 "state", 
														 state));
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeTester, 
														 NULL, 
														 pOutput[i].keyNode, 
														 "state", 
														 state));
			i++;
		};
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickTester"
static HRESULT CVICALLBACK BkTreeNodeClickTester (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeTester, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeTester, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeTester, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeTester, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeTester, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickStation1"
static HRESULT CVICALLBACK BkTreeNodeClickStation1 (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeStation1, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeStation1, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeStation1, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeStation1, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation1, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickStation2"
static HRESULT CVICALLBACK BkTreeNodeClickStation2 (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeStation2, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeStation2, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeStation2, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeStation2, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation2, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickStation7"
static HRESULT CVICALLBACK BkTreeNodeClickStation7 (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeStation7, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeStation7, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeStation7, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeStation7, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation7, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickStation3"
static HRESULT CVICALLBACK BkTreeNodeClickStation3 (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeStation3, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeStation3, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeStation3, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeStation3, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation3, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickStation4"
static HRESULT CVICALLBACK BkTreeNodeClickStation4 (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeStation4, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeStation4, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeStation4, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeStation4, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation4, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickStation5"
static HRESULT CVICALLBACK BkTreeNodeClickStation5 (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeStation5, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeStation5, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeStation5, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeStation5, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation5, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickStation6"
static HRESULT CVICALLBACK BkTreeNodeClickStation6 (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeStation6, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeStation6, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeStation6, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeStation6, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation6, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeNodeClickStation8"
static HRESULT CVICALLBACK BkTreeNodeClickStation8 (
   CAObjHandle caServerObjHandle,
    void *caCallbackData,
    char *keyNode
)
{
	SElExceptionPtr pexception =  NULL; 
   	int32_t      	error = 0;
	HRESULT			hr;
	char*			parentNode = NULL;
	char*			id = NULL;
	int32_t			state;

    EXCCHECKCOM( BKLib2__DBKTreeGetParentNode(gs_hTreeStation8, NULL, keyNode, &parentNode));

	if( 0==strcmp(parentNode, OUTPUT_STR))
	{
		/* set output */
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemData(gs_hTreeStation8, NULL, keyNode, "state", &state));
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeItemText(gs_hTreeStation8, NULL, keyNode, "id", &id));

		EXCCHECK( gs_pSensors->SetState(gs_pSensors, id, !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData(gs_hTreeStation8, NULL, keyNode, "state", !state));
		EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation8, NULL, keyNode, "state", !state));
	}

Error: 
	EXCDISPLAY( pexception);
   	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayStation1"
void CVICALLBACK PostDeferredDisplayStation1 (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( DisplayPanel(gs_panel_1[STATION_1]));
	EXCCHECK( GetOutputStates( gs_pOutputStation1));

Error:
	EXCDISPLAY(pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayStation2"
void CVICALLBACK PostDeferredDisplayStation2 (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( DisplayPanel(gs_panel_2[STATION_2]));
	EXCCHECK( GetOutputStates( gs_pOutputStation2));

Error:
	EXCDISPLAY(pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayStation3"
void CVICALLBACK PostDeferredDisplayStation3 (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( DisplayPanel(gs_panel_3[STATION_3]));
	EXCCHECK( GetOutputStates( gs_pOutputStation3));

Error:
	EXCDISPLAY(pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayStation4"
void CVICALLBACK PostDeferredDisplayStation4 (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( DisplayPanel(gs_panel_4[STATION_4]));
	EXCCHECK( GetOutputStates( gs_pOutputStation4));

Error:
	EXCDISPLAY(pexception);
}/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayStation5"
void CVICALLBACK PostDeferredDisplayStation5 (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( DisplayPanel(gs_panel_5[STATION_5]));
	EXCCHECK( GetOutputStates( gs_pOutputStation5));

Error:
	EXCDISPLAY(pexception);
}/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayStation6"
void CVICALLBACK PostDeferredDisplayStation6 (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( DisplayPanel(gs_panel_6[STATION_6]));
	EXCCHECK( GetOutputStates( gs_pOutputStation6));

Error:
	EXCDISPLAY(pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayStation7"
void CVICALLBACK PostDeferredDisplayStation7 (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( DisplayPanel(gs_panel_7[STATION_7]));
	EXCCHECK( GetOutputStates( gs_pOutputStation7));

Error:
	EXCDISPLAY(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayStation8"
void CVICALLBACK PostDeferredDisplayStation8 (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECKCVI( DisplayPanel(gs_panel_8[STATION_8]));
	EXCCHECK( GetOutputStates( gs_pOutputStation8));

Error:
	EXCDISPLAY(pexception);
}


void CVICALLBACK PostDeferredHideStation1 (void *callbackData) { HidePanel(gs_panel_1[STATION_1]);}
void CVICALLBACK PostDeferredHideStation2 (void *callbackData) { HidePanel(gs_panel_2[STATION_2]);}
void CVICALLBACK PostDeferredHideStation3 (void *callbackData) { HidePanel(gs_panel_3[STATION_3]);}
void CVICALLBACK PostDeferredHideStation4 (void *callbackData) { HidePanel(gs_panel_4[STATION_4]);}
void CVICALLBACK PostDeferredHideStation5 (void *callbackData) { HidePanel(gs_panel_5[STATION_5]);}
void CVICALLBACK PostDeferredHideStation6 (void *callbackData) { HidePanel(gs_panel_6[STATION_6]);}
void CVICALLBACK PostDeferredHideStation7 (void *callbackData) { HidePanel(gs_panel_7[STATION_7]);}
void CVICALLBACK PostDeferredHideStation8 (void *callbackData) { HidePanel(gs_panel_8[STATION_8]);}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredDisplayPanel"
void CVICALLBACK PostDeferredDisplayPanel (void *callbackData) 
{ 
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;

	EXCCHECK( GetOutputStates(gs_pOutputTester));

Error:
	EXCDISPLAY(pexception);
}

/*---------------------------------------------------------------------------*/
void CVICALLBACK PostDeferredHidePanel (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		HidePanel(*panel);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ShowPanel"
static SElExceptionPtr ShowPanel(void)
{
	HRESULT			hr;
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			visible;
	
	EXCCHECKCVI( GetPanelAttribute( gs_panel[TESTER], ATTR_VISIBLE, &visible));
	if(!visible)
	{
		EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem (gs_hTreeTester, NULL, INPUT_STR));
		EXCCHECKCVI( DisplayPanel(gs_panel[TESTER]));
		EXCCHECK( GetOutputStates(gs_pOutputTester));
		EXCCHECK( ThreadStart( ThreadProcTester, gs_pInputTester, &gs_hThread[THREAD_TESTER], &gs_hEventStop[THREAD_TESTER]));   //ARAIBIA
	}

Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ClosePanel"
static SElExceptionPtr ClosePanel(void)          
{
	int32_t			error = 0;
   	SElExceptionPtr	pexception = NULL;
	int				visible;
	int				i;

	EXCCHECKCVI( GetPanelAttribute (gs_panel[TESTER], ATTR_VISIBLE, &visible));
	if(visible)
	{
	  	EXCCHECKCVI( HidePanel(gs_panel[TESTER])); 
		EXCCHECKCVI( HidePanel(gs_panel_1[STATION_1]));
		EXCCHECKCVI( HidePanel(gs_panel_2[STATION_2]));
		EXCCHECKCVI( HidePanel(gs_panel_3[STATION_3]));
		EXCCHECKCVI( HidePanel(gs_panel_4[STATION_4]));
		EXCCHECKCVI( HidePanel(gs_panel_5[STATION_5]));
		EXCCHECKCVI( HidePanel(gs_panel_6[STATION_6]));
		EXCCHECKCVI( HidePanel(gs_panel_7[STATION_7]));
		EXCCHECKCVI( HidePanel(gs_panel_8[STATION_8]));

		for(i=0;i<NB_OF_THREAD;i++)
			EXCCHECK( ThreadStop(&gs_hThread[i], &gs_hEventStop[i]));
	}
	
Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PostDeferredCallBkTreeTester"
void CVICALLBACK PostDeferredCallBkTreeTester(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputTester)
	{
		i = 0;
		while(0 != strlen(gs_pInputTester[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeTester, 
														 NULL,  
														 gs_pInputTester[i].keyNode, 
														 "state", 
														 gs_pInputTester[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcTester"
static DWORD WINAPI ThreadProcTester(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	bool_t			data_changed = FALSE;
	char            status[9+1] ;
	bool_t			Is_Ready = FALSE;

	memset (status, 0x00, 8);
	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_TESTER], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}

		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeTester, NULL));
			data_changed = FALSE;
		}
	
		gs_pTester->GetStationStatus(gs_pTester, status);
		for (i=0;i<8;i++)
		{
		Is_Ready = (int)status[i];
	//	printf("Station %d Ready = %d \n\r", i, Is_Ready);
	//	printf("Station %d Ready = %s \n\r", i, status[i]);
		SetCtrlVal (gs_panel[TESTER], gs_status[i], Is_Ready);
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__													  
#define __FUNC__ "PostDeferredCallBkTreeStation1"
void CVICALLBACK PostDeferredCallBkTreeStation1(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputStation1)
	{
		i = 0;
		while(0 != strlen(gs_pInputStation1[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation1, 
														 NULL,  
														 gs_pInputStation1[i].keyNode, 
														 "state", 
														 gs_pInputStation1[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__													  
#define __FUNC__ "PostDeferredCallBkTreeStation2"
void CVICALLBACK PostDeferredCallBkTreeStation2(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputStation2)
	{
		i = 0;
		while(0 != strlen(gs_pInputStation2[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation2, 
														 NULL,  
														 gs_pInputStation2[i].keyNode, 
														 "state", 
														 gs_pInputStation2[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__													  
#define __FUNC__ "PostDeferredCallBkTreeStation7"
void CVICALLBACK PostDeferredCallBkTreeStation7(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputStation7)
	{
		i = 0;
		while(0 != strlen(gs_pInputStation7[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation7, 
														 NULL,  
														 gs_pInputStation7[i].keyNode, 
														 "state", 
														 gs_pInputStation7[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__													  
#define __FUNC__ "PostDeferredCallBkTreeStation3"
void CVICALLBACK PostDeferredCallBkTreeStation3(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputStation3)
	{
		i = 0;
		while(0 != strlen(gs_pInputStation3[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation3, 
														 NULL,  
														 gs_pInputStation3[i].keyNode, 
														 "state", 
														 gs_pInputStation3[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__													  
#define __FUNC__ "PostDeferredCallBkTreeStation4"
void CVICALLBACK PostDeferredCallBkTreeStation4(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputStation4)
	{
		i = 0;
		while(0 != strlen(gs_pInputStation4[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation4, 
														 NULL,  
														 gs_pInputStation4[i].keyNode, 
														 "state", 
														 gs_pInputStation4[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__													  
#define __FUNC__ "PostDeferredCallBkTreeStation5"
void CVICALLBACK PostDeferredCallBkTreeStation5(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputStation5)
	{
		i = 0;
		while(0 != strlen(gs_pInputStation5[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation5, 
														 NULL,  
														 gs_pInputStation5[i].keyNode, 
														 "state", 
														 gs_pInputStation5[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__													  
#define __FUNC__ "PostDeferredCallBkTreeStation6"
void CVICALLBACK PostDeferredCallBkTreeStation6(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputStation6)
	{
		i = 0;
		while(0 != strlen(gs_pInputStation6[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation6, 
														 NULL,  
														 gs_pInputStation6[i].keyNode, 
														 "state", 
														 gs_pInputStation6[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__													  
#define __FUNC__ "PostDeferredCallBkTreeStation8"
void CVICALLBACK PostDeferredCallBkTreeStation8(void *callbackData)
{
	SElExceptionPtr	pexception = NULL;
	HRESULT			hr;
	int32_t			i;
	uint32_t		state;
	
	if(gs_pInputStation8)
	{
		i = 0;
		while(0 != strlen(gs_pInputStation8[i].keyNode))
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemImage(gs_hTreeStation8, 
														 NULL,  
														 gs_pInputStation8[i].keyNode, 
														 "state", 
														 gs_pInputStation8[i].state));
			i++;
		};
	}

Error:
	EXCDISPLAY( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcStation1"
static DWORD WINAPI ThreadProcStation1(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	HRESULT			hr;
	bool_t			data_changed = FALSE;

	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_STATION_1], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeStation1, NULL));
			data_changed = FALSE;
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcStation2"
static DWORD WINAPI ThreadProcStation2(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	HRESULT			hr;
	bool_t			data_changed = FALSE;

	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_STATION_2], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeStation2, NULL));
			data_changed = FALSE;
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcStation7"
static DWORD WINAPI ThreadProcStation7(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	HRESULT			hr;
	bool_t			data_changed = FALSE;

	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_STATION_7], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeStation7, NULL));
			data_changed = FALSE;
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcStation3"
static DWORD WINAPI ThreadProcStation3(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	HRESULT			hr;
	bool_t			data_changed = FALSE;

	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_STATION_3], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeStation3, NULL));
			data_changed = FALSE;
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcStation4"
static DWORD WINAPI ThreadProcStation4(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	HRESULT			hr;
	bool_t			data_changed = FALSE;

	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_STATION_4], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeStation4, NULL));
			data_changed = FALSE;
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcStation5"
static DWORD WINAPI ThreadProcStation5(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	HRESULT			hr;
	bool_t			data_changed = FALSE;

	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_STATION_5], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeStation5, NULL));
			data_changed = FALSE;
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcStation6"
static DWORD WINAPI ThreadProcStation6(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	HRESULT			hr;
	bool_t			data_changed = FALSE;

	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_STATION_6], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeStation6, NULL));
			data_changed = FALSE;
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ThreadProcStation8"
static DWORD WINAPI ThreadProcStation8(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLinePtr		pInput = (SLinePtr)param;
	int32_t			i = 0;
	uint32_t		state;
	HRESULT			hr;
	bool_t			data_changed = FALSE;

	while ( TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop[THREAD_STATION_8], 500) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		i = 0;
		while(0 != strlen(pInput[i].keyNode))
		{
			EXCCHECK( gs_pSensors->GetState(gs_pSensors, pInput[i].channel, &state));

			if(pInput[i].state != state)
			{
				pInput[i].state = state;
				data_changed = TRUE;
			}
			i++;
		};

		if( data_changed)
		{
			EXCCHECKCVI( PostDeferredCall( PostDeferredCallBkTreeStation8, NULL));
			data_changed = FALSE;
		}
	};

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ChangeMode"
static SElExceptionPtr ChangeMode(EMode mode)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	
	switch(gs_mode)
	{
		case MODE_TESTER:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_TESTER], &gs_hEventStop[THREAD_TESTER]));
			break;
		case MODE_STATION_1:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_1], &gs_hEventStop[THREAD_STATION_1]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation1, NULL));   
			break;
		case MODE_STATION_2:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_2], &gs_hEventStop[THREAD_STATION_2]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation2, NULL));   
			break;
		case MODE_STATION_7:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_7], &gs_hEventStop[THREAD_STATION_7]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation7, NULL));   
			break;
		case MODE_STATION_3:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_3], &gs_hEventStop[THREAD_STATION_3]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation3, NULL));   
			break;
		case MODE_STATION_4:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_4], &gs_hEventStop[THREAD_STATION_4]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation4, NULL));   
			break;
		case MODE_STATION_5:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_5], &gs_hEventStop[THREAD_STATION_5]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation5, NULL));   
			break;
		case MODE_STATION_6:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_6], &gs_hEventStop[THREAD_STATION_6]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation6, NULL));   
			break;
		case MODE_STATION_8:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_8], &gs_hEventStop[THREAD_STATION_8]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation8, NULL));   
			break;
	}

	switch(mode)
	{
		case MODE_TESTER:
			EXCCHECK( ThreadStart( ThreadProcTester, gs_pInputTester, &gs_hThread[THREAD_TESTER], &gs_hEventStop[THREAD_TESTER])); 
			break;
		case MODE_STATION_1:
			if(gs_pInputStation1)
			{
				EXCCHECK( ThreadStart( ThreadProcStation1, gs_pInputStation1, &gs_hThread[THREAD_STATION_1], &gs_hEventStop[THREAD_STATION_1]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation1, NULL)); 
			break;
		case MODE_STATION_2:
			if(gs_pInputStation2)
			{
				EXCCHECK( ThreadStart( ThreadProcStation2, gs_pInputStation2, &gs_hThread[THREAD_STATION_2], &gs_hEventStop[THREAD_STATION_2]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation2, NULL)); 
			break;
		case MODE_STATION_7:
			if(gs_pInputStation7)
			{
				EXCCHECK( ThreadStart( ThreadProcStation7, gs_pInputStation7, &gs_hThread[THREAD_STATION_7], &gs_hEventStop[THREAD_STATION_7]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation7, NULL)); 
			break;
		case MODE_STATION_3:
			if(gs_pInputStation3)
			{
				EXCCHECK( ThreadStart( ThreadProcStation3, gs_pInputStation3, &gs_hThread[THREAD_STATION_3], &gs_hEventStop[THREAD_STATION_3]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation3, NULL)); 
			break;
		case MODE_STATION_4:
			if(gs_pInputStation4)
			{
				EXCCHECK( ThreadStart( ThreadProcStation4, gs_pInputStation4, &gs_hThread[THREAD_STATION_4], &gs_hEventStop[THREAD_STATION_4]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation4, NULL)); 
			break;
		case MODE_STATION_5:
			if(gs_pInputStation5)
			{
				EXCCHECK( ThreadStart( ThreadProcStation5, gs_pInputStation5, &gs_hThread[THREAD_STATION_5], &gs_hEventStop[THREAD_STATION_5]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation5, NULL)); 
			break;
		case MODE_STATION_6:
			if(gs_pInputStation6)
			{
				EXCCHECK( ThreadStart( ThreadProcStation6, gs_pInputStation6, &gs_hThread[THREAD_STATION_6], &gs_hEventStop[THREAD_STATION_6]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation6, NULL)); 
			break;
		case MODE_STATION_8:
			if(gs_pInputStation8)
			{
				EXCCHECK( ThreadStart( ThreadProcStation8, gs_pInputStation8, &gs_hThread[THREAD_STATION_8], &gs_hEventStop[THREAD_STATION_8]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation8, NULL)); 
			break;
	}
		
	gs_mode = mode;

Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
static SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	int32_t          error = 0;
	SElExceptionPtr  pexception = NULL;

	if(0==strcmp(eventName, "EVNT_GUI_TESTER"))
	{
		EXCCHECK(ChangeMode(MODE_TESTER));
	}
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_1"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_1));	
	}
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_2"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_2));	
	}
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_7"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_7));	
	}
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_3"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_3));	
	}
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_4"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_4));	
	}
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_5"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_5));	
	}
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_6"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_6));	
	}
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_8"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_8));	
	}


Error:      
   EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
   int            error = 0;
   SElExceptionPtr   pexception = NULL;
   
   if ( gs_EventFactory )
   {
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_STATION_1",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_STATION_2",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_STATION_7",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_STATION_3",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_STATION_4",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_STATION_5",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_STATION_6",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_STATION_8",
                                         EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
                                         "EVNT_GUI_TESTER_PANEL",
                                         EventFunctionCallback));
   }
   
Error:
   EXCRETHROW(pexception);
}   /* RegisterEventFunction */

/*---------------------------------------------------------------------------*/
#if defined(_CVI_DLL_) || \
	defined(_WINDLL)
void DiscardUIObjectsForDLL (void)
{
   /* Discard the panels loaded in InitUIForDLL */
}

int __stdcall DllMain(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved
)
{
   switch (fdwReason)
      {
      case DLL_PROCESS_ATTACH:
#if (!defined(_CVI_)&&defined(_DEBUG))
		//_CrtSetBreakAlloc(156);
#endif
         if (InitCVIRTE (hinstDLL, 0, 0) == 0)      
            return 0;   /* out of memory */
         break;
      case DLL_PROCESS_DETACH:
         if (!CVIRTEHasBeenDetached())      /* Do not call CVI functions if cvirte.dll has already been detached. */
         {
            DiscardUIObjectsForDLL ();      /* Discard the panels loaded in InitUIForDLL */
            CloseCVIRTE ();      
         }
#if !defined(_CVI_) && defined(_DEBUG) && defined(DUMP_MEMORY_LEAKS)
			_CrtDumpMemoryLeaks();
#endif
         break;
      }

   return 1;
}

int __stdcall DllEntryPoint(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved
)
{
   /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}

#undef __FUNC__
#define __FUNC__ "{Tester Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr  	pexception = NULL;    
	int32_t         	error = 0;
	int32_t         	main_panel = 0;
	int            		monitor_id, top, height,width;  
	HRESULT				hr;
   
	gs_PluginArgs = a_pPluginArgs;

   	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
                            "MaintenancePanel",
                            sizeof(main_panel),
                            &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "WindowHandle",
									 sizeof(hWnd),
									 &hWnd));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "Modules",
	                        sizeof(SObjListPtr),
	                        &gs_Modules));

   	/* gettext */
	if(gs_Modules)
	{
		gs_pLanguage = (SLanguagePtr)gs_Modules->Get(gs_Modules, MODULE_LANGUAGE);  
		if(gs_pLanguage)
		{
			char pathname[MAX_PATHNAME_LEN];
			EXCCHECKCVI( GetProjectDir( pathname));
			strcat(pathname, "\\lang");
			LANGUAGE_PTR->bindtextdomain (LANGUAGE_PTR, PACKAGE, pathname);
			LANGUAGE_PTR->textdomain (LANGUAGE_PTR, PACKAGE);
		}
	}
	
	if(gs_Modules)
		gs_IviBridge = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
	
	if (gs_IviBridge)
	{
		char file_name[1024] = "";

		EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "DAQ", &gs_DAQ));
		EXCCHECK( gs_IviBridge->GetSession( gs_IviBridge, "DAQ2", &gs_DAQ_2)); 
	}
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "tester_gui.uir", TESTER, TESTERlang, &gs_panel[TESTER])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_1_gui.uir", STATION_1, STATION_1_lang, &gs_panel_1[STATION_1])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_2_gui.uir", STATION_2, STATION_2_lang, &gs_panel_2[STATION_2])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_3_gui.uir", STATION_3, STATION_3_lang, &gs_panel_3[STATION_3])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_4_gui.uir", STATION_4, STATION_4_lang, &gs_panel_4[STATION_4])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_5_gui.uir", STATION_5, STATION_5_lang, &gs_panel_5[STATION_5]));
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_6_gui.uir", STATION_6, STATION_6_lang, &gs_panel_6[STATION_6])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_7_gui.uir", STATION_7, STATION_7_lang, &gs_panel_7[STATION_7])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_8_gui.uir", STATION_8, STATION_8_lang, &gs_panel_8[STATION_8])); 

	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "TesterPanel", ARG_INT32, &gs_panel[TESTER])); 
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_1_Panel", ARG_INT32, &gs_panel_1[STATION_1]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_2_Panel", ARG_INT32, &gs_panel_2[STATION_2]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_3_Panel", ARG_INT32, &gs_panel_3[STATION_3]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_4_Panel", ARG_INT32, &gs_panel_4[STATION_4]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_5_Panel", ARG_INT32, &gs_panel_5[STATION_5]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_6_Panel", ARG_INT32, &gs_panel_6[STATION_6]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_7_Panel", ARG_INT32, &gs_panel_7[STATION_7]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_8_Panel", ARG_INT32, &gs_panel_8[STATION_8]));

	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[TESTER], TESTER_BKTREE, &gs_hTreeTester) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_1[STATION_1], STATION_1_BKTREE, &gs_hTreeStation1) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_2[STATION_2], STATION_2_BKTREE, &gs_hTreeStation2) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_3[STATION_3], STATION_3_BKTREE, &gs_hTreeStation3) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_4[STATION_4], STATION_4_BKTREE, &gs_hTreeStation4) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_5[STATION_5], STATION_5_BKTREE, &gs_hTreeStation5) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_6[STATION_6], STATION_6_BKTREE, &gs_hTreeStation6) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_7[STATION_7], STATION_7_BKTREE, &gs_hTreeStation7) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_8[STATION_8], STATION_8_BKTREE, &gs_hTreeStation8) );

	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[TESTER], TESTER_IMAGELIST, &gs_hImageList) );


	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeTester, BkTreeNodeClickTester, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeStation1, BkTreeNodeClickStation1, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeStation2, BkTreeNodeClickStation2, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeStation3, BkTreeNodeClickStation3, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeStation4, BkTreeNodeClickStation4, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeStation5, BkTreeNodeClickStation5, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeStation6, BkTreeNodeClickStation6, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeStation7, BkTreeNodeClickStation7, NULL, 1, NULL));
	EXCCHECKCOM( BKLib2__DBKTreeEventsRegOnNodeClick (gs_hTreeStation8, BkTreeNodeClickStation8, NULL, 1, NULL));


	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeTester, NULL, gs_hImageList));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeTester, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeStation1, NULL, gs_hImageList));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeStation2, NULL, gs_hImageList));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeStation3, NULL, gs_hImageList));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeStation4, NULL, gs_hImageList));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeStation5, NULL, gs_hImageList));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeStation6, NULL, gs_hImageList));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeStation7, NULL, gs_hImageList));
	EXCCHECKCOM( BKLib2__DBKTreeSetImageList (gs_hTreeStation8, NULL, gs_hImageList));

	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeStation1, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeStation2, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeStation3, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeStation4, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeStation5, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeStation6, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeStation7, NULL, VFALSE));
	EXCCHECKCOM( BKLib2__DBKTreeSetAutoImages (gs_hTreeStation8, NULL, VFALSE));


	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP, &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_WIDTH, &width)); 
   
	EXCCHECKCVI( SetPanelAttribute (gs_panel[TESTER], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[TESTER], ATTR_LEFT, 0));
	/********************************************************************///modifier le 05/12
	EXCCHECKCVI( SetPanelAttribute (gs_panel[TESTER], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[TESTER], ATTR_WIDTH, width));
	/********************************************************************/ //fin modification
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_1[STATION_1], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_1[STATION_1], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_1[STATION_1], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_1[STATION_1], ATTR_WIDTH, width));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_2[STATION_2], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_2[STATION_2], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_2[STATION_2], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_2[STATION_2], ATTR_WIDTH, width));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_3], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_3], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_3], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_3], ATTR_WIDTH, width));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_4], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_4], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_4], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_4], ATTR_WIDTH, width));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_5], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_5], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_5], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_5], ATTR_WIDTH, width));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_6], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_6], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_6], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_3[STATION_6], ATTR_WIDTH, width));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_7[STATION_7], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_7[STATION_7], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_7[STATION_7], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_7[STATION_7], ATTR_WIDTH, width));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_8[STATION_8], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_8[STATION_8], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_8[STATION_8], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_8[STATION_8], ATTR_WIDTH, width));
	
	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "EventFactory",
	                        sizeof(SElEventFactoryPtr),
	                        &gs_EventFactory));

	if(gs_Modules)
	{
		char uuid[65];

		gs_pTester = (STesterPtr)gs_Modules->Get(gs_Modules, MODULE_TESTER); 
		gs_pSensors = (SDIOSensorsPtr)gs_Modules->Get(gs_Modules, MODULE_SENSORS);
		gs_paxiss = (SaxissPtr)gs_Modules->Get(gs_Modules, MODULE_AXIS);  
		gs_pProfilometers = (SProfilometersPtr)gs_Modules->Get(gs_Modules, MODULE_PROFILOMETERS);  


		if(gs_pTester)
		{
			EXCCHECK(SetTree(gs_pTester->path, MODULE_TESTER, gs_hTreeTester, &gs_pInputTester, &gs_pOutputTester));
			EXCCHECK(SetTreeStations(gs_pTester->path, MODULE_TESTER, gs_hTreeStation1, &gs_pInputStation1, &gs_pOutputStation1,1));
			EXCCHECK(SetTreeStations(gs_pTester->path, MODULE_TESTER, gs_hTreeStation2, &gs_pInputStation2, &gs_pOutputStation2,2));
			EXCCHECK(SetTreeStations(gs_pTester->path, MODULE_TESTER, gs_hTreeStation3, &gs_pInputStation3, &gs_pOutputStation3,3));
			EXCCHECK(SetTreeStations(gs_pTester->path, MODULE_TESTER, gs_hTreeStation4, &gs_pInputStation4, &gs_pOutputStation4,4));
			EXCCHECK(SetTreeStations(gs_pTester->path, MODULE_TESTER, gs_hTreeStation5, &gs_pInputStation5, &gs_pOutputStation5,5));
			EXCCHECK(SetTreeStations(gs_pTester->path, MODULE_TESTER, gs_hTreeStation6, &gs_pInputStation6, &gs_pOutputStation6,6));
			EXCCHECK(SetTreeStations(gs_pTester->path, MODULE_TESTER, gs_hTreeStation7, &gs_pInputStation7, &gs_pOutputStation7,7));
			EXCCHECK(SetTreeStations(gs_pTester->path, MODULE_TESTER, gs_hTreeStation8, &gs_pInputStation8, &gs_pOutputStation8,8));

		}
	}
	
	EXCCHECK( RegisterEventFunction());  

Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Tester Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	if( 0==strcmp( uuid, PLUGIN_TESTER))
	{
		EXCCHECK( ShowPanel());
	}
				
Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Tester Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
   
	EXCCHECK( ClosePanel());

Error:   
   EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{Tester Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Tester Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif 
 
/*---------------------------------------------------------------------------*/
void CVICALLBACK PostDeferredDisplayPanel2 (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		DisplayPanel(*panel);
}

/*---------------------------------------------------------------------------*/
void CVICALLBACK PostDeferredHidePanel2 (void *callbackData) 
{ 
	int *panel = (int*)callbackData;
	if(*panel)
		HidePanel(*panel);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ChangeMode_St4"
static SElExceptionPtr ChangeMode_St4 (int32_t mode)
{
	SElExceptionPtr 	pexception = NULL;  
	int32_t				error = 0;
	static enum _EDisplay	last_mode;
	
	switch(last_mode)
	{
		case IO:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel2, &gs_panel_4[IO]));
			break;
		case PANEL:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel2, &gs_panel_4[PANEL]));   
			break;
		case FORCE:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel2, &gs_panel_4[FORCE]));   
			break;
		case GAP:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel2, &gs_panel_4[GAP]));   
			break;
	}
	
	switch(mode)
	{
		case IO:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel2, &gs_panel_4[IO]));
			break;
		case PANEL:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel2, &gs_panel_4[PANEL]));   
			break;
		case FORCE:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel2, &gs_panel_4[FORCE]));   
			break;
		case GAP:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel2, &gs_panel_4[GAP]));   
			break;
	}
	
	last_mode = mode;

Error:      
   EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/

int CVICALLBACK OnDisplay_St4 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL; 
	int32_t				error = 0;
	int					mode;
	
	switch (event)
	{
		case EVENT_COMMIT:

			
			EXCCHECKCVI( GetCtrlVal(panel, control, &mode));
			EXCCHECK( ChangeMode_St4( mode));	
		
			break;
	}
	
Error:
	EXCDISPLAY( pexception);   
	return 0;
}
/*----------------------------------------------------------------------*/
#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2

typedef enum _EStep
{
	AXIS_Z_HOME = 0,
	AXIS_X_Y_HOME,
	END,
} EStep;

int CVICALLBACK OnFncStation4 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0,i=0;
	int                 push;
	int                 microCam;
 	HANDLE  			hThread;

	
	switch (event)
	{
		case EVENT_COMMIT:

			switch(control)
			{
				case STATION_4_SELECT_VS11:
				
					pexception = gs_pTester->SelectVS11Station40(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_SELECT_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_SELECT_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_SELECT_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
					
				case STATION_4_SELECT_X52:
				
					pexception = gs_pTester->SelectX52Station40(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_SELECT_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_SELECT_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_SELECT_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_4_PLUG_X52:
				
					pexception = gs_pTester->PlugPanelStation40_X52(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_PLUG_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_PLUG_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_UNPLUG_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_4_UNPLUG_X52:
				
					pexception = gs_pTester->UnplugPanelStation40_X52(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_UNPLUG_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_UNPLUG_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_PLUG_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_4_PLUG_VS11:
				
					pexception = gs_pTester->PlugPanelStation40_VS11(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_PLUG_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_PLUG_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_UNPLUG_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_4_UNPLUG_VS11:
				
					pexception = gs_pTester->UnplugPanelStation40_VS11(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_UNPLUG_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_UNPLUG_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_LED_PLUG_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_4_RUN:
					
					EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &microCam));
					
				  	if (microCam)
						EXCCHECK(ThreadStart(Thread_Test, (void*)40, &hThread, NULL));  
							
						
				  
					else 
						EXCTHROW( -1, "Plateau pas en position!");
				
				break;
				

			}
		break;
	}
			
Error:
	EXCDISPLAY( pexception);
	return 0;
}

int CVICALLBACK OnFncStation5 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
	int                 push;
	int                 microCam;
 	HANDLE  			hThread;
	
	switch (event)
	{
		case EVENT_COMMIT:

			switch(control)
			{
				case STATION_5_SELECT_VS11:
				
					pexception = gs_pTester->SelectVS11Station50(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_SELECT_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_SELECT_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_SELECT_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
					
				case STATION_5_SELECT_X52:
				
					pexception = gs_pTester->SelectX52Station50(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_SELECT_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_SELECT_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_SELECT_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_5_PLUG_X52:
				
					pexception = gs_pTester->PlugPanelStation50_X52(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_PLUG_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_PLUG_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_UNPLUG_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_5_UNPLUG_X52:
				
					pexception = gs_pTester->UnplugPanelStation50_X52(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_UNPLUG_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_UNPLUG_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_PLUG_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
					case STATION_5_PLUG_VS11:
				
					pexception = gs_pTester->PlugPanelStation50_VS11(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_PLUG_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_PLUG_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_UNPLUG_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_5_UNPLUG_VS11:
				
					pexception = gs_pTester->UnplugPanelStation50_VS11(gs_pTester, 6000);  
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_UNPLUG_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_UNPLUG_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_5_LED_PLUG_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;	
					
				
				case STATION_5_RUN:
					
					EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &microCam));
					
				  	if (microCam)
						EXCCHECK(ThreadStart(Thread_Test, (void*)50, &hThread, NULL)); 
				  
					else 
						EXCTHROW( -1, "Plateau pas en position!");
				
				break;
				

			}
		break;
	}
			
Error:
	EXCDISPLAY( pexception);
	return 0;
}

int CVICALLBACK OnFncStation6 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
	int                 push;
	int                 output0,
		                output1,
		                output2,
		                output3;
	int                 microCam;
 	HANDLE  			hThread;
	
	switch (event)
	{
		case EVENT_COMMIT:

			switch(control)
			{
				case STATION_6_SELECT_VS11:
				
					pexception = gs_pTester->SelectVS11Station60(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_SELECT_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_SELECT_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_SELECT_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
					
				case STATION_6_SELECT_X52:
				
					pexception = gs_pTester->SelectX52Station60(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_SELECT_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_SELECT_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_SELECT_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_6_PLUG_X52:
				
					pexception = gs_pTester->PlugPanelStation60_X52(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_PLUG_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_PLUG_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_UNPLUG_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_6_UNPLUG_X52:
				
					pexception = gs_pTester->UnplugPanelStation60_X52(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_UNPLUG_X52,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_UNPLUG_X52,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_PLUG_X52, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_6_PLUG_VS11:
				
					pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_PLUG_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_PLUG_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_UNPLUG_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
				case STATION_6_UNPLUG_VS11:
				
					pexception = gs_pTester->UnplugPanelStation60_VS11(gs_pTester, 6000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_UNPLUG_VS11,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_UNPLUG_VS11,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_PLUG_VS11, ATTR_FRAME_COLOR, VAL_WHITE));
					
				break;
				
//				case STATION_6_TRIGGER_CHECKER_RIGHT:
//					
//				pexception = gs_pTester->TestChecker(gs_pTester, CHECKER_RIGHT, &output0, &output1, &output2, &output3);
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_RIGHT_OUT0,ATTR_FRAME_COLOR, output0? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_RIGHT_OUT1,ATTR_FRAME_COLOR, output1? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_RIGHT_OUT2,ATTR_FRAME_COLOR, output2? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_RIGHT_OUT3,ATTR_FRAME_COLOR, output3? VAL_RED : VAL_WHITE));
//				ProcessSystemEvents ();
//				
//				break;
//				
//				case STATION_6_TRIGGER_CHECKER_LEFT:
//					
//				pexception = gs_pTester->TestChecker(gs_pTester, CHECKER_LEFT, &output0, &output1, &output2, &output3);
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_LEFT_OUT0,ATTR_FRAME_COLOR, output0? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_LEFT_OUT1,ATTR_FRAME_COLOR, output1? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_LEFT_OUT2,ATTR_FRAME_COLOR, output2? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_LEFT_OUT3,ATTR_FRAME_COLOR, output3? VAL_RED : VAL_WHITE));
//				ProcessSystemEvents ();
//				
//				break;
//				
//				case STATION_6_TRIGGER_CHECKER_CENT:
//					
//				pexception = gs_pTester->TestChecker(gs_pTester, CHECKER_CENTRAL, &output0, &output1, &output2, &output3);
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_CENTRAL_OUT0,ATTR_FRAME_COLOR, output0? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_CENTRAL_OUT1,ATTR_FRAME_COLOR, output1? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_CENTRAL_OUT2,ATTR_FRAME_COLOR, output2? VAL_GREEN : VAL_WHITE));
//				EXCCHECKCVI( SetCtrlAttribute(panel, STATION_6_LED_CENTRAL_OUT3,ATTR_FRAME_COLOR, output3? VAL_RED : VAL_WHITE));
//				ProcessSystemEvents ();
//				
//				break;
				
		    	case STATION_6_RUN:
					
					EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &microCam));
					
				  	if (microCam)
						EXCCHECK(ThreadStart(Thread_Test, (void*)60, &hThread, NULL)); 
				  
					else 
						EXCTHROW( -1, "Plateau pas en position!");
				
				break;
				
			}
		break;
	}
			
Error:
	EXCDISPLAY( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnFncStation7"
int CVICALLBACK OnFncStation7 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
	int                 microCam;
 	HANDLE  			hThread;
		
	switch (event)
		{
		case EVENT_COMMIT:
			
				case STATION_7_PRINT:
				
					pexception = gs_pTester->CyclePrint(gs_pTester, 15000);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_7_LED_PRINT,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_7_LED_PRINT,ATTR_FRAME_COLOR, VAL_RED));
						ProcessSystemEvents ();
					}
				break;
				
				
				  	case STATION_7_READY:
				
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_7_LED_READY,ATTR_FRAME_COLOR, VAL_WHITE));

					pexception = gs_pTester->ReadyPrint(gs_pTester, 3000);

					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_7_LED_READY,ATTR_FRAME_COLOR, VAL_GREEN));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_7_LED_READY,ATTR_FRAME_COLOR, VAL_RED));
					}
					
				break;  
				
					
			
		break;

		}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/



int CVICALLBACK OnFncStation8 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
	int                 push;
	int                 microCam;
 	HANDLE  			hThread;
	
	switch (event)
	{
		case EVENT_COMMIT:

			switch(control)
			{
				case STATION_8_PICK_PLACE:
				
					pexception = gs_pTester->PickPlace(gs_pTester, 8000, TRUE);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_PICK_PLACE,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_RESET_PICK_PLACE,ATTR_FRAME_COLOR, VAL_WHITE)); 
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_PICK_PLACE,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
				case STATION_8_RESET_PICK_PLACE:
				
					pexception = gs_pTester->ResetPickPlace(gs_pTester, 4000, TRUE);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_RESET_PICK_PLACE,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_PICK_PLACE,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_RESET_PICK_PLACE,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
				case STATION_8_MOVE_X_AVANT:
				
					pexception = gs_pTester->MoveXFront(gs_pTester, 4000);	  //pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 4000);   
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_MOVE_FRONT,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_MOVE_REAR,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_MOVE_FRONT,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
				case STATION_8_MOVE_X_ARRIERE:
				
					pexception = gs_pTester->MoveXRear(gs_pTester, 4000);	  //pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 4000);   
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_MOVE_REAR,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_MOVE_FRONT,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_MOVE_REAR,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
				case STATION_8_MOVE_Z_UP:
				
					pexception = gs_pTester->MoveZUp(gs_pTester, 4000);	  //pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 4000);   
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_Z_UP,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_Z_DOWN,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_Z_UP,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
				case STATION_8_MOVE_Z_DOWN:
				
					pexception = gs_pTester->MoveZDown(gs_pTester, 4000);	  //pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 4000);   
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_Z_DOWN,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_Z_UP,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_Z_DOWN,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
				case STATION_8_OPEN_PINCE:
				
					pexception = gs_pTester->OpenPince(gs_pTester, 4000);	  //pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 4000);   
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_OPEN_PINCE,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_CLOSE_PINCE,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_OPEN_PINCE,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
					case STATION_8_CLOSE_PINCE:
				
					pexception = gs_pTester->ClosePince(gs_pTester, 4000);	  //pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 4000);   
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_CLOSE_PINCE,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_OPEN_PINCE,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_CLOSE_PINCE,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
					case STATION_8_LOCK:
				
					pexception = gs_pTester->LockPanelStation80(gs_pTester, 4000);	  //pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 4000);   
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_LOCK,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_UNLOCK,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_LOCK,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
				case STATION_8_UNLOCK:
				
					pexception = gs_pTester->UnlockPanelStation80(gs_pTester, 4000);	  //pexception = gs_pTester->PlugPanelStation60_VS11(gs_pTester, 4000);   
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_UNLOCK,ATTR_FRAME_COLOR, VAL_GREEN));
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_LOCK,ATTR_FRAME_COLOR, VAL_WHITE));
						ProcessSystemEvents ();
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_8_LED_UNLOCK,ATTR_FRAME_COLOR, VAL_RED));
					}
				break;
				
				case STATION_8_BARCODE:
						
					DeleteTextBoxLines (panel, STATION_8_CODE_STRING, 0, -1);
					pexception = gs_pTester->GetItemCode(gs_pTester, 0, 2, 512, buffer, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlVal(panel, STATION_8_CODE_STRING, buffer));
					}
					else
					{
						EXCDISPLAY( pexception); 
						EXCCHECKCVI( SetCtrlVal(panel, STATION_8_CODE_STRING, "#error"));
					}
					
				break;
				
				
			}
		break;
	}
			
Error:
	EXCDISPLAY( pexception);
	return 0;
}
