#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>


void *write_to_cl(void *ptr)
{
	char str_send[100];
	int comm_fd;

	comm_fd = *((int *)ptr);

	while(1)
	{
        fgets(str_send,100,stdin);
        write(comm_fd, str_send, strlen(str_send)+1);
        if(strstr(str_send, "BYE") !=NULL)
        {
            break;
        }
	}

    return;
}

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


     /*Thread to send messages to client*/
     pthread_t wt;
     pthread_create(&wt,NULL,write_to_cl,(void *)&comm_fd);


    do
    {

        bzero( str_recv, 100);
	read(comm_fd,str_recv,100);

	/*Receiving from Client*/
	printf("Received from Client: %s \n",str_recv);


	if(strstr(str_recv,"BYE")!=NULL)
	  {
		break;
	  }




    } while(1);

	//pthread_join(wt,NULL);


return 0;
}
