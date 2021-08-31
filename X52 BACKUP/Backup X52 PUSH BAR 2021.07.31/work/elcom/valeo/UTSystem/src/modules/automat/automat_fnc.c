#include "msxmldom.h" 
#include <toolbox.h>
#include <utility.h>  
#include <multithread/multithread.h>
#include <guid.h>
#include <language/language.h>
#include "automat.h" 
#include "automat_fnc.h"
#include <NIDAQmx.h>
#include <formatio.h>
#include <time.h> 
#include <analysis.h>
#include <export.h>

// Constants
#define GOOD_PART 0
#define BAD_PART 1
#define BREAKDOWN 2
#define KANBAN 3
// Constants
#define DISCARD_HANDLE(handle) \
	if (handle) \
	{ \
		CA_DiscardObjHandle(handle); \
		handle = 0; \
	}

#define CA_FREE_MEMORY(mem) \
	if (mem) \
	{ \
		CA_FreeMemory(mem); \
		mem = NULL; \
	}

#define MAX_STEPS 			  	200
#define MAX_MESURES 		  	20
#define MAX_CHARACTERS_MASTER 	128

extern int					gs_panel[];
extern SElEventFactoryPtr	gs_EventFactory;

static HANDLE gs_hEventStop = NULL;
static DWORD WINAPI Thread_WaitAnimation(LPVOID param);
static SElExceptionPtr CloseAnimation(HANDLE* hThread, HANDLE* hStopEvent);



int 		Steps;
int			Mesures[MAX_STEPS];
int			Used_Step_Test[MAX_STEPS];
int			ID_Step_Test[MAX_STEPS];
int  		Failed_Step_Test[MAX_STEPS];
char		Desc_Step_Test[MAX_STEPS][MAX_CHARACTERS_MASTER];
int 		Failed_Mesure_Test[MAX_STEPS][MAX_MESURES];
char 		Desc_Mesure_Test[MAX_STEPS][MAX_MESURES][MAX_CHARACTERS_MASTER];

int 		Steps_Master;
int			Mesures_Master[MAX_STEPS];
int			Used_Step_Test_Master[MAX_STEPS];
int			ID_Step_Test_Master[MAX_STEPS];
int			Failed_Step_Test_Master[MAX_STEPS];
char		Desc_Step_Test_Master[MAX_STEPS][MAX_CHARACTERS_MASTER];
int			Failed_Mesure_Test_Master[MAX_STEPS][MAX_MESURES];
char 		Desc_Mesure_Test_Master[MAX_STEPS][MAX_MESURES][MAX_CHARACTERS_MASTER];

/*---------------------------------------------------------------------------*/
#define ERROR_SHOW() \
	if(pexception) { \
		if( pexception->GetErrorCode(pexception)<0) /* error */ { \
			SetCtrlVal( gs_panel[AUTOMAT], AUTOMAT_MSG, _("Load test sequence fault!")); \
			SetCtrlVal( gs_panel[AUTOMAT], AUTOMAT_ERROR, pexception->GetErrorMsgPtr(pexception)); } \
		else /* message */ { \
			SetCtrlVal( gs_panel[AUTOMAT], AUTOMAT_MSG, pexception->GetErrorMsgPtr(pexception)); } \
		if(Error) *Error = pexception->GetErrorCode(pexception); \
		EXCDELETE( pexception); } \
	else { \
		SetCtrlVal( gs_panel[AUTOMAT], AUTOMAT_MSG, ""); \
		SetCtrlVal( gs_panel[AUTOMAT], AUTOMAT_ERROR, ""); } 

int _FindString(char *Buffer, char *DebutTAG, char *FinTAG, char *StrVal , char *Message)
{
	int index1=0,
	    index2=0;
	
	if(DebutTAG != NULL)
		{
			if((index1=FindPattern (Buffer, 0, -1, DebutTAG, 0, 0))<0)
				{
					sprintf(Message,"none");  
						return -100;
					}
			}
	
	else if (DebutTAG == NULL) 
		{
				index1=0;
				 DebutTAG ="\0";
				}
	
	if((index2 = FindPattern (Buffer, index1, -1, FinTAG, 0, 0))<0)
	{
		sprintf(Message,"none");  
		return -200;
	}
	
	index2=index2-(index1+strlen(DebutTAG));
	index1=index1+strlen(DebutTAG);
	CopyBytes (StrVal,0,Buffer,index1,index2);
	StrVal[index2]='\0';
	
	return index2;
}

void convert_date ( char *timeStr)
{
	char ch1[100]="";
	char ch2[100]=""; 
	int index_space=0;
	
	if (timeStr[0]=='0')
		CopyString(timeStr,0,timeStr,1,-1);
	if( timeStr[2]=='0')
	{
		CopyString(ch1,0,timeStr,3,-1);
		 timeStr[2]=0;
		 strcat(timeStr,ch1);
	}
	else if( timeStr[3]=='0') 
	{
		CopyString(ch1,0,timeStr,4,-1);
		 timeStr[3]=0;
		 strcat(timeStr,ch1);
	}


	index_space= FindPattern(timeStr,0,-1," ",0,0);
	if(  timeStr[index_space+1]=='0')
		{
		CopyString(ch1,0,timeStr,index_space+2,-1);
		 timeStr[index_space+1]=0;
		 strcat(timeStr,ch1);
	}
	index_space= FindPattern(timeStr,0,-1,":",0,0);
	if(  timeStr[index_space+1]=='0')
		{
		CopyString(ch1,0,timeStr,index_space+2,-1);
		 timeStr[index_space+1]=0;
		 strcat(timeStr,ch1);
	}
}

