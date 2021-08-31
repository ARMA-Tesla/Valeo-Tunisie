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
#include <laser/class/lasers.h>
#include <Camera/class/cameras.h>
#include <motor/class/motor.h>
#include "automat.h"
#include "automat_fnc.h"
#include "automat_plugin.h" 
#include <NIDAQmx.h>
#include <rs232.h>
#include <formatio.h>
#include "toolbox.h"
#include <eldaq/class/eldaq.h>
#include <IviBridge/IviBridge.h>  					
#include <local.h>
#include <NIDAQmx.h>


#define ROTATION 							1
#define _CHANGE_VERSION_NO_EMPTY			0 /* 1 no empty */


const char* HISTORY = \
"[2008-05-27] rT 1.10\n"
" * ProcessEventGuiProductSelected edited. No master part after pause is needed.\n"
"[2007-12-13] rT 1.10\n"
" * Function LoadTestSequence was edited. Reason was a application freezing.\n"
"[2007-12-15] rT 1.20\n"
" * Retest panel algorithm edited\n";


/* error handling */
#define EXCCHECKIVIDAQ( fCall) \
	EXCCHECKIVI(eldaq, gs_DAQ, fCall)

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
	EVNT_JIG_NEW_,
	EVNT_JIG_START_,
	/* event count */
	EVENT_HANDLE_NB
};


#define NB_OF_PANEL				1  
#define NUMBER_OF_STATIONS		8

#define OK						0 
#define NOK						1 
#define RUN						2 
#define FREE					3 



int						gs_panel[NB_OF_PANEL + 1];
static HANDLE			gs_EventHandle[EVENT_HANDLE_NB];
SElEventFactoryPtr		gs_EventFactory = NULL;  
static SObjListPtr 		gs_Modules = NULL; 
static SLabelListPtr	gs_pLabels = NULL;
static ViSession		gs_DAQ = 0;
static SDBSPtr			gs_pDbs = NULL;
static SBoxPtr			gs_pBox = NULL;
static STesterPtr		gs_pTester = NULL;
static SPrintersPtr		gs_pPrinters = NULL;
static SLasersPtr		gs_pLasers = NULL;                  
static SaxissPtr		gs_paxiss = NULL;
static SmotorsPtr		gs_pmotors = NULL;
static SCamerasPtr		gs_pcameras = NULL;
static SIviBridgePtr	gs_IviBridge = NULL;
static SAutomat			gs_Automat;
static HANDLE			gs_hThread = NULL;
static HANDLE			hEventStop = NULL;
SLanguagePtr			gs_pLanguage = NULL; 
int 					gs_Table_Rotation;
int32_t					Reset = FALSE;
static HANDLE gs_hEventStop = NULL;
int32_t					gs_RnR_Station;
int32_t					gs_RnR_Repeat;
char 					gs_RnR_Palett[10+1]; 
char 					gs_RnR_Piece[10+1];


typedef struct _SBarcodeData{
	/* input data */
	uint32_t			  barcode_ID;
	/* returned data */
	char				  barcode[256];
	
} SBarcodeData, *SBarcodeDataPtr;


	
#define PANEL_MSG(control, msg) \
	do{ \
		if(gs_Automat.pTestSeq) \
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

#define UNLOCK_DELAY					500
#define TIMEOUT							2000
#define COLDMARKING_TIMEOUT				6000
#define ROTATION_TIMEOUT				5000

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2


CRITICAL_SECTION   _Lock;

int 					gs_PalettID;

static SElExceptionPtr JigPanelLock(int32_t* pError);
static SElExceptionPtr JigPanelUnlock(int32_t* pError);
static SElExceptionPtr JigPanelInsert(int32_t* pError);
static SElExceptionPtr JigPanelInsert2(int32_t* pError);
static SElExceptionPtr JigPanelRelease(struct _SSledge* me, bool_t fault);

static void CVICALLBACK PostDeferredDisplayPanel (void *callbackData){ DisplayPanel(gs_panel[AUTOMAT]); }
static void CVICALLBACK PostDeferredHidePanel (void *callbackData){ HidePanel(gs_panel[AUTOMAT]); }

static DWORD WINAPI Thread_Sledge(LPVOID param);
static SElExceptionPtr Activate(struct _SSledge* me);
static SElExceptionPtr Deactivate(struct _SSledge* me);
void CyclePrintStatus(int Time, int Step); 

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

#undef __FUNC__
#define __FUNC__	"LogSerialNumber"
void LogSerialNumber(int retest, char *Message_Erreur, char *char1, char *Char2)   
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
	sprintf(result_dir, "%s\\Serial\\%04d.%02d.%02d", project_dir, st.wYear, st.wMonth, st.wDay);
	CheckDirectory(result_dir); 

	
	/* enreg des data */
	
	memset(log, 0x00, 256);
	
	sprintf (fileName, "%s/Log_serial.txt", result_dir);
	
	HandleFile = OpenFile (fileName, VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	
//	sprintf (log, "18: 30 -- hello && it is me");
//	sprintf (log, sdate_time, DATE_TIME_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf (log,"---------------------------------------\r\n%02d.%02d.%02d %02d.%02d.%02d\r\nRETEST=%d\r\nMessage_ERREUR=%s\r\nChar1=%s\r\nChar2=%s\r\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,retest,Message_Erreur,char1,Char2);  
//	sprintf (log, "IS_BARCODE_REAR=%d\r\n", is_barcode_rear); 
//	sprintf (log, "SENSOR_CYLINDER_X_REAR_STATION_80=", is_cylinder_x_rear); 
	
	
	WriteLine (HandleFile, log, sizeof (log));	
	
	CloseFile (HandleFile);
}


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::GetPalettID"
static SElExceptionPtr GetPalettID(int *palettID)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int 			HandleFile;
	int 			sensor;
	int 			count;
	char            buffer[2+1];
	char            data[2+1];
	
	
	gs_pTester->GetSensor(gs_pTester, "SENSOR_PALLET_1_STATION_10", &sensor);   
//	printf("sensor_value= %d\n\r", sensor);

	if (sensor == 1)
	{
		
//		printf("IN SENSOR =1\n\r");
	HandleFile = OpenFile ("Pallet.dat", VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
	count = 6;
	WriteLine (HandleFile, "6", -1);
	CloseFile  (HandleFile);
	}
	else 
	{
		
//		printf("IN ELSE\n\r");
	HandleFile = OpenFile ("Pallet.dat", VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_ASCII);
	ReadLine (HandleFile, buffer, -1);
	buffer[1] = 0; 
//	printf ("buffer %s\r\n", buffer);
	count =  atoi(buffer);
//	printf ("count %d\r\n", count); 
	
	if(count==8)
		count=1;
	else
	  {count = count + 1;}
//	printf ("count %d\r\n", count); 
	CloseFile  (HandleFile);

	HandleFile = OpenFile ("Pallet.dat", VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII);
	sprintf (data, "%d", count);
	data[1] = 0; 
//	printf ("data %s\r\n", data);
	WriteLine (HandleFile, data, -1);
	CloseFile  (HandleFile);
	}
	
	*palettID = count;

Error:
	CloseFile  (HandleFile);
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ExtractDoubleBarcode"
static SElExceptionPtr ExtractDoubleBarcode(char *Buffer, char *Barcode1)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
    int i, Index1, Index2;
	
	Index1 = strlen(Buffer);

	for(i=0; i<Index1;i++)
	{
		if (Buffer[i] == 0x0D)
			break;
		else 
			Barcode1[i] = Buffer[i];
	}
	
	Barcode1[i] = 0x00;

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
#undef __FUNC__
#define __FUNC__ "LoadJobCamera"
SElExceptionPtr LoadJobCamera (char *job)
{
SElExceptionPtr	pexception = NULL;
int32_t			error = 0;
SCameraPtr		pcamera = NULL;
char     		data[256];
HANDLE			hThread = NULL;
	
	
	EXCCHECK( gs_pcameras->GetCameraByIndex(gs_pcameras, 0, &pcamera));
	EXCCHECK( pcamera->Cognex_disconnect(pcamera));
	EXCCHECK( pcamera->openTCPserver(pcamera));
	EXCCHECK( pcamera->Cognex_login(pcamera));
	EXCCHECK( pcamera->Cognex_Offline(pcamera));
	EXCCHECK( pcamera->Cognex_Job(pcamera, job));
	EXCCHECK( pcamera->Cognex_Online(pcamera));


Error:
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ResetStations"
static SElExceptionPtr ResetStations(void)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int32_t			is_Error = 0;
	int             i;

//	EXCCHECK( LoadJobCamera (gs_Automat.pTestSeq->JobCamera));
	if (gs_pTester)
	{
	pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 1200);

	
	for (i=0; i<8; i++)
	EXCCHECK( gs_pTester->InitializeStations(gs_pTester, i, 0));

	for (i=0; i<8; i++)
	EXCCHECK( gs_pTester->InitializeStations(gs_pTester, i, 5000));
	}
	
Error:
	EXCRETHROW( pexception);  
}



#undef __FUNC__
#define __FUNC__ "CheckAxisPosition0"
SElExceptionPtr CheckAxisPosition0( SaxisPtr paxis, int station, int axis, double timeout)
{
	SElExceptionPtr pexception = NULL;
	double Initial_Position;
	double Currentposition;
	int   status;
	clock_t _start_test_clock;
	double  time;
	int     statusAlarm;
   	char	Error[256] = "";
	char	sStation[256] = "";
	
	
	paxis->CurrentPosition(paxis, axis, &Currentposition);
	
	_start_test_clock = clock();
	
	do{
		if (Currentposition >= 99.0)
		{
		paxis->SetPosition(paxis, axis, 0.01, 50.0);
		Sleep (100);
		}
			  
		paxis->CurrentPosition(paxis, axis, &Currentposition);

		 if (Currentposition < +0.2 && Currentposition > - 0.2)
			break;
	
		else 
			paxis->CurrentPosition(paxis, axis, &Currentposition);
	
     time = (double)(clock()-_start_test_clock);

	}while(time < timeout);
	
	if (time>=timeout)
	{
	paxis->CurrentPosition(paxis, axis, &Currentposition);
	paxis->PresentAlarm(paxis, axis, &statusAlarm);
	if (station == 0)
		sprintf(sStation, "STATION 30 RIGHT");
  	else if (station == 1)
		sprintf(sStation, "STATION 30 LEFT");
	else if (station == 2)
		sprintf(sStation, "STATION 40 RIGHT");
	else if (station == 3)
		sprintf(sStation, "STATION 40 LEFT");
			
	if (axis == AXIS_X)
	sprintf(Error, "%s AXE X : Alarm Code : (0X%04X)h  Position  = %fmm Target = 0.0mm", sStation, statusAlarm, Currentposition);
	else if (axis == AXIS_Y)
	sprintf(Error, "%s AXE Y : Alarm Code : (0X%04X)h  Position  = %fmm Target = 0.0mm", sStation, statusAlarm, Currentposition);
	else if (axis == AXIS_Z)
	sprintf(Error, "%s AXE Z : Alarm Code : (0X%04X)h  Position  = %fmm Target = 0.0mm", sStation, statusAlarm, Currentposition);
	
	EXCTHROW(-1, Error);
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
	SaxisPtr		paxis[10];
	SmotorPtr		pmotor[4];
	int             i;
	int32_t			insert1, insert2;
	int 	status;
	int status_micro_cam, barrier;
    int *servo = 0;	
	
	if (Reset == FALSE)
	{
	Reset = TRUE;

	PANEL_MSG( PANEL_HELP1, _("Reset Tester..."));
	PANEL_MSG( PANEL_HELP2, _(""));
//	EXCCHECK( LoadJobCamera (gs_Automat.pTestSeq->JobCamera));
	ProcessSystemEvents ();
	ProcessDrawEvents ();

	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &status_micro_cam));
	if (status_micro_cam == 0)
	{
		do{
			gs_pTester->GetSensor(gs_pTester, "INFRARED_BARRIER", &barrier);
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			ProcessSystemEvents ();
			ProcessDrawEvents ();
		}while(!barrier);
		
	gs_pTester->UnlockPanelStation10(gs_pTester, 1200);
	gs_pTester->TableRotate(gs_pTester, 5000,TRUE);
	}
	EXCCHECK( gs_pTester->Disactivate_Axis(gs_pTester));
//	printf("Axis Disactivated\r\n");
	Sleep(1000);
	EXCCHECK( gs_pTester->Activate_Axis(gs_pTester));
//	printf("Axis Activated\r\n");
	Sleep(1000);
	for (i=0; i<8; i++)
		EXCCHECK( gs_pTester->InitializeStations(gs_pTester, i, 0));
	
	/* init axis x y*/
	for (i=0; i<4; i++)
		EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, i, &paxis[i]));
	
	/* init axis SMAC */
	for (i=4; i<6; i++)
		EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, i, &paxis[i]));
	/* init axis Z */
	for (i=6; i<10; i++)
		EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, i, &paxis[i]));
	
	for (i=6; i<10; i++)
	{
	EXCCHECK( paxis[i]->AlarmReset(paxis[i], AXIS_Z));
	EXCCHECK( paxis[i]->EnableAxis(paxis[i], AXIS_Z));
	EXCCHECK( paxis[i]->ControllerStatus(paxis[i], AXIS_Z, 1));
	}
	Delay (1);
	
	//TEST_USE(AXIS);
	//INITIALIZE_AXIS (4);
	EXCCHECK( paxis[5]->EnableAxis(paxis[5], 32));
	//Sleep(200);
	EXCCHECK( paxis[4]->EnableAxis(paxis[4], 1));
	
	for (i=6; i<10; i++)
		EXCCHECK( paxis[i]->HomeReturn(paxis[i], AXIS_Z));

	Delay (4);

	for (i=6; i<10; i++)   //0 = Z ST30 RIGHT   1 = Z ST30 LEFT 
		EXCCHECK( CheckAxisPosition0(paxis[i], i, AXIS_Z, 5000));

	
	/* autres verif capteur inductif des axes Z*/
	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "SENSOR_Z_HOME_STATION_30_RIGHT", &status));
	if (status != 1)
		EXCTHROW(-1, "SENSOR_Z_HOME_STATION_30_RIGHT pas en position 0");
	
	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "SENSOR_Z_HOME_STATION_30_LEFT", &status));
	if (status != 1)
		EXCTHROW(-1, "SENSOR_Z_HOME_STATION_30_LEFT pas en position 0");
	
	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "SENSOR_Z_HOME_STATION_40_RIGHT", &status));
	if (status != 1)
		EXCTHROW(-1, "SENSOR_Z_HOME_STATION_40_RIGHT pas en position 0");
	
	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "SENSOR_Z_HOME_STATION_40_LEFT", &status));
	if (status != 1)
		EXCTHROW(-1, "SENSOR_Z_HOME_STATION_40_LEFT pas en position 0");

	
	for (i=0; i<4; i++)  
	{
	EXCCHECK( paxis[i]->AlarmReset(paxis[i], AXIS_X));
	EXCCHECK( paxis[i]->EnableAxis(paxis[i], AXIS_X));
	EXCCHECK( paxis[i]->ControllerStatus(paxis[i], AXIS_X, 1));
	}

	for (i=0; i<4; i++)
	{
	EXCCHECK( paxis[i]->AlarmReset(paxis[i], AXIS_Y));
	EXCCHECK( paxis[i]->EnableAxis(paxis[i], AXIS_Y));
	EXCCHECK( paxis[i]->ControllerStatus(paxis[i], AXIS_Y, 1));
	}
	

	for (i=0; i<4; i++)
	{	
#if 0    /* formerly excluded lines */
		paxis[i]->HomeReturn(paxis[i], AXIS_X);
	     Sleep(1000);
	     paxis[i]->SetPosition(paxis[i], AXIS_X,1,100);
	     paxis[i]->SetPosition(paxis[i], AXIS_X,1,100);
#endif   /* formerly excluded lines */
		 EXCCHECK (paxis[i]->HomeReturn(paxis[i], AXIS_X));
	
	}

	for (i=0; i<4; i++)
	{	
#if 0    /* formerly excluded lines */
		paxis[i]->HomeReturn(paxis[i], AXIS_Y);
		Sleep(1000);
	    paxis[i]->SetPosition(paxis[i], AXIS_Y,1,100);
	    paxis[i]->SetPosition(paxis[i], AXIS_Y,1,100);
#endif   /* formerly excluded lines */
		EXCCHECK( paxis[i]->HomeReturn(paxis[i], AXIS_Y));   
	}
	
	Delay (5);

	for (i=0; i<4; i++)
	EXCCHECK( CheckAxisPosition0(paxis[i], i, AXIS_X, 3000));
	for (i=0; i<4; i++)
	EXCCHECK( CheckAxisPosition0(paxis[i], i, AXIS_Y, 3000));
	
	
	//reset sensor motor wipe
	for (i=0; i<2; i++)
		EXCCHECK( gs_pmotors->GetmotorByIndex(gs_pmotors, i, &pmotor[i])); 

	//pmotor[1]->ControllerStatus(pmotor[1], 1, &status);
	//pmotor[3]->ControllerStatus(pmotor[3], 3, &status);
	
	/* Unlock Pallett in the first time */
	gs_pTester->UnlockPanelStation10(gs_pTester, 1200);
	PANEL_MSG( PANEL_HELP2, _(""));

	
	for (i=0; i<8; i++)
	EXCCHECK( gs_pTester->InitializeStations(gs_pTester, i, 5000));
	
	
	if(gs_Automat.shift_id==0)
		{
		PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
		PANEL_MSG( PANEL_HELP2, "");
		}
	
	else if(gs_Automat.masterpart)
		{
		PANEL_MSG( PANEL_HELP2, _("Insert Master Panel"));
		}
	
	else 
	{

		EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
		EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));  
	
			if (gs_Automat.EmtyTable == TRUE) //mode vidage 
				{
				if (gs_Automat.ChangeVersion == FALSE) //End Version
						PANEL_MSG( PANEL_HELP1, _("End Of Actual Version"));

				else if (gs_Automat.ChangeVersion == TRUE) //Change Version
						PANEL_MSG( PANEL_HELP1, _("Change Version")); 

				if (insert1 || insert2)
					PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
				
				else if (!insert1 && !insert2)
					PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
				}
		
			else 	  //mode production normal
				{
					
				PANEL_MSG( PANEL_HELP1, _("Production"));
		
				PANEL_SET_REFERENCES(gs_Automat.pTestSeq->ProductPartNumber, 
							 gs_Automat.pTestSeq->ProductCustomerRef);
		
				if (!insert1 || !insert2)
				{	PANEL_MSG( PANEL_HELP2, _("Insert Panel"));  
				   //printf("Insert panel in ProcessEventGuiResetProduct\n\r"); 
				}
				else if (insert1 && insert2)
				{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle")); 
				//	printf("Push Start Cycle in ProcessEventGuiResetProduct\n\r"); 
				}
				}
	}
		
	ProcessSystemEvents ();
	ProcessDrawEvents ();
	
	Reset = FALSE;
	}

