/*
 *			s t a t b l o c k . c
 * $Revision: 109 $
 * $Author: eckertb $
 *
 * RMS Gateway
 *
 * Copyright (c) 2004-2009 Hans-J. Barthen - DL5DI
 * Copyright (c) 2008-2009 Brian R. Eckert - W3SG
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
static char svnid[] = "$Id: statblock.c 109 2009-08-10 08:31:14Z eckertb $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "rms.h"
#include "rmslib.h"
#include "rmsshm.h"

/*
 * TODO: will probably change this mechanism later so that there is a library
 * call to toggle on/off
 */
extern int shm_debug;

static char *aci_state_map[] = {
     "Unused",
     "Running",
     "Idle",
     "Connecting",
     "Connected",
     "Add Channel",
     "Update Channel",
     "Update Version",
     "Disconnecting",
     "Disconnected",
     NULL
};

static char *gw_state_map[] = {
     "Unused",
     "Running",
     "Idle",
     "Connecting",
     "Authorizing",
     "Connected",
     "Login",
     "Logged In",
     "Comm Wait",
     "Sending",
     "Receiving",
     "Disconnecting",
     "Disconnected",
     NULL
};

static char *gw_comm_map[] = {
     "  ",
     "~~",
     "..",
     "==",
     "++",
     "--",
     "^^",
     "--",
     "><",
     "->",
     "<-",
     "vv",
     "##",
     NULL
};


/***
 *  set_shm_debug()
 *
 *  determine if shared memory debugging should be turned on
 *
 *  returns: 1 for debugging on, 0 for off
 */
int set_shm_debug(void)
{
     struct stat stbuf;

     if (stat("/etc/rmsgw/shm_debug", &stbuf) == 0) {
	  return(1); /* shared memory debugging on */
     }

     return(0); /* shared memory debugging off */
}


/***
 *  map_aci_state(state)
 *
 *  map an aci state to a descriptive text message
 *
 *  returns: pointer to text version of state
 */
char *map_aci_state(ACISTATE s) {
     /* FIXME: this is simplistic and unchecked */
     return(aci_state_map[s]);
}


/***
 *  map_gw_state(state)
 *
 *  map a gateway state to a descriptive text message
 *
 *  returns: pointer to text version of state
 */
char *map_gw_state(GWSTATE s) {
     /* FIXME: this is simplistic and unchecked */
     return(gw_state_map[s]);
}


/***
 *  map_gw_comm(state)
 *
 *  map a gateway state to a comm symbols
 *
 *  returns: pointer to symbol string for state
 */
char *map_gw_comm(GWSTATE s) {
     /* FIXME: this is simplistic and unchecked */
     return(gw_comm_map[s]);
}


/***
 *  statblock_attach(pointer to addr)
 *
 *  attach the status shared memory segment to the process and
 *  initialize it if necessary
 *
 *  returns: 0 and the attached segment address in addr on success
 *           -1 on failure
 */
