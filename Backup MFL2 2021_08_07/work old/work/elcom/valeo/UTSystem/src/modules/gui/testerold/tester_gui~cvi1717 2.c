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
#include <ansi_c.h>
#include "trans_language.h"

int Axis_Station_50_In_Home;
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

#define NUMBER_OF_PANELS 	1+1
#define NUMBER_OF_PANELS_STATION_1  1+1
#define NUMBER_OF_PANELS_STATION_2  1+1  
#define NUMBER_OF_PANELS_STATION_3  1+1  
#define NUMBER_OF_PANELS_STATION_4  1+1  
#define NUMBER_OF_PANELS_STATION_5  4+1  
#define NUMBER_OF_PANELS_STATION_6  4+1  
#define NUMBER_OF_PANELS_STATION_7  4+1  
#define NUMBER_OF_PANELS_STATION_8  1+1  

static int gs_status[] = {
	TESTER_LED_2,
	TESTER_LED_3,
	TESTER_LED_4,
	TESTER_LED_5,
	TESTER_LED_6,
	TESTER_LED_7,
	TESTER_LED_8,
	TESTER_LED_9};

static EMode				gs_mode;
static int					gs_panel[NUMBER_OF_PANELS+1];
static int					gs_panel_1[NUMBER_OF_PANELS_STATION_1+1];
static int					gs_panel_2[NUMBER_OF_PANELS_STATION_2+1]; 
static int					gs_panel_3[NUMBER_OF_PANELS_STATION_3+1]; 
static int					gs_panel_4[NUMBER_OF_PANELS_STATION_4+1]; 
static int					gs_panel_5[NUMBER_OF_PANELS_STATION_5+1]; 
static int					gs_panel_6[NUMBER_OF_PANELS_STATION_6+1]; 
static int					gs_panel_7[NUMBER_OF_PANELS_STATION_7+1]; 
static int					gs_panel_8[NUMBER_OF_PANELS_STATION_8+1]; 

SLanguagePtr 				gs_pLanguage;
static SElEventFactoryPtr   gs_EventFactory = NULL;  
static SObjListPtr          gs_Modules = NULL; 
static STesterPtr			gs_pTester = NULL;
static SDIOSensorsPtr		gs_pSensors = NULL;
static SaxissPtr			gs_paxiss = NULL;
static SProfilometersPtr    gs_pProfilometers = NULL; 
static CAObjHandle			gs_hTreeTester;
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
		
	switch (event)
		{
		case EVENT_COMMIT:

#if 0    /* formerly excluded lines */
			switch(control)
			{
				case STATION_1_CONNECT:
					pexception = gs_pJig->jig_ConnectToItem(gs_pJig, 0, TIMEOUT); 
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_CONNECT_LED,ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_CONNECT_LED,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, JIG_DISCONNECT_LED,ATTR_FRAME_COLOR, VAL_WHITE)); 
					break;
				case STATION_1_DISCONNECT:
					pexception = gs_pJig->jig_DisconnectFromItem(gs_pJig, 0, TIMEOUT); 
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_DISCONNECT_LED,ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_DISCONNECT_LED,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, JIG_CONNECT_LED,ATTR_FRAME_COLOR, VAL_WHITE));   
					break;
				case STATION_1_LOCK:
					pexception = gs_pJig->jig_LockItem(gs_pJig, 0, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_LOCK_LED,ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_LOCK_LED,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, JIG_UNLOCK_LED,ATTR_FRAME_COLOR, VAL_WHITE));  
					break;
				case STATION_1_UNLOCK:
					pexception = gs_pJig->jig_UnlockItem(gs_pJig, 0, TIMEOUT); 
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_UNLOCK_LED,ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_UNLOCK_LED,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, JIG_LOCK_LED,ATTR_FRAME_COLOR, VAL_WHITE));  
					break;
				case STATION_1_MARKING:
					pexception = gs_pJig->jig_ColdMarking(gs_pJig, 0, TIMEOUT); 
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_MARKING_LED,ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, JIG_MARKING_LED,ATTR_FRAME_COLOR, VAL_RED));
					}
				case STATION_1_POSITION:
					EXCCHECKCVI( GetCtrlVal(panel, JIG_POSITION_SET, &position));
					pexception = gs_pJig->jig_SetItemPosition(gs_pJig, 0, position, TIMEOUT); 
					if(pexception==NULL)
					{
						pexception = gs_pJig->jig_GetItemPosition(gs_pJig, 0, &position); 	
						if(pexception==NULL)
						{
							EXCCHECKCVI( SetCtrlVal(panel, JIG_POSITION_GET, position));
						}
						else
						{
							EXCDISPLAY( pexception);
							EXCCHECKCVI( SetCtrlVal(panel, JIG_POSITION_GET, -1)); 
						}
					}
					else
					{
						EXCDISPLAY( pexception);
						EXCCHECKCVI( SetCtrlVal(panel, JIG_POSITION_GET, -1)); 
					}
					break;
				case STATION_1_CODE:
					pexception = gs_pJig->jig_GetItemCode(gs_pJig, 0, 0, 512, buffer, TIMEOUT);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlVal(panel, JIG_CODE_STRING, buffer));
					}
					else
					{
						EXCDISPLAY( pexception); 
						EXCCHECKCVI( SetCtrlVal(panel, JIG_CODE_STRING, "#error"));
					}
					break;	
			}
