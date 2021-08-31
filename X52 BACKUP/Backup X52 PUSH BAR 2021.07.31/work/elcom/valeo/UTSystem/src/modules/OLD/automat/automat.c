#include <labels/tester_labels.h>
#include <test/test.h>
#include <multithread/multithread.h>
#include <language/language.h>
#include <utility.h>
#include <stdio.h>
#include <dbs/dbs.h>
#include <box/box.h>
#include <tester/tester.h>
#include <printer/class/printers.h>
#include <Camera/class/cameras.h>
#include "automat.h"
#include "automat_fnc.h"
#include "automat_plugin.h" 
#include "eldaq.h"
#include <rs232.h>
#include <formatio.h>
#include "toolbox.h"
#include <eldaq/class/eldaq.h>
#include <IviBridge/IviBridge.h>  
#include <local.h>
#include <NIDAQmx.h>
#include <cviauto.h>
#include <cvirte.h>
#include <export.h>
#include <analysis.h>
#include <cvidef.h>

#include <guid.h>
#include <jig/class/jig_class.h>
#include <bklib/bklib2.h>
#include <ansi_c.h>
#include <test/testseq.h>
#include "toolbox.h"



#define PORTDOUCHETTE 	22
#define PORTCIM		  	10

#define CAMERA_PICTO	0
#define CAMERA_LED		1


#define ROTATION        1 


#define _CHANGE_VERSION_NO_EMPTY	0 /* 1 no empty */

#define OK						0 
#define NOK						1 
#define RUN						2 
#define FREE					3 

//#define PATH_PRODUCTION					"%s\\imprimante\\%04d.%02d.%02d"

char				TicketDirPath[1024];
char                Production_Serial[4+1];
char				Time[32];
char				Date[32];
char				TicketFilePath[1024];


const char* HISTORY = \
"[2008-05-27] rT 1.10\n"
" * ProcessEventGuiProductSelected edited. No master part after pause is needed.\n"
"[2007-12-13] rT 1.10\n"
" * Function LoadTestSequence was edited. Reason was a application freezing.\n"
"[2007-12-15] rT 1.20\n"
" * Retest panel algorithm edited\n";


clock_t _start_cycle_clock;
double time_cycle;

static ViSession			gs_DAQ = 1;
static ViSession			gs_DAQ_2 = 1; 

/* error handling */
#define EXCCHECKIVIDAQ( fCall) \
	EXCCHECKIVI(eldaq, gs_DAQ, fCall)

#define EXCCHECKIVIDAQ_2( fCall) \
	EXCCHECKIVI(eldaq, gs_DAQ_2, fCall)

enum {
	EVNT_STOP_THREAD = 0,
	EVNT_GUI_PRODUCTION,
	EVNT_GUI_QUALITY_RELEASE,
	EVNT_GUI_QUALITY_RETEST,
	EVNT_GUI_QUALITY_MASTER,
	EVNT_GUI_EXPERTISE_STEP_MODE,
	EVNT_GUI_EXPERTISE_GO_MODE,
	EVNT_GUI_EXPERTISE_SEQUENCE_CREATED,
	EVNT_GUI_EXPERTISE_RNR,
	EVNT_GUI_PRODUCT_SELECTED,
	EVNT_TEST_SEQUENCE_RUN,
	EVNT_EXPERTISE_STEP_MODE_NEXT,
	EVNT_EXPERTISE_STEP_MODE_REPEAT,
	EVNT_EXPERTISE_STEP_MODE_BACK,
	EVNT_EXPERTISE_STEP_MODE_STOP,
	EVNT_GUI_KEY_ESC_PRESSED,
	EVNT_RESET_PRODUCT,
	EVNT_JIG_ITEM_INSERTED_,
	EVNT_JIG_ITEM_RELEASED_,
	EVNT_JIG_CYCLE_START_,
	EVNT_JIG_START_,
	/* event count */
	EVENT_HANDLE_NB
};

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

#define MAX_NB_AI_CHANNELS 32

#define   SAMPLES 1
SCalibrationPtr	pScaleInfo[MAX_NB_AI_CHANNELS];


#define ON_LINE  				00
#define HEAD_OPEN_ONLINE		01
#define OPERTATING 				02
#define PAUSE	  				04
#define WAITING_FOR_STRIPPING  	05
#define COMMAND_ANALYSE_ERROR  	06
#define COMMS_ERROR  			07
#define PAPER_JAM  				11
#define CUTTER_ERROR		  	12
#define NO_PAPER		  		13
#define NO_RIBBON		  		14
#define HEAD_OPEN		  		15
#define HEAD_ERROR		  		17
#define EXCESS_HEAD_TEMP  		18
#define RIBBON_ERROR		  	21
#define REWIND_FULL			  	22


#define NB_OF_PANEL				1 
#define NUMBER_OF_STATIONS		8 



int						gs_panel[NB_OF_PANEL + 1];
static HANDLE			gs_EventHandle[EVENT_HANDLE_NB];
SElEventFactoryPtr		gs_EventFactory = NULL;  
static SObjListPtr 		gs_Modules = NULL; 
static SLabelListPtr	gs_pLabels = NULL;
static SDBSPtr			gs_pDbs = NULL;
static SBoxPtr			gs_pBox = NULL;
static STesterPtr		gs_pTester = NULL;
static SIviBridgePtr	gs_IviBridge = NULL;
static SPrintersPtr		gs_pPrinters = NULL;
static SCamerasPtr		gs_pcameras = NULL;
static SaxissPtr		gs_paxiss = NULL;
static SAutomat			gs_Automat;
static HANDLE			gs_hThread = NULL;
static HANDLE			hEventStop = NULL;
SLanguagePtr			gs_pLanguage = NULL; 
int 					gs_Table_Rotation;
int32_t					gs_RnR_Station;
int32_t					gs_RnR_Repeat;
char 					gs_RnR_Palett[10+1]; 
char 					gs_RnR_Piece[10+1];

int 					gs_PalettID; 

typedef struct _SBarcodeData{
	/* input data */
	uint32_t			  barcode_ID;
	/* returned data */
	char				  barcode[256];
	
} SBarcodeData, *SBarcodeDataPtr;
 
#define PANEL_MSG(control, msg) \
	do{ \
		if(gs_Automat.pTestSeq && control) \
			EXCCHECK( gs_Automat.pTestSeq->SetPanelDescription(gs_Automat.pTestSeq, control, msg)); \
	}while(0)

#define PANEL_TABLE_WARNING(control , status) \
	do{ \
		if(gs_Automat.pTestSeq) \
			EXCCHECK( gs_Automat.pTestSeq->SetPanelWarning(gs_Automat.pTestSeq, control, status)); \
	}while(0)

#define PANEL_STATION_STATUS(station , status) \
	do{ \
		if(gs_Automat.pTestSeq) \
			EXCCHECK( gs_Automat.pTestSeq->SetPanelStationStatus(gs_Automat.pTestSeq, station, status)); \
	}while(0)

#define PANEL_SET_REFERENCES(Refvaleo, Refcustomer) \
	do{ \
		if(gs_Automat.pTestSeq) \
			EXCCHECK( gs_Automat.pTestSeq->SetTestPanelReferences(gs_Automat.pTestSeq, Refvaleo, Refcustomer)); \
	}while(0)

#define PANEL_MODE(mode) \
	do{ \
		if(gs_Automat.pTestSeq) \
			EXCCHECK( gs_Automat.pTestSeq->SetPanelMode(gs_Automat.pTestSeq, E_TSPM_##mode)); \
	}while(0)

#define CHECK_ERR( is_Error) \
	if(is_Error!=0) goto Error; \
	else
 
#define CHECK_ERR2( is_Error) \
	if(is_Error2!=0) goto Error2; \
	else

#define INSERT_EMPTY_BOX() \
	EXCCHECK( gs_pBox->SetPanelsInBox(gs_pBox, gs_pBox->_PanelsInBox))

#define TIMEOUT_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > ((double)timeout/1000.0) ) { EXCTHROW( TEST_ERR_TIMEOUT, "Printer Status Timeout"); } else


#define UNLOCK_DELAY					500
#define TIMEOUT							2000
#define TIMEOUT_PRINTER					10000
#define COLDMARKING_TIMEOUT				6000
#define ROTATION_TIMEOUT				15000

#define MAX_MASTER_PARTS				50


	
static SElExceptionPtr ProcessEventMasterSequenceRun(bool_t Load_Params);
static SElExceptionPtr ProcessEventGuiProduction(void);
static SElExceptionPtr GetMasters(void);

static SElExceptionPtr JigPanelBarcodeHousing(struct _SSledge* me, int32_t* pError);
static SElExceptionPtr JigPanelBarcodeTopCover(struct _SSledge* me, int32_t* pError);
static SElExceptionPtr SelectProductType(struct _SSledge* me, int32_t* pError);
static SElExceptionPtr TestProductPresence(int32_t* pError, int32_t* insert1, int32_t* insert2); 			   
static SElExceptionPtr JigPanelInsert(int32_t* pError);
static SElExceptionPtr JigPanelInsert2(int32_t* pError);
static SElExceptionPtr JigPanelRelease(struct _SSledge* me, bool_t fault);
static SElExceptionPtr JigPanelReleaseFinal(struct _SSledge* me, bool_t fault); 
static SElExceptionPtr JigPanelPrinter(struct _SSledge* me, bool_t fault); 
static SElExceptionPtr JigPanelBarcode(struct _SSledge* me, int32_t* pError) ;
static SElExceptionPtr JigPanelRelease2(struct _SSledge* me, bool_t fault);
static SElExceptionPtr JigPanelLock(int32_t* pError);
static SElExceptionPtr JigPanelUnlock(int32_t* pError);

static void CVICALLBACK PostDeferredDisplayPanel (void *callbackData){ DisplayPanel(gs_panel[AUTOMAT]); }
static void CVICALLBACK PostDeferredHidePanel (void *callbackData){ HidePanel(gs_panel[AUTOMAT]); }

static DWORD WINAPI Thread_Sledge(LPVOID param);
static SElExceptionPtr Activate(struct _SSledge* me);
static SElExceptionPtr Deactivate(struct _SSledge* me);

CRITICAL_SECTION   _Lock;
int 			number_of_master_parts;
int 			Actual_master_part = 0;
int 			Finished_master_parts = 0;
int				NOK_Master_Parts = 0;
char			MastersBarcodes[MAX_MASTER_PARTS][512];
char			MastersDescription[MAX_MASTER_PARTS][512];
char			MastersName[MAX_MASTER_PARTS][512];
int				Status [MAX_MASTER_PARTS];
int 			MastersStations[MAX_MASTER_PARTS];


void CyclePrintStatus(int StatusOnline,int Interpreter);

#if 0    /* formerly excluded lines */
 /*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadJobCamera"
SElExceptionPtr LoadJobCamera (int Index, char *job)
{
SElExceptionPtr	pexception = NULL;
int32_t			error = 0;
SCameraPtr		pcamera = NULL;
char     		data[256];
HANDLE			hThread = NULL;
clock_t _start_test_clock;
double   time;
	
	
	EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, Index, &pcamera));
	
    EXCCHECK( pcamera->Cognex_disconnect(pcamera));
	EXCCHECK( pcamera->openTCPserver(pcamera));
	EXCCHECK( pcamera->Cognex_login(pcamera));
	EXCCHECK( pcamera->Cognex_Offline(pcamera));
	EXCCHECK( pcamera->Cognex_Job(pcamera, job));
    Sleep(500);
	EXCCHECK( pcamera->Cognex_disconnect(pcamera));
	EXCCHECK( pcamera->openTCPserver(pcamera));
	EXCCHECK( pcamera->Cognex_login(pcamera));
	EXCCHECK( pcamera->Cognex_Online(pcamera));
    Sleep(500);
	EXCCHECK( pcamera->Cognex_Trigger(pcamera));
    Sleep(200);
	EXCCHECK( pcamera->Cognex_Read(pcamera, data));
	ProcessSystemEvents ();
	EXCCHECK( pcamera->Cognex_Read(pcamera, data)); 
	
Error:
	EXCRETHROW( pexception);
}


#endif   /* formerly excluded lines */
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadJobCamera"
SElExceptionPtr LoadJobCamera (int Index, char *job)
{
SElExceptionPtr	pexception = NULL;
int32_t			error = 0;
SCameraPtr		pcamera = NULL;
char     		data[256];
HANDLE			hThread = NULL;
clock_t _start_test_clock;
double   time;
	

 EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, 0, &pcamera));
 EXCCHECK( pcamera->openTCPserver(pcamera));
 //Sleep(50);
	 
 EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, 0, &pcamera));
 EXCCHECK( pcamera->Cognex_login(pcamera));
 //Sleep(50);
	 
 EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, 0, &pcamera));
 EXCCHECK( pcamera->Cognex_Offline(pcamera));
 //Sleep(50);
	 
 EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, 0, &pcamera));
 EXCCHECK( pcamera->Cognex_Job(pcamera, job));
 //Sleep(50);

 EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, 0, &pcamera));
 EXCCHECK( pcamera->Cognex_Online(pcamera));
	
Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::DisplayWait"
SElExceptionPtr DisplayWait (void)
{
SElExceptionPtr	pexception = NULL;
int32_t			error = 0;
static int step = 0;

switch (step)
	{
	case 0 :	
		PANEL_MSG( PANEL_HELP2, _("Please Wait For The Test Finish"));
		step ++;
		Sleep (150);
	break;	
	case 1 :	
		PANEL_MSG( PANEL_HELP2, _(" Please Wait For The Test Finish."));
		step ++;
		Sleep (150);
	break;
	case 2 :	
		PANEL_MSG( PANEL_HELP2, _("  Please Wait For The Test Finish.."));
		step ++;
		Sleep (150);
	break;
	case 3 :	
		PANEL_MSG( PANEL_HELP2, _("   Please Wait For The Test Finish..."));
		step ++;
		Sleep (150);
	break;
	case 4 :	
		step = 0;
	break;
	}

Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CameraCheckJob"
SElExceptionPtr CameraCheckJob(char *job_database)
{
 int32_t   error = 0;
 SElExceptionPtr pexception = NULL;
 CAObjHandle  pnode = 0;
 HRESULT   hr;
 char   *ptext = NULL;
 SCameraPtr  pcamera = NULL;
 int32_t   job = 0;
 char       msg[128];
 clock_t	_start_clock ;
 clock_t _start_test_clock;
 double   time;
	
		
		_start_test_clock = clock();
		do{
			EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, 0, &pcamera));

			pcamera->Cognex_act_job(pcamera, &job);
		
			//ProcessTCPEvents ();
			DelayWithEventProcessing (0.2);
			
			time = (double)(clock()-_start_test_clock);
			
		}while ((job != atoi(job_database)) && (time < 8000));
		
 		if (job != atoi(job_database))
			 {
		 	sprintf (msg, "Erreur Fichier Job Camera \r\n JobCamera : Eclairage = %s \r\n [Job Actuel : %02d]\r\n VÃ©rifier que la CamÃ©ra est en Ligne", job_database, job);
	 		///printf ("%s \r\n", msg);
			EXCTHROW(-1, msg);
 		   	 }
  
Error: 
 EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadCameraPictos"