/*---------------------------------------------------------------------------*/
void time_format(char * EventDateTime)
{	  
	struct tm *t1;
	char *buffer;
	time_t num_seconds=0;
	 char timeStr[250]="";
	num_seconds=time (NULL);
	t1 =localtime( &num_seconds);
	buffer=asctime(t1);
	strftime(timeStr, sizeof(timeStr), "%m/%d/%Y %I:%M %p", t1); //10/12/2019 02:19 PM  
	convert_date ( timeStr);
	sprintf(EventDateTime,timeStr);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "sendxml"
SElExceptionPtr sendxml(int type, char *SERial_nb, char* REF,char* CycleTime, char* code_defaut,char* IP)
{
		
	int32_t			   	error = 0;
	SElExceptionPtr    	pexception = NULL; 
	char Oxpathfile[512];
	HRESULT			hr = S_OK;
	VARIANT_BOOL	is_xml_success = 1;
	VARIANT			file,file1;
	CAObjHandle		document = 0;
	CAObjHandle		printer = 0;
	CAObjHandle		printer_list = 0;
	CAObjHandle		alias = 0;
	char*			WC = NULL;
	char*			printer_xml = NULL;
	char sNode[512];
	int nbr1=26,nbr2=1;
	int cnt=0,index,size ;
	double t1=0,t2=0,t3=0;
	char ch1[100]="";  
	char value[50]="";
	char SN[100]="";
	char SN_BAD[10]="";
	int result=-1000;
	char loh[100]="\\FLEXNET\\FLATFILES\\TODO\\";;
	
	
	GetDir (Oxpathfile);
	

	switch (type)
	{
		case GOOD_PART: 
			strcat(Oxpathfile,"\\templates\\good_part.xml");
			break;
		case BAD_PART:
			strcat(Oxpathfile,"\\templates\\bad_part.xml");
			break;
		case BREAKDOWN:
			strcat(Oxpathfile,"\\templates\\breakdown.xml");
			break;
		case KANBAN: 
			strcat(Oxpathfile,"\\templates\\good_part.xml");
			break;
	
	}
	
	
	EXCCHECKMSXML(MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &document));
	CA_VariantSetCString (&file, Oxpathfile);
	EXCCHECKMSXML(MSXML_IXMLDOMDocumentload(document, NULL, file, &is_xml_success));
	
	
	sprintf(sNode, "//FIInvocationSynchronousEvent");
	EXCCHECKMSXML(MSXML_IXMLDOMDocumentselectSingleNode(document, NULL, sNode, &printer_list));



	/*SerialNo*/
	//CopyString (SN, 0, TrameBarflow, 0, strlen(TrameBarflow)-2); 
	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","SerialNo");
	EXCCHECKMSXML(MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer));
	EXCCHECKMSXML(MSXML_IXMLDOMNodeSettext(printer, NULL, SERial_nb));


	/*ProductNo*/ 
	//CopyString (ch1, 0, ProductList[LoadedRef].Ref, 0, strlen(ProductList[LoadedRef].Ref)-2);
	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","ProductNo");
	EXCCHECKMSXML(MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer));
	EXCCHECKMSXML(MSXML_IXMLDOMNodeSettext(printer, NULL, REF));

   
	
	
	/*EventDateTime*/
	time_format(value);
	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","EventDateTime");
	EXCCHECKMSXML(MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer));
	EXCCHECKMSXML(MSXML_IXMLDOMNodeSettext(printer, NULL, value));
	
	///*WorkCenter*/
	//GetCtrlVal( panelHandle,PANEL_WorkCenter,value);
	//sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","WorkCenter");
	//MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer);
	//MSXML_IXMLDOMNodeSettext(printer, NULL, value);
	
	
	
	
	if ((type==GOOD_PART)||(type==BAD_PART))  //good or bad
	{
		  
		/*Quantity*/
		//GetCtrlVal( panelHandle,PANEL_Quantity,value);
		sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","Quantity");
		EXCCHECKMSXML(MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer));
		EXCCHECKMSXML(MSXML_IXMLDOMNodeSettext(printer, NULL, "1"));
		
	}
	
	/*CycleTime*/
	if (type==GOOD_PART)	// good part
	{
		sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","CycleTime");
		EXCCHECKMSXML(MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer));
		EXCCHECKMSXML(MSXML_IXMLDOMNodeSettext(printer, NULL, CycleTime));
	}
		
	/*ReasonCodeName*/
	if (type==BAD_PART)	//bad part
	{
		sprintf(ch1,"%s\0",code_defaut);
		sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","ReasonCodeName");

		EXCCHECKMSXML(MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer));
		EXCCHECKMSXML(MSXML_IXMLDOMNodeSettext(printer, NULL, ch1));
	}
	///*Equipement*/
	//if ((type==BAD_PART)||(type==BREAKDOWN))  // bad or breakdown
	//{
	//	GetCtrlVal( panelHandle,PANEL_Equipement,value);
	//	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","Equipement");
	//	MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer);
	//	MSXML_IXMLDOMNodeSettext(printer, NULL, value);
	//	
	//}
	//
	//if (type==BREAKDOWN)	// breakdown
	//{
	//	/*PLCErrorCode*/
	//	GetCtrlVal( panelHandle,PANEL_PLCErrorCode,value);
	//	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","PLCErrorCode");
	//	MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer);
	//	MSXML_IXMLDOMNodeSettext(printer, NULL, value);
	//	
	//	/*PLCErrorDesc*/
	//	GetCtrlVal( panelHandle,PANEL_PLCErrorDesc,value);
	//	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","PLCErrorDesc");
	//	MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer);
	//	MSXML_IXMLDOMNodeSettext(printer, NULL, value);
	//	
	//	/*State*/
	//	GetCtrlVal( panelHandle,PANEL_State,&cnt);
	//	sprintf (value,"%d",cnt);
	//	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","State");
	//	MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer);
	//	MSXML_IXMLDOMNodeSettext(printer, NULL, value);
	//}
	GetDir (Oxpathfile);

	// get_workcenter from xml template	  
	WC=calloc (100,sizeof(char) );
	if (type==BAD_PART)
	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","sWorkCenter");
	else if ((type==GOOD_PART)||   (type==KANBAN ))
	sprintf(sNode,"Parameters/Inputs[InputName='%s']/InputValue","WorkCenter");
	
	EXCCHECKMSXML(MSXML_IXMLDOMNodeselectSingleNode(printer_list, NULL,sNode, &printer));
	EXCCHECKMSXML(MSXML_IXMLDOMNodeGettext(printer, NULL, &WC));
	if (type==GOOD_PART)
		sprintf(ch1 ,"PD_%s_%s.xml",WC,SERial_nb);
	else  if (type==BAD_PART)
		sprintf(ch1 ,"BD_%s_%s.xml",WC,SERial_nb);
	else if    (type==KANBAN )
		sprintf(ch1 ,"CK_%s_%s.xml",WC,SERial_nb);
	
	  if(FileExists ("report_xml", 0) == 0)
			MakeDir( "report_xml");
	  
	strcat( Oxpathfile,"\\report_xml\\");
	strcat(Oxpathfile,   ch1);
	CA_VariantSetCString (&file, Oxpathfile);
	 
	EXCCHECKMSXML(MSXML_IXMLDOMDocumentsave( document,NULL,  file));
	
	//printf ("result \r\n");
	//sprintf (loh,"%s%s.xml",loh,SN);
	//result = send_ftp (IP/*Config.IP*/,ch1);
	//printf ("result = %d\r\n", result);
	//if (result < 0 )
	//	EXCTHROW(-1, "error send FTP");

