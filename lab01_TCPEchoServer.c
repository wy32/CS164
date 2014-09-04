#include <stdio.h>		/*for printf() and fprintf()*/
#include <sys/socket.h>		/*for socket(), bind() and connect()*/
#include <arpa/inet.h>		/*for sockaddr_in() and inet_ntoa()*/
#include <stdlib.h>		/*for atoi()*/
#include <string.h>		/*for memset()*/
#include <unistd.h>		/*for close()*/

#define MAXPENDING 5   /*the maximum number of connetion requests*/
#define RCVBUFFERSIZE 32

int main(int argc,char *argv[])
{

	int servSock;			/*the socket descriptor for the server socket*/
	int clntSock;			/*the socket descriptor for the client socket*/
	struct sockaddr_in echoServAddr;/*Local - Server address*/
	struct sockaddr_in echoClntAddr;/*Client Address*/
	int echoServPort;		/*Server Port*/
	int clntLen;			/*Length of Client address data structure*/
	char echoBuffer[RCVBUFFERSIZE];
	char sendBuf1[RCVBUFFERSIZE];
	char sumBuf[32];
	int recvMsgSize;
	int num1;
	int num2;
	int sumrcv;
	int sum;

	if(argc!=2)
	{
		fprintf(stderr,"Usage : %s <Server port>\n",argv[0]);
		exit(1);
	}

	echoServPort=atoi(argv[1]);

	if((servSock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
	{
		perror("socket() failed");
		exit(1);
	}
	
	/*construct the local address for the server*/
	memset(&echoServAddr,0,sizeof(echoServAddr));
	echoServAddr.sin_family=AF_INET;
	echoServAddr.sin_port=htons(echoServPort);
	echoServAddr.sin_addr.s_addr=htonl(INADDR_ANY);

	/*bind the server socket*/
	if(bind(servSock,(struct sockaddr *)&echoServAddr,sizeof(echoServAddr))<0)
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
		
		char *pch;
  		//printf ("Splitting string \"%s\" into tokens:\n",echoBuffer);
  		pch = strtok (echoBuffer,"-");
  			
		num1 = atoi(pch);
		printf ("num1: %i\n",num1);
			
		pch = strtok (NULL, "-");
		num2 = atoi(pch);
		printf ("num2: %i\n",num2);

		pch = strtok (NULL, "-");
		sumrcv = atoi(pch);
		printf ("sum rcv: %i\n",sumrcv);
			
		sum = num1+num2;

		if(sumrcv != sum)
		{
			sprintf(sumBuf,"%d",sum);
			strcpy(sendBuf1,"NOT CORRECT-Answer: ");
			strcat(sendBuf1,sumBuf);
		}
		else
			strcpy(sendBuf1,"CORRECT");

		if(send(clntSock,sendBuf1,RCVBUFFERSIZE,0)!=RCVBUFFERSIZE)
		{
			perror("send() failed");
			exit(1);
		}
		
		close(clntSock);
	}
}

	
	
