#!/usr/bin/python -u

# colorize timer with id=11

import sys, time

while 1:
	s = sys.stdin.readline()
	if s == '':
		break
		
	now = time.localtime(time.time())

	s = s.rstrip()
	
	if s.startswith('11  ') and s.endswith('%'):
		s = '\x1b[32m' + s + '\x1b[0m'
	
	print time.strftime('\x1b[33m%H:%M:%S\x1b[0m', now), s

