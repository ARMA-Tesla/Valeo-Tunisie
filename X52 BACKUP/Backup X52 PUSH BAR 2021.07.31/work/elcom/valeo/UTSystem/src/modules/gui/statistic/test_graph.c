#include <bklib/BKLib2.h>
#include <ansi_c.h>
#include <userint.h>
#include <elstring.h>
#include <dbs/dbs.h> 
#include <gui/report/calendar.h>
#include "test_graph.h"
#include "statistic.h"

extern int				gs_panel[]; 
extern CAObjHandle		gs_treeHandle; 
extern SDBSStoragePtr	gs_pStorage;
extern SDBSPtr    		gs_pDbs;
static SEProductPtr		gs_productlist = NULL;
static int32_t			gs_productlistSize = 0;

static SElExceptionPtr InitializeStatList(int32_t product_id, int32_t test_id, int32_t parameter_id);
static SElExceptionPtr PlotGraph(int32_t product_id, int32_t test_id, int32_t parameter_id);

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "TestTreeItemLevel"
static SElExceptionPtr	TestTreeItemLevel(char* keyNode, int32_t* level)
{
	SElExceptionPtr pexception = NULL;    
	int32_t 		error = 0;
	char*			result = NULL;
	char			keynode[32] = "";
	
	*level = 0;
	
	strcpy(keynode, keyNode);
	result = strtok(keynode, "_");			
	while( result != NULL)
	{
		*level = *level + 1;
		result = strtok(NULL, "_");
	}
	
/* Error: */
	EXCRETHROW( pexception);       
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "StatisticTestGraph"
SElExceptionPtr	StatisticTestGraph(void)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	
	EXCCHECK( InitializeCal( gs_panel[STAT], 
							 STAT_MONTH_DISPLAY, 
							 STAT_CAL_DISPLAY, 
							 STAT_MONTH_NEXT,
							 STAT_MONTH_PREV,
							 -1, -1));

	EXCCHECKCVI( SetCtrlAttribute(gs_panel[STAT], STAT_MONTH_DISPLAY, ATTR_DIMMED, TRUE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[STAT], STAT_CAL_DISPLAY, ATTR_DIMMED, TRUE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[STAT], STAT_MONTH_NEXT, ATTR_DIMMED, TRUE));
	EXCCHECKCVI( SetCtrlAttribute(gs_panel[STAT], STAT_MONTH_PREV, ATTR_DIMMED, TRUE));

	EXCCHECK( InitializeStatList(0, 0, 0));
	
	EXCCHECKCVI( DeleteGraphPlot (gs_panel[STAT], STAT_GRAPH, -1, VAL_IMMEDIATE_DRAW));     
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_YNAME, "Count"));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XNAME, "Value"));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XGRID_VISIBLE, TRUE));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XUSE_LABEL_STRINGS, FALSE));
	EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XPRECISION, 0)); 
	EXCCHECKCVI( SetAxisScalingMode(gs_panel[STAT], STAT_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 1));
	EXCCHECKCVI( SetAxisScalingMode(gs_panel[STAT], STAT_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, 0, 1));  
	EXCCHECKCVI( SetAxisScalingMode(gs_panel[STAT], STAT_GRAPH, VAL_LEFT_YAXIS, VAL_AUTOSCALE, 0, 1)); 

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "StatisticTestGraphClean"
SElExceptionPtr	StatisticTestGraphClean(void)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	
	if(gs_productlist) 
		EXCCHECK(gs_pStorage->TestListFree(gs_pStorage, &gs_productlist, gs_productlistSize));

	gs_productlistSize = 0;

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "BkTreeTestNodeClick"
SElExceptionPtr BkTreeTestNodeClick (char *keyNode)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int32_t			product_id, test_id, parameter_id, nb;
	int32_t			found = 0;
	HRESULT			hr;
	short int		AXvalue;
	char			level[64];
	short int		has_child;
	
	found = sscanf(keyNode, "%d_%d_%d", &product_id, &test_id, &parameter_id);
	EXCCHECK( TestTreeItemLevel(keyNode, &found) );	
	if(found==1)
	{
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeHasChildren(gs_treeHandle, NULL, keyNode, &has_child));
		if(!has_child)
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetRedraw(gs_treeHandle, NULL, 0, &AXvalue));

			EXCCHECK( InitializeStatList(product_id, 0, 0));
			
			EXCCHECKCOM( BKLib2__DBKTreeExpandLevel (gs_treeHandle, NULL, 2, &AXvalue));
			EXCCHECKCOM( BKLib2__DBKTreeExpand(gs_treeHandle, NULL, keyNode, &AXvalue));
			EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(gs_treeHandle, NULL, keyNode));
			EXCCHECKCOM( BKLib2__DBKTreeEnsureVisible(gs_treeHandle, NULL, keyNode));
			EXCCHECKCOM( BKLib2__DBKTreeSetRedraw(gs_treeHandle, NULL, 1, &AXvalue));
			EXCCHECKCOM( BKLib2__DBKTreeSetScrollBarVisible(gs_treeHandle, NULL, 0, 1));
		}
	}
	else if(found==2)
	{
		EXCCHECKCOM( BKLib2__DBKTreeGetNodeHasChildren(gs_treeHandle, NULL, keyNode, &has_child));
		if(!has_child)
		{
			EXCCHECKCOM( BKLib2__DBKTreeSetRedraw(gs_treeHandle, NULL, 0, &AXvalue));
			
			EXCCHECK( InitializeStatList(product_id, test_id, 0));
			
			EXCCHECKCOM( BKLib2__DBKTreeExpandLevel (gs_treeHandle, NULL, 2, &AXvalue));
			sprintf(level, "%d", product_id);
			EXCCHECKCOM( BKLib2__DBKTreeExpand(gs_treeHandle, NULL, level, &AXvalue));
			EXCCHECKCOM( BKLib2__DBKTreeExpand(gs_treeHandle, NULL, keyNode, &AXvalue));
			EXCCHECKCOM( BKLib2__DBKTreeSetSelectedItem(gs_treeHandle, NULL, keyNode));
			EXCCHECKCOM( BKLib2__DBKTreeEnsureVisible(gs_treeHandle, NULL, keyNode));
			EXCCHECKCOM( BKLib2__DBKTreeSetRedraw(gs_treeHandle, NULL, 1, &AXvalue));
			EXCCHECKCOM( BKLib2__DBKTreeSetScrollBarVisible(gs_treeHandle, NULL, 0, 1));
		}
	}
	else if(found==3) 
	{	
		EXCCHECK( PlotGraph(product_id, test_id, parameter_id) );
	}
	
