/*
 *			l o g _ l o g o u t . c
 * $Revision: 32 $
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
static char svnid[] = "$Id: log_logout.c 32 2008-05-01 11:06:00Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>

#include "rms.h"


/***
 *  log_logout()
 *
 *  log logouttime, usercall and statistics
 */
void log_logout(char *usercall, struct ust userstat,
		time_t tstart, time_t tstop)
{
    double tused;
    float rate;

    time(&tstop);
    tused = difftime(tstop, tstart);
    rate = (userstat.bytes_sent + userstat.bytes_recv) / tused;
    syslog(LOG_NOTICE,
           "Logout %-9.9s tx:%li rx:%li %.1fs %.1f Bytes/s (%d)\n",
           usercall, userstat.bytes_sent, userstat.bytes_recv,
           tused, rate, userstat.errcode);
}
