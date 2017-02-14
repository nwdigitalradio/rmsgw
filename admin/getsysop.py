#!/usr/bin/python
#			g e t s y s o p . p y
# $Revision: 165 $
# $Author: eckertb $
# $Id: getsysop.py 165 2014-06-05 11:28:26Z eckertb $
#
# Description:
#	RMS Gateway - get the sysop info currently stored in
#	by the winlink system
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
import re
import requests
from xml.etree import ElementTree
from optparse import OptionParser

#################################
# BEGIN CONFIGURATION SECTION
#################################

gateway_config = '/etc/rmsgw/gateway.conf'
service_config_xml = '/etc/rmsgw/winlinkservice.xml'

#################################
# END CONFIGURATION SECTION
#################################

cmdlineparser = OptionParser()
cmdlineparser.add_option("-d", "--debug",
                         action="store_true", dest="DEBUG", default=False,
                         help="turn on debug output")
cmdlineparser.add_option("-c", "--callsign",
                         action="store", metavar="CALLSIGN",
                         dest="callsign", default=None,
                         help="get a specific callsign")
(options, args) = cmdlineparser.parse_args()

#
# dictionaries for config info
#
gw_config = {}
ws_config = {}
svc_calls = {}
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

if options.DEBUG: print 'Gateway config =', gw_config

#
# load service config from XML
#
winlink_service = ElementTree.parse(service_config_xml)
winlink_config = winlink_service.getroot()

for svc_config in winlink_config.iter('config'):
    # basic configuration data
    ws_config['WebServiceAccessCode'] =  svc_config.find('WebServiceAccessCode').text
    ws_config['svchost'] = svc_config.find('svchost').text
    ws_config['svcport'] = svc_config.find('svcport').text
    ws_config['namespace'] = svc_config.find('namespace').text

    # for the service operations, the tags are the operations,
    # element text is the service call detail
    for svc_ops in svc_config.findall('svcops'):
        for svc_call in svc_ops:
            svc_calls[svc_call.tag] = svc_call.text
            param_roots[svc_call.tag] = svc_call.attrib['paramRoot']

if options.DEBUG: print 'ws_config =', ws_config
if options.DEBUG: print 'svc_calls =', svc_calls
if options.DEBUG: print 'param_roots =', param_roots

#
# need a callsign - unless we have a command line parameter
# first try the gateway config then ask the user for a
# callsign
#
if options.callsign == None: # no callsign given on cmd line?
    #
    # first check for gateway callsign and extract
    # the basecall
    #
    if 'GWCALL' in gw_config:
        options.callsign = gw_config['GWCALL'].split('-', 1)[0] # first item of list
    else:
        # ask for callsign if none given on command line
        # and none in the gateway config
        options.callsign = raw_input('Enter gateway callsign (without SSID): ')

options.callsign = options.callsign.upper()

if options.DEBUG: print 'callsign =', options.callsign

#
# prepare and make webservice call
#
headers = {'Content-Type': 'application/xml'}

svc_url = 'http://' + ws_config['svchost'] + ':' + ws_config['svcport'] + svc_calls['sysopget']
if options.DEBUG: print 'svc_url =', svc_url

#
# prepare xml parameters for call
#
sysop_get = ElementTree.Element(param_roots['sysopget'])
sysop_get.set('xmlns:i', 'http://www.w3.org/2001/XMLSchema-instance')
sysop_get.set('xmlns', ws_config['namespace'])

child = ElementTree.SubElement(sysop_get, 'WebServiceAccessCode')
child.text = ws_config['WebServiceAccessCode']

child = ElementTree.SubElement(sysop_get, 'Callsign')
child.text = options.callsign

if options.DEBUG: print 'sysop_get XML =', ElementTree.tostring(sysop_get)

response = requests.post(svc_url, data=ElementTree.tostring(sysop_get), headers=headers)

if options.DEBUG: print 'Response =', response.content

#
# build xml element tree from xml response while
# stripping the namespace from the xml response string
# (the namespace info here is simply a royal pain and,
#  at least presently, adds no value to the handling
#  of the reponse data... so off to the burner it goes)
#
document = ElementTree.ElementTree(ElementTree.fromstring(re.sub(' xmlns="[^"]+"', '', response.content, count=1)))
root = document.getroot()

#
# check for errors coming back first
#
for error_info in root.iter('WebServiceResponse'):
    error_code = int(error_info.find('ErrorCode').text)
    if options.DEBUG: print 'Returned ErrorCode =', error_code
    if error_code > 0:
        print 'Get for', options.callsign, 'failed. ErrorCode =', error_code, '-', error_info.find('ErrorMessage').text
        sys.exit(1)

print '=== Current Sysop Record In the Winlink System ==='
for sysop_info in root.iter('SysopGetResponse'):
    #
    # check that we got something useful back
    #
    error_code = int(sysop_info.find('ErrorCode').text)
    error_text = sysop_info.find('ErrorMessage').text
    server_name = sysop_info.find('ServerName').text
    if options.DEBUG: print 'ErrorCode =', error_code
    if options.DEBUG: print 'ErrorMessage =', error_text
    if options.DEBUG: print 'ServerName =', server_name

    
    if error_code != 0:
        # this is unexpected!
        print '*** Get for', options.callsign, 'failed, ErrorCode =', error_code, '-', error_text
        sys.exit(1)

    for sysop_record in sysop_info.findall('Sysop'):
       if options.DEBUG: print ElementTree.tostring(sysop_record)

       returned_callsign = sysop_record.find('Callsign').text

       # it looks like this will be caught by the winlink system
       # and give an appropriate error in the response, so this
       # check may not be necessary any longer, but it hurts nothing
       if returned_callsign == None:
           print '*** No record found for', options.callsign
           sys.exit(2)


       #
       # display the returned data
       #
       print '(received from:', server_name, ')'
       print 'Callsign:', returned_callsign
       print 'GridSquare:', sysop_record.find('GridSquare').text
       print 'SysopName:', sysop_record.find('SysopName').text
       print 'StreetAddress1:', sysop_record.find('StreetAddress1').text
       print 'StreetAddress2:', sysop_record.find('StreetAddress2').text
       print 'City:', sysop_record.find('City').text
       print 'State:', sysop_record.find('State').text
       print 'Country:', sysop_record.find('Country').text
       print 'PostalCode:', sysop_record.find('PostalCode').text
       print 'Email:', sysop_record.find('Email').text
       print 'Phones:', sysop_record.find('Phones').text
       print 'Website:', sysop_record.find('Website').text
       print 'Comments:', sysop_record.find('Comments').text
       print 'Last Updated:', sysop_record.find('Timestamp').text

sys.exit(0)
