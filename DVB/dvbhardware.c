//#include <event.h>
#include <sys/ioctl.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/poll.h>
#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>
#include <fcntl.h>
#include <linux/falloc.h>


#include "iniparser.h"
#include "configuration.h"
#include "dvbhardware.h"
#include "database.h"
#include "utils.h"
#include "telnetservermanager.h"
#include "channels.h"
#include "session.h"

//#define SLOF (11500*1000UL)
#define SLOF (11700*1000UL)
#define LOF1 (9750*1000UL)
#define LOF2 (10600*1000UL)


static char *bandwidth_ISO[] = { "8MHz\0","7MHz\0","6MHz\0","5MHz\0" };
static char *constellation_ISO[] = { "QPSK\0","QAM16\0","QAM64\0" };
static char *constellation_Satellite_ISO[] = { "AUTO\0","QPSK\0","PSK8\0", "QAM16\0" };
static char *fec_ISO[] = {"1/2\0","2/3\0","3/4\0","5/6\0","7/8\0"};
static char *hor_vert_ISO[] = {"H","V"};


void dvb_getTunerConfigs(void);
void *dvb_telnetDVBDevices(int fd, char *inString);
int initDVBDevice(struct dvb_device_t *tuner);
void addInitialData_dvbt(linked_list *ll, char *intext);
void loadInitialFile(struct dvb_device_t *tuner);
int compare_dvbt_entries(struct dvb_frequency_dvbt_t *entry_A, struct dvb_frequency_dvbt_t *entry_B);
void dvb_start_hardware_thread(struct dvb_device_t *tuner);
void *dvb_hardware_thread(void *vtuner);
int *dvb_select_callback(int fd,void *ptr);
unsigned char dvb_readstatus(struct dvb_device_t *tuner);
int dvb_readBer(struct dvb_device_t *tuner);
struct dvb_frequency_dvbt_t *getInitialTuning_dvbt(struct dvb_device_t *tuner,int id);
int bandwidth_string_to_number(char *bandwidth);
int AddPID_to_Demux(struct dvb_device_t *tuner, unsigned short pid, char type);
void *dvb_telnetShowTunerFrequency(int fd, char *inString);
void *dvb_telnetShowTunerChannels(int fd, char *inString);
static void message(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg);
static void process_pat(void* p_zero, dvbpsi_pat_t* p_pat);
static void process_subtable_pid11(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,void * p_zero);
static void process_sdt(void* p_zero, dvbpsi_sdt_t* p_sdt);
void addInitialData_dvbs(linked_list *ll, char *intext);
int fec_string_to_number(char *fec);
int modulation_to_number(char *modulation);
int compare_dvbs_entries(struct dvb_frequency_dvbs_t *entry_A, struct dvb_frequency_dvbs_t *entry_B);
void DumpDescriptors_sdt(const char* str, dvbpsi_descriptor_t* p_descriptor, dvbpsi_sdt_service_t* p_service , void *p_zero, int onid);
struct dvb_frequency_dvbs_t *getInitialTuning_dvbs(struct dvb_device_t *tuner, int id);
static void process_subtable_pid10(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,void * p_zero);
static void process_nit(void* p_zero, dvbpsi_nit_t* p_nit);
void addSatelliteFrequency(struct dvb_device_t *tuner,unsigned int frequency, unsigned int sr, int pol, int modulation);
static void process_cat(void* p_zero, dvbpsi_cat_t* p_cat);
static void process_subtable_pid12(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,void * p_zero);
int dvb_read_data(int fd,void *ptr);


int dvb_init()
{
	dvb_getTunerConfigs();

	telnet_addcommand(dvb_telnetDVBDevices,"dvb.showdevice","Shows all DVB devices");
	telnet_addcommand(dvb_telnetShowTunerFrequency,"dvb.showfreq","Shows all valid frequencies for tuner");
	telnet_addcommand(dvb_telnetShowTunerChannels,"dvb.showchannels","Shows all channels");



        printf("%s EXIT\n",__FUNCTION__);
	return 0;
}

