#include "libtest/test.h"
#define TIMER_PREFIX "main"
#include "../include/time.h"
#include <stdlib.h>
int main()
{
	int i;
	for(i = 0; i < 32; i ++)
	{
		struct TYPE a;
		int j;
		for(j = 0; j < sizeof(a); j += 4)
			*(uint32_t*)(((char*)&a) + j) = rand();
		record_time();
		FUNC(a);
		record_time();
	}
	return 0;
}
