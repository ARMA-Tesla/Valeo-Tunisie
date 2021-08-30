#include <bklib/BKLib2.h>
#include <utility.h>
#include <toolbox.h>
#include "dbs_global.h"
#include "dbs_product_param.h"							     

extern SGlobalPtr		global;  
extern int				gs_panel[]; 

static int32_t			gs_product_id;

static    char				boutton_1_libelle[32][20];
static	  int				boutton_1_valeur[20];
static    char				boutton_1_string[20];
static    int				boutton_1_value=0;

static    char				boutton_2_libelle[32][20];
static	  int				boutton_2_valeur[20];
static    char				boutton_2_string[20];
static    int				boutton_2_value=0;


static    char				boutton_3_libelle[32][20];
static	  int				boutton_3_valeur[20];
static    char				boutton_3_string[20];
static    int				boutton_3_value=0;

static    char				boutton_4_libelle[32][20];
static	  int				boutton_4_valeur[20];
static    char				boutton_4_string[20];
static    int				boutton_4_value=0;

static    char				boutton_5_libelle[32][20];
static	  int				boutton_5_valeur[20];
static    char				boutton_5_string[20];
static    int				boutton_5_value=0;

static    char				boutton_6_libelle[32][20];
static	  int				boutton_6_valeur[20];
static    char				boutton_6_string[20];
static    int				boutton_67_value=0;

static    int   NumButton_1,
	            NumButton_2,
				NumButton_3,
				NumButton_4,
				NumButton_5,
				NumButton_6;

static int CVICALLBACK OnExitEdit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2);
static int CVICALLBACK OnExitNew (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2);
static int CVICALLBACK OnExitEditInsert (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2);

//static void CVICALLBACK LoadCameraConfig(); 

STestSequencePtr pTestSeq=NULL;


#undef __FUNC__
#define __FUNC__ "LoadCameraConfig"
SElExceptionPtr LoadCameraConfig()
{
	int32_t			error = 0;
	SElExceptionPtr	pexception = NULL;
	HRESULT         hr = 0;
	CAObjHandle     pxml  = 0;
	CAObjHandle     pnode2 = 0;
	CAObjHandle     pparameter = 0;
	CAObjHandle     pparameter2 = 0;
	CAObjHandle		pnode2old = 0;
	CAObjHandle		xml_configuration = 0;
	CAObjHandle		document = 0;
//	const char*     pfile_xml = (fileXml) ? fileXml : "test_form.xml";
	VBOOL           is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL;
	char*           ptext2 = NULL;
	char         	path[256];
	int            	i = 0;
	
//	printf("Hello world load camera\n\r"); 
	
	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &document));
	EXCCHECKCOM( CA_VariantSetCString(&pfile_name_var, "C:\\FlexiTester.OK\\modules\\camera.xml"));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (document, NULL, pfile_name_var, &is_xml_success));
	//	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success)); 
	
		EXCCHECKCOM( MSXML_IXMLDOMDocumentselectSingleNode(document, NULL,
													   "camera",
													   &xml_configuration));
	if (xml_configuration == 0)
		EXCTHROW( -1, "Missing Configuration Node");


/* lire les valeurs de Boutton 1  */
	i = 0;
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (xml_configuration, NULL, "BOUTTONS_1/BOUTTON_1", &pnode2));
	while(pnode2!=0 && i < 20)
		{
			
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@libelle", &pparameter)); 
			if(pparameter)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				sprintf (boutton_1_libelle[i], ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@valeur", &pparameter));
			if(pparameter)
				{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				boutton_1_valeur[i] = atoi (ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				}
			}
		
			pnode2old = pnode2;
			MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2); 
			CA_DiscardObjHandle(pnode2old);
			pnode2old = 0; 
			i++;
		}
	
		NumButton_1 = i;

/* lire les valeurs de Boutton 2  */
	i = 0;
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (xml_configuration, NULL, "BOUTTONS_2/BOUTTON_2", &pnode2));
	while(pnode2!=0 && i < 20)
		{
			
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@libelle", &pparameter)); 
			if(pparameter)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				sprintf (boutton_2_libelle[i], ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@valeur", &pparameter));
			if(pparameter)
				{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				boutton_2_valeur[i] = atoi (ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				}
			}
		
			pnode2old = pnode2;
			MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2); 
			CA_DiscardObjHandle(pnode2old);
			pnode2old = 0; 
			i++;
		}
	
		NumButton_2 = i;
