/*
 *			r m s g w m o n . c
 * $Revision: 168 $
 * $Author: eckertb $
 *
 * RMS Gateway - status monitor
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
 */
#ifndef lint
static char svnid[] = "$Id: rmsgwmon.c 168 2014-09-30 23:33:51Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <errno.h>
#include <limits.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <ncurses.h>

#include "rms.h"
#include "rmslib.h"
#include "mapname.h"

/*
 * curses color pair ids
 */
#define SCR_COLOR_ACI_IDLE	1
#define	SCR_COLOR_ACI_ACTIVE	2
#define SCR_COLOR_HEADING	3
#define SCR_COLOR_LAST_CMS	4

#ifdef notdef
#define SCR_COLOR_BLAH_BLAH	5 /* unused */
#endif


#define SCR_COLOR_NORMAL	0
#define SCR_COLOR_ERROR		6
#ifdef notdef
#define SCR_COLOR_MORE_BLAH	7 /* unused */
#define SCR_COLOR_AND_MORE_BLAH	8 /* unused */
#endif

extern rms_status *rms_stat;

WINDOW	*msgwin; /* general message window */
WINDOW	*bstatwin, *aciwin, *gwwin; /* status windows and status border win */


/***
 *  dumpshm()
 *
 *  dump contents of shared memory to screen (stdout)
 */
void dumpshm(void)
{
     int i;

     printf("*** Linux RMS Gateway Shared Memory Dump***\n");
     printf("-------------------------------------------\n");
     printf("hdr.rms_hdr_magic: %.*s\n", RMS_SHM_MAGICSIZE,
	    rms_stat->hdr.rms_hdr_magic);


     printf("aci.aci_pid: %ld\n", (long) rms_stat->aci.aci_pid);
     printf("aci.aci_state: %ld\n", (long) rms_stat->aci.aci_state);
     printf("aci.aci_hostidx: %ld\n", (long) rms_stat->aci.aci_hostidx);

     for (i = 0; i < RMS_CMS_TABLE_SIZE; i++) {
	  printf("aci.aci_host[%d].aci_host: \"%.*s\"\n", i,
		 RMS_MAXHOSTNAME,
		 rms_stat->aci.aci_host[i].aci_host);
	  printf("aci.aci_host[%d].aci_host_last_ci: %ld\n", i,
		 rms_stat->aci.aci_host[i].aci_host_last_ci);
	  printf("aci.aci_host[%d].aci_host_last_failure: %ld\n", i,
		 rms_stat->aci.aci_host[i].aci_host_last_failure);
	  printf("aci.aci_host[%d].aci_host_tot_fail_count: %ld\n", i,
		 rms_stat->aci.aci_host[i].aci_host_tot_fail_count);
	  printf("aci.aci_host[%d].aci_host_cur_fail_count: %ld\n", i,
		 rms_stat->aci.aci_host[i].aci_host_cur_fail_count);
	  printf("aci.aci_host[%d].aci_host_cur_channel_updates: %d\n", i,
		 rms_stat->aci.aci_host[i].aci_host_cur_channel_updates);
	  printf("aci.aci_host[%d].aci_host_cur_channel_errors: %d\n", i,
		 rms_stat->aci.aci_host[i].aci_host_cur_channel_errors);
     }


     for (i = 0; i < RMS_GW_TABLE_SIZE; i++) {
	  printf("rmsgw[%d].gw_pid: %ld\n", i,
		 (long) rms_stat->rmsgw[i].gw_pid);
	  printf("rmsgw[%d].gw_state: %d\n", i,
		 rms_stat->rmsgw[i].gw_state);
	  printf("rmsgw[%d].gw_call: \"%.*s\"\n", i,
		 MAXGWCALL + 1,
		 rms_stat->rmsgw[i].gw_gwcall);
	  printf("rmsgw[%d].gw_client: \"%.*s\"\n", i,
		 MAXUSERCALL + 1,
		 rms_stat->rmsgw[i].gw_client);
	  printf("rmsgw[%d].gw_cms: \"%.*s\"\n", i,
		 RMS_MAXHOSTNAME,
		 rms_stat->rmsgw[i].gw_cms);
	  printf("rmsgw[%d].gw_connects: %ld\n", i,
		 rms_stat->rmsgw[i].gw_connects);
	  printf("rmsgw[%d].gw_bytes_in: %lld\n", i,
		 rms_stat->rmsgw[i].gw_bytes_in);
	  printf("rmsgw[%d].gw_bytes_out: %lld\n", i,
		 rms_stat->rmsgw[i].gw_bytes_out);
     }
}


