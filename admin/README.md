## Winlink Web Services scripts

* These notes are for scripts supporting V5 Winlink Web Services

* These xml files in /etc/rmsgw need to be modified to support V5 Winlink Web Services
  * winlinkservice.xml
  * channels.xml
  * sysop.xml
* This text file in /etc/rmsgw need to be modified:
  * hosts

###### Files:
* /etc/rmsgw/winlinkservice.xml

### Sysop

* updatesysop.py
  * Need to edit /etc/rmsgw/sysop.xml
* getsysop.py
  * Currently not functioning
* mksysop.py
  * Currently not functioning

###### Files:
* /etc/rmsgw/sysop.xml

### Channel

* updatechannel.py
  * Called from rmsgw_aci
  * Check log file for output
* getchan.py

###### Files:
* /etc/rmsgw/channel.xml


### Version

* updateversion.py
  * Called from rmsgw_aci
  * Check log file for output

###### Files:
* **Do Not Modify**
* /etc/rmsgw/.version_info
