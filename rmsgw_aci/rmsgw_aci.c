/*
 *			r m s g w _ a c i . c
 * $Revision: 176 $
 * $Author: eckertb $
 *
 * RMS Gateway
 *
 * Copyright (c) 2004-2009 Hans-J. Barthen - DL5DI
 * Copyright (c) 2008-2009 Brian R. Eckert - W3SG
 *
 * Questions or problems regarding this program can be emailed
 * to linux-rmsgw@w3sg.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef lint
static char svnid[] = "$Id: rmsgw_aci.c 176 2014-10-27 09:07:54Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <errno.h>
#include <limits.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "rms.h"
#include "rmslib.h"
#include "mapname.h"

#include "aci.h"

/*
 * aci globals
 */
char *active_configfile = NULL; /* the config file actually being used */

/*
 * externs
 */
extern int shm_debug;


/***
 *  dohelp()
 */
static void dohelp(char *helpfile)
{
     if (printfile(helpfile) < 0) {
	  fprintf(stderr, "Can't dislay helpfile '%s' (errno = %d)\n",
		  helpfile, errno);
     }
}


/***
 *  main()
 */
int main(int argc, char *argv[])
{
     char *gwcallarg = NULL; /* command line supplied gateway callsign */
     char *logmaskarg = NULL; /* command line over-ride of logmask */
     char *configfilearg = NULL; /* command line over-ride of config file */
     config *cfg;
     time_t age;
     char statfile[PATH_MAX];
     int rc; /* generic return code var */
     int opt;
     version_blk *version;
     time_t version_update_interval = (time_t) 0;
     extern int optind;
     extern char *optarg;
     extern rms_status *rms_stat;

     /*
      * set shared memory activity debugging flag
      */
     shm_debug = set_shm_debug();

     /*
      * intially open the log and set the default log mask
      * to get started and until we get the configuration
      */
     openlog("rmsgw_aci", LOG_CONS|LOG_PID|LOG_NDELAY, LOG_LOCAL0);
     setlogmask(LOG_UPTO(LOG_INFO));

     /*
      * parse the command line options and take appropriate actions
      */
     while ((opt = getopt(argc, argv, "hc:l:g:")) != -1) {
	  switch (opt) {
	  case 'h': /* help */
	       dohelp(ACIHELPFILE);
	       exit(0);
	       break;
	  case 'c': /* config file */
	       configfilearg = strdup(optarg);
	       break;
	  case 'l': /* log mask */
	       logmaskarg = strdup(optarg);
	       break;
	  case 'g': /* gateway call */
	       gwcallarg = strdup(optarg);
	       break;
	  case ':':
	  case '?':
	  default:
	       syslog(LOG_ERR, "Invalid option usage");
	       exit(1);
	  }
     }

     /*
      * get the version info
      */
     if ((version = loadVersion(RMSVERSIONFILE, &rc)) == NULL) {
	  switch (rc) {
	  case ERR_VER_MISSINGPACK:
	       syslog(LOG_ERR, "ERROR: Version: Missing Package Name!");
	       break;
	  case ERR_VER_MISSINGPROG:
	       syslog(LOG_ERR, "ERROR: Version: Missing Program Name!");
	       break;
	  case ERR_VER_MISSINGLAB:
	       syslog(LOG_ERR, "ERROR Version: Missing Version Label!");
	       break;
	  case ERR_VER_MAPERROR:
	       syslog(LOG_ERR,
		      "ERROR: in version file: %s", maperrmsg(map_errno));
	       break;
	  default: /* should never happen! */
	       syslog(LOG_CRIT, "UNKNOWN ERROR (%d) IN VERSION FILE", rc);
	       break;
	  }

	  fprintf(stderr, "%s: failed to load version info. See syslog.\n",
		  argv[0]);
	  exit(1);
     }

     /*
      * get the configuration
      */
     active_configfile = configfilearg ? configfilearg : configfile;
     if ((cfg = loadConfig(active_configfile, &rc)) == NULL) {
	  switch (rc) {
	  case ERR_CFG_MISSINGFILE:
	       syslog(LOG_ERR, "ERROR: missing config file");
	       break;
	  case ERR_CFG_INVALIDCALL:
	       syslog(LOG_ERR, "ERROR: invalid gateway callsign");
	       break;
	  case ERR_CFG_MISSINGCALL:
	       syslog(LOG_ERR,
		      "ERROR: missing gateway callsign in config");
	       break;
	  case ERR_CFG_MAPERROR:
	       syslog(LOG_ERR,
		      "ERROR: config file: %s", maperrmsg(map_errno));
	       break;
	  default: /* should never happen! */
	       syslog(LOG_CRIT, "UNKNOWN ERROR IN CONFIG FILE");
	       break;
	  }
	  fprintf(stderr, "%s: failed to load configuration. See syslog.\n",
		  argv[0]);
	  exit(1);
     }

     /*
      * replace configured values with any supplied command
      * line over-rides
      */
     if (gwcallarg) { /* gateway call over-ride? */
	  free(cfg->gwcall);
	  cfg->gwcall = gwcallarg;
     }

     if (logmaskarg) { /* logmask over-ride? */
	  free(cfg->logmask);
	  cfg->logmask = logmaskarg;
     }

     /*
      * reopen syslog using the configured facility/logmask
      */
     closelog();
     openlog("rmsgw_aci", LOG_CONS|LOG_PID|LOG_NDELAY,
	     mapfacility(downcase(cfg->logfacility)));
     setlogmask(LOG_UPTO(mappriority(downcase(cfg->logmask))));

     syslog(LOG_INFO,"%s - %s ACI %s %s (%s)",
 	    cfg->gwcall, version->package, version->label,
	    __DATE__, cfg->gridsquare);

     /*
      * check for unconfigured or improperly configured gateway
      * (this check is rather simplistic, but will catch a basic
      *  case of an incomplete configuration)
      */
     if (!strcmp(cfg->gwcall, "N0CALL")) {
	  syslog(LOG_CRIT,
		 "FATAL: Incomplete/incorrect gateway configuration (N0CALL)");
	  exit(1);
     }

     /*
      * attach the status block to the process (attached memory address will
      * be at returned value in rms_stat)
      */
     if (statblock_attach(&rms_stat) < 0) {
	  syslog(LOG_CRIT, "FATAL: cannot attach shared memory segment");

	  fprintf(stderr,
		  "FATAL error: can't attach status block (see syslog)\n");

	  closelog();
	  exit(1);
     }

     /*
      * update status block - set initial condition
      */
     sbset_aci_pid(rms_stat);
     sbset_aci_state(rms_stat, ACI_RUNNING);

     /*
      * set any environment variables for this execution environment
      */
     if (cfg->rmsgwacienv != NULL) {
	  if (file_exists(cfg->rmsgwacienv)) {
	       if (loadenv(cfg->rmsgwacienv) != SUCCEED) {
		    syslog(LOG_WARNING, "unable to set environment variables");
	       }
	  }
     }
	 
     /*
      * call gateway startup hook
      */
     runhook(cfg->hookdir, START_RMSGW_ACI, active_configfile, NULL);

     /*
      * update channel status
      */
     if (wl2k_aci(cfg, rms_stat) < 0) {
	  sbset_aci_state(rms_stat, ACI_IDLE);
	  statblock_detach(rms_stat);
	  exit(1); /* failed aci */
     }/* else... */

     /*
      * get the update interval for version info
      */
     if (cfg->versionupd == NULL || ((version_update_interval = atol(cfg->versionupd)) == 0)) {
	  version_update_interval = DFLT_VERSIONUPD;
     }
     syslog(LOG_DEBUG, "Version update interval = %ld", version_update_interval);
       
     /*
      * check status file for version update -
      * if the version hasn't been updated in an day or more,
      * send it
      */
     sprintf(statfile, "%s/.version.%s", CMSSTATDIR, cfg->gwcall);
     age = fileage(statfile);
     syslog(LOG_DEBUG, "Version file '%s' age %ld", statfile, age);

     if (age >= version_update_interval) {
	  sbset_aci_state(rms_stat, ACI_VERSIONUPDATE);

	  runhook(cfg->hookdir, PRE_RMSGW_ACI_VERSION_UPDATE, active_configfile, statfile, NULL);

	  sendVersion(cfg);
	  sbset_aci_state(rms_stat, ACI_RUNNING);

	  /*
	   * update the status file
	   */
	  syslog(LOG_DEBUG, "touch %s", statfile);
	  setstatfile(statfile);

	  runhook(cfg->hookdir, POST_RMSGW_ACI_VERSION_UPDATE, active_configfile, statfile, NULL);
     }

     sbset_aci_state(rms_stat, ACI_IDLE);
     statblock_detach(rms_stat);

     runhook(cfg->hookdir, END_RMSGW_ACI, active_configfile, NULL);

     exit(0);
}
