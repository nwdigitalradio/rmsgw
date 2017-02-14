/*
 *			g w i n i t . c
 * $Revision: 131 $
 * $Author: eckertb $
 *
 * RMS Gateway
 *
 * Copyright (c) 2004-2008 Hans-J. Barthen - DL5DI
 * Copyright (c) 2008 Brian R. Eckert - W3SG
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
static char svnid[] = "$Id: gwinit.c 131 2011-11-28 19:31:57Z eckertb $";
#endif /* lint */

#include <syslog.h>
#include <stdlib.h>

#include "rms.h"
#include "rmslib.h"
#include "mapname.h"

#include "rmsgw.h"


/***
 *  gwinit(configfile, gwcall, logmask, bannerfile)
 *
 *  initialize the gateway
 *
 *  configfile = config file to be loaded
 *  gwcall = gateway call over-ride
 *  logmask = logmask over-ride
 *  bannerfile = banner file over-ride
 *
 *  returns: pointer to loaded config info
 */
config *gwinit(const char *configfile, char *gwcall, char *logmask,
	       char *bannerfile)
{
     config *cfg;
     int rc; /* generic return code var */
     char buffer[MAXBUF];

     /*
      * get the configuration
      */
     if ((cfg = loadConfig(configfile, &rc)) == NULL) {
	  switch (rc) {
	  case ERR_CFG_MISSINGFILE:
	       err("ERROR: Missing Config-File!");
	       break;
	  case ERR_CFG_INVALIDCALL:
	       err("ERROR: Invalid Gateway-Callsign!");
	       break;
	  case ERR_CFG_MISSINGCALL:
	       err("ERROR IN CONFIG FILE - Missing Gateway-Callsign!");
	       break;
	  case ERR_CFG_MAPERROR:
	       snprintf(buffer, sizeof(buffer), "ERROR in config file: %s",
		       maperrmsg(map_errno));
	       err(buffer);
	       break;
	  default: /* should never happen! */
	       snprintf(buffer, sizeof(buffer), "UNKNOWN ERROR (%d) IN CONFIG FILE", rc);
	       err(buffer);
	       break;
	  }
     }

     /*
      * replace configured values with any supplied command
      * line over-rides
      */
     if (gwcall) { /* gateway call over-ride? */
	  free(cfg->gwcall);
	  cfg->gwcall = gwcall;
     }

     if (logmask) { /* logmake over-ride? */
	  free(cfg->logmask);
	  cfg->logmask = logmask;
     }

     if (bannerfile) { /* banner file over-ride */
	  free(cfg->bannerfile);
	  cfg->bannerfile = bannerfile;
     }

     return(cfg);
}