Error:
	Reset = FALSE;
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
	SCameraPtr		pcamera = NULL;
	char     		data[256];
	
	
	if(gs_EventFactory && set_panel)
	{
		/* set panel to default */
		EXCCHECK( eleventdata_new(&pdata)); 
		EXCCHECK( pdata->SetStringPtr(pdata, "panel;2"));
		EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_GUI_PANEL", pdata));
	}

	/* set mode */
	gs_Automat.Mode = AUTOMAT_PRODUCTION;

	EXCCHECKCVI( GetProjectDir(test_form_xml));
	strcat(test_form_xml, PATH_TEST_FILE_FORM_XML);

	EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, TRUE, &is_Error)); 
	CHECK_ERR( is_Error);
	
	EXCCHECK( LoadJobCamera (gs_Automat.pTestSeq->JobCamera));
	
	gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;

	if(gs_Automat.box_active && set_box)
		gs_Automat.box = TRUE;

	if(gs_Automat.master_active && set_master)
		gs_Automat.masterpart = TRUE;

	if(gs_Automat.shift_id==0)
	{
		PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
		PANEL_MSG( PANEL_HELP2, "");
	}
	else
	{
		PANEL_MSG( PANEL_HELP1, _("Production"));
		PANEL_SET_REFERENCES(gs_Automat.pTestSeq->ProductPartNumber, 
							 gs_Automat.pTestSeq->ProductCustomerRef);

		if( gs_Automat.box)
		{
			PANEL_MSG( PANEL_HELP2, _("Insert Empty Box"));	
			INSERT_EMPTY_BOX();
		}

/* Laser + RED BOX */
#if 0    /* formerly excluded lines */
		else if(gs_Automat.Mode = AUTOMAT_PRODUCTION)
		{
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			if (full)
				{
				PANEL_MSG( PANEL_HELP2, _("Empty Red Box"));
				}
			else if (!closed)
				{
				PANEL_MSG( PANEL_HELP2, _("Close Red Box"));
				}
			else if (erreur)
				{
				PANEL_MSG( PANEL_HELP2, _("Reset Printer Controller"));
				}
		}
#endif   /* formerly excluded lines */
	
		
		else if( gs_Automat.masterpart)
			PANEL_MSG( PANEL_HELP2, _("Insert Master Panel"));
		else
		{	PANEL_MSG( PANEL_HELP2, _("Insert Panel")); 
		//printf("Insert panel in ProcessEventGuiProductSelected\n\r");	
		}
	}

			
	/*Unlock Palett STATION 10 */
	gs_pTester->UnlockPanelStation10(gs_pTester, 1200);

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
	int32_t			insert1, insert2;
	SCameraPtr		pcamera = NULL;

	if (gs_Automat.EmtyTable == FALSE)
	{
	/* set mode */
	gs_Automat.Mode = AUTOMAT_PRODUCTION;
	
	/* load testsequence for production */
	if(gs_Automat.TestSeqMode!=TESTSEQ_PRODUCTION)
	{
		EXCCHECKCVI( GetProjectDir(test_form_xml));
		strcat(test_form_xml, PATH_TEST_FILE_FORM_XML); 
	
		EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, TRUE, &is_Error));
		CHECK_ERR( is_Error);
		
		gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;

		EXCCHECK( LoadJobCamera (gs_Automat.pTestSeq->JobCamera));
	
		PANEL_SET_REFERENCES(gs_Automat.pTestSeq->ProductPartNumber, 
							 gs_Automat.pTestSeq->ProductCustomerRef);
	}

	if(gs_Automat.shift_id==0)
	{
		PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
		PANEL_MSG( PANEL_HELP2, "");
	}
	else
	{
		PANEL_MSG( PANEL_HELP1, _("Production"));

		PANEL_SET_REFERENCES(gs_Automat.pTestSeq->ProductPartNumber, 
							 gs_Automat.pTestSeq->ProductCustomerRef);
				
		if(gs_Automat.box)
		{
			PANEL_MSG( PANEL_HELP2, _("Insert Empty Box"));
			INSERT_EMPTY_BOX();
		}
/* Laser + RED BOX */
#if 0    /* formerly excluded lines */
		else if(gs_Automat.Mode = AUTOMAT_PRODUCTION)
		{
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			if (full)
				{
				PANEL_MSG( PANEL_HELP2, _("Empty Red Box"));
				}
			else if (!closed)
				{
				PANEL_MSG( PANEL_HELP2, _("Close Red Box"));
				}
			else if (erreur)
				{
				PANEL_MSG( PANEL_HELP2, _("Reset Printer Controller"));
				}
		}
#endif   /* formerly excluded lines */
		else if(gs_Automat.masterpart)
		{
			PANEL_MSG( PANEL_HELP2, _("Insert Master Panel"));
		}
		
		else
		{
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
				if (!insert1 || !insert2)
				{	PANEL_MSG( PANEL_HELP2, _("Insert Panel")); 
				   	//printf("Insert panel in ProcessEventGuiProduction\n\r");
				}
				else if (insert1 && insert2)
				{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));   
					//printf("Push Start Cycle in ProcessEventGuiProduction\n\r");
				}
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
	PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
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
   	int32_t			insert1, insert2;

	/* set mode */
	gs_Automat.Mode = AUTOMAT_QUALITY_RETEST;
	
	if(gs_Automat.shift_id==0)
		{
			PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
			PANEL_MSG( PANEL_HELP2, "");
		}
	else 
	{
	PANEL_MSG( PANEL_HELP1, _("Quality Retest"));
	
	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
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
			{	PANEL_MSG( PANEL_HELP2, _("Insert Panel"));   
				//printf("Insert panel in ProcessEventGuiQualityRetest\n\r");  
			}
			else if (insert1 && insert2)
			{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));   
			//	printf("Push Start Cycle in ProcessEventGuiQualityRetest\n\r"); 
			}
		}
	}

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
#define __FUNC__ "{Automat Plugin}::ProcessEventTestSequenceRun"
static SElExceptionPtr ProcessEventTestSequenceRun(struct _SSledge* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char			form_xml[MAX_PATHNAME_LEN];        

	if(gs_Automat.TestSeqMode==TESTSEQ_PRODUCTION)
	{
		EXCCHECKCVI( GetProjectDir(form_xml));
		strcat(form_xml, PATH_TEST_FILE_FORM_XML); 
		
		EXCCHECK( RunTestSequence(form_xml, me, &gs_Automat, gs_EventFactory)); 
	}
	else if(gs_Automat.TestSeqMode==TESTSEQ_EXPERTISE)
	{
		EXCCHECKCVI( GetProjectDir(form_xml));
		strcat(form_xml, PATH_TESTEXP_FILE_FORM_XML); 
	
		EXCCHECK( RunTestSequence(form_xml, me, &gs_Automat, gs_EventFactory)); 
	}
	else if(gs_Automat.TestSeqMode==TESTSEQ_MASTERPART)
	{
		EXCCHECKCVI( GetProjectDir(form_xml));
		strcat(form_xml, PATH_TESTMASTER_FILE_FORM_XML); 
	
		EXCCHECK( RunTestSequence(form_xml, me, &gs_Automat, gs_EventFactory)); 
	}
				
Error:
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventJigNew"
static SElExceptionPtr ProcessEventJigNew()
{
SElExceptionPtr	pexception = NULL;
int32_t			error = 0;
bool_t			barrier = FALSE;

JigPanelUnlock(&error);
CHECK_ERR( error);

Error:
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::ProcessEventJigItemCycleStarted"
static SElExceptionPtr ProcessEventJigItemCycleStarted(struct _SSledge* me)
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
    int32_t 		status_micro_cam;

	if( !gs_Automat.disabled)
	{
		
		if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.shift_id==0)
		{
			PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
			PANEL_MSG( PANEL_HELP2, "");
			CHECK_ERR( -1);
	//		printf("Slect shift");
		}

		sprintf(me->barcode, "no_barcode");
	//	 printf("wait on lock");
		/* Lock Panel */
		JigPanelLock(&error);
	//	printf("lock jig started");
		CHECK_ERR( error);
		
#if ROTATION
		
		//Sleep(1000); /* switch barrier status */
		do{
			gs_pTester->GetSensor(gs_pTester, "INFRARED_BARRIER", &barrier);
			DisplayWait();
			ProcessSystemEvents ();
			ProcessDrawEvents ();
		}while((!StatusStations[1] || !StatusStations[2] || !StatusStations[3] ||
			    !StatusStations[4] || !StatusStations[5] || !StatusStations[6] || !StatusStations[7]) 
				&& barrier);
		
		
		if (!barrier)  //re push Start Cycle Buttom
		{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			goto Error; 
		}

		/* Rotate_Table */
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
		
		//Sleep (1000);
		StatusStations[0] = TRUE;
		gs_pTester->UnlockPanelStation10(gs_pTester, 2000);
		
		/* check presence product */
		EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
		EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
		if (!insert1 || !insert2)
		{
			/* Unlock palett N°1 */
			gs_pTester->UnlockPanelStation10(gs_pTester, 2000);
			PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
				//printf("Insert Panel in ProcessEventJigItemCycleStarted\n\r");   
		}
		else if (insert1 && insert2)
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
	//	printf("Push Start Cycle in ProcessEventJigItemCycleStarted\n\r");

		/* Table not empty */
		gs_Automat.isTableEmpty = FALSE;
		gs_Automat.EmtyTable = FALSE;
		
		/*parameters RnR */
		me->Palett_RnR = atoi(gs_RnR_Palett);	
		me->Part_RnR = atoi(gs_RnR_Piece);	
		me->Station_RnR = gs_RnR_Station;
		me->Repeat_RnR = gs_RnR_Repeat;
		
		if ( gs_PalettID == 1)
			sprintf (me->Pallet, "P8"); 
		else 
			sprintf (me->Pallet, "P%d", gs_PalettID - 1);
				
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
				
		else if( gs_Automat.Mode==AUTOMAT_MASTERPART && pexception==NULL)
		{
			char test_form_xml[MAX_PATHNAME_LEN];

			/* reload production testsequence */
			gs_Automat.Mode = AUTOMAT_PRODUCTION;

			EXCCHECKCVI( GetProjectDir(test_form_xml));
			strcat(test_form_xml, PATH_TEST_FILE_FORM_XML); 

			/* load testsequence */
			EXCCHECK( LoadTestSequence(test_form_xml, gs_Automat.pTestSeq, gs_Modules, FALSE, &error)); 
			CHECK_ERR( error);

			gs_Automat.TestSeqMode = TESTSEQ_PRODUCTION;
			gs_Automat.masterpart = FALSE;
		}
	
		if( (gs_Automat.Mode==AUTOMAT_PRODUCTION || gs_Automat.Mode==AUTOMAT_QUALITY_RETEST || AUTOMAT_EXPERTISE_GO_MODE) && gs_Automat.Mode!=AUTOMAT_EXPERTISE_STEP_MODE)
		{
		pexception =  JigPanelRelease(me, failed);
			if(pexception==NULL)
			{
			goto Error2; 
			}
		}

		if( gs_Automat.Mode==AUTOMAT_MASTERPART)
		{
			gs_Automat.Mode = AUTOMAT_PRODUCTION;
		}
	}

	
Error:
	EXCCHECK( gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &status_micro_cam));
	if(gs_Automat.shift_id && status_micro_cam)
	{
	gs_pTester->UnlockPanelStation10(gs_pTester, 1200);
	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
	}
	StatusStations[0] = TRUE;
	EXCRETHROW( pexception);
	
