/*
 *			r u n h o o k . c
 * $Revision: 176 $
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
 *	run a hook script
 */
#ifdef HOOKS
#ifndef lint
static char svnid[] = "$Id: runhook.c 176 2014-10-27 09:07:54Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <limits.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include "rms.h"
#include "rmslib.h"

/***
 *  runhook(hookdir, hookname, ..., NULL)
 *
 *  run indicated hook
 *
 *  NOTE: all variable args must be strings, and the list
 *	  of args must end with NULL
 *
 *  returns: 0 on success, non-zero otherwise
 */
int runhook(char* hookdir, char *hook_name, ...)
{
     char hook_path[PATH_MAX];
     int rc, status, child_status;
     char *args[MAXHOOKARGS];
     int argcnt = 0;
     int nullfd;
     pid_t pid;
     va_list ap;

     /*
      * some sanity checking
      */
     if (hook_name == NULL || strlen(hook_name) <= 0) {
	  syslog(LOG_ERR, "runhook: hook_name is NULL or empty!");
	  return (1);
     } else if (hookdir == NULL || strlen(hookdir) <= 0) {
	  syslog(LOG_ERR, "runhook: %s: hookdir is NULL or empty", hook_name);
	  return(1);
     }

     if (!file_exists(hookdir)) {
	  syslog(LOG_WARNING, "runhook: hookdir '%s' does not exist", hookdir);
	  return(1);
     }

     /*
      * build command string
      */
     sprintf(hook_path, "%s/%s", hookdir, hook_name);
     syslog(LOG_DEBUG, "running hook: %s", hook_path);

     /*
      * if the hook script doesn't exist, we're done here!
      */
     if (!file_exists(hook_path)) {
	  /*
	   * this is not necessarily an error (for the time being)
	   * the presence of hook scripts is optional, thus if it's not
	   * there, we don't really care -- on the otherhand, if the the
	   * sysop is doing something special, then this matters a whole
	   * lot, so we will at least log this fact if we are debugging
	   */
	  syslog(LOG_DEBUG, "runhook: skipping non-existant hook '%s'", hook_path);
	  return(0);
     }
	  
     /*
      * gather up the arguments for the exec call
      */
     va_start(ap, hook_name);
     args[argcnt++] = "-c"; /* we're executing a shell script... */
     args[argcnt++] = hook_path; /* ... and this is the script */
     while ((args[argcnt++] = va_arg(ap, char *)) != NULL) {
	  ;
     }
     va_end(ap);

     /*
      * fork and exec the hook script
      */
     switch (pid = fork()) {
     case 0: /* child */
	  signal(SIGINT, SIG_IGN);
	  signal(SIGQUIT, SIG_IGN);

	  /*
	   * redirect stdin, stdout, and stderr
	   * to /dev/null
	   */
	  if ((nullfd = open("/dev/null", O_RDONLY)) >= 0) {
	       close(0);
	       if (dup(nullfd) < 0) {
		    syslog(LOG_ERR, "runhook: dup() for stdin failed--errno %d (%s)",
			   errno, strerror(errno));
	       }
	       close(nullfd);
	  } else {
	       syslog(LOG_ERR, "runhook: open(/dev/null, O_RDONLY) failed--errno %d (%s)",
		      errno, strerror(errno));
	  }

	  if ((nullfd = open("/dev/null", O_WRONLY)) >= 0) {
	       close(1);
	       close(2);
	       if (dup(nullfd) < 0) {
		    syslog(LOG_ERR, "runhook: dup() for stdout failed--errno %d (%s)",
			   errno, strerror(errno));
	       }
	       if (dup(nullfd) < 0) {
		    syslog(LOG_ERR, "runhook: dup() for stderr failed--errno %d (%s)",
			   errno, strerror(errno));
	       }
	       close(nullfd);
	  } else {
	       syslog(LOG_ERR, "runhook: open(/dev/null, O_WRONLY) failed--errno %d (%s)",
		      errno, strerror(errno));
	  }

	  /*
	   * try to exec the hook script
	   */
	  execv(HOOKSHELL, args);

	  /*
	   * complete failure
	   */
	  syslog(LOG_ERR, "runhook: can't run %s (errno = %d -- %s)",
		 hook_path, errno, strerror(errno));
	  exit(1); /* deny login */
     case -1:
	  syslog(LOG_ERR, "runhook: fork failed (errno = %d -- %s)",
		 errno, strerror(errno));
	  return(1);
     default: /* parent */
	  /*
	   * wait for child to terminate
	   */
	  while (pid != wait(&status)) {
	       ;
	  }
     }

     child_status = WEXITSTATUS(status);

     if (child_status != 0) {
	  if (child_status < 0) {
	       syslog(LOG_ERR, "ERROR: failed to run hook %s",
		      hook_name);
	  } else if (child_status == 127) {
	       syslog(LOG_ERR, "ERROR: unable to execute hook %s",
		      hook_name);
	  }

	  syslog(LOG_ERR, "hook script %s failed: status %d, child_status %d",
		 hook_path, status, child_status);
	  return(1);
     }

     syslog(LOG_DEBUG, "hook %s succeeded", hook_name);
     return(0);
}
#else
/***
 *  runhook() for when hooks are not enabled
 */
int runhook(char* hookdir, char *hook_name, ...)
{
     return(0);
}
#endif /* HOOKS*/
