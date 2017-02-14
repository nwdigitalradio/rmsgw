/*
 *			r m s s h m . h
 * $Revision: 106 $
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
 *	defines, structures, and prototypes used for the Linux
 *	RMS Gateway shared memory gateway status blocks
 */
#ifndef _rmsshm_h
#define _rmsshm_h	1

#ifndef lint
static char _rmsshm_h_svnid[] = "$Id: rmsshm.h 106 2009-04-28 15:09:24Z eckertb $";
#endif /* lint */

/*
 * Shared memory general layout (see actual structure declarations below):
 *
 * rms_status {
 *	hdr {
 *		rms_hdr_magic
 *		rms_station_count
 * 	}
 *	aci {
 *		aci_pid
 *		aci_state
 *		aci_hostidx
 *		aci_host[1..RMS_CMS_TABLE_SIZE] {
 *			aci_host
 *			aci_host_last_ci
 *			aci_host_ci_count
 *			aci_host_last_failure
 *			aci_host_tot_fail_count
 *			aci_host_cur_fail_count
 *			aci_host_cur_channel_updates
 *			aci_host_cur_channel_errors
 *		}
 * 	}
 * 	rmsgw[1..RMS_GW_TABLE_SIZE] {
 *		gw_pid
 *		gw_state
 *		gw_gwcall
 *		gw_client
 *		gw_cms
 *		gw_connects
 *		gw_bytes_in
 *		gw_bytes_out
 * 	}
 * }
 */
#define RMS_SHM_MAGIC	"!RMSGW00"	/* magic "number" for shared mem seg */
#define RMS_SHM_MAGICSIZE	8	/* MUST BE # BYTES IN RMS_SHM_MAGIC */
#define RMS_SHM_PERMS	0666		/* perms for shared mem seg */

#define RMS_MAXHOSTNAME	64		/* max size of a hostname */

#define RMS_CMS_TABLE_SIZE	10	/* number of shared memory "slots"
					   for tracking CMS host activity */
#define RMS_GW_TABLE_SIZE	10	/* number of shared memory "slots"
					   for tracking gateway activity --
					   the system will use one slot for
					   each separate callsign which is
					   used by gateway processes */

/*
 * gateway state definitions
 */
typedef enum { GW_UNUSED, GW_RUNNING, GW_IDLE, GW_CONNECTING, GW_AUTHORIZING,
	       GW_CONNECTED, GW_LOGIN, GW_LOGGEDIN,GW_COMMWAIT,
	       GW_SENDING, GW_RECEIVING,
	       GW_DISCONNECTING, GW_DISCONNECTED } GWSTATE;

/*
 * ACI state definitions
 */
typedef enum { ACI_UNUSED, ACI_RUNNING, ACI_IDLE, ACI_CONNECTING, ACI_CONNECTED,
	       ACI_CHANNELADD, ACI_CHANNELUPDATE, ACI_VERSIONUPDATE,
	       ACI_DISCONNECTING, ACI_DISCONNECTED } ACISTATE;

/*
 * shared memory header structure for gateway status
 */
typedef struct {
     char rms_hdr_magic[RMS_SHM_MAGICSIZE]; /* magic number for an rmsgw
					       shared memory segment */
     /*int rms_hdr_shm_debug; /* flag to turn on shared memory operation
			       debugging */
} rms_stat_hdr;

/*
 * ACI host status structure (this structure is used within the ACI status
 * structure)
 */
typedef struct {
     char aci_host[RMS_MAXHOSTNAME];
     time_t aci_host_last_ci; /* time of last successful check-in
				 via this host */
     unsigned long aci_host_ci_count; /* counter for number of check-ins
					 wtih this host */
     time_t aci_host_last_failure; /* time of last checkin failure with
				      this host */
     unsigned long aci_host_tot_fail_count; /* number of checkin failures
					       with this host */
     unsigned long aci_host_cur_fail_count; /* number of (current) successive
					       failures */
     unsigned int aci_host_cur_channel_updates; /* number of channels updated
						   on last check-in */
     unsigned int aci_host_cur_channel_errors; /* number of channels with error
						  on last check-in */
} aci_host_stat;

/*
 * general ACI status structure
 */
typedef struct {
     pid_t aci_pid; /* current or most recent ACI process id */
     ACISTATE aci_state; /* current state of ACI process */
     int aci_hostidx; /* which host index is currently/most recently  active */
     aci_host_stat aci_host[RMS_CMS_TABLE_SIZE];
} aci_stat;

/*
 * structure of each gateway "station" being tracked
 */
typedef struct {
     pid_t gw_pid; /* current or most recent rmsgw process id */
     GWSTATE gw_state; /* current state of rmsgw process */
     char gw_gwcall[MAXGWCALL + 1]; /* gateway callsign (+1 for terminator)*/
     char gw_client[MAXUSERCALL + 1]; /* remote client call
					 (+1 for terminator) */
     char gw_cms[RMS_MAXHOSTNAME]; /* the CMS we're connecting/connected to */
     unsigned long gw_connects; /* number of connections to this gateway */
     unsigned long long gw_bytes_in; /* number of bytes in */
     unsigned long long gw_bytes_out; /* number of bytes out */
} rmsgw_stat;

/*
 * the actual shared memory status structure definition in all its... um...
 * glory
 */
typedef struct {
     rms_stat_hdr hdr; /* status header block */
     aci_stat aci; /* aci status block */
     rmsgw_stat rmsgw[RMS_GW_TABLE_SIZE]; /* gateway status block table */
} rms_status;

extern key_t shm_getkey(char *pathname, int proj_id);
extern int shm_get(char *pathname, int proj_id, int segment_size, int *doinit);
extern void *shm_attach(int shmid);
extern int shm_detach(const void *addr);
#endif /* _rmsshm_h */
