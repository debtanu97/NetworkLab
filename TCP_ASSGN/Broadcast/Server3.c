#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include<unistd.h>
#define MAX_CL 10

int client_st[MAX_CL];
int to_all = -1;

/*Details of the Client Side Required to send and receive messages*/
struct client_rec
{
	int sock_no; // Socket Number
	struct sockaddr_in claddr; // Address of the client 
	char name[20]; // Name of the client
	char pname[20];
	int sender; // id of the client who is currently sending messages
    pthread_t rd; // thread to handle a client
    int addrlen;
} clients[MAX_CL]; 


/*Server side function to send and receive messages*/
void *read_cl(void *ptr)
{
    char str_recv[1024], buffer[1024],str[1024],str1[1024];
    int temp = *((int *)ptr);
    int sender = temp;

  	/*Acknowledgement of a new client joining the Chat Room*/  
    strcpy(str,"---------->");
    strcat(str,clients[sender].name);
    strcat(str," Joined Just Now ......\n");

    write_all(sender,str);
    printf("%s \n", str);
    bzero(str,1024);

    while(1)
    {
        read(clients[sender].sock_no, buffer, 1023);

        if(strstr(buffer,"BYE") != NULL)
        {
            client_st[sender] = 0;
            break;
        }

        strcpy(str_recv,clients[sender].name);
        strcat(str_recv,"--->");
        strcat(str_recv, buffer);

        write_all(sender, str_recv);
        printf("%s ", str_recv);
        bzero(buffer,1024);
    }

        strcpy(str,"-----------> ");
        strcat(str,clients[sender].name);
        strcat(str,"Left.....");

        write_all(to_all,str);
        printf("%s \n", str);
        
        return;
}


/*Write to all clients except from whom the message came from*/
void write_all(int id, char to_send[1024])
{
	for(int i=0;i<MAX_CL;i++)
    {
        if(id == i || client_st[i] == 0)
        {
            continue;
        }
        else
        {
            write(clients[i].sock_no, to_send, strlen(to_send));
        }
    }
return;
}

int main()
{

    char str_send[100], name[20];
    int listen_fd, comm_fd[MAX_CL],n,flag=0,end;

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

     /*Listening for Clients*/
     listen(listen_fd, 10);
     printf("Listening for Clients :\n");
    
     for(int i=0;i<MAX_CL;i++)
     {
            client_st[i] = 0;
     }

     
    /*to get the result for port number from the client */
    int m;
 	
	for(int i=0;i<MAX_CL;i++)
        {
            struct client_rec temp;
            n=sizeof(clients[i].addrlen);
            m = sizeof(temp.addrlen);
            clients[i].sock_no = accept(listen_fd, (struct sockaddr *)&clients[i].claddr,&n);
            
            client_st[i] = 1;
            clients[i].sender = i;

            bzero(clients[i].name, 20);
            read(clients[i].sock_no, clients[i].name, 19);

            end = strlen(clients[i].name);
            clients[i].name[end-1] = '\0';

            printf("\nportnumber of the connected socket is: %d \n", getpeername(clients[i].sock_no, (struct sockaddr *)&temp.claddr, &n ));
            printf("%d \n", temp.claddr.sin_port);

            pthread_create(&clients[i].rd,NULL,read_cl,&clients[i].sender);
        } 

    for(int i=0;i<MAX_CL;i++)
    {
        pthread_join(clients[i].rd,NULL);
    }     

    shutdown(listen_fd,2); // shutdown(sockno, how) ---> how == 2 >> stop both reception and transmission from socket
    printf("\n Server is Shutting Down \n");

    return 0;
}
