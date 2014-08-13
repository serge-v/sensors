history -s r
history -s g
history -s t

while [[ 1 ]]; do
	read -e -p "> " cmd
	[[ $cmd == "q" ]] && { exit ; }
	echo $cmd > /dev/ttyACM1
	history -s $cmd
done
