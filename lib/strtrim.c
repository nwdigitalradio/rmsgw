/*
 *			s t r t r i m . c
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
 */
#ifndef lint
static char svnid[] = "$Id: strtrim.c 56 2008-08-06 10:40:10Z eckertb $";
#endif

#include <stdio.h>
#include <stdlib.h>

#include "set.h"
#include "rmslib.h"


/***
 *  strtrim(destination, source, chars, trim)
 *
 *  trim prefixes/suffixes from a string
 *
 *  if trim < 0 then trim prefix
 *  if trim == 0 then trim prefix and suffix
 *  if trim > 0 then trim suffix
 *
 *  returns: pointer to destination string
 */
char *strtrim(char *d, char *s, char *trim_chars, int trim)
{
     char *p = d; /* save start of dest string pointer */
     set tset;

     mkset(&tset, trim_chars);

     /*
      * leading chars in set (skip them)
      */
     if (trim <= 0) {
	  while (issetmem(&tset, *s)) {
	       s++;
	  }
     }

     /*
      * trailing characters in set (mark start of run and terminate string
      * there)
      */
     if (trim >= 0) {
	  register c;
	  register char	*mark = d; /* remember where we are in dest buffer */

	  /*
	   * we will copy the source buffer looking for a char that
	   * is in the trim set
	   *
	   * if char is not in the set, move the mark; while the chars
	   * in source are in the set, the mark stays in place until we hit
	   * a non-set char or the end of the source buffer; in this way,
	   * we will trim a continuous run of chars in the set which end
	   * out the string, but trim chars which are within the string are
	   * preserved
	   */
	  while (c = *s++) {
	       *d++ = c; /* copy current char to dest */
	       if (!issetmem(&tset, c)) { /* char in set? */
		    mark = d; /* new save point */
	       }
	  }

	  /*
	   * move the dest back to mark where the continuous run or trim
	   * characters begins (note: we may actually be at the end of the
	   * string here if there was not a trailing run of trim chars), and
	   * terminate the string there
	   */
	  d = mark;
	  *d = '\0';
     } else { /* just copy chars from source to destination */
	  while (*d++ = *s++) {
	       ;
	  }
     }

     return(p);
}
