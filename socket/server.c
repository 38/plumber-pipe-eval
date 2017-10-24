#define TIMER_PREFIX "server"
#include "../include/time.h"
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in saddr = {}, caddr = {};
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(atoi(argv[2]));
	bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));

	int cfd;
	socklen_t sl = sizeof(caddr);
	listen(sfd, 128);
	perror("");
	sfd = accept(sfd, (struct sockaddr*)&caddr, &sl);


	int i;
	int size = atoi(argv[1]);
	for(i = 0; i < 32; i ++)
	{
		char  buf[size];
		int todo = size;
		while(todo > 0) 
			todo -= read(sfd, buf + size - todo, size);
		record_time();
		record_time();
		todo = size;
		while(todo > 0)
			todo -= write(sfd, buf + size - todo, size);
	}
	return 0;
}
