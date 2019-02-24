#include <string.h>
#include "webserv.h"
#include "destination.h"
#include "session.h"
#include "utils.h"



int command_to_number(char *command);
int64_t session_addNew(unsigned short onid, unsigned short channel_id, struct destination_t *dest);
void get_ip(struct sockaddr *ip, char *str, int length);

#define PAGE "<html><head><title>INVALID URL</title></head><body>File not found</body></html>"
#define LIVE_PAGE "<html><head><title>dvbServe</title></head><body>LIVE</body></html>"


static ssize_t file_reader (void *cls, uint64_t pos, char *buf, size_t max)
{
  FILE *file = cls;

  (void)  fseek (file, pos, SEEK_SET);
  return fread (buf, 1, max, file);
}

static ssize_t stream_buffer_reader (void *cls, uint64_t pos, char *buf, size_t max)
{
  int rc = destination_getDatafromBuffer(cls,buf,max);
  return rc;
}



static void free_callback (void *cls)
{
  FILE *file = cls;
  fclose (file);
}

static void free_stream_callback (void *cls)
{
    struct destination_t *dest = cls;
	dest->expired = 1;
}


static int ahc_echo (void *cls,
          struct MHD_Connection *connection,
          const char *url,
          const char *method,
          const char *version,
          const char *upload_data,
	  size_t *upload_data_size, void **ptr)
{
  static int aptr;
  struct MHD_Response *response;
  int ret;
  FILE *file;
  struct stat buf;
  char *tokens[10];
   void *dest;

  if (0 != strcmp (method, MHD_HTTP_METHOD_GET))
    return MHD_NO;              /* unexpected method */
  if (&aptr != *ptr)
    {
      /* do never respond on first call */
      *ptr = &aptr;
      return MHD_YES;
    }
  *ptr = NULL;                  /* reset when done */


  if (0 == stat (&url[1], &buf))
    file = fopen (&url[1], "rb");
  else
    file = NULL;
	
  printf("URL is %s\n",&url[1]);	
  char tokenURL[255];
  strncpy(tokenURL,&url[1],254);
  int rc = tokenize(tokens,10,tokenURL,'/');	
  
  int z;
  for (z=0;z<rc;z++)
  {
	  printf("Token %d : %s\r\n",z,tokens[z]);
  }
  
  if (rc <= 1)
    {
      response = MHD_create_response_from_buffer (strlen (PAGE),(void *) PAGE,MHD_RESPMEM_PERSISTENT);
      ret = MHD_queue_response (connection, MHD_HTTP_NOT_FOUND, response);
      MHD_destroy_response (response);
    }
  else
    {

	int command_num = command_to_number(tokens[0]);
  
  
	printf("Command number is %d\r\n",command_num);
	
	if (((command_num==1) || (command_num==2) || (command_num==3)) && (rc >= 3))
	 {
		 
		 //char str[20];
		 //get_ip(connection->addr, str, 20);
		if (command_num == 1)
		{ 
		   dest = destination_create("http://localhost/stream/content"); 
		} else if (command_num == 2)
		{
		   dest = destination_create("live://localhost/live/content");	
		} else if (command_num == 3)
		{
			dest = destination_create("live://localhost/live2/content");
		}
			
			
		if (dest == NULL)
		{
		   response = MHD_create_response_from_buffer (strlen (PAGE),(void *) PAGE,MHD_RESPMEM_PERSISTENT);
           ret = MHD_queue_response (connection, MHD_HTTP_NOT_FOUND, response);
           MHD_destroy_response (response);
		   return ret;
		}
		
		
		int onid = convertoUint64(tokens[1]);
		int chid = convertoUint64(tokens[2]);
				
		unsigned long long session = session_addNew(onid,chid,dest);
		
		if (session == 0)
		{
           response = MHD_create_response_from_buffer (strlen (PAGE),(void *) PAGE,MHD_RESPMEM_PERSISTENT);
           ret = MHD_queue_response (connection, MHD_HTTP_NOT_FOUND, response);
           MHD_destroy_response (response);
		   return ret;
		} else
		{
		   if (command_num == 1)
           {	
		   response = MHD_create_response_from_callback (MHD_SIZE_UNKNOWN,4096,&stream_buffer_reader,dest,&free_stream_callback);
		   } else
		   {
		   response = MHD_create_response_from_buffer (strlen (LIVE_PAGE),(void *) LIVE_PAGE,MHD_RESPMEM_PERSISTENT);
		   ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
		   MHD_destroy_response (response);
		   return ret;
		   }
		}
 
	 } else
	 {
		if (file == NULL)
		{	 
		   response = MHD_create_response_from_buffer (strlen (PAGE),(void *) PAGE,MHD_RESPMEM_PERSISTENT);
           ret = MHD_queue_response (connection, MHD_HTTP_NOT_FOUND, response);
           MHD_destroy_response (response);
		   return ret;

		} else 
           response = MHD_create_response_from_callback (MHD_SIZE_UNKNOWN,2024,&file_reader,file,&free_callback);
	 }
	 
	 if (response == NULL)
	    {
	       fclose (file);
	       return MHD_NO;
		}
	
    }	
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);
    
  return ret;
}

int main_web_start (int port)
{

  printf ("HTTPD Server started on port %d\r\n", port);
  g_httpd = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG,port,NULL, NULL, &ahc_echo, PAGE, MHD_OPTION_END);
  
  if (g_httpd == NULL)
    return -1;
	
  // MHD_stop_daemon (d);
  return 0;
}

int main_web_stop (struct MHD_Daemon *d)
{
	MHD_stop_daemon (d);
	return 0;
}

int command_to_number(char *command)
{
	if (!memcmp(command,"stream",6))
		return 1;
	if (!memcmp(command,"live2",5))
		return 3;	
	if (!memcmp(command,"live",5))
		return 2;
	
	return 0;
}

void get_ip(struct sockaddr *ip, char *str, int length) 
{

	/* Check the sockaddr family so we can cast it to the appropriate
	 * address type, IPv4 or IPv6 */
	if (ip->sa_family == AF_INET) {
		/* IPv4 - cast the generic sockaddr to a sockaddr_in */
		struct sockaddr_in *v4 = (struct sockaddr_in *)ip;
		/* Use inet_ntop to convert the address into a string using
		 * dotted decimal notation */
		printf("%s ", inet_ntop(AF_INET, &(v4->sin_addr), str, length));
	}

	if (ip->sa_family == AF_INET6) {
		/* IPv6 - cast the generic sockaddr to a sockaddr_in6 */
		struct sockaddr_in6 *v6 = (struct sockaddr_in6 *)ip;
		/* Use inet_ntop to convert the address into a string using
		 * IPv6 address notation */
		printf("%s ", inet_ntop(AF_INET6, &(v6->sin6_addr), str, length));
	}

  return;
}
