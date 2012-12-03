
#include "chat.h"
#include "dbllist.c"

int main(int argc, char* argv[])
{
    int fd, newfd;                  //descriptors
    struct sockaddr_in cli;         //client socket address
    struct msgbuf msg, login_msg;
    char user_name[32];
    char host_name[128];
    int port_addr;
    int nbytes;
    char client_name[32];

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
    if(connect(fd, (struct sockaddr*) &cli, sizeof(cli)) < 0)
    {
        fprintf(stderr, "error connecting to server.\n");
        exit(1);
    }

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
        else
        {
            if (msg.msg_type==MSG)
            {
                printf("%s", msg.msg_text);
                fgets(client_name, 32, stdin);
            }
            if (msg.msg_type==CMD)
            {
                printf("do something...\n");
            }
        }

        /** write to server */
        if (nbytes = write(newfd, (struct msgbuf*) &login_msg, sizeof(login_msg))<0)
        {
            fprintf(stderr, "error writting to server.\n");
            exit(1);
        }
    } while (strcmp("exit\n", msg.msg_text)!=0);

    /**close connection */
    close(fd);

    return 0;
}
