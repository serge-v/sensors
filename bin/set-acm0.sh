set -x
stty -F /dev/ttyACM0 cs8 19200 \
-parenb -parodd cs8 -hupcl -cstopb cread clocal -crtscts \
-ignbrk brkint ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl ixon -ixoff \
-iuclc -ixany -imaxbel -iutf8 \
-opost -olcuc -ocrnl -onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0 \
-isig -icanon iexten -echo echoe echok -echonl -noflsh -xcase -tostop -echoprt \
echoctl echoke