Error:
	CA_FREE_MEMORY(WC);
	CA_FREE_MEMORY(printer_xml);
	DISCARD_HANDLE(printer);
	DISCARD_HANDLE(printer_list);
	DISCARD_HANDLE(document);
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Barfolow_MES"
SElExceptionPtr Barfolow_MES(struct _SSledge* me, CAObjHandle pxml, SElExceptionPtr ptest_exception)
{

	SElExceptionPtr    	pexception = NULL; 
	int32_t			   	error = 0;
	HRESULT         	hr = 0;
	//CAObjHandle     	pxml = 0; 
	CAObjHandle     	pnode = 0;
	char                default_name[512] = "none";
	char*				data_mes=NULL; 
	char				CycleTime[10]="0.0";
	char				REF_MES[10]="";
	char 				index_barflow;
	char 				codebarflow[30]="";
	char*				ptext=NULL; 
   	char 				Message[512]="";
	char 				sTrame[512]="";
	
			/* Ref VALEO */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode(pxml, NULL, "/testseq/product/name", &pnode));
			if(pnode)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &data_mes));
				if(data_mes && *data_mes)
				{

					memset(REF_MES, 0x00, 10);
					CopyString(REF_MES,0,data_mes,0,strlen(data_mes));

					strcat(REF_MES, "SE");
					
					CA_FreeMemory(data_mes);
					data_mes=NULL;
					pnode = 0;
				}
			} 
			
			/* duration */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/duration", &pnode));
			if(pnode)
			{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext( pnode, NULL, &data_mes));
					if(data_mes && *data_mes)
					{

						sprintf(CycleTime,"%s\0", data_mes);
						CA_FreeMemory(data_mes);
						data_mes=NULL;
						pnode = 0;
					}
				}
				
			/* Event Label Print OK */ 
			if (ptest_exception==NULL) /* piéce PASS*/
			{	
				
			   /* Indice barflow */
				switch(strlen(REF_MES))
				{

					case 10:
						index_barflow='K';
						break;
					case 9:
						index_barflow='P';
						break;
					case 8:
						index_barflow='N';
						break;
					case 7:
						index_barflow='J';
						break;
					case 6:
						index_barflow='F';
						break;
					default :
						index_barflow='O';
						break;
				}

				/* code barflow */
				sprintf(codebarflow,"%c%s\0",index_barflow , me->pTestParam->Barcode);

				/* sendxml barflow with FTP for PASS */
				sendxml(GOOD_PART , codebarflow, REF_MES, CycleTime, 0,"10.65.4.55");
			}
				
			else  /* piéce FAIL*/
			{
				/* Description de défaut */
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pxml, NULL, "/testseq/test[failed='1']/measure[failed='1']/config", &pnode));
				if(pnode!=0)
				{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pnode, NULL, &ptext));
					if(ptext)
					{
						if (_FindString(ptext,"[","]",sTrame, Message) < 0 )
							sprintf(default_name, Message);
						else 
							sprintf(default_name, sTrame);
			
						CA_FreeMemory(ptext);
						ptext=NULL;
					}

					EXCCHECKCVI( CA_DiscardObjHandle(pnode));
					pnode = 0;
				}
				
				/* sendxml barflow with FTP for FAIL */
				sendxml( BAD_PART , me->pTestParam->Barcode, REF_MES,"", default_name,"10.65.4.55");
			}
Error:
	DISCARD_HANDLE(pnode);
	EXCRETHROW(pexception);
}				
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Fill_CSV_Steps_Label"
SElExceptionPtr Fill_CSV_Steps_Label (CAObjHandle pxml, int pfile_csv, int Station)
{
	int32_t				error = 0;
	SElExceptionPtr    	pexception = NULL; 
	HRESULT         	hr = 0;
	//CAObjHandle     	pxml = 0; 
	CAObjHandle     	pnode = 0;
	VBOOL            	is_xml_success = 1;
	char         		path[256];
	char         		line[MAX_LENGTH_CSV];
	CAObjHandle     	pparameter = 0;
	char*           	ptext = NULL;
	char*           	Step_Description = NULL;
	int            		i = 0;
	int            		j = 0;
	int 				Test, SubTest;

	
		/* initialize line */
		memset(line, 0X00, MAX_LENGTH_CSV);
		sprintf (line, "");

		/* Read from XML test Steps */
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//testseq", &pnode));
	
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
			
				if (Station == Test/10 - 1) /*check if station to Run RnR */
					{
					sprintf(path, "test[@step='%d']/description", i); 
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
					if(pparameter)
						{
							EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &Step_Description))

							do
							{
							sprintf(path, "test[@step='%d']/measure[@step='%d']/default_name", i, ++j); 
							EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
								if(pparameter)
								{
									EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
									if(ptext && *ptext)
										{
											if (j==1)
											{
											/* add description in CSV file Line 1*/
											strcat(line, Step_Description);
											strcat(line, ";");
											CA_FreeMemory(Step_Description);
											Step_Description=NULL;
											}
											else 
											{
											/* add comma [;] */
											strcat(line, "              ;");
											CA_FreeMemory(ptext);
											ptext=NULL;
											}
										}
									EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
									pparameter = 0;
								}
								else
								{
									j=0;
									break;
								}
							
							}while(TRUE);
		
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
						}
					}
				CA_FreeMemory(ptext);
				ptext=NULL;
				}
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
			}
	
		else
		{
		//write line 
		WriteLine (pfile_csv, line, MAX_LENGTH_CSV-1);
		//CloseFile (pfile_csv);
		break;
		}
		
	}while(TRUE);

Error:
	if (ptext) CA_FreeMemory(ptext);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	//if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Fill_CSV_Mesures_Label"
