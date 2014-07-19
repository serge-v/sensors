set -x

img=$1
bname=$(basename ${img} .img)

mkdir /mnt/${bname}-1
mkdir /mnt/${bname}-5

part1_start=`/sbin/fdisk -lu $img|grep ${img}1|awk '{ print $2 }'`
part2_start=`/sbin/fdisk -lu $img|grep ${img}2|awk '{ print $2 }'`
part5_start=`/sbin/fdisk -lu $img|grep ${img}5|awk '{ print $2 }'`

mount -t vfat -o loop,offset=$(( ${part1_start} * 512 )) ${img} /mnt/${bname}-1
mount -t ext4 -o loop,offset=$(( ${part5_start} * 512 )) ${img} /mnt/${bname}-5

