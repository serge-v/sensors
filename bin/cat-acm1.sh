DIR=`dirname "$0"`

xterm-title.sh ttyACM1
${DIR}/set-acm1.sh

while [[ 1 ]] ; do
	date +'===== %Y-%m-%d-%H:%M:%S =======' | tee -a ttyACM1.log
	cat /dev/ttyACM1 | tee -a ttyACM1.log | ${DIR}/../mc/rx-hub/filter.py
	echo restarting...
	sleep 5
done

	
