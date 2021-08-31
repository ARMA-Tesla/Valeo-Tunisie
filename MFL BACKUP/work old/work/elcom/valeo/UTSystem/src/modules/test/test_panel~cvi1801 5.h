#if !defined(__TEST_PANEL_H__)
#define __TEST_PANEL_H__

#include <test/eltest.h>
#include "test.h"
#include <eltypes.h>
#include <elerror.h>
#include <language/language.h>
#include <userint.h>

int							gs_TestPanel2;
int							gs_TestPanel3;
int							gs_TestPanel4;



static int panelHelpStation [8] = 
	{
		PANEL_STATION_10, 	PANEL_STATION_20, 	PANEL_STATION_30, 	PANEL_STATION_40,
		PANEL_STATION_50, 	PANEL_STATION_60, 	PANEL_STATION_70, 	PANEL_STATION_80,
	};

static int panelStatusStation [8] = 
	{
		PANEL_STATE_STATION_10, 	PANEL_STATE_STATION_20, 	PANEL_STATE_STATION_30, 	PANEL_STATE_STATION_40,
		PANEL_STATE_STATION_50, 	PANEL_STATE_STATION_60, 	PANEL_STATE_STATION_70, 	PANEL_STATE_STATION_80,
	};


TEST_API SElExceptionPtr _FUNCC IniTestPanel(const char* picture, bool_t reset);
TEST_API SElExceptionPtr _FUNCC CloseTestPanel(void); 
TEST_API SElExceptionPtr _FUNCC SetTestPanelDescription(int32_t control, const char* description);
TEST_API SElExceptionPtr _FUNCC SetPanelWarning(int32_t control,  bool_t status);
TEST_API SElExceptionPtr _FUNCC SetPanelStationStatus(int32_t station,  bool_t status);
TEST_API SElExceptionPtr _FUNCC SetTestPanelReferences(char* RefValeo,  char* RefCustomer);
TEST_API SElExceptionPtr _FUNCC SetTestPanelMode(int32_t mode);

TEST_API SElExceptionPtr _FUNCC GetTestFunction(int32_t index, STestListItemPtr* pTest, int32_t* count);
TEST_API SElExceptionPtr _FUNCC GetTestSeqFunction(STestSeqPtr* pTestSeq);

#define COLOR_GRID  0x00347B

#define TEST_HELP(help) \
	EXCCHECKCVI( SetCtrlVal(gs_TestPanel, PANEL_HELP2, help)) \

#define TEST_GRAPH_RIGHT_STATION_30(name, data, sampsPerCan) \
	DeleteGraphPlot (gs_TestPanel2, PANEL_2_GRAPH,-1, VAL_IMMEDIATE_DRAW); \
	SetCtrlVal( gs_TestPanel2, PANEL_2_NAME_RIGHT_STATION_30, name); \
	SetAxisRange (gs_TestPanel2, PANEL_2_GRAPH, VAL_MANUAL,getMinDoubleArray(data[MESS_WEG]), getMaxDoubleArray(data[MESS_WEG]), VAL_MANUAL,0, getMaxDoubleArray(data[MESS_KRAFT])); \
	PlotXY (gs_TestPanel2,PANEL_2_GRAPH, &data[MESS_WEG], &data[MESS_KRAFT],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_FORCE); \
	PlotXY (gs_TestPanel2,PANEL_2_GRAPH, &data[MESS_WEG], &data[MESS_CONTACT],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_CONTACT); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define TEST_GRAPH_LEFT_STATION_30(name, data, sampsPerCan) \
	DeleteGraphPlot (gs_TestPanel2, PANEL_2_GRAPH_2,-1, VAL_IMMEDIATE_DRAW); \
	SetCtrlVal( gs_TestPanel2, PANEL_2_NAME_LEFT_STATION_30, name); \
	SetAxisRange (gs_TestPanel2, PANEL_2_GRAPH_2, VAL_MANUAL,getMinDoubleArray(data[MESS_WEG]), getMaxDoubleArray(data[MESS_WEG]), VAL_MANUAL,0, getMaxDoubleArray(data[MESS_KRAFT])); \
	PlotXY (gs_TestPanel2,PANEL_2_GRAPH_2, &data[MESS_WEG], &data[MESS_KRAFT],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_FORCE); \
	PlotXY (gs_TestPanel2,PANEL_2_GRAPH_2, &data[MESS_WEG], &data[MESS_CONTACT],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_CONTACT); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define DELETE_GRAPH_RIGHT_STATION_30(name) \
	DeleteGraphPlot (gs_TestPanel2, PANEL_2_GRAPH,-1, VAL_IMMEDIATE_DRAW); \
	SetCtrlVal( gs_TestPanel2, PANEL_2_NAME_RIGHT_STATION_30, name); 

