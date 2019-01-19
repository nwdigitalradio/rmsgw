/*
 *			g e t c m s l i s t . c
 * $Revision: 172 $
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
 *	create a linked list CMS host entries
 */
#ifndef lint
static char svnid[] = "$Id: getcmslist.c 172 2014-10-19 10:11:04Z eckertb $";
#endif /* lint */

#include <string.h>
#include <limits.h>
#include <syslog.h>
#include <errno.h>

#include <sys/stat.h>

#include "rms.h"
#include "rmslib.h"


/***
 *  getcmslist()
 *
 *  create a linked list of CMS hosts
 *
 *  returns: pointer to head node of linked list (NULL if list is empty)
 */
cmsnode *getcmslist(void)
{
     int hostcnt = 0;
     cms *c;
     cmsnode *cmslist = NULL;

     /*
      * create an ordered list (LRU) of the CMS hosts to be tried by the
      * gateway
      */
     while ((c = getcmsent()) != NULL) {
	  char statfile[PATH_MAX];
	  struct stat st;
	  

	  hostcnt++;

	  syslog(LOG_DEBUG, "getcmslist(): add host = %s, port = %d, passwd = %s",
		 c->cms_host, c->cms_port, c->cms_passwd);

	  if (strlen(c->cms_host) < 1) {
	       syslog(LOG_WARNING, "missing cms host at line %d. Skipped.",
		      hostcnt);
	       continue;
	  }
	       
	  /*
	   * Do not accept any port numbers in the IANA official well-known
	   * services range, since CMS hosts should not be listening
	   * on ports in the 1-1023 range. Also don't allow ports that
	   * are above the last possible value.
	   */
	  if (c->cms_port < 1024 || c->cms_port > 65535) {
	       syslog(LOG_WARNING, "bad port (%d) for host '%s' at line %d. Skipped.",
		      c->cms_port, c->cms_host, hostcnt);
	       continue;
	  }
	       
	  if (strlen(c->cms_passwd) < 1) {
	       syslog(LOG_WARNING, "missing password for host '%s' at line %d. Skipped. ",
		      c->cms_host, hostcnt);
	       continue;
	  }

	  /*
	   * check status file for host
	   */
	  snprintf(statfile, sizeof(statfile), "%s/%s", CMSSTATDIR, c->cms_host);
	  if (stat(statfile, &st) < 0) {
	       /* can't stat file, check why */
	       switch (errno) {
	       case ENOENT: /* file doesn't exist */
		    /*
		     * proceed
		     */
		    st.st_atime = (time_t) 0;
		    break;
	       default:
		    syslog(LOG_WARNING, "Can't stat %s (errno = %d)",
			   statfile, errno);
		    continue; /* skip to next entry */
	       }
	  }

	  /*
	   * add host to linked list
	   */
	  cmslist = addcmsnode(cmslist, c, st.st_atime);
     }

     endcmsent();
     return(cmslist);
}
