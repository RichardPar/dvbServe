#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <iconv.h>

#include <ctype.h>



#include "utils.h"

unsigned int tokenize(char **result, unsigned int reslen, char *str, char delim)
{
	char *p, *n;
	unsigned int i = 0;

	if(!str)
		return 0;
	for(n = str; *n == ' '; n++);
	p = n;
	for(i = 0; *n != 0;) {
		if(i == reslen)
			return i;
		if(*n == delim) {
			*n = 0;
			if(strlen(p))
				result[i++] = p;
			p = ++n;
		} else
			n++;
	}
	if(strlen(p))
		result[i++] = p;
	return i;                   /* number of tokens */
}


unsigned long getDVB_Serial(char *inFrontEnd)
{
	unsigned long serial = 0;
	unsigned int adapterNumber=-1;
	unsigned int frontendNumber=-1;
    char sysFS[128];
	char SN[128];
	int rhandle;
	int rc=0;
    char instring[128];
	
	strcpy(instring,inFrontEnd);


	char *tokens[5];
	rc = tokenize(tokens,5,instring,'/');	
	if (rc >= 4)
	{
	if (!memcmp(tokens[2],"adapter",7))
	   {
		adapterNumber = atoi(tokens[2]+7);
	   }
	if (!memcmp(tokens[3],"frontend",8))
	   {
		frontendNumber = atoi(tokens[2]+8);
	   }
	
	
	sprintf(sysFS,"/sys/class/dvb/dvb%d.frontend%d/device/serial",adapterNumber,frontendNumber);
	
	rhandle = open(sysFS, O_RDONLY);
	if (rhandle > 0) {
		sprintf(SN,"%d%d",adapterNumber+1,frontendNumber+1);
		rc = read(rhandle,SN+strlen(SN),32);
		close(rhandle);
		serial = atol(SN);
		if (serial == 0)
		    serial = (adapterNumber+1) << 8 | (frontendNumber+1);
	} else
	  serial = (adapterNumber+1) << 8 | (frontendNumber+1);
	}
	
	printf("................\n");
	return serial;
}


int getBandwidth(char *bw)
{
	int rc;

	switch (bw[0]) {
	case '5' :
		rc=5;
		break;
	case '6' :
		rc=6;
		break;
	case '7' :
		rc=7;
		break;
	default:
		rc = 8;
	}

	return rc;
}

void terminateCRLF(char *inString)
{
	int a;

	for (a=0; a<strlen(inString); a++) {
		if (inString[a]==13)
			inString[a]=0;
		if (inString[a]==10)
			inString[a]=0;
	}
}


void printHeading(char *appName)
{
	printf("\r\n");
	printf("%s Starting...\r\n",appName);
	return;
}


void hexdump(int indent, char *prefix, unsigned char *buf, int buflen)
{
	int i;
	int j;
	int max;
	char line[512];

	for(i=0; i< buflen; i+=16) {
		max = 16;
		if ((i + max) > buflen)
			max = buflen - i;

		memset(line, 0, sizeof(line));
		memset(line + 4 + 48 + 1, ' ', 16);
		sprintf(line, "%02x: ", i);
		for(j=0; j<max; j++) {
			sprintf(line + 4 + (j*3), "%02x", buf[i+j]);
			if ((buf[i+j] > 31) && (buf[i+j] < 127))
				line[4 + 48 + 1 + j] = buf[i+j];
			else
				line[4 + 48 + 1 + j] = '.';
		}

		for(j=0; j< 4 + 48;  j++) {
			if (!line[j])
				line[j] = ' ';
		}
		line[4+48] = '|';

		for(j=0; j < indent; j++) {
			printf("\t");
		}
		printf("%s%s|\n", prefix, line);
	}
}

void iprintf(int indent, char *fmt, ...)
{
	va_list ap;

	while(indent--) {
		printf("\t");
	}

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}


void getRandomCharacters(void  *outChar, int len)
{
	int rhandle;
	int rc=0;
	rhandle = open("/dev/urandom", O_RDONLY);
	if (rhandle > 0) {
		rc = read(rhandle,outChar,len);
        rc++;
	}
	close(rhandle);
	return;
}



