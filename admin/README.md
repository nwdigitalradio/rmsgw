## Winlink Web Services scripts

* These notes are for scripts supporting V5 Winlink Web Services
* The python scripts in this directory require python version 2.7.9 or newer to run.

* The following files in /etc/rmsgw needed to be updated to support V5 Winlink Web Services
  * winlinkservice.xml
  * sysop.xml
  * .version_info
  * hosts
  * updatechannel.py
  * updateversion.py
  * updatesysop.py

### Before You update
* Check if you have an /etc/rmsgw/sysop.xml file
* If you don't have a sysop.xml file you need to create one.

##### How to create a sysop.xml file before you update
```
sudo su
cd /etc/rmsgw
./mksysop.py
cat new-sysop.xml
# Using an editor modify new-sysop.xml to suit
cp  new-sysop.xml sysop.xml
```
##### If you already updated your admin files and do NOT have a sysop.xml file
* Follow the parameter advice from the [CMS Web Services for SysopAdd](https://cms.winlink.org/xml/metadata?op=SysopAdd)
```
cd /etc/rmsgw
cp sysop-template.xml sysop.xml
# Using an editor modify sysop.xml to suit
```

##### If you already have a /etc/rmsgw/sysop.xml file then update the admin files.


* To update the admin files:
```
# become root
cd admin
./admin-update.sh
```

* To test the new scripts, either run the following 4 scripts or run testwlapi.sh then check the https://winlink.org website:

#####
```
# Become root
cd /etc/rmsgw
sudo -u rmsgw ./updateversion.py ; echo $?
sudo -u rmsgw ./updatechannel.py ; echo $?
sudo -u rmsgw ./updatesysop.py
sudo -u rmsgw ./getchannel.py

# As root run the test script
# Note what time it is

./testwlapi.sh
```

* Search the generated log file for any errors
* The log file grabs some of the rms.debug log file and you are only
concerned with errors found after you ran the test script

 ```
grep -i error
/root/tmp/debuglog.txt
```

* **Winlink Packet RMS Map:**
  * verify a balloon exists at approx your location
  * Click on balloon & verify information
* **Winlink Packet RMS List:**
  * Verify your call sign is in the list
  * Verify information on same line as call sign
  * Hover cursor over call sign & verify information
* **Winlink Gateway Versions:**
  * Verify your call sign is in the list
  * Verify line with your call sign contains RMS Gateway with version 2.5.0

### Sysop

* updatesysop.py
  * An added <Password> xml element is required in /etc/rmsgw/sysop.xml
  * This is done automatically with *admin-update.sh* script
* getsysop.py
  * Currently *not functioning*, used to verify sysop record.
* mksysop.py
  * Currently *not functioning*, used to make sysop record.

###### Files:
* /etc/rmsgw/sysop.xml

### Channel

* updatechannel.py
  * Called from *rmsgw_aci*
  * Check log file for output
* getchannel.py
  * Used to verify channel record

###### Files:
* /etc/rmsgw/channel.xml

### Version

* updateversion.py
  * Called from *rmsgw_aci*
  * Check log file for output

###### Files:
* **Do Not Modify** this file: /etc/rmsgw/.version_info

### Debug
* Set LOGMASK=DEBUG in file: /etc/rmsgw/gateway.conf
* To turn on debug all the python scripts support the '-d' option on the command line
* If you need a debug info file to help with any problems you should run the *testwlapi.sh* script as root
  * Debug info file: /root/tmp/debuglog.txt
```
# become root
sudo su

# go to directory with test script to run
cd rmsgw/admin
./testwlapi.sh

# Look for a freshly created debug log file: /root/tmp/debuglog.txt
```

### Notes
* channels.xml file contains extra elements not used by Winlink Web Services
  * https://cms.winlink.org/xml/metadata?op=ChannelAdd
  * Extra elements: groupreference, autoonly, statuschecker
  * statuschecker xml contents are referenced in:
    * devutils/chantest.c
    * rmsgw_aci/wl2k_aci.c
    * librms/getchan.c
