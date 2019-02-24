#include "telnetservermanager.h"
#include "socketserver.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "configuration.h"
#include "iniparser.h"
#include "sqlite3.h"
#include "utils.h"


// Forward define
int setValueforTag_global(char *owner, char *intag, char *inValue);
int telnet_addcommand(void *func, char *command, char *helpText);
int telnet_import_initialFile(int fd, char *commandLine);
int telnet_command_help(int fd, char *commandLine);
int telnet_dispatchCommand(int fd, char *commandLine);
int verifyTelnetUser_text(char *username, char *password);

int telnet_init()
{
	char *inFile_ptr;
	char *inType_ptr;
	dictionary  *   ini ;

	printf("FUNCTION : %s\n",__FUNCTION__);
	TELNET_USERS_LL = linkedList_create ( NULL ); // create the TELNET users Linked list
	TELNET_COMMANDS_LL = linkedList_create ( NULL );

	ini = iniparser_load(G_mainConfigFile);
	if (ini==NULL) {
		printf("Cannot parse file: %s\n", G_mainConfigFile);
	} else {
		inFile_ptr=iniparser_getstring(ini, "telnet:userlist",NULL);
		strncpy(g_TelnetData.telnetFilename,inFile_ptr,99);
		inType_ptr=iniparser_getstring(ini, "telnet:database",NULL);
		if (inType_ptr != NULL)
			strncpy(g_TelnetData.telnetDatabase,inType_ptr,99);

		iniparser_freedict(ini);
	}

	printf("FUNCTION : %s Exit\n",__FUNCTION__);

	return 1;
}

int telnet_onconnect (int fd, char *ip, int port)
{

	linked_iterator itr;
	struct telnet_users_t *telnetUser;
	//const unsigned char telnet_negotiate[] = { 255, 252, 1, 255 , 253 ,34, 255, 250, 34, 1, 0, 255, 240};
	const char telnet_negotiate[] = { 255, 251, 1 ,255 , 253 ,34};
	const char telnet_welcome[] = "Welcome to the Console\nLogin : ";

	printf("Telnet - New Connection from %s\n",ip);
	telnetUser = linkedList_create_iterator ( TELNET_USERS_LL , &itr );
	while ( telnetUser != NULL ) {
		if ( telnetUser->fd==fd ) {
			free ( telnetUser );
			telnetUser = linkedList_iterator_remove_internal ( TELNET_USERS_LL, &itr );
		} else
			telnetUser = linkedList_iterate ( TELNET_USERS_LL,&itr );
	}
	linkedList_release_iterator ( TELNET_USERS_LL ,&itr );
	telnetUser = malloc ( sizeof ( struct telnet_users_t ) );
	telnetUser->fd=fd;
	telnetUser->currentPtr=0;
	telnetUser->skipChars=0;
	telnetUser->authenticated=0;
	telnetUser->failedLogin=0;
	strcpy(telnetUser->ip,ip);
	linkedList_append ( TELNET_USERS_LL , telnetUser );

	send(fd,telnet_welcome,strlen(telnet_welcome),0);
	send(fd,telnet_negotiate,6,0);

	return 0;
}

int telnet_ondisconnect (int fd, int port)
{

	linked_iterator itr;
	struct telnet_users_t *telnetUser;

	telnetUser = linkedList_create_iterator ( TELNET_USERS_LL , &itr );
	while ( telnetUser != NULL ) {
		if ( telnetUser->fd==fd ) {
			printf("Disconnected fd %d\n",fd);
			free ( telnetUser );
			telnetUser = linkedList_iterator_remove_internal ( TELNET_USERS_LL, &itr );
		} else
			telnetUser = linkedList_iterate ( TELNET_USERS_LL,&itr );
	}
	linkedList_release_iterator ( TELNET_USERS_LL ,&itr );

	return 1;
}

