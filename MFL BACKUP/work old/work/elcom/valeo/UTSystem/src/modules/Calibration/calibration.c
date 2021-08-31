/****************************************************************************
 *                       C A L I B R A T I O N
 *---------------------------------------------------------------------------
 *    Copyright   2005 - 2007 ELCOM, a.s.  All Rights Reserved.        
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:       calibration.c                                                    
 * Purpose:     Calibration interface
 * Version:     1.0  
 ****************************************************************************/
#include "calibration.h"
#include <elerror.h>
#include <stdio.h> /* should be in elerror.h */
#include <stdlib.h>
#include <cviauto.h>
#include <msxmldom.h>
#include <guid.h>
#include <elstring.h>
#include <toolbox.h>
#include <utility.h>

#pragma warning(disable: 4996)

#define MAX_LENGTH_IVI_LOGICAL_NAME	64

/*****************************************************************************
 * Static no-exported functions declaration **********************************
 *****************************************************************************/
static	SElExceptionPtr	calibrationLock(
		SCalibrationPtr	pCalibration
);
static	SElExceptionPtr	calibrationUnlock(
		SCalibrationPtr	pCalibration
);
static	SElExceptionPtr calibrationCalculate(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scale,
		double					value,
		double*					value2
);
static	SElExceptionPtr calibrationLoad(
		struct _SCalibration*	me,
		const char*				calibSource
);
static	SElExceptionPtr calibrationSave(
		struct _SCalibration*	me,
		const char*				calibSource
);
	
static	SElExceptionPtr calibrationRemoveScale(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName
);
static	SElExceptionPtr calibrationAddLinearScale(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName,
		double					x1,
		double					y1,
		double					x2,
		double					y2
);
static	SElExceptionPtr calibrationAddLinearScaleEx(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName,
		double					gain,
		double					offset
);
static	SElExceptionPtr calibrationGetLinearScale(
	struct _SCalibration*	me,
	const char*				channel,
	const char*				scaleName,
	double*					x1,
	double*					y1,
	double*					x2,
	double*					y2
);
static	SElExceptionPtr calibrationGetLinearScaleEx(
		struct _SCalibration*	me,
		const char*				channel,
		const char*				scaleName,
		double*					gain,
		double*					offset
);
	
static	SElExceptionPtr calibrationNewList(struct _SCalibration* me);

static SElExceptionPtr	GetAllChannelsXML(
	SCalibrationPtr	me,
	CAObjHandle		document
);
static SElExceptionPtr	GetAllScalesXML(
	SCalibrationPtr	me,
	CAObjHandle		root,
	SChannelPtr		pChannel
);
static SElExceptionPtr	GetLinearScaleValuesXML(
	SCalibrationPtr	me,
	CAObjHandle		root,
	SScalePtr		pScale
);
static SElExceptionPtr	GetChannelFromName(
	SCalibrationPtr	me,
	const char*		channel,
	SChannelPtr*	pChannel
);
static SElExceptionPtr	GetScaleFromName(
	SCalibrationPtr	me,
	const char*		scale,
	SChannelPtr		pChannel,
	SScalePtr*		pScale
);
static SElExceptionPtr	GenerateXMLFile(
	SCalibrationPtr	me,
	SELStringPtr	pXMLString
);
SElExceptionPtr GenerateXMLChannels(SCalibrationPtr me, SELStringPtr pXMLString);
SElExceptionPtr GenerateXMLLinearScale(
	SCalibrationPtr	me,
	SELStringPtr	pXMLString,
	SScalePtr		pScale
);

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "calibration_new"
SElExceptionPtr calibration_new(
	SCalibrationPtr*	pCalibration
)
{
	SElExceptionPtr    	pexception = NULL;
	int32_t				error = 0;

	*pCalibration = calloc( 1, sizeof(SCalibration));
	EXCCHECKALLOC( *pCalibration);

	(*pCalibration)->Calculate		= calibrationCalculate;
	(*pCalibration)->Load			= calibrationLoad;
	(*pCalibration)->Save			= calibrationSave;
	(*pCalibration)->RemoveScale	= calibrationRemoveScale;
	(*pCalibration)->AddLinearScale	= calibrationAddLinearScale;
	(*pCalibration)->AddLinearScaleEx= calibrationAddLinearScaleEx;
	(*pCalibration)->GetLinearScale	= calibrationGetLinearScale;
	(*pCalibration)->GetLinearScaleEx= calibrationGetLinearScaleEx;
	(*pCalibration)->NewList		= calibrationNewList;

	(*pCalibration)->pChannelList = NULL;
	(*pCalibration)->channelListCount = 0;

Error:
	EXCRETHROW( pexception); 
}/* calibration_new */

