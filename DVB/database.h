#include "sqlite3.h"

void database_init();
void database_addchannel(unsigned long serial, unsigned short onid, unsigned short chid, char *channelName);
void database_addevent(unsigned short onid, unsigned short chid, int start_time, int duration, char *event_name, char *event_desc, char *lang);


sqlite3 *g_database;