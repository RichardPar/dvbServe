#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dvbhardware.h"
#include "database.h"
#include "channels.h"
#include "utils.h"
#include "telnetservermanager.h"



void channel_addPAT(struct dvb_device_t *tuner, int channel_id, int pmt_pid)
{

	struct dvb_frequency_base_t *base=tuner->curr_frequency_ptr;

	linked_list *ll = base->channels_ll;
	struct channels_dvb_t *channel;
	struct channels_dvb_t *channel_temp;
	linked_iterator itr;
	int hit=0;

	if (tuner->last_sequence != tuner->sequence)
		return;

	if (channel_id == 0)
		return;

	channel=malloc(sizeof(struct channels_dvb_t));
	channel->pmt_pid=pmt_pid;
	channel->service_id = channel_id;
	channel->running_status=0;
	channel->ca_mode=0;
	channel->onid=0;
	channel->service_type=0;
	memset(channel->channelName,0,128);
	memset(channel->providerName,0,128);


	channel_temp = linkedList_create_iterator ( ll , &itr );
	while (channel_temp != NULL) {
		if (channel_temp->service_id == channel_id) {
			hit=1;
			break;
		}

		channel_temp = linkedList_iterate ( ll,&itr );
	}
	linkedList_release_iterator (ll ,&itr );

	if (hit == 1) {
		free(channel);
	} else {

		printf("Added channel_id %4.4x: PMT %4.4x\n",channel_id, pmt_pid);
		linkedList_append ( ll,channel );
	}


	return;
}

void channel_addSDT_Info1(struct dvb_device_t *tuner, int channel_id, int running, int ca_mode)
{

	struct dvb_frequency_base_t *base=tuner->curr_frequency_ptr;
	linked_list *ll = base->channels_ll;
	struct channels_dvb_t *channel_temp;
	linked_iterator itr;

	if (tuner->last_sequence != tuner->sequence)
		return;


	channel_temp = linkedList_create_iterator ( ll , &itr );
	while (channel_temp != NULL) {
		if (channel_temp->service_id == channel_id) {
			channel_temp->ca_mode = ca_mode;
			channel_temp->running_status = running;
			break;
		}
		channel_temp = linkedList_iterate ( ll,&itr );
	}
	linkedList_release_iterator (ll ,&itr );
	return;
}

void channel_addSDT_Info2(struct dvb_device_t *tuner, int channel_id, char *channel_name, char *provider_name ,int service_type, int running, int onid, int ca)
{

	struct dvb_frequency_base_t *base=tuner->curr_frequency_ptr;
	linked_list *ll = base->channels_ll;
	//struct channels_dvb_t *channel;
	struct channels_dvb_t *channel_temp;
	linked_iterator itr;
//	int hit=0;

	if (tuner->last_sequence != tuner->sequence)
		return;

    database_addchannel(tuner->serial,onid,channel_id,channel_name); 

	//printf("Adding %s %s (%4.4X)\n",channel_name, provider_name, channel_id);
	channel_temp = linkedList_create_iterator ( ll , &itr );
	while (channel_temp != NULL) {
		if (channel_temp->service_id == channel_id) {
			strncpy(channel_temp->channelName,channel_name,127);
			
			if (provider_name != NULL)
			   strncpy(channel_temp->providerName,provider_name,127);
			   
			channel_temp->running_status = running;
			channel_temp->service_type=service_type;
			channel_temp->onid=onid;
			channel_temp->ca_mode=ca;
			break;
		}
		channel_temp = linkedList_iterate ( ll,&itr );
	}
	linkedList_release_iterator (ll ,&itr );
	return;
}


int getChannelByNumber_addRef(int onid, int channel_id)
{
	int tunerNumber;
	int session=0;
	struct channels_dvb_t *channel;
	struct dvb_frequency_base_t  *freq_base;
	linked_iterator freq_itr;
	linked_list *freq_ll;
	linked_list *channel_ll;


	linked_iterator channel_itr;


	for (tunerNumber=0; tunerNumber < MAX_DVB_DEVICES; tunerNumber++) {
		// Iterate through the frequencies
		freq_ll = g_dvb_devices[tunerNumber].frequency_ll;
		freq_base = linkedList_create_iterator ( freq_ll , &freq_itr );
		while (freq_base != NULL) {

			channel_ll= freq_base->channels_ll;
			channel = linkedList_create_iterator ( channel_ll , &channel_itr );
			while (channel != NULL) {
				{

					if ((channel->onid == onid) && (channel->service_id) && (g_dvb_devices[tunerNumber].currentState < 2)) {
						// Tuner is not busy... so allocate it and tune.
						// TODO : add checks so that shared tuners can be allocated to ReferenceCnt
						g_dvb_devices[tunerNumber].refcnt++;
						g_dvb_devices[tunerNumber].currentState=busy;

						if ((g_dvb_devices[tunerNumber].dvbType == dvb_t) || (g_dvb_devices[tunerNumber].dvbType == dvb_t2)) {
							dvbt_tune(&g_dvb_devices[tunerNumber],(struct dvb_frequency_dvbt_t *)freq_base);
						} // dvb_t tuning
						if ((g_dvb_devices[tunerNumber].dvbType == dvb_s) || (g_dvb_devices[tunerNumber].dvbType == dvb_s2)) {
							dvbs_tune(&g_dvb_devices[tunerNumber],(struct dvb_frequency_dvbs_t *)freq_base);
						} // dvb_s tuning
						getRandomCharacters(&session,4);

						linkedList_release_iterator (channel_ll ,&channel_itr );
						linkedList_release_iterator (freq_ll ,&freq_itr );
						return session;
					}
					channel = linkedList_iterate ( channel_ll,&channel_itr );
				}
				linkedList_release_iterator (channel_ll ,&channel_itr );
				// End of Channel Iterator
				freq_base = linkedList_iterate ( freq_ll,&freq_itr );
			}

			linkedList_release_iterator (freq_ll ,&freq_itr );
			// End of Frequency Iterator
		}

	}
	return 0;
}


struct channels_dvb_t *getChannelByNumber_tuners(struct dvb_device_t *tuner,  int onid, int channel_id)
{
	struct channels_dvb_t *channel;
	struct dvb_frequency_base_t  *freq_base;
	linked_iterator freq_itr;
	linked_list *freq_ll;
	linked_list *channel_ll;


	linked_iterator channel_itr;

	freq_ll = tuner->frequency_ll;
	freq_base = linkedList_create_iterator ( freq_ll , &freq_itr );
	while (freq_base != NULL) { //CC

		channel_ll= freq_base->channels_ll;
		channel = linkedList_create_iterator ( channel_ll , &channel_itr );
		while (channel != NULL) {
			//BB
			if (((unsigned short)channel->onid == (unsigned short)onid) && ((unsigned short)channel->service_id==(unsigned short)channel_id)) {  //AA
				linkedList_release_iterator (channel_ll ,&channel_itr );
				linkedList_release_iterator (freq_ll ,&freq_itr );
				return channel;
			} //AA
			channel = linkedList_iterate ( channel_ll,&channel_itr );
		} //BB
		linkedList_release_iterator (channel_ll ,&channel_itr );
		freq_base = linkedList_iterate ( freq_ll,&freq_itr );
	} //CC

	linkedList_release_iterator (freq_ll ,&freq_itr );
	// End of Frequency Iterator
	return NULL;
}


int channel_isProgramValid(struct dvb_device_t *tuner, int onid, int channel_id)
{
	int hit = -1;
	struct channels_dvb_t *channel;
	struct dvb_frequency_base_t  *freq_base;
	linked_iterator freq_itr;
	linked_list *freq_ll;
	linked_list *channel_ll;


	linked_iterator channel_itr;

	freq_ll = tuner->frequency_ll;
	freq_base = linkedList_create_iterator ( freq_ll , &freq_itr );
	while (freq_base != NULL) {

		channel_ll= freq_base->channels_ll;
		channel = linkedList_create_iterator ( channel_ll , &channel_itr );

		while (channel != NULL) {
			if ((channel->onid == onid) && (channel->service_id==channel_id)) {
				hit++;
			}
			channel = linkedList_iterate ( channel_ll,&channel_itr );
		}
		linkedList_release_iterator (channel_ll ,&channel_itr );
		// End of Channel Iterator
		freq_base = linkedList_iterate ( freq_ll,&freq_itr );
	}
	linkedList_release_iterator (freq_ll ,&freq_itr );
	// End of Frequency Iterator
	return hit;
}


struct dvb_frequency_base_t *channel_getFrequencyForTuner(struct dvb_device_t *tuner, int onid, int channel_id)
{
	struct channels_dvb_t *channel;
	struct dvb_frequency_base_t  *freq_base;
	linked_iterator freq_itr;
	linked_list *freq_ll;
	linked_list *channel_ll;

	linked_iterator channel_itr;
	freq_ll = tuner->frequency_ll;
	freq_base = linkedList_create_iterator ( freq_ll , &freq_itr );
	while (freq_base != NULL) {
		//CC

		channel_ll= freq_base->channels_ll;
		channel = linkedList_create_iterator ( channel_ll , &channel_itr );

		while (channel != NULL) {
			//BB

			if (((unsigned short)channel->onid == (unsigned short)onid) && ((unsigned short)channel->service_id==(unsigned short)channel_id)) { //AA
				linkedList_release_iterator (channel_ll ,&channel_itr );
				linkedList_release_iterator (freq_ll ,&freq_itr );
				return freq_base;
			} //AA
			channel = linkedList_iterate ( channel_ll,&channel_itr );
		} //BB
		linkedList_release_iterator (channel_ll ,&channel_itr );
		// End of Channel Iterator
		freq_base = linkedList_iterate ( freq_ll,&freq_itr );
	} //CC

	linkedList_release_iterator (freq_ll ,&freq_itr );
	return NULL;
}
