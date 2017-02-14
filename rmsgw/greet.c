/*
 *			g r e e t . c
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
 */
#ifndef lint
static char svnid[] = "$Id: greet.c 167 2014-09-30 10:27:26Z eckertb $";
#endif /* lint */

#include <string.h>
#include <unistd.h>
#include <syslog.h>

#include "rms.h"
#include "rmslib.h"

#include "rmsgw.h"


/***
 *  greet(fd, cfg)
 *
 *  format and write the greeting message
 */
void greet(int fd, config *cfg, version_blk *ver)
{
     char greeting[MAXGREETING];

     /*
      * format the greeting string to send to the RF side
      *
      * NOTE: we use newline (\n) to signify a newline, and the
      * RF sending routines will replace that with the appropriate
      * end of line value. This keeps our strings in a standard
      * UNIX/Linux format for normal file operations
      */
     snprintf(greeting, sizeof(greeting), "\n%s - %s %s %s (%s)\n\n",
	     cfg->gwcall, ver->package, ver->label,
	     __DATE__, cfg->gridsquare);
     /*
      * log the greeting for version identification
      * in the log
      */
     syslog(LOG_INFO, "%s", greeting);

     /*
      * send the gateway greeting
      */
     sendrf(STDOUT_FILENO, greeting, strlen(greeting), SRF_EOLXLATE);
}