SElExceptionPtr LoadCameraPictos ()
{
SElExceptionPtr	pexception = NULL;
int32_t			error = 0;
SCameraPtr		pcamera = NULL;
char     		data[256];
HANDLE			hThread = NULL;
clock_t _start_test_clock;
double   time;
int i=0;
char char_array[64]= "BCDEFGH";	
	
	EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, 0, &pcamera));
	if (!strcmp(gs_Automat.pTestSeq->ProductType,"X52_7"))
	{  

		
		EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, 0));
		Sleep(1000);
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"C", gs_Automat.pTestSeq->Picto_Boutton_2));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"D", gs_Automat.pTestSeq->Picto_Boutton_3));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"E", gs_Automat.pTestSeq->Picto_Boutton_4));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"F", gs_Automat.pTestSeq->Picto_Boutton_5)); 
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"G", gs_Automat.pTestSeq->Picto_Boutton_6)); 

		
	}
	
	else if (!strcmp(gs_Automat.pTestSeq->ProductType,"X52_9"))
	{ 
		
		EXCCHECK( pcamera->Cognex_set_number_boutton(pcamera, 1));
		
	    Sleep(1000);
	    EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"B", gs_Automat.pTestSeq->Picto_Boutton_1));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"C", gs_Automat.pTestSeq->Picto_Boutton_2));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"D", gs_Automat.pTestSeq->Picto_Boutton_3));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"E", gs_Automat.pTestSeq->Picto_Boutton_4));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"F", gs_Automat.pTestSeq->Picto_Boutton_5)); 
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"G", gs_Automat.pTestSeq->Picto_Boutton_6));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"H", gs_Automat.pTestSeq->Picto_Boutton_7));
	}
	
	else if (!strcmp(gs_Automat.pTestSeq->ProductType,"VS11"))
	{     
	    EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"B", gs_Automat.pTestSeq->Picto_Boutton_1));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"C", gs_Automat.pTestSeq->Picto_Boutton_2));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"D", gs_Automat.pTestSeq->Picto_Boutton_3));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"E", gs_Automat.pTestSeq->Picto_Boutton_4));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"F", gs_Automat.pTestSeq->Picto_Boutton_5)); 
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"G", gs_Automat.pTestSeq->Picto_Boutton_6));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"H", gs_Automat.pTestSeq->Picto_Boutton_7));
		EXCCHECK( pcamera->Cognex_set_parameters(pcamera,"I", gs_Automat.pTestSeq->Picto_Boutton_8));
		
	}
	
Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelUnlock"
static SElExceptionPtr JigPanelUnlock(int32_t* pError)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	bool_t			    barrier, MicroCam;
	

		do{
		gs_pTester->GetSensor(gs_pTester, "INFRARED_BARRIER", &barrier);
		gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &MicroCam);
		ProcessSystemEvents ();
		ProcessDrawEvents ();
		}while(!barrier || !MicroCam);
				
		pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 7000);
		if(pexception)
		{
		EXCDELETE(pexception);
		if(pError) *pError = -1;
		CHECK_ERR( *pError);
		}
			
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelLock"
static SElExceptionPtr JigPanelLock(int32_t* pError)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	bool_t			    barrier;
	
		
		PANEL_MSG( PANEL_HELP2, _("Lock Panel"));
		
		pexception = gs_pTester->LockPanelStation10(gs_pTester, 7000);
		if(pexception)
		{	
		EXCDELETE(pexception);
		if(pError) *pError = -1;
		CHECK_ERR( *pError);
		}
			
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckDirectory"
void CheckDirectory(const char* directory)
{
	int					size;

	FileExists (directory, &size);
	if(size==-1)
	{
		/* create directory */	
		 MakeDir(directory);
	}
}
 
/*---------------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__	"CyclePrintStatus"
void CyclePrintStatus(int StatusOnline,int Interpreter)   
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
	
	
	GetLocalTime(&st);
	GetProjectDir(project_dir);
	sprintf(result_dir, "%s\\imprimante\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	
	/* enreg des data */
	
	memset(log, 0x00, 256);
	
	sprintf (fileName, "%s/Log_Buffer.txt", result_dir);
	
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	
//	sprintf (log, "18: 30 -- hello && it is me");
//	sprintf (log, sdate_time, DATE_TIME_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf (log,"---------------------------------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\nStatus Online=%d\r\nInterpreter=%d\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, StatusOnline, Interpreter);  
//	sprintf (log, "IS_BARCODE_REAR=%d\r\n", is_barcode_rear); 
//	sprintf (log, "SENSOR_CYLINDER_X_REAR_STATION_80=", is_cylinder_x_rear); 
	
	
	WriteLine (HandleFile, log, sizeof (log));	
	
	CloseFile (HandleFile);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "fncLock"
static SElExceptionPtr fncLock(void)
{
   EnterCriticalSection( &_Lock);
   return NULL;
} 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "fncUnlock"
static SElExceptionPtr fncUnlock(void)
{
   LeaveCriticalSection( &_Lock);
   return NULL;
} 

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2



/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::WaitRotation"
SElExceptionPtr WaitRotation(int32_t timeout)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 
	
Error:
	EXCRETHROW( pexception);  
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Station_Status"
SElExceptionPtr Station_Status(int32_t station, int32_t status)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	PANEL_STATION_STATUS(station, status); 
	
Error:
	EXCRETHROW( pexception);  
}



/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ExtractDoubleBarcode"
static SElExceptionPtr ExtractDoubleBarcode(char *Buffer, char *Barcode1, char *Barcode2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
    int i, Index1, Index2;
	
	Index1 = FindPattern (Buffer, 0, -1, "\r", 0, 0);
	
	for(i=0; i<Index1;i++)
	{
		if(Buffer[i] > 48)
		{
		CopyString (Barcode1, 0, Buffer, i, Index1-i);		
		break;
		}
	}
	
	Index2 = FindPattern (Buffer, Index1+1, -1, "\r", 0, 0);
	
	for(i=Index1+1; i<Index2;i++)
	{
		if(Buffer[i] > 48)
		{
		CopyString (Barcode2, 0, Buffer, i, Index2-i);		
		break;
		}
	}
		
Error:
	EXCRETHROW( pexception);  
}

void CVICALLBACK ReadFunction (int portNumber, int eventMask, void *callbackData)
{

	SElExceptionPtr	pexception = NULL; 
	int 			Lenght=0;
	char 			ChaineLue[32]="";
	int32_t      	Index = -1;
	char 			barcode[32]="";
	int             tested = 0;
   	int             failed = 1;
		
	Lenght = GetInQLen (PORTDOUCHETTE);  	
	
	if(Lenght>0 && Lenght<32)
	{
		ComRd(PORTDOUCHETTE,ChaineLue,Lenght);
	}
	
	
	FlushInQ (PORTDOUCHETTE);
	memset(barcode , 0X00, 32);
	
	/* verification # */
	Index = FindPattern (ChaineLue, 0, -1, "#", 0, 0);

	/* lecture barcode parts */
	if (Index != -1)
	CopyString (barcode, 0, ChaineLue, Index+1, strlen(ChaineLue)-2);
	else 
	CopyString (barcode, 0, ChaineLue, Index+1, strlen(ChaineLue)-1);

	
	/* cas étiquette kanban */
	if (barcode[0] == 'H' || barcode[0] == 'Z' )
	{
	 strcat(barcode ,"\r");
	 ComWrt (PORTCIM, barcode, StringLength (barcode));
	 Sleep(200);
	 ReturnRS232Err ();
	 Sleep(200);
	FlushInQ (PORTCIM); }
	
	
	/* cas étiquette produit */
	else 
	{
	EXCCHECK( gs_pLabels->GetBarCodeStatus(gs_pLabels, barcode, &tested, &failed));
		if (tested)
		{
			if(!failed)
			{	
			 strcat(barcode ,"\r");
   			 ComWrt (PORTCIM, barcode, StringLength (barcode));
			 Sleep(200);
			 ReturnRS232Err ();
			 Sleep(200);
		FlushInQ (PORTCIM);	}
		}
	}


Error:
	FlushInQ (PORTCIM);
	FlushOutQ (PORTCIM);
	EXCDISPLAY( pexception);  
}


 /*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::StartBarflow"
static SElExceptionPtr StartBarflow (void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	
	/* Start CIM+ */
	if (gs_Automat.CIM_PLUS_active)
		{
		EXCCHECKCVI( OpenComConfig (PORTDOUCHETTE, "", 9600, 0, 8, 1, 512, 512));
    	EXCCHECKCVI( InstallComCallback (PORTDOUCHETTE, LWRS_RXCHAR, 0, 0, ReadFunction, 0));
		EXCCHECKCVI( FlushInQ (PORTDOUCHETTE));
		EXCCHECKCVI( FlushOutQ (PORTDOUCHETTE)); 
		EXCCHECKCVI( OpenComConfig (PORTCIM, "", 9600, 0, 8, 1, 512, 512));
		EXCCHECKCVI( FlushInQ (PORTCIM));
		EXCCHECKCVI( FlushOutQ (PORTCIM)); 
		}
		

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventGuiResetProduct"
static SElExceptionPtr ProcessEventGuiResetProduct(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			test_form_xml[MAX_PATHNAME_LEN]; 
	int32_t			is_Error = 0;
	SaxisPtr		paxis = NULL;
	SaxisPtr		paxis1 = NULL;
    int i;
	int32_t			insert1=0, insert2=0;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	int32_t			axis_Z_Homed_Station40;
	int32_t			axis_Z_Homed_Station50;
	clock_t 		_start_clock;
	
	PANEL_MSG( PANEL_HELP2, _("Reset Tester..."));
	
     //gs_pTester->UnlockPanelStation10(gs_pTester, 7000)
		
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 2, &paxis));
	
	EXCCHECK( paxis->AlarmReset(paxis, AXIS_Z));
	Delay(0.2);
	EXCCHECK( paxis->EnableAxis(paxis, AXIS_Z));
	Delay(0.2);
	EXCCHECK( paxis->HomeReturn(paxis, AXIS_Z));
	Delay(0.2);
	
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 3, &paxis));
	
	EXCCHECK( paxis->AlarmReset(paxis, AXIS_Z));
	Delay(0.2);
	EXCCHECK( paxis->EnableAxis(paxis, AXIS_Z));
	Delay(0.2);
	EXCCHECK( paxis->HomeReturn(paxis, AXIS_Z));
	Delay(0.2);
	

	
	_start_clock =  clock();
	do{
		Sleep(500); 
		EXCCHECK( gs_pTester->GetSensor(gs_pTester, "AXIS_Z_HOME_STATION_40", &axis_Z_Homed_Station40));
		EXCCHECK( gs_pTester->GetSensor(gs_pTester, "AXIS_Z_HOME_STATION_50", &axis_Z_Homed_Station50));
		if (axis_Z_Homed_Station40 == 1 && axis_Z_Homed_Station50 == 1)		
		 	break;							
		TIMEOUT_CHECK (5000);
	 }while (TRUE);
	 
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 0, &paxis));
	EXCCHECK( paxis->AlarmReset(paxis, AXIS_Y));
	EXCCHECK( paxis->EnableAxis(paxis, AXIS_Y));
	EXCCHECK( paxis->HomeReturn(paxis, AXIS_Y));

	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 1, &paxis));
	EXCCHECK( paxis->AlarmReset(paxis, AXIS_Y));
	EXCCHECK( paxis->EnableAxis(paxis, AXIS_Y));
	EXCCHECK( paxis->HomeReturn(paxis, AXIS_Y));

	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 0, &paxis));
	EXCCHECK( paxis->AlarmReset(paxis, AXIS_X));
	EXCCHECK( paxis->EnableAxis(paxis, AXIS_X));
	EXCCHECK( paxis->HomeReturn(paxis, AXIS_X));

	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 1, &paxis));
	EXCCHECK( paxis->AlarmReset(paxis, AXIS_X));
	EXCCHECK( paxis->EnableAxis(paxis, AXIS_X));
	EXCCHECK( paxis->HomeReturn(paxis, AXIS_X));
	
	//ajout check home completetion
	for (i=0; i<NUMBER_OF_STATIONS; i++)
	EXCCHECK( gs_pTester->ResetStations(gs_pTester, i, 6000));
	
	gs_pTester->ResetPickPlace(gs_pTester, 15000, TRUE);  //MA
	
	
	EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
	EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
		
		if(gs_Automat.shift_id==0)
		{
		PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
		PANEL_MSG( PANEL_HELP2, "");
		}	

	else if (full)	   // Old value (!full) changed by oussama       
			{
			PANEL_MSG( PANEL_HELP2, _("Vider goulotte rouge"));
			}
		else if (!closed)
			{
			PANEL_MSG( PANEL_HELP2, _("Fermer tiroir goulotte rouge"));
			}
		else
		{
		 
		 /*  check presence panel */		
		 EXCCHECK(TestProductPresence(&error, &insert1, &insert2)); 
		 //printf("Insert 1= %d: Insert 2=%d ProcessEventGuiResetProduct\n\r",insert1, insert2); 
		
		if (gs_Automat.EmtyTable == TRUE) //mode vidage 
		{	 
			//printf(" gs_Automat.EmtyTable dans ProcessEventGuiResetProduct\n\r");
			if (insert1 || insert2)
				PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
			else if (!insert1 && !insert2)
			{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));	   
			//printf("reset product 1\n\r");
			}
		}
		
		else 	  //mode production normal
		{	
			if (!insert1 || !insert2)
				PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
			else if (insert1 && insert2)
			{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle")); 
				//printf("reset product 2\n\r");
			}
		}		
		
		
		}

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventGuiProductSelected"
static SElExceptionPtr ProcessEventGuiProductSelected(bool_t set_panel, bool_t set_box, bool_t set_master)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			test_form_xml[MAX_PATHNAME_LEN]; 
	int32_t			is_Error = 0;
	SElEventDataPtr pdata = NULL;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	int32_t			master_id;
	char			Msg[128]; 
   	SCameraPtr		pcamera = NULL;
	char data[128] = "";

	if(gs_EventFactory && set_panel)
	{
		/* set panel to default */
		EXCCHECK( eleventdata_new(&pdata)); 
		EXCCHECK( pdata->SetStringPtr(pdata, "panel;2"));
		EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_GUI_PANEL", pdata));
	}

	if(gs_Automat.master_active)
	{
		//printf("gs_Automat.master_active\r\n");		
		Sleep(200);
		
				gs_Automat.masterpart = TRUE; 
			
				/* set mode passage piece type avant production */
				gs_Automat.Mode = AUTOMAT_MASTERPART;
		
				EXCCHECK( GetMasters ());

				if (number_of_master_parts == 0) 
				{
					
				/* reload production testsequence */
				gs_Automat.Mode = AUTOMAT_PRODUCTION;
				
				gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;
				
				gs_Automat.masterpart = FALSE;
				
				EXCCHECK(  ProcessEventGuiProduction ());
				
				if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_MAN_active)
					EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", NULL));
				
				EXCCHECKCVI( GetProjectDir(test_form_xml));
				strcat(test_form_xml, PATH_TEST_FILE_FORM_XML);
	
				EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, TRUE, &is_Error));
				CHECK_ERR( is_Error);
				
				PANEL_SET_REFERENCES(gs_Automat.pTestSeq->ProductPartNumber, 
									 gs_Automat.pTestSeq->ProductCustomerRef);
				
			
		        EXCCHECK( LoadJobCamera (CAMERA_PICTO, gs_Automat.pTestSeq->JobCameraPicto));
		        Sleep(1000);
			//	EXCCHECK( CameraCheckJob (gs_Automat.pTestSeq->JobCameraPicto));
				//printf("gs_Automat.pTestSeq->JobCameraPicto=%c\r\n",gs_Automat.pTestSeq->JobCameraPicto);
				EXCCHECK( LoadCameraPictos ());


				}
				else 
				{
				PANEL_MSG( PANEL_HELP1, _("Master Part Sequence")); 

				/* Load first master barcode */
				EXCCHECK( ProcessEventMasterSequenceRun (TRUE));
				}
		}

	else 
	{
	/* set mode */
	gs_Automat.Mode = AUTOMAT_PRODUCTION;
	gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;

	EXCCHECK(  ProcessEventGuiProduction ());

	if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_MAN_active)
		EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", NULL));
	
	EXCCHECKCVI( GetProjectDir(test_form_xml));
	strcat(test_form_xml, PATH_TEST_FILE_FORM_XML);
	
	EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, TRUE, &is_Error));
	CHECK_ERR( is_Error);
	PANEL_SET_REFERENCES(gs_Automat.pTestSeq->ProductPartNumber, 
						 gs_Automat.pTestSeq->ProductCustomerRef);
	
	if (!strcmp(gs_Automat.pTestSeq->ProductType,"VS11"))			
		
	{  gs_pTester->SelectRodageVS11 (gs_pTester, 3000); 
	   gs_pTester->UnplugPanelStation40_X52 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation40_VS11 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation50_X52 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation50_VS11 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation60_X52 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation60_VS11 (gs_pTester, 3000);
	   gs_pTester->SelectVS11Station40(gs_pTester, 3000);
	   gs_pTester->SelectVS11Station50(gs_pTester, 3000);  
	   gs_pTester->SelectVS11Station60(gs_pTester, 3000);
	 //  printf("Executing VS11\n\r");
	}
	
	else 
	{
	   gs_pTester->SelectRodageX52 (gs_pTester, 3000); 
	   gs_pTester->UnplugPanelStation40_X52 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation40_VS11 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation50_X52 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation50_VS11 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation60_X52 (gs_pTester, 3000);
	   gs_pTester->UnplugPanelStation60_VS11 (gs_pTester, 3000);
	   gs_pTester->SelectX52Station40(gs_pTester, 3000);
	   gs_pTester->SelectX52Station50(gs_pTester, 3000);  
	   gs_pTester->SelectX52Station60(gs_pTester, 3000);
	 //  printf("Executing X52\n\r"); 
	
	}
	
		EXCCHECK( LoadJobCamera (CAMERA_PICTO, gs_Automat.pTestSeq->JobCameraPicto));
		Sleep(1000);
	//	EXCCHECK( CameraCheckJob (gs_Automat.pTestSeq->JobCameraPicto));
		EXCCHECK( LoadCameraPictos ());
	}

	gs_Automat.ChangeVersion = FALSE;
	gs_pTester->UnlockPanelStation10(gs_pTester, 7000); 

	PANEL_MODE( DEFAULT_MODE);

