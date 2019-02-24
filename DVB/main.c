#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "telnetservermanager.h"
#include "utils.h"
#include "configuration.h"
#include "dvbhardware.h"
#include "session.h"
#include "webserv.h"
#include "database.h"


int main(int argc, char **argv)
{
	printHeading(argv[0]);
    getGlobalConfigFile (argc,argv);
	database_init();
	telnet_init(); // Create the linked lists..
	sessions_init();
	destination_init();
	startTelnetServer();
	main_web_start(8080);
        dvb_init();
	while (1) {
            usleep(500000);
        }

	main_web_stop(g_httpd);
	return 0;
}