uint64_t convertoUint64(const char *inString)
{
	char *value = (char *)inString;
	char *end;
	uint64_t rc = 0;
	rc = strtoll(value,&end,0);

	return rc;
}




unsigned long time_diff_us(struct timeval x , struct timeval y)
{
    double x_ms , y_ms , diff;
     
    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
     
    diff = (double)y_ms - (double)x_ms;
     
    return diff;
}

char *convert_to_UTF8(char *inStr, size_t inLen)
{
	
    char *dst = malloc(inLen * 4);
    size_t srclen = inLen;
    size_t dstlen = inLen*4;
 
    char * pIn = inStr;
    char * pOut = ( char*)dst;
 
    iconv_t conv = iconv_open("UTF-8","ISO_8859-1");
    iconv(conv, &pIn, &srclen, &pOut, &dstlen);
    iconv_close(conv);

  return dst;
}


int64_t session_timegm( int i_year, int i_month, int i_mday, int i_hour, int i_minute, int i_second )
{
	static const int pn_day[12+1] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	int64_t i_day;

	if( i_year < 70 ||
	    i_month < 0 || i_month > 11 || i_mday < 1 || i_mday > 31 ||
	    i_hour < 0 || i_hour > 23 || i_minute < 0 || i_minute > 59 || i_second < 0 || i_second > 59 )
		return -1;

	/* Count the number of days */
	i_day = 365 * (i_year-70) + pn_day[i_month] + i_mday - 1;
#define LEAP(y) ( ((y)%4) == 0 && (((y)%100) != 0 || ((y)%400) == 0) ? 1 : 0)
	int i;
	for( i = 70; i < i_year; i++ )
		i_day += LEAP(1900+i);
	if( i_month > 1 )
		i_day += LEAP(1900+i_year);
#undef LEAP
	/**/
	return ((24*i_day + i_hour)*60 + i_minute)*60 + i_second;
}

void EITDecodeMjd( int i_mjd, int *p_y, int *p_m, int *p_d )
{
	const int yp = (int)( ( (double)i_mjd - 15078.2)/365.25 );
	const int mp = (int)( ((double)i_mjd - 14956.1 - (int)(yp * 365.25)) / 30.6001 );
	const int c = ( mp == 14 || mp == 15 ) ? 1 : 0;

	*p_y = 1900 + yp + c*1;
	*p_m = mp - 1 - c*12;
	*p_d = i_mjd - 14956 - (int)(yp*365.25) - (int)(mp*30.6001);
}



#define CVT_FROM_BCD(v) ((((v) >> 4)&0xf)*10 + ((v)&0xf))
int64_t EITConvertStartTime( uint64_t i_date )
{
	const int i_mjd = i_date >> 24;
	const int i_hour   = CVT_FROM_BCD(i_date >> 16);
	const int i_minute = CVT_FROM_BCD(i_date >>  8);
	const int i_second = CVT_FROM_BCD(i_date      );
	int i_year;
	int i_month;
	int i_day;

	/* if all 40 bits are 1, the start is unknown */
	if( i_date == UINT64_C(0xffffffffff) )
		return -1;

	EITDecodeMjd( i_mjd, &i_year, &i_month, &i_day );
	return session_timegm( i_year - 1900, i_month - 1, i_day, i_hour, i_minute, i_second );
}

int EITConvertDuration( uint32_t i_duration )
{
	return CVT_FROM_BCD(i_duration >> 16) * 3600 +
	       CVT_FROM_BCD(i_duration >> 8 ) * 60 +
	       CVT_FROM_BCD(i_duration      );
}

 

/*
 * Prototype declarations
 */
static __inline__ int _is_scheme_char(int);
/*
 * Check whether the character is permitted in scheme string
 */
static __inline__ int
_is_scheme_char(int c)
{
    return (!isalpha(c) && '+' != c && '-' != c && '.' != c) ? 0 : 1;
}

/*
 * See RFC 1738, 3986
 */
