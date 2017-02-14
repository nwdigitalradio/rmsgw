/*
 *			g a t e w a y . c
 * $Revision: 149 $
 * $Author: eckertb $
 *
 * RMS Gateway
 *
 * Copyright (c) 2004-2013 Hans-J. Barthen - DL5DI
 * Copyright (c) 2008-2013 Brian R. Eckert - W3SG
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
static char svnid[] = "$Id: gateway.c 149 2013-07-03 02:01:55Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>

#include "rms.h"
#include "rmslib.h"

#include "rmsgw.h"

/*
** the four sense states below are used for monitoring between
** the CMS and client for a completion handshake: when the client
** is done, it will send an "FF" to the CMS, which will either respond
** with an "FQ" to complete the conversation, or turn the line around
** to begin message traffic to the client; when the CMS is done, it
** will send an "FF" to the client, which will either respond with an
** "FQ" to complete the conversation, or turn the line around to send
** traffic to the CMS
**
** note: the lower nibble of the first byte is used for the uplink
** (CMS) sense bits, and the upper nible of the first byte is used
** for the downlink (client) sense bits
*/
#define FFUP	0x01 /* uplink (CMS) sent FF */
#define FQUP	0x02 /* uplink (CMS) sent FQ */
#define FFDN	0x10 /* downlink (client) sent FF */
#define FQDN	0x20 /* downlink (client) sent FQ */


#define BUFLEN	8192

static unsigned char buf[BUFLEN];


/***
 *  gateway()
 *
 *  gateway conversation between connection rf client and CMS
 */