#define DELETE_GRAPH_LEFT_STATION_30(name) \
	DeleteGraphPlot (gs_TestPanel2, PANEL_2_GRAPH_2,-1, VAL_IMMEDIATE_DRAW); \
	SetCtrlVal( gs_TestPanel2, PANEL_2_NAME_LEFT_STATION_30, name); 


#define TEST_GRAPH_MESS_RIGHT_STATION_30(data, x, y, color) \
	x_vect[0] = getMinDoubleArray(data[MESS_WEG]); \
	y_vect[0] = y; \
	x_vect[1] = x; \
	y_vect[1] = y; \
	PlotXY (gs_TestPanel2,PANEL_2_GRAPH, x_vect, y_vect,2,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,color); \
	x_vect[0] = x; \
	y_vect[0] = getMinDoubleArray(data[MESS_KRAFT]); \
	x_vect[1] = x; \
	y_vect[1] = y; \
	PlotXY (gs_TestPanel2,PANEL_2_GRAPH, x_vect, y_vect,2,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,color); \
	ProcessSystemEvents (); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define TEST_GRAPH_MESS_LEFT_STATION_30(data, x, y, color) \
	x_vect[0] = getMinDoubleArray(data[MESS_WEG]); \
	y_vect[0] = y; \
	x_vect[1] = x; \
	y_vect[1] = y; \
	PlotXY (gs_TestPanel2,PANEL_2_GRAPH_2, x_vect, y_vect,2,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,color); \
	x_vect[0] = x; \
	y_vect[0] = getMinDoubleArray(data[MESS_KRAFT]); \
	x_vect[1] = x; \
	y_vect[1] = y; \
	PlotXY (gs_TestPanel2,PANEL_2_GRAPH_2, x_vect, y_vect,2,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,color); \
	ProcessSystemEvents (); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define CONVERT_GRAPH_TO_PNG_STATION_30(path) \
	SaveGraphPNG (gs_TestPanel2,PANEL_2, path); \

#define TEST_GRAPH_RIGHT_STATION_40(name, data, sampsPerCan) \
	DeleteGraphPlot (gs_TestPanel3, PANEL_3_GRAPH,-1, VAL_IMMEDIATE_DRAW); \
	SetCtrlVal( gs_TestPanel3, PANEL_3_NAME_RIGHT_STATION_40, name); \
	SetAxisRange (gs_TestPanel3, PANEL_3_GRAPH, VAL_MANUAL,getMinDoubleArray(data[MESS_WEG]), getMaxDoubleArray(data[MESS_WEG]), VAL_MANUAL,0, getMaxDoubleArray(data[MESS_KRAFT])); \
	PlotXY (gs_TestPanel3,PANEL_3_GRAPH, &data[MESS_WEG], &data[MESS_KRAFT],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_FORCE); \
	PlotXY (gs_TestPanel3,PANEL_3_GRAPH, &data[MESS_WEG], &data[MESS_CONTACT],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_CONTACT); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define TEST_GRAPH_LEFT_STATION_40(name, data, sampsPerCan) \
	DeleteGraphPlot (gs_TestPanel3, PANEL_3_GRAPH_2,-1, VAL_IMMEDIATE_DRAW); \
	SetCtrlVal( gs_TestPanel3, PANEL_3_NAME_LEFT_STATION_40, name); \
	SetAxisRange (gs_TestPanel3, PANEL_3_GRAPH_2, VAL_MANUAL,getMinDoubleArray(data[MESS_WEG]), getMaxDoubleArray(data[MESS_WEG]), VAL_MANUAL,0, getMaxDoubleArray(data[MESS_KRAFT])); \
	PlotXY (gs_TestPanel3,PANEL_3_GRAPH_2, &data[MESS_WEG], &data[MESS_KRAFT],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_FORCE); \
	PlotXY (gs_TestPanel3,PANEL_3_GRAPH_2, &data[MESS_WEG], &data[MESS_CONTACT],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_CONTACT); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define DELETE_GRAPH_RIGHT_STATION_40(name) \
	DeleteGraphPlot (gs_TestPanel3, PANEL_3_GRAPH,-1, VAL_IMMEDIATE_DRAW); \
	SetCtrlVal( gs_TestPanel3, PANEL_3_NAME_RIGHT_STATION_40, name); 

