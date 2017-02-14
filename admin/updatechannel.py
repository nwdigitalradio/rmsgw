#!/usr/bin/python
#			u p d a t e c h a n n e l . p y
# $Revision: 175 $
# $Author: eckertb $
# $Id: updatechannel.py 175 2014-10-19 19:01:52Z eckertb $
#
# Description:
#	RMS Gateway - update the channel info currently stored in
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
channel_config_xml = '/etc/rmsgw/channels.xml'

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

if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'ws_config = {}'.format(ws_config))
if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'svc_calls = {}'.format(svc_calls))
if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'param_roots = {}'.format(param_roots))

headers = {'Content-Type': 'application/xml'}

svc_url = 'http://' + ws_config['svchost'] + ':' + ws_config['svcport'] + svc_calls['channeladd']
if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'svc_url = {}'.format(svc_url))

#
# load channel info from XML
#
document = ElementTree.parse(channel_config_xml)
rmschannels = document.getroot()

if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, "rmschannels xml = {}".format(ElementTree.tostring(rmschannels)))

#
# create xml tree for call parameters
#
channel_add = ElementTree.Element(param_roots['channeladd'])
channel_add.set('xmlns:i', 'http://www.w3.org/2001/XMLSchema-instance')
channel_add.set('xmlns', ws_config['namespace'])
ElementTree.SubElement(channel_add, "Callsign")
ElementTree.SubElement(channel_add, "BaseCallsign")
ElementTree.SubElement(channel_add, "GridSquare")
ElementTree.SubElement(channel_add, "Frequency")
ElementTree.SubElement(channel_add, "Mode")
ElementTree.SubElement(channel_add, "Baud")
ElementTree.SubElement(channel_add, "Power")
ElementTree.SubElement(channel_add, "Height")
ElementTree.SubElement(channel_add, "Gain")
ElementTree.SubElement(channel_add, "Direction")
ElementTree.SubElement(channel_add, "Hours")
ElementTree.SubElement(channel_add, "ServiceCode")

if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'bare channel_add XML = {}'.format(ElementTree.tostring(channel_add)))

#
# with each configured channel,
# populate call xml tree with values from
# gateway channel config and make service call
#
ns = '{http://www.namespace.org}'
if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'ns = {}'.format(ns))
for channel in rmschannels.findall("%schannel" % (ns)):
    if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'channel xml = {}'.format(ElementTree.tostring(channel)))
    callsign = channel.find("%scallsign" % (ns)).text
 
    syslog.syslog(syslog.LOG_INFO, 'Posting channel record updates for {}...'.format(callsign))

    #
    # prepare xml parameters for call
    #
    channel_add.find('Callsign').text = channel.find('%scallsign' % (ns)).text
    channel_add.find('BaseCallsign').text = channel.find('%sbasecall' % (ns)).text
    channel_add.find('GridSquare').text = channel.find('%sgridsquare' % (ns)).text
    channel_add.find('Frequency').text = channel.find('%sfrequency' % (ns)).text
    channel_add.find('Mode').text = channel.find('%smode' % (ns)).text
    channel_add.find('Baud').text = channel.find('%sbaud' % (ns)).text
    channel_add.find('Power').text = channel.find('%spower' % (ns)).text
    channel_add.find('Height').text = channel.find('%sheight' % (ns)).text
    channel_add.find('Gain').text = channel.find('%sgain' % (ns)).text
    channel_add.find('Direction').text = channel.find('%sdirection' % (ns)).text
    channel_add.find('Hours').text = channel.find('%shours' % (ns)).text
    channel_add.find('ServiceCode').text = channel.find('%sservicecode' % (ns)).text

    if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'channel_add XML = {}'.format(ElementTree.tostring(channel_add)))

    response = requests.post(svc_url, data=ElementTree.tostring(channel_add), headers=headers)
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
        if options.DEBUG: print 'Returned ErrorCode =', error_code
        if error_code > 0:
            syslog.syslog(syslog.LOG_ERR, 'Channel update for {} failed. ErrorCode = {} - {}'. format(callsign, error_code, error_info.find,  error_info.find(ws_config['namespace'] + 'ErrorMessage').text))
            # since theoretically we can have more than one channel in the xml,
            # allow the loop to just continue rather than exiting here
            #sys.exit(1)
    
    #
    # get status response (if there is one) and confirm success
    #
    for channel_add_info in root.iter('{' + ws_config['namespace'] + '}' + 'ChannelAddResponse'):
        #
        # check that we got a good status response
        #
        error_code = int(channel_add_info.find('{' + ws_config['namespace'] + '}' + 'ErrorCode').text)
        error_text = channel_add_info.find('{' + ws_config['namespace'] + '}' + 'ErrorMessage').text
        if options.DEBUG: syslog.syslog(syslog.LOG_DEBUG, 'StatusResponse ErrorCode = {}'.format(error_code))

        if error_code != 0:
            # this is unexpected!
            print '*** Channel Update for', callsign, 'failed, ErrorCode = ', error_code, '-', error_text
            errors += 1
        else:
            syslog.syslog(syslog.LOG_INFO, 'Channel update for {} successful.'.format(callsign))

syslog.closelog()

if errors > 0:
    sys.exit(1)
#else
sys.exit(0)