int statblock_attach(rms_status **addr)
{
     int shm_must_init; /* shared memory initialization flag */
     int shmid;

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: attach status block shm");
     }

     /*
      * establish shared memory segment for the gateway
      */
     if ((shmid =
	  shm_get("/etc/rmsgw", 0, sizeof(rms_status), &shm_must_init)) < 0) {
	  return(-1); /* shm_get() logs details of failure */
     }
	  
     /*
      * attach the shared memory segment
      */
     if ((*addr = shm_attach(shmid)) == (void *) -1) {
	  return(-1); /* shm_attach() logs details of failure */
     }

     /*
      * check feedback variable for possible initialization
      */
     if (shm_must_init) {
	  if (shm_debug) {
	       syslog(LOG_DEBUG, "SHMDEBUG: initializing shm");
	  }

	  /*
	   * TODO: implement monitor entry here!
	   */
	  /*
	   * check if the magic string is present at the front of the segment
	   */
	  if (strncmp((*addr)->hdr.rms_hdr_magic,
		      RMS_SHM_MAGIC, RMS_SHM_MAGICSIZE) != 0) {
	       int i;

	       /*
		* no magic string, go ahead and initialize
		*/
	       memset(*addr, '\0', sizeof(rms_status));

	       /*
		* intialize header
		*/
	       strncpy((*addr)->hdr.rms_hdr_magic,
		       RMS_SHM_MAGIC, RMS_SHM_MAGICSIZE);

	       /*
		* initialize aci table
		*/
	       (*addr)->aci.aci_pid = (pid_t) -1;
	       (*addr)->aci.aci_state = ACI_UNUSED;
	       (*addr)->aci.aci_hostidx = -1;
	       for (i = 0; i < RMS_CMS_TABLE_SIZE; i++) {
		    (*addr)->aci.aci_host[i].aci_host_last_ci = (time_t) 0;
		    (*addr)->aci.aci_host[i].aci_host_last_failure = (time_t) 0;
		    (*addr)->aci.aci_host[i].aci_host_tot_fail_count = 0L;
		    (*addr)->aci.aci_host[i].aci_host_cur_fail_count = 0L;
		    (*addr)->aci.aci_host[i].aci_host_cur_channel_updates = 0;
		    (*addr)->aci.aci_host[i].aci_host_cur_channel_errors = 0;
	       }

	       /*
		* intialize gateway table
		*/
	       for (i = 0; i < RMS_GW_TABLE_SIZE; i++) {
		    (*addr)->rmsgw[i].gw_pid = (pid_t) -1;
		    (*addr)->rmsgw[i].gw_state = GW_UNUSED;
		    (*addr)->rmsgw[i].gw_connects = 0L;
		    (*addr)->rmsgw[i].gw_bytes_in = (long long) 0;
		    (*addr)->rmsgw[i].gw_bytes_out = (long long) 0;
	       }
	  }

	  /*
	   * TODO: implement monitor exit here!
	   */
     }

     return(0);
}


/***
 *  statblock_detach(pointer to status block)
 *
 *  detach the status shared memory segment at addr
 *
 *  returns: 0 on success, -1 on error
 */
int statblock_detach(rms_status *addr)
{
     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: detach shm");
     }

     return(shm_detach(addr));
}


/***
 *  sbset_aci_state(status block pointer, new ACI state)
 *
 *  set the status of the ACI in the status block
 */
void sbset_aci_state(rms_status *p, ACISTATE s)
{
     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: sbset_aci_state: %s", map_aci_state(s));
     }

     /* TODO: enter monitor */
     p->aci.aci_state = s;
     /* TODO: leave monitor */
}


/***
 *  sbset_aci_pid(status block pointer)
 *
 *  set the pid for the ACI process in the status block using the pid of
 *  the calling process
 */
void sbset_aci_pid(rms_status *p)
{
     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: sbset_aci_pid()");
     }

     /* TODO: enter monitor */
     p->aci.aci_pid = getpid();
     /* TODO: leave monitor */
}


/***
 *  get_cms_slot(pointer to status block, cms host to be located/assigned)
 *
 *  search the ACI host table for a match or assign a new host to a slot
 *  and return the slot number (index) in the table for the host
 *
 *  returns: host slot index, or -1 if table is full and no match
 */
int get_cms_slot(rms_status *p, char *cms_host)
{
     register int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: get_cms_slot(cms_host=%s)",
		 cms_host);
     }

     /*
      * search table for host
      */
     for (i = 0; i < RMS_CMS_TABLE_SIZE; i++) {
	  if (strlen(p->aci.aci_host[i].aci_host) <= 0) {
	       /*
		* found empty table entry without matching
		*/
	       strncpy(p->aci.aci_host[i].aci_host, cms_host, RMS_MAXHOSTNAME);
	       if (shm_debug) {
		    syslog(LOG_DEBUG, "SHMDEBUG: get_cms_slot(): new slot %d",
			   i);
	       }
	       break; /* this is now our match */
	  }

	  if (STRNEQUAL(cms_host, p->aci.aci_host[i].aci_host,
			MIN(RMS_MAXHOSTNAME, strlen(cms_host)))) {
	       /*
		* matched!
		*/
	       if (shm_debug) {
		    syslog(LOG_DEBUG, "SHMDEBUG: get_cms_slot(): matched %d",
			   i);
	       }
	       break;
/*	       return(i); */
	  }
     }

     if (i >= RMS_CMS_TABLE_SIZE) {
	  syslog(LOG_WARNING,
		 "ACI host status table FULL - %s counters not stored",
		 cms_host);
	  return(-1); /* table is full */
     }

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: get_cms_slot(): index is %d", i);
     }

     p->aci.aci_hostidx = i; /* remember/pubish host index */
     return(i); /* return newly added cms host slot index */
}


