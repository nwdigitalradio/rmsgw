/*
 *			g l o b . c
 * $Revision: 106 $
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
static char svnid[] = "$Id: glob.c 106 2009-04-28 15:09:24Z eckertb $";
#endif /* lint */

#include "rms.h"
#include "rmsshm.h"

int paclen = DFLTPACLEN; /* default to a sane value */
const char configfile[] = RMSCONFIGFILE;

rms_status *rms_stat;
int shm_debug = 0;
