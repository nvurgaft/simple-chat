
/** TCP chat server - Client side application */

#include "chat.h"

void error(const char *errmsg);

// arg[1] = localhost, arg[2] = port
int main(int argc, char* argv[])
{

	if (argc !=3) {
		error("invalid number of arguments, ‫client <host_name> <port‬‬>\n");
	}

	struct sockaddr_in serv_addr;
	struct hostent *server_host;
	int sockfd;
	int nbytes, portno;
	char buff[1024];

	/* 	check localhost name, if server and clients are deployed on the same machine
	 *	the local host name should be 'localhost'.
	 */
	if (strcmp(argv[1], "localhost")==0) {
        char hostname[1024];
        hostname[1023] = "\0";
        gethostname(hostname, 1023);
        server_host = gethostbyname(hostname);
        if (server_host==NULL) {
        	error("ERROR unable to resolve hostname");
        }
    }

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(80);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd<0) error("Error on Socket (client side)");

	bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bcopy((char*) server_host->h_addr, (char*) &serv_addr.sin_addr.s_addr, server_host->h_length);
    serv_addr.sin_port = htons(portno);

	// connect to server
	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) <0) {
		error("Error on Connect (client side)\n");
	}
	///////////////////////////
	// the conversation loop //
	///////////////////////////	

	while (1) {

		// read from server
		if ((nbytes = read(sockfd, buff, sizeof(buff))) < 0) {
			error("Error on Read (client side - loop)");
		}
		puts(buff);

		// write to server
		fgets(buff, sizeof(buff), stdin);
		if ((nbytes = write(sockfd, buff, sizeof(buff))) < 0) {
			error("Error on Write (client side - loop)\n");
		}
		
	}
	exit(0);
}

void error(const char *errmsg) {
    perror(errmsg);
    exit(1);
}