/*---------------------------------------------------------------------------*/
#undef  __FUNC__
#define __FUNC__ "calibration_delete"
SElExceptionPtr	calibration_delete(
	SCalibrationPtr*	pCalibration
)
{
	int	i = 0;
	
	if ( pCalibration && *pCalibration )
	{
		for (i = 0; i < (*pCalibration)->channelListCount; i++)
		{
			free(((*pCalibration)->pChannelList[i].pScaleList));
		}
		free((*pCalibration)->pChannelList);

		free( *pCalibration);
		*pCalibration = NULL;
	}

/* Error: */
	return NULL;
} /* calibration_delete */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationCalculate"
static	SElExceptionPtr calibrationCalculate(
	struct _SCalibration*	me,
	const char*				channel,
	const char*				scale,
	double					value,
	double*					value2
)
{
	SElExceptionPtr	pexception = NULL;
	SChannelPtr		p2channel = NULL;
	SScalePtr		p2scale = NULL;
	char			message[256] = "";

	EXCCHECK( GetChannelFromName(me, channel, &p2channel));
	if (p2channel == NULL)
	{
		sprintf(message, "Unknown Channel Name (%s)", channel);
		EXCTHROW( CALIB_ERROR_UNKNOWN_CHANNEL_NAME, message);
	}
	EXCCHECK( GetScaleFromName(me, scale, p2channel, &p2scale));
	if (p2scale == NULL)
	{
		sprintf(message, "Unknown Scale Name (%s)", scale);
		EXCTHROW( CALIB_ERROR_UNKNOWN_SCALE_NAME, message);
	}

	if (value2)
	{
		switch (p2scale->scaleType)
		{
		case E_SCALE_TYPE_LINEAR:
			{
				double	gain = (p2scale->linear.y2 - p2scale->linear.y1) / (p2scale->linear.x2 - p2scale->linear.x1); 
				double	offset = p2scale->linear.y1 - gain * p2scale->linear.x1;
				
				*value2 = gain * value + offset;
			}
			break;
		}
	}
	
Error:
	EXCRETHROW( pexception);
} /* calibrationCalculate */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationLoad"
/**
 * Load stored calibration data.
 *
 * @param me - pointer to self
 * @param calibSource - full path to data source
 *                      file://file_name.xml - Xml file on disk
 *                      dbs://user:password@odbs_name/database/table - Database source ( not supported yet )
 */
static	SElExceptionPtr calibrationLoad(
	struct _SCalibration*	me,
	const char*				calibSource
)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			error = 0;
	char*			pnew_source = NULL;
	VARIANT			pfile_name_var = CA_VariantEmpty();
	VARIANT_BOOL	is_xml_success = 1;
	CAObjHandle		doc = 0;
	HRESULT			hr = S_OK;
	char			path[MAX_PATHNAME_LEN];

	pnew_source = calloc(strlen(calibSource) + 1, sizeof(char));
	EXCCHECKALLOC( pnew_source);

	me->NewList(me);

	if (!strncmp(calibSource, "file://", 7))
	{
		/* XML file with calibration data */
		strcpy(pnew_source, calibSource + 7);

		if(strlen(pnew_source)>1 && 0==strncmp(pnew_source+1, ":", 1)) /* absolute path */
		{
			strcpy(path, pnew_source);
		}
		else
		{
			EXCCHECKCVI( GetModuleDir (__CVIUserHInst, path)); 
			strcat(path, "\\");
			strcat(path, pnew_source);
		}

		/* Open XML File */
		CA_VariantSetCString(&pfile_name_var, path);
		EXCCHECKMSXML( MSXML_NewDOMDocumentIXMLDOMDocument(NULL, 1,
														   LOCALE_NEUTRAL, 0,
														   &doc));
		EXCCHECKMSXMLLOAD( MSXML_IXMLDOMDocumentload(doc, NULL, pfile_name_var,
													 &is_xml_success));
		/* Get All Channels */
		EXCCHECK( GetAllChannelsXML( me, doc));
	}
	else
	{
		EXCTHROW( CALIB_ERROR_UNKNOWN_SOURCE, "Unknown source type");
	}
	
