#!/usr/bin/python
#			u p d a t e s y s o p . p y
# $Revision: 164 $
# $Author: eckertb $
# $Id: updatesysop.py 164 2014-05-10 23:11:15Z eckertb $
#
# Description:
#	RMS Gateway - update the sysop record in the winlink
#	system from the contents of the local XML file that
#	is maintained by the sysop
#
# RMS Gateway
#
# Copyright (c) 2004-2013 Hans-J. Barthen - DL5DI
# Copyright (c) 2008-2013 Brian R. Eckert - W3SG
#
# Questions or problems regarding this program can be emailed
# to linux-rmsgw@w3sg.org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#

import sys
import requests
import json
import datetime
import platform
from xml.etree import ElementTree
from distutils.version import StrictVersion

#################################
# BEGIN CONFIGURATION SECTION
#################################

DEBUG = 0
service_config_xml = '/etc/rmsgw/winlinkservice.xml'
sysop_config_xml = '/etc/rmsgw/sysop.xml'
channel_config_xml = '/etc/rmsgw/channels.xml'
py_version_require='2.7.9'

#################################
# END CONFIGURATION SECTION
#################################

#
# check python version
#
python_version=platform.python_version()

if StrictVersion(python_version) >= StrictVersion(py_version_require):
    if DEBUG: print 'Python Version Check: ' + str(python_version) + ' OK'
else:
    print 'Need more current Python version, require version: ' + str(py_version_require) + ' or newer'
    print 'Exiting ...'
    sys.exit(1)

errors = 0

#
# load channel config from XML - need password
#

document = ElementTree.parse(channel_config_xml)
rmschannels = document.getroot()

#
# load service config from XML
#
winlink_service = ElementTree.parse(service_config_xml)
winlink_config = winlink_service.getroot()

#
# dictionaries for config info
#
rms_chans = {}
ws_config = {}
svc_calls = {}
param_roots = {}

for svc_config in winlink_config.iter('config'):
    ws_config['WebServiceAccessCode'] =  svc_config.find('WebServiceAccessCode').text
    ws_config['svchost'] = svc_config.find('svchost').text
    ws_config['svcport'] = svc_config.find('svcport').text
    ws_config['namespace'] = svc_config.find('namespace').text

    for svc_ops in svc_config.findall('svcops'):
        for svc_call in svc_ops:
            svc_calls[svc_call.tag] = svc_call.text
            param_roots[svc_call.tag] = svc_call.attrib['paramRoot']

#if DEBUG: print "rmschannels xml = {}".format(ElementTree.tostring(rmschannels))

ns = '{http://www.namespace.org}'
for channel in rmschannels.findall("%schannel" % (ns)):
#    if DEBUG: print 'channel xml = {}'.format(ElementTree.tostring(channel))

    callsign = channel.find("%scallsign" % (ns)).text
    rms_chans['callsign'] = callsign

    password = channel.find("%spassword" % (ns)).text
    rms_chans['password'] = password

if DEBUG: print 'ws_config =', ws_config
if DEBUG: print 'svc_calls =', svc_calls
if DEBUG: print 'param_roots =', param_roots
if DEBUG: print 'rms_channels =', rms_chans

headers = {'Content-Type': 'application/xml'}

#
# load sysop info from XML
#
document = ElementTree.parse(sysop_config_xml)
sysops = document.getroot()

for sysop in sysops.findall('sysop'):
    callsign = sysop.find('Callsign').text
    callsign = callsign.upper()
    print 'Posting sysop record update for', callsign, '...'

    #
    # prepare xml parameters for call
    #
    sysop_add = ElementTree.Element(param_roots['sysopadd'])
    sysop_add.set('xmlns:i', 'http://www.w3.org/2001/XMLSchema-instance')
    sysop_add.set('xmlns', ws_config['namespace'])
    access_code = ElementTree.SubElement(sysop_add, 'Key')
    access_code.text = ws_config['WebServiceAccessCode']

    sysop_add.append(sysop.find('Callsign'))
    sysop_add.append(sysop.find('City'))
#    sysop_add.append(sysop.find('Comments'))

    comment = ElementTree.SubElement(sysop_add, 'Comments')
    comment.text = 'V5 Services Test: ' + str(datetime.datetime.now())

    sysop_add.append(sysop.find('Country'))
    sysop_add.append(sysop.find('Email'))
    sysop_add.append(sysop.find('GridSquare'))

    pass_code = ElementTree.SubElement(sysop_add, 'Password')
    pass_code.text = rms_chans['password']

    sysop_add.append(sysop.find('Phones'))
    sysop_add.append(sysop.find('PostalCode'))
    sysop_add.append(sysop.find('State'))
    sysop_add.append(sysop.find('StreetAddress1'))
    sysop_add.append(sysop.find('StreetAddress2'))
    sysop_add.append(sysop.find('SysopName'))
    sysop_add.append(sysop.find('Website'))

    if DEBUG: print 'sysop_add XML =', ElementTree.tostring(sysop_add)

    # Old winlink services url format
    #svc_url = 'http://' + ws_config['svchost'] + ':' + ws_config['svcport'] + svc_calls['sysopadd']
    svc_url = 'https://' + ws_config['svchost'] + svc_calls['sysopadd'] + '?' + 'format=json'
    if DEBUG: print 'svc_url =', svc_url

    # Post the request
    response = requests.post(svc_url, data=ElementTree.tostring(sysop_add), headers=headers)
    if DEBUG: print 'Response =', response.content

    json_data = response.json()
    if DEBUG: print(json.dumps(json_data, indent=2))
    json_dict = json.loads(response.text)

    # print the return code of this request, should be 200 which is "OK"
    if DEBUG: print "Request status code: " + str(response.status_code)
    if DEBUG: print 'Debug: Response =', response.content
    if DEBUG: print "Debug: Content type: " + response.headers['content-type']

    #
    # Verify request status code
    #
    if response.ok:
        if DEBUG: print "Debug: Good Request status code"
    else:
        print "Debug: Bad Response status code: " + str(response.status_code)
        print '*** Sysop update for', callsign, 'failed, ErrorCode =',  str(response.status_code)
        print '*** Error code:    ' + json_dict['ResponseStatus']['ErrorCode']
        print '*** Error message: ' + json_dict['ResponseStatus']['Message']
        errors += 1

    #
    # check for errors coming back
    #
    if json_dict['ResponseStatus']:
        print 'ResponseStatus not NULL: ', json_dict['ResponseStatus']
        errors += 1
    else:
        if DEBUG: print 'ResponseStatus is NULL: ', json_dict['ResponseStatus']
        print 'Sysop update for {} successful.'.format(callsign)

if errors > 0:
    sys.exit(1)
#else
sys.exit(0)