Error2:
	StatusStations[7] = TRUE;
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
	//	printf("lock released");  
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
		pexception = JigPanelInsert2(&error);
		
		gs_pTester->GetSensor(gs_pTester, "MICRO_CAM", &MicroCam);
		gs_pTester->GetSensor(gs_pTester, "INFRARED_BARRIER", &barrier);
		
		if (!MicroCam)  // table pas dans position pas error => franchi barriere par operateur
			{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  being entered to station 20 */
			}
		else if (MicroCam && !barrier)  // table pas encore bougé => franchi barriere par operateur
			{
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			EXCTHROW(-1, "rotation Fault"); 			
			}
		//Sleep (500);
		StatusStations[0] = TRUE;
		
		
		/* Check if product in pallett */
		EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
		EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
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
	else if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.masterpart)
		PANEL_MSG( PANEL_HELP2, _("Insert Master Panel"));

	else 
		{
			if (gs_Automat.EmtyTable == TRUE)
				{
				PANEL_STATION_STATUS(0,FREE);
				PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
				 	//printf("Push Start Cycle in ProcessEventJigItemReleased\n\r");
				}
				else
				{
				PANEL_STATION_STATUS(0,FREE);
				if( !gs_Automat.EmtyTable)
					{
					PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
				//	printf("Insert Panel in ProcessEventJigItemReleased\n\r");  
					}
				}
		}

	#if 0    /* formerly excluded lines */
	else
	{
		/* Laser + RED BOX */
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			if (full)
				{
				PANEL_MSG( PANEL_HELP2, _("Empty Red Box"));
				}
			else if (!closed)
				{
				PANEL_MSG( PANEL_HELP2, _("Close Red Box"));
				}
			else if (erreur && StatusStations[7])
				{
				PANEL_MSG( PANEL_HELP2, _("Reset Printer Controller"));
				}
		}
	#endif   /* formerly excluded lines */

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
	else if( gs_Automat.Mode==AUTOMAT_PRODUCTION && gs_Automat.masterpart)
		PANEL_MSG( PANEL_HELP2, _("Insert Master Panel"));
	
	else 
		{  	
		PANEL_STATION_STATUS(0,RUN);
		if (gs_Automat.EmtyTable == TRUE)
			PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
		else
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
		}
   
#if 0    /* formerly excluded lines */

	else
	{
		/* Laser + RED BOX */
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			if (full)
				{
				PANEL_MSG( PANEL_HELP2, _("Empty Red Box"));
				}
			else if (!closed)
				{
				PANEL_MSG( PANEL_HELP2, _("Close Red Box"));
				}
			else if (erreur && StatusStations[7])
				{
				PANEL_MSG( PANEL_HELP2, _("Reset Printer Controller"));
				}
	}
