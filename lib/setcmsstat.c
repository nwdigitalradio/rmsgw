/*
 *			s e t c m s t a t . c
 * $Revision: 131 $
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
 *	set/update CMS host status file
 */
#ifndef lint
static char svnid[] = "$Id: setcmsstat.c 131 2011-11-28 19:31:57Z eckertb $";
#endif /* lint */

#include <limits.h>
#include <fcntl.h>
#include <utime.h>

#include <sys/types.h>

#include "rms.h"
#include "rmslib.h"


/***
 *  setcmsstat(cmsnode)
 *
 *  set/update the status of the CMS host pointed to by cmsnode
 *
 *  returns: 0 on success, -1 on error
 */
int setcmsstat(cmsnode *p)
{
     char statfile[PATH_MAX];
     int fd;

     snprintf(statfile, sizeof(statfile), "%s/%s", CMSSTATDIR, p->cms_host);

     return(setstatfile(statfile));
}
