#!/bin/bash
# Example script that dynamically generates a Miranda batch file for invoking the UPnP AddPortMapping action.
set -x
HOST=$1
LAN_PORT=$2
WAN_PORT=$3
BATCH_FILE="/tmp/mir_upnp_batch.txt"
# Generate a random port number. This is used if the lan/wan ports are not specified.
RAND_PORT=$((0x$(dd if=/dev/urandom bs=2 count=1 2>/dev/null | hexdump | head -1 | cut -d' ' -f2)))

if [ "$HOST" == "" ] || [ "$HOST" == "-h" ] || [ "$HOST" == "--help" ]
then
	echo ""
	echo "Usage: $0 <internal ip> [lan port] [wan port]"
	echo ""
	exit 1
fi

if [ "$LAN_PORT" == "" ]
then
	LAN_PORT=$RAND_PORT
fi

if [ "$WAN_PORT" == "" ]
then
	WAN_PORT=$RAND_PORT
fi

# This generates the Miranda batch file for adding a port mapping based on user supplied input.
# It assumes that the AddPortMapping and related ations are under WANConnectionDevice -> WANIPConnection.
echo -e "set timeout 5" > $BATCH_FILE
echo -e "set max 1" >> $BATCH_FILE
#echo -e "msearch service WANIPConnection" >> $BATCH_FILE
echo -e "load struct_verizon.mir" >> $BATCH_FILE
echo -e "host get 0" >> $BATCH_FILE

echo -e "host send 0 WANConnectionDevice WANIPConnection DeletePortMapping" >> $BATCH_FILE
echo -e "TCP" >> $BATCH_FILE
echo -e "$WAN_PORT" >> $BATCH_FILE
echo -e "" >> $BATCH_FILE

echo -e "host send 0 WANConnectionDevice WANIPConnection AddPortMapping" >> $BATCH_FILE
echo -e "portmapper" >> $BATCH_FILE
echo -e "0" >> $BATCH_FILE
echo -e "$HOST" >> $BATCH_FILE
echo -e "1" >> $BATCH_FILE
echo -e "$WAN_PORT" >> $BATCH_FILE
echo -e "" >> $BATCH_FILE
echo -e "TCP" >> $BATCH_FILE
echo -e "$LAN_PORT" >> $BATCH_FILE

echo -e "host send 0 WANConnectionDevice WANIPConnection GetSpecificPortMappingEntry\n$WAN_PORT\n\nTCP\nexit" >> $BATCH_FILE

echo -e "exit" >> $BATCH_FILE

MIRANDA=./miranda.py

# Run Miranda in batch mode
$MIRANDA -b $BATCH_FILE

# Clean up the batch file
#rm -f $BATCH_FILE


