#if !defined(__VISIONCONFIG_FNC_H__)
#define __VISIONCONFIG_FNC_H__

#define IMAQ_DISPLAY_INI			1
#define IMAQ_DISPLAY_IMAGE			0

typedef struct _SPostDefData
{
	Image*		pImage;
}SPostDefData, *SPostDefDataPtr;

SElExceptionPtr XmlOpenDocument(bool_t new, const char* fileName);
SElExceptionPtr XmlSaveDocument(const char* fileName); 
SElExceptionPtr XmlLoadTaskTree(const char* selected_task);
SElExceptionPtr XmlDeleteTask(const char* selected_task);
SElExceptionPtr XmlLoadGlobal(void);

SElExceptionPtr	iniDisplayWindow(int32_t top, int32_t left);
SElExceptionPtr closeDisplayWindow(void);
SElExceptionPtr MyDisplayFce(Image* pImage, double fRate, double mean);

SElExceptionPtr CheckTaskName(int panel, int control);   
SElExceptionPtr	CheckStepName(int panel,int	control); 
SElExceptionPtr	CheckStepTemplate(int panel, Image* pImage);

SElExceptionPtr	CreateRegion(int window, const char* buffer);
SElExceptionPtr	FormatRegion(int window, int coIndex, char buffer[], uint32_t bufferSize);   

SElExceptionPtr DisplayAllBuffers( int panel, int control);

SElExceptionPtr ResultToBktree(CAObjHandle bktree, SELStringPtr result);
SElExceptionPtr ResultToView(SELStringPtr result);

#endif
