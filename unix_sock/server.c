#define TIMER_PREFIX "server"
#include "../include/time.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un saddr = {}, caddr = {};
	saddr.sun_family = AF_UNIX;
	snprintf(saddr.sun_path, sizeof(saddr.sun_path), "./socket_%s", argv[2]);
	bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));

	int cfd;
	socklen_t sl = sizeof(caddr);
	listen(sfd, 128);
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
