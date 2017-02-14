#
#			f u n c t i o n s . m k
# $Revision$
# $Author$
# $Id$
#
# Description:
#	GNU Make functions used for builing the gateway
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

###
#  rmake - macro for recursive make
#
#  uses (and requires) a 'subdirs' variable in the referencing Makefile
#  to identify the directories to be visited for the target
#
define rmake
+for dir in $(subdirs); do \
	make -C $$dir $@ ; \
done
endef
