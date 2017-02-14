/*
 *			a c i . h
 * $Revision: 171 $
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

#ifndef _aci_h
#define _aci_h	1

#ifndef lint
static char	_aci_h_svnid[] = "$Id: aci.h 171 2014-10-19 10:00:22Z eckertb $";
#endif /* lint */

#define DEFAULT_PYTHON_PATH	"/usr/bin/python"

extern int run_python_script(char* python_path, char *script_file);
extern int sendVersion(config *cfg);
extern int sendChannel(config *cfg, channel *chnl);

extern int wl2k_aci(config *cfg, rms_status *sp);

#endif /* _aci_h */
