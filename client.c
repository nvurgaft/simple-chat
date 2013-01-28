/** client side */

#include "chat.h"

void error(const char *msg);

int main(int argc, char *argv[])
{
    // declarations on the stack
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server_host;
    char buffer[BUFFER_SIZE];

    // start client
    if (argc < 3) error("ERROR invalid arg count [command format: ./client <local host> <port number>]");
    
    // setting local host given by argv[1]
    if (strcmp(argv[1], "localhost")==0) {
        char hostname[1024];
        hostname[1023] = "\0";
        gethostname(hostname, 1023);
        server_host = gethostbyname(hostname);
        if (server_host==NULL) error("ERROR unable to resolve hostname");
    }

    // setting port number given by arg[2]
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bcopy((char*) server_host->h_addr, (char*) &serv_addr.sin_addr.s_addr, server_host->h_length);
    serv_addr.sin_port = htons(portno);

    // connect to the server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");

    while (1)
    {
        // read
        bzero(buffer,sizeof(buffer));
        n = read(sockfd,buffer,sizeof(buffer));
        if (n < 0) error("ERROR reading from socket");
        printf("%s\n",buffer);

        // write
        printf("input: ");
        bzero(buffer,sizeof(buffer));
        fgets(buffer,sizeof(buffer)-1,stdin);

        // if the user types 'exit' the loop will break and the client will exit
        if (strstr(buffer, "exit\n")!=NULL) break;
        n = write(sockfd,buffer,sizeof(buffer));
        if (n < 0) error("ERROR writing to socket");
    }

    // end program routines
    printf("closing client, goodbye!\n");
    close(sockfd);
    return 0;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}
