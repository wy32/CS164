#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECHOMAX 255

int main(int argc,char *argv[])
{
	int sock;
	int servPort;
	struct sockaddr_in ServAddr;
	struct sockaddr_in ClntAddr;
	int sizeClntAddr;
	struct data {
                int num1;
                int num2;
                int sum;
        } mydata;
	struct dataBack {
                int num1;
                int num2;
                int sum;
                int ack;
        } msgS;
        void *buffer=(void *) &mydata;
	int rBytes,rv;

	if(argc!=2)
	{
		fprintf(stderr,"%s wasn't run properly\n");
		exit(1);
	}
	
	servPort=atoi(argv[1]);

	if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))<=0)
	{
		perror("socket() failed");
		exit(1);
	}
	
	memset(&ServAddr,0,sizeof(ServAddr));
	ServAddr.sin_family=AF_INET;
	ServAddr.sin_port=htons(servPort);
	ServAddr.sin_addr.s_addr=htons(INADDR_ANY);
	
	if(bind(sock,(struct sockaddr *)&ServAddr,sizeof(ServAddr))<0)
	{
		perror("bind() failed");
		exit(1);
	}
	
	for(;;)
	{
		sizeClntAddr=sizeof(ClntAddr);

		/*recvfrom() and sendto()*/
		for(rBytes=0;rBytes<sizeof(mydata);rBytes+=rv)
        	{
                	rv=recvfrom(sock,buffer+rBytes,sizeof(mydata)-rBytes,0,(struct sockaddr *)&ClntAddr,&sizeClntAddr);
        	}
		printf("Handling client: %s\n",inet_ntoa(ClntAddr.sin_addr));
		if (mydata.sum==(mydata.num1+mydata.num2))
			msgS.ack=1;
		else
			msgS.ack=0;
		msgS.num1=mydata.num1;
		msgS.num2=mydata.num2;
		msgS.sum=msgS.num1+msgS.num2;

		if(sendto(sock,&msgS,sizeof(msgS),0,(struct sockaddr *)&ClntAddr,sizeof(ClntAddr))!=sizeof(msgS))
		{
			perror("sendto() failed");
			exit(1);
		}
		
	}
} 	
