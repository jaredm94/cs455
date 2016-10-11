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

void nullTerminatedCmd(int sock,char * arr, int bytesread)
{


int16_t len = strlen(arr);
char buff[500];
memset(buff,0,500);

char buff[500] = "Null Terminated: ";
int k = strlen(buff);

memcpy(buff+k,&len,2);

send(sock,buff,k+2,0);

return;

}


void givenLengthcmd(int sock,char * arr, int bytesread)
{

char temp[2];
 memcpy(temp,arr,2);

int16_t len = atio(temp)
len = ntos(len);


char buff[500];
memset(buff,0,500);

char buff[500] = "Given Length: " ;
int k = strlen(buff);

memcpy(buff+k,&len,2);

send(sock,buff,k+2,0);

return;

}


void givenLengthcmd(int sock,char * arr, int bytesread)
{

char temp[2];
 memcpy(temp,arr,2);

int16_t len = atio(temp)
len = ntos(len);


char buff[500];
memset(buff,0,500);

char buff[500] = "Given Length: " ;
int k = strlen(buff);

memcpy(buff+k,&len,2);

send(sock,buff,k+2,0);

return;

}


void goodIntCmd(int  sock, char * arr)
{

int j = atoi(arr);
j = htons(j);
char * m = "Good Int: ";
char buf[strnlen(m)+4];

memcpy(buf,m,strnlen(m));
memcpy(buf+strlen(m),&j,4);
send(sock,buff,strlen(m)+4);



}

void BadIntCmd(int  sock, char * arr)
{

int j = atoi(arr);
j = htons(j);
char * m = "Bad Int: ";
char buf[strnlen(m)+4];

memcpy(buf,m,strnlen(m));
memcpy(buf+strlen(m),&j,4);
send(sock,buff,strlen(m)+4);

}


int bytesAtATimeCmd(int sock, char * arr, int bytesread)
{
    int num_rcv = 1;


    int num = atoi(arr);
    num = ntol(num);
    char buf = 0;
    bytesread -= 4;
// subtract what weve already Read
if(bytesread >= num)
{
  return;
}

  num -= bytesread;



int i = 0;

while((i=recv(sock,buf,1,0))!=0 && num > 0)
{
  num -= i;
  num_rcv++;
}

return num_rcv;

}



int kbytesAtATimeCmd(int sock, char * arr, int bytesread)
{
    int num_rcv = 1;


    int num = atoi(arr);
    num = ntol(num);
    char buf[1000] = 0;
    bytesread -= 4;
// subtract what weve already Read

if(bytesread >= num)
{
  return;
}

    num -= bytesread;



int i = 0;

while((i=recv(sock,buf,1000,0))!=0 && num > 0)
{
  num -= i;
  num_rcv++;
}

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
	log  = fopen("log.txt", 'a'); // open log file for append
	
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

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

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

			while((read = recv(clntSock, buffer+bytesRecvd, RCVBUFSIZE-1, 0))> 0)
			{
				if(bytesRecvd == 0) // initial read
				{
					memset(buf2, 0, RCVBUFSIZE); // reset buf2 to 0
				}
				bytesRecvd += read;
				totalBytesRecvd += read;	

			    switch((int8_t)buffer[0])
			    {
					case 1:		read = 0;
								nullTerminatedCmd(clntSock, buffer + 1, read); 
								bytesRecvd = 0;									break; // nullTerminatedCmd
					case 2:		givenLengthcmd(clntSock, buffer + 1, read);
								bytesRecvd = 0;									break; // givenLengthCmd
					case 3:		goodIntCmd(clntSock, buffer + 1);
								bytesRecvd = 0;									break; // badIntCmd
					case 4:		BadIntCmd(clntSock, buffer + 1);
								bytesRecvd = 0;									break; // goodIntCmd
					case 5:		recvCalls++;
								bytesAtATimeCmd(clntSock, buffer + 1, read);		break; // bytesAtATimeCmd
					case 6:		recvCalls++;
								kbytesAtATimeCmd(clntSock, buffer + 1, read);		break; // KbyteAtATimeCmd
			    }
				fwrite(buffer, sizeof(buffer[0]), sizeof(buffer)/sizeof(buffer[0]), log);
			}
		}
	}
	close(servSock);
	fclose(log);
	exit(0);
    /* NOT REACHED */
}
int serverByteAtATimeCmd(int sock, int numOps)
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
