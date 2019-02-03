/*
 *			d o w n c a s e . c
 * $Revision: 51 $
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
static char	svnid[] = "$Id: downcase.c 51 2008-07-10 03:17:06Z eckertb $";
#endif

#include <stdio.h>
#include <ctype.h>

#include "rmslib.h"


/***
 *  downcase(string)
 *
 *  convert chars of a string to lower case
 *
 *  returns: pointer to converted string
 */
char *downcase(char *s)
{
     register char *p = s;

     if (p != NULL) {
	  while(*p) {
	       *p++ = tolower(*p);
	  }
     }

     return(s);
}