Error:
	EXCDISPLAY( pexception);
	return 0;
}

#define TREE_ROW_SIZE 		1024 

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "InitializeStatList"
static SElExceptionPtr InitializeStatList(
	int32_t product_id,
	int32_t test_id,
	int32_t parameter_id
)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t 		error = 0;
	HRESULT			hr;
	SELStringPtr	pelstring = NULL;
	short int		AXvalue;
	int				i, j, k;
	char			row[TREE_ROW_SIZE] = "";    
	char			path[512] = "";
	
	EXCCHECK( elstring_new("", &pelstring));
	EXCCHECK( pelstring->Append(pelstring, "root|Product^w315$desc|Description^w90\n" ) );
	EXCCHECK( pelstring->Append(pelstring, "|root|Product\n" ) ); 
	
	if( gs_pStorage)
	{
		EXCDBGSTR( gs_pStorage->TestListGet(gs_pStorage, product_id, test_id, parameter_id, &gs_productlist, &gs_productlistSize));
		
		for(i=0; i<gs_productlistSize; i++)
		{
			EXCDBGSTR( gs_pDbs->ProductList->ProductGetIdPath(gs_pDbs->ProductList, gs_productlist[i].product_id, path, 2));
			sprintf(row, "root|%d|%s$%s\n", 
						 gs_productlist[i].product_id, 
						 gs_productlist[i].name,
						 path);
			EXCCHECK( pelstring->Append(pelstring, row) );
		
			for(j=0; j<gs_productlist[i].testSize; j++)
			{
				sprintf(row, "%d|%d_%d|%s\n", 
							 gs_productlist[i].product_id, 
							 gs_productlist[i].product_id,
							 gs_productlist[i].test[j].test_id, 
							 gs_productlist[i].test[j].description);
				EXCCHECK( pelstring->Append(pelstring, row) ); 
			
				for(k=0; k<gs_productlist[i].test[j].parameterSize; k++)
				{
					sprintf(row, "%d_%d|%d_%d_%d|%s$%s\n", 
								 gs_productlist[i].product_id, 
								 gs_productlist[i].test[j].test_id, 
								 gs_productlist[i].product_id, 
								 gs_productlist[i].test[j].test_id, 
								 gs_productlist[i].test[j].parameter[k].parameter_id,
								 gs_productlist[i].test[j].parameter[k].description,
								 gs_productlist[i].test[j].parameter[k].value);
					EXCCHECK( pelstring->Append(pelstring, row) ); 	
				}
			}
		}
	}
	
	EXCCHECKCOM( BKLib2__DBKTreeFromString (gs_treeHandle, NULL, pelstring->GetPtr(pelstring), "$", "\n", &AXvalue)); 
	if(product_id==0)
		EXCCHECKCOM( BKLib2__DBKTreeExpandLevel (gs_treeHandle, NULL, 2, &AXvalue));
	
Error:
	elstring_delete(&pelstring);
	EXCRETHROW( pexception);
}

