
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>


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
        printf("Received from Server: %s",recvline);
		if(strstr(recvline,"BYE") != NULL)
		{
			printf("\nQUITTING\n");
			break;	
		}
	}

	return;
}

int main(int argc,char **argv)
{
    int sockfd,n;
    char sendline[100];
    char recvline[100];
    struct sockaddr_in servaddr;

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22000);

    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    //printf("To go into Thread \n");
    pthread_t wt;
    pthread_create(&wt, NULL, recvserv,(void*)&sockfd);
    do
    {

	/*Clear Array before sending/receiving new data*/
	bzero( sendline, 100);
       // bzero( recvline, 100);

	//printf("Input text to be sent to server:\n ");

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
		printf("\nQUITTING\n");
		break;
	}

	}while(1);
	printf("\n out of main\n");
	//pthread_join(wt,NULL);

return 0;
}
