s??        ON   ? ?~  k?   ?   ????                               BKLib2                          BKLib2.1 ActiveX Control module (2.1)                                                                 ?  ??BKLib2Obj_Font *  ?  ??BKLib2Obj_Font  ?  ??CAObjHandle *  ?  ??short *  ?  ??VBOOL *  ?  ??BKLib2Type_OLE_COLOR *  ?   ??char **  ? ? ??long *  ? 	? 	??VARIANT *     ? ??BKLib2Type_OLE_COLOR     (? (??_DBKTreeEventsRegOnDragDrop_CallbackType     (? (??_DBKTreeEventsRegOnDragOver_CallbackType     4? 4??_DBKTreeEventsRegOnPrePopupMenuCallback_CallbackType     &? &??_DBKTreeEventsRegOnModify_CallbackType     -? -??_DBKTreeEventsRegOnMouseOverCell_CallbackType     0? 0??_DBKTreeEventsRegOnPopupMenuCommand_CallbackType     (? (??_DBKTreeEventsRegOnCollapse_CallbackType     &? &??_DBKTreeEventsRegOnExpand_CallbackType     )? )??_DBKTreeEventsRegOnNodeClick_CallbackType     ? ??const char *     ? ??LCID     	? 	??ERRORINFO  ?  ? ??HRESULT  ? ? ??SAFEARRAY *  ? 	? 	??LPUNKNOWN     ? ??VARIANT  ? ? ??VBOOL  ?  ? ??SCODE  ? ? ??CAObjHandle  ? ? ??DATE     ? ??CURRENCY   '    BKLib2.1 ActiveX Control module (2.1)    ^    This class contains functions you use to register callbacks that the ActiveX server objects call when events occur.  The callback registration functions are grouped into event classes.  The ActiveX server specifies which event classes are supported by each server object.  These relationships are documented in the event class' function panel help.    /    This class contains functions you use to register callbacks that ActiveX server objects call when events occur.  This class contains all of the callback registration functions for the _DBKTreeEvents event class.  The _DBKTreeEvents event class is supported by the following server object(s): _DBKTree.     -    Dispatch interface for BKTree Control (2.1)     -    Dispatch interface for BKGrid Control (2.1)    -    Call this function to register a callback for the NodeClick event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.          A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    c    Pass the function that the CVI ActiveX library calls when the server fires a(n) NodeClick event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.       ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    ?
????  ?    Status                             -   ?  ?    Server Object                      - ? ?  ?    Callback Function                 m -?    ?    Callback Data                     d ?      ?    Enable Callbacks                  ? ? ?     ?    Callback Id                        	                   NULL    1            NULL   *    Call this function to register a callback for the Expand event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.          A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    `    Pass the function that the CVI ActiveX library calls when the server fires a(n) Expand event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.       ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    ?
????  ?    Status                             -   ?  ?    Server Object                      - ? ?  ?    Callback Function                 "k -?    ?    Callback Data                     #b ?      ?    Enable Callbacks                  '? ? ?     ?    Callback Id                        	                   NULL    1            NULL   ,    Call this function to register a callback for the Collapse event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.          A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    b    Pass the function that the CVI ActiveX library calls when the server fires a(n) Collapse event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.       ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    -?
????  ?    Status                            / -   ?  ?    Server Object                     0 - ? ?  ?    Callback Function                 3m -?    ?    Callback Data                     4d ?      ?    Enable Callbacks                  8? ? ?     ?    Callback Id                        	                   NULL    1            NULL   ?    Call this function to register a callback for the PopupMenuCommand event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol,
                              long  menuItem);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.          A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    ?    Pass the function that the CVI ActiveX library calls when the server fires a(n) PopupMenuCommand event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol,
                              long  menuItem);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.       ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    ?c
????  ?    Status                            @q -   ?  ?    Server Object                     Ag - ? ?  ?    Callback Function                 E3 -?    ?    Callback Data                     F* ?      ?    Enable Callbacks                  J_ ? ?     ?    Callback Id                        	                   NULL    1            NULL   ]    Call this function to register a callback for the MouseOverCell event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.          A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    ?    Pass the function that the CVI ActiveX library calls when the server fires a(n) MouseOverCell event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.       ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    P?
????  ?    Status                            R -   ?  ?    Server Object                     R? - ? ?  ?    Callback Function                 V? -?    ?    Callback Data                     W? ?      ?    Enable Callbacks                  [? ? ?     ?    Callback Id                        	                   NULL    1            NULL   ?    Call this function to register a callback for the Modify event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol,
                              long  modifyType);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.          A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    ?    Pass the function that the CVI ActiveX library calls when the server fires a(n) Modify event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol,
                              long  modifyType);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.       ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    b?
????  ?    Status                            c? -   ?  ?    Server Object                     d? - ? ?  ?    Callback Function                 hM -?    ?    Callback Data                     iD ?      ?    Enable Callbacks                  my ? ?     ?    Callback Id                        	                   NULL    1            NULL   	?    Call this function to register a callback for the PrePopupMenuCallback event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol,
                              char **pMenuItem);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.  

This event callback specifies one or more dynamically allocated input/output or output only parameters.  Before you replace the value of a dynamically allocated input/output parameter, you must first use the appropriate function to free the value passed in.  Before you return from the callback, every dynamically allocated input/output or output only parameter must contain a valid value.  You can either allocate a new value for each such parameter, or you can leave the current value in the parameter.  For this reason, you should temporarily store all input/output and output only parameters that you allocate into local variables.  At the end of your callback function, after you have determined that you will not return an error from the callback, deallocate the current value of every input/output parameter and assign the new values from your temporary local variables to each input/output and output only parameter.  If you return an error, free the values in the local variables and do not change or deallocate the current values of the parameters.

The parameter pMenuItem is a dynamically allocated input/output parameter.  If you want to replace the value of this parameter, call CA_FreeMemory to free the value of pMenuItem parameter and call CA_AllocMemory to allocate a new value for parameter pMenuItem.        A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    ?    Pass the function that the CVI ActiveX library calls when the server fires a(n) PrePopupMenuCallback event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              char *keyCol,
                              char **pMenuItem);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.  

This event callback specifies one or more dynamically allocated input/output or output only parameters.  Before you replace the value of a dynamically allocated input/output parameter, you must first use the appropriate function to free the value passed in.  Before you return from the callback, every dynamically allocated input/output or output only parameter must contain a valid value.  You can either allocate a new value for each such parameter, or you can leave the current value in the parameter.  For this reason, you should temporarily store all input/output and output only parameters that you allocate into local variables.  At the end of your callback function, after you have determined that you will not return an error from the callback, deallocate the current value of every input/output parameter and assign the new values from your temporary local variables to each input/output and output only parameter.  If you return an error, free the values in the local variables and do not change or deallocate the current values of the parameters.

The parameter pMenuItem is a dynamically allocated input/output parameter.  If you want to replace the value of this parameter, call CA_FreeMemory to free the value of pMenuItem parameter and call CA_AllocMemory to allocate a new value for parameter pMenuItem.     ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    ys
????  ?    Status                            z? -   ?  ?    Server Object                     {w - ? ?  ?    Callback Function                 ?s -?    ?    Callback Data                     ?j ?      ?    Enable Callbacks                  ?? ? ?     ?    Callback Id                        	                   NULL    1            NULL   ?    Call this function to register a callback for the DragOver event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              short  nPosition,
                              short  state);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.          A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    ?    Pass the function that the CVI ActiveX library calls when the server fires a(n) DragOver event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              short  nPosition,
                              short  state);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.       ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    ?c
????  ?    Status                            ?q -   ?  ?    Server Object                     ?g - ? ?  ?    Callback Function                 ?- -?    ?    Callback Data                     ?$ ?      ?    Enable Callbacks                  ?Y ? ?     ?    Callback Id                        	                   NULL    1            NULL   \    Call this function to register a callback for the DragDrop event of the _DBKTreeEvents event class.  When you register the callback, you must specify the CAObjHandle of the server object from which you want to receive events.  The following server objects generate _DBKTreeEvents events: _DBKTree.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              short  nPosition);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.          A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    Pass the CAObjHandle of the server object from which you want to receive events.  You must pass a CAObjHandle to a server object that supports _DBKTreeEvents events.  The following server objects support _DBKTreeEvents events: _DBKTree.    ?    Pass the function that the CVI ActiveX library calls when the server fires a(n) DragDrop event.

The callback function that you specify must have the following prototype:

HRESULT CVICALLBACK Callback (CAObjHandle caServerObjHandle,
                              void *caCallbackData,
                              char *keyNode,
                              short  nPosition);

Upon entry to the callback, the caServerObjHandle parameter identifies the object that is firing the event.  You can obtain the panel and control ID for the ActiveX control or document object firing the event by calling GetActiveXCtrlFromObjHandle.  The caCallbackData parameter contains the value you passed in the Callback Data parameter of this function.  The other parameters are event-specific and are specified and (optionally) documented by the server.  The return value is currently unused.  You should always return S_OK.       ?    Pass a value that you want the CVI ActiveX library to pass to your callback as the caCallbackData parameter.  Do not pass the address of the of a local variable or any other variable that might not be valid when the callback is executed.    -    This parameter specifies whether this registration function enables the registered callbacks for the server.  Pass 1 to enable all of the registered callbacks in the _DBKTreeEvents class associated with the server object passed in the Server Object parameter of this function.  Pass 0 to specify that this call to the registration function will not enable the callbacks.  Once the callbacks in the _DBKTreeEvents class have been enabled for a particular server object, the value of this parameter is ignored for subsequent callback registration functions in the _DBKTreeEvents class.

Typically, you pass 1 to enable callbacks immediately.  Pass 0 when you have a set of callbacks that must be enabled simultaneously in order for you to properly respond to the server events.  In this case, you must explicitly advise the server when you are ready to begin receiving events.  You can advise the server either by passing 1 for this parameter when you register the final callback, or by calling CA_EnableEventsForServerObject when you are ready to enable the callbacks.     ?    This parameter returns a unique identifier for the callback.  Pass this identifier to CA_UnregisterEventCallback to unregister the callback.  Pass NULL if you do not want the Callback Id.    ??
????  ?    Status                            ?? -   ?  ?    Server Object                     ?? - ? ?  ?    Callback Function                 ?? -?    ?    Callback Data                     ?? ?      ?    Enable Callbacks                  ?? ? ?     ?    Callback Id                        	                   NULL    1            NULL   [    Use this function to create a new BKTree ActiveX control or document object.

If the server application is already running, this function may or may not start another copy of the application.  This is determined by the server application.

You must call CA_InitActiveXThreadStyleForCurrentThread with COINIT_APARTMENTTHREADED if you register any ActiveX event callbacks and want the callbacks to be called from the same thread in which they were registered.  If you do not call CA_InitActiveXThreadStyleForCurrentThread with COINIT_APARTMENTTHREADED your callbacks will be called from a system thread.    ?    A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

A value of E_CVIAUTO_CVI_UI_ERROR indicates a User Interface Library error, in which case the specific UIL error code is returned separately as an output parameter.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    The specifier for a particular panel that is currently in memory.

This handle will have been returned by the LoadPanel(), NewPanel(), or DuplicatePanel() function.     V    The label of the new ActiveX control or document object.

Pass "" or 0 for no label.    ;    The vertical coordinate at which the upper left corner of the ActiveX control or document object (not including labels) is placed.

The coordinate must be an integer value from -32768 to 32767.

The origin (0,0) is at the upper-left corner of the panel (directly below the title bar) before the panel is scrolled.    =    The horizontal coordinate at which the upper left corner of the ActiveX control or document object (not including labels) is placed.

The coordinate must be an integer value from -32768 to 32767.

The origin (0,0) is at the upper-left corner of the panel (directly below the title bar) before the panel is scrolled.     ?    The ID used to specify this ActiveX control or document object in subsequent function calls.  Negative values indicate that an error occurred.  See cvi\include\userint.h for a list of possible error codes.  Zero is not a valid ID.     ?    The User Interface Library error code if this function causes a UIL error.  In the event of a UIL error this function will return a value of E_CVIAUTO_CVI_UI_ERROR.    ?R
????  ?    Status                            ? -       ?    Panel                             ?? - ? ?  ?    Label                             ? -?     ?    Top                               ?U ?      ?    Left                              ?? ? ?     ?    Control ID                        ?? ??     ?    UIL Error                          	               ""            	            	           ?    Use this function to create a BKTree document object from a file.

If the server application is already running, this function may or may not start another copy of the application.  The server application determines whether the function starts another copy of the application.

You must call CA_InitActiveXThreadStyleForCurrentThread with COINIT_APARTMENTTHREADED if you register any ActiveX event callbacks and want the callbacks to be called from the same thread in which they were registered.  If you do not call CA_InitActiveXThreadStyleForCurrentThread with COINIT_APARTMENTTHREADED your callbacks will be called from a system thread.    ?    A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

A value of E_CVIAUTO_CVI_UI_ERROR indicates a User Interface Library error, in which case the specific UIL error code is returned separately as an output parameter.

You can use CA_GetAutomationErrorString to get the description of an error code.     <    A file containing the data for an ActiveX document object.     ?    The specifier for a particular panel that is currently in memory.

This handle will have been returned by the LoadPanel(), NewPanel(), or DuplicatePanel() function.     V    The label of the new ActiveX control or document object.

Pass "" or 0 for no label.    ;    The vertical coordinate at which the upper left corner of the ActiveX control or document object (not including labels) is placed.

The coordinate must be an integer value from -32768 to 32767.

The origin (0,0) is at the upper-left corner of the panel (directly below the title bar) before the panel is scrolled.    =    The horizontal coordinate at which the upper left corner of the ActiveX control or document object (not including labels) is placed.

The coordinate must be an integer value from -32768 to 32767.

The origin (0,0) is at the upper-left corner of the panel (directly below the title bar) before the panel is scrolled.     ?    The ID used to specify this ActiveX control or document object in subsequent function calls.  Negative values indicate that an error occurred.  See cvi\include\userint.h for a list of possible error codes.  Zero is not a valid ID.     ?    The User Interface Library error code if this function causes a UIL error.  In the event of a UIL error this function will return a value of E_CVIAUTO_CVI_UI_ERROR.    ?j
????  ?    Status                            ? -   ?  ?    File Name                         ?b - ?     ?    Panel                             ? -? ?  ?    Label                             ?n ?      ?    Top                               ?? ? ?     ?    Left                              ?? ??     ?    Control ID                        ?? ?      ?    UIL Error                          	                   ""            	            	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.    ?v
????  ?    Status                            Ǖ -   ?  ?    Object Handle                     ?? - ? ?  ?    Error Info                        ? -? ?  ?    Key Col                           ?i ?  ?  ?    Name                              ?? ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.    ??
????  ?    Status                            ?? -   ?  ?    Object Handle                     ?@ - ? ?  ?    Error Info                        ?l -? ?  ?    Key Col                           ?? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.    ?&
????  ?    Status                            ?E -   ?  ?    Object Handle                     ه - ? ?  ?    Error Info                        ۳ -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.    ?M
????  ?    Status                            ?l -   ?  ?    Object Handle                     ?? - ? ?  ?    Error Info                        ?? -? ?  ?    Key Col                           ?@ ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.    ?
????  ?    Status                            ?4 -   ?  ?    Object Handle                     ?v - ? ?  ?    Error Info                        ?? -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.    ?<
????  ?    Status                            ?[ -   ?  ?    Object Handle                     ?? - ? ?  ?    Error Info                        ?? -? ?  ?    Key Col                           ?/ ?  ?  ?    Name                              ?? ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.    ??
????  ?    Status                            ?? -   ?  ?    Object Handle                     ? - ? ?  ?    Error Info                        ?2 -? ?  ?    Key Parent                        ?? ?  ?  ?    Key Node                          ?? ? ? ?  ?    Text                              ?d ??    ?    Return Value                       	                       NULL    ""    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.    ??
????  ?    Status                            ?? -   ?  ?    Object Handle                     - ? ?  ?    Error Info                       < -? ?  ?    Key Node                         ? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   w
????  ?    Status                           ? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                        -? ?  ?    Key Node                         j ?  ?  ?    Key Col                          ? ? ? ?  ?    Text                             6 ??    ?    Return Value                       	                       NULL    ""    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   ?
????  ?    Status                           ? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                        -? ?  ?    Key Node                         t ?  ?  ?    Key Col                          ? ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   i
????  ?    Status                           ? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ? -? ?  ?    Key Node                         \ ?  ?  ?    Key Col                          ? ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.    Q
????  ?    Status                           "p -   ?  ?    Object Handle                    #? - ? ?  ?    Error Info                       %? -? ?  ?    Key Col                          &D ?     ?    N Width                          &? ? ?    ?    Return Value                       	                       NULL    ""        	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   (?
????  ?    Status                           *? -   ?  ?    Object Handle                    , - ? ?  ?    Error Info                       .E -? ?  ?    Key Node                         .? ?  ?  ?    Key Col                          / ? ?    ?    N Image                            	                       NULL    ""    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   0?
????  ?    Status                           3 -   ?  ?    Object Handle                    4F - ? ?  ?    Error Info                       6r -?    ?    B Redraw                         6? ?     ?    Return Value                       	                       NULL        	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   8?
????  ?    Status                           :? -   ?  ?    Object Handle                    < - ? ?  ?    Error Info                       >8 -? ?  ?    Key Node                         >? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   @s
????  ?    Status                           B? -   ?  ?    Object Handle                    C? - ? ?  ?    Error Info                       F  -? ?  ?    Key Node                         Ff ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   H;
????  ?    Status                           JZ -   ?  ?    Object Handle                    K? - ? ?  ?    Error Info                       M? -?    ?    N Level                          N. ?     ?    Return Value                       	                       NULL        	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   P
????  ?    Status                           R  -   ?  ?    Object Handle                    Sb - ? ?  ?    Error Info                       U? -? ?  ?    Key Node                         U? ?  ?  ?    Key Col                          VZ ? ?    ?    Node Format                        	                       NULL    ""    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   X.
????  ?    Status                           ZM -   ?  ?    Object Handle                    [? - ? ?  ?    Error Info                       ]? -? ?  ?    Key Node                         ^! ?  ?  ?    Key Col                          ^? ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   `?
????  ?    Status                           b? -   ?  ?    Object Handle                    c? - ? ?  ?    Error Info                       f$ -? ?  ?    Key Node                         f? ?     ?    Node Style                         	                       NULL    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   h#
????  ?    Status                           jB -   ?  ?    Object Handle                    k? - ? ?  ?    Error Info                       m? -? ?  ?    Key Node                         n ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   o?
????  ?    Status                           r
 -   ?  ?    Object Handle                    sL - ? ?  ?    Error Info                       ux -? ?  ?    Key Node                         u? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   x2
????  ?    Status                           zQ -   ?  ?    Object Handle                    {? - ? ?  ?    Error Info                       }? -?    ?    N Bar                            ~% ?     ?    N New Value                        	                       NULL               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?I -?    ?    N Bar                            ?? ?     ?    Return Value                       	                       NULL        	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ? -? ?  ?    Key Node                         ?u ?  ?  ?    Key Col                          ?? ? ?    ?    Node Style                         	                       NULL    ""    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?< -? ?  ?    Key Node                         ?? ?  ?  ?    Key Col                          ? ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ?7 -   ?  ?    Object Handle                    ?y - ? ?  ?    Error Info                       ?? -? ?  ?    Key Node                         ? ?  ?  ?    Key Col                          ?q ? ? ?  ?    N New Value                        	                       NULL    ""    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?E
????  ?    Status                           ?d -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?  ?    Key Node                         ?8 ?  ?  ?    Key Col                          ?? ? ? ?  ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?; -? ?  ?    Key Node                         ?? ?  ?  ?    Key Col                          ? ? ? ?  ?    N New Value                        	                       NULL    ""    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?< - ? ?  ?    Error Info                       ?h -? ?  ?    Key Node                         ?? ?  ?  ?    Key Col                          ?4 ? ? ?  ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?D
????  ?    Status                           ?c -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?  ?    Key Node                         ?7 ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ?+ -   ?  ?    Object Handle                    ?m - ? ?  ?    Error Info                       ƙ -? ?  ?    Key Node                         ?? ?  ?  ?    Key Target Node                  ?e ? ?    ?    N Position                       ?? ??    ?    Return Value                       	                       NULL    ""    ""        	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ?3 -   ?  ?    Object Handle                    ?u - ? ?  ?    Error Info                       ϡ -? ?  ?    Menu Items                       ? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?= - ? ?  ?    Error Info                       ?i -? ?  ?    Key Node                         ?? ?  ?  ?    Key Col                          ?5 ? ?    ?    B Editable                         	                       NULL    ""    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?	
????  ?    Status                           ?( -   ?  ?    Object Handle                    ?j - ? ?  ?    Error Info                       ߖ -? ?  ?    Key Node                         ?? ?  ?  ?    Key Col                          ?b ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?r
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?  ?    V Array                          ?e ?     ?    Return Value                       	                       NULL        	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.    9    The Variant returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

The Variant Related Functions class in the CVI ActiveX Library contains functions to help you query the type of value stored in a Variant and to retrieve values from a Variant.

   ?8
????  ?    Status                           ?W -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ?% -   ?  ?    Object Handle                    ?g - ? ?  ?    Error Info                       ?? -? ?  ?    Str String                       ?? ?  ?  ?    Ch Separator                     ?_ ? ? ?  ?    Ch Line                          ?? ??    ?    Return Value                       	                       NULL    ""    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   ?
????  ?    Status                           ?/ -   ?  ?    Object Handle                    ?q - ? ?  ?    Error Info                        ? -? ?  ?    Ch Separator                      ?  ?  ?    Ch Line                          i ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   ?
????  ?    Status                            -   ?  ?    Object Handle                    Y - ? ?  ?    Error Info                       	? -? ?  ?    Ch Separator                     	? ?  ?  ?    Ch Line                          
Q ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   ?
????  ?    Status                           ? -   ?  ?    Object Handle                    A - ? ?  ?    Error Info                       m -? ?  ?    Key Col                          ? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   '
????  ?    Status                           F -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ? -? ?  ?    Key Node                          ?     ?    Position                         ? ? ?    ?    Return Value                       	                       NULL    ""        	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   
????  ?    Status                            , -   ?  ?    Object Handle                    !n - ? ?  ?    Error Info                       #? -? ?  ?    Key Node                         $  ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   %?
????  ?    Status                           '? -   ?  ?    Object Handle                    )6 - ? ?  ?    Error Info                       +b -? ?  ?    Key Node                         +? ?  ?  ?    Key Col                          ,. ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   .>
????  ?    Status                           0] -   ?  ?    Object Handle                    1? - ? ?  ?    Error Info                       3? -? ?  ?    Key Node                         41 ?  ?  ?    Key Col                          4? ? ? ?  ?    Text                             4? ??    ?    Return Value                       	                       NULL    ""    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   7H
????  ?    Status                           9g -   ?  ?    Object Handle                    :? - ? ?  ?    Error Info                       <? -? ?  ?    Key Node                           	                       NULL    ""       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   >5
????  ?    Status                           @T -   ?  ?    Object Handle                    A? - ? ?  ?    Error Info                       C? -? ?  ?    Key Node                         D( ?  ?  ?    Key New                          D? ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   F?
????  ?    Status                           H? -   ?  ?    Object Handle                    I? - ? ?  ?    Error Info                       L+ -? ?  ?    Key Node                         L? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   Nf
????  ?    Status                           P? -   ?  ?    Object Handle                    Q? - ? ?  ?    Error Info                       S? -? ?  ?    Key Col                          TY ?     ?    Head Style                         	                       NULL    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   U?
????  ?    Status                           X -   ?  ?    Object Handle                    YS - ? ?  ?    Error Info                       [ -? ?  ?    Key Col                          [? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   ]?
????  ?    Status                           _? -   ?  ?    Object Handle                    a - ? ?  ?    Error Info                       cG -? ?  ?    Key Col                          c? ?     ?    Head Format                        	                       NULL    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   eF
????  ?    Status                           ge -   ?  ?    Object Handle                    h? - ? ?  ?    Error Info                       j? -? ?  ?    Key Col                          k9 ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   m
????  ?    Status                           o- -   ?  ?    Object Handle                    po - ? ?  ?    Error Info                       r? -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   t?
????  ?    Status                           v? -   ?  ?    Object Handle                    x - ? ?  ?    Error Info                       zA -?    ?    N New Style                      z? ?     ?    N New Style Ex                     	                       NULL               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   |>
????  ?    Status                           ~] -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?? -? ?  ?    P Style                          ?1 ?  ?  ?    P Style Ex                         	                       NULL               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?) - ? ?  ?    Error Info                       ?U -? ?  ?    Str XML                          ?? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ? -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?6
????  ?    Status                           ?U -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?  ?    Key Node                         ?) ?     ?    L Data                             	                       NULL    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?# - ? ?  ?    Error Info                       ?O -? ?  ?    Key Node                         ?? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ? -? ?  ?    Key Node                         ?} ?  ?  ?    Key Col                          ?? ? ?    ?    L Data                             	                       NULL    ""    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?D -? ?  ?    Key Node                         ?? ?  ?  ?    Key Col                          ? ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ? 
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?  ?    Key Col                          ? ?     ?    Return Value                       	                       NULL    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ? -   ?  ?    Object Handle                    ?I - ? ?  ?    Error Info                       ?u -? ?  ?    Key Node                         ?? ?  ?  ?    Key Col                          ?A ? ?    ?    B Enable                           	                       NULL    ""    ""           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.     ^    Documentation for this function, if provided by the server, is located in the function help.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ?4 -   ?  ?    Object Handle                    ?v - ? ?  ?    Error Info                       Ȣ -? ?  ?    Key Node                         ? ?  ?  ?    Key Col                          ?n ? ?    ?    Return Value                       	                       NULL    ""    ""    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.   ?~
????  ?    Status                           ͝ -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                         	                       NULL       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?+ - ? ?  ?    Error Info                         	                       NULL       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.        The string returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

When it is no longer needed, you must free the string returned in this parameter by calling the CVI ActiveX Library function CA_FreeMemory.   ?
????  ?    Status                           ?5 -   ?  ?    Object Handle                    ?w - ? ?  ?    Error Info                       ݣ -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ߼
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?I -? ?  ?    New Value                          	                       NULL    ""       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?
 - ? ?  ?    Error Info                       ?6 -?    ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?! -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?H -? ?  ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       3 -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                        -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   	?
????  ?    Status                           ? -   ?  ?    Object Handle                     - ? ?  ?    Error Info                       E -? ?  ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ? -   ?  ?    Object Handle                     - ? ?  ?    Error Info                       0 -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                        -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                            ? -   ?  ?    Object Handle                    " - ? ?  ?    Error Info                       $B -? ?  ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   %?
????  ?    Status                           '? -   ?  ?    Object Handle                    ) - ? ?  ?    Error Info                       +- -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ,?
????  ?    Status                           .? -   ?  ?    Object Handle                    /? - ? ?  ?    Error Info                       2 -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   3?
????  ?    Status                           5? -   ?  ?    Object Handle                    7 - ? ?  ?    Error Info                       9? -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   :?
????  ?    Status                           <? -   ?  ?    Object Handle                    > - ? ?  ?    Error Info                       @C -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   A?
????  ?    Status                           C? -   ?  ?    Object Handle                    E - ? ?  ?    Error Info                       G. -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   H?
????  ?    Status                           J? -   ?  ?    Object Handle                    L) - ? ?  ?    Error Info                       NU -?    ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   O?
????  ?    Status                           Q? -   ?  ?    Object Handle                    S - ? ?  ?    Error Info                       U@ -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   V?
????  ?    Status                           X? -   ?  ?    Object Handle                    Y? - ? ?  ?    Error Info                       \+ -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ]?
????  ?    Status                           _? -   ?  ?    Object Handle                    a& - ? ?  ?    Error Info                       cR -? ?  ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   d?
????  ?    Status                           f? -   ?  ?    Object Handle                    h - ? ?  ?    Error Info                       j= -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.    ?    The handle to an ActiveX object returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

Use this handle to call methods and get/set properties of the appropriate ActiveX object.

When it is no longer needed, you must discard this handle using CA_DiscardObjHandle.

See function help for more information.

NOTE: CAObjHandles created by this function inherit multithreading support and locale values from the Object Handle parameter.

To use different values for multithreading support and locale, you can call CA_SetSupportForMultithreading and CA_SetLocale to specify the desired values.   k?
????  ?    Status                           m? -   ?  ?    Object Handle                    n? - ? ?  ?    Error Info                       q( -? ?  ?    Return Value                       	                       NULL    	              A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   t?
????  ?    Status                           v? -   ?  ?    Object Handle                    x  - ? ?  ?    Error Info                       zL -? ?  ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   {?
????  ?    Status                           }? -   ?  ?    Object Handle                     - ? ?  ?    Error Info                       ?7 -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.    ?    The handle to an ActiveX object returned by the function.

Documentation for this function, if provided by the server, is located in the function help.

Use this handle to call methods and get/set properties of the appropriate ActiveX object.

When it is no longer needed, you must discard this handle using CA_DiscardObjHandle.

See function help for more information.

NOTE: CAObjHandles created by this function inherit multithreading support and locale values from the Object Handle parameter.

To use different values for multithreading support and locale, you can call CA_SetSupportForMultithreading and CA_SetLocale to specify the desired values.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?" -?   ?    Return Value                       	                       NULL    	              A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?F -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?1 -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ? -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?C -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?G -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?2 -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?- - ? ?  ?    Error Info                       ?Y -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?1 - ? ?  ?    Error Info                       ?] -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   û
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?H -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ? -   ?  ?    Object Handle                    ?C - ? ?  ?    Error Info                       ?o -? ?  ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?. - ? ?  ?    Error Info                       ?Z -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ظ
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?E -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?@ - ? ?  ?    Error Info                       ?l -?    ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?+ - ? ?  ?    Error Info                       ?W -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ? - ? ?  ?    Error Info                       ?B -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?= - ? ?  ?    Error Info                       ?i -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ??
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?A - ? ?  ?    Error Info                       m -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ? -   ?  ?    Object Handle                    , - ? ?  ?    Error Info                       X -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   	?
????  ?    Status                            -   ?  ?    Object Handle                    S - ? ?  ?    Error Info                        -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                            -   ?  ?    Object Handle                    W - ? ?  ?    Error Info                       ? -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                             -   ?  ?    Object Handle                    B - ? ?  ?    Error Info                       n -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   
????  ?    Status                           !' -   ?  ?    Object Handle                    "i - ? ?  ?    Error Info                       $? -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   &
????  ?    Status                           (+ -   ?  ?    Object Handle                    )m - ? ?  ?    Error Info                       +? -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ,?
????  ?    Status                           / -   ?  ?    Object Handle                    0X - ? ?  ?    Error Info                       2? -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   4
????  ?    Status                           6= -   ?  ?    Object Handle                    7 - ? ?  ?    Error Info                       9? -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ;"
????  ?    Status                           =A -   ?  ?    Object Handle                    >? - ? ?  ?    Error Info                       @? -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   B
????  ?    Status                           D, -   ?  ?    Object Handle                    En - ? ?  ?    Error Info                       G? -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   I4
????  ?    Status                           KS -   ?  ?    Object Handle                    L? - ? ?  ?    Error Info                       N? -?    ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   P
????  ?    Status                           R> -   ?  ?    Object Handle                    S? - ? ?  ?    Error Info                       U? -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   W

????  ?    Status                           Y) -   ?  ?    Object Handle                    Zk - ? ?  ?    Error Info                       \? -?    ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ^1
????  ?    Status                           `P -   ?  ?    Object Handle                    a? - ? ?  ?    Error Info                       c? -?    ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   e
????  ?    Status                           g; -   ?  ?    Object Handle                    h} - ? ?  ?    Error Info                       j? -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   l
????  ?    Status                           n& -   ?  ?    Object Handle                    oh - ? ?  ?    Error Info                       q? -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   s.
????  ?    Status                           uM -   ?  ?    Object Handle                    v? - ? ?  ?    Error Info                       x? -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   z2
????  ?    Status                           |Q -   ?  ?    Object Handle                    }? - ? ?  ?    Error Info                       ? -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?
????  ?    Status                           ?< -   ?  ?    Object Handle                    ?~ - ? ?  ?    Error Info                       ?? -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?D
????  ?    Status                           ?c -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?H
????  ?    Status                           ?g -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?3
????  ?    Status                           ?R -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?Z
????  ?    Status                           ?y -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?^
????  ?    Status                           ?} -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -?   ?    New Value                          	                       NULL           A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ?    The value returned by the ActiveX server function.

Documentation for this function, if provided by the server, is located in the function help.   ?I
????  ?    Status                           ?h -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?  ?    Return Value                       	                       NULL    	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?p
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ?? -? ?       New Value                          	                       NULL    VTRUE VTRUE VFALSE VFALSE       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.     ^    Documentation for this function, if provided by the server, is located in the function help.   ?t
????  ?    Status                           ?? -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                       ? -?   ?    New Value                          	                       NULL       [    Use this function to create a new BKGrid ActiveX control or document object.

If the server application is already running, this function may or may not start another copy of the application.  This is determined by the server application.

You must call CA_InitActiveXThreadStyleForCurrentThread with COINIT_APARTMENTTHREADED if you register any ActiveX event callbacks and want the callbacks to be called from the same thread in which they were registered.  If you do not call CA_InitActiveXThreadStyleForCurrentThread with COINIT_APARTMENTTHREADED your callbacks will be called from a system thread.    ?    A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

A value of E_CVIAUTO_CVI_UI_ERROR indicates a User Interface Library error, in which case the specific UIL error code is returned separately as an output parameter.

You can use CA_GetAutomationErrorString to get the description of an error code.     ?    The specifier for a particular panel that is currently in memory.

This handle will have been returned by the LoadPanel(), NewPanel(), or DuplicatePanel() function.     V    The label of the new ActiveX control or document object.

Pass "" or 0 for no label.    ;    The vertical coordinate at which the upper left corner of the ActiveX control or document object (not including labels) is placed.

The coordinate must be an integer value from -32768 to 32767.

The origin (0,0) is at the upper-left corner of the panel (directly below the title bar) before the panel is scrolled.    =    The horizontal coordinate at which the upper left corner of the ActiveX control or document object (not including labels) is placed.

The coordinate must be an integer value from -32768 to 32767.

The origin (0,0) is at the upper-left corner of the panel (directly below the title bar) before the panel is scrolled.     ?    The ID used to specify this ActiveX control or document object in subsequent function calls.  Negative values indicate that an error occurred.  See cvi\include\userint.h for a list of possible error codes.  Zero is not a valid ID.     ?    The User Interface Library error code if this function causes a UIL error.  In the event of a UIL error this function will return a value of E_CVIAUTO_CVI_UI_ERROR.   ??
????  ?    Status                           ?v -       ?    Panel                            ?$ - ? ?  ?    Label                            ł -?     ?    Top                              ?? ?      ?    Left                             ?
 ? ?     ?    Control ID                       ?? ??     ?    UIL Error                          	               ""            	            	           ?    Use this function to create a BKGrid document object from a file.

If the server application is already running, this function may or may not start another copy of the application.  The server application determines whether the function starts another copy of the application.

You must call CA_InitActiveXThreadStyleForCurrentThread with COINIT_APARTMENTTHREADED if you register any ActiveX event callbacks and want the callbacks to be called from the same thread in which they were registered.  If you do not call CA_InitActiveXThreadStyleForCurrentThread with COINIT_APARTMENTTHREADED your callbacks will be called from a system thread.    ?    A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

A value of E_CVIAUTO_CVI_UI_ERROR indicates a User Interface Library error, in which case the specific UIL error code is returned separately as an output parameter.

You can use CA_GetAutomationErrorString to get the description of an error code.     <    A file containing the data for an ActiveX document object.     ?    The specifier for a particular panel that is currently in memory.

This handle will have been returned by the LoadPanel(), NewPanel(), or DuplicatePanel() function.     V    The label of the new ActiveX control or document object.

Pass "" or 0 for no label.    ;    The vertical coordinate at which the upper left corner of the ActiveX control or document object (not including labels) is placed.

The coordinate must be an integer value from -32768 to 32767.

The origin (0,0) is at the upper-left corner of the panel (directly below the title bar) before the panel is scrolled.    =    The horizontal coordinate at which the upper left corner of the ActiveX control or document object (not including labels) is placed.

The coordinate must be an integer value from -32768 to 32767.

The origin (0,0) is at the upper-left corner of the panel (directly below the title bar) before the panel is scrolled.     ?    The ID used to specify this ActiveX control or document object in subsequent function calls.  Negative values indicate that an error occurred.  See cvi\include\userint.h for a list of possible error codes.  Zero is not a valid ID.     ?    The User Interface Library error code if this function causes a UIL error.  In the event of a UIL error this function will return a value of E_CVIAUTO_CVI_UI_ERROR.   ??
????  ?    Status                           ώ -   ?  ?    File Name                        ?? - ?     ?    Panel                            Ѐ -? ?  ?    Label                            ?? ?      ?    Top                              ?! ? ?     ?    Left                             ?f ??     ?    Control ID                       ?V ?      ?    UIL Error                          	                   ""            	            	               A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.   ??
????  ?    Status                           ? -   ?  ?    Object Handle                    ?G - ? ?  ?    Error Info                         	                       NULL       A value indicating whether an error occurred.  A negative error code indicates function failure.

Error codes are defined in cvi\include\cviauto.h and cvi\sdk\include\winerror.h.

You can use CA_GetAutomationErrorString to get the description of an error code or CA_DisplayErrorInfo to display the description of the error code.

If the error code is DISP_E_EXCEPTION  (0x80020009 or -2147352567), then the Error Info parameter contains additional error information.  You can use CA_DisplayErrorInfo to display the error information.    :    An ActiveX object handle obtained from GetObjHandleFromActiveXCtrl or an ActiveX method or property.

All of the methods that can be applied to a particular object are grouped under a single class in the function tree.  The name of the class corresponds to the type of the object to which this handle must refer.    $    When an ActiveX server function fails with the error code DISP_E_EXCEPTION, descriptive information about the error code is stored in this parameter.  The descriptive information may include the error's code, source, and description.  It may also include a help file and help file context.

When an ActiveX server function fails with the error codes DISP_E_PARAMNOTFOUND, DISP_E_TYPEMISMATCH, or E_INVALIDARG, the parameter position of the invalid argument may be stored in the errorParamPos member of this parameter.

This parameter may be NULL.   ?2
????  ?    Status                           ?Q -   ?  ?    Object Handle                    ?? - ? ?  ?    Error Info                         	                       NULL ????         ?  ^             K.        _DBKTreeEventsRegOnNodeClick                                                                                                            ????         ?  (\             K.        _DBKTreeEventsRegOnExpand                                                                                                               ????         )?  9^             K.        _DBKTreeEventsRegOnCollapse                                                                                                             ????         :?  K$             K.        _DBKTreeEventsRegOnPopupMenuCommand                                                                                                     ????         L?  \?             K.        _DBKTreeEventsRegOnMouseOverCell                                                                                                        ????         ]?  n>             K.        _DBKTreeEventsRegOnModify                                                                                                               ????         o?  ?d             K.        _DBKTreeEventsRegOnPrePopupMenuCallback                                                                                                 ????         ??  ?             K.        _DBKTreeEventsRegOnDragOver                                                                                                             ????         ??  ??             K.        _DBKTreeEventsRegOnDragDrop                                                                                                             ????         ??  ?8             K.        New_DBKTree                                                                                                                             ????         ??  Ô             K.        Open_DBKTree                                                                                                                            ????       ????  ?i             K.        _DBKTreeAddColumn                                                                                                                       ????       ????  ??             K.        _DBKTreeGetColumnName                                                                                                                   ????       ????  ?M             K.        _DBKTreeGetColumnCount                                                                                                                  ????       ????  ??             K.        _DBKTreeRemoveColumn                                                                                                                    ????       ????  ?<             K.        _DBKTreeResetContent                                                                                                                    ????       ????  ?/             K.        _DBKTreeSetColumnName                                                                                                                   ????       ????  ??             K.        _DBKTreeAddNode                                                                                                                         ????       ???? <             K.        _DBKTreeRemoveNode                                                                                                                      ????       ???? ?             K.        _DBKTreeSetNodeItemText                                                                                                                 ????       ???? ?             K.        _DBKTreeGetNodeItemText                                                                                                                 ????       ???? ?             K.        _DBKTreeGetNodeItemTextEx                                                                                                               ????       ???? 'D             K.        _DBKTreeSetColumnWidth                                                                                                                  ????       ???? /w             K.        _DBKTreeSetNodeItemImage                                                                                                                ????       ???? 7r             K.        _DBKTreeSetRedraw                                                                                                                       ????       ???? ?8             K.        _DBKTreeExpand                                                                                                                          ????       ???? G              K.        _DBKTreeCollapse                                                                                                                        ????       ???? N?             K.        _DBKTreeExpandLevel                                                                                                                     ????       ???? V?             K.        _DBKTreeSetNodeItemFormat                                                                                                               ????       ???? _!             K.        _DBKTreeGetNodeItemFormat                                                                                                               ????       ???? f?             K.        _DBKTreeSetNodeFontStyle                                                                                                                ????       ???? n?             K.        _DBKTreeGetNodeFontStyle                                                                                                                ????       ???? v?             K.        _DBKTreeGetParentNode                                                                                                                   ????       ???? ~?             K.        _DBKTreeSetScrollBarVisible                                                                                                             ????       ???? ?I             K.        _DBKTreeGetScrollBarVisible                                                                                                             ????       ???? ?A             K.        _DBKTreeSetNodeItemFontStyle                                                                                                            ????       ???? ??             K.        _DBKTreeGetNodeItemFontStyle                                                                                                            ????       ???? ??             K.        _DBKTreeSetNodeItemTextColor                                                                                                            ????       ???? ?8             K.        _DBKTreeGetNodeItemTextColor                                                                                                            ????       ???? ?m             K.        _DBKTreeSetNodeItemBackColor                                                                                                            ????       ???? ??             K.        _DBKTreeGetNodeItemBackColor                                                                                                            ????       ???? ??             K.        _DBKTreeIsExpanded                                                                                                                      ????       ???? ?e             K.        _DBKTreeMoveNode                                                                                                                        ????       ???? С             K.        _DBKTreeSetPopupMenu                                                                                                                    ????       ???? ؛             K.        _DBKTreeSetNodeItemEditable                                                                                                             ????       ???? ??             K.        _DBKTreeGetNodeItemEditable                                                                                                             ????       ???? ??             K.        _DBKTreeFromArray                                                                                                                       ????       ???? ?             K.        _DBKTreeToArray                                                                                                                         ????       ???? ?_             K.        _DBKTreeFromString                                                                                                                      ????       ???? ?             K.        _DBKTreeToString                                                                                                                        ????       ???? j             K.        _DBKTreeToString2                                                                                                                       ????       ???? ?             K.        _DBKTreeGetColumnNext                                                                                                                   ????       ???? ?             K.        _DBKTreeGetNodeNext                                                                                                                     ????       ???? $?             K.        _DBKTreeGetNodeHasChildren                                                                                                              ????       ???? ,?             K.        _DBKTreeGetNodeItemImage                                                                                                                ????       ???? 5?             K.        _DBKTreeSetNodeItemTextEx                                                                                                               ????       ???? =;             K.        _DBKTreeEnsureVisible                                                                                                                   ????       ???? E(             K.        _DBKTreeRenameNodeKey                                                                                                                   ????       ???? M+             K.        _DBKTreeIsNodeExist                                                                                                                     ????       ???? T?             K.        _DBKTreeSetColumnHeadFontStyle                                                                                                          ????       ???? \             K.        _DBKTreeGetColumnHeadFontStyle                                                                                                          ????       ???? d             K.        _DBKTreeSetColumnHeadFormat                                                                                                             ????       ???? k?             K.        _DBKTreeGetColumnHeadFormat                                                                                                             ????       ???? s?             K.        _DBKTreeGetVersion                                                                                                                      ????       ???? {             K.        _DBKTreeSetFrameBorder                                                                                                                  ????       ???? ??             K.        _DBKTreeGetFrameBorder                                                                                                                  ????       ???? ?U             K.        _DBKTreeFromXML                                                                                                                         ????       ???? ?6             K.        _DBKTreeToXML                                                                                                                           ????       ???? ??             K.        _DBKTreeSetNodeData                                                                                                                     ????       ???? ?O             K.        _DBKTreeGetNodeData                                                                                                                     ????       ???? ?I             K.        _DBKTreeSetNodeItemData                                                                                                                 ????       ???? ??             K.        _DBKTreeGetNodeItemData                                                                                                                 ????       ???? ??             K.        _DBKTreeGetColumnWidth                                                                                                                  ????       ???? ??             K.        _DBKTreeSetNodeItemTipTextEnable                                                                                                        ????       ???? ?             K.        _DBKTreeGetNodeItemTipTextEnable                                                                                                        ????       ???? ?             K.        _DBKTreeRefresh                                                                                                                         ????       ???? ?W             K.        _DBKTreeAboutBox                                                                                                                        ????       ???? ޼             K.        _DBKTreeGetSelectedItem                                                                                                                 ????       ???? ??             K.        _DBKTreeSetSelectedItem                                                                                                                 ????       ???? ??             K.        _DBKTreeSetByRefSelectedItem                                                                                                            ????       ???? ??             K.        _DBKTreeGetBackColor                                                                                                                    ????       ???? ??             K.        _DBKTreeSetBackColor                                                                                                                    ????       ???? ?             K.        _DBKTreeSetByRefBackColor                                                                                                               ????       ???? ?             K.        _DBKTreeGetBackTextColor                                                                                                                ????       ???? ?             K.        _DBKTreeSetBackTextColor                                                                                                                ????       ???? ?             K.        _DBKTreeSetByRefBackTextColor                                                                                                           ????       ???? ?             K.        _DBKTreeGetForeTextColor                                                                                                                ????       ???? $?             K.        _DBKTreeSetForeTextColor                                                                                                                ????       ???? +?             K.        _DBKTreeSetByRefForeTextColor                                                                                                           ????       ???? 2?             K.        _DBKTreeGetColumnHeadingVisible                                                                                                         ????       ???? 9?             K.        _DBKTreeSetColumnHeadingVisible                                                                                                         ????       ???? @?             K.        _DBKTreeSetByRefColumnHeadingVisible                                                                                                    ????       ???? G?             K.        _DBKTreeGetGridLines                                                                                                                    ????       ???? N?             K.        _DBKTreeSetGridLines                                                                                                                    ????       ???? U?             K.        _DBKTreeSetByRefGridLines                                                                                                               ????       ???? \?             K.        _DBKTreeGetGridLineColor                                                                                                                ????       ???? c?             K.        _DBKTreeSetGridLineColor                                                                                                                ????       ???? j?             K.        _DBKTreeSetByRefGridLineColor                                                                                                           ????       ???? s?             K.        _DBKTreeGetImageList                                                                                                                    ????       ???? z?             K.        _DBKTreeSetImageList                                                                                                                    ????       ???? ??             K.        _DBKTreeSetByRefImageList                                                                                                               ????       ???? ??             K.        _DBKTreeGetFont                                                                                                                         ????       ???? ??             K.        _DBKTreeSetFont                                                                                                                         ????       ???? ??             K.        _DBKTreeSetByRefFont                                                                                                                    ????       ???? ??             K.        _DBKTreeGetColumnResizing                                                                                                               ????       ???? ??             K.        _DBKTreeSetColumnResizing                                                                                                               ????       ???? ??             K.        _DBKTreeSetByRefColumnResizing                                                                                                          ????       ???? ??             K.        _DBKTreeGetAutoImages                                                                                                                   ????       ???? ??             K.        _DBKTreeSetAutoImages                                                                                                                   ????       ???? ??             K.        _DBKTreeSetByRefAutoImages                                                                                                              ????       ???? ??             K.        _DBKTreeGetTreeLineColor                                                                                                                ????       ???? ??             K.        _DBKTreeSetTreeLineColor                                                                                                                ????       ???? ??             K.        _DBKTreeSetByRefTreeLineColor                                                                                                           ????       ???? ??             K.        _DBKTreeGetTreeLines                                                                                                                    ????       ???? ??             K.        _DBKTreeSetTreeLines                                                                                                                    ????       ???? ??             K.        _DBKTreeSetByRefTreeLines                                                                                                               ????       ???? ??             K.        _DBKTreeGetEnableEventMouseOverCell                                                                                                     ????       ???? ??             K.        _DBKTreeSetEnableEventMouseOverCell                                                                                                     ????       ???? ?             K.        _DBKTreeSetByRefEnableEventMouseOverCell                                                                                                ????       ???? ?             K.        _DBKTreeGetEnableEventModify                                                                                                            ????       ???? ?             K.        _DBKTreeSetEnableEventModify                                                                                                            ????       ???? ?             K.        _DBKTreeSetByRefEnableEventModify                                                                                                       ????       ????              K.        _DBKTreeGetEditable                                                                                                                     ????       ???? $?             K.        _DBKTreeSetEditable                                                                                                                     ????       ???? +?             K.        _DBKTreeSetByRefEditable                                                                                                                ????       ???? 3             K.        _DBKTreeGetEnableDragAndDrop                                                                                                            ????       ???? :             K.        _DBKTreeSetEnableDragAndDrop                                                                                                            ????       ???? A             K.        _DBKTreeSetByRefEnableDragAndDrop                                                                                                       ????       ???? H4             K.        _DBKTreeGetDragMode                                                                                                                     ????       ???? O'             K.        _DBKTreeSetDragMode                                                                                                                     ????       ???? V             K.        _DBKTreeSetByRefDragMode                                                                                                                ????       ???? ]1             K.        _DBKTreeGetDragIcon                                                                                                                     ????       ???? d$             K.        _DBKTreeSetDragIcon                                                                                                                     ????       ???? k             K.        _DBKTreeSetByRefDragIcon                                                                                                                ????       ???? r.             K.        _DBKTreeGetEnableSelection                                                                                                              ????       ???? y!             K.        _DBKTreeSetEnableSelection                                                                                                              ????       ???? ?%             K.        _DBKTreeSetByRefEnableSelection                                                                                                         ????       ???? ?D             K.        _DBKTreeGetEnableFocusCell                                                                                                              ????       ???? ?7             K.        _DBKTreeSetEnableFocusCell                                                                                                              ????       ???? ?;             K.        _DBKTreeSetByRefEnableFocusCell                                                                                                         ????       ???? ?Z             K.        _DBKTreeGetEnableTreeKeyDirection                                                                                                       ????       ???? ?M             K.        _DBKTreeSetEnableTreeKeyDirection                                                                                                       ????       ???? ?Q             K.        _DBKTreeSetByRefEnableTreeKeyDirection                                                                                                  ????       ???? ?p             K.        _DBKTreeGetListMode                                                                                                                     ????       ???? ?c             K.        _DBKTreeSetListMode                                                                                                                     ????       ???? ?g             K.        _DBKTreeSetByRefListMode                                                                                                                ????        ?_ ɨ             K.        New_DBKGrid                                                                                                                             ????        ?Q ?             K.        Open_DBKGrid                                                                                                                            ????       ???? ?s             K.        _DBKGridRefresh                                                                                                                         ????       ???? ??             K.        _DBKGridAboutBox                                                                                                                              ?                                                                                    ?Event Callback Registration Functions                                               (_DBKTreeEvents                                                                       ?Register NodeClick Callback                                                          ?Register Expand Callback                                                             ?Register Collapse Callback                                                           ?Register PopupMenuCommand Callback                                                   ?Register MouseOverCell Callback                                                      ?Register Modify Callback                                                             ?Register PrePopupMenuCallback Callback                                               ?Register DragOver Callback                                                           ?Register DragDrop Callback                                                          __DBKTree                                                                             ?New _DBKTree                                                                         ?Open _DBKTree                                                                        ?Add Column                                                                           ?Get Column Name                                                                      ?Get Column Count                                                                     ?Remove Column                                                                        ?Reset Content                                                                        ?Set Column Name                                                                      ?Add Node                                                                             ?Remove Node                                                                          ?Set Node Item Text                                                                   ?Get Node Item Text                                                                   ?Get Node Item Text Ex                                                                ?Set Column Width                                                                     ?Set Node Item Image                                                                  ?Set Redraw                                                                           ?Expand                                                                               ?Collapse                                                                             ?Expand Level                                                                         ?Set Node Item Format                                                                 ?Get Node Item Format                                                                 ?Set Node Font Style                                                                  ?Get Node Font Style                                                                  ?Get Parent Node                                                                      ?Set Scroll Bar Visible                                                               ?Get Scroll Bar Visible                                                               ?Set Node Item Font Style                                                             ?Get Node Item Font Style                                                             ?Set Node Item Text Color                                                             ?Get Node Item Text Color                                                             ?Set Node Item Back Color                                                             ?Get Node Item Back Color                                                             ?Is Expanded                                                                          ?Move Node                                                                            ?Set Popup Menu                                                                       ?Set Node Item Editable                                                               ?Get Node Item Editable                                                               ?From Array                                                                           ?To Array                                                                             ?From String                                                                          ?To String                                                                            ?To String2                                                                           ?Get Column Next                                                                      ?Get Node Next                                                                        ?Get Node Has Children                                                                ?Get Node Item Image                                                                  ?Set Node Item Text Ex                                                                ?Ensure Visible                                                                       ?Rename Node Key                                                                      ?Is Node Exist                                                                        ?Set Column Head Font Style                                                           ?Get Column Head Font Style                                                           ?Set Column Head Format                                                               ?Get Column Head Format                                                               ?Get Version                                                                          ?Set Frame Border                                                                     ?Get Frame Border                                                                     ?From XML                                                                             ?To XML                                                                               ?Set Node Data                                                                        ?Get Node Data                                                                        ?Set Node Item Data                                                                   ?Get Node Item Data                                                                   ?Get Column Width                                                                     ?Set Node Item Tip Text Enable                                                        ?Get Node Item Tip Text Enable                                                        ?Refresh                                                                              ?About Box                                                                            ?Get Selected Item                                                                    ?Set Selected Item                                                                    ?Set By Ref Selected Item                                                             ?Get Back Color                                                                       ?Set Back Color                                                                       ?Set By Ref Back Color                                                                ?Get Back Text Color                                                                  ?Set Back Text Color                                                                  ?Set By Ref Back Text Color                                                           ?Get Fore Text Color                                                                  ?Set Fore Text Color                                                                  ?Set By Ref Fore Text Color                                                           ?Get Column Heading Visible                                                           ?Set Column Heading Visible                                                           ?Set By Ref Column Heading Visible                                                    ?Get Grid Lines                                                                       ?Set Grid Lines                                                                       ?Set By Ref Grid Lines                                                                ?Get Grid Line Color                                                                  ?Set Grid Line Color                                                                  ?Set By Ref Grid Line Color                                                           ?Get Image List                                                                       ?Set Image List                                                                       ?Set By Ref Image List                                                                ?Get Font                                                                             ?Set Font                                                                             ?Set By Ref Font                                                                      ?Get Column Resizing                                                                  ?Set Column Resizing                                                                  ?Set By Ref Column Resizing                                                           ?Get Auto Images                                                                      ?Set Auto Images                                                                      ?Set By Ref Auto Images                                                               ?Get Tree Line Color                                                                  ?Set Tree Line Color                                                                  ?Set By Ref Tree Line Color                                                           ?Get Tree Lines                                                                       ?Set Tree Lines                                                                       ?Set By Ref Tree Lines                                                                ?Get Enable Event Mouse Over Cell                                                     ?Set Enable Event Mouse Over Cell                                                     ?Set By Ref Enable Event Mouse Over Cell                                              ?Get Enable Event Modify                                                              ?Set Enable Event Modify                                                              ?Set By Ref Enable Event Modify                                                       ?Get Editable                                                                         ?Set Editable                                                                         ?Set By Ref Editable                                                                  ?Get Enable Drag And Drop                                                             ?Set Enable Drag And Drop                                                             ?Set By Ref Enable Drag And Drop                                                      ?Get Drag Mode                                                                        ?Set Drag Mode                                                                        ?Set By Ref Drag Mode                                                                 ?Get Drag Icon                                                                        ?Set Drag Icon                                                                        ?Set By Ref Drag Icon                                                                 ?Get Enable Selection                                                                 ?Set Enable Selection                                                                 ?Set By Ref Enable Selection                                                          ?Get Enable Focus Cell                                                                ?Set Enable Focus Cell                                                                ?Set By Ref Enable Focus Cell                                                         ?Get Enable Tree Key Direction                                                        ?Set Enable Tree Key Direction                                                        ?Set By Ref Enable Tree Key Direction                                                 ?Get List Mode                                                                        ?Set List Mode                                                                        ?Set By Ref List Mode                                                                ?_DBKGrid                                                                             ?New _DBKGrid                                                                         ?Open _DBKGrid                                                                        ?Refresh                                                                              ?About Box                                                                       