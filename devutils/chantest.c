/*
 *			c h a n t e s t . c
 * $Revision: 149 $
 * $Author: eckertb $
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
static char svnid[] = "$Id: chantest.c 149 2013-07-03 02:01:55Z eckertb $";
#endif /* lint */

#include <stdio.h>
#include <stdlib.h>

#include "rms.h"
#include "rmslib.h"

main(int argc, char *argv[])
{
     channel *ch;

     setchanfile("channels.xml");

     while ((ch = getchanent()) != NULL) {
	  /*
	   * if no service code is defined, default it
	   */
	  if (ch->ch_servicecode == NULL || strlen(ch->ch_servicecode) <= 0) {
	       ch->ch_servicecode = strdup(DFLT_SERVICECODE); /* need to allocate the string,
								   since the library will free
								   this on subsequent calls */
	  }
	  printf("Found channel '%s' (active=%s)\n",
		 ch->ch_name, ch->ch_active ? ch->ch_active : "(null)");
	  printf("\t      basecall = %s\n", ch->ch_basecall);
	  printf("\t      callsign = %s\n", ch->ch_callsign);
	  printf("\t    gridsquare = %s\n", ch->ch_gridsquare);
	  printf("\t     frequency = %s\n", ch->ch_frequency);
	  printf("\t          mode = %s\n", ch->ch_mode);
	  printf("\t      autoonly = %s\n", ch->ch_autoonly);
	  printf("\t          baud = %s\n", ch->ch_baud);
	  printf("\t         power = %s\n", ch->ch_power);
	  printf("\t        height = %s\n", ch->ch_height);
	  printf("\t          gain = %s\n", ch->ch_gain);
	  printf("\t     direction = %s\n", ch->ch_direction);
	  printf("\t         hours = %s\n", ch->ch_hours);
	  printf("\tgroupreference = %s\n", ch->ch_groupreference);
	  printf("\t   servicecode = %s\n", ch->ch_servicecode);
	  printf("\t statuschecker = %s\n", ch->ch_statuschecker);
     }

     endchanent();

     if ((ch = getchannam("radio", "N0CALL-10")) != NULL) {
	  printf("Found channel (by name) '%s' (active=%s)\n",
		 ch->ch_name, ch->ch_active ? ch->ch_active : "(null)");
	  printf("\t      basecall = %s\n", ch->ch_basecall);
	  printf("\t      callsign = %s\n", ch->ch_callsign);
	  printf("\t    gridsquare = %s\n", ch->ch_gridsquare);
	  printf("\t     frequency = %s\n", ch->ch_frequency);
	  printf("\t          mode = %s\n", ch->ch_mode);
	  printf("\t      autoonly = %s\n", ch->ch_autoonly);
	  printf("\t          baud = %s\n", ch->ch_baud);
	  printf("\t         power = %s\n", ch->ch_power);
	  printf("\t        height = %s\n", ch->ch_height);
	  printf("\t          gain = %s\n", ch->ch_gain);
	  printf("\t     direction = %s\n", ch->ch_direction);
	  printf("\t         hours = %s\n", ch->ch_hours);
	  printf("\tgroupreference = %s\n", ch->ch_groupreference);
	  printf("\t   servicecode = %s\n", ch->ch_servicecode);
	  printf("\t statuschecker = %s\n", ch->ch_statuschecker);
     }

     endchanent();

     exit(0);
}
