/*
 *			s e n d r f . c
 * $Revision: 118 $
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
static char	svnid[] = "$Id: sendrf.c 118 2010-08-31 11:13:15Z eckertb $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>

#include <sys/select.h>
#include <sys/time.h>

#include "rmslib.h"


#ifdef notdef
/***
 *  sockReady()
 *
 *  wait for socket to become ready or timeout
 *
 *  returns: 1 if ready, 0 if timeout/error
 */
static int sockReady(int sd)
{
     struct timeval tv;
     fd_set writefds;
     int sStatus;
     socklen_t sStatusSize = sizeof(sStatus);
     int rc;

     /*
      * wait for socket ready for write or complete failure
      * completely
      */
     tv.tv_sec = 5;
     tv.tv_usec = 0; 
	  
     FD_ZERO(&writefds);       
     FD_SET(sd, &writefds);        

     if ((rc = select(sd + 1, NULL, &writefds, NULL, &tv)) > 0) {
	  /*
	   * check the socket status for error
	   */
	  if (getsockopt(sd, SOL_SOCKET, SO_ERROR,
			 &sStatus, &sStatusSize) < 0) {
	       /*
		* bad stuff!
		*/
	       syslog(LOG_CRIT,
		      "ERROR: sockReady(): getsockopt()failed (%d)",
		      errno);
	       return(0);          
	  } else if (sStatus != 0) { /* socket error? */
	       syslog(LOG_ERR,
		      "ERROR: sockReady() failed (SO_ERROR = %d)",
		      sStatus);
	       return(0);
	  }
			  
	  /* socket ready */
	  return(1);
     } else {
	  /*
	   * timed out or error
	   */
	  if (rc < 0) {
	       syslog(LOG_ERR, "ERROR: sockReady() select error (%d)",
		      errno);
	  } else { /* timeout */
	       syslog(LOG_ERR,
		      "ERROR: sockReady(): timeout waiting for socket.");
	  }
	  return(0);
     }
     /*NOTREACHED*/
}
#endif /* notdef */

/***
 *  sendrf(fd, buffer, buffer len, send options)
 *
 *  send a buffer to the (supposed) rf link with appropriate
 *  charater translations and treatments (it is almost like
 *  adding a line discipline)
 *
 *  returns: count of bytes sent, or -1 on error
 */
int sendrf(int fd, register char *buf, register int len, int opts)
{
     char *sbuf; /* actual send buffer */
     int slen = len; /* length of internal send buffer */
     register char *bp, *sbp; /* buffer pointers for copying/translation */
     int nleft, nwritten;
     int retry_count;

     /*
      * allocate an internal buffer for data conversion before
      * sending (there of course is a slight performance
      * penalty here because of the extra copying, but it
      * eliminates the side effect of modifying the original
      * buffer that was passed in)
      */
     if (opts & (SRF_ADDEOL)) {
	  slen++; /* make space for the final end of line */
     }

     if ((sbuf = malloc(slen)) == NULL) {
	  /*
	   * couldn't allocate memory (conforming malloc implementations
	   * set errno)
	   */
	  return(-1);
     }

     /*
      * copy passed buffer to internal buffer (sbuf) and translate data
      * as appropriate
      */
     for (bp = buf, sbp = sbuf; bp < buf + len; bp++, sbp++) {
	  switch(*bp) {
	  case '\n': /* translate UNIX/Linux newline */
	       if (opts & (SRF_EOLXLATE)) {
		    *sbp = '\r';
		    break;
	       }
	       /* fall through to... */
	  default:
	       *sbp = *bp;
	       break;
	  }
     }

     /*
      * add in the final end of line if requested
      */
     if (opts & (SRF_ADDEOL) && slen > 0) {
	  *(sbuf + slen - 1) = '\r';
     }

     /*
      * send it!
      */
     syslog(LOG_DEBUG, "sendrf(): [%.*s]", slen, sbuf);
     retry_count = 0;
     for (nleft = slen; nleft > 0; nleft -= nwritten, buf += nwritten) {
	  if ((nwritten = write(fd, buf, nleft)) < 0) {
	       if (errno != EAGAIN) {
		    free(sbuf);
		    return(-1);
	       } else {
		    /*
		     * Delay before retrying the write --
		     * we'll do a back-off on each retry, but
		     * only to a point (we don't want to
		     * wait minutes before retrying, just a few
		     * seconds). We will also limit the number of
		     * time we retry the write and then just give up.
		     */
		    int sleep_time = retry_count++; /* sleep time follows retries */

		    if (retry_count > 15) { /* arbitrary uppper limit for retries */
			 syslog(LOG_ERR, "ERROR: sendrf(): giving up after %d attempts.",
				retry_count);
			 free(sbuf);
			 return(-1);
		    }
		    if (sleep_time <= 0) { /* ignore stupid values */
			 syslog(LOG_DEBUG, "sendrf(): write retry");
			 continue;
		    } else if (sleep_time > 10) {
			 sleep_time = 10; /* we won't sleep longer than 10 secs */
		    }
		    syslog(LOG_DEBUG, "sendrf(): write retry (delay = %d)", sleep_time);
		    sleep(sleep_time);
	       }
	  } else {
	       /*
		* successful write, clear retry_counter
		*/
	       retry_count = 0;
	  }
	  syslog(LOG_DEBUG, "sendrf(): wrote %d of %d characters", nwritten, nleft);
     }
	

     /*
      * free our internal buffer and return result of send()
      */
     free(sbuf);

     return(slen - nleft);
}
