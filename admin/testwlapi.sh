#!/bin/bash

# Lowest python version required to run the admin scripts
# This version ships with Debian jessie
lowest_pyver="2.7.9"

rmsgw_dir="/etc/rmsgw"
debuglog_dir="/root/tmp"
debuglog_file="debuglog.txt"

# Function ver_lte, compare version numbers
function ver_lte() {
    [  "$1" = "`echo -e "$1\n$2" | sort -V | head -n1`" ]
}

# make sure we're running as root
if [[ $EUID != 0 ]] ; then
   echo "Must be root"
   exit 1
fi

if [ ! -d "$debuglog_dir" ] ; then
    echo "Directory $debuglog_dir does not exist ... creating"
    mkdir -p "$debuglog_dir"
fi

{
lsb_release -a
echo

current_pyver="$(python --version 2>&1)"
echo "Debug: pyver: $current_pyver"
if [ -z "$current_pyver" ] ; then
    echo "No python version string found."
else
    echo "Check Python ver #, current: $current_pyver, lowest: $lowest_pyver"
    ver_lte $current_pyver $lowest_pyver
    if [ "$?" -eq 0 ] ; then
        echo "Version check($?) less than required, current ver: $current_pyver"
    else
        echo "Version check($?) OK, current ver: $current_pyver"
    fi
fi

cd /etc/rmsgw

echo
echo "===== getchan ====="
sudo -u rmsgw ./getchannel.py -d
echo
echo "===== updateversion ====="
sudo -u rmsgw ./updateversion.py -d; echo $?
echo
echo "===== updatechannel ====="
sudo -u rmsgw ./updatechannel.py -d; echo $?
echo
echo "===== updatesysop ====="
sudo -u rmsgw ./updatesysop.py -d
echo
echo "===== rms.debug log ====="
tail -n 50 /var/log/rms.debug

echo
echo "===== channels.xml ====="
cat /etc/rmsgw/channels.xml
echo
echo "===== winlinkservice.xml ====="
cat /etc/rmsgw/winlinkservice.xml
echo
echo "===== sysop.xml ====="
cat /etc/rmsgw/sysop.xml
} > $debuglog_dir/$debuglog_file 2>&1

# get rid of all passwords in xml files
sed -i "s|\(<password>\)[^<>]*\(</password>\)|\1notyourpassword\2|" $debuglog_dir/$debuglog_file
sed -i "s|\(<ns0:password>\)[^<>]*\(</ns0:password>\)|\1notyourpassword\2|g" $debuglog_dir/$debuglog_file

exit 0
