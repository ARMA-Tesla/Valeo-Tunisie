s??        ??   8 ?\  l   ?   ????                               ni6229      National Instruments 6229                     ? ? ??ViInt16  ?  ? ??ViInt32  ? ? ??ViReal64     ? ??ViRsrc     	? 	??ViBoolean     	? 	??ViSession     ? ??ViStatus     ?  ViChar[]     ? ??ViChar     ? ??ViString     	?  ViInt16[]     	?  ViInt32[]     
?  	ViReal64[]     ? 	 
ViBoolean[]     ? ??ViConstString     ? ??ViAttr   ?    This instrument driver contains programming support for the DAQ Measurement Card.  This driver has all the functions that IVI and VXIplug&play require.  

Note:  This driver requires the VISA and IVI libraries.       ?    This class contains functions and sub-classes that configure the instrument.  The class includes high-level functions that configure multiple instrument settings as well as low-level functions that set, get, and check individual attribute values.
     T    This class contains sub-classes for the set, get, and check attribute functions.       ?    This class contains functions that set an attribute to a new value.  There are typesafe functions for each attribute data type.     ?    This class contains functions that obtain the current value of an attribute.  There are typesafe functions for each attribute data type.     ?    This class contains functions that obtain the current value of an attribute.  There are typesafe functions for each attribute data type.     m    This class contains functions and sub-classes that initiate instrument operations and report their status.
     _    This class contains functions and sub-classes that transfer data to and from the instrument.
    _    This class contains functions and sub-classes that control common instrument operations.  These functions include many of functions that VXIplug&play require, such as reset, self-test, revision query, error query, and error message.  This class also contains functions that access IVI error infomation, lock the session, and perform instrument I/O.
     R    This class contains functions that retrieve and clear the IVI error information.     ?    This class contains functions that retrieve coercion records.     J    This class contains functions that retrieve interchangeability warnings.     k    This class contains functions that lock and unlock IVI instrument driver sessions for multithread safefy.    ?    This function performs the following initialization actions:

- Creates a new IVI instrument driver session.

- Opens a session to the specified device using the interface and address you specify for the Resource Name parameter.

- If the ID Query parameter is set to VI_TRUE, this function queries the instrument ID and checks that it is valid for this instrument driver.

- If the Reset parameter is set to VI_TRUE, this function resets the instrument to a known state.

- Sends initialization commands to set the instrument to the state necessary for the operation of the instrument driver.

- Returns a ViSession handle that you use to identify the instrument in all subsequent instrument driver function calls.

Note:  This function creates a new session each time you invoke it. Although you can open more than one IVI session for the same resource, it is best not to do so.  You can use the same session in multiple program threads.  You can use the eldaq_LockSession and eldaq_UnlockSession functions to protect sections of code that require exclusive access to the resource.

        Pass the resource name of the device to initialize.

You can also pass the name of a driver session or logical name that you configure with the IVI Configuration utility.  The driver session identifies a specific device and specifies the initial settings for the session.  A logical Name identifies a particular driver session.

Refer to the following table below for the exact grammar to use for this parameter.  Optional fields are shown in square brackets ([]).

Syntax
------------------------------------------------------
GPIB[board]::<primary address>[::secondary address]::INSTR
VXI[board]::<logical address>::INSTR
GPIB-VXI[board]::<logical address>::INSTR
ASRL<port>::INSTR
<LogicalName>
<Driver Session>

If you do not specify a value for an optional field, the following values are used:

Optional Field - Value
------------------------------------------------------
board - 0
secondary address - none (31)

The following table contains example valid values for this parameter.

"Valid Value" - Description
------------------------------------------------------
"GPIB::22::INSTR" - GPIB board 0, primary address 22 no
                    secondary address
"GPIB::22::5::INSTR" - GPIB board 0, primary address 22
                       secondary address 5
"GPIB1::22::5::INSTR" - GPIB board 1, primary address 22
                        secondary address 5
"VXI::64::INSTR" - VXI board 0, logical address 64
"VXI1::64::INSTR" - VXI board 1, logical address 64
"GPIB-VXI::64::INSTR" - GPIB-VXI board 0, logical address 64
"GPIB-VXI1::64::INSTR" - GPIB-VXI board 1, logical address 64
"ASRL2::INSTR" - COM port 2
"SampleInstr" - Logical name "SampleInstr"
"xyz432" - Logical Name or Driver Session "xyz432"

/*=CHANGE:===================================================* 

Modify the following default value so that it reflects the default address for your instrument.  You must make the corresponding change to the Default Value entry for the control.

 *================================================END=CHANGE=*/ 
Default Value:  "GPIB::14::INSTR"

        Specify whether you want the instrument driver to perform an ID Query.

Valid Range:
VI_TRUE  (1) - Perform ID Query (Default Value)
VI_FALSE (0) - Skip ID Query

When you set this parameter to VI_TRUE, the driver verifies that the instrument you initialize is a type that this driver supports.  

Circumstances can arise where it is undesirable to send an ID Query command string to the instrument.  When you set this parameter to VI_FALSE, the function initializes the instrument without performing an ID Query.     ?    Specify whether you want the to reset the instrument during the initialization procedure.

Valid Range:
VI_TRUE  (1) - Reset Device (Default Value)
VI_FALSE (0) - Don't Reset

    ?    Returns a ViSession handle that you use to identify the instrument in all subsequent instrument driver function calls.

Notes:

(1) This function creates a new session each time you invoke it.  This is useful if you have multiple physical instances of the same type of instrument.  

(2) Avoid creating multiple concurrent sessions to the same physical instrument.  Although you can create more than one IVI session for the same resource, it is best not to do so.  A better approach is to use the same IVI session in multiple execution threads.  You can use functions eldaq_LockSession and eldaq_UnlockSession to protect sections of code that require exclusive access to the resource.

    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors    ? =   ?  ?    Resource Name                     ? : ?       ID Query                          ? =? ?       Reset Device                      | ?C ?  ?    Instrument Handle                 4#????  ?    Status                             "GPIB::14::INSTR"   Yes VI_TRUE No VI_FALSE   Yes VI_TRUE No VI_FALSE    	           	          ;    This function performs the following initialization actions:

