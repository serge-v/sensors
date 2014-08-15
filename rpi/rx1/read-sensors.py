#!/usr/bin/python2

import httplib, os, sys

# disabled for driver testing
sys.exit(0)

if 'debug' in sys.argv:
	print "disabled"
	sys.exit(0)

log = open('/tmp/rx.log', 'wt')
log.write('started\n')

f = os.popen('/home/noro/src/xtree/sensors/rpi/rx1/rx2')
body = f.read()
rc = f.close()
if rc is not None:
	log.write('rc: %d\n', rc)
	print >> sys.stderr, 'rc: %d' % rc
	sys.exit(1)

conn = httplib.HTTPSConnection('api.xively.com')

headers = {
	'X-ApiKey': 'BYK5wBPWPnbqOK8EQH8lhw0GXhANbxNMFl6iwhSqWXjXIm3u',
	'Accept': 'text/csv'
	}

conn.request('PUT', '/v2/feeds/1700338307', body, headers)
response = conn.getresponse()
print response.status, response.reason
text = response.read()
print text
log.write('%s %s\nbody: \n%s\n' % (response.status, response.reason, body))
