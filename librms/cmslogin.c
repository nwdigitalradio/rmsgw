/*
 *			c m s l o g i n . c
 * $Revision: 173 $
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
static char svnid[] = "$Id: cmslogin.c 173 2014-10-19 10:12:34Z eckertb $";
#endif /* lint */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
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

#define TIMEOUT	1	/* timeout return status */


static int	timedOut = 0;

/***
 *  onalrm()
 *
 *  expectsend() alarm catcher
 */
static void onalrm(int sig)
{
     timedOut++;
}


/***
 *  expectsend()
 *
 *
 *  returns: SUCCEED on success, FAIL on failure, TIMEOUT on timeout
 */
static int expectsend(int fd, char *expectStr, char *sendStr, int timeOut)
{
     int nwritten, nread;
     char rcvbuf[2048];
     char *rp = rcvbuf;

     memset(rcvbuf, '\0', sizeof(rcvbuf));

     syslog(LOG_DEBUG, "expectsend(): expect [%s], send [%s]", expectStr, sendStr);

     /*
      * if we expect something, wait for it before sending
      */
     if (expectStr && strlen(expectStr) > 0) {
	  *rp = '\0'; /* initialially an empty buffer */

	  /*
	   * setup alarm handler and set the alarm
	   */
	  timedOut = 0;
	  signal(SIGALRM, onalrm);
	  alarm(timeOut);

	  while (!timedOut && ((char *) strstr(rcvbuf, expectStr) == NULL)) {
	       /*
		* if pointer is at end of buffer, shift buffer
		* contents, letting oldest byte read "fall off"
		* the front
		*/
	       /* code here */
	       /*
		* read and log
		* read one char at a time to simplify buffer handling
		*/
	       if ((nread = read(fd, rp, 1)) < 0) {
		    syslog(LOG_ERR, "expectsend(): read error (errno = %d)",
			   errno);
		    return(FAIL);
	       }

/*
	       if ((nread = read(fd, rp,
				 sizeof(rcvbuf) - (rp - rcvbuf))) < 0) {
		    syslog(LOG_ERR, "expectsend(): read error (errno = %d)",
			   errno);
		    return(FAIL);
	       }
*/
	       rp += nread;
	       syslog(LOG_DEBUG, "expectsend() read %d bytes, buf = [%s]",
		      nread, rcvbuf);
	  }

	  alarm(0); /* stop the alarm */

	  if (timedOut) {
	       syslog(LOG_WARNING, "WARNING: expectsend(): expect timeout; "
		      "buffer = [%.*s]",
		      (int) (rp - rcvbuf), rcvbuf);
	       return(TIMEOUT);
	  }
	  
	  syslog(LOG_DEBUG, "expectsend() got [%.*s]",
		 (int) (rp - rcvbuf), rcvbuf);
     }

     /*
      * if we have a string to send, send it!
      */
     if (sendStr && strlen(sendStr) > 0) {
	  if ((nwritten = write(fd, sendStr, strlen(sendStr))) < strlen(sendStr)) {
	       if (nwritten < 0) {
		    syslog(LOG_ERR, "expectsend(): write error (errno = %d)",
			   errno);
		    return(FAIL);
	       } else {
		    syslog(LOG_ERR, "ERROR: expectsend() short write (errno = %d!",
			   errno);
		    return(FAIL);
	       }
	  }
     }

     return(SUCCEED);
}

/***
 *  telnetProcess()
 *
 *  execute the (old) cms telnet login process flow
 *
 *  returns: 0 on success, -1 on failure
 */
static int telnetProcess(int sd, char *passwd, char *usercall, char *gwcall, char *freq, char *mode)
{
     char sbuf[1024];
     int rc;

     syslog(LOG_DEBUG, "running telnetProcess...");

     /*
      * do cms telnet login "chat" via socket to CMS
      */
     snprintf(sbuf, sizeof(sbuf), ".%s\r", usercall);
     if ((rc = expectsend(sd, "Callsign :", sbuf, 15)) != 0) {
	  /*
	   * doesn't matter if it was an error or timeout, we will fail
	   * this login and let the gateway move on
	   */
	  return(FAIL);
     }

     snprintf(sbuf, sizeof(sbuf), "%s %s %s %s\r",
	     passwd, gwcall, freq, mode);
     if ((rc = expectsend(sd, "Password :", sbuf, 20)) != 0) {
	  /*
	   * just fail it!
	   */
	  return(FAIL);
     }

     return(SUCCEED);
}


/***
 *  sglProcess()
 *
 *  execute the (new) secure gateway login process flow
 *
 *  returns: 0 on success, -1 on failure
 */
