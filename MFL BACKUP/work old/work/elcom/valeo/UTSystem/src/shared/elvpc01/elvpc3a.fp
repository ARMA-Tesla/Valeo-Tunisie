s??        .o   8 #  l   ?   ????                               elvpc3a     ELCOM-VPC Power Switch                        ? ? ??ViInt16  ?  ? ??ViInt32  ? ? ??ViReal64     ? ??ViRsrc     	? 	??ViBoolean     	? 	??ViSession     ? ??ViStatus     ?  ViChar[]     ? ??ViChar     ? ??ViString     	?  ViInt16[]     	?  ViInt32[]     
?  	ViReal64[]     ? 	 
ViBoolean[]     ? ??ViConstString     ? ??ViAttr   ?    This instrument driver contains programming support for the ELCOM-VPC Power Switch.  This driver has all the functions that IVI and VXIplug&play require.  

Note:  This driver requires the VISA and IVI libraries.       ?    This class provides functions and classes that configure the instrument.

Functions/SubClasses:

Set/Get/Check Attribute (Class) - Contains functions that set, get, and check values of attributes.     T    This class contains sub-classes for the set, get, and check attribute functions.       ?    This class contains functions that set an attribute to a new value.  There are typesafe functions for each attribute data type.     ?    This class contains functions that obtain the current value of an attribute.  There are typesafe functions for each attribute data type.     ?    This class contains functions that obtain the current value of an attribute.  There are typesafe functions for each attribute data type.     ?    This class contains functions and classes that initiate instrument operations and report their status.

Functions/SubClasses:

Paths - Contains funcitons for manipulating switch connection paths.
     >    This class contains manipulation of switch connection paths.    _    This class contains functions and sub-classes that control common instrument operations.  These functions include many of functions that VXIplug&play require, such as reset, self-test, revision query, error query, and error message.  This class also contains functions that access IVI error infomation, lock the session, and perform instrument I/O.
     R    This class contains functions that retrieve and clear the IVI error information.     ?    This class contains functions that retrieve coercion records.     J    This class contains functions that retrieve interchangeability warnings.     k    This class contains functions that lock and unlock IVI instrument driver sessions for multithread safefy.    A    This function performs the following initialization actions:

- Creates a new IVI instrument driver session.

- Opens a session to the specified device using the interface and address you specify for the Resource Name parameter.

- If the ID Query parameter is set to VI_TRUE, this function queries the instrument ID and checks that it is valid for this instrument driver.

- If the Reset parameter is set to VI_TRUE, this function resets the instrument to a known state.

- Sends initialization commands to set the instrument to the state necessary for the operation of the instrument driver.

- Returns a ViSession handle that you use to identify the instrument in all subsequent instrument driver function calls.

Note:  This function creates a new session each time you invoke it. Although you can open more than one IVI session for the same resource, it is best not to do so.  You can use the same session in multiple program threads.  You can use the elvpc3a_LockSession and elvpc3a_UnlockSession functions to protect sections of code that require exclusive access to the resource.    
    Pass the resource name of the device to initialize.

You can also pass the name of a virtual instrument or logical name that you configure with the IVI Configuration utility.  The virtual instrument identifies a specific device and specifies the initial settings for the session.  A logical Name identifies a particular virtual instrument.

Refer to the following table below for the exact grammar to use for this parameter.  Optional fields are shown in square brackets ([]).

Syntax
------------------------------------------------------
GPIB[board]::<primary address>[::secondary address]::INSTR
VXI[board]::<logical address>::INSTR
GPIB-VXI[board]::<logical address>::INSTR
ASRL<port>::INSTR
<LogicalName>
<DriverSession>

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
"xyz432" - Driver Session "xyz432"

/*=CHANGE:===================================================* 

Modify the following default value so that it reflects the default address for your instrument.  You must make the corresponding change to the Default Value entry for the control.

 *================================================END=CHANGE=*/ 
Default Value:  "VXI::14::INSTR"
        Specify whether you want the instrument driver to perform an ID Query.

Valid Range:
VI_TRUE  (1) - Perform ID Query (Default Value)
VI_FALSE (0) - Skip ID Query

When you set this parameter to VI_TRUE, the driver verifies that the instrument you initialize is a type that this driver supports.  

Circumstances can arise where it is undesirable to send an ID Query command string to the instrument.  When you set this parameter to VI_FALSE, the function initializes the instrument without performing an ID Query.     ?    Specify whether you want the to reset the instrument during the initialization procedure.

Valid Range:
VI_TRUE  (1) - Reset Device (Default Value)
VI_FALSE (0) - Don't Reset    ?    Returns a ViSession handle that you use to identify the instrument in all subsequent instrument driver function calls.

Notes:

(1) This function creates a new session each time you invoke it.  This is useful if you have multiple physical instances of the same type of instrument.  

(2) Avoid creating multiple concurrent sessions to the same physical instrument.  Although you can create more than one IVI session for the same resource, it is best not to do so.  A better approach is to use the same IVI session in multiple execution threads.  You can use functions elvpc3a_LockSession and elvpc3a_UnlockSession to protect sections of code that require exclusive access to the resource.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors    ? =   ?  ?    Resource Name                     ? : ?       ID Query                          ? =? ?       Reset Device                      | ?C ?  ?    Instrument Handle                 7#????  ?    Status                             "VXI::14::INSTR"   Yes VI_TRUE No VI_FALSE   Yes VI_TRUE No VI_FALSE    	           	          G    This function performs the following initialization actions:

- Creates a new IVI instrument driver and optionally sets the initial state of the following session attributes:

    ELVPC3A_ATTR_RANGE_CHECK         
    ELVPC3A_ATTR_QUERY_INSTRUMENT_STATUS  
    ELVPC3A_ATTR_CACHE               
    ELVPC3A_ATTR_SIMULATE            
    ELVPC3A_ATTR_RECORD_COERCIONS    

- Opens a session to the specified device using the interface and address you specify for the Resource Name parameter.

- If the ID Query parameter is set to VI_TRUE, this function queries the instrument ID and checks that it is valid for this instrument driver.

- If the Reset parameter is set to VI_TRUE, this function resets the instrument to a known state.

- Sends initialization commands to set the instrument to the state necessary for the operation of the instrument driver.

- Returns a ViSession handle that you use to identify the instrument in all subsequent instrument driver function calls.

Note:  This function creates a new session each time you invoke it. Although you can open more than one IVI session for the same resource, it is best not to do so.  You can use the same session in multiple program threads.  You can use the elvpc3a_LockSession and elvpc3a_UnlockSession functions to protect sections of code that require exclusive access to the resource.    
    Pass the resource name of the device to initialize.

You can also pass the name of a virtual instrument or logical name that you configure with the IVI Configuration utility.  The virtual instrument identifies a specific device and specifies the initial settings for the session.  A logical Name identifies a particular virtual instrument.

Refer to the following table below for the exact grammar to use for this parameter.  Optional fields are shown in square brackets ([]).

Syntax
------------------------------------------------------
GPIB[board]::<primary address>[::secondary address]::INSTR
VXI[board]::<logical address>::INSTR
GPIB-VXI[board]::<logical address>::INSTR
ASRL<port>::INSTR
<LogicalName>
<DriverSession>

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
"xyz432" - Driver Session "xyz432"

