#ifndef _CHANNELS_H
#define _CHANNELS_H

#include "dvbhardware.h"

void channel_addPAT(struct dvb_device_t *tuner, int channel_id, int pmt_pid);
void channel_addSDT_Info1(struct dvb_device_t *tuner, int channel_id, int running, int ca_mode);
void channel_addSDT_Info2(struct dvb_device_t *tuner, int channel_id, char *channel_name, char *provider_name ,int service_type, int running, int onid, int ca);
int channel_isProgramValid(struct dvb_device_t *tuner, int onid, int channel_id);
struct dvb_frequency_base_t *channel_getFrequencyForTuner(struct dvb_device_t *tuner, int onid, int channel_id);
struct channels_dvb_t *getChannelByNumber_tuners(struct dvb_device_t *tuner,  int onid, int channel_id);

struct channels_dvb_t {
	unsigned short onid;
	unsigned short service_id;
	unsigned short pmt_pid;
	char           channelName[128];
	char           providerName[128];
	int 	       running_status;
	int			   ca_mode;
	int            service_type;
};


struct channel_search_t {
	int    dvbType; // DVB-T or DVB-S
	struct channels_dvb_t *channel;
};


#endif