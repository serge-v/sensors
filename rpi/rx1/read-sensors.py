#!/usr/bin/python

import httplib, os

f = os.popen('/home/pi/src/sensors/rpi/rx1/rx1')
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
