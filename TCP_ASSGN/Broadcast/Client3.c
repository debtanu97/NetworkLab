
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include<unistd.h>


void *recvserv(void *ptr)
{
	int sockfd;
	char recvline[100];
	sockfd = *((int *)ptr);

	//printf("reached thread to receive");

	while(1)
	{
		bzero( recvline, 100);
		read(sockfd,recvline,100);
        printf("%s \n",recvline);
	}

}

int main(int argc,char **argv)
{
    int sockfd,n;
    char sendline[100];
    char recvline[100], name[20], pname[20];
    struct sockaddr_in servaddr;

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    
	servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22000);
    //servaddr.sin_addr.s_addr = htons("127.0.0.1");

    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	printf("Connected To Server \n");
    
    printf("Enter your name to join chat \n:");
    fgets(name,19,stdin);
    write(sockfd,name,strlen(name));
    printf("Welcome to New Chat : %s", name);

   /* printf("Enter the person with whom you want to chat:");
    fgets(pname,19,stdin);
    write(sockfd,pname,strlen(pname)); */

    //printf("To go into Thread \n");
    pthread_t wt;
    pthread_create(&wt, NULL, recvserv,(void*)&sockfd);
    do
    {

	/*Clear Array before sending/receiving new data*/
	bzero( sendline, 100);
       

	//printf("Input text to be sent to server:\n ");

	//scanf("%s", sendline);
	fgets(sendline,100,stdin);

	//printf("after fgets\n");

	/*writing to socket*/
	//printf("Sending to Server: %s/n",sendline);
	
	write(sockfd,sendline,strlen(sendline)+1);
	/*reading from socket*/
	//read(sockfd,recvline,100);
        //printf("Received from Server: %s",recvline);

	if(strstr(sendline,"BYE") != NULL)
	{
		
		printf("\nGoing Offline\n");
		break;
	}

	}while(1);

	shutdown(sockfd,2); // Stop sending and receiving from server
	//printf("Connection Closed");
	

return 0;
}