void dvb_getTunerConfigs(void)
{
	char *inString;
	dictionary  *   ini ;
	char sectionName[32];
	int loop;
	int loop2;

	memset(g_dvb_devices,sizeof(struct dvb_device_t)*MAX_DVB_DEVICES,0);
	ini = iniparser_load(G_mainConfigFile);
	if (ini==NULL) {
		printf("Cannot parse file: %s\n", G_mainConfigFile);
	} else {


		for (loop=0; loop<MAX_DVB_DEVICES; loop++) {
			sprintf(sectionName,"tuner_%d:frontend",loop);
			inString=iniparser_getstring(ini,sectionName,NULL);
			if (inString != NULL)
				strncpy(g_dvb_devices[loop].frontendDevice,inString,32);
			
			sprintf(sectionName,"tuner_%d:dvr",loop);
			inString=iniparser_getstring(ini,sectionName,NULL);
			if (inString != NULL)
				strncpy(g_dvb_devices[loop].dvrDevice,inString,32);

			sprintf(sectionName,"tuner_%d:demux",loop);
			inString=iniparser_getstring(ini,sectionName,NULL);
			if (inString != NULL)
				strncpy(g_dvb_devices[loop].demuxDevice,inString,32);

			sprintf(sectionName,"tuner_%d:initialtuning",loop);
			inString=iniparser_getstring(ini,sectionName,NULL);
			if (inString != NULL)
				strncpy(g_dvb_devices[loop].initialTuning,inString,256);


			g_dvb_devices[loop].logicalNumber = loop;
			g_dvb_devices[loop].refcnt=0;
			g_dvb_devices[loop].currentState = none;
			g_dvb_devices[loop].sequence=0;

			g_dvb_devices[loop].outFileHandle = open("/tmp/outfile.ts",O_WRONLY |O_CREAT,0666);
			g_dvb_devices[loop].frequency_ll = linkedList_create ( NULL );

			g_dvb_devices[loop].pid0_dvbpsi = dvbpsi_new(&message, DVBPSI_MSG_DEBUG);
			dvbpsi_pat_attach(g_dvb_devices[loop].pid0_dvbpsi, process_pat, &g_dvb_devices[loop]);

			g_dvb_devices[loop].pid1_dvbpsi = dvbpsi_new(&message, DVBPSI_MSG_DEBUG);
		    dvbpsi_cat_attach(g_dvb_devices[loop].pid1_dvbpsi, process_cat, &g_dvb_devices[loop]);

			g_dvb_devices[loop].pid11_dvbpsi = dvbpsi_new(&message, DVBPSI_MSG_DEBUG);
			dvbpsi_AttachDemux(g_dvb_devices[loop].pid11_dvbpsi, process_subtable_pid11,&g_dvb_devices[loop]);

			g_dvb_devices[loop].pid10_dvbpsi = dvbpsi_new(&message, DVBPSI_MSG_DEBUG);
			dvbpsi_AttachDemux(g_dvb_devices[loop].pid10_dvbpsi, process_subtable_pid10,&g_dvb_devices[loop]);

			g_dvb_devices[loop].pid12_dvbpsi = dvbpsi_new(&message, DVBPSI_MSG_DEBUG);
			dvbpsi_AttachDemux(g_dvb_devices[loop].pid12_dvbpsi, process_subtable_pid12,&g_dvb_devices[loop]);



			for (loop2=0; loop2 < MAX_FILTER_PID; loop2++) {
				g_dvb_devices[loop].pids[loop2].demuxHandle = open(g_dvb_devices[loop].demuxDevice, O_RDWR|O_NONBLOCK);
				g_dvb_devices[loop].pids[loop2].pid = 65535;
			}
		}
		iniparser_freedict(ini);
	}

	printf("Registered\n");
	for (loop=0; loop<MAX_DVB_DEVICES; loop++) {
		if (strlen(g_dvb_devices[loop].frontendDevice) > 0) {
			
			g_dvb_devices[loop].serial = getDVB_Serial(g_dvb_devices[loop].frontendDevice);
			printf("Serial is %lx\r\n",g_dvb_devices[loop].serial);
			
			printf("Tuner %s\n",g_dvb_devices[loop].frontendDevice);
			initDVBDevice(&g_dvb_devices[loop]);
			loadInitialFile(&g_dvb_devices[loop]);
			dvb_start_hardware_thread(&g_dvb_devices[loop]);
			if ((g_dvb_devices[loop].dvbType == dvb_t) || (g_dvb_devices[loop].dvbType == dvb_t2)) {
				struct dvb_frequency_dvbt_t *it = getInitialTuning_dvbt(&g_dvb_devices[loop],0);
				dvbt_tune(&g_dvb_devices[loop],it);
				//sleep(1);
			} // dvb_t tuning
			if ((g_dvb_devices[loop].dvbType == dvb_s) || (g_dvb_devices[loop].dvbType == dvb_s2)) {
				struct dvb_frequency_dvbs_t *it = getInitialTuning_dvbs(&g_dvb_devices[loop],0);
				dvbs_tune(&g_dvb_devices[loop],it);
				//sleep(1);
			} // dvb_t tuning

			usleep(10000);
			AddPID_to_Demux(&g_dvb_devices[loop],0x0,0);
			AddPID_to_Demux(&g_dvb_devices[loop],0x1,0);
			AddPID_to_Demux(&g_dvb_devices[loop],0x10,0);
			AddPID_to_Demux(&g_dvb_devices[loop],0x11,0);
			AddPID_to_Demux(&g_dvb_devices[loop],0x12,0);
		}

	}

  printf("%s Exit\n",__FUNCTION__);
  return;
}

void *dvb_telnetShowTunerFrequency(int fd, char *inString)
{
	const unsigned char CRLF[] = {10,13};
	char outString[256];
	int loop;
	linked_iterator itr;

	//int rc;
	struct dvb_frequency_dvbt_t *dvbt_initial_temp;

	send(fd,CRLF,2,0);
	for (loop=0; loop<MAX_DVB_DEVICES; loop++) {
		if (strlen(g_dvb_devices[loop].frontendDevice) > 0) {
			sprintf(outString,"Device %d\r\n\tTuner:%s (0x%2.2x)\r\n\tDVR  :%s\r\n\tDemux:%s\r\n",loop,
			        g_dvb_devices[loop].frontendDevice, g_dvb_devices[loop].lastStat,
			        g_dvb_devices[loop].dvrDevice,
			        g_dvb_devices[loop].demuxDevice
			       );
			send(fd,outString,(int)strlen(outString),0);


			dvbt_initial_temp = linkedList_create_iterator ( g_dvb_devices[loop].frequency_ll , &itr );
			while (dvbt_initial_temp != NULL) {
				sprintf(outString,"ID %d\t%luMhz\t%s\t(%d services)\r\n",dvbt_initial_temp->id,
				        (unsigned long)dvbt_initial_temp->frequency,
				        dvbt_initial_temp->modulation,linkedList_item_count(dvbt_initial_temp->channels_ll));

				send(fd,outString,(int)strlen(outString),0);
				dvbt_initial_temp = linkedList_iterate ( g_dvb_devices[loop].frequency_ll,&itr );
			}
			linkedList_release_iterator ( g_dvb_devices[loop].frequency_ll ,&itr );

		}
	}
	return 0;
}


void dvb_telnetPrintChannelList(int fd, linked_list *ll, char mode)
{
	linked_iterator itr;
	struct channels_dvb_t *channel;
	char outStr[1024];
	char channelName[16];

   
	channel = linkedList_create_iterator ( ll , &itr );
	while (channel != NULL) {

		memset(channelName,'.',16);
	    channelName[15]=0;
		if (strlen(channel->channelName) > 15)
		{
		   memcpy(channelName,channel->channelName,13);	
		} else
		   memcpy(channelName,channel->channelName,strlen(channel->channelName)+1);	
			
		sprintf(outStr,"\t%4.4x:%4.4x : %-15s pmt:%4.4x\tCA:%d:Running:%d Type:0x%2.2X\r\n",
		        channel->onid,
				channel->service_id,
		        channelName,
		        channel->pmt_pid,
		        channel->ca_mode,
		        channel->running_status,
				channel->service_type);
				
		if (mode > 0)
		{
		   if ((mode == 1) && (channel->ca_mode == 0))
		   {
		      send(fd,outStr,strlen(outStr),0);
		   } else if ((mode == 2) && (channel->ca_mode == 1))
		   {
			   send(fd,outStr,strlen(outStr),0);
		   }

	  
		} else
		   send(fd,outStr,strlen(outStr),0);
		
		channel = linkedList_iterate ( ll,&itr );
	}
	linkedList_release_iterator ( ll ,&itr );

	return;
}

