## Winlink Web Services scripts

* These notes are for scripts supporting V5 Winlink Web Services
* The python scripts in this directory require python version 2.7.9 or newer to run.

* The following files in /etc/rmsgw need to be modified to support V5 Winlink Web Services
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
./update.sh
```
* Set LOGMASK=DEBUG in file: /etc/rmsgw/gateway.conf
To test the new scripts are working, run the following 3 scripts then check the https://winlink.org website:
```
# Become root
cd /etc/rmsgw
sudo -u rmsgw ./getchannel.py
sudo -u rmsgw ./updateversion.py ; echo $?
sudo -u rmsgw ./updatechannel.py ; echo $?
sudo -u rmsgw ./updatesysop.py
```

* **Packet RMS Map:**
  * verify a balloon exists at approx your location
  * Click on balloon & verify information
* **Packet RMS List:**
  * Verify your call sign is in the list
  * Verify information on same line as call sign
  * Hover cursor over call sign & verify information
* **Gateway Versions:**
  * Verify your call sign is in the list
  * Verify line with your call sign contains RMS Gateway with version 2.4.1

### Sysop

* updatesysop.py
  * Need to edit /etc/rmsgw/sysop.xml
  * This is done automatically with *update.sh* script
* getsysop.py
  * Currently not functioning, used to verify sysop record.
* mksysop.py
  * Currently not functioning

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
  * Called from rmsgw_aci
  * Check log file for output

###### Files:
* **Do Not Modify** this file: /etc/rmsgw/.version_info

### Notes
* channels.xml file contains extra elements not used by Winlink Web Services
  * https://cms.winlink.org/xml/metadata?op=ChannelAdd
  * Extra elements: groupreference, autoonly, statuschecker
  * statuschecker xml contents are referenced in:
    * devutils/chantest.c
    * rmsgw_aci/wl2k_aci.c
    * librms/getchan.c