Error:
	eleventdata_delete(&pdata);
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventGuiProduction"
static SElExceptionPtr ProcessEventGuiProduction(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			test_form_xml[MAX_PATHNAME_LEN];
	int32_t			is_Error = 0;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	char			Msg[128]; 
   	int32_t			insert1=0, insert2=0;
	SCameraPtr		pcamera = NULL;
   	char data[128] = "";

	
	if (gs_Automat.Mode != AUTOMAT_MASTERPART && gs_Automat.EmtyTable==FALSE)
	{

	/* set mode */
	gs_Automat.Mode = AUTOMAT_PRODUCTION;
	
	
	/* Check if product in pallett */
	//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_LEFT", &insert1));
	//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_RIGHT", &insert2)); 
		
		EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
	//	printf("Insert 1= %d: Insert 2=%d Outside function\n\r",insert1, insert2); 
		if (insert1 || insert2)
			{
				PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			}
		else 
			PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
		
	/* load test sequence for production */
	if(gs_Automat.TestSeqMode!=TESTSEQ_PRODUCTION)
	{
		EXCCHECKCVI( GetProjectDir(test_form_xml));
		strcat(test_form_xml, PATH_TEST_FILE_FORM_XML); 
	
		EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, TRUE, &is_Error));
		CHECK_ERR( is_Error);
		gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;
		PANEL_SET_REFERENCES(gs_Automat.pTestSeq->ProductPartNumber, 
							 gs_Automat.pTestSeq->ProductCustomerRef);
		
		EXCCHECK( LoadJobCamera (CAMERA_PICTO, gs_Automat.pTestSeq->JobCameraPicto));  //MA
		Sleep(1000);
	//	EXCCHECK( CameraCheckJob (gs_Automat.pTestSeq->JobCameraPicto));
		EXCCHECK( LoadCameraPictos ());
	}
	
	if(gs_Automat.shift_id==0)
	{
		PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
		PANEL_MSG( PANEL_HELP2, "");
	}

	else
	{
		
		PANEL_SET_REFERENCES(gs_Automat.pTestSeq->ProductPartNumber, 
							 gs_Automat.pTestSeq->ProductCustomerRef);
				
		PANEL_MSG( PANEL_HELP1, _("Production"));
		
		if(gs_Automat.box)
		{
			PANEL_MSG( PANEL_HELP2, _("Insert Empty Box"));
			INSERT_EMPTY_BOX();
		}
		else if(gs_Automat.Mode = AUTOMAT_PRODUCTION)
		{
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			
		 if (full)	   // Old value (!full) changed by oussama       
				{
				PANEL_MSG( PANEL_HELP2, _("Vider goulotte rouge"));
				}
			else if (!closed)
			{
				PANEL_MSG( PANEL_HELP2, _("Fermer tiroir goulotte rouge"));
			}
		}
		else
		{
			PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
		}
	}

	PANEL_MODE( DEFAULT_MODE);
	}
		

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/  
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventGuiExpertiseGoMode"
static SElExceptionPtr ProcessEventGuiExpertiseGoMode(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	/* set mode */
	gs_Automat.Mode = AUTOMAT_EXPERTISE_GO_MODE;
	
	if(gs_Automat.TestSeqMode==TESTSEQ_PRODUCTION)
	{
		PANEL_MSG( PANEL_HELP1, _("Expertise, Go Mode"));
	}
	else if(gs_Automat.TestSeqMode==TESTSEQ_EXPERTISE)
	{
		PANEL_MSG( PANEL_HELP1, _("Expertise Sequence, Go Mode"));
	}
	PANEL_MSG( PANEL_HELP2, _("Insérer Un Produit"));
	PANEL_MODE( DEFAULT_MODE);
				
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventGuiExpertiseSequenceCreated"
static SElExceptionPtr ProcessEventGuiExpertiseSequenceCreated(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			testexp_form_xml[MAX_PATHNAME_LEN]; 
	int32_t			is_Error = 0;

	
	/* set mode */
	gs_Automat.Mode = AUTOMAT_EXPERTISE_SEQUENCE;
	
	//printf ("AUTOMAT_EXPERTISE_SEQUENCE\r\n\r\n\r\n");
	
	//gs_Automat.TestSeqMode = TESTSEQ_EXPERTISE;
	gs_Automat.Mode = AUTOMAT_EXPERTISE_SEQUENCE;
	
	if (gs_Automat.Mode == AUTOMAT_EXPERTISE_SEQUENCE)
		{
			PromptPopup ("Informations R&R", "Ajouter Numero Palette", gs_RnR_Palett, 10);	
			PromptPopup ("Informations R&R", "Ajouter Numero Piéce", gs_RnR_Piece, 10);	
		}
	PANEL_MSG( PANEL_HELP1, _("Expertise R&R"));  
	PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
	PANEL_MODE( DEFAULT_MODE);
				
Error:
	EXCRETHROW( pexception);  
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventGuiQualityRetest"
static SElExceptionPtr ProcessEventGuiQualityRetest(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			test_form_xml[MAX_PATHNAME_LEN];
	int32_t			is_Error = 0;

	/* set mode */
	gs_Automat.Mode = AUTOMAT_QUALITY_RETEST;
	
	/* load testsequence for production */
	if(gs_Automat.TestSeqMode!=TESTSEQ_PRODUCTION)
	{
		EXCCHECKCVI( GetProjectDir(test_form_xml));
		strcat(test_form_xml, PATH_TEST_FILE_FORM_XML);
		
		EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, TRUE, &is_Error));
		CHECK_ERR( is_Error);

		gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;
	}
	
	PANEL_MSG( PANEL_HELP1, _("Quality Retest"));
	PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
	PANEL_MODE( DEFAULT_MODE);

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventGuiQualityMaster"
static SElExceptionPtr ProcessEventGuiQualityMaster(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			test_form_xml[MAX_PATHNAME_LEN]; 
	int32_t			is_Error = 0;

	/* set mode */
	gs_Automat.Mode = AUTOMAT_QUALITY_MASTER;
	
	/* load testsequence for production */
	if(gs_Automat.TestSeqMode!=TESTSEQ_PRODUCTION)
	{   
		EXCCHECKCVI( GetProjectDir(test_form_xml));
		strcat(test_form_xml, PATH_TEST_FILE_FORM_XML);
		
		EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, TRUE, &is_Error));
		CHECK_ERR( is_Error);
		
		gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;
	}
	
	PANEL_MSG( PANEL_HELP1, _("Quality Master"));
	PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
	PANEL_MODE( DEFAULT_MODE);
				
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadMasterInformations"
SElExceptionPtr ReadMasterInformations(const char* pfile_xml, char *description, char *name, char *station)
{
	int32_t				error = 0;
	SElExceptionPtr    	pexception = NULL; 
	HRESULT         	hr = 0;
	CAObjHandle     	pxml = 0; 
	CAObjHandle     	pnode = 0;
	VARIANT         	pfile_name_var = CA_VariantEmpty();
	VBOOL            	is_xml_success = 1;
	char*           	ptext = NULL;
	
	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	
	/* description */
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/description", &pnode));
	if(pnode)
	{
	EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode, NULL, &ptext));
		if(ptext && *ptext)
			{
			sprintf (description, "%s", ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
			}
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	/* name */
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/name", &pnode));
	if(pnode)
	{
	EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode, NULL, &ptext));
		if(ptext && *ptext)
			{
			sprintf (name, "%s", ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
			}
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	/* station */
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/station", &pnode));
	if(pnode)
	{
	EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode, NULL, &ptext));
		if(ptext && *ptext)
			{
			sprintf (station, "%s", ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
			}
		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}

Error:
	CA_VariantClear(&pfile_name_var);
	if (ptext) CA_FreeMemory(ptext);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::GetMasters"
static SElExceptionPtr GetMasters(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char testmaster_form_xml[MAX_PATHNAME_LEN];
	int32_t			master_id;
	int32_t			is_Error = 0;
	SElEventDataPtr pdata = NULL;
	char			Msg[128]; 
	char**			Barcodes = NULL;
	char			description[512]; 
	char			name[512]; 
	char			station[128]; 
	char 			pfile_xml[512];
	int 			i;
	int             iStation;

	if(gs_pDbs) 
		{
	
		
		/* get the number of master parts in this verion */
		Barcodes = (char**)calloc(512, sizeof(char*));
		
		EXCCHECK( gs_pDbs->ProductMasterList->ProductMasterGetBarCodes(gs_pDbs->ProductMasterList, gs_pDbs->TesterProductId,
																				&number_of_master_parts, Barcodes));
	//	printf("number_of_master_parts=%d\r\n",number_of_master_parts);
		if (number_of_master_parts > 0)
			{
			for (i=0; i<number_of_master_parts; i++)
				{
				/*Read Masters description*/
				sprintf (pfile_xml, ".//Masters//%s.XML", Barcodes[i]);
				memset (name, 0x00, 512);
				memset (description, 0x00, 512);
				EXCCHECK(ReadMasterInformations(pfile_xml, description, name, station));
				sprintf (MastersBarcodes[i], "%s", Barcodes[i]);
				sprintf (MastersDescription[i], "%s", description);
				sprintf (MastersName[i], "%s", name);
				MastersStations[i] = atoi(station);
				}
			}
			gs_Automat.TestSeqMode = TESTSEQ_MASTERPART;
		}
				
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventMasterSequenceRun"
static SElExceptionPtr ProcessEventMasterSequenceRun(bool_t Load_Params)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char 			test_form_xml[MAX_PATHNAME_LEN];
	int32_t			master_id;
	int32_t			is_Error = 0;
	SElEventDataPtr pdata = NULL;
	char			Msg[128]; 
			
		/* Start CIM+ */
#if 0    /* formerly excluded lines */
		EXCDISPLAY( StartBarflow());
#endif   /* formerly excluded lines */
	
		if (number_of_master_parts > 0)
			{
			/* init Finished Master parts for new tour */
			Finished_master_parts = 0;	
			
			/* Start with first index of master part */
			Actual_master_part = 0;

			if (Load_Params)
			{
			EXCCHECKCVI( GetProjectDir(test_form_xml));
			strcat(test_form_xml, PATH_TEST_FILE_FORM_XML); 

			/* load testsequence */
			EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, FALSE, &is_Error)); 

			/* load job camera */
			EXCCHECK( LoadJobCamera (CAMERA_PICTO, gs_Automat.pTestSeq->JobCameraPicto));
			Sleep(1000);
		//	EXCCHECK( CameraCheckJob (gs_Automat.pTestSeq->JobCameraPicto));
		//	printf("gs_Automat.pTestSeq->JobCameraPicto=%c\r\n",gs_Automat.pTestSeq->JobCameraPicto); 
		    EXCCHECK( LoadCameraPictos ());
			}
		
			/* Display */
			sprintf (Msg, "Insert Master Panel : %s[%s]", MastersName[Actual_master_part], MastersDescription[Actual_master_part]);	
			PANEL_MSG( PANEL_HELP2, _(Msg));
			
			gs_Automat.TestSeqMode = TESTSEQ_MASTERPART;
			}
				
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventTestSequenceRun"
static SElExceptionPtr ProcessEventTestSequenceRun(struct _SSledge* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			form_xml[MAX_PATHNAME_LEN];        

	
	EXCCHECKCVI( GetProjectDir(form_xml));
	strcat(form_xml, PATH_TEST_FILE_FORM_XML); 
	EXCCHECK( RunTestSequence(form_xml, me, &gs_Automat, gs_EventFactory)); 

				
Error:
	EXCRETHROW( pexception);  
}

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Barcode"
DWORD WINAPI Thread_Barcode(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
   	SBarcodeDataPtr	pthread_data = (SBarcodeDataPtr)param;
	uint32_t	barcode_ID = pthread_data->barcode_ID;
	char  barcode[256]="";
	
	
	sprintf(pthread_data->barcode, "none");
	
	gs_pTester->GetItemCode(gs_pTester, "", barcode_ID, BARCODE_BUFFER_SIZE, barcode, 5000);
		
	sprintf(pthread_data->barcode, barcode);
	
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
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventJigItemCycleStarted"
static SElExceptionPtr ProcessEventJigItemCycleStarted(struct _SSledge* me)
{
	SElExceptionPtr	pexception = NULL;
	SElExceptionPtr	pexception2 = NULL;
	SElExceptionPtr ptest_exception = NULL;
	int32_t			error = 0;
	bool_t			failed = FALSE;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	int32_t			barrier;
	int32_t			StatusStationsReady = 0; 
	int32_t			i = 0; 
	int32_t			MicroCam;
	int32_t			insert1, insert2;
    int32_t 		status_micro_cam;
	int             product_valid=0;
	

	int32_t			count;
	int32_t			master_id;
	int32_t			product_id;
	int32_t			is_Error = 0;
	char			Msg[128]; 
	SBarcodeDataPtr	pthread_data[2];
   	HANDLE  		hThreadBarcode[2];

	if( !gs_Automat.disabled)
	{   //printf("!gs_Automat.disabled\n\r");
		
		if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.shift_id==0)
		{
			PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
			PANEL_MSG( PANEL_HELP2, "");
			CHECK_ERR( -1);
		}



//#if 0    /* formerly excluded lines */
        sprintf(me->barcode, "none");
		
		PANEL_MSG( PANEL_HELP2, _("Check Barcodes"));
		
		
		/* Start thread read barcodes */
		for (i=0; i<2; i++)
		{
		pthread_data[i] = (SBarcodeDataPtr)calloc(1, sizeof(SBarcodeData)); 
		EXCCHECKALLOC( pthread_data[i]);
		pthread_data[i]->barcode_ID = i;
		EXCCHECK(ThreadStart(Thread_Barcode, pthread_data[i], &hThreadBarcode[i], NULL));
		}
	
		/* Wait thread to finish */
		for (i=0; i< 2 ; i++)
			pexception = ThreadStatus(hThreadBarcode[i], 5000, NULL);
		
		
	    if (!strcmp (pthread_data[1]->barcode, ""))
			sprintf(me->barcode_housing, "none");
		else 
	 		sprintf (me->barcode_housing, "%s", pthread_data[1]->barcode); 
		
		
	   if (!strcmp (pthread_data[0]->barcode, ""))
			sprintf(me->barcode_top_cover, "none");
	   else 
			sprintf (me->barcode_top_cover, "%s", pthread_data[0]->barcode);
	   
		/* free thread data */
		for (i=0; i< 2 ; i++)
			free(pthread_data[i]);
	
		/* free thread handle */
		for (i=0; i< 2 ; i++)
		{
			if(hThreadBarcode[i])
			{
			CloseHandle(hThreadBarcode[i]);
			hThreadBarcode[i] = NULL;
			}
		}
		
		/* read Barcode Housing */
		
		PANEL_MSG( PANEL_HELP2, _("Check Barcode Housing"));
		if (gs_Automat.Mode!=AUTOMAT_MASTERPART)
		{
			EXCCHECK( JigPanelBarcodeHousing(me, &error));  
			CHECK_ERR( error);
		}
		
		/* read Barcode Top cover */ 
		PANEL_MSG( PANEL_HELP2, _("Check Barcode Rear cover"));
		EXCCHECK( JigPanelBarcodeTopCover(me, &error));  
		CHECK_ERR( error);
		
		PANEL_MSG( PANEL_HELP2, _("Please wait until all stations are finished "));	

		/* Lock Panel */
		JigPanelLock(&error);
		CHECK_ERR( error);
		
#if ROTATION
		
		//Sleep(1000); /* switch barrier status */
		do{
			gs_pTester->GetSensor(gs_pTester, "INFRARED_BARRIER", &barrier);
			DisplayWait();
			ProcessSystemEvents ();
		}while((!StatusStations[1] || !StatusStations[2] || !StatusStations[3] ||
			    !StatusStations[4] || !StatusStations[5] || !StatusStations[6] || !StatusStations[7]) 
				&& barrier);
		
		
		if (!barrier)  //re push Start Cycle Buttom
		{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			goto Error; 
		}

		/* Rotate_Table */
		EXCCHECK (SelectProductType(me, &error));
		CHECK_ERR( error);

		pexception = JigPanelInsert(&error);

		gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &MicroCam);
		gs_pTester->GetSensor(gs_pTester, "INFRARED_BARRIER", &barrier);
		if (MicroCam == 0)  // table pas dans position pas error => franchi barriere par operateur
			{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  being entered to station 20 */
			}
		else   //check if error table rotation
			{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			CHECK_ERR( error);
			}
		
#endif   

		StatusStations[0] = TRUE;
		gs_pTester->UnlockPanelStation10(gs_pTester, 2000);
		
		/* check presence product */
	//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_LEFT", &insert1));
	//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_RIGHT", &insert2));
		
		EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
	//	printf("Insert 1= %d: Insert 2=%d Outside function\n\r",insert1, insert2); 
		if (!insert1 || !insert2)
		{
			/* Unlock palett N°1 */
			gs_pTester->UnlockPanelStation10(gs_pTester, 2000);
			PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
		}
		else if (insert1 && insert2)
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));

		/* Table not empty */
		gs_Automat.isTableEmpty = FALSE;
		gs_Automat.EmtyTable = FALSE;
		
		/*parameters RnR */
		me->Station_RnR = gs_RnR_Station;
		me->Repeat_RnR = gs_RnR_Repeat;
		
		if ( gs_PalettID == 1)
			sprintf (me->Pallet, "P8"); 
		else 
			sprintf (me->Pallet, "P%d", gs_PalettID - 1);

		if(gs_Automat.Mode == AUTOMAT_MASTERPART)
		{
	//		printf("master part mode\r\n");
			/* affect Actual_Master_Part to this Thread */
			me->Actual_Master_Part = Actual_master_part;
			sprintf (me->barcode_master_Part ,"%s\0", MastersBarcodes[Actual_master_part]);
			sprintf (me->name_master_Part ,"%s\0", MastersName[Actual_master_part]);
			me->Station_Master = MastersStations[Actual_master_part];

			/* increment next master part */
			Actual_master_part++;
		
			/* if list of master parts are finished to empty table */
			if (Actual_master_part == number_of_master_parts)
			{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			
			gs_Automat.EmtyTable = TRUE;
			gs_Table_Rotation = 0;
			}
			/* if list of master parts are not yet finished  */
			else 
			{
				if (MastersBarcodes[Actual_master_part] != NULL)
				{
				sprintf (Msg, "Insert Master Panel : %s[%s]", MastersName[Actual_master_part], MastersDescription[Actual_master_part]);	
				PANEL_MSG( PANEL_HELP2, _(Msg));
				}		
			}
		}
		
		if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_MAN_active && gs_Automat.Mode == AUTOMAT_PRODUCTION)
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", NULL));
		
		/* run test sequence */
		pexception = ProcessEventTestSequenceRun(me);

		if(  pexception==NULL)
		{
			failed = FALSE;
		}
		
		else if(pexception)
		{
			EXCDBGSTR( pexception);
			failed = TRUE;

		}	
		if( gs_Automat.Mode==AUTOMAT_PRODUCTION) 
		{			  
			   pexception =  JigPanelRelease(me, failed);
		       if(pexception==NULL)
		          {
		             goto Error2; 
		          }	
		}
		
		if( gs_Automat.Mode==AUTOMAT_MASTERPART)
		{
			pexception =  JigPanelRelease2(me, failed);
		    WaitRotation (INFINITE);
		    pexception =  JigPanelReleaseFinal(me, failed);
			if(pexception==NULL)
			{
			goto Error2; 
			}	
		}
	}
	

	