Error:
	if (doc)
		CA_DiscardObjHandle(doc);

	if (!CA_VariantIsEmpty(&pfile_name_var))
		CA_VariantClear(&pfile_name_var);

	if (pnew_source)
		free(pnew_source);

	EXCRETHROW( pexception);
} /* calibrationLoad */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationSave"
/**
 * Store calibration data.
 *
 * @param me - pointer to self
 * @param calibSource - full path to data source
 *                      file://c:/file_name.xml - Xml file on disk
 *                      dbs://user:password@odbs_name/database/table - Database source ( not supported yet )
 */
static	SElExceptionPtr calibrationSave(
	struct _SCalibration*	me,
	const char*				calibSource
)
{
	SElExceptionPtr	pexception = NULL;
	char*			pnew_source = NULL;
	VARIANT			pfile_name_var = CA_VariantEmpty();
	CAObjHandle		doc = 0;
	HRESULT			hr = S_OK;
	FILE*			output = NULL;
	SELStringPtr	p2xml_data = NULL;

	pnew_source = calloc(strlen(calibSource) + 1, sizeof(char));
	EXCCHECKALLOC( pnew_source);

	if (!strncmp(calibSource, "file://", 7))
	{
		/* XML file with calibration data */
		strcpy(pnew_source, calibSource + 7);

		EXCCHECK( elstring_new("", &p2xml_data));

		/* Generate XML file */
		EXCCHECK( GenerateXMLFile(me, p2xml_data));

		/* Save XML File */
		output = fopen(pnew_source, "w+");
		fwrite(p2xml_data->GetPtr(p2xml_data), sizeof(char), p2xml_data->Size(p2xml_data), output);
		fclose(output);
	}
	else
	{
		EXCTHROW( CALIB_ERROR_UNKNOWN_SOURCE, "Unknown source type");
	}

Error:
	elstring_delete(&p2xml_data);
	EXCRETHROW( pexception);
} /* calibrationSave */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationRemoveScale"
static	SElExceptionPtr calibrationRemoveScale(
	struct _SCalibration*	me,
	const char*				channel,
	const char*				scaleName
)
{
	SElExceptionPtr	pexception = NULL;
	SChannelPtr		p2channel = NULL;
	SScalePtr		p2scale = NULL;
	char			message[256] = "";

	EXCCHECK( GetChannelFromName(me, channel, &p2channel));
	if (p2channel == NULL)
	{
		sprintf(message, "Unknown Channel Name (%s)", channel);
		EXCTHROW( CALIB_ERROR_UNKNOWN_CHANNEL_NAME, message);
	}
	EXCCHECK( GetScaleFromName(me, scaleName, p2channel, &p2scale));
	if (p2scale == NULL)
	{
		sprintf(message, "Unknown Scale Name (%s)", scaleName);
		EXCTHROW( CALIB_ERROR_UNKNOWN_SCALE_NAME, message);
	}

Error:
	EXCRETHROW( pexception);
} /* calibrationRemoveScale */
	
