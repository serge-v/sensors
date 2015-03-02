Sensors
=======
Sensors network using Raspberry PI, attiny, rfm12b.

bin           scripts for mounting images, set terminal speed etc.

mc/avrisp     Arduino base ISP programmer for taken from Arduino IDE.

mc/blink-t85  ATTiny85 and RFM12B based sensor. Transmits temp and 
              humidity every 20 sec.

mc/rx-hub     Arduino and RFM12B based tranceiver. Receives sensors
              data and sends to serial interface.

mc/node12     Simple ATTiny85 and RFM12B based sensor. Uses internal
              temperature sensor. Transmits temp and humidity
              every 20 sec.

rpi/rx2       Raspberry PI and RFM12B based tranceiver. Receives sensors
              data and uploads it to the xively account.

router        Open home router for incoming connections using UPNP.