Error:
	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &status_micro_cam));
	if(gs_Automat.shift_id && status_micro_cam && gs_Automat.Mode!=AUTOMAT_MASTERPART)
	{
	gs_pTester->UnlockPanelStation10(gs_pTester, 1200);
	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
	}
	StatusStations[0] = TRUE;
	EXCRETHROW( pexception);
	
Error2:
	StatusStations[7] = TRUE;
	StatusStations[0] = TRUE;
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventJigItemEmpty"
static SElExceptionPtr ProcessEventJigItemEmpty(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			failed = FALSE;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	int32_t			barrier;
	int32_t			StatusStationsReady = 0; 
	int32_t			i = 0; 
	int32_t			MicroCam;
	int32_t			insert1, insert2;
	SPrinterPtr		printer;

	if( !gs_Automat.disabled)
	{
	
#if 0    /* formerly excluded lines */
		if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.shift_id==0)
		{
			PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
			PANEL_MSG( PANEL_HELP2, "");
			EXCTHROW(-1, "rotation Fault"); 
		}
	
		else if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.box)
		{ 
			PANEL_MSG( PANEL_HELP2, _("Insert Empty Box"));//par pulse event EVNT_BOX_CHANGE
			INSERT_EMPTY_BOX();
			EXCTHROW(-1, "rotation Fault"); 		
		}
	
		else if( gs_Automat.Mode==AUTOMAT_PRODUCTION)
		{ 
			//check box empty + check printer no error cycle
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			if (full)
				{
				PANEL_MSG( PANEL_HELP2, _("Empty Red Box"));
				EXCTHROW(-1, "rotation Fault"); 				
				}
			else if (!closed)
				{
				PANEL_MSG( PANEL_HELP2, _("Close Red Box"));
				EXCTHROW(-100, "rotation Fault"); 			
				}
			else if (erreur)
				{
				PANEL_MSG( PANEL_HELP2, _("Reset Printer Controller"));
				EXCTHROW(-1, "rotation Fault"); 			
				}
		}
#endif   /* formerly excluded lines */
		
		
		/* Lock Panel */
		PANEL_MSG( PANEL_HELP2, _("Lock Panel "));
		JigPanelLock(&error);
		CHECK_ERR( error);
		

		//Sleep(1000); /* switch barrier status */
		do{
			gs_pTester->GetSensor(gs_pTester, "INFRARED_BARRIER", &barrier);
			DisplayWait();
			ProcessSystemEvents ();
		}while((!StatusStations[1] || !StatusStations[2] || !StatusStations[3] ||
			    !StatusStations[4] || !StatusStations[5] || !StatusStations[6] || !StatusStations[7]) 
				&& barrier);
		
		/* Rotate_Table */
		if (!barrier)  //re push Start Cycle Buttom
		{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			EXCTHROW(-1, "rotation Fault"); 			
		}
		pexception = JigPanelInsert2(&error);
		
		//printf("****Rotation in jigPanel Empty****\r\n");
		
		gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &MicroCam);
		gs_pTester->GetSensor(gs_pTester, "INFRARED_BARRIER", &barrier);
		
		if (!MicroCam && pexception)  // table pas dans position pas error => franchi barriere par operateur
			{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  being entered to station 20 */
			}
		else if (MicroCam && !barrier)  // table pas encore bougé => franchi barriere par operateur
			{
				
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			EXCTHROW(-1, "rotation Fault"); 			
			}
		
		else if (MicroCam && barrier && pexception)  // table pas encore bougé => franchi barriere par operateur
			{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			EXCTHROW(-1, "rotation Fault");
			}
		//Sleep (500);
		StatusStations[0] = TRUE;
		
		
		/* Check if product in pallett */
	//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
	//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
		
		EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
		//printf("Insert 1= %d: Insert 2=%d ProcessEventJigItemEmpty\n\r",insert1, insert2); 
		if (insert1 || insert2)
			{
			/* Unlock palett N°1 */
			gs_pTester->UnlockPanelStation10(gs_pTester, 1200);
			//JigPanelUnlock(&error);
			CHECK_ERR( error);
			PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
			}
		else 
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
	
	}

	
Error:
	Sleep (1000);
	StatusStations[0] = TRUE;
	EXCRETHROW_NO_DISP( pexception);	
	
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventJigItemReleased"
static SElExceptionPtr ProcessEventJigItemReleased(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	char			Msg[128]; 

	PANEL_MSG( PANEL_RESULT_MSG, _(""));

	if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.shift_id==0)
	{
		PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
		PANEL_MSG( PANEL_HELP2, "");
	}

	else if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.box)
	{
		PANEL_MSG( PANEL_HELP2, _("Insert Empty Box"));
		INSERT_EMPTY_BOX();
	}

	else if ( gs_Automat.Mode==AUTOMAT_MASTERPART && !gs_Automat.disabled)
	{ 		
		PANEL_STATION_STATUS(0,3);
		if( !gs_Automat.EmtyTable)
			{
				if (MastersBarcodes[Actual_master_part] != NULL)
				{
				sprintf (Msg, "Insert Master Panel : %s[%s]", MastersName[Actual_master_part], MastersDescription[Actual_master_part]);	
				PANEL_MSG( PANEL_HELP2, _(Msg));
				}		
			}
	}
	else
	{
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			
		 if (full)	   // Old value (!full) changed by oussama       
				{
				PANEL_MSG( PANEL_HELP2, _("Vider goulotte rouge"));
				}
			else if (!closed)
			{
				PANEL_MSG( PANEL_HELP2, _("Fermer tiroir goulotte rouge"));
			}
			else 
			{
				if (gs_Automat.EmtyTable == TRUE)
				{
				PANEL_STATION_STATUS(0,3);
				PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
				}
				else
				{
				PANEL_STATION_STATUS(0,3);
				if( !gs_Automat.EmtyTable)
					{
					PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
					}
				}
			}
	}

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventJigItemInserted"
static SElExceptionPtr ProcessEventJigItemInserted(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	char			Msg[128]; 

	if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.shift_id==0)
	{
		PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
		PANEL_MSG( PANEL_HELP2, "");
	}
	else if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.box)
	{
		PANEL_MSG( PANEL_HELP2, _("Insert Empty Box"));
		INSERT_EMPTY_BOX();
	}
	else
	{
		PANEL_STATION_STATUS(0,2);
		if (gs_Automat.EmtyTable == TRUE)
		{	PANEL_MSG( PANEL_HELP2, _("Remove Panel"));  
			//printf(" ProcessEventJigItemInserted\n\r");
		}
		else
		{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle")); 
		}
	}
