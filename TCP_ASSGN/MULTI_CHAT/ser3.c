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
#define MAX_CL 5

int client_st[MAX_CL];
int to_all = -1;

/*Details of the Client Side Required to send and receive messages*/
struct client_rec
{
    int sock_no;                // Socket Number
    struct sockaddr_in claddr;  // Address of the client 
    char name[20];              // Name of the client
    int sender;                 // id of the client who is currently sending messages
    pthread_t rd;               // thread to handle a client
    int addrlen;
} clients[MAX_CL];

/*Write to all clients except from whom the message came from*/
void write_to_all(int id, char to_send[1024])
{
    for(int i=0;i<MAX_CL;i++)
    {
        if(id == i || client_st[i] == 0){
            continue;
        }
        else{
            write(clients[i].sock_no, to_send, strlen(to_send));
        }
    }
    return;
}

/*Server side function to send and receive messages*/

void *conn_handler(void *ptr)
{
    char data_from_client[1024], buffer[256],str[1024];
    int temp = *((int *)ptr);
    int sender = temp;

    //New Client Connected to Server  
    strcpy(str,clients[sender].name);
    strcat(str," Joined");

    //write_to_all(sender,str);
    printf("%s \n", str);
    bzero(str,1024);

    while(1)
    {
        read(clients[sender].sock_no, buffer, sizeof(buffer));

        if(strcmp(buffer,"BYE") == 0)
        {
            //if client sends BYE then close connection and set client status to 0 or 'Not Connected to Server'
            client_st[sender] = 0;
            break;
        }


        /**********************************************************************************************************************
        Here, we assume that any client (say C1), that wishes to communicate with another client (say C2), will first input the 
        name of the desired recepient, in this case C2, followed by the desired message (ABC DEF). Thus the message sent by the 
        sender, C1 would be "C2 : ABC DEF". When this message reaches the server, the server should first identify the recepient 
        and then forward the message to the recepient.
        In the following few lines of code, the server first stores the recepient's identity in the variable 'name_to_send' and
        the message in the string msg_to_send. Once this is done, the serverthen checks its list of connected clients for the 
        recepient and when it finds the desired client, it sends the message.
        **********************************************************************************************************************/

        int i, j, k;
        for(i = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++);
        

        char name_to_send[20];
        char msg_to_send[256];      
        
        for(j = 0 ; j < i; j++)
            name_to_send[j] = buffer[j];
            name_to_send[j] = '\0';
        
        for(j = i+1, k = 0; buffer[j] != '\0'; j++, k++)
            msg_to_send[k] = buffer[j];
            msg_to_send[k] = '\0';

        strcpy(data_from_client,clients[sender].name);
        strcat(data_from_client,">>>>>>>>");
        strcat(data_from_client, msg_to_send);
        
        int f = 0;
        for(i = 0; i < MAX_CL; i++)
        {
            if(strcmp(name_to_send, clients[i].name) == 0 && client_st[i]==1){
                write(clients[i].sock_no, data_from_client, sizeof(data_from_client));
                f = 1;
                break;
            }
        }
        if(f == 0)
        {
            char error[256];
            strcpy(error, "\nThe desired recepient could not be found. Please check the name of the client!!\n");
            write(clients[sender].sock_no, error, sizeof(error));
        }
        
        bzero(buffer,1024);
    }

    strcpy(str,clients[sender].name);
    strcat(str," Left");

    //write_to_all(to_all,str);
    printf("%s \n", str);
    return NULL;
}



int main()
{
    //create server socket
    int server;
    server = socket(AF_INET, SOCK_STREAM, 0);
    

    //define the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    //bind the socket to the specified IP and port
    bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr));

    //listen on the socket for connections.
    listen(server, 5);
    printf("Waiting for client to connect...\n");

    
    //set client status to 0 or 'not connected to server' for all clients
    for(int i = 0; i < MAX_CL; i++)
    {
        client_st[i] = 0;
    }

    for(int i = 0; i < MAX_CL; i++)
    {
        int n = sizeof(clients[i].addrlen);
        clients[i].sock_no = accept(server, (struct sockaddr *)&clients[i].claddr, (socklen_t *)&n);
        
        //set client status to 1 or 'connected to server' when a client connects to the server
        client_st[i] = 1;

        //set the sender id of the current client to i
        clients[i].sender = i;

        char name[20];

        //recieve the name of the connected client from the client
        read(clients[i].sock_no, &clients[i].name, sizeof(clients[i].name));

        //create a thread to handle the client
        pthread_create(&clients[i].rd, NULL, conn_handler, &clients[i].sender);
        
    }

    close(server); 
    printf("\n Server is Shutting Down \n");

    return 0;
}
