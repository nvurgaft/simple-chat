
/**

  @file  chat.h

  @brief A header to apply common interface and language between
        the client and a server sides.


*/

#ifndef CHAT_H_
#define CHAT_H_

/**includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include "dbllist.c"

#define BUFFER_SIZE 1024
#define THREADS_MAX 64

typedef enum {false=0, true} bool;

/** types of messages */
typedef enum {MSG = 1, CMD = 2} msg_type_t;

/** message struct */
struct msgbuf
{
    int msg_type;
    char msg_text[BUFFER_SIZE];
};

/** The user struct */
struct user
{
    char name[32]; // the string name of the client user
    int sockfd; // the socket uased by this user
    int id; // the place in the queue
    bool logged; // is the client logged or not
};

#endif
