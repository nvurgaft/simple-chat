/** server side */

#include "chat.h"

void *client_thread(void* buffer);
void print_user_list(dbllist_t* list);
void error(const char *msg);

int sockfd, newsockfd[THREADS_MAX];
pthread_mutex_t mutex;
char buffer[BUFFER_SIZE];

int main(int argc, char *argv[])
{
    /** declarations */
    int portno, n, tc=0;;
    socklen_t clilen;
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

    if (pthread_mutex_init(&mutex, NULL)!=0)
    {
        error("ERROR failed to init mutex");
    }

    while (tc<THREADS_MAX)
    {
        int i=0;
        char buffer2[2048];
        bzero(buffer2, sizeof(buffer2));

        clilen = sizeof(cli_addr);
        newsockfd[tc] = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd[tc] < 0) error("ERROR on accept");

        bzero(buffer,sizeof(buffer));
        n = write(newsockfd[tc], "Hello, please enter your username\n", sizeof(buffer));
        if (n < 0) error("ERROR writing to socket");

        // read, logs user name
        bzero(buffer,sizeof(buffer));
        n = read(newsockfd[tc], buffer, sizeof(buffer));
        if (n < 0) error("ERROR reading from socket");

        printf("user name is: %s\n",buffer);

        // use list to find out if user name already exists
        dbllist_node_t* current = user_list->head;

        // if list is empty then the first user name is always a valid one
        if (user_list->size==0)
        {
            dbllist_prepend(user_list, buffer);
            dbllist_node_t* current = user_list->head;

            strcpy(buffer2, "Username OK\nWho do you want to talk to? Write usernames seperated by ;\n");
            strcat(buffer2, "Online users:\n");
            while (current!=NULL)
            {
                strncat(buffer2, (char*) current->data, 32);
                current = current->next;
            }
            n = write(newsockfd[tc], buffer2, sizeof(buffer));
            if (n < 0) error("ERROR writing to socket");
            print_user_list(user_list);
        }
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
                    dbllist_prepend(user_list, buffer);
                    dbllist_node_t* current = user_list->head;

                    strcpy(buffer2, "Username OK\nWho do you want to talk to? Write usernames seperated by ;\n");
                    strcat(buffer2, "Online users:\n");
                    while (current!=NULL)
                    {
                        strncat(buffer2, (char*) current->data, 32);
                        current = current->next;
                    }
                    n = write(newsockfd[tc], buffer2, sizeof(buffer));
                    if (n < 0) error("ERROR writing to socket");
                    break;
                }
            }
        }
        /// read the name of the recepient client

        bzero(buffer,sizeof(buffer));
        n = read(newsockfd[tc], buffer, sizeof(buffer));
        if (n < 0) error("ERROR reading from socket");

        /// find the recepient client
        current = user_list->head;
        while (current!=NULL)
        {
            if ((char*)current->data==buffer)
            {
                n = write(newsockfd[tc], "\nConnection established, you can start chatting.\n", sizeof(buffer));
                if (n < 0) error("ERROR writing to socket");
                break;
            }
            current = current->next;
        }
        if (current==NULL)
        {
            n = write(newsockfd[tc], "\nUser not listed!\n", sizeof(buffer));
            if (n < 0) error("ERROR writing to socket");
        }

        print_user_list(user_list); //print all users up to this point
        pthread_create(&threads[tc], NULL, client_thread, &tc);
        tc++;
    }

    /** end program routines */
    for (tc=0; tc<THREADS_MAX; tc++)
    {
        pthread_join (threads[tc], NULL);
        // removes the user from the list
        int i=0;
        dbllist_node_t* current = user_list->head;
        while (current!=NULL)
        {
            if (i==tc) dbllist_remove(user_list, current, 0);
            current = current->next;
            i++;
        }
    }

    pthread_mutex_destroy(&mutex);

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

// thread routine
void* client_thread(void* tc)
{
    int user_c = *(int*) tc;
    int n;

    // critical section
    pthread_mutex_lock(&mutex);
    while (1)
    {
        printf("=== in thread %d ===\n", user_c);

        // read
        bzero(buffer,sizeof(buffer));
        n = read(newsockfd[user_c], buffer, sizeof(buffer));
        if (n < 0)
        {
            fprintf(stderr, "ERROR reading from socket\n");
            pthread_exit(NULL);
        }
        printf("Message received: %s\n",buffer);
        if (strstr(buffer, "exit\n")!=NULL)
        {
            printf("exit received, now leaving client!\n");
            break;
        }
        // write
        //bzero(buffer,sizeof(buffer));
        //fgets(buffer,sizeof(buffer)-1,stdin);
        n = write(newsockfd[user_c], buffer, sizeof(buffer));
        if (n < 0)
        {
            fprintf(stderr, "ERROR writing to socket\n");
            pthread_exit(NULL);
        }
    }
    pthread_mutex_unlock(&mutex);
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
        printf("*\t Online users:\n");
        while (current!=NULL)
        {
            printf("*\t %d. %s", i, (char*) current->data);
            current = current->next;
            i++;
        }
    }
    printf("*\t Total users logged-in: %d\n", list->size);
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}