void *dvb_telnetShowTunerChannels(int fd, char *inString)
{
	const unsigned char CRLF[] = {10,13};
	char outString[256];
	int loop;
	linked_iterator itr;
    char channelName[17];
	char *tokens[2];
	char mode = 0;
	
	int rc = tokenize(tokens,2,inString,' ');
	if (rc >= 2)
	{
    if (!memcmp(tokens[1],"fta",3)) 
         mode = 1;

    if (!memcmp(tokens[1],"enc",3)) 
         mode = 2;
    }

	//int rc;
    memset(channelName,0,17);
	send(fd,CRLF,2,0);
	for (loop=0; loop<MAX_DVB_DEVICES; loop++) {
		if (strlen(g_dvb_devices[loop].frontendDevice) > 0) {
			
			sprintf(outString,"Device %d\r\n\tTuner:%s (0x%2.2x)\r\n\tDVR  :%s\r\n\tDemux:%s\r\n",loop,
			        g_dvb_devices[loop].frontendDevice, g_dvb_devices[loop].lastStat,
			        g_dvb_devices[loop].dvrDevice,
			        g_dvb_devices[loop].demuxDevice
			       );
			send(fd,outString,(int)strlen(outString),0);


			if ((g_dvb_devices[loop].dvbType == dvb_t) || (g_dvb_devices[loop].dvbType == dvb_t2)) {
				struct dvb_frequency_dvbt_t *dvbt_initial_temp;
				dvbt_initial_temp = linkedList_create_iterator ( g_dvb_devices[loop].frequency_ll , &itr );
				while (dvbt_initial_temp != NULL) {
					
					sprintf(outString,"ID %d\t%luMhz\t%s\t(%d services)\r\n",dvbt_initial_temp->id,
					        (unsigned long)dvbt_initial_temp->frequency,
					        dvbt_initial_temp->modulation,linkedList_item_count(dvbt_initial_temp->channels_ll));

					send(fd,outString,(int)strlen(outString),0);
					dvb_telnetPrintChannelList(fd,dvbt_initial_temp->channels_ll,mode);
					dvbt_initial_temp = linkedList_iterate ( g_dvb_devices[loop].frequency_ll,&itr );
				}
				linkedList_release_iterator ( g_dvb_devices[loop].frequency_ll ,&itr );
			}

			if ((g_dvb_devices[loop].dvbType == dvb_s) || (g_dvb_devices[loop].dvbType == dvb_s2)) {
				struct dvb_frequency_dvbs_t *dvbs_initial_temp;

				dvbs_initial_temp = linkedList_create_iterator ( g_dvb_devices[loop].frequency_ll , &itr );
				while (dvbs_initial_temp != NULL) {
					sprintf(outString,"ID %d\t%luMhz %c\t%s\t(%d services)\r\n",dvbs_initial_temp->id,
					        (unsigned long)dvbs_initial_temp->frequency, dvbs_initial_temp->polarity,
					        dvbs_initial_temp->modulation,linkedList_item_count(dvbs_initial_temp->channels_ll));

					send(fd,outString,(int)strlen(outString),0);
					dvb_telnetPrintChannelList(fd,dvbs_initial_temp->channels_ll,mode);
					dvbs_initial_temp = linkedList_iterate ( g_dvb_devices[loop].frequency_ll,&itr );
				}
				linkedList_release_iterator ( g_dvb_devices[loop].frequency_ll ,&itr );
			}
		}
	}
	return 0;
}


void *dvb_telnetDVBDevices(int fd, char *inString)
{
	const unsigned char CRLF[] = {10,13};
	char outString[256];
	int loop;

	send(fd,CRLF,2,0);
	for (loop=0; loop<MAX_DVB_DEVICES; loop++) {
		if (strlen(g_dvb_devices[loop].frontendDevice) > 0) {
			sprintf(outString,"Device %d\r\n\tTuner:%s (0x%2.2x)\r\n\tDVR  :%s\r\n\tDemux:%s\r\n",loop,
			        g_dvb_devices[loop].frontendDevice, g_dvb_devices[loop].lastStat,
			        g_dvb_devices[loop].dvrDevice,
			        g_dvb_devices[loop].demuxDevice
			       );
			send(fd,outString,(int)strlen(outString),0);
		}
	}
	return 0;
}


int initDVBDevice(struct dvb_device_t *tuner)
{
	struct dvb_frontend_info info;
	tuner->dvbType = 255;

    pthread_mutex_init(&tuner->tunerLockMutex,NULL);
	tuner->frontendHandle = open(tuner->frontendDevice,O_RDWR);
	if (tuner->frontendHandle > 0) {
		ioctl(tuner->frontendHandle, FE_GET_INFO, &info);

		switch (info.type) {
		case FE_QPSK:
			tuner->dvbType = dvb_s2;
			break;
		case FE_QAM:
			tuner->dvbType = dvb_cable;
			break;
		default:
		case FE_OFDM:
			tuner->dvbType = dvb_t2;
			break;
		}
		//printf("Frontend %s (Handle %i) (%d)\r\n",tuner->frontendDevice, tuner->frontendHandle, tuner->dvbType);

		tuner->dvrHandle = open(tuner->dvrDevice,O_RDONLY |O_NONBLOCK);
		if (tuner->dvrHandle < 0) {
			perror("Can't open file for dvr test");
			close(tuner->frontendHandle);
			tuner->frontendHandle = -1;
			tuner->dvbType = 255;
		}
		printf("Frontend %s (Handle %i) (%d) (DVR Handle %d)\r\n",tuner->frontendDevice, tuner->frontendHandle, tuner->dvbType,tuner->dvrHandle);
	}
	return tuner->frontendHandle;
}

void loadInitialFile(struct dvb_device_t *tuner)
{
	int type = tuner->dvbType;
	FILE *fp;
	char line[128];

	printf("Loading initial tuning file %s [type=%d]\n",tuner->initialTuning,type);
	fp = fopen (tuner->initialTuning, "r");  /* open the file for reading */
	if (fp == NULL)
		return;

	while(fgets(line, 128, fp) != NULL) {

		if ((type == dvb_t) || (type == dvb_t2)) {
			addInitialData_dvbt(tuner->frequency_ll,line);
		}
		if ((type == dvb_s) || (type == dvb_s2)) {
			addInitialData_dvbs(tuner->frequency_ll,line);
		}


	}
	fclose(fp);

	return;
}


#if 0
void addTerrestrialFrequency(struct dvb_device_t *tuner, struct dvb_terrestrial_delivery_descriptor *dx)
{
	struct dvb_frequency_dvbt_t *dvbtf;
	struct dvb_frequency_dvbt_t *dvbt_initial_temp;
	linked_iterator itr;

	if (tuner->last_sequence != tuner->sequence)
		return;


	linked_list *ll= tuner->frequency_ll;
	int hit;
	int rc;
	int id=0;

	dvbtf = malloc(sizeof(struct dvb_frequency_dvbt_t));
	dvbtf->frequency = dx->centre_frequency;
	dvbtf->state = 0;
	strcpy(dvbtf->modulation,constellation_ISO[dx->constellation]);
	strcpy(dvbtf->bandwidth,bandwidth_ISO[dx->bandwidth]);
	strcpy(dvbtf->fec_hi,fec_ISO[dx->code_rate_hp_stream]);
	strcpy(dvbtf->fec_lo,fec_ISO[dx->code_rate_lp_stream]);

	hit=0;
	dvbt_initial_temp = linkedList_create_iterator ( ll , &itr );
	while (dvbt_initial_temp != NULL) {

		rc = compare_dvbt_entries(dvbtf,dvbt_initial_temp);
		if (rc >= 0) {
			hit=1;
			break;
		}
		dvbt_initial_temp = linkedList_iterate ( ll,&itr );
		id++;
	}
	linkedList_release_iterator ( ll ,&itr );

	if (hit ==0) {
		dvbtf->id = id;
		printf("ID %d : Discovered frequency %u\n",id,(unsigned int)dvbtf->frequency);
		dvbtf->channels_ll = linkedList_create ( NULL );
		linkedList_append ( ll,dvbtf );
	} else
		free(dvbtf);

	return;
}
#endif