- Creates a new IVI instrument driver and optionally sets the initial state of the following session attributes:

    ELDAQ_ATTR_RANGE_CHECK         
    ELDAQ_ATTR_QUERY_INSTRUMENT_STATUS  
    ELDAQ_ATTR_CACHE               
    ELDAQ_ATTR_SIMULATE            
    ELDAQ_ATTR_RECORD_COERCIONS    

- Opens a session to the specified device using the interface and address you specify for the Resource Name parameter.

- If the ID Query parameter is set to VI_TRUE, this function queries the instrument ID and checks that it is valid for this instrument driver.

- If the Reset parameter is set to VI_TRUE, this function resets the instrument to a known state.

- Sends initialization commands to set the instrument to the state necessary for the operation of the instrument driver.

- Returns a ViSession handle that you use to identify the instrument in all subsequent instrument driver function calls.

Note:  This function creates a new session each time you invoke it. Although you can open more than one IVI session for the same resource, it is best not to do so.  You can use the same session in multiple program threads.  You can use the eldaq_LockSession and eldaq_UnlockSession functions to protect sections of code that require exclusive access to the resource.

        Pass the resource name of the device to initialize.

You can also pass the name of a driver session or logical name that you configure with the IVI Configuration utility.  The driver session identifies a specific device and specifies the initial settings for the session.  A logical Name identifies a particular driver session.

Refer to the following table below for the exact grammar to use for this parameter.  Optional fields are shown in square brackets ([]).

Syntax
------------------------------------------------------
GPIB[board]::<primary address>[::secondary address]::INSTR
VXI[board]::<logical address>::INSTR
GPIB-VXI[board]::<logical address>::INSTR
ASRL<port>::INSTR
<LogicalName>
<Driver Session>

If you do not specify a value for an optional field, the following values are used:

Optional Field - Value
------------------------------------------------------
board - 0
secondary address - none (31)

The following table contains example valid values for this parameter.

"Valid Value" - Description
------------------------------------------------------
"GPIB::22::INSTR" - GPIB board 0, primary address 22 no
                    secondary address
"GPIB::22::5::INSTR" - GPIB board 0, primary address 22
                       secondary address 5
"GPIB1::22::5::INSTR" - GPIB board 1, primary address 22
                        secondary address 5
"VXI::64::INSTR" - VXI board 0, logical address 64
"VXI1::64::INSTR" - VXI board 1, logical address 64
"GPIB-VXI::64::INSTR" - GPIB-VXI board 0, logical address 64
"GPIB-VXI1::64::INSTR" - GPIB-VXI board 1, logical address 64
"ASRL2::INSTR" - COM port 2
"SampleInstr" - Logical name "SampleInstr"
"xyz432" - Logical Name or Driver Session "xyz432"

/*=CHANGE:===================================================* 

Modify the following default value so that it reflects the default address for your instrument.  You must make the corresponding change to the Default Value entry for the control.

 *================================================END=CHANGE=*/ 
Default Value:  "GPIB::14::INSTR"

        Specify whether you want the instrument driver to perform an ID Query.

Valid Range:
VI_TRUE  (1) - Perform ID Query (Default Value)
VI_FALSE (0) - Skip ID Query

When you set this parameter to VI_TRUE, the driver verifies that the instrument you initialize is a type that this driver supports.  

Circumstances can arise where it is undesirable to send an ID Query command string to the instrument.  When you set this parameter to VI_FALSE, the function initializes the instrument without performing an ID Query.     ?    Specify whether you want the to reset the instrument during the initialization procedure.

Valid Range:
VI_TRUE  (1) - Reset Device (Default Value)
VI_FALSE (0) - Don't Reset

    ?    Returns a ViSession handle that you use to identify the instrument in all subsequent instrument driver function calls.

Notes:

(1) This function creates a new session each time you invoke it.  This is useful if you have multiple physical instances of the same type of instrument.  

(2) Avoid creating multiple concurrent sessions to the same physical instrument.  Although you can create more than one IVI session for the same resource, it is best not to do so.  A better approach is to use the same IVI session in multiple execution threads.  You can use functions eldaq_LockSession and eldaq_UnlockSession to protect sections of code that require exclusive access to the resource.

    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
    d    You can use this control to set the initial value of certain attributes for the session.  The following table lists the attributes and the name you use in this parameter to identify the attribute.

Name              Attribute Defined Constant   
--------------------------------------------
RangeCheck        ELDAQ_ATTR_RANGE_CHECK
QueryInstrStatus  ELDAQ_ATTR_QUERY_INSTRUMENT_STATUS   
Cache             ELDAQ_ATTR_CACHE   
Simulate          ELDAQ_ATTR_SIMULATE  
RecordCoercions   ELDAQ_ATTR_RECORD_COERCIONS

The format of this string is, "AttributeName=Value" where AttributeName is the name of the attribute and Value is the value to which the attribute will be set.  To set multiple attributes, separate their assignments with a comma.  

If you pass NULL or an empty string for this parameter, the session uses the default values for the attributes.   You can override the default values by assigning a value explicitly in a string you pass for this parameter.  You do not have to specify all of the attributes and may leave any of them out.  If you do not specify one of the attributes, its default value will be used.  

The default values for the attributes are shown below:

    Attribute Name     Default Value
    ----------------   -------------
    RangeCheck         VI_TRUE
    QueryInstrStatus   VI_FALSE
    Cache              VI_TRUE
    Simulate           VI_FALSE
    RecordCoercions    VI_FALSE
    

The following are the valid values for ViBoolean attributes:

    True:     1, True, or VI_TRUE
    False:    0, False, or VI_FALSE


Default Value:
       "Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1"
    )k =   ?  ?    Resource Name                     1? : ?       ID Query                          3? =? ?       Reset Device                      4I ?Y ?  ?    Instrument Handle                 7#????  ?    Status                            >? ?  ? ?    Option String                      "GPIB::14::INSTR"   Yes VI_TRUE No VI_FALSE   Yes VI_TRUE No VI_FALSE    	           	           5"Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1"   ?    This function sets the value of a ViString attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        Specifies list of the analog input channels which can be read wirh Read Measurement functions.

