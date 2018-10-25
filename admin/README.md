## Winlink Web Services scripts

* These notes are for scripts supporting V5 Winlink Web Services
* The python scripts in this directory require python version 2.7.9 or newer to run.

* The following files in /etc/rmsgw needed to be modified to support V5 Winlink Web Services
  * winlinkservice.xml
  * sysop.xml
  * .version_info
  * hosts
  * updatechannel.py
  * updateversion.py
  * updatesysop.py

* To update these files:
```
# become root
cd admin
./admin-update.sh
```

* To test the new scripts, run the following 4 scripts then check the https://winlink.org website:
```
# Become root
cd /etc/rmsgw
sudo -u rmsgw ./updateversion.py ; echo $?
sudo -u rmsgw ./updatechannel.py ; echo $?
sudo -u rmsgw ./updatesysop.py
sudo -u rmsgw ./getchannel.py
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
  * Called from rmsgw_aci
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
