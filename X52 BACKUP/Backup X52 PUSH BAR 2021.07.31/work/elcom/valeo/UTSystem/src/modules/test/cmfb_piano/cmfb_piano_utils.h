#if !defined(__CMFB_PIANO_UTILS_H__)
#define __CMFB_PIANO_UTILS_H__

#include <test/eltest.h>
#include <eltypes.h>
#include <elerror.h>

#ifdef __cplusplus
    extern "C" {
#endif 

#define DEFAULT_MAX_RESISTANCE  9.99e+39
#define TIMEOUT_DEFAULT			10000
#define TIMEOUT_TABLE			INFINITE
#define TIMEOUT_AXIS			10000

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
		


/* BFFA2008  No explicit path exists between the two channels.
 * BFFA200C  The channels are explicitly connected.
 */
#define checkSwtch(fCall) \
	if (error = (fCall), (error = (error<0 && error!=0xBFFA2008 && error!=0xBFFA200C) ? error : VI_SUCCESS)) \
		{ goto Error; }  else error = error

#define CONNECT_BLOWER(position) \
	do { \
		if(position==1){ checkSwtch( IviSwtch_Connect (vi_PSW1, "ch0", "com0"));} \
		else if(position==2){ checkSwtch( IviSwtch_Connect (vi_PSW1, "ch1", "com1"));} \
		else if(position==3){ checkSwtch( IviSwtch_Connect (vi_PSW1, "ch2", "com2"));} \
		else if(position==4){ checkSwtch( IviSwtch_Connect (vi_PSW1, "ch3", "com3"));} \
	}while(0)

#define CONNECT_BLOWER_DMM(position) \
	do { \
		if(position==1){ checkSwtch( IviSwtch_Connect (vi_SW5, "ch4", "com0"));} \
		else if(position==2){ checkSwtch( IviSwtch_Connect (vi_SW5, "ch5", "com0"));} \
		else if(position==3){ checkSwtch( IviSwtch_Connect (vi_SW5, "ch6", "com0"));} \
		else if(position==4){ checkSwtch( IviSwtch_Connect (vi_SW5, "ch7", "com0"));} \
	}while(0)

#define DISCONNECT_BLOWERS \
	do { \
		checkSwtch( IviSwtch_Disconnect (vi_PSW1, "ch0", "com0")); \
		checkSwtch( IviSwtch_Disconnect (vi_PSW1, "ch1", "com1")); \
		checkSwtch( IviSwtch_Disconnect (vi_PSW1, "ch2", "com2")); \
		checkSwtch( IviSwtch_Disconnect (vi_PSW1, "ch3", "com3")); \
	} while(0)

#define DISCONNECT_BLOWERS_DMM \
	do { \
		checkSwtch( IviSwtch_DisconnectAll (vi_SW5)); \
	} while(0)

#define IF_NOT_CONNECT_BLOWER(position) \
	do { \
		ViInt32 path_capability=0; \
		if(position==1){ checkSwtch( IviSwtch_CanConnect (vi_PSW1, "ch0", "com0", &path_capability));} \
		else if(position==2){ checkSwtch( IviSwtch_CanConnect (vi_PSW1, "ch1", "com1", &path_capability));} \
		else if(position==3){ checkSwtch( IviSwtch_CanConnect (vi_PSW1, "ch2", "com2", &path_capability));} \
		else if(position==4){ checkSwtch( IviSwtch_CanConnect (vi_PSW1, "ch3", "com3", &path_capability));} \
		if( path_capability==IVISWTCH_VAL_PATH_AVAILABLE) {

#define IF_NOT_CONNECT_BLOWER_DMM(position) \
	do { \
		ViInt32 path_capability=0; \
		if(position==1){ checkSwtch( IviSwtch_CanConnect (vi_SW5, "ch4", "com0", &path_capability));} \
		else if(position==2){ checkSwtch( IviSwtch_CanConnect (vi_SW5, "ch5", "com0", &path_capability));} \
		else if(position==3){ checkSwtch( IviSwtch_CanConnect (vi_SW5, "ch6", "com0", &path_capability));} \
		else if(position==4){ checkSwtch( IviSwtch_CanConnect (vi_SW5, "ch7", "com0", &path_capability));} \
		if( path_capability==IVISWTCH_VAL_PATH_AVAILABLE) {

#define TIMEOUT_INIT() 	clock_t	_start_clock = clock()

#define TIMEOUT_CHECK(timeout) \
	if ( ((clock() - _start_clock) / (double)CLOCKS_PER_SEC) > ((double)timeout/1000.0) ) { EXCTHROW( TEST_ERR_TIMEOUT, "Button Timeout"); } else

typedef enum _EBlowerPosition
{
	POSITION_0 = 0,
	POSITION_1,
	POSITION_2,
	POSITION_3,
	POSITION_4
} EBlowerPosition;

SElExceptionPtr ReadAnalogWithScale(char *Used_Channel, char *Channel, SCalibrationPtr pScaleInfo, double *Value);
SElExceptionPtr ReadAnalogWithScale2(char *Used_Channel, char *Channel, SCalibrationPtr pScaleInfo, double *Value); 
//SElExceptionPtr ReadAnalogWithScale(char *Channel, SCalibrationPtr pScaleInfo, double *Value);
SElExceptionPtr LineaTable1D(double *InputTable, SCalibrationPtr pScaleInfo, int size, double *OutputTable);
SElExceptionPtr ReadAnalog(char *Channel, double *Value);
SElExceptionPtr ReadAnalog2(char *Channel, double *Value);

SElExceptionPtr MoveToPush(SaxissPtr paxiss,
						   int index,
						   int check, 
						   double position_axis_Z, 
						   double position_axis_Y, 
						   double position_axis_X);

SElExceptionPtr MoveToPush2(SaxissPtr paxiss,
						   int index,
						   int check, 
						   double position_axis_Y, 
						   double position_axis_X);

SElExceptionPtr Get_GAP_Measures(SObjListPtr pModules, double *Mean_GAP_50, double *Mean_GAP_60);

void ElSleep(DWORD time);
void RealTimeWaiting50(double time_ms);
void RealTimeWaiting60(double time_ms);

SElExceptionPtr CheckDirectory(const char* directory);

#define BUTTON_HFS		0
#define BUTTON_HRW		1
#define BUTTON_AC		2

#define CHECK_BUTTON(button_id) CHECK_HIGHLIGHT(##button_id)

#define DRAW_HIGHLIGHT(resource) DRAW_HIGHLIGHT_##resource##(0)

#define CHECK_HIGHLIGHT(button_id) \
	DRAW_HIGHLIGHT_##button_id##( (pexception) ? 2 : 1)

#define DRAW_HIGHLIGHT_BUTTON_HFS(pid)		DISPLAY_CIRCLE(135, 180, 25, pid)
#define DRAW_HIGHLIGHT_BUTTON_HRW(pid)		DISPLAY_CIRCLE(370, 180, 25, pid)
#define DRAW_HIGHLIGHT_BUTTON_AC(pid)		DISPLAY_CIRCLE(610, 180, 25, pid)
#define DRAW_HIGHLIGHT_DEFROST(pid)			DISPLAY_CIRCLE(450, 230, 25, pid)
#define DRAW_HIGHLIGHT_BLOWER_0(pid)		DISPLAY_CIRCLE(101, 161, 12, pid)
#define DRAW_HIGHLIGHT_BLOWER_1(pid)		DISPLAY_CIRCLE(115, 145, 12, pid)
#define DRAW_HIGHLIGHT_BLOWER_2(pid)		DISPLAY_CIRCLE(136, 140, 12, pid)
#define DRAW_HIGHLIGHT_BLOWER_3(pid)		DISPLAY_CIRCLE(157, 145, 12, pid)
#define DRAW_HIGHLIGHT_BLOWER_4(pid)		DISPLAY_CIRCLE(173, 161, 12, pid)
#define DRAW_HIGHLIGHT_HEATER_RIGHT(pid)	DISPLAY_ARROW_RIGHT(634, 120, 25, (pid))
#define DRAW_HIGHLIGHT_HEATER_LEFT(pid)		DISPLAY_ARROW_LEFT(584, 120, 25, (pid))
#define DRAW_HIGHLIGHT_BLOWER_RIGHT(pid)	DISPLAY_ARROW_RIGHT(160, 120, 25, (pid))
#define DRAW_HIGHLIGHT_BLOWER_LEFT(pid)		DISPLAY_ARROW_LEFT(110, 120, 25, (pid))

#if defined(__cplusplus)
    } /* extern "C" */
#endif

#endif /* __CMFB_PIANO_UTILS_H__ */
