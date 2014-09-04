#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECHOMAX 255 /*the max number of the string to be echoed*/

int main(int argc,char *argv[])
{
	int sock;
	char *servIP;
	struct data {
		int num1;
		int num2;
		int sum;
	} mydata;
	int servPort;
	struct sockaddr_in UDPServ;
	struct sockaddr_in fromAddr;	/*the addres of the server that replies to the message send from the client*/
	int fromSize;			
	struct dataBack {
		int num1;
		int num2;
		int sum;
		int ack;
	} msgS;
	void *buffer=(void *) &msgS;
	int rBytes,rv;

	if((argc<5)||(argc>6))
	{
		fprintf(stderr,"Usage of %s is not proper",argv[0]);
		exit(1);
	}

	servIP=argv[1];
	mydata.num1=atoi(argv[2]);
	mydata.num2=atoi(argv[3]);
	mydata.sum=atoi(argv[4]);;

	if(argc==6)
		servPort=atoi(argv[5]);
	else
		servPort=7;

	/*creation of the UDP socket*/
	if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))<=0)
	{
		perror("socket() failed");
		exit(1);
	}

	/* it is a UDP socket so we don't need to connect it*/
	/*send data through this socket*/
	memset(&UDPServ,0,sizeof(UDPServ));
	UDPServ.sin_family=AF_INET;
	UDPServ.sin_port=htons(servPort);
	UDPServ.sin_addr.s_addr=inet_addr(servIP);
	if(sendto(sock,&mydata,sizeof(mydata),0,(struct sockaddr *)&UDPServ,sizeof(UDPServ))!=sizeof(mydata))
	{
		perror("sendto() failed");
		exit(1);
	}

	fromSize=sizeof(fromAddr);
	for(rBytes=0;rBytes<sizeof(msgS);rBytes+=rv)
        {
                rv=recvfrom(sock,buffer+rBytes,sizeof(msgS)-rBytes,0,(struct sockaddr *)&fromAddr,&fromSize);
        }

	if(UDPServ.sin_addr.s_addr!=fromAddr.sin_addr.s_addr)
	{
		perror("The server that replied was not the one that was supposed to do so");
		exit(1);
	}
	
	printf("The sum of the two numbers is %d",msgS.sum);
	if(msgS.ack==1)
		printf(" and it was computed correctly from the client\n");
	else
		printf(" and it was not computed correctly from the client\n");

	printf("\n");
	close(sock);
	exit(0);
}
			
			
