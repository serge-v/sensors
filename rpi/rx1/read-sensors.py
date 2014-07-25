#!/usr/bin/python2

import httplib, os, sys

if 'a' not in sys.argv:
	print "disabled"
	sys.exit(0)

f = os.popen('/home/noro/src/xtree/sensors/rpi/rx1/rx2')
body = f.read()
f.close()

conn = httplib.HTTPSConnection('api.xively.com')

headers = {
	'X-ApiKey': 'BYK5wBPWPnbqOK8EQH8lhw0GXhANbxNMFl6iwhSqWXjXIm3u',
	'Accept': 'text/csv'
	}

conn.request('PUT', '/v2/feeds/1700338307', body, headers)
response = conn.getresponse()
print response.status, response.reason
print response.read()
