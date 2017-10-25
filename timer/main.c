#define TIMER_PREFIX "main"
#include "../include/time.h"
#include <stdlib.h>
int main()
{
	int i;
	for(i = 0; i < 128; i ++)
	{
		record_time();
		record_time();
	}
	return 0;
}
