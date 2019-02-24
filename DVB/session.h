#ifndef _SESSION_H_
#define _SESSION_H_

#include <pthread.h>
#include "linkedlist.h"
#include "channels.h"
#include "destination.h"
#include "sys/time.h"

#define MAX_PIDS_PER_SESSION 28

enum  { pid_audio,pid_video,pid_pmt,pid_other };

struct session_pid_list_t {
	unsigned short pid;
	char           pid_type;
};


struct session_pid_t {
	struct session_pid_list_t pidList[32];
	int    pidCnt;
};

struct sessions_t {
	linked_list	*sessions_ll;
	pthread_t master_session_thread;
};

struct bandwidth_t {
  struct timeval snap_time;
  long   totalCount;	
};

struct buffer_t {
	volatile char       state; // RUNNING, QUIT etc etc
	pthread_mutex_t     bufferLockMutex;
	char                buffer[1024*1024];
	int                 bufferPtr;
    uint64_t            sessionID;
    struct session_service_t *session; // backwards looking..	
};

struct session_service_t {
	struct dvb_device_t *tuner;
	dvbpsi_t            *pidPMT_dvbpsi;
	uint64_t            sessionID;
	unsigned short		onid;
	unsigned short		channel_id;
	unsigned short		caid;
	struct channels_dvb_t *channel;
	linked_list         *destination_ll;
	int                 outbytes_total;
	struct              session_pid_t pids;
	volatile char       state; 
	unsigned long long  currentBandwidth;
	struct buffer_t     *buffer;
	pthread_t           buffer_thread;
	pthread_t           session_thread;
	struct bandwidth_t  bw;
};


struct sessions_t g_sessions;
int               g_write_time;

void sessions_init();
void session_write(unsigned short pid, unsigned char *data, int len, struct dvb_device_t *vtuner);
int session_stop(uint64_t session_id);


#endif
