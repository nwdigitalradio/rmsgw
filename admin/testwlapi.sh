#!/bin/bash

debuglog_dir="/root/tmp"
debuglog_file="debuglog.txt"

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
python --version

cd /etc/rmsgw
echo
echo "===== getchan ====="
sudo -u rmsgw ./getchan.py
echo
echo "===== updateversion ====="
sudo -u rmsgw ./updateversion.py -d; echo $?
echo
echo "===== updatechannel ====="
sudo -u rmsgw ./updatechannel.py -d; echo $?
echo
echo "===== updatesysop ====="
sudo -u rmsgw ./updatesysop.py
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