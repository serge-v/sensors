set -x

img=$1

mkdir /mnt/cam1
mkdir /mnt/cam2

part1_start=`/sbin/fdisk -lu $img|grep ${img}1|awk '{ print $2 }'`
part2_start=`/sbin/fdisk -lu $img|grep ${img}2|awk '{ print $2 }'`

mount -t vfat -o loop,offset=$(( ${part1_start} * 512 )) ${img} /mnt/cam1/
mount -t ext4 -o loop,offset=$(( ${part2_start} * 512 )) ${img} /mnt/cam2/
