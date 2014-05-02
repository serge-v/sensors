set-acm1.sh
echo i > /dev/ttyACM1
while [[ 1 ]] ; do cat /dev/ttyACM1 ; echo restarting... ; sleep 5; done
