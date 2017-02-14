/*
 *			r m s l i b . h
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
#ifndef _rmslib_h
#define _rmslib_h	1

#ifndef lint
static char	_rmslib_h_svnid[] = "$Id: rmslib.h 176 2014-10-27 09:07:54Z eckertb $";
#endif /* lint */

#include <stdbool.h>
#include "set.h"
#include "rms.h"
#include "rmsshm.h"

/*
 * macros
 */
#ifndef MAX
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif

#ifndef STREQUAL
#define STREQUAL(a, b)  (!strcmp((a), (b)))
#endif

#ifndef STRNEQUAL
#define STRNEQUAL(a, b, n)      (!strncmp((a), (b), (n)))
#endif

#ifndef STRNCMP
#define STRNCMP(a, b)   (strncmp((a), (b), strlen(a)))
#endif

#ifndef DIM
#define DIM(a)		(sizeof(a)/sizeof(*(a)))	/* length of array */
#endif

#ifndef CTRL
#ifdef __STDC__
#define CTRL(c)	(*(#c)&037)	/* inefficient but correct */
#else
#define CTRL(c)		('c' ^ 0x40)
#endif
#endif

/*
 * general type definitions
 */

/*
 * package version structure
 */
typedef struct _version {
     char *package; /* the package name */
     char *program; /* the program name for version reporting to winlink */
     char *label; /* the version label string */
     /* the remaining items reflect subversion
        keyword values */
     char *revision; /* the repository revision */
     char *date; /* revision date */
     char *author; /* revision author */
     char *id; /* revision id string */
} version_blk;

/*
 * possible error codes for loadVersion()
 */
#define ERR_VER_MAPERROR	1
#define ERR_VER_MISSINGFILE	2
#define ERR_VER_MISSINGPACK	3
#define ERR_VER_MISSINGPROG	4
#define ERR_VER_MISSINGLAB	5     

/*
 * some values for fallbacks
 */
#define VER_FALLBACK_PACKAGE	"Linux RMS Gateway"
#define VER_FALLBACK_PROGRAM	"RMS Gateway"
#define VER_FALLBACK_LABEL	"0.0.0"

extern version_blk *loadVersion(const char *verfile, int *rc);

/*
 * structure to hold CMS host entries as read from
 * host file
 *
 * host file structure is a set of lines formatted like:
 *	cms-hostname:connection-port:host-password
 */
typedef struct _cms {
     char *cms_host;	/* CMS host name */
     int  cms_port;	/* CMS connect port */
     char *cms_passwd;	/* CMS host password */
} cms;
extern void setcmsfile(char *cp);
extern int setcmsent(void);
extern void endcmsent(void);
extern cms *getcmsent(void);
extern cms *getcmsnam(register char *name);
extern void putcmsent(register cms *c, register FILE *fp);

/*
 * cms host linked list stucture and function prototypes
 */
typedef struct cmsnode {
     char *cms_host;
     int cms_port;
     char *cms_passwd;
     time_t cms_stat_time;
     struct cmsnode *next;

} cmsnode;

extern cmsnode *getcmslist(void);
extern cmsnode *addcmsnode(cmsnode *clist, cms *c, time_t stime);

/*
 * structure to hold channel entries as read from
 * channel XML file
 *
 * RMS channels structure and library function prototypes
 */
typedef struct _channel {
     char *ch_name;	/* port/channel name (e.g., port from ax25ports) */
     char *ch_type;	/* type of channel (e.g., ax25) */
     char *ch_active;	/* channel active indicator (yes/no) */
     char *ch_basecall;	/* base callsign of the gateway */
     char *ch_callsign;	/* callsign/ssid for the gw channel */
     char *ch_password;	/* winlink secure gateway login password */
     char *ch_gridsquare;/*gridsquare of gateway */
     char *ch_frequency;/* the channel's frequency (in Hz) */
     char *ch_mode;	/* the "pactor" mode of the channel */
     char *ch_autoonly;	/* auto only indicator (0 or 1) */
     char *ch_baud;	/* baud rate of channel */
     char *ch_power;	/* power (watts) of channel */
     char *ch_height;	/* antenna height */
     char *ch_gain;	/* antenna gain in db (0 is unity) */
     char *ch_direction;/* antenna direction (0 is omni) */
     char *ch_hours;	/* hour of operation for this channel */
     char *ch_groupreference;/* channel group reference */
     char *ch_servicecode; /* channel service code */
     char *ch_statuschecker;/* pathname of local channel status check progam */
} channel;
extern void setchanfile(char *p);
extern int setchanent(void);
extern void endchanent(void);
extern channel *getchanent(void);
extern channel *getchannam(register char *chname, register char *callsign);

/*
 * configuration structure used by loadConfig()
 */