#if 1
void addSatelliteFrequency(struct dvb_device_t *tuner,unsigned int infreq, unsigned int sr, int pol,int modulation)
{
	struct dvb_frequency_dvbs_t *dvbsf;
	struct dvb_frequency_dvbs_t *dvbs_initial_temp;
	linked_iterator itr;

	if (tuner->last_sequence != tuner->sequence)
		return;

//    unsigned long frequency=infreq;
//	unsigned long tempfreq = infreq/1000;
	
//	if ((infreq % 100) > 0)
//		tempfreq++;
		
	unsigned long frequency = infreq;
	
	

	linked_list *ll= tuner->frequency_ll;
	int hit;
	int rc;
	int id=0;

	dvbsf = malloc(sizeof(struct dvb_frequency_dvbs_t));
	dvbsf->frequency = frequency;
	dvbsf->symbolrate=sr;
	dvbsf->state = 0;
	dvbsf->polarity = hor_vert_ISO[pol][0];
	strcpy(dvbsf->modulation,constellation_Satellite_ISO[modulation]);

	//printf(">>>>>>> Got Modulation %s (sr %d)<<<<<<,\n",dvbsf->modulation,sr);

	hit=0;
	dvbs_initial_temp = linkedList_create_iterator ( ll , &itr );
	while (dvbs_initial_temp != NULL) {

		rc = compare_dvbs_entries(dvbsf,dvbs_initial_temp);
		if (rc >= 0) {
			hit=1;
			break;
		}
		dvbs_initial_temp = linkedList_iterate ( ll,&itr );
		id++;
	}
	linkedList_release_iterator ( ll ,&itr );

	if (hit ==0) {
		dvbsf->id = id;
		printf("ID %d : Discovered frequency %u\n",id,(unsigned int)dvbsf->frequency);
		dvbsf->channels_ll = linkedList_create ( NULL );
		linkedList_append ( ll,dvbsf );
	} else
		free(dvbsf);

	return;
}
#endif


void addInitialData_dvbt(linked_list *ll, char *intext)
{

	linked_iterator itr;
	char *tokens[20];
	int id = 0;
	int rc, hit;
	struct dvb_frequency_dvbt_t *dvbt_initial;
	struct dvb_frequency_dvbt_t *dvbt_initial_temp;


	if (intext[0]=='#') {
		//comment line .. no one cares
		return;
	}

	rc = tokenize(tokens,20,intext,' ');
	//if (rc != 9) {
	// amount of items mismatched
	//	return;
	//}

	if (tokens[0][0] != 'T') {
		//bail - the entry is not a DVB-T initial file
		return;
	}

//# T freq bw fec_hi fec_lo mod transmission-mode guard-interval hierarchy
//T 641833000 8MHz 3/4 NONE QAM16 2k 1/32 NONE

	dvbt_initial_temp = malloc(sizeof(struct dvb_frequency_dvbt_t));
	dvbt_initial_temp->frequency = atol(tokens[1]);
	dvbt_initial_temp->state = 0;
	strncpy(dvbt_initial_temp->bandwidth,tokens[2],10);
	strncpy(dvbt_initial_temp->fec_hi,tokens[3],10);
	strncpy(dvbt_initial_temp->fec_lo,tokens[4],10);
	strncpy(dvbt_initial_temp->modulation,tokens[5],10);
	strncpy(dvbt_initial_temp->transmission_mode,tokens[6],10);
	strncpy(dvbt_initial_temp->guard,tokens[7],10);
	strncpy(dvbt_initial_temp->heirachy,tokens[8],10);

	hit=0;
	dvbt_initial = linkedList_create_iterator ( ll , &itr );
	while (dvbt_initial != NULL) {

		rc = compare_dvbt_entries(dvbt_initial,dvbt_initial_temp);
		if (rc >= 0) {
			hit=1;
			break;
		}
		dvbt_initial = linkedList_iterate ( ll,&itr );
		id++;
	}
	linkedList_release_iterator ( ll ,&itr );

	if (hit ==0) {
		dvbt_initial_temp->id = id;
		dvbt_initial_temp->channels_ll = linkedList_create ( NULL );
		printf("ID %d : Initial entry added for frequency %u\n",id,(unsigned int)dvbt_initial_temp->frequency);
		linkedList_append ( ll,dvbt_initial_temp );
	} else
		free(dvbt_initial_temp);

	return;
}


void addInitialData_dvbs(linked_list *ll, char *intext)
{

	linked_iterator itr;
	char *tokens[20];
	int id = 0;
	int rc, hit;
	struct dvb_frequency_dvbs_t *dvbs_initial;
	struct dvb_frequency_dvbs_t *dvbs_initial_temp;


	if (intext[0]=='#') {
		//comment line .. no one cares
		return;
	}

	rc = tokenize(tokens,20,intext,' ');

	if (tokens[0][0] != 'S') {
		//bail - the entry is not a DVB-T initial file
		return;
	}

//#T Freq Pol SR FEC
//S 11720000 H 29500000 3/4


	dvbs_initial_temp = malloc(sizeof(struct dvb_frequency_dvbt_t));
	dvbs_initial_temp->frequency = atol(tokens[1]);
	dvbs_initial_temp->state = 0;
	dvbs_initial_temp->symbolrate = atol(tokens[3]);
	strncpy(dvbs_initial_temp->fec,tokens[4],10);
	dvbs_initial_temp->polarity = tokens[2][0];
    dvbs_initial_temp->modulation[0] = 0;

	hit=0;
	dvbs_initial = linkedList_create_iterator ( ll , &itr );
	while (dvbs_initial != NULL) {

		rc = compare_dvbs_entries(dvbs_initial,dvbs_initial_temp);
		if (rc >= 0) {
			hit=1;
			break;
		}
		dvbs_initial = linkedList_iterate ( ll,&itr );
		id++;
	}
	linkedList_release_iterator ( ll ,&itr );

	if (hit ==0) {
		dvbs_initial_temp->id = id;
		dvbs_initial_temp->channels_ll = linkedList_create ( NULL );
		printf("ID %d : Initial entry added for frequency %u\n",id,(unsigned int)dvbs_initial_temp->frequency);
		linkedList_append ( ll,dvbs_initial_temp );
	} else
		free(dvbs_initial_temp);

	return;
}

int compare_dvbt_entries(struct dvb_frequency_dvbt_t *entry_A, struct dvb_frequency_dvbt_t *entry_B)
{
	if (entry_A->frequency == entry_B->frequency) {
		return 0;
	}
	return -1;
}

int compare_dvbs_entries(struct dvb_frequency_dvbs_t *entry_A, struct dvb_frequency_dvbs_t *entry_B)
{
	if ((entry_A->frequency == entry_B->frequency) &&
	    (entry_A->polarity == entry_B->polarity)) {
		return 0;
	}
	return -1;
}


void *dvb_hardware_thread(void *vtuner)
{
	
// 100mSec 		
#define IOCTL_TIME 100000
#define CONT_TIME 500000 / IOCTL_TIME

	struct dvb_device_t *tuner = vtuner;
	int currStat;
	int currber;
	struct pollfd pfd[1];
	struct timeval current_time;
	struct timeval snap_time;
	
	

	tuner->currentState = idle;
	printf("Thread started for tuner %d\n",tuner->uid);

	int id=0;
	int cnt=0;
	
	pfd[0].fd = tuner->dvrHandle;
    pfd[0].events = POLLIN;

	gettimeofday(&current_time,NULL);
	//gettimeofday(&snap_time,NULL);

	while (1) {
		
		int rc = poll(pfd,1,10);
		if (rc > 0)
		{
			dvb_read_data(tuner->dvrHandle,tuner);
		}
		gettimeofday(&current_time,NULL);
		unsigned long diff_ms = time_diff_us(snap_time,current_time ) / 1000; // MSec
		
		if (diff_ms > 500)
		{
		currber = dvb_readBer(tuner); 
		currStat = dvb_readstatus(tuner);
		if (currStat != tuner->lastStat) {
			printf("Tuner Status changed to %2.2X\n",currStat);
			tuner->lastStat = currStat;
		}

		if (currber != tuner->lastber) {
			//printf("BER Changed to %d\n",currber);
			tuner->lastber = currber;
		}
//============================//
		cnt++;
		if ((tuner->lastStat < 0x1F) && (cnt > CONT_TIME * 2))
			cnt = CONT_TIME * 5;
            
		if (cnt >= CONT_TIME * 5) {
			cnt=0;

			if (((tuner->dvbType == dvb_t) || (tuner->dvbType == dvb_t2)) && (tuner->currentState == idle)) {
				struct dvb_frequency_dvbt_t *it = getInitialTuning_dvbt(tuner,id);
				if (it != NULL) {
					id++;
					dvbt_tune(tuner,it);
					if (tuner->currentState == idle)
					{
					AddPID_to_Demux(tuner,0x0,0);
					AddPID_to_Demux(tuner,0x1,0);
					AddPID_to_Demux(tuner,0x10,0);
					AddPID_to_Demux(tuner,0x11,0);
					AddPID_to_Demux(tuner,0x12,0);
					}
				} else
					id=0;

			} else 	if (((tuner->dvbType == dvb_s) || (tuner->dvbType == dvb_s2))  && (tuner->currentState == idle) ) {
				struct dvb_frequency_dvbs_t *it = getInitialTuning_dvbs(tuner,id);
				if (it != NULL) {
					id++;
					int z = dvbs_tune(tuner,it);
					if (z < 0)
						continue;
						
					if (tuner->currentState == idle)
					{
					AddPID_to_Demux(tuner,0x0,0);
					AddPID_to_Demux(tuner,0x1,0);
					AddPID_to_Demux(tuner,0x10,0);
					AddPID_to_Demux(tuner,0x11,0);
					AddPID_to_Demux(tuner,0x12,0);
					}
				} else
					id=0;
			}
		}
//=============================//
			
				gettimeofday(&snap_time,NULL);
		}
	}

	pthread_exit(0);
}

void dvb_start_hardware_thread(struct dvb_device_t *tuner)
{
	int rc;
	pthread_attr_t attributes;
	pthread_attr_init ( &attributes );
	pthread_attr_setdetachstate ( &attributes, PTHREAD_CREATE_DETACHED );
	
	rc = pthread_create(&tuner->hardware_thread, &attributes, dvb_hardware_thread,tuner);
	if (rc)
	{
		printf("TUNER FAILED\r\n");
		tuner->currentState = failed;
	}

	return;
}


int dvb_read_data(int fd,void *ptr)
{
	unsigned char buffer[188*5];
	int rc;
	uint16_t i_pid;
	struct dvb_device_t *tuner = ptr;


	if (tuner->last_sequence != tuner->sequence) {
		tuner->last_sequence = tuner->sequence;
		printf("Resetting Sections buffers\n");
		// fill in reset code here
	}


	rc = read(fd,buffer,188*5);
	
	if (tuner->lastStat < 0x10) // If there is no lock, then DUMP the data!
		return 0;

	//if (tuner->lastber > 500) // BER! yuk!
	//	return 0;

	while (rc > 0) {
		
		i_pid = ((uint16_t)(buffer[1] & 0x1f) << 8) + buffer[2];

		switch (i_pid) {
		    case 0   : dvbpsi_packet_push(tuner->pid0_dvbpsi, buffer);
                               break;
		    case 0x1 : dvbpsi_packet_push(tuner->pid1_dvbpsi, buffer);
			       break;
		    case 0x11: dvbpsi_packet_push(tuner->pid11_dvbpsi, buffer);
			       break;
		    case 0x12 : dvbpsi_packet_push(tuner->pid12_dvbpsi, buffer);
			       break;
		    case 0x10 : dvbpsi_packet_push(tuner->pid10_dvbpsi, buffer);
			       break;
		    default : break;
		}

        session_write(i_pid,buffer,188,tuner);
		rc -=188;
		memmove(buffer,buffer+188,188*4);
	}

	return 0;
}


unsigned char dvb_readstatus(struct dvb_device_t *tuner)
{
	int rc;
	int stat;

	if (tuner->frontendHandle < 0)
		return 0;


	if ( (rc = ioctl(tuner->frontendHandle ,FE_READ_STATUS,&stat) < 0)) {
		perror("FE READ STATUS: ");
		return -1;
	}

#if 0
	if (stat & FE_HAS_SIGNAL)
		printf("FE HAS SIGNAL\n");

	if (stat & FE_HAS_SYNC)
		printf("FE_HAS_SYNC\n");

	if (stat & FE_HAS_LOCK)
		printf("FE_HAS_LOCK\n");

	if (stat & FE_TIMEDOUT)
		printf("FE TIMEDOUT\n");
#endif
	return stat;
}


