#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 100   /* Size of receive buffer */

void DieWithError(char *err)
{

fprintf(stderr,"%s",err);

exit(0);

}

void nullTerminatedCmd(char *string, int  sock)
{

char buff[500];
int8_t cmd = 1;
memset(buff,0,500);
memcpy(buff,&cmd,1);
memcpy(buff+1,string,strlen(string)+1);
send(sock,buff, strlen(string)+1+1, 0);// +1 for the null


}

void noMoreCommands(int sock)
{

  close(sock);
}

/*
* For givenLengthCmd:
* Send the string’s length as a 16 bit number in network byte order followed by the
* characters of the string; do not include a null character.
*/
int givenLengthCmd(char * sendt,int sock)
{

int16_t h = strlen(send);
char buff[500];
int8_t cmd = 2;

h = htons(h);
memset(buff,0,500);
memcpy(buff,&cmd,1);


memcpy(buff+1,&h,2);
memcpy((buff+2+1),&sendt,strlen(sendt));

send(sock,buff, (strlen(sendt)+2+1), 0);

}

/*
* For badIntCmd:
* Convert command.arg to an int and send the 4 bytes without applying htonl() to the value.
* This is the incorrect way to go! Note that the number you get back from the server won’t
* be what was sent.
*/


void badIntCmd(char * arg, int sock)
{

int int2send = atoi(arg);
char buff[500];
memset(buff,0,500);
int8_t cmd = 3;


memcpy(buff,&cmd,1);

memcpy(buff+1,&int2send,4);

send(sock,buff, 4+1 , 0);


}

/*
* For goodIntCmd:
* Convert command.arg to an int and send the 4 bytes resulting from applying htonl() to it.
*/
void goodIntCmd(char * arg, int sock )
{

int sendInt = atoi(arg);
int8_t cmd = 3;

sendInt = htonl(sendInt);

char buff[500];
memcpy(buff,&cmd,1);
memset(buff,0,500);

memcpy(buff+1,&sendInt,4);

send(sock,buff, 4 + 1 , 0);


}

/* For byteAtATimeCmd and kByteAtATimeCmd:
* Convert command.arg to an int; send the int (after apply htonl) and then send
* that many bytes of alternr ating 1000-byte blocks of 0 bytes and 1 bytes.
* ByteAtATime - use 1-byte sends and receives
* KByteAtATime - use 1000-byte sends and receives (except for the last) */
void byteAtATimeCmd(char * arg,int sock)
{
 int numsend = atoi(arg);
 int hl = htonl(numsend);
 int8_t cmd = 5;



char buff[500];
memset(buff,0,500);
memcpy(buff,&cmd,1);
memcpy(buff+1,hl,sizeof(int));

send(sock,buff, 1+sizeof(int), 0);//  the first chunk

int i = 0;
char bigbuf[1000];

while(i<numsend)
{
  memset(bigbuf,(i%2),1000);
  int j = 0;

  while(j<1000)
  {
    send(sock,buff,1,0);
    j++;
  }

  i++;
}


}

void KbyteAtATimeCmd(char * arg,int sock)
{
 int numsend = atoi(arg);
 int hl = htonl(numsend);
 int8_t cmd = 6;



char buff[500];
memset(buff,0,500);
memcpy(buff,&cmd,1);
memcpy(buff+1,hl,sizeof(int));

send(sock,buff, 1+sizeof(int), 0);//  the first chunk

int i = 0;
char bigbuf[1000];

while(i<numsend)
{
  memset(bigbuf,(i%2),1000);
  send(sock,buff,1000,0);

  i++;
}


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

while(1)
{

int secondwhilbytes = 0;
char buf2[500];

send()

while((i = recv(sock, Buffer, RCVBUFSIZE - 1, 0))>0)
{

  printf("%s\n",Buffer);



}

}
    close(sock);
    exit(0);
}

