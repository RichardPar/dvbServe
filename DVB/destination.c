#define _GNU_SOURCE

#include <pthread.h>
#include "destination.h"
#include "linkedlist.h"
#include "session.h"
#include "linkedlist.h"
#include "iniparser.h"
#include "configuration.h"
#include "telnetservermanager.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>

void *destination_thread_run(struct buffer_t *buffer);
void destination_processMCAST(struct destination_t *dest, char *indata, int inLen);
void destination_processHTTP(struct destination_t *dest, char *indata, int inLen);
void destination_processUDP(struct destination_t *dest, char *indata, int inLen);
int destination_process(linked_list *ll, char *inData, int inLen);
void destination_destroy(struct destination_t *dest);
void destination_processFILE(struct destination_t *dest, char *indata, int inLen);
int destination_print(linked_list *ll, int fd);
void *destination_show(int fd, char *inString);


int destination_thread_start(struct buffer_t *buffer)
{
	pthread_attr_t attributes;

	pthread_attr_init ( &attributes );
	pthread_attr_setdetachstate ( &attributes, PTHREAD_CREATE_DETACHED );
	pthread_create(&buffer->session->buffer_thread, &attributes, (void *)destination_thread_run,(void *)buffer);
	return 1;
}


void *destination_show(int fd, char *inString)
{
	linked_iterator itr;
	struct session_service_t *session;
	char outString[256];

	session = linkedList_create_iterator ( g_sessions.sessions_ll , &itr );
	while (session) {
		sprintf(outString,"\r\nChannel : %s  (0x%016lx)\r\n",session->channel->channelName, session->sessionID);
		send(fd,outString,strlen(outString),0);
		destination_print(session->destination_ll,fd);
		session = linkedList_iterate ( g_sessions.sessions_ll,&itr );
	}
	linkedList_release_iterator ( g_sessions.sessions_ll,&itr );

	return 0;
}

void *destination_thread_run(struct buffer_t *buffer)
{
	struct session_service_t *session = buffer->session;
	char inTempBuffer[1024*1024];
	int  dataCount;

	while (session->state == 1) {
		pthread_mutex_lock(&session->buffer->bufferLockMutex);
		memcpy(inTempBuffer,session->buffer->buffer,session->buffer->bufferPtr);
		dataCount = session->buffer->bufferPtr;
		session->buffer->bufferPtr = 0;
		pthread_mutex_unlock(&session->buffer->bufferLockMutex);
		destination_process(session->destination_ll, inTempBuffer,dataCount);
		usleep(500);
	}
	pthread_exit(0);
}


int destination_process(linked_list *ll, char *inData, int inLen)
{
	linked_iterator itr;
	struct destination_t *destination;

	destination = linkedList_create_iterator ( ll , &itr );
	while (destination) {
		if (destination->expired > 0) {
			destination_destroy(destination);
			destination = linkedList_iterator_remove_internal ( ll, &itr );
			continue;
		}

		switch (destination->destination_type) {
		case dest_udp :
			destination_processUDP(destination,inData,inLen);
			break;
		case dest_file :
			destination_processFILE(destination,inData,inLen);
			break;
		case dest_http :
			destination_processHTTP(destination,inData,inLen);
			break;
		case dest_mcast :
			destination_processMCAST(destination,inData,inLen);
			break;

		default  :
			break;
		}
		destination = linkedList_iterate ( ll,&itr );
	}
	linkedList_release_iterator ( ll,&itr );

	return 0;
}

void destination_addToBuffer(struct destination_t *dest, char *indata, int inLen)
{

	pthread_mutex_lock(&dest->bufferLockMutex);
	if (inLen+dest->bufptr < dest->bufferSize) {
		memcpy(dest->buffer+dest->bufptr,indata,inLen);
		dest->bufptr += inLen;
	} else {
		printf("Buffer dumped\n");
		dest->bufptr = 0;
	}
	pthread_mutex_unlock(&dest->bufferLockMutex);
	return;
}