#define DELETE_GRAPH_LEFT_STATION_40(name) \
	DeleteGraphPlot (gs_TestPanel3, PANEL_3_GRAPH_2,-1, VAL_IMMEDIATE_DRAW); \
	SetCtrlVal( gs_TestPanel3, PANEL_3_NAME_LEFT_STATION_40, name); 


#define TEST_GRAPH_MESS_RIGHT_STATION_40(data, x, y, color) \
	x_vect[0] = getMinDoubleArray(data[MESS_WEG]); \
	y_vect[0] = y; \
	x_vect[1] = x; \
	y_vect[1] = y; \
	PlotXY (gs_TestPanel3,PANEL_3_GRAPH, x_vect, y_vect,2,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,color); \
	x_vect[0] = x; \
	y_vect[0] = getMinDoubleArray(data[MESS_KRAFT]); \
	x_vect[1] = x; \
	y_vect[1] = y; \
	PlotXY (gs_TestPanel3,PANEL_3_GRAPH, x_vect, y_vect,2,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,color); \
	ProcessSystemEvents (); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define TEST_GRAPH_MESS_LEFT_STATION_40(data, x, y, color) \
	x_vect[0] = getMinDoubleArray(data[MESS_WEG]); \
	y_vect[0] = y; \
	x_vect[1] = x; \
	y_vect[1] = y; \
	PlotXY (gs_TestPanel3,PANEL_3_GRAPH_2, x_vect, y_vect,2,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,color); \
	x_vect[0] = x; \
	y_vect[0] = getMinDoubleArray(data[MESS_KRAFT]); \
	x_vect[1] = x; \
	y_vect[1] = y; \
	PlotXY (gs_TestPanel3,PANEL_3_GRAPH_2, x_vect, y_vect,2,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,color); \
	ProcessSystemEvents (); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define CONVERT_GRAPH_TO_PNG_STATION_40(path) \
	SaveGraphPNG (gs_TestPanel3,PANEL_3, path); \


#define TEST_GRAPH_RIGHT_STATION_60(data, sampsPerCan) \
	DeleteGraphPlot (gs_TestPanel4, PANEL_3_GRAPH,-1, VAL_IMMEDIATE_DRAW); \
	//SetAxisRange (gs_TestPanel4, PANEL_4_GRAPH, VAL_MANUAL,getMinDoubleArray(data[MESS_ANGLE]), getMaxDoubleArray(data[MESS_ANGLE]), VAL_MANUAL,0, getMaxDoubleArray(data[MESS_TORQUE])); \
	PlotXY (gs_TestPanel4,PANEL_4_GRAPH, &data[MESS_ANGLE], &data[MESS_TORQUE],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); \
	//PlotXY (gs_TestPanel4,PANEL_4_GRAPH, &data[MESS_ANGLE], &data[MESS_CONTACT2],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,COLOR_CONTACT3); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();

#define TEST_GRAPH_LEFT_STATION_60(data, sampsPerCan) \
	DeleteGraphPlot (gs_TestPanel4, PANEL_4_GRAPH_2, -1, VAL_IMMEDIATE_DRAW); \
	SetAxisRange (gs_TestPanel4, PANEL_4_GRAPH_2, VAL_MANUAL,0, 25, VAL_MANUAL,0, 0.1)); \
	PlotXY (gs_TestPanel4,PANEL_4_GRAPH_2, &data[MESS_ANGLE], &data[MESS_TORQUE],sampsPerCan,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,1,VAL_BLUE); \
	ProcessSystemEvents (); \
	ProcessDrawEvents ();
	
#define CONVERT_GRAPH_TO_PNG_STATION_60(path) \
	SaveGraphPNG (gs_TestPanel4,PANEL_4, path); \