#endif   /* formerly excluded lines */


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
	int32_t			error = 0;
	int32_t			status;

	/* start thread */
	EXCCHECK( ThreadStart(Thread_Sledge, me, &me->_hThread, NULL));  

	
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

	 pexception  = ProcessEventJigItemEmpty();
	 if (pexception == NULL && !gs_Automat.disabled)
		{
		gs_Table_Rotation++;
			if (gs_Table_Rotation == NUMBER_OF_STATIONS)
				{
				PANEL_STATION_STATUS(7,FREE);
				PANEL_STATION_STATUS(0,FREE);
				gs_Automat.isTableEmpty = TRUE;
				gs_Automat.EmtyTable = FALSE;
				/* Unlock Pallett in the first time */
				gs_pTester->UnlockPanelStation10(gs_pTester, 1500);
				PANEL_MSG( PANEL_HELP2, _(""));
				/*Start Normal production Mode */
				EXCDISPLAY( ProcessEventGuiProduction());
				}
			else 
				PANEL_STATION_STATUS(gs_Table_Rotation,FREE);
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
				PANEL_STATION_STATUS(7,FREE);
				PANEL_STATION_STATUS(0,FREE);
				EXCDISPLAY( DisplayVersionList());
				PANEL_MSG( PANEL_RESULT_MSG, _(""));
				gs_Automat.isTableEmpty = TRUE;
				gs_Automat.EmtyTable = FALSE;
			}
	
		else 
			PANEL_STATION_STATUS(gs_Table_Rotation,FREE);
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
	int32_t			insert1, insert2;

			/* Check if product in pallett */
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
			if (!insert1 && !insert2)
			{	
				if (gs_Automat.ChangeVersion == FALSE) //End Version
					EXCDISPLAY( ProcessEndVerion());
				else if (gs_Automat.ChangeVersion == TRUE) //Change Version  
			   		EXCDISPLAY( ProcessChangeVerion());
			}
			else
			{
				StatusStations[0] = TRUE;
				PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
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
	int32_t			error = 0;
	DWORD 			result;
	bool_t			b_loop_stop = FALSE;
	char			barcode[512];
	bool_t			is_locked = FALSE;
	SSledgePtr		pSledge = NULL;
	int             i;
	
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
				for(i=0; i< NB_OF_STATIONS ;i++)
				StatusStations[i] = 1;
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
				if (StatusStations[0] == TRUE && gs_Automat.EmtyTable == TRUE)
				{
				StatusStations[0] = FALSE;
				EXCDISPLAY( ProcessEmptyTable());
				}
				break;
			case WAIT_OBJECT_0 + EVNT_JIG_CYCLE_START_:
			//	printf(" Status Stations =%d\n\r",StatusStations[0]);
			//		printf("gs_Automat.EmtyTable =%d\n\r",gs_Automat.EmtyTable);
				if (StatusStations[0] == TRUE && gs_Automat.EmtyTable == FALSE)
				{
				StatusStations[0] = FALSE;
				EXCCHECK( sledge_new(&pSledge)); //creation de nouvelle structure pSledge
				EXCCHECK( pSledge->Activate(pSledge)); //lancement d'un nouveau thread de pSledge 
				}
				break;
			case WAIT_OBJECT_0 + EVNT_STOP_THREAD:
				{
				MessagePopup ("THREAD", "EVNT_STOP_THREAD");
				b_loop_stop = TRUE;
				}
				break;
			case WAIT_TIMEOUT:
				break;
			case WAIT_FAILED:
				EXCTHROW(-2, "Thread_Automat Fault");
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

/*----------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_Barcode"
DWORD WINAPI Thread_Barcode(LPVOID param)
{
	SElExceptionPtr			 	pexception = NULL;
	int32_t						error = 0;
   	SBarcodeDataPtr	pthread_data = (SBarcodeDataPtr)param;
	uint32_t	barcode_ID = pthread_data->barcode_ID;
	char  		barcode[256]="";
	
	
	sprintf(pthread_data->barcode, "none");
	
	printf("barcode=%s\r\n",barcode);
	printf("barcode_ID=%s\r\n",barcode_ID); 
	printf("pthread_data->barcode_ID=%s\r\n",pthread_data->barcode_ID);
	
	gs_pTester->GetItemCode(gs_pTester, "", barcode_ID, BARCODE_BUFFER_SIZE, barcode, 2000);
	
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

		
	EXCDISPLAY( ProcessEventJigItemCycleStarted(pSledge)); 
//	printf("ProcessEventJigItemCycleStarted\r\n");
	/* free pointer pSledge */
	EXCCHECK( sledge_delete(&pSledge));
	
	/* terminate thread */
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



#undef __FUNC__
#define __FUNC__ "JigPanelLabel"
SElExceptionPtr JigPanelLabel(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLaserPtr	    plaser = NULL;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[1024]= "";
	char            barcode[512]= "";
	char            barcode_Left[512]= "";
	char            barcode_Right[512]= "";
	
	char            barcode_file[512]= "";
	char            barcode_Left_file[512]= "";
	char            barcode_Right_file[512]= "";
	char            Reference_VALEO[512]= "";
	char 			change_release [512]="";
	char 			BMW_PN[512]="";
	char			HW_SW [512]="";
	char			date_file[512]="";
	char            String_in_laser[512]="";
	
	char 			ch1[5]="";
	char 			ch2[5]="";
	char 			ch3[5]="";

	int 			FileHandle = 0;
	int             index;
	int				Ready;
	int             i=0;
	clock_t 		_start_test_clock;
	uint32_t		timeout=6000;
	double          time;
//	SBarcodeDataPtr	pthread_data[2];
  // 	HANDLE  		hThreadBarcode[2];
	int             ErrorStatus;
	char            ErrorCode [512];
	int lasering_barcode_file_done=0;
	int lasering_barcode_Right_file_done=0;
	int lasering_Reference_VALEO_done=0;
	int lasering_change_release_done=0;
	int lasering_BMW_PN_done=0;
	int lasering_HW_SW_done=0;
	int lasering_date_file_done=0;
	int String_OK=0;
	int retest=0;
	int Erreur_laserage=0;
	
	
	PANEL_STATION_STATUS(7,RUN);

	if(gs_Automat.Mode==AUTOMAT_PRODUCTION)
	{
		
		StatusStations[7] = FALSE;
		
    
		
	
		EXCCHECK( gs_pLasers->GetLaserByIndex(gs_pLasers, 0, &plaser));
				
	
		/* 1- create new Laser keyence File */
		sprintf(Buffer, "%s;%s", me->pTestParam->Date, me->pTestParam->Production_Serial);
		EXCCHECK( me->pData->SetStringPtr(me->pData, Buffer));
		EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", me->pData));
		
		/* 2- wait update file */
		Sleep(1000);
		
		/* 2- Barcodes are in rear position */
		gs_pTester->MoveBarcodeRear(gs_pTester, 10000);
		
		
		/* 4- Read Barcodes have to be Lasered from file LASIT */
	 	FileHandle = OpenFile ("C:\\keyence\\MFL_Laser_data.txt", VAL_READ_ONLY, VAL_TRUNCATE, VAL_ASCII); /* open File Laser Stream*/
		
		/* - Laser Line 1 */
		ReadLine (FileHandle, barcode_file, -1); 	/* Read Line 1 */
		/* - Laser Line 2 */
		ReadLine (FileHandle, barcode_Left_file, -1); 	/* Read Line 2 */
		/* - Laser Line 3 */
		ReadLine (FileHandle, barcode_Right_file, -1); 	/* Read Line 3 */
		/* - Laser Line 4 */
		ReadLine (FileHandle, Reference_VALEO, -1); 	/* Read Line 4 */
		/* - Laser Line 5 */
		ReadLine (FileHandle, change_release, -1); 	/* Read Line 5 */
		/* - Laser Line 6 */ 
		ReadLine (FileHandle, BMW_PN, -1); 	/* Read Line 6 */
		/* - Laser Line 7 */ 
		ReadLine (FileHandle, HW_SW, -1); 	/* Read Line 7 */
		/* - Laser Line 8 */ 
		ReadLine (FileHandle, date_file, -1); 	/* Read Line 8 */
		
		CloseFile (FileHandle); /* close File Laser Stream*/ 
	//	printf("me->pTestParam->Production_Serial=%s\r\n",me->pTestParam->Production_Serial);
	//	sprintf(barcode_Right_file,"%s", "Hello");
		
		if (FindPattern (barcode_Right_file, 0, -1, me->pTestParam->Production_Serial, 0, 0) < 0)
		{   Erreur_laserage=1;
		//	LogSerialNumber(retest, "Production_serial different of barcode_right_file", me->pTestParam->Production_Serial, barcode_Right_file ); 
			goto RETEST;
		}
		
	    EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
		
		EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
		if(ErrorStatus==1)
			    {
				  EXCCHECK( plaser->ErrorClear(plaser));
				  Sleep(200);
				}
	/*----------------------- Lasering barcode----------------------*/			
	    EXCCHECK( plaser->RunProgram(plaser, 0007,000));               //RunProgram(SLaserPtr me, int program, int block)
	 	EXCCHECK( plaser->WriteString(plaser,0007,000, barcode_file));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		Sleep(100);
		EXCCHECK( plaser->ReadString(plaser,0007,000, barcode_file,&String_OK,String_in_laser));	      // ReadString(SLaserPtr me, int program, int block, const char* string)
#if 0    /* formerly excluded lines */
		printf("******************barcode_file********************\r\n");
		printf("barcode_file=%s\r\n",barcode_file);
		printf("String_in_laser=%s\r\n",String_in_laser);
		printf("String_OK=%d\r\n",String_OK);
#endif   /* formerly excluded lines */
		if(String_OK==1)
				{
					EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
				    EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
				    EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
				    _start_test_clock =  clock(); 
				    do{
				   	  StatusStations[7] = FALSE;
					  plaser->MarkerReady(plaser, &Ready);
					  time = (double)(clock()-_start_test_clock);
					   if (Ready == 0)
					   {
						   lasering_barcode_file_done=1;
						   break;
					   }
					   Sleep(50);
			          } while(time<timeout);
						if (time >= timeout)	
						{	Erreur_laserage=1;	
		//					LogSerialNumber(retest, "TimeOUT in barcode_file",barcode_file,String_in_laser);
		//					plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode); 
					        goto RETEST;;
							
						}
				}
		else
				{   Erreur_laserage=1;
		//			LogSerialNumber(retest, "Barcode_File different of barcode_in_laser",barcode_file,String_in_laser);
					goto RETEST;
				}
		
				EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
		
			    EXCCHECK(plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
				if(ErrorStatus==1)
			    {
				  EXCCHECK( plaser->ErrorClear(plaser));
				  Sleep(200);
				}
	/*----------------------- Lasering barcode_Right----------------------*/					
			    		EXCCHECK( plaser->RunProgram(plaser, 0002,000));               //RunProgram(SLaserPtr me, int program, int block)
			 			EXCCHECK( plaser->WriteString(plaser,0002,000, barcode_Right_file));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
						Sleep(100);
				        EXCCHECK( plaser->ReadString(plaser,0002,000, barcode_Right_file,&String_OK,String_in_laser));	      // ReadString(SLaserPtr me, int program, int block, const char* string)
#if 0    /* formerly excluded lines */
						printf("******************barcode_Right_file********************\r\n");
		                printf("barcode_Right_file=%s\r\n",barcode_Right_file);
		                printf("String_in_laser=%s\r\n",String_in_laser);
		                printf("String_OK=%d\r\n",String_OK);
#endif   /* formerly excluded lines */
						if(String_OK==1)
				{
						EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
						EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
						EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
						_start_test_clock =  clock();
						do{
					 		 StatusStations[7] = FALSE;
					  		EXCCHECK( plaser->MarkerReady(plaser, &Ready));
					   		if (Ready == 0)
								   {
									   lasering_barcode_Right_file_done=1;
									   break;
								   }
					   		Sleep(50);
					  		time = (double)(clock()-_start_test_clock);
			    		} while(time<timeout);
						if (time >= timeout)	
						{	Erreur_laserage=1;	
		//					LogSerialNumber(retest, "TimeOut Laser in barcode_File_Right",barcode_Right_file ,String_in_laser);
		//					EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode)); 
					        goto RETEST;
						}
				}
		else
				{   Erreur_laserage=1;
		//			LogSerialNumber(retest, "Barcode_Right_File different of barcode_in_laser",barcode_Right_file ,String_in_laser);
					goto RETEST;
				}

				EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
		
				EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
				if(ErrorStatus==1)
			    {
				  EXCCHECK( plaser->ErrorClear(plaser));
				  Sleep(200);
				}
	/*----------------------- Lasering Reference_VALEO----------------------*/					
			    EXCCHECK( plaser->RunProgram(plaser, 8,000));                             //RunProgram(SLaserPtr me, int program, int block)
			 	EXCCHECK( plaser->WriteString(plaser,8,000, Reference_VALEO));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
				Sleep(100);
		        EXCCHECK( plaser->ReadString(plaser,8,000, Reference_VALEO,&String_OK,String_in_laser));	      // ReadString(SLaserPtr me, int program, int block, const char* string)
#if 0    /* formerly excluded lines */
					printf("******************Reference_VALEO********************\r\n");
		                printf("Reference_VALEO=%s\r\n",Reference_VALEO);
		                printf("String_in_laser=%s\r\n",String_in_laser);
		                printf("String_OK=%d\r\n",String_OK);
#endif   /* formerly excluded lines */
				if(String_OK==1)
		{
				EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
				EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
				EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
				_start_test_clock =  clock(); 
				do{
				  	  StatusStations[7] = FALSE;
					  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
					   if (Ready == 0)
						   {
							   lasering_Reference_VALEO_done=1;
							   break;
						   }
					   Sleep(50);
					  time = (double)(clock()-_start_test_clock);
			    } while(time<timeout);
				if (time >= timeout)	
				{	Erreur_laserage=1;	
				//    LogSerialNumber(retest, "TimeOut in Reference Valeo",Reference_VALEO ,String_in_laser); 	
				//	EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
					goto RETEST;
				}
		}
		else
				{  Erreur_laserage=1;
				//	LogSerialNumber(retest, "Reference Valeo in File different of Reference_Valeo_in_laser",Reference_VALEO ,String_in_laser);
					goto RETEST;
				}
				
				EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
		
				EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
				if(ErrorStatus==1)
					    {
						  EXCCHECK( plaser->ErrorClear(plaser));
						  Sleep(200);
						}
	/*----------------------- Lasering change_release---------------------*/					
			    EXCCHECK( plaser->RunProgram(plaser, 9,000));               //RunProgram(SLaserPtr me, int program, int block)
			 	EXCCHECK( plaser->WriteString(plaser,9,000, change_release));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
				Sleep(100);
				EXCCHECK( plaser->ReadString(plaser,9,000, change_release,&String_OK,String_in_laser));	      // ReadString(SLaserPtr me, int program, int block, const char* string)
#if 0    /* formerly excluded lines */
					printf("******************change_release********************\r\n");
		                printf("change_release=%s\r\n",change_release);
		                printf("String_in_laser=%s\r\n",String_in_laser);
		                printf("String_OK=%d\r\n",String_OK);
#endif   /* formerly excluded lines */
				if(String_OK==1)
		{
				EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
				EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
				EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
				_start_test_clock =  clock(); 
				do{
				  	  StatusStations[7] = FALSE;
					  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
					   if (Ready == 0)
						   {
							   lasering_change_release_done=1;
							   break;
						   }
					   Sleep(50);
					  time = (double)(clock()-_start_test_clock);
			    } while(time<timeout);
				if (time >= timeout)	
				{	Erreur_laserage=1;	
				//	LogSerialNumber(retest, "TimeOut in change_release",change_release ,String_in_laser);
				//	EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
					goto RETEST;
				}		
				
		}
		else
				{   Erreur_laserage=1;
				//	LogSerialNumber(retest, "change_release in File different of change_release_in_laser",change_release ,String_in_laser);
					goto RETEST;
				}
				//introducing spaces
				CopyString(ch1,0,BMW_PN,0,1);
				CopyString(ch2,0,BMW_PN,1,3);
				CopyString(ch3,0,BMW_PN,4,3);
				sprintf(BMW_PN,"%s %s %s",ch1,ch2,ch3); 
				
				EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
				
				if(ErrorStatus==1)
					    {
						  EXCCHECK( plaser->ErrorClear(plaser));
						  Sleep(200);
						}
	/*----------------------- Lasering Reference_BMW_PN----------------------*/	
			    EXCCHECK( plaser->RunProgram(plaser, 10,000));               //RunProgram(SLaserPtr me, int program, int block)
			 	EXCCHECK( plaser->WriteString(plaser,10,000, BMW_PN));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
				Sleep(100);
				EXCCHECK( plaser->ReadString(plaser,10,000, BMW_PN,&String_OK,String_in_laser));	      // ReadString(SLaserPtr me, int program, int block, const char* string)
#if 0    /* formerly excluded lines */
				printf("******************BMW_PN********************\r\n");
		                printf("BMW_PN=%s\r\n",BMW_PN);
		                printf("String_in_laser=%s\r\n",String_in_laser);
		                printf("String_OK=%d\r\n",String_OK);
#endif   /* formerly excluded lines */
				if(String_OK==1)
		{
				EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
				EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
				EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
				_start_test_clock =  clock(); 
				do{
				  	  StatusStations[7] = FALSE;
					  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
					   if (Ready == 0)
						   {
							   lasering_BMW_PN_done=1;
							   break;
						   }
					   Sleep(50);
					  time = (double)(clock()-_start_test_clock);
					   
			    } while(time<timeout);
				if (time >= timeout)	
				{	Erreur_laserage=1;	
				//	LogSerialNumber(retest, "TimeOut in BMW_PN",BMW_PN ,String_in_laser);
				//	EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
					goto RETEST;
				}
				;
		}
		else
				{   Erreur_laserage=1;
				//	LogSerialNumber(retest, "BMW_PN in File different of BMW_PN_in_laser",BMW_PN ,String_in_laser);
					goto RETEST;
				}

				EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
				
				if(ErrorStatus==1)
					    {
						  EXCCHECK( plaser->ErrorClear(plaser));
						  Sleep(200);
						}
	/*----------------------- Lasering Reference_HW_SW----------------------*/
			    EXCCHECK( plaser->RunProgram(plaser, 11,000));               //RunProgram(SLaserPtr me, int program, int block)
			 	EXCCHECK( plaser->WriteString(plaser,11,000, HW_SW));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
				Sleep(100);
				EXCCHECK( plaser->ReadString(plaser,11,000, HW_SW,&String_OK,String_in_laser));	      // ReadString(SLaserPtr me, int program, int block, const char* string)
#if 0    /* formerly excluded lines */
					printf("******************HW_SW********************\r\n");
		                printf("HW_SW=%s\r\n",HW_SW);
		                printf("String_in_laser=%s\r\n",String_in_laser);
		                printf("String_OK=%d\r\n",String_OK);		
#endif   /* formerly excluded lines */
				if(String_OK==1)
		{
				EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
				EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
				EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
				_start_test_clock =  clock(); 
				do{
				  	  StatusStations[7] = FALSE;
					  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
					   if (Ready == 0)
						   {
							   lasering_HW_SW_done=1;
							   break;
						   }
					   Sleep(50);
					  time = (double)(clock()-_start_test_clock);
					   
			    } while(time<timeout);
				if (time >= timeout)	
				{	Erreur_laserage=1;	
				//	LogSerialNumber(retest, "TimeOut in HW_SW",HW_SW ,String_in_laser); 
				//	EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
					goto RETEST;
				}
		}
			else
				{  Erreur_laserage=1;
				//	LogSerialNumber(retest, "HW_SW in File different of HW_SW_in_laser",HW_SW ,String_in_laser);
					goto RETEST;
				}
		

			    EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
				
				if(ErrorStatus==1)
					    {
						  EXCCHECK( plaser->ErrorClear(plaser));
						  Sleep(200);
						}
	/*----------------------- Lasering Reference_date_file----------------------*/
			    EXCCHECK( plaser->RunProgram(plaser, 12,000));               //RunProgram(SLaserPtr me, int program, int block)
			 	EXCCHECK( plaser->WriteString(plaser,12,000, date_file));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
				Sleep(100);
				EXCCHECK( plaser->ReadString(plaser,12,000, date_file,&String_OK,String_in_laser));	      // ReadString(SLaserPtr me, int program, int block, const char* string)
#if 0    /* formerly excluded lines */
					printf("******************date_file********************\r\n");
		                printf("date_file=%s\r\n",date_file);
		                printf("String_in_laser=%s\r\n",String_in_laser);
		                printf("String_OK=%d\r\n",String_OK);		
#endif   /* formerly excluded lines */
				if(String_OK==1)
		{
				EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
				EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
				EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
				_start_test_clock =  clock(); 
				do{
				  	  StatusStations[7] = FALSE;
					  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
					if (Ready == 0)
						   {
							   lasering_date_file_done=1;
							   break;
						   }
					   Sleep(50);
					  time = (double)(clock()-_start_test_clock);
			    } while(time<timeout);
				if (time >= timeout)	
				{	Erreur_laserage=1;	
				//	LogSerialNumber(retest, "TimeOut in date_file",date_file ,String_in_laser);
				//	EXCCHECK( plaser->ErrorStatus(plaser,&ErrorStatus,ErrorCode));
					goto RETEST;
				}
		
		}
		else
				{
					Erreur_laserage=1;
				//	LogSerialNumber(retest, "date_file in File different of date_file_in_laser",date_file ,String_in_laser);
					goto RETEST;
				}
		
	
RETEST:
	
if(Erreur_laserage==1)
	{
	 sprintf(me->barcode, "no_barcode");
	 sprintf(me->barcode_Right, "no_barcode");
	 EXCTHROW(-1, "Erreur Laserage:\nEliminer la piece dans la station laser");  
	
	}
	 else
	 {
		 /* 4- Read Barcodes */
		gs_pTester->MoveBarcodeFront(gs_pTester, 10000);
		
		
		EXCCHECK( gs_pTester->GetItemCode(gs_pTester, "", 0, BARCODE_BUFFER_SIZE, Buffer, 2000));//Read Barcode Right
		ExtractDoubleBarcode(Buffer, barcode_Right);
		
	//	printf("barcode_Right=%s\r\n",barcode_Right);
		
		EXCCHECK( gs_pTester->GetItemCode(gs_pTester, "", 2, BARCODE_BUFFER_SIZE, Buffer, 2000));//Read Barcode Central
		ExtractDoubleBarcode(Buffer, barcode);
		
	//	printf("barcode=%s\r\n",barcode);
		
		gs_pTester->MoveBarcodeRear(gs_pTester, 0);
		 
#if 0    /* formerly excluded lines */
		 /* 4- Read Barcodes */
		gs_pTester->MoveBarcodeFront(gs_pTester, 10000);

	
		pthread_data[0] = (SBarcodeDataPtr)calloc(1, sizeof(SBarcodeData)); 
		EXCCHECKALLOC( pthread_data[0]);
		pthread_data[0]->barcode_ID = 0;
		EXCCHECK(ThreadStart(Thread_Barcode, pthread_data[0], &hThreadBarcode[0], NULL));
		
		pthread_data[1] = (SBarcodeDataPtr)calloc(1, sizeof(SBarcodeData));  // old 2
		EXCCHECKALLOC( pthread_data[1]);
		pthread_data[1]->barcode_ID = 2;
		EXCCHECK(ThreadStart(Thread_Barcode, pthread_data[1], &hThreadBarcode[1], NULL));
		
		time = (double)(clock()-_start_test_clock);
		
		/* Wait thread to finish */
		ThreadStatus(hThreadBarcode[0], 5000, NULL);
	    ThreadStatus(hThreadBarcode[1], 5000, NULL);

		ExtractDoubleBarcode(pthread_data[0]->barcode, barcode_Right);
		ExtractDoubleBarcode(pthread_data[1]->barcode ,barcode);
		
		
		/* free thread data */
		free(pthread_data[0]);
		free(pthread_data[1]);
	
		printf(" free data barcode\r\n"); 
		/* free thread handle */
		
			if(hThreadBarcode[0])
			{
			CloseHandle(hThreadBarcode[0]);
			hThreadBarcode[0] = NULL;
			}
			
			if(hThreadBarcode[1])
			{
			CloseHandle(hThreadBarcode[1]);
			hThreadBarcode[1] = NULL;
			}
#endif   /* formerly excluded lines */
		
		
/********************************************************************************************/

		/*5- Check Barcodes in LASIT file if equal to Lasered Barcodes */
		if (!strcmp(barcode_file, barcode))
		    sprintf(me->barcode,"%s", barcode);
		    
		else 
		{
		//	LogSerialNumber(retest, "Barcode different of barcode_file", barcode, barcode_file ); 
			sprintf(me->barcode, "no_barcode");
			gs_pTester->MoveBarcodeRear(gs_pTester, 0);
		//	EXCTHROW(-1, "Erreur Laserage: 1-Redemarrez l'application,2-verifier l'état du laser,3- Eliminer la piece dans la station laser");  
		}
			
		if (!strcmp(barcode_Right_file, barcode_Right))
			sprintf(me->barcode_Right,"%s", barcode_Right);
		else 
		{
		//    LogSerialNumber(retest, "Barcode Right different of barcode_right_file", barcode_Right, barcode_Right_file ); 
			sprintf(me->barcode_Right, "no_barcode");
			gs_pTester->MoveBarcodeRear(gs_pTester, 0);
		//	EXCTHROW(-1, "Erreur Laserage: 1-Redemarrez l'application,2-verifier l'état du laser,3- Eliminer la piece dans la station laser"); 
		}
	}
	}

Error:
//	CloseFile (FileHandle); /* close File Laser Stream*/ 
	Erreur_laserage=1;
	gs_pTester->MoveBarcodeRear(gs_pTester, 0);
	EXCRETHROW( pexception);  
}




/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelLabel2"
SElExceptionPtr JigPanelLabel2(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SLaserPtr	    plaser = NULL;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[1024]= "";
	char            barcode[512]= "";
	char            barcode_Left[512]= "";
	char            barcode_Right[512]= "";
	
	char            barcode_file[512]= "";
	char            barcode_Left_file[512]= "";
	char            barcode_Right_file[512]= "";
	char            Reference_VALEO[512]= "";

	int 			FileHandle;
	int             index;
	int				Ready;
	int             i=0;
	clock_t 		_start_test_clock;
	uint32_t		timeout=35000;
	double          time;
	SBarcodeDataPtr	pthread_data[3];
   	HANDLE  		hThreadBarcode[3];
	
	
	PANEL_STATION_STATUS(7,RUN);

	if(gs_Automat.Mode==AUTOMAT_PRODUCTION)
	{
		
		StatusStations[7] = FALSE;
		_start_test_clock =  clock();
		
		EXCCHECK( gs_pLasers->GetLaserByIndex(gs_pLasers, 0, &plaser));

		/* 1- create new Laser keyence File */
		sprintf(Buffer, "%s;%s", me->pTestParam->Date, me->pTestParam->Production_Serial);
		
		EXCCHECK( me->pData->SetStringPtr(me->pData, Buffer));
		
		EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TESTER_BUTTON_PRESSED_LABEL", me->pData));
		
		/* 2- wait update file */
		Sleep(500); 
		
		/* 2- Retour codes à barres arriéres */
		gs_pTester->MoveBarcodeRear(gs_pTester, 10000);

		/* 3- Read Barcodes have to be Lasered from file LASIT */
	 	FileHandle = OpenFile ("C:\\keyence\\MFL_Laser_data.txt", VAL_READ_ONLY, VAL_TRUNCATE, VAL_ASCII); /* open File Laser Stream*/
	
		/* - Laser Line 1 */
		ReadLine (FileHandle, barcode_file, -1); 	/* Read Line 1 */
				
	    EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
		
	    EXCCHECK( plaser->RunProgram(plaser, 0003,000));               //RunProgram(SLaserPtr me, int program, int block)
		
	 	EXCCHECK( plaser->WriteString(plaser,0003,000, barcode_file));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		
		EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
		
		EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready
		
		EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
		Sleep(200); 

		_start_test_clock =  clock(); 
		
		do{
		   	  StatusStations[7] = FALSE;
			  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
			  
			  time = (double)(clock()-_start_test_clock);
			   if (Ready == 0)
				   break;
			 
			   Sleep(50);
			 
			  
			   
	    } while(time<timeout);
		
		if (time >= timeout)	
		{		
			EXCTHROW(-1, "Timeout Laser");
		}
		
		
		/* - Laser Line 2 */
		ReadLine (FileHandle, barcode_Left_file, -1); 	/* Read Line 2 */
		
		EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
			  
	    EXCCHECK( plaser->RunProgram(plaser, 0001,000));               //RunProgram(SLaserPtr me, int program, int block)
		
	 	EXCCHECK( plaser->WriteString(plaser,0001,000, barcode_Left_file));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		
		EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
		
		EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
		
		EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)

		Sleep(200);
		_start_test_clock =  clock(); 
		
		do{
		   	  StatusStations[7] = FALSE;
			  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
			   if (Ready == 0)
				   break;
			 
			   Sleep(50);
			 
			  time = (double)(clock()-_start_test_clock);
			   
	    } while(time<timeout);
		
		if (time >= timeout)	
		{		
			EXCTHROW(-1, "Timeout Laser");
		}
		
		
		
		/* - Laser Line 3 */
		ReadLine (FileHandle, barcode_Right_file, -1); 	/* Read Line 3 */		
		
		 EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
	    EXCCHECK( plaser->RunProgram(plaser, 0002,000));               //RunProgram(SLaserPtr me, int program, int block)
	 	EXCCHECK( plaser->WriteString(plaser,0002,000, barcode_Right_file));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
		EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready
		
		EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr m
		
		Sleep(200); 
		_start_test_clock =  clock();
		
		do{
			  StatusStations[7] = FALSE;
			  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
			  
			   if (Ready == 0)
				   break;
			 
			   Sleep(50);
			 
			  time = (double)(clock()-_start_test_clock);
			   
	    } while(time<timeout);
		
		if (time >= timeout)	
		{		
			EXCTHROW(-1, "Timeout Laser");
		}

		
			
		/* - Laser Line 4 */
		ReadLine (FileHandle, Reference_VALEO, -1); 	/* Read Line 4 */		
		
		 EXCCHECK( plaser->Connect(plaser));               //RunProgram(SLaserPtr me, int program, int block)
	    EXCCHECK( plaser->RunProgram(plaser, 0000,000));               //RunProgram(SLaserPtr me, int program, int block)
	 	EXCCHECK( plaser->WriteString(plaser,0000,000, Reference_VALEO));	      // WriteString(SLaserPtr me, int program, int block, const char* string)
		EXCCHECK( plaser->EnableMarking(plaser));	                  //EnableMarking(SLaserPtr me)
		EXCCHECK( plaser->MarkerReady(plaser, &Ready));		              //MarkerReady(SLaserPtr me, int *Ready)
		EXCCHECK( plaser->StartLasering(plaser));		  //StartLasering(SLaserPtr me)
		Sleep(200); 
		
		_start_test_clock =  clock(); 
		do{
		  	  StatusStations[7] = FALSE;
			  EXCCHECK( plaser->MarkerReady(plaser, &Ready));
			  
			   if (Ready == 0)
				   break;
			 
			   Sleep(50);
			 
			  time = (double)(clock()-_start_test_clock);
			   
	    } while(time<timeout);
		
		if (time >= timeout)	
		{		
			EXCTHROW(-1, "Timeout Laser");
		}	
	
		
		/* 4- Read Barcodes */
		gs_pTester->MoveBarcodeFront(gs_pTester, 10000);
		
/*********************************************************************************************************************************/
		
		for (i=0; i<3; i++)
		{
			
			if(i!=1)
			{
		pthread_data[i] = (SBarcodeDataPtr)calloc(1, sizeof(SBarcodeData)); 
		EXCCHECKALLOC( pthread_data[i]);
		pthread_data[i]->barcode_ID = i;
		EXCCHECK(ThreadStart(Thread_Barcode, pthread_data[i], &hThreadBarcode[i], NULL));
		}
		}
	
#if 0    /* formerly excluded lines */
		/* Wait thread to finish */
		for (i=0; i< 3 ; i++)
			pexception = ThreadStatus(hThreadBarcode[i], 5000, NULL); 
#endif   /* formerly excluded lines */
		
		pexception = ThreadStatus(hThreadBarcode[0], 5000, NULL);
		pexception = ThreadStatus(hThreadBarcode[2], 5000, NULL);

		ExtractDoubleBarcode(pthread_data[0]->barcode, barcode_Right);
	//	ExtractDoubleBarcode(pthread_data[1]->barcode, barcode_Left);
		ExtractDoubleBarcode(pthread_data[2]->barcode ,barcode);
		
#if 0    /* formerly excluded lines */
		/* free thread data */
		for (i=0; i< 3 ; i++)
			free(pthread_data[i]);
#endif   /* formerly excluded lines */
		free(pthread_data[0]);  
		free(pthread_data[2]);  
	
#if 0    /* formerly excluded lines */
		/* free thread handle */
		for (i=0; i< 3 ; i++)
		{
			if(hThreadBarcode[i])
			{
			CloseHandle(hThreadBarcode[i]);
			hThreadBarcode[i] = NULL;
			}
		}
#endif   /* formerly excluded lines */
		
		if(hThreadBarcode[0])
			{
			CloseHandle(hThreadBarcode[0]);
			hThreadBarcode[0] = NULL;
			}
		if(hThreadBarcode[2])
			{
			CloseHandle(hThreadBarcode[2]);
			hThreadBarcode[2] = NULL;
			}
		
/********************************************************************************************/

		/*5- Check Barcodes in LASIT file if equal to Lasered Barcodes */
		
	
		if (!strcmp(barcode_file, barcode))
		    sprintf(me->barcode,"%s", barcode);
		    
		else 
		{
			//printf("strlen (barcode_file) = %d \r\n", strlen (barcode_file));
			//printf("strlen (barcode) = %d \r\n", strlen (barcode));
			//printf("barcode_file = %s\r\n", barcode_file);
			//printf("barcode = %s\r\n", barcode);
			sprintf(me->barcode, "no_barcode");
		}
			
		
#if 0    /* formerly excluded lines */
		if (!strcmp(barcode_Left_file, barcode_Left))
		 	sprintf(me->barcode_Left,"%s", barcode_Left);
		else 
		{
			//printf("strlen (barcode_Left_file) = %d \r\n", strlen (barcode_Left_file));
			//printf("strlen (barcode_Left) = %d \r\n", strlen (barcode_Left));
			//printf("barcode_Left_file = %s\r\n", barcode_Left_file);
			//printf("barcode_Left = %s\r\n", barcode_Left);
			sprintf(me->barcode_Left, "no_barcode");
		}
#endif   /* formerly excluded lines */
		
		
		if (!strcmp(barcode_Right_file, barcode_Right))
			sprintf(me->barcode_Right,"%s", barcode_Right);
		else 
		{
			//printf("strlen (barcode_Right_file) = %d \r\n", strlen (barcode_Right_file));
			//printf("strlen (barcode_Right) = %d \r\n", strlen (barcode_Right));
			//printf("barcode_Right_file = %s\r\n", barcode_Right_file);
			//printf("barcode_Right = %s\r\n", barcode_Right);
		    sprintf(me->barcode_Right, "no_barcode");
		}
	}

Error:
	CloseFile (FileHandle); /* close File Laser Stream*/ 
	gs_pTester->MoveBarcodeRear(gs_pTester, 0);
	EXCRETHROW( pexception);  
}

#if 0    /* formerly excluded lines */
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelLabelCheck"
SElExceptionPtr JigPanelLabelCheck(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[1024]= "";
	char            barcode[512]= "";
	char            barcode_Left[512]= "";
	char            barcode_Right[512]= "";
	char            barcode_file[512]= "";
	char            barcode_Left_file[512]= "";
	char            barcode_Right_file[512]= "";
	int 			FileHandle;
	int             index;
	int             i=0;
	
	SBarcodeDataPtr	pthread_data[2];
   	HANDLE  		hThreadBarcode[2];
	
	
	PANEL_STATION_STATUS(7,RUN);

	if(gs_Automat.Mode==AUTOMAT_PRODUCTION)
	{
	
		/* 1- init Value Barcodes */
		sprintf(me->barcode, "%s\0", "no_barcode");
		sprintf(me->barcode_Right, "%s\0", "no_barcode");
		
		EXCCHECK( gs_pTester->MoveBarcodeFront(gs_pTester, 3000));
	
		
		pthread_data[0] = (SBarcodeDataPtr)calloc(1, sizeof(SBarcodeData)); 
		EXCCHECKALLOC( pthread_data[0]);
		pthread_data[0]->barcode_ID = 0;
		EXCCHECK(ThreadStart(Thread_Barcode, pthread_data[0], &hThreadBarcode[0], NULL));
		
		pthread_data[1] = (SBarcodeDataPtr)calloc(1, sizeof(SBarcodeData)); 
		EXCCHECKALLOC( pthread_data[1]);
		pthread_data[1]->barcode_ID = 2; // old 2  
		EXCCHECK(ThreadStart(Thread_Barcode, pthread_data[1], &hThreadBarcode[1], NULL));
	

			pexception = ThreadStatus(hThreadBarcode[0], 2000, NULL); 
		    pexception = ThreadStatus(hThreadBarcode[1], 2000, NULL); // old 2  

		ExtractDoubleBarcode(pthread_data[0]->barcode, me->barcode_Right);
		ExtractDoubleBarcode(pthread_data[1]->barcode ,me->barcode);   // old 2  
		
		/* free thread data */
			free(pthread_data[0]);
			free(pthread_data[1]); // old 2  
	
		/* free thread handle */

		
		if(hThreadBarcode[0])
			{
			CloseHandle(hThreadBarcode[0]);
			hThreadBarcode[0] = NULL;
			}
		
		if(hThreadBarcode[1])   // old 2  
			{
			CloseHandle(hThreadBarcode[1]);	   // old 2  
			hThreadBarcode[1] = NULL;   // old 2
			}
		

		if (strlen (me->barcode_Right) != 60)
		{
		//	printf ("strlen (me->barcode_Right) = %d \r\n", strlen (me->barcode_Right));
		//	printf ("me->barcode_Right = %s \r\n", me->barcode_Right);
			EXCTHROW(-1, "barcode_Right not Found");
		}
			
		/*if (strlen (me->barcode_Left) != 60)
		{			
		//	printf ("strlen (me->barcode_Left) = %d \r\n", strlen (me->barcode_Left));
		//	printf ("me->barcode_Left = %s \r\n", me->barcode_Left);	
			EXCTHROW(-1, "barcode_Left not Found");
		}	  */ 
		
		if (strlen (me->barcode) != 11)
		{
		//	printf ("strlen (me->barcode) = %d \r\n", strlen (me->barcode));
		//	printf ("me->barcode = %s \r\n", me->barcode); 
			EXCTHROW(-1, "Barcode not Found");		  
		}	

			
	}
		
Error: 
	gs_pTester->MoveBarcodeRear(gs_pTester, 0);
	EXCRETHROW( pexception);  
}
#endif   /* formerly excluded lines */

 //#if 0    /* formerly excluded lines */  
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "JigPanelLabelCheck"
SElExceptionPtr JigPanelLabelCheck(struct _SSledge* me, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	bool_t			isCorrect = FALSE;
	int32_t			master_id;
	char            Buffer[1024]= "";
	char            barcode[512]= "";
	char            barcode_Left[512]= "";
	char            barcode_Right[512]= "";
	char            barcode_file[512]= "";
	char            barcode_Left_file[512]= "";
	char            barcode_Right_file[512]= "";
	int 			FileHandle;
	int             index;
	int             i=0;
	
	 *pError=0; 

	PANEL_STATION_STATUS(7,RUN);

	if(gs_Automat.Mode==AUTOMAT_PRODUCTION)
	{
	
		/* 1- init Value Barcodes */
		sprintf(me->barcode, "%s\0", "no_barcode");
		sprintf(me->barcode_Right, "%s\0", "no_barcode");
		
		EXCCHECK( gs_pTester->MoveBarcodeFront(gs_pTester, 3000));
		
		EXCCHECK( gs_pTester->GetItemCode(gs_pTester, "", 0, BARCODE_BUFFER_SIZE, Buffer, 2000));//Read Label Barcode right
		EXCCHECK( ExtractDoubleBarcode(Buffer, me->barcode_Right));
		
	//	printf("barcode_Right_check=%s\r\n",me->barcode_Right);
		
		EXCCHECK( gs_pTester->GetItemCode(gs_pTester, "", 2, BARCODE_BUFFER_SIZE, Buffer, 2000));//Read Label 2 Barcode
		EXCCHECK( ExtractDoubleBarcode(Buffer, me->barcode));//ABH elimination codebarre left  barcode_Left));
//		printf("barcode_check=%s\r\n",me->barcode);
		
//		printf("strlen_barcode_Right=%d\r\n",strlen (me->barcode_Right));
//		printf("strlen_barcode=%d\r\n",strlen (me->barcode));
		
		if (strlen (me->barcode_Right) != 60 && strlen (me->barcode_Right) == 0 && strlen (me->barcode) != 11 && strlen (me->barcode) == 0)
		{
	       EXCTHROW(-1, "barcode not Found");
		   *pError=0;
		} 
		else if ((strlen (me->barcode_Right) != 60 && strlen (me->barcode_Right) != 0) || (strlen (me->barcode) != 11 && strlen (me->barcode) != 0) ) 
		{	
			sprintf(me->barcode_Right, "%s\0", "no_barcode"); 
		    sprintf(me->barcode ,"%s\0", "no_barcode");
			*pError=1;
		}
			
	}
		
Error: 
	gs_pTester->MoveBarcodeRear(gs_pTester, 0);
	EXCRETHROW( pexception);  
}

//#endif   /* formerly excluded lines */ 
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetNewSerial"
SElExceptionPtr GetNewSerial(int* counter, int32_t* pError)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	EIdShift		shift_id = 0;
	bool_t			isOpen = FALSE;
	


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
				
		pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 1200);
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
		
				
		pexception = gs_pTester->LockPanelStation10(gs_pTester, 2000);
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
#define __FUNC__ "JigPanelInsert"
static SElExceptionPtr JigPanelInsert(int32_t* pError)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;

		gs_pTester->SetSensor(gs_pTester, "INDICATOR_OK", TRUE);
		PANEL_TABLE_WARNING (PANEL_WARNING , TRUE);
		PANEL_MSG( PANEL_HELP2, _("Table Rotation... "));
		//Sleep(500);  /* wait ready all actuators */
		pexception = gs_pTester->TableRotate(gs_pTester, ROTATION_TIMEOUT, TRUE);
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

		gs_pTester->SetSensor(gs_pTester, "INDICATOR_OK", TRUE);
		PANEL_MSG( PANEL_HELP2, _("Table Rotation... "));
		PANEL_TABLE_WARNING (PANEL_WARNING , TRUE);
		//Sleep(500);  /* wait ready all actuators */
		EXCCHECK(gs_pTester->TableRotate(gs_pTester, ROTATION_TIMEOUT, TRUE));
			
