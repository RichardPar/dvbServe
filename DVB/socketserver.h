#ifndef _SOCKETSERVER_H
#define _SOCKETSERVER_H

#include <unistd.h>
#include <sys/types.h>

struct socket_server_t
{
  int (* init)(int, fd_set *,int *);
  int (* connect)(int, char *, int);
  int (* disconnect)(int, int);
  int (* process)(int, unsigned char *, int, int);
  int (* shutdown)(int);
  int (* housekeeping)(void);
  int port;
};



int socketserver_main(struct socket_server_t *inSocketServer);
void *socketserver_main_thread(void *param);


#endif