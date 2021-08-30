#if !defined(__TEST_GRAPH_H__)
#define __TEST_GRAPH_H__

#include <eltypes.h>

SElExceptionPtr	StatisticTestGraph(void);
SElExceptionPtr	StatisticTestGraphClean(void);
SElExceptionPtr BkTreeTestNodeClick (char *keyNode);
SElExceptionPtr CalendarCallbackKosu(time_t time_selected);

#endif  
