# rmsgw
Linux RMS Gateway

[![GitPitch](https://gitpitch.com/assets/badge.svg)](https://gitpitch.com/nwdigitalradio/rmsgw/master)

* To get a copy of the repository:

```
git clone https://github.com/nwdigitalradio/rmsgw
```

* **NOTE:** Installation & configuration scripts & notes live in [n7nix/rmsgw](https://github.com/nwdigitalradio/n7nix/tree/master/rmsgw)

### Installation - overview

#### Build Requirements
* Install AX.25 library & support utilities
* xutils-dev libxml2 libxml2-dev

#### Installation/runtime requirements
* AX.25 kernel support
* libax25 runtime
* python 2.7.9 or greater
* python-requests

### Install AX.25 library & support utilities

### Install & configure Linux RMS Gateway
* Currently (3/2017) RMS Gateway needs to be built from source
  * Goal to make it a Debian package
#### Clone this repository
```
git clone https://github.com/nwdigitalradio/rmsgw
cd rmsgw
```

* Verify that directory _/usr/local/etc/rmsgw_ does NOT exist and
create a symbolic link to it.

```
ln -s /etc/rmsgw /usr/local/etc/rmsgw
```
* Do the build

```
./autogen.sh
./configure --enable-postfix
make
# Install as root
sudo make install

# Now edit the config files in /etc/rmsgw
```

#### Use scripts
* There are some scripts that automate the process which does the following:
  * Download source as a zipped tarball.
  * Untar source
  * Build source
  * Install result
  * Edit config files

### Installing AX.25 library, tools & apps
* Use a script, it's easier

### Basic AX.25 support packages:
* libax25 - runtime library
* ax25-tools - utilities
* ax25-apps - applications

## RMS Gateway Installation scripts

### Install notes & scripts
* Charles Schuman K4GBB has excellent notes & install scripts for the Raspberry Pi [here](http://k4gbb.no-ip.org/docs/raspberry.html)
  * (http://k4gbb.no-ip.org/docs/raspberry.html)

* The install script in n7nix/rmsgw uses Charles' script modified to include all the necessary steps from his notes [here](https://github.com/nwdigitalradio/n7nix/tree/master/rmsgw)
  * (https://github.com/nwdigitalradio/n7nix/tree/master/rmsgw)
