cd ~/src/xtree

aptitude show gcc-arm-linux-gnueabihf | grep 'State: installed'
RETVAL=$?

if [ $RETVAL != 0 ]; then
	echo sudo apt-get install bc gcc-arm-linux-gnueabihf make ncurses-dev
fi

git clone -b rpi-3.10.y https://github.com/raspberrypi/linux.git


export CCPREFIX=/usr/bin/arm-linux-gnueabihf-

cd ~/src/xtree/linux
make mrproper
wget -O .config https://raw.github.com/raspberrypi/linux/rpi-3.10.y/arch/arm/configs/bcmrpi_defconfig
make ARCH=arm CROSS_COMPILE=${CCPREFIX} menuconfig

