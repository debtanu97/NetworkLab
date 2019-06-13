/*
    TFTP client
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFLEN 516	//Max length of buffer
#define PORT 69		//tftp port number	
#define MAXRETR 5	//Max number of re tries

static const char* MODE="octet";

#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERR 5


/*Upload File to server*/
void write_file(int sockfd, char *filename, struct sockaddr_in serv_addr, int slen)
{
	FILE *fp;
	fp = fopen(filename, "r");
	int filename_len = strlen(filename);
	if(fp==NULL)
	{
		perror("Error opening file.");
		exit(1);
	}

	// first send a WRQ to port 69
	char message[516]  , buf[516];
	bzero(message, 516);
	
	/*OPCODE WRQ*/
	message[0]=0;
	message[1]=WRQ;
	
	/*Filename*/
	strcpy(message+2, filename);
	
	/*Mode - "Octet"*/
	strcpy(message+2+filename_len+1 , MODE);
	
	int req_len=2+strlen(filename)+1+strlen(MODE)+1;

	if (sendto(sockfd, message, req_len , 0 , (struct sockaddr *) &serv_addr, slen)==-1)
    {
        perror("sendto()");
    	exit(0);
    }
    printf("Write Request Sent\n");
  
	int n;
    int blocknum=0;
    struct sockaddr_in reply_addr ;
    int addrlen= sizeof(reply_addr);

    int i;
    int ackblock;
    while(1)
    {
    	for(i=1;i<=MAXRETR;i++)
    	{
    		bzero(buf,BUFLEN);
		    if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr *) &reply_addr, &addrlen) == -1)
		    {
		        perror("recvfrom()");
		    	exit(0);
		    }
		    ackblock = (buf[2]<<8) + buf[3];

		    if((buf[1]==ERR) || (ackblock==(blocknum-1)))
		    {
		    	printf("Error sending blocknum, trying  again.%d\n", blocknum);
		    	if (sendto(sockfd, message, BUFLEN , 0 , (struct sockaddr *) &serv_addr, slen)==-1)
			    {
			        perror("sendto()");
			    	exit(1);
			    }

		    }
		    else
		    	break;
    	}
    	
    	if(i>MAXRETR)
    		{
    			printf("Giving up on sending file. :( \n" );
    			return ;
    		}
    	

	    printf("ACK received for block number %d.\n", blocknum);
	    fflush(stdout);

	    blocknum++;
    	bzero(message, BUFLEN);
    	
    	/*OPCODE*/
    	message[1]=DATA;
    	
    	/*Block Number*/
    	message[2]=blocknum>>8;
		message[3]=blocknum%(256);

    	/*Read 512 bytes into memory and send*/
    	n = fread(message+4 , 1 , 512 , fp);

    	printf("Sending block %d of %d bytes.\n", blocknum,n);
    	if (sendto(sockfd, message, n+4 , 0 , (struct sockaddr *) &reply_addr, addrlen)==-1)
	    {
	        perror("sendto()");
	    	exit(1);	
	    }
	    
    	/*Last Block of message*/
    	if(n<512)
    		break;
    }
    
    fclose(fp);
    printf("Transfer complete.\n");


}

void read_file(int sockfd, char *filename, struct sockaddr_in serv_addr, int slen)
{
	FILE *fp;
	fp = fopen(filename, "w");
	int filename_len = strlen(filename);

	/// first send a RRQ to port 69
	char message[516]  , buf[516];
	bzero(message, BUFLEN);
	
	/*OPCODE RRQ*/
	message[0]=0;
	message[1]=RRQ; 
	
	/*File name*/
	strcpy(message+2, filename);
	
	/*Mode = "Octet"*/
	strcpy(message+2+filename_len+1 , MODE);

	//send RRQ
	if (sendto(sockfd, message, 516 , 0 , (struct sockaddr *) &serv_addr, slen)==-1)
    {
            perror("sendto()");
    }
    printf("Sent RRQ.\n");


    int n;
    int blocknum=1;
    struct sockaddr_in reply_addr ;
    int addrlen= sizeof(reply_addr);

    while(1)
    {
    	addrlen= sizeof(reply_addr);
    	bzero(buf,516);
    	n = recvfrom(sockfd, buf, 516, 0, (struct sockaddr *) &reply_addr, &addrlen);
    	
    	if (n == -1)
	    {
	        perror("recvfrom()");
	    	exit(1);
	    }
	    n -= 4;
	    if(buf[1]==ERR)
	    	perror("Server transfer failure");
	     	
	  

	    fwrite(&buf[4],1,n,fp);
	    printf("Received block of size n = %d\n", n);

	    bzero(message, BUFLEN);
		message[0]=0;
		message[1]=ACK;
		message[2]=blocknum>>8;
		message[3]=blocknum%(256);

		if (sendto(sockfd, message, 4 , 0 , (struct sockaddr *) &reply_addr, addrlen)==-1)
	    {
	            perror("sendto()");
	    		exit(1);
	    }    	
    	printf("Sent ACK for block %d.\n", blocknum);
		
		blocknum++;
	    if(n<512)
	    {
	    	break;
	    }

    }

    fclose(fp);
    printf("Transfer complete.\n");

}
int main(int argc , char *argv[])
{
	int sockfd,slen;
	struct sockaddr_in serv_addr;
	char addr[100];
	
	if(argc == 1)
		strcpy(addr,"127.0.0.1");
	else
		strcpy(addr,argv[1]);
    
	int  i;
	slen=sizeof(serv_addr);
    
    sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("socket");
    	exit(1);
    }
    
	memset((void*)&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr(addr);    


    /*Timeout value for socket*/
    struct timeval timeout;      
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    /*Receive Time out*/
    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
        perror("setsockopt failed\n");

    /*Send Time out*/
    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
        perror("setsockopt failed\n");


	char operation[100] , filename[200];
	
	while(1)
	{
		printf("tftp_cl > ");
		scanf("%s",operation);
		if(strcmp(operation,"quit")==0)
		{
			break;
		}
		
		else if(strcmp(operation,"put")==0)
		{
			scanf("%s",filename);
			write_file(sockfd, filename, serv_addr, slen);
		}

		else if(strcmp(operation,"get")==0)
		{
			scanf("%s",filename);
			read_file(sockfd, filename, serv_addr, slen);
		}
		else
		{
			fprintf(stderr, "\nUnrecognized command");
			continue;
		}

	}
}