#define LOOP_LIMIT 3 /* # times to try to get login prompt */
static int sglProcess(int sd, char *passwd, char *usercall, char *gwcall)
{
     char sbuf[1024];
     char rbuf[1024];
     int rc;
     int n_read;
     SGLSTATE sgl_state = SGL_CALLSIGN; /* initial state for SGL login DFA */
     bool running = true;
     char challenge[16]; /* holds the challenge code from the CMS */
     char *response; /* will point to the response code to be sent to CMS */
     int loop_cnt = 0;

     /*
      * do secure gateway login  "chat" via socket to CMS
      *
      * precondition: we've already have a connected socket to the CMS, but
      * have not done any actual communication with it.
      *
      * read lines (teminated by a CR) and "run" the state machine
      *
      * we eventually what to fully detect and act on the login processing
      * in the state machine here, rather than wait for a failure to
      * be figured out higher up in the gateway session setup
      * e.g.: "bad login response - FAIL" among other possible messages
      */
     syslog(LOG_DEBUG, "running sglProcess...");
     while (running) {
	  /*
	   * read a line from the CMS
	   */
	  n_read = readln(sd, rbuf, sizeof(rbuf), '\r');
	  syslog(LOG_DEBUG, "readln() read %d bytes", n_read);
	  if (n_read > 0) {
	       syslog(LOG_DEBUG, "sglProcess(): readln() returned [%s]",
		      rbuf);
	  }

	  /*
	   * process read data according to our current processing state
	   */
	  switch (sgl_state) {
	  case SGL_CALLSIGN: /* looking for the "login" prompt */
	       syslog(LOG_DEBUG, "sglProcess(): state = SGL_CALLSIGN");

	       if (n_read <= 0) { /* nothing read or error */
		    sgl_state = SGL_CALLSIGNFAIL;
		    running = false;
	       } else if (strcmp(rbuf, "\r") == 0) {
		    /*
		     * got a lone CR from the CMS.
		     * eat it and look again
		     */
		    syslog(LOG_DEBUG, "eat CR1"); /* do nothing */
	       } else if (strcasestr(rbuf, "Callsign :") != NULL) {
		    syslog(LOG_DEBUG, "sglProcess(): got login prompt: [%s]",
			 rbuf);
		    /* found login prompt, send callsigns */
		    snprintf(sbuf, sizeof(sbuf), "%s %s\r", usercall, gwcall);
		    
		    if ((rc = expectsend(sd, "", sbuf, 15)) != 0) {
			 /*
			  * doesn't matter if it was an error or timeout, we will fail
			  * this login and let the gateway move on
			  */
			 sgl_state = SGL_CALLSIGNFAIL;
			 running = false;
		    } else {
			 sgl_state = SGL_CHALLENGE;
		    }
	       } else {
		    syslog(LOG_DEBUG, "sglProcess(): login prompt not seen, got: [%s]",
			 rbuf);
		    if (loop_cnt++ > LOOP_LIMIT) {
			 sgl_state = SGL_CALLSIGNFAIL;
			 running = false;
		    } else {
			 /* send CR to attempt to elicit prompt from CMS */
			 sleep(2); /* wait 2 secs before sending CR */
			 if (expectsend(sd, "", "\r", 15) < 0) {
			      sgl_state = SGL_CALLSIGNFAIL;
			      running = false;
			 }
		    }
		    /* else: remain in the current state */
	       }
	       break;
	  case SGL_CHALLENGE: /* looking for login challenge */
	       syslog(LOG_DEBUG, "sglProcess(): state = SGL_CHALLENGE");

	       if (n_read <= 0) {
		    sgl_state = SGL_CHALLENGEFAIL;
		    running = false;
	       } else if (strcmp(rbuf, "\r") == 0) {
		    /*
		     * got a lone CR from the CMS.
		     * eat it and look again
		     */
		    syslog(LOG_DEBUG, "eat CR2"); /* do nothing */
	       } else if (strcasestr(rbuf, ";SQ: ") != NULL) {
		    /*
		     * got the challenge, parse out challenge code,
		     * compute and send response
		     */
		    memset(challenge, '\0', sizeof(challenge));
		    sscanf(rbuf, ";SQ: %8s\r", challenge);
		    syslog(LOG_DEBUG, "challenge code from CMS = [%s]", challenge);

		    /*
		     * compute the response
		     */

		    response = sgl_challenge_response(challenge, passwd);
		    syslog(LOG_DEBUG, "computed response code = [%s]", response);

		    /*
		     * format and send the response
		     */
		    sprintf(sbuf, ";SR: %s 25000001 20\r", response);
		    if (expectsend(sd, "", sbuf, 15) < 0) {
			 /*
			  * we'll consider the login complete at this point.
			  * if the login is not valid, the CMS will send
			  * and message a disconnected, which will be
			  * detected in the layer above
			  */
			 sgl_state = SGL_LOGINCOMPLETE;
			 running = false;
		    } else {
			 /*
			  * we sent the response -- for now, we'll end the
			  * state machine and consider the login flow complete
			  *
			  * what happens next depends on the CMS accepting our
			  * login response and that will be handed in the layers
			  * above... later we'll carry our more processing here
			  * to deal better with the possible failures more
			  * intelligently
			  */
			 sgl_state = SGL_RESPONSESENT;
			 running = false;
		    }
	       } else {
		    syslog(LOG_WARNING, "WARNING: SGL Challenge not received (saw: [%s]",
			 rbuf);
		    sgl_state = SGL_CHALLENGEFAIL;
		    running = false;
	       }
	       break;
	  default:
	       /* unknown state! uh oh! */
	       syslog(LOG_ERR, "ERROR: sglProcess hit IMPOSSIBLE STATE: %d", sgl_state);
	       sgl_state = SGL_BADSTATE;
	       running = false;
	       break;
	  }
     }

     /*
      * figure out the general status of the login attempt here
      * and return
      */
     switch (sgl_state) {
     case SGL_CALLSIGNFAIL:
     case SGL_CHALLENGEFAIL:
     case SGL_BADSTATE:
	  syslog(LOG_DEBUG, "sglProcess(): failed");
	  return(FAIL);
	  break;
     default:
	  break;
     }

     syslog(LOG_DEBUG, "sglProcess(): succeeded");

     return(SUCCEED);
}


int cmslogin(int sd, config *cfg, channel *c, char *usercall, char *cmspasswd)
{
     int rc;

     if (c == NULL) { /* can't login if no channel info */
	  return(FAIL);
     } else if (cfg == NULL) { /* or a configuration */
	  return(FAIL);
     }

     /*
      * Secure Gateway Login
      */
     syslog(LOG_INFO, "*** Secure Gateway Logon");
     rc = sglProcess(sd, c->ch_password, usercall, c->ch_callsign);

     return(rc); /* login successful */
}

