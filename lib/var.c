/*
 *			v a r . c
 * $Revision: 56 $
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
 *	Functions to handle dynamic variable through a symbol table
 *
 */
#ifndef lint
static char svnid[] = "$Id: var.c 56 2008-08-06 10:40:10Z eckertb $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <memory.h>

#include "set.h"
#include "rmslib.h"
#include "symbol.h"
#include "var.h"


/***
 *  _varassign(symbol table, symbol, value)
 *
 *  lower half of varassign() function below
 *  (symbol name and value already broken apart) -- add or replace variable
 *  in symbol table
 *
 *  sp = pointer to symbol name
 *  vp = pointer to symbol's value string
 *
 *  returns: 0 on success (with symbol table updated) and -1 on error
 */
int _varassign(Symbol **symtab, char *sp, char *vp)
{
     register Symbol *symp; /* pointer to symbol table entry */
     char *whitespace = " \t";

     /*
      * strip off leading and trailing spaces
      * from symbol and value strings
      */
     strtrim(sp, sp, whitespace, 0);
     strtrim(vp, vp, whitespace, 0);

     /*
      * see if symbol exists:
      * replace it if it does,
      * add it if it doesn't
      */
     if ((symp = symLookup(*symtab, sp)) == NULL) {
	  /*
	   * new symbol, install it
	   */
	  if ((symp = symAdd(symtab, sp, vp)) == NULL) {
	       return(-1);
	  }
     } else {
	  /*
	   * symbol exists, replace it
	   */
	  free(symp->sym_value);
	  symp->sym_value = strdup(vp);
     }

     return(0);
}


/***
 *  varassign(symbol table, assignment string)
 *
 *  handle variable assignment (assignment string in form of var=value)
 *
 *  abuf = buffer holding assignment string
 *
 *  returns: current symbol table with status value 0 on success, -1 on error
 */
int varassign(Symbol **symtab, char *abuf)
{
     register char *sp = abuf; /* pointer to symbol name */
     register char *vp; /* pointer to symbol value */

     /*
      * locate the equal sign and split the string there
      */
     if ((vp = strrchr(abuf, '=')) == NULL) {
	  vp = "\0";
     } else {
	  *vp++ = '\0';
     }

     return(_varassign(symtab, sp, vp));
}


/***
 *  varsubst(symbol table, copy buffer, raw buffer)
 *
 *  perform variable substitution for a string using a symbol table
 *
 *  symtab = pointer to 
 *  cbuf = pointer to buffer to hold string after substitution is finished
 *  rbuf = pointer to buffer holding string on which substitution is to be done
 *
 *  returns: 0 on success, -1 on error
 */
int varsubst(Symbol *symtab, char *cbuf, char *rbuf)
{
     char vbuf[256];	/* variable name buffer */
     register Symbol *sp; /* pointer to symbol table entry */
     register char *s, *d, *v;
     int esc = 0, var = 0;

     /*
      * copy string while performing substitution
      */
     for (s = rbuf, d = cbuf; ; s++) {
	  if (esc) { /* char escaped, treat it literally */
	       *d++ = *s;
	       esc = 0; /* end escaped char "mode" */
	  } else if (var) { /* collecting a variable name */
	       if (isalnum(*s) || *s == '_' || *s == '$') {
		    *v++ = *s; /* copy allowed chars for var name */
	       } else {
		    *v = '\0'; /* end var and go find its value */
		    if ((sp = symLookup(symtab, vbuf)) != NULL) {
			 strcpy(d, sp->sym_value);
			 d += strlen(sp->sym_value);
		    }
		    var = 0;

		    /*
		     * most copy char that that triggered end of the
		     * variable name to the destination string
		     */
		    *d++ = *s;
	       }
	  } else {
	       switch (*s) {
	       case '\\': /* escape char */
		    esc = 1;
		    break;
	       case '$': /* variable */
		    var = 1;
		    v = vbuf;
		    break;
	       default: /* copy through */
		    *d++ = *s;
		    break;
	       }
	  }

	  if (*s == '\0') { /* end of string? */
	       break; /* all done */
	  }
     }

     return(0);
}
