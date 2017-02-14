/*
 *			c m s c o n n e c t . c
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
static char svnid[] = "$Id: cmsconnect.c 167 2014-09-30 10:27:26Z eckertb $";
#endif /* lint */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#include <netdb.h>
#include <netinet/in.h>

#include "rms.h"
#include "rmslib.h"


/***
 *  cmsConnect (cms host pointer)
 *
 *  create a socket and connect to indicated host
 *
 *  returns: connected socket descriptor on success, -1 on error
 */
int cmsConnect(cmsnode *c)
{
     struct timeval tv;
     fd_set writefds;
     long arg;
     int rc, sd;
     int sStatus;
     socklen_t sStatusSize = sizeof(sStatus);
     struct sockaddr_in sockAddr;
     struct hostent *hent;  
     char mybuf[80];

     if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	  err("FATAL ERROR! Could not create socket\n");
     }

     snprintf(mybuf, sizeof(mybuf), "INFO: Host Name %s, Port %d \r\n",
	     c->cms_host,
	     c->cms_port);
     syslog(LOG_DEBUG,"%s", mybuf);
     /*     send(STDOUT_FILENO, mybuf, strlen(mybuf), 0);*/
     write(STDOUT_FILENO, mybuf, strlen(mybuf));

     if ((hent = gethostbyname(c->cms_host)) == NULL) {
	  snprintf(mybuf,sizeof(mybuf), "ERROR: Unknown host %s. Skipping...\r\n",
		  c->cms_host);
	  syslog(LOG_ERR, "%s", mybuf);
	  /* send(STDOUT_FILENO, mybuf, strlen(mybuf), 0);*/
	  write(STDOUT_FILENO, mybuf, strlen(mybuf));
	  return(-1);
     }

     sockAddr.sin_family = AF_INET;
     sockAddr.sin_port = htons(c->cms_port);
     sockAddr.sin_addr = *((struct in_addr *)hent->h_addr);
     bzero(&(sockAddr.sin_zero), 8);

     /*
      * Set socket to non-blocking
      */
     arg = fcntl(sd, F_GETFL, NULL); 
     arg |= O_NONBLOCK; 
     syslog(LOG_DEBUG, "Setting socket to non-blocking...");
     fcntl(sd, F_SETFL, arg);   

     if (connect(sd, (struct sockaddr *)&sockAddr,
		 sizeof(struct sockaddr)) < 0) {
	  if (errno == EINPROGRESS) {
	       /*
		* wait for socket to become ready or connect to fail
		* completely
		*/
	       while (1) {
		    tv.tv_sec = CONNECT_TIMEOUT;
		    tv.tv_usec = 0; 

		    FD_ZERO(&writefds);       
		    FD_SET(sd, &writefds);        

		    if (select(sd + 1, NULL, &writefds, NULL, &tv) > 0) {
			 /*
			  * check the socket status for error
			  */
			 if (getsockopt(sd, SOL_SOCKET, SO_ERROR,
					&sStatus, &sStatusSize) < 0) {
			      /*
			       * bad stuff!
			       */
			      snprintf(mybuf, sizeof(mybuf),
				      "ERROR: getsockopt()failed for node %s (errno = %d). "
				      "Skipping...\r\n",
				      c->cms_host, errno);
			      syslog(LOG_CRIT, "%s", mybuf);
			      /* send(STDOUT_FILENO, mybuf, strlen(mybuf), 0);*/
			      write(STDOUT_FILENO, mybuf, strlen(mybuf));
			      close(sd);
			      return(-1);          
			 } else if (sStatus != 0) { /* socket error? */
			      snprintf(mybuf, sizeof(mybuf),
				     "Connection failed (SO_ERROR = %d)\r\n",
				     sStatus);
			      syslog(LOG_ERR, "%s", mybuf);
			      /*send(STDOUT_FILENO, mybuf, strlen(mybuf), 0);*/
			      write(STDOUT_FILENO, mybuf, strlen(mybuf));
			      return(-1);
			 }
			  
			 /* socket ready */
			 snprintf(mybuf, sizeof(mybuf), "Connected\r\n");
			 syslog(LOG_DEBUG, "%s", mybuf);
			 /*send(STDOUT_FILENO, mybuf, strlen(mybuf), 0);*/
			 write(STDOUT_FILENO, mybuf, strlen(mybuf));

			 break;
		    } else {
			 /*
			  * timed out or error
			  */
			 snprintf(mybuf, sizeof(mybuf),
				 "ERROR: Timeout connecting to node %s. "
				 "Skipping...\r\n", c->cms_host);
			 syslog(LOG_ERR, "%s", mybuf);
			 /*send(STDOUT_FILENO, mybuf, strlen(mybuf), 0);*/
			 write(STDOUT_FILENO, mybuf, strlen(mybuf));
			 close(sd);
			 return(-1);          
		    }  
	       }
	  } else {
	       /*
		* probably should kill the process here!
		*/
	       snprintf(mybuf, sizeof(mybuf), "FATAL ERROR: connection failed\r\n");
	       syslog(LOG_CRIT, "%s", mybuf);
	       /*send(STDOUT_FILENO, mybuf, strlen(mybuf), 0);*/
	       write(STDOUT_FILENO, mybuf, strlen(mybuf));
	       close(sd);
	       return(-1);       
	  }
     } else {
	  syslog(LOG_DEBUG, "cmsConnect(): Connected immediatly.");
     }

     /*
      * Set back to blocking mode and return
      * the connected socket descriptor
      */
     arg = fcntl(sd, F_GETFL, NULL); 
     arg &= ~O_NONBLOCK; 
     syslog(LOG_DEBUG, "Setting socket back to blocking...");
     fcntl(sd, F_SETFL, arg);           
     return(sd);
}
