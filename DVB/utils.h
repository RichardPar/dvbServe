#ifndef _UTILS_H
#define _UTILS_H

#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdint.h>


struct parsed_url {
    char *scheme;               /* mandatory */
    char *host;                 /* mandatory */
    char *port;                 /* optional */
    char *path;                 /* optional */
    char *query;                /* optional */
    char *fragment;             /* optional */
    char *username;             /* optional */
    char *password;             /* optional */
};


unsigned int tokenize(char **result, unsigned int reslen, char *str, char delim);
unsigned long getDVB_Serial(char *inFrontEnd);
void printHeading(char *appName);
void terminateCRLF(char *inString);
void hexdump(int indent, char *prefix, unsigned char *buf, int buflen);
void iprintf(int indent, char *fmt, ...);
void getRandomCharacters(void *outChar, int len);
uint64_t convertoUint64(const char *inString);
unsigned long time_diff_us(struct timeval x , struct timeval y);
int64_t EITConvertStartTime( uint64_t i_date );
int EITConvertDuration( uint32_t i_duration );
void EITDecodeMjd( int i_mjd, int *p_y, int *p_m, int *p_d );
int64_t session_timegm( int i_year, int i_month, int i_mday, int i_hour, int i_minute, int i_second );
struct parsed_url *parse_url(const char *url);
void parsed_url_free(struct parsed_url *purl);



#endif