/* lire les valeurs de Boutton 3  */
	i = 0;
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (xml_configuration, NULL, "BOUTTONS_3/BOUTTON_3", &pnode2));
	while(pnode2!=0 && i < 20)
		{
			
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@libelle", &pparameter)); 
			if(pparameter)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				sprintf (boutton_3_libelle[i], ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@valeur", &pparameter));
			if(pparameter)
				{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				boutton_3_valeur[i] = atoi (ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				}
			}
		
			pnode2old = pnode2;
			MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2); 
			CA_DiscardObjHandle(pnode2old);
			pnode2old = 0; 
			i++;
		}
	
		NumButton_3 = i;
/* lire les valeurs de Boutton 4  */
	i = 0;
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (xml_configuration, NULL, "BOUTTONS_4/BOUTTON_4", &pnode2));
	while(pnode2!=0 && i < 20)
		{
			
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@libelle", &pparameter)); 
			if(pparameter)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				sprintf (boutton_4_libelle[i], ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@valeur", &pparameter));
			if(pparameter)
				{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				boutton_4_valeur[i] = atoi (ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				}
			}
		
			pnode2old = pnode2;
			MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2); 
			CA_DiscardObjHandle(pnode2old);
			pnode2old = 0; 
			i++;
		}
	
		NumButton_4 = i;
/* lire les valeurs de Boutton 5  */
	i = 0;
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (xml_configuration, NULL, "BOUTTONS_5/BOUTTON_5", &pnode2));
	while(pnode2!=0 && i < 20)
		{
			
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@libelle", &pparameter)); 
			if(pparameter)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				sprintf (boutton_5_libelle[i], ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@valeur", &pparameter));
			if(pparameter)
				{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				boutton_5_valeur[i] = atoi (ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				}
			}
		
			pnode2old = pnode2;
			MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2); 
			CA_DiscardObjHandle(pnode2old);
			pnode2old = 0; 
			i++;
		}
	
		NumButton_5 = i;
/* lire les valeurs de Boutton 6  */
	i = 0;
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (xml_configuration, NULL, "BOUTTONS_6/BOUTTON_6", &pnode2));
	while(pnode2!=0 && i < 20)
		{
			
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@libelle", &pparameter)); 
			if(pparameter)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				sprintf (boutton_6_libelle[i], ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode2, NULL, "@valeur", &pparameter));
			if(pparameter)
				{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
	
				boutton_6_valeur[i] = atoi (ptext);
				CA_DiscardObjHandle(pparameter);
				pparameter = 0;
				CA_FreeMemory(ptext);
				ptext=NULL;
				}
			}
		
			pnode2old = pnode2;
			MSXML_IXMLDOMNodeGetnextSibling (pnode2, NULL, &pnode2); 
			CA_DiscardObjHandle(pnode2old);
			pnode2old = 0; 
			i++;
		}
	
		NumButton_6 = i;
Error:
	CA_VariantClear(&pfile_name_var);
	if (ptext) CA_FreeMemory(ptext);
	if (pparameter) CA_DiscardObjHandle(pparameter);
	if (pnode2) CA_DiscardObjHandle(pnode2);
	if (pxml) CA_DiscardObjHandle(pxml);
//	if (pexception) ERROR_SHOW();
	EXCRETHROW( pexception);
	return 0;
}


