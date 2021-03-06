#include <userint.h>
#include <msxmldom2.h>
#include <utility.h>
#include <eventfactory/EventFactory.h>
#include <stdio.h>
#include <language/language.h>
#include <ivibridge/ivibridge.h>
#include <can/can.h>
#include <multithread/multithread.h>
#include <guid.h>
#include "canGUI_plugin.h"
#include <ansi_c.h>
#include "canGUI.h"

static const char* HISTORY_INFO = \
"[08-01-15] KP\n"\
"  + Plug-in Created";

#pragma warning( push)
#pragma warning( error: 4013)
#pragma warning( disable: 4100)

#define NUMBER_OF_PANELS 	3
#define KWPPANEL			NUMBER_OF_PANELS-1
#define LOWPANEL			NUMBER_OF_PANELS

#define TIMEOUT_CAN			400
#define TIMEOUT_CAN_DEFAULT	2000

SLanguagePtr 				gs_pLanguage = NULL;
void*						gs_hWnd;
static int					gs_panel[NUMBER_OF_PANELS+1];
static SElEventFactoryPtr   gs_EventFactory = NULL;
static SObjListPtr          gs_Modules = NULL;
static SCANListPtr			gs_can = NULL;
static int32_t				gs_ClearEnabled = 0;
static int32_t				gs_oldCacheState = 0;
static PLogFuncPtr			gs_oldLogFunc = NULL;
static STimerPtr			gs_timer = NULL;
static bool_t				gs_ReadPause = FALSE;

static HANDLE				gs_hThread = NULL;
static HANDLE				gs_hEventStop = NULL;

static char					gs_include[512] = "0x6..";
static char					gs_exclude[512] = "";
static int32_t				gs_includeEnabled = 0;
static int32_t				gs_excludeEnabled = 0;

typedef struct _SCmd
{
	char		cmd[256];
	uint32_t	size;
	uint8_t		data[112];
} SCmd, *SCmdPtr;

static SCmd					gs_Cmd[32] = {0};

/* language support pattern */
static SLanguageItem	gs_canGUILang[] = {
   	P(CANGUI,                	_noop("CAN Communication")),
   	{0 , NULL, 0}
};

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Thread_CANReadLoop"
static DWORD WINAPI Thread_CANReadLoop(LPVOID param)
{
	SElExceptionPtr	pexception = NULL;
	uint32_t		count = 0,
					i = 0,
					ident = 0;
	const char*		channel = NULL;
	uint8_t			buffer[8];
	int32_t			size = 0;

#pragma warning( push)
#pragma warning( disable: 4127)
	while (TRUE)
#pragma warning( pop)
	{
		if (WaitForSingleObject(gs_hEventStop, 100) == WAIT_OBJECT_0)
		{
			ExitThread(THREAD_EXIT_CODE);
		}

		count = gs_can->_channelList->GetCount( gs_can->_channelList);
		for(i=0; i<count; i++)
		{
			size = sizeof(buffer);
			ident = READ_ANY;

			channel = gs_can->_channelList->GetNameOnIndex( gs_can->_channelList, i);

			if (WaitForSingleObject(gs_hEventStop, 10) == WAIT_OBJECT_0)
			{
				ExitThread(THREAD_EXIT_CODE);
			}
			
			if(gs_ReadPause)
				continue;

			pexception = gs_can->ReadLowLevel(gs_can, channel, &ident, &size, buffer);
			if (pexception &&
				pexception->GetErrorCode(pexception) != CAN_ERROR_TIMEOUT &&
				pexception->GetErrorCode(pexception) != CanErrFunctionTimeout)
			{
				EXCCHECK( pexception);
			}
			else
				EXCDBGSTR( pexception);
		}
	}

Error:
	EXCDISPLAY( pexception);
	ExitThread(0);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "IsInFilter"
int32_t IsInFilter( int32_t ident, char* filter)
{
	int32_t	start = 1,
			result = 0,
			cmp_ok = 0;
	char	ident_str[20] = "";
	char*	p2str = filter;
	char*	p2ident = NULL;

	while (*p2str != '\0')
	{
		if (start)
		{
			if (!strncmp(p2str, "0x", 2))
			{
				sprintf(ident_str, "%X", ident);
				p2str += 2;
			}
			else if (strncmp(p2str, "0x", 2))
			{
				sprintf(ident_str, "%d", ident);
			}
			
			start = 0;
			p2ident = ident_str;
			cmp_ok = 1;
			while (*p2str == '0') p2str++;
		}

		if (*p2str == '\0')
			break;
		
		if (*p2str == ';')
		{
			if (cmp_ok && *p2ident == '\0')
			{
				result = 1;
				break;
			}

			start = 1;
		}
		else if (*p2str != '.')
		{
			if (*p2str != *p2ident)
			{
				cmp_ok = 0;
			}
		}
		
		p2str++;
		p2ident++;
	}

	if (p2ident && *p2ident == '\0' && cmp_ok)
		result = 1;

	return result;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadPredefinedCommands"
SElExceptionPtr LoadPredefinedCommands(const char* fileXml)
{
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	int32_t			error = 0;
	VBOOL           is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL,
					*pdata = NULL;
	const char*		pfile_xml = (fileXml) ? fileXml : "./maintenance/can_gui.xml";
	CAObjHandle     pxml = 0;
	CAObjHandle     plist = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;
	char			path[512];
	long			length;
	int32_t			i, j;

	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));

	sprintf(path, "//plugin[@id='%s']/KWPCommands/command", PLUGIN_CANGUI);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes (pxml, NULL, path, &plist));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));

	for(i=0; i<length; i++)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pnode));
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "name", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				strncpy(gs_Cmd[i].cmd, ptext, 255);
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "size", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				gs_Cmd[i].size = strtoul(ptext, NULL, 10);
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "data", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				j = 0;
				gs_Cmd[i].data[j++] = (uint8_t)strtoul(ptext, &pdata, 16);
				while(pdata && *pdata)
				{
					gs_Cmd[i].data[j++] = (uint8_t)strtoul(++pdata, &pdata, 16);
				}

				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}
	}

	for(i=0; i<length; i++)
	{
		EXCCHECKCVI( InsertListItem (gs_panel[KWPPANEL], KWPPANEL_CMD, -1, gs_Cmd[i].cmd, i));
	}
	
