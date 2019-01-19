/*
 *			s t a t f i l e . c
 * $Revision: 64 $
 * $Author: eckertb $
 *
 * Linux RMS Gateway
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
 *
 *	set/update a status file (file used as timestamp)
 */
#ifndef lint
static char svnid[] = "$Id: statfile.c 64 2008-08-19 07:46:15Z eckertb $";
#endif /* lint */

#include <limits.h>
#include <fcntl.h>
#include <utime.h>

#include <sys/types.h>

#include "rms.h"
#include "rmslib.h"


#define STAT_MODE	0644 /* file access mode bits */


/***
 *  setstatfile(stat file pathname)
 *
 *  set/update the access/mod times of a file for simple timestamping
 *
 *  returns: 0 on success, -1 on error
 */
int setstatfile(char *statfile)
{
     int fd;

     /*
      * "touch" the status file
      */

     /*
      * make sure file exists first
      */
     if ((fd = open(statfile, O_CREAT|O_RDWR, STAT_MODE)) < 0) {
	  return(-1);
     }

     close(fd); /* don't want to leak file descriptors */

     /*
      * set [am]time to "now"
      */
     if (utime(statfile, NULL) < 0) {
	  return(-1);
     }

     return(0);
}
