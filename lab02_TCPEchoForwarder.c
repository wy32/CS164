#include <stdio.h>		/*for printf() and fprintf()*/
#include <sys/socket.h>		/*for socket(), bind() and connect()*/
#include <arpa/inet.h>		/*for sockaddr_in() and inet_ntoa()*/
#include <stdlib.h>		/*for atoi()*/
#include <string.h>		/*for memset()*/
#include <unistd.h>		/*for close()*/
#include <netdb.h>

#define MAXPENDING 5   /*the maximum number of connetion requests*/
#define RCVBUFFERSIZE 32

int main(int argc,char *argv[])
{

	int servSock;			/*the socket descriptor for the server socket*/
	int clntSock;			/*the socket descriptor for the client socket*/
	struct sockaddr_in echoForAddr;/*Local - Server address*/
	struct sockaddr_in echoClntAddr;/*Client Address*/
	int echoForPort;		/*Server Port*/
	int clntLen;			/*Length of Client address data structure*/
	char echoBuffer[RCVBUFFERSIZE];
	char sendBuf1[RCVBUFFERSIZE];
	//char sumBuf[32];
	int recvMsgSize;
	
	int sock;
	struct sockaddr_in echoServAddr;
	int echoStringLen;
	int echoServPort;
	char *servIP;
	struct hostent *hp;
	struct in_addr ip_addr;
	char* serName;
	char* serPort;
	char* serMsg;

	//int num1;
	//int num2;
	//int sumrcv;
	//int sum;

	if(argc!=2)
	{
		fprintf(stderr,"Usage : %s <Server port>\n",argv[0]);
		exit(1);
	}

	echoForPort=atoi(argv[1]);

	if((servSock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
	{
		perror("socket() failed");
		exit(1);
	}
	
	/*construct the local address for the server*/
	memset(&echoForAddr,0,sizeof(echoForAddr));
	echoForAddr.sin_family=AF_INET;
	echoForAddr.sin_port=htons(echoForPort);
	echoForAddr.sin_addr.s_addr=htonl(INADDR_ANY);

	/*bind the server socket*/
	if(bind(servSock,(struct sockaddr *)&echoForAddr,sizeof(echoForAddr))<0)
	{
		perror("bind() failed");
		exit(1);
	}
	
	/*make the server to listen for incoming connections*/
	if(listen(servSock,MAXPENDING)<0)
	{
		perror("listen() failed");
		exit(1);
	}
	
	for(;;)
	{
		clntLen=sizeof(echoClntAddr);
		if((clntSock=accept(servSock,(struct sockaddr *)&echoClntAddr,&clntLen))<0)
		{
			perror("accept() failed");
			exit(1);
		}
		printf("Handling client: %s\n",inet_ntoa(echoClntAddr.sin_addr));
		/*recv() and send()*/
		if((recvMsgSize=recv(clntSock,echoBuffer,RCVBUFFERSIZE,0))<0)
		{
			perror("recv() failed");
			exit(1);
		}
		
		//while(recvMsgSize>0)   //zero indicated the end of transmission
		//{
			
  		char *pch;
  			//printf ("Splitting string \"%s\" into tokens:\n",echoBuffer);
  		pch = strtok (echoBuffer," ");
		serName = pch;

		pch = strtok (NULL, " ");
		serPort = pch;
		echoServPort = atoi(serPort);

		pch = strtok (NULL, " ");
		serMsg = pch;
		
		printf("Received message: %s %s %s\n",serName,serPort,serMsg);

		hp = gethostbyname(serName);
		ip_addr = *(struct in_addr *)(hp->h_addr);
		printf("Hostname: %s, was resolved to: %s\n", argv[3],inet_ntoa(ip_addr));
		servIP = inet_ntoa(ip_addr);
		//servIP = atoi(serPort);

		/* Creation of the socket*/
		if((sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			perror("socket() failed");
			exit(1);
		}

		/*Configure the server*/
		memset(&echoServAddr,0,sizeof(echoServAddr));
		echoServAddr.sin_family=AF_INET;
		echoServAddr.sin_port=htons(echoServPort);
		echoServAddr.sin_addr.s_addr=inet_addr(servIP);
	
		/*Establish the connection*/
		if(connect(sock,(struct sockaddr *)&echoServAddr,sizeof(echoServAddr))<0)
		{
			perror("connect() failed");
			exit(1);
		}

		/*Sending of the data*/
		char echoString[RCVBUFFERSIZE];
		strcpy(echoString,serMsg);
		echoStringLen=strlen(echoString);

		//printf(echoString);
		if(send(sock,echoString,echoStringLen,0)!=echoStringLen)
		{
			perror("send() failed");
			exit(1);
		}
			//strcpy(sendBuf1,serName);
			//strcat(sendBuf1," ");
			//strcat(sendBuf1,serPort);
			//strcat(sendBuf1," ");
			//strcat(sendBuf1,serMsg);

			//if(send(clntSock,sendBuf1,RCVBUFFERSIZE,0)!=RCVBUFFERSIZE)
			//{
			//	perror("send() failed");
			//	exit(1);
			//}
			/*if((recvMsgSize=recv(clntSock,echoBuffer,RCVBUFFERSIZE,0))<0)
			{
				perror("recv() failed");
				exit(1);
			}*/
		//}
		close(clntSock);
	}
}

	
	
