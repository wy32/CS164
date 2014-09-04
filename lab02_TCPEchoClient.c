#include <stdio.h>          /*for printf() and fprintf() */
#include <sys/socket.h>     /*for socket(), connect(), send(), recv()*/
#include <arpa/inet.h> 	    /*for sockaddr_in and inet_addr()*/
#include <stdlib.h> 	    /*for atoi()*/
#include <string.h>	    /*for memset()*/
#include <unistd.h>	    /*for close()*/
#include <netdb.h>

#define RCVBUFFERSIZE 32    /* Size of the receive buffer*/

int main(int argc,char *argv[])
{

	int sock;			/*Socket discriptor*/
	char *servIP;			/*Server's IP*/
	//char *num1;		/*String to be echoed*/
	//char *num2;
	//char *sum;
	struct hostent *hp;
	struct in_addr ip_addr;

	int echoServPort;		/*the port of the server*/
	char *serName;
	int echoForPort;		/*the port of the server*/
	char *forName;

	//int echoServPort;		/*the port of the server*/
	struct sockaddr_in echoServAddr;/*the adress of the server socket*/
	int echoStringLen;		/*the length of the echo string*/
	int bytesRcv,totalBytesRcv;
	char echoBuffer[RCVBUFFERSIZE];

	/*Checking for wrong arguments*/
	if((argc<3) || (argc>6)) 
	{
		fprintf(stderr,"Usage: %s <Server name> <Server port> <Forwarder name> <Forwarder Port> <Forwarding String>\n",argv[0]);
		exit(1);
	}

	hp = gethostbyname(argv[3]);
	ip_addr = *(struct in_addr *)(hp->h_addr);
	printf("Hostname: %s, was resolved to: %s\n", argv[3],inet_ntoa(ip_addr));
	servIP = inet_ntoa(ip_addr);
	
	//servIP=argv[1];  
	//echoString=argv[2];

	if(argc==6)
	{
		echoServPort=atoi(argv[2]);
		echoForPort=atoi(argv[4]);
		serName=argv[1];
		forName=argv[3];
	}
	else
		echoServPort=7;  /*the default echo server port*/

	/* Creation of the socket*/
	if((sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
	{
		perror("socket() failed");
		exit(1);
	}

	/*Configure the server*/
	memset(&echoServAddr,0,sizeof(echoServAddr));
	echoServAddr.sin_family=AF_INET;
	echoServAddr.sin_port=htons(echoForPort);
	echoServAddr.sin_addr.s_addr=inet_addr(servIP);
	
	/*Establish the connection*/
	if(connect(sock,(struct sockaddr *)&echoServAddr,sizeof(echoServAddr))<0)
	{
		perror("connect() failed");
		exit(1);
	}
	
	/*Sending of the data*/
	char echoString[RCVBUFFERSIZE];
	strcpy(echoString,argv[1]);
	strcat(echoString," ");
	strcat(echoString,argv[2]);
	strcat(echoString," ");
	strcat(echoString,argv[5]);
	echoStringLen=strlen(echoString);

	//printf(echoString);
	if(send(sock,echoString,echoStringLen,0)!=echoStringLen)
	{
		perror("send() failed");
		exit(1);
	}

	/*Receive the same string back from the echo server*/
	/*totalBytesRcv=0;
	printf("Received back from the server: ");
	while(totalBytesRcv<echoStringLen)
	{
		if((bytesRcv=recv(sock,echoBuffer,RCVBUFFERSIZE-1,0))<=0)
		{
			perror("recv() failed");
			exit(1);
		}
		totalBytesRcv+=bytesRcv;
		echoBuffer[bytesRcv]='\0';
		printf(echoBuffer);
	}
	printf("\n");*/
	close(sock);
	exit(0);
}

	
