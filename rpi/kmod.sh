MP="/usr/src/kmod-15/tools/modprobe --show-modversions"

cat ~/src/xtree/linux/Module.symvers | grep -H --label Module.symvers module_layout
${MP} ~/src/xtree/rfm12b-linux/rfm12b.ko | grep -H --label rfm12b module_layout
${MP} /mnt/cam2/lib/modules/3.10.25+/kernel/fs/ecryptfs/ecryptfs.ko | grep -H --label ecryptfs.ko module_layout