struct parsed_url *parse_url(const char *url)
{
    struct parsed_url *purl;
    const char *tmpstr;
    const char *curstr;
    int len;
    int i;
    int userpass_flag;
    int bracket_flag;

    /* Allocate the parsed url storage */
    purl = malloc(sizeof(struct parsed_url));
    if ( NULL == purl ) {
        return NULL;
    }
    purl->scheme = NULL;
    purl->host = NULL;
    purl->port = NULL;
    purl->path = NULL;
    purl->query = NULL;
    purl->fragment = NULL;
    purl->username = NULL;
    purl->password = NULL;

    curstr = url;

    /*
     * <scheme>:<scheme-specific-part>
     * <scheme> := [a-z\+\-\.]+
     *             upper case = lower case for resiliency
     */
    /* Read scheme */
    tmpstr = strchr(curstr, ':');
    if ( NULL == tmpstr ) {
        /* Not found the character */
        parsed_url_free(purl);
        return NULL;
    }
    /* Get the scheme length */
    len = tmpstr - curstr;
    /* Check restrictions */
    for ( i = 0; i < len; i++ ) {
        if ( !_is_scheme_char(curstr[i]) ) {
            /* Invalid format */
            parsed_url_free(purl);
            return NULL;
        }
    }
    /* Copy the scheme to the storage */
    purl->scheme = malloc(sizeof(char) * (len + 1));
    if ( NULL == purl->scheme ) {
        parsed_url_free(purl);
        return NULL;
    }
    (void)strncpy(purl->scheme, curstr, len);
    purl->scheme[len] = '\0';
    /* Make the character to lower if it is upper case. */
    for ( i = 0; i < len; i++ ) {
        purl->scheme[i] = tolower(purl->scheme[i]);
    }
    /* Skip ':' */
    tmpstr++;
    curstr = tmpstr;

    /*
     * //<user>:<password>@<host>:<port>/<url-path>
     * Any ":", "@" and "/" must be encoded.
     */
    /* Eat "//" */
    for ( i = 0; i < 2; i++ ) {
        if ( '/' != *curstr ) {
            parsed_url_free(purl);
            return NULL;
        }
        curstr++;
    }

    /* Check if the user (and password) are specified. */
    userpass_flag = 0;
    tmpstr = curstr;
    while ( '\0' != *tmpstr ) {
        if ( '@' == *tmpstr ) {
            /* Username and password are specified */
            userpass_flag = 1;
            break;
        } else if ( '/' == *tmpstr ) {
            /* End of <host>:<port> specification */
            userpass_flag = 0;
            break;
        }
        tmpstr++;
    }

    /* User and password specification */
    tmpstr = curstr;
    if ( userpass_flag ) {
        /* Read username */
        while ( '\0' != *tmpstr && ':' != *tmpstr && '@' != *tmpstr ) {
            tmpstr++;
        }
        len = tmpstr - curstr;
        purl->username = malloc(sizeof(char) * (len + 1));
        if ( NULL == purl->username ) {
            parsed_url_free(purl);
            return NULL;
        }
        (void)strncpy(purl->username, curstr, len);
        purl->username[len] = '\0';
        /* Proceed current pointer */
        curstr = tmpstr;
        if ( ':' == *curstr ) {
            /* Skip ':' */
            curstr++;
            /* Read password */
            tmpstr = curstr;
            while ( '\0' != *tmpstr && '@' != *tmpstr ) {
                tmpstr++;
            }
            len = tmpstr - curstr;
            purl->password = malloc(sizeof(char) * (len + 1));
            if ( NULL == purl->password ) {
                parsed_url_free(purl);
                return NULL;
            }
            (void)strncpy(purl->password, curstr, len);
            purl->password[len] = '\0';
            curstr = tmpstr;
        }
        /* Skip '@' */
        if ( '@' != *curstr ) {
            parsed_url_free(purl);
            return NULL;
        }
        curstr++;
    }

