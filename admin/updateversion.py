#!/usr/bin/python
#			u p d a t e v e r s i o n . p y
# $Revision: 171 $
# $Author: eckertb $
# $Id: updateversion.py 171 2014-10-19 10:00:22Z eckertb $
#
# Description:
#	RMS Gateway - update the version info currently stored in
#	the winlink system
#
# RMS Gateway
#
# Copyright (c) 2004-2014 Hans-J. Barthen - DL5DI
# Copyright (c) 2008-2014 Brian R. Eckert - W3SG
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
import re
import requests
from xml.etree import ElementTree
from optparse import OptionParser
import syslog

#################################
# BEGIN CONFIGURATION SECTION
#################################

DEBUG = 0
service_config_xml = '/etc/rmsgw/winlinkservice.xml'
gateway_config = '/etc/rmsgw/gateway.conf'
version_info = '/etc/rmsgw/.version_info'

#################################
# END CONFIGURATION SECTION
#################################
cmdlineparser = OptionParser()
cmdlineparser.add_option("-d", "--debug",
                         action="store_true", dest="DEBUG", default=False,
                         help="turn on debug output")
(options, args) = cmdlineparser.parse_args()

errors = 0

ws_config = {}
svc_calls = {}
gw_config = {}
version = {}
param_roots = {}

#
# load gateway config
#
with open(gateway_config) as gwfile:
    for line in gwfile:
        if not line.strip().startswith('#'):
            name, val = line.partition("=")[::2]
            gw_config[name.strip()] = val.strip()
gwfile.close()

#
# setup syslog
#
fac = eval('syslog.LOG_' + gw_config['LOGFACILITY'].upper())
mask = eval('syslog.LOG_' + gw_config['LOGMASK'].upper())
syslog.openlog(logoption=syslog.LOG_PID, facility=fac)
syslog.setlogmask(syslog.LOG_UPTO(mask))

#
# load service config from XML
#
winlink_service = ElementTree.parse(service_config_xml)
winlink_config = winlink_service.getroot()

for svc_config in winlink_config.iter('config'):
    ws_config['WebServiceAccessCode'] =  svc_config.find('WebServiceAccessCode').text
    ws_config['svchost'] = svc_config.find('svchost').text
    ws_config['svcport'] = svc_config.find('svcport').text
    ws_config['namespace'] = svc_config.find('namespace').text

    for svc_ops in svc_config.findall('svcops'):
        for svc_call in svc_ops:
            svc_calls[svc_call.tag] = svc_call.text
            param_roots[svc_call.tag] = svc_call.attrib['paramRoot']

#
# load version info
#
with open(version_info) as versionfile:
    for line in versionfile:
        if not line.strip().startswith('#'):
            name, val = line.partition("=")[::2]
            version[name.strip()] = val.strip()
gwfile.close()

if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'ws_config = {}'.format(ws_config))
if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'svc_calls = {}'.format(svc_calls))
if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'param_roots = {}'.format(param_roots))

#
# get gateway callsign from config
# the basecall
#
if 'GWCALL' in gw_config:
    callsign = gw_config['GWCALL'].upper()

if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'callsign = {}'.format(callsign))

headers = {'Content-Type': 'application/xml'}

svc_url = 'http://' + ws_config['svchost'] + ':' + ws_config['svcport'] + svc_calls['versionadd']
if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'svc_url = {}'.format(svc_url))

#
# create xml tree for call parameters
#
version_add = ElementTree.Element(param_roots['versionadd'])
version_add.set('xmlns:i', 'http://www.w3.org/2001/XMLSchema-instance')
version_add.set('xmlns', ws_config['namespace'])
ElementTree.SubElement(version_add, "Callsign")
ElementTree.SubElement(version_add, "Program")
ElementTree.SubElement(version_add, "Version")
ElementTree.SubElement(version_add, "Comments")

if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'bare version_add XML = {}'.format(ElementTree.tostring(version_add)))

version_add.find('Callsign').text = callsign
version_add.find('Program').text = version['PROGRAM']
version_add.find('Version').text = version['LABEL']
version_add.find('Comments').text = version['PACKAGE']

if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'version_add XML = {}'.format(ElementTree.tostring(version_add)))

response = requests.post(svc_url, data=ElementTree.tostring(version_add), headers=headers)
if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'Response = {}'.format(response.content))

#
# build xml element tree from xml response
#
document = ElementTree.ElementTree(ElementTree.fromstring(response.content))
root = document.getroot()

#
# check for errors coming back first
#
for error_info in root.iter(ws_config['namespace'] + 'WebServiceResponse'):
    error_code = int(error_info.find(ws_config['namespace'] + 'ErrorCode').text)
    if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'Returned ErrorCode = {}'.format(error_code))
    if error_code > 0:
        syslog.syslog(syslog.LOG_INFO, 'Version update for {} failed. ErrorCode = {} - {}'.format(callsign, error_code, error_info.find(ws_config['namespace'] + 'ErrorMessage').text))
        sys.exit(1)
    
#
# get status response (if there is one) and confirm success
#
for version_add_info in root.iter('{' + ws_config['namespace'] + '}' + 'VersionAddResponse'):
    #
    # check that we got a good status response
    #
    error_code = int(version_add_info.find('{' + ws_config['namespace'] + '}' + 'ErrorCode').text)
    error_text = version_add_info.find('{' + ws_config['namespace'] + '}' + 'ErrorMessage').text
    if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'StatusResponse ErrorCode = {}'.format(error_code))

    if error_code != 0:
        # this is unexpected!
        syslog.syslog(syslog.LOG_ERR, '*** Version update for {} failed, ErrorCode = {} - {}'.format(callsign, error_code, error_text))
        errors += 1
    else:
        syslog.syslog(syslog.LOG_INFO, 'Version update for {} successful.'.format(callsign))

if errors > 0:
    sys.exit(1)
#else
sys.exit(0)
