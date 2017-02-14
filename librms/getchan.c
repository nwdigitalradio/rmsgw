/*
 *			g e t c h a n . c
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
 *
 * Description:
 *
 * Group of functions ala getpwent() and friends which will read
 * channel information from a structured file into a local structure.
 *
 */
#ifndef lint
static char svnid[] = "$Id: getchan.c 149 2013-07-03 02:01:55Z eckertb $";
#endif


#include <stdio.h>
#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "rmslib.h"
#include "symbol.h"

#define BIGBUF	2048

static char *chanfile = XMLCHANNELFILE;
static xmlDocPtr doc = NULL;
static xmlNode *root = NULL;
static channel	curent; /* static data to return */
static xmlNode *cur_node = NULL;
static Symbol *chan_sym_tab = NULL; /* channel symbol table (for var subst) */
/* TODO: don't know if want to expose the symbol table itself and maybe
   eliminate the channel structure -- something to consider later */
		

/***
 *  setchanfile(filename)
 *
 *  set the name of the file we want to use
 *  for the channel file
 */
void setchanfile(char *p)
{
     chanfile = p;
}


/***
 *  setchanent()
 *
 *  start or restart the XML parser for the channel file
 *
 *  returns: 1 (true) on success, 0 on error (file open)
 */
int setchanent(void)
{
     /*
      * toss the current symbol table (if it exists) -- we are attempting
      * to restart the xml parsing so the current symbol tabel will not
      * be valid, regarless of success/failure of the remaining actions
      */
     symDestroy(chan_sym_tab);
     chan_sym_tab = NULL;

     /*
      * if currently have document, cleanup and restart
      */
     if (doc) {
	  /* TODO: determine if this creates a memory leak for
	   * elements of 'curent' that were previously allocated
	   * by the parser */
	  xmlFreeDoc(doc);
	  xmlCleanupParser();
	  doc = NULL;
	  cur_node = NULL;

     }

     /*
      * get the document and start the parser
      */
     if ((doc = xmlParseFile(chanfile)) == NULL) {
	  return(0);
     }

     root = xmlDocGetRootElement(doc);

     if (!root
     ||  !root->name
     ||  xmlStrcmp(root->name,(xmlChar *) "rmschannels")) {
	  xmlFreeDoc(doc);
	  doc = NULL;
	  cur_node = NULL;
	  return(0);
     }
     return(1);
}


/***
 *  nextent()
 *
 *  internal function to read/parse the next entry
 *  from the channels XML file
 *
 *  returns: 1 (true) if entry read, 0 otherwise
 */