/*=CHANGE:===================================================* 

Modify the following default value so that it reflects the default address for your instrument.  You must make the corresponding change to the Default Value entry for the control.

 *================================================END=CHANGE=*/ 
Default Value:  "VXI::14::INSTR"
        Specify whether you want the instrument driver to perform an ID Query.

Valid Range:
VI_TRUE  (1) - Perform ID Query (Default Value)
VI_FALSE (0) - Skip ID Query

When you set this parameter to VI_TRUE, the driver verifies that the instrument you initialize is a type that this driver supports.  

Circumstances can arise where it is undesirable to send an ID Query command string to the instrument.  When you set this parameter to VI_FALSE, the function initializes the instrument without performing an ID Query.     ?    Specify whether you want the to reset the instrument during the initialization procedure.

Valid Range:
VI_TRUE  (1) - Reset Device (Default Value)
VI_FALSE (0) - Don't Reset    ?    Returns a ViSession handle that you use to identify the instrument in all subsequent instrument driver function calls.

Notes:

(1) This function creates a new session each time you invoke it.  This is useful if you have multiple physical instances of the same type of instrument.  

(2) Avoid creating multiple concurrent sessions to the same physical instrument.  Although you can create more than one IVI session for the same resource, it is best not to do so.  A better approach is to use the same IVI session in multiple execution threads.  You can use functions elvpc3a_LockSession and elvpc3a_UnlockSession to protect sections of code that require exclusive access to the resource.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors    ?    You can use this control to set the initial value of certain attributes for the session.  The following table lists the attributes and the name you use in this parameter to identify the attribute.

Name              Attribute Defined Constant   --------------------------------------------
RangeCheck        ELVPC3A_ATTR_RANGE_CHECK
QueryInstrStatus  ELVPC3A_ATTR_QUERY_INSTRUMENT_STATUS
Cache             ELVPC3A_ATTR_CACHE
Simulate          ELVPC3A_ATTR_SIMULATE
RecordCoercions   ELVPC3A_ATTR_RECORD_COERCIONS

The format of this string is, "AttributeName=Value" where AttributeName is the name of the attribute and Value is the value to which the attribute will be set.  To set multiple attributes, separate their assignments with a comma.

If you pass NULL or an empty string for this parameter and a VISA resource descriptor for the Resource Name parameter, the session uses the default values for the attributes. The default values for the attributes are shown below:

    Attribute Name     Default Value
    ----------------   -------------
    RangeCheck         VI_TRUE
    QueryInstrStatus   VI_FALSE
    Cache              VI_TRUE
    Simulate           VI_FALSE
    RecordCoercions    VI_FALSE

If you pass NULL or an empty string for this parameter and a virtual instrument or logical name for the Resource Name parameter, the session uses the values that you configure for virtual instrument or logical name with the IVI Configuration utility.

You can override the values of the attributes by assigning a value explicitly in a string you pass for this parameter.  You do not have to specify all of the attributes and may leave any of them out.  If you do not specify one of the attributes, its default value or the value that you configure with the IVI Configuration utility will be used.

The following are the valid values for ViBoolean attributes:
    True:     1, TRUE, or VI_TRUE
    False:    0, False, or VI_FALSE

Default Value:
"Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1"    )? =   ?  ?    Resource Name                     1? : ?       ID Query                          3? =? ?       Reset Device                      4x ?Y ?  ?    Instrument Handle                 72#????  ?    Status                            >? ?  ? ?    Option String                      "VXI::14::INSTR"   Yes VI_TRUE No VI_FALSE   Yes VI_TRUE No VI_FALSE    	           	           5"Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1"   ?    This function sets the value of a ViInt32 attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""
    Mo-   ?  ?    Instrument Handle                 N'#????  ?    Status                            V
 ? ? ?  ?    Attribute Value                 ???? ? ???                                           X% = ? ? ?    Attribute ID                      ]p =  ?  ?    Channel Name                           	               .Press <ENTER> for a list of 
value constants.                0    ""   ?    This function sets the value of a ViReal64 attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""
    d?-   ?  ?    Instrument Handle                 e?#????  ?    Status                            m? ? ? ?  ?    Attribute Value                 ???? ? ???                                           o? = ? ? ?    Attribute ID                      t? =  ?  ?    Channel Name                           	               .Press <ENTER> for a list of 
value constants.                0    ""   ?    This function sets the value of a ViString attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""
    |?-   ?  ?    Instrument Handle                 }F#????  ?    Status                            ?) ? ? ?  ?    Attribute Value                 ???? ? ???                                           ?D = ? ? ?    Attribute ID                      ?? =  ?  ?    Channel Name                           	               .Press <ENTER> for a list of 
value constants.                0    ""   ?    This function sets the value of a ViBoolean attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""
    ?-   ?  ?    Instrument Handle                 ??#????  ?    Status                            ?? ? ? ?  ?    Attribute Value                 ???? ? ???                                           ?? = ? ? ?    Attribute ID                      ? =  ?  ?    Channel Name                           	               .Press <ENTER> for a list of 
value constants.                0    ""   ?    This function sets the value of a ViSession attribute.

This is a low-level function that you can use to set the values of instrument-specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid or is different than the value you specify. 

This instrument driver contains high-level functions that set most of the instrument attributes.  It is best to use the high-level driver functions as much as possible.  They handle order dependencies and multithread locking for you.  In addition, they perform status checking only after setting all of the attributes.  In contrast, when you set multiple attributes using the SetAttribute functions, the functions check the instrument status after each call.

Also, when state caching is enabled, the high-level functions that configure multiple attributes perform instrument I/O only for the attributes whose value you change.  Thus, you can safely call the high-level functions without the penalty of redundant instrument I/O.


     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to set the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
    ??-   ?  ?    Instrument Handle                 ?f#????  ?    Status                            ?I ? ? ?  ?    Attribute Value                   ?d =  ?  ?    Channel Name                      ?? = ? ? ?    Attribute ID                    ???? ? ???                                                	               ""                0    .Press <ENTER> for a list of 
value constants.   ?    This function queries the value of a ViInt32 attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
    ?'-   ?  ?    Instrument Handle                 ??#????  ?    Status                            ?? ? ? ?  ?    Attribute Value                   ʀ =  ?  ?    Channel Name                      ˣ = ? ? ?    Attribute ID                           	           	            ""                0   ?    This function queries the value of a ViReal64 attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
    ?>-   ?  ?    Instrument Handle                 ??#????  ?    Status                            ?? ? ? ?  ?    Attribute Value                   ݘ =  ?  ?    Channel Name                      ޻ = ? ? ?    Attribute ID                           	           	           ""                0   4    This function queries the value of a ViString attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

You must provide a ViChar array to serve as a buffer for the value.  You pass the number of bytes in the buffer as the Buffer Size parameter.  If the current value of the attribute, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you want to call this function just to get the required buffer size, you can pass 0 for the Buffer Size and VI_NULL for the Attribute Value buffer.  

If you want the function to fill in the buffer regardless of the   number of bytes in the value, pass a negative number for the Buffer Size parameter.  


     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    	?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""
    ?    Pass the number of bytes in the ViChar array you specify for the Attribute Value parameter.  

If the current value of the attribute, including the terminating NUL byte, contains more bytes that you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Attribute Value buffer parameter.