Valid Channel Names:  ai0,ai1,ai2,ai3,ai4,...,ai31

Default Value:  ""

Note: if the NULL or emty string is pass all channels are configured for measurement. 
     ?    Specifies sample rate for the specified channels.

Valid Range:
Range depends on the number of the specified channels.
For NI-6229 card mus be following formula pass:
    count * sample rate <= 250 000

Default Value: 10000        specified time interval from measured value is callculated. This interval also defines max length of the record.
It's recommended that this value should be multiply of power line time period.

For 50Hz power network is recomended n*20.0e-3

Default Value: 40.0e-3

    K?-   ?  ?    Instrument Handle                 LW#????  ?    Status                            S? =  ?  ?    Measured Channels                 T? = ? ?  ?    Sample Rate                       U? >J ?  ?    Measured Interval [s]                  	           ""    10000    40.0e-3       Specifies list of the analog input channels which can be read wirh Read Measurement functions.

Valid Channel Names:  ai0,ai1,ai2,ai3,ai4,...,ai31

Default Value:  ""

Note: if the NULL or emty string is pass all channels are configured for measurement. 
     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    X =  ?  ?    digital Channels                  Y"(   ?  ?    Instrument Handle                 Y?????  ?    Status                          ???? <? ?  ?    Status                             ""        	           0   ?    This function sets the value of a ViString attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        Specifies list of the analog input channels which can be read wirh Read Measurement functions.

Valid Channel Names:  ai0,ai1,ai2,ai3,ai4,...,ai31

Default Value:  ""

Note: if the NULL or emty string is pass all channels are configured for measurement. 
    g#-   ?  ?    Instrument Handle                 g?#????  ?    Status                            oa =  ?      Calibration File                       	           ""   ?    This function sets the value of a ViInt32 attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        Pass the value to which you want to set the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none    C    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViInt32 type.   
  If you choose to see all IVI attributes, the data types appear
  to the right of the attribute names in the list box. 
  Attributes with data types other than ViInt32 are dim.  If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    u?-   ?  ?    Instrument Handle                 v?#????  ?    Status                            ~0 ? ? ?  ?    Attribute Value                 ???? ? ???                                           ?K = ? ? ?    Attribute ID                      ?? =  ?  ?    Channel Name                           	               .Press <ENTER> for a list of 
value constants.                0    ""   ?    This function sets the value of a ViReal64 attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        Pass the value to which you want to set the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none    B    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViReal64
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box.
  Attributes with data types other than ViReal64 are dim.  If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    ?#-   ?  ?    Instrument Handle                 ??#????  ?    Status                            ?a ? ? ?  ?    Attribute Value                 ???? ? ???                                           ?| = ? ? ?    Attribute ID                      ?? =  ?  ?    Channel Name                           	               .Press <ENTER> for a list of 
value constants.                0    ""   ?    This function sets the value of a ViString attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        Pass the value to which you want to set the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none    A    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViString
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box.
  Attributes with data types other than ViString are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    ?S-   ?  ?    Instrument Handle                 ?#????  ?    Status                            ?? ? ? ?  ?    Attribute Value                 ???? ? ???                                           ?? = ? ? ?    Attribute ID                      ?? =  ?  ?    Channel Name                           	               .Press <ENTER> for a list of 
value constants.                0    ""   ?    This function sets the value of a ViBoolean attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        Pass the value to which you want to set the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none    C    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViBoolean
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box.
  Attributes with data types other than ViBoolean are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    ??-   ?  ?    Instrument Handle                 ?7#????  ?    Status                            ?? ? ? ?  ?    Attribute Value                 ???? ? ???                                           ?? = ? ? ?    Attribute ID                      ?' =  ?  ?    Channel Name                           	               .Press <ENTER> for a list of 
value constants.                0    ""   ?    This function sets the value of a ViSession attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        Pass the value to which you want to set the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    C    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViSession
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box.
  Attributes with data types other than ViSession are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
    ҵ-   ?  ?    Instrument Handle                 ?i#????  ?    Status                            ?? ? ? ?  ?    Attribute Value                   ? =  ?  ?    Channel Name                      ?, = ? ? ?    Attribute ID                    ???? ? ???                                                	               ""                0    .Press <ENTER> for a list of 
value constants.   ?    This function queries the value of a ViInt32 attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    ?    Returns the current value of the attribute.  Pass the address of a ViInt32 variable.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has named constants as valid values, you can view a
  list of the constants by pressing <ENTER> on this control.  
  Select a value by double-clicking on it or by selecting it and 
  then pressing <ENTER>.  
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    ?    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Help text is
  shown for each attribute.  Select an attribute by 
  double-clicking on it or by selecting it and then pressing
  <ENTER>.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViInt32 type. 
  If you choose to see all IVI attributes, the data types appear
  to the right of the attribute names in the list box.  
  Attributes with data types other than ViInt32 are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
    ??-   ?  ?    Instrument Handle                 ??#????  ?    Status                            ? ? ? ?  ?    Attribute Value                   ?? =  ?  ?    Channel Name                      ?? = ? ? ?    Attribute ID                           	           	            ""                0   ?    This function queries the value of a ViReal64 attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    ?    Returns the current value of the attribute.  Pass the address of a ViReal64 variable.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has named constants as valid values, you can view a
  list of the constants by pressing <ENTER> on this control.  
  Select a value by double-clicking on it or by selecting it and 
  then pressing <ENTER>.  
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    ?    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Help text is
  shown for each attribute.  Select an attribute by 
  double-clicking on it or by selecting it and then pressing
  <ENTER>.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViReal64
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box. 
  Attributes with data types other than ViReal64 are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
    ??-   ?  ?    Instrument Handle                 ?;#????  ?    Status                           ? ? ? ?  ?    Attribute Value                  ? =  ?  ?    Channel Name                     ? = ? ? ?    Attribute ID                           	           	           ""                0   4    This function queries the value of a ViString attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

