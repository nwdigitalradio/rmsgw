#
#			M a k e f i l e
# $Revision: 105 $
# $Author: eckertb $
# $Id: Makefile 105 2009-04-25 08:02:50Z eckertb $
#
# RMS Gateway
#
# Copyright (c) 2004-2009 Hans-J. Barthen - DL5DI
# Copyright (c) 2008-2009 Brian R. Eckert - W3SG
#
# Questions or problems regarding this program can be emailed
# to linux-rmsgw@w3sg.org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

LEVEL := 1
include ../rmsgw.mk

RMSGWMONSRCS=	rmsgwmon.c glob.c
RMSGWMONOBJS=	$(RMSGWMONSRCS:.c=.o)
RMSGWMONPROG=	rmsgwmon

DEFINES		:= $(PKG_DEFINES)
INCLUDES	:= $(PKG_INCLUDES)
LIBDIRS		:= $(PKG_LIBDIRS)

LIBS		:= $(PKG_LIBS) -lncurses
CFLAGS		:= $(DEFINES) $(INCLUDES) $(PKG_CFLAGS)
LDFLAGS		:= $(LIBDIRS) $(PKG_LDFLAGS)

all: depend $(RMSGWMONPROG)

$(RMSGWMONPROG): $(RMSGWMONOBJS)
	$(CC) $(LDFLAGS) $(RMSGWMONOBJS) -o $@ $(LIBS)
	size $@

install: $(RMSGWMONPROG)
	install -v -m 711 -o $(INSTUSR) -g $(INSTGRP) $(RMSGWMONPROG) $(INSTDIR)/bin

clean:
	rm -f $(RMSGWMONOBJS) depend

clobber: clean
	rm -f $(RMSGWMONPROG)

depend:
	cat Makefile |  makedepend -f - -- $(CFLAGS) -- $(RMSGWMONSRCS) >$@ 2>/dev/null

.PHONY: all install clean clobber depend

##########
include depend