/*---------------------------------------------------------------------------*/ 
#undef __FUNC__
#define __FUNC__ "{Database Plugin}::OnVtype"
int  CVICALLBACK OnVtype(int panel, int control, int event, 
   void *callbackData, int eventData1, int eventData2)
{
	SElExceptionPtr pexception =  NULL;  
	int32_t      	error = 0;
	int32_t			vtype = 0;

	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_VTYPE, &vtype));
			EXCCHECKCVI( SetPanelAttribute(gs_panel[PARAM], ATTR_HEIGHT, (vtype==E_DVT_TEXT)? PARAMETER_DIALOG_HIGH_WITH_TEXT:PARAMETER_DIALOG_HIGH));
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "EditProductParamDialog"
SElExceptionPtr EditProductParamDialog(SProductParameter parameter)
{
	int32_t		error = 0;
	SElExceptionPtr pexception = NULL;
	int32_t		top = 100, left = 30; 
	char*		path;
	int i=0;
	

	/* check editable parameter */
	if( 0 == strcmp(parameter.name, "@none") )
	{	
		MessageBox((HWND)hWnd, _("Not editable cell"), _("Warning"), MB_OK|MB_ICONWARNING);
		goto Error;
	}
	if( 0 == strcmp(parameter.value,"@new") )
	{	
		/* clear name */
		EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_VALUE, "")); 
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_EXIT_OK, ATTR_CALLBACK_FUNCTION_POINTER, OnExitEditInsert));
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_EXIT_CANCEL, ATTR_CALLBACK_FUNCTION_POINTER, OnExitEditInsert));
		
		EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_TESTER, 0)); 
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PARAM], PARAM_TESTER, ATTR_VISIBLE, FALSE));
	}
	else
	{
		EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_VALUE, parameter.value)); 
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_EXIT_OK, ATTR_CALLBACK_FUNCTION_POINTER, OnExitEdit));
		EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_EXIT_CANCEL, ATTR_CALLBACK_FUNCTION_POINTER, OnExitEdit));

		EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_TESTER, (parameter.tester_id!=0))); 
		EXCCHECKCVI( SetCtrlAttribute(gs_panel[PARAM], PARAM_TESTER, ATTR_VISIBLE, TRUE));
	}
	
	gs_product_id = parameter.product_id;
	EXCCHECK( PPRODUCT->ProductGetIdName(PPRODUCT, parameter.product_id, &path) );
	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_NB, path));

	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_NAME, parameter.name));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_DESCRIPT, GET_STRING(parameter.description)));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PARAM], ATTR_TITLE, _("Edit Current Parameter")));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_NB, ATTR_LABEL_TEXT, _("Product")));
	
	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_VTYPE, parameter.vtype));  
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PARAM], PARAM_VTYPE, ATTR_VISIBLE, TRUE));
	EXCCHECKCVI( SetPanelAttribute(gs_panel[PARAM], ATTR_HEIGHT, (parameter.vtype==E_DVT_TEXT)? PARAMETER_DIALOG_HIGH_WITH_TEXT:PARAMETER_DIALOG_HIGH));
	EXCCHECKCVI( ResetTextBox(gs_panel[PARAM], PARAM_TEXTBOX, GET_STRING(parameter.text)));
	
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_NAME, ATTR_NO_EDIT_TEXT, 1));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_NAME, ATTR_TEXT_BGCOLOR, 0x00D4D0C8));
	
	EXCCHECKCVI( SetPanelPos (gs_panel[PARAM], top, left));
	EXCCHECKCVI( InstallPopup(gs_panel[PARAM]));
//	printf("Number Boutton = %d\n\r", pTestSeq->number_boutton);
#if 0    /* formerly excluded lines */
	printf("Hello world!\n\r");
	if(pTestSeq->number_boutton==7) 
//	if(!strcmp(parameter.ProductType,"X52_7") ) 
	{
#endif   /* formerly excluded lines */
	SetCtrlAttribute (gs_panel[PARAM], PARAM_BOUTTON_1, ATTR_DIMMED, 1);
	SetCtrlAttribute (gs_panel[PARAM], PARAM_BOUTTON_6, ATTR_DIMMED, 1);
#if 0    /* formerly excluded lines */
	printf("Hello world if\n\r"); 
	}
	else
	{
	SetCtrlAttribute (gs_panel[PARAM], PARAM_BOUTTON_1, ATTR_DIMMED, 0);
	SetCtrlAttribute (gs_panel[PARAM], PARAM_BOUTTON_6, ATTR_DIMMED, 0);
	printf("Hello world else\n\r"); 
	}
#endif   /* formerly excluded lines */
	
	DeleteListItem (gs_panel[PARAM], PARAM_BOUTTON_1, 0, -1 );
	DeleteListItem (gs_panel[PARAM], PARAM_BOUTTON_2, 0, -1 );
    DeleteListItem (gs_panel[PARAM], PARAM_BOUTTON_3, 0, -1 );
	DeleteListItem (gs_panel[PARAM], PARAM_BOUTTON_4, 0, -1 );
    DeleteListItem (gs_panel[PARAM], PARAM_BOUTTON_5, 0, -1 );
	DeleteListItem (gs_panel[PARAM], PARAM_BOUTTON_6, 0, -1 );
//	printf("Hello world end\n\r"); 
	
	LoadCameraConfig();
	
   for (i=0; i < 5; i++) 
	   InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_1, i, boutton_1_libelle[i], boutton_1_valeur[i]);

   for (i=0; i < 5; i++) 
	   InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_2, i, boutton_2_libelle[i], boutton_2_valeur[i]);
   
   for (i=0; i < 5; i++) 
	   InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_3, i, boutton_3_libelle[i], boutton_3_valeur[i]);
	
   for (i=0; i < 5; i++) 
	   InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_4, i, boutton_4_libelle[i], boutton_4_valeur[i]);
	 
   for (i=0; i < 5; i++) 
	   InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_5, i, boutton_5_libelle[i], boutton_5_valeur[i]);
	  
   for (i=0; i < 5; i++) 
	   InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_6, i, boutton_6_libelle[i], boutton_6_valeur[i]);
   
   
#if 0    /* formerly excluded lines */
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_1, &boutton_1_value));
   sprintf(boutton_1_string, "%d", boutton_1_value);
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_2, &boutton_2_value));
   sprintf(boutton_2_string, "%d", boutton_2_value);
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_3, &boutton_3_value));
   sprintf(boutton_3_string, "%d", boutton_3_value);
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_4, &boutton_4_value));
   sprintf(boutton_4_string, "%d", boutton_4_value);
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_5, &boutton_5_value));
   sprintf(boutton_5_string, "%d", boutton_5_value);
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_6, &boutton_6_value));
   sprintf(boutton_6_string, "%d", boutton_6_value);
   
   strcpy(parameter.value, boutton_1_string);
   strcat(parameter.value, boutton_2_string);
   strcat(parameter.value, boutton_3_string);
   strcat(parameter.value, boutton_4_string);
   strcat(parameter.value, boutton_5_string);
   strcat(parameter.value, boutton_6_string);
   
   EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_VALUE, parameter.value));
#endif   /* formerly excluded lines */
   
#if 0    /* formerly excluded lines */
	
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_1, i, "Boutton 1 valeur 1", 0);
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_1, i, "Boutton 1 valeur 2", 0);
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_1, i, "Boutton 1 valeur 3", 0);
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_1, i, "Boutton 1 valeur 4", 0);
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_2, i, "Boutton 2 valeur 1", 0); 
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_3, i, "Boutton 3 valeur 1", 0);
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_4, i, "Boutton 4 valeur 1", 0);
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_5, i, "Boutton 5 valeur 1", 0);
	InsertListItem (gs_panel[PARAM], PARAM_BOUTTON_6, i, "Boutton 6 valeur 1", 0); 
