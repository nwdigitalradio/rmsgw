#HSLIDE

## Linux RMS Gateway
##### Basil Gunn  N7NIX,  last edit March 2, 2017
##### https://gitpitch.com/nwdigitalradio/rmsgw/master

#HSLIDE

## History
* First entry in history file October 5, 2004

#### Authors
* Hans-J. Barthen - DL5DI
* Brian R. Eckert - W3SG

#HSLIDE

### What is Linux RMS Gateway?
* RF to internet gateway to the Winlink 2000 ham radio e-mail system.

#HSLIDE

### How does it work?

#HSLIDE

### Installation - overview

#### Requirements
##### Build Requirements
* Install AX.25 library & support utilities
* xutils-dev libxml2 libxml2-dev

#### Installation/runtime requirements
* AX.25 kernel support
* libax25 runtime
* python 2.7 or greater
* python-requests

#VSLIDE

### Install AX.25 library & support utilities

#VSLIDE

### Install & configure Linux RMS Gateway
* Currently (3/2017) RMS Gateway needs to be built from source
  * Goal to make it a Debian package
* There are some scripts that automate the process which looks like this:
  * Download source as a zipped tarball.
  * Untar source
  * Build source
  * Install result
  * Edit config files

#VSLIDE

### Installing AX.25 library, tools & apps
* Use a script, it's easier
### Basic AX.25 support packages:
* libax25 - runtime library
* ax25-tools - utilities
* ax25-apps - applications

#VSLIDE

## RMS Gateway Installation scripts

### Install notes & scripts
* Charles Schuman K4GBB has excellent [notes & install scripts for the Raspberry Pi [here](http://k4gbb.no-ip.org/docs/Raspberry.html)
  * (http://k4gbb.no-ip.org/docs/Raspberry.html)

* I used/stole Charles' script & modified it to include all the necessary steps from his notes [here](https://github.com/nwdigitalradio/n7nix/tree/master/rmsgw)
  * (https://github.com/nwdigitalradio/n7nix/tree/master/rmsgw)

#VSLIDE

### Configuration files in /etc/rmsgw/ directory
* channels.xml
* gateway.conf
* updatechannel.py
* banner

### Python files in /etc/rmsgw/ directory
* getsysop.py
* mksysop.py
* updatechannel.py
* updatesysop.py
* updateversion.py

#HSLIDE

### Running an RMS Gateway

* Winlink expects an RMS Gateway to run 24/7
* Check logs daily using cron, script and e-mail
* Verify in http://winlink.org/RMSChannels
  * click on packet & zoom in to your location

#HSLIDE

### Documentaion
#### man pages
* cms-hosts.5
* getsysop.py.8
* mksysop.py.8
* rms-channels.5
* rms-config.5
* rmsgw.1
* rmsgw_aci.1
* updatesysop.py.8

### Forum

* https://groups.yahoo.com/neo/groups/LinuxRMS/info