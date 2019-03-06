## Winlink Web Services scripts

* These notes are for scripts supporting V5 Winlink Web Services
* The python scripts in [this directory](https://github.com/nwdigitalradio/rmsgw/tree/master/admin) require python version 2.7.9 or newer to run.

* The following files in /etc/rmsgw needed to be updated to support V5 Winlink Web Services
  * winlinkservice.xml
  * sysop.xml
  * .version_info
  * hosts
  * updatechannel.py
  * updateversion.py
  * updatesysop.py
  * getsysop.py
  * mksysop.py

* The following files are new & are used with the above scripts.
  * getchannel.py
  * versionlist.sh
    * calls getversionlist.py
  * testwlapi.sh

### Before You update
* Check if you have an /etc/rmsgw/sysop.xml file
  * If you don't have a sysop.xml file you should create one now.

##### How to create a sysop.xml file
```
sudo su
cd /etc/rmsgw
./mksysop.py

# Verify new sysop.xml file
cat new-sysop.xml

# Using an editor modify new-sysop.xml to suit
# and copy to working file name.
cp  new-sysop.xml sysop.xml
```

##### Now update the RMS Gateway admin files.

```
# as root
cd admin
./admin-update.sh
```

##### Test your updated admin files
* To test the new scripts, either run the following scripts or run *testwlapi.sh*
* Now check the Winlink https://winlink.org/RMSChannels website:

```
# as root
cd /etc/rmsgw
sudo -u rmsgw ./updateversion.py ; echo $?
sudo -u rmsgw ./updatechannel.py ; echo $?
./updatesysop.py
./getchannel.py
./getsysop.py
```
* Alternatively just run the test script

```
# as root
cd admin
./testwlapi.sh
```

* Search the generated log file for any errors

 ```
grep -i error /root/tmp/debuglog.txt
```
* Now verify your gateway on the Winlink web site

### What to check on the [Winlink web site](https://winlink.org/RMSChannels)

* **Winlink Packet RMS Map:**
  * verify a balloon exists at approx your location
  * Click on balloon & verify information
* **Winlink Packet RMS List:**
  * Verify your call sign is in the RMS List
  * Verify information on same line as call sign
  * Hover cursor over call sign & verify information
* **Winlink Gateway Versions:**
  * Verify your call sign is in the Gateway Versions list right hand column.
  * Verify line with your call sign contains RMS Gateway with version 2.5.0

### Python Script Descriptions by Function

#### Sysop

* updatesysop.py
  * An added <Password> xml element is required in /etc/rmsgw/sysop.xml
  * This is done automatically with *admin-update.sh* script
  * *updatesysop.py* script will copy password found in channels.xml file.
* getsysop.py
  * Used to verify the sysop record on the Winlink site
* mksysop.py
  * Used to make a new sysop record based on the Winlink sysop information.
  * Makes a sysop xml record in file new-sysop.xml in directory where *mksysop.py* script is run

###### Files:
* /etc/rmsgw/sysop.xml

#### Channel

* updatechannel.py
  * Called from *rmsgw_aci*
  * Check log file for output
* getchannel.py
  * Used to verify channel record

###### Files:
* /etc/rmsgw/channel.xml

#### Version

* updateversion.py
  * Called from *rmsgw_aci*
  * Check log file for output
* versionlist.sh [-c][-d][-h][-t][-T]
  * Calls *getversionlist.py* to get a count of stations that are running down rev software.

###### Files:
* **Do Not Modify** this file: /etc/rmsgw/.version_info

### Debug
* Set LOGMASK=DEBUG in file: /etc/rmsgw/gateway.conf
* To turn on debug all the python scripts support the '-d' option on the command line
* If you need a debug info file to help with any problems you should run the *testwlapi.sh* script as root
  * Debug info file: /root/tmp/debuglog.txt
```
# Become root & go to directory with the test script
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
