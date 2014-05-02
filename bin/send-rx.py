#!/usr/bin/python

import time, os

f = open('/dev/ttyACM0', 'wt')

def cmd(hi, lo):
	print 'sending c %02X %02X s' % (hi, lo)
	f.write('c %02X %02X s' % (hi, lo))
	f.flush()
	time.sleep(0.1)

def cmds(s):
	f.write(s)
	f.flush()
	time.sleep(0.1)

def send():
	
	cmds('wwpqd')

	cmd(0, 0)
	cmds('d')
	cmd(0x82, 0x05)
	
	#  7  6  5  4  3  2  1  0        POR
	# el ef b1 b0 x3 x2 x1 x0                8008h
	#        0  1 433
	#     |enable FIFO mode
	# |enable dataregister 
	cmd(0x80, 0xD8) # EL (ena TX), EF (ena RX FIFO), 12.0pF 
	
	freq = 10*(43+0x40/4000.0)
	print 'freq:', freq
	cmd(0xA6, 0x40) # 868MHz 
	cmd(0xC6, 0x06) # approx 49.2 Kbps, i.e. 10000/29/(1+6) Kbps
	cmd(0x94, 0xA2) # VDI,FAST,134kHz,0dBm,-91dBm 
	cmd(0xC2, 0xAC) # AL,!ml,DIG,DQD4 

	cmd(0xCA, 0x83) # FIFO8,2-SYNC,!ff,DR 
	
	time.sleep(2)
	cmds('r 2')
	
	cmd(0xCE, 0xd4) # SYNC=2DXX 
	cmd(0xC4, 0x83) # @PWR,NO RSTRIC,!st,!fi,OE,EN 
	cmd(0x98, 0x50) # !mp,90kHz,MAX OUT 
	cmd(0xCC, 0x77) # OB1,OB0, LPX,!ddy,DDIT,BW0 
	cmd(0xE0, 0x00) # NOT USE 
	cmd(0xC8, 0x00) # NOT USE 
	cmd(0xC0, 0x49) # 1.66MHz,3.1V 

	time.sleep(2)
	cmd(0xCA, 0x81) # receiver on 
	cmd(0xCA, 0x83) # receiver on 
	cmd(0x82, 0xDD) # receiver on 
	cmds('r 2')
	cmds('d')

send()
f.close()
