#
#			M a k e f i l e
# $Revision: 176 $
# $Author: eckertb $
# $Id: Makefile 176 2014-10-27 09:07:54Z eckertb $
#
# RMS Gateway
#
# Copyright (c) 2004-2014 Hans-J. Barthen - DL5DI
# Copyright (c) 2008-2014 Brian R. Eckert - W3SG
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

RMSLIB=		$(PKG_LIBDIR)/librms.a
RMSLIBSRCS=	fileage.c syslogmap.c cmsconnect.c loadconfig.c \
		getcms.c getcmslist.c cmsnode.c statfile.c setcmsstat.c \
		cmslogin.c upcase.c downcase.c strcvt.c fgetline.c \
		fgetlinecr.c err.c mapname.c sendrf.c printfile.c \
		getchan.c set.c symbol.c var.c strtrim.c senddatagram.c \
		shm.c statblock.c md5.c readln.c sglchallenge.c \
		loadversion.c loadenv.c file_exists.c runhook.c
RMSLIBOBJS=	$(RMSLIBSRCS:.c=.o)

DEFINES		:= $(PKG_DEFINES)
INCLUDES	:= $(PKG_INCLUDES)
LIBDIRS		:= $(PKG_LIBDIRS)

CFLAGS		:= $(DEFINES) $(INCLUDES) $(PKG_CFLAGS)

LIBS		:= $(PKG_LIBS)
LDFLAGS		:= $(LIBDIRS) $(PKG_LDFLAGS)

all: depend libs

libs: $(RMSLIB)

$(RMSLIB): $(RMSLIBOBJS)
	$(AR) $(ARFLAGS) $@ $(RMSLIBOBJS)

maptest: all maptest.c
	$(CC) $(LDFLAGS) $(RMSOBJS) -o $@ maptest.o $(LIBS)

clean:
	rm -f $(RMSLIBOBJS) depend

clobber: clean
	rm -f $ $(RMSLIB)

depend:
	cat Makefile |  makedepend -f - -- $(CFLAGS) -- $(RMSLIBSRCS) >$@ 2>/dev/null

.PHONY: all clean clobber install

##########
include depend
