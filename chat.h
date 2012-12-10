
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

#define MAX_THREADS 64

typedef enum {TRUE=1, FALSE=0} bool_t;

/** types of messages */
typedef enum {MSG = 1, CMD = 2} msg_type_t;

/** message struct */
struct msgbuf
{
    int msg_type;
    char msg_text[1024];
};

/** user struct */
struct user
{
    char name[32];
    int id;
    bool_t logged;
};

#endif
