Raspberry PI with Wifi dongle
#############################

Copying img
===========

diskutil list
dd if=/path/to/freebsd.img of=/dev/diskidentifier bs=1M conv=sync

Stupid fix
==========
ln libcrypto.so.8 libcrypto.so.7 # pkg required old version

Install packages
================
pkg install rsync

Configure ssh
=============
echo ssh-rsa YOUR_PUB_KEY .ssh/authorized_keys
chmod 400 .ssh/authorized_keys

Copying firmware
================
copy startx.elf from firmware.
get firmware:
git clone --depth=1 https://github.com/raspberrypi/firmware
copy start_x.elf, fixup_x.dat to the SD card.

Enable camera
=============
# cat /boot/msdos/CONFIG.TXT
gpu_mem=128
device_tree=rpi.dtb
device_tree_address=0x100
disable_commandline_tags=1
kernel=u-boot.img
start_x=1 # enable loading start_x.elf with camera support

Enable WiFi
===========
# cat /boot/loader.conf
legal.realtek.license_ack=1
if_urtwn_load="YES"
wlan_wep_load="YES"
wlan_ccmp_load="YES"
wlan_tkip_load="YES"
verbose_loading="YES"

Configure hostna, ssh, wifi, ntp
================================
# cat /etc/rc.conf
hostname="rpi-b"
ifconfig_DEFAULT="DHCP"
sshd_enable="YES"
sendmail_enable="NONE"
sendmail_submit_enable="NO"
sendmail_outbound_enable="NO"
sendmail_msp_queue_enable="NO"
growfs_enable="YES"
wlans_urtwn0="wlan0"
ifconfig_wlan0="WPA DHCP"
ntpd_enable="YES"
ntpd_sync_on_start="YES"

# cat /etc/wpa_supplicant.conf
network={
  ssid="SSID"
  psk="PWD"
}

Configure outgoing mail
=======================
read http://btt.red/b/2/0/freebsd-use-gmail-for-outgoing-mail

Take and send picture
=====================
ldconfig /opt/vc/lib
raspistill -o /tmp/1.jpg
uuencode /tmp/1.jpg 1.jpg | mail -s "pic" serge0x76+readme@gmail.com


