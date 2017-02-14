/*
 *			s h m . c
 * $Revision: 118 $
 * $Author: eckertb $
 *
 * RMS Gateway
 *
 * Copyright (c) 2009 Hans-J. Barthen - DL5DI
 * Copyright (c) 2009 Brian R. Eckert - W3SG
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
 *	RMS Gateway shared memory support functions
 */
#ifndef lint
static char svnid[] = "$Id: shm.c 118 2010-08-31 11:13:15Z eckertb $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <memory.h>

#include <sys/types.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include "rms.h"
#include "rmsshm.h"


/***
 *  shm_getkey(pathname, proj_id)
 *
 *  calculate a (hopefully system unique) shared memory key
 *
 *  pathname = pathname of a file to be used as part of the
 *		key calculation
 *  proj_id = integer id to be used in the key calculation
 *
 *  returns: key on success, -1 on error
 */
key_t shm_getkey(char *pathname, int proj_id)
{
     /*
      * calculate a key value for a shared memory segment
      */
     return(ftok(pathname, proj_id));
}


/***
 *  shm_get()
 *
 *  establish a shared memory segment the gateway
 *
 *  pathname = pathname of file used for key calculation
 *  proj_id = initeger id used for key calculation
 *  segment_size = the size of the segment (if it needs to be created)
 *  doinit = feedback flag to notify caller if the segment will require
 *	initialization (i.e., it was newly created by this call)
 *
 *  returns: shared memory id on success, -1 on error
 */
int shm_get(char *pathname, int proj_id, int segment_size, int *doinit)
{
     int shmid;
     key_t shmkey;

     /*
      * initialize the doinit flag for no initialization--will be set later
      * if it's determined initialization is required
      *
      * the point of the initialization flag is that different async processes
      * could be first in needing the shared memory segment, but only the
      * the first process should initialize the segment obtained, so we will
      * use the 'doinit' parameter to provide feedback to the calling process
      * to indicate if the initialization is required after the segment has
      * been obtained and attached
      */
     *doinit = 0;

     /*
      * attempt to get a unique shared memory key
      */
     if ((shmkey = shm_getkey(pathname, proj_id)) < 0) {
	  syslog(LOG_ERR,"ERROR: shared memory key generation failed (%s)",
		 strerror(errno));
	  return(-1);
     }
     syslog(LOG_DEBUG, "shm_init(): shmkey = %d (0x%x)", shmkey, shmkey);

     /*
      * create/get a shared memory segment
      */
     if ((shmid = shmget(shmkey, segment_size,
			 RMS_SHM_PERMS)) < 0) {
	  if (errno == ENOENT) { /* doesn't exist, create it */
	       if ((shmid = shmget(shmkey, segment_size,
				   (IPC_CREAT|RMS_SHM_PERMS))) < 0) {
		    syslog(LOG_ERR,
			   "ERROR: failed to CREATE shared memory segment (%s)",
			   strerror(errno));
		    return(-1);
	       } else { /* must initialize if the segment was just created */
			 *doinit = 1;
	       }
	  } else { /* couldn't get the id for some reason other than it
		      doesn't exist */
	       syslog(LOG_ERR,
		      "ERROR: failed to GET shared memmory segment (%s)",
		      strerror(errno));
	       return(-1);
	  }
     }

     syslog(LOG_DEBUG, "shm_init(): shmid = %d (0x%x)\n", shmid, shmid);

     return(shmid);
}


/***
 *  shm_attach(shmid)
 *
 *  attach the shared memory segment associated with shmid
 *
 *  shmid = the id of the shared memory segment to be attached
 *
 *  returns: pointer to attached segment on success, (void *) -1 on error
 */
void *shm_attach(int shmid)
{
     void *segment;

     /*
      * attach the shared memory segment
      */
     if ((segment = shmat(shmid, NULL, 0)) == (void *) -1) {
	  syslog(LOG_ERR,
		 "ERROR: failed to attach shared memory segment %d (%s)",
		 shmid,
		 strerror(errno));
	  return((void *) -1);
     }

     return(segment);
}


/***
 *  shm_detach(addr)
 *
 *  detach the shared memory segment at addr
 *
 *  returns: 0 on success, -1 on error
 */
int shm_detach(const void *addr)
{
     if (shmdt(addr) < 0) {
	  syslog(LOG_WARNING,
		 "WARNING: failed to detach shared memory segment at %p",
		 addr);
	  return(-1);
     }

     return(0);
}
