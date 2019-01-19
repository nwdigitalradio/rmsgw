/*
 *			f i l e _ e x i s t s . c
 * $Revision: 176 $
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
static char	svnid[] = "$Id: file_exists.c 176 2014-10-27 09:07:54Z eckertb $";
#endif

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "rmslib.h"


/***
 *  file_exists(filename)
 *
 *  check if a file exists
 *
 *  return true if the file exists, false otherwise
 */
bool file_exists(const char *filename)
{
     struct stat st;

     return((filename != NULL && strlen(filename) > 0 && stat(filename, &st) == 0) ? true : false);
}