Error:
	PANEL_TABLE_WARNING (PANEL_WARNING , FALSE);
	gs_pTester->SetSensor(gs_pTester, "INDICATOR_OK", FALSE);
	EXCRETHROW( pexception);  
}

#undef __FUNC__
#define __FUNC__ "JigPanelRelease"
static SElExceptionPtr JigPanelRelease(struct _SSledge* me, bool_t fault)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	char				buffer[2048] = "";
	char				AutomatModeStr[AUTOMAT_MODE_SIZE][32] = AUTOMAT_MODE_STR;

	
	if(fault)   //Print FAIL Label & put panel in the Red BOX
	{

			PANEL_STATION_STATUS(7,RUN);

			if (gs_Automat.PICK_PLACE_active)
				{
				EXCCHECK( gs_pTester->UnlockPanelStation80(gs_pTester, 5000));
				Sleep(500);//wait pallett open
				EXCCHECK( gs_pTester->PickPlace(gs_pTester, 15000, TRUE));
				Sleep(500);//wait pallett open
				EXCCHECK( gs_pTester->ResetPickPlace(gs_pTester, 15000, TRUE));
				Sleep(500);//wait pallett open
				EXCCHECK( gs_pTester->LockPanelStation80(gs_pTester, 2000));
				}
		
			StatusStations[7] = TRUE;
			
			PANEL_STATION_STATUS(7,NOK);
			
			EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  don't display TEST NOK in station 8 */
		
			if (gs_Automat.PRINTER_active)
			{
				if (gs_Automat.LASER_active)  // check if barcode has been read or not
				{
					if (!strcmp(me->barcode,"no_barcode") && 
				//	!strcmp(me->barcode_Left,"no_barcode") && 
					!strcmp(me->barcode_Right,"no_barcode"))
					{
					sprintf(buffer, "%s;%s;%s", "PRODUCTION", ".//system_fault.xml", "FAULT");
					EXCCHECK( eleventdata_new(&(me->pData)));
					EXCCHECK( me->pData->SetStringPtr(me->pData, buffer));
					/* Save failed status in datatbase and print Fail Label if Fail */ 
					EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TEST_SEQUENCE_END", me->pData));
					}
					else
					{
					/* Save failed status in datatbase and print Fail Label if Fail */ 
					EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TEST_SEQUENCE_END", me->pData));
					}
				}
				else
					{
					/* Save failed status in datatbase and print Fail Label if Fail */ 
					EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_TEST_SEQUENCE_END", me->pData));
					}
				
			}
			
			PANEL_MSG( PANEL_RESULT_MSG, _("TEST NOK"));
			PANEL_STATION_STATUS(0,NOK);
	}

	else 
	{		   

		if (gs_Automat.LASER_active)  // check if barcode has been read or not
			{
				
				if (!strcmp(me->barcode,"no_barcode") || !strcmp(me->barcode_Right,"no_barcode") )
					{
						PANEL_STATION_STATUS(7,NOK);
						
						if (gs_Automat.PICK_PLACE_active)
						{
						EXCCHECK( gs_pTester->PickPlace(gs_pTester, 15000, TRUE));
						Sleep(500);//wait pallett open
						EXCCHECK( gs_pTester->ResetPickPlace(gs_pTester, 15000, TRUE));
						Sleep(500);//wait pallett open
						EXCCHECK( gs_pTester->LockPanelStation80(gs_pTester, 2000));
						}
						
					}
				else PANEL_STATION_STATUS(7,OK);
			}
	
		else PANEL_STATION_STATUS(7,OK);
	
		StatusStations[7] = TRUE;
		
		EXCCHECK( gs_pTester->WaitRotationFinished(gs_pTester, INFINITE)); 	/*  don't display TEST OK in station 8 */
	
		if (gs_Automat.LASER_active)
		{
		if (!strcmp(me->barcode,"no_barcode") || 
			!strcmp(me->barcode_Right,"no_barcode") 
		    )
				{
				PANEL_MSG( PANEL_RESULT_MSG, _("TEST NOK"));
				PANEL_STATION_STATUS(0,NOK);
				}
				else 
				{
				PANEL_MSG( PANEL_RESULT_MSG, _("TEST OK"));
				PANEL_STATION_STATUS(0,OK);
				PANEL_MSG( PANEL_HELP2, _("Take Panel"));
				}
		}
	
		else 
		{
		PANEL_MSG( PANEL_RESULT_MSG, _("TEST OK"));
		PANEL_STATION_STATUS(0,OK);
		PANEL_MSG( PANEL_HELP2, _("Take Panel"));
		}
	}
