#HSLIDE

## Linux RMS Gateway
##### Basil Gunn N7NIX, last edit March 2, 2017
#HSLIDE

## History
* First entry in history file October 5, 2004

## Authors
* Hans-J. Barthen - DL5DI
* Brian R. Eckert - W3SG

#HSLIDE
### What is Linux RMS Gateway?
* RF to internet gateway to the Winlink 2000 ham radio e-mail system.

#HSLIDE
### How does it work?

#HSLIDE
## Installation - overview
### Installation requirements
* AX.25 kernel support
* libax25
* python 2.7 or greater
* xutils-dev libxml2 libxml2-dev python-requests
### Install AX.25 library & support utilities
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
## RMS Gateway Installation - scripts

### Install notes & scriptss
* Charles Schuman K4GBB has excellent [notes & install scripts for the Raspberry Pi here](http://k4gbb.no-ip.org/docs/Raspberry.html)

* I used/stole Charles' script & modified it to include all the necessary steps from his notes [here](https://github.com/nwdigitalradio/n7nix/tree/master/rmsgw)