You must provide a ViChar array to serve as a buffer for the value.  You pass the number of bytes in the buffer as the Buffer Size parameter.  If the current value of the attribute, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you want to call this function just to get the required buffer size, you can pass 0 for the Buffer Size and VI_NULL for the Attribute Value buffer.  

If you want the function to fill in the buffer regardless of the   number of bytes in the value, pass a negative number for the Buffer Size parameter.  


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    	@    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    &    The buffer in which the function returns the current value of the attribute.  The buffer must be of type ViChar and have at least as many bytes as indicated in the Buffer Size parameter.

If the current value of the attribute, including the terminating NUL byte, contains more bytes that you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you specify 0 for the Buffer Size parameter, you can pass VI_NULL for this parameter.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has named constants as valid values, you can view a
  list of the constants by pressing <ENTER> on this control.  
  Select a value by double-clicking on it or by selecting it and 
  then pressing <ENTER>.  
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    ?    Pass the number of bytes in the ViChar array you specify for the Attribute Value parameter.  

If the current value of the attribute, including the terminating NUL byte, contains more bytes that you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Attribute Value buffer parameter.
    ?    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Help text is
  shown for each attribute.  Select an attribute by 
  double-clicking on it or by selecting it and then pressing
  <ENTER>.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViString
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box. 
  Attributes with data types other than ViString are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
   ?-   ?  ?    Instrument Handle                s#????  ?    Status or Required Size          ? ? L ? ?    Attribute Value                  ? =  ?  ?    Channel Name                     
 =? ?  ?    Buffer Size                      !? = ? ? ?    Attribute ID                           	           	            ""    512                0   ?    This function queries the value of a ViBoolean attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    ?    Returns the current value of the attribute.  Pass the address of a ViBoolean variable.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has named constants as valid values, you can view a
  list of the constants by pressing <ENTER> on this control.  
  Select a value by double-clicking on it or by selecting it and 
  then pressing <ENTER>.  
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    ?    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Help text is
  shown for each attribute.  Select an attribute by 
  double-clicking on it or by selecting it and then pressing
  <ENTER>.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViBoolean
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box. 
  Attributes with data types other than ViBoolean are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
   )?-   ?  ?    Instrument Handle                *z#????  ?    Status                           2 ? ? ?  ?    Attribute Value                  3? =  ?  ?    Channel Name                     4? = ? ? ?    Attribute ID                           	           	            ""                0   ?    This function queries the value of a ViSession attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    ?    Returns the current value of the attribute.  Pass the address of a ViSession variable.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has named constants as valid values, you can view a
  list of the constants by pressing <ENTER> on this control.  
  Select a value by double-clicking on it or by selecting it and 
  then pressing <ENTER>.  
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    ?    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Help text is
  shown for each attribute.  Select an attribute by 
  double-clicking on it or by selecting it and then pressing
  <ENTER>.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViSession
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box. 
  Attributes with data types other than ViSession are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
   <?-   ?  ?    Instrument Handle                =8#????  ?    Status                           D? ? ? ?  ?    Attribute Value                  F? =  ?  ?    Channel Name                     G? = ? ? ?    Attribute ID                           	           	            ""                0    S    This function checks the validity of a value you specify for a ViInt32 attribute.     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    (    Pass the value which you want to verify as a valid value for the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    @    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViInt32 type. 
  If you choose to see all IVI attributes, the data types appear
  to the right of the attribute names in the list box. 
  Attributes with data types other than ViInt32 are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
   M?-   ?  ?    Instrument Handle                N?#????  ?    Status                           V ? ? ?  ?    Attribute Value                  XL =  ?  ?    Channel Name                    ???? ? ???                                          Ye = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0    T    This function checks the validity of a value you specify for a ViReal64 attribute.     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    (    Pass the value which you want to verify as a valid value for the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    B    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViReal64
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box. 
  Attributes with data types other than ViReal64 are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
   `?-   ?  ?    Instrument Handle                aX#????  ?    Status                           h? ? ? ?  ?    Attribute Value                  k =  ?  ?    Channel Name                    ???? ? ???                                          l+ = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0    T    This function checks the validity of a value you specify for a ViString attribute.     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    (    Pass the value which you want to verify as a valid value for the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    B    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViString
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box. 
  Attributes with data types other than ViString are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
   sl-   ?  ?    Instrument Handle                t #????  ?    Status                           {? ? ? ?  ?    Attribute Value                  }? =  ?  ?    Channel Name                    ???? ? ???                                          ~? = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0    U    This function checks the validity of a value you specify for a ViBoolean attribute.     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    (    Pass the value which you want to verify as a valid value for the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    D    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViBoolean
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box. 
  Attributes with data types other than ViBoolean are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
   ?5-   ?  ?    Instrument Handle                ??#????  ?    Status                           ?s ? ? ?  ?    Attribute Value                  ?? =  ?  ?    Channel Name                    ???? ? ???                                          ?? = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0    U    This function checks the validity of a value you specify for a ViSession attribute.     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

    (    Pass the value which you want to verify as a valid value for the attribute.

From the function panel window, you can use this control as follows.

- If the attribute currently showing in the Attribute ID ring
  control has constants as valid values, you can view a list of
  the constants by pressing <ENTER> on this control.  Select a
  value by double-clicking on it or by selecting it and then
  pressing <ENTER>.  

  Note:  Some of the values might not be valid depending on the
  current settings of the instrument session.

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
    D    Pass the ID of an attribute.

From the function panel window, you can use this control as follows.

- Click on the control or press <ENTER>, <spacebar>, or
  <ctrl-down arrow>, to display a dialog box containing a
  hierarchical list of the available attributes.  Attributes 
  whose value cannot be set are dim.  Help text is shown for 
  each attribute.  Select an attribute by double-clicking on it  
  or by selecting it and then pressing <ENTER>.

  Read-only attributes appear dim in the list box.  If you 
  select a read-only attribute, an error message appears.

  A ring control at the top of the dialog box allows you to see 
  all IVI attributes or only the attributes of the ViSession
  type.  If you choose to see all IVI attributes, the data types
  appear to the right of the attribute names in the list box. 
  Attributes with data types other than ViSession are dim. If
  you select an attribute data type that is dim, LabWindows/CVI
  transfers you to the function panel for the corresponding
  function that is consistent with the data type.

- If you want to enter a variable name, press <CTRL-T> to change
  this ring control to a manual input box.

- If the attribute in this ring control has named constants as  
  valid values, you can view the constants by moving to the 
  Attribute Value control and pressing <ENTER>.
   ? -   ?  ?    Instrument Handle                ??#????  ?    Status                           ?> ? ? ?  ?    Attribute Value                  ?n =  ?  ?    Channel Name                    ???? ? ???                                          ?? = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0   ?    This function sets the value of a ViString attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

   ?B-   ?  ?    Instrument Handle                ??#????  ?    Status                                 	          ?    This function sets the value of a ViString attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

   ??-   ?  ?    Instrument Handle                ??#????  ?    Status                                 	           ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        Specifies list of the analog input channels which can be read wirh Read Measurement functions.

Valid Channel Names:  ai0,ai1,ai2,ai3,ai4,...,ai31

Default Value:  ""

Note: if the NULL or emty string is pass all channels are configured for measurement. 
     ?    Specifies sample rate for the specified channels.

Valid Range:
Range depends on the number of the specified channels.
For NI-6229 card mus be following formula pass:
    count * sample rate <= 250 000

Default Value: 10000        specified time interval from measured value is callculated. This interval also defines max length of the record.
It's recommended that this value should be multiply of power line time period.

For 50Hz power network is recomended n*20.0e-3

Default Value: 40.0e-3

        Specifies list of the analog input channels which can be read wirh Read Measurement functions.

Valid Channel Names:  ai0,ai1,ai2,ai3,ai4,...,ai31

Default Value:  ""

Note: if the NULL or emty string is pass all channels are configured for measurement. 
        specified time interval from measured value is callculated. This interval also defines max length of the record.
It's recommended that this value should be multiply of power line time period.

For 50Hz power network is recomended n*20.0e-3

Default Value: 40.0e-3

     ?    Specifies sample rate for the specified channels.

Valid Range:
Range depends on the number of the specified channels.
For NI-6229 card mus be following formula pass:
    count * sample rate <= 250 000

Default Value: 10000     ?    Specifies sample rate for the specified channels.

Valid Range:
Range depends on the number of the specified channels.
For NI-6229 card mus be following formula pass:
    count * sample rate <= 250 000

Default Value: 10000   Ɔ   ?  ?    vi                               ?:????  ?    Status                           ?? !  ?  ?    channels                         ?? S ? ?  ?    sampsPerChan                     ж S  ?  ?    Max                              ??   ? ?  ?    Clock Source                     ??  I ?  ?    Min                              ?? SK ?  ?    rate                             ?? ?     ?    data                                   	           ""    500    40.0e-3    ""    40.0e-3    500    500   ?    This function sets the value of a ViString attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
   ܯ-   ?  ?    Instrument Handle                ?c#????  ?    Status                           ?? =  ?  ?    Channel Name                     ? ; ?    ?    ScaleInfo                       ???? 9- ? ?    Measurement Type                ???? ?8 ?  ?    value                                  	           ""    ""              zRMS ELDAQ_VAL_MEASUREMENT_RMS Min ELDAQ_VAL_MEASUREMENT_MIN Max ELDAQ_VAL_MEASUREMENT_MAX Mean ELDAQ_VAL_MEASUREMENT_MEAN    	          ?    This function sets the value of a ViString attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
   ??-   ?  ?    Instrument Handle                ??#????  ?    Status                           ?* =  ?  ?    Channel Name                     ?H ; ? ?  ?    Scale                           ???? ?  ?  ?    data                             ?f 6Q ?  ?    Array Size                      ???? ? ? ?  ?    Start Time                      ???? ?R ?  ?    x Increment                            	           ""    ""    	                	           	           ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  AI1,AI2,AI3,AO1,A02,AO3,AO4

Default Value:  ""
   ?p   ?  ?    Instrument Handle                ?$????  ?    Status                           ? !  ?  ?    Channel Name                     ? ! ? ?  ?    scaleName                        ?  ?    ?    _pScaleInfo                            	           ""    ""    ""   &    This function resets the instrument to a known state and sends initialization commands to the instrument.  The initialization commands set instrument settings such as Headers Off, Short Command form, and Data Transfer Binary to the state necessary for the operation of the instrument driver.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   	a#????  ?    Status                           ?-   ?  ?    Instrument Handle                  	               ?    This function resets the instrument and applies initial user specified settings from the Logical Name which was used to initialize the session.  If the session was created without a Logical Name, this function is equivalent to the eldaq_reset function.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   #????  ?    Status                           ?-   ?  ?    Instrument Handle                  	               ?    This function places the instrument in a quiescent state where it has minimal or no impact on the system to which it is connected.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   c#????  ?    Status                           #?-   ?  ?    Instrument Handle                  	               Z    This function runs the instrument's self test routine and returns the test result(s). 

    &    This control contains the value returned from the instrument self test.  Zero means success.  For any other code, see the device's operator's manual.

Self-Test Code    Description
---------------------------------------
   0              Passed self test
   1              Self test failed

     ?    Returns the self-test response string from the instrument. See the device's operation manual for an explanation of the string's contents.

You must pass a ViChar array with at least 256 bytes.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   %} =  ?  ?    Self Test Result                 &? = ? ? ,    Self-Test Message                'u#????  ?    Status                           .?-   ?  ?    Instrument Handle                  	           	            	               `    This function returns the revision numbers of the instrument driver and instrument firmware.

     ?    Returns the instrument driver software revision numbers in the form of a string.

