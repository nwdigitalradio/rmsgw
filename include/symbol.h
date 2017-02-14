/*
 *			s y m b o l . h
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
 */
#ifndef _symbol_h
#define _symbol_h	1

#ifndef lint
static char _symbol_h_svnid[] = "$Id: symbol.h 56 2008-08-06 10:40:10Z eckertb $";
#endif /* lint */

/*
 * symbol table entry structure
 */
typedef struct Symbol {
     char *sym_name; /* the name of the symbol */
     char *sym_value; /* the symbol's value */
     struct Symbol *sym_next; /* next symbol in table */
} Symbol;

extern Symbol *symLookup(Symbol *symtab, char *s);
extern Symbol *symAdd(Symbol **symtab, char *s, char *v);
extern int symDestroy(Symbol *symtab);

#endif /* _symbol_h */