#define STAT_SIZE_OF_ARRAY		2000+1
#define STAT_NB_OF_INTERVAL		10
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "PlotGraph"
static SElExceptionPtr PlotGraph(int32_t product_id, int32_t test_id, int32_t parameter_id)
{
	SElExceptionPtr	pexception = NULL; 
	int32_t			error = 0;
	int32_t			vtyp, count = STAT_SIZE_OF_ARRAY;
	double			column_size;
	int				i, j, element_count;
	char			label[32], format[32];
	
	EXCCHECKCVI( DeleteGraphPlot (gs_panel[STAT], STAT_GRAPH, -1, VAL_IMMEDIATE_DRAW));   
	
	EXCCHECK( gs_pStorage->GetTestVType(gs_pStorage, product_id, test_id, parameter_id, &vtyp, &count) );

	switch(vtyp)
	{
		case E_DVT_INT32:
			{
				int32_t	imax, imin; 
				double	davg;
				int32_t	ivalue[STAT_SIZE_OF_ARRAY]; 

				memset(ivalue, 0, sizeof(int32_t)*STAT_SIZE_OF_ARRAY);
				EXCCHECK( gs_pStorage->GetTestIValue(gs_pStorage, ivalue, &count, &imax, &imin, &davg) );
				
				column_size = (double)(imax-imin)/(double)STAT_NB_OF_INTERVAL;
				
				if(column_size<1.0)
					column_size = 1.0; /* for int */
				
				for(i=0; i<STAT_NB_OF_INTERVAL+1; i++)
				{
					element_count = 0;
					
					for(j=0; j<count; j++)
					{
						if( ivalue[j] < (imin + i*column_size + column_size/2) && ivalue[j] >= (imin + i*column_size - column_size/2) )
							element_count++;	
					}
					
					EXCCHECKCVI( PlotRectangle (gs_panel[STAT], STAT_GRAPH, 
								   imin + i*column_size - column_size/2, 
								   0, 
								   imin + i*column_size + column_size/2, 
								   element_count, 
								   VAL_LT_GRAY, 
								   VAL_BLUE)); 
				}
				
				EXCCHECKCVI( SetAxisScalingMode(gs_panel[STAT], 
									 STAT_GRAPH, 
									 VAL_BOTTOM_XAXIS, 
									 VAL_MANUAL, 
									 imin - column_size, 
									 imax + column_size));
				
				EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XPRECISION, 0)); 
			}		 
			break;
		case E_DVT_REAL64:
			{
				double	dmax, dmin, davg;
				double	dvalue[STAT_SIZE_OF_ARRAY];

				memset(dvalue, 0, sizeof(double)*STAT_SIZE_OF_ARRAY);
				EXCCHECK( gs_pStorage->GetTestDValue(gs_pStorage, dvalue, &count, &dmax, &dmin, &davg) );

				if(dmin>1.0e+9)
				{
					EXCCHECKCVI( SetAxisScalingMode(gs_panel[STAT], 
									 STAT_GRAPH, 
									 VAL_BOTTOM_XAXIS, 
									 VAL_MANUAL, 
									 0.0, 
									 1.0));
					EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XPRECISION, 0));
					EXCCHECKCVI( SetAxisScalingMode(gs_panel[STAT], STAT_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, 0, 1));  
					EXCCHECKCVI( SetAxisScalingMode(gs_panel[STAT], STAT_GRAPH, VAL_LEFT_YAXIS, VAL_AUTOSCALE, 0, 1)); 
					break;
				}

				column_size = (double)(dmax-dmin)/(double)STAT_NB_OF_INTERVAL;
				if(column_size==0.0)
					column_size = 0.1; 

				for(i=0; i<STAT_NB_OF_INTERVAL+1; i++)
				{
					element_count = 0;
					
					for(j=0; j<count; j++)
					{
						if( dvalue[j] < (dmin + i*column_size + column_size/2) && dvalue[j] >= (dmin + i*column_size - column_size/2) )
							element_count++;	
					}
					
					EXCCHECKCVI( PlotRectangle (gs_panel[STAT], STAT_GRAPH, 
								   dmin + i*column_size - column_size/2, 
								   0, 
								   dmin + i*column_size + column_size/2, 
								   element_count, 
								   VAL_LT_GRAY, 
								   VAL_BLUE)); 
				}
				
				EXCCHECKCVI( SetAxisScalingMode(gs_panel[STAT], 
									 STAT_GRAPH, 
									 VAL_BOTTOM_XAXIS, 
									 VAL_MANUAL, 
									 dmin - column_size, 
									 dmax + column_size));
				
				if((dmax-dmin)<1.0)
					EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XPRECISION, 3));
				else if((dmax-dmin)<10.0)
					EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XPRECISION, 2));
				else if((dmax-dmin)<100.0)   
					EXCCHECKCVI( SetCtrlAttribute (gs_panel[STAT], STAT_GRAPH, ATTR_XPRECISION, 1));
			}
			break;
	}

Error:
	EXCRETHROW( pexception);
}
