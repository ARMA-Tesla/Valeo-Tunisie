#include <bklib/bklib2.h>
#include <ansi_c.h>
#include <userint.h>
#include <time.h>
#include <dbs/dbs.h> 
#include <elstring.h>
#include <gui/report/calendar.h>
#include "statistic.h"
#include "kosu_graph.h"

#define MAX_GRID_POINTS				48

extern int				gs_panel[];
extern CAObjHandle		gs_treeHandle;
extern SDBSPtr 			gs_pDbs;
extern SDBSStoragePtr	gs_pStorage; 
extern SDBSKOSUPtr		gs_pKosu; 

static SElExceptionPtr	InitializeKosuGraph(
	int panel, 
	int control,
	time_t start_time     
); 
static SElExceptionPtr	InitializeKosuList(
	int panel, 
	int control,
	time_t time_start,
	time_t time_end
);
static SElExceptionPtr	DisplayAlarms(
	int panel, 
	int control
);
static SElExceptionPtr	DisplayAlarmsInKosuGraph(
	int panel,
	int control,
	time_t	t1,
	time_t	t2,
	time_t	gt1,
	time_t	gt2
);
static SElExceptionPtr	DisplayAlarmInKosuGraph(
	int panel,
	int control,
	int	color,
	int32_t	alarm,
	time_t	t1,
	time_t	t2,
	time_t	gt1,
	time_t	gt2
);
static SElExceptionPtr	DisplayAvgKOSUInKosuGraph(
	int panel,
	int control,
	time_t	t1,
	time_t	t2,
	time_t	gt1,
	time_t	gt2
);

static SElExceptionPtr ConvertTimestampFromANSI(time_t time, char* timestamp);

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "KosuGraph"
SElExceptionPtr	KosuGraph(void)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;
	
	EXCCHECK( InitializeCal( gs_panel[STAT], 
							 STAT_MONTH_DISPLAY, 
							 STAT_CAL_DISPLAY, 
							 STAT_MONTH_NEXT,
							 STAT_MONTH_PREV,
							 -1, -1));

	EXCCHECKCVI( SetCtrlAttribute(gs_panel[STAT], STAT_MONTH_DISPLAY, ATTR_DIMMED, FALSE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[STAT], STAT_CAL_DISPLAY, ATTR_DIMMED, FALSE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[STAT], STAT_MONTH_NEXT, ATTR_DIMMED, FALSE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[STAT], STAT_MONTH_PREV, ATTR_DIMMED, FALSE));

	EXCCHECKCVI( SetActiveCtrl(gs_panel[STAT], STAT_CAL_DISPLAY));

	EXCCHECK( InitializeKosuGraph(gs_panel[STAT], STAT_GRAPH, 0)); 
	
Error:
	EXCRETHROW( pexception);   
}

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "CalendarCallbackKosu"
SElExceptionPtr CalendarCallbackKosu(time_t time_selected)
{
	SElExceptionPtr	pexception = NULL; 
	time_t			time_start = time_selected, 
					time_end = 0;
	struct tm		*timeinfo;

	timeinfo = localtime(&time_selected);
	timeinfo->tm_hour = 23;
	timeinfo->tm_min = 59;
	timeinfo->tm_sec = 59;
	
	time_end = mktime(timeinfo);

	EXCCHECK( InitializeKosuList(gs_panel[STAT], STAT_BKTREE, time_start, time_end)); 

Error:
	EXCRETHROW( pexception);      	
}

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "BkTreeKosuNodeClick"
SElExceptionPtr BkTreeKosuNodeClick (char *keyNode)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			start_time = 0;
	HRESULT			hr;

	EXCCHECKCOM( BKLib2__DBKTreeGetNodeData (gs_treeHandle, NULL, keyNode, &start_time));
	
	if(keyNode!=0)
		EXCCHECK( InitializeKosuGraph(gs_panel[STAT], STAT_GRAPH, (time_t)start_time) );  
	
Error:
	EXCRETHROW( pexception);      	
}

#define TREE_ROW_SIZE 		2048   