SElExceptionPtr Fill_CSV_Mesures_Label (CAObjHandle pxml, int pfile_csv, int Station)
{
	int32_t				error = 0;
	SElExceptionPtr    	pexception = NULL; 
	HRESULT         	hr = 0;
	char         		line[MAX_LENGTH_CSV];
	char         		min[32];
	char         		max[32];
	CAObjHandle     	pnode = 0;
	VBOOL            	is_xml_success = 1;
	char         		path[256];
	CAObjHandle     	pparameter = 0;
	char*           	ptext = NULL;
	char*           	Step_Description = NULL;
	int            		i = 0;
	int            		j = 0;
	int 				Test, SubTest;

	
		/* initialize line */
		memset(line, 0X00, MAX_LENGTH_CSV);
		sprintf (line, "");
											
		/* Read from XML test Steps */
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//testseq", &pnode));
	
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
			
				if (Station == Test/10 - 1) /*check if station to Run RnR */
					{
					sprintf(path, "test[@step='%d']/description", i); 
					EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
					if(pparameter)
						{
							do
							{
								sprintf(path, "test[@step='%d']/measure[@step='%d']/default_name", i, ++j); 
								EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
								if(pparameter)
								{
									EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
									if(ptext && *ptext)
										{
											/* add Mesures Config Line 2 */
											strcat(line, ptext);
											CA_FreeMemory(ptext);
											ptext=NULL;
										}
									EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
									pparameter = 0;
								}
							
								sprintf(path, "test[@step='%d']/measure[@step='%d']/limit/value1", i, j);
								EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
								if(pparameter)
								{
									EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
									if(ptext && *ptext)
										{
											/* add min */
											sprintf (min,"[%s - ", ptext);
											strcat(line, min);
											CA_FreeMemory(ptext);
											ptext=NULL;
										}
									EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
									pparameter = 0;
								}	
							
								sprintf(path, "test[@step='%d']/measure[@step='%d']/limit/value2", i, j);
								EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
								if(pparameter)
								{
									EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
									if(ptext && *ptext)
										{
											/* add max */
											sprintf (max,"%s]", ptext);
											strcat(line, max);
											strcat(line, ";");
											CA_FreeMemory(ptext);
											ptext=NULL;
										}
									EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
									pparameter = 0;
								}	
							
								else
								{
									j=0;
									break;
								}
							
							}while(TRUE);
		
						EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
						pparameter = 0;
						}
					}
				CA_FreeMemory(ptext);
				ptext=NULL;
				}
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
			}
	
		else
		{
		/* write line */ 
		WriteLine (pfile_csv, line, MAX_LENGTH_CSV-1);
		//CloseFile (pfile_csv);
		break;
		}
		
	}while(TRUE);

Error:
	if (ptext) CA_FreeMemory(ptext);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	//if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadTestSequence"
SElExceptionPtr LoadTestSequence(
	const char* fileXml, 
	STestSequencePtr pTestSeq, 
	SObjListPtr pModules,
	bool_t		reset_panel,
	int32_t*	Error
)
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;
	CAObjHandle     pparameter2 = 0;
	const char*     pfile_xml = (fileXml) ? fileXml : "test_form.xml";
	VBOOL           is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL;
	char*           ptext2 = NULL;
	char         	path[256];
	int            	i = 0,picto, first_picto, last_picto, number_boutton; 
	STestParam		TestParam = {0};  
	SElEventDataPtr	pdata = NULL;
	char			testdll_path[MAX_PATHNAME_LEN] = "";
	char			picture_path[MAX_PATHNAME_LEN] = "";
	char			buffer[512];
        char			picto_number[10];
	char			picto_name[256];
	HANDLE			hThread = NULL;
	int32_t			jig_code = 0, 
					jig_required = 0;
	
	EXCCHECKCVI( ProcessSystemEvents());

	ERROR_SHOW();  

	EXCCHECK( eleventdata_new(&pdata));	
	//EXCCHECK( ThreadStart(Thread_WaitAnimation, NULL, &hThread, &gs_hEventStop));

	/* unload old test panel */
	EXCCHECK( pTestSeq->UnLoad(pTestSeq));

	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//testseq", &pnode));
		
	/* set panel info */
	if(gs_EventFactory && reset_panel)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='ProductPartNumber']/value", &pparameter));	
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		
			if(ptext)
			{
				sprintf(buffer, "PANEL_MSG_VERSION:%s", ptext);
				EXCCHECK( pdata->SetStringPtr(pdata, buffer));
				EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_GUI_INFO_SET", pdata));

				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='TesterName']/value", &pparameter));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='ProductName']/value", &pparameter2));
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(pparameter2)
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter2, NULL, &ptext2));
		
			sprintf(buffer, "PANEL_MSG_TITLE:%s - %s", (ptext)? ptext:"", (ptext2)? ptext2:"");
			EXCCHECK( pdata->SetStringPtr(pdata, buffer));
			EXCCHECK( gs_EventFactory->PulseEvent(gs_EventFactory, "EVNT_GUI_INFO_SET", pdata));
			
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
			CA_FreeMemory(ptext);
			ptext=NULL;

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter2));
			pparameter2 = 0;
			CA_FreeMemory(ptext2);
			ptext2=NULL;
		}
	}

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
		EXCTHROW(ERROR_AUTOMAT_NO_LIBRARY_NAME, _("Library name does not specified in xml file!"));
	}
	
	/* Product Part Number */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='PartNumberHousing']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			sprintf(pTestSeq->ProductPartNumber, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
/* Product Type */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='ProductType']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			sprintf(pTestSeq->ProductType, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
/* Product Customer Ref */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='PartNumberTopCover']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			sprintf(pTestSeq->ProductCustomerRef, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
/* Load Job Camera */ 	
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

	
//#if 0    /* formerly excluded lines */
	
		
	/* Product Picto_Button_1 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='Picto_Boutton_1']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			pTestSeq->Picto_Boutton_1 = strtoul(ptext, NULL, 10); 
		//	sprintf(pTestSeq->Picto_Button_CPE, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
	
	/* Product Picto_Button_2 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='Picto_Boutton_2']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			pTestSeq->Picto_Boutton_2 = strtoul(ptext, NULL, 10);
		//	sprintf(pTestSeq->Picto_Button_Start_Stop, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
	/* Product Picto_Button_3 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='Picto_Boutton_3']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			pTestSeq->Picto_Boutton_3 = strtoul(ptext, NULL, 10); 
		//	sprintf(pTestSeq->Picto_Button_ECO, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
	/* Product Picto_Button_4 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='Picto_Boutton_4']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			pTestSeq->Picto_Boutton_4 = strtoul(ptext, NULL, 10); 
		//	sprintf(pTestSeq->Picto_Button_ECO, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	/* Product Picto_Button_5 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='Picto_Boutton_5']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			pTestSeq->Picto_Boutton_5 = strtoul(ptext, NULL, 10); 
		//	sprintf(pTestSeq->Picto_Button_ECO, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	/* Product Picto_Button_6 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='Picto_Boutton_6']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			pTestSeq->Picto_Boutton_6 = strtoul(ptext, NULL, 10); 
		//	sprintf(pTestSeq->Picto_Button_ECO, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	/* Product Picto_Button_7 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='Picto_Boutton_7']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			pTestSeq->Picto_Boutton_7 = strtoul(ptext, NULL, 10); 
		//	sprintf(pTestSeq->Picto_Button_ECO, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
	/* Product Picto_Button_8 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='Picto_Boutton_8']/value", &pparameter));		
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			pTestSeq->Picto_Boutton_8 = strtoul(ptext, NULL, 10); 
		//	sprintf(pTestSeq->Picto_Button_ECO, ptext);
			CA_FreeMemory(ptext);
			ptext=NULL;
		}
		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	
//#endif   /* formerly excluded lines */
	
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
				EXCCHECK(pTestSeq->SetTestActive(pTestSeq, ptext, i));
			
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
	TestParam.pTicket = pxml;
	
	EXCCHECK( pTestSeq->UsageValidation(pTestSeq, &TestParam));  
	
	/* load test panel */  
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "global/parameter[name='ProductPicture']/value", &pparameter));
	if ( pparameter )
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
		if(ptext && *ptext)
		{
			EXCCHECKCVI( GetProjectDir(picture_path)); 
			strcat(picture_path, "\\images\\");
			strcat(picture_path, ptext);
			
			CA_FreeMemory(ptext);
			ptext=NULL;
		}		
		EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
		pparameter = 0;
	}
	EXCCHECK( pTestSeq->IniPanel(pTestSeq, picture_path, reset_panel));

