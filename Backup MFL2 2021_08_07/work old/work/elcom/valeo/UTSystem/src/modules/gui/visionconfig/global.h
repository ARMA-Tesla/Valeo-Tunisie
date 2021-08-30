#if !defined(__GLOBAL_H__)
#define __GLOBAL_H__

#include <windows.h>
#include <userint.h>
#include <camera/camera.h> 
#include <language/language.h> 

#define IMAQ_DISPLAY_IMAGE_INTERNAL() \
	{ \
		Image* pImage=gs_pIMAQ->GetImage(gs_pIMAQ, INTERNAL_IMAGE_NAME); \
		if(pImage) { \
			EXCCHECKIMAQ( imaqDisplayImage (pImage, IMAQ_WINDOW_NUMBER, FALSE)); } \
	}

#define IMAQ_DISPLAY_IMAGE_BEGIN() \
	{ \
		Image* pImage=gs_pIMAQ->GetImage(gs_pIMAQ, BEGIN_STEP_IMAGE_NAME); \
		if(pImage) { \
			EXCCHECKIMAQ( imaqDisplayImage (pImage, IMAQ_WINDOW_NUMBER, FALSE)); } \
	}

#define XML_NODE_ADD_NEW_ELEMENT(document, node, name, value) \
	{ \
		CAObjHandle pparameter=0, pparameter_old=0; \
		EXCCHECKMSXML( MSXML_IXMLDOMDocumentcreateElement (document, NULL, name, &pparameter)); \
		if(pparameter) { \
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value)); \
			EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (node, NULL, pparameter, &pparameter_old)); \
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter)); \
			pparameter = 0; \
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter_old)); \
			pparameter_old = 0; } \
	}

#define XML_NODE_GET_TEXT(node, name, fnc) \
	{ \
		CAObjHandle pparameter=0; \
		char* ptext=NULL; \
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (node, NULL, name, &pparameter)); \
		if(pparameter) { \
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext)); \
			if(ptext) { fnc; } \
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter)); \
			pparameter = 0; \
			CA_FreeMemory(ptext); \
			ptext=NULL; } \
	}

#define XML_NODE_SET_TEXT(node, name, value) \
	{ \
		CAObjHandle pparameter=0; \
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (node, NULL, name, &pparameter)); \
		if(pparameter) { \
			EXCCHECKMSXML( MSXML_IXMLDOMNodeSettext (pparameter, NULL, value)); \
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter)); \
			pparameter = 0; } \
	}

#define XML_TASK_SAVE_CHANGES() \
	{ \
		CAObjHandle ptaskold=0; \
 		if(pglobal->task) { \
			EXCCHECKMSXML( MSXML_IXMLDOMNodereplaceChild (pglobal->root, NULL, pglobal->task_clone, pglobal->task, &ptaskold)); \
			EXCCHECKCVI( CA_DiscardObjHandle( ptaskold)); \
			ptaskold = 0; } \
		else { \
			EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pglobal->root, NULL, pglobal->task_clone, &ptaskold)); \
			EXCCHECKCVI( CA_DiscardObjHandle( ptaskold)); \
			ptaskold = 0; } \
		EXCCHECKCVI( CA_DiscardObjHandle(pglobal->task_clone)); \
		pglobal->task_clone = 0; \
	}

#define XML_STEP_SAVE_CHANGES() \
	{ \
		CAObjHandle pstepold=0; \
		if(pglobal->step) { \
			EXCCHECKMSXML( MSXML_IXMLDOMNodereplaceChild (pglobal->task_clone, NULL, pglobal->step_clone, pglobal->step, &pstepold)); \
			EXCCHECKCVI( CA_DiscardObjHandle( pstepold)); \
			pstepold = 0; } \
		else { \
			EXCCHECKMSXML( MSXML_IXMLDOMNodeappendChild (pglobal->task_clone, NULL, pglobal->step_clone, &pstepold)); \
			EXCCHECKCVI( CA_DiscardObjHandle( pstepold)); \
			pstepold = 0; } \
		EXCCHECKCVI( CA_DiscardObjHandle(pglobal->step_clone)); \
		pglobal->step_clone = 0; \
	}

#define MAX_TASK_NAME		64
#define MAX_STEP_NAME		64

#define STEP_NEW			0
#define STEP_EDIT			1

#define IMAQ_WINDOW_NUMBER	1

#define CHECKEND( buf, ch) \
	if ( *buf != ch ) goto Error; else

#define BKTREE_RESULT_HEADER "root|Step^w115$result|Result^w115\n"

typedef struct _SGlobal
{
	char			PatternPath[MAX_PATHNAME_LEN+1];

	long int		HWpanel;

	Image*			pImage; /* displayed image */
	
	CAObjHandle     pxml; 
	CAObjHandle		root; /* visionconfig */   
	CAObjHandle		task; /* task */
	CAObjHandle		task_clone;
	CAObjHandle		step; /* step */  
	CAObjHandle		step_clone;
	CAObjHandle		bktree;
	CAObjHandle		bktreeGT; /* golden template result */
	CAObjHandle		bktreePM; /* pattern matching result */
	CAObjHandle		bktreeH; /* histogram result */
	CAObjHandle		bktreeFC; /* face check result */
	
} SGlobal, *SGlobalPtr;

SGlobalPtr GetGlobal( SGlobalPtr* pGlobal);

#endif /* __GLOBAL_H__ */
