/*
 *			c m s n o d e . c
 * $Revision: 51 $
 * $Author: eckertb $
 *
 * Linux RMS Gateway
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
 *	linked list management for list of CMS hosts
 */
#ifndef lint
static char svnid[] = "$Id: cmsnode.c 51 2008-07-10 03:17:06Z eckertb $";
#endif /* lint */

#include <stdlib.h>
#include <string.h>

#include "rms.h"
#include "rmslib.h"


/***
 *  addcmsnode(nodelist, cmsentry, statustime)
 *
 *  add cmsentry with status time to linked list nodelist
 *
 *  returns: head node of linked list
 */
cmsnode *addcmsnode(cmsnode *clist, cms *c, time_t stime)
{
     cmsnode *t, *p, *q;

     /*
      * allocate a new node for cms host info
      */
     if ((t = (cmsnode *) malloc(sizeof(cmsnode))) == NULL) {
	  /*
	   * could not allocate memory, so we'll return NULL
	   * here to signal to the caller it's time to bail
	   */
	  return(NULL);
     }

     /*
      * copy data into new node
      */
     t->next = NULL;
     t->cms_port = c->cms_port;
     t->cms_stat_time = stime;
     if ((t->cms_host = strdup(c->cms_host)) == NULL) {
	  return(NULL); /* can't alloc memory */
     }
     if ((t->cms_passwd = strdup(c->cms_passwd)) == NULL) {
	  return(NULL); /* can't alloc memory */
     }

     /*
      * add new node into list in stime order
      */

     /*
      * walk the list until we find the insertion point --
      * we want them in increasing order of the "status" time,
      * and after the last node with the same "status" time
      */
     for (q = NULL, p = clist;
	  p != NULL && t->cms_stat_time >= p->cms_stat_time;
	  p = p->next) {
	  q = p;
     }

     /*
      * we now have the insertion point for t in clist
      */
     if (q == NULL) {
	  /*
	   * goes to the head of the list
	   */
	  t->next = clist;
	  clist = t;
     } else {
	  /*
	   * insert after
	   */
	  t->next = q->next;
	  q->next = t;
     }

     return(clist);
}