/***
 *  aciStatus()
 *
 *  Show status of ACI activity
 */
void aciStatus(WINDOW *win)
{
     register int	i, j;
     struct tm	*t;

/*     wclear(win);*/
     werase(win);

     wmove(win, 0, 0);
     wattron(win, A_BOLD);
     wprintw(win, "ACI ");
     wattroff(win, A_BOLD);

     /*
      * set colors for ACI status
      */
     if (rms_stat->aci.aci_state == ACI_IDLE) {
	  wattron(win, COLOR_PAIR(SCR_COLOR_ACI_IDLE) | A_BOLD);
	  wprintw(win, "%s (%d)",
		  map_aci_state(rms_stat->aci.aci_state),
		  rms_stat->aci.aci_hostidx);
	  wattroff(win, COLOR_PAIR(SCR_COLOR_ACI_IDLE) | A_BOLD);
     } else {
	  wattron(win, COLOR_PAIR(SCR_COLOR_ACI_ACTIVE) | A_BOLD);
	  wprintw(win, "%s (%d) (%6d)",
		  map_aci_state(rms_stat->aci.aci_state),
		  rms_stat->aci.aci_hostidx,
		  rms_stat->aci.aci_pid);
	  wattroff(win, COLOR_PAIR(SCR_COLOR_ACI_ACTIVE) | A_BOLD);
     }
     wrefresh(win);

     /*
      * display heading for ACI fields
      */
     wmove(win, 1, 0);
     wattron(win, COLOR_PAIR(SCR_COLOR_HEADING)| A_UNDERLINE| A_BOLD);
     wprintw(win, " #  %-12.12s %-15.15s %-15.15s %8.8s %8.8s %8.8s",
	     "ACI Host", "Last CI", "Lst Fail", "CIs", "Ttl Fail", "Cur Fail");
     wattroff(win, COLOR_PAIR(SCR_COLOR_HEADING)|A_UNDERLINE | A_BOLD);
     wrefresh(win);

     /*
      * display fields for each CMS used by the ACI
      */
     for (i = 0; i < RMS_CMS_TABLE_SIZE; i++) {
	  char lasttime[16];
	  char lastfailtime[16];

	  if (strlen(rms_stat->aci.aci_host[i].aci_host) <= 0) {
	       break;
	  }

	  if (rms_stat->aci.aci_host[i].aci_host_last_ci <= (time_t) 0) {
	       strcpy(lasttime, "[never]");
	  } else if ((t = localtime(&(rms_stat->aci.aci_host[i].aci_host_last_ci)))) {
	       snprintf(lasttime, sizeof(lasttime), "%4d%02d%02d-%02d%02d%02d",
			t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);
	  } else {
	       strcpy(lasttime, "[time error]");
	  }

	  if (rms_stat->aci.aci_host[i].aci_host_last_failure <= (time_t) 0) {
	       strcpy(lastfailtime, "[never]");
	  } else if ((t = localtime(&(rms_stat->aci.aci_host[i].aci_host_last_failure)))) {
	       snprintf(lastfailtime, sizeof(lastfailtime), "%4d%02d%02d-%02d%02d%02d",
			t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);
	  } else {
	       strcpy(lastfailtime, "[time error]");
	  }

	  wmove(win, i + 2, 0);

	  /*
	   * set desired color for row depending on status of cms activity
	   */
	  if (rms_stat->aci.aci_state != ACI_IDLE) {
	       if (rms_stat->aci.aci_hostidx == i) {
		    wattron(win, COLOR_PAIR(SCR_COLOR_ACI_ACTIVE) | A_BOLD);
	       }
	  } else if (rms_stat->aci.aci_hostidx == i) {
	       wattron(win, COLOR_PAIR(SCR_COLOR_LAST_CMS) | A_BOLD);
	  } else if (rms_stat->aci.aci_host[i].aci_host_cur_fail_count > 0) {
	       wattron(win, COLOR_PAIR(SCR_COLOR_ERROR) | A_BOLD);
	  }


	  wprintw(win,
		  "%2d: %-12.12s %-15.15s %-15.15s %8ld %8ld ",
		  i + 1,
		  rms_stat->aci.aci_host[i].aci_host,
		  lasttime,
		  lastfailtime,
		  rms_stat->aci.aci_host[i].aci_host_ci_count,
		  rms_stat->aci.aci_host[i].aci_host_tot_fail_count);

	  /*
	   * current failure count isolated out in case we wish to adjust
	   * display attributes further for this statistic
	   */
	  wprintw(win, "%8ld",
		  rms_stat->aci.aci_host[i].aci_host_cur_fail_count);

	  /*
	   * reset color for row (see above)
	   */
	  if (rms_stat->aci.aci_state != ACI_IDLE) {
	       if (rms_stat->aci.aci_hostidx == i) {
		    wattroff(win, COLOR_PAIR(SCR_COLOR_ACI_ACTIVE) | A_BOLD);
	       }
	  } else if (rms_stat->aci.aci_hostidx == i) {
	       wattroff(win, COLOR_PAIR(SCR_COLOR_LAST_CMS) | A_BOLD);
	  } else if (rms_stat->aci.aci_host[i].aci_host_cur_fail_count > 0) {
	       wattroff(win, COLOR_PAIR(SCR_COLOR_ERROR) | A_BOLD);
	  }

     }
	
     wmove(win, 0, 0);
     wrefresh(win);
}


