PIN=22

echo "${PIN}" > /sys/class/gpio/export
echo "in" > /sys/class/gpio/gpio${PIN}/direction
echo "1" > /sys/class/gpio/gpio${PIN}/value
echo "falling" > /sys/class/gpio/gpio${PIN}/edge
