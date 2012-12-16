/** server side */

#include "chat.h"

void *client_thread(void* buffer);
void print_user_list(dbllist_t* list);
void error(const char *msg);

pthread_mutex_t mutex;
char buffer[BUFFER_SIZE];
user_t user[THREADS_MAX];

int main(int argc, char *argv[])
{
    //check argv validity
    if (argc < 2) error("ERROR no port provided [command format: ./server <port number>]");

    /// declarations
    int portno, n, sockfd, tc=0;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t threads[THREADS_MAX];

    /// users connected to the server
    dbllist_t* user_list = (dbllist_t*) malloc(sizeof(dbllist_t));
    dbllist_init(user_list);

    /// make temp list of the users the client is requesting connection to
    dbllist_t* res_list = (dbllist_t*) malloc(sizeof(dbllist_t));
    dbllist_init(res_list);

    /// start main server routines
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");
    listen(sockfd,5);

    /// This is where we start to thread out the different clients to be connected to the server
    if (pthread_mutex_init(&mutex, NULL)!=0) error("ERROR failed to init mutex");

    dbllist_node_t* current = user_list->head;
    dbllist_node_t* rcurrent;
    // the server is always on line and may also act as a recepient
    dbllist_append(user_list, "server\n");

    while (tc<THREADS_MAX)
    {
        char buffer2[1024];
        char user_buffer[1024];
        bzero(buffer2,sizeof(buffer2));

        clilen = sizeof(cli_addr);
        user[tc].sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (user[tc].sockfd < 0) error("ERROR on accept");

        bzero(buffer,sizeof(buffer));
        n = write(user[tc].sockfd, "Hello, please enter your username\n", sizeof(buffer));
        if (n < 0) error("ERROR writing to socket");

        /// read, logs user name
        bzero(user_buffer,sizeof(user_buffer));
        n = read(user[tc].sockfd, user_buffer, sizeof(buffer));
        if (n < 0) error("ERROR reading from socket");

        strcpy(user[tc].name, user_buffer);

        printf("a new client has just connected: %s\n",buffer);

        /// traverse the list to see if the user name already exists
        current = user_list->head;
        while (current!=NULL)
        {
            if (memcmp(current->data, user_buffer, sizeof(current->data))==0)
            {
                bzero(buffer,sizeof(buffer));
                n = write(user[tc].sockfd, "Username already exists, try again\n", sizeof(buffer));
                if (n < 0) error("ERROR writing to socket");

                while (memcmp(current->data, user_buffer, sizeof(current->data))==0) 
                {
                    n = read(user[tc].sockfd, user_buffer, sizeof(buffer));
                    if (n < 0) error("ERROR reading from socket");

                    bzero(buffer,sizeof(buffer));
                    n = write(user[tc].sockfd, "Username already exists, try again\n", sizeof(buffer));
                    if (n < 0) error("ERROR writing to socket");
                
                }
                break;
            }
            current = current->next;
        }
        //if the list was traversed and the user name not found or the list was empty to begin with
        if (current==NULL)
        {
            dbllist_append(user_list, (void*) &user[tc]);
            current = user_list->head;

            bzero(buffer,sizeof(buffer));
            strcpy(buffer2, "Username OK\nWho do you want to talk to? Write usernames seperated by ;\nOnline users:\n");
            while (current!=NULL)
            {
                strncat(buffer2, (char*) current->data, sizeof(buffer2));
                current = current->next;
            }
            n = write(user[tc].sockfd, buffer2, sizeof(buffer2));
            if (n < 0) error("ERROR writing to socket");
            print_user_list(user_list);
        }

        /// read the names of the target recepients
        while (1) 
        {
            bzero(buffer,sizeof(buffer));
            n = read(user[tc].sockfd, buffer, sizeof(buffer));
            if (n < 0) error("ERROR reading from socket");
            
            char* usr = strtok(buffer, ";");

            while (usr!=NULL)
            {
                dbllist_append(res_list, usr);
                usr = strtok(NULL, ";");
            }

            /// find the recepient clients
            current = user_list->head;
            rcurrent = res_list->head;
            while (current!=NULL)
            {
                if (memcmp(current->data, buffer, sizeof(buffer))==0)
                {
                    while (rcurrent!=NULL)
                    {
                        dbllist_node_t* node = rcurrent;
                        while (memcmp(current->data, node->data, 32!=0))
                        {
                            node = node->next;
                        }
                        if (node==NULL)
                        {
                            dbllist_remove(res_list, rcurrent, 0);
                        }
                        rcurrent = rcurrent->next;
                    }

                    n = write(user[tc].sockfd, "\nConnection established, you can start chatting.\n", sizeof(buffer));
                    if (n < 0) error("ERROR writing to socket");
                    break;
                    
                    for (rcurrent = res_list->head; rcurrent!=NULL; rcurrent = rcurrent->next)
                    {
                        //place talking to other clients here
                    }
                }
                current = current->next;
            }
            if (current==NULL)
            {
                n = write(user[tc].sockfd, "\nDo(es) not exist, do you want to try another user/s? (y/n)\n", sizeof(buffer));
                if (n < 0) error("ERROR writing to socket");

                bzero(buffer,sizeof(buffer));
                n = read(user[tc].sockfd, buffer, sizeof(buffer));
                if (n < 0) error("ERROR reading from socket");
                if ((buffer[0]=='n') || (buffer[0]=='N')) break;

            }
            else break;
        }
        printf("All the connected users to this point ... \n");
        print_user_list(user_list); //print on screen all the logged users up to this time
        //pthread_create(&threads[tc], NULL, client_thread, &tc);
        tc++;
    }

    /** end program routines */
    for (tc=0; tc<THREADS_MAX; tc++)
    {
        //pthread_join (threads[tc], NULL);
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
        close(user[tc].sockfd);
    }

    close(sockfd);
    dbllist_destroy(user_list, 0);
    dbllist_destroy(res_list, 0);
    printf("closing server, goodbye!\n");

    return 0;
}

// the thread routine/function
void* client_thread(void* tc) {
    int ttc = *(int*) tc;
    int n;

    // critical section
    pthread_mutex_lock(&mutex);
    while (1)
    {
        printf("=== in thread %d ===\n", ttc);

        // read
        bzero(buffer,sizeof(buffer));
        n = read(user[ttc].sockfd, buffer, sizeof(buffer));
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
        bzero(buffer,sizeof(buffer));
        fgets(buffer,sizeof(buffer)-1,stdin);
        n = write(user[ttc].sockfd, buffer, sizeof(buffer));
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
            printf("*\t %d. %s", i, ((user_t*) current->data)->name);
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