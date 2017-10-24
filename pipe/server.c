#define TIMER_PREFIX "server"
#include "../include/time.h"
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int size = atoi(argv[1]);
	char buf[size];
	int i;
	for(i = 0; i < 32; i ++)
	{
		int todo = size;
		while(todo > 0) 
			todo -= read(STDIN_FILENO, buf + size - todo, size);
		record_time();
		record_time();
		todo = size;
		while(todo > 0)
			todo -= write(123, buf + size - todo, size);
	}
	return 0;
}
