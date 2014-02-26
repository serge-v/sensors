PIN=${1-27}

echo "${PIN}" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio${PIN}/direction
echo "1" > /sys/class/gpio/gpio${PIN}/value
sleep 1
echo "0" > /sys/class/gpio/gpio${PIN}/value
echo "${PIN}" > /sys/class/gpio/unexport
