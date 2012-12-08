
#include "chat.h"
#include "dbllist.c"

int main(int argc, char* argv[])
{
    int sock_fd;
    struct sockaddr_in serv_addr;         //client socket address
    struct msgbuf msg, login_msg;
    char client_name[32], host_name[128], input[1024];
    struct hostent *server;
    int port_addr;
    int nbytes;
    char varify[1];

    /** create socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd<0)
    {
        fprintf(stderr, "error creating socket.\n");
        exit(1);
    }

    port_addr = DEFAULT_PORT;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;           //address family internet
    //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_addr);    //default port is 5000

    /** connect to server */
    do
    {
        if(connect(sock_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        {
            fprintf(stderr, "error connecting to server.\nretry? [y/n]\n");
            scanf("%s", &varify);
            if (strstr(varify, "n")!=NULL)
            {
                printf("\nleaving...\n");
                exit(1);
            }
        }
    }
    while (strstr(varify, "y")!=NULL);
    printf("\nconnection established to server. type 'exit' to quit\n");

    /** if a connection was established send a login request */
    login_msg.msg_type = MSG;
    strcpy(login_msg.msg_text, client_name);

    if (nbytes = write(sock_fd, (struct msgbuf*) &login_msg, sizeof(login_msg)+1)<0)
    {
        fprintf(stderr, "error sending name to server.\n");
        exit(1);
    }

    do
    {
        /** read from server */
        printf("read from server\n");
        if (nbytes = read(sock_fd, (struct msgbuf*) &msg, sizeof(msg))<0)
        {
            fprintf(stderr, "error reading from server.\n");
            exit(1);
        }
        printf("%s\n", msg.msg_text);

        //write stuff (find better method, buffer is still cluttered)
        //fgets(input, 1024, stdin);
        //strcpy(msg.msg_text, input);

        /** write to server */
        printf("write to server\n");
        if (nbytes = write(sock_fd, (struct msgbuf*) &msg, sizeof(msg))<0)
        {
            fprintf(stderr, "error writting to server.\n");
            exit(1);
        }
    }
    while (strcmp(msg.msg_text, "exit\n")!=0);
    printf("\nclosing client and leaving...\n");

    /**cleaning up */
    close(sock_fd);
    exit(0);
}
