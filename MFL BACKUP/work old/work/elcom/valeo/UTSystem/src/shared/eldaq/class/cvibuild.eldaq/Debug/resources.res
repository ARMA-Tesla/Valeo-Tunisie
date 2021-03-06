        ??  ??                  ?      ?? ??     0 	        ?4   V S _ V E R S I O N _ I N F O     ???                 ?                        ?   S t r i n g F i l e I n f o   ?   0 4 0 9 0 4 b 0        C o m p a n y N a m e     R   F i l e D e s c r i p t i o n     e l d a q _ 3 2   ( D e b u g   x 8 6 )     (   F i l e V e r s i o n     1 . 0   2 	  I n t e r n a l N a m e   e l d a q _ 3 2     H   L e g a l C o p y r i g h t   C o p y r i g h t   ?     2 0 1 8   B   O r i g i n a l F i l e n a m e   e l d a q _ 3 2 . d l l     4 
  P r o d u c t N a m e       e l d a q _ 3 2   ,   P r o d u c t V e r s i o n   1 . 0   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	?H  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        long __stdcall eldaq_CheckAttributeViBoolean(unsigned long vi, const char *channelName, unsigned long attributeId, unsigned short value);
long __stdcall eldaq_CheckAttributeViInt32(unsigned long vi, const char *channelName, unsigned long attributeId, long value);
long __stdcall eldaq_CheckAttributeViReal64(unsigned long vi, const char *channelName, unsigned long attributeId, double value);
long __stdcall eldaq_CheckAttributeViSession(unsigned long vi, const char *channelName, unsigned long attributeId, unsigned long value);
long __stdcall eldaq_CheckAttributeViString(unsigned long vi, const char *channelName, unsigned long attributeId, const char *value);
long __stdcall eldaq_ClearError(unsigned long vi);
long __stdcall eldaq_ClearInterchangeWarnings(unsigned long vi);
long __stdcall eldaq_ConfigureMeasurement(unsigned long vi, char *measuredChannels, long sampleRate, double measuredIntervals);
long __stdcall eldaq_Disable(unsigned long vi);
long __stdcall eldaq_GetAttributeViBoolean(unsigned long vi, const char *channelName, unsigned long attributeId, unsigned short *value);
long __stdcall eldaq_GetAttributeViInt32(unsigned long vi, const char *channelName, unsigned long attributeId, long *value);
long __stdcall eldaq_GetAttributeViReal64(unsigned long vi, const char *channelName, unsigned long attributeId, double *value);
long __stdcall eldaq_GetAttributeViSession(unsigned long vi, const char *channelName, unsigned long attributeId, unsigned long *value);
long __stdcall eldaq_GetAttributeViString(unsigned long vi, const char *channelName, unsigned long attributeId, long bufSize, char *value);
long __stdcall eldaq_GetChannelScale(unsigned long vi, char *channelName, const char *scaleName, void *_pScaleInfo);
long __stdcall eldaq_GetError(unsigned long vi, long *errorCode, long bufferSize, char *description);
long __stdcall eldaq_GetNextCoercionRecord(unsigned long vi, long bufferSize, char *recordBuf);
long __stdcall eldaq_GetNextInterchangeWarning(unsigned long vi, long bufferSize, char *warnString);
long __stdcall eldaq_GetSpecificDriverCHandle(unsigned long vi, unsigned long *specificHandle);
long __stdcall eldaq_GetSpecificDriverIUnknownPtr(unsigned long vi, void *ref);
long __stdcall eldaq_InitWithOptions(char *logicalName, unsigned short idQuery, unsigned short resetDevice, const char *optionString, unsigned long *newSession);
long __stdcall eldaq_InvalidateAllAttributes(unsigned long vi);
long __stdcall eldaq_LoadCalibration(unsigned long vi, const char *calibrationFile);
long __stdcall eldaq_LockSession(unsigned long vi, unsigned short *callerHasLock);
long __stdcall eldaq_ReadForce(unsigned long vi, char *channels, char *clockSource, double min, double max, long sampsPerChan, long rate, double *data);
long __stdcall eldaq_ReadMeasurement(unsigned long vi, char *channelName, void *scaleInfo, long measurementType, double *value);
long __stdcall eldaq_ReadWaveform(unsigned long vi, char *channelName, char *scale, long arraySize, double *data, double *startTime, double *xIncrement);
long __stdcall eldaq_ResetInterchangeCheck(unsigned long vi);
long __stdcall eldaq_ResetWithDefaults(unsigned long vi);
long __stdcall eldaq_SetAttributeViBoolean(unsigned long vi, const char *channelName, unsigned long attributeId, unsigned short value);
long __stdcall eldaq_SetAttributeViInt32(unsigned long vi, const char *channelName, unsigned long attributeId, long value);
long __stdcall eldaq_SetAttributeViReal64(unsigned long vi, const char *channelName, unsigned long attributeId, double value);
long __stdcall eldaq_SetAttributeViSession(unsigned long vi, const char *channelName, unsigned long attributeId, unsigned long value);
long __stdcall eldaq_SetAttributeViString(unsigned long vi, const char *channelName, unsigned long attributeId, const char *value);
long __stdcall eldaq_StartMeasurement(unsigned long vi);
long __stdcall eldaq_StopMeasurement(unsigned long vi);
long __stdcall eldaq_UnlockSession(unsigned long vi, unsigned short *callerHasLock);
long __stdcall eldaq_WriteDigitalLine(unsigned long vi, char *digitalChannels, long status);
long __stdcall eldaq_close(unsigned long vi);
long __stdcall eldaq_error_message(unsigned long vi, long statusCode, char *message);
long __stdcall eldaq_error_query(unsigned long vi, long *errorCode, char *errorMessage);
long __stdcall eldaq_init(char *logicalName, unsigned short idQuery, unsigned short resetDevice, unsigned long *newSession);
long __stdcall eldaq_reset(unsigned long vi);
long __stdcall eldaq_revision_query(unsigned long vi, char *driverRev, char *instrRev);
long __stdcall eldaq_self_test(unsigned long vi, short *testResult, char *testMessage);
   