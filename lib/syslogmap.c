/*
 *		s y s l o g m a p . c
 * $Revision: 167 $
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
 *
 * Description:
 *	map a string representation of the syslog log priorities to
 *	it's associated defined value
 */
#ifndef lint
static char svnid[] = "$Id: syslogmap.c 167 2014-09-30 10:27:26Z eckertb $";
#endif /* lint */

#include <stdio.h>

#define SYSLOG_NAMES	1
#include <syslog.h>

#include "rmslib.h"

/***
 *  mappriority(priority)
 *
 *  priority = a name used to represent the internal priority definition
 *	one of: emerg, alert, crit, error, warning, notice, info, debug
 *	(decending rank order)
 *
 *  returns: the defined value of the priority
 */
int mappriority(char *pri)
{
     int	i;

     /*
      * loop through the syslog defined names until a match
      * or the list is exhausted
      */
     for (i = 0; prioritynames[i].c_name != NULL; i++) {
	  if (!strcmp(prioritynames[i].c_name, pri)) {
	       return(prioritynames[i].c_val);
	  }
     }

     /*
      * didn't find a match, do something sensible
      */
     return(LOG_INFO);
}


/***
 *  mapfacility(facility)
 *
 *  facility = a name used to represent the internal facility definition
 *	one of: auth, authpriv, cron, daemon, ftp, kern, lpr, mail,
 *		news, syslog, user, uucp, local0, local1, local 2, local3
 *		local4, local5, local6, local7
 *
 *  returns: the defined value of the facility
 */
int mapfacility(char *facility)
{
     int	i;

     /*
      * loop through the syslog defined names until a match
      * or the list is exhausted
      */
     for (i = 0; facilitynames[i].c_name != NULL; i++) {
	  if (!strcmp(facilitynames[i].c_name, facility)) {
	       return(facilitynames[i].c_val);
	  }
     }

     /*
      * didn't find a match, do something sensible
      */
     return(LOG_LOCAL0);
}

