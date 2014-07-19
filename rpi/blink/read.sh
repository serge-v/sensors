PIN=${1-22}
#echo "${PIN}" > /sys/class/gpio/export
#echo "in" > /sys/class/gpio/gpio${PIN}/direction
cat /sys/class/gpio/gpio${PIN}/value
