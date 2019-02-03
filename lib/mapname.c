/*
 *			m a p n a m e . c
 * $Revision: 150 $
 * $Author: eckertb $
 *
 * This module originally:
 * Copyright (c) 1989,1990,1991,1992,2008 by Brian R. Eckert
 *
 * Now incorporated into...
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
static char svnid[] = "$Id: mapname.c 150 2013-07-04 01:05:41Z eckertb $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rmslib.h"
#include "mapname.h"


int map_errno;
int nmap_err = NMAPERRS;
char *map_logical = NULL;

static char *map_err_msgs[] = {
     "no map error",
     "no MAPDB in environment",
     "cannot read map file",
     "storage allocator failure during mapping",
     "logical not found in mapfile",
     "illegal mapctl command",
     NULL
};

/*
 * map control option structure
 */
static struct {
     int FailOnNotFound;
} Control = {
     0
};


/***
 *  maperrmsg(map error number)
 *
 *  return pointer to appropriate error
 *  message for indicated map error number
 */
char *maperrmsg(int err)
{
     return(err > nmap_err ?
	    "unknown map error code" : map_err_msgs[err]);
}


/***
 *  mapctl()
 *
 *  control various aspects of the mapname functions
 *
 *  cmd = command
 *  arg = point to any requried argument
 */
int mapctl(int cmd)
{
     switch (cmd) {
     case MCTL_SETNOTFNDERR: /* set not found error disposition */
	  Control.FailOnNotFound = 1;
	  break;
     case MCTL_CLRNOTFNDERR: /* clr not found error disposition */
	  Control.FailOnNotFound = 0;
	  break;
     default:
	  map_errno = ME_ILLCOMMAND;
	  return(-1);
     }

     return(0);
}


/***
 *  fmapname(mapfile, logical var list, physical var list)
 *
 *  map the list of logical names and set appriopriate physical name space
 *  pointers to the mapped name from the indicated map file
 *
 *  mapfile = pointer to name of map file to use
 *  logical = array of pointers to logical names
 *  physical = array of pointers to pointer to physical names
 *
 *  returns: 0 on success, -1 on error with map_errno set
 */
int fmapname(const char *mapfile, char **logical, char ***physical)
{
     FILE *mfp;
     char **lp,		/* pointer to logical name */
	  ***pp,		/* pointer to physical name */
	  namebuf[256],	/* buffer to hold qualified logical name */
	  entrybuf[2048],	/* buffer to hold a complete map entry */
	  *ebp;

     /*
      * open the map file
      */
     if ((mfp = fopen(mapfile, "r")) == NULL) {
	  map_errno = ME_MAPOPEN;
	  return(-1);
     }

     /*
      * loop through logicals, picking up names from the file and
      * duping names in the physicals
      */
     for (pp = physical, map_logical = *(lp = logical);
	  *lp != NULL; pp++, map_logical = *++lp) {
	  rewind(mfp);

	  snprintf(namebuf, sizeof(namebuf), "%s=", *lp);

	  /*
	   * search for logical name match
	   */
	  while ((ebp = fgetline(entrybuf, sizeof(entrybuf), mfp)) != NULL) {
	       strtrim(entrybuf, entrybuf, " \t", 1);
	       if (!strncmp(namebuf, entrybuf, strlen(namebuf)))
		    break;
	  }

	  if (ebp != NULL) {
	       if ((**pp = strdup(ebp + strlen(namebuf))) == NULL) {
		    map_errno = ME_NOMEM;
		    fclose(mfp);
		    return(-1);
	       }
	  } else {
	       **pp = NULL;
	       if (Control.FailOnNotFound) {
		    map_errno = ME_NOTFOUND;
		    fclose(mfp);
		    return(-1);
	       }
	  }
     }

     fclose(mfp);

     return(0);
}


/***
 *  mapname(logical names, physical name space pointers)
 *
 *  map the list of logical names and set appriopriate physical name space
 *  pointers to the mapped name (this is the original interface)
 *
 *  returns: 0 on success, -1 on failure
 */
int mapname(char **logical, char ***physical)
{
     char	*mapfile;

     /*
      * get map data base name
      */
     if ((mapfile = getenv("MAPDB")) == NULL) {
	  map_errno = ME_NOMAPDB;
	  return(-1);
     }

     return(fmapname(mapfile, logical, physical));
}
