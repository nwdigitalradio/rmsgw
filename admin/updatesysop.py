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
from xml.etree import ElementTree

#################################
# BEGIN CONFIGURATION SECTION
#################################

DEBUG = 0
service_config_xml = '/etc/rmsgw/winlinkservice.xml'
sysop_config_xml = '/etc/rmsgw/sysop.xml'

#################################
# END CONFIGURATION SECTION
#################################

errors = 0

#
# load service config from XML
#
winlink_service = ElementTree.parse(service_config_xml)
winlink_config = winlink_service.getroot()

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

if DEBUG: print 'ws_config =', ws_config
if DEBUG: print 'svc_calls =', svc_calls
if DEBUG: print 'param_roots =', param_roots

headers = {'Content-Type': 'application/xml'}

svc_url = 'http://' + ws_config['svchost'] + ':' + ws_config['svcport'] + svc_calls['sysopadd']
if DEBUG: print 'svc_url =', svc_url


#
# load sysop info from XML
#
document = ElementTree.parse(sysop_config_xml)
sysops = document.getroot()

for sysop in sysops.findall('sysop'):
    callsign = sysop.find('Callsign').text
    print 'Posting sysop record updates for', callsign, '...'

    #
    # prepare xml parameters for call
    #
    sysop_add = ElementTree.Element(param_roots['sysopadd'])
    sysop_add.set('xmlns:i', 'http://www.w3.org/2001/XMLSchema-instance')
    sysop_add.set('xmlns', ws_config['namespace'])
    access_code = ElementTree.SubElement(sysop_add, 'WebServiceAccessCode')
    access_code.text = ws_config['WebServiceAccessCode']

    sysop_add.append(sysop.find('Callsign'))
    sysop_add.append(sysop.find('GridSquare'))
    sysop_add.append(sysop.find('SysopName'))
    sysop_add.append(sysop.find('StreetAddress1'))
    sysop_add.append(sysop.find('StreetAddress2'))
    sysop_add.append(sysop.find('City'))
    sysop_add.append(sysop.find('State'))
    sysop_add.append(sysop.find('Country'))
    sysop_add.append(sysop.find('PostalCode'))
    sysop_add.append(sysop.find('Email'))
    sysop_add.append(sysop.find('Phones'))
    sysop_add.append(sysop.find('Website'))
    sysop_add.append(sysop.find('Comments'))

    if DEBUG: print 'sysop_add XML =', ElementTree.tostring(sysop_add)

    response = requests.post(svc_url, data=ElementTree.tostring(sysop_add), headers=headers)
    if DEBUG: print 'Response =', response.content

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
        if DEBUG: print 'Returned ErrorCode =', error_code
        if error_code > 0:
            print 'Get for', callsign, 'failed. ErrorCode =', error_code, '-', error_info.find(ws_config['namespace'] + 'ErrorMessage').text
            # since theoretically we can have more than one sysop in the xml,
            # allow the loop to just continue rather than exiting here
            #sys.exit(1)
    
    #
    # get status response (if there is one) and confirm success
    #
    for sysop_info in root.iter('{' + ws_config['namespace'] + '}' + 'StatusResponse'):
        #
        # check that we got a good status response
        #
        error_code = int(sysop_info.find('{' + ws_config['namespace'] + '}' + 'ErrorCode').text)
        error_text = sysop_info.find('{' + ws_config['namespace'] + '}' + 'ErrorMessage').text
        if DEBUG: print 'StatusResponse ErrorCode =', error_code

        if error_code != 0:
            # this is unexpected!
            print '*** Update for', callsign, 'failed, ErrorCode = ', error_code, '-', error_text
            errors += 1
        else:
            print 'Update for', callsign, 'successful.'

if errors > 0:
    sys.exit(1)
#else
sys.exit(0)