/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationAddLinearScale"
static	SElExceptionPtr calibrationAddLinearScale(
	struct _SCalibration*	me,
	const char*				channel,
	const char*				scaleName,
	double					x1,
	double					y1,
	double					x2,
	double					y2
)
{
	SElExceptionPtr	pexception = NULL;
	SChannelPtr		p2channel = NULL;
	SScalePtr		p2scale = NULL;
	
	EXCCHECK( GetChannelFromName(me, channel, &p2channel));
	if (p2channel == NULL)
	{
		me->pChannelList = realloc(me->pChannelList, (me->channelListCount + 1) * sizeof(SChannel));
		EXCCHECKALLOC( me->pChannelList);
		p2channel = &(me->pChannelList[me->channelListCount]);
		me->channelListCount++;
		strcpy(p2channel->channelName, channel);
		p2channel->pScaleList = NULL;
		p2channel->scaleListCount = 0;
		
	}
	EXCCHECK( GetScaleFromName(me, scaleName, p2channel, &p2scale));
	if (p2scale == NULL)
	{
		p2channel->pScaleList = realloc(p2channel->pScaleList, (p2channel->scaleListCount + 1) * sizeof(SScale));
		EXCCHECKALLOC( p2channel->pScaleList);
		p2scale = &(p2channel->pScaleList[p2channel->scaleListCount]);
		p2channel->scaleListCount++;
		strcpy(p2scale->scaleName, scaleName);
		p2scale->scaleType = E_SCALE_TYPE_LINEAR;
	}

	p2scale->linear.x1 = x1;
	p2scale->linear.x2 = x2;
	p2scale->linear.y1 = y1;
	p2scale->linear.y2 = y2;
	
Error:
	EXCRETHROW( pexception);
} /* calibrationAddLinearScale */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationAddLinearScaleEx"
static	SElExceptionPtr calibrationAddLinearScaleEx(
	SCalibrationPtr			me,
	const char*				channel,
	const char*				scaleName,
	double					gain,
	double					offset
)
{
	SElExceptionPtr	pexception = NULL;
	SChannelPtr		p2channel = NULL;
	SScalePtr		p2scale = NULL;
	
	EXCCHECK( GetChannelFromName(me, channel, &p2channel));
	if (p2channel == NULL)
	{
		me->pChannelList = realloc(me->pChannelList, (me->channelListCount + 1) * sizeof(SChannel));
		EXCCHECKALLOC( me->pChannelList);
		p2channel = &(me->pChannelList[me->channelListCount]);
		me->channelListCount++;
		strcpy(p2channel->channelName, channel);
		p2channel->pScaleList = NULL;
		p2channel->scaleListCount = 0;
		
	}
	EXCCHECK( GetScaleFromName(me, scaleName, p2channel, &p2scale));
	if (p2scale == NULL)
	{
		p2channel->pScaleList = realloc(p2channel->pScaleList, (p2channel->scaleListCount + 1) * sizeof(SScale));
		EXCCHECKALLOC( p2channel->pScaleList);
		p2scale = &(p2channel->pScaleList[p2channel->scaleListCount]);
		p2channel->scaleListCount++;
		strcpy(p2scale->scaleName, scaleName);
		p2scale->scaleType = E_SCALE_TYPE_LINEAR;
	}

	p2scale->linear.x1 = 0;
	p2scale->linear.x2 = 1;
	p2scale->linear.y1 = gain * p2scale->linear.x1 + offset;
	p2scale->linear.y2 = gain * p2scale->linear.x2 + offset;
	
Error:
	EXCRETHROW( pexception);
} /* calibrationAddLinearScaleEx */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationGetLinearScale"
static	SElExceptionPtr calibrationGetLinearScale(
	struct _SCalibration*	me,
	const char*				channel,
	const char*				scaleName,
	double*					x1,
	double*					y1,
	double*					x2,
	double*					y2
)
{
	SElExceptionPtr	pexception = NULL;
	SChannelPtr		p2channel = NULL;
	SScalePtr		p2scale = NULL;
	char			message[256] = "";

	EXCCHECK( GetChannelFromName(me, channel, &p2channel));
	if (p2channel == NULL)
	{
		sprintf(message, "Unknown Channel Name (%s)", channel);
		EXCTHROW( CALIB_ERROR_UNKNOWN_CHANNEL_NAME, message);
	}
	EXCCHECK( GetScaleFromName(me, scaleName, p2channel, &p2scale));
	if (p2scale == NULL)
	{
		sprintf(message, "Unknown Scale Name (%s)", scaleName);
		EXCTHROW( CALIB_ERROR_UNKNOWN_SCALE_NAME, message);
	}

	if (x1)
		*x1 = p2scale->linear.x1;

	if (x2)
		*x2 = p2scale->linear.x2;

	if (y1)
		*y1 = p2scale->linear.y1;

	if (y2)
		*y2 = p2scale->linear.y2;
	
Error:
	EXCRETHROW( pexception);
} /* calibrationGetLinearScale */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationGetLinearScaleEx"
static	SElExceptionPtr calibrationGetLinearScaleEx(
	struct _SCalibration*	me,
	const char*				channel,
	const char*				scaleName,
	double*					gain,
	double*					offset
)
{
	SElExceptionPtr	pexception = NULL;
	SChannelPtr		p2channel = NULL;
	SScalePtr		p2scale = NULL;
	double			count_gain = 1.0;
	double			count_offset = 0.0;
	char			message[256] = "";

	EXCCHECK( GetChannelFromName(me, channel, &p2channel));
	if (p2channel == NULL)
	{
		sprintf(message, "Unknown Channel Name (%s)", channel);
		EXCTHROW( CALIB_ERROR_UNKNOWN_CHANNEL_NAME, message);
	}
	EXCCHECK( GetScaleFromName(me, scaleName, p2channel, &p2scale));
	if (p2scale == NULL)
	{
		sprintf(message, "Unknown Scale Name (%s)", scaleName);
		EXCTHROW( CALIB_ERROR_UNKNOWN_SCALE_NAME, message);
	}

	count_gain = (p2scale->linear.y2 - p2scale->linear.y1) / (p2scale->linear.x2 - p2scale->linear.x1); 
	count_offset = p2scale->linear.y1 - count_gain * p2scale->linear.x1;

	if (gain)
		*gain = count_gain;

	if (offset)
		*offset = count_offset;
	
Error:
	EXCRETHROW( pexception);
} /* calibrationGetLinearScaleEx */

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "calibrationNewList"
static	SElExceptionPtr calibrationNewList(struct _SCalibration* me)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0;

	for (i = 0; i < me->channelListCount; i++)
	{
		free(me->pChannelList[i].pScaleList);
		me->pChannelList[i].pScaleList = NULL;
		me->channelListCount = 0;
	}

	free(me->pChannelList);
	me->pChannelList = NULL;
	me->channelListCount = 0;
	
