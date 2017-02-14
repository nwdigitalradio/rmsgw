/*
 *			s e n d D a t a G r a m . c
 * $Revision: 134 $
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
static char svnid[] = "$Id: senddatagram.c 134 2011-11-28 21:40:58Z eckertb $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>


/***
 *  sendDataGram()
 *
 *  send a datagram (message) using UDP
 *
 *  returns: 0 on success, -1 on error
 */
int sendDataGram(char *host, char *port, unsigned char* msg, int msg_len)
{
     int sd;
     struct sockaddr_in addr;
     struct addrinfo hints;
     struct addrinfo *ai;
     struct addrinfo *p;
     int rc = 0;

     syslog(LOG_DEBUG, "sendDataGram(host=\"%s\", port=\"%s\", msg=\"%s\", msg_len=%d)",
	    host, port, msg, msg_len);

     /*
      * setup hints for getaddrinfo(3)
      */
     hints.ai_family = AF_UNSPEC; /* can be ipv4 or ipv6 */
     hints.ai_socktype = SOCK_DGRAM; /* datagram */
     hints.ai_flags = 0;
     hints.ai_protocol = 0; /* any protocol */

     /*
      * resolve host into a list of addresses
      */
     if ((rc = getaddrinfo(host, port, &hints, &ai)) != 0) {
	  syslog(LOG_ERR, "ERROR: getaddrinfo() failed (%s)", gai_strerror(rc));
	  return(-1);
     }
 
     if (ai == NULL) {
	  syslog(LOG_ERR, "ERROR address lookup returned no results");
	  return(-1);
     }
 
     for (p = ai; p != NULL; p = p->ai_next) {
	  if ((sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
	       syslog(LOG_INFO, "INFO: sendDataGram(): failed socket() (errno = %d). Continuing...",
		      errno);
	       continue; /* try next "addrinfo" */
	  } /* else - try to connect using the obtained socket */

	  if (connect(sd, p->ai_addr, p->ai_addrlen) != -1) {
	       break; /* connect succeeded */
	  }
	  syslog(LOG_INFO, "INFO: sendDataGram(): failed connect() (errno = %d). Continuing...",
	       errno);

       close(sd); /* close this socket and try next in list */
     }

     /*
      * free allocated memory for list obtain before proceeding
      */
     freeaddrinfo(ai);

     /*
      * did we exhaust the list without making a successful socket/connect
      * sequence?
      */
     if (!p) {
	  syslog(LOG_ERR, "ERROR: sendDataGram(): failed to connect.");
	  return(-1);
     }

     if (send(sd, msg, msg_len, 0) < 0) {
          fprintf(stderr,
		  "ERROR: sendDataGram() failed send() (errno = %d)\n",
		  errno);
	  return(-1);
     }

     close(sd);

     return(0);
}