Error:
	EXCRETHROW( pexception);  
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "sledge_new"
SElExceptionPtr	sledge_new(SSledgePtr* pSledge)
{
	SElExceptionPtr	pexception = NULL;
	SSledgePtr		me = NULL;

	me = calloc(1, sizeof(SSledge));
	EXCCHECKALLOC( me);

	me->Activate = Activate;
	me->Deactivate = Deactivate;

	InitializeCriticalSection(&me->_Lock);
	if(pSledge) *pSledge = me;

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "sledge_delete"
SElExceptionPtr	sledge_delete(SSledgePtr* pSledge)
{
	SElExceptionPtr	pexception = NULL;

	if(pSledge && *pSledge)
	{
		SSledgePtr	me = *pSledge;

		DeleteCriticalSection(&me->_Lock);

		free(*pSledge);
		*pSledge = NULL;
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "Activate"
static SElExceptionPtr Activate(struct _SSledge* me)
{
	SElExceptionPtr pexception = NULL;
	int32_t			status;
    int error;
	
	EXCCHECK( ThreadStart(Thread_Sledge, me, &me->_hThread, NULL));
//	printf("activate thread\n\r");
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "Deactivate"
static SElExceptionPtr Deactivate(struct _SSledge* me)
{
	SElExceptionPtr pexception = NULL;

	EXCCHECK( ThreadStop(me->_hThread, me->_hEventStop));

Error:   
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEndVerion"
static SElExceptionPtr ProcessEndVerion(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;

	 //printf("ProcessEndVerion\r\n");
	 pexception  = ProcessEventJigItemEmpty();
	 if (pexception == NULL && !gs_Automat.disabled)
		{
		gs_Table_Rotation++;
			if (gs_Table_Rotation == NUMBER_OF_STATIONS)
				{
				PANEL_STATION_STATUS(7,3);
				PANEL_STATION_STATUS(0,3);
				gs_Automat.isTableEmpty = TRUE;
				gs_Automat.EmtyTable = FALSE;

                               /* Unlock Pallett in the first time */
				gs_pTester->UnlockPanelStation10(gs_pTester, 7000);

				EXCDISPLAY( ProcessEventGuiProduction());
				
				if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_MAN_active)
					EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", NULL));
				}
			else 
				PANEL_STATION_STATUS(gs_Table_Rotation,3);
		}

Error:
	EXCRETHROW( pexception);  
}

 /*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::DisplayVersionList"
static SElExceptionPtr DisplayVersionList(void)
{
	SElExceptionPtr	pexception = NULL;
	SElEventDataPtr pdata = NULL;
	int32_t			error = 0;
	
	//plugin product Run
	EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_GUI_MENU_HIDE", pdata));	
	EXCCHECK( eleventdata_new(&pdata)); 
	ProcessSystemEvents ();
	ProcessDrawEvents ();
	EXCCHECK( pdata->SetStringPtr(pdata, PLUGIN_PRODUCT));
	EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_PLUGIN_RUN", pdata));
	EXCCHECK( eleventdata_new(&pdata)); 
	EXCCHECK( pdata->SetStringPtr(pdata, "product"));
	EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_GUI_PANEL", pdata));
	
	//Close Com port PORTDOUCHETTE + PORTCIM
/*	if (gs_Automat.CIM_PLUS_active)
		{
		CloseCom (PORTDOUCHETTE);
		CloseCom (PORTCIM);
		}
*/	
 Error:
	EXCRETHROW( pexception);
	
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessChangeVerion"
static SElExceptionPtr ProcessChangeVerion(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
 
	 pexception  = ProcessEventJigItemEmpty();
	 if (pexception == NULL && !gs_Automat.disabled)
		{
		gs_Table_Rotation++;
			if (gs_Table_Rotation == NUMBER_OF_STATIONS)
			{
				PANEL_STATION_STATUS(7,3);
				PANEL_STATION_STATUS(0,3);
				EXCDISPLAY( DisplayVersionList());
				PANEL_MSG( PANEL_RESULT_MSG, _(""));
				gs_Automat.isTableEmpty = TRUE;
				gs_Automat.EmtyTable = FALSE;
			}
			else 
				PANEL_STATION_STATUS(gs_Table_Rotation,3);
		}

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEmptyTable"
static SElExceptionPtr ProcessEmptyTable(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	char			Msg[128];
	int32_t			insert1=0, insert2=0;


        	EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
			if (!insert1 && !insert2)
				{
				if (gs_Automat.ChangeVersion == FALSE) //End Version
				{	EXCDISPLAY( ProcessEndVerion());  
				}
				else if (gs_Automat.ChangeVersion == TRUE) //Change Version  
			   	{	EXCDISPLAY( ProcessChangeVerion());  
				}
			}
			else
			{
				StatusStations[0] = TRUE;
				PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
				pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 7000);
			}

Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Automat"
static DWORD WINAPI Thread_Automat(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0, insert1=0, insert2=0;
	DWORD 			result;
	bool_t			b_loop_stop = FALSE;
	char			barcode[512];
	bool_t			is_locked = FALSE;
	SSledgePtr		pSledge = NULL;
	int             i, panel_inserted=0;
	
	do
	{
	result = WaitForMultipleObjects(EVENT_HANDLE_NB, gs_EventHandle, FALSE, 10000);
		
		EXCCHECK( fncLock());
		is_locked = TRUE;  
		switch(result)
		{	
			case WAIT_OBJECT_0 + EVNT_GUI_PRODUCT_SELECTED:
				for(i=0; i< NB_OF_STATIONS ;i++)
				StatusStations[i] = 1;
				EXCDISPLAY( ProcessEventGuiProductSelected(TRUE, TRUE, TRUE));
				break; 
			case WAIT_OBJECT_0 + EVNT_RESET_PRODUCT:
				EXCDISPLAY( ProcessEventGuiResetProduct());
				
				break;
			case WAIT_OBJECT_0 + EVNT_GUI_PRODUCTION:
				EXCDISPLAY( ProcessEventGuiProduction());
				break;
			case WAIT_OBJECT_0 + EVNT_GUI_QUALITY_RETEST:
				EXCDISPLAY( ProcessEventGuiQualityRetest());
				break;
			case WAIT_OBJECT_0 + EVNT_GUI_QUALITY_MASTER:
				EXCDISPLAY( ProcessEventGuiQualityMaster());
				break;
			case WAIT_OBJECT_0 + EVNT_GUI_EXPERTISE_GO_MODE:
				EXCDISPLAY( ProcessEventGuiExpertiseGoMode());
				break;
			case WAIT_OBJECT_0 + EVNT_GUI_EXPERTISE_SEQUENCE_CREATED:
				EXCDISPLAY( ProcessEventGuiExpertiseSequenceCreated());
				break;
			case WAIT_OBJECT_0 + EVNT_JIG_START_:
				EXCCHECK(TestProductPresence(&error, &insert1, &insert2)); 
				if(!insert1 && !insert2)
					panel_inserted=0;
				if (StatusStations[0] == TRUE && gs_Automat.EmtyTable == TRUE && !panel_inserted)
				{
					StatusStations[0] = FALSE;
					EXCDISPLAY( ProcessEmptyTable());
					panel_inserted=1;
				}
				else if (StatusStations[0] == TRUE && gs_Automat.EmtyTable == FALSE && !panel_inserted)
				{
					gs_pTester->UnlockPanelStation10(gs_pTester, 7000); 
				}
				break;
			case WAIT_OBJECT_0 + EVNT_JIG_CYCLE_START_:
				EXCCHECK(TestProductPresence(&error, &insert1, &insert2)); 
				if(insert1 && insert2)
					panel_inserted=1;
				if (StatusStations[0] == TRUE && gs_Automat.EmtyTable == FALSE && panel_inserted)
				{
				StatusStations[0] = FALSE;
				EXCCHECK( sledge_new(&pSledge)); //creation de nouvelle structure pSledge
				EXCCHECK( pSledge->Activate(pSledge)); //lancement d'un nouveau thread de pSledge
				panel_inserted=0;
				}
				else if (StatusStations[0] == TRUE && gs_Automat.EmtyTable == TRUE)
				{
				StatusStations[0] = FALSE;
				EXCDISPLAY( ProcessEmptyTable());
				}
				break;
			case WAIT_OBJECT_0 + EVNT_STOP_THREAD:
				{
				MessagePopup ("THREAD", "EVNT_STOP_THREAD");
				b_loop_stop = TRUE;
				}
			//	printf("AIT_OBJECT_0 + EVNT_STOP_THREAD\n\r"); 
				break;
			case WAIT_TIMEOUT:
			//	printf("WAIT_TIMEOUT: \n\r"); 
				break;
			case WAIT_FAILED:
				EXCTHROW(-2, "Thread_Automat Fault");
			//	 printf("WWAIT_FAILED: \n\r");
				break;
		}
		
		fncUnlock();
		is_locked = FALSE;

	}while(!b_loop_stop);

Error:
	if( is_locked) fncUnlock();
	EXCDISPLAY(pexception);
	ExitThread(THREAD_EXIT_CODE);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Sledge"
static DWORD WINAPI Thread_Sledge(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	DWORD 			result;
	bool_t			b_loop_stop = FALSE;
	char			barcode[512];
	bool_t			is_locked = FALSE;
	SSledgePtr		pSledge = (SSledgePtr) param;
	SSledgePtr		me = (SSledgePtr) param;//Added
		
	ProcessEventJigItemCycleStarted(pSledge); 
	/* free pointer pSledge */
	EXCCHECK( sledge_delete(&pSledge));

	if(me->_hThread)
	{
		CloseHandle(me->_hThread);
		me->_hThread = NULL;
	}
	
Error:
	EXCDISPLAY(pexception);
	ExitThread(THREAD_EXIT_CODE);
	return 0;
}
 
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelBarcode"
static SElExceptionPtr JigPanelBarcode(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;

	EXCCHECK( gs_pLabels->CheckBarCode(gs_pLabels, "Label:Check", -1, me->barcode, &isCorrect, NULL));
	if(!isCorrect)
		{
		EXCTHROW(-1, "barcode NOK");
		}
	
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelBarcodeHousing"
static SElExceptionPtr JigPanelBarcodeHousing(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            barcode_housing[128];
	int32_t			Index;
	char 			msg[256];		
	

	if (!strcmp(me->barcode_housing, "none"))
		{
	//		printf("barcode rear cover = none\r\n");
			sprintf(msg, "probléme lecture BarCode Housing ! '(%s)'", me->barcode_housing);
			PANEL_MSG( PANEL_HELP2, msg);
			if(pError) *pError = -1;
			CHECK_ERR( *pError);
		}
	

   if(gs_Automat.Mode==AUTOMAT_MASTERPART && gs_Automat.master_active && gs_Automat.masterpart)
	{	 
#if 0    /* formerly excluded lines */
		/* check if scanned barcode exist in the list of master parts of this version */
		if (strcmp(me->barcode_housing, MastersBarcodes[Actual_master_part]))
		{
		sprintf(msg, "Master BarCode non valide ! '(%s)'", me->barcode_housing);
		PANEL_MSG( PANEL_HELP2, msg);
		if(pError) *pError = -1;
		CHECK_ERR( *pError);
		}
#endif   /* formerly excluded lines */

	}
   
   else 
	{
		EXCCHECK( gs_pLabels->CheckBarCode(gs_pLabels, "LabelHousing:Check", -1, me->barcode_housing, &isCorrect, NULL));
	//	printf("Housing is correct =%d\r\n",isCorrect);
	//	printf("Barcode Housing =%s\n\r", me->barcode_housing);
		if(!isCorrect)
		{
			PANEL_MSG( PANEL_HELP2, _("Invalid Housing BarCode"));
			if(pError) *pError = -1;
			CHECK_ERR( *pError);
		}
		else 
			
			
			if(gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.retest_active)
		{
			bool_t tested;

			EXCCHECK( gs_pLabels->GetBarCodeHousingStatus(gs_pLabels, me->barcode_housing, &tested, NULL));
			if(tested)
			{
				PANEL_MSG( PANEL_HELP2, _("Housing BarCode was tested"));
				if(pError) *pError = -1;
				CHECK_ERR( *pError);
			}
		}
		else if(gs_Automat.Mode==AUTOMAT_QUALITY_RETEST)
		{
			bool_t tested, failed;
			EXCCHECK( gs_pLabels->GetBarCodeHousingStatus(gs_pLabels, me->barcode_housing, &tested, &failed));
			if(!tested)
			{
				PANEL_MSG( PANEL_HELP2, _("Housing BarCode has not been tested"));
				if(pError) *pError = -1;
				CHECK_ERR( *pError);
			}
		}
	}
	
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelBarcodeTopCover"
static SElExceptionPtr JigPanelBarcodeTopCover(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			Index;
	char            barcode_top_cover[128];
	char 			msg[256];		
	
	
	if (!strcmp(me->barcode_top_cover, "none"))
		{
	//		printf("barcode rear cover = none\r\n");
			sprintf(msg, "probléme lecture BarCode Top Cover ! '(%s)'", me->barcode_top_cover);
			PANEL_MSG( PANEL_HELP2, msg);
			if(pError) *pError = -1;
			CHECK_ERR( *pError);
		}
	
	/* delete CR LF */
	Index = FindPattern (me->barcode_top_cover, 0, -1, "\r\n", 0, 0);
	CopyBytes (barcode_top_cover, 0, me->barcode_top_cover, 0, Index);
	barcode_top_cover[Index]=0;
		
   if(gs_Automat.Mode==AUTOMAT_MASTERPART && gs_Automat.master_active && gs_Automat.masterpart)
	{
		/* check if scanned barcode exist in the list of master parts of this version */
		if (strcmp(barcode_top_cover, MastersBarcodes[Actual_master_part]))
		{
			sprintf(msg, "Master BarCode non valide ! '(%s)'", barcode_top_cover);
			PANEL_MSG( PANEL_HELP2, msg);
			if(pError) *pError = -1;
			CHECK_ERR( *pError);
		}
	}
  
   else
	{
		EXCCHECK( gs_pLabels->CheckBarCode(gs_pLabels, "LabelTopCover:Check", -1, me->barcode_top_cover, &isCorrect, NULL));
	//	printf("Barcode Top  cover Correct=%d\n\r",isCorrect);
	//	printf("Barcode Top  cover =%s\n\r", me->barcode_top_cover);  
		if(!isCorrect)
		{
			PANEL_MSG( PANEL_HELP2, _("Invalid Rear Cover BarCode"));
			if(pError) *pError = -1;
			CHECK_ERR( *pError);
		}
		else if(gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.retest_active)
		{
			bool_t tested;

			EXCCHECK( gs_pLabels->GetBarCodeTopCoverStatus(gs_pLabels, barcode_top_cover, &tested, NULL));
	//	    printf("Barcode Top  cover =%s\n\r", barcode_top_cover); 
			if(tested)
			{
				PANEL_MSG( PANEL_HELP2, _("Rear Cover BarCode was tested"));
				if(pError) *pError = -1;
				CHECK_ERR( *pError);
			}
		}
		else if(gs_Automat.Mode==AUTOMAT_QUALITY_RETEST)
		{
			bool_t tested, failed;

			EXCCHECK( gs_pLabels->GetBarCodeTopCoverStatus(gs_pLabels, barcode_top_cover, &tested, &failed));
			if(!tested)
			{
				PANEL_MSG( PANEL_HELP2, _("Top Cover BarCode has not been tested"));
				if(pError) *pError = -1;
				CHECK_ERR( *pError);
			}
		}
	}
	
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Printer_Ready_Reset"
SElExceptionPtr Printer_Ready_Reset(struct _SSledge* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[256]= "";
	int             index;	
	SPrinterPtr		printer;
	int 			iStatus;
	clock_t 		_start_clock;
	double   		time;	
	int				StatusOnline;
	int				Interpreter;
	int				AmmountLabels;
	char			Error[128] = "";
	char			sAmmount[128] = "";	
	
	
	gs_pPrinters->GetPrinterByIndex(gs_pPrinters, 0, &printer);

	EXCCHECK( gs_pTester->Activate_Air_Printer(gs_pTester));
	Sleep(500);
	/* make reset Printer */
	printer->Reset(printer);
	Sleep(500);
	
	/* Check if Printer Reset */
	_start_clock =  clock();
	do{
		Sleep(500); 
		printer->Status(printer, &StatusOnline, Error, &AmmountLabels, &Interpreter);
		time=clock()-_start_clock;
		if (StatusOnline == 1 && strcmp (Error, "No error") == 0 && Interpreter == 0)		
		 	break;							
		TIMEOUT_CHECK (5000);
	 }while (TRUE);
	
	/* make Ready Printer */
	//EXCCHECK( gs_pTester->ReadyPrint(gs_pTester, 6000));
		
Error: 
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelLabel"
SElExceptionPtr JigPanelLabel(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[256]= "";
	int             index;	
	SPrinterPtr		printer;
	int 			iStatus;
	clock_t 		_start_clock;
	double   		time=0;	
	int				StatusOnline;
	int				Interpreter;
	int				AmmountLabels;
	char			Error[128] = "";
	char			sAmmount[128] = "";	
	int32_t			microCam;
	int32_t			printer_input2 = FALSE;
		
		PANEL_STATION_STATUS(6,2);
		StatusStations[6] = FALSE;	
		gs_pPrinters->GetPrinterByIndex(gs_pPrinters, 0, &printer);
		
		/* Printer Ready */
    	Printer_Ready_Reset(me);
	
		/* Printer Send data label */
		EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", NULL));  //Event Send label to printer if Test OK
		//printf(" *************************************  JigPanelLabel: Printer Send data label ************************************** \r \n");
		time=clock()-_start_clock;

		/* Check if data already sent to Printer */
		_start_clock =  clock();
		do{
				StatusStations[6] = FALSE;				
				Sleep(300);
				printer->Status(printer, &StatusOnline, Error, &AmmountLabels, &Interpreter);
				time=clock()-_start_clock;
				if (StatusOnline == 1 && Interpreter == 1)		
				 	break;

			}while (time<6000);

					
		
		/* Start Applicateur  */
		//printf("JigPanelLabel : CyclePrint \r \n");			
		EXCCHECK (gs_pTester->CyclePrint(gs_pTester, 10000));

		if (gs_Automat.EmtyTable == TRUE) //mode vidage
			Sleep(6000); //applicateur pas en pos initial
		
        PANEL_STATION_STATUS(6,OK);
Error: 
	if (pexception)	
	{	
		PANEL_STATION_STATUS(6,NOK);
	}		
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelLabelFail"
SElExceptionPtr JigPanelLabelFail(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[256]= "";
	int             index;	
	SPrinterPtr		printer;
	int 			iStatus;
	clock_t 		_start_clock;
	double   		time=0;	
	int				StatusOnline;
	int				Interpreter;
	int				AmmountLabels;
	char			Error[128] = "";
	char			sAmmount[128] = "";	
	int32_t			microCam;
	int32_t			printer_input2 = FALSE;
		
		PANEL_STATION_STATUS(6,2);
		StatusStations[6] = FALSE;	
		gs_pPrinters->GetPrinterByIndex(gs_pPrinters, 0, &printer);

		/* Check if data already sent to Printer */
		_start_clock =  clock();
		do{
				StatusStations[6] = FALSE;				
				Sleep(300);
				printer->Status(printer, &StatusOnline, Error, &AmmountLabels, &Interpreter);
				time=clock()-_start_clock;
				if (StatusOnline == 1 && Interpreter == 1)		
				 	break;

			}while (time<10000);
			

				
		/* Start Applicateur  */
		//printf("JigPanelLabelFail : CyclePrint \r \n");
		EXCCHECK (gs_pTester->CyclePrint(gs_pTester, 10000));
	
		if (gs_Automat.EmtyTable == TRUE) //mode vidage
			Sleep(6000); //applicateur pas en pos initial
		
        PANEL_STATION_STATUS(6,OK);
Error: 
	if (pexception)	
	{	
		PANEL_STATION_STATUS(6,NOK);
	}		
	EXCRETHROW( pexception);  
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelLabel2"
SElExceptionPtr JigPanelLabel2(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[256]= "";
	int             index;	
	SPrinterPtr		printer;
	int 			iStatus;
	clock_t _start_test_clock;
	double   time;	
		
		StatusStations[7] = FALSE;

		//printf("Executing jigpanel label 2\r\n");
		EXCCHECK( gs_pTester->GetItemCode(gs_pTester, "", 2, BARCODE_BUFFER_SIZE, Buffer, 3000));//Read Label Barcode
		
		sprintf(me->barcode, "%s", Buffer);
		EXCCHECK( JigPanelBarcode (me, pError));

Error: 
//	CyclePrintStatus(StatusOnline,Interpreter); 
	EXCRETHROW( pexception); 		
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelReadBarcodeOK"
SElExceptionPtr JigPanelReadBarcodeOK(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[256]= "";
	int             index;	
	SPrinterPtr		printer;
	int 			iStatus;
	clock_t _start_test_clock;
	double   time;	
		
		StatusStations[7] = FALSE;

		EXCCHECK( gs_pTester->GetItemCode(gs_pTester, "", 2, BARCODE_BUFFER_SIZE, Buffer, 3000));//Read Label Barcode
		
		sprintf(me->barcode, "%s", Buffer);

Error: 
	if (pexception)	PANEL_MSG( PANEL_HELP2, _("Erreur Lecture Code à barre"));
	else PANEL_MSG( PANEL_HELP2, _("Code à barre OK"));
//	CyclePrintStatus(StatusOnline,Interpreter); 
	EXCRETHROW( pexception); 		
}



/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetNewSerial"
SElExceptionPtr GetNewSerial(int* counter, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	EIdShift	shift_id = 0;
	bool_t		isOpen = FALSE;
	


		EXCCHECK( gs_pDbs->Storage->IsShiftOpen(gs_pDbs->Storage, &isOpen, &shift_id));
		if(isOpen)
		{
			pexception = gs_pDbs->Storage->GetProductCounter(gs_pDbs->Storage, gs_pDbs->TesterProductId, counter);
			if(pexception)
			{	
			EXCDELETE(pexception);
			if(pError) *pError = -1;
			CHECK_ERR( *pError);
			}
		}

Error:
	EXCRETHROW( pexception);  
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelInsert"
static SElExceptionPtr JigPanelInsert(int32_t* pError)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;

		//PANEL_MSG( PANEL_HELP2, _("Table Rotation"));
	  //  printf ("JigPanelInsert\r\n");
		gs_pTester->SetSensor(gs_pTester, "INDICATOR_OK", TRUE);
		PANEL_TABLE_WARNING (PANEL_WARNING , TRUE);
		Sleep(250);  /* wait ready all actuators */
	//	printf("Execution of jig panel insert\n\r")  ;
		pexception = gs_pTester->TableRotate(gs_pTester, ROTATION_TIMEOUT, TRUE);
	//	printf("Execution of jig panel insert after Rotation\n\r")   ;
		if(pexception)
		{	
		EXCDELETE(pexception);
		if(pError) *pError = -1;
		CHECK_ERR( *pError);
		}
			
Error:
	PANEL_TABLE_WARNING (PANEL_WARNING , FALSE);
	gs_pTester->SetSensor(gs_pTester, "INDICATOR_OK", FALSE);
	EXCRETHROW( pexception);  
}
 
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelInsert2"
static SElExceptionPtr JigPanelInsert2(int32_t* pError)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;

	//	printf ("JigPanelInsert_2\r\n");
	    PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
		gs_pTester->SetSensor(gs_pTester, "INDICATOR_OK", TRUE);
		PANEL_TABLE_WARNING (PANEL_WARNING , TRUE);
		Sleep(500);  /* wait ready all actuators */
		//printf("Ready for TableRotate\r\n");
		EXCCHECK(gs_pTester->TableRotate(gs_pTester, ROTATION_TIMEOUT, TRUE));
																						
Error:																	   
	PANEL_TABLE_WARNING (PANEL_WARNING , FALSE);
	gs_pTester->SetSensor(gs_pTester, "INDICATOR_OK", FALSE);
	EXCRETHROW( pexception);  
}

#undef __FUNC__
#define __FUNC__ "PanelRemove"
static SElExceptionPtr PanelRemove(void)
{
SElExceptionPtr pexception = NULL;
int32_t error = 0;
    bool_t              insert1=0, insert2=0, status_barrier;


	StatusStations[0] = BUSY;

	do{
	ProcessDrawEvents ();
	ProcessSystemEvents ();
	
//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_LEFT", &insert1));
//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_RIGHT", &insert2));
	
	EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
//		printf("Insert 1= %d: Insert 2=%d Outside function\n\r",insert1, insert2); 
	if (insert1 == 0 &&  insert2 == 0)
		break;

	Sleep (500);
	
	}while (TRUE);

Error:
EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelRelease"
static SElExceptionPtr JigPanelRelease(struct _SSledge* me, bool_t fault)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	SPrinterPtr		printer;
	int 			iStatus;
	clock_t 		_start_clock;
	double   		time;	
	int				StatusOnline;
	int				Interpreter;
	int				AmmountLabels;
	char			Error[128] = "";
	char			sAmmount[128] = "";
	char            Buffer[256]= "";


	if(fault)   //Print FAIL Label & put panel in the Red BOX
	{
			PANEL_STATION_STATUS(7,0);
		    EXCCHECK(JigPanelLabelFail(me,fault));
			CHECK_ERR( error);  
			Sleep(500);
			StatusStations[6] = TRUE;
		   
			WaitRotation (INFINITE);
			   
			if (gs_Automat.PICK_PLACE_active)
			{
				StatusStations[7] = FALSE;
				EXCCHECK( gs_pTester->UnlockPanelStation80(gs_pTester, 5000));
				Sleep(500);//wait pallett open
				EXCCHECK( gs_pTester->PickPlace(gs_pTester, 15000, TRUE));
				Sleep(500);//wait pallett open
				//printf("Ready for Reset Pick& Place JigPanelRelease\r\n");
				EXCCHECK( gs_pTester->ResetPickPlace(gs_pTester, 15000, TRUE));
				Sleep(500);//wait pallett open
				//printf("Ready for LockPanelStation80 JigPanelRelease\r\n"); 
				EXCCHECK( gs_pTester->LockPanelStation80(gs_pTester, 2000));
			}
		
			StatusStations[7] = TRUE;
			PANEL_STATION_STATUS(7,1);
			
			EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  don't display TEST NOK in station 8 */
		
			
			PANEL_MSG( PANEL_RESULT_MSG, _("TEST NOK"));
			PANEL_STATION_STATUS(0,3);
	}
	else 
	{		   
		if (gs_Automat.PRINTER_active)
			{  
			if (!strcmp(me->barcode,"none"))
				{  
					PANEL_STATION_STATUS(7,1);	
					if (gs_Automat.PICK_PLACE_active)
				        {
				           EXCCHECK( gs_pTester->UnlockPanelStation80(gs_pTester, 5000));
				           Sleep(500);//wait pallett open
				           EXCCHECK( gs_pTester->PickPlace(gs_pTester, 15000, TRUE));
				           Sleep(500);//wait pallett open
				           EXCCHECK( gs_pTester->ResetPickPlace(gs_pTester, 15000, TRUE));
				           Sleep(500);//wait pallett open
				           EXCCHECK( gs_pTester->LockPanelStation80(gs_pTester, 2000));  }
				        }
				else 
					
				{	
					PANEL_STATION_STATUS(7,0); 
				}
			}
	
		else 
		{	
			PANEL_STATION_STATUS(7,0);	 
		}

		StatusStations[7] = TRUE;
		
		EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  don't display TEST OK in station 8 */
	
		if (strcmp(me->barcode,"none"))
		{
		//	printf("Test OK \r\n");
		PANEL_MSG( PANEL_RESULT_MSG, _("TEST OK"));
		PANEL_STATION_STATUS(0,3);  //  	PANEL_STATION_STATUS(0,3);
		PANEL_MSG( PANEL_HELP2, _("Enlever la piece bonne"));
		StatusStations[0] = TRUE;
	//	EXCCHECK( PanelRemove());
	//	printf("Status station man\n\r");
		}
		else
		{
		if (gs_Automat.PRINTER_active)
		{
		PANEL_MSG( PANEL_RESULT_MSG, _("TEST NOK"));
		MessagePopup ("Erreur Etiquette Client", "Erreur Lecture Etiquette");
		}
		else
		PANEL_MSG( PANEL_RESULT_MSG, _("TEST OK"));
		PANEL_STATION_STATUS(0,3);
	//	printf("PANEL_STATION_STATUS\n\r");
		}
			}
	
Error:
	if (pexception)
	{
	if (gs_Automat.PICK_PLACE_active)
			gs_pTester->ResetPickPlace(gs_pTester, 15000, TRUE);
	gs_pTester->LockPanelStation80(gs_pTester, 2000);
		
	PANEL_STATION_STATUS(0,3);
	}
	StatusStations[7] = TRUE;
	eleventdata_delete(&(me->pData));
	EXCRETHROW( pexception);  
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelRelease2"
static SElExceptionPtr JigPanelRelease2(struct _SSledge* me, bool_t fault)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	int32_t				i = 0;
	bool_t 				insert1=0,insert2=0;
	SPrinterPtr			printer;
	int 				iStatus;
	clock_t 			_start_clock;
	double   		time;	
	int				StatusOnline;
	int				Interpreter;
	int				AmmountLabels;
	char			Error[128] = "";
	char			sAmmount[128] = "";
	char			msg[128] = "";


	if(fault)  
	{
			PANEL_STATION_STATUS(6,3);
			ProcessSystemEvents ();
			StatusStations[6] = TRUE;
	}

	else 
	{
		if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_AUTO_active)
				{
					
				StatusStations[6] = FALSE;
				gs_pPrinters->GetPrinterByIndex(gs_pPrinters, 0, &printer);

				/* Check if data already sent to Printer */
				_start_clock =  clock();
				do{
						StatusStations[6] = FALSE;
						Sleep(100);//TEST old  Sleep(300);
						printer->Status(printer, &StatusOnline, Error, &AmmountLabels, &Interpreter);
						if (StatusOnline == 1 && Interpreter == 1)		
		  					break;
						TIMEOUT_CHECK (5000);
				}while (TRUE);
		
				/* Print label */								  
				EXCCHECK( gs_pTester->CyclePrint(gs_pTester, 10000));
				 	
				if (gs_Automat.EmtyTable == TRUE) //mode vidage
					Sleep(6000); //applicateur pas en pos initial

				StatusStations[6] = FALSE;				
				}
		
		if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_MAN_active)
			{
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TEST_SEQUENCE_END", me->pData));
			}
			
		
		PANEL_STATION_STATUS(6,0);
		StatusStations[6] = TRUE;
		ProcessSystemEvents ();
		ProcessDrawEvents ();
	}
	
Error:
	StatusStations[6] = TRUE;
	EXCRETHROW( pexception);  
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::GetEventHandle"
static SElExceptionPtr GetEventHandle(void)
{
	SElExceptionPtr	pexception = NULL;
   
	if ( gs_EventFactory )
	{
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_PRODUCTION",
		                                   &gs_EventHandle[EVNT_GUI_PRODUCTION]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_QUALITY_RELEASE",
		                                   &gs_EventHandle[EVNT_GUI_QUALITY_RELEASE]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_QUALITY_RETEST",
		                                   &gs_EventHandle[EVNT_GUI_QUALITY_RETEST]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_QUALITY_MASTER",
		                                   &gs_EventHandle[EVNT_GUI_QUALITY_MASTER]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_EXPERTISE_STEP_MODE",
		                                   &gs_EventHandle[EVNT_GUI_EXPERTISE_STEP_MODE]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_EXPERTISE_GO_MODE",
		                                   &gs_EventHandle[EVNT_GUI_EXPERTISE_GO_MODE]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_EXPERTISE_SEQUENCE_CREATED",
		                                   &gs_EventHandle[EVNT_GUI_EXPERTISE_SEQUENCE_CREATED]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_EXPERTISE_RNR",
		                                   &gs_EventHandle[EVNT_GUI_EXPERTISE_RNR]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_TEST_SEQUENCE_RUN",
		                                   &gs_EventHandle[EVNT_TEST_SEQUENCE_RUN]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_PRODUCT_SELECTED",
		                                   &gs_EventHandle[EVNT_GUI_PRODUCT_SELECTED]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_EXPERTISE_STEP_MODE_NEXT",
		                                   &gs_EventHandle[EVNT_EXPERTISE_STEP_MODE_NEXT]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_EXPERTISE_STEP_MODE_REPEAT",
		                                   &gs_EventHandle[EVNT_EXPERTISE_STEP_MODE_REPEAT]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_EXPERTISE_STEP_MODE_BACK",
		                                   &gs_EventHandle[EVNT_EXPERTISE_STEP_MODE_BACK]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_EXPERTISE_STEP_MODE_STOP",
		                                   &gs_EventHandle[EVNT_EXPERTISE_STEP_MODE_STOP]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_GUI_KEY_ESC_PRESSED",
		                                   &gs_EventHandle[EVNT_GUI_KEY_ESC_PRESSED]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_RESET_PRODUCT",
		                                   &gs_EventHandle[EVNT_RESET_PRODUCT]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_JIG_ITEM_INSERTED",
		                                   &gs_EventHandle[EVNT_JIG_ITEM_INSERTED_]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_JIG_ITEM_RELEASED",
		                                   &gs_EventHandle[EVNT_JIG_ITEM_RELEASED_]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_JIG_CYCLE_START",
		                                   &gs_EventHandle[EVNT_JIG_CYCLE_START_]));
		EXCCHECK( gs_EventFactory->GetEvent(gs_EventFactory,
		                                   "EVNT_JIG_START",
		                                   &gs_EventHandle[EVNT_JIG_START_]));

	}
    
Error:
	EXCRETHROW(pexception);
}  

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionCallback"
static SElExceptionPtr EventFunctionCallback( const char* eventName, void* pData)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( fncLock());

	if(0==strcmp(eventName, "EVNT_JIG_LOADED") || 0==strcmp(eventName, "EVNT_JIG_UNLOADED") )
	{
		EXCCHECK( ProcessEventGuiProductSelected(!gs_Automat.disabled, TRUE, TRUE));
		/*if( gs_EventFactory && !gs_Automat.disabled)
		{
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_PLUGIN_LEAVE", NULL));	   //ARAIBIA
		}*/
	}
	else if(0==strcmp(eventName, "EVNT_JIG_ITEM_RELEASED"))
	{
		EXCCHECK( ProcessEventJigItemReleased()); 
	}
	else if(0==strcmp(eventName, "EVNT_JIG_ITEM_INSERTED"))
	{
		EXCCHECK( ProcessEventJigItemInserted()); 
	}

Error:   
	fncUnlock();
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EventFunctionPluginCallback"
static SElExceptionPtr EventFunctionPluginCallback( const char* eventName, void* pData)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
	const char*		uuid = NULL;
	int32_t			full;
	int32_t			closed;
	int32_t			finished;
	int32_t			erreur;
	SElEventDataPtr pdata = NULL;
	int32_t			count;
	int32_t			master_id;
	int32_t			product_id;
	int32_t			is_Error = 0;
	char			Msg[128]; 
	int32_t			insert1=0, insert2=0;

	EXCCHECK( fncLock());

	if( pData)
		uuid = ((SElEventDataPtr)pData)->GetStringPtr(pData);

	if(0==strcmp(eventName, "EVNT_GUI_TEST"))
	{
	EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, EVNT_JIG_CYCLE_START, NULL));
	}
		
	if(0==strcmp(eventName, "EVNT_PLUGIN_RUN"))
	{
		if( uuid &&( 0==strcmp(uuid, PLUGIN_MAINTENANCE)
					 || 0==strcmp(uuid, PLUGIN_REPORT) 
					 || 0==strcmp(uuid, PLUGIN_STATISTIC)
					 || 0==strcmp(uuid, PLUGIN_PRODUCT) ))
		{
			EXCCHECKCVI( PostDeferredCall(PostDeferredHidePanel, NULL));
			gs_Automat.disabled = TRUE;
		}
	}

	else if(0==strcmp(eventName, "EVNT_GUI_SHIFT"))
	{
		gs_Automat.shift_id = strtol(uuid, NULL, 10);

		if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.shift_id!=0)
		{
			PANEL_MSG( PANEL_HELP1, _("Production")); 
		
		if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_MAN_active)
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", NULL));
			
#if 0    /* formerly excluded lines */
			/* Start CIM+ */
			EXCDISPLAY( StartBarflow());
#endif   /* formerly excluded lines */
			
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			
		 if (full)	   // Old value (!full) changed by oussama       
			{
				PANEL_MSG( PANEL_HELP2, _("Vider goulotte rouge"));
			}
			else if (!closed)
			{
				PANEL_MSG( PANEL_HELP2, _("Fermer tiroir goulotte rouge"));
			}
			
			else if( gs_Automat.box)
			{
				PANEL_MSG( PANEL_HELP2, _("Insert Empty Box"));
				INSERT_EMPTY_BOX();
			}
			else
			{
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_LEFT", &insert1));
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_RIGHT", &insert2));
			
				EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
				if (gs_Automat.EmtyTable == TRUE) //mode vidage 
					{   
					if (insert1 || insert2)
						PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
					else if (!insert1 && !insert2)
					{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));  
					}
					}
		
				else 	  //mode production normal
					{	
					if (!insert1 || !insert2)
						PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
					else if (insert1 && insert2)
                    	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle")); 
					}
			}
		}

		else if( gs_Automat.Mode==AUTOMAT_MASTERPART && gs_Automat.shift_id!=0)
		{
			//printf("AUTOMAT_MASTERPART\r\n");	
			EXCCHECK( GetMasters ());

				if (number_of_master_parts == 0) 
				{
				/* reload production testsequence */
				gs_Automat.Mode = AUTOMAT_PRODUCTION;
				
				gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;
				
				gs_Automat.masterpart = FALSE;
				
				EXCCHECK(  ProcessEventGuiProduction ());
				
				if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_MAN_active)
					EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", NULL));
				
				PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
	
				}
				else 
				{
				PANEL_MSG( PANEL_HELP1, _("Master Part Sequence")); 

				/* Load first master barcode */
				EXCCHECK( ProcessEventMasterSequenceRun (TRUE));
				}
		}

	}

	else if(0==strcmp(eventName, "EVNT_GUI_END_VERSION"))
	{
	if(gs_Automat.shift_id!=0 && gs_Automat.EmtyTable==FALSE && gs_Automat.isTableEmpty == FALSE)
		{
			if(gs_Automat.Mode!=AUTOMAT_QUALITY_MASTER)
			{
			PANEL_MSG( PANEL_HELP1, _("End Of Actual Version"));
			}

			/* Check if product in pallett */
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
			
			EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
	    	//printf("Insert 1= %d: Insert 2=%d EVNT_GUI_END_VERSION\n\r",insert1, insert2); 
			if (!insert1 && !insert2)
			{	
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			}
			else 
			{
			pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 7000);
			PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
			}
			gs_Automat.EmtyTable = TRUE;
			gs_Table_Rotation = 0;
			
		}
	}

	else if(0==strcmp(eventName, "EVNT_GUI_CHANGE_VERSION"))
	{
		
		#if _CHANGE_VERSION_NO_EMPTY	
								   
		gs_Automat.isTableEmpty = TRUE;
	
		#endif

		if( gs_Automat.shift_id!=0 && gs_Automat.isTableEmpty == TRUE)
		{
		EXCDISPLAY( DisplayVersionList());
		}
		
		else if( gs_Automat.shift_id!=0 && gs_Automat.isTableEmpty == FALSE && gs_Automat.EmtyTable==FALSE)
		{
			//End Actual Version
			PANEL_MSG( PANEL_HELP1, _("End Of Actual Version")); 

			/* Check if product in pallett */
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
			
					EXCCHECK( TestProductPresence(&error, &insert1, &insert2));
					//printf("Insert 1= %d: Insert 2=%d EVNT_GUI_CHANGE_VERSION\n\r",insert1, insert2); 
			if (!insert1 && !insert2)
			{	
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			}
			else 
			{
			pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 7000);
			PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
			}
			gs_Automat.EmtyTable = TRUE;
			gs_Automat.ChangeVersion = TRUE;
			gs_Table_Rotation = 0;
		}
	}
		
	else if(0==strcmp(eventName, "EVNT_BOX_INSERTED"))
	{
		if( gs_Automat.Mode==AUTOMAT_PRODUCTION)
		{

			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			
		 if (full)	   // Old value (!full) changed by oussama       
			{
				PANEL_MSG( PANEL_HELP2, _("Vider goulotte rouge"));
			}
			else if (!closed)
			{
			PANEL_MSG( PANEL_HELP2, _("Fermer tiroir goulotte rouge"));
			}
			else if(gs_Automat.shift_id==0)
			{
				PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
				PANEL_MSG( PANEL_HELP2, "");
			}
			else
			{
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_LEFT", &insert1));
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_RIGHT", &insert2));
			
					EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
		            //printf("Insert 1= %d: Insert 2=%d EVNT_BOX_INSERTED\n\r",insert1, insert2);  
				if (gs_Automat.EmtyTable == TRUE) //mode vidage 
					{
					if (insert1 || insert2)
						PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
					else if (!insert1 && !insert2)
					{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));  
					}
					}
		
				else 	  //mode production normal
					{	
					if (!insert1 || !insert2)
						PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
					else if (insert1 && insert2)
					{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));   
					}
					}	
				
			}

		}
	}

	else if(0==strcmp(eventName, "EVNT_RED_BOX_CLOSED"))
	{
			PANEL_MSG( PANEL_HELP2, _("Fermer tiroir goulotte rouge"));
	}

	else if(0==strcmp(eventName, "EVNT_RED_BOX_EMPTY"))
	{
		
			PANEL_MSG( PANEL_HELP2, _("Vider goulotte rouge"));
	}

	
