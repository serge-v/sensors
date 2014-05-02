Raspberry PI and RFM12BS on breadboard
======================================

        ┌──┬────┬RFM12BS┬────┬─┐
        │ 8│ANT │    ┌─┐│GND │7│
[VCC]───┤ 9│VCC │    │Z││NRES│6│
[GND]───┤10│GND │    │Z││CLK │5│
        │11│NINT│┌──┐│Z││DCLK│4│
  ┌─────┤12│SDI ││IC│└─┘│FSK │3│
  │ ┌───┤13│SCK │└──┘   │NIRQ│2├────┐
  │ │ ┌─┤14│NSEL│       │SDO │1├─┐  │
  │ │ │ └──┴────┴───────┴────┴─┘ │  │
  │ │ │                          │  │        ──────────────────┐
  │ │ │                          │  │                     ┌───┐│
  │ │ │                          │  │                     │x x││
  │ │ │                          │  │                     │x x││
  │ │ │                          │  │   ┌──┬───────────┐   ... │
  │ │ └──────────────────────────┼──┼───┤13│GPIO 27    ├──┤x x││
  │ │                            │  └───┤15│GPIO 22    ├──┤x x││
  │ │                            │     ─┤17│3.3V       ├──┤x x││
  └─┼────────────────────────────┼──────┤19│MOSI       ├──┤x x││
    │                            └──────┤21│MISO       ├──┤x x││
    └───────────────────────────────────┤23│SCK        ├──┤x x││
                                       ─┤25│GND        ├──┤x x││
                                        └──┴───────────┘  └───┘│
                                                               │



Raspberry pi backup steps
==========================

	sudo dd bs=4M if=/dev/sdd of=`date +%Y%m%dT%H%M`-cam1.img


Raspberry pi recovery steps
===========================

Make sure that SD device is on /dev/sdd

	ls -l /dev/disk/by-id | grep _SD_MMC

Copy image to the sd card

	sudo dd bs=4M if=20140301-2-resized.img of=/dev/sdd

Fix HDMI blank screen. Change config.txt to:

	disable_overscan=0
	hdmi_force_hotplug=1
	hdmi_group=1
	hdmi_mode=16 # 1920x1080


Raspberry pi install steps without keyboard and HDMI monitor
============================================================

Make sure that SD device is on /dev/sdd

	ls -l /dev/disk/by-id | grep _SD_MMC

Copy image

	sudo dd bs=4M if=image.img of=/dev/sdd

Mount image
	mount-cam1-sd.sh

Edit network config

	cd /mnt/cam1-sd/p2/etc/network
	sudo nano interfaces

Put text
	
	auto wlan0
	iface wlan0 inet dhcp
        wireless-essid [ESSID]
        wireless-mode [MODE] 

Umount image
	
	sudo umount /mnt/cam1-sd/p1
	sudo umount /mnt/cam1-sd/p2

Connect RS-232 cable
--------------------

    ............. |
    ........GWB.R |
    ---------------+
    Green - TX
    White - RX
    Black - 0
    Red   - +5V (optional)

After boot
----------

Change password:

    passwd

Enable wireless networking (if not done before):

    sudo nano /etc/network/interfaces

Change following lines:

    wireless-essid  NAME
    wireless-key PASSWORD

Save file (Ctrl+X, Y) and then reboot:

    sudo reboot

Install mc and scripts:

    sudo apt-get update
    sudo apt-get install mc
    git clone https://github.com/serge-v/homedir.git

Manually copy homedir to $HOME

Upgrade:

    sudo apt-get update
    sudo apt-get upgrade

Start configuration:

    sudo raspi-config
    sudo dpkg-reconfigure console-setup

Autologin:

Open:

    sudo nano /etc/inittab

Change respawn for tty1:

    1:2345:respawn:/bin/login -f pi tty1 </dev/tty1 >/dev/tty1 2>&1