//Error:
	EXCRETHROW( pexception);
} /* calibrationNewList */	

/*===========================================================================*/
#undef __FUNC__
#define __FUNC__ "GetAllChannelsXML"
static SElExceptionPtr	GetAllChannelsXML(
	SCalibrationPtr	me,
	CAObjHandle		document
)
{
	SElExceptionPtr	pexception = NULL;
	char*			channel_name = NULL;
	CAObjHandle		channel_list = 0;
	CAObjHandle		channel_node = 0;
	CAObjHandle		channel_name_node = 0;
	HRESULT			hr = S_OK;

	EXCCHECKMSXML( MSXML_IXMLDOMDocumentselectNodes(document, NULL,
					"//modules/module[@id = '" MODULE_CALIBRATION "']/channel",
					&channel_list));
	
	if (channel_list)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset(channel_list, NULL));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(channel_list, NULL, &channel_node));
		while (channel_node != 0)
		{
			/* Get Channel Name */
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(channel_node, NULL, "name", &channel_name_node));
			if (channel_name_node)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(channel_name_node, NULL, &channel_name));
				if (channel_name)
				{
					/* Add New Channel Into List */
					me->pChannelList = realloc(me->pChannelList, (me->channelListCount + 1) * sizeof(SChannel));
					EXCCHECKALLOC( me->pChannelList);
					strcpy(me->pChannelList[me->channelListCount].channelName, channel_name);
					me->pChannelList[me->channelListCount].pScaleList = NULL;
					me->pChannelList[me->channelListCount].scaleListCount = 0;
					me->channelListCount++;

					/* Get All Scales */
					EXCCHECK( GetAllScalesXML(me, channel_node, &(me->pChannelList[me->channelListCount - 1])));

					CA_FreeMemory(channel_name); channel_name = NULL;
				}
				CA_DiscardObjHandle(channel_name_node); channel_name_node = 0;
			}

			CA_DiscardObjHandle(channel_node); channel_node = 0;
			EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(channel_list, NULL, &channel_node));
		}
		CA_DiscardObjHandle(channel_list);
		channel_list = 0;
	}