#endif   /* formerly excluded lines */
			
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
		
	switch (event)
		{
		case EVENT_COMMIT:

			
			break;
		}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnFncStation3"
int CVICALLBACK OnFncStation3 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL;
	int32_t				error = 0;
	char				buffer[512];
	int					position = 0;
		
	switch (event)
		{
		case EVENT_COMMIT:

			
			break;
		}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnFncStation4"
int CVICALLBACK OnFncStation4 (int panel, int control, int event,
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

	switch (event)
		{
		case EVENT_COMMIT:
	
		switch(control)
			{
			case STATION_4_RODAGE_DOWN :
					pexception = gs_pTester->SetSensor(gs_pTester, "RODAGE_DOWN", TRUE);
					pexception = gs_pTester->SetSensor(gs_pTester, "RODAGE_UP", FALSE);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_RODAGE_DOWN_LED,ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_RODAGE_DOWN_LED,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_RODAGE_UP_LED,ATTR_FRAME_COLOR, VAL_WHITE));

			break;
			
			case STATION_4_RODAGE_UP :
					pexception = gs_pTester->SetSensor(gs_pTester, "RODAGE_DOWN", FALSE);
					pexception = gs_pTester->SetSensor(gs_pTester, "RODAGE_UP", TRUE);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_RODAGE_UP_LED, ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_RODAGE_UP_LED,ATTR_FRAME_COLOR, VAL_RED));
					}
					EXCCHECKCVI( SetCtrlAttribute(panel, STATION_4_RODAGE_DOWN_LED,ATTR_FRAME_COLOR, VAL_WHITE));

			break;
			
			case STATION_4_START_RODAGE :
				
					GetCtrlVal(panel, STATION_4_CYCLE, &cycle);
					GetCtrlVal(panel, STATION_4_TIME_ON, &timeOn);
					GetCtrlVal(panel, STATION_4_TIME_OFF, &timeOff);
					GetCtrlVal(panel, STATION_4_ENABLE_PUSH_1, &EnablePush1);
					GetCtrlVal(panel, STATION_4_ENABLE_PUSH_2, &EnablePush2);
					GetCtrlVal(panel, STATION_4_ENABLE_PUSH_3, &EnablePush3);
					GetCtrlVal(panel, STATION_4_ENABLE_PUSH_4, &EnablePush4);
					GetCtrlVal(panel, STATION_4_ENABLE_PUSH_5, &EnablePush5);
					GetCtrlVal(panel, STATION_4_ENABLE_PUSH_6, &EnablePush6);
					GetCtrlVal(panel, STATION_4_ENABLE_PUSH_7, &EnablePush7);
					GetCtrlVal(panel, STATION_4_ENABLE_PUSH_8, &EnablePush8);


				
					for (i=0; i<cycle; i++)
					{
					/* Down */
					if (EnablePush1)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_06", FALSE);
					if (EnablePush2)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_10", FALSE);
					if (EnablePush3)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_12", FALSE);
					if (EnablePush4)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_14", FALSE);
					if (EnablePush5)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_16", FALSE);
					if (EnablePush6)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_20", FALSE);
					if (EnablePush7)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_22", FALSE);
					if (EnablePush8)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_24", FALSE);
					
					Delay(0.1);//wait to stabilise the coil
					
					if (EnablePush1)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_07", TRUE);
					if (EnablePush2)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_11", TRUE);
					if (EnablePush3)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_13", TRUE);
					if (EnablePush4)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_15", TRUE);
					if (EnablePush5)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_17", TRUE);
					if (EnablePush6)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_21", TRUE);
					if (EnablePush7)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_23", TRUE);
					if (EnablePush8)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_25", TRUE);
					
				
					
					Delay(timeOn);//On Time 
					
					if (EnablePush1)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_07", FALSE);
					if (EnablePush2)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_11", FALSE);
					if (EnablePush3)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_13", FALSE);
					if (EnablePush4)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_15", FALSE);
					if (EnablePush5)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_17", FALSE);
					if (EnablePush6)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_21", FALSE);
					if (EnablePush7)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_23", FALSE);
					if (EnablePush8)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_25", FALSE);
					

					Delay(0.1);//wait to stabilise the coil
					
					if (EnablePush1)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_06", TRUE);
					if (EnablePush2)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_10", TRUE);
					if (EnablePush3)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_12", TRUE);
					if (EnablePush4)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_14", TRUE);
					if (EnablePush5)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_16", TRUE);
					if (EnablePush6)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_20", TRUE);
					if (EnablePush7)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_22", TRUE);
					if (EnablePush8)
					pexception = gs_pSensors->SetState(gs_pSensors, "CPV_24", TRUE);
					
					Delay(timeOff);//On Time 
					}
					
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
		
	switch (event)
		{
		case EVENT_COMMIT:

			switch(control)
			{
				case TESTER_MASK_CLOSE:
					EXCCHECKCVI( SetCtrlAttribute(panel, TESTER_MASK_CLOSE_LED,ATTR_FRAME_COLOR, VAL_WHITE));
					ProcessSystemEvents ();
					pexception = gs_pTester->TableRotate(gs_pTester, 10000, TRUE);
					if(pexception==NULL) 
					{
						EXCCHECKCVI( SetCtrlAttribute(panel, TESTER_MASK_CLOSE_LED,ATTR_FRAME_COLOR, VAL_GREEN));
					}
					else
					{
						EXCDISPLAY( pexception);   
						EXCCHECKCVI( SetCtrlAttribute(panel, TESTER_MASK_CLOSE_LED,ATTR_FRAME_COLOR, VAL_RED));
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
}/*---------------------------------------------------------------------------*/
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
}/*---------------------------------------------------------------------------*/
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

	EXCCHECKCVI( DisplayPanel(gs_panel_5[IO]));
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
}/*---------------------------------------------------------------------------*/
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
}/*---------------------------------------------------------------------------*/
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
void CVICALLBACK PostDeferredHideStation5 (void *callbackData) { HidePanel(gs_panel_5[IO]);}
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
		EXCCHECK( ThreadStart( ThreadProcTester, gs_pInputTester, &gs_hThread[THREAD_TESTER], &gs_hEventStop[THREAD_TESTER]));
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
		EXCCHECKCVI( HidePanel(gs_panel_5[IO]));
		EXCCHECKCVI( HidePanel(gs_panel_5[PANEL]));
		EXCCHECKCVI( HidePanel(gs_panel_5[FORCE]));
		EXCCHECKCVI( HidePanel(gs_panel_5[GAP]));
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
		SetCtrlVal (gs_panel[TESTER], gs_status[i], Is_Ready);
		}
		Is_Ready = (int)status[8];
		SetCtrlVal (gs_panel[TESTER], TESTER_LED, Is_Ready);
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
		case MODE_STATION_7:
			EXCCHECK( ThreadStop(&gs_hThread[THREAD_STATION_7], &gs_hEventStop[THREAD_STATION_7]));
			EXCCHECKCVI( PostDeferredCall( PostDeferredHideStation7, NULL));   
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
		case MODE_STATION_7:
			if(gs_pInputStation7)
			{
				EXCCHECK( ThreadStart( ThreadProcStation7, gs_pInputStation7, &gs_hThread[THREAD_STATION_7], &gs_hEventStop[THREAD_STATION_7]));
			}
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayStation7, NULL)); 
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
	else if(0==strcmp(eventName, "EVNT_GUI_TESTER_STATION_7"))
	{
		EXCCHECK(ChangeMode(MODE_STATION_7));	
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
                                         "EVNT_GUI_TESTER_STATION_7",
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
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "tester_gui.uir", TESTER, TESTERlang, &gs_panel[TESTER])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_1_gui.uir", STATION_1, STATION_1_lang, &gs_panel_1[STATION_1])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_2_gui.uir", STATION_2, STATION_2_lang, &gs_panel_2[STATION_2])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_3_gui.uir", STATION_3, STATION_3_lang, &gs_panel_3[STATION_3])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_4_gui.uir", STATION_4, STATION_4_lang, &gs_panel_4[STATION_4])); 
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_5_gui.uir", ST_5_IO, ST_5_IO_lang, &gs_panel_5[IO])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_5_gui.uir", ST_5_PANEL, ST_5_PANEL_lang, &gs_panel_5[PANEL])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_5_gui.uir", ST_5_FORCE, ST_5_FORCE_lang, &gs_panel_5[FORCE])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_5_gui.uir", ST_5_GAP, ST_5_GAP_lang, &gs_panel_5[GAP])); 
																																	 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_6_gui.uir", STATION_6, STATION_6_lang, &gs_panel_6[STATION_6])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_7_gui.uir", STATION_7, STATION_7_lang, &gs_panel_7[STATION_7])); 
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "//Stations GUI//station_8_gui.uir", STATION_8, STATION_8_lang, &gs_panel_8[STATION_8])); 

	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "TesterPanel", ARG_INT32, &gs_panel[TESTER])); 
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_1_Panel", ARG_INT32, &gs_panel_1[STATION_1]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_2_Panel", ARG_INT32, &gs_panel_2[STATION_2]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_3_Panel", ARG_INT32, &gs_panel_3[STATION_3]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_4_Panel", ARG_INT32, &gs_panel_4[STATION_4]));
	
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "gs_panel_5_IO_Panel", ARG_INT32, &gs_panel_5[IO]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "gs_panel_5_Panel_Panel", ARG_INT32, &gs_panel_5[PANEL]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "gs_panel_5_Force_Panel", ARG_INT32, &gs_panel_5[FORCE]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "gs_panel_5_Gap_Panel", ARG_INT32, &gs_panel_5[GAP]));

	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_6_Panel", ARG_INT32, &gs_panel_6[STATION_6]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_7_Panel", ARG_INT32, &gs_panel_7[STATION_7]));
	EXCCHECK( a_pPluginArgs->SetArg(a_pPluginArgs, "Station_8_Panel", ARG_INT32, &gs_panel_8[STATION_8]));

   	
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel[TESTER], TESTER_BKTREE, &gs_hTreeTester) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_1[STATION_1], STATION_1_BKTREE, &gs_hTreeStation1) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_2[STATION_2], STATION_2_BKTREE, &gs_hTreeStation2) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_3[STATION_3], STATION_3_BKTREE, &gs_hTreeStation3) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_4[STATION_4], STATION_4_BKTREE, &gs_hTreeStation4) );
	EXCCHECKCVI( GetObjHandleFromActiveXCtrl (gs_panel_5[IO], ST_5_IO_BKTREE, &gs_hTreeStation5) );
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
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_4[STATION_4], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_4[STATION_4], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_4[STATION_4], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_4[STATION_4], ATTR_WIDTH, width));
	
	/* STATION 50 */
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[IO], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[IO], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[IO], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[IO], ATTR_WIDTH, width));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[PANEL], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[PANEL], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[PANEL], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[PANEL], ATTR_WIDTH, width));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[FORCE], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[FORCE], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[FORCE], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[FORCE], ATTR_WIDTH, width));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[GAP], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[GAP], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[GAP], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_5[GAP], ATTR_WIDTH, width));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel_6[STATION_6], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_6[STATION_6], ATTR_LEFT, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_6[STATION_6], ATTR_HEIGHT, height-78));
	EXCCHECKCVI( SetPanelAttribute (gs_panel_6[STATION_6], ATTR_WIDTH, width));
	
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
#define __FUNC__ "ChangeMode_St5"
static SElExceptionPtr ChangeMode_St5 (int32_t mode)
{
	SElExceptionPtr 	pexception = NULL;  
	int32_t				error = 0;
	static enum _EDisplay	last_mode;
	
	switch(last_mode)
	{
		case IO:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel2, &gs_panel_5[IO]));
			break;
		case PANEL:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel2, &gs_panel_5[PANEL]));   
			break;
		case FORCE:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel2, &gs_panel_5[FORCE]));   
			break;
		case GAP:
			EXCCHECKCVI( PostDeferredCall( PostDeferredHidePanel2, &gs_panel_5[GAP]));   
			break;
	}
	
	switch(mode)
	{
		case IO:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel2, &gs_panel_5[IO]));
			break;
		case PANEL:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel2, &gs_panel_5[PANEL]));   
			break;
		case FORCE:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel2, &gs_panel_5[FORCE]));   
			break;
		case GAP:
			EXCCHECKCVI( PostDeferredCall( PostDeferredDisplayPanel2, &gs_panel_5[GAP]));   
			break;
	}
	
	last_mode = mode;