Error:
	eleventdata_delete(&pdata);
	//CloseAnimation(&hThread, &gs_hEventStop);
	CA_VariantClear(&pfile_name_var);
	if (ptext) CA_FreeMemory(ptext);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);
	if (pexception) ERROR_SHOW();
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "CheckDirectory"
static SElExceptionPtr CheckDirectory(const char* directory)
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

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadMasterResults"
SElExceptionPtr ReadMasterResults(const char* pfile_xml)
{
	int32_t				error = 0;
	SElExceptionPtr    	pexception = NULL; 
	HRESULT         	hr = 0;
	CAObjHandle     	pxml = 0; 
	CAObjHandle     	pnode = 0;
	VARIANT         	pfile_name_var = CA_VariantEmpty();
	VBOOL            	is_xml_success = 1;
	char         		path[256];
	CAObjHandle     	pparameter = 0;
	char*           	ptext = NULL;
	int            		i = 0;
	int            		j = 0;
	
	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));

	
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//testseq", &pnode));
	do
	{
		sprintf(path, "test[@step='%d']/failed", ++i); 
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext && *ptext)
			{
				Failed_Step_Test_Master[i] = atoi(ptext);
	
				sprintf(path, "test[@step='%d']/id", i); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					 EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						ID_Step_Test_Master[i] = atoi(ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
				
								
				sprintf(path, "test[@step='%d']/used", i); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					 EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						Used_Step_Test_Master[i] = atoi(ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
			
				sprintf(path, "test[@step='%d']/description", i); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					 EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						sprintf (Desc_Step_Test_Master[i], "%s", ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
				
				do
				{
				sprintf(path, "test[@step='%d']/measure[@step='%d']/failed", i, ++j); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						Failed_Mesure_Test_Master[i][j] = atoi(ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
					
				sprintf(path, "test[@step='%d']/measure[@step='%d']/default_name", i, j); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						sprintf (Desc_Mesure_Test_Master[i][j], "%s", ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
			
				else
					{
					Mesures_Master[i] = j;//Number of measures for step i  
					j=0;
					break;
					}
		
				}while(TRUE);
				
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}
		else
		{
			Steps_Master = i;//number of test steps
			break;
		}
		
	}while(TRUE);
	

Error:
	CA_VariantClear(&pfile_name_var);
	if (ptext) CA_FreeMemory(ptext);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode) CA_DiscardObjHandle(pnode);
	if (pxml) CA_DiscardObjHandle(pxml);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ReadTestedResults"
SElExceptionPtr ReadTestedResults(	CAObjHandle pxml)
{
	int32_t				error = 0;
	SElExceptionPtr    	pexception = NULL; 
	HRESULT         	hr = 0;
	CAObjHandle     	pnode = 0;
	VARIANT         	pfile_name_var = CA_VariantEmpty();
	VBOOL            	is_xml_success = 1;
	char         		path[256];
	CAObjHandle     	pparameter = 0;
	char*           	ptext = NULL;
	int            		i = 0;
	int            		j = 0;
	
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode (pxml, NULL, "//testseq", &pnode));
	do
	{
		sprintf(path, "test[@step='%d']/failed", ++i); 
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext && *ptext)
			{
				Failed_Step_Test[i] = atoi(ptext);
	
				sprintf(path, "test[@step='%d']/id", i); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					 EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						ID_Step_Test[i] = atoi(ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
				
								
				sprintf(path, "test[@step='%d']/used", i); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					 EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						Used_Step_Test[i] = atoi(ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
			
				sprintf(path, "test[@step='%d']/description", i); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					 EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						sprintf (Desc_Step_Test[i], "%s", ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
				
				do
				{
				sprintf(path, "test[@step='%d']/measure[@step='%d']/failed", i, ++j); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						Failed_Mesure_Test[i][j] = atoi(ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
					
				sprintf(path, "test[@step='%d']/measure[@step='%d']/default_name", i, j); 
				EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, path, &pparameter));
				if(pparameter)
					{
					EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
					if(ptext && *ptext)
						{
						sprintf (Desc_Mesure_Test[i][j], "%s", ptext);
						CA_FreeMemory(ptext);
						ptext=NULL;
						}
					EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
					pparameter = 0;
					}
			
				else
					{
					Mesures[i] = j;//Number of measures for step i  
					j=0;
					break;
					}
		
				}while(TRUE);
				
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}
		else
		{
			Steps = i;//number of test steps
			break;
		}
		
	}while(TRUE);


Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "CompareXML"
SElExceptionPtr CompareXML(char *barcode_Master, CAObjHandle pxml, char* Error)
{
int32_t			   	error = 0;
SElExceptionPtr    	pexception = NULL; 
char 				pfile_xml[512];
int 				i,j,k;
char 				Error_Msg[256];
	
	/*Read Masters Results*/
	sprintf (pfile_xml, ".//Masters//%s.XML", barcode_Master);
	EXCCHECK(ReadMasterResults(pfile_xml));
			
	/* Read Tested Results*/
	EXCCHECK(ReadTestedResults(pxml));
		
	/*compare XML files */
	for(i=1; i<Steps; i++)//producion version
		{
		for(j=1; j<Steps_Master; j++) //
			{
			if (ID_Step_Test[i] == ID_Step_Test_Master[j])
				break;	
			}

			if (Failed_Step_Test_Master[j] != 2 && Used_Step_Test[i] == 1) 
			{
								
				if (Mesures[i] > 0)  /* il ya des mesures sous step i */
				{
					for (k=1; k<Mesures[i]; k++)
					{
						if (Failed_Mesure_Test_Master[j][k] != 2) 
						{
							
							if (Failed_Mesure_Test[i][k] != Failed_Mesure_Test_Master[j][k])
							{
							sprintf (Error_Msg,"Mesure : %s = %d '(Attendu : %s = %d)'", Desc_Mesure_Test[i][k], 
																						 Failed_Mesure_Test[i][k], 
																						 Desc_Mesure_Test_Master[j][k], 
																						 Failed_Mesure_Test_Master[j][k]);
							sprintf (Error, "%s", Error_Msg);
							EXCTHROW( -1, "Master Part NOK");
							}
						}
					}
				}
			
				else  /* pas des mesures sous step i */
				{
					
					if (Failed_Step_Test_Master[j] != 2) 
						{
						if (Failed_Step_Test[i] != Failed_Step_Test_Master[j])
							{
							sprintf (Error_Msg,"Step : %s = %d '(Attendu : %s = %d)'", Desc_Step_Test[i], 
																					   Failed_Step_Test[i], 
																				  	   Desc_Step_Test_Master[j],
																					   Failed_Step_Test_Master[j]);
							sprintf (Error, "%s", Error_Msg);
							EXCTHROW( -1, "Master Part NOK");
							}
						}
				}
			}
			
		}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "RunTestSequence"
SElExceptionPtr RunTestSequence(
	const char* fileFormXml, 
	struct _SSledge* me, 
	SAutomatPtr pAutomat, 
	SElEventFactoryPtr pEventFactory
)
{
	int32_t			   	error = 0;
	SElExceptionPtr    	pexception = NULL, 
						ptest_exception = NULL;
	EAutomatMode 		AutomatMode = pAutomat->Mode;
	HRESULT         	hr = 0;
	CAObjHandle     	pxml = 0; 
	CAObjHandle     	pnode = 0; 
	const char*     	pfile_form_xml = (fileFormXml)? fileFormXml : "test_form.xml";
	VBOOL            	is_xml_success = 1;
	VARIANT         	pfile_name_var = CA_VariantEmpty();
	char				project_dir[MAX_PATHNAME_LEN],
						result_dir[MAX_PATHNAME_LEN],
						fileName[MAX_PATHNAME_LEN],
						csv_dir[MAX_PATHNAME_LEN],
						sdate_time[256],
						buffer[2048] = "",
						AutomatModeStr[AUTOMAT_MODE_SIZE][32] = AUTOMAT_MODE_STR;
	SYSTEMTIME			st;
	bool_t				is_locked = FALSE;
	int                 serial;
	bool_t			    failed = FALSE;
	
	me->pTestSeq = calloc(1, sizeof(STestSequence));
	EXCCHECKALLOC( me->pTestSeq);
	me->pTestParam = calloc(1, sizeof(STestParam));
	EXCCHECKALLOC( me->pTestParam);
	
	memcpy(me->pTestSeq,pAutomat->pTestSeq,sizeof(STestSequence));
	memcpy(me->pTestParam,&pAutomat->TestParam,sizeof(STestParam));

	/* check destination directory */
	GetLocalTime(&st);
	EXCCHECKCVI(GetProjectDir(project_dir));
	switch(AutomatMode)
	{
		case AUTOMAT_PRODUCTION:
		case AUTOMAT_QUALITY_MASTER:
			sprintf(result_dir, PATH_PRODUCTION, project_dir, st.wYear, st.wMonth, st.wDay);
			break;
			
		case AUTOMAT_QUALITY_RETEST:
		case AUTOMAT_MASTERPART:
			sprintf(result_dir, PATH_QUALITY, project_dir, st.wYear, st.wMonth, st.wDay);
			break;
			
		case AUTOMAT_EXPERTISE_GO_MODE:
		case AUTOMAT_EXPERTISE_STEP_MODE:
			sprintf(result_dir, PATH_EXPERTISE, project_dir, st.wYear, st.wMonth, st.wDay);
			break;
				
		case AUTOMAT_EXPERTISE_SEQUENCE:
		case AUTOMAT_EXPERTISE_RNR_STUDY:
			sprintf(result_dir, PATH_RNR, project_dir, st.wYear, st.wMonth, st.wDay);
			break;
	}
	EXCCHECK( CheckDirectory(result_dir));
	/* open xml file */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, pfile_form_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));
	/* mode */
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/mode", &pnode));
	if(pnode)
	{
		char smode[AUTOMAT_MODE_SIZE][32] = AUTOMAT_MODE_DESCRIPTION_STR;

		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, smode[AutomatMode]));

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	/* date_time */
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/date_time", &pnode));
	if(pnode)
	{
		sprintf(sdate_time, DATE_TIME_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);      
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, sdate_time));

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	
	/* Barcode TOP Cover*/
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/barcode_top_cover", &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, (me->barcode_top_cover)? me->barcode_top_cover:""));

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}
	/* Barcode Housing */
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/barcode_housing", &pnode));
	if(pnode)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, (me->barcode_housing)? me->barcode_housing:""));

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}

	/* if Mode R&R create head of file CSV from balise default_name in test_form.xml */
	if (AutomatMode == AUTOMAT_EXPERTISE_SEQUENCE)
	{
	sprintf(csv_dir, "%s\\%02d.%02d.%02d", result_dir, st.wHour, st.wMinute, st.wSecond);
	sprintf(me->pTestParam->csv_dir, "%s", csv_dir);
	EXCCHECK( CheckDirectory(csv_dir)); 
	sprintf(fileName, "%s\\STATION_%02d.csv", csv_dir, 10*(me->Station_RnR+1));
	
	me->pTestParam->pTicket_CSV = OpenFile (fileName, VAL_READ_WRITE, VAL_OPEN_AS_IS, VAL_ASCII);
	EXCCHECK(Fill_CSV_Steps_Label(pxml, me->pTestParam->pTicket_CSV, me->Station_RnR));
	EXCCHECK(Fill_CSV_Mesures_Label(pxml, me->pTestParam->pTicket_CSV, me->Station_RnR));
	me->pTestParam->Test_RnR = TRUE;
	memset(me->pTestParam->line_CSV, 0X00, MAX_LENGTH_CSV);
	}
	
	/* prepare test parameter data */
	me->pTestParam->pTicket = pxml;
	me->pTestParam->shift_id = pAutomat->shift_id;
	sprintf(me->pTestParam->barcode_top_cover, me->barcode_top_cover);
	sprintf(me->pTestParam->barcode_housing, me->barcode_housing);
	sprintf(me->pTestParam->Time, "%02d.%02d.%02d", st.wHour, st.wMinute, st.wSecond);
	sprintf(me->pTestParam->Date, "%04d.%02d.%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(me->pTestParam->TicketDirPath, result_dir);
	
	EXCCHECK( eleventdata_new(&(me->pData)));
	switch(AutomatMode)
	{
		case AUTOMAT_PRODUCTION:
		case AUTOMAT_QUALITY_RETEST:
			
			me->pTestSeq->LockTest = me->_Lock;
			me->pTestParam->fault_continue = FALSE;
			sprintf(me->pTestParam->ProductType, "%s", pAutomat->pTestSeq->ProductType);
			
			ptest_exception = me->pTestSeq->RunAll(me->pTestSeq, me->pTestParam, FALSE);
			
			/* Status Station 7 RUN */
			StatusStations[6] = FALSE;
		    Station_Status (6,2);
			me->pTestSeq->SetPanelStationStatus(me->pTestSeq, 6, 2);
			sprintf(me->pTestParam->Barcode, "%s\0", me->barcode);

			/* Event Label Print OK */ 
			if (ptest_exception==NULL)
			{	
			    if (pAutomat->PRINTER_active && pAutomat->PRINTER_AUTO_active)
			    {
				  JigPanelLabel(me, &error);  
			    }
				StatusStations[6] = TRUE;
	            
				/*wait table rotation from 70->80 */		
			    WaitRotation (INFINITE);
				
			    Station_Status (6,2);
			    me->pTestSeq->SetPanelStationStatus(me->pTestSeq, 6, 2);
	            
				StatusStations[7] = FALSE;
				/*	Read Barcode OK and save XML report with barcode */	
				if (pAutomat->PRINTER_active && pAutomat->PRINTER_AUTO_active)
		     	{
				  JigPanelReadBarcodeOK(me, &error);  
			    }
			
			   if (pAutomat->PRINTER_active && pAutomat->PRINTER_MAN_active)
			    {	
			      EXCCHECK(JigPanelLabel2(me, &error));  
			    }
		
			    sprintf(me->pTestParam->Barcode, "%s\0", me->barcode); 
			
			    /* Barcode save XML */
			   EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/barcode", &pnode));
			   if(pnode)
			   {
			    EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, (me->barcode)? me->barcode:"none"));
			    EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			    pnode = 0;
			   }
			   
			    /* send datas OK to Barflow MED */
			   Barfolow_MES (me, pxml, ptest_exception);
			   
			   SaveXmlResultFile(me->pTestParam, 0);
			}
		
			/* Get new serial number and save XML report with serial number */ 
			if (ptest_exception)
			{
			     EXCCHECK( GetNewSerial(&serial, &error));
			     sprintf(me->barcode, "%04d\0", serial);
			     sprintf(me->pTestParam->Barcode, "%s\0", me->barcode);
				 sprintf(me->pTestParam->Production_Serial, "%s\0", me->barcode);
			    
				 /* Barcode save XML */
			     EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/barcode", &pnode));
			     if(pnode)
			     {
			        EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, (me->barcode)? me->barcode:"none"));
			        EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			        pnode = 0;
			     }
				 /* send datas OK to Barflow MED */
			   	Barfolow_MES (me, pxml, ptest_exception);
			    
				SaveXmlResultFile(me->pTestParam, 1);
				 
			}
		
			/* path XML for database */ 
			sprintf(buffer, "%s;%s;%s", AutomatModeStr[AutomatMode], me->pTestParam->TicketFilePath, (ptest_exception==NULL)? "OK":"FAULT");
			EXCCHECK( me->pData->SetStringPtr(me->pData, buffer));

			/* Save failed status in datatbase and print Fail Label if Fail */
			if (pAutomat->PRINTER_active)
			{
			/* Printer Ready */
    		if (ptest_exception) Printer_Ready_Reset(me);
				EXCCHECK( pEventFactory->PulseEvent(pEventFactory, "EVNT_TEST_SEQUENCE_END", me->pData)); //Event Send label to printer if Test NOK
			}
		
		break;
			
			case AUTOMAT_MASTERPART:
			
			//printf("master part mode automat.fnc\r\n");
			me->pTestSeq->LockTest = me->_Lock;
			me->pTestParam->fault_continue = TRUE;
			
			
			if (me->Station_Master == 0)
				ptest_exception = me->pTestSeq->RunAll(me->pTestSeq, me->pTestParam, TRUE);
			else 
				ptest_exception = me->pTestSeq->RunStationMaster(me->pTestSeq, me->pTestParam, FALSE, me->Station_Master);  // NO TOUCH
			
			sprintf(me->pTestParam->Barcode, "%s\0", me->barcode_top_cover);	   
	//	    printf("me->barcode_top_cover=%s\r\n",me->barcode_top_cover);
			/*compare XML files */
			ptest_exception = CompareXML(me->barcode_master_Part, pxml, me->pTestParam->Name_Fault); 
	//		printf("me->barcode_master_Part=%s\r\n",me->barcode_master_Part);
			/* save XML report with barode */ 
			sprintf(me->pTestParam->Barcode, "%s-%s\0", me->name_master_Part ,me->barcode_top_cover);
		//	printf("ptest_exception=%d\r\n",ptest_exception);
			/* Barcode save XML */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/barcode", &pnode));
			if(pnode)
			{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, (me->barcode)? me->barcode:"none"));
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
			}
	
			if (ptest_exception==NULL)
				SaveXmlResultFile(me->pTestParam, 0);
			else if (ptest_exception)
				SaveXmlResultFile(me->pTestParam, 1);

			/* path XML for dtatabase */ 
			sprintf(buffer, "%s;%s;%s", AutomatModeStr[AutomatMode], me->pTestParam->TicketFilePath, (ptest_exception==NULL)? "OK":"FAULT");
			EXCCHECK( me->pData->SetStringPtr(me->pData, buffer));

			/* Printer Ready */
			Printer_Ready_Reset(me);
					
			/* Save failed status in datatbase and print Fail Label if Fail */ 
			EXCCHECK( pEventFactory->PulseEvent(pEventFactory, "EVNT_TEST_SEQUENCE_END", me->pData));
			
		break;

		
		case AUTOMAT_EXPERTISE_SEQUENCE:
			
			me->pTestSeq->LockTest = me->_Lock;
			me->pTestParam->fault_continue = FALSE;
			
			ptest_exception = me->pTestSeq->RunStation(me->pTestSeq, me->pTestParam, FALSE, me->Station_RnR, me->Repeat_RnR);
 
			StatusStations[7] = FALSE;
			sprintf(me->barcode, "RnR%02d_%02d_%02d", st.wHour, st.wMinute, st.wSecond);
			
			/* Event Label Print OK and save XML report with barode */ 
			if (ptest_exception==NULL )
			{
			sprintf(me->pTestParam->Barcode, "%s\0", me->barcode);
			
			/* Barcodes save XML */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/barcode", &pnode));
			if(pnode)
			{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, (me->barcode)? me->barcode:"no_barcode"));
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
			}
			SaveXmlResultFile(me->pTestParam, 0);
			}
		
			/* Get new serial number and save XML report with serial number */ 
			if (ptest_exception)
			{
			sprintf(me->pTestParam->Barcode, "%s\0", me->barcode);

			/* Barcode save XML */
			EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectSingleNode( pxml, NULL, "/testseq/product/barcode", &pnode));
			if(pnode)
			{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext( pnode, NULL, (me->barcode)? me->barcode:"no_barcode"));
			EXCCHECKCVI( CA_DiscardObjHandle(pnode));
			pnode = 0;
			}
			SaveXmlResultFile(me->pTestParam, 1);
			}
			
			/* path XML for dtatabase */ 
			sprintf(buffer, "%s;%s;%s", AutomatModeStr[AutomatMode], me->pTestParam->TicketFilePath, (ptest_exception==NULL)? "OK":"FAULT");
			EXCCHECK( me->pData->SetStringPtr(me->pData, buffer));

			/* Save good status in datatbase and print Fail Label if Fail */ 
	
			if (ptest_exception==NULL)
			{
				EXCCHECK( pEventFactory->PulseEvent(pEventFactory, "EVNT_TEST_SEQUENCE_END", me->pData)); 
			
			}
			
		break;
	}	

	EXCCHECK(ptest_exception);
	
	
	
