/*
 *			r m s . h
 * $Revision: 176 $
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
 * Description:
 *	defines, structures, and prototypes used for the Linux
 *	RMS Gateway.
 */

#ifndef _rms_h
#define _rms_h	1

#ifndef lint
static char _rms_h_svnid[] = "$Id: rms.h 176 2014-10-27 09:07:54Z eckertb $";
#endif /* lint */

#include <stdio.h>
#include <sys/types.h>

#define MAXBUF		512	/* general IO/String buffer size */
#define MAXGREETING	80	/* max length of greeting string */
#define MAXUSERCALL	9	/* the connected user callsign max length */
#define MAXGWCALL	9	/* local (rms) callsign max length */
#define MAXBTEXT	2048	/* max size of banner text "line" */
#define MAXSERVICECODE	16	/* max length of sysop defined service code */

#define DFLT_SERVICECODE	"PUBLIC"	/* default service code string if none defined */

#define DFLTPACLEN	128	/* default packet length */

#define DFLT_VERSIONUPD	86400	/* version updates daily */
#define DFLT_CHANNELUPD	7140	/* channel updates every two hours less one minute */

#define RMSCONFIGFILE	"/etc/rmsgw/gateway.conf" /* default config file */
#define RMSVERSIONFILE	"/etc/rmsgw/.version_info" /* version info file */
#define RMSBTEXTFILE	"/etc/rmsgw/banner" /* default banner file */
#define CMSHOSTFILE	"/etc/rmsgw/hosts" /* default cms host file */
#define CHANNELFILE	"/etc/rmsgw/channels" /* default channels file */
#define XMLCHANNELFILE	"/etc/rmsgw/channels.xml" /* dflt xml channels file */
#define GWHELPFILE	"/etc/rmsgw/gwhelp" /* gateway help file */
#define ACIHELPFILE	"/etc/rmsgw/acihelp" /* aci help file */
#define CMSSTATDIR	"/etc/rmsgw/stat" /* cms status directory */
#define CHANNELUPDATER	"/etc/rmsgw/updatechannel.py" /* program to update channels */
#define VERSIONUPDATER	"/etc/rmsgw/updateversion.py" /* program to update version */
#define HOOKDIR		"/etc/rmsgw/hooks" /* directory the holds gateway hook scripts */

#define CONNECT_TIMEOUT	5	/* cms host connection timeout value */
#define CONV_TIMEOUT	60	/* cms conversation timeout value */

#define SECSPERDAY	86400	/* number of seconds in 24 hours */
#define SECSPERHOUR	3600	/* number of seconds in 1 hour */

extern const char configfile[];
extern int verbose;
extern int paclen;

#ifdef HOOKS
#define NBRSTRSIZE	32
#endif
struct ust {
     int errcode;
#ifdef HOOKS
     char errcode_str[NBRSTRSIZE];
#endif
     unsigned long bytes_sent;
#ifdef HOOKS
     char bytes_sent_str[NBRSTRSIZE];
#endif
     unsigned long bytes_recv;
#ifdef HOOKS
     char bytes_recv_str[NBRSTRSIZE];
#endif
};

/*
 * Secure Gateway Login state definitions
 */
typedef enum { SGL_CALLSIGN, SGL_CALLSIGNFAIL,
	       SGL_CHALLENGE, SGL_CHALLENGEFAIL,
	       SGL_RESPONSESENT,
	       SGL_LOGINCOMPLETE, SGL_BADSTATE } SGLSTATE;

/*
 * gateway error returns (ust structure rc codes)
 */
#define ERR_GW_LOGIN		-1 /* login error */
#define ERR_GW_TIMEO		1 /* gatway timeout with CMS */
#define ERR_GW_FATAL		-101 /* fatal error code lower limit */
#define ERR_GW_SOCK		-101 /* socket error */
#define ERR_GW_AX25		-102 /* ax25 error */
#define ERR_GW_SOCKIO		-103 /* to/from CMS */
#define ERR_GW_AX25IO		-104 /* to/from AX.25 */
#define ERR_GW_INVPORT		-105 /* invalid/missing port */

extern int log_aci(char *hostname, struct ust userstat,
		   time_t tstart, time_t tstop);

/*
** hook related stuff (this is all experimental, and will only be used
** if the gateway is compiled with -DHOOKS (see rmsgw.mk)
*/
#define HOOKSHELL	"/bin/sh"
#define MAXHOOKARGS	128

/*
 * hook names
 */
#define START_RMSGW	"start-rmsgw"
#define END_RMSGW	"end-rmsgw"
#define PRE_RMSGW_SESSION	"pre-rmsgw-session"
#define POST_RMSGW_SESSION	"post-rmsgw-session"

#define START_RMSGW_ACI	"start-rmsgw_aci"
#define END_RMSGW_ACI	"end-rmsgw_aci"
#define PRE_RMSGW_ACI_CHANNEL_UPDATE	"pre-rmsgw_aci-channel-update"
#define POST_RMSGW_ACI_CHANNEL_UPDATE	"post-rmsgw_aci-channel-update"
#define PRE_RMSGW_ACI_VERSION_UPDATE	"pre-rmsgw_aci-version-update"
#define POST_RMSGW_ACI_VERSION_UPDATE	"post-rmsgw_aci-version-update"

#endif /* _rms_h */