Error:
	if (channel_name)
		CA_FreeMemory(channel_name);
	
	if (channel_name_node)
		CA_DiscardObjHandle(channel_name_node);

	if (channel_list)
		CA_DiscardObjHandle(channel_list);

	if (channel_node)
		CA_DiscardObjHandle(channel_node);

	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetAllScalesXML"
static SElExceptionPtr	GetAllScalesXML(
	SCalibrationPtr	me,
	CAObjHandle		root,
	SChannelPtr		pChannel
)
{
	SElExceptionPtr	pexception = NULL;
	char*			scale_name = NULL;
	char*			scale_type = NULL;
	CAObjHandle		scale_list = 0;
	CAObjHandle		scale_node = 0;
	CAObjHandle		scale_name_node = 0;
	CAObjHandle		scale_type_node = 0;
	HRESULT			hr = S_OK;

	/* Get All Scales for Active Channel */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectNodes(root, NULL, "scale", &scale_list));
	
	if (scale_list)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListreset(scale_list, NULL));
		EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(scale_list, NULL, &scale_node));
		while (scale_node != 0)
		{
			/* Get Scale Type */
			EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(scale_node, NULL, "@type", &scale_type_node));
			if (scale_type_node)
			{
				EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(scale_type_node, NULL, &scale_type));
				if (scale_type)
				{
					if (!strcmp(scale_type, "linear"))
					{
						/* Get Scale Name */
						EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(scale_node, NULL, "name", &scale_name_node));
						if (scale_name_node)
						{
							EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(scale_name_node, NULL, &scale_name));
							if (scale_name)
							{
								/* Add New Scale Into Channel */
								pChannel->pScaleList = realloc(pChannel->pScaleList, (pChannel->scaleListCount + 1) * sizeof(SScale));
								EXCCHECKALLOC( pChannel->pScaleList);
								strcpy(pChannel->pScaleList[pChannel->scaleListCount].scaleName, scale_name);
								pChannel->pScaleList[pChannel->scaleListCount].scaleType = E_SCALE_TYPE_LINEAR;
								pChannel->pScaleList[pChannel->scaleListCount].linear.x1 = 0;
								pChannel->pScaleList[pChannel->scaleListCount].linear.y1 = 0;
								pChannel->pScaleList[pChannel->scaleListCount].linear.x2 = 1;
								pChannel->pScaleList[pChannel->scaleListCount].linear.y2 = 1;
								pChannel->scaleListCount++;

								/* TODO: Get Scale's Points */
								EXCCHECK( GetLinearScaleValuesXML( me, scale_node, &(pChannel->pScaleList[pChannel->scaleListCount - 1])));

								CA_FreeMemory(scale_name); scale_name = NULL;
							}
							CA_DiscardObjHandle(scale_name_node); scale_name_node = 0;
						}

						CA_DiscardObjHandle(scale_node); scale_node = 0;
						EXCCHECKMSXML( MSXML_IXMLDOMNodeListnextNode(scale_list, NULL, &scale_node));
					}
					else
					{
						EXCTHROW( CALIB_ERROR_UNKNOWN_TYPE, "Unsupported Scale Type");
					}

					CA_FreeMemory(scale_type); scale_type = NULL;
				}
				CA_DiscardObjHandle(scale_type_node); scale_type_node = 0;
			}
			else
			{
				EXCTHROW( CALIB_ERROR_UNKNOWN_TYPE, "Type of Scale Not Defined");
			}
		}
		CA_DiscardObjHandle(scale_list);
		scale_list = 0;
	}

Error:
	if (scale_name)
		CA_FreeMemory(scale_name);
	
	if (scale_name_node)
		CA_DiscardObjHandle(scale_name_node);

	if (scale_type)
		CA_FreeMemory(scale_type);
	
	if (scale_type_node)
		CA_DiscardObjHandle(scale_type_node);

	if (scale_list)
		CA_DiscardObjHandle(scale_list);

	if (scale_node)
		CA_DiscardObjHandle(scale_node);

	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetLinearScaleValuesXML"
