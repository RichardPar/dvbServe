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
#include "database.h"
#include "sqlite3.h"


void database_init()
{
   char sql[512];
   char *zErrMsg = 0;
   int  rc;	
	
   rc = sqlite3_open("dvbServe.db", &g_database);
   if (rc < 0)
      g_database=NULL;

// Create CHANNELS table
   sprintf(sql,"CREATE TABLE IF NOT EXISTS CHANNELS_DVB (id INTEGER PRIMARY KEY AUTOINCREMENT, serial NUMERIC ,onid NUMERIC, chid NUMERIC, name TEXT);");
    rc = sqlite3_exec(g_database, sql, NULL, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "CHANNELS_DVB Table created successfully\n");
   }
	
   sprintf(sql,"CREATE TABLE IF NOT EXISTS EVENTS (id INTEGER PRIMARY KEY AUTOINCREMENT, onid INTEGER, chid INTEGER, language TEXT,start_time INTEGER, duration INTEGER, event_name TEXT,event_desc TEXT);");
   rc = sqlite3_exec(g_database, sql, NULL, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "EVENTS Table created successfully\n");
   }
	
}


void database_addevent(unsigned short onid, unsigned short chid, int start_time, int duration, char *event_name, char *event_desc, char *lang)
{
    char sqlString[512];
	sqlite3_stmt *stmt;
	int result,val;
	
	sprintf(sqlString,"select id from EVENTS where start_time=%d and onid=%d and chid=%d;",start_time,onid,chid);		  
	result = sqlite3_prepare_v2(g_database, sqlString, -1, &stmt, NULL) ;	
    if (result)
    {
		printf("Database Query failed\n");
		return;
	}	
	int cols = sqlite3_column_count(stmt);
	printf("%d Columns returned\n",cols);
	result = sqlite3_step(stmt);
	val = sqlite3_column_int(stmt,0);
	sqlite3_finalize(stmt); 
	if (val == 0)
	{
    sprintf(sqlString,"insert or replace into EVENTS (onid,chid,language,start_time,duration,event_name,event_desc) values(%d,%d,?,%d,%d,?,?);",onid,chid,start_time,duration);		  
	result = sqlite3_prepare_v2(g_database, sqlString,-1, &stmt, NULL) ;	
    sqlite3_bind_text(stmt,1,lang,3,NULL);
    sqlite3_bind_text(stmt,2,event_name,strlen(event_name)+1,NULL);
	sqlite3_bind_text(stmt,3,event_desc,strlen(event_desc)+1,NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);  
	} else
	{
		// Update Table here
		
		
		
		
	}	
	
	
	
	
	
	
}	

void database_addchannel(unsigned long serial, unsigned short onid, unsigned short chid, char *channelName)
{
	char sqlString[512];
	sqlite3_stmt *stmt;
	int result;
	
	if (g_database == NULL)
	          return;
	
	sprintf(sqlString,"select id from CHANNELS_DVB where serial=%lu and onid=%d and chid=%d;",serial,onid,chid);		  
	result = sqlite3_prepare_v2(g_database, sqlString, -1, &stmt, NULL) ;	
    if (result)
    {
		printf("Database Query failed\n");
		return;
	}	
	int cols = sqlite3_column_count(stmt);
	printf("%d Columns returned\n",cols);
	result = sqlite3_step(stmt);
	int val = sqlite3_column_int(stmt,0);
	sqlite3_finalize(stmt);
	printf("Val is %d\n",val);
 
	if (val == 0)
	{
     sprintf(sqlString,"insert or replace into CHANNELS_DVB (serial,onid,chid,name) values(%lu,%d,%d,?);",serial,onid,chid);		  
	 result = sqlite3_prepare_v2(g_database, sqlString, strlen(sqlString)+1, &stmt, NULL) ;	
     sqlite3_bind_text(stmt,1,channelName,strlen(channelName),NULL);	  
	 sqlite3_step(stmt);
	 sqlite3_finalize(stmt);  
	}
	
	return;
}


void database_createBookmarks(void)
{
   char *zErrMsg = 0;

   char *sqlString1 = "drop table bookmarks;\0";
   char *sqlString2 = "create table bookmarks (ID INTEGER PRIMARY KEY AUTOINCREMENT , CHANNEL_NUM INTEGER , NAME TEXT) ;\0";	
   char *sqlString3 = "insert into bookmarks (NAME) select  distinct NAME from channels_dvb;\0";
	
	sqlite3_exec(g_database, sqlString1, NULL, 0, &zErrMsg);
	sqlite3_exec(g_database, sqlString2, NULL, 0, &zErrMsg);
	sqlite3_exec(g_database, sqlString3, NULL, 0, &zErrMsg);
	

	return;
}