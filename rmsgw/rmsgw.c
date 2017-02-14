/*
 *			r m s g w . c
 * $Revision: 176 $
 * $Author: eckertb $
 *
 * Linux RMS Gateway
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
static char svnid[] = "$Id: rmsgw.c 176 2014-10-27 09:07:54Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "rms.h"
#include "rmslib.h"
#include "mapname.h"

#include "rmsgw.h"

char *active_configfile;

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
 *  fmtucall(call)
 *
 *  format the usercall according to established rules
 *
 *  call = raw user call
 *
 *  returns: pointer to formatted user callsign string
 */
static char *fmtucall(char *call)
{
     char *uc;

     /*
      * get the usercall, stripping of any ending '-0' SSID
      */
     if ((uc = strdup(call)) == NULL) {
	  err("INTERNAL ERROR: Can't allocate memory for user callsign!");
     } else if (strlen(uc) > MAXUSERCALL) {
	  err("ERROR: Invalid user callsign!");
     } else {
	  char *p;

	  /*
	   * does the call end in '-0'?
	   */
	  if ((p = strchr(uc, '-')) != NULL) {
	       if (!strcmp(p, "-0")) { /* yes... */
		    *p = '\0'; /* terminate the string at the '-' */
	       }
	  }
     }

     return(uc);
}


/***
 *  main()
 */