#define DISPLAY_TEST( msg ) \
	do { \
		int	rows = 1; char _buffer[256] = "Test: "; \
		strcat(_buffer, msg); \
		SetCtrlVal( gs_TestPanel, PANEL_TEXT3, _buffer); \
		InsertTableRows (gs_TestPanel, PANEL_TABLE, -1, 1, VAL_CELL_STRING); \
		GetNumTableRows (gs_TestPanel, PANEL_TABLE, &rows);\
		SetTableRowAttribute (gs_TestPanel, PANEL_TABLE, rows, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE); \
		SetTableRowAttribute (gs_TestPanel, PANEL_TABLE, rows, ATTR_ROW_HEIGHT, 25); \
		SetTableCellAttribute (gs_TestPanel, PANEL_TABLE, MakePoint(1, rows), ATTR_HORIZONTAL_GRID_COLOR, COLOR_GRID); \
		SetTableCellAttribute (gs_TestPanel, PANEL_TABLE, MakePoint(1, rows), ATTR_VERTICAL_GRID_COLOR, COLOR_GRID); \
		SetTableCellAttribute (gs_TestPanel, PANEL_TABLE, MakePoint(1, rows), ATTR_CELL_JUSTIFY, VAL_CENTER_LEFT_JUSTIFIED); \
		SetTableCellAttribute (gs_TestPanel, PANEL_TABLE, MakePoint(1, rows), ATTR_CTRL_VAL, msg); \
		SetActiveTableCell (gs_TestPanel, PANEL_TABLE, MakePoint(1, rows)); \
	} while (0)

#define DISPLAY_TEST_RESULT( iStation) \
	do { \
		if(((STestParamPtr)pTID)->pexception!=NULL)  SetCtrlIndex( gs_TestPanel, panelStatusStation[iStation], 1); \
	} while (0)

#define DISPLAY_TESTSEQ_RESULT(iStation ) \
	do { \
		if(((STestParamPtr)pTID)->pexception==NULL)  SetCtrlIndex( gs_TestPanel, panelStatusStation[iStation], 0); \
	} while (0)

#define DISPLAY_OBJECT( shift, x, y, r, picture_id) \
	do { \
		int bid; \
		GetCtrlBitmap (gs_TestPanel, PANEL_CIRCLERING, ((picture_id)+(shift)), &bid); \
		CanvasDrawBitmap (gs_TestPanel, PANEL_CANVAS, bid, VAL_ENTIRE_OBJECT, MakeRect((y)-(r),(x)-(r),2*(r),2*(r))); \
		DiscardBitmap(bid); \
	} while (0) 

#define DISPLAY_TEXT(x, y, size, text, color_id) /* color_id: 0-yellow, 1-green, 2-red */ \
	do{ \
		SetCtrlAttribute (gs_TestPanel, PANEL_CANVAS, ATTR_PEN_COLOR, ((color_id)==0)? VAL_YELLOW:(((color_id)==1)? VAL_GREEN:(((color_id)==2)? VAL_RED:VAL_TRANSPARENT))); \
		SetCtrlAttribute (gs_TestPanel, PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_TRANSPARENT); \
		CreateMetaFont ("MyMetaFont", "Trebuchet MS", (size), 1, 0, 0, 0); \
		CanvasDrawTextAtPoint (gs_TestPanel, PANEL_CANVAS, (text), "MyMetaFont", MakePoint((y), (x)), VAL_CENTER); \
	}while(0)

#define DISPLAY_CLEAR_ALL() \
	do{ \
		int bid, cw, ch, bw, bh; \
		EXCCHECKCVI( CanvasClear (gs_TestPanel, PANEL_CANVAS, VAL_ENTIRE_OBJECT)); \
		EXCCHECKCVI( GetCtrlBitmap (gs_TestPanel, PANEL_PANELPICTURE, -1, &bid)); \
		EXCCHECKCVI( GetBitmapData(bid, NULL, NULL, &bw, &bh, NULL, NULL, NULL)); \
		EXCCHECKCVI( GetCtrlAttribute( gs_TestPanel, PANEL_CANVAS, ATTR_HEIGHT, &ch)); \
		EXCCHECKCVI( GetCtrlAttribute( gs_TestPanel, PANEL_CANVAS, ATTR_WIDTH, &cw)); \
		EXCCHECKCVI( CanvasDrawBitmap (gs_TestPanel, PANEL_CANVAS, bid, VAL_ENTIRE_OBJECT, MakeRect(ch/2 - bh/2, cw/2 - bw/2,VAL_KEEP_SAME_SIZE,VAL_KEEP_SAME_SIZE))); \
		DiscardBitmap(bid); \
	} while (0) 

