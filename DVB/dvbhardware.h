#ifndef _DVB_HARDWARE_H
#define _DVB_HARDWARE_H

#include <stdbool.h>
#include <pthread.h>


#include <stdint.h>
#include <unistd.h>
#include <dvbpsi/dvbpsi.h>
#include <dvbpsi/demux.h>
#include <dvbpsi/psi.h>
#include <dvbpsi/pat.h>
#include <dvbpsi/descriptor.h>
#include <dvbpsi/sdt.h>
#include <dvbpsi/nit.h>
#include <dvbpsi/cat.h>
#include <dvbpsi/dr.h>
#include <dvbpsi/eit.h>



#include <linux/dvb/frontend.h>

#include "linkedlist.h"

int dvb_init(void);


#define MAX_DVB_DEVICES 8
#define MAX_FILTER_PID 32


typedef struct ts_stream_t ts_stream_t;

struct pid_list_t {
	volatile int pid;
	volatile int demuxHandle;
};

enum  { dvb_cable, dvb_t, dvb_t2, dvb_s, dvb_s2 } dvb_types;
enum  { none , idle, scanning , busy ,failed } dvb_state;

struct dvb_device_t {
	 unsigned short    logicalNumber;
	 unsigned short    uid;
	 unsigned short    mode;
	 volatile unsigned int 	   sequence;
	 volatile unsigned int 	   last_sequence;
	 char              frontendDevice[32];
	 char              demuxDevice[32];
	 char              dvrDevice[32];
     char              deviceName[32];
	 unsigned long	   serial;
	 char              initialTuning[256];
	 linked_list	   *frequency_ll;
	 void              *curr_frequency_ptr;
	 int               outFileHandle;
	 
	 unsigned char     dvbType;
	 int               frontendHandle;
	 int               dvrHandle;
	 struct pid_list_t pids[MAX_FILTER_PID];
	 int               lastStat;
	 int               lastber;
	 volatile int      currentState;
	 int			   refcnt;
   
     dvbpsi_t          *pid0_dvbpsi;
	 dvbpsi_t          *pid1_dvbpsi; // CAT Table
     dvbpsi_t          *pid11_dvbpsi;
	 dvbpsi_t          *pid12_dvbpsi;  
	 dvbpsi_t          *pid10_dvbpsi;
	 struct dvb_frontend_parameters fe_params;
	 pthread_mutex_t    tunerLockMutex;
     pthread_t         hardware_thread;	 
	
};


//# T freq      bw   fec_hi fec_lo mod   transmission-mode guard-interval hierarchy
//  T 641833000 8MHz 3/4    NONE   QAM16 2k                1/32           NONE
struct dvb_frequency_dvbt_t {
	int    onLogicalDevice;
	int    id;
	int    type;
	linked_list *channels_ll;
	int    state;
//=======================//	
	long   frequency;
	char   bandwidth[10];
	char   fec_hi[10];
	char   fec_lo[10];
	char   modulation[10];
	char   transmission_mode[10];
	char   guard[10];
	char   heirachy[10];
};

//#T Freq Pol SR FEC
//S 11720000 H 29500000 3/4
struct dvb_frequency_dvbs_t {
	int    onLogicalDevice;
	int    id;
	int    type;
	linked_list *channels_ll;
	int    state;
//===========================//	
	long   frequency;
	char   polarity;
	char   fec[10];
	char   modulation[10];
	long   symbolrate;
};

struct dvb_frequency_base_t {
	int    onLogicalDevice;
	int    id;
	int    type;
	linked_list *channels_ll;
	int    state;
//===========================//	
};


int dvbs_tune(struct dvb_device_t *tuner, struct dvb_frequency_dvbs_t *freq);
int dvbt_tune(struct dvb_device_t *tuner, struct dvb_frequency_dvbt_t *freq);
int AddPID_to_Demux(struct dvb_device_t *tuner, unsigned short pid, char type);
int compare_dvbt_entries(struct dvb_frequency_dvbt_t *entry_A, struct dvb_frequency_dvbt_t *entry_B);
int compare_dvbs_entries(struct dvb_frequency_dvbs_t *entry_A, struct dvb_frequency_dvbs_t *entry_B);
int removePID_from_Demux(struct dvb_device_t *tuner, unsigned short pid);

struct dvb_device_t g_dvb_devices[MAX_DVB_DEVICES];

#endif