/***
 *  sb_incr_aci_error(status block pointer, cms hostname)
 *
 *  increment the error counters for the cms host (will add to
 *  host table entry if the cms host is "new")
 */
void sb_incr_aci_error(rms_status *p, char *cms_host)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: sb_incr_aci_error(cms_host=%s)",
		 cms_host);
     }

     if ((i = get_cms_slot(p, cms_host)) < 0) {
	  return; /* table is full, aci stats not updated */
     }

     p->aci.aci_host[i].aci_host_last_failure = time(NULL);
     p->aci.aci_host[i].aci_host_tot_fail_count++;
     p->aci.aci_host[i].aci_host_cur_fail_count++;
}


/***
 *  sb_incr_aci_checkin(status block pointer, cms hostname)
 *
 *  increment the success counter, clear the current failure count
 *  and set the last successful check in time
 */
void sb_incr_aci_checkin(rms_status *p, char *cms_host)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: sb_incr_aci_checkin(cms_host=%s)",
		 cms_host);
     }

     if ((i = get_cms_slot(p, cms_host)) < 0) {
	  return; /* table is full, aci stats not updated */
     }

     p->aci.aci_host[i].aci_host_last_ci = time(NULL);
     p->aci.aci_host[i].aci_host_ci_count++;
     p->aci.aci_host[i].aci_host_cur_fail_count = (unsigned long) 0L;
}


/***
 *  sbset_channel_stats(staus block pointer, cms hostname, updates, errors)
 *
 *  set the number of channels updated and errored on last checkin
 */
void sbset_channel_stats(rms_status *p, char *cms_host,
			 int updates, int errors)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG,
		 "SHMDEBUG: sbset_channel_stats(cms_host=%s, updates=%d, "
		 "errors=%d)",
		 cms_host, updates, errors);
     }

     if ((i = get_cms_slot(p, cms_host)) < 0) {
	  return; /* table is full, aci stats not updated */
     }

     p->aci.aci_host[i].aci_host_cur_channel_updates = updates;
     p->aci.aci_host[i].aci_host_cur_channel_errors = errors;
}

/***
 *  get_gw_slot(pointer to status block, gw call to be located/assigned)
 *
 *  search the gateway table for a match or assign a callsign to a slot
 *  and return the slot number (index) in the table for the gateway
 *
 *  returns: gateway slot index, or -1 if table is full and no match
 */
static int get_gw_slot(rms_status *p, char *gwcall)
{
     register int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: get_gw_slot(gwcall=%s)",
		 gwcall);
     }

     /*
      * search table for gateway call
      */
     for (i = 0; i < RMS_GW_TABLE_SIZE; i++) {
	  if (strlen(p->rmsgw[i].gw_gwcall) <= 0) {
	       /*
		* found empty table entry without matching
		*/
	       strncpy(p->rmsgw[i].gw_gwcall, gwcall, MAXGWCALL);
	       if (shm_debug) {
		    syslog(LOG_DEBUG, "SHMDEBUG: get_gw_slot(): new slot %d",
			   i);
	       }
	       break; /* this is now our match */
	  }

	  if (STRNEQUAL(gwcall, p->rmsgw[i].gw_gwcall,
			MIN(MAXGWCALL, strlen(gwcall)))) {
	       /*
		* matched!
		*/
	       if (shm_debug) {
		    syslog(LOG_DEBUG, "SHMDEBUG: get_cms_slot(): matched %d",
			   i);
	       }
	       break;
	       /*return(i);*/
	  }
     }

     if (i >= RMS_GW_TABLE_SIZE) {
	  syslog(LOG_WARNING,
		 "gateway status table FULL - stats for %s not stored",
		 gwcall);
	  return(-1); /* table is full */
     }

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: get_gw_slot(): index is %d", i);
     }

     return(i); /* return newly added gw slot index */
}


/***
 *  sbset_gw_state(status block pointer, new GW state, gateway callsign)
 *
 *  set the status of the GW in the status block
 */
