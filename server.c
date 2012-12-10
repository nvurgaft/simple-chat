/** server side */

#include "chat.h"
#include "dbllist.c"

void *client_thread(void* buffer);
void print_user_list(dbllist_t* list);
void error(const char *msg);

int sockfd, newsockfd[THREADS_MAX];

int main(int argc, char *argv[])
{
    /** declarations */
    int portno, n, tc=0;;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t threads[THREADS_MAX];

    //a list of all the users in session
    dbllist_t* user_list = (dbllist_t*) malloc(sizeof(dbllist_t));
    dbllist_init(user_list);

    /** start server */
    if (argc < 2)
    {
        error("ERROR no port provided [command format: ./server <port number>]");
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

    /** great fun with threads ahead */

    while (tc<THREADS_MAX)
    {
        clilen = sizeof(cli_addr);
        newsockfd[tc] = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd[tc] < 0) error("ERROR on accept");

        n = write(newsockfd[tc], "Hello, please enter your username\n", sizeof(buffer));
        if (n < 0) error("ERROR writing to socket");

        // read, logs user name
        bzero(buffer,sizeof(buffer));
        n = read(newsockfd[tc], buffer, sizeof(buffer));
        if (n < 0) error("ERROR reading from socket");

        printf("user name is: %s\n",buffer);

        // use list to find out if user name already exists
        dbllist_node_t* current = user_list->head;
        if (user_list->head==NULL) dbllist_append(user_list, buffer);
        else
        {
            while (1)
            {
                //traverse the list to see if the user name already exists
                while (current!=NULL)
                {
                    if ((char*)current->data==buffer)
                    {
                        n = write(newsockfd[tc], "Username already exists, try again\n", sizeof(buffer));
                        if (n < 0) error("ERROR writing to socket");
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

                    n = write(newsockfd[tc], "Username OK\n", sizeof(buffer));
                    if (n < 0) error("ERROR writing to socket");
                    break;
                }
            }
        }
        pthread_create(&threads[tc], NULL, client_thread, &tc);
        tc++;
    }

    /** ----------------------- */

    for (tc=0; tc<THREADS_MAX; tc++)
    {
        pthread_join (threads[tc], NULL);
    }

    /** end program routines */
    for (tc=0; tc<THREADS_MAX; tc++)
    {
        close(newsockfd[tc]);
    }

    close(sockfd);
    // changing the 0 to 1 will cause a world of pain
    dbllist_destroy(user_list, 0);
    printf("closing server, goodbye!\n");

    return 0;
}

void* client_thread(void* userc)
{
    int user_c = *(int*) userc;

    char buffer[BUFFER_SIZE];
    int n;

    while (1)
    {
        // write, but the server doesn't need the user to write directly to client
        // bzero(buffer,sizeof(buffer));
        // fgets(buffer,sizeof(buffer)-1,stdin);
        if (strstr(buffer, "exit\n")!=NULL) break;
        n = write(newsockfd[user_c], buffer, sizeof(buffer));
        if (n < 0)
        {
            error("ERROR writing to socket");
        }

        // read
        bzero(buffer,sizeof(buffer));
        n = read(newsockfd[user_c], buffer, sizeof(buffer));
        if (n < 0)
        {
            error("ERROR reading from socket");
        }
        printf("Message received: %s\n",buffer);
    }

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
