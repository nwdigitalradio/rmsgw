#
#			M a k e f i l e
# $Revision: 167 $
# $Author: eckertb $
# $Id: Makefile 167 2014-09-30 10:27:26Z eckertb $
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

RMSACISRCS=	rmsgw_aci.c run_python_script.c wl2k_aci.c sendversion.c \
		sendchannel.c glob.c
RMSACIOBJS=	$(RMSACISRCS:.c=.o)
RMSACIPROG=	rmsgw_aci

DEFINES		:= $(PKG_DEFINES)
INCLUDES	:= $(PKG_INCLUDES)
LIBDIRS		:= $(PKG_LIBDIRS)

LIBS		:= $(PKG_LIBS)
CFLAGS		:= $(DEFINES) $(INCLUDES) $(PKG_CFLAGS)
LDFLAGS		:= $(LIBDIRS) $(PKG_LDFLAGS)

all: depend $(RMSACIPROG)

$(RMSACIPROG): $(RMSACIOBJS)
	$(CC) $(LDFLAGS) $(RMSACIOBJS) -o $@ $(LIBS)
	size $@

install: $(RMSACIPROG)
	install -v -m 711 -o $(INSTUSR) -g $(INSTGRP) $(RMSACIPROG) $(INSTDIR)/bin

clean:
	rm -f $(RMSACIOBJS) depend

clobber: clean
	rm -f $(RMSACIPROG)

depend:
	cat Makefile |  makedepend -f - -- $(CFLAGS) -- $(RMSACISRCS) >$@ 2>/dev/null

.PHONY: all install clean clobber depend

##########
include depend
