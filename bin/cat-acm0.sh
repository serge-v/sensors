xterm-title.sh ttyACM0
set-acm0.sh

while [[ 1 ]] ; do
	date +'===== %Y-%m-%d-%H:%M:%S =======' | tee -a ttyACM0.log
	cat /dev/ttyACM0 | tee -a ttyACM0.log | ~/src/sensors/mc/tx4/filter.py
	echo restarting...
	sleep 5
done

