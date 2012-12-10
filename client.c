/** client side */

#include "chat.h"
#include "dbllist.c"

void error(const char *msg);

int main(int argc, char *argv[])
{
    // declarations
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[1024];

    // start client
    if (argc < 2)
    {
       error("ERROR no port provided [command format: ./client <port number>]");
    }
    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }// read
        bzero(buffer,sizeof(buffer));
        n = read(sockfd,buffer,sizeof(buffer));
        if (n < 0)
        {
            error("ERROR reading from socket");
        }
        printf("%s\n",buffer);

    while (strcmp(buffer, "exit\n")!=0)
    {
        printf("input: ");
        // write
        bzero(buffer,sizeof(buffer));
        fgets(buffer,sizeof(buffer)-1,stdin);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
        {
            error("ERROR writing to socket");
        }

        // read
        bzero(buffer,sizeof(buffer));
        n = read(sockfd,buffer,sizeof(buffer));
        if (n < 0)
        {
            error("ERROR reading from socket");
        }
        printf("%s\n",buffer);
    }

    // end program routines
    close(sockfd);
    return 0;
}

void error(const char *msg) {
    perror(msg);
    exit(0);
}