Error:
	if (pexception)
	{
	sprintf(me->pTestParam->Barcode, "%s\0", "none");
	SaveXmlResultFile(me->pTestParam, 1);
	}
	if( is_locked) LeaveCriticalSection( &(me->_Lock));
	eleventdata_delete(&(me->pData));
	CA_VariantClear(&pfile_name_var);
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::StepTestSequence"
SElExceptionPtr StepTestSequence(
	const char* fileXml, 
	SAutomatPtr pAutomat, 
	SElEventFactoryPtr pEventFactory, 
	int step_mode,
	bool_t* is_active,
	bool_t* last_step
)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;
	STestSequencePtr 	pTestSeq = pAutomat->pTestSeq;
	bool_t				b_last_step = FALSE; 
	SElEventDataPtr 	pData = NULL;  
	char				buffer[1024]= ""; 
	
	if(pTestSeq && pTestSeq->StepSequenceActive(pTestSeq))
	{
		if(is_active) *is_active = TRUE;
		if(last_step) *last_step = FALSE;

		switch(step_mode)
		{
			case STEP_SEQUENCE_REPEAT:
				EXCCHECK( pTestSeq->StepRepeat(pTestSeq));   
				break;
			case STEP_SEQUENCE_NEXT:
				EXCCHECK( pTestSeq->StepNext(pTestSeq, &b_last_step));
				if(last_step) *last_step = b_last_step;
				if(b_last_step)
				{
					if(pEventFactory)
					{
						sprintf(buffer, "EXPERTISE_STEP_MODE;%s;", fileXml); 
						EXCCHECK( eleventdata_new(&pData));
						EXCCHECK( pData->SetStringPtr(pData, buffer));
						EXCCHECK( pEventFactory->PulseEvent(pEventFactory, "EVNT_TEST_SEQUENCE_END", pData));
					}
				}
				break;		
			case STEP_SEQUENCE_BACK:
				EXCCHECK( pTestSeq->StepBack(pTestSeq));   
				break;
			case STEP_SEQUENCE_CANCEL:
				EXCCHECK( pTestSeq->StepStop(pTestSeq)); 
			
				if(pEventFactory)
				{
					sprintf(buffer, "EXPERTISE_STEP_MODE;%s;", fileXml); 
					EXCCHECK( eleventdata_new(&pData));
					EXCCHECK( pData->SetStringPtr(pData, buffer));
					EXCCHECK( pEventFactory->PulseEvent(pEventFactory, "EVNT_TEST_SEQUENCE_END", pData));
				}
				break;
		}
	}
	else
	{
		if(is_active) *is_active = FALSE;
		if(last_step) *last_step = FALSE;
	}
				
Error:
	eleventdata_delete(&pData);    
	EXCRETHROW( pexception);  
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::Thread_WaitAnimation"
static DWORD WINAPI Thread_WaitAnimation(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	int				i = 0; 
	
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[AUTOMAT], AUTOMAT_ANIMATION, ATTR_VISIBLE, TRUE));
	
	while(TRUE)
	{
		if (WaitForSingleObject(gs_hEventStop, 100) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}
		
		EXCCHECKCVI( SetCtrlIndex (gs_panel[AUTOMAT], AUTOMAT_ANIMATION, i++));
		if(i > 11) 
			i = 0;	
	};
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Automat Plugin}::CloseAnimation"
static SElExceptionPtr CloseAnimation(HANDLE* hThread, HANDLE* hStopEvent)
{
	SElExceptionPtr		pexception = NULL;
	int32_t				error = 0;

	EXCCHECK( ThreadStop( hThread, hStopEvent)); 

	EXCCHECKCVI( SetCtrlAttribute (gs_panel[AUTOMAT], AUTOMAT_ANIMATION, ATTR_VISIBLE, FALSE));

Error:   
	EXCRETHROW( pexception);  
}
/*---------------------------------------------------------------------------*/
