#!/usr/bin/python -u

import sys, time

while 1:
	s = sys.stdin.readline()
	if s == '':
		break
		
	now = time.localtime(time.time())

	print time.strftime('\x1b[33m%H:%M:%S\x1b[0m', now), s.rstrip()

