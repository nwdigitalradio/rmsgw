/*
 *			s e n d c h a n n e l . c
 * $Revision: 171 $
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
static char svnid[] = "$Id: sendchannel.c 171 2014-10-19 10:00:22Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>

#include "rms.h"
#include "rmslib.h"

#include "aci.h"


/***
 *  sendChannel(config, channel)
 *
 *  send the channel info to the winlink system for status update
 *  the winlink system
 *
 *  returns: 0 if channel update successfully sent, -1 otherwise
 */
int sendChannel(config *cfg, channel *chnl)
{
     if (run_python_script(cfg->python, CHANNELUPDATER) != 0) {
	  /* lower function logs errors, nothing more to do here */
	  return(-1);
     }

     return(0);
}
