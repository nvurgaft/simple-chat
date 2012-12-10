/** server */

#include "chat.h"
#include "dbllist.c"

void *func(void* buffer);
void print_user_list(dbllist_t* list);
void error(const char *msg);

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[1024];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    bool_t first_post=TRUE;
    //pthread_t threads[1000];
    //int rc, t;

    //a list of all the users in session
    dbllist_t* user_list = (dbllist_t*) malloc(sizeof(dbllist_t));
    dbllist_init(user_list);

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

     while (strcmp(buffer, "exit\n")!=0);
    {
        if (first_post)
        {
            /** write, asks client for user name */
            //bzero(buffer,sizeof(buffer));
            n = write(newsockfd, "Hello, please enter your username\n", sizeof(buffer));
            if (n < 0)
            {
                error("ERROR writing to socket");
            }

            /** read, logs user name*/
            bzero(buffer,sizeof(buffer));
            n = read(newsockfd, buffer, sizeof(buffer));
            if (n < 0)
            {
                error("ERROR reading from socket");
            }
            printf("user name is: %s\n",buffer);

            ///--- traverse user list to see if user exists ---///
            dbllist_node_t* current = user_list->head;
            if (user_list->head==NULL)
            {
                dbllist_append(user_list, buffer);
            }
            else
            {
                while (current!=NULL)
                {
                    if ((char*)current->data==buffer)
                    {
                        printf("Username already exist, try again\n");
                        break;
                    }
                    current = current->next;
                }
            }
            ///------------------------------------------------///

            dbllist_append(user_list, buffer);

            /** confirmation */
            if (TRUE)
            {
                n = write(newsockfd, "Username OK\n", sizeof(buffer));
                if (n < 0)
                {
                    error("ERROR writing to socket");
                }

                first_post = FALSE;
            }
        }


        /** read */
        bzero(buffer,sizeof(buffer));
        n = read(newsockfd, buffer, sizeof(buffer));
        if (n < 0)
        {
            error("ERROR reading from socket");
        }
        printf("Message received: %s\n",buffer);

        /** write */
        bzero(buffer,sizeof(buffer));
        n = write(newsockfd, buffer, sizeof(buffer));
        if (n < 0)
        {
            error("ERROR writing to socket");
        }
    }
    close(newsockfd);
    close(sockfd);
    dbllist_destroy(user_list,1);
    return 0;
}

void* func(void* buffer)
{

    pthread_exit(NULL);
}

void print_user_list(dbllist_t* list)
{
    if (list->head==NULL)
    {
        printf("nothing to print, no users are logged in\n");
    }
    else
    {
        dbllist_node_t* current = list->head;
        printf("printing users list:\n");
        printf("====================\n");
        while (current!=NULL)
        {
            printf("%s", (char*) current->data);
            current = current->next;
        }
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
