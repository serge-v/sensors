set -x

#wget -u http://downloads.raspberrypi.org/raspbian_latest

#unzip 2014-01-07-wheezy-raspbian.zip

img=2014-01-07-wheezy-raspbian.img

mkdir /mnt/raspbian1
mkdir /mnt/raspbian2

part1_start=`/sbin/fdisk -lu $img|grep ${img}1|awk '{ print $2 }'`
part2_start=`/sbin/fdisk -lu $img|grep ${img}2|awk '{ print $2 }'`

mount -t vfat -o loop,offset=$(( ${part1_start} * 512 )) ${img} /mnt/raspbian1/
mount -t ext4 -o loop,offset=$(( ${part2_start} * 512 )) ${img} /mnt/raspbian2/