int destination_getBufferCount(struct destination_t *dest)
{
	int len;

	if (dest->expired > 0) {
		len = -1;
	} else {
		pthread_mutex_lock(&dest->bufferLockMutex);
		len = dest->bufptr;
		pthread_mutex_unlock(&dest->bufferLockMutex);
	}
	return len;
}

int destination_getDatafromBuffer(struct destination_t *dest, char *outdata, int reqlen)
{
	int len=reqlen;

	if (dest->expired > 0)
		return 0;

	pthread_mutex_lock(&dest->bufferLockMutex);

	if (dest->bufptr >= 188*50) {
		if (len > dest->bufptr)
			len = dest->bufptr;

		memcpy(outdata,dest->buffer,len);
		memmove(dest->buffer,dest->buffer+len,dest->bufptr-len);

		dest->bufptr-=len;
	} else {
		len = 0;
	}
	pthread_mutex_unlock(&dest->bufferLockMutex);
	if (len == 0)
		usleep(500);

	return len;
}


void destination_processHTTP(struct destination_t *dest, char *indata, int inLen)
{
	destination_addToBuffer(dest,indata,inLen);
	// Data gets read out from elsewhere! .. the HTTP server has a reader that shares the buffer
}


void destination_processUDP(struct destination_t *dest, char *indata, int inLen)
{
	destination_addToBuffer(dest,indata,inLen);

	if (dest->fd < 0) {
		dest->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		memset((char *) &dest->saddr, 0, sizeof(dest->saddr));
		dest->saddr.sin_family = AF_INET;
		dest->saddr.sin_port = htons(atoi(dest->url->port));
		inet_aton(dest->url->host, &dest->saddr.sin_addr);
	}

	if ((dest->fd > 0) && (dest->bufptr > 188*4)) {
		int destSize = 188*3;

		int slen=sizeof(dest->saddr);
		int rc = sendto(dest->fd, dest->buffer, destSize, 0, &dest->saddr, slen);
		if (rc >= 0) {
			memmove(dest->buffer,dest->buffer+rc,dest->bufptr-rc); // move buffer down..
			dest->bufptr -= rc;
		}

	}
	return;
}


void destination_processMCAST(struct destination_t *dest, char *indata, int inLen)
{
	unsigned char ttl = 3;
	unsigned char one = 1;

	destination_addToBuffer(dest,indata,inLen);

	if (dest->fd < 0) {

		memset(&dest->saddr, 0, sizeof(struct sockaddr_in));
		memset(&dest->iaddr, 0, sizeof(struct in_addr));

		dest->fd = socket(AF_INET, SOCK_DGRAM, 0);

		dest->saddr.sin_family = PF_INET;
		dest->saddr.sin_port = htons(0); // Use the first free port
		dest->saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind socket to any interface
		bind(dest->fd, (struct sockaddr *)&dest->saddr, sizeof(struct sockaddr_in));
		dest->iaddr.s_addr = INADDR_ANY; // use DEFAULT interface
		// Set the outgoing interface to DEFAULT
		setsockopt(dest->fd, IPPROTO_IP, IP_MULTICAST_IF, &dest->iaddr,sizeof(struct in_addr));
		// Set multicast packet TTL to 3; default TTL is 1
		setsockopt(dest->fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,sizeof(unsigned char));
		// send multicast traffic to myself too
		setsockopt(dest->fd, IPPROTO_IP, IP_MULTICAST_LOOP,&one, sizeof(unsigned char));
		// set destination multicast address
		dest->saddr.sin_family = PF_INET;
		dest->saddr.sin_addr.s_addr = inet_addr(dest->url->host);
		dest->saddr.sin_port = htons(atoi(dest->url->port));
	}

	if ((dest->fd > 0) && (dest->bufptr > 188*10)) {
		int destSize = dest->bufptr;

		int slen=sizeof(dest->saddr);
		int rc = sendto(dest->fd, dest->buffer, destSize, 0, &dest->saddr, slen);
		if (rc >= 0) {
			memmove(dest->buffer,dest->buffer+rc,dest->bufptr-rc); // move buffer down..
			dest->bufptr -= rc;
		}
	}
	return;
}



void destination_processFILE(struct destination_t *dest, char *indata, int inLen)
{
	char outFile[256];
	char write_data = 0;
	struct timeval current_time;


	gettimeofday(&current_time,NULL);
	destination_addToBuffer(dest,indata,inLen);
	if (dest->fd < 0) {
		sprintf(outFile,"/%s/%s",dest->url->host,  dest->url->path);
		terminateCRLF(outFile);

		dest->fd = open(outFile,O_RDWR | O_CREAT | O_DIRECT | O_LARGEFILE ,0666 );
		if (dest->fd < 0) {
			perror("   : Huh?!?!?!  : \n");
			exit(1);
		}
		printf("Writing to file %s on fd %d\n",outFile,dest->fd);
	}

	if (g_write_time > 0) {
		unsigned long diff_ms = time_diff_us(dest->snap_time,current_time ) / 1000; // MSec
		if (diff_ms > g_write_time) {
			write_data=1;
			gettimeofday(&dest->snap_time,NULL);
		}
	} else {
		if (dest->bufptr > (dest->bufferSize-(1024*32)))
			write_data=1;
	}


	if ((dest->fd > 0) && (write_data=1)) {

		int writeLen = (dest->bufptr/4096) * 4096; // get nearest 4096 block
		if (writeLen > 0) {
			int rc = write(dest->fd, dest->buffer, writeLen);
			if (rc < 0) {
				perror("WRITE");
				exit(1);
			}
			memmove(dest->buffer,dest->buffer+rc,dest->bufptr-rc); // move buffer down..
			dest->bufptr -= rc;
		}
	}


	return;
}


int destination_close_type(linked_list *ll, int stream_type)
{
	linked_iterator itr;
	struct destination_t *destination;
	int reap;

	destination = linkedList_create_iterator ( ll , &itr );
	while (destination) {

		reap = 0;
		if (destination->destination_type == stream_type) {
			printf("Found Matching session... expiring now\r\n");
			destination->expired=1;
			reap = 1;
#ifdef USE_GSTREAMER
			//g_main_loop_quit(destination->loop);
			usleep(5000);
#endif
		}
		if (reap == 1) {
			destination_destroy(destination);
			destination = linkedList_iterator_remove_internal ( ll, &itr );
		} else
			destination = linkedList_iterate ( ll,&itr );
				
	}
	
	linkedList_release_iterator ( ll,&itr );
	return 0;
}



int destination_close_all(linked_list *ll)
{
	linked_iterator itr;
	struct destination_t *destination;
	int reap;

	destination = linkedList_create_iterator ( ll , &itr );
	while (destination) {
		destination->expired=1;
		reap = 1;
		switch (destination->destination_type) {
		case dest_fifo :
			close(destination->fd);
			// Remove file
			break;
		case dest_live :
#ifdef USE_GSTREAMER
			//g_main_loop_quit(destination->loop);
			usleep(5000);
			reap = 1;
#endif
			break;
		case dest_live2 :
#ifdef USE_GSTREAMER
			//g_main_loop_quit(destination->loop);
			usleep(5000);
			reap = 1;
#endif
			break;



		default :
			close(destination->fd);   // just a generic close.
		}

		if (reap == 1) {
			destination_destroy(destination);
			destination = linkedList_iterator_remove_internal ( ll, &itr );
		} else
			destination = linkedList_iterate ( ll,&itr );
	}
	linkedList_release_iterator ( ll,&itr );
	return 0;
}