You must pass a ViChar array with at least 256 bytes.     ?    Returns the instrument firmware revision numbers in the form of a string.

You must pass a ViChar array with at least 256 bytes.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   1 = 3 ?  ?    Instrument Driver Revision       1? =6 ?  ?    Firmware Revision                23#????  ?    Status                           9?-   ?  ?    Instrument Handle                  	            	            	               V    This function reads an error code and a message from the instrument's error queue.

     B    Returns the error code read from the instrument's error queue.

     ?    Returns the error message string read from the instrument's error message queue.

You must pass a ViChar array with at least 256 bytes.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   ;? =  ?  ?    Error Code                       < = ? ? ,    Error Message                    <?#????  ?    Status                           D1-   ?  ?    Instrument Handle                  	           	            	               n    This function converts a status code returned by an instrument driver function into a user-readable string.      z    Pass the Status parameter that is returned from any of the instrument driver functions.

Default Value:  0  (VI_SUCCESS)     ?    Returns the user-readable message string that corresponds to the status code you specify.

You must pass a ViChar array with at least 256 bytes.
    H    Reports the status of this operation.  

This function can return only three possible status codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFF0085  Unknown status code (warning).

BFFF000A  Invalid parameter (Error Message buffer is VI_NULL).

        The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

You can pass VI_NULL for this parameter.  This is useful when one of the initialize functions fail.

