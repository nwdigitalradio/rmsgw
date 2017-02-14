/*
 *			r m s g w . h
 * $Revision: 167 $
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

#ifndef _rmsgw_h
#define _rmsgw_h	1

#ifndef lint
static char	_rmsgw_h_svnid[] = "$Id: rmsgw.h 167 2014-09-30 10:27:26Z eckertb $";
#endif /* lint */

#include <stdio.h>
#include <sys/types.h>

/*
 * cmsSession error returns
 */
#define ERR_SESS_CONNECT	-1
#define ERR_SESS_GWSKIP		-2
#define ERR_SESS_FATAL		-999

extern config *gwinit(const char *configfile, char *gwcall, char *logmask,
		      char *bannerfile);
extern void greet(int fd, config *cfg, version_blk *ver);
extern int banner(int fd, char *bfile);
extern int cmsSession(cmsnode *the_cms, config *cfg,
		      char *ax25port, char *usercall);
extern struct ust gateway(int s, config *cfg, char *usercall, char *passwd, channel *chnl);
extern void log_logon(char *usercall, char *port, char *hostname);
extern void log_logout(char *usercall, struct ust userstat,
		       time_t tstart, time_t tstop);

#endif /* _rmsgw_h */