#endif   /* formerly excluded lines */

Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "InsertProductParamDialog"
SElExceptionPtr InsertProductParamDialog(SProductParameter parameter)
{
	int32_t		error = 0;
	SElExceptionPtr pexception = NULL;
	int32_t		top = 100, left = 30; 
	char*		path;  

	gs_product_id = parameter.product_id;
	EXCCHECK( PPRODUCT->ProductGetIdName(PPRODUCT, parameter.product_id, &path) );
	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_NB, path));

	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_NAME, ""));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_DESCRIPT, GET_STRING(parameter.description)));
	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_VALUE, parameter.value));
	
	EXCCHECKCVI( SetPanelAttribute (gs_panel[PARAM], ATTR_TITLE, _("New Parameter")));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_NB, ATTR_LABEL_TEXT, _("Product")));
	
	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_VTYPE, E_DVT_STRING)); 
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PARAM], PARAM_VTYPE, ATTR_VISIBLE, TRUE)); 
	EXCCHECKCVI( SetPanelAttribute(gs_panel[PARAM], ATTR_HEIGHT, PARAMETER_DIALOG_HIGH));
	EXCCHECKCVI( ResetTextBox(gs_panel[PARAM], PARAM_TEXTBOX, ""));

	EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_TESTER, 0)); 
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[PARAM], PARAM_TESTER, ATTR_VISIBLE, FALSE));
	
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_NAME, ATTR_NO_EDIT_TEXT, 0));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_NAME, ATTR_TEXT_BGCOLOR, VAL_WHITE));
	
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_EXIT_OK, ATTR_CALLBACK_FUNCTION_POINTER, OnExitNew));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[PARAM], PARAM_EXIT_CANCEL, ATTR_CALLBACK_FUNCTION_POINTER, OnExitNew));  
	
	EXCCHECKCVI( SetPanelPos (gs_panel[PARAM], top, left));
	EXCCHECKCVI( InstallPopup(gs_panel[PARAM]));

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DelProductParamDialog"
SElExceptionPtr DelProductParamDialog(	
		SProductParameter	parameter,
		const char* keyNode,
		const char* keyCol
)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	
	/* check editable parameter */
	if( 0 == strcmp(parameter.value, "@new") 
		|| 0 == strcmp(parameter.name, "@none") )
	{	
		MessageBox((HWND)hWnd, _("Not editable cell"), _("Warning"), MB_OK|MB_ICONWARNING);
		goto Error;
	}
	
	if(IDOK==MessageBox((HWND)hWnd, _("Delete selected parametr?\n - child parameters will be removed"), _("Delete Dialog"), MB_OKCANCEL|MB_ICONQUESTION))
	{
		char* 		nextCol;
		short int	AXvalue;
		
		parameter.user_id = global->activeUser.user_id;
		EXCCHECKCOM( BKLib2__DBKTreeGetColumnNext (global->treeHandle, NULL, "desc", &nextCol));
		
		/* reload */  
		if( 0==strcmp(nextCol, keyCol) )
		{
			EXCCHECK( PPARAM->ProductParamDel(PPARAM, parameter) );   
			EXCCHECK( TreeProductParamSet(PRODUCT_PARAM_RELOAD) );
		}
		else
		{		
			/* disable sorting algorithm */
			PPARAM->sort = FALSE;
			
			EXCCHECK( PPARAM->ProductParamDel(PPARAM, parameter) );   
			
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemText (global->treeHandle, NULL, keyNode, keyCol, "", &AXvalue));
			EXCCHECKCOM( BKLib2__DBKTreeSetNodeItemData (global->treeHandle, NULL, keyNode, keyCol, 0));
		}
		
		CA_FreeMemory( nextCol );
	}
	
Error:
	PPARAM->sort = TRUE;
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Database Plugin}::OnExitEdit"
static int CVICALLBACK OnExitEdit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	SProductParameter	parameter = {0};
	char			text[4096+1] = "";

	switch (event)
		{
		case EVENT_COMMIT:
			
			if(control == PARAM_EXIT_OK)
			{
				parameter.product_id = gs_product_id;
				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_NAME, parameter.name));
				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_VTYPE, &parameter.vtype)); 
#if 0    /* formerly excluded lines */
				if(parameter.vtype==E_DVT_TEXT)
				{
					EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_TEXTBOX, text));
					printf("Text = %s\n\r", text);
					printf("Size 1 = %d\n\r", strlen(text));
					PDBS_ALLOC(&parameter.text, strlen(text));
					printf("Size 2 = %d\n\r", strlen(text));  
					strcpy(PDBS_ALLOC(&parameter.text, strlen(text)), text);
					printf("Size 3 = %d\n\r", strlen(text));  
					
					CHECK_VALUE( text);
				}
				else
				{
#endif   /* formerly excluded lines */
					
#if 0    /* formerly excluded lines */
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_1, &boutton_1_value));
   sprintf(boutton_1_string, "%d", boutton_1_value);
#endif   /* formerly excluded lines */
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_2, &boutton_2_value));
   sprintf(boutton_2_string, "%d", boutton_2_value);
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_3, &boutton_3_value));
   sprintf(boutton_3_string, "%d", boutton_3_value);
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_4, &boutton_4_value));
   sprintf(boutton_4_string, "%d", boutton_4_value);
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_5, &boutton_5_value));
   sprintf(boutton_5_string, "%d", boutton_5_value);
#if 0    /* formerly excluded lines */
   
   EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_BOUTTON_6, &boutton_6_value));
   sprintf(boutton_6_string, "%d", boutton_6_value);
