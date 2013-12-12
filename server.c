
/** TCP chat server - Server size application */

#include "chat.h"

void error(const char *errmsg);

int portno;
dbllist_t* user_list;

// signatures
void talk_routine(void*);
void displayUsers(dbllist_t*);
void removeUser(dbllist_t*, char*);

// arg[1] = port number 
int main(int argc, char* argv[])
{
	// check for arg validity
	if (argc != 2) {
		error("port number was not provided, server <port>");
	}

	// declare assets
	struct sockaddr_in serv_addr, cli_addr;
	int sockfd, newfd;
	int nbytes;
	int cli_len = sizeof(cli_addr);
	char buff[1024];
	int assignID = 0;

	user_list = (dbllist_t*) malloc(sizeof(dbllist_t));
    dbllist_init(user_list);

	// declare socket 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd<0) error("Error on Socket (server side)");
	

	bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
	
	printf("server is now running\n");

	// bind socket
	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		error("Error on Bind (server side)");
	}

	// loop here so server could always listen to new incoming connection requests !!

	while(true) { 
		// listen to incoming connection requests
		if (listen(sockfd, 5) < 0) {
			error("Error on Listen (server side)");
		}

		// accept connection request
		newfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);
		if (newfd < 0) {
			error("Error on Accept (server side)");
		}

		//////////////////////////////////////
		// accepting and validate user name //
		//////////////////////////////////////
		
		// sends the newly connected client a welcome message asking his (user)name
		if ((nbytes = write(newfd, "Hello, please enter your username\n", 35)) < 0) {
			error("Error on Write (server side - validating user name)\n");
		}

		do {
			// reads the given user name and checks it for duplicats/if valid
			if ((nbytes = read(newfd, buff, sizeof(buff))) < 0) {
				error("Error on Read (server side - validating user name)");
			}
			printf("user trying to connect: ");
			puts(buff);

			dbllist_node_t* current = user_list->head;
			while (current!=NULL) {		
				printf("comparing %s to %s", current->data, buff);
				if (strcmp(current->data, buff)==0) {
					if ((nbytes = write(newfd, "Username already exists, try again\n", 36)) < 0) {
						error("Error on Write (server side - validating user name)\n");
					}
					break;
				}
				current = current->next;
			}
			// if user list was traversed and a duplicate name was found
			if (current==NULL) {
				printf("new user approved: ");
				puts(buff);

				// when a new user is approved, struct him and add to user list
				user_t* new_user = (user_t*) malloc(sizeof(user_t));
				strncpy(new_user->name, buff, sizeof(new_user->name));
				new_user->name[31] = '\0';
				// attach sockets to struct
				new_user->sockfd[0] = newfd;
				new_user->id = assignID++;
				new_user->talking_to = NULL;
				//new_user->thread = NULL;

				dbllist_append(user_list, new_user);

				char buffer[1024];
				bzero(buffer, sizeof(buffer));

				strcpy(buffer, "Username OK\n\n");
				strcat(buffer, "Online Users:\n\n");

				current = user_list->head;
				while (current!=NULL) {
					strncat(buffer, current->data, sizeof(current->data)+1);
					current = current->next;
				}

				// send the user list to the new client
				if ((nbytes = write(newfd, buffer, sizeof(buffer))) < 0) {
					error("Error on Write (server side - validating user name)\n");
				}
				pthread_create(&new_user->thread, NULL, (void*) talk_routine, (void*) new_user);

				break;
			}

		} while (true);
		// end of iteration
	}

	exit(0);
}



//	*************************
// 	*	thread function		*
//	*************************
// the discussion between users happens here
void talk_routine(void* param) {

	user_t* user = (user_t*) param;

	char buff[1024];
	int nbytes;

	bzero(buff, sizeof(buff));
	
	while (true) {

		// read from client
		if ((nbytes = read(user->sockfd[0], buff, sizeof(buff))) < 0) {
			error("Error on Read (server side - loop)");
		}
		puts(buff);

		// write to client
		fgets(buff, sizeof(buff), stdin);
		if ((nbytes = write(user->sockfd[0], buff, sizeof(buff))) < 0) {
			error("Error on Write (server side - loop)\n");
		}
	}
	pthread_exit(0);
}

//	*****************************
// 	*	additional functions	*
//	*****************************
// takes a dbllist of users, traverses it, displays the users connected and their amount 
void displayUsers(dbllist_t* ulist) {

	dbllist_node_t* current = ulist->head;
	printf("users online: \n");
	while (current!=NULL) {

		printf("%s, ", ((char*) ((user_t*) current->data)->name));
		current = current->next;
	}
	printf("\nnumber of users: %d\n", ulist->size);
}

// removes a user from chat
void removeUser(dbllist_t* ulist, char* uname) {

	dbllist_node_t* current = ulist->head;
	while (current!=NULL) {
		if (((user_t*) current->data)->name == uname) {
			// join his thread
			pthread_join(((user_t*) current->data)->thread, NULL);
			// remove him from list
			dbllist_remove(ulist, current, 1);
			printf("user %s was removed from chat\n", uname);
			break;
		}
		current = current->next;
	}
	
}

void error(const char *errmsg) {
    perror(errmsg);
    exit(1);
}