Error:
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadAsyncFrames"
SElExceptionPtr LoadAsyncFrames(const char* fileXml)
{
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	int32_t			error = 0;
	VBOOL           is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL,
					*pdata = NULL;
	const char*		pfile_xml = (fileXml) ? fileXml : "./maintenance/can_gui.xml";
	CAObjHandle     pxml = 0;
	CAObjHandle     plist = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;
	char			path[512];
	long			length;
	int32_t			i, j;
	uint32_t		uvalue;

	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));

	sprintf(path, "//plugin[@id='%s']/LowLevel/AsyncFrames/Frame", PLUGIN_CANGUI);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes (pxml, NULL, path, &plist));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));

	for(i=0; i<length; i++)
	{
		EXCCHECKCVI( InsertTableRows (gs_panel[LOWPANEL], LOWPANEL_LIST, -1, 1, VAL_USE_MASTER_CELL_TYPE));

		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pnode));
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "channel", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(1,i+1), ptext));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "identifier", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, NULL, 16);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(2,i+1), uvalue));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "TypeFrame", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, NULL, 10);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(3,i+1), uvalue));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "repeatTime", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, NULL, 10);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(4,i+1), uvalue));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "size", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, NULL, 10);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(5,i+1), uvalue));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "data", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, &pdata, 16);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(6,i+1), uvalue));
				j=7;
				while(pdata && *pdata)
				{
					uvalue = strtoul(++pdata, &pdata, 16);
					EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(j++,i+1), uvalue));
				}
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}

