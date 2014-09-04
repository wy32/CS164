#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>


#define MAXLINE 4096 /*max text line length*/
//#define SERV_PORT 8080 /*port*/
#define LISTENQ 8 /*maximum number of client connections*/

int main (int argc, char **argv)
{
	int listenfd, connfd, n;
	int SERV_PORT;
	pid_t childpid;
	socklen_t clilen;
	char buf[MAXLINE];
	char exit_msg[4];
	struct sockaddr_in cliaddr, servaddr;

	if (argc !=2) 
	{
  		printf("Usage: server <Server Port>\n");
  		exit(1);
 	}

	SERV_PORT = atoi(argv[1]);
	strcpy(exit_msg,"exit");

	//Create a socket for the soclet
	//If sockfd<0 there was an error in the creation of the socket
 	if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) 
	{
  		printf("Problem in creating the socket");
  		exit(2);
 	}


 	//preparation of the socket address
 	servaddr.sin_family = AF_INET;
 	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 	servaddr.sin_port = htons(SERV_PORT);

 	//bind the socket
 	bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

 	//listen to the socket by creating a connection queue, then wait for clients
 	listen (listenfd, LISTENQ);

 	printf("%s\n","Server running...waiting for connections.");

 	for ( ; ; ) 
	{

  		clilen = sizeof(cliaddr);
  		//accept a connection
  		connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

  		printf("%s\n","Received request...");

  		if ( (childpid = fork ()) == 0 ) 
		{//if it’s 0, it’s child process
    			printf ("%s %d\n","Child created with pid: ",(unsigned int)getpid());

    			//close listening socket
    			close (listenfd);

    			while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  
			{
     				printf("%s","String received from and resent to the client:");
     				printf("%s\n",buf);
				if(buf[0]==exit_msg[0] && buf[1]==exit_msg[1] && buf[2]==exit_msg[2] && buf[3]==exit_msg[3])
				{
					printf("%s\n","Received exit command, closing connection");
        				close(connfd);
				}
     				send(connfd, buf, n, 0);
    			}
			
    			exit(0);
  		}
  
		//close socket of the server
  		close(connfd);
	}
}

