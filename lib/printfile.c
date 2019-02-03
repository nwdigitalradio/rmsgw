/*
 *			p r i n t f i l e . c
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
static char svnid[] = "$Id: printfile.c 51 2008-07-10 03:17:06Z eckertb $";
#endif

#include <stdio.h>
#include <string.h>

#include "rmslib.h"


/***
 *  printfile(file)
 *
 *  prints the contents file file to stdout
 *
 *  returns: 0 on success, -1 on error
 */
int printfile(char *file)
{
     FILE *fp;
     int c;

     /*
      * if we don't have a filename, just give up quietly
      */
     if (file == NULL || strlen(file) <= 0) {
	  return(-1);
     }

     if ((fp = fopen(file, "r")) == NULL) {
	  return(-1); /* quietly return if can't open file  (errno is set) */
     }

     /*
      * get chars from file and put on stdout (note, these operations are
      * buffered, so there is no significant effeciency loss here)
      */
     while((c = getc(fp)) != EOF) {
	  putchar(c);
     }

     fclose(fp);
     return(0);
}
