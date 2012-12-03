
#include "chat.h"
#include "dbllist.c"

int main(int argc, char* argv[])
{
    int sock_fd, new_sock_fd;       //descriptors
    struct sockaddr_in srv, cli;    //server & client socket address
    struct msgbuf intro_msg, msg;
    int port_addr;
    int cli_len = sizeof(cli);      //client length
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

    bzero((char*) &srv, sizeof(srv));
    srv.sin_family = AF_INET;           //address family internet
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(port_addr);    //port 80
    printf("socket port is %d\n", port_addr);

    /** bind socket to a port*/
    if (bind(sock_fd, (struct sockaddr*)&srv, sizeof(srv))<0)
    {
        fprintf(stderr, "error binding socket.\n");
        exit(1);
    }
    printf("socket bound to port...\n");

    /** listen to any incoming connection attempts */
    if (listen(sock_fd, 5)<0)
    {
        fprintf(stderr, "error establishing listening.\n");
        exit(1);
    }
    printf("listening to incoming connection attempts...\n");

    /** use threads (+ thread pull) */

        while (1) /**standing by for a client */
        {
            /** accept */
            if (new_sock_fd = accept(sock_fd, (struct sockaddr*) &cli, &cli_len)<0)
            {
                fprintf(stderr, "error accepting client.\n");
                exit(1);
            }

            pthread_create();

            bzero((struct msgbuf*) &intro_msg, sizeof(intro_msg));

            intro_msg.msg_type = MSG;
            strcpy(intro_msg.msg_text, "username already exists, try again\n");

            /** write to client */
            if (nbytes = write(new_sock_fd, (struct msgbuf*) &intro_msg, sizeof(intro_msg)+1)<0)
            {
                fprintf(stderr, "error writting to server.\n");
                exit(1);
            }

            /** read from client */
            if (nbytes = read(new_sock_fd, (struct msgbuf*) &msg, sizeof(msgfgets(retry, 2, stdin)))<0)
            {
                fprintf(stderr, "error reading from client.\n");
                exit(1);
            }

            /** close connections */
            close(new_sock_fd);
            close(sock_fd);
        }

    dbllist_destroy(user_list, 0);

    return 0;
}
