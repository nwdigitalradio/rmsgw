/*
 *			r u n _ p y t h o n _ s c r i p t . c
 * $Revision: 171 $
 * $Author: eckertb $
 *
 * RMS Gateway
 *
 * Copyright (c) 2014 Hans-J. Barthen - DL5DI
 * Copyright (c) 2014 Brian R. Eckert - W3SG
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
 *	execute a python script
 */
#ifndef lint
static char svnid[] = "$Id: run_python_script.c 171 2014-10-19 10:00:22Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <limits.h>
#include <syslog.h>

#include "rms.h"
#include "rmslib.h"
#include "aci.h"

/***
 *  run_python_script(script file)
 *
 *  run the supplied script with python
 *  returns: 0 on success, non-zero otherwise
 */
int run_python_script(char* python_path, char *script_file)
{
     char python_cmd[PATH_MAX];
     int rc, status;

     /*
      * build python command string -- gateway python scripts
      * use -d argument for debugging; syslog settings
      * in gateway.conf determine if debug log messages
      * actually get written (that is, the script looks
      * at gateway.conf for determining the logging
      * level it will use)
      */
     sprintf(python_cmd, "%s %s -d",
	     python_path == NULL || *python_path == '\0' ? DEFAULT_PYTHON_PATH : python_path,
	     script_file);
     syslog(LOG_DEBUG, "running python command: %s",
	    python_cmd);
     
     rc = system(python_cmd);

     status = WEXITSTATUS(rc);

     if (status == 0) {
	  syslog(LOG_DEBUG, "python script %s succeeded",
		 script_file);
	  return(0);
     } else if (status < 0) {
	  syslog(LOG_ERR, "ERROR: failed to run python script %s",
		 script_file);
	  return(1);
     } else if (status == 127) {
	  syslog(LOG_ERR, "ERROR: unable to execute python for script %s",
		 script_file);
	  return(1);
     } else {
	  syslog(LOG_ERR, "python script %s failed, status %d",
		 script_file, status);
	  return(1);
     }
}

