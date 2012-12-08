
#include "chat.h"
#include "dbllist.c"

int main(int argc, char* argv[])
{
    int sock_fd, new_sock_fd; //descriptors
    struct sockaddr_in serv_addr, cli; //server & client socket address
    struct msgbuf intro_msg, msg;
    char input[1024];
    int port_addr;
    int cli_len = sizeof(cli); //client length
    int nbytes;

    dbllist_t* user_list = (dbllist_t*) malloc(sizeof(dbllist_t)); //a list of all the users in session
    dbllist_init(user_list);

    /** create socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd<0)
    {
        fprintf(stderr, "error creating socket.\n");
        exit(1);
    }
    port_addr = DEFAULT_PORT;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //address family internet
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_addr); //default port 5000
    printf("socket port is %d\n", port_addr);

    /** bind socket to a port*/
    if (bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
    {
        fprintf(stderr, "error binding socket.\n");
        exit(1);
    }
    printf("socket bound to port...\n");

    /** listen to any incoming connection attempts */
    printf("listening to incoming connection attempts...\n");
    if (listen(sock_fd, 5)<0)
    {
        fprintf(stderr, "error establishing listening.\n");
        exit(1);
    }

    bzero(&msg, sizeof(msg));
    msg.msg_type = MSG;
    strcpy(msg.msg_text, "Hello, please enter your username\n");    //login msg

    // use threads here (try thread pulling)

    do /**standing by for a client */
    {
        printf("awaiting accept\n");
        /** accept */
        if (new_sock_fd = accept(sock_fd, (struct sockaddr*) &cli, &cli_len)<0)
        {
            fprintf(stderr, "error accepting client.\n");
            exit(1);
        }
        printf("after accept, handshake with client successful.\n");

        /** write to client */
        nbytes = write(new_sock_fd, (struct msgbuf*) &msg, sizeof(msg));
        if (nbytes<0)
        {
            fprintf(stderr, "error writting to client.\n");
            exit(1);
        }

        /** read from client */
        nbytes = read(new_sock_fd, (struct msgbuf*) &msg, sizeof(msg));
        if (nbytes<0)
        {
            fprintf(stderr, "error reading from client.\n");
            exit(1);
        }
        //printf("%s\n", msg.msg_text);


    }
    while (strcmp(msg.msg_text, "exit\n")!=0);
    printf("\nclosing server and leaving...\n");

    /** cleaning up */
    close(new_sock_fd);
    close(sock_fd);
    dbllist_destroy(user_list, 0);
    exit(0);
}