#define DISPLAY_CIRCLE( x, y, r, picture_id) \
	DISPLAY_OBJECT( 0, x, y, r, picture_id)

#define DISPLAY_ARROW_RIGHT( x, y, r, picture_id) \
	DISPLAY_OBJECT( 3, x, y, r, picture_id)

#define DISPLAY_ARROW_LEFT( x, y, r, picture_id) \
	DISPLAY_OBJECT( 6, x, y, r, picture_id)

#define DISPLAY_ARROW_UP( x, y, r, picture_id) \
	DISPLAY_OBJECT( 9, x, y, r, picture_id)

#define DISPLAY_ARROW_DOWN( x, y, r, picture_id) \
	DISPLAY_OBJECT( 12, x, y, r, picture_id)

#define DISPLAY_TESTSEQUENCE_DURATION(time) \
	do { \
		char buffer[256]; \
		sprintf(buffer, "Time: %.1f s", time); \
		SetCtrlVal( gs_TestPanel, PANEL_TEXT1, buffer); \
	} while (0)

/*
#define PANEL_INI(iStation) \
	do{ \
	if (((STestParamPtr)pTID)->pexception!=NULL) { \
		char Last_Error[512] = "";\
		SetCtrlIndex( gs_TestPanel, panelStatusStation[iStation], 1); \
		GetCtrlVal(gs_TestPanel, panelHelpStation[iStation-1], Last_Error); \
		SetCtrlVal(gs_TestPanel, panelHelpStation[iStation], Last_Error); } \
	else SetCtrlIndex( gs_TestPanel, panelStatusStation[iStation], 2); \
	ProcessSystemEvents (); \
	ProcessSystemEvents (); \
	ProcessDrawEvents (); \
	} while(0)
*/

#define PANEL_INI(iStation) \
	do{ \
	if(((STestParamPtr)pTID)->pexception!=NULL)  SetCtrlIndex( gs_TestPanel, panelStatusStation[iStation], 1); \
	else SetCtrlIndex( gs_TestPanel, panelStatusStation[iStation], 2); \
	ProcessSystemEvents (); \
	ProcessDrawEvents (); \
	} while(0)

#define PANEL_CLEAN() \
	do { \
		int index; \
		EXCCHECKCVI( GetCtrlIndex( gs_TestPanel, PANEL_ACTION, &index)); \
		if(index==2) SetCtrlIndex( gs_TestPanel, PANEL_ACTION, 1); \
		EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_FORWARD, ATTR_DIMMED, TRUE)); \
		EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_BACKWARD, ATTR_DIMMED, TRUE)); \
		EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_REPEAT, ATTR_DIMMED, TRUE)); \
		EXCCHECKCVI( SetCtrlAttribute(gs_TestPanel, PANEL_STOP, ATTR_DIMMED, TRUE)); \
	} while (0)



#define START_SEND_DATAS_CAN(mode) \
	do { \
		EXCCHECKCVI( SetCtrlAttribute (gs_TestPanel,PANEL_TIMER_50 , ATTR_ENABLED, TRUE)); \
		EXCCHECKCVI( SetCtrlAttribute (gs_TestPanel,PANEL_TIMER_100 , ATTR_ENABLED, TRUE)); \
		EXCCHECKCVI( SetCtrlAttribute (gs_TestPanel,PANEL_TIMER_200 , ATTR_ENABLED, TRUE)); \
		EXCCHECKCVI( SetCtrlAttribute (gs_TestPanel,PANEL_TIMER_500 , ATTR_ENABLED, TRUE)); \
		EXCCHECKCVI( SetCtrlAttribute (gs_TestPanel,PANEL_TIMER_1000 , ATTR_ENABLED, TRUE)); \
	} while (0) 


#endif