    if ( '[' == *curstr ) {
        bracket_flag = 1;
    } else {
        bracket_flag = 0;
    }
    /* Proceed on by delimiters with reading host */
    tmpstr = curstr;
    while ( '\0' != *tmpstr ) {
        if ( bracket_flag && ']' == *tmpstr ) {
            /* End of IPv6 address. */
            tmpstr++;
            break;
        } else if ( !bracket_flag && (':' == *tmpstr || '/' == *tmpstr) ) {
            /* Port number is specified. */
            break;
        }
        tmpstr++;
    }
    len = tmpstr - curstr;
    purl->host = malloc(sizeof(char) * (len + 1));
    if ( NULL == purl->host || len <= 0 ) {
        parsed_url_free(purl);
        return NULL;
    }
    (void)strncpy(purl->host, curstr, len);
    purl->host[len] = '\0';
    curstr = tmpstr;

    /* Is port number specified? */
    if ( ':' == *curstr ) {
        curstr++;
        /* Read port number */
        tmpstr = curstr;
        while ( '\0' != *tmpstr && '/' != *tmpstr ) {
            tmpstr++;
        }
        len = tmpstr - curstr;
        purl->port = malloc(sizeof(char) * (len + 1));
        if ( NULL == purl->port ) {
            parsed_url_free(purl);
            return NULL;
        }
        (void)strncpy(purl->port, curstr, len);
        purl->port[len] = '\0';
        curstr = tmpstr;
    }

    /* End of the string */
    if ( '\0' == *curstr ) {
        return purl;
    }

    /* Skip '/' */
    if ( '/' != *curstr ) {
        parsed_url_free(purl);
        return NULL;
    }
    curstr++;

    /* Parse path */
    tmpstr = curstr;
    while ( '\0' != *tmpstr && '#' != *tmpstr  && '?' != *tmpstr ) {
        tmpstr++;
    }
    len = tmpstr - curstr;
    purl->path = malloc(sizeof(char) * (len + 1));
    if ( NULL == purl->path ) {
        parsed_url_free(purl);
        return NULL;
    }
    (void)strncpy(purl->path, curstr, len);
    purl->path[len] = '\0';
    curstr = tmpstr;

    /* Is query specified? */
    if ( '?' == *curstr ) {
        /* Skip '?' */
        curstr++;
        /* Read query */
        tmpstr = curstr;
        while ( '\0' != *tmpstr && '#' != *tmpstr ) {
            tmpstr++;
        }
        len = tmpstr - curstr;
        purl->query = malloc(sizeof(char) * (len + 1));
        if ( NULL == purl->query ) {
            parsed_url_free(purl);
            return NULL;
        }
        (void)strncpy(purl->query, curstr, len);
        purl->query[len] = '\0';
        curstr = tmpstr;
    }

    /* Is fragment specified? */
    if ( '#' == *curstr ) {
        /* Skip '#' */
        curstr++;
        /* Read fragment */
        tmpstr = curstr;
        while ( '\0' != *tmpstr ) {
            tmpstr++;
        }
        len = tmpstr - curstr;
        purl->fragment = malloc(sizeof(char) * (len + 1));
        if ( NULL == purl->fragment ) {
            parsed_url_free(purl);
            return NULL;
        }
        (void)strncpy(purl->fragment, curstr, len);
        purl->fragment[len] = '\0';
        curstr = tmpstr;
    }

    return purl;
}

/*
 * Free memory of parsed url
 */
void parsed_url_free(struct parsed_url *purl)
{
    if ( NULL != purl ) {
        if ( NULL != purl->scheme ) {
            free(purl->scheme);
        }
        if ( NULL != purl->host ) {
            free(purl->host);
        }
        if ( NULL != purl->port ) {
            free(purl->port);
        }
        if ( NULL != purl->path ) {
            free(purl->path);
        }
        if ( NULL != purl->query ) {
            free(purl->query);
        }
        if ( NULL != purl->fragment ) {
            free(purl->fragment);
        }
        if ( NULL != purl->username ) {
            free(purl->username);
        }
        if ( NULL != purl->password ) {
            free(purl->password);
        }
        free(purl);
    }
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

