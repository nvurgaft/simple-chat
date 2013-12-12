/**
  @file  chat.h

  @brief A header file to apply common interface and language between
        the client and a server sides.
*/

#ifndef CHAT_H_
#define CHAT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include "dbllist.c"

#define BUFFER_SIZE 1024
#define THREADS_MAX 64

/** bool */
typedef enum {false=0, true=1} bool;

/** types of messages */
typedef enum {MSG = 0, CMD = 1} msg_type_t;

/** The user struct */
typedef struct user 
{
    char name[32]; 			// this buffer holds the user's name  
    /*
     *	each user client has two(2) sockets, the first socket [0] will be used to make the
     *	connection with the server, when the client will start to chat with the users he 
     *	requested, then the sockfd[0] will be used for READING ONLY, and sockfd[1] for WRITTING ONLY.	
     */
    pthread_t thread;
    int sockfd[2];
    dbllist_t* talking_to; //all the users this client talks to thru the server
	int id;					// id for the client, used for id'ing running threads
} user_t;

/** The messege struct*/
typedef struct msg
{
    char sender[32];
    char receiver[32];
	char buff[1024];
	msg_type_t type;
} msg_t;

#endif