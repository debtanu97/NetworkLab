
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>

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

    do
    {

	/*Clear Array before sending/receiving new data*/
	bzero( sendline, 100);
        bzero( recvline, 100);

	printf("Input text to be sent to server: ");

	fgets(sendline,100,stdin);

	/*writing to socket*/
	write(sockfd,sendline,strlen(sendline)+1);
        printf("Sending to Server: %s",sendline);

	/*reading from socket*/
	read(sockfd,recvline,100);
        printf("Received from Server: %s",recvline);

	}while(1);

return 0;
}
