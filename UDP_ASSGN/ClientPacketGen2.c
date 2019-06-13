#include<stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#define C 4294967296


int main(int argc , char *argv[])
{
    int client, p, ttl, num_pac, port, seq, time_to_live, rtt, avg_rtt, num_p_recv,st;
    struct timeval t0, t1, t2;
    unsigned long long int t_ref, ts,t;
    unsigned int ts1,ts2;
    FILE *fp;
    fp = fopen(argv[6],"a");
    
    /* atoi(*ch) converts the command line arguments(strings) to integers*/
    
    port = atoi(argv[2]);             //port number (should be equal to the server's port number)
    p = atoi(argv[3]);                //size of payload (between 100 to 1300 bytes)
    time_to_live = atoi(argv[4]);     //time to live (between 2 to 20)
    num_pac = atoi(argv[5]);          //number of packets to be sent (between 1 to 50 packets)
    
    /*Create UDP socket*/
    client = socket(AF_INET, SOCK_DGRAM, 0);
    
    /*Configure settings in address struct*/
    struct sockaddr_in server;
    socklen_t addr_size;
    
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    addr_size = sizeof(server);

    avg_rtt = 0;                                    //set initial value of Average Round Trip Time to 0
    num_p_recv = 0;                                  //set initial value of number of packets received to 0
    
    gettimeofday(&t0, NULL);                        //reference time - all timestamp values will be calculated with reference to this time
    t_ref = (t0.tv_sec * 1000000 + t0.tv_usec);     //convert into microseconds
    
    /*TO PACK*/
    unsigned short sno;
    unsigned int tsp;
    unsigned char ttli = (unsigned char)time_to_live;
    int offset;

    for(int j = 0; j < num_pac; j++)
    {
    
        unsigned char data_pac[p + 7];                         

        /*Packing SeqNumber*/
        sno = (j+1);
        memcpy(&data_pac[0], (unsigned char*)&sno, sizeof(unsigned short));
                
        data_pac[6] = ttli;

        /*Storing the PAYLOAD - Assume that the Payload consists of P-1 'D's followed by a NULL*/
        for(int i = 0; i <p; i++)
            data_pac[i + 7] = 'D';

        /*Storing the TIMESTAMP*/                 
        gettimeofday(&t1, NULL);                      //get current time
        t = (t1.tv_sec * 1000000 + t1.tv_usec);       //convert it into microseconds
        tsp = t%C;
        memcpy(&data_pac[2], (unsigned char*)&tsp, 4);
        
       
        /*Keep doing till ttl is not zero*/
        while(data_pac[6] !=0)   
        {    /*Send to the Server*/
            sendto(client,data_pac,(p+7), 0, (struct sockaddr *)&server, addr_size);

            /*Clear Data Packet after sending the Data*/
            bzero(data_pac, p+7);

            /*Recieve from the Server*/
            st = recvfrom(client,data_pac,(p+7), 0, (struct sockaddr *)&server, &addr_size);
            data_pac[6]--;
        }   
            
        memcpy(&ts1 ,(unsigned int*)&data_pac[2],4); // Unpack the Timestamp value (Original)
        
        gettimeofday(&t2, NULL);                      //get current time
        t = (t2.tv_sec * 1000000 + t2.tv_usec);       //convert into microseconds
        ts2 = t%C;                              

        /*Calculate Round Trip Time RTT as current timestamp - packet times*/
        rtt = (ts2 - ts1);

        /*Update values of average round trip time and number of packets successfully recieved*/
        num_p_recv++;
        avg_rtt = avg_rtt + rtt;
    }

    avg_rtt = avg_rtt/num_p_recv;
    printf("\nAverage Round Trip Time (RTT)              : %d \n ",avg_rtt); 
    
    fprintf(fp,"%d,%d\n",p,avg_rtt);



    return 0;
}