void destination_destroy(struct destination_t *dest)
{
	pthread_mutex_destroy(&dest->bufferLockMutex);
	parsed_url_free(dest->url);
	free(dest->buffer);
	free(dest);
	return;
}

struct destination_t *destination_create(char *inURL)
{
	struct destination_t *dest=NULL;
	char URL[1024];


	dest = malloc(sizeof(struct destination_t));
	memset(dest,0,sizeof(struct destination_t));
	strncpy(URL,inURL,1023);
	strncpy(dest->url_text,URL,1023);

	printf("Destination URL %s\r\n",dest->url_text);
	pthread_mutex_init(&dest->bufferLockMutex,NULL);
	dest->fd = -1;
	dest->bufptr=0;
	dest->state = 0;
	dest->expired = 0;
	getRandomCharacters(&(dest->destid),2);

	dest->url = parse_url(URL);

	if (dest->url != NULL) {
		if (dest->url->scheme != NULL)
			printf("Scheme\t : %s\n",dest->url->scheme);
		if (dest->url->host != NULL)
			printf("Host\t : %s\n",dest->url->host);
		if (dest->url->port != NULL)
			printf("Port\t : %s\n",dest->url->port);
		if (dest->url->path != NULL)
			printf("Path\t : %s\n",dest->url->path);
		if (dest->url->fragment != NULL)
			printf("Frag\t : %s\n",dest->url->fragment);

		dest->bufferSize=0;
		dest->buffer = NULL;
		if (dest->url->scheme != NULL) {
			if (strcmp(dest->url->scheme,"http") == 0) {
				dest->bufferSize = (1024*1024);
				dest->destination_type = dest_http;
			} else if (strcmp(dest->url->scheme,"live") == 0) {
				dest->bufferSize = 1024*1024*2;
				dest->destination_type = dest_live;
			} else if (strcmp(dest->url->scheme,"mcast") == 0) {
				dest->bufferSize = 1024*1024;
				dest->destination_type = dest_mcast;
			} else if (strcmp(dest->url->scheme,"udp") == 0) {
				dest->bufferSize = 1024*1024;
				dest->destination_type = dest_udp;
			} else if (strcmp(dest->url->scheme,"fifo") == 0) {
				dest->bufferSize = 1024;
				dest->destination_type = dest_fifo;
			} else if (strcmp(dest->url->scheme,"file") == 0) {
				dest->bufferSize = 1024*1024;
				dest->destination_type = dest_file;
				dest->buffer = (void *)memalign((size_t)4096,dest->bufferSize);
			}
			if (dest->buffer == NULL) {
				dest->buffer = malloc(dest->bufferSize);
			}
			if (dest->destination_type == dest_live)
			{
				printf("dest->url->path %s\r\n",dest->url->path);
			   	if (strncmp(dest->url->path,"live2",5) == 0)
				{
				   dest->destination_type = dest_live2;
				   printf("------------- setting dest2 -------------\r\n");
				}
			}
			
			
		} // End of NULL check
	} else {
		free(dest);
		return NULL;
	}

	if (dest->bufferSize == 0) {
		printf("Unknown Transport....\r\n");
		free(dest);
		return NULL;
	}

	return dest;
}


int destination_print(linked_list *ll, int fd)
{
	linked_iterator itr;
	struct destination_t *destination;
	char outString[256];

	destination = linkedList_create_iterator ( ll , &itr );
	while (destination) {
		sprintf(outString,"\tSessionID: 0x%4.4x\r\n\tURL: %s\r\n\tState: %d\r\n\tBuffer: %dK of %dK\r\n",destination->destid, destination->url_text,destination->state,destination->bufptr/1024,destination->bufferSize/1024);
		send(fd,outString,strlen(outString),0);
		destination = linkedList_iterate ( ll,&itr );
	}
	linkedList_release_iterator ( ll,&itr );

	return 0;

}


void destination_init(void)
{

	telnet_addcommand(destination_show,"destination.show","show current destinations being served");
}