struct ust gateway(int s, config *cfg, char *usercall,
		   char *passwd, channel *chnl)
{
     fd_set read_fd;
     int m = 0;
     int n = 0;
     int loop = 0;
     unsigned char *cp;
     struct ust rc;
     char mybuf[80];
     int res;
     extern rms_status *rms_stat;
     unsigned int sense = 0; /* conversation sense flags - see defines above */
     int retry_count = 0; /* AX.25 write retry counter */
	
     /*
      * initialize the return code structure
      */
     rc.errcode = 0;
     rc.bytes_recv = 0;
     rc.bytes_sent = 0;

     /*
      * perform login "chat"
      */
     sbset_gw_state(rms_stat, GW_LOGIN, cfg->gwcall);
     if (cmslogin(s, cfg, chnl, usercall, passwd) < 0) {
	  syslog(LOG_ERR, "ERROR: CMS login FAILED!");
	  rc.errcode = ERR_GW_LOGIN;
	  return(rc);
     }

     sbset_gw_state(rms_stat, GW_LOGGEDIN, cfg->gwcall);
     syslog(LOG_DEBUG, "CMS login succeeded");

     /*
      * go into non-blocking mode for the CMS socket and
      * ax25 file descriptor
      */
     if (fcntl(s, F_SETFL, O_NONBLOCK) < 0) {
	  close(s);
	  rc.errcode = ERR_GW_SOCK; /* can not open socket */
	  return(rc);
     }

     if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) < 0) {
	  close(s);
	  rc.errcode = ERR_GW_AX25; /* can not open ax25 */
	  return(rc);
     }

     /*
      * Loop until one end of the connection goes away.
      */
     for (;;) {
	  sbset_gw_state(rms_stat, GW_COMMWAIT, cfg->gwcall);
	  FD_ZERO(&read_fd);
	  FD_SET(STDIN_FILENO, &read_fd);
	  FD_SET(s, &read_fd);
 
	  syslog(LOG_DEBUG, "select (gateway)");
	  if ((res = select(s + 1, &read_fd, NULL, NULL, NULL)) < 0) {
	       syslog(LOG_ERR, "ERROR: select(), errno = %d",
		      errno);
	       /* FIXME: let it continue for the moment, but we should
		  take a more direct action here */
	  }
	  syslog(LOG_DEBUG, "select exit (gateway)");

	  if (FD_ISSET(s, &read_fd)) {
	       /*
		* If the socket indicated ready to read but returns 0 bytes
                * on a receive then the other end has closed.
                * Only ask for one byte to minimize work done by the following
                * recv statement as it is called every time through the loop.
                * n = recv(s, buf, 1, MSG_PEEK | MSG_DONTWAIT);
		*/
	       if ((n = recv(s, buf, 1, MSG_PEEK | MSG_DONTWAIT)) == 0) {
                    snprintf(mybuf, sizeof(mybuf),
			     "; INFO: Connection closed by CMS (sense = 0x%04x)\r\n",
			     sense);
		    syslog(LOG_INFO, "%s", mybuf);
                    write(STDOUT_FILENO, mybuf, strlen(mybuf));
                    rc.errcode = 0;
                    sleep(1);
                    close(s);
		    sbset_gw_state(rms_stat, GW_CONNECTED, cfg->gwcall);
                    return(rc);
	       }

	       /* syslog(LOG_DEBUG, "CMS peek sees [%.*s]", n, buf);*/

	  }

	  /*
	   * Receive from CMS and send to AX.25
	   * analyze last 4 characters (EOL on telnet link is CR/LF)
	   */
	  if (FD_ISSET(s, &read_fd)) {
	       sbset_gw_state(rms_stat, GW_SENDING, cfg->gwcall);
	       while ((n = recv(s, buf, BUFLEN, 0)) > 0) {
		    rc.bytes_sent += n;
		    cp = buf;
		    loop = 0;

		    /* Uplink sent FF? */
		    if ((*(cp+n-3) == 'F') && (*(cp+n-2) == 'F')) {
			 syslog(LOG_DEBUG, "CMS sent FF");
			 /*ffup = 1;*/
			 sense |= (FFUP);
		    } else {
			 /* Uplink sent FQ after Downlink sent FF? */
			 if ((n == 3) && (sense & (FFDN))) {
			      if ((*cp == 'F') && (*(cp+1) == 'Q')) {
				   /*fqup = 1;*/
				   sense |= (FQUP);
				   loop = CONV_TIMEOUT;
				   rc.errcode = 10; 	//normal final
				   syslog(LOG_DEBUG,
					  "CMS sent FQ (FINAL)");
			      } else {
				   /*ffdn = 0;*/
				   sense &= ~(FFDN);
				   syslog(LOG_DEBUG, "AX25 FF cleared");
			      }
			 }
		    }
                            
		    /*
		     * filter "keep alive" packet from CMS
		     * (this will not be sufficient if the buffer
		     * data we are looking for is split between
		     * multiple reads, however to handle this
		     * condition will mean changing the way the
		     * gateway loop is structured)
		     */
		    if (!strncmp(cp, ";;;;;;\r\n", 8)) {
			 continue; /* eat the buffer */
		    }

		    /*
		     * send buffer to ax25
		     */
		    retry_count = 0;
#ifdef notdef
		    syslog(LOG_DEBUG, "AX.25 buffer write: %d bytes in buffer", n);
#endif
		    syslog(LOG_DEBUG, "CMS [%.*s] -> AX25 (%d bytes)", n, cp, n);
		    while (n > 0) {

			 errno = 0; /* clear any previous error code */

#ifdef notdef
			 syslog(LOG_DEBUG, "CMS [%.*s] -> AX25",
				MIN(paclen, n), cp);
#endif
			 if ((m = write(STDOUT_FILENO, cp, MIN(paclen, n))) < 0) {
			      if (errno != EAGAIN) {
				   syslog(LOG_ERR, "ERROR: write: can't send to AX25 (errno = %d)",
					  errno);
				   close(s);
				   rc.errcode = ERR_GW_AX25IO;
				   return(rc);
			      } else { /* EAGAIN logic */
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
					syslog(LOG_ERR, "ERROR: write: can't send to AX25 after %d attempts."
					       "Giving up.", retry_count);
					close(s);
					rc.errcode = ERR_GW_AX25IO;
					return(rc);
				   }
				   if (sleep_time <= 0) { /* ignore stupid values */
					syslog(LOG_DEBUG, "write: AX25 retry");
					continue;
				   } else if (sleep_time > 10) {
					sleep_time = 10; /* we won't sleep longer than 10 secs */
				   }
				   syslog(LOG_DEBUG, "write: AX25 retry (delay = %d)", sleep_time);
				   sleep(sleep_time);
				   
			      }
			 } else {
			      /*
			       * successful write, clear retry_counter
			       */
			      retry_count = 0;
			 }

			 if (m > 0) { /* adjust buffer pointer and bytes remaining */
			      syslog(LOG_DEBUG, "AX.25: wrote %d of %d bytes remaining", m, n);
			      cp += m;
			      n -= m;
			 }
		    }
		    if(n < 0 && errno != EAGAIN) {
			 close(s);
			 rc.errcode = ERR_GW_AX25IO;
			 return(rc);
		    }
		    syslog(LOG_DEBUG, "AX.25 buffer write complete");
	       }
	  }

	  /*
	   * Receive from AX.25 and send to CMS
	   * analyze last 3 characters (EOL on ax25 link is CR)
	   */
	  if (FD_ISSET(STDIN_FILENO, &read_fd)) {
	       sbset_gw_state(rms_stat, GW_RECEIVING, cfg->gwcall);
	       while ((n = read(STDIN_FILENO, buf, BUFLEN)) > 0) {
		    rc.bytes_recv += n;
		    cp = buf;
		    loop = 0;
		    /* Downlink sent FF? */
		    if ((*(cp+n-3) == 'F') && (*(cp+n-2) == 'F')) {
			 syslog(LOG_DEBUG, "AX25 sent FF");
			 /*ffdn = 1;*/
			 sense |= (FFDN);
		    } else {
			 /* Downlink sent FQ after Uplink sent FF? */
			 if ((n == 3) && (sense & (FFUP))) {
			      if ((*cp == 'F') && (*(cp+1) == 'Q')) {
				   /*fqdn = 1;*/
				   sense |= (FQDN);
				   syslog(LOG_DEBUG,
					  "AX25 sent FQ (FINAL)");
				   loop = CONV_TIMEOUT;
				   rc.errcode = 10; /* normal final */
			      } else {
				   /*ffup = 0;*/
				   sense &= ~(FFUP);
				   syslog(LOG_DEBUG, "CMS FF cleared");
			      }
			 }
		    }

		    /*
		     * send buffer to CMS
		     */
		    while (n > 0) {
			 syslog(LOG_DEBUG, "AX25 [%.*s] -> CMS", n, cp);
			 if ((m = write(s, cp, n)) < 0) {
			      if (errno != EAGAIN) { /* need to give up on this connection */
				   syslog(LOG_ERR, "ERROR: write: can't send to CMS (errno = %d)",
					  errno);
				   close(s);
				   rc.errcode = ERR_GW_SOCKIO;
				   return(rc);
			      }
			 }
			 if (m > 0) {
			      cp += m;
			      n -= m;
			 }
		    }
		    if(n < 0 && errno != EAGAIN) {
			 close(s);
			 rc.errcode = ERR_GW_SOCKIO;
			 return(rc);
		    }
	       }
	  }
		
	  sbset_gw_state(rms_stat, GW_COMMWAIT, cfg->gwcall);

	  if(loop > 0){
	       sleep(1);
	  }

	  loop++;
		
	  if(loop > CONV_TIMEOUT) {
	       if (rc.errcode == 10){
		    rc.errcode = 0; /* normal final */
	       } else {
		    rc.errcode = 1; /* TimeOut */
	       }
	       close(s);
	       sbset_gw_state(rms_stat, GW_CONNECTED, cfg->gwcall);
	       return(rc);
	  }
     }

     sbset_gw_state(rms_stat, GW_CONNECTED, cfg->gwcall);
     return(rc);
}