Error:
	if(pxml) CA_DiscardObjHandle(pxml);
	if(plist) CA_DiscardObjHandle(plist);
	if(pnode) CA_DiscardObjHandle(pnode);
	if(pparameter) CA_DiscardObjHandle(pparameter);
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "LoadMultiFrames"
SElExceptionPtr LoadMultiFrames(const char* fileXml)
{
	SElExceptionPtr pexception = NULL;
	HRESULT			hr;
	int32_t			error = 0;
	VBOOL           is_xml_success;
	VARIANT         pfile_name_var = CA_VariantEmpty();
	char*           ptext = NULL,
					*pdata = NULL;
	const char*		pfile_xml = (fileXml) ? fileXml : "./maintenance/can_gui.xml";
	CAObjHandle     pxml = 0;
	CAObjHandle     plist = 0;
	CAObjHandle     pnode = 0;
	CAObjHandle     pparameter = 0;
	char			path[512];
	long			length;
	int32_t			i, j;
	uint32_t		uvalue;

	/* configure test sequence from XML */
	EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument (NULL, 1, LOCALE_NEUTRAL, 0, &pxml));
	EXCCHECKCVI( CA_VariantSetCString(  &pfile_name_var, pfile_xml));
	EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload (pxml, NULL, pfile_name_var, &is_xml_success));

	sprintf(path, "//plugin[@id='%s']/LowLevel/MultiFrames/Frame", PLUGIN_CANGUI);
	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes (pxml, NULL, path, &plist));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListGetlength (plist, NULL, &length));
	EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset (plist, NULL));

	for(i=0; i<length; i++)
	{
		EXCCHECKCVI( InsertTableRows (gs_panel[LOWPANEL], LOWPANEL_LIST_2, -1, 1, VAL_USE_MASTER_CELL_TYPE));

		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode (plist, NULL, &pnode));
		
		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "channel", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(1,i+1), ptext));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}
			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "identifier", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, NULL, 16);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(2,i+1), uvalue));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "TypeFrame", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, NULL, 10);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(3,i+1), uvalue));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "size", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, NULL, 10);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(4,i+1), uvalue));
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode (pnode, NULL, "data", &pparameter));  
		if(pparameter)
		{
			EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext (pparameter, NULL, &ptext));
			if(ptext)
			{
				uvalue = strtoul(ptext, &pdata, 16);
				EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(5,i+1), uvalue));
				j=6;
				while(pdata && *pdata)
				{
					uvalue = strtoul(++pdata, &pdata, 16);
					EXCCHECKCVI( SetTableCellVal (gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(j++,i+1), uvalue));
				}
				CA_FreeMemory(ptext);
				ptext=NULL;
			}

			EXCCHECKCVI( CA_DiscardObjHandle(pparameter));
			pparameter = 0;
		}

		EXCCHECKCVI( CA_DiscardObjHandle(pnode));
		pnode = 0;
	}

