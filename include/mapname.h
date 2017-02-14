/*
 *			m a p n a m e . h
 * $Revision: 59 $
 * $Author: eckertb $
 *
 * This module originally:
 * Copyright (c) 1989,1990,1991,1992,2008 by Brian R. Eckert
 *
 * Now incoporated into..
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
 */

#ifndef _mapname_h
#define _mapname_h	1

#ifndef lint
static char	_mapname_h_svnid[] = "$Id: mapname.h 59 2008-08-19 06:57:59Z eckertb $";
#endif /* lint */

#define ME_NOMAPDB	1	/* no MAPDB file defined in environment */
#define ME_MAPOPEN	2	/* can't open map db */
#define ME_NOMEM	3	/* storage allocator failure */
#define ME_ILLCOMMAND	4	/* illegal mapctl command */
#define ME_NOTFOUND	5	/* logical name not found */
#define NMAPERRS	5	/* number of map error codes */
extern int	map_errno;
extern int	nmap_err;

extern char *maperrmsg(int err);
extern int mapname(char **logical, char ***physical);
extern int fmapname(const char *mapfile, char **logical, char ***physical);

/*
 * mapname control command defines
 */
#define MCTL_SETNOTFNDERR	1
#define MCTL_CLRNOTFNDERR	2

extern int mapctl(int cmd);

#endif /* _mapname_H */
