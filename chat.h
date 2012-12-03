#ifndef CHAT_H_
#define CHAT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define DEFAULT_PORT 5000

/** types of messages */
typedef enum {MSG = 1, CMD = 2} msg_type_t;

/** message struct */
struct msgbuf
{
    int msg_type;
    char msg_text[1024];
};

#endif