Error:      
   EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/

int CVICALLBACK OnDisplay_St5 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr 	pexception = NULL; 
	int32_t				error = 0;
	int					mode;
	
	switch (event)
	{
		case EVENT_COMMIT:

			
			EXCCHECKCVI( GetCtrlVal(panel, control, &mode));
			EXCCHECK( ChangeMode_St5( mode));	
		
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
/*----------------------------------------------------------------------*/

int CVICALLBACK OnTestGap (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
SElExceptionPtr 	pexception = NULL; 
SProfilometerPtr	pProfilometer = NULL;
SaxisPtr			paxis = NULL;
MEASURE_DATA pMeasure[16];
static int process_started = 0;
char Value[32];


	switch (event)
	{
		case EVENT_COMMIT:

			if (process_started == 0)
			{
			SetCtrlAttribute (panel, ST_5_GAP_OUT_1_0, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlAttribute (panel, ST_5_GAP_OUT_1_1, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlAttribute (panel, ST_5_GAP_OUT_2_0, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlAttribute (panel, ST_5_GAP_OUT_2_1, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlAttribute (panel, ST_5_GAP_OUT_3_0, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlAttribute (panel, ST_5_GAP_OUT_3_1, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlAttribute (panel, ST_5_GAP_OUT_4_0, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlAttribute (panel, ST_5_GAP_OUT_4_1, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlAttribute (panel, ST_5_GAP_OUT_5_0, ATTR_TEXT_BGCOLOR, VAL_BLACK);
			SetCtrlVal(panel, ST_5_GAP_OUT_1_0, "");
			SetCtrlVal(panel, ST_5_GAP_OUT_1_1, "");
			SetCtrlVal(panel, ST_5_GAP_OUT_2_0, "");
			SetCtrlVal(panel, ST_5_GAP_OUT_2_1, "");
			SetCtrlVal(panel, ST_5_GAP_OUT_3_0, "");
			SetCtrlVal(panel, ST_5_GAP_OUT_3_1, "");
			SetCtrlVal(panel, ST_5_GAP_OUT_4_0, "");
			SetCtrlVal(panel, ST_5_GAP_OUT_4_1, "");
			SetCtrlVal(panel, ST_5_GAP_OUT_5_0, "");

			process_started = 1;
			EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 0, &paxis));
			EXCCHECK( gs_pProfilometers->GetProfilometerByIndex(gs_pProfilometers, 0, &pProfilometer));
				
			/*move X to target position 0 */
			EXCCHECK( paxis->MoveAxisPosition(paxis, AXIS_X, 0));
			Delay(2);
		
			/*move Y to target position 0 */
			EXCCHECK( paxis->MoveAxisPosition(paxis, AXIS_Y, 0));
			Delay(2);
			
			/*move Z to target position 0 */
			EXCCHECK( paxis->MoveAxisPosition(paxis, AXIS_Z, 0));
			Delay(2);
			
			/*Test Push 1 */
			EXCCHECK( pProfilometer->GetMeasurementValue(pProfilometer, pMeasure));

			/* Out 0 */
			if (pMeasure[0].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[0].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[0].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_1_0, Value);
					SetCtrlAttribute (panel, ST_5_GAP_OUT_1_0, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_1_0, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			/* Out 1 */
			if (pMeasure[1].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[1].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[1].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_1_1, Value);
					SetCtrlAttribute (panel, ST_5_GAP_OUT_1_1, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_1_1, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			
			/*Test Push 2 */
			EXCCHECK( paxis->MoveAxisPosition(paxis, AXIS_X, 1));
			Delay(2);
			EXCCHECK( pProfilometer->GetMeasurementValue(pProfilometer, pMeasure));
			
			/* Out 0 */
			if (pMeasure[0].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[0].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[0].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_2_0, Value);
					SetCtrlAttribute (panel, ST_5_GAP_OUT_2_0, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_2_0, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			/* Out 1 */
			if (pMeasure[1].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[1].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[1].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_2_1, Value);
					SetCtrlAttribute (panel,ST_5_GAP_OUT_2_1, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_2_1, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			
			/*Test Push 3 */
			EXCCHECK( paxis->MoveAxisPosition(paxis, AXIS_X, 2));
			Delay(2);
			EXCCHECK( pProfilometer->GetMeasurementValue(pProfilometer, pMeasure));
			
			/* Out 0 */
			if (pMeasure[0].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[0].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[0].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_3_0, Value);
					SetCtrlAttribute (panel, ST_5_GAP_OUT_3_0, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_3_0, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			/* Out 1 */
			if (pMeasure[1].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[1].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[1].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_3_1, Value);
					SetCtrlAttribute (panel,ST_5_GAP_OUT_3_1, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_3_1, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			/*Test Push 4 */
			EXCCHECK( paxis->MoveAxisPosition(paxis, AXIS_X, 3));
			Delay(2);
			EXCCHECK( pProfilometer->GetMeasurementValue(pProfilometer, pMeasure));
			
			/* Out 0 */
			if (pMeasure[0].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[0].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[0].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_4_0, Value);
					SetCtrlAttribute (panel, ST_5_GAP_OUT_4_0, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_4_0, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			/* Out 1 */
			if (pMeasure[1].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[1].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[1].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_4_1, Value);
					SetCtrlAttribute (panel,ST_5_GAP_OUT_4_1, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_4_1, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			
			/*Test Push 5 */
			EXCCHECK( paxis->MoveAxisPosition(paxis, AXIS_X, 4));
			Delay(2);
			EXCCHECK( pProfilometer->GetMeasurementValue(pProfilometer, pMeasure));
			
			/* Out 1 */
			if (pMeasure[1].byDataInfo == LJV7IF_MEASURE_DATA_INFO_VALID)
			{
				if (pMeasure[1].byJudge == LJV7IF_JUDGE_RESULT_GO)
				{
					sprintf (Value, "%0.2f mm",pMeasure[1].fValue);
					SetCtrlVal(panel, ST_5_GAP_OUT_5_0, Value);
					SetCtrlAttribute (panel, ST_5_GAP_OUT_5_0, ATTR_TEXT_BGCOLOR, VAL_GREEN);
					ProcessSystemEvents ();
					ProcessDrawEvents ();
				}

			}
			else
				SetCtrlAttribute (panel, ST_5_GAP_OUT_5_0, ATTR_TEXT_BGCOLOR, VAL_RED);
			
			
			/* return to position 0 end cycle */

			EXCCHECK( paxis->MoveAxisPosition(paxis, AXIS_X, 0));

			Delay(2); //to return POS 0
			
			ProcessSystemEvents ();
			process_started = 0;
			}
		break;
	}

Error:
	EXCDISPLAY( pexception);   
	return 0;
}

/*-----------------------------------------------------------------*/
int CVICALLBACK OnTestGapHome (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
SElExceptionPtr 	pexception = NULL; 
SProfilometerPtr	pProfilometer = NULL;
SaxisPtr			paxis = NULL;


	switch (event)
	{
		case EVENT_COMMIT:

			if (Axis_Station_50_In_Home == 0)
			{
			EXCCHECK( gs_paxiss->GetaxisByIndex(gs_paxiss, 0, &paxis));
			EXCCHECK( gs_pProfilometers->GetProfilometerByIndex(gs_pProfilometers, 0, &pProfilometer));

			EXCCHECK( paxis->EnableAxis(paxis, AXIS_X));
			EXCCHECK( paxis->EnableAxis(paxis, AXIS_Y));
			EXCCHECK( paxis->EnableAxis(paxis, AXIS_Z));
			ProcessSystemEvents ();

			EXCCHECK( paxis->HomeReturn(paxis, AXIS_Z));
			EXCCHECK( paxis->HomeReturn(paxis, AXIS_Z));
			Delay(2); //to return home

			EXCCHECK( paxis->HomeReturn(paxis, AXIS_Y));
			EXCCHECK( paxis->HomeReturn(paxis, AXIS_Y));
			Delay(4); //to return home
						
			EXCCHECK( paxis->HomeReturn(paxis, AXIS_X));
			EXCCHECK( paxis->HomeReturn(paxis, AXIS_X));
			Delay(6); //to return home
		
			Axis_Station_50_In_Home = 1;
			}
		break;
	}

Error:
	EXCDISPLAY( pexception);   
	return 0;
}
