#include "platform.h"
#include <microhttpd.h>
#include <unistd.h>



int main_web_start (int port);
int main_web_stop  (struct MHD_Daemon *d);


struct MHD_Daemon *g_httpd;