static int nextent(void)
{
     xmlNode *child_node;
     char *content;
     char cbuf[BIGBUF];

     if (!doc && !setchanent()) {
	  return(0);
     }

     /*
      * if first time here, point to first child of root,
      * otherwise, move to the next child of the root
      */
     if (cur_node == NULL) {
	  cur_node = root->children;
     } else {
	  cur_node = cur_node->next;
     }

     /*
      * throw away current symbol table (may be empty if first time here)
      * so that we get a fresh one for each channel entry -- presently
      * the only purpose of the symbol table is to (in my view) make
      * variable substitution easier at the end
      */
     symDestroy(chan_sym_tab);
     chan_sym_tab = NULL;

     /*
      * walk tree until we find the next channel node
      */
     for (; cur_node != NULL; cur_node = cur_node->next) {
	  if (cur_node->type == XML_ELEMENT_NODE
	  &&  !xmlStrcmp(cur_node->name, (const xmlChar *) "channel")) {
	       break;
	  }
     }

     /*
      * find a channel node?
      */
     if (cur_node == NULL) {
	  return(0); /* no */
     }

     if (curent.ch_name) {
	  xmlFree(curent.ch_name);
     }
     if ((curent.ch_name = xmlGetProp(cur_node,(xmlChar *) "name"))) {
	  symAdd(&chan_sym_tab, "name", curent.ch_name); /* errors? if so, we got
							    really big problems
							    anyway */
     }

     if (curent.ch_type) {
	  xmlFree(curent.ch_type);
     }
     if ((curent.ch_type = xmlGetProp(cur_node,(xmlChar *) "type"))) {
	  symAdd(&chan_sym_tab, "type", curent.ch_type);
     }

     if (curent.ch_active) {
	  xmlFree(curent.ch_active);
     }
     if ((curent.ch_active = xmlGetProp(cur_node,(xmlChar *) "active"))) {
	  symAdd(&chan_sym_tab, "active", curent.ch_active);
     }

     /*
      * child nodes provide the remaining details of the channel definition
      * -- walk the list of children of the channel node
      */
     for (child_node = cur_node->children;
	  child_node != NULL;
	  child_node = child_node->next) {

	  if (child_node->type == XML_ELEMENT_NODE) {
	       /*
		* get the content for this node and then trim leading/trailing
		* whitespace
		*/
	       content = (char *) xmlNodeGetContent(child_node);
	       strtrim(cbuf, content, " \t\n", 0);
	       xmlFree(content);

	       /*
		* now duplicate the remaining string in the buffer into
		* content for use below
		*/
	       content = strdup(cbuf);

	       /*
		* add whatever we got to the symbol table with the symbol
		* being the element name and the value being the content.
		*
		* doing this *may* add some "noise" to the table, but it is
		* a small matter, since we expect the channel xml file to be
		* well-formed and valid anyway
		*/
	       symAdd(&chan_sym_tab, (char *) child_node->name, content);

	       /*
		* now update the channel structure depending on the
		* element we have
		*/
	       if (!xmlStrcmp(child_node->name,
			      (const xmlChar *)"basecall")) {
		    if (curent.ch_basecall) {
			 free(curent.ch_basecall);
		    }
		    curent.ch_basecall = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"callsign")) {
		    if (curent.ch_callsign) {
			 free(curent.ch_callsign);
		    }
		    curent.ch_callsign = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"password")) {
		    if (curent.ch_password) {
			 free(curent.ch_password);
		    }
		    curent.ch_password = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"gridsquare")) {
		    if (curent.ch_gridsquare) {
			 free(curent.ch_gridsquare);
		    }
		    curent.ch_gridsquare = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"frequency")) {
		    if (curent.ch_frequency) {
			 free(curent.ch_frequency);
		    }
		    curent.ch_frequency = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"mode")) {
		    if (curent.ch_mode) {
			 free(curent.ch_mode);
		    }
		    curent.ch_mode = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"autoonly")) {
		    if (curent.ch_autoonly) {
			 free(curent.ch_autoonly);
		    }
		    curent.ch_autoonly = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"baud")) {
		    if (curent.ch_baud) {
			 free(curent.ch_baud);
		    }
		    curent.ch_baud = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"power")) {
		    if (curent.ch_power) {
			 free(curent.ch_power);
		    }
		    curent.ch_power = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"height")) {
		    if (curent.ch_height) {
			 free(curent.ch_height);
		    }
		    curent.ch_height = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"gain")) {
		    if (curent.ch_gain) {
			 free(curent.ch_gain);
		    }
		    curent.ch_gain = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"direction")) {
		    if (curent.ch_direction) {
			 free(curent.ch_direction);
		    }
		    curent.ch_direction = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"hours")) {
		    if (curent.ch_hours) {
			 free(curent.ch_hours);
		    }
		    curent.ch_hours = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"groupreference")) {
		    if (curent.ch_groupreference) {
			 free(curent.ch_groupreference);
		    }
		    curent.ch_groupreference = content;
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"servicecode")) {
		    if (curent.ch_servicecode) {
			 free(curent.ch_servicecode);
		    }
		    curent.ch_servicecode = content;

		    /*
		     * limit length of service code per specification
		     */
		    if (strlen(curent.ch_servicecode) > MAXSERVICECODE) {
			 curent.ch_servicecode[MAXSERVICECODE] = '\0';
		    }
	       } else if (!xmlStrcmp(child_node->name,
				     (const xmlChar *)"statuschecker")) {
		    if (curent.ch_statuschecker) {
			 free(curent.ch_statuschecker);
		    }
		    curent.ch_statuschecker = content;
	       } else {
		    /*
		     * didn't match any expected element -- should not happen
		     * but safe to ignore
		     *
		     * the element's content is not meaningful (and the
		     * symbol table has its own copy of the content) so
		     * we will now just free it, otherwise we will introduce
		     * a memory leak here
		     */
		    free(content);
	       }
	  }
     }

     /* TODO: can't guarantee that the data are actually complete here;
	more checking should be done */
     /*
      * at this point we expect to have a complete set of data for
      * the channel, so now we will perform any substitutions for the
      * status checker
      */
     varsubst(chan_sym_tab, cbuf, curent.ch_statuschecker);
     free(curent.ch_statuschecker); /* out with the old... */
     curent.ch_statuschecker = strdup(cbuf); /* ... in with the new */

     return(1);
}


/***
 *  endchanent()
 *
 *  cleanup and close open files
 */
void endchanent(void)
{
     if (doc) {
	  /* TODO: determine if this creates a memory leak for
	   * elements of curent that were previously allocated
	   * by the parser */
	  xmlFreeDoc(doc);
	  xmlCleanupParser();
	  doc = NULL;
	  cur_node = NULL;
     }
}


/***
 *  getxmlchanent()
 *
 *  on each call, get the next channel entry from the XML tree
 *  and place in channel structure and return a pointer
 *  to that structure.
 *
 *  Caveat: be careful of aliasing the returned pointer, since
 *  the contents will change on successive calls, therefore you
 *  will need to duplicate the structure and its fields if you
 *  expect to use the result later after more calls to getchanent().
 *
 *  returns: pointer to current chan entry structure, or NULL on
 *		EOF or error.
 */
channel *getchanent(void)
{
     /*
      * open the file if necessary
      */
     if (!doc && !setchanent()) {
	  return(NULL); /* that didn't work! */
     }

     /*
      * get the next entry from the channel table
      */
     if (!nextent()) {
	  return(NULL); /* no more */
     }

     return(&curent); /* entry just retrieved */
}


/***
 *  getchannam(channel name, callsign)
 *
 *  search for channelname/callsign pair in XML doc
 *
 * returns: pointer to matching entry, NULL if not found or error
 */
channel *getchannam(register char *chname, register char *callsign)
{

     /*
      * open the file or position to the beginning
      */
     if (!setchanent()) {
	  return(NULL); /* couldn't do it */
     }

     /*
      * spin through the entries looking for a name match
      */
     while (nextent()) {
	  if (!strncmp(curent.ch_name, chname, strlen(chname))
	  &&  !strncasecmp(curent.ch_callsign, callsign, strlen(callsign))) {
	       return(&curent); /* found it! */
	  }
     }

     return(NULL); /* no match */
}
