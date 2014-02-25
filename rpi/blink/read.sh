echo "22" > /sys/class/gpio/export
echo "in" > /sys/class/gpio/gpio22/direction
cat /sys/class/gpio/gpio22/value
