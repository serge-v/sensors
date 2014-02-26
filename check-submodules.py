#!/usr/bin/python

import os

f = open('submodules.txt')

for s in f:
	ss = s.split()
	if len(ss) > 1:
		cdir = os.getcwd()
		os.chdir(ss[0])
		print ss[0], ':'
		cmd = 'git log ..@{u}'
		print os.system(cmd)
		os.chdir(cdir)