int telnet_process ( int fd, unsigned char *buf, int nbytes, int port )
{

	linked_iterator itr;
	struct telnet_users_t *telnetUser;
	const unsigned char CRLF[] = {10,13};
	const unsigned char CMD[] = "CMD> ";

	telnetUser = linkedList_create_iterator ( TELNET_USERS_LL , &itr );
	while ( telnetUser != NULL ) {
		if ( telnetUser->fd==fd ) {
			int a;
			for (a=0; a<nbytes; a++) {
				if (telnetUser->skipChars > 0) {
					telnetUser->skipChars--;
					if (buf[a]==(unsigned char)250) { // Telnet SUBNEGOTIATION start... its painful
						telnetUser->skipChars=50; // Guard character counter... 50 should be enough
					} else if (buf[a]==(unsigned char)240) { // Telnet SUBNEGOTIATION END.. continue as normal
						telnetUser->skipChars=0;
						break;
					}
					continue;
				}


				if (((unsigned char)buf[a] != (unsigned char)0x0a) && (isprint(buf[a]))) {
					telnetUser->buf[(int)telnetUser->currentPtr] = (char)buf[a];
					telnetUser->currentPtr++;
					telnetUser->buf[(int)telnetUser->currentPtr] = 0;
					if (telnetUser->authenticated != 1) {
						send(fd,buf+a,1,0);
					} else {
						send(fd,"*",1,0);
					}

				} else if (((unsigned char)buf[a] == (unsigned char)0x7f)) {


					if (telnetUser->currentPtr > 0) {
						char backspace[] = {0x08,0x20,0x08};
						telnetUser->currentPtr--;
						telnetUser->buf[(int)telnetUser->currentPtr] = 0;
						send(fd,backspace,3,0);
					}
				} else if (((unsigned char)buf[a] == (unsigned char)0x0d)) {
					if (!strncmp(telnetUser->buf,"help",4) && (strlen(telnetUser->buf)==4) && (telnetUser->authenticated == 2)) {
						telnet_command_help(fd,telnetUser->buf);
					}

					if (!strncmp(telnetUser->buf,"quit",4) && (strlen(telnetUser->buf)==4)&& (telnetUser->authenticated == 2)) {
						linkedList_release_iterator ( TELNET_USERS_LL ,&itr );
						return -255; // Disconnect in Socket server
					} else {
					
					        printf("State %d\r\n",telnetUser->authenticated);       
						if (telnetUser->authenticated == 0) {
						         
							strncpy(telnetUser->username,telnetUser->buf,19);
							telnetUser->authenticated++;
							send(fd,CRLF,2,0);
							char passprompt[] = "Password: ";
							send(fd,passprompt,10,0);
							// Username
						} else if (telnetUser->authenticated == 1) {
							telnetUser->authenticated++;
							printf("User %s using password %s\n",telnetUser->username,telnetUser->buf);
							int rc = verifyTelnetUser_text(telnetUser->username,(char *)telnetUser->buf);

							if (rc < 0) {
								const char telnet_login[] = "\r\nLogin failed.....\r\n\nLogin : ";
								send(fd,telnet_login,strlen(telnet_login),0);
								telnetUser->authenticated = 0;
								if (telnetUser->failedLogin++ >=2) {
									linkedList_release_iterator ( TELNET_USERS_LL ,&itr );
									return -255;
								}
							} else
							{
								telnetUser->authenticated = 2;
								telnetUser->failedLogin=0;
							}

							//Password
						} else 	{ 
							if (telnetUser->currentPtr > 0)
							      telnet_dispatchCommand(fd,telnetUser->buf);
						}
					}

					telnetUser->currentPtr=0;
					memset(telnetUser->buf,10,0);
					telnetUser->buf[(int)telnetUser->currentPtr]=0;
					if (telnetUser->authenticated == 2) {
						send(fd,CRLF,2,0);
						send(fd,CMD,5,0);
					}

				} else if ((unsigned char)buf[a] == (unsigned char)0xff) {
					telnetUser->skipChars = 2;
					telnetUser->currentPtr=0;
				}
			}
			break;
		} else
			telnetUser = linkedList_iterate ( TELNET_USERS_LL,&itr );
	}
	linkedList_release_iterator ( TELNET_USERS_LL ,&itr );
	return 1;
}

int findChar(unsigned char needle, char *haystack )
{
	int a;

	if ((unsigned char)haystack[0] > (unsigned char)127)
		return 1;

	if ((unsigned char)haystack[0] <= (unsigned char)0x0a)
		return 1;

	for (a=0; a<strlen(haystack); a++) {
		if ((unsigned char)haystack[a] == (unsigned char)needle)
			return a+1;
	}
	return -1;
}

