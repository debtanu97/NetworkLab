#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>


/*void *write_to_cl(void *ptr)
{
	char str_send[100];
	int comm_fd;

	comm_fd = *((int *)ptr);

	printf("Response to Client from Server:");
        fgets(str_send,100,stdin);

        write(comm_fd, str_send, strlen(str_send)+1);

}*/

int main()
{

    char str_recv[100], str_send[100];
    int listen_fd, comm_fd,n;

    struct sockaddr_in servaddr;
    struct sockaddr_in claddr;

    /*Socket definition Server side*/
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    /*Clearing space in servaddr*/
    bzero( &servaddr, sizeof(servaddr));

   /*Putting the values of the servaddr*/
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = htons(INADDR_ANY);
     servaddr.sin_port = htons(22000);


    /*Bind the server address, port and socket number */
     bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

     listen(listen_fd, 10);
     n = sizeof(claddr);
	
     comm_fd = accept(listen_fd, (struct sockaddr*)&claddr, &n);

    do
    {

        bzero( str_recv, 100);


	read(comm_fd,str_recv,100);

	printf("CONNECTED WITH CLIENT \n" );
	printf("Received from Client: %s",str_recv);


	printf("Response to Client from Server:");
	fgets(str_send,100,stdin); 

	write(comm_fd, str_send, strlen(str_send)+1);


    } while(1);




return 0;
}