#if 0    /* formerly excluded lines */
	else if(0==strcmp(eventName, "EVNT_PRINTER_READY"))
	{
		{
			gs_Automat.box = FALSE;

			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			
		else if (full)	   // Old value (!full) changed by oussama       
			{
				PANEL_MSG( PANEL_HELP2, _("Vider goulotte rouge"));
			}
			else if (!closed)
			{
			PANEL_MSG( PANEL_HELP2, _("Fermer tiroir goulotte rouge"));
			}
			else if(gs_Automat.shift_id==0)
			{
				PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
				PANEL_MSG( PANEL_HELP2, "");
			}
			else
			{
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_LEFT", &insert1));
		//	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_GO_RIGHT", &insert2));
			
			EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
	     	//printf("Insert 1= %d: Insert 2=%d EVNT_PRINTER_READY\n\r",insert1, insert2);  
				if (gs_Automat.EmtyTable == TRUE) //mode vidage 
					{
					if (insert1 || insert2)
						PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
					else if (!insert1 && !insert2)
						PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
					}
		
				else 	  //mode production normal
					{	
					if (!insert1 || !insert2)
						PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
					else if (insert1 && insert2)
					{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle")); 
					}
					}	
				
			}
		}
		
	}
#endif   /* formerly excluded lines */

	else if(0==strcmp(eventName, "EVNT_BOX_CHANGE"))
	{
		if(gs_Automat.box_active)
			gs_Automat.box = TRUE;
	}

	else if(0==strcmp(eventName, "EVNT_PAUSE_STARTED"))
	{
		EXCCHECKCVI( PostDeferredCall(PostDeferredHidePanel, NULL));
		gs_Automat.disabled = TRUE;

		gs_Automat.pause = TRUE;

		EXCCHECK( ProcessEventGuiProductSelected(TRUE, FALSE, FALSE));
		if( gs_EventFactory)
		{
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_PLUGIN_LEAVE", NULL));
		}
	}

	else if(0==strcmp(eventName, "EVNT_PAUSE_STOPED"))
	{
		gs_Automat.pause = FALSE;

		EXCCHECK( ProcessEventGuiProductSelected(TRUE, FALSE, FALSE));
		if( gs_EventFactory)
		{
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_PLUGIN_LEAVE", NULL));
		}

		EXCCHECKCVI( PostDeferredCall(PostDeferredDisplayPanel, NULL));
		gs_Automat.disabled = FALSE;
	}

	else if(0==strcmp(eventName, "EVNT_GUI_PRODUCTION")
		  || 0==strcmp(eventName, "EVNT_GUI_PRODUCT_SELECTED"))
	{
		if(!gs_Automat.pause)
		{
			EXCCHECKCVI( PostDeferredCall(PostDeferredDisplayPanel, NULL));
			gs_Automat.disabled = FALSE;
		}
		else
		{
			EXCCHECKCVI( PostDeferredCall(PostDeferredHidePanel, NULL));
			gs_Automat.disabled = TRUE;
		}
	}

	else if( 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_STEP_MODE")
		  || 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_GO_MODE")
		  || 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_SEQUENCE_CREATED")  
		  || 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_RNR")
		  || 0==strcmp(eventName, "EVNT_GUI_QUALITY_MASTER")
		  || 0==strcmp(eventName, "EVNT_GUI_QUALITY_RETEST"))
	{
		EXCCHECKCVI( PostDeferredCall(PostDeferredDisplayPanel, NULL));
		gs_Automat.disabled = FALSE;
		
		if (0==strcmp(eventName, "EVNT_GUI_EXPERTISE_SEQUENCE_CREATED")) 
		{
			
			/* Get data [Station+Repeat] for R&R  */ 
			const char* buffer = ((SElEventDataPtr)pData)->GetStringPtr(pData);   

			char	data[10];
			char*	pdata = NULL;
			char*	pdata2 = NULL;
			
			strcpy_s(data, sizeof(data), buffer);
			
			pdata = data;				/* Station */
			pdata2 = strchr(pdata, ';');
			if (pdata2 != NULL)			/*Repeat */
			{
				*pdata2 = '\0';
				pdata2++;
			}
			
			gs_RnR_Station = strtol(pdata, NULL, 10);
			gs_RnR_Repeat = strtol(pdata2, NULL, 10);
			
		}
	}

