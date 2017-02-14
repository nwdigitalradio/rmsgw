/*
 *			b a n n e r . c
 * $Revision: 29 $
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
static char	*svnid = "$Id: banner.c 29 2008-04-29 21:26:20Z eckertb $";
#endif

#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#include "rms.h"
#include "rmslib.h"
#include "rmsgw.h"


/***
 *  banner(filedescriptor, bannerfile)
 *
 *  open and send the contents of the banner file to the rf link
 */
int banner(int fd, char *bfile)
{
     FILE *bfp;
     char lbuf[MAXBTEXT + 1];

     /*
      * open banner file, and send each line found there
      */
     if ((bfp = fopen(bfile, "r")) == NULL) {
	  /*
	   * log a message and return
	   */
	  syslog(LOG_WARNING, "WARNING: can't open banner file '%s'",
		 bfile);
	  return(-1);
     }
	  
     while (fgets(lbuf, MAXBTEXT, bfp) != NULL) {
	  syslog(LOG_DEBUG, "Banner Line: %s", lbuf);
	  if (sendrf(fd, lbuf, strlen(lbuf), SRF_EOLXLATE) < 0) {
	       /*
		* log a message and return
		*/
	       syslog(LOG_ERR, "Banner FAILED! (errno = %d)", errno);
	       fclose(bfp);
	       return(-1);
	  }
     }

     fclose(bfp);
     return(0);
}


