//Configuration subsystem

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "configuration.h"


int  getGlobalConfigFile (int argc, char **argv)
     {
       char *cvalue = NULL;
       int c;
          
       while ((c = getopt (argc, argv, "c:")) != -1)
         switch (c)
           {
           case 'c':
             cvalue = optarg;
			 strcpy(G_mainConfigFile,cvalue);
             break;
           default:
             abort ();
           }
      
       if (cvalue == NULL)
		   sprintf(G_mainConfigFile,"config.ini");
		   
	printf("Using config file %s\n",G_mainConfigFile);	   
 
       return 0;
}

