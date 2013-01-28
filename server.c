/** server side */

#include "chat.h"

// function signatures
void *client_thread(void* buffer);
void print_user_list(dbllist_t* list);
void remove_user(dbllist_t* list, user_t* user);
void error(const char *msg);

// globals
pthread_mutex_t mutex;
char buffer[BUFFER_SIZE];
user_t user[THREADS_MAX];
dbllist_t* user_list;
int portno;

int main(int argc, char *argv[])
{
    //check argv validity
    if (argc < 2) error("ERROR no port provided [command format: ./server <port number>]");

    /// declarations
    int sockfd, tc=0;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t threads[THREADS_MAX];

    /// users connected to the server
    user_list = (dbllist_t*) malloc(sizeof(dbllist_t));
    dbllist_init(user_list);

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

    /// init the mutex
    if (pthread_mutex_init(&mutex, NULL)!=0) error("ERROR failed to init mutex");
 
    // the server is always on line and may also act as an echo service
    dbllist_append(user_list, "server");

    while (tc<THREADS_MAX)
    {
        user[tc].id = tc;
        clilen = sizeof(cli_addr);
        user[tc].sockfd[0] = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
        if (user[tc].sockfd[0] < 0) error("ERROR on accept");

        // threading occurs here!
        pthread_create(&threads[tc], NULL, client_thread, &user[tc]);
        
        tc++;
    }

    //////////////////////////
    // end program routines //
    //////////////////////////
    for (tc=0; tc<THREADS_MAX; tc++) // join the threads
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
    
    pthread_mutex_destroy(&mutex); // destroy the mutex

    for (tc=0; tc<THREADS_MAX; tc++) // close all sockets
    {
        close(user[tc].sockfd[0]);
        close(user[tc].sockfd[1]);
        dbllist_destroy(user[tc].talking_to, 0);
        printf("[%s] thread was closed\n", user[tc].name);
    }

    close(sockfd);
    dbllist_destroy(user_list, 0);
    printf("closing server, goodbye!\n");

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * the thread routine/function
 */

void* client_thread(void* usr) 
{ 
    // usr is a user_t struct
    user_t user = *(user_t*) usr;
    int n;
    dbllist_node_t* current;

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    clilen = sizeof(cli_addr);

    char buffer[BUFFER_SIZE], user_buffer[BUFFER_SIZE];
    bzero(buffer,sizeof(buffer));

        /// make temp list of the users the client is requesting connection to
    dbllist_t* request_list = (dbllist_t*) malloc(sizeof(dbllist_t));
    dbllist_init(request_list);

    bzero(buffer,sizeof(buffer));
    n = write(user.sockfd[0], "Hello, please enter your username\n", sizeof(buffer));
    if (n < 0) error("ERROR writing to socket");

    /////////////////////////////////////////////////////////////////////////
    // check to see if the user name isn't already logged in to the server //
    /////////////////////////////////////////////////////////////////////////
    while (1) 
    {
        bzero(user_buffer,sizeof(user_buffer));
        n = read(user.sockfd[0], user_buffer, sizeof(buffer));
        if (n < 0) error("ERROR reading from socket");

        user_buffer[strlen(user_buffer)-1] = 0; //zero the newline terminator at the end of the name for easy handling
            
        dbllist_node_t* current = user_list->head;

        while (current!=NULL)
        {
            // if a matching user name is found
            if (strncmp(current->data, user_buffer, sizeof(current->data))==0) 
            {
                bzero(buffer,sizeof(buffer));
                n = write(user.sockfd[0], "Username already exists, try again\n", sizeof(buffer));
                if (n < 0) error("ERROR writing to socket");
                break; // stop searching 
            }
            current = current->next;
        }
        // if a matching name wasn't found by traversing the list, then we can move on..
        if (current==NULL) {
            strncpy(user.name, user_buffer, 32); // copy the name to the user struct
            printf("a new client has just connected: %s\n",user.name);
            dbllist_append(user_list, (void*) &user);

            bzero(buffer,sizeof(buffer));
            strncpy(buffer, "Username OK\nWho do you want to talk to? Write usernames seperated by ;\nOnline users:\n", sizeof(buffer));
                
            current = user_list->head;
            while (current!=NULL) {
                strncat(buffer, current->data, sizeof(buffer));
                strcat(buffer, "\n");
                current = current->next;
            }

            n = write(user.sockfd[0], buffer, sizeof(buffer));
            if (n < 0) error("ERROR writing to socket");
            print_user_list(user_list);
    
            break; // exit the while(1) loop
            }
        }

        /////////////////////////////////////////////
        // connect the client to whom it requested //
        /////////////////////////////////////////////
        while (1) 
        {
            int clear_flag = 0;

            bzero(buffer,sizeof(buffer));
            n = read(user.sockfd[0], buffer, sizeof(buffer));
            if (n < 0) error("ERROR reading from socket");

            buffer[strlen(buffer)-1] = 0; //zero the newline terminator at the end of the string for easy handling
            
            //split the users in the buffer and append them to a linked list
            char* usr = strtok(buffer, "; ");
            while (usr!=NULL)
            {
                dbllist_append(request_list, usr);
                usr = strtok(NULL, "; ");
            }

            /// make sure all the other clients that this client requests are connected
            char not_connected[1024];
            dbllist_node_t* req_current = request_list->head;

            while (req_current!=NULL)
            {
                dbllist_node_t* users_current = user_list->head; 

                while (users_current!=NULL)
                {
                    if (strncmp(req_current->data, users_current->data, sizeof(req_current->data))!=0)
                    {
                        users_current = users_current->next;
                    }
                    else break; // if a match was found
                }
                if (users_current==NULL)
                {
                    strncat(not_connected, req_current->data, sizeof(not_connected));
                    strncat(not_connected, "\n", 1);
                    clear_flag++;
                }
                req_current = req_current->next;
            }
            
            // if a one or more requested clients are not connected
            if (clear_flag>0)
            {
                bzero(buffer,sizeof(buffer));
                strncat(buffer, "\nDo(es) not exist, do you want to try another user/s? (y/n)\n", sizeof(buffer));
                n = write(user.sockfd[0], buffer, sizeof(buffer));
                if (n < 0) error("ERROR writing to socket");

                bzero(buffer,sizeof(buffer));
                n = read(user.sockfd[0], buffer, sizeof(buffer));
                if (n < 0) error("ERROR reading from socket");
                if ((buffer[0]=='n') || (buffer[0]=='N')) break;
            }
            else { // everyone are connected
                user.talking_to = (dbllist_t*) malloc(sizeof(dbllist_t));
                dbllist_init(user.talking_to);
                
                dbllist_node_t* users_current = user_list->head;
                while (users_current!=NULL)
                {
                    current = request_list->head;
                    while (current!=NULL)
                    {
                        if ((strncmp(users_current->data, current->data, sizeof(users_current->data)))==0) {
                            dbllist_append(user.talking_to, users_current);
                        }
                        current = current->next;
                    }
                    users_current = users_current->next;
                }
                break;
            }
        }
        
        n = write(user.sockfd[0], "\nConnection established, you can start chatting.\n", sizeof(buffer));
        if (n < 0) error("ERROR writing to socket");
        
        ///////////////////////////////////
        // connect the other socket here //
        ///////////////////////////////////

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) error("ERROR opening socket");

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno+1);

        if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");

        listen(sockfd,5);

        user.sockfd[1] = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (user.sockfd[1] < 0) error("ERROR on accept");

        dbllist_destroy(request_list, 0); // we don't need this list anymore for this client
                    
        ////////////////////////////////////
        // client talks to others in here // sock[0] is rdonly, sock[1] is wrtonly
        ////////////////////////////////////      

        printf("A list of all the connected users to this point ... \n");
        print_user_list(user_list); //print on screen all the logged users up to this time

    // critical section
    while (1)
    {
        // read
        bzero(buffer,sizeof(buffer));
        n = read(user.sockfd[0], buffer, sizeof(buffer));
        if (n < 0)
        {
            fprintf(stderr, "ERROR reading from socket\n");
            pthread_exit(NULL);
        }
        printf("Message received: %s\n",buffer);
        if (strstr(buffer, "exit\n")!=NULL) // exit code
        {
            remove_user(user_list, &user); // removes the user from all lists

            strcat(buffer, "User ");
            strncat(buffer, user.name , sizeof(user.name));
            strcat(buffer, " is no longer connected");

            n = write(user.sockfd[1], buffer, sizeof(buffer));
            if (n < 0)
            {
                fprintf(stderr, "ERROR writing to socket\n");
                pthread_exit(NULL);
            }
            break;
        }
        // write to all connected
        bzero(buffer,sizeof(buffer));
        fgets(buffer,sizeof(buffer)-1,stdin);
        
        current = user.talking_to->head;
        while (current!=NULL) {
            n = write(((user_t*) current->data)->sockfd[1], buffer, sizeof(buffer));
            if (n < 0)
            {
                fprintf(stderr, "ERROR writing to socket\n");
                pthread_exit(NULL);
            }
            current = current->next;
        }
    }
    pthread_exit(NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * this function traverses the users list and prints them
 */

void print_user_list(dbllist_t* list) {
    if (list->head==NULL) printf("nothing to print, no users are logged in\n");
    else {
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
    //printf("*\t Total users logged-in: %d\n", list->size);
}

/*
 * the error handler function, since we aren't doing anything fancy with our errors
 * we just print it and terminate the application.
 */

void error(const char *msg) {
    perror(msg);
    exit(1);
}

/*
* removes a node (a user) from existance within the list, it does it by steps
* 1. remove the argumented user from the list
* 2. traverse the other users and remove the user from their talk_to lists 
*/

void remove_user(dbllist_t* list, user_t* user) {

    dbllist_node_t* icurr = list->head;
    dbllist_node_t* jcurr = ((user_t*) icurr->data)->talking_to->head;

    while(icurr!=NULL) 
    {
        if (strncmp(((user_t*) icurr->data)->name, user->name, sizeof(user->name))==0)
        {
            dbllist_remove(list, icurr, 0);
        } else {
            while(jcurr!=NULL) 
            {
                if (strncmp(((user_t*) jcurr->data)->name, user->name, sizeof(user->name))==0)
                    dbllist_remove(list, jcurr, 0);
                jcurr = jcurr->next;
            }
        }
        icurr = icurr->next;
    }
}