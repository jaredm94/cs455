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
        while(1)
		{
			int bytesRecvd = 0;
			char buf2[RCVBUFSIZE];
			while((read = recv(clntSock, buffer, RCVBUFSIZE-1, 0))> 0)
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
							for(;read<RCVBUFSIZE;)
							 bytesRecvd = 0;
													break; // null
					case 2:		bytesRecvd = 0;		break; // givenLengthCmd
					case 3:		bytesRecvd = 0;		break; // badIntCmd
					case 4:		bytesRecvd = 0;		break; // goodIntCmd
					case 5:		bytesRecvd = 0;		break; // bytesAtATimeCmd
					case 6:		bytesRecvd = 0;		break; // KbyteAtATimeCmd
			    }
			}
		}
	}
	close(servSock);
	exit(0);
    /* NOT REACHED */
}
int serverBytesAtATimeCmd(int sock, int bytesRead)
{
	char sendBuff[500];
	char *commandRcvd = "byteAtATimeCmd";
	memcpy(buf, commandRcvd, sizeof(commandRcvd + 1));
	memcpy(buf[sizeof(commandRcvd) + 1],  
}

int serverKBytesAtATimeCmd()
{

}
