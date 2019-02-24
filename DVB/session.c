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
#include <errno.h>
#include <iconv.h>

#include "telnetservermanager.h"
#include "utils.h"
#include "dvbhardware.h"
#include "destination.h"
#include <dvbpsi/pmt.h>
#include "session.h"
#include "iniparser.h"
#include "configuration.h"

static void process_pmt(void* p_zero, dvbpsi_pmt_t* p_pmt);
static void process_descriptors(const char* str, dvbpsi_descriptor_t* p_descriptor, struct session_service_t *sess );
static char* GetTypeName(uint8_t type);
char *FromCharset(const char *charset, const void *data, size_t data_size);
void *telnet_session_print(int fd, char inString);
int session_threadStart(void);
int session_isPidMine(unsigned short pid, struct session_service_t *session);
int destination_thread_start(struct buffer_t *buffer);
uint64_t session_addNew(unsigned short onid, unsigned short channel_id, struct destination_t *dest);
void *telnet_session_setwrite(int fd, char *inString);

int live_start(struct destination_t *dest);

static void session_message(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
{
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

int session_addPid(struct session_service_t *sess, int pid, int pidtype )
{
	int rc=0;

	for (rc=0; rc < sess->pids.pidCnt; rc++ ) {
		if (sess->pids.pidList[rc].pid == pid) {
			return 0;
		}
	}

	printf("Session : Adding PID %4.4x to list\n",pid);
	sess->pids.pidList[sess->pids.pidCnt].pid = pid;
	sess->pids.pidList[sess->pids.pidCnt].pid_type = pidtype;
	AddPID_to_Demux(sess->tuner,pid,pidtype);
	sess->pids.pidCnt++;

	return rc;
}


void *session_telnetCheckChannel(int fd, char *inString)
{
	char *tokens[5];
	const char CRLF[] = {10,13};
	const char *ERROR = "ERROR - Parameter count incorrect\r\n\0";
	char outString[128];

	int rc;
	int channel_OK;
	int hit =0;
	int loop;

	send(fd,CRLF,2,0);
	rc = tokenize(tokens,5,inString,' ');
	if (rc == 3) {

		int onid = convertoUint64(tokens[1]);
		int channelid = convertoUint64(tokens[2]);

		for (loop=0; loop<MAX_DVB_DEVICES; loop++) {
			channel_OK = channel_isProgramValid(&g_dvb_devices[loop],onid,channelid);
			if (channel_OK >= 0) {
				hit++;
			}
		}
		sprintf(outString,"ONID 0x%4.4x ChannelID 0x%4.4X can be serviced by %d tuners\r\n",onid,channelid,hit);
		send(fd,outString,strlen(outString),0);

	} else {
		send(fd,ERROR,strlen(ERROR),0);
	}

	return 0;
}



	


void *session_telnetStartSession(int fd, char *inString)
{
	char *tokens[5];
	const char CRLF[] = {10,13};
	const char *ERROR = "ERROR - Parameter count incorrect\r\n\0";
	const char *ERROR2 = "ERROR - Failed to start session\r\n\0";
	char outString[128];

	unsigned int rctokens = 0;
	uint64_t rc=0;

	send(fd,CRLF,2,0);
	rctokens = tokenize(tokens,5,inString,' ');
	if (rctokens == 3) {
		int onid = convertoUint64(tokens[1]);
		int channelid = convertoUint64(tokens[2]);
		rc = session_addNew(onid,channelid,NULL);
	} else {
		send(fd,ERROR,strlen(ERROR),0);
	}


	if (rc > 0) {
		sprintf(outString,"Session 0x%016llx started\n",(unsigned long long)rc);
		send(fd,outString,strlen(outString),0);
	} else
		send(fd,ERROR2,strlen(ERROR),0);

	return 0;
}

void *session_telnetStartSessionURL(int fd, char *inString)
{
	char *tokens[5];
	const char CRLF[] = {10,13};
	const char *ERROR = "ERROR - Parameter count incorrect\r\n\0";
	const char *ERROR2 = "ERROR - Failed to start session\r\n\0";
    const char *ERROR3 = "ERROR - Not a valild URL\r\n\0";
	char outString[128];
	void *dest;


	unsigned int rctokens = 0;
	uint64_t rc=0;

	send(fd,CRLF,2,0);
	rctokens = tokenize(tokens,5,inString,' ');
	if (rctokens == 4) {
		int onid = convertoUint64(tokens[1]);
		int channelid = convertoUint64(tokens[2]);
		dest = destination_create(tokens[3]);
		
		if (dest == NULL)
		{
			send(fd,ERROR3,strlen(ERROR3),0);
		} else
		   rc = session_addNew(onid,channelid,dest);
	} else {
		send(fd,ERROR,strlen(ERROR),0);
	}


	if (rc > 0) {
		sprintf(outString,"Session 0x%016llx started\n",(unsigned long long)rc);
		send(fd,outString,strlen(outString),0);
	} else
		send(fd,ERROR2,strlen(ERROR),0);

	return 0;
}

void *telnet_session_setwrite(int fd, char *inString)
{
	char *tokens[2];
	int rc;
	char outString[512];
	
	rc = tokenize(tokens,2,inString,' ');
	if (rc > 1)
	{
	g_write_time = atoi(tokens[1]);
	sprintf(outString,"\r\nSetting Write time to %d (0=BUCKET)\r\n",g_write_time);
	send(fd,outString,strlen(outString),0);
	}
	
	return 0;
}



void *session_telnetStopSession(int fd, char *inString)
{
	char *tokens[5];
	const char CRLF[] = {10,13};
	const char *ERROR = "ERROR - Parameter count incorrect\r\n\0";
	const char *ERROR2 = "ERROR - Failed to start session\r\n\0";
	char outString[128];

	unsigned int rc, rctokens = 0;

	send(fd,CRLF,2,0);
	rctokens = tokenize(tokens,5,inString,' ');
	if (rctokens == 2) {
		uint64_t session_id = convertoUint64(tokens[1]);
		rc = session_stop(session_id);
	} else {
		send(fd,ERROR,strlen(ERROR),0);
	}


	if (rc == 0) {
		sprintf(outString,"Session stopped\n");
		send(fd,outString,strlen(outString),0);
	} else
		send(fd,ERROR2,strlen(ERROR),0);

	return 0;
}


void sessions_init()
{

	dictionary  *   ini ;
	char *inString;


	printf("Initialising Sessions\n");

    ini = iniparser_load(G_mainConfigFile);
    inString=iniparser_getstring(ini,"global:file_write_time",NULL);
	
	if (inString != NULL)
	{
		g_write_time = atoi(inString);
	} else
 	    g_write_time = 0;
	iniparser_freedict(ini);
	g_sessions.sessions_ll = linkedList_create ( NULL );

	telnet_addcommand(session_telnetCheckChannel,"session.check","Check to see if a ONID CHANID is present");
	telnet_addcommand(session_telnetStartSession,"session.start","Start a session for ONID CHANID");
	telnet_addcommand(session_telnetStartSessionURL,"session.start.url","Start an output session for ONID CHANID URL");
	telnet_addcommand(session_telnetStopSession,"session.stop","Stop a session");
	telnet_addcommand(telnet_session_print,"session.show","Show running sessions");
	telnet_addcommand(telnet_session_setwrite,"session.set.writetime","Set Write timer");

	session_threadStart();

	return;
}


int session_stop_type(int dest_type)
{
	linked_iterator itr;
	struct session_service_t *session;
	linked_list *dest_ll = NULL;
	
	printf("Requesting to close type %d\r\n",dest_type);
	session = linkedList_create_iterator ( g_sessions.sessions_ll , &itr );
	while (session) {
	    dest_ll = session->destination_ll;
	    destination_close_type(dest_ll,dest_type);
       session = linkedList_iterate ( g_sessions.sessions_ll,&itr );
	}
   	linkedList_release_iterator ( g_sessions.sessions_ll,&itr );
	
  printf(">>>>>>>. Removed Session type %d\n",dest_type);	
  return 0;	
}

int session_stop(uint64_t session_id)
{
	linked_iterator itr;
	struct session_service_t *session;
	linked_list *dest_ll = NULL;
	char hit=-1;

	session = linkedList_create_iterator ( g_sessions.sessions_ll , &itr );
	while (session) {

		if ((uint64_t)session->sessionID == (uint64_t)session_id) {
			removePID_from_Demux(session->tuner, session->channel->pmt_pid);
			int z;
			for (z=0; z<session->pids.pidCnt; z++) {
				removePID_from_Demux(session->tuner, session->pids.pidList[z].pid);
			}

			dvbpsi_pmt_detach(session->pidPMT_dvbpsi);
			dvbpsi_delete(session->pidPMT_dvbpsi);
			dest_ll = session->destination_ll;
			session->tuner->refcnt--;
			session->buffer->bufferPtr=0;
			session->state=0; // Stop thread..
			free(session->buffer);

			if (session->tuner->refcnt <= 0)
				session->tuner->currentState = idle;

			free(session);
			session = linkedList_iterator_remove_internal ( g_sessions.sessions_ll, &itr );
			hit=0;
			break;
		} else
			session = linkedList_iterate ( g_sessions.sessions_ll,&itr );
	}
	linkedList_release_iterator ( g_sessions.sessions_ll,&itr );

	if (dest_ll != NULL) {
		destination_close_all(dest_ll);
		linkedList_destroy(dest_ll);
	}

	if (hit != 0) {
		printf("Could not locate session id 0x%llx\n",(unsigned long long)session_id);
	}

	return hit;
}


uint64_t session_addNew(unsigned short onid, unsigned short channel_id, struct destination_t *dest)
{
	struct session_service_t *session;
	struct session_service_t *session_temp;
	linked_iterator itr;
	int hit=0;
	int channel_OK=0;
	int loop;
	uint64_t rc=0;
	
	
	printf("DEST TYPE %d (live %d) (live2 %d)\r\n",dest->destination_type,dest_live,dest_live2);
	
	
	for (loop=0; loop<MAX_DVB_DEVICES; loop++) {
		channel_OK = channel_isProgramValid(&g_dvb_devices[loop],onid,channel_id);
		if (channel_OK >= 0) {
			printf("Channel can be served\n");
			break;
		}
	}

	if (channel_OK == -1) {
		return 0;
	}

	if (dest->destination_type == dest_live)
			{
				session_stop_type(dest_live);
				usleep(100000);
			}

	if (dest->destination_type == dest_live2)
			{
				session_stop_type(dest_live2);
				usleep(100000);
			}
	

	session = malloc(sizeof(struct session_service_t));
	memset(session,0,sizeof(struct session_service_t));
	getRandomCharacters(&(session->sessionID),8); // 64bits
	session->sessionID = session->sessionID >> 4; // Drop 4 bits off..
	
	session->caid=0;
	session->buffer=malloc(sizeof(struct buffer_t));
	session->buffer->bufferPtr=0;
	session->buffer->state=1; // Running... 0=stopped
	session->state=1;
	pthread_mutex_init(&session->buffer->bufferLockMutex,NULL);
	session->buffer->session=session;
	session->buffer->sessionID=session->sessionID;

	int a;
	session->pids.pidCnt=0;
	for (a=0; a<32; a++) {
		session->pids.pidList[a].pid=-1;
		session->pids.pidList[a].pid_type=-1;
	}

	session_temp = linkedList_create_iterator ( g_sessions.sessions_ll , &itr );
	while (session_temp) {
		if ((session_temp->onid == onid) && (session_temp->channel_id == channel_id)) {
			rc = session_temp->sessionID;
			break;
		}
		session_temp = linkedList_iterate ( g_sessions.sessions_ll,&itr );
	}
	linkedList_release_iterator ( g_sessions.sessions_ll,&itr );

	if (session_temp == NULL) {

		for (loop=0; loop<MAX_DVB_DEVICES; loop++) {
			struct dvb_frequency_base_t *db = channel_getFrequencyForTuner(&g_dvb_devices[loop],onid,channel_id);

			if (db == NULL) {
				printf("This tuner does not support this ONID\n");
				continue;
			}

			session->channel = getChannelByNumber_tuners(&g_dvb_devices[loop],onid,channel_id);
			if (session->channel == NULL) {
				printf("Session values are NULL\n");
				continue;
			}


			if (g_dvb_devices[loop].refcnt == 0) {
				printf("Tuner is not busy.... refrence count = 0\n");
				g_dvb_devices[loop].currentState = busy;
				g_dvb_devices[loop].refcnt++;


				if ((g_dvb_devices[loop].dvbType == dvb_s) || (g_dvb_devices[loop].dvbType == dvb_s2)) {
					dvbs_tune(&g_dvb_devices[loop],(struct dvb_frequency_dvbs_t *)db);
				} else if ((g_dvb_devices[loop].dvbType == dvb_t) || (g_dvb_devices[loop].dvbType == dvb_t2)) {
					dvbt_tune(&g_dvb_devices[loop],(struct dvb_frequency_dvbt_t *)db);
				}
				session->tuner = &g_dvb_devices[loop];
				AddPID_to_Demux(&g_dvb_devices[loop],0x0,0);
				AddPID_to_Demux(&g_dvb_devices[loop],0x10,0);
				AddPID_to_Demux(&g_dvb_devices[loop],0x11,0);
				AddPID_to_Demux(&g_dvb_devices[loop],0x12,0);
				AddPID_to_Demux(&g_dvb_devices[loop],session->channel->pmt_pid, 0);
				hit=1;
				rc = session->sessionID;
				destination_thread_start(session->buffer);
				break; // Got the tuning parameters... now go away and finish the structure
			} else {
				printf("A tuner is busy... checking for sharing\n");
				int rc1=-1;

				if ((g_dvb_devices[loop].dvbType == dvb_t) || (g_dvb_devices[loop].dvbType == dvb_t2)) {
					rc1 = compare_dvbt_entries((struct dvb_frequency_dvbt_t *)db,(struct dvb_frequency_dvbt_t *)g_dvb_devices[loop].curr_frequency_ptr);
				} else if ((g_dvb_devices[loop].dvbType == dvb_s) || (g_dvb_devices[loop].dvbType == dvb_s2)) {
					rc1 = compare_dvbs_entries((struct dvb_frequency_dvbs_t *)db,(struct dvb_frequency_dvbs_t *)g_dvb_devices[loop].curr_frequency_ptr);
				}
				if (rc1 >= 0) {
					printf("Yipee!!!! I can share this tuner! .. its on the frequency already\n!");
					session->tuner = &g_dvb_devices[loop];
					g_dvb_devices[loop].refcnt++;
					AddPID_to_Demux(&g_dvb_devices[loop],session->channel->pmt_pid, 0);
					hit = 1;
					rc = session->sessionID;
					break;
				}
				// compare frequencies if its busy
			}

			if (hit==1)
				break;
		} // END of maximum DVB devices

		if (hit == 0) {
			printf
			("No tuners are avaliable\n");
			// No tuners are avaliable
			free(session);
			return 0;
		}

		// New Session... add users and create structures..
		session->onid = onid;
		session->channel_id=channel_id;
		session->destination_ll = linkedList_create ( NULL );

		if (dest != NULL)
			linkedList_append(session->destination_ll,dest);

		session->pidPMT_dvbpsi = dvbpsi_new(&session_message, DVBPSI_MSG_DEBUG);
		dvbpsi_pmt_attach(session->pidPMT_dvbpsi, session->channel->service_id, process_pmt, session);
		linkedList_append ( g_sessions.sessions_ll,session );
		session->buffer->session = session;
		destination_thread_start(session->buffer);
	} else {

		if (dest != NULL)
			linkedList_append(session_temp->destination_ll,dest);

		printf("Already a session on this onid/channel_id - adding the destination\n");
		free(session);
	}

	return rc;
}


void session_write(unsigned short pid, unsigned char *data, int len, struct dvb_device_t *vtuner)
{
	linked_iterator itr;
	struct session_service_t *session;

	session = linkedList_create_iterator ( g_sessions.sessions_ll , &itr );
	while (session) {

		if (vtuner->logicalNumber != session->tuner->logicalNumber) {
			session = linkedList_iterate ( g_sessions.sessions_ll,&itr );
			// Continue... as this session is not linked to the tuner
			continue;
		}

		if (session_isPidMine(pid,session) < 0) {
			session = linkedList_iterate ( g_sessions.sessions_ll,&itr );
			// Continue... as this session does not have any PID's I am interested in
			continue;
		}
		if (session->channel->pmt_pid == pid)  {
			dvbpsi_packet_push(session->pidPMT_dvbpsi, data);
		}
		if (session->buffer->bufferPtr+len < 1024*1024) {
			session->outbytes_total+=len;
			pthread_mutex_lock(&session->buffer->bufferLockMutex);
			memcpy(session->buffer->buffer+session->buffer->bufferPtr,data,len);
			session->buffer->bufferPtr += len;
			pthread_mutex_unlock(&session->buffer->bufferLockMutex);
		} else {
			printf("Session buffer is not being cleared!!!! Dumping the lot and starting again...\n");
			pthread_mutex_lock(&session->buffer->bufferLockMutex);
			memcpy(session->buffer->buffer,data,len);
			session->buffer->bufferPtr=len;
			pthread_mutex_unlock(&session->buffer->bufferLockMutex);
		}
		session = linkedList_iterate ( g_sessions.sessions_ll,&itr );
	}
	linkedList_release_iterator ( g_sessions.sessions_ll,&itr );

	return;
}


static void process_pmt(void* p_zero, dvbpsi_pmt_t* p_pmt)
{
	dvbpsi_pmt_es_t* p_es = p_pmt->p_first_es;
//	printf(  "\n");
//	printf(  "New active PMT\n");
//	printf(  "  program_number : %d\n",
//	         p_pmt->i_program_number);
//	printf(  "  version_number : %d\n",
//	         p_pmt->i_version);
//	printf(  "  PCR_PID        : 0x%x (%d)\n",
//	         p_pmt->i_pcr_pid, p_pmt->i_pcr_pid);
	process_descriptors("    ]", p_pmt->p_first_descriptor,p_zero);
//	printf(  "    | type @ elementary_PID\n");
	while(p_es) {
//		printf("    | 0x%02x (%s) @ 0x%x (%d)\n",
//		       p_es->i_type, GetTypeName(p_es->i_type),
//		       p_es->i_pid, p_es->i_pid);
		session_addPid(p_zero,p_es->i_pid,p_es->i_type);
		process_descriptors("    |  ]", p_es->p_first_descriptor,p_zero);
		p_es = p_es->p_next;
	}
	dvbpsi_pmt_delete(p_pmt);
}


static void process_descriptors(const char* str, dvbpsi_descriptor_t* p_descriptor, struct session_service_t *sess )
{

	while(p_descriptor) {
		if (p_descriptor->i_tag == 0x09) {
			dvbpsi_ca_dr_t *p_t =  dvbpsi_DecodeCADr(p_descriptor);
			if (p_t != NULL) {
				session_addPid(sess,p_t->i_ca_pid,0xFF);
				sess->caid = p_t->i_ca_system_id;
				//printf("--> CAID 0x%4.4X on PID (0x%4.4X)\n",p_t->i_ca_system_id,p_t->i_ca_pid );
			}


		}

		//printf(">>>>>>>>>>>> %s 0x%02x : \r\n", str, p_descriptor->i_tag);
		p_descriptor = p_descriptor->p_next;
	}

}


/*****************************************************************************
 * GetTypeName
 *****************************************************************************/
static char* GetTypeName(uint8_t type)
{
	switch (type) {
	case 0x00:
		return "Reserved";
	case 0x01:
		return "ISO/IEC 11172 Video";
	case 0x02:
		return "ISO/IEC 13818-2 Video";
	case 0x03:
		return "ISO/IEC 11172 Audio";
	case 0x04:
		return "ISO/IEC 13818-3 Audio";
	case 0x05:
		return "ISO/IEC 13818-1 Private Section";
	case 0x06:
		return "ISO/IEC 13818-1 Private PES data packets";
	case 0x07:
		return "ISO/IEC 13522 MHEG";
	case 0x08:
		return "ISO/IEC 13818-1 Annex A DSM CC";
	case 0x09:
		return "H222.1";
	case 0x0A:
		return "ISO/IEC 13818-6 type A";
	case 0x0B:
		return "ISO/IEC 13818-6 type B";
	case 0x0C:
		return "ISO/IEC 13818-6 type C";
	case 0x0D:
		return "ISO/IEC 13818-6 type D";
	case 0x0E:
		return "ISO/IEC 13818-1 auxillary";
	case 0xFF:
		return "CA ECM/EMM";
	default:
		if (type < 0x80)
			return "ISO/IEC 13818-1 reserved";
		else
			return "User Private";
	}
}


void *session_thread_run()
{
	linked_iterator itr;
	struct session_service_t *session;
	struct timeval current_time;
    void *tmp_buf;

	while (1) {

		session = linkedList_create_iterator ( g_sessions.sessions_ll , &itr );
		while (session) {

			if (linkedList_item_count(session->destination_ll) == 0) {

				removePID_from_Demux(session->tuner, session->channel->pmt_pid);
				int z;
				for (z=0; z<session->pids.pidCnt; z++) {
					removePID_from_Demux(session->tuner, session->pids.pidList[z].pid);
				}

				dvbpsi_pmt_detach(session->pidPMT_dvbpsi);
				dvbpsi_delete(session->pidPMT_dvbpsi);
				session->tuner->refcnt--;
				session->buffer->bufferPtr=0;
				session->state=0; // Stop thread..
				//pthread_cancel(session->session_thread);
				tmp_buf = session->buffer;

				if (session->tuner->refcnt <= 0)
					session->tuner->currentState = idle;
				
				linkedList_destroy(session->destination_ll);
				free(tmp_buf);
				free(session);
				session = linkedList_iterator_remove_internal ( g_sessions.sessions_ll, &itr );
				continue;

			}
			gettimeofday(&current_time,NULL);
			unsigned long diff_ms = time_diff_us(session->bw.snap_time,current_time ) / 1000;   // mSeconds
			if (diff_ms > 500) {
				
				long Bps = (1000 / diff_ms) *  (session->outbytes_total - session->bw.totalCount);
				printf("KBytes/Second %lu\n",(unsigned long)Bps/1000);
				session->currentBandwidth = Bps;
				session->bw.totalCount =  session->outbytes_total;
				gettimeofday(&session->bw.snap_time,NULL);
			}
			session = linkedList_iterate ( g_sessions.sessions_ll,&itr );
		}
		linkedList_release_iterator ( g_sessions.sessions_ll,&itr );
		usleep(100000);
	}

	pthread_exit(0);
}


int session_threadStart(void)
{
	pthread_attr_t attributes;
	pthread_attr_init ( &attributes );
	pthread_attr_setdetachstate ( &attributes, PTHREAD_CREATE_DETACHED );
	pthread_create(&g_sessions.master_session_thread, &attributes, session_thread_run,NULL);
	return 0;
}




void *telnet_session_print(int fd, char inString)
{
	linked_iterator itr;
	struct session_service_t *session;
	char outString[256];
	const char CRLF[] = {10,13};

	session = linkedList_create_iterator ( g_sessions.sessions_ll , &itr );
	while (session) {
		outString[0]=0;
		unsigned int sessionCounter = linkedList_item_count(session->destination_ll);
		sprintf(outString,"\r\nSession id:0x%016llx\tTuner #%d (BW:%lu KBps)\r\n\tChannel : %s (onid:0x%4.4x chid:0x%4.4x pmt:0x%4.4x caid:0x%4.4x)\r\n\tClients : %d\r\n",
		        (unsigned long long)session->sessionID,
		        session->tuner->logicalNumber,
		        (unsigned long)session->currentBandwidth/1000,
		        session->channel->channelName,
		        session->onid,
		        session->channel_id,
		        session->channel->pmt_pid,
		        session->caid,sessionCounter);
		send(fd,outString,strlen(outString),0);

		int a;
		for (a=0; a<session->pids.pidCnt; a++) {
			sprintf(outString,"\tpid:0x%4.4x Type:%s\r\n",session->pids.pidList[a].pid,GetTypeName((unsigned char)session->pids.pidList[a].pid_type));
			send(fd,outString,strlen(outString),0);
		}
		send(fd,CRLF,2,0);
		session = linkedList_iterate ( g_sessions.sessions_ll,&itr );
	}
	linkedList_release_iterator ( g_sessions.sessions_ll,&itr );

	return 0;
}

int session_isPidMine(unsigned short pid, struct session_service_t *session)
{
	int rc = -1;

	switch (pid) {
	case 0x00:
		rc=0;
		break;
	case 0x10:
		rc=0;
		break;
	case 0x11:
		rc=0;
		break;
	case 0x12:
		rc=0;
		break;
	default   :
		rc=-1;
	}

	if ((unsigned short)session->channel->pmt_pid == (unsigned short)pid)
		rc=0;

	if (rc == -1) {
		int a;
		for (a=0; a<session->pids.pidCnt; a++) {
			if (session->pids.pidList[a].pid == pid) {
				rc = 0;
				break;
			}
		}
	}
	return rc;
}