int dvb_readBer(struct dvb_device_t *tuner)
{
	int rc;
	int ber;

	if (tuner->frontendHandle < 0)
		return 0;

	if ( (rc = ioctl(tuner->frontendHandle,FE_READ_BER, &ber) < 0)) {
		perror("FE READ_BER: ");
		return 0;
	}
	return ber;
}



int dvbt_tune(struct dvb_device_t *tuner, struct dvb_frequency_dvbt_t *freq)
{
	int rc=0;
	int i;

	unsigned int frequency =  freq->frequency;
	char *bandwidth = freq->bandwidth;
	char *fec_hi = freq->fec_hi;
	char *fec_lo = freq->fec_lo;
	char *qam = freq->modulation;

	if (freq == NULL)
		return 1;

	if (frequency != tuner->fe_params.frequency) {
		for (i=0; i<MAX_FILTER_PID; i++) {
			if (tuner->pids[i].demuxHandle > 0) {
				//printf("Clearing DEMUX handle %d\n\r",tuner->pids[i].demuxHandle);
				ioctl(tuner->pids[i].demuxHandle,DMX_STOP);
				//close(tuner->pids[i].demuxHandle);
				//tuner->pids[i].demuxHandle=0;
				tuner->pids[i].pid = 65535;
				usleep(1000);
			}
		}

		printf("Setting tuner %d to frequency %u (BW %s)\n",tuner->uid,frequency, bandwidth);
		tuner->fe_params.frequency = frequency;
		tuner->fe_params.u.ofdm.bandwidth = bandwidth_string_to_number(bandwidth);
		tuner->fe_params.u.ofdm.code_rate_HP = fec_string_to_number(fec_hi);
		tuner->fe_params.u.ofdm.code_rate_LP = fec_string_to_number(fec_lo);
		tuner->fe_params.u.ofdm.constellation = modulation_to_number(qam);
		tuner->fe_params.u.ofdm.transmission_mode = TRANSMISSION_MODE_8K;
		tuner->fe_params.u.ofdm.guard_interval = GUARD_INTERVAL_AUTO;
		tuner->fe_params.u.ofdm.hierarchy_information = HIERARCHY_AUTO;

		rc = ioctl(tuner->frontendHandle, FE_SET_FRONTEND, &tuner->fe_params);
		tuner->sequence++;
		tuner->curr_frequency_ptr = freq;
	} else
		printf("Tuner already on the frequency\n");

	usleep(2000);
	return rc;

}


static int dvbs_tone_voltage(int fd, char polv, int hi_lo)
{
	//fe_sec_voltage_t voltage;

	int pol = 0;

	if (polv=='V')
		pol=1;

	fprintf(stderr, "Setting only tone %s and voltage %dV\n", (hi_lo ? "ON" : "OFF"), (pol ? 13 : 18));

	if(ioctl(fd, FE_SET_VOLTAGE, (pol ? SEC_VOLTAGE_13 : SEC_VOLTAGE_18)) < 0)
		return -1;

	if(ioctl(fd, FE_SET_TONE, (hi_lo ? SEC_TONE_ON : SEC_TONE_OFF)) < 0)
		return -1;

	usleep(15 * 1000);

	return 0;

}

int dvbs_tune(struct dvb_device_t *tuner, struct dvb_frequency_dvbs_t *freq)
{
	int rc=0;
	int i;
//	int tone;
	int hi_lo;
	int base;

	unsigned int frequency =  freq->frequency;
	unsigned int sr = freq->symbolrate;
	char *fec = freq->fec;

	if (freq == NULL)
		return 0;


    pthread_mutex_lock(&tuner->tunerLockMutex);
	usleep(1000);
	if (frequency != tuner->fe_params.frequency) {
		tuner->lastber = 9000;
		printf("Clearing PID filter\n");
		for (i=0; i<MAX_FILTER_PID; i++) {
			if (tuner->pids[i].demuxHandle > 0) {
				ioctl(tuner->pids[i].demuxHandle,DMX_STOP);
				tuner->pids[i].pid = 65535;
			}
		}
		if (frequency < SLOF) {
			tuner->fe_params.frequency=(frequency-LOF1);
			hi_lo = 0;
			base = LOF1;
		} else {
			tuner->fe_params.frequency=(frequency-LOF2);
			hi_lo = 1;
			base = LOF2;
		}

		dvbs_tone_voltage(tuner->frontendHandle,freq->polarity,hi_lo);

		printf("Setting tuner %d to frequency %u (Base %d) Polarity %c (sr %d)\n",tuner->uid,tuner->fe_params.frequency,base,freq->polarity,sr);
		//usleep(1000);
		tuner->fe_params.u.qpsk.symbol_rate = sr;
		tuner->fe_params.u.qpsk.fec_inner = fec_string_to_number(fec);
		rc = ioctl(tuner->frontendHandle, FE_SET_FRONTEND, &(tuner->fe_params));
		tuner->sequence++;
		tuner->curr_frequency_ptr = freq;

	} else
		printf("Tuner already on the frequency\n");
		
	printf("EXIT %s\r\n",__FUNCTION__);
	usleep(100000);
    pthread_mutex_unlock(&tuner->tunerLockMutex);
	return rc;

}

struct dvb_frequency_dvbt_t *getInitialTuning_dvbt(struct dvb_device_t *tuner, int id)
{
	struct dvb_frequency_dvbt_t *itune;
	int last = 0;
	linked_iterator itr;

	itune = linkedList_create_iterator ( tuner->frequency_ll , &itr );
	while ( itune != NULL ) {
		if (id == last)
			break;
		last++;
		itune = linkedList_iterate ( tuner->frequency_ll,&itr );
	}
	linkedList_release_iterator ( tuner->frequency_ll ,&itr );

	return itune;
}

struct dvb_frequency_dvbs_t *getInitialTuning_dvbs(struct dvb_device_t *tuner, int id)
{
	struct dvb_frequency_dvbs_t *itune;
	int last = 0;
	linked_iterator itr;

	itune = linkedList_create_iterator ( tuner->frequency_ll , &itr );
	while ( itune != NULL ) {
		if (id == last)
			break;
		last++;
		itune = linkedList_iterate ( tuner->frequency_ll,&itr );
	}
	linkedList_release_iterator ( tuner->frequency_ll ,&itr );

	return itune;
}


int bandwidth_string_to_number(char *bandwidth)
{
	if (!memcmp(bandwidth,"8MHz",4))
		return BANDWIDTH_8_MHZ;

	if (!memcmp(bandwidth,"7MHz",4))
		return BANDWIDTH_7_MHZ;

	if (!memcmp(bandwidth,"6MHz",4))
		return BANDWIDTH_6_MHZ;

	if (!memcmp(bandwidth,"5MHz",4))
		return BANDWIDTH_5_MHZ;

	return BANDWIDTH_AUTO;
}