static SElExceptionPtr	GetLinearScaleValuesXML(
	SCalibrationPtr	me,
	CAObjHandle		root,
	SScalePtr		pScale
)
{
	SElExceptionPtr	pexception = NULL;
	char*			scale_value = NULL;
	CAObjHandle		scale_value_node = 0;
	HRESULT			hr = S_OK;

	/* Get Uncalibrated Point 1 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(root, NULL, "uncalibrated/value1", &scale_value_node));
	
	if (scale_value_node)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(scale_value_node, NULL, &scale_value));
		if (scale_value)
		{
			pScale->linear.x1 = atof(scale_value);
			CA_FreeMemory(scale_value); scale_value = NULL;
		}
		CA_DiscardObjHandle(scale_value_node); scale_value_node = 0;
	}
	else
	{
		EXCTHROW( CALIB_ERROR_MISSING_SCALE_VALUES, "Missing Scale Values");
	}

	/* Get Uncalibrated Point 1 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(root, NULL, "uncalibrated/value2", &scale_value_node));
	
	if (scale_value_node)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(scale_value_node, NULL, &scale_value));
		if (scale_value)
		{
			pScale->linear.x2 = atof(scale_value);
			CA_FreeMemory(scale_value); scale_value = NULL;
		}
		CA_DiscardObjHandle(scale_value_node); scale_value_node = 0;
	}
	else
	{
		EXCTHROW( CALIB_ERROR_MISSING_SCALE_VALUES, "Missing Scale Values");
	}

	/* Get Calibrated Point 1 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(root, NULL, "calibrated/value1", &scale_value_node));
	
	if (scale_value_node)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(scale_value_node, NULL, &scale_value));
		if (scale_value)
		{
			pScale->linear.y1 = atof(scale_value);
			CA_FreeMemory(scale_value); scale_value = NULL;
		}
		CA_DiscardObjHandle(scale_value_node); scale_value_node = 0;
	}
	else
	{
		EXCTHROW( CALIB_ERROR_MISSING_SCALE_VALUES, "Missing Scale Values");
	}

	/* Get Calibrated Point 2 */
	EXCCHECKMSXML( MSXML_IXMLDOMNodeselectSingleNode(root, NULL, "calibrated/value2", &scale_value_node));
	
	if (scale_value_node)
	{
		EXCCHECKMSXML( MSXML_IXMLDOMNodeGettext(scale_value_node, NULL, &scale_value));
		if (scale_value)
		{
			pScale->linear.y2 = atof(scale_value);
			CA_FreeMemory(scale_value); scale_value = NULL;
		}
		CA_DiscardObjHandle(scale_value_node); scale_value_node = 0;
	}
	else
	{
		EXCTHROW( CALIB_ERROR_MISSING_SCALE_VALUES, "Missing Scale Values");
	}

Error:
	if (scale_value)
		CA_FreeMemory(scale_value);
	
	if (scale_value_node)
		CA_DiscardObjHandle(scale_value_node);

	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetChannelFromName"
static SElExceptionPtr	GetChannelFromName(
	SCalibrationPtr	me,
	const char*		channel,
	SChannelPtr*	pChannel
)
{
	SElExceptionPtr	pexception = NULL;
	SChannelPtr		ret_channel = NULL;
	int32_t			i = 0;

	for (i = 0; i < me->channelListCount; i++)
	{
		if (!strcmp(me->pChannelList[i].channelName, channel))
			break;
	}

	if (i < me->channelListCount)
	{
		ret_channel = &(me->pChannelList[i]);
	}

//Error:
	if (pChannel)
		*pChannel = ret_channel;

	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GetScaleFromName"
static SElExceptionPtr	GetScaleFromName(
	SCalibrationPtr	me,
	const char*		scale,
	SChannelPtr		pChannel,
	SScalePtr*		pScale
)
{
	SElExceptionPtr	pexception = NULL;
	SScalePtr		ret_scale = NULL;
	int32_t			i = 0;

	for (i = 0; i < pChannel->scaleListCount; i++)
	{
		if (!strcmp(pChannel->pScaleList[i].scaleName, scale))
			break;
	}

	if (i < pChannel->scaleListCount)
	{
		ret_scale = &(pChannel->pScaleList[i]);
	}

//Error:
	if (pScale)
		*pScale = ret_scale;

	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GenerateXMLFile"
static SElExceptionPtr	GenerateXMLFile(
	SCalibrationPtr	me,
	SELStringPtr	pXMLString
)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( pXMLString->Empty(pXMLString));
	EXCCHECK( pXMLString->Append(pXMLString, "<?xml version=\"1.0\" encoding=\"windows-1250\" ?>\n"));
	EXCCHECK( pXMLString->Append(pXMLString, "<configuration>\n"));
	EXCCHECK( pXMLString->Append(pXMLString, "	<modules>\n"));
	EXCCHECK( pXMLString->Append(pXMLString, "		<module id=\"" MODULE_CALIBRATION "\">\n"));
	EXCCHECK( pXMLString->Append(pXMLString, "			<name>Calibration</name>\n"));
	EXCCHECK( GenerateXMLChannels(me, pXMLString));
	EXCCHECK( pXMLString->Append(pXMLString, "		</module>\n"));
	EXCCHECK( pXMLString->Append(pXMLString, "	</modules>\n"));
	EXCCHECK( pXMLString->Append(pXMLString, "</configuration>\n"));

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GenerateXMLChannels"
SElExceptionPtr GenerateXMLChannels(SCalibrationPtr me, SELStringPtr pXMLString)
{
	SElExceptionPtr	pexception = NULL;
	int32_t			i = 0,
					j = 0;

	for (i = 0; i < me->channelListCount; i++)
	{
		EXCCHECK( pXMLString->Append(pXMLString, "			<channel>\n"));
		EXCCHECK( pXMLString->Format(pXMLString, "				<name>%s</name>\n", me->pChannelList[i].channelName));
		for (j = 0; j < me->pChannelList[i].scaleListCount; j++)
		{
			switch (me->pChannelList[i].pScaleList[j].scaleType)
			{
			case E_SCALE_TYPE_LINEAR:
				EXCCHECK( GenerateXMLLinearScale(me, pXMLString, &(me->pChannelList[i].pScaleList[j])));
				break;
			}
		}
		EXCCHECK( pXMLString->Append(pXMLString, "			</channel>\n"));
	}

Error:
	EXCRETHROW( pexception);
}

/*---------------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "GenerateXMLLinearScale"
SElExceptionPtr GenerateXMLLinearScale(
	SCalibrationPtr	me,
	SELStringPtr	pXMLString,
	SScalePtr		pScale
)
{
	SElExceptionPtr	pexception = NULL;

	EXCCHECK( pXMLString->Append(pXMLString, "				<scale type=\"linear\">\n"));
	EXCCHECK( pXMLString->Format(pXMLString, "					<name>%s</name>\n", pScale->scaleName));
	EXCCHECK( pXMLString->Append(pXMLString, "					<uncalibrated>\n"));
	EXCCHECK( pXMLString->Format(pXMLString, "						<value1>%lf</value1>\n", pScale->linear.x1));
	EXCCHECK( pXMLString->Format(pXMLString, "						<value2>%lf</value2>\n", pScale->linear.x2));
	EXCCHECK( pXMLString->Append(pXMLString, "					</uncalibrated>\n"));
	EXCCHECK( pXMLString->Append(pXMLString, "					<calibrated>\n"));
	EXCCHECK( pXMLString->Format(pXMLString, "						<value1>%lf</value1>\n", pScale->linear.y1));
	EXCCHECK( pXMLString->Format(pXMLString, "						<value2>%lf</value2>\n", pScale->linear.y2));
	EXCCHECK( pXMLString->Append(pXMLString, "					</calibrated>\n"));
	EXCCHECK( pXMLString->Append(pXMLString, "				</scale>\n"));

Error:
	EXCRETHROW( pexception);
}
