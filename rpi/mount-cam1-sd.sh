#!/bin/bash

if [ ! -d /mnt/cam1-sd/p1 ] ; then
	mkdir /mnt/cam1-sd
	mkdir /mnt/cam1-sd/p1
	mkdir /mnt/cam1-sd/p2
fi

if [[ "$1" == "-u" ]]; then
	umount /mnt/cam1-sd/p1
	umount /mnt/cam1-sd/p2
	exit
fi

mount /dev/sdd1 /mnt/cam1-sd/p1
mount /dev/sdd2 /mnt/cam1-sd/p2
