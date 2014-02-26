#!/usr/bin/python

import spidev
import time
import select

spi = spidev.SpiDev()
spi.open(0, 1)
spi.max_speed_hz = 1000000
spi.mode = 0
#spi.threewire = True

def rf12_cmd(b1, b2):
	resp = spi.xfer([b1, b2])
	print resp

def rf12_reset_fifo():
	rf12_cmd(0xCA, 0x81) # // clear ef bit
	rf12_cmd(0xCA, 0x83) # // set ef bit


RF_CONFIG = 0x80
RF_CONFIG_EL = 0x80 # // enable TX register
RF_CONFIG_EF = 0x40 # // enable FIFO
RF_PWR_MGMT  = 0x82
RF_PWR_ER = 0x80
RF_PWR_EX = 0x08
RF_PWR_EB = 0x04
RF_PWR_EBB = 0x40
RF_PWR_ES  = 0x10
RF_PWR_DC = 0x01
RF_FREQ_CFG  = 0xA6
RF_FFREQ_433 = 0x10 # // 0 1
RF_FFREQ_866 = 0x20 # // 1 0
RF_FFREQ_915 = 0x30 # // 1 1
RF_CAP_120pF = 0x7 # // 0  1  1  1  12.0
RF_DRATE_CFG = 0xC6
RF_RX_CTRL = 0x90
RF_RX_VDI_OUT = 0x04
RF_RX_RESP_FAST = 0x0 # // 0 0
RF_RX_BW_134 = 0xA # // 1 0 1 134
RF_RX_GAIN_0 =  0x00
RF_RX_RSSI_M91 = 0x02
RF_DF = 0xC2
RF_DF_AL = 0x80 # // auto lock
RF_DF_ML = 0x40 # // fast mode: 4 to 8-bit preamble (1010...) is recommended
RF_DF_SBITS = 0x28
RF_DF_S = 0x00 # // digital
RF_DF_DQD4 = 0x0C #// digital
RF_FIFO = 0xCA
RF_PATTERN = 0xCE
RF_AFC = 0xC4
RF_TX_CFG = 0x98
RF_PLL_CFG = 0xCC
RF_WAKEUP_CFG = 0xE0
RF_DUTY_CFG = 0xC8
RF_BATT_CFG = 0xC0

def rf12_rx_on():
	rf12_reset_fifo()
	rf12_cmd(RF_PWR_MGMT, RF_PWR_ER|RF_PWR_EBB|RF_PWR_ES | RF_PWR_EX|RF_PWR_EB|RF_PWR_DC)


rf12_cmd(0, 0)
rf12_cmd(RF_PWR_MGMT, RF_PWR_EB | RF_PWR_DC)
rf12_cmd(RF_CONFIG, RF_CONFIG_EL | RF_CONFIG_EF | RF_FFREQ_433 | RF_CAP_120pF)
rf12_cmd(RF_FREQ_CFG, 0x40) # // 433.26MHz
# rf12_cmd(RF_DRATE_CFG, 0x06) # // approx 49.2 Kbps, i.e. 10000/29/(1+6) Kbps
rf12_cmd(RF_DRATE_CFG, 0x11) # ; // 19200
rf12_cmd(RF_RX_CTRL|RF_RX_VDI_OUT, RF_RX_RESP_FAST | RF_RX_BW_134 | RF_RX_GAIN_0 | RF_RX_RSSI_M91)
rf12_cmd(RF_DF, RF_DF_AL | RF_DF_SBITS | RF_DF_DQD4)
rf12_cmd(RF_FIFO, 0x81) # // FIFO8,2-SYNC,!ff,DR
rf12_cmd(RF_PATTERN, 0xD4) # // SYNC=2DXX
rf12_cmd(RF_AFC, 0x83) #  // @PWR,NO RSTRIC,!st,!fi,OE,EN
rf12_cmd(RF_TX_CFG, 0x50) # // !mp,90kHz,MAX OUT
rf12_cmd(RF_PLL_CFG, 0x77) # // OB1,OB0, LPX,!ddy,DDIT,BW0. CC67
rf12_cmd(RF_WAKEUP_CFG, 0x00) #  always on
rf12_cmd(RF_DUTY_CFG, 0x00) # no low duty mode
rf12_cmd(RF_BATT_CFG, 0x49) # 1.66MHz,3.1V -- change V
# // rf12_cmd(RF_BATT_CFG, 0x00) ; // 1MHz,2.2V
rf12_cmd(RF_PWR_MGMT, RF_PWR_ER|RF_PWR_EBB|RF_PWR_ES | RF_PWR_EX|RF_PWR_EB|RF_PWR_DC)
rf12_rx_on()

print 'mode:     ', spi.mode
print '3wire     ', spi.threewire
print 'loop:     ', spi.loop
print 'speed:    ', spi.max_speed_hz
print 'lsbfirst: ', spi.lsbfirst
print 'bpw:      ', spi.bits_per_word
print 'cshigh:   ', spi.cshigh

id = 0
len = 0
idx = 0
buf = [0]*30
sbuf = [0]*30

def read_packet():
	global id, len, idx, buf, sbuf

	d = spi.xfer2([0xB0, 0])
	id = d[1]
	s = spi.xfer2([0, 0])
	d = spi.xfer2([0xB0, 0])
	len = d[1]
	if len >= 26:
		len = 25

	for i in range(0, len + 2):
#		time.sleep(0.00050)
		s = spi.xfer2([0, 0])
		sbuf[i] = s[1]
		d = spi.xfer2([0xB0, 0])
		buf[i] = d[1]

	print id, 'len:', len
	for c in buf:
		print '%02X ' % c,
	print
	for c in sbuf:
		print '%02X ' % c,
	print
	idx = 0
	id = 0
	len = 0
	buf = [0]*30
	sbuf = [0]*30
	rf12_rx_on()


gpio22 = open('/sys/class/gpio/gpio22/value', 'rt')

done = False

while True:
	r, w, x = select.select([gpio22], [], [], 5)
	#print r
	if gpio22 in r:
		str = gpio22.read()
		s = spi.xfer2([0, 0])
		if s[0] & 0x80:
			read_packet()

print id, len, idx, buf


