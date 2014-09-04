#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/poll.h>
#include <sys/wait.h>
#include <time.h>

#define MAXCONPEND 5

int main(int argc, char *argv[])
{
    	struct sockaddr_in sadd;
    	struct sockaddr_in cadd;
    	unsigned short port;
    	
	socklen_t clntLen;
    	int connfd;
    	int sock,sfd;
    	char buf[128];
    	int clientTotal = atoi(argv[2])+1;
    	int fsarray[clientTotal+1];
    	int i = 1;

	if(argc != 3)
	{
		printf("Usage: %s server <Server port> <# of Clients>\n",argv[0]);
	}

	port = atoi(argv[1]);

    	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{        	
		printf("socket() failed");
		exit(1);      	
	}

    	memset(&sadd, 0, sizeof(sadd));
    	sadd.sin_family = AF_INET;               
    	sadd.sin_addr.s_addr = htonl(INADDR_ANY); 
    	sadd.sin_port = htons(port);    

    	if (bind(sock, (struct sockaddr *) &sadd, sizeof(sadd)) < 0)
	{        	
		printf("bind() failed");
		exit(1);      	
	}

    	if (listen(sock, MAXCONPEND) < 0)
	{        	
		printf("listen() failed");
		exit(1);      	
	}

	struct pollfd ufds[clientTotal+1];
	ufds[0].fd = sock;
	ufds[0].events = POLLIN;
	
	int maxi = 0;
	for(i = 1; i < clientTotal+1;i++) 
		ufds[i].fd = -1;
	srand(time(0));
	int random = rand()%100+1;
	printf("%d\n",random);
	int pollInt;

	for(;;)
	{
		pollInt = poll(ufds,clientTotal,5000);
		if(pollInt== -1)
			perror("ERROR\n");
		else if(pollInt == 0)
			printf("Timeout\n");
		if(ufds[0].revents & POLLIN) 
		{
			clntLen = sizeof(cadd);
			connfd = accept(sock, (struct sockaddr *) &cadd, &clntLen);
			printf("handling new client\n");
			for(i = 1; i < clientTotal+1;i++)
			{
				if(ufds[i].fd < 0)
				{
					ufds[i].fd = connfd;
					break;
				}
			}
			if(i == clientTotal+1)
				perror("Too many clients\n");
			ufds[i].events = POLLIN;
			if(i>maxi)
				maxi = i;
			if(--pollInt<=0)
				continue;
		}
		for(i = 1; i <=maxi;i++)
		{
			if((sfd = ufds[i].fd)<0) continue;
			if(ufds[i].revents & POLLIN)
			{
				recv(sfd,buf,128,0);
				int ans = atoi(buf);
				fsarray[i] = ans;
			}
		}
		int ansPolling;
		ansPolling = 1;
		for(i = 1; i < clientTotal;i++)
		{
			if(fsarray[i]==0)
				ansPolling = 0;

		}
		if(ansPolling)
		{
			int winner = 1;
			int closest;
			int clients[clientTotal+1];
			for(i = 1; i < clientTotal+1; i++)
			{
				clients[i] = abs(random - fsarray[i]);
			}
			closest = clients[1];
			for(i = 2; i < clientTotal+1; i++)
			{
				if(clients[i] < closest)
				{
					winner = i;
					closest = clients[i];
				}
			}
			
			for(i = 1; i < clientTotal+1; i++)
			{
				ufds[i].events = POLLOUT;
				if(i == winner)
				{
					char message[] = "you win.";
					strcpy(buf,message);
					if((sfd = ufds[i].fd)<0) continue;
					if(ufds[i].revents & POLLOUT) send(sfd,buf,128,0);
				}
				else
				{
					char message[] = "you lost.";
					strcpy(buf,message);
					if((sfd = ufds[i].fd)<0) continue;
					if(ufds[i].revents & POLLOUT) send(sfd,buf,128,0);
				}
			}
			
		}
	}

    	return 0;
}
