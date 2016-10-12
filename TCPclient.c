#include <stdio.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "project1.h"

#define RCVBUFSIZE 100   /* Size of receive buffer */

void DieWithError(char *err)
{
	fprintf(stderr,"%s",err);
	exit(0);
}

void nullTerminatedCmdC(char *string, int  sock)
{

char buff[500];
int8_t cmd = commands[nullTerminatedCmd].cmd;
memset(buff,0,500);
memcpy(buff,&cmd,1);
memcpy(buff+1,string,strlen(string)+1);
send(sock,buff, strlen(string)+1+1, 0);// +1 for the null
int i = 0 ;
i = recv(sock,buff,500,0);
buff[i] = 0;
printf("%s\n",buff);

}

void noMoreCommandsC(int sock)
{

  close(sock);
}

/*
* For givenLengthCmd:
* Send the string’s length as a 16 bit number in network byte order followed by the
* characters of the string; do not include a null character.
*/
int givenLengthCmdC(char * sendt,int sock)
{

int16_t h = strlen(sendt);
char buff[500];
int8_t cmd = commands[givenLengthCmd].cmd;

h = htons(h);
memset(buff,0,500);
memcpy(buff,&cmd,1);


memcpy(buff+1,&h,2);
memcpy((buff+2+1),sendt,strlen(sendt));

send(sock,buff, (strlen(sendt)+2+1), 0);
int i =0;
i = recv(sock,buff,500,0);
buff[i] = 0;
printf("%s\n",buff);

}

/*
* For badIntCmd:
* Convert command.arg to an int and send the 4 bytes without applying htonl() to the value.
* This is the incorrect way to go! Note that the number you get back from the server won’t
* be what was sent.
*/


void badIntCmdC(char * arg, int sock)
{

int int2send = atoi(arg);
printf("%d\n", int2send);
char buff[500];
memset(buff,0,500);
int8_t cmd = commands[badIntCmd].cmd;


memcpy(buff,&cmd,1);

memcpy(buff+1,&int2send,4);

send(sock,buff, 4+1 , 0);
printf("%s\n",buff);
int i = 0;
i = recv(sock,buff,500,0);
int temp;

memcpy(&temp,buff+(i-4),4);

buff[i-4] = 0;
printf("%s%d\n",buff,temp);


}

/*
* For goodIntCmd:
* Convert command.arg to an int and send the 4 bytes resulting from applying htonl() to it.
*/
void goodIntCmdC(char * arg, int sock )
{

int sendInt = atoi(arg);
int8_t cmd = commands[goodIntCmd].cmd;

sendInt = htonl(sendInt);

char buff[500];
memset(buff,0,500);
memcpy(buff,&cmd,1);


memcpy(buff+1,&sendInt,4);

send(sock,buff, 4 + 1 , 0);

int i =0;
i = recv(sock,buff,500,0);
int temp;

memcpy(&temp,buff+(i-4),4);

buff[i-4] = 0;
printf("%s%d\n",buff,temp);

}

/* For byteAtATimeCmd and kByteAtATimeCmd:
* Convert command.arg to an int; send the int (after apply htonl) and then send
* that many bytes of alternr ating 1000-byte blocks of 0 bytes and 1 bytes.
* ByteAtATime - use 1-byte sends and receives
* KByteAtATime - use 1000-byte sends and receives (except for the last) */
void byteAtATimeCmdC(char * arg,int sock)
{
 int numsend = atoi(arg);
 int hl = htonl(numsend);
 int8_t cmd = commands[byteAtATimeCmd].cmd;

char buff[500];
memset(buff,0,500);
memcpy(buff,&cmd,1);
memcpy(buff+1,&hl,sizeof(int));

send(sock,buff, 1+sizeof(int), 0);//  the first chunk

int i = 0;
char bigbuf[1000];

int bytesToSend = 500000;
while(i<bytesToSend)
{
//	printf("i=%d\n", i%2);
	memset(buff,(i%2),1);
	send(sock,buff,1,0);
	memset(buff, 0, 500);
	recv(sock,buff,500,0);
//	printf("server: %s\n", buff);	
	i++;
}
recv(sock, buff, 500, 0);
//printf("recvd %d bytes\n", i);
//buff[i] = 0;
printf("bytes sent: %d\n", i);
printf("%s\n",buff);

}

void KbyteAtATimeCmdC(char * arg,int sock)
{
 int numsend = atoi(arg);
  int hl = htonl(numsend);
   int8_t cmd = commands[kByteAtATimeCmd].cmd;

   char bigBuf[1000];
   memset(bigBuf,0,500);
   memcpy(bigBuf,&cmd,1);
   memcpy(bigBuf+1,&hl,sizeof(int));

   send(sock,bigBuf, 1+sizeof(int), 0);//  the first chunk

   int i = 0;
   int bytesToSend = 500000;

   while(i<bytesToSend)
   {
	//	printf("i=%d\n", i%2);
        memset(bigBuf,(i%2),1);
        send(sock,bigBuf,1000,0);
        memset(bigBuf, 0, 1000);
        recv(sock,bigBuf,1000,0);
	//  printf("server: %s\n", buff);   
	    i++;
    }
		   
    //printf("recvd %d bytes\n", i);
    //buff[i] = 0;
    printf("%s\n",bigBuf);
		   		   
}

int main(int argc, char *argv[])
{
  int whilebytesRcvd = 0;
    int sock;                        /* Socket descriptor */
    struct sockaddr_in ServAddr; /* Echo server address */
    unsigned short ServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *String;                /* String to send to echo server */
    char Buffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int StringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv()
                                        and total bytes read */
    int send = 3;



 //   if (argc < 2)    /* Test for correct number of arguments */
   // {
     //  fprintf(stderr, "Usage: %s <Server IP> [<Port>]\n",
       //        argv[0]);
       //exit(1);
    //}

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
            //port number


    ServPort = atoi(argv[2]); /* Use given port, if any */


    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&ServAddr, 0, sizeof(ServAddr));     /* Zero out structure */
    ServAddr.sin_family      = AF_INET;             /* Internet address family */
    ServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    ServAddr.sin_port        = htons(ServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0)
        DieWithError("connect() failed");

        printf("\nDone Connecting\n");

	totalBytesRcvd = 0;


	int i= 0; // holds bytes read on each call.

	int secondwhilbytes = 0;
	char buf2[500];
//		getchar();
//	nullTerminatedCmd("Send as a Null Terminated String.", sock);
	nullTerminatedCmdC(commands[nullTerminatedCmd].arg, sock);
	
	givenLengthCmdC(commands[givenLengthCmd].arg,sock);
	goodIntCmdC(commands[goodIntCmd].arg,sock);
	badIntCmdC(commands[goodIntCmd].arg,sock);
	byteAtATimeCmdC(commands[byteAtATimeCmd].arg,sock);
	KbyteAtATimeCmdC(commands[kByteAtATimeCmd].arg, sock);
    close(sock);
    exit(0);
}