int main(int argc, char **argv)
{
     char buffer[MAXBUF];
     char *gwcallarg = NULL; /* command line supplied gateway callsign */
     char *logmaskarg = NULL; /* command line over-ride of logmask */
     char *configfilearg = NULL; /* command line over-ride of config file */
     char *bannerfilearg = NULL; /* command line over-ride of banner file */
     char *ax25port = NULL; /* the AX.25 port name of connection */
     int hostTries = 0; /* number hosts we tried for connection */
     int i;
     char *usercall = NULL;
     char *cp;
     config *cfg;
     version_blk *version;
     cmsnode *cmslist = NULL, *cmsp = NULL;
     int rc;
     int opt;
     extern int optind;
     extern char *optarg;
     extern rms_status *rms_stat;

     /*
      * set shared memory activity debugging flag
      */
     shm_debug = set_shm_debug();

     /*
      * needed initializations before we get started
      */
     paclen = DFLTPACLEN;
	
     /*
      * intially open the log and set the default log mask
      * to get started and until we get the configuration
      */
     openlog("rmsgw", LOG_CONS|LOG_PID|LOG_NDELAY, LOG_LOCAL0);
     setlogmask(LOG_UPTO(LOG_DEBUG));

     /*
      * parse the command line options and take appropriate actions
      */
     while ((opt = getopt(argc, argv, "hc:o:p:l:g:b:P:")) != -1) {
	  syslog(LOG_DEBUG, "getopt saw '-%c'", opt);
	  if (optarg != NULL && *optarg) {
	       syslog(LOG_DEBUG, "     optarg = %s", optarg);
	  }
	  switch (opt) {
	  case 'h': /* help */
	       dohelp(GWHELPFILE);
	       exit(0);
	       break;
	  case 'c': /* config file */
	       configfilearg = strdup(optarg);
	       break;
	  case 'o': /* packet length */
	       ; /* depricated: fall through to... */
	  case 'p':
	       paclen = atoi(optarg);
	       break;
	  case 'l': /* over-ride log mask */
	       logmaskarg = strdup(optarg);
	       break;
	  case 'g': /* gateway call */
	       gwcallarg = strdup(optarg);
	       break;
	  case 'b': /* banner file */
	       bannerfilearg = strdup(optarg);
	       break;
	  case 'P': /* AX.25 port of this connection */
	       ax25port = strdup(optarg);
	       break;
	  case ':':
	  case '?':
	  default:
	       err("ERROR: invalid option usage");
	       /*NOTREACHED*/
	       exit(2);
	  }
     }

     /*
      * check the packet length, and set to something sane if necessary
      */
     if (paclen < 1) {
	  paclen = DFLTPACLEN;
     }

     /*
      * remaining arg should be the usercall from ax25
      */
     if ((argc - optind) != 1) {
	  err("ERROR: missing 'usercall' after option parsing");
     }

     /*
      * general initialization and configuration
      */
     syslog(LOG_DEBUG, "configfilearg = %s", configfilearg);
     syslog(LOG_DEBUG, "configfile = %s", configfile);
     active_configfile = configfilearg ? configfilearg : configfile;
     cfg = gwinit(active_configfile, gwcallarg, logmaskarg, bannerfilearg);

     if ((version = loadVersion(RMSVERSIONFILE, &rc)) == NULL) {
	  switch (rc) {
	  case ERR_VER_MISSINGPACK:
	       err("ERROR: Version: Missing Package Name!");
	       break;
	  case ERR_VER_MISSINGPROG:
	       err("ERROR: Version: Missing Program Name!");
	       break;
	  case ERR_VER_MISSINGLAB:
	       err("ERROR Version: Missing Version Label!");
	       break;
	  case ERR_VER_MAPERROR:
	       snprintf(buffer, sizeof(buffer), "ERROR in version file: %s",
		       maperrmsg(map_errno));
	       err(buffer);
	       break;
	  default: /* should never happen! */
	       snprintf(buffer, sizeof(buffer), "UNKNOWN ERROR (%d) IN VERSION FILE", rc);
	       err(buffer);
	       break;
	  }

	  /*
	   * set falback values for version info so we can try
	   * to continue
	   */
	  if (version->package == NULL) {
	    version->package = VER_FALLBACK_PACKAGE;
	  }
	  if (version->program == NULL) {
	    version->program = VER_FALLBACK_PROGRAM;
	  }
	  if (version->label == NULL) {
	    version->label = VER_FALLBACK_LABEL;
	  }
     }

     /*
      * reopen syslog using the configured facility/logmask
      */
     closelog();
     openlog("rmsgw", LOG_CONS|LOG_PID|LOG_NDELAY,
	     mapfacility(downcase(cfg->logfacility)));
     setlogmask(LOG_UPTO(mappriority(downcase(cfg->logmask))));

     /*
      * attach the status block to the process (attached memory address will
      * be at returned value in rms_stat)
      */
     if (statblock_attach(&rms_stat) < 0) {
	  char *msg =
	       "FATAL error on gateway initialization. Cannot continue session\n\n";

	  syslog(LOG_CRIT, "FATAL: cannot get shared memory segment");
/*	  sendrf(STDOUT_FILENO, msg, strlen(msg), SRF_EOLXLATE);*/
	  sendrf(STDOUT_FILENO, msg, strlen(msg), 0);

	  closelog();
	  exit(1);
     }

     /*
      * check for unconfigured or improperly configured gateway
      * (this check is rather simplistic, but will catch a basic
      *  case of an incomplete configuration)
      */
     if (!strcmp(cfg->gwcall, "N0CALL")) {
	  char *msg =
	       "FATAL: Incomplete/incorrect gateway configuration (N0CALL)\n\n";

	  syslog(LOG_CRIT, "%s",  msg);
	  sendrf(STDOUT_FILENO, msg, strlen(msg), SRF_EOLXLATE);
	  closelog();
	  exit(1);
     }

     /*
      * update status block - set initial condition
      */
     sbset_gw_pid(rms_stat, cfg->gwcall);
     sbset_gw_state(rms_stat, GW_RUNNING, cfg->gwcall);
     sb_incr_gw_connects(rms_stat, cfg->gwcall);

     /*
      * set any environment variables for this execution environment
      */
     if (cfg->rmsgwenv != NULL) {
	  if (file_exists(cfg->rmsgwenv)) {
	       if (loadenv(cfg->rmsgwenv) != SUCCEED) {
		    syslog(LOG_WARNING, "unable to set environment variables");
	       }
	  }
     }
	 
     /*
      * call gateway startup hook
      */
     runhook(cfg->hookdir, START_RMSGW, active_configfile, NULL);

     /*
      * send the greeting to the connected client
      */
     greet(STDOUT_FILENO, cfg, version);

     /*
      * send out the banner
      */
     banner(STDOUT_FILENO, cfg->bannerfile);

     /*
      * get and format the user callsign
      */
     usercall = fmtucall(argv[optind]);
     sbset_gw_usercall(rms_stat, cfg->gwcall, usercall);

     /*
      * get a list of CMS hosts to be tried
      */
     cmslist = getcmslist();
     
     /*
      * spin through CMS host list and connect to first "available"
      */
     for (hostTries = 0, cmsp = cmslist; cmsp != NULL; cmsp = cmsp->next) {
	  sbset_gw_cms(rms_stat, cfg->gwcall, cmsp->cms_host);

	  hostTries++;

	  syslog(LOG_DEBUG, "trying %d: host = %s, port = %d, passwd = %s",
		 hostTries,
		 cmsp->cms_host, cmsp->cms_port, cmsp->cms_passwd);

	  /*
	   * set/update the status of this host (regardless of successful
	   * connection or not)
	   */
	  if (setcmsstat(cmsp) < 0) {
	       syslog(LOG_WARNING, "WARNING: setcmsstat() failed (errno = %d)",
		      errno);
	  }
	  
	  /*
	   * attempt a session between CMS and RF client
	   */
	  if ((rc = cmsSession(cmsp, cfg, ax25port, usercall)) < 0) {
	       if (rc == ERR_SESS_FATAL) { /* can't continue */
		    syslog(LOG_CRIT,
			   "FATAL ERROR: session startup not possible");
		    sbset_gw_state(rms_stat, GW_IDLE, cfg->gwcall);
		    exit(99); /* try no more */
	       }
	       /* otherwse will continue to try other hosts */
	  } else {
	       /*
		* session was successful
		*/
	       break;
	  }
     }

     /*
      * did we actually make a connection?
      */
     if (cmsp == NULL) { /* will be NULL if exhausted/bad host table */
	  if (hostTries < 1) {
	       err("ERROR: Invalid CMS host table!");
	  } else {
	       snprintf(buffer, sizeof(buffer),
		       "ERROR: failed to connect to any CMS host "
		       "(Tried %d hosts).",
		       hostTries);
	       err(buffer);
	  }
	  sbset_gw_state(rms_stat, GW_IDLE, cfg->gwcall);
	  exit(1);
     }

     sbset_gw_state(rms_stat, GW_IDLE, cfg->gwcall);

     /*
      * call gateway shutdown hook
      */
     runhook(cfg->hookdir, END_RMSGW, active_configfile, NULL);

     exit(0);
}
