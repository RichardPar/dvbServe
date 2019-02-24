
#include "linkedlist.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_TELNET_STRING 128

int telnet_init();
int startTelnetServer(void);
int findChar(unsigned char needle, char *haystack );
int telnet_telnetSetport(int fd, char *commandLine);
int telnet_addcommand(void *func, char *command, char *helpText);

struct telnet_data_t {
	char telnetFilename[100];
	char telnetDatabase[100];
};


struct telnet_users_t {
	char username[20];
	char failedLogin;
	char authenticated;
	int  fd;
	char ip[20];
	char buf[MAX_TELNET_STRING];
	int  currentPtr;
	char skipChars;
};

struct telnet_commands_t {
	int (* commandFunc)(int,char *);
	char command[20];
	int  number_of_parameters;
	char helpText[255];
};

linked_list	*TELNET_USERS_LL;
linked_list	*TELNET_COMMANDS_LL;

struct telnet_data_t g_TelnetData;