int modulation_to_number(char *modulation)
{
	if (!memcmp(modulation,"QPSK",4))
		return QPSK;
	if (!memcmp(modulation,"QAM16",5))
		return QAM_16;
	if (!memcmp(modulation,"QAM32",5))
		return QAM_32;
	if (!memcmp(modulation,"QAM64",5))
		return QAM_64;
	if (!memcmp(modulation,"QAM256",6))
		return QAM_256;

	return QAM_AUTO;
}


int fec_string_to_number(char *fec)
{

	if (!memcmp(fec,"1/2",3))
		return FEC_1_2;
	if (!memcmp(fec,"2/3",3))
		return FEC_2_3;
	if (!memcmp(fec,"3/4",3))
		return FEC_3_4;
	if (!memcmp(fec,"3/5",3))
		return FEC_3_5;
	if (!memcmp(fec,"4/5",3))
		return FEC_4_5;
	if (!memcmp(fec,"5/6",3))
		return FEC_5_6;
	if (!memcmp(fec,"6/7",3))
		return FEC_6_7;
	if (!memcmp(fec,"7/8",3))
		return FEC_7_8;
	if (!memcmp(fec,"8/9",3))
		return FEC_8_9;
	if (!memcmp(fec,"9/10",4))
		return FEC_9_10;
	if (!memcmp(fec,"NONE",4))
		return FEC_NONE;

	return FEC_AUTO;
}


int removePID_from_Demux(struct dvb_device_t *tuner, unsigned short pid)
{
//	struct dmx_pes_filter_params pesFilterParams;
	int dmxpos;
//	int hit=-1;

	for (dmxpos=0; dmxpos < MAX_FILTER_PID; dmxpos++) {
		if (tuner->pids[dmxpos].pid == pid) {
			ioctl(tuner->pids[dmxpos].demuxHandle,DMX_STOP);
			//close(tuner->pids[dmxpos].demuxHandle);
			tuner->pids[dmxpos].pid=65535;
			//tuner->pids[dmxpos].demuxHandle=0;
			break;
		}
	}
  return 0;
}

int AddPID_to_Demux(struct dvb_device_t *tuner, unsigned short pid, char type)
{
	struct dmx_pes_filter_params pesFilterParams;
	int dmxpos;
	int hit=-1;

	pthread_mutex_lock(&tuner->tunerLockMutex);

	for (dmxpos=0; dmxpos < MAX_FILTER_PID; dmxpos++) {
		if (tuner->pids[dmxpos].pid == pid) {
			pthread_mutex_unlock(&tuner->tunerLockMutex);
       		return 1;
			break;
		}
	}

	for (dmxpos=0; dmxpos < MAX_FILTER_PID; dmxpos++) {
		if (tuner->pids[dmxpos].pid == 65535) {
			//tuner->pids[dmxpos].demuxHandle=open(tuner->demuxDevice, O_RDWR|O_NONBLOCK);
			//if (tuner->pids[dmxpos].demuxHandle < 0)
			//{
			//   printf("Failed to open a new DEMUX handle!\n");	
			//   pthread_mutex_unlock(&tuner->tunerLockMutex);
       		//   return 1;
			//}
			hit=dmxpos;
			break;
		}
	}

	if (hit==-1) {
		pthread_mutex_unlock(&tuner->tunerLockMutex);
		return -1;
	}

    int inpid = pid;
	pesFilterParams.pid     = inpid;
	pesFilterParams.input   = DMX_IN_FRONTEND;
	pesFilterParams.output  = DMX_OUT_TS_TAP;
	pesFilterParams.pes_type = DMX_PES_OTHER;
	pesFilterParams.flags   = DMX_IMMEDIATE_START;
	tuner->pids[hit].pid = pid;
	int rc = ioctl(tuner->pids[hit].demuxHandle, DMX_SET_PES_FILTER, &pesFilterParams);
	
	if (rc < 0)
	{
		printf("IOCTL Failed for PID %2.2x to handle %d on slot %d\n",pid,tuner->pids[hit].demuxHandle,hit);
		close(tuner->pids[hit].demuxHandle);
    
	} else
	{
		printf("Added PID %2.2x to handle %d on slot %d\n",pid,tuner->pids[hit].demuxHandle,hit);
    }
    pthread_mutex_unlock(&tuner->tunerLockMutex);

    return 1;
}

