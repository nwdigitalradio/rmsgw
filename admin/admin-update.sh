#!/bin/bash
#
# Update python scripts & other files to be compatible with
#  Winlink Web Services V5

rmsgw_dir="/etc/rmsgw"

NEW_SERVICE_FILES="winlinkservice.xml .version_info ../etc/hosts getchannel.py updatechannel.py updateversion.py updatesysop.py"

echo "Update to V5 Winlink Web Service API"

# make sure we're running as root
if [[ $EUID != 0 ]] ; then
   echo "Must be root"
   exit 1
fi

if [ ! -d "$rmsgw_dir" ] ; then
    echo "Directory $rmsgw_dir does not exist ... exiting"
#    mkdir -p "$dest_dir"
    exit 1
fi

if [ -e "$rmsgw_dir/sysop.xml" ] ; then
    echo "Found file: $rmsgw_dir/sysop.xml"
    grep "Password" $rmsgw_dir/sysop.xml  > /dev/null 2>&1
    if [ $? -ne 0 ] ; then
        echo "Adding password to $rmsgw_dir/sysop.xml"
        sed -i -e '/<Callsign>/a\
    <Password></Password>' $rmsgw_dir/sysop.xml
    else
        echo "Password element found."
    fi
else
    echo "File: $rmsgw_dir/sysop.xml not found."
fi

for filename in `echo ${NEW_SERVICE_FILES}` ; do
    cp "$filename" "$rmsgw_dir"
    if [ "$?" -ne 0 ] ; then
       echo "Error copying file: $filename"
    else
       chown rmsgw:rmsgw "$rmsgw_dir/$(basename $filename)"
    fi
done
echo "RMS Gateway files & scripts update completed."

exit 0
