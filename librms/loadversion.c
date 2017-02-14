/*
 *			l o a d c o n f i g . c
 * $Revision: 167 $
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
static char	svnid[] = "$Id: loadversion.c 167 2014-09-30 10:27:26Z eckertb $";
#endif

#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "rmslib.h"
#include "mapname.h"

static version_blk version;

/*
 * logical names of version info variables
 */
static char *logicals[] = {
     "PACKAGE",
     "PROGRAM",
     "LABEL",
     "REVISION",
     "DATE",
     "AUTHOR",
     "ID",
     NULL
};

/*
 * physical varibables for pointing to values of the logicals
 * NOTE: MAKE SURE THEY MATCH UP WITH THE LOGICALS ABOVE
 */
static char **physicals[] = {
     &version.package,
     &version.program,
     &version.label,
     &version.revision,
     &version.date,
     &version.author,
     &version.id
};
     

/***
 *  loadConfig(config file name, pointer to int return code)
 *
 *  read the version info and store
 *  values in the version block
 */
version_blk *loadVersion(const char *verfile, int *rc)
{
     *rc = 0;

     syslog(LOG_DEBUG, "using version file %s", verfile);

     /*
      * initialize the  parameters
      */
     version.package = version.program = version.label = NULL;
     version.revision = version.date = version.author = NULL;
     version.id = NULL;

     /*
      * no error on logical not found
      */
     if (mapctl(MCTL_CLRNOTFNDERR) < 0) {
	  *rc = ERR_VER_MAPERROR;
	  goto bailout;
     }

     /*
      * map the configuration variables
      */
     if (fmapname(verfile, logicals, physicals) < 0) {
	  *rc = ERR_VER_MAPERROR;
	  goto bailout;
     }

     /*
      * check that we have a valid version block, return appropriate
      * errors if not
      */
     if (version.package == NULL) {
	  *rc = ERR_VER_MISSINGPACK;
     } else if (version.program == NULL) {
	  *rc = ERR_VER_MISSINGPROG;
     } else if (version.label == NULL) {
	  *rc = ERR_VER_MISSINGLAB;
     }

     /*
      * Other values not being set are okay
      */

     /*
      * if we caught an error, return NULL, otherwise return
      * a pointer to our internal version structure
      */
 bailout:
     if (*rc != 0) {
	  return(NULL);
     }

     return(&version); /* successfully loaded version */
}

