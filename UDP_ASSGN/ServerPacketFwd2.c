#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


int main(int argc , char *argv[])
{
    int server, size_p;
    char ttl;
    unsigned char data[1024],seq[2],tm[4];

    /*Create UDP Socket*/
    server = socket(PF_INET, SOCK_DGRAM, 0);                  
    
    /*Define Server Address*/
    struct sockaddr_in serverAddr;
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

    /*Used to identify the sender of the packets*/                                   
    struct sockaddr_storage serverStorage;                           
    socklen_t addr_size;
    addr_size = sizeof(serverStorage);

    /*Bind socket with address*/
    bind(server, (struct sockaddr *) &serverAddr, sizeof(serverAddr));    
    
    while(1)
    {
        fflush(stdin);
        
        /*Recieve message from client*/
        size_p = recvfrom(server, data,1024, 0, (struct sockaddr *)&serverStorage, &addr_size);  
        
        if(size_p < 0)
        {
            printf("Error Recieveing from Client");
            continue;
        }
       
        /*Decrement the TTL Value*/
        data[6] = data[6] - 1;                              
      
        printf("\nReceived\n");

        /*Send packet back to client*/
        sendto(server,data, size_p, 0, (struct sockaddr *)&serverStorage, addr_size);  
        /*Calculate Sequence Number of the Packet*/
        //printf("\nPacket with Sequence Number = %d recieved and TTL =  %d \n", seq, (int)(data[6] + 1));
    }

    return 0;
}