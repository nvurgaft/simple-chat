
#include "chat.h"
#include "dbllist.c"

int main(int argc, char* argv[])
{
    int fd, newfd;                  //descriptors
    struct sockaddr_in cli;         //client socket address
    struct msgbuf msg, login_msg;
    char client_name[32];
    char host_name[128];
    int port_addr;
    int nbytes;
    char varify[1];

    printf("Please enter your name:\n");
    fgets(client_name, 32, stdin);

    /** commit socket */
    if (fd = socket(PF_INET, SOCK_STREAM, 0)<0)
    {
        fprintf(stderr, "error creating socket.\n");
        exit(1);
    }

    port_addr = DEFAULT_PORT;

    cli.sin_family = AF_INET;           //address family internet
    cli.sin_port = htons(port_addr);    //default port is 5000
    //cli.sin_addr.s_addr = inet_addr("128.2.35.50"); // ???

    /** connect to server */
    do {
        if(connect(fd, (struct sockaddr*) &cli, sizeof(cli)) < 0)
        {
            fprintf(stderr, "error connecting to server.\nretry? [y/n]\n");
            scanf("%s", &varify);
            if (strstr(varify, "n")!=NULL)
            {
                exit(1);
            }
        }
    } while (strstr(varify, "y")!=NULL);

    /** if a connection was established send a login request */
    login_msg.msg_type = MSG;
    strcpy(login_msg.msg_text, client_name);

    if (nbytes = write(newfd, (struct msgbuf*) &login_msg, sizeof(login_msg))<0)
    {
        fprintf(stderr, "error sending login request.\n");
        exit(1);
    }

    printf("connection established to server. type 'exit' to quit\n");
    do {
        /** read from server */
        if (nbytes = read(newfd, (struct msgbuf*) &msg, sizeof(msg))<0)
        {
            fprintf(stderr, "error reading from client.\n");
            exit(1);
        }

        /** write to server */
        if (nbytes = write(newfd, (struct msgbuf*) &login_msg, sizeof(login_msg))<0)
        {
            fprintf(stderr, "error writting to server.\n");
            exit(1);
        }
    } while ((strstr(msg.msg_text, "exit")!=NULL));

    /**close connection */
    close(fd);

    return 0;
}