Error:   
	fncUnlock();
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RegisterEventFunction"
static SElExceptionPtr RegisterEventFunction(void)
{
	SElExceptionPtr	pexception = NULL;
	
	if ( gs_EventFactory )
	{
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_TEST",
														 EventFunctionPluginCallback));	
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_JIG_LOADED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_JIG_UNLOADED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_JIG_ITEM_RELEASED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_JIG_ITEM_INSERTED",
														 EventFunctionCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_PLUGIN_RUN",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_SHIFT",
														 EventFunctionPluginCallback));	
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_END_VERSION",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_CHANGE_VERSION",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_PAUSE_STARTED",
														 EventFunctionPluginCallback));	
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_PAUSE_STOPED",
														 EventFunctionPluginCallback));	
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_PRODUCTION",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_EXPERTISE_STEP_MODE",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_EXPERTISE_GO_MODE",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_EXPERTISE_SEQUENCE_CREATED",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_EXPERTISE_RNR",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_QUALITY_MASTER",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_QUALITY_RETEST",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_PRODUCT_SELECTED",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_BOX_CHANGE",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_BOX_INSERTED",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_RED_BOX_CLOSED",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_RED_BOX_EMPTY",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_PRINTER_READY",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_TESTER_BUTTON_PRESSED_LABEL",
														 EventFunctionPluginCallback));
		
	}

Error:
	EXCRETHROW(pexception);
}	/* RegisterEventFunction */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadConfiguration"
static SElExceptionPtr LoadConfiguration(
   const char*   fileName
)
{
	SElExceptionPtr pexception = NULL;
	int32_t			error = 0;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;   
	const char*     pfile_name = (fileName) ? fileName : "\\automat.xml";
	VBOOL         	is_xml_success = 1;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*         	ptext= NULL;
	char			path[MAX_PATHNAME_LEN ];  

	EXCCHECKCVI( GetModuleDir (__CVIUserHInst, path));  
	strcat(path, pfile_name);
	
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, path));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//plugin[@id='"PLUGIN_AUTOMAT"']", &pnode));   
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/master_panel", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.master_active = TRUE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}

	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/retest_quality", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.retest_active = TRUE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}

	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/box", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.box_active = TRUE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
    EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/CIM", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.CIM_PLUS_active = TRUE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}

	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/PRINTER", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.PRINTER_active = TRUE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/PRINTER_AUTO", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.PRINTER_AUTO_active = TRUE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/PRINTER_MAN", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.PRINTER_MAN_active = TRUE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
	
	
    EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/PICK_PLACE", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.PICK_PLACE_active = TRUE;
			 else 
				gs_Automat.PICK_PLACE_active = FALSE;
			
			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/panel_release", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "QUALITY"))
				gs_Automat.release_mode = E_PRM_QUALITY;
			else if(0==strcmp(ptext, "NOK"))
				gs_Automat.release_mode = E_PRM_NOK;
			else
				gs_Automat.release_mode = E_PRM_FALSE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}

	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/printer_label", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.label_active = TRUE;

			CA_FreeMemory(ptext);
			ptext=NULL;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}

Error:
	CA_VariantClear(&pfile_name_var);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#if 0    /* formerly excluded lines */
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Start_Alim_Zup"
SElExceptionPtr Start_Alim_Zup (void)
{
SElExceptionPtr	pexception = NULL;
int32_t			error = 0;
int RS232Error;
int i, stringsize;
char send_data[256];
int comport = 5;


    RS232Error = OpenComConfig (comport, "", 9600, 0, 8, 1, 512, 512);
	if (RS232Error < 0)
		 EXCTHROW( TEST_ERR_TIMEOUT, "Erreur Ouverture Port ALIM COM6");
	
	
		
	sprintf (send_data , ":RMT1;\r");
	stringsize = StringLength (send_data);
	/* Envoi octet par octet */
		for (i = 0; i < stringsize; i++)
		{
			ComWrtByte (comport, send_data [i]);
			Delay (0.001);
		}
		
	Delay (0.1);
		
	sprintf (send_data , ":ADR01;\r");
	stringsize = StringLength (send_data);
	/* Envoi octet par octet */
	for (i = 0; i < stringsize; i++)
		{
			ComWrtByte (comport, send_data [i]);
			Delay (0.001);
		}

	Delay (0.1);
	
#if 0    /* formerly excluded lines */
	sprintf (send_data , "SOUR:VOLT %05.2f V\n\r", 12.0);
	stringsize = StringLength (send_data);
	/* Envoi octet par octet */
	for (i = 0; i < stringsize; i++)
		{
			ComWrtByte (comport, send_data [i]);
			Delay (0.001);
		}

	Delay (0.1);
	
	sprintf (send_data , "SOUR:CURR %05.2f A\n\r", 1.0);
	stringsize = StringLength (send_data);
	/* Envoi octet par octet */
	for (i = 0; i < stringsize; i++)
		{
			ComWrtByte (comport, send_data [i]);
			Delay (0.001);
		}

	Delay (0.1);
#endif   /* formerly excluded lines */
	
	
	sprintf (send_data , ":OUT1;\r");
	stringsize = StringLength (send_data);
	/* Envoi octet par octet */
	for (i = 0; i < stringsize; i++)
		{
			ComWrtByte (comport, send_data [i]);
			Delay (0.001);
		}
	
Error:
	EXCRETHROW( pexception);
}


/*---------------------------------------------------------------------------*/
#endif   /* formerly excluded lines */
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
			//_CrtSetBreakAlloc(77);
