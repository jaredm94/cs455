#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "project1.h"

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 500

void DieWithError(char *err)
{

fprintf(stderr,"%s",err);

exit(0);

}

void nullTerminatedCmdS(int sock,char * arr, int bytesread)
{


int16_t len = strlen(arr);


char buff[500]= "Null Terminated: ";
int k = strlen(buff);

int16_t lenPack = 2 + len + k;

char buffer[500];

memcpy(buffer,&lenPack,2);
memcpy(buffer+2, &buff,k);
memcpy(buffer+2+k,arr,len);


send(sock,buffer,lenPack,0);

return;

}



void givenLengthcmdS(int sock,char * arr, int bytesread)
{

int16_t len;
memcpy(&len,arr,2);
len = ntohs(len);


char buff[500];
char * temp = "Given Length: " ;
int templen = strlen(temp);

int16_t total = templen + len+2;

memcpy(buff,&total,2);
memcpy(buff+2,temp,templen);
memcpy(buff+2+templen,arr+2,len);

send(sock,buff,total,0);

return;

}


void goodIntCmdS(int  sock, char * arr)
{

int j;
memcpy(&j,arr,4);
j = ntohl(j);
char * m = "Good Int: ";
char buf[500];
int16_t total  = strlen(m)+4+2;

memcpy(buf,&total,2);
memcpy(buf+2,m,strlen(m));
memcpy(buf+strlen(m)+2,&j,4);
send(sock,buf,total,0);

}

void BadIntCmdS(int  sock, char * arr)
{

int j;
memcpy(&j,arr,4);
j = ntohl(j);
char * m = "Bad Int: ";
char buf[500];
int16_t total  = strlen(m)+4+2;

memcpy(buf,&total,2);
memcpy(buf+2,m,strlen(m));
memcpy(buf+strlen(m)+2,&j,4);
send(sock,buf,total,0);

}


int bytesAtATimeCmdS(int sock, char * arr, int bytesread)
{
    int num_rcv = 0;
    int16_t size;

    int num;
    memcpy(&num,arr,4);
    num = ntohl(num);
    char buf[20];
    bytesread -= 5;
// subtract what weve already Read
if(bytesread >= num)
{

char * m = "Byte At A Time: ";
char buff[500];


memcpy(buff,m,strlen(m));
memcpy(buff+strlen(m),&num_rcv,4);

send(sock,buff,strlen(m)+4,0);

  return num_rcv;
}

  num -= bytesread;



int i = 0;

while( num > 0)
{
i=recv(sock,buf,1,0);
if(i <= 0)
{
DieWithError("Die in bytes!");
}

  num -= i;
  num_rcv++;
}

char * m = "Byte At A Time: ";
char buff[500];

size = strlen(m)+4 +2;

memcpy(buff,&size,2);
memcpy(buff+2,m,strlen(m));
memcpy(buff+strlen(m)+2,&num_rcv,4);


send(sock,buff,size,0);


return num_rcv;

}



int kbytesAtATimeCmdS(int sock, char * arr, int bytesread)
{
    int num_rcv = 0;


    int num;
    int16_t size;
    memcpy(&num,arr,4);
    num = ntohl(num);
    char buf[1000];
    bytesread -= 5;
// subtract what weve already Read

if(bytesread >= num)
{
  return  num_rcv;
}

    num -= bytesread;



int i = 0;

while(num > 0 && (i=recv(sock,buf,1000,0))!=0)
{
  num -= i;
  num_rcv++;
}

char * m = "KByte At A Time: ";
char buff[500];

size = strlen(m)+4 +2;

memcpy(buff,&size,2);
memcpy(buff+2,m,strlen(m));
memcpy(buff+strlen(m)+2,&num_rcv,4);


send(sock,buff,size,0);

return num_rcv;

}



int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in servAddr; /* Local address */
    struct sockaddr_in clntAddr; /* Client address */
    unsigned short servPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    int totalBytesRecvd = 0;
	char buffer[RCVBUFSIZE];
	FILE *log;
  printf("#3\n");
	//log  = fopen("log.txt", 'a'); // open log file for append


    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    servPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&servAddr, 0, sizeof(servAddr));   /* Zero out structure */
    servAddr.sin_family = AF_INET;                /* Internet address family */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    servAddr.sin_port = htons(servPort);      /* Local port */
printf("#2\n");
    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("bind() failed");



    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");
printf("#1\n");
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(clntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &clntAddr,&clntLen)) < 0)
			DieWithError("accept() failed");

	    /* clntSock is connected to a client! */
        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

		int read = 0; // hold bytes read on each call
		int recvCalls = 0;


			while(1)
			{

            int bytesRecvd = 0;
			char buf2[RCVBUFSIZE];

        read = recv(clntSock, buffer+bytesRecvd, 100, 0);
    if(read==0)
        {break;}

      //  printf("Read Now %d: %s",(int8_t)buffer[0],buffer);

				if(bytesRecvd == 0) // initial read
				{
					memset(buf2, 0, RCVBUFSIZE); // reset buf2 to 0
				}
				bytesRecvd += read;
				totalBytesRecvd += read;

			    switch((int8_t)buffer[0])
			    {
					case 1:		read = 0;
								nullTerminatedCmdS(clntSock, buffer+1, read);
								bytesRecvd = 0;									break; // nullTerminatedCmd
					case 2:		givenLengthcmdS(clntSock, buffer+1, read);
								bytesRecvd = 0;									break; // givenLengthCmd
					case 3:		BadIntCmdS(clntSock, buffer+1);
								bytesRecvd = 0;									break; // badIntCmd
					case 4:		goodIntCmdS(clntSock, buffer+1);
								bytesRecvd = 0;									break; // goodIntCmd
					case 5:		recvCalls++;
								bytesAtATimeCmdS(clntSock, buffer+1, read);		break; // bytesAtATimeCmd
					case 6:		recvCalls++;
								kbytesAtATimeCmdS(clntSock, buffer+1, read);		break; // KbyteAtATimeCmd
			    }
				//fwrite(buffer, sizeof(buffer[0]), sizeof(buffer)/sizeof(buffer[0]), log);

		}
	}
	close(servSock);
//	fclose(log);
	exit(0);
    /* NOT REACHED */
}
/*int serverByteAtATimeCmd(int sock, int numOps)
{
	char sendBuf[500];
	char line[500];
	int8_t netByteOrder = htons(numOps);         		// not sure if 'int8_t' is the proper type for network byte order

	sprintf(line, "byteAtATimeCmd: %d", (numOps));  	// love love love love sprinf lolololol
	int8_t netByteOrder = htons(strlen(line));
	memcpy(sendBuf, netByteOrder, 2);
	memcpy(sendBuf + 2, line, strlen(line));
	send(sock, sendBuf, strlen(line) + 2, 0);			// netbyteorder is supposed to be 16-bit = 2-bytes... hmmm... not sure...
}

int serverKByteAtATimeCmd()
{
    char sendBuf[500];
    char line[500];
    int8_t netByteOrder = htons(numOps);                // not sure if 'int8_t' is the proper type for network byte order

    sprintf(line, "kByteAtATimeCmd: %d", (numOps));    // love love love love sprintf lolololol
    int8_t netByteOrder = htons(strlen(line));
    memcpy(sendBuf, netByteOrder, 2);
    memcpy(sendBuf + 2, line, strlen(line));
    send(sock, sendBuf, strlen(line) + 2, 0);           // netbyteorder is supposed to be 16-bit = 2-bytes... hmmm... not sure...
}
*/
