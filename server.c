/** server side */

#include "chat.h"
#include "dbllist.c"

void *func(void* buffer);
void print_user_list(dbllist_t* list);
void error(const char *msg);

int main(int argc, char *argv[])
{
    /** declarations */
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    bool_t first_post=TRUE, valid_user=FALSE;
    //pthread_t threads[1000];
    //int rc, t;

    //a list of all the users in session
    dbllist_t* user_list = (dbllist_t*) malloc(sizeof(dbllist_t));
    dbllist_init(user_list);

    /** start server */
    if (argc < 2)
    {
        fprintf(stderr,"ERROR no port provided [command format: ./server <port number>]");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    {
        error("ERROR on accept");
    }

     while (1)
    {
        if (first_post==TRUE)
        {
            // write, asks client for user name
            //bzero(buffer,sizeof(buffer));
            n = write(newsockfd, "Hello, please enter your username\n", sizeof(buffer));
            if (n < 0)
            {
                error("ERROR writing to socket");
            }

            // read, logs user name
            bzero(buffer,sizeof(buffer));
            n = read(newsockfd, buffer, sizeof(buffer));
            if (n < 0)
            {
                error("ERROR reading from socket");
            }
            printf("user name is: %s\n",buffer);

            // use list to find out if user name already exists
            dbllist_node_t* current = user_list->head;
            if (user_list->head==NULL)
            {
                dbllist_append(user_list, buffer);
            }
            else
            {
                while (valid_user==FALSE)
                {
                    //traverse the list to see if the user name already exists
                    while (current!=NULL)
                    {
                        if ((char*)current->data==buffer)
                        {
                            n = write(newsockfd, "Hello, please enter your username\n", sizeof(buffer));
                            if (n < 0)
                            {
                                error("ERROR writing to socket");
                            }
                            break;
                        }
                        current = current->next;
                    }
                    //if the list was traversed and the user name not found ...
                    if (current==NULL)
                    {
                        dbllist_append(user_list, buffer);
                        // confirmation
                        printf("Username OK\n");
                        n = write(newsockfd, "Username OK\n", sizeof(buffer));
                        if (n < 0)
                        {
                            error("ERROR writing to socket");
                        }

                        valid_user==TRUE;
                    }
                }
            }
            print_user_list(user_list);
            first_post = FALSE;
        }

        // write, but the server doesn't need a user to write to client
        //bzero(buffer,sizeof(buffer));
        //fgets(buffer,sizeof(buffer)-1,stdin);
        if (strstr(buffer, "kill server\n")!=NULL) break;
        n = write(newsockfd, buffer, sizeof(buffer));
        if (n < 0)
        {
            error("ERROR writing to socket");
        }

        // read
        bzero(buffer,sizeof(buffer));
        n = read(newsockfd, buffer, sizeof(buffer));
        if (n < 0)
        {
            error("ERROR reading from socket");
        }
        printf("Message received: %s\n",buffer);
    }

    /** end program routines */
    printf("closing server, goodbye!\n");
    close(newsockfd);
    close(sockfd);
    dbllist_destroy(user_list,1);
    return 0;
}

void* func(void* buffer) {

    pthread_exit(NULL);
}

void print_user_list(dbllist_t* list) {
    if (list->head==NULL)
    {
        printf("nothing to print, no users are logged in\n");
    }
    else
    {
        int i=1;
        dbllist_node_t* current = list->head;
        printf("Online users:\n");
        while (current!=NULL)
        {
            printf("%d. %s", i, (char*) current->data);
            current = current->next;
            i++;
        }
    }
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}