Default Value:  VI_NULL
   FW =  ?  h    Error Code                       F? = ? ? ?    Error Message                    Gt#????  ?    Status                           H?-   ?  ?    Instrument Handle                  0    	            	           VI_NULL    P    This function invalidates the cached values of all attributes for the session.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   K4#????  ?    Status                           R?-   ?  ?    Instrument Handle                  	              ?    This function retrieves and then clears the IVI error information for the session or the current execution thread. One exception exists: If the BufferSize parameter is 0, the function does not clear the error information. By passing 0 for the buffer size, the caller can ascertain the buffer size required to get the entire error description string and then call the function again with a sufficiently large buffer.

If the user specifies a valid IVI session for the InstrumentHandle parameter, Get Error retrieves and then clears the error information for the session.  If the user passes VI_NULL for the InstrumentHandle parameter, this function retrieves and then clears the error information for the current execution thread.  If the InstrumentHandle parameter is an invalid session, the function does nothing and returns an error. Normally, the error information describes the first error that occurred since the user last called eldaq_GetError or eldaq_ClearError.
    	>    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Pass the number of bytes in the ViChar array you specify for the Description parameter.

If the error description, including the terminating NUL byte, contains more bytes than you indicate in this parameter, the function copies BufferSize - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Description buffer parameter.

Default Value:  None     ?    Returns the error code for the session or execution thread.

If you pass 0 for the Buffer Size, you can pass VI_NULL for this parameter.
    ?    Returns the error description for the IVI session or execution thread.  If there is no description, the function returns an empty string.

The buffer must contain at least as many elements as the value you specify with the Buffer Size parameter.  If the error description, including the terminating NUL byte, contains more bytes than you indicate with the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass 0 for the Buffer Size, you can pass VI_NULL for this parameter.   W?#????  ?    Status or Required Size          a-   ?  ?    Instrument Handle                a? 3< ?  ?    BufferSize                       d? 3 P ?  ?    Code                             e. ? Q ? ?    Description                        	                   	           	           \    This function clears the error code and error description for the IVI session. If the user specifies a valid IVI session for the instrument_handle parameter, this function clears the error information for the session. If the user passes VI_NULL for the Vi parameter, this function clears the error information for the current execution thread. If the Vi parameter is an invalid session, the function does nothing and returns an error.
The function clears the error code by setting it to VI_SUCCESS.  If the error description string is non-NULL, the function de-allocates the error description string and sets the address to VI_NULL.

Maintaining the error information separately for each thread is useful if the user does not have a session handle to pass to the eldaq_GetError function, which occurs when a call to eldaq_Init or eldaq_InitWithOptions fails.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   l?#????  ?    Status                           tM-   ?  ?    Instrument Handle                  	              ?    This function returns the coercion information associated with the IVI session.  This function retrieves and clears the oldest instance in which the instrument driver coerced a value you specified to another value.

If you set the ELDAQ_ATTR_RECORD_COERCIONS attribute to VI_TRUE, the instrument driver keeps a list of all coercions it makes on ViInt32 or ViReal64 values you pass to instrument driver functions.  You use this function to retrieve information from that list.

If the next coercion record string, including the terminating NUL byte, contains more bytes than you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Coercion Record buffer parameter.

The function returns an empty string in the Coercion Record parameter if no coercion records remain for the session.

    	>    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors     ?    The ViSession handle that you obtain from the eldaq_init function.  The handle identifies a particular instrument session.

Default Value:  None    ?    Returns the next coercion record for the IVI session.  If there are no coercion records, the function returns an empty string.

The buffer must contain at least as many elements as the value you specify with the Buffer Size parameter.  If the next coercion record string, including the terminating NUL byte, contains more bytes than you indicate with the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

This parameter returns an empty string if no coercion records remain for the session.
    ?    Pass the number of bytes in the ViChar array you specify for the Coercion Record parameter.

If the next coercion record string, including the terminating NUL byte, contains more bytes than you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Coercion Record buffer parameter.

Default Value:  None

   zP#????  ?    Status or Required Size          ??-   ?  ?    Instrument Handle                ?0 ? Q ? ?    Coercion Record                  ?6 = ? ?  ?    Buffer Size                        	               	               ,    This function returns the interchangeability warnings associated with the IVI session. It retrieves and clears the oldest instance in which the class driver recorded an interchangeability warning.  Interchangeability warnings indicate that using your application with a different instrument might cause different behavior. You use this function to retrieve interchangeability warnings.

The driver performs interchangeability checking when the ELDAQ_ATTR_INTERCHANGE_CHECK attribute is set to VI_TRUE.

The function returns an empty string in the Interchange Warning parameter if no interchangeability warnings remain for the session.

In general, the instrument driver generates interchangeability warnings when an attribute that affects the behavior of the instrument is in a state that you did not specify.
    	>    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Pass the number of bytes in the ViChar array you specify for the Interchange Warning parameter.

If the next interchangeability warning string, including the terminating NUL byte, contains more bytes than you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value. For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Interchange Warning buffer parameter.

Default Value:  None
        Returns the next interchange warning for the IVI session. If there are no interchange warnings, the function returns an empty  string.

The buffer must contain at least as many elements as the value you specify with the Buffer Size parameter. If the next interchangeability warning string, including the terminating NUL byte, contains more bytes than you indicate with the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