/***
 *  gwStatus()
 *
 *  show status of gateways
 */
void gwStatus(WINDOW *win)
{
     register int	i, j;
     int		names = 1;

/*     wclear(win);*/
     werase(win);
     wmove(win, 0, 0);
     wattron(win, A_BOLD);
     wprintw(win, "Gateway");
     wattroff(win, A_BOLD);

     wmove(win, 1, 0);
     wattron(win, COLOR_PAIR(SCR_COLOR_HEADING)| A_UNDERLINE| A_BOLD);
     wprintw(win,
	     " #  %6.6s %12.12s %12.12s %12.12s %12.12s %9.9s  %-9.9s",
	     "PID", "State", "Connects", "Bytes In", "Bytes Out", "GW Call",
	     "Client");
     wattroff(win, COLOR_PAIR(SCR_COLOR_HEADING)|A_UNDERLINE | A_BOLD);
     wrefresh(win);
     for (i = 0; i < RMS_GW_TABLE_SIZE; i++) {
	  if (rms_stat->rmsgw[i].gw_state == GW_UNUSED) {
	       continue; /* skip unused slots */
	  }

	  wmove(win, i + 2, 0);
	  wprintw(win,
		  "%2d: ",
		  i + 1);

	  /*
	   * if gw is idle, suppress pid, since there is no
	   * active process -- might still want to display this
	   * somehow to help in log searches
	   */
	  if (rms_stat->rmsgw[i].gw_state == GW_IDLE) {
	       wattron(win, A_INVIS);
	  }
	  wprintw(win, "%6ld ",
		  (long) rms_stat->rmsgw[i].gw_pid);
	  if (rms_stat->rmsgw[i].gw_state == GW_IDLE) {
	       wattroff(win, A_INVIS);
	  }

	  wprintw(win,
		  "%12s %12ld %12lld %12lld %9.9s",
		  map_gw_state(rms_stat->rmsgw[i].gw_state),
		  rms_stat->rmsgw[i].gw_connects,
		  rms_stat->rmsgw[i].gw_bytes_in,
		  rms_stat->rmsgw[i].gw_bytes_out,
		  rms_stat->rmsgw[i].gw_gwcall);

	  /*
	   * if gw is idle, suppress comm stat and client, as they are
	   * meaningless
	   */
	  if (rms_stat->rmsgw[i].gw_state == GW_IDLE) {
	       wattron(win, A_INVIS);
	  }
	  wprintw(win,
		  "%2s%-9.9s",
		  map_gw_comm(rms_stat->rmsgw[i].gw_state),
		  rms_stat->rmsgw[i].gw_client);
	  if (rms_stat->rmsgw[i].gw_state == GW_IDLE) {
	       wattroff(win, A_INVIS);
	  }

     }
	  
     wmove(win, 0, 0);
     wrefresh(win);
}


