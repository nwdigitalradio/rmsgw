/*
 *			l o a d c o n f i g . c
 * $Revision: 176 $
 * $Author: eckertb $
 *
 * RMS Gateway
 *
 * Copyright (c) 2004-2014 Hans-J. Barthen - DL5DI
 * Copyright (c) 2008-2014 Brian R. Eckert - W3SG
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
static char	svnid[] = "$Id: loadconfig.c 176 2014-10-27 09:07:54Z eckertb $";
#endif

#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "rmslib.h"
#include "mapname.h"

static config conf;

/*
 * logical name of configuration variables
 */
static char *logicals[] = {
     "GWCALL",
     "CHANNELFILE",
     "GRIDSQUARE",
     "BANNERFILE",
     "LOGFACILITY",
     "LOGMASK",
     "PYTHON",
     "VERSIONUPD",
     "CHANNELUPD",
     "RMSGWENV",
     "RMSGWACIENV",
     "HOOKDIR",
     NULL
};

/*
 * physical varibables for pointing to values of the logicals
 * NOTE: MAKE SURE THEY MATCH UP WITH THE LOGICALS ABOVE
 */
static char **physicals[] = {
     &conf.gwcall,
     &conf.channelfile,
     &conf.gridsquare,
     &conf.bannerfile,
     &conf.logfacility,
     &conf.logmask,
     &conf.python,
     &conf.versionupd,
     &conf.channelupd,
     &conf.rmsgwenv,
     &conf.rmsgwacienv,
     &conf.hookdir
};
     

/***
 *  loadConfig(config file name, pointer to int return code)
 *
 *  read the configuration and file and store
 *  values in the config structure
 */
config *loadConfig(const char *cfgfile, int *rc)
{
     char buf[MAXBUF];
     FILE *cfp;

     *rc = 0;

     syslog(LOG_DEBUG, "using config file %s", cfgfile);

     /*
      * initialize the configuration parameters
      */
     conf.gwcall = conf.logfacility = conf.logmask = NULL;
     conf.channelfile = conf.gridsquare = conf.bannerfile = NULL;
     conf.logfacility = conf.logmask = conf.python = NULL;
     conf.versionupd = conf.channelupd = NULL;
     conf.rmsgwenv = conf.rmsgwacienv = NULL;
     conf.hookdir = NULL;

     /*
      * no error on logical not found
      */
     if (mapctl(MCTL_CLRNOTFNDERR) < 0) {
	  *rc = ERR_CFG_MAPERROR;
	  goto bailout;
     }

     /*
      * map the configuration variables
      */
     if (fmapname(cfgfile, logicals, physicals) < 0) {
	  *rc = ERR_CFG_MAPERROR;
	  goto bailout;
     }

     /*
      * check that we have a valid gateway call, return appropriate
      * errors if not
      */
     if (conf.gwcall == NULL) {
	  *rc = ERR_CFG_MISSINGCALL;
	  goto bailout;
     } else if (strlen(conf.gwcall) > MAXGWCALL) {
	  *rc = ERR_CFG_INVALIDCALL;
	  goto bailout;
     } else {
	  upcase(conf.gwcall);
     }

     /*
      * adjust for any other defaults we can do here
      */
     if (conf.hookdir == NULL) {
	  conf.hookdir = strdup(HOOKDIR);
     }

     /*
      * other values not being set are okay, as long as defaults are
      * handled elsewhere in the gateway code
      */

     /*
      * if we caught an error, return NULL, otherwise return
      * a pointer to our internal config structure
      */
 bailout:
     if (*rc != 0) {
	  return(NULL);
     }

     return(&conf); /* successfully loaded config */
}

