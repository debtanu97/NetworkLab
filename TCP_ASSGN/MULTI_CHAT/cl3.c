#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 9001

void *receive(void *cl)
{
	char data_from_server[256];
	int *client = (int *)cl;

	while(1)
	{
		/*Receive data sent by the server*/
        bzero(data_from_server, 256);
		read(*client, &data_from_server, sizeof(data_from_server));
        printf("%s\n", data_from_server);
	}

}

int main()
{
    //create a socket
    int client;
    client = socket(AF_INET, SOCK_STREAM, 0);
    

    //specify an address for the socket
    struct sockaddr_in server_addr;    
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //connect to the server
	int connection = connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr));
	
    printf("Connection established.\n");

    char name[20];
    printf("Enter your name to join chat:");
    scanf(" %[^\n]s",name);
    write(client,name,strlen(name));
    printf("Welcome to New Chat : %s\n", name);

    int n;
    char data_to_server[256];
    
    //create thread to receive from server
    pthread_t wt;
    pthread_create(&wt, NULL, receive, (void*)&client);
    

    //to send to server
    while(1)
    {
		//clear the buffer
		bzero(data_to_server, 256);

		scanf(" %[^\n]s", data_to_server);

		//send to socket	
		write(client, &data_to_server, sizeof(data_to_server));
	

		if(strcmp(data_to_server,"BYE") == 0)
		{
			break;
		}

	}
	return 0;
}