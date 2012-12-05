
#include "chat.h"
#include "dbllist.c"

int main(int argc, char* argv[])
{
    int fd;                        //descriptors
    struct sockaddr_in serv_addr;         //client socket address
    struct hostent *server;
    struct msgbuf msg, login_msg;
    char client_name[32], host_name[128], input[1024];
    int port_addr;
    int nbytes;
    char varify[1];

    printf("Please enter your name:\n");
    fgets(client_name, 32, stdin);

    /** commit socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd<0)
    {
        fprintf(stderr, "error creating socket.\n");
        exit(1);
    }

    port_addr = DEFAULT_PORT;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;           //address family internet
    serv_addr.sin_port = htons(port_addr);    //default port is 5000

    /** connect to server */
    do
    {
        if(connect(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        {
            fprintf(stderr, "error connecting to server.\nretry? [y/n]\n");
            scanf("%s", &varify);
            if (strstr(varify, "n")!=NULL)
            {
                exit(1);
            }
        }
    }
    while (strstr(varify, "y")!=NULL);
    printf("\nconnection established!\n");

    /** if a connection was established send a login request */
    login_msg.msg_type = MSG;
    strcpy(login_msg.msg_text, client_name);

    if (nbytes = write(fd, (struct msgbuf*) &login_msg, sizeof(login_msg)+1)<0)
    {
        fprintf(stderr, "error sending login request.\n");
        exit(1);
    }

    printf("connection established to server. type 'exit' to quit\n");
    do
    {
        /** read from server */
        printf("read from server\n");
        if (nbytes = read(fd, (struct msgbuf*) &msg, sizeof(msg))<0)
        {
            fprintf(stderr, "error reading from client.\n");
            exit(1);
        }

        fgets(input, 1024, stdin);
        strcpy(msg.msg_text, input);

        /** write to server */
        printf("write to server server\n");
        if (nbytes = write(fd, (struct msgbuf*) &msg, sizeof(msg))<0)
        {
            fprintf(stderr, "error writting to client.\n");
            exit(1);
        }
    }
    while ((strstr(msg.msg_text, "exit")!=NULL));

    /**close connection */
    close(fd);
    exit(0);
}