This parameter returns an empty string if no interchangeability
warnings remain for the session.

   ?S#????  ?    Status or Required Size          ??-   ?  ?    Instrument Handle                ?M = ? ?  ?    Buffer Size                      ?S ? Q ? ?    Interchange Warning                	                   	            A    This function clears the list of current interchange warnings.
     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

   ??-   ?  ?    Instrument Handle                ?h#????  ?    Status                                 	          ?    When developing a complex test system that consists of multiple test modules, it is generally a good idea to design the test modules so that they can run in any order.  To do so requires ensuring that each test module completely configures the state of each instrument it uses.  If a particular test module does not completely configure the state of an instrument, the state of the instrument depends on the configuration from a previously executed test module.  If you execute the test modules in a different order, the behavior of the instrument and therefore the entire test module is likely to change.  This change in behavior is generally instrument specific and represents an interchangeability problem.

You can use this function to test for such cases.  After you call this function, the interchangeability checking algorithms in the specific driver ignore all previous configuration operations.  By calling this function at the beginning of a test module, you can determine whether the test module has dependencies on the operation of previously executed test modules.

This function does not clear the interchangeability warnings from the list of previously recorded interchangeability warnings.  If you want to guarantee that the eldaq_GetNextInterchangeWarning function only returns those interchangeability warnings that are generated after calling this function, you must clear the list of interchangeability warnings.  You can clear the interchangeability warnings list by repeatedly calling the eldaq_GetNextInterchangeWarning function until no more interchangeability warnings are returned.  If you are not interested in the content of those warnings, you can call the eldaq_ClearInterchangeWarnings function.
     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

   ?5-   ?  ?    Instrument Handle                ??#????  ?    Status                                 	              This function obtains a multithread lock on the instrument session.  Before it does so, it waits until all other execution threads have released their locks on the instrument session.

Other threads might have obtained a lock on this session in the following ways:

- The user's application called eldaq_LockSession.

- A call to the instrument driver locked the session.

- A call to the IVI engine locked the session.

After your call to eldaq_LockSession returns successfully, no other threads can access the instrument session until you call eldaq_UnlockSession.

Use eldaq_LockSession and eldaq_UnlockSession around a sequence of calls to instrument driver functions if you require that the instrument retain its settings through the end of the sequence.

You can safely make nested calls to eldaq_LockSession within the same thread.  To completely unlock the session, you must balance each call to eldaq_LockSession with a call to eldaq_UnlockSession.  If, however, you use the Caller Has Lock parameter in all calls to eldaq_LockSession and eldaq_UnlockSession within a function, the IVI Library locks the session only once within the function regardless of the number of calls you make to eldaq_LockSession.  This allows you to call eldaq_UnlockSession just once at the end of the function. 
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    This parameter serves as a convenience.  If you do not want to use this parameter, pass VI_NULL. 

Use this parameter in complex functions to keep track of whether you obtain a lock and therefore need to unlock the session.  Pass the address of a local ViBoolean variable.  In the declaration of the local variable, initialize it to VI_FALSE.  Pass the address of the same local variable to any other calls you make to eldaq_LockSession or eldaq_UnlockSession in the same function.

The parameter is an input/output parameter.  eldaq_LockSession and eldaq_UnlockSession each inspect the current value and take the following actions:

- If the value is VI_TRUE, eldaq_LockSession does not lock the session again.  If the value is VI_FALSE, eldaq_LockSession obtains the lock and sets the value of the parameter to VI_TRUE.

- If the value is VI_FALSE, eldaq_UnlockSession does not attempt to unlock the session.  If the value is VI_TRUE, eldaq_UnlockSession releases the lock and sets the value of the parameter to VI_FALSE.
 
Thus, you can, call eldaq_UnlockSession at the end of your function without worrying about whether you actually have the lock.  

Example:

ViStatus TestFunc (ViSession vi, ViInt32 flags)
{
    ViStatus error = VI_SUCCESS;
    ViBoolean haveLock = VI_FALSE;

    if (flags & BIT_1)
        {
        viCheckErr( eldaq_LockSession(vi, &haveLock));
        viCheckErr( TakeAction1(vi));
        if (flags & BIT_2)
            {
            viCheckErr( eldaq_UnlockSession(vi, &haveLock));
            viCheckErr( TakeAction2(vi));
            viCheckErr( eldaq_LockSession(vi, &haveLock);
            } 
        if (flags & BIT_3)
            viCheckErr( TakeAction3(vi));
        }

Error:
    /* 
       At this point, you cannot really be sure that 
       you have the lock.  Fortunately, the haveLock 
       variable takes care of that for you.          
    */
    eldaq_UnlockSession(vi, &haveLock);
    return error;
}   ?#????  ?    Status                           ĕ-   ?  ?    Instrument Handle                ?I H ? ?  ?    Caller Has Lock                    	               	            ?    This function releases a lock that you acquired on an instrument session using eldaq_LockSession.  Refer to eldaq_LockSession for additional information on session locks.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    This parameter serves as a convenience.  If you do not want to use this parameter, pass VI_NULL. 

Use this parameter in complex functions to keep track of whether you obtain a lock and therefore need to unlock the session. 
Pass the address of a local ViBoolean variable.  In the declaration of the local variable, initialize it to VI_FALSE.  Pass the address of the same local variable to any other calls you make to eldaq_LockSession or eldaq_UnlockSession in the same function.

The parameter is an input/output parameter.  eldaq_LockSession and eldaq_UnlockSession each inspect the current value and take the following actions:

- If the value is VI_TRUE, eldaq_LockSession does not lock the session again.  If the value is VI_FALSE, eldaq_LockSession obtains the lock and sets the value of the parameter to VI_TRUE.

- If the value is VI_FALSE, eldaq_UnlockSession does not attempt to unlock the session.  If the value is VI_TRUE, eldaq_UnlockSession releases the lock and sets the value of the parameter to VI_FALSE.
 
Thus, you can, call eldaq_UnlockSession at the end of your function without worrying about whether you actually have the lock.  

Example:

ViStatus TestFunc (ViSession vi, ViInt32 flags)
{
    ViStatus error = VI_SUCCESS;
    ViBoolean haveLock = VI_FALSE;

    if (flags & BIT_1)
        {
        viCheckErr( eldaq_LockSession(vi, &haveLock));
        viCheckErr( TakeAction1(vi));
        if (flags & BIT_2)
            {
            viCheckErr( eldaq_UnlockSession(vi, &haveLock));
            viCheckErr( TakeAction2(vi));
            viCheckErr( eldaq_LockSession(vi, &haveLock);
            } 
        if (flags & BIT_3)
            viCheckErr( TakeAction3(vi));
        }

Error:
    /* 
       At this point, you cannot really be sure that 
       you have the lock.  Fortunately, the haveLock 
       variable takes care of that for you.          
    */
    eldaq_UnlockSession(vi, &haveLock);
    return error;
}   ?b#????  ?    Status                           ??-   ?  ?    Instrument Handle                ֠ H ? ?  ?    Caller Has Lock                    	               	           ?    This function performs the following operations:

- Closes the instrument I/O session.

- Destroys the instrument driver session and all of its attributes.

- Deallocates any memory resources the driver uses.

Notes:

(1) You must unlock the session before calling eldaq_close.

(2) After calling eldaq_close, you cannot use the instrument driver again until you call eldaq_init or eldaq_InitWithOptions.

    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the eldaq_error_message function.  To obtain additional information about the error condition, call the eldaq_GetError function.  To clear the error information from the driver, call the eldaq_ClearError function.
          
The general meaning of the status code is as follows:

Value                  Meaning
-------------------------------
0                      Success
Positive Values        Warnings
Negative Values        Errors

This driver defines the following status codes:
          
Status    Description
-------------------------------------------------
WARNINGS:
/*=CHANGE:===================================================* 
Insert Instrument-defined warning codes here.
 *================================================END=CHANGE=*/ 

ERRORS:
/*=CHANGE:===================================================* 
Insert Instrument-defined error codes here.
 *================================================END=CHANGE=*/ 
          
This instrument driver also returns errors and warnings defined by other sources.  The following table defines the ranges of additional status codes that this driver can return.  The table lists the different include files that contain the defined constants for the particular status codes:
          
Numeric Range (in Hex)   Status Code Types    
-------------------------------------------------
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors

     ?    The ViSession handle that you obtain from the eldaq_init or eldaq_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   ??#????  ?    Status                           ?.-   ?  ?    Instrument Handle                  	            ????         	W  "?     K.    init                            ????         $(  D?     K.    InitWithOptions                 ????         F?  V?     K.    ConfigureMeasurement            ????       ????  a`     K.    WriteDigitalLine                ????         bO  pj     K.    LoadCalibration                 ????         q  ??     K.    SetAttributeViInt32             ????         ?O  ??     K.    SetAttributeViReal64            ????         ?  ?     K.    SetAttributeViString            ????         ??  ?E     K.    SetAttributeViBoolean           ????         ??  ?w     K.    SetAttributeViSession           ????         ?  ??     K.    GetAttributeViInt32             ????         ?? 	F     K.    GetAttributeViReal64            ????        
? &?     K.    GetAttributeViString            ????        ( 9?     K.    GetAttributeViBoolean           ????        :? LF     K.    GetAttributeViSession           ????        M? ^?     K.    CheckAttributeViInt32           ????        `H qu     K.    CheckAttributeViReal64          ????        s ?=     K.    CheckAttributeViString          ????        ?? ?     K.    CheckAttributeViBoolean         ????        ?? ??     K.    CheckAttributeViSession         ????        ?n ??     K.    StartMeasurement                ????        ?? ?     K.    StopMeasurement                 ????       ???? շ 	    K.    ReadForce                       ????        ?? ?)     K.    ReadMeasurement                 ????        ? ??     K.    ReadWaveform                    ????       ????      K.    GetChannelScale                 ????        3 ?     K.    reset                           ????         ]     K.    ResetWithDefaults               ????        ? $?     K.    Disable                         ????        % /?     K.    self_test                       ????        0? :q     K.    revision_query                  ????        ;m D?     K.    error_query                     ????        E? I?     K.    error_message                   ????        J? Sr     K.    InvalidateAllAttributes         ????        S? h*     K.    GetError                        ????        i_ u     K.    ClearError                      ????        u{ ?+     K.    GetNextCoercionRecord           ????        ? ?w     K.    GetNextInterchangeWarning       ????        ?k ??     K.    ClearInterchangeWarnings        ????        ?l ?s     K.    ResetInterchangeCheck           ????        ?? ??     K.    LockSession                     ????        ͭ ?I     K.    UnlockSession                   ????        ? ??     K.    close                                 ?                                     DInitialize                           DInitialize With Options             ?Configuration Functions              DConfigure Measurement                DWriteDigitalLine                     DLoad Calibration                    ?Set/Get/Check Attribute             Set Attribute                        DSet Attribute ViInt32                DSet Attribute ViReal64               DSet Attribute ViString               DSet Attribute ViBoolean              DSet Attribute ViSession             ?Get Attribute                        DGet Attribute ViInt32                DGet Attribute ViReal64               DGet Attribute ViString               DGet Attribute ViBoolean              DGet Attribute ViSession             Check Attribute                      DCheck Attribute ViInt32              DCheck Attribute ViReal64             DCheck Attribute ViString             DCheck Attribute ViBoolean            DCheck Attribute ViSession           ?Acquisition Functions                DStart Measurement                    DStop Measurement                     DReadForce                           #Measurement Functions                DRead Measurement                     DRead Waveform                       ?Utility Functions                    DGetChannelScale                      DReset                                DReset With Defaults                  DDisable                              DSelf-Test                            DRevision Query                       DError-Query                          DError Message                        DInvalidate All Attributes           ?Error Info                           DGet Error                            DClear Error                         KCoercion Info                        DGet Next Coercion Record            ?Interchangeability Info              DGet Next Interchange Warning         DClear Interchange Warnings           DReset Interchange Check             ?Locking                              DLock Session                         DUnlock Session                       DClose                           