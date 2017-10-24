#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
static const char* _prefix = TIMER_PREFIX;
static uint64_t _ts[128];
static uint32_t _tsc = 0;
static inline void record_time()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	_ts[_tsc] = ts.tv_sec * 1000000000ull;
	_ts[_tsc] += ts.tv_nsec;
	_tsc ++;
}
__attribute__((destructor)) static void dump_time() 
{
	uint32_t i;
	for(i = 0; i < _tsc; i ++)
		printf("%s[%u] = %"PRIu64"\n", _prefix, i, _ts[i]);
}
