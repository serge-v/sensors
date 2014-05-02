DIR=`dirname "$0"`

xterm-title.sh ttyUSB0
${DIR}/set-usb0-19200.sh

while [[ 1 ]] ; do
	date +'===== %Y-%m-%d-%H:%M:%S =======' | tee -a ttyUSB0.log
	cat /dev/ttyUSB0 | tee -a ttyUSB0.log
	echo restarting...
	sleep 5
done

