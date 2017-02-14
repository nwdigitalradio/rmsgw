#
#			M a k e f i l e
# $Revision: 56 $
# $Author: eckertb $
# $Id: Makefile 56 2008-08-06 10:40:10Z eckertb $
#
# RMS Gateway
#
# Copyright (c) 2004-2008 Hans-J. Barthen - DL5DI
# Copyright (c) 2008 Brian R. Eckert - W3SG
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

RMSPROG		:= rmsgw
RMSSRCS		:= rmsgw.c gwinit.c greet.c banner.c session.c gateway.c \
		   log_login.c  log_logout.c glob.c
RMSOBJS		:= $(RMSSRCS:.c=.o)

DEFINES		:= $(PKG_DEFINES)
INCLUDES	:= $(PKG_INCLUDES)
LIBDIRS		:= $(PKG_LIBDIRS)

LIBS		:= $(PKG_LIBS)
CFLAGS		:= $(DEFINES) $(INCLUDES) $(PKG_CFLAGS)
LDFLAGS		:= $(LIBDIRS) $(PKG_LDFLAGS)

all: depend $(RMSPROG)

$(RMSPROG): $(RMSOBJS)
	$(CC) $(LDFLAGS) $(RMSOBJS) -o $@ $(LIBS)
	size $@

install: $(RMSPROG)
	install -v -m 711 -o $(INSTUSR) -g $(INSTGRP) $(RMSPROG) $(INSTDIR)/bin

clean:
	rm -f $(RMSOBJS) depend

clobber: clean
	rm -f $(RMSPROG)

depend:
	cat Makefile |  makedepend -f - -- $(CFLAGS) -- $(RMSSRCS) >$@ 2>/dev/null

.PHONY: all install clean clobber depend

##########
include depend
