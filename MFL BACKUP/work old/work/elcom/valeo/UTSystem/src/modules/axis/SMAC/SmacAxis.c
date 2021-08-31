#include <rs232.h>
#include <ansi_c.h>
#include <userint.h>
#include "SmacAxis.h"
#include <stdio.h>
#include <utility.h>
#include <formatio.h>
#include "CVI_SMAC_CAN.h"

#pragma warning( push)
#pragma warning( disable: 4100 4127)

#define AXIS_TIMEOUT				500
	
int ID_SMAC=0;//0==>5 


// Control codes for the Gateway axis

#define HEADER      0X3A       // Header function
#define LF      	0X0A      // Line feed
#define CR      	0X0D      // Carriage return
#define END      	0X00      // end string 

#define CR_CHAR			'\x0D'	// CR character
#define LF_CHAR			'\x0A'  // LF character


#undef __FUNC__
#define __FUNC__	"{IaiAxis}::Initialize" 
SElExceptionPtr Initialize(SaxisPtr me, int32_t ID_NODE)
{
	SElExceptionPtr	pexception = NULL;
	char  Message[512];
	
	SMAC_CAN_Close(me->ID_SMAC); 
	

	
	if(SMAC_CAN_Open(me->ID_SMAC, me->interfaceCAN)!=0)
	{
		sprintf(Message, "Erreur ouverture %s !",me->interfaceCAN);
		MessagePopup ("Erreur SMAC", Message);
		EXCTHROW( -1, Message);
	}
 
	SMAC_CAN_NMT_Services_Commands(me->ID_SMAC, me->ID_NODE,3);
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{IaiAxis}::PresentAlarm"  
SElExceptionPtr PresentAlarm(SaxisPtr me, int32_t ID_NODE, int32_t* status)
{
	SElExceptionPtr	pexception = NULL;


	
	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{IaiAxis}::AlarmReset" 
SElExceptionPtr AlarmReset(SaxisPtr me, int32_t ID_NODE)
{
	SElExceptionPtr	pexception = NULL;

	


Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{IaiAxis}::CurrentPosition"   
SElExceptionPtr CurrentPosition(SaxisPtr me,/*int32_t ID_SMAC,*/ int32_t ID_NODE, double* position)
{
	SElExceptionPtr	pexception = NULL;
	char  Message[512]; 

	
	if (SMAC_CAN_READ_ACTUAL_POSITION(me->ID_SMAC, me->ID_NODE, position, 1.0, 0.00)!=0)
	{
		sprintf(Message, "Erreur Read Current Position interface %s Node ID : %d!",me->interfaceCAN, me->ID_NODE);
		EXCTHROW( -1, Message);
	}
	
	//Formule de conversion de echelle SMAC - mm
	*position = *position * 0.006105 ;
	
	

Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{IaiAxis}::CurrentAnalog"   
SElExceptionPtr CurrentAnalog(SaxisPtr me, int32_t ID_NODE, double* analog, double Coefficient, double Offsets)
{

	SElExceptionPtr	pexception = NULL;
	char  Message[512];

	
	if(SMAC_CAN_READ_ANALOG2(me->ID_SMAC, me->ID_NODE,analog,Coefficient,Offsets)!=0)
	{
		sprintf(Message, "Erreur Read_Analog interface %s Node ID : %d!",me->interfaceCAN, me->ID_NODE);
		EXCTHROW( -1, Message);
	}
	
	//Formule de conversion de echelle SMAC - Newton
	*analog= (2047-*analog)*0.004882;
	

Error:
	EXCRETHROW( pexception);
}



#undef __FUNC__
#define __FUNC__	"{IaiAxis}::HomeReturn"  
SElExceptionPtr HomeReturn(SaxisPtr me, int32_t ID_NODE)
{
	SElExceptionPtr	pexception = NULL;


Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{IaiAxis}::EnableAxis" 
SElExceptionPtr EnableAxis(SaxisPtr me, int32_t ID_NODE)
{
	SElExceptionPtr	pexception = NULL;

char  Message[512];

SMAC_CAN_Close(me->ID_SMAC);
	
	if(SMAC_CAN_Open(me->ID_SMAC, me->interfaceCAN)!=0)
	{
		sprintf(Message, "Erreur ouverture %s !",me->interfaceCAN);
		MessagePopup ("Erreur SMAC", Message);
		EXCTHROW( -1, Message);
	}
	
	SMAC_CAN_NMT_Services_Commands(me->ID_SMAC, me->ID_NODE,3);
	
	
	if(SMAC_CAN_Start_Macro(me->ID_SMAC, me->ID_NODE, 1)!=0)
	{
		sprintf(Message, "Erreur Start_Macro interface %s Node ID : %d!",me->interfaceCAN, me->ID_NODE);
		EXCTHROW( -1, Message);
	}	

	
Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{IaiAxis}::StopAxis"  
SElExceptionPtr StopAxis(SaxisPtr me, int32_t ID_NODE)
{
	SElExceptionPtr	pexception = NULL;
	

	
	


Error:
	EXCRETHROW( pexception);
}

#undef __FUNC__
#define __FUNC__	"{IaiAxis}::SetPosition" 
SElExceptionPtr SetPosition(SaxisPtr me, int32_t ID_NODE, double position, double speed)
{
	SElExceptionPtr	pexception = NULL;
	char  Message[512];

position= position*163.8;
speed= speed*163.8;
	
	
	if(SMAC_CAN_Set_Profile_Velocity(me->ID_SMAC, me->ID_NODE, speed)!=0)
	{
		sprintf(Message, "Erreur Set_Profile_Velocity interface CAN : %s Node ID : %d!", me->interfaceCAN, me->ID_NODE);
		EXCTHROW( -1, Message);
	}
	if(SMAC_CAN_Set_Target_Position(me->ID_SMAC, me->ID_NODE, position)!=0)
	{
		sprintf(Message, "Erreur Set_Target_Position interface CAN : %s Node ID : %d!", me->interfaceCAN, me->ID_NODE);
		EXCTHROW( -1, Message);
}
	if(SMAC_CAN_Start_Macro(me->ID_SMAC, me->ID_NODE, 10)!=0)
	{
		sprintf(Message, "Erreur Start_Macro interface %s Node ID : %d!",me->interfaceCAN, me->ID_NODE);
		EXCTHROW( -1, Message);
	}

Error:
	EXCRETHROW( pexception);
}


#pragma warning( pop)