/***
 *  ShowStatus()
 */
void ShowStatus(void)
{
     aciStatus(aciwin);
     gwStatus(gwwin);
}


/***
 *  ShowMessage()
 *
 *  display a general message in the indicated window
 */
void ShowMessage(WINDOW *win, char *msg)
{
     register int	i, j;

     wattron(win, COLOR_PAIR(SCR_COLOR_NORMAL));
     mvwprintw(win, 0, 0, "%s", msg);

     wmove(win, 0, 0);
     wrefresh(win);
}


/***
 *  scrInit()
 *
 *  initialize the screen
 */
int scrInit(void)
{
     initscr();
     keypad(stdscr, TRUE);
     raw();
     noecho();

     start_color();

     init_pair(SCR_COLOR_NORMAL, COLOR_WHITE, COLOR_BLACK);
     init_pair(SCR_COLOR_ACI_IDLE, COLOR_YELLOW, COLOR_BLACK);
     init_pair(SCR_COLOR_ACI_ACTIVE, COLOR_GREEN, COLOR_BLACK);
     init_pair(SCR_COLOR_HEADING, COLOR_CYAN, COLOR_BLACK);
     init_pair(SCR_COLOR_LAST_CMS, COLOR_BLUE, COLOR_BLACK);
     init_pair(SCR_COLOR_ERROR, COLOR_RED, COLOR_BLACK);

     return(0);
}


/***
 *  scrShut()
 *
 *  shutdown the screen
 */
int scrShut(void)
{
     endwin();

     return(0);
}


/***
 *  gwStatRun()
 *
 *  run the gateway status loop
 *
 *  interval = update interval
 */
void gwStatRun(int interval)
{
     int	running = 1;
     fd_set	rds;
     int	nsel;
     struct timeval	stimer;
     int splitrow;
     int acistart, aciend;
     int gwstart, gwend;

#ifdef notdef
     /*
      * setup necessary signal handlers
      */
     sigaction(SIGINT, NULL, &old_handler);
#endif

     /*
      * interval sanity check
      */
     if (interval < 1) {
	  interval = 5;
     }

     /*
      * initialize curses/windows
      */
     scrInit();

     msgwin = newwin(1, COLS, LINES - 1, 0);
     scrollok(msgwin, FALSE);

     /*
      * setup window which will be only for the border (NOTE: this may
      * go away, since is largely a waste of two lines on the display)
      */
     bstatwin = newwin(LINES - 1, COLS, 0, 0);

     /*
      * calculate subwindow sizes for the aci and gw
      */
     splitrow = bstatwin->_maxy / 2; /* where is the split to be */
     acistart = 1;
     aciend = splitrow - 1;
     gwstart = splitrow + 1;
     gwend = bstatwin->_maxy - 2;

/*     aciwin = subwin(bstatwin, bstatwin->_maxy - 2, bstatwin->_maxx - 2,
       1, 1);*/
     aciwin = subwin(bstatwin, aciend - acistart + 1, bstatwin->_maxx - 2,
		     acistart, 1);
     gwwin = subwin(bstatwin, gwend - gwstart + 1, bstatwin->_maxx - 2,
		    gwstart, 1);
     scrollok(aciwin, FALSE);
     wmove(msgwin, 0, 0);

     /*
      * go to it
      */
     while (running) {
	  ShowStatus();
	  //ShowMessage(msgwin, "[ message area -- currently unused ]");
	  ShowMessage(msgwin, "[ no messages ]"); /* TODO: eventually will do
						     something useful with
						     this */

	  box(bstatwin, 0, 0);
	  mvwhline(bstatwin, splitrow, 1, 0, bstatwin->_maxx - 1);
	  wrefresh(bstatwin);


	  /*
	   * wait for keystroke or timeout
	   */
#ifdef notdef
	  stimer.tv_sec = (long) interval;
	  stimer.tv_usec = 0L;
#endif

	  stimer.tv_sec = 0L;
	  stimer.tv_usec = 500000L;

	  FD_ZERO(&rds);
	  FD_SET(0, &rds);

	  if ((nsel = select(FD_SETSIZE, &rds, NULL, NULL, &stimer)) < 0) {
	       if (errno == EINTR) {
		    continue;
	       }
	       scrShut();
	       
	       syslog(LOG_ERR,"select failed (errno = %d)", errno);
	       exit(1);
	  }

	  if (nsel) {
	       int	key;

	       key = getch();

	       switch (key) {
	       case '0': /* change refresh interval */
	       case '1':
	       case '2':
	       case '3':
	       case '4':
	       case '5':
	       case '6':
	       case '7':
	       case '8':
	       case '9':
		    /* TODO: redo the refresh interval handling */
		    /*
		     * 0 means 10 seconds
		     */
		    if (key == '0') {
			 key = '9' + 1;
		    }

		    interval = (key - 0x30);
		    break;
	       case KEY_F(3): /* QUIT */
	       case '\033':
	       case 'Q':
	       case 'q':
	       case 'X':
	       case 'x':
		    running = 0;
		    break;
	       }
	  }
     }

     scrShut();
}


