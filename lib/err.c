/*
 *			e r r . c
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
static char svnid[] = "$Id: err.c 167 2014-09-30 10:27:26Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include "rmslib.h"


/***
 *  err(message)
 *
 *  send and log an error message
 *
 *  NOTE: the message will have an eol as set above sent to
 *  end the message
 */
void err(char *message)
{

     sendrf(STDOUT_FILENO, message, strlen(message), SRF_EOLXLATE|SRF_ADDEOL);

     syslog(LOG_ERR, "Logout %s ERROR!", message);

     closelog();
     exit(1);
}