static void message(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
{
      return;
	switch(level) {
	case DVBPSI_MSG_ERROR:
		fprintf(stderr, "Error: ");
		break;
	case DVBPSI_MSG_WARN:
		fprintf(stderr, "Warning: ");
		break;
		//case DVBPSI_MSG_DEBUG: fprintf(stderr, "Debug: "); break;
	default: /* do nothing */
		return;
	}
	fprintf(stderr, "%s\n", msg);
}

static void DumpDescriptors_cat(void *p_zero, dvbpsi_descriptor_t* p_descriptor)
{
   while (p_descriptor)
    {	
	  	
	   printf("CAT 0x%02x\r\n", p_descriptor->i_tag);
	   if (p_descriptor->i_tag == 0x09)
	   {
		  dvbpsi_ca_dr_t *p_t =  dvbpsi_DecodeCADr(p_descriptor);
		  if (p_t != NULL)
		  {
			  printf("CAID 0x%4.4X on PID (0x%4.4X)\n",p_t->i_ca_system_id,p_t->i_ca_pid );
		  }
	   }	
      p_descriptor = p_descriptor->p_next;		
	}	
	return;
}

static void process_cat(void* p_zero, dvbpsi_cat_t* p_cat)
{
    DumpDescriptors_cat(p_zero,p_cat->p_first_descriptor);		
	dvbpsi_cat_delete(p_cat);
}

static void process_pat(void* p_zero, dvbpsi_pat_t* p_pat)
{
	dvbpsi_pat_program_t* p_program = p_pat->p_first_program;
	struct dvb_device_t *tuner = p_zero;

//	printf(  "\n");
//	printf(  "New PAT\n");
//	printf(  "  transport_stream_id : %d\n", p_pat->i_ts_id);
//	printf(  "  version_number      : %d\n", p_pat->i_version);
//	printf(  "    | program_number @ [NIT|PMT]_PID\n");
	while(p_program) {
		channel_addPAT(tuner, p_program->i_number,p_program->i_pid );
//		printf("    | %14d @ 0x%x (%d)\n",p_program->i_number, p_program->i_pid, p_program->i_pid);
		p_program = p_program->p_next;
	}
//	printf(  "  active              : %d\n", p_pat->b_current_next);
	dvbpsi_pat_delete(p_pat);
}

static void process_subtable_pid10(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,void * p_zero)
{
	struct dvb_device_t *tuner = p_zero;

	if ((i_table_id == 0x40) || (i_table_id == 0x41)){
		if (!dvbpsi_nit_attach(p_dvbpsi, i_table_id, i_extension, process_nit, tuner))
			fprintf(stderr, "Failed to attach NIT subdecoder\n");
	}

}

static void process_eit( void *p_zero, dvbpsi_eit_t *p_eit )
{
	dvbpsi_eit_event_t *p_evt;
	char    psz_name[64];
	char    psz_text[256];

	unsigned short channel_id = p_eit->i_extension;
	unsigned short onid = p_eit->i_network_id;

	for( p_evt = p_eit->p_first_event; p_evt; p_evt = p_evt->p_next ) {

		int64_t i_start;
		int i_duration;

		i_start = EITConvertStartTime( p_evt->i_start_time );


		dvbpsi_descriptor_t *p_dr;
		for( p_dr = p_evt->p_first_descriptor; p_dr; p_dr = p_dr->p_next ) {
			if (p_dr->i_tag == 0x4D) {
				dvbpsi_short_event_dr_t *pE = dvbpsi_DecodeShortEventDr( p_dr );
                
				memset(psz_name,0,64);
				memcpy(psz_name,pE->i_event_name,pE->i_event_name_length);

				memset(psz_text,0,256);
				memcpy(psz_text,pE->i_text,pE->i_text_length);

				printf("Channel: %4.4x-onid %4.4x - Short event lang=%3.3s '%s' : '%lu'\n", channel_id, onid,pE->i_iso_639_code, psz_name, (unsigned long)i_start );
				//free( psz_name );
				//free( psz_text );
				database_addevent(onid,channel_id,(unsigned long)i_start,0,psz_name,psz_text,"eng" );

				
			}
		} // subloop
	}

	dvbpsi_eit_delete( p_eit );
	return;
}


static void process_subtable_pid12(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,void * p_zero)
{
	struct dvb_device_t *tuner = p_zero;

	if (i_table_id == 0x4E) {
		dvbpsi_eit_attach( p_dvbpsi, i_table_id, i_extension, process_eit, p_zero );
	}
  return;
}


static void process_subtable_pid11(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,void * p_zero)
{
	struct dvb_device_t *tuner = p_zero;


	if(i_table_id == 0x42) {
		if (!dvbpsi_sdt_attach(p_dvbpsi, i_table_id, i_extension, process_sdt, tuner))
			fprintf(stderr, "Failed to attach SDT subdecoder\n");
	}
}

static void process_nit(void* p_zero, dvbpsi_nit_t* p_nit)
{
	dvbpsi_descriptor_t *p_dsc;
	
	for( p_dsc = p_nit->p_first_descriptor; p_dsc != NULL; p_dsc = p_dsc->p_next ) {
		if( p_dsc->i_tag == 0x40 ) {
			char str1[257];
			memcpy( str1, p_dsc->p_data, p_dsc->i_length );
			str1[p_dsc->i_length] = '\0';
//			printf("Network %s\n",str1);
		}
	}

	dvbpsi_nit_ts_t *p_ts;
	for( p_ts = p_nit->p_first_ts; p_ts != NULL; p_ts = p_ts->p_next ) {

		dvbpsi_descriptor_t *p_dsc;
		for( p_dsc = p_ts->p_first_descriptor; p_dsc != NULL; p_dsc = p_dsc->p_next ) {
			//printf("Delivery descriptor %2.2X\n",p_dsc->i_tag);
     		if (p_dsc->i_tag == 0x43) {
				dvbpsi_sat_deliv_sys_dr_t *p_t = dvbpsi_DecodeSatDelivSysDr( p_dsc );
				if (p_t != NULL)
				{
				uint32_t freq = dvbpsi_Bcd8ToUint32(p_t->i_frequency);
				freq = freq*10;
				printf("Sattelite Delivery system -- %8.8X\n",p_t->i_frequency);
				printf("Frequency %i : Symbol Rate %i  Pol:%d\n",freq,dvbpsi_Bcd8ToUint32(p_t->i_symbol_rate),p_t->i_polarization);
				addSatelliteFrequency(p_zero,freq,dvbpsi_Bcd8ToUint32(p_t->i_symbol_rate)*10,p_t->i_polarization,p_t->i_modulation_type);
				}
			}
		} // end of p_dsc iterator
	}
	dvbpsi_nit_delete(p_nit);
}


static void process_sdt(void* p_zero, dvbpsi_sdt_t* p_sdt)
{
	dvbpsi_sdt_service_t* p_service = p_sdt->p_first_service;

	while(p_service) {
		DumpDescriptors_sdt("    |  ]", p_service->p_first_descriptor,p_service,p_zero,p_sdt->i_network_id);
		p_service = p_service->p_next;
	}
	dvbpsi_sdt_delete(p_sdt);
}


/*****************************************************************************
 * DumpDescriptors
 *****************************************************************************/
void DumpDescriptors_sdt(const char* str, dvbpsi_descriptor_t* p_descriptor, dvbpsi_sdt_service_t* p_service , void *p_zero, int onid)
{
//	unsigned char start;
//	char channel_name[32];
//	char provider_name[32];
	

	while(p_descriptor) {
		printf("%s 0x%02x :\"", str, p_descriptor->i_tag);

		if (p_descriptor->i_tag == 0x48) {
			dvbpsi_service_dr_t *p_t = dvbpsi_DecodeServiceDr( p_descriptor );
			if (p_t != NULL)
			{
				char service_name[128];
				memset(service_name,0,128);
				memcpy(service_name,p_t->i_service_name, p_t->i_service_name_length);
				
				char provider_name[128];
				memset(provider_name,0,128);
				memcpy(provider_name,p_t->i_service_provider_name, p_t->i_service_provider_name_length);
				
				
				channel_addSDT_Info2(p_zero, p_service->i_service_id,service_name,provider_name,p_t->i_service_type,p_service->i_running_status,
									onid,p_service->b_free_ca);
									
			}
		}
		
//		if (p_descriptor->i_tag == 0xC0) {
//			memcpy(channel_name,p_descriptor->p_data,p_descriptor->i_length);
//			channel_addSDT_Info2(p_zero, p_service->i_service_id,channel_name,provider_name,0,p_service->i_running_status,onid,p_service->b_free_ca);
//		}

#if 0
		int i;
		for(i = 0; i < p_descriptor->i_length; i++) {
			if (isprint(p_descriptor->p_data[i])) {
				printf(" %c ", p_descriptor->p_data[i]);
			} else printf("%2.2X", p_descriptor->p_data[i]);

		}
		printf("\"\n");
#endif

		p_descriptor = p_descriptor->p_next;
	}
}
