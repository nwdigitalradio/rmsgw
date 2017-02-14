/*
 *			l o a d e n v . c
 * $Revision: 176 $
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
static char	svnid[] = "$Id: loadenv.c 176 2014-10-27 09:07:54Z eckertb $";
#endif

#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#include "rms.h"
#include "rmslib.h"


/***
 *  loadenv(env file name)
 *
 *  read environment variable file and add them to the current
 *  program environment
 *
 *  returns 0 on success, -1 on failure
 */
int loadenv(const char *envfile)
{
     int fail_count = 0;
     char buf[MAXBUF];
     register char *bp, *lp;
     FILE *efp;
     char toks[2][MAXBUF]; /* we are only working with two
			      tokens, so disregard the "magic
			      number" here :-) */
     char *toksep = "=";
     register char *tok;
     int t_cnt;

     syslog(LOG_DEBUG, "using environment file %s", envfile);

     if ((efp = fopen(envfile, "r")) == NULL) {
	  syslog(LOG_ERR, "can't open environment file '%s' - errno = %d (%s)",
		 envfile, errno, strerror(errno));
	  fail_count++;
     } else {
	  while ((lp = fgetline(buf, sizeof(buf), efp)) != NULL) {
	       /*
		* eat comments
		*/
	       if ((bp = strchr(buf, '#')) != NULL) {
		    *bp = '\0';
	       }

	       /*
		* peel off token values and place in token array
		* NOTICE: NOT THREAD SAFE!
		*/
	       for (t_cnt = 0, tok = strtok(buf, toksep);
		    t_cnt < 3 && tok != NULL;
		    t_cnt++, tok = strtok(NULL, toksep)) {
		    strcpy(toks[t_cnt], tok);
	       }

	       /*
		* if we don't have exactly two tokens, then the line
		* is not properly formed to set an environment variable,
		* so swallow it and move on
		*/
	       if (t_cnt == 2) {
		    syslog(LOG_DEBUG, "setenv %s=%s", toks[0], toks[1]);
		    if (setenv(toks[0], toks[1], 1) != 0) {
			 syslog(LOG_ERR, "setenv for %s failed - errno = %d (%s)",
				toks[0], errno, strerror(errno));
			 fail_count++;
			 /* continue on anyway */
		    }
	       }
	  }
	  fclose(efp);
     }

     return(fail_count > 0 ? FAIL : SUCCEED);
}

