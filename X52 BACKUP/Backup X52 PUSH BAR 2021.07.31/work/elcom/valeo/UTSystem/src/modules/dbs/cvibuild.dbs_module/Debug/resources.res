        ??  ??                  ?       ?? ??     0 	        <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0" />l      ?? ??     0 	        l4   V S _ V E R S I O N _ I N F O     ???               ?                        ?   S t r i n g F i l e I n f o   ?   0 4 0 9 0 4 b 0        C o m p a n y N a m e     H   F i l e D e s c r i p t i o n     d b s   ( D e b u g   x 8 6 )   (   F i l e V e r s i o n     2 . 1   (   I n t e r n a l N a m e   d b s   H   L e g a l C o p y r i g h t   C o p y r i g h t   ?     2 0 2 1   8   O r i g i n a l F i l e n a m e   d b s . d l l   *   P r o d u c t N a m e       d b s     ,   P r o d u c t V e r s i o n   2 . 1   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	?\  X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S         0 	        int DllMain(struct HINSTANCE__ *hinstDLL, unsigned long fdwReason, void *lpvReserved);
struct _SElException *GetModuleAttribute(unsigned int attrID, unsigned int size, void *value) __declspec(dllexport);
struct _SElException *InitializeElcomModule(void *pInstance, const char *configuration, struct _SPluginArgs *pParam) __declspec(dllexport);
struct _SElException *InverseIntervals(unsigned int t1, unsigned int t2, struct _SIntervalResult *pIntervals, struct _SIntervalResult **pResult, unsigned int *interval_time);
struct _SElException *LoadElcomModule(const char *configuration, void **pInstance, struct _SPluginArgs *pParam) __declspec(dllexport);
struct _SElException *MergeIntervals(struct _SIntervalResult *pIntervals1, struct _SIntervalResult *pIntervals2, struct _SIntervalResult **pResult, unsigned int *interval_time);
struct _SElException *ThreadStart(unsigned long (*pThreadFnc)(void *) __attribute__((stdcall)), void *pData, HANDLE *hThread, HANDLE *hStopEvent);
struct _SElException *ThreadStatus(HANDLE hThread, unsigned int timeout, int *pStatus);
struct _SElException *ThreadStop(HANDLE *hThread, HANDLE *hStopEvent);
struct _SElException *UnloadElcomModule(void *pInstance) __declspec(dllexport);
int compare_LineParam(const void *a, const void *b);
int compare_Process(const void *a, const void *b);
int compare_Product(const void *a, const void *b);
int compare_ProductMaster(const void *a, const void *b);
int compare_ProductParameter(const void *a, const void *b);
int compare_Step(const void *a, const void *b);
int compare_Step2(const void *a, const void *b);
int compare_Test(const void *a, const void *b);
int compare_TestParameter(const void *a, const void *b);
int compare_pid(const void *a, const void *b);
struct _SElException *dbs_delete(struct _SDBS **pDBSPtr);
struct _SElException *dbs_new(struct _SDBS **pDBSPtr);
struct _SElException *dbsexport_delete(struct _SDBSExport **pDBSExportPtr);
struct _SElException *dbsexport_new(struct _SDBSExport **pDBSExportPtr, void *pDBS);
struct _SElException *dbskosu_delete(struct _SDBSKOSU **pKOSU);
struct _SElException *dbskosu_new(struct _SDBSKOSU **pKOSU, void *pDBS);
struct _SElException *dbslineexport_delete(struct _SDBSLineExport **pDBSLineExportPtr);
struct _SElException *dbslineexport_new(struct _SDBSLineExport **pDBSLineExportPtr, void *pDBS);
struct _SElException *dbslineparamlist_delete(struct _SDBSLineParamList **pDBSLineParamListPtr);
struct _SElException *dbslineparamlist_new(struct _SDBSLineParamList **pDBSLineParamListPtr, void *pDBS);
struct _SElException *dbslineprocesslist_delete(struct _SDBSLineProcessList **pDBSLineProcessListPtr);
struct _SElException *dbslineprocesslist_new(struct _SDBSLineProcessList **pDBSLineProcessListPtr, void *pDBS);
struct _SElException *dbslinesteplist_delete(struct _SDBSLineStepList **pDBSLineStepListPtr);
struct _SElException *dbslinesteplist_new(struct _SDBSLineStepList **pDBSLineStepListPtr, void *pDBS);
struct _SElException *dbsmasterlist_delete(struct _SDBSMasterList **pDBSMasterListPtr);
struct _SElException *dbsmasterlist_new(struct _SDBSMasterList **pDBSMasterListPtr, void *pDBS);
struct _SElException *dbsparamlist_delete(struct _SDBSParamList **pDBSParListPtr);
struct _SElException *dbsparamlist_new(struct _SDBSParamList **pDBSParListPtr, void *pDBS);
struct _SElException *dbsproductlist_delete(struct _SDBSProductList **pDBSProductListPtr);
struct _SElException *dbsproductlist_new(struct _SDBSProductList **pDBSProductListPtr, void *pDBS);
struct _SElException *dbsrights_delete(struct _SDBSUserRights **pDBSUserRightsPtr);
struct _SElException *dbsrights_new(struct _SDBSUserRights **pDBSUserRightsPtr, int count, void *pDBS);
struct _SElException *dbsstorage_delete(struct _SDBSStorage **pDBSStoragePtr);
struct _SElException *dbsstorage_new(struct _SDBSStorage **pDBSStoragePtr, void *pDBS);
struct _SElException *dbstesterlist_delete(struct _SDBSTesterList **pDBSTesterListPtr);
struct _SElException *dbstesterlist_new(struct _SDBSTesterList **pDBSTesterListPtr, void *pDBS);
struct _SElException *dbstestlist_delete(struct _SDBSTestList **pDBSTestListPtr);
struct _SElException *dbstestlist_new(struct _SDBSTestList **pDBSTestListPtr, void *pDBS);
struct _SElException *dbstestparamlist_delete(struct _SDBSTestParamList **pDBSTestParamListPtr);
struct _SElException *dbstestparamlist_new(struct _SDBSTestParamList **pDBSTestParamListPtr, void *pDBS);
struct _SElException *dbsuser_delete(struct _SDBSUser **pDBSUserPtr);
struct _SElException *dbsuser_new(struct _SDBSUser **pDBSUserPtr, int count, void *pDBS);
struct _SElException *dbsuserlist_delete(struct _SDBSUserList **pDBSUserListPtr);
struct _SElException *dbsuserlist_new(struct _SDBSUserList **pDBSUserListPtr, void *pDBS);
struct _SElException *eleventdata_delete(struct _SElEventData **pEventData);
struct _SElException *eleventdata_new(struct _SElEventData **pEventData);
void elexception_delete(struct _SElException **pException);
void elexception_log_set(void (*fn_log)(struct _SElException *));
struct _SElException *elexception_new(int errorCode, const char *messsage, const char *file, long line);
struct _SElException *elstring_delete(struct _SELString **pString);
struct _SElException *elstring_new(const char *pStr, struct _SELString **pString);
void excdisplay(struct _SElException *pException);
void excdispstr(struct _SElException *pException, char *str, int str_size);
struct _SElException *intervalresult_delete(struct _SIntervalResult **pInterval);
struct _SElException *intervalresult_new(struct _SIntervalResult **pInterval);
struct _SElException *objlist_delete(struct _SObjList **pList);
struct _SElException *objlist_new(struct _SObjList **pObjList);
struct _SElException *odbc_delete(struct _SOdbc **pOdbc);
struct _SElException *odbc_new(struct _SOdbc **pOdbc);
struct _SElException *userintervals_delete(struct _SUserIntervals **pUserIntervals);
struct _SElException *userintervals_new(struct _SUserIntervals **pUserIntervals);
     X   C V I D L L P R O T O T Y P E S   C V I D L L E X P O R T S 2       0 	                ?? ??     0	                                        