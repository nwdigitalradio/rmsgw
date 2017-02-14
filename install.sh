#!/bin/sh
#			i n s t a l l . s h
# $Revision: 181 $
# $Author: eckertb $
# $Id: install.sh 181 2014-10-30 11:34:46Z eckertb $
#
# Description:
#	RMS Gateway basic installation script
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
#  install_man()
#
#  install the gateway manpages
#
install_man() {    
    echo "Installing man pages..."
    for mansect in 1 2 3 4 5 6 7 8 9; do
	MANSUBDIR=man${mansect}
	for manpage in man/*.${mansect}; do
	    if [ -f "$manpage" ]; then
		mkdir -v -p $MANDIR/$MANSUBDIR
		install -v -m 644 -o $GWOWNER -g $GWGROUP \
		    $manpage $MANDIR/$MANSUBDIR
	    fi
	done
    done
}


###
#  install_config()
#
#  install the configuration and support files for the gateway itself
#  (this does not address the needed AX.25 configuration to make the
#   gateway operational)
#
install_config() {
    # clear out any existing status directory and re-establish it
    # so the new reports go out as soon as possible follwing an upgrade
    if [ "$3" != "" ]; then
	rm -rf $3/stat >/dev/null 2>&1
    fi

    echo "Installing configuration and support files"
    echo "\t--existing config files will be backed up before being replaced..."
    install -d -v -m 775 -o $1 -g $2 $3
    install -d -v -m 775 -o $1 -g $2 $3/stat
    install -d -v -m 755 -o $1 -g $2 $3/hooks

    for file in etc/gateway.conf etc/channels.xml etc/banner; do
	basefile=$(basename $file)
	if [ -f "$3/$basefile" ]; then
	    echo -n "Replace file $3/$basefile [y/N]? "
	    read ans
	    case $ans in
		[Yy]|[Yy][Ee][Ss])
		    ;;
		*)
		    continue
		    ;;
	    esac
	fi
	install --backup=numbered -v -m 664 -o $1 -g $2 $file $3
    done

    for file in etc/channels.xsd etc/hosts; do
	basefile=$(basename $file)
	if [ -f "$3/$basefile" ]; then
	    echo -n "Replace file $3/$basefile [Y/n]? "
	    read ans
	    case $ans in
		[Nn]|[Nn][Oo])
		    continue
		    ;;
		*)
		    ;;
	    esac
	fi
	install --backup=numbered -v -m 664 -o $1 -g $2 $file $3
    done

    #
    # install hook scripts
    #
    for file in etc/hooks/*; do
	basefile=$(basename $file)
	if [ -f "$3/hooks/$basefile" ]; then
	    echo -n "Replace file $3/hooks/$basefile [y/N]? "
	    read ans
	    case $ans in
		[Yy]|[Yy][Ee][Ss])
		    ;;
		*)
		    continue
		    ;;
	    esac
	fi
	install --backup=numbered -v -m 664 -o $1 -g $2 $file $3/hooks
    done

    #
    # prune any "dead" configuration options from gateway.conf
    #
    if [ -f "$3/gateway.conf" ]; then
	CONF="$3/gateway.conf"
	CONFTMP="/tmp/gateway.conf.$$"

	cat $CONF | awk '
BEGIN { needpython = 1 }
$0 ~ /^AUTHMODE=/ { next }
$0 ~ /^UDPMSGHOST=/ { next }
$0 ~ /^UDPMSGPORT=/ { next }
$0 ~ /^GWPASSWD=/ { next }
$0 ~ /^PYTHON=/ { needpython = 0 }
    { print }
END {
	if (needpython) {
	    print "PYTHON=/usr/bin/python"
	}
    }' >$CONFTMP

	#
	# if the config has been changed, install
	# the updated version
	#
	diff $CONFTMP $CONF >/dev/null 2>&1
	if [ $? -ne 0 ]; then
	    echo "--> Cleaning up $CONF..."
	    install --backup=numbered -v -m 644 -o $1 -g $2 $CONFTMP $CONF
	fi
	rm -f $CONFTMP
    fi

    #
    # special handling for channels.xml to update if it has the
    # old DTD in it -- want to replace with the xml schema that
    # was added in release 2.4.0
    #
    if [ -f "$3/channels.xml" ]; then
	CONF="$3/channels.xml"
	CONFTMP="/tmp/channels.xml.$$"

	# if we have the DOCTYPE in the file, then it needs to
	# be updated to ref the schema
	grep "^<!DOCTYPE" $CONF >/dev/null 2>&1
	if [ $? -eq 0 ]; then
	    echo "--> Upgrading $CONF to use xml schema..."

	    #
	    # the awk script below replaces the DTD with the schema reference
	    #
	    cat $CONF | awk '
$0 == "<!DOCTYPE rmschannels [" { filtering = 1; next }
$0 == "<rmschannels>" {
    filtering = 0
    print "<rmschannels xmlns=\"http://www.namespace.org\"\n  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n  xsi:schemaLocation=\"file:///etc/rmsgw/channels.xsd\">"
    next
}
!filtering { print } ' >$CONFTMP

	    install --backup=numbered -v -m 644 -o $1 -g $2 $CONFTMP $CONF
	    rm -f $CONFTMP
	fi
    fi

    for file in etc/acihelp etc/gwhelp; do
	basefile=$(basename $file)
	if [ -f "$3/$basefile" ]; then
	    echo -n "Replace file $3/$basefile [Y/n]? "
	    read ans
	    case $ans in
		[Nn]|[Nn][Oo])
		    continue
		    ;;
		*)
		    ;;
	    esac
	fi
	install --backup=numbered -v -m 664 -o $1 -g $2 $file $3
    done

    # remove defunct rms.auth file if present
    if [ -f $3/rms.auth ]; then
	rm -f $3/rms.auth
    fi
}


###
#  install_progs
#
#  install the gateway programs and scripts
#
install_progs() {
    echo "Installing gateway programs..."
    mkdir -v -p $3
    for prog in rmsgw/rmsgw rmsgw_aci/rmsgw_aci rmschanstat/rmschanstat rmsgwmon/rmsgwmon ; do
	install --backup=numbered -v -m 755 \
	    -o $1 -g $2 $prog $3
    done
}


##
## start the actual installation
##
echo "***** Installing RMS Gateway *****"

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
# install the man pages
#
install_man $GWOWNER $GWGROUP

#
# install gateway configuration and support files
#
install_config $GWOWNER $GWGROUP $GWDIR

#
# install programs
#
install_progs $GWOWNER $GWGROUP $BINDIR

#
# run the admin tool install script
#
(cd admin; ./admin-install.sh)

exit 0