/*---------------------------------------------------------------------------*/	
#undef  __FUNC__
#define __FUNC__ "InitializeKosuList"
static SElExceptionPtr	InitializeKosuList(
	int panel, 
	int control,
	time_t time_start,
	time_t time_end
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	SStatusItemPtr	pList = NULL;
	int32_t			ListCount;
	char			label[256], stime[256], stime2[256];
	int				i;
	SELStringPtr	pelstring = NULL;    
	char			row[TREE_ROW_SIZE] = ""; 
	short int		AXvalue;    
	HRESULT			hr;
	
	EXCCHECK( elstring_new("", &pelstring));
	EXCCHECK( pelstring->Append(pelstring, "root|Shift^w67$desc|Description^w143\n"));
	
	if(gs_pStorage)
	{
		EXCDBGSTR( gs_pStorage->GetShiftList(gs_pStorage, &pList, &ListCount));

		for(i=0; i<ListCount; i++)
		{
			if(difftime(pList[i].time_end, pList[i].time)>=(60.0*60.0)
				&& (time_start==0 
					|| (time_start>0 && pList[i].time>=time_start && pList[i].time_end<time_end)))
			{
				EXCCHECK( ConvertTimestampFromANSI(pList[i].time, stime));
				EXCCHECK( ConvertTimestampFromANSI(pList[i].time_end, stime2));

				sprintf(row, "|%d|%s^d%u$Shift %d (%s)\n",
							 i,
							 stime+11,
							 (unsigned int)pList[i].time,
							 pList[i].id,
							 stime2);
			
				EXCCHECK( pelstring->Append(pelstring, row));	
			}
		}
	}
	
	EXCCHECKCOM( BKLib2__DBKTreeFromString (gs_treeHandle, NULL, pelstring->GetPtr(pelstring), "$", "\n", &AXvalue)); 
	
Error:
	elstring_delete(&pelstring);    
	if(pList) gs_pDbs->Free(pList);
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "InitializeKosuGraph"
static SElExceptionPtr InitializeKosuGraph(
	int panel,
	int control,
	time_t start_time
)
{
	SElExceptionPtr	pexception = NULL;    
	int32_t			error = 0;
	int32_t			nb_of_items;
	int32_t			shift_hours = 8;
	int32_t			hour_offset = 0;
	char			label[16] = ""; /* 00:00 */
	time_t			start, stop, tmp, graph_start, graph_stop;
	struct tm*		ptm = NULL;
	struct tm		tm_time;
	time_t			grid[MAX_GRID_POINTS];
	uint32_t		used_grid = 0;

	EXCCHECKCVI( DeleteGraphPlot (panel, control, -1, VAL_IMMEDIATE_DRAW));     
	EXCCHECKCVI( SetCtrlAttribute (panel, control, ATTR_YNAME, "KOSU (s)"));
	EXCCHECKCVI( SetCtrlAttribute (panel, control, ATTR_XNAME, "Time"));   
	EXCCHECKCVI( SetCtrlAttribute (panel, control, ATTR_XGRID_VISIBLE, FALSE));
	EXCCHECKCVI( SetCtrlAttribute (panel, control, ATTR_XUSE_LABEL_STRINGS, TRUE));
	EXCCHECKCVI( SetAxisScalingMode(panel, control, VAL_LEFT_YAXIS, VAL_MANUAL, 0, 1));  
	EXCCHECKCVI( SetAxisScalingMode(panel, control, VAL_LEFT_YAXIS, VAL_AUTOSCALE, 0, 1)); 
	EXCCHECKCVI( ClearAxisItems( panel, control, VAL_BOTTOM_XAXIS));

	if(gs_pKosu)
	{
		start = start_time;
		EXCCHECK( gs_pKosu->GetShiftRange( gs_pKosu, &start, &stop) );
		
		/* check graph limits */
		if( (stop - start) < 3600 )
			goto Error;
			
		/* calculate graph start time */
		ptm = localtime(&start);
		memcpy(&tm_time, ptm, sizeof(struct tm));
		tm_time.tm_sec = 0;
		
		if(tm_time.tm_min < 30)
		{
			tm_time.tm_min = 0;  
			graph_start = mktime(&tm_time);		
		}
		else if(tm_time.tm_min >= 30) 
		{
			tm_time.tm_min = 0;  
			graph_start = mktime(&tm_time) + 3600;
		}
		
		/* calculate graph stop time */  
		ptm = localtime(&stop);
		memcpy(&tm_time, ptm, sizeof(struct tm));
		tm_time.tm_sec = 0;
		
		if (tm_time.tm_min < 30)
		{
			tm_time.tm_min = 0;  
			graph_stop = mktime(&tm_time);		
		}
		else if(tm_time.tm_min >= 30) 
		{
			tm_time.tm_min = 0;  
			graph_stop = mktime(&tm_time) + 3600;
		}

		
		nb_of_items = (int32_t)ceil( ((double)graph_start - (double)graph_stop) / (30.0*60.0));

		tmp = graph_start;
		for (; tmp<=graph_stop; tmp+=(30*60))
		{
			ptm = localtime( &tmp);
			
			sprintf(label, "%02d:%02d", (int)ptm->tm_hour, (int)ptm->tm_min);
			
			if ( used_grid < MAX_GRID_POINTS )
				grid[used_grid++] = tmp;
			
			EXCCHECKCVI( InsertAxisItem (panel, control, VAL_BOTTOM_XAXIS, -1, label, (double)tmp));
		} 

		EXCCHECKCVI( SetAxisScalingMode( panel, 
										 control, 
										 VAL_BOTTOM_XAXIS, 
										 VAL_MANUAL, 
										 (double)graph_start, 
										 (double)graph_stop));
		
		EXCCHECK( DisplayAvgKOSUInKosuGraph( panel, control, start, stop, graph_start, graph_stop));
		EXCCHECK( DisplayAlarmsInKosuGraph( panel, control, start, stop, graph_start, graph_stop));

		if (used_grid>0)
		{
			uint32_t	i = 0;
			double		max = 100.0;
			int			color = 119;
			
			ptm = localtime(&grid[0]);

			EXCCHECKCVI( GetAxisScalingMode( panel, control, VAL_LEFT_YAXIS, NULL, NULL, &max));
			
			for ( i = 0; i < used_grid; i+=2)
			{
				EXCCHECKCVI( PlotLine ( panel, 
										control, 
										(double)grid[i], 
										0.0, 
										(double)grid[i], 
										max, 
										MakeColor(color,color,color))
										);
			}
		}
	}

Error:
	EXCRETHROW( pexception);  
} 


/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DisplayAlarmsInKosuGraph"
static SElExceptionPtr	DisplayAlarmsInKosuGraph(
	int panel,
	int control,
	time_t	t1,
	time_t	t2,
	time_t	gt1,
	time_t	gt2
)
{
	SElExceptionPtr	pexception = NULL;  
	int32_t			error = 0;  

	EXCCHECK( DisplayAlarmInKosuGraph( panel, control, VAL_YELLOW, 1, t1, t2, gt1, gt2));
	EXCCHECK( DisplayAlarmInKosuGraph( panel, control, VAL_GREEN, 2, t1, t2, gt1, gt2));
	EXCCHECK( DisplayAlarmInKosuGraph( panel, control, VAL_BLUE, 3, t1, t2, gt1, gt2));
	EXCCHECK( DisplayAlarmInKosuGraph( panel, control, VAL_RED, 4, t1, t2, gt1, gt2));

Error:
	EXCRETHROW( pexception);
} 

								  
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DisplayAlarmInKosuGraph"
static SElExceptionPtr DisplayAlarmInKosuGraph(
	int panel,
	int control,
	int	color,
	int32_t	alarm,
	time_t	t1,
	time_t	t2,
	time_t	gt1,
	time_t	gt2
)
{
	SElExceptionPtr		pexception = NULL;   
	int32_t				error = 0;  
	SIntervalResultPtr	pintervals = NULL;
	uint32_t			icount, count, i, hours;
	double				value, tvalue;
	time_t				start, stop;
	time_t				start_i, stop_i;

	EXCCHECK( intervalresult_new( &pintervals));
	EXCCHECK( gs_pKosu->GetAlarmInInterval( gs_pKosu, alarm, t1, t2, &pintervals)); 

	hours = (uint32_t)((gt2 - gt1)/(60*60));

	count = pintervals->Count( pintervals);

	start_i = gt1;
	stop_i = gt1 + (60*60);
	
	for (i=0, icount=0;  i<hours; i++, start_i+=(60*60), stop_i+=(60*60))
	{

		if(icount<count)
		{
			value = (pintervals->intervals+icount)->value;
			start = (pintervals->intervals+icount)->start;
			stop = (pintervals->intervals+icount)->stop;
		
			if(stop<stop_i)
			{
				/* calculate avarage value */
				icount++;
				if(icount<count)
				{
				  	tvalue = (pintervals->intervals+icount)->value;
					value = ( value*(stop-start_i) + tvalue*(stop_i-stop) )/3600;
				}
			}
		}

		if(count>0)
		{
			EXCCHECKCVI( PlotRectangle ( panel, 
										 control, 
										 (double)start_i, 
										 value+0.4, 
										 (double)stop_i, 
										 value-0.4, 
										 color, 
										 color)
										 );
		}
	}

Error:
	intervalresult_delete( &pintervals);
	EXCRETHROW( pexception); 
} /* kosu_GetRequiredKOSU */
								  
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DisplayAvgKOSUInKosuGraph"
static SElExceptionPtr DisplayAvgKOSUInKosuGraph(
	int panel,
	int control,
	time_t	t1,
	time_t	t2,
	time_t	gt1,
	time_t	gt2
)
{
	SElExceptionPtr	pexception = NULL;       
	int32_t			error = 0;
	time_t			tmp1, tmp2;
	int32_t			avg, max_alarms;
	uint32_t		pause_time = 0;
	
	EXCCHECK( gs_pKosu->GetMaxAlarm(gs_pKosu, &max_alarms) ); 
	
	for(tmp1=gt1; tmp1<gt2; tmp1=tmp2)
	{
		time_t	actual_time = time(NULL);
		tmp2 = tmp1 + 3600;
		tmp2 = (tmp2>gt2)? gt2 : tmp2;
		
		/* first bar length */
		if(tmp1==gt1 && t1>gt1) tmp1 = t1;
		
		/* last bar lenght */
		if(tmp2==gt2 && t2<gt2) tmp2 = t2;

		if(tmp1>=tmp2)
			break;
			
		if(tmp1<actual_time && tmp2>actual_time) /* actual hour */
		{
			EXCCHECK( gs_pKosu->GetAvgKOSU( gs_pKosu, tmp1, actual_time, &avg, &pause_time) );   
		}
		else if(tmp1<actual_time && tmp2<actual_time) /* hours before */ 
		{
			EXCCHECK( gs_pKosu->GetAvgKOSU( gs_pKosu, tmp1, tmp2, &avg, &pause_time) );		  
		}
		else /* next hours */ 
		{
			avg=0;					
			pause_time = 0;
		}
		
		if(error == DBS_STORAGE_WARNING_VALID_AVG_INTERVAL)
			avg = 0;
		
		if(avg > (max_alarms*2)) /* plot with "KOSU xxx" descroption */
		{
			char	label[128];
		
			EXCCHECKCVI( PlotRectangle ( panel, 
										 control, 
										 (double)tmp1, 
										 0, 
										 (double)tmp2, 
										 max_alarms*2-5, 
										 VAL_DK_GRAY, 
										 VAL_LT_GRAY));  
			
			/* first bar length back */  
			if(tmp1==t1) tmp1=gt1;
			
			/* last bar length back */
			if(tmp2==t2) tmp2=gt2;
			
			EXCCHECKCVI( PlotRectangle ( panel, 
										 control, 
										 (double)tmp1, 
										 max_alarms*2-5, 
										 (double)tmp2, 
										 max_alarms*2, 
										 VAL_DK_GRAY, 
										 VAL_TRANSPARENT));  
			
			sprintf(label, "KOSU %02d:%02d", (int32_t)(avg/60), (int32_t)(fmod(avg, 60)));
			
			EXCCHECKCVI( PlotText ( panel, 
				                    control, 
									(double)(tmp1+100), 
									max_alarms*2-5, 
									label, 
									VAL_APP_META_FONT, 
									VAL_RED, 
									VAL_TRANSPARENT));
		}
		else if(pause_time>0) /* plot with "PAUSE 00:00" description */ 
		{
			char	label[128];
			
			EXCCHECKCVI( PlotRectangle ( panel, 
										 control, 
										 (double)tmp1, 
										 5, 
										 (double)tmp2, 
										 (avg<5)? 5 : avg, 
										 VAL_DK_GRAY, 
										 VAL_LT_GRAY)); 
		
			/* first bar length back */  
			if(tmp1==t1) tmp1=gt1;
		
			/* last bar length back */
			if(tmp2==t2) tmp2=gt2;
		
			EXCCHECKCVI( PlotRectangle ( panel, 
										 control, 
										 (double)tmp1, 
										 0, 
										 (double)tmp2, 
										 5, 
										 VAL_DK_GRAY, 
										 VAL_TRANSPARENT));  
			
			sprintf(label, "Pause %02d:%02d", (int32_t)(pause_time/60), (int32_t)(fmod(pause_time, 60)));
			
			EXCCHECKCVI( PlotText ( panel, 
									control, 
									(double)(tmp1+100), 
									0, 
									label, 
									VAL_APP_META_FONT, 
									VAL_BLUE, 
									VAL_TRANSPARENT));
		}
		else
		{
			EXCCHECKCVI( PlotRectangle ( panel, 
										 control, 
										 (double)tmp1, 
										 0, 
										 (double)tmp2, 
										 avg, 
										 VAL_DK_GRAY, 
										 VAL_LT_GRAY)); 
		}
	}

Error:
	EXCRETHROW( pexception); 
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "DisplayAvgKOSUInKosuGraph"
static SElExceptionPtr ConvertTimestampFromANSI(time_t time, char* timestamp)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	struct tm*		ptm = NULL;

	ptm = localtime(&time);
	
	if(ptm)
	{
		sprintf(timestamp, "%d-%02d-%02d %02d:%02d:%02d", 
				(ptm->tm_year+1900), 
				ptm->tm_mon+1, 
				ptm->tm_mday, 
				ptm->tm_hour,
				ptm->tm_min,
				ptm->tm_sec);
	}
	else
	{
		EXCTHROW( -1, "Not Valid Time Format!");
	}

Error:
	EXCRETHROW( pexception);
}


