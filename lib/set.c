/*
 *			s e t . c
 * $Revision: 56 $
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
 *
 * Description:
 *
 * Group of functions ala getpwent() and friends which will read
 * channel information from a structured file into a local structure.
 *
 */
#ifndef lint
static char svnid[] = "$Id: set.c 56 2008-08-06 10:40:10Z eckertb $";
#endif

/*
 * simple set operations for ASCII character values
 */

#include <stdio.h>
#include <stdlib.h>

#include "set.h"


/***
 *  clearset(set)
 *
 *  clear a set
 *
 *  returns: 1
 */
int clearset(set *the_set)
{
     register	i;

     for (i = 0; i < 256; i++) {
	  the_set->set_arr[i] = 0;
     }

     return(1);
}


/***
 *  mkset(set, string)
 *
 *  convert string to a set
 *
 *  returns: 1
 */
int mkset(set *the_set, register char *s)
{
     clearset(the_set);

     while (*s) {
	  the_set->set_arr[*s++] = 1;
     }

     return(1);
}


/***
 *  addset(set, char)
 *
 *  add a character to a set
 *
 *  returns: 1
 */
int addset(set *the_set, unsigned char c)
{
     the_set->set_arr[c & 0xff] = 1;

     return(1);
}


/***
 *  issetmem(set, char)
 *
 *  check if character is a member of the set
 *
 *  returns: 1 if char in set, 0 if not
 */
int issetmem(set *the_set, unsigned char c)
{
     return(the_set->set_arr[c & 0xff]);
}
