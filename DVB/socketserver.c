#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <fcntl.h>

#include "socketserver.h"

static void FD_SET_Handle (fd_set *master, int *fdHandle,int *fdMax );
static void FD_CLR_Handle (fd_set *master,int *fdHandle );

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int socketserver_main(struct socket_server_t *inSocketServer) {
	struct socket_server_t *parameters;
	pthread_t socket_thread;

    pthread_attr_t attributes;
	pthread_attr_init ( &attributes );
	pthread_attr_setdetachstate ( &attributes, PTHREAD_CREATE_DETACHED );
	parameters = malloc ( sizeof ( struct socket_server_t ) );
	memcpy ( parameters, inSocketServer, sizeof ( struct socket_server_t ) );

	printf("Cloning settings...\n");
	pthread_create ( &socket_thread, &attributes, socketserver_main_thread, parameters );

    return 1;
}

void *socketserver_main_thread(void *param) {

	struct socket_server_t *parameters = param;

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	struct sockaddr_in myaddr;
	int fdmax;        // maximum file descriptor number

	int listener;     // listening socket descriptor
	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	struct timeval tv;
	char buf[256];    // buffer for client drecvata
	int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

//	int yes=1;        // for setsockopt() SO_REUSEADDR, below
	int i;

	printf("Starting server on port %d\n",parameters->port);

	FD_ZERO(&master);    // clear the master and temp sets
	FD_ZERO(&read_fds);

	/*
		// get us a socket and bind it
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		if ((rv = getaddrinfo(NULL,"", &hints, &ai)) != 0) {
			fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
			goto quit;
		}
	*/
	if ( ( listener = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
		perror ( "socket" );
		goto quit;
	}
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons ( parameters->port );
	memset ( & ( myaddr.sin_zero ), '\0', 8 );
	if ( bind ( listener, ( struct sockaddr * ) &myaddr, sizeof ( myaddr ) ) == -1 ) {
		perror ( "bind" );
		goto quit;
	}

	fdmax = listener;
	if (parameters->init != NULL) {
		parameters->init(parameters->port,&master,&fdmax);
	}
	// listen
	if (listen(listener, 10) == -1) {
		perror("listen");
		goto quit;
	}
	fcntl ( listener, F_SETFL, O_NONBLOCK );
	// add the listener to the master set
	FD_SET_Handle (&master,&listener,&fdmax );
	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	// main loop
	for(;;) {
		tv.tv_sec = 0;
		tv.tv_usec = 500000;
		read_fds = master; // copy it

		int rv = select(fdmax+1, &read_fds, NULL, NULL, &tv);
		if (rv == -1) {
			perror("select");
			continue;
		} else if (rv == 0) {
			if (parameters->housekeeping != NULL) {
				parameters->housekeeping();
			}
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
				if (i == listener) {
					// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(listener,
					               (struct sockaddr *)&remoteaddr,
					               &addrlen);

					if (newfd == -1) {
						perror("accept");
					} else {
						FD_SET_Handle (&master,&newfd,&fdmax );

						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}
						printf("selectserver: new connection from %s on "
						       "socket %d\n",
						       inet_ntop(remoteaddr.ss_family,
						                 get_in_addr((struct sockaddr*)&remoteaddr),
						                 remoteIP, INET6_ADDRSTRLEN),
						       newfd);

						fcntl ( newfd, F_SETFL, O_NONBLOCK );	// non blocking port
						if (parameters->connect != NULL) {
							char ip[20];
							sprintf(ip,"%s",inet_ntop(remoteaddr.ss_family,
							                          get_in_addr((struct sockaddr*)&remoteaddr),
							                          remoteIP, INET6_ADDRSTRLEN));
							int rc = parameters->connect(newfd,ip,parameters->port);
							if (rc > 0) {
								close(newfd);
								continue; // Cycle back to the start
							}
						}
					}
				} else {
					// handle data from a client
//					int buflen = 100; // sizeof buf
					if ((nbytes = recv(i, buf,1, 0)) <= 0) {
						// got error or connection closed by client
						if (nbytes == 0) {
							// connection closed
							printf("selectserver: socket %d hung up\n", i);
							if ( parameters->disconnect != NULL ) {
								parameters->disconnect ( i, parameters->port );
							}
						} else {
							perror("recv");
						}
						close(i); // bye!
						FD_CLR_Handle(&master,&i); // remove from master set
					} else {
						if (parameters->process != NULL) {
							int rc = parameters->process ( i, ( unsigned char * ) buf, nbytes, parameters->port );
							if (rc == -255) {
								if ( parameters->disconnect != NULL ) {
									parameters->disconnect ( i, parameters->port );
								}
								close(i); // bye!
								FD_CLR_Handle(&master,&i); // remove from master set
							}
						}
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

quit:
	free(parameters);
	pthread_exit(NULL);
}


static void FD_SET_Handle (fd_set *master, int *fdHandle,int *fdMax ) {
	printf("FD_SET on handle %d\n",*fdHandle);
	FD_SET ( *fdHandle,master );
	if ( *fdHandle > *fdMax )
		*fdMax = *fdHandle;
}

static void FD_CLR_Handle (fd_set *master,int *fdHandle ) {
	printf("FD_CLR handle %d\n",*fdHandle);
	FD_CLR ( *fdHandle,master );
}