void sbset_gw_state(rms_status *p, ACISTATE s, char *gwcall)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: sbset_gw_state(gwcall=%s): %s",
		 gwcall, map_gw_state(s));
     }

     if ((i = get_gw_slot(p, gwcall)) < 0) {
	  return; /* table is full, stats not updated */
     }

     /* TODO: enter monitor */
     p->rmsgw[i].gw_state = s;
     /* TODO: leave monitor */
}


/***
 *  sbset_gw_pid(status block pointer, gateway callsign)
 *
 *  set the pid for the ACI process in the status block using the pid of
 *  the calling process
 */
void sbset_gw_pid(rms_status *p, char *gwcall)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: sbset_gw_pid(gwcall=%s)", gwcall);
     }

     if ((i = get_gw_slot(p, gwcall)) < 0) {
	  return; /* table is full, stats not updated */
     }

     /* TODO: enter monitor */
     p->rmsgw[i].gw_pid = getpid(); /* TODO: this should be handled in get_gw_slot() as there can be multiple channels from a particular gateway, each with own process ??? */
     /* TODO: leave monitor */
}


/***
 *  sbset_gw_usercall(status block pointer, gateway callsign, user callsign)
 *
 *  set the usercall of the connected station for GW in the status block
 */
void sbset_gw_usercall(rms_status *p, char *gwcall, char *usercall)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG,
		 "SHMDEBUG: sbset_gw_usercall(gwcall=%s, usercall=%s)",
		 gwcall, usercall);
     }

     if ((i = get_gw_slot(p, gwcall)) < 0) {
	  return; /* table is full, stats not updated */
     }

     /* TODO: enter monitor */
     if (usercall != NULL) {
	  strncpy(p->rmsgw[i].gw_client, usercall, MAXUSERCALL);
     }
     /* TODO: leave monitor */
}


/***
 *  sbset_gw_cms(status block pointer, gateway callsign, cms host)
 *
 *  set the usercall of the connected station for GW in the status block
 */
void sbset_gw_cms(rms_status *p, char *gwcall, char *cms_host)
{
     int i;

     if ((i = get_gw_slot(p, gwcall)) < 0) {
	  return; /* table is full, stats not updated */
     }

     /* TODO: enter monitor */
     if (cms_host != NULL) {
	  strncpy(p->rmsgw[i].gw_cms, cms_host, RMS_MAXHOSTNAME);
     }
     /* TODO: leave monitor */
}


/***
 *  sb_incr_gw_connects(status block pointer, gateway callsign)
 *
 *  increments the number of connections to the gateway by one
 */
void sb_incr_gw_connects(rms_status *p, char *gwcall)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG, "SHMDEBUG: sb_incr_gw_connects(gwcall=%s)",
		 gwcall);
     }

     if ((i = get_gw_slot(p, gwcall)) < 0) {
	  return; /* table is full, stats not updated */
     }

     /* TODO: enter monitor */
     p->rmsgw[i].gw_connects++;
     /* TODO: leave monitor */
}


/***
 *  sb_add_gw_bytes_in(status block pointer, gateway callsign, count)
 *
 *  add 'count' bytes to the gateway's bytes in
 */
void sb_add_gw_bytes_in(rms_status *p, char *gwcall, long count)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG,
		 "SHMDEBUG: sb_add_gw_bytes_in(gwcall=%s, count=%ld)",
		 gwcall, count);
     }

     if ((i = get_gw_slot(p, gwcall)) < 0) {
	  return; /* table is full, stats not updated */
     }

     /* TODO: enter monitor */
     p->rmsgw[i].gw_bytes_in += count;
     /* TODO: leave monitor */
}


/***
 *  sb_add_gw_bytes_out(status block pointer, gateway callsign, count)
 *
 *  add 'count' bytes to the gateway's bytes out
 */
void sb_add_gw_bytes_out(rms_status *p, char *gwcall, long count)
{
     int i;

     if (shm_debug) {
	  syslog(LOG_DEBUG,
		 "SHMDEBUG: sb_add_gw_bytes_out(gwcall=%s, count=%ld)",
		 gwcall, count);
     }

     if ((i = get_gw_slot(p, gwcall)) < 0) {
	  return; /* table is full, stats not updated */
     }

     /* TODO: enter monitor */
     p->rmsgw[i].gw_bytes_out += count;
     /* TODO: leave monitor */
}
