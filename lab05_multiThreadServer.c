#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h>

#define NUMCLI 4

int guessArray[100];
int guessIndex = 0;
int sockArr[100];
int sockArrIndex = 0;
int randNum = 0;

void *process(void *ptr)
{

   int sock = *(int*)ptr;
   int read_size; 
   char client_message[2000];
   char msg[1024];
   
   //Receive a message from client
   while(1)
   {
   	if((read_size = recv(sock , client_message , 2000 , 0)) > 0)
   	{
   		printf("Received: %d\n",atoi(client_message));
   	 	guessArray[guessIndex] = atoi(client_message);
   	 	sockArr[sockArrIndex] = sock;
   	 	guessIndex++;
   	 	sockArrIndex++;
   	 	break;
   	}
   }
    
   if(read_size == 0)
   {
       printf("Client disconnected\n");
   }
   else if(read_size == -1)
   {
       perror("recv failed\n");
   }
   if(guessIndex == NUMCLI)
   {
  	int i;   	 	
   	int closest;
   	int winner = 0;
   	int WinnerArray[NUMCLI];
   	
   	for(i = 0; i < NUMCLI; i++)
   	{
   		WinnerArray[i] = abs(randNum - guessArray[i]);
   	}

   	closest = WinnerArray[0];
   	
	for(i = 1; i < NUMCLI; i++)
   	{
   		if(WinnerArray[i] < closest)
   	 	{
   	 		winner = i;
   	 		closest = WinnerArray[i];
   	 	}
   	}
   	for(i = 0; i < NUMCLI; i++)
   	{
   	 	if(i == winner)
   	 	{
			strcpy(msg, "You Won\n");
   	 		write(sockArr[i],msg,strlen(msg));
   	 	}
   	 	else
   	 	{
			strcpy(msg, "You Lost\n");
   	 		write(sockArr[i],msg,strlen(msg));
   		}
   	}
   	for(i = 0; i < NUMCLI; i++)
   	{
   	 	close(sockArr[i]);
		guessIndex = 0;
		sockArrIndex = 0;
   	}
	//close(sock);
	//exit(0);
   }
    
   return 0;
}


int main(int argc , char *argv[])
{
	int ptr , client_sock , c , *new_sock;
   	struct sockaddr_in server , client;
 
	if(argc != 2)
	{
		printf("Usage: server [port_num]\n");
		exit(1);
	}
  
   	srand(time(0));
   	randNum = rand() % 100 + 1;
   	printf("Random number: %d%s", randNum,"\n");
   
    
   	//Create socket
   	ptr = socket(AF_INET , SOCK_STREAM , 0);
   	if (ptr == -1)
   	{
       		printf("Could not create socket");
   	}
   
	server.sin_family = AF_INET;
   	server.sin_addr.s_addr = INADDR_ANY;
   	server.sin_port = htons( atoi(argv[1]) );
    
   	if( bind(ptr,(struct sockaddr *)&server , sizeof(server)) < 0)
   	{
       		//print the error message
       		perror("bind failed. Error");
       		return 1;
   	}
   
   	listen(ptr , 3); 
    
   	//Accept and incoming connection
   	printf("Waiting for incoming connections...\n");
   	c = sizeof(struct sockaddr_in);
   	while(1)
   	{
   		client_sock = accept(ptr, (struct sockaddr *)&client, (socklen_t*)&c); 
       		printf("Connection accepted\n");
        
       		pthread_t pthread;
       		new_sock = malloc(1);
       		*new_sock = client_sock;
        
       		if( pthread_create( &pthread , NULL ,  process , (void*) new_sock) < 0)
       		{
           		perror("could not create thread\n");
           		return 1;
       		}
   	}
    
   	if (client_sock < 0)
   	{
       		perror("accept failed\n");
       		return 1;
   	}
	exit(0);
    	close(ptr);
   	return 0;
}
