/*
 *			r e a d l n . c
 *
 * $Revision: 167 $
 * $Author: eckertb $
 *
 * readln: Copyright (c) 2007 Brian R. Eckert - KB3KLJ
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
static char svnid[] = "$Id";
#endif /* lint */

#include <stdio.h>
#include "rmslib.h"


/***
 *  readln()
 *
 *  read a line from a file descriptor. the line is read one byte at a time,
 *  looking for the eol character (typically 0x0a or 0x0d). The eol is stored
 *  in the buffer and a NUL (0x00) is added (the same as fgets()).
 *
 *  fd = descriptor to read from
 *  buf = buffer to read into
 *  maxlen = maximum length of buffer
 *  eol = end of line character
 *
 *  returns: number of bytes actually read (including the eol character),
 *	     < 0 on error
 */
int readln(register int fd, register char *buf, register int maxlen, unsigned char eol)
{
     register int	n;
     int		rc;
     char		c;

     for (n = 1; n < maxlen; n++) {
	  if ((rc = read(fd, &c, 1)) == 1) {
	       if ((*buf++ = c) == eol) {
		    break;
	       }
	  } else if (rc == 0) {
	       if (n == 1) {
		    return(0); /* EOF, no data read */
	       } else {
		    break; /* EOF, some data read */
	       }
	  } else {
	       return(-1); /* error */
	  }
     }

     *buf = '\0';
     return(n);		
}
