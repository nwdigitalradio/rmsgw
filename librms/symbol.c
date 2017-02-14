/*
 *			s y m b o l . c
 * $Revision: 64 $
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
 *	Functions to provide a basic symbol table capability
 *
 */
#ifndef lint
static char svnid[] = "$Id: symbol.c 64 2008-08-19 07:46:15Z eckertb $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <memory.h>

#include "symbol.h"


/***
 *  symLookup()
 *
 *  search for a symbol in the symbol table
 *
 *  s = symbol to search for
 *
 *  returns: pointer to sybol table entry or
 *	     NULL if not found
 */
Symbol *symLookup(Symbol *symtab, char *s)
{
     register Symbol *sp;

     for (sp = symtab; sp != NULL; sp = sp->sym_next) {
	  if (!strcmp(sp->sym_name, s)) {
	       return(sp); /* found it! */
	  }
     }

     return(NULL);	/* not found */
}


/***
 *  symAdd()
 *
 *  add a symbol in the symbol table
 *
 *  returns: pointer to installed symbol (with symtab pointer updated) or
 *	     NULL on error
 */
Symbol *symAdd(Symbol **symtab, char *s, char *v)
{
     Symbol *sp;

     /*
      * give up if there is no symbol name
      */
     if (s == NULL || strlen(s) < 1) {
       return(NULL);
     }

     /*
      * allocate new node for symbol
      */
     if ((sp = (Symbol *) malloc(sizeof(Symbol))) == NULL) {
	  return(NULL);
     }

     /*
      * store symbol info and add to table
      */
     sp->sym_name = strdup(s);
     sp->sym_value = v != NULL ? strdup(v) : strdup("");
     sp->sym_next = *symtab; /* put at front of symbol table */
     *symtab = sp; /* newly added symbol is now the start of table */
     return(sp);
}


/***
 *  symDestroy()
 *
 *  Destroy (free) a symbol table
 *
 *  returns: 0 on success, -1 on error
 */
int symDestroy(Symbol *symtab)
{
     register Symbol	*cp, *lp;

     /*
      * free each node of the symbol table until empty
      */
     for (cp = symtab; cp != NULL; ) {
	  lp = cp; /* save current */
	  cp = cp->sym_next; /* current pointer */
	  free(lp->sym_name); /* free last... */
	  free(lp->sym_value);
	  free(lp);
     }

     return(0);
}