#endif   /* formerly excluded lines */
   
   strcpy(parameter.value, boutton_2_string);
   strcat(parameter.value, boutton_3_string);
   strcat(parameter.value, boutton_4_string);
   strcat(parameter.value, boutton_5_string);
#if 0    /* formerly excluded lines */
   strcat(parameter.value, boutton_5_string);
   strcat(parameter.value, boutton_6_string);
#endif   /* formerly excluded lines */
   
   EXCCHECKCVI( SetCtrlVal(gs_panel[PARAM], PARAM_VALUE, parameter.value));
					EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_VALUE, parameter.value));
					CHECK_VALUE( parameter.value);
					printf("Value = %s\n\r", parameter.value);
			//	}

				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_TESTER, &parameter.tester_id));

				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_DESCRIPT, PDBS_ALLOC(&parameter.description, DBS_RECORD_LENGHT_DESCRIPTION)));
				parameter.user_id = global->activeUser.user_id;
				
				EXCCHECK( PPARAM->ProductParamEdit(PPARAM, parameter));
				EXCCHECK( TreeProductParamSet(PRODUCT_PARAM_RELOAD));
			}
			EXCCHECKCVI( RemovePopup(gs_panel[PARAM]));  
			break;
		}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Database Plugin}::OnExitNew"
static int CVICALLBACK OnExitNew (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	SProductParameter	parameter = {0};
	char			text[4096+1] = "1111";

	switch (event)
		{
		case EVENT_COMMIT:
			
			if(control == PARAM_EXIT_OK)
			{
				parameter.product_id = gs_product_id; 
				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_NAME, parameter.name));
				CHECK_VALUE( parameter.name);

				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_VTYPE, &parameter.vtype)); 
#if 0    /* formerly excluded lines */
				if(parameter.vtype==E_DVT_TEXT)
				{
					EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_TEXTBOX, text));
					PDBS_ALLOC(&parameter.text, strlen(text));
					strcpy(PDBS_ALLOC(&parameter.text, strlen(text)), text);
					CHECK_VALUE( text);
				}
				else
				{
#endif   /* formerly excluded lines */
					EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_VALUE, parameter.value));
					CHECK_VALUE( parameter.value);
		//		}

				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_DESCRIPT, PDBS_ALLOC(&parameter.description, DBS_RECORD_LENGHT_DESCRIPTION)));
				parameter.user_id = global->activeUser.user_id;
				
				EXCCHECK( PPARAM->ProductParamInsert(PPARAM, parameter));
				EXCCHECK( TreeProductParamSet(PRODUCT_PARAM_RELOAD));
			}
			EXCCHECKCVI( RemovePopup(gs_panel[PARAM]));  
			break;
		}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "{Database Plugin}::OnExitEditInsert"
static int CVICALLBACK OnExitEditInsert (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
	SElExceptionPtr pexception = NULL;
	SProductParameter	parameter = {0};
	char			text[4096+1] = "1111";

	switch (event)
		{
		case EVENT_COMMIT:
			
			if(control == PARAM_EXIT_OK)
			{
				parameter.product_id = gs_product_id; 
				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_NAME, parameter.name));

				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_VTYPE, &parameter.vtype)); 
#if 0    /* formerly excluded lines */
				if(parameter.vtype==E_DVT_TEXT)
				{
					EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_TEXTBOX, text));
					PDBS_ALLOC(&parameter.text, strlen(text));
					strcpy(PDBS_ALLOC(&parameter.text, strlen(text)), text);
					CHECK_VALUE( text);
				}
				else
				{
#endif   /* formerly excluded lines */
					EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_VALUE, parameter.value));
					CHECK_VALUE( parameter.value);
			//	}

				EXCCHECKCVI( GetCtrlVal(gs_panel[PARAM], PARAM_DESCRIPT, PDBS_ALLOC(&parameter.description, DBS_RECORD_LENGHT_DESCRIPTION)));
				parameter.user_id = global->activeUser.user_id;  
				
				EXCCHECK( PPARAM->ProductParamInsert(PPARAM, parameter) );
				EXCCHECK( TreeProductParamSet(PRODUCT_PARAM_RELOAD) );
			}
			EXCCHECKCVI( RemovePopup(gs_panel[PARAM]));  
			break;
		}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

