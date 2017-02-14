#!/bin/sh
#			a d m i n - i n s t a l l . s h
# $Revision: 174 $
# $Author: eckertb $
# $Id: admin-install.sh 174 2014-10-19 10:18:31Z eckertb $
#
# Description:
#	RMS Gateway administration tools install
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

##
## fixed installation information
##
ETCDIR=/etc
GWDIR=$ETCDIR/rmsgw

##
## variable installation information (calculated/derived)
##

#
# get the install user and group (will be 1st and 2nd arg)
# -- if missing, default to rmsgw
#
GWOWNER=${1:-rmsgw}
GWGROUP=${2:-rmsgw}

#
# get directory prefix, default to /usr/local
# then set other directory locations
#
PREFIX=${3:-/usr/local}
BINDIR=${PREFIX}/bin
MANDIR=${PREFIX}/man


##
## shell functions for the installation steps
##

###
#  check_user()
#
#  ensure that this script is running as indicated user
#
check_user() {
    #
    # get the current username (based on effective uid)
    #
    USERID=`id | sed -e "s/uid=[0-9]*(\([a-zA-Z][a-zA-Z0-9]*\)).*/\1/"`

    if [ "$USERID" != "$1" ]; then
	echo "You must be '$1' to perform installation!"
	exit 1
    fi
}


###
#  setup_group()
#
#  create the group for the gateway if it doesn't exist
#
setup_group() {
    grep "^${1}:" /etc/group >/dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo "Creating group $1..."
	groupadd $1
    fi
}


###
#  setup_user()
#
#  create the gateway user if it doesn't exist and lock the account
#
setup_user() {
    grep "^${1}:" /etc/passwd >/dev/null 2>&1

    if [ $? -ne 0 ]; then
	#
	# create the account
	#
	echo "Creating user $1..."
	useradd -s /bin/false -g $2 $1
    fi

    #
    # lock the account to prevent a potential hole, unless the
    # owner is root
    #
    if [ "$GWOWNER" != root ]; then
	echo "Locking user account $GWOWNER..."
	passwd -l $GWOWNER >/dev/null
	# while the account is locked, make the password to
	# never expire so that cron will be happy
	chage -E-1 $GWOWNER >/dev/null
    fi
}


###
#  install_config()
#
#  install the configuration and support files for the admin tools
#
install_config() {
    echo "Installing configuration and support files..."

    for file in .version_info winlinkservice.xml sysop-template.xml; do
	install --backup=numbered -v -m 660 -o $1 -g $2 $file $3
    done

    for file in .version_info; do
	install --backup=numbered -v -m 664 -o $1 -g $2 $file $3
    done
}


###
#  install_tools
#
#  install the admin tools
#
install_tools() {
    echo "Installing the admin tools..."
    for prog in mksysop.py getsysop.py updatesysop.py updatechannel.py updateversion.py ; do
	install --backup=numbered -v -m 755 \
	    -o $1 -g $2 $prog $3
    done
}


##
## start the actual installation
##
echo "***** Installing RMS Gateway Administation Tools and Files *****"

#
# make sure we have the priviledges to do
# the install
check_user root

#
# create gateway group if necessary
#
setup_group $GWGROUP

#
# create gateway user if necessary
#
setup_user $GWOWNER $GWGROUP

#
# install configuration files
#
install_config $GWOWNER $GWGROUP $GWDIR

#
# install programs
#
install_tools $GWOWNER $GWGROUP $GWDIR

exit 0
