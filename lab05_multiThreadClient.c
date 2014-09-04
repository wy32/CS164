#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 32   /* Size of receive buffer */

int main(int argc, char *argv[])
{
    	int sock;                        /* Socket descriptor */
    	struct sockaddr_in echoServAddr; /* Echo server address */
    	unsigned short echoServPort;     /* Echo server port */
    	char *servIP;                    /* Server IP address (dotted quad) */
    	char *msg;                /* String to send to echo server */
    	char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    	unsigned int msgLen;      /* Length of string to echo */
    	int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */
    	int guess;

    	if ((argc < 3) || (argc > 4)) 
    	{
       		fprintf(stderr, "Usage: %s <Server IP> <Guess> [<Echo Port>]\n",
               	argv[0]);
       		exit(1);
    	}

    	servIP = argv[1];             
    	msg = argv[2]; 

    	if (argc == 4)
        	echoServPort = atoi(argv[3]);
    	else
        	echoServPort = 7;

    	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        	printf("socket() failed");

	//Create Client Socket 
    	memset(&echoServAddr, 0, sizeof(echoServAddr));
    	echoServAddr.sin_family      = AF_INET; 
    	echoServAddr.sin_addr.s_addr = inet_addr(servIP); 
    	echoServAddr.sin_port        = htons(echoServPort);

    	//Establish connect
    	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        	printf("connect() failed");

    	msgLen = strlen(msg);

    
    	if (send(sock, msg, msgLen, 0) != msgLen)
        	printf("send() error bytes");

    
    	totalBytesRcvd = 0;
    	printf("Received: ");              
    	while (totalBytesRcvd < msgLen)
    	{
        	if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
		{
            		printf("recv() failed or connection closed prematurely\n");
			exit(1);
		}
        	totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        	echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
        	printf("%s", echoBuffer);      /* Print the echo buffer */
    	}

    	printf("\n");    /* Print a final linefeed */

    	close(sock);
    	exit(0);
}
