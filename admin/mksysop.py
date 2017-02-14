#!/usr/bin/python
#			m k s y s o p . p y
# $Revision: 165 $
# $Author: eckertb $
# $Id: mksysop.py 165 2014-06-05 11:28:26Z eckertb $
#
# Description:
#	RMS Gateway - generate sysop XML file to maintain
#	the sysop record in the winlink system
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
sysop_template_xml = '/etc/rmsgw/sysop-template.xml'
sysop_config_xml = '/etc/rmsgw/sysop.xml'
new_sysop_config_xml = 'new-sysop.xml'

#################################
# END CONFIGURATION SECTION
#################################

### Local Functions #############

def NotFoundHelp():
    print '*' * 75
    print 'If you need to create your initial sysop record,'
    print 'copy', sysop_template_xml, 'to', sysop_config_xml
    print 'and enter the appropriate text within each of the'
    print 'sysop element tags.'
    print '*' * 75

#################################

cmdlineparser = OptionParser()
cmdlineparser.add_option("-d", "--debug",
                         action="store_true", dest="DEBUG", default=False,
                         help="turn on debug output")
cmdlineparser.add_option("-c", "--callsign",
                         action="store", metavar="CALLSIGN",
                         dest="callsign", default=None,
                         help="use a specific callsign")
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
# need a callsign to try to get things setup
#
# if there is existing sysop info stored in
# the winlink system, we'll grab that and create
# the initial local XML file of values so that
# it can be easily maintained and updated going
# forward
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

# we'll load the returned xml into this dictionary
return_data = {}

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
for errorInfo in root.iter('WebServiceResponse'):
    ErrorCode = int(errorInfo.find('ErrorCode').text)
    if options.DEBUG: print 'Returned ErrorCode =', ErrorCode
    if ErrorCode > 0:
        print 'Get for', callsign, 'failed. ErrorCode =', ErrorCode, '-', errorInfo.find('ErrorMessage').text
        NotFoundHelp()
        sys.exit(1)
    
#
# turn xml into dictionary (yes, this whole thing looks inefficient--xml to dict only to go back
# to xml, but it was a shortcut of sorts, and we may want to do more with the returned data in the
# future and the dictionary is easier to work with once return data is in it)
#
for sysopInfo in root.iter('SysopGetResponse'):
    return_data['ErrorCode'] = sysopInfo.find('ErrorCode').text
    return_data['ErrorMessage'] = sysopInfo.find('ErrorMessage').text
    return_data['ServerName'] = sysopInfo.find('ServerName').text

#    if return_data['ErrorCode'] != 0:
        # this is unexpected!
#        print '*** Get for', options.callsign, 'failed, ErrorCode =', return_data['ErrorCode'] , '-', return_data['ErrorMessage'] 
#        sys.exit(1)
    for sysop_record in sysopInfo.findall('Sysop'):
       if options.DEBUG: print ElementTree.tostring(sysop_record)
       return_data['Callsign'] = sysop_record.find('Callsign').text
       return_data['GridSquare'] = sysop_record.find('GridSquare').text
       return_data['SysopName'] = sysop_record.find('SysopName').text
       return_data['StreetAddress1'] = sysop_record.find('StreetAddress1').text
       return_data['StreetAddress2'] = sysop_record.find('StreetAddress2').text
       return_data['City'] = sysop_record.find('City').text
       return_data['State'] = sysop_record.find('State').text
       return_data['Country'] = sysop_record.find('Country').text
       return_data['PostalCode'] = sysop_record.find('PostalCode').text
       return_data['Email'] = sysop_record.find('Email').text
       return_data['Phones'] = sysop_record.find('Phones').text
       return_data['Website'] = sysop_record.find('Website').text
       return_data['Comments'] = sysop_record.find('Comments').text
       return_data['Timestamp'] = sysop_record.find('Timestamp').text

if options.DEBUG: print 'return_data dict =', return_data

#
# check that we got something useful back
#
if int(return_data['ErrorCode']) != 0:
    print 'Get for', options.callsign, 'failed ErrorCode = ', return_data['ErrorCode'], '-', return_data['ErrorMessage']
    NotFoundHelp()
    sys.exit(1)
elif 'Callsign' not in return_data:
    print 'Get for', callsign, 'failed - callsign not returned.\n'
    NotFoundHelp()
    sys.exit(2)

#
# load the sysop XML doc (this is an unpopulated template
# for our purposes here)
#
document = ElementTree.parse(sysop_template_xml)
sysops = document.getroot()

#
# update the XML document with the values returned from
# our webservice call...
#
for sysop in sysops.findall('sysop'):
    sysop.find('Callsign').text = options.callsign
    sysop.find('GridSquare').text = return_data['GridSquare']
    sysop.find('SysopName').text = return_data['SysopName']
    sysop.find('StreetAddress1').text = return_data['StreetAddress1']
    sysop.find('StreetAddress2').text = return_data['StreetAddress2']
    sysop.find('City').text = return_data['City']
    sysop.find('State').text = return_data['State']
    sysop.find('Country').text = return_data['Country']
    sysop.find('PostalCode').text = return_data['PostalCode']
    sysop.find('Email').text = return_data['Email']
    sysop.find('Phones').text = return_data['Phones']
    sysop.find('Website').text = return_data['Website']
    sysop.find('Comments').text = return_data['Comments']

#
# ... and write the updated document to a new XML file
# for the sysop to inspect and move into place
# as appropriate
document.write(new_sysop_config_xml)

print 'New sysop XML data written to', new_sysop_config_xml
print 'Please inspect the results and copy to', sysop_config_xml
print 'if it is correct.'

sys.exit(0)

