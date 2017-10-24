#include "test.h"
#define  TIMER_PREFIX "static_lib"
#include "../../include/time.h"
#define F(x) struct _c_##x f##x(struct _c_##x c)\
{\
	record_time();\
	record_time();\
	return c;\
}

F(1);
F(64);
F(4096);
F(65536);
F(131072);