/***
 *  main()
 */
main(int argc, char *argv[])
{
     register int	i, j;
     char		*optstr = "i:l:d";
     int		interval = 5; /* refresh interval */
     int		shmid;
     int		rc;
     char buffer[MAXBUF];
     char *logmaskarg = NULL;
     char logmask[64];
     char facility[64];
     int dump = 0;
     version_blk *version;
     int		opt;
     extern int		optind;
     extern char	*optarg;

     strcpy(logmask, "INFO");
     strcpy(facility, "local0");

     /*
      * parse the command line arguments
      */
     while ((opt = getopt(argc, argv, optstr)) != EOF) {
	  switch (opt) {
	  case 'i': /* refresh interval */
	       interval = atoi(optarg);
	       break;
	  case 'l': /* log mask */
	       strcpy(logmask, optarg);
	       break;
	  case 'd': /* just dump shared memory to screen */
	       dump++;
	       break;
	  case '?': /* unknown option */
	  default:
	       fprintf(stderr, "%s: ignoring unrecognized option '%c'\n",
		       argv[0], opt);
	       break;
	  }
     }

     if ((version = loadVersion(RMSVERSIONFILE, &rc)) == NULL) {
	  switch (rc) {
	  case ERR_VER_MISSINGPACK:
	       err("ERROR: Version: Missing Package Name!");
	       break;
	  case ERR_VER_MISSINGPROG:
	       err("ERROR: Version: Missing Program Name!");
	       break;
	  case ERR_VER_MISSINGLAB:
	       err("ERROR Version: Missing Version Label!");
	       break;
	  case ERR_VER_MAPERROR:
	       snprintf(buffer, sizeof(buffer), "ERROR in version file: %s",
		       maperrmsg(map_errno));
	       err(buffer);
	       break;
	  default: /* should never happen! */
	       snprintf(buffer, sizeof(buffer), "UNKNOWN ERROR (%d) IN VERSION FILE", rc);
	       err(buffer);
	       break;
	  }

	  /*
	   * set falback values for version info so we can try
	   * to continue
	   */
	  if (version->package == NULL) {
	    version->package = VER_FALLBACK_PACKAGE;
	  }
	  if (version->program == NULL) {
	    version->program = VER_FALLBACK_PROGRAM;
	  }
	  if (version->label == NULL) {
	    version->label = VER_FALLBACK_LABEL;
	  }
     }

     /*
      * reopen syslog using the configured facility/logmask
      */
     closelog();
     openlog("rmsgwmon", LOG_CONS|LOG_PID|LOG_NDELAY,
	     mapfacility(downcase(facility)));
     setlogmask(LOG_UPTO(mappriority(downcase(logmask))));

     syslog(LOG_INFO,"%s Gateway Monitor %s %s",
 	    version->package, version->label, __DATE__);

     /*
      * attach the status block to the process (attached memory address will
      * be at returned value in rms_stat)
      */
     if (statblock_attach(&rms_stat) < 0) {
	  syslog(LOG_CRIT, "FATAL: cannot attach shared memory segment");

	  fprintf(stderr,
		  "FATAL error: can't attach status block (see syslog)\n");

	  closelog();
	  exit(1);
     }

     if (dump) {
	  /*
	   * do a simple dump of what's in shared memory
	   */
	  dumpshm();
     } else {
	  /*
	   * continuous, curses output, main loop
	   */
	  gwStatRun(interval);
     }

     exit(0);
}