Error:
	if (pexception)
	{
	PANEL_STATION_STATUS(7,NOK);

		if (gs_Automat.PICK_PLACE_active)
			gs_pTester->ResetPickPlace(gs_pTester, 15000, TRUE);
	gs_pTester->LockPanelStation80(gs_pTester, 2000);
	PANEL_STATION_STATUS(0,FREE);
	}
	StatusStations[7] = TRUE;
	eleventdata_delete(&(me->pData));
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
		                                   "EVNT_JIG_NEW",
		                                   &gs_EventHandle[EVNT_JIG_NEW_]));
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

	if(0==strcmp(eventName, "EVNT_TESTER_ROTATION_FINISHED") )
	{
	GetPalettID(&gs_PalettID);
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
	int32_t			insert1, insert2;
	
	
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

			/* Laser + RED BOX */
			#if 0    /* formerly excluded lines */
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			if (full)
			{
				PANEL_MSG( PANEL_HELP2, _("Empty Red Box"));
			}
			else if (!closed)
			{
				PANEL_MSG( PANEL_HELP2, _("Close Red Box"));
			}
			else if (erreur)
			{
				PANEL_MSG( PANEL_HELP2, _("Reset Printer Controller"));
			}
			#endif   /* formerly excluded lines */

		   if( gs_Automat.box)
			{
				PANEL_MSG( PANEL_HELP2, _("Insert Empty Box"));
				INSERT_EMPTY_BOX();
			}
			
			else if( gs_Automat.masterpart)
			{
				PANEL_MSG( PANEL_HELP2, _("Insert Master Panel"));
			}
			
			else
			{
#if 0    /* formerly excluded lines */
			/* Unlock Pallett in the first time */
			PANEL_MSG( PANEL_HELP2, _("Push Start to Open Pallett"));
			JigPanelUnlock(&error);
#endif   /* formerly excluded lines */
	
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
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
					{	PANEL_MSG( PANEL_HELP2, _("Insert Panel")); 
						//printf("Insert Panel in EventFunctionPluginCallback\n\r");  
					}
					else if (insert1 && insert2)
					{	PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));  
						//	printf("Push Start Cycle in EventFunctionPluginCallback\n\r");         
					}
					}
			}
		}
	
	}
	
	else if(0==strcmp(eventName, "EVNT_GUI_END_VERSION"))
	{

		if (gs_Automat.EmtyTable==FALSE && gs_Automat.isTableEmpty == FALSE && gs_Automat.shift_id!=0)
		{
			if(gs_Automat.Mode!=AUTOMAT_QUALITY_MASTER)
			{
			PANEL_MSG( PANEL_HELP1, _("End Of Actual Version"));
			}
			/* Check if product in pallett */
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
			if (!insert1 && !insert2)
			{	
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			}
			else 
			{
			pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 3000);
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

	 if (gs_Automat.isTableEmpty == TRUE)
		{
		EXCDISPLAY( DisplayVersionList());
		}
		
	 if(gs_Automat.isTableEmpty == FALSE && gs_Automat.EmtyTable == FALSE && gs_Automat.shift_id!=0)
		{
			//End Actual Version
			PANEL_MSG( PANEL_HELP1, _("Change Version")); 
			/* Check if product in pallett */
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_LEFT", &insert1));
			EXCCHECK( gs_pTester->GetSensor(gs_pTester, "OUTPUT_LASER_RIGHT", &insert2));
			if (!insert1 && !insert2)
			{	
			PANEL_MSG( PANEL_HELP2, _("Push Start Cycle"));
			}
			else 
			{
			pexception = gs_pTester->UnlockPanelStation10(gs_pTester, 3000);
			PANEL_MSG( PANEL_HELP2, _("Remove Panel"));
			}
			gs_Automat.EmtyTable = TRUE;
			gs_Automat.ChangeVersion = TRUE;
			gs_Table_Rotation = 0;
		}
	}

	else if(0==strcmp(eventName, "EVNT_RED_BOX_EMPTY"))
	{
		{
			gs_Automat.box = FALSE;

			/* RED BOX */
			#if 0    /* formerly excluded lines */
			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			if (full)
			{
				PANEL_MSG( PANEL_HELP2, _("Empty Red Box"));
			}
			else if (!closed)
			{
				PANEL_MSG( PANEL_HELP2, _("Close Red Box"));
			}
			else if (erreur)
			{
				PANEL_MSG( PANEL_HELP2, _("Reset Printer Controller"));
			}
			#endif   /* formerly excluded lines */
		   if(gs_Automat.shift_id==0)
			{
				PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
				PANEL_MSG( PANEL_HELP2, "");
			}
			else
			{
				if( !gs_Automat.EmtyTable)
					PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
				//printf("Insert Panel in EventFunctionPluginCallback RED BOX\n\r"); 
			}
		}
		
	}

/* Laser */
#if 0    /* formerly excluded lines */
	else if(0==strcmp(eventName, "EVNT_PRINTER_READY"))
	{
		{
			gs_Automat.box = FALSE;

			EXCCHECK( gs_pTester->GetStatusRedBox(gs_pTester, &full, &closed));
			EXCCHECK( gs_pTester->GetStatusPrinter(gs_pTester, &finished, &erreur));
			if (full)
			{
				PANEL_MSG( PANEL_HELP2, _("Empty Red Box"));
			}
			else if (!closed)
			{
				PANEL_MSG( PANEL_HELP2, _("Close Red Box"));
			}
			else if (erreur)
			{
				PANEL_MSG( PANEL_HELP2, _("Reset Printer Controller"));
			}
			else if(gs_Automat.shift_id==0)
			{
				PANEL_MSG( PANEL_HELP1, _("Select Shift"));  
				PANEL_MSG( PANEL_HELP2, "");
			}
			else
			{
				if( !gs_Automat.EmtyTable)
					PANEL_MSG( PANEL_HELP2, _("Insert Panel"));
				//printf("Insert Panel in EventFunctionPluginCallback EVNT_PRINTER_READY\n\r");  
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
		  || 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_RNR")
		  || 0==strcmp(eventName, "EVNT_GUI_EXPERTISE_SEQUENCE_CREATED")
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
			
			pdata = data;				/* tester mode */
			pdata2 = strchr(pdata, ';');
			if (pdata2 != NULL)			/* xml path */
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
														 "EVNT_TESTER_ROTATION_FINISHED",
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
														 "EVNT_GUI_TEST",
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
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_END_VERSION",
														 EventFunctionPluginCallback));
		EXCCHECK( gs_EventFactory->RegisterEventFunction(gs_EventFactory,
														 "EVNT_GUI_CHANGE_VERSION",
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

	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "settings/LASER", &pparameter));	
	if(pparameter!=0)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext)
		{
			if(0==strcmp(ptext, "TRUE"))
				gs_Automat.LASER_active = TRUE;

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
		gs_pLasers = gs_Modules->Get(gs_Modules, MODULE_LASERS);
		gs_paxiss = gs_Modules->Get(gs_Modules, MODULE_AXIS);
		gs_pmotors = gs_Modules->Get(gs_Modules, MODULE_MOTOR);
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
	SPrinterPtr		printer;
    TaskHandle taskOut;
	StructAxisPtr pthread_data_Axis;

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

	/*--- Read All Analog Channel Simultaneously ----*/
	
	EXCCHECKIVIDAQ(DAQmxCreateTask("", &taskOut));

	EXCCHECKIVIDAQ(DAQmxCreateAIVoltageChan(taskOut, "Dev2/ai0:6", "",DAQmx_Val_Diff, -10, 10, DAQmx_Val_Volts, ""));	

	EXCCHECKIVIDAQ(DAQmxStartTask (taskOut));

	Run_DAQ = 1;
	
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, ThreadDAQ, (void*)taskOut, &threadDAQId);
	
	/*--- Start Printer  ----*/
	if (gs_Automat.PRINTER_active)
	{
		gs_pPrinters->GetPrinterByIndex(gs_pPrinters, 0, &printer);
		EXCCHECK(printer->Connect (printer));
	}
	
	/* Reset all actuators to don't move down */
	EXCCHECK( ResetStations ());
	
	/* Thread Automat */
	EXCCHECK( ThreadStart(Thread_Automat, NULL, &gs_hThread, &hEventStop)); 
	
#if 0    /* formerly excluded lines */
	/* Thread Security Z axis */
	if (gs_paxiss)
	{
	pthread_data_Axis = (StructAxisPtr)calloc(1, sizeof(StructAxis));
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 0, &(pthread_data_Axis->paxis_right_station_30)));
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 1, &(pthread_data_Axis->paxis_left_station_30)));
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 2, &(pthread_data_Axis->paxis_right_station_40)));
	EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 3, &(pthread_data_Axis->paxis_left_station_40)));
	
	Run_Axis = 1;

	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, Thread_Axis, (void*)pthread_data_Axis, &thread_Id_Axis);
	}
#endif   /* formerly excluded lines */
	
	if(gs_Automat.master_active)
		gs_Automat.masterpart = TRUE;
	
	if(gs_Automat.box_active)
		gs_Automat.box = TRUE;

	gs_Automat.EmtyTable = FALSE;
		
	EXCCHECK( ProcessEventGuiProduction());
		
Error:	
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
 
					
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
   
	EXCCHECK( ThreadStop( &gs_hThread, &gs_EventHandle[EVNT_STOP_THREAD]));
	EXCCHECK( gs_Automat.pTestSeq->UnLoad(gs_Automat.pTestSeq));  
	EXCCHECK( testseq_delete(&gs_Automat.pTestSeq)); 
	if(gs_panel[AUTOMAT])
		EXCCHECKCVI( DiscardPanel(gs_panel[AUTOMAT]));

	DeleteCriticalSection( &_Lock);

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
