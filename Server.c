#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */


#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 500

void DieWithError(char *err)
{

fprintf(stderr,"%s",err);

exit(0);

}
void printAddress(struct sockaddr_in client)
{
		        printf("%d.%d.%d.%d\n",(int)(client.sin_addr.s_addr&0xFF),(int)((client.sin_addr.s_addr&0xFF00)>>8),(int)((client.sin_addr.s_addr&0xFF0000)>>16),(int)((client.sin_addr.s_addr&0xFF000000)>>24));
}

void nullTerminatedCmd(int sock,char * arr, int bytesread)
{


int16_t len = strlen(arr);

char buff[500] = "Null Terminated: ";
int k = strlen(buff);

memcpy(buff+k,arr,len);

send(sock,buff,k+len,0);

return;

}



void givenLengthcmd(int sock,char * arr, int bytesread)
{

//char temp[2];
// memcpy(temp,arr,2);

int16_t len;
memcpy(&len,arr,2);
len = ntohs(len);


char buff[500] = "Given Length: " ;
int k = strlen(buff);

memcpy(buff+k,arr+2,len);

send(sock,buff,len+k,0);

return;

}


void goodIntCmd(int  sock, char * arr)
{

int j;
memcpy(&j,arr,4);
j = ntohl(j);
char * m = "Good Int: ";
char buf[strlen(m)+4+1];

memcpy(buf,m,strlen(m));
memcpy(buf+strlen(m),&j,4);
send(sock,buf,strlen(m)+4,0);

}

void BadIntCmd(int  sock, char * arr)
{

int j;
memcpy(&j,arr,4);
j = ntohl(j);
char * m = "Bad Int: ";
char buf[strlen(m)+4];

memcpy(buf,m,strlen(m));
memcpy(buf+strlen(m),&j,4);
send(sock,buf,strlen(m)+4,0);

}


int bytesAtATimeCmd(int sock, char * arr, int bytesread)
{
    int num_rcv = 1;
    int num;
    memcpy(&num,arr,4);
    num = ntohl(num);
    char buf = 0;
	char rplyBuf[500];
	bytesread -= 5;
	// subtract what weve already Read
	if(bytesread >= num)
	{
	  return;
	}

	num -= bytesread;

	int i = 0;
	num_rcv++;
//	memcpy(&rplyBuf, cmdName, strlen(cmdName));
//	char numRcvBuf[10];
	num_rcv = htons(num_rcv);
	sprintf(rplyBuf, "byteAtATimeCmd: %d", num_rcv);
	send(sock, rplyBuf, 500, 0);
	while(num > 0 && (i=recv(sock,buf,1,0))!= 0)
	{
		num -= i;
		num_rcv++;
	  	if(i>0)
		  printf("i=%d\n", i);
//	  	printf("num=%d\n", num);
		memset(rplyBuf, 0, 500);
		num_rcv = htons(num_rcv);
		sprintf(rplyBuf, "Byte At A Time: %d", num_rcv);
	  	send(sock, rplyBuf, 500, 0);
	}
	printf("bytes recieved: %d\n", num_rcv);
	send(sock, rplyBuf, 500, 0);
	return num_rcv;

}



int kbytesAtATimeCmd(int sock, char * arr, int bytesread)
{
    int num_rcv = 1;
    int num;
    memcpy(&num,arr,4);
    num = ntohl(num);
    char buf = 0;
    char rplyBuf[1000];
    bytesread -= 5;
    // subtract what weve already Read
    if(bytesread >= num)
    {
        return;
    }
    num -= bytesread;
    int i = 0;
    num_rcv++;
//  memcpy(&rplyBuf, cmdName, strlen(cmdName));
//  char numRcvBuf[10];
  num_rcv = htons(num_rcv);
  sprintf(rplyBuf, "KbyteAtATimeCmd: %d", num_rcv);
  send(sock, rplyBuf, 1000, 0);
  while(num > 0 && (i=recv(sock,buf,1000,0))!= 0)
  {
      num -= i;
      num_rcv++;
      if(i>0)
          printf("i=%d\n", i);
//      printf("num=%d\n", num);
      memset(rplyBuf, 0, 1000);
      num_rcv = htons(num_rcv);
      sprintf(rplyBuf, "KByte At A Time: %d", num_rcv);
      //send(sock, rplyBuf, 1000, 0);
   }
  send(sock, rplyBuf, 1000, 0);
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
//  printf("#3\n");
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
	printf("...Socket Open...\n");
    /* Construct local address structure */
    memset(&servAddr, 0, sizeof(servAddr));   /* Zero out structure */
    servAddr.sin_family = AF_INET;                /* Internet address family */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    servAddr.sin_port = htons(servPort);      /* Local port */
    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("bind() failed");
	printf("...Server Bound...\n");
	/* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");
	printf("...Server Listening...\n");
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

			while(1)
			{
//				sleep(1);
    		    read = recv(clntSock, buffer+bytesRecvd, 100, 0);

	        	printf("Read Now %d: %s",(int8_t)buffer[0],buffer);

				if(bytesRecvd == 0) // initial read
				{
					memset(buf2, 0, RCVBUFSIZE); // reset buf2 to 0
				}
				bytesRecvd += read;
				totalBytesRecvd += read;

			    switch((int8_t)buffer[0])
			    {
					case 1:		printf("nullTerminatedCmd() called\n");
								read = 0;
								nullTerminatedCmd(clntSock, buffer+1, read);
								bytesRecvd = 0;									break; // nullTerminatedCmd
					case 2:		printf("givenLengthcmd() called\n");
								givenLengthcmd(clntSock, buffer+1, read);
								bytesRecvd = 0;									break; // givenLengthCmd
					case 3:		printf("goodIntCmd() called\n");
								goodIntCmd(clntSock, buffer+1);
								bytesRecvd = 0;									break; // badIntCmd
					case 4:		printf("BadIntCmd() called\n");
								BadIntCmd(clntSock, buffer+1);
								bytesRecvd = 0;									break; // goodIntCmd
					case 5:		printf("bytesAtATimeCmd() called\n");
								recvCalls++;
								bytesAtATimeCmd(clntSock, buffer+1, read);		break; // bytesAtATimeCmd
					case 6:		printf("kbytesAtATimeCmd() called\n");
								recvCalls++;
								kbytesAtATimeCmd(clntSock, buffer+1, read);		break; // KbyteAtATimeCmd
			    }
				//fwrite(buffer, sizeof(buffer[0]), sizeof(buffer)/sizeof(buffer[0]), log);
			}
		}
	}
	printf("total bytes read: %d\n", totalBytesRecvd);
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
    memcpy(sendBuf, netByteOrder, 2);f
    memcpy(sendBuf + 2, line, strlen(line));
    send(sock, sendBuf, strlen(line) + 2, 0);           // netbyteorder is supposed to be 16-bit = 2-bytes... hmmm... not sure...
}
*/
