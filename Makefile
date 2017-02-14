#
#			M a k e f i l e
# $Revision: 105 $
# $Author: eckertb $
# $Id: Makefile 105 2009-04-25 08:02:50Z eckertb $
#
# Description:
#	toplevel Makefile for RMS Gateway
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

include functions.mk
include rmsgw.mk

#
# sub-directories for recursive make (rmake make function)
#
subdirs = librms rmsgw rmsgw_aci rmsgwmon

all: init
	$(rmake)

init:
	@echo "Building Linux RMS Gateway for" $(ARCH) "platform..."

clean:
	$(rmake)

clobber:
	$(rmake)

depend:
	$(rmake)

install: all
	@./install.sh $(INSTUSR) $(INSTGRP) $(INSTDIR)

.PHONY: init all clean clobber depend install

rmsgw: lib
rmsgw_aci: lib