int telnet_dispatchCommand(int fd, char *commandLine)
{
	struct telnet_commands_t *commands;
	linked_iterator itr;
	char *original;
	int rv = 1;
	char OK[] = "\r\nOK\r\n";
	char ERROR[] = "\r\nERROR\r\n";
	original = strdup(commandLine);
	char *tokens[2];
	
	tokenize(tokens,2,commandLine,' ');
	// Tokenise the string here
	// Get Command..
	// Call function.
    
	commands = linkedList_create_iterator ( TELNET_COMMANDS_LL , &itr );
	while (commands != NULL) {
		if (!strncmp(commands->command,tokens[0],strlen(commands->command)) &&  strlen(commands->command) == strlen(tokens[0])) {
			if (commands->commandFunc != NULL) {
				rv = commands->commandFunc(fd,original);
				printf("Return code is %d\n",rv);
				if (rv==-1) {
					send(fd,ERROR,5,0);
				} else {
					send(fd,OK,6,0);
				}
			}
		}
		commands = linkedList_iterate ( TELNET_COMMANDS_LL,&itr );
	}
	free(original);
	linkedList_release_iterator ( TELNET_COMMANDS_LL ,&itr );
	return rv;
}


int telnet_command_help(int fd, char *commandLine)
{
	struct telnet_commands_t *commands;
	linked_iterator itr;
	const char header[] = "\r\n== Avaliable Commands ==\r\n";
	const char footer[] = "\r\n========================\r\n";
	const unsigned char CRLF[] = {10,13};

	send(fd,header,(int)strlen(header),0);
	commands = linkedList_create_iterator ( TELNET_COMMANDS_LL , &itr );
	while (commands != NULL) {
		send(fd,commands->command,(int)strlen(commands->command),0);
		send(fd,CRLF,2,0);

		commands = linkedList_iterate ( TELNET_COMMANDS_LL,&itr );
	}
	linkedList_release_iterator ( TELNET_COMMANDS_LL,&itr );
	send(fd,footer,(int)strlen(header),0);
	return 1;
}


int telnet_addcommand(void *func, char *command, char *helpText)
{
	struct telnet_commands_t *tnc;

	tnc = malloc(sizeof(struct telnet_commands_t));

	strncpy(tnc->command,command,20);
	strncpy(tnc->helpText,helpText,255);
	tnc->commandFunc = func;

	linkedList_append ( TELNET_COMMANDS_LL,tnc );
	return 1;
}

int startTelnetServer(void)
{
	struct socket_server_t sock;
	dictionary  *   ini ;



	sock.init=NULL;//telnet_init;
	sock.connect=telnet_onconnect;
	sock.disconnect=telnet_ondisconnect;
	sock.process=telnet_process;
	sock.housekeeping=NULL;
	sock.port=9000;
	ini = iniparser_load(G_mainConfigFile);
	if (ini==NULL) {
		printf("Cannot parse file: %s\n", G_mainConfigFile);
	} else {
		sock.port=iniparser_getint(ini, "telnet:port", -1);
		iniparser_freedict(ini);
		if (sock.port <= 0)
			sock.port=9000;
	}

	socketserver_main(&sock);

	return 0;
}

int verifyTelnetUser_sqlite(char *username, char *password)
{
	int retval, rc = -1;
	sqlite3 *g_handle;
	char query[100];



	retval = sqlite3_open("settings.db",&g_handle);
	if (retval) {
		printf("database failed\n");
		// Database open failed... does it exist?
	} else {
		printf("database opened\n");
		//Database opened.
		sprintf(query,"select * from TelnetUsers where username=\"%s\" and password=\"%s\";",username,password);
		sqlite3_close(g_handle);
	}




	return rc;
}


int verifyTelnetUser_text(char *username, char *password)
{
	int rc, rv = -1;
	FILE *fp;
	char line[80];
	char *tokens[2];

	printf("Checking for user in file %s\n",g_TelnetData.telnetFilename);
	fp = fopen (g_TelnetData.telnetFilename, "r");  /* open the file for reading */
	if (fp == NULL)
		return -1;
	while(fgets(line, 80, fp) != NULL) {
		rc = tokenize(tokens,2,line,':');
		if (rc == 2) {
		    terminateCRLF(tokens[0]);
			terminateCRLF(tokens[1]);
			
			if (strlen(username) == strlen(tokens[0]) && strlen(password) == strlen(tokens[1])) {
				if (!strncmp(username,tokens[0],strlen(username)) && !strncmp(password,tokens[1],strlen(password))) {
					rv = 0;
					printf("User %s autehnticated\n",username);
					break;
				}
			}
		}
	}
	fclose(fp);
	return rv;
}

