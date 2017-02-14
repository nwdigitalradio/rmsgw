#include <stdio.h>
#include <stdlib.h>

#include "rmslib.h"
main(int argc, char *argv[])
{
     config *cfg;
     char *cfgfile;
     int rc;

     if (argc > 1) {
	  cfgfile = argv[1];
     } else {
	  cfgfile = "nofile";
     }

     /*
      * get the configuration
      */
     if ((cfg = loadConfig(cfgfile, &rc)) == NULL) {
	  switch (rc) {
	  case ERR_CFG_MISSINGFILE:
	       fprintf(stderr, "Missing Config-File!\n");
	       break;
	  case ERR_CFG_INVALIDCALL:
	       fprintf(stderr, "Invalid Node-Callsign!\n");
	       break;
	  case ERR_CFG_MISSINGCALL:
	       fprintf(stderr, "Missing Node-Callsign in config!\n");
	       break;
	  default: /* should never happen! */
	       fprintf(stderr, "UNKNOWN ERROR IN CONFIG FILE\n");
	       break;
	  }
     } else {
	  printf("gwcall = %s\n", cfg->gwcall);
	  printf("ctext = [%s]\n", cfg->ctext);
     }

     exit(0);
}