Default Value: 512    ?    Pass the ID of an attribute.

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
    ??-   ?  ?    Instrument Handle                 ??#????  ?    Status or Required Size           ?. ? L ? ?    Attribute Value                   ?\ =  ?  ?    Channel Name                      ? =? ?  ?    Buffer Size                       ?u = ? ? ?    Attribute ID                           	           	            ""    512                0   ?    This function queries the value of a ViBoolean attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
   N-   ?  ?    Instrument Handle                #????  ?    Status                           ? ? ? ?  ?    Attribute Value                  ? =  ?  ?    Channel Name                     ? = ? ? ?    Attribute ID                           	           	            ""                0   ?    This function queries the value of a ViSession attribute.

You can use this function to get the values of instrument- specific attributes and inherent IVI attributes.  If the attribute represents an instrument state, this function performs instrument I/O in the following cases:

- State caching is disabled for the entire session or for the particular attribute.

- State caching is enabled and the currently cached value is invalid. 

     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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
        If the attribute is channel-based, this parameter specifies the name of the channel on which to obtain the value of the attribute. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
   k-   ?  ?    Instrument Handle                ##????  ?    Status                             ? ? ?  ?    Attribute Value                  !? =  ?  ?    Channel Name                     "? = ? ? ?    Attribute ID                           	           	            ""                0    S    This function checks the validity of a value you specify for a ViInt32 attribute.     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
   )$-   ?  ?    Instrument Handle                )?#????  ?    Status                           1? ? ? ?  ?    Attribute Value                  3? =  ?  ?    Channel Name                    ???? ? ???                                          5
 = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0    T    This function checks the validity of a value you specify for a ViReal64 attribute.     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
   <I-   ?  ?    Instrument Handle                =#????  ?    Status                           D? ? ? ?  ?    Attribute Value                  G =  ?  ?    Channel Name                    ???? ? ???                                          H/ = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0    T    This function checks the validity of a value you specify for a ViString attribute.     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
   Op-   ?  ?    Instrument Handle                P(#????  ?    Status                           X ? ? ?  ?    Attribute Value                  Z; =  ?  ?    Channel Name                    ???? ? ???                                          [V = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0    U    This function checks the validity of a value you specify for a ViBoolean attribute.     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
   b?-   ?  ?    Instrument Handle                cP#????  ?    Status                           k3 ? ? ?  ?    Attribute Value                  mc =  ?  ?    Channel Name                    ???? ? ???                                          n~ = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0    U    This function checks the validity of a value you specify for a ViSession attribute.     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
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

Default Value: none        If the attribute is channel-based, this parameter specifies the name of the channel on which to check the attribute value. If the attribute is not channel-based, then pass VI_NULL or an empty string.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

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
   u?-   ?  ?    Instrument Handle                vz#????  ?    Status                           ~] ? ? ?  ?    Attribute Value                  ?? =  ?  ?    Channel Name                    ???? ? ???                                          ?? = ? ? ?    Attribute ID                           	               ""    .Press <ENTER> for a list of 
value constants.                0   
    This function creates a path between Channel 1 and Channel 2.  The driver calculates the shortest path between the two channels.

/*=CHANGE:====================================================*/
    Add comments here about possible connections for your instrument. 
/*==================================================END=CHANGE*/

If a path is not available, the function returns one of the following errors:

ELVPC3A_ERROR_EXPLICIT_CONNECTION_EXISTS (0xBFFA200C), if the two channels
                  are already explicitly connected by calling
                  either the elvpc3a_Connect or 
                  elvpc3a_SetPath function.

ELVPC3A_ERROR_IS_CONFIGURATION_CHANNEL (0xBFFA2009), if a channel is a
                  configuration channel.  Error elaboration
                  contains information about which of the two
                  channels is a configuration channel.

ELVPC3A_ERROR_ATTEMPT_TO_CONNECT_SOURCES (0xBFFA200B), if both channels are
                  connected to a different source.  Error
                  elaboration contains information about sources
                  channel 1 and 2 connect to.

ELVPC3A_ERROR_CANNOT_CONNECT_TO_ITSELF (0xBFFA2015), if channels 1 and 2 are
                  one and the same channel.

ELVPC3A_ERROR_PATH_NOT_FOUND (0xBFFA2011), if the driver cannot find a path
                  between the two channels.

Notes:

(1) The paths are bidirectional. For example, if a path exists
    between channels CH1 and CH2, then the path between channels
    CH2 and CH1 also exists.     ?    You identify a path with two channels. Pass one of the channel names for which you want to create a path. Pass the other channel name as the Channel 2 parameter.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors     ?    You identify a path with two channels. Pass one of the channel names for which you want to create a path. Pass the other channel name as the Channel 1 parameter.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""   ?? Q 9 ?  ?    Channel 1                        ??-   ?  ?    Instrument Handle                ?M#????  ?    Status                           ?% Q` ?  ?    Channel 2                          ""        	           ""   F    This function destroys the path between two channels that you create with the elvpc3a_Connect or elvpc3a_SetPath function.

/*=CHANGE:====================================================*/
    Add comments here about possible connections for your instrument.
/*==================================================END=CHANGE*/
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    You identify a path with two channels. Pass one of the channel names that identify the path you want to destroy. Pass the other channel name as the Channel 2 parameter.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""
     ?    You identify a path with two channels. Pass one of the channel names that identify the path you want to destroy. Pass the other channel name as the Channel 1 parameter.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""
   ?W-   ?  ?    Instrument Handle                ?#????  ?    Status                           ?? Q 9 ?  ?    Channel 1                        ?? Q` ?  ?    Channel 2                              	           ""    ""    ?    This function disconnects all existing paths.

Note: If the switch module is not capable of disconnecting all paths, this function returns ELVPC3A_WARN_PATH_REMAINS (0x3FFA2001) warning.     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings

BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors   ??  	  ?  ?    Instrument Handle                ?U ????  ?    Status                                 	           v    This function returns the state of the switch module. It indicates if all the paths that you created have settled.

     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors        Indicates the state of the switch module. The driver returns the value of ELVPC3A_ATTR_IS_DEBOUNCED attribute.

The value VI_TRUE indicates that all the paths that you created have settled.

The value VI_FALSE indicates that all the paths that you created have  not settled.   ?? 	  ?  ?    Instrument Handle                ??????  ?    Status                           ?g b ? ?  ?    Is Debounced                           	           	           K    This function returns after all the paths that you create have settled.

     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings

BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors     ?    Specifies the maximum length of time for this function to wait until all switches in the switch module debounce.
If the time you specify elapses before all switches debounce, this function returns a timeout error.

The units are milliseconds.   ??   ?  ?    Instrument Handle                ?H????  ?    Status                           ?? a ? ?  ?    Maximum Time (ms)                      	                  This function verifies that the switch module is capable of creating a path between the two channels you specify with the Channel 1 and Channel 2 parameters.  If the switch module is capable of creating a path, this function indicates whether the path is currently available given the existing connections.

If the path is not available due to the currently existing connections, but the implicit connection between the two channels already exists, the function returns the ELVPC3A_WARN_IMPLICIT_CONNECTION_EXISTS (0x3FFA2002) warning.     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings

BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors     ?    You identify a path with two channels. Pass one of the channel names for which you want to verify a path. Pass the other channel name as the Channel 2 parameter.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""     ?    You identify a path with two channels. Pass one of the channel names for which you want to verify a path. Pass the other channel name as the Channel 1 parameter.


Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""    8    Indicates whether a path is valid. Possible values include:

Status Name                           Actual Value
--------------------------------------------------
ELVPC3A_VAL_PATH_AVAILABLE         1
ELVPC3A_VAL_PATH_EXISTS            2
ELVPC3A_VAL_PATH_UNSUPPORTED       3
ELVPC3A_VAL_RSRC_IN_USE            4
ELVPC3A_VAL_SOURCE_CONFLICT        5
ELVPC3A_VAL_CHANNEL_NOT_AVAILABLE  6

Notes:

(1) ELVPC3A_VAL_PATH_AVAILABLE indicates that the driver can create the path at this time.

(2) ELVPC3A_VAL_PATH_EXISTS indicates that the path already exists.

(3) ELVPC3A_VAL_PATH_UNSUPPORTED indicates that the instrument is not capable of creating a path between the channels you specify.

(4) ELVPC3A_VAL_RSRC_IN_USE indicates that although the path is valid, the driver cannot create the path at this moment because the switch module is currently using one or more of the required channels to create another path. You must destroy the other path before creating this one.

(5) ELVPC3A_VAL_SOURCE_CONFLICT indicates that the instrument cannot create a path because both channels are connected to a different source channel.

(6) ELVPC3A_VAL_CHANNEL_NOT_AVAILABLE indicates that the driver cannot create a path between the two channels because one of the channels is a configuration channel and thus unavailable for external connections.   ??   ?  ?    Instrument Handle                ?{????  ?    Status                           ?# \  ?  ?    Channel 1                        ? \ ? ?  ?    Channel 2                        ? \? ?  ?    Path Capability                        	           ""    ""    	           6    This function connects two channels by establishing the exact path you specify with the pathList parameter.

/*=CHANGE:====================================================*/
    Add comments here about possible connections for your instrument.
/*==================================================END=CHANGE*/     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings

BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors        Pass the path list for the path you previously created that you want the switch module to establish. You obtain the path list for a path you previously created with the elvpc3a_GetPath function.

Example: The path from Ch1 to Ch3 over Conf1 is:

"Ch1->Conf1,Conf1->Ch3"   ٻ 	  ?  ?    Instrument Handle                ?r????  ?    Status                           ? L ? ?      Path List                              	           ""   n    In some cases there is more than one possible path between two channels. The driver or the instrument selects the path when you connect two channels with the elvpc3a_Connect function. Thus, you cannot guarantee that every call to the elvpc3a_Connect function establishes exactly the same path when you pass the same channels. This function returns a string that uniquely identifies the path you create with the elvpc3a_Connect function. You can pass this string to the elvpc3a_SetPath function to establish the exact same path in the future.

Note:

(1) This function returns only those paths that you explicitly
    create by calling elvpc3a_Connect and elvpc3a_SetPath
    functions. For example, if you connect channels CH1 and CH3,
    and then channels CH2 and CH3, the explicit path between
    channels CH1 and Ch2 does not exist and this function
    returns an error.     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None    	b    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings

BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors     ?    You identify a path with two channels. Pass one of the channel names for which you want to obtain a path. Pass the other channel name as the Channel 2 parameter.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""     ?    You identify a path with two channels. Pass one of the channel names for which you want to obtain a path. Pass the other channel name as the Channel 1 parameter.

Valid Channel Names:  CH1A,CH1B,C1A1,C1A2,C1B1,C1B2

Default Value:  ""     b    The comma-separated path between channels you specify in the Channel 1 and Channel 2 parameters.    ?    Pass the number of bytes in the ViChar array you specify for the Path List parameter.

If the current value of the attribute, including the terminating NUL byte, contains more bytes that you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "R1->C1" and the Buffer Size is 4, the function places "R1-" into the buffer and returns 7.

If you pass 0, you can pass VI_NULL for the Path parameter. This enables you to find out the path size and to allocate the buffer of the appropriate size before calling this function again.   ?` 	  ?  ?    Instrument Handle                ?????  ?    Status or Required Size          ?? J  ?  ?    Channel 1                        ?u J ? ?  ?    Channel 2                        ?i ? ? ?      Path                             ?? J? ?  ?    Buffer Size                            	           ""    ""    	               &    This function resets the instrument to a known state and sends initialization commands to the instrument.  The initialization commands set instrument settings such as Headers Off, Short Command form, and Data Transfer Binary to the state necessary for the operation of the instrument driver.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   ?9#????  ?    Status                           -   ?  ?    Instrument Handle                  	                   This function resets the instrument and applies initial user specified settings from the Logical Name which was used to initialize the session.  If the session was created without a Logical Name, this function is equivalent to the elvpc3a_reset function.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   V#????  ?    Status                           9-   ?  ?    Instrument Handle                  	               ?    This function places the instrument in a quiescent state where it has minimal or no impact on the system to which it is connected.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   ?#????  ?    Status                           ?-   ?  ?    Instrument Handle                  	               Z    This function runs the instrument's self test routine and returns the test result(s). 

    &    This control contains the value returned from the instrument self test.  Zero means success.  For any other code, see the device's operator's manual.

Self-Test Code    Description
---------------------------------------
   0              Passed self test
   1              Self test failed

     ?    Returns the self-test response string from the instrument. See the device's operation manual for an explanation of the string's contents.

You must pass a ViChar array with at least 256 bytes.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   n =  ?  ?    Self Test Result                 ? = ? ? ,    Self-Test Message                f#????  ?    Status                            I-   ?  ?    Instrument Handle                  	           	            	               `    This function returns the revision numbers of the instrument driver and instrument firmware.

     ?    Returns the instrument driver software revision numbers in the form of a string.

You must pass a ViChar array with at least 256 bytes.     ?    Returns the instrument firmware revision numbers in the form of a string.

You must pass a ViChar array with at least 256 bytes.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   "e = 3 ?  ?    Instrument Driver Revision       "? =6 ?  ?    Firmware Revision                #?#????  ?    Status                           +d-   ?  ?    Instrument Handle                  	            	            	               V    This function reads an error code and a message from the instrument's error queue.

     B    Returns the error code read from the instrument's error queue.

     ?    Returns the error message string read from the instrument's error message queue.

You must pass a ViChar array with at least 256 bytes.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   -v =  ?  ?    Error Code                       -? = ? ? ,    Error Message                    .R#????  ?    Status                           65-   ?  ?    Instrument Handle                  	           	            	               n    This function converts a status code returned by an instrument driver function into a user-readable string.     *?    Pass the Status parameter that is returned from any of the instrument driver functions.
          
Default Value:  0  (VI_SUCCESS)
          
elvpc3a Status Codes:
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

IviSwtch Status Codes:
Status    Description
-------------------------------------------------
WARNINGS:
3FFA2001  Some connections remain after disconnecting.
3FFA2002  The channels are implicitly connected.  

ERRORS:
BFFA2001  Invalid path string.
BFFA2002  Invalid Scan List string.
BFFA2003  One of the channels is in use.
BFFA2004  The scan list string is empty.
BFFA2005  The path string is empty.
BFFA2006  The switch module is currently in scanning mode.
BFFA2007  The switch module is not currently in scanning mode.
BFFA2008  No explicit path exists between the two channels.
BFFA2009  Cannot explicitly connect a configuration channel.
BFFA200A  One path channel is not a configuration channel.
BFFA200B  Cannot connect two sources.
BFFA200C  The channels are explicitly connected.
BFFA200D  A leg in the path does not begin with a channel name.
BFFA200E  A leg in the path is missing the second channel name.
BFFA200F  The first and second channels in the leg are the same.
BFFA2010  Duplicate channel in the path string.
BFFA2011  No path found between the channels.
BFFA2012  Beginning and end of adjacent legs are not the same.
BFFA2013  Path contains a leg with un-connectable channels.
BFFA2014  A leg in the path is already connected.
BFFA2015  A channel cannot be connected to itself.

IVI Engine Status Codes:
Status    Description
-------------------------------------------------
ERRORS:
BFFA0001  Instrument error. Call elvpc3a_error_query.
BFFA0002  Cannot open file.
BFFA0003  Error reading from file.
BFFA0004  Error writing to file.
BFFA0005  Driver module file not found.
BFFA0006  Cannot open driver module file for reading.
BFFA0007  Driver module has invalid file format or invalid data.
BFFA0008  Driver module contains undefined references.
BFFA0009  Cannot find function in driver module.
BFFA000A  Failure loading driver module.
BFFA000B  Invalid path name.
BFFA000C  Invalid attribute.
BFFA000D  IVI attribute is not writable.
BFFA000E  IVI attribute is not readable.
BFFA000F  Invalid parameter.
BFFA0010  Invalid value.
BFFA0011  Function not supported.
BFFA0012  Attribute not supported.
BFFA0013  Value not supported.
BFFA0014  Invalid type.
BFFA0015  Types do not match.
BFFA0016  Attribute already has a value waiting to be updated.
BFFA0017  Specified item already exists.
BFFA0018  Not a valid configuration.
BFFA0019  Requested item does not exist or value not available.
BFFA001A  Requested attribute value not known.
BFFA001B  No range table.
BFFA001C  Range table is invalid.
BFFA001D  Object or item is not initialized.
BFFA001E  Non-interchangeable behavior.
BFFA001F  No channel table has been built for the session.
BFFA0020  Channel name specified is not valid.
BFFA0021  Unable to allocate system resource.
BFFA0022  Permission to access file was denied.
BFFA0023  Too many files are already open.
BFFA0024  Unable to create temporary file in target directory.
BFFA0025  All temporary filenames already used.
BFFA0026  Disk is full.
BFFA0027  Cannot find configuration file on disk.
BFFA0028  Cannot open configuration file.
BFFA0029  Error reading configuration file.
BFFA002A  Invalid ViInt32 value in configuration file.
BFFA002B  Invalid ViReal64 value in configuration file.
BFFA002C  Invalid ViBoolean value in configuration file.
BFFA002D  Entry missing from configuration file.
BFFA002E  Initialization failed in driver DLL.
BFFA002F  Driver module has unresolved external reference.
BFFA0030  Cannot find CVI Run-Time Engine.
BFFA0031  Cannot open CVI Run-Time Engine.
BFFA0032  CVI Run-Time Engine has invalid format.
BFFA0033  CVI Run-Time Engine is missing required function(s).
BFFA0034  CVI Run-Time Engine initialization failed.
BFFA0035  CVI Run-Time Engine has unresolved external reference.
BFFA0036  Failure loading CVI Run-Time Engine.
BFFA0037  Cannot open DLL for read exports.
BFFA0038  DLL file is corrupt.
BFFA0039  No DLL export table in DLL.
BFFA003A  Unknown attribute name in default configuration file.
BFFA003B  Unknown attribute value in default configuration file.
BFFA003C  Memory pointer specified is not known.
BFFA003D  Unable to find any channel strings.
BFFA003E  Duplicate channel string.
BFFA003F  Duplicate virtual channel name.
BFFA0040  Missing virtual channel name.
BFFA0041  Bad virtual channel name.
BFFA0042  Unassigned virtual channel name.
BFFA0043  Bad virtual channel assignment.
BFFA0044  Channel name required.
BFFA0045  Channel name not allowed.
BFFA0046  Attribute not valid for channel.
BFFA0047  Attribute must be channel based.
BFFA0048  Channel already excluded.
BFFA0049  Missing option name (nothing before the '=').
BFFA004A  Missing option value (nothing after the '=').
BFFA004B  Bad option name.
BFFA004C  Bad option value.
BFFA004D  Operation only valid on a class driver session.
BFFA004E  "ivi.ini" filename is reserved.
BFFA004F  Duplicate run-time configuration entry.
BFFA0050  Index parameter is one-based.
BFFA0051  Index parameter is too high.
BFFA0052  Attribute is not cacheable.
BFFA0053  You cannot export a ViAddr attribute to the end-user.
BFFA0054  Bad channel string in channel string list.
BFFA0055  Bad prefix name in default configuration file.
         
VISA Status Codes:
Status    Description
-------------------------------------------------
WARNINGS:
3FFF0002 Event enabled for one or more specified mechanisms.
3FFF0003 Event disabled for one or more specified mechanisms.
3FFF0004 Successful, but queue already empty.
3FFF0005 Specified termination character was read.
3FFF0006 Number of bytes transferred equals input count.
3FFF0077 Configuration non-existant or could not be loaded.
3FFF007D Open successful, but the device not responding.
3FFF0080 Wait successful, but more event objects available.
3FFF0082 Specified object reference is uninitialized.
3FFF0084 Attribute value not supported.
3FFF0085 Status code could not be interpreted.
3FFF0088 Specified I/O buffer type not supported.
3FFF0098 Successful,  but invoke no handlers for this event.
3FFF0099 Successful but session has nested shared locks.
3FFF009A Successful but session has nested exclusive locks.
3FFF009B Successful but operation not asynchronous.
         
ERRORS:
BFFF0000 Unknown system error (miscellaneous error).
BFFF000E Session or object reference is invalid.
BFFF000F Resource is locked.
BFFF0010 Invalid expression specified for search.
BFFF0011 Resource is not present in the system.
BFFF0012 Invalid resource reference specified.  Parsing error.
BFFF0013 Invalid access mode.
BFFF0015 Timeout expired before operation completed.
BFFF0016 Unable to deallocate session data structures.
BFFF001B Specified degree is invalid.
BFFF001C Specified job identifier is invalid.
BFFF001D Attribute is not supported by the referenced object.
BFFF001E Attribute state not supported by the referenced object.
BFFF001F Specified attribute is read-only.
BFFF0020 Lock type lock not supported by this resource.
BFFF0021 Invalid access key.
BFFF0026 Specified event type not supported by the resource.
BFFF0027 Invalid mechanism specified.
BFFF0028 A handler was not installed.
BFFF0029 Handler reference either invalid or was not installed.
BFFF002A Specified event context invalid.
BFFF002D Event queue for specified type has overflowed.
BFFF002F Event type must be enabled in order to receive.
BFFF0030 User abort during transfer.
BFFF0034 Violation of raw write protocol during transfer.
BFFF0035 Violation of raw read protocol during transfer.
BFFF0036 Device reported output protocol error during transfer.
BFFF0037 Device reported input protocol error during transfer.
BFFF0038 Bus error during transfer.
BFFF0039 Unable to queue asynchronous operation.
BFFF003A Unable to start operation because setup is invalid.
BFFF003B Unable to queue the asynchronous operation.
BFFF003C Insufficient resources to perform memory allocation.
BFFF003D Invalid buffer mask specified.
BFFF003E I/O error.
BFFF003F Format specifier invalid.
BFFF0041 Format specifier not supported.
BFFF0042 Trigger line is currently in use.
BFFF004A Service request not received for the session.
BFFF004E Invalid address space specified.
BFFF0051 Invalid offset specified.
BFFF0052 Invalid access width specified.
BFFF0054 Offset not accessible from this hardware.
BFFF0055 Source and destination widths are different.
BFFF0057 Session not currently mapped.
BFFF0059 Previous response still pending.
BFFF005F No listeners condition detected.
BFFF0060 Interface not currently the controller in charge.
BFFF0061 Interface not the system controller.
BFFF0067 Session does not support this operation.
BFFF006A A parity error occurred during transfer.
BFFF006B A framing error occurred during transfer.
BFFF006C An overrun error occurred during transfer.
BFFF0070 Offset not properly aligned for operation access width.
BFFF0071 Specified user buffer not valid.
BFFF0072 Resource valid, but VISA cannot access it.
BFFF0076 Width not supported by this hardware.
BFFF0078 Invalid parameter value, parameter unknown.
BFFF0079 Invalid protocol.
BFFF007B Invalid window size.
BFFF0080 Session currently contains a mapped window.
BFFF0081 Operation not implemented.
BFFF0083 Invalid length.
BFFF0091 Invalid mode.
BFFF009C Session did not have a lock on the resource.
BFFF009D The device does not export any memory.
BFFF009E VISA-required code library not located or not loaded.
         
VXIPnP Driver Status Codes:
Status    Description
-------------------------------------------------
WARNINGS:
3FFC0101  Instrument does not have ID Query capability.
3FFC0102  Instrument does not have Reset capability.
3FFC0103  Instrument does not have Self-Test capability.
3FFC0104  Instrument does not have Error Query capability.
3FFC0105  Instrument does not have Revision Query capability.
          
ERRORS:
BFFC0001  Parameter 1 out of range, or error trying to set it.
BFFC0002  Parameter 2 out of range, or error trying to set it.
BFFC0003  Parameter 3 out of range, or error trying to set it.
BFFC0004  Parameter 4 out of range, or error trying to set it.
BFFC0005  Parameter 5 out of range, or error trying to set it.
BFFC0006  Parameter 6 out of range, or error trying to set it.
BFFC0007  Parameter 7 out of range, or error trying to set it.
BFFC0008  Parameter 8 out of range, or error trying to set it.
BFFC0011  Instrument failed the ID Query.
BFFC0012  Invalid response from instrument.     ?    Returns the user-readable message string that corresponds to the status code you specify.

You must pass a ViChar array with at least 256 bytes.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
        The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

You can pass VI_NULL for this parameter.  This is useful when one of the initialize functions fail.

Default Value:  VI_NULL
   8_ =  ?  h    Error Code                       cM = ? ? ?    Error Message                    c?#????  ?    Status                           k?-   ?  ?    Instrument Handle                  0    	            	           VI_NULL    P    This function invalidates the cached values of all attributes for the session.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   n?#????  ?    Status                           v"-   ?  ?    Instrument Handle                  	              b    This function returns the highest-level channel name that corresponds to the specific driver channel string that is in the channel table at an index you specify.  By passing 0 for the buffer size, the caller can ascertain the buffer size required to get the entire channel name string and then call the function again with a sufficiently large buffer.
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None    	z    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, or if the status code is not listed below, call the elvpc3a_error_message function.  To obtain additional information about the error condition, use the elvpc3a_GetError and elvpc3a_ClearError functions.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ;    A 1-based index into the channel table.

Default Value: 1        Returns the highest-level channel name that corresponds to the specific driver channel string that is in the channel table at an index you specify..

The buffer must contain at least as many elements as the value you specify with the Buffer Size parameter.  If the channel name description, including the terminating NUL byte, contains more bytes than you indicate with the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass 0 for the Buffer Size, you can pass VI_NULL for this parameter.    ?    Pass the number of bytes in the ViChar array you specify for the Channel Name parameter.

If the channel name, including the terminating NUL byte, contains more bytes than you indicate in this parameter, the function copies BufferSize - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Channel Name buffer parameter.

Default Value:  None   x?-   ?  ?    Instrument Handle                yu#????  ?    Status                           ?? = 0 ?  ?    Index                            ?: ?6 ?  ?    Channel Name                     ?H ? 1 ?  ?    Buffer Size                            	           1    	               ?    This function retrieves and then clears the IVI error information for the session or the current execution thread. One exception exists: If the BufferSize parameter is 0, the function does not clear the error information. By passing 0 for the buffer size, the caller can ascertain the buffer size required to get the entire error description string and then call the function again with a sufficiently large buffer.

If the user specifies a valid IVI session for the InstrumentHandle parameter, Get Error retrieves and then clears the error information for the session.  If the user passes VI_NULL for the InstrumentHandle parameter, this function retrieves and then clears the error information for the current execution thread.  If the InstrumentHandle parameter is an invalid session, the function does nothing and returns an error. Normally, the error information describes the first error that occurred since the user last called elvpc3a_GetError or elvpc3a_ClearError.
    	D    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Pass the number of bytes in the ViChar array you specify for the Description parameter.

If the error description, including the terminating NUL byte, contains more bytes than you indicate in this parameter, the function copies BufferSize - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Description buffer parameter.

Default Value:  None     ?    Returns the error code for the session or execution thread.

If you pass 0 for the Buffer Size, you can pass VI_NULL for this parameter.
    ?    Returns the error description for the IVI session or execution thread.  If there is no description, the function returns an empty string.

The buffer must contain at least as many elements as the value you specify with the Buffer Size parameter.  If the error description, including the terminating NUL byte, contains more bytes than you indicate with the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass 0 for the Buffer Size, you can pass VI_NULL for this parameter.   ?,#????  ?    Status or Required Size          ?x-   ?  ?    Instrument Handle                ?0 3< ?  ?    BufferSize                       ? 3 P ?  ?    Code                             ?? ? Q ? ?    Description                        	                   	           	           a    This function clears the error code and error description for the IVI session. If the user specifies a valid IVI session for the InstrumentHandle parameter, this function clears the error information for the session. If the user passes VI_NULL for the Vi parameter, this function clears the error information for the current execution thread. If the Vi parameter is an invalid session, the function does nothing and returns an error.
The function clears the error code by setting it to VI_SUCCESS.  If the error description string is non-NULL, the function de-allocates the error description string and sets the address to VI_NULL.

Maintaining the error information separately for each thread is useful if the user does not have a session handle to pass to the elvpc3a_GetError function, which occurs when a call to elvpc3a_init or elvpc3a_InitWithOptions fails.    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   ?=#????  ?    Status                           ? -   ?  ?    Instrument Handle                  	              ?    This function returns the coercion information associated with the IVI session.  This function retrieves and clears the oldest instance in which the instrument driver coerced a value you specified to another value.

If you set the ELVPC3A_ATTR_RECORD_COERCIONS attribute to VI_TRUE, the instrument driver keeps a list of all coercions it makes on ViInt32 or ViReal64 values you pass to instrument driver functions.  You use this function to retrieve information from that list.

If the next coercion record string, including the terminating NUL byte, contains more bytes than you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Coercion Record buffer parameter.

The function returns an empty string in the Coercion Record parameter if no coercion records remain for the session.

    	?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init function.  The handle identifies a particular instrument session.

Default Value:  None    ?    Returns the next coercion record for the IVI session.  If there are no coercion records, the function returns an empty string.

The buffer must contain at least as many elements as the value you specify with the Buffer Size parameter.  If the next coercion record string, including the terminating NUL byte, contains more bytes than you indicate with the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

This parameter returns an empty string if no coercion records remain for the session.
    ?    Pass the number of bytes in the ViChar array you specify for the Coercion Record parameter.

If the next coercion record string, including the terminating NUL byte, contains more bytes than you indicate in this parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

If you pass a negative number, the function copies the value to the buffer regardless of the number of bytes in the value.

If you pass 0, you can pass VI_NULL for the Coercion Record buffer parameter.

Default Value:  None

   ?)#????  ?    Status or Required Size          ??-   ?  ?    Instrument Handle                ?f ? Q ? ?    Coercion Record                  ?l = ? ?  ?    Buffer Size                        	               	               .    This function returns the interchangeability warnings associated with the IVI session. It retrieves and clears the oldest instance in which the class driver recorded an interchangeability warning.  Interchangeability warnings indicate that using your application with a different instrument might cause different behavior. You use this function to retrieve interchangeability warnings.

The driver performs interchangeability checking when the ELVPC3A_ATTR_INTERCHANGE_CHECK attribute is set to VI_TRUE.

The function returns an empty string in the Interchange Warning parameter if no interchangeability warnings remain for the session.

In general, the instrument driver generates interchangeability warnings when an attribute that affects the behavior of the instrument is in a state that you did not specify.
    	z    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

If the current value of the return buffer, including the terminating NUL byte, is larger than the size you indicate in the Buffer Size parameter, the function copies Buffer Size - 1 bytes into the buffer, places an ASCII NUL byte at the end of the buffer, and returns the buffer size you must pass to get the entire value.  For example, if the value is "123456" and the Buffer Size is 4, the function places "123" into the buffer and returns 7.

To obtain a text description of the status code, or if the status code is not listed below, call the elvpc3a_error_message function.  To obtain additional information about the error condition, use the elvpc3a_GetError and elvpc3a_ClearError functions.

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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

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

   ŋ#????  ?    Status or Required Size          ?-   ?  ?    Instrument Handle                ?? = ? ?  ?    Buffer Size                      ?? ? Q ? ?    Interchange Warning                	                   	            A    This function clears the list of current interchange warnings.
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
   ?,-   ?  ?    Instrument Handle                ??#????  ?    Status                                 	          ?    When developing a complex test system that consists of multiple test modules, it is generally a good idea to design the test modules so that they can run in any order.  To do so requires ensuring that each test module completely configures the state of each instrument it uses.  If a particular test module does not completely configure the state of an instrument, the state of the instrument depends on the configuration from a previously executed test module.  If you execute the test modules in a different order, the behavior of the instrument and therefore the entire test module is likely to change.  This change in behavior is generally instrument specific and represents an interchangeability problem.

You can use this function to test for such cases.  After you call this function, the interchangeability checking algorithms in the specific driver ignore all previous configuration operations.  By calling this function at the beginning of a test module, you can determine whether the test module has dependencies on the operation of previously executed test modules.

This function does not clear the interchangeability warnings from the list of previously recorded interchangeability warnings.  If you want to guarantee that the elvpc3a_GetNextInterchangeWarning function only returns those interchangeability warnings that are generated after calling this function, you must clear the list of interchangeability warnings.  You can clear the interchangeability warnings list by repeatedly calling the elvpc3a_GetNextInterchangeWarning function until no more interchangeability warnings are returned.  If you are not interested in the content of those warnings, you can call the elvpc3a_ClearInterchangeWarnings function.
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
   ?-   ?  ?    Instrument Handle                ??#????  ?    Status                                 	          .    This function obtains a multithread lock on the instrument session.  Before it does so, it waits until all other execution threads have released their locks on the instrument session.

Other threads might have obtained a lock on this session in the following ways:

- The user's application called elvpc3a_LockSession.

- A call to the instrument driver locked the session.

- A call to the IVI engine locked the session.

After your call to elvpc3a_LockSession returns successfully, no other threads can access the instrument session until you call elvpc3a_UnlockSession.

Use elvpc3a_LockSession and elvpc3a_UnlockSession around a sequence of calls to instrument driver functions if you require that the instrument retain its settings through the end of the sequence.

You can safely make nested calls to elvpc3a_LockSession within the same thread.  To completely unlock the session, you must balance each call to elvpc3a_LockSession with a call to elvpc3a_UnlockSession.  If, however, you use the Caller Has Lock parameter in all calls to elvpc3a_LockSession and elvpc3a_UnlockSession within a function, the IVI Library locks the session only once within the function regardless of the number of calls you make to elvpc3a_LockSession.  This allows you to call elvpc3a_UnlockSession just once at the end of the function. 
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    This parameter serves as a convenience.  If you do not want to use this parameter, pass VI_NULL. 

Use this parameter in complex functions to keep track of whether you obtain a lock and therefore need to unlock the session.  Pass the address of a local ViBoolean variable.  In the declaration of the local variable, initialize it to VI_FALSE.  Pass the address of the same local variable to any other calls you make to elvpc3a_LockSession or elvpc3a_UnlockSession in the same function.

The parameter is an input/output parameter.  elvpc3a_LockSession and elvpc3a_UnlockSession each inspect the current value and take the following actions:

- If the value is VI_TRUE, elvpc3a_LockSession does not lock the session again.  If the value is VI_FALSE, elvpc3a_LockSession obtains the lock and sets the value of the parameter to VI_TRUE.

- If the value is VI_FALSE, elvpc3a_UnlockSession does not attempt to unlock the session.  If the value is VI_TRUE, elvpc3a_UnlockSession releases the lock and sets the value of the parameter to VI_FALSE.
 
Thus, you can, call elvpc3a_UnlockSession at the end of your function without worrying about whether you actually have the lock.  

Example:

ViStatus TestFunc (ViSession vi, ViInt32 flags)
{
    ViStatus error = VI_SUCCESS;
    ViBoolean haveLock = VI_FALSE;

    if (flags & BIT_1)
        {
        viCheckErr( elvpc3a_LockSession(vi, &haveLock));
        viCheckErr( TakeAction1(vi));
        if (flags & BIT_2)
            {
            viCheckErr( elvpc3a_UnlockSession(vi, &haveLock));
            viCheckErr( TakeAction2(vi));
            viCheckErr( elvpc3a_LockSession(vi, &haveLock);
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
    elvpc3a_UnlockSession(vi, &haveLock);
    return error;
}   ?[#????  ?    Status                           ?>-   ?  ?    Instrument Handle                ?? H ? ?  ?    Caller Has Lock                    	               	            ?    This function releases a lock that you acquired on an instrument session using elvpc3a_LockSession.  Refer to elvpc3a_LockSession for additional information on session locks.
    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
    ?    This parameter serves as a convenience.  If you do not want to use this parameter, pass VI_NULL. 

Use this parameter in complex functions to keep track of whether you obtain a lock and therefore need to unlock the session. 
Pass the address of a local ViBoolean variable.  In the declaration of the local variable, initialize it to VI_FALSE.  Pass the address of the same local variable to any other calls you make to elvpc3a_LockSession or elvpc3a_UnlockSession in the same function.

The parameter is an input/output parameter.  elvpc3a_LockSession and elvpc3a_UnlockSession each inspect the current value and take the following actions:

- If the value is VI_TRUE, elvpc3a_LockSession does not lock the session again.  If the value is VI_FALSE, elvpc3a_LockSession obtains the lock and sets the value of the parameter to VI_TRUE.

- If the value is VI_FALSE, elvpc3a_UnlockSession does not attempt to unlock the session.  If the value is VI_TRUE, elvpc3a_UnlockSession releases the lock and sets the value of the parameter to VI_FALSE.
 
Thus, you can, call elvpc3a_UnlockSession at the end of your function without worrying about whether you actually have the lock.  

Example:

ViStatus TestFunc (ViSession vi, ViInt32 flags)
{
    ViStatus error = VI_SUCCESS;
    ViBoolean haveLock = VI_FALSE;

    if (flags & BIT_1)
        {
        viCheckErr( elvpc3a_LockSession(vi, &haveLock));
        viCheckErr( TakeAction1(vi));
        if (flags & BIT_2)
            {
            viCheckErr( elvpc3a_UnlockSession(vi, &haveLock));
            viCheckErr( TakeAction2(vi));
            viCheckErr( elvpc3a_LockSession(vi, &haveLock);
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
    elvpc3a_UnlockSession(vi, &haveLock);
    return error;
}   -#????  ?    Status                           -   ?  ?    Instrument Handle                ? H ? ?  ?    Caller Has Lock                    	               	           ?    This function performs the following operations:

- Closes the instrument I/O session.

- Destroys the instrument driver session and all of its attributes.

- Deallocates any memory resources the driver uses.

Notes:

(1) You must unlock the session before calling elvpc3a_close.

(2) After calling elvpc3a_close, you cannot use the instrument driver again until you call elvpc3a_init or elvpc3a_InitWithOptions.

    ?    Returns the status code of this operation.  The status code  either indicates success or describes an error or warning condition.  You examine the status code from each call to an instrument driver function to determine if an error occurred.

To obtain a text description of the status code, call the elvpc3a_error_message function.  To obtain additional information about the error condition, call the elvpc3a_GetError function.  To clear the error information from the driver, call the elvpc3a_ClearError function.
          
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
3FFA2000 to 3FFA3FFF     IviSwtch Warnings
3FFA0000 to 3FFA1FFF     IVI      Warnings
3FFF0000 to 3FFFFFFF     VISA     Warnings
3FFC0000 to 3FFCFFFF     VXIPnP   Driver Warnings
          
BFFA2000 to BFFA3FFF     IviSwtch Errors
BFFA0000 to BFFA1FFF     IVI      Errors
BFFF0000 to BFFFFFFF     VISA     Errors
BFFC0000 to BFFCFFFF     VXIPnP   Driver Errors
     ?    The ViSession handle that you obtain from the elvpc3a_init or elvpc3a_InitWithOptions function.  The handle identifies a particular instrument session.

Default Value:  None
   ?#????  ?    Status                           !?-   ?  ?    Instrument Handle                  	            ????         	^  "?     K.    init                            ????         $T  F?     K.    InitWithOptions                 ????         H?  ^?     K.    SetAttributeViInt32             ????         `+  v     K.    SetAttributeViReal64            ????         w?  ??     K.    SetAttributeViString            ????         ?H  ?>     K.    SetAttributeViBoolean           ????         ??  ??     K.    SetAttributeViSession           ????         ?j  ?C     K.    GetAttributeViInt32             ????         р  ?\     K.    GetAttributeViReal64            ????         ??      K.    GetAttributeViString            ????        ? o     K.    GetAttributeViBoolean           ????        ? '?     K.    GetAttributeViSession           ????        (? :R     K.    CheckAttributeViInt32           ????        ;? My     K.    CheckAttributeViReal64          ????        O `?     K.    CheckAttributeViString          ????        b; s?     K.    CheckAttributeViBoolean         ????        ue ??     K.    CheckAttributeViSession         ????        ?? ?     K.    Connect                         ????        ?	 ??     K.    Disconnect                      ????        ?? ?     K.    DisconnectAll                   ????        ?{ ??     K.    IsDebounced                     ????        ?> ??     K.    WaitForDebounce                 ????        ţ ?L     K.    CanConnect                      ????        ?} ?5     K.    SetPath                         ????        ?? ??     K.    GetPath                         ????        ? ?     K.    reset                           ????        N ?     K.    ResetWithDefaults               ????        k ?     K.    Disable                         ????         !     K.    self_test                       ????        !? ,     K.    revision_query                  ????        - 6?     K.    error_query                     ????        7? l?     K.    error_message                   ????        m? v?     K.    InvalidateAllAttributes         ????        wT ?%     K.    GetChannelName                  ????        ?S ??     K.    GetError                        ????        ?? ??     K.    ClearError                      ????        ?R ?a     K.    GetNextCoercionRecord           ????        ?U ??     K.    GetNextInterchangeWarning       ????        ?? ??     K.    ClearInterchangeWarnings        ????        ?A ??     K.    ResetInterchangeCheck           ????        ?% ?     K.    LockSession                     ????        t ?     K.    UnlockSession                   ????        F "?     K.    close                                 ?                                     DInitialize                           DInitialize With Options             ?Configuration                       tSet/Get/Check Attribute             ?Set Attribute                        DSet Attribute ViInt32                DSet Attribute ViReal64               DSet Attribute ViString               DSet Attribute ViBoolean              DSet Attribute ViSession             YGet Attribute                        DGet Attribute ViInt32                DGet Attribute ViReal64               DGet Attribute ViString               DGet Attribute ViBoolean              DGet Attribute ViSession             ?Check Attribute                      DCheck Attribute ViInt32              DCheck Attribute ViReal64             DCheck Attribute ViString             DCheck Attribute ViBoolean            DCheck Attribute ViSession           }Route                                DConnect Channels                     DDisconnect Channels                  DDisconnect All Channels              DSwitch Is Debounced?                 DWait For Debounce                    DCan Connect Channels?               KPaths                                DSet Path                             DGet Path                            ?Utility                              DReset                                DReset With Defaults                  DDisable                              DSelf-Test                            DRevision Query                       DError-Query                          DError Message                        DInvalidate All Attributes            DGet Channel Name                    ?Error Info                           DGet Error                            DClear Error                         RCoercion Info                        DGet Next Coercion Record            ?Interchangeability Info              DGet Next Interchange Warning         DClear Interchange Warnings           DReset Interchange Check             ?Locking                              DLock Session                         DUnlock Session                       DClose                           