echo "22" > /sys/class/gpio/export

#echo "in" > /sys/class/gpio/gpio22/direction
echo "out" > /sys/class/gpio/gpio22/direction

echo "1" > /sys/class/gpio/gpio22/value
sleep 1
echo "0" > /sys/class/gpio/gpio22/value

#echo "22" > /sys/class/gpio/unexport