#endif
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)		
				return 0;	/* out of memory */
			break;
		case DLL_PROCESS_DETACH:
			if (!CVIRTEHasBeenDetached())		/* Do not call CVI functions if cvirte.dll has already been detached. */
			{
				DiscardUIObjectsForDLL ();		/* Discard the panels loaded in InitUIForDLL */
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
#define __FUNC__ "{Automat Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	const char*		pconfiguration_file = NULL;
	int32_t			main_panel = 0;
	int				monitor_id, top, height;  

	InitializeCriticalSection( &_Lock);

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "MainPanel",
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
	
	EXCCHECKCVI( gs_panel[AUTOMAT] = LoadPanelEx (main_panel, "automat.uir", AUTOMAT, __CVIUserHInst));

	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "AutomatPanel", ARG_INT32, &gs_panel[PANEL]));  

	EXCCHECKCVI( GetSystemAttribute( ATTR_PRIMARY_MONITOR, &monitor_id));
    EXCCHECKCVI( GetMonitorAttribute (monitor_id, ATTR_TOP,    &top));
    EXCCHECKCVI( GetMonitorAttribute( monitor_id, ATTR_HEIGHT, &height));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[AUTOMAT], ATTR_TOP, top + height - 80 - 479));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[AUTOMAT], ATTR_LEFT, 11));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
									 "EventFactory",
									 sizeof(SElEventFactoryPtr),
									 &gs_EventFactory));
	
	if(gs_Modules)
	{
		gs_pLabels = (SLabelListPtr)gs_Modules->Get(gs_Modules, MODULE_LABELS);
		gs_pDbs = (SDBSPtr)gs_Modules->Get(gs_Modules, MODULE_DBS);
		gs_pBox = (SBoxPtr)gs_Modules->Get(gs_Modules, MODULE_BOX);
		gs_pTester = (STesterPtr)gs_Modules->Get(gs_Modules, MODULE_TESTER);
		gs_pPrinters = gs_Modules->Get(gs_Modules, MODULE_PRINTERS);
		gs_paxiss = gs_Modules->Get(gs_Modules, MODULE_AXIS);
		gs_IviBridge = (SIviBridgePtr)gs_Modules->Get(gs_Modules, MODULE_IVIBRIDGE);
		gs_pcameras = gs_Modules->Get(gs_Modules, MODULE_CAMERA);
	}

Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int i;
    TaskHandle taskOut;
	TaskHandle taskOut_2;
	SPrinterPtr		printer;
	StructAxisPtr   pthread_data_Axis;
	
	EXCCHECK( testseq_new(&gs_Automat.pTestSeq));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "TestSequence", ARG_PTR, gs_Automat.pTestSeq));  
	EXCCHECK( LoadConfiguration(NULL));

	EXCCHECK( GetEventHandle()); 
	EXCCHECK( RegisterEventFunction());

	EXCCHECKCVI( DisplayPanel(gs_panel[AUTOMAT]));

	
	gs_EventHandle[EVNT_STOP_THREAD] = CreateEvent( NULL, 1, 0, NULL);
	if (gs_EventHandle[EVNT_STOP_THREAD] == NULL)
		EXCTHROW(-1, "Create event failed");
		
	for(i=0; i< NB_OF_STATIONS ;i++)
		StatusStations[i] = 1;
	
	
	/*--- Read All Analog Channel from DAQ1 Simultaneously ----*/
	
	EXCCHECKIVIDAQ(DAQmxCreateTask("", &taskOut));
	
	//DAQmxErrChk (DAQmxCfgSampClkTiming(taskOut,"",10000,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
			
	EXCCHECKIVIDAQ(DAQmxCreateAIVoltageChan(taskOut, "Dev2/ai0:31", "",DAQmx_Val_NRSE, -10, 10, DAQmx_Val_Volts, ""));	

	EXCCHECKIVIDAQ(DAQmxStartTask (taskOut));

	Run_DAQ = 1;
	
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, ThreadDAQ, (void*)taskOut, &threadDAQId);	//TEST
	
	
		/*--- Read All Analog Channel from DAQ2 Simultaneously ----*/
	
	EXCCHECKIVIDAQ(DAQmxCreateTask("", &taskOut_2));
	
	//DAQmxErrChk (DAQmxCfgSampClkTiming(taskOut,"",10000,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
			
	EXCCHECKIVIDAQ(DAQmxCreateAIVoltageChan(taskOut_2, "Dev1/ai0:15", "",DAQmx_Val_RSE, -10, 10, DAQmx_Val_Volts, ""));	

	EXCCHECKIVIDAQ(DAQmxStartTask (taskOut_2));

	Run_DAQ_2 = 1;
	
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, ThreadDAQ_2, (void*)taskOut_2, &threadDAQId_2);	//TEST
	
	
//	/*--- Start Printer  ----*/
//#if 0    /* formerly excluded lines */
	if (gs_Automat.PRINTER_active)
	{
		gs_pPrinters->GetPrinterByIndex(gs_pPrinters, 0, &printer);
		EXCCHECK(printer->Disconnect (printer));
		EXCCHECK(printer->Connect (printer));
	}
//#endif   /* formerly excluded lines */
	
	
	EXCCHECK( ThreadStart(Thread_Automat, NULL, &gs_hThread, &hEventStop)); 

	/* Thread Security Z axis */
	pthread_data_Axis = (StructAxisPtr)calloc(1, sizeof(StructAxis));
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 2, &(pthread_data_Axis->paxis_station_40)));
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 3, &(pthread_data_Axis->paxis_station_50)));
	Run_Axis = 1;
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, Thread_Axis, (void*)pthread_data_Axis, &thread_Id_Axis);

	if(gs_Automat.box_active)
		gs_Automat.box = TRUE;

	EXCDISPLAY( StartBarflow());
	
	EXCCHECK( ProcessEventGuiProduction());


	if(gs_Automat.master_active)
	{
		gs_Automat.masterpart = TRUE;

		/* set mode passage piece type avant production */
		gs_Automat.Mode = AUTOMAT_MASTERPART;
	}
	
//	printf("ready to execute ProcessEventGuiResetProduct\r\n");
	EXCDISPLAY( ProcessEventGuiResetProduct()); //MA
	
		
//#endif   /* formerly excluded lines */
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;

	/* Stop Loop */
	Run_DAQ = 0;
	
	/* Wait for the thread functions to finish executing */
    CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE, threadDAQId, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
    
    /* Release thread functions */
    CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, threadDAQId);
	
	/* Stop Loop */
	Run_DAQ_2 = 0;
	
	/* Wait for the thread functions to finish executing */
    CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE, threadDAQId_2, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
    
    /* Release thread functions */
    CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, threadDAQId_2);

	
#if 0    /* formerly excluded lines */
	PulseEvent(gs_EventHandle[EVNT_STOP_THREAD]); 
	Delay(1);
#endif   /* formerly excluded lines */
	
Error:	
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
   				 
	/* Stop Loop */
	Run_DAQ = 0;
	
	/* Wait for the thread functions to finish executing */
    CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE, threadDAQId, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
    
    /* Release thread functions */
    CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, threadDAQId);

	EXCCHECK( ThreadStop( &gs_hThread, &gs_EventHandle[EVNT_STOP_THREAD]));
	
	/* Stop Loop */
	Run_DAQ_2 = 0;
	
	/* Wait for the thread functions to finish executing */
    CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE, threadDAQId_2, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
    
    /* Release thread functions */
    CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, threadDAQId_2);

	EXCCHECK( ThreadStop( &gs_hThread, &gs_EventHandle[EVNT_STOP_THREAD]));
	
	EXCCHECK( gs_Automat.pTestSeq->UnLoad(gs_Automat.pTestSeq));  
	EXCCHECK( testseq_delete(&gs_Automat.pTestSeq)); 
	
	if(gs_panel[AUTOMAT])
		EXCCHECKCVI( DiscardPanel(gs_panel[AUTOMAT]));

	DeleteCriticalSection( &_Lock);

Error:
	EXCRETHROW( pexception);
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
		pdata = _data_Analog + channel_index;

		if ( pScaleInfo && pScaleInfo->calType == CAL_TYPE_LINEAR )
		{
			SLinearCalibrationPtr plincal = (SLinearCalibrationPtr)pScaleInfo->pCalibData;
			double k = plincal->pPoints[0].realValue / (plincal->pPoints[0].measValue - plincal->pPoints[1].measValue);
			double q = (plincal->pPoints[1].realValue - plincal->pPoints[1].measValue * k);
			
			LinEv1D( pdata, 1, k, q,  pChannelData);
			*Value = pChannelData[0]; 
		}
		else 
			*Value = _data_Analog[channel_index]; 
Error:
	EXCRETHROW( pexception);
	
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "SelectProductType"
static SElExceptionPtr SelectProductType(struct _SSledge* me, int32_t*	pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			Index;
	double          rvalue, value_1, value_2;
	char            productType[128]="";
	int             number_boutton;

//	TEST_USE(DAQ);
	SCalibrationPtr	pScaleInfo = NULL;
	
	
    	if ( gs_DAQ )
				{

				checkErr( eldaq_GetChannelScale ( gs_DAQ, "ai3", "voltage", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale("ai3", pScaleInfo, &rvalue));
				value_1 = rvalue;

				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ, "ai4", "voltage", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale("ai4", pScaleInfo,&rvalue));
				value_2 = rvalue;
				
//#if 0    /* formerly excluded lines */
				if(value_1<3.35 && value_1>3.05 && value_2<3.35 && value_2>3.05)
					{
//#endif   /* formerly excluded lines */
					 number_boutton=7;
					 sprintf(productType, "X52_7");
//#if 0    /* formerly excluded lines */
					}
				else if(value_1<3.75 && value_1>3.40 && value_2<3.75 && value_2>3.40) 
				
					
					{
					  number_boutton=8;
					  sprintf(productType, "VS11");
					}
					
				else if(value_1<4.1 && value_1>3.80 && value_2<4.1 && value_2>3.80)
				    
					    
					  {
						number_boutton=9; 
						sprintf(productType, "X52_9");
					  }
					
				 else
						
					  {
						 number_boutton=0; 
						 sprintf(productType, "Invalid");
					   }

				
//#endif   /* formerly excluded lines */
					}
	//	 printf("productType = %s\n\r",productType);
	//	 printf("productType Database = %f\n\r",gs_Automat.pTestSeq->ProductType); 
	//	printf("Number_Boutton = %d\n\r",number_boutton);
	//	printf("Number Boutton DataBase = %d\n\r", gs_Automat.pTestSeq->number_boutton);
	
	if (strcmp(gs_Automat.pTestSeq->ProductType,productType))
	//	*product_valid=1;
	{	PANEL_MSG( PANEL_HELP2, _("Invalid Product Type/ Bridage mauvais"));
			if(pError) *pError = -1;
			CHECK_ERR( *pError); }
	
	
Error:
	EXCRETHROW( pexception);  
}

#undef __FUNC__
#define __FUNC__ "TestProductPresence"
static SElExceptionPtr TestProductPresence(int32_t* pError, int32_t* insert1, int32_t* insert2)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			Index;
	double          rvalue, value_1, value_2;

//	TEST_USE(DAQ);
	SCalibrationPtr	pScaleInfo = NULL;
	
	
    	if ( gs_DAQ )
				{
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ, "ai5", "voltage", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale("ai5", pScaleInfo, &rvalue));
				value_1 = rvalue;

				
				
				checkErr( eldaq_GetChannelScale ( gs_DAQ, "ai31", "voltage", (void *)&pScaleInfo));
				EXCCHECK( ReadAnalogWithScale("ai31", pScaleInfo,&rvalue));
				value_2 = rvalue;
				
				if(value_1<0.8 && value_1>0.67 && value_2<0.8 && value_2>0.67)
					{
					 *insert1=1;
					 *insert2=1;
					}
					
				 else
						
					  {
						*insert1=0;
					    *insert2=0; 
					  }
			//	 printf("Insert 1= %d: Insert 2=%d Inside function\n\r",insert1, insert2);
				
					}	
	
Error:
	EXCRETHROW( pexception);  
}

#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
	return NULL;
}

#endif 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelReleaseFinal"
static SElExceptionPtr JigPanelReleaseFinal(struct _SSledge* me, bool_t fault)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	int32_t				i = 0;
	bool_t 				insert1=0,insert2=0;
	SPrinterPtr			printer;
	int 				iStatus;
	clock_t 			_start_clock;
	double   		time;	
	int				StatusOnline;
	int				Interpreter;
	int				AmmountLabels;
	char			Error[128] = "";
	char			sAmmount[128] = "";
	char			msg[128] = "";

	if(fault)  
	{
		StatusStations[7] = TRUE;
		PANEL_STATION_STATUS(7,1);
			
			EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  don't display TEST NOK in station 8 */
			
			/* Loop to remove last master panel */
				do{
					sprintf (msg, "DEFAUT:'%s'", me->pTestParam->Name_Fault);
					PANEL_MSG( PANEL_HELP2, msg);
					EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
				}while (insert1 || insert2);	

			PANEL_MSG( PANEL_RESULT_MSG, _("TEST NOK"));
			PANEL_STATION_STATUS(7,3);
			PANEL_STATION_STATUS(0,3);
			ProcessSystemEvents ();
	}

	else 
	{
		StatusStations[7] = TRUE;
		
		EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  don't display TEST OK in station 8 */
		
		PANEL_MSG( PANEL_RESULT_MSG, _("TEST OK"));
		PANEL_STATION_STATUS(7,3);
		PANEL_STATION_STATUS(0,0);
		ProcessSystemEvents ();
	}

	/* next master part */
	Finished_master_parts++;
	Status [me->Actual_Master_Part] = fault;

	/* if Last Finished Master Part */
	if (Finished_master_parts == number_of_master_parts) //the tour is finished
		{
				
				//printf("Finished_master_parts=%d\r\n",Finished_master_parts);
				NOK_Master_Parts = 0;
			
				/* check if all masters parts are OK */	
				for (i=0; i<number_of_master_parts; i++)
				{
					if (Status [i] == TRUE )
					{
					sprintf (MastersBarcodes[NOK_Master_Parts], "%s", MastersBarcodes[i]);//shift register	
					sprintf (MastersDescription[NOK_Master_Parts], "%s", MastersDescription[i]);//shift register
					sprintf (MastersName[NOK_Master_Parts], "%s", MastersName[i]);//shift register
					MastersStations[NOK_Master_Parts] = MastersStations[i];//shift register	
					NOK_Master_Parts++;
					}
				}
		    
				/* if all master parts are OK : GO production */
		
				if (NOK_Master_Parts == 0)
				{
				/* Loop to remove last master panel */
				PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
				do{
					EXCCHECK(TestProductPresence(&error, &insert1, &insert2));
				}while (insert1 || insert2);
				
				/* reload production testsequence */
				gs_Automat.Mode = AUTOMAT_PRODUCTION;
			
				/* Disable master parts mode */
				gs_Automat.masterpart = FALSE;
				
				/* Table not empty */
				gs_Automat.isTableEmpty = FALSE;
				gs_Automat.EmtyTable = FALSE;
		
				PANEL_MSG( PANEL_HELP1, _("Production"));
				
				EXCCHECK( ProcessEventGuiProduction());
				
				if (gs_Automat.PRINTER_active && gs_Automat.PRINTER_MAN_active)
					EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", NULL));
				
				}
				else 
				{
					int ok = 0;
					/* Loop to remove last master panel */
					PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
					do{
						EXCCHECK(TestProductPresence(&error, &insert1, &insert2));

					}while (insert1 || insert2);
					
					/* Table not empty */
					gs_Automat.isTableEmpty = FALSE;
					gs_Automat.EmtyTable = FALSE;
					
					number_of_master_parts = NOK_Master_Parts; //new number of master parts to test	
					EXCCHECK( ProcessEventMasterSequenceRun (FALSE));
				}
		}
	
Error:
	EXCRETHROW( pexception);  
}
