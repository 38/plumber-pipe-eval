#define TIMER_PREFIX "client"
#include "../include/time.h"
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char** argv)
{
	int size = atoi(argv[1]);
	int chin[2], chout[2], pid;
	assert(pipe(chin) >= 0);
	assert(pipe(chout) >= 0);
	if((pid = fork()) == 0)
	{
		close(chin[1]);
		close(chout[0]);
		dup2(chin[0], STDIN_FILENO);
		dup2(chout[1], 123);
		close(chin[0]);
		close(chout[1]);
		char* args[] = {"./server", strdup(argv[1]), NULL};
		execvp("./server", args);
		perror("exec");
		exit(1);
	}
	close(chin[0]);
	close(chout[1]);

	int i;
	for(i = 0; i < 32; i ++)
	{
		char buf[size];
		int j;
		for(j = 0; j < size; j += 4)
			*(uint32_t*)(((char*)&buf) + j) = rand();
		int todo = size;
		record_time();
		while(todo > 0) 
			todo -= write(chin[1], buf + size - todo, todo);
		todo = size;
		while(todo > 0) 
			todo -= read(chout[0], buf + size - todo, todo);
		record_time();
	}
	waitpid(pid, NULL, 0);
	return 0;
}
