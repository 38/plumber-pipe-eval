#define TIMER_PREFIX "client"
#include "../include/time.h"
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int size = atoi(argv[1]);
	srand((unsigned)time(NULL));
	int pid, port = rand() % 32768 + 32768;
	if((pid = fork()) == 0)
	{
		static char buf[32];
		snprintf(buf,sizeof(buf), "%d", port);
		char* args[] = {"./server", argv[1], buf, NULL};
		execvp("./server", args);
		perror("exec");
		exit(1);
	}
	sleep(1);
	int i;
	int sockfd = -1;
	struct sockaddr_in dest;
	for(i = 0; i < 32; i ++)
	{
		char buf[size];
		int j;
		for(j = 0; j < size; j += 4)
			*(uint32_t*)(((char*)&buf) + j) = rand();
		int todo = size;
		record_time();
		if(sockfd < 0)
		{
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			dest.sin_family = AF_INET;
			dest.sin_port = htons(port);
			inet_aton("127.0.0.1", &dest.sin_addr.s_addr);
			connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
		}
		while(todo > 0) 
			todo -= write(sockfd, buf + size - todo, todo);
		todo = size;
		while(todo > 0) 
			todo -= read(sockfd, buf + size - todo, todo);
		record_time();
	}
	return 0;
}