typedef struct _config {
     char *gwcall;	/* gateway callsign (configured default) */
     char *channelfile;	/* channel file for login/status reporting */
     char *gridsquare;	/* gateway's grid square */
     char *bannerfile;	/* banner file to be sent once connected */
     char *authfile;	/* rms authorization file for -- no longer used */
     char *logfacility;	/* syslog facility name */
     char *logmask;	/* syslog log maks */
     char *python;	/* the path for python (available as configuration option because some
			   systems may need to manually install a newer version supporting the
			   web service modules used */
     char *versionupd;	/* version update frequency in seconds */
     char *channelupd;	/* channel update frequency in seconds */
     char *rmsgwenv;	/* file holding environment variables for rmsgw */
     char *rmsgwacienv;	/* file holding environment variables for rmsgw_aci */
     char *hookdir;	/* directory where gateway hook scripts live */
} config;

/*
 * possible error codes for loadConfig()
 */
#define ERR_CFG_MAPERROR	1
#define ERR_CFG_MISSINGFILE	2
#define ERR_CFG_INVALIDCALL	3
#define ERR_CFG_MISSINGCALL     4

extern config *loadConfig(const char *cfgfile, int *rc);
extern int loadenv(const char *envfile);

/*
 * miscellaneous function prototypes
 */
extern time_t fileage(char *the_file);
extern char *strtrim(char *dest, char *src, char *set, int trim);
extern char *upcase(char *s);
extern char *downcase(char *s);
extern char *strcvt(char *s, char f, char t);
extern char *fgetline(char *s, int size, FILE *fp);
extern char *fgetlinecr(char *s, int size, FILE *fp);
extern int printfile(char *file);

extern int readln(register int fd, register char *buf, register int maxlen, unsigned char eol);
extern int cmsConnect(cmsnode *c);
extern char *sgl_challenge_response(char *challenge, char *pass);
extern int cmslogin(int sd, config *cfg, channel *c, char *usercall, char *passwd);
extern int setcmsstat(cmsnode *p);

extern int mappriority(char *pri);
extern int mapfacility(char *facility);

extern void err(char *message);
extern void msg(char *message);
extern bool file_exists(const char *filename);
extern int runhook(char* hookdir, char *hook_name, ...);

/*
 * options and prototype for sendrf()
 */
#define SRF_EOLXLATE	0x00000001 /* translate UNIX newline */
#define SRF_ADDEOL	0x00000002 /* add a final end of line on send */

extern int sendrf(int sd, register char *buf, register int len, int opts);

extern int sendDataGram(char *host, char *port, unsigned char* msg, int msg_len);

/*
 * status block function prototypes
 */
extern int set_shm_debug(void);
extern int get_cms_slot(rms_status *p, char *cms_host);
extern int statblock_attach(rms_status **addr);
extern int statblock_detach(rms_status *addr);
extern void sbset_aci_state(rms_status *p, ACISTATE s);
extern void sbset_aci_pid(rms_status *p);
extern void sb_incr_aci_error(rms_status *p, char *cms_host);
extern void sb_incr_aci_checkin(rms_status *p, char *cms_host);
extern void sbset_channel_stats(rms_status *p, char *cms_host, int updates, int errors);
extern void sbset_gw_state(rms_status *p, ACISTATE s, char *gwcall);
extern void sbset_gw_pid(rms_status *p, char *gwcall);
extern void sbset_gw_usercall(rms_status *p, char *gwcall, char *usercall);
extern void sb_incr_gw_connects(rms_status *p, char *gwcall);
extern void sb_add_gw_bytes_in(rms_status *p, char *gwcall, long count);
extern void sb_add_gw_bytes_out(rms_status *p, char *gwcall, long count);

/*
 * GENERAL CONSTANTS
 */

#define	SUCCEED		0		/* for use in exit() and retruns */
#define	FAIL		-1		/* for exit() & error returns */

/*
 * named characters
 */
#define A_NUL		'\000'
#define EOS		A_NUL
#define A_SOH		'\001'
#define A_STX		'\002'
#define A_ETX		'\003'
#define A_EOT		'\004'
#define A_ENQ		'\005'
#define A_ACK		'\006'
#define A_BEL		'\007'
#define A_BS		'\010'
#define A_HT		'\011'
#define TAB		A_HT
#define HTAB		A_HT
#define A_LF		'\012'
#define NEWLINE		A_LF
#define A_VT		'\013'
#define VTAB		A_VT
#define A_FF		'\014'
#define FORMFEED	A_FF
#define A_CR		'\015'
#define RETURN		A_CR
#define A_SO		'\016'
#define A_SI		'\017'
#define A_DLE		'\020'
#define A_DC1		'\021'
#define A_DC2		'\022'
#define A_DC3		'\023'
#define A_DC4		'\024'
#define A_NAK		'\025'
#define A_SYN		'\026'
#define A_ETB		'\027'
#define A_CAN		'\030'
#define A_EM		'\031'
#define A_SUB		'\032'
#define A_ESC		'\033'
#define A_FS		'\034'
#define A_GS		'\035'
#define A_RS		'\036'
#define A_US		'\037'
#define BLANK		' '
#define SPACE		BLANK
#define A_DEL		'\177'

#endif /* _rmslib_h */
