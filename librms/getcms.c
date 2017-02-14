/*
 *			g e t c m s . c
 * $Revision: 167 $
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
 *
 * Group of functions ala getpwent() and friends which will read
 * CMS host information from a structured file into a
 * local structure.
 *
 */
#ifndef lint
static char	svnid[] = "$Id: getcms.c 167 2014-09-30 10:27:26Z eckertb $";
#endif

#include <stdio.h>
#include <string.h>

#include "rmslib.h"

#define BUFSIZE	2048	/* size of buffer for reading cms host file */

static char	*cmsfile = CMSHOSTFILE;
static FILE	*cmsfp = NULL;
static cms	curent; /* static data to return */
static char	cbuf[BUFSIZE]; /* buffer to hold line from file for parsing */
		

/***
 *  setcmsfile(filename)
 *
 *  set the name of the file we want to use
 *  for the cms host table
 */
void setcmsfile(char *p)
{
     cmsfile = p;
}


/***
 *  setcmsent()
 *
 *  open the host table file if not already open,
 *  or reposition to the start of the file
 *
 *  returns: 1 (true) on success, 0 on error (file open)
 */
int setcmsent(void)
{
     if (cmsfp) {
	  rewind(cmsfp);
     } else if ((cmsfp = fopen(cmsfile, "r")) == NULL) {
	  return(0);
     }
     return(1);
}


/***
 *  nextent()
 *
 *  internal function to read/parse the next entry
 *  from the cms host table
 *
 *  returns: 1 (true) if entry read, 0 otherwise
 */
static int nextent(void)
{
     register char	*p;

     if (!cmsfp && !setcmsent()) {
	  return(0);
     }

     while (fgets(cbuf, sizeof(cbuf), cmsfp) != NULL) {
	  /*
	   * lines beginning with a '#' are comments,
	   * skip them
	   */
	  if (cbuf[0] == '#') {
	       continue;
	  }

	  /*
	   * ignore leading whitespace
	   */
	  for (p = cbuf; (p - cbuf) <= sizeof(cbuf) && isspace(*p); p++) {
	       ;
	  }

	  if ((p - cbuf) >= sizeof(cbuf) || *p == '\0' || *p == '\n') {
	       continue; /* completely empty or excessively long line */
	  }

	  /*
	   * find end of first field
	   */
	  while (*p && *p != ':') {
	       p++;
	  }

	  /*
	   * first field is the cms host name
	   */
	  curent.cms_host = cbuf;
	  *p++ = '\0';

	  /*
	   * second field is the TCP port for connecting
	   */
	  curent.cms_port = atoi(p);

	  for (; *p && *p != ':'; p++) {
	       ;
	  }
	  *p++ = '\0';

	  /*
	   * last field is the host password
	   */
	  curent.cms_passwd = p;
	  for (; *p && *p != ':' && *p != '\n'; p++) {
	       ;
	  }
	  *p++ = '\0';
	  return(1);
     }
     return(0);
}


/***
 *  endcmsent()
 *
 *  cleanup and close open files
 */
void endcmsent(void)
{
     if (cmsfp) {
	  fclose(cmsfp);
	  cmsfp = NULL;
     }
}


/***
 *  getcmsent()
 *
 *  on each call, read the next line from the host file,
 *  parse it into the cms structure and return a pointer
 *  to that structure.
 *
 *  Caveat: be careful of aliasing the returned pointer, since
 *  the contents will change on successive calls, therefore you
 *  will need to duplicate the structure and its fields if you
 *  expect to use the result later after more calls to getcmsent().
 *
 *  returns: pointer to current cms host entry structure, or NULL on
 *		EOF or error.
 */
cms *getcmsent(void)
{
     /*
      * open the file if necessary
      */
     if (!cmsfp && !setcmsent()) {
	  return(NULL); /* that didn't work! */
     }

     /*
      * get the next entry from the cms host table
      */
     if (!nextent()) {
	  return(NULL); /* no more */
     }

     return(&curent); /* entry just retrieved */
}


/***
 *  getcmsnam(hostname)
 *
 *  search cms host table for hostname
 *
 * returns: pointer to matching entry, NULL if not found or error
 */
cms *getcmsnam(register char *name)
{

     /*
      * open the file or position to the beginning
      */
     if (!setcmsent()) {
	  return(NULL); /* couldn't do it */
     }

     /*
      * spin through the entries looking for a name match
      */
     while (nextent()) {
	  if (!strncmp(curent.cms_host, name, strlen(name))) {
	       return(&curent); /* found it! */
	  }
     }

     return(NULL); /* no match */
}


/***
 *  putcmsent(cms entry, output file)
 *
 *  place an CMS entry in the file associated with the
 *  output file pointer
 */
void putcmsent(register cms *c, register FILE *ofp)
{
	fprintf(ofp, "%s:%d:%s\n",
		c->cms_host,
		c->cms_port,
		c->cms_passwd);
	fflush(ofp);
}
