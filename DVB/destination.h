#ifndef _DESTINATION_H_
#define _DESTINATION_H_
#include "linkedlist.h"
#include "utils.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#ifdef USE_GSTREAMER	
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#endif

void destination_init(void);

enum  { dest_none, dest_udp, dest_http, dest_fifo , dest_file , dest_mcast, dest_live, dest_live2};



struct destination_t {
	int  fd;
	pthread_mutex_t  bufferLockMutex;
	unsigned short destid;
	int  destination_type;
	char url_text[1024];
	char *buffer;
	int  bufferSize;
	int  bufptr;
	int  blocksize;
	int  state; // STOPPED, RUNNING, PAUSE
	struct parsed_url *url;
	struct timeval snap_time;
	// UDP
	struct sockaddr_in saddr;
    struct in_addr iaddr;
	char expired;
	
#ifdef USE_GSTREAMER	
    GstPipeline *pipeline;
    GstAppSrc *src;
    GstElement *sink;
    GstElement *decoder;
	GstElement *deinterlace;
    GstElement *colorspace;
    GstElement *xvimagesink;
	GstElement *audiosink;
	GstElement *audioQueue;
	GstElement *audioconvert;
	GstElement *audioresample;
	
	guint gotAudio;
    guint sourceid;
	GMainLoop *loop;
	GstBus *bus;
#endif	
	
};

int destination_close_all(linked_list *ll);
struct destination_t *destination_create(char *inURL);
int destination_getDatafromBuffer(struct destination_t *dest, char *outdata, int reqlen);
int destination_close_type(linked_list *ll, int stream_type);
#endif