Error:
	if(pxml) CA_DiscardObjHandle(pxml);
	if(plist) CA_DiscardObjHandle(plist);
	if(pnode) CA_DiscardObjHandle(pnode);
	if(pparameter) CA_DiscardObjHandle(pparameter);
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DisplayCommunication"
SElExceptionPtr DisplayCommunication(
	char*			channel,
	int32_t			type,
	int32_t			indentifier,
	uint8_t*		pData,
	uint32_t		dataSize,
	NCTYPE_ABS_TIME	timestamp
)
{
	int32_t			error = 0,
					rest_size = 0,
					write_count = 0;
	SElExceptionPtr	pexception = NULL;
	SELStringPtr	output = NULL;
	uint32_t		i = 0;
	SYSTEMTIME		sys_time;
	char			tmp_ascii[20] = "";
	char*			p2tmp_ascii = tmp_ascii;

	if ((gs_excludeEnabled || gs_includeEnabled) &&
		!(gs_includeEnabled && IsInFilter(indentifier, gs_include)) &&
		(gs_includeEnabled || IsInFilter(indentifier, gs_exclude)))
	{
		goto Error;
	}

	rest_size = sizeof(tmp_ascii);
	
	EXCCHECK( elstring_new("", &output));
	EXCCHECK( output->Format(output, "%c %s::%04x: ", (type == CAN_WRITE ? 'W' : 'R'),
							 channel, indentifier));

	for (i = 0; i < 8; i++)
	{
		if (i < dataSize)
		{
			EXCCHECK( output->Format(output, "%02X ", pData[i]));
			write_count = sprintf( p2tmp_ascii, "%c", (pData[i] < 32 || pData[i] > 127) ? '.' : pData[i]);
			p2tmp_ascii += write_count;
			rest_size -= write_count;
		}
		else
		{
			EXCCHECK( output->Append(output, "   "));
			write_count = sprintf(p2tmp_ascii, " ");
			p2tmp_ascii += write_count;
			rest_size -= write_count;
		}
	}

	EXCCHECK( output->Append(output, "       "));
	EXCCHECK( output->Append(output, tmp_ascii));
	EXCCHECK( output->Format(output, "     [%07.03lf s]", gs_timer->GetTimeDiff(gs_timer)));

	FileTimeToSystemTime((const FILETIME*) &timestamp, &sys_time);
	EXCCHECK( output->Format(output, " [TS: %02d:%02d:%02d,%03d]", sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds));

	EXCCHECKCVI( InsertTextBoxLine(gs_panel[CANGUI], CANGUI_COMM_LIST, -1, output->GetPtr(output)));
	if (!gs_ClearEnabled)
	{
		EXCCHECKCVI( SetCtrlAttribute( gs_panel[CANGUI], CANGUI_CLEAR, ATTR_DIMMED, 0));
		gs_ClearEnabled = 1;
	}

Error:
	elstring_delete(&output);
	EXCRETHROW(pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSizeChange"
int CVICALLBACK OnSizeChange (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0,
					value = 0,
					lines = 0;
   	SElExceptionPtr pexception = NULL;
	Rect			range;
	static int32_t	last_rows = 1;
	
	switch (event)
	{
	case EVENT_COMMIT:
		EXCCHECKCVI( GetCtrlVal(panel, control, &value));

		if (value > 112)
		{
			EXCCHECKCVI( SetCtrlVal(panel, control, 112));
			return 0;
		}

		if (value < 1)
		{
			EXCCHECKCVI( SetCtrlVal(panel, control, 1));
			return 0;
		}

		lines = value / 16 + ((value % 16) ? 1 : 0);
		if (lines != last_rows)
		{
			if (lines > last_rows)
			{
				EXCCHECKCVI( InsertTableRows( panel, KWPPANEL_TABLE, -1, lines - last_rows, VAL_CELL_NUMERIC));
			}
			if (lines < last_rows)
			{
				EXCCHECKCVI( DeleteTableRows( panel, KWPPANEL_TABLE, lines + 1, -1));
			}

			last_rows = lines;

			range.left = 1;
			range.top = 1;
			range.width = 16;
			range.height = lines;
			EXCCHECKCVI( SetTableCellRangeAttribute( panel, KWPPANEL_TABLE, range, ATTR_DATA_TYPE, VAL_UNSIGNED_CHAR));
			EXCCHECKCVI( SetTableCellRangeAttribute( panel, KWPPANEL_TABLE, range, ATTR_MIN_VALUE, 0));
			EXCCHECKCVI( SetTableCellRangeAttribute( panel, KWPPANEL_TABLE, range, ATTR_MAX_VALUE, 0xFF));
			EXCCHECKCVI( SetTableCellRangeAttribute( panel, KWPPANEL_TABLE, range, ATTR_SHOW_INCDEC_ARROWS, 0));
			EXCCHECKCVI( SetTableCellRangeAttribute( panel, KWPPANEL_TABLE, range, ATTR_FORMAT, VAL_HEX_FORMAT));
			EXCCHECKCVI( SetTableCellRangeAttribute( panel, KWPPANEL_TABLE, range, ATTR_PADDING, 2));
		}

		range.left = 1;
		range.top = 1;
		range.width = 16;
		range.height = last_rows;
		EXCCHECKCVI( SetTableCellRangeAttribute( panel, KWPPANEL_TABLE, range, ATTR_CELL_DIMMED, 0));

		if (value % 16)
		{
			range.left = value % 16 + 1;
			range.top = last_rows;
			range.width = 16 - value % 16;
			range.height = 1;
			EXCCHECKCVI( SetTableCellRangeAttribute( panel, KWPPANEL_TABLE, range, ATTR_CELL_DIMMED, 1));
		}
		break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSend1"
int CVICALLBACK OnSend1 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0,
					row = 0,
					column = 0,
					i = 0,
					size = 0,
					read_size = 0;
   	SElExceptionPtr pexception = NULL;
	char			channel[256] = "";
	uint8_t*		buffer = NULL;
	uint8_t			read_buffer[1024];
	uint32_t		rident = 0,
					wident = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( SetCtrlAttribute( panel, control, ATTR_DIMMED, 1));
			gs_ReadPause = TRUE;

			/* Get Write Parameters */
			EXCCHECKCVI( GetCtrlVal( panel, KWPPANEL_CHANNEL, channel));
			EXCCHECKCVI( GetCtrlVal( panel, KWPPANEL_SIZE, &size));
			EXCCHECKCVI( GetCtrlVal( panel, KWPPANEL_READ_ID, &rident));
			EXCCHECKCVI( GetCtrlVal( panel, KWPPANEL_WRITE_ID, &wident));

			/* Get Data to Send */
			buffer = (uint8_t*) calloc( size, sizeof(uint8_t));
			for (i = 0; i < size; i++)
			{
				row = i / 16 + 1;
				column = i % 16 + 1;
				EXCCHECKCVI( GetTableCellVal(panel, KWPPANEL_TABLE, MakePoint(column, row), &buffer[i]));
			}

			/* Send KWP2000 Data */
			read_size = sizeof(read_buffer);
			memset( read_buffer, 0, read_size);

			if(gs_hThread) /* wait for thread pause */
				Sleep(TIMEOUT_CAN+100);

			EXCCHECK( gs_can->QueryEx(gs_can, channel, wident, buffer, size, rident, &read_size, read_buffer));
			break;
	}

Error:
	if(gs_ReadPause) gs_ReadPause = FALSE;
	if(buffer) free(buffer);
	SetCtrlAttribute( panel, control, ATTR_DIMMED, 0);
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnSend2"
int CVICALLBACK OnSend2 (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0,
					i = 0,
					packet_type = 0;
   	SElExceptionPtr pexception = NULL;
	uint8_t			buffer[8];
	char			channel[128] = "";
	uint32_t		wident = 0;
	uint32_t		wsize = 0;
					
	switch (event)
	{
	case EVENT_COMMIT:
		EXCCHECKCVI( SetCtrlAttribute( panel, control, ATTR_DIMMED, 1));

		/* Get Write Parameters */
		EXCCHECKCVI( GetCtrlVal( panel, LOWPANEL_CHANNEL, channel));
		EXCCHECKCVI( GetCtrlVal( panel, LOWPANEL_WRITE_ID, &wident));
		EXCCHECKCVI( GetCtrlVal( panel, LOWPANEL_WRITE_SIZE, &wsize));
		EXCCHECKCVI( GetCtrlVal( panel, LOWPANEL_PACKET_TYPE, &packet_type));
		
		/* Get Data to Send */
		memset(buffer, 0xFF, 8);
		for (i = 0; i < wsize; i++)
		{
			EXCCHECKCVI( GetTableCellVal(panel, LOWPANEL_TABLE, MakePoint(i + 1, 1), &buffer[i]));
		}
		
		EXCCHECK( gs_can->WriteLowLevel(gs_can, channel, wident, packet_type, wsize, buffer));
		break;
	}

Error:
	SetCtrlAttribute( panel, control, ATTR_DIMMED, 0);
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnRecChanged"
int CVICALLBACK OnRecChanged (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			i;
	const char*		channel = NULL;
	
	switch (event)
	{
	case EVENT_COMMIT:
		switch (control)
		{
		case CANGUI_START:
			for(i=0; i<(int)gs_can->_channelList->GetCount(gs_can->_channelList); i++)
			{
				channel = gs_can->_channelList->GetNameOnIndex( gs_can->_channelList, i);
				EXCCHECK( gs_can->SetTimeout(gs_can, channel, TIMEOUT_CAN));
			}
			
			EXCCHECK( ThreadStart(Thread_CANReadLoop, NULL, &gs_hThread, &gs_hEventStop));

			EXCCHECKCVI( SetCtrlAttribute( panel, CANGUI_START, ATTR_DIMMED, 1));
			EXCCHECKCVI( SetCtrlAttribute( panel, CANGUI_STOP, ATTR_DIMMED, 0));
			break;
		case CANGUI_STOP:
			EXCCHECK( ThreadStop(&gs_hThread, &gs_hEventStop));

			for(i=0; i<(int)gs_can->_channelList->GetCount(gs_can->_channelList); i++)
			{
				channel = gs_can->_channelList->GetNameOnIndex( gs_can->_channelList, i);
				EXCCHECK( gs_can->SetTimeout(gs_can, channel, TIMEOUT_CAN_DEFAULT));
			}

			EXCCHECKCVI( SetCtrlAttribute( panel, CANGUI_START, ATTR_DIMMED, 0));
			EXCCHECKCVI( SetCtrlAttribute( panel, CANGUI_STOP, ATTR_DIMMED, 1));
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
#define __FUNC__ "OnChannelChanged"
int CVICALLBACK OnChannelChanged (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
   	SElExceptionPtr pexception = NULL;
	char			channel[128] = "";
	
	switch (event)
	{
	case EVENT_COMMIT:
		EXCCHECKCVI( GetCtrlVal(panel, control, channel));

		if (!strncmp(channel, "CAN", 3))
		{
			EXCCHECKCVI( ClearListCtrl( panel, LOWPANEL_PACKET_TYPE));
			EXCCHECKCVI( InsertListItem( panel, LOWPANEL_PACKET_TYPE, -1, "Data", 0));
			EXCCHECKCVI( InsertListItem( panel, LOWPANEL_PACKET_TYPE, -1, "Remote", 1));
		}
		else if (!strncmp(channel, "LIN", 3))
		{
			EXCCHECKCVI( ClearListCtrl( panel, LOWPANEL_PACKET_TYPE));
			EXCCHECKCVI( InsertListItem( panel, LOWPANEL_PACKET_TYPE, -1, "Header Frame", 17));
			EXCCHECKCVI( InsertListItem( panel, LOWPANEL_PACKET_TYPE, -1, "Full Frame", 18));
		}
		break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnApply"
int CVICALLBACK OnApply (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
   	SElExceptionPtr pexception = NULL;
	char			tmp_filter[512] = "";
	char*			p2tmp = NULL;
	
	switch (event)
	{
	case EVENT_COMMIT:
		EXCCHECKCVI( GetCtrlVal( panel, FILTER_INC_ENABLE, &gs_includeEnabled));
		EXCCHECKCVI( GetCtrlVal( panel, FILTER_EXC_ENABLE, &gs_excludeEnabled));
		EXCCHECKCVI( GetCtrlVal( panel, FILTER_INCLUDE, tmp_filter));
		
		/* To Upper Case */
		p2tmp = tmp_filter;
		while (*p2tmp != '\0')
		{
			if (*p2tmp >= 'a' && *p2tmp <= 'f')
				*p2tmp -= 'a' - 'A';

			p2tmp++;
		}
		strcpy_s(gs_include, sizeof(gs_include), tmp_filter);

		EXCCHECKCVI( GetCtrlVal( panel, FILTER_EXCLUDE, tmp_filter));
		
		/* To Upper Case */
		p2tmp = tmp_filter;
		while (*p2tmp != '\0')
		{
			if (*p2tmp >= 'a' && *p2tmp <= 'f')
				*p2tmp -= 'a' - 'A';

			p2tmp++;
		}
		strcpy_s(gs_exclude, sizeof(gs_exclude), tmp_filter);
		break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnLoopEnabled"
int CVICALLBACK OnLoopEnabled (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0,
					value = 0;
   	SElExceptionPtr pexception = NULL;
	
	switch (event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlVal( panel, control, &value));
			if (value)
			{
				uint8_t		packet[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
				char		channel[32] = "CAN0";
				uint32_t	ident = 0,
							type = 18,
							time = 0,
							size = 0,
							i, j;
	
				EXCCHECKCVI( GetNumTableRows (gs_panel[LOWPANEL], LOWPANEL_LIST, &value));
				for(i=0; i<(uint32_t)value; i++)
				{
					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(1, i+1), channel));
					if(strlen(channel)==0)
						continue;

					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(2, i+1), &ident));
					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(3, i+1), &type));
					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(4, i+1), &time));
					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(5, i+1), &size));

					for(j=0; j<size; j++)
					{
						EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST, MakePoint(6+j, i+1), &packet[j]));
					}

					EXCCHECK( gs_can->AddThreadItem(gs_can, channel, ident, type, packet, size, time));
				}
				
				EXCCHECK( gs_can->StartThread(gs_can, NULL));
			}
			else
			{
				EXCCHECK( gs_can->StopThread(gs_can, NULL));
				EXCCHECK( gs_can->ClearThreadItems(gs_can, NULL));
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnMultEnabled"
int CVICALLBACK OnMultEnabled (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0,
					value = 0;
   	SElExceptionPtr pexception = NULL;
	uint8_t		packet[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	char		channel[32] = "CAN0";
	uint32_t	ident = 0,
				type = 18,
				time = 0,
				size = 0,
				i, j;
	
	switch (event)
	{
		case EVENT_COMMIT:

	
				EXCCHECKCVI( GetNumTableRows (gs_panel[LOWPANEL], LOWPANEL_LIST_2, &value));
				
				for(i=0; i<(uint32_t)value; i++)
				{
					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(1, i+1), channel));
					if(strlen(channel)==0)
						continue;

					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(2, i+1), &ident));
					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(3, i+1), &type));
					EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(4, i+1), &size));

					for(j=0; j<size; j++)
					{
						EXCCHECKCVI( GetTableCellVal(gs_panel[LOWPANEL], LOWPANEL_LIST_2, MakePoint(5+j, i+1), &packet[j]));
					}
					
				EXCCHECK( gs_can->WriteLowLevel(gs_can, channel, ident, type, size, packet));
				}
				
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnLoopList"
int CVICALLBACK OnLoopList (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
   	SElExceptionPtr pexception = NULL;
	int				value;

	switch (event)
	{
		case EVENT_COMMIT:
			switch(control)
			{
				case LOWPANEL_ADD_2:
					EXCCHECKCVI( InsertTableRows (gs_panel[LOWPANEL], LOWPANEL_LIST, -1, 1, VAL_USE_MASTER_CELL_TYPE));
					break;
				case LOWPANEL_REMOVE_2:
					EXCCHECKCVI( GetNumTableRows (gs_panel[LOWPANEL], LOWPANEL_LIST, &value));
					if(value>0)
						EXCCHECKCVI( DeleteTableRows (gs_panel[LOWPANEL], LOWPANEL_LIST, value, 1));
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
#define __FUNC__ "OnCmdChanged"
int CVICALLBACK OnCmdChanged (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
   	SElExceptionPtr pexception = NULL;
	int				index = 0,
					value = 0,
					i;

	switch(event)
	{
		case EVENT_COMMIT:
			EXCCHECKCVI( GetCtrlIndex(panel, control, &index));
			EXCCHECKCVI( GetValueFromIndex (panel, control, index, &value));
			if(index==0)
			{
				EXCCHECKCVI( SetCtrlVal(panel, KWPPANEL_SIZE, 2));
				OnSizeChange (gs_panel[KWPPANEL], KWPPANEL_SIZE, EVENT_COMMIT, NULL, 0, 0);

				for(i=0; i<2; i++)
				{
					EXCCHECKCVI( SetTableCellVal(panel, KWPPANEL_TABLE, MakePoint(i+1, 1), 0));
				}
			}
			else /* predefined command */
			{
				EXCCHECKCVI( SetCtrlVal(panel, KWPPANEL_SIZE, gs_Cmd[value].size));
				OnSizeChange (gs_panel[KWPPANEL], KWPPANEL_SIZE, EVENT_COMMIT, NULL, 0, 0);

				for(i=0; i<(int)gs_Cmd[value].size; i++)
				{
					EXCCHECKCVI( SetTableCellVal(panel, KWPPANEL_TABLE, MakePoint(i+1, 1), gs_Cmd[value].data[i]));
				}
			}
			break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "OnClear"
int CVICALLBACK OnClear (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int32_t			error = 0;
   	SElExceptionPtr pexception = NULL;
	
	switch(event)
	{
	case EVENT_COMMIT:
		EXCCHECKCVI( ResetTextBox(panel, CANGUI_COMM_LIST, ""));
		EXCCHECKCVI( SetCtrlAttribute(panel, control, ATTR_DIMMED, 1));
		gs_ClearEnabled = 0;
		if (gs_timer)
			gs_timer->Start(gs_timer);
		break;
	}

Error:
	EXCDISPLAY( pexception);
	return 0;
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "ShowPanel"
static SElExceptionPtr ShowPanel(void)
{
	int32_t         error = 0;
   	SElExceptionPtr pexception = NULL;
	int32_t			visible;
	int32_t			cache = 1;
	
	EXCCHECKCVI( GetPanelAttribute( gs_panel[CANGUI], ATTR_VISIBLE, &visible));
	if(!visible)
	{
		EXCCHECKCVI( DisplayPanel(gs_panel[CANGUI])); 

		/* can log */
		EXCCHECK( gs_can->GetAttribute(gs_can, NULL, DATA_TYPE_INT32, CAN_ATTR_CACHE_ENABLED,
									   &gs_oldCacheState, sizeof(gs_oldCacheState)));
		EXCCHECK( gs_can->SetAttribute(gs_can, NULL, DATA_TYPE_INT32, CAN_ATTR_CACHE_ENABLED, &cache));
		EXCCHECK( gs_can->RegisterLogFunction(gs_can, NULL, DisplayCommunication, &gs_oldLogFunc));
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
	int32_t			visible;
	
	EXCCHECKCVI( GetPanelAttribute( gs_panel[CANGUI], ATTR_VISIBLE, &visible));
	if(visible)
	{
		EXCCHECKCVI( HidePanel(gs_panel[CANGUI])); 

		OnRecChanged(gs_panel[CANGUI], CANGUI_STOP, EVENT_COMMIT, 0, 0, 0);		

		EXCCHECK( gs_can->RegisterLogFunction(gs_can, NULL, gs_oldLogFunc, NULL));
		EXCCHECK( gs_can->SetAttribute(gs_can, NULL, DATA_TYPE_INT32, CAN_ATTR_CACHE_ENABLED, &gs_oldCacheState));
	}

Error:      
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
#define __FUNC__ "{CanGUI Plugin}::Initialize"
PLUGIN_API SElExceptionPtr PLUGIN_INIT_NAME_CODE( SPluginArgsPtr a_pPluginArgs )
{
	SElExceptionPtr  	pexception = NULL;    
	int32_t         	error = 0,
						main_panel = 0;
	uint32_t			i = 0;
	const char*			channel = NULL;
   
   	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
                            "CardPanel",
                            sizeof(main_panel),
                            &main_panel));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
							 "WindowHandle",
							 sizeof(gs_hWnd),
							 &gs_hWnd));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "Modules",
	                        sizeof(SObjListPtr),
	                        &gs_Modules));

	EXCCHECK( a_pPluginArgs->GetArg( a_pPluginArgs,
	                        "EventFactory",
	                        sizeof(SElEventFactoryPtr),
	                        &gs_EventFactory));

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
	
	EXCCHECK( LoadPanelExWithLanguage(main_panel, "canGUI.uir", CANGUI, gs_canGUILang, &gs_panel[CANGUI]));
   
	EXCCHECKCVI( SetPanelAttribute (gs_panel[CANGUI], ATTR_TOP, 0));
	EXCCHECKCVI( SetPanelAttribute (gs_panel[CANGUI], ATTR_LEFT, 0));

	EXCCHECKCVI( ResetTextBox(gs_panel[CANGUI], CANGUI_COMM_LIST, ""));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[CANGUI], CANGUI_CLEAR, ATTR_DIMMED, 1));

	/* Fill Channel List */
	EXCCHECKCVI( GetPanelHandleFromTabPage( gs_panel[CANGUI], CANGUI_TAB, 0, &gs_panel[KWPPANEL]));
	EXCCHECKCVI( GetPanelHandleFromTabPage( gs_panel[CANGUI], CANGUI_TAB, 1, &gs_panel[LOWPANEL]));

	EXCCHECKCVI( ClearListCtrl( gs_panel[KWPPANEL], KWPPANEL_CHANNEL));
	EXCCHECKCVI( ClearListCtrl( gs_panel[LOWPANEL], LOWPANEL_CHANNEL));

	if(gs_Modules)
	{
		gs_can = (SCANListPtr) gs_Modules->Get(gs_Modules, MODULE_CAN);

		if(gs_can)
		{
			for (i = 0; i < gs_can->_channelList->GetCount(gs_can->_channelList); i++)
			{
				channel = gs_can->_channelList->GetNameOnIndex( gs_can->_channelList, i);
				EXCCHECKCVI( InsertListItem( gs_panel[KWPPANEL], KWPPANEL_CHANNEL, -1, channel, channel));
				EXCCHECKCVI( InsertListItem( gs_panel[LOWPANEL], LOWPANEL_CHANNEL, -1, channel, channel));
			}
		}
	}

	OnSizeChange (gs_panel[KWPPANEL], KWPPANEL_SIZE, EVENT_COMMIT, NULL, 0, 0);
	OnChannelChanged (gs_panel[LOWPANEL], LOWPANEL_CHANNEL, EVENT_COMMIT, NULL, 0, 0);

	/* Fill Predefined Commands */
	EXCCHECK( LoadPredefinedCommands(NULL));

	/* Fill Async Frames Commands */
	EXCCHECK( LoadAsyncFrames(NULL));
	
	/* Fill Multi-Frames Commands */
	EXCCHECK( LoadMultiFrames(NULL));
			  
	EXCCHECK( timer_new(&gs_timer));
	gs_timer->Start(gs_timer);
	
Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{CanGUI Plugin}::Run"
PLUGIN_API SElExceptionPtr PLUGIN_RUN_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
	SElExceptionPtr	pexception = NULL;
    
	if(0 == strcmp(uuid, PLUGIN_CANGUI))
	{
		EXCCHECK( ShowPanel());
	}
				
Error:   
	EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{CanGUI Plugin}::Leave"
PLUGIN_API SElExceptionPtr PLUGIN_LEAVE_NAME_CODE( SPluginArgsPtr a_pPluginArgs, const char* uuid)
{
   	SElExceptionPtr	pexception = NULL;
   
	EXCCHECK( ClosePanel());

Error:   
   	EXCRETHROW( pexception);

}

#undef __FUNC__
#define __FUNC__ "{CanGUI Plugin}::Shutdown"
PLUGIN_API SElExceptionPtr PLUGIN_SHUTDOWN_NAME_CODE( SPluginArgsPtr a_pPluginArgs)
{
   	SElExceptionPtr	pexception = NULL;
   	int32_t         error = 0;

	if(gs_panel[CANGUI])
		EXCCHECKCVI( DiscardPanel(gs_panel[CANGUI]));

	if (gs_timer)
		timer_delete(&gs_timer);
	
Error:   
   EXCRETHROW( pexception);
}


#undef __FUNC__
#define __FUNC__ "{CanGUI Plugin}::Info"
PLUGIN_API SElExceptionPtr PLUGIN_GETINFO_NAME_CODE( SPluginInfoPtr* a_pInfo)
{
	SElExceptionPtr	pexception = NULL;  
	
	if(a_pInfo) EXCCHECK( (*a_pInfo)->SetInfoData(*a_pInfo, &gs_InfoData));
	
Error:	
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__ "{CanGUI Plugin}::Attribute"
PLUGIN_API SElExceptionPtr PLUGIN_GETATTR_NAME_CODE( uint32_t  attrID, uint32_t size, void* value)
{
   return NULL;
}

#endif 

#pragma warning( pop)
