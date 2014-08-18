node12
======

ATTiny and RFM12B.
Temperature sensor using internal temperature sensor
on ADC4.
Transmits temperature every 16 seconds.
After transmit listens for 2 seconds for calibration command.

Features
--------
 - NIRQ is not used. Spinning is using on status MSB bit.
 - Debugging software serial output on PB3.


Attiny85 and RFM12BS on breadboard
==================================

        ┌───────RFM12BS───────┐
        │ 8 ANT         GND  7│
[VCC]───┤ 9 VCC     [Z] NRES 6│
[GND]───┤10 GND     [Z] CLK  5│
        │11 NINT    [Z] DCLK 4│
  ┌─────┤12 SDI [IC]    FSK  3│
  │ ┌───┤13 SCK         NIRQ 2│
  │ │ ┌─┤14 NSEL        SDO  1├────┐
  │ │ │ └─────────────────────┘    │
  │ │ │                            │
  │ │ │                            │
  │ │ │                            │
  │ │ └──────────────────────┐     │
  │ │  ┌─────────────────────┼─────┘
  │ │  │     ┌───ATTINY85──┐ │
  │ │  o─────┤5 DI    GND 4├─┼──────────[GND]
  └─┼──┼─────┤6 DO    PB4 3├─┘
    └──┼───o─┤7 SCK   PB3 2├───────[330]─────[RS232-RX-White]
[VCC]──┼─o─┼─┤8 VCC   SS  1├─┐        [GND]──[RS232-GND-Black]
       │ │ │ └────────────o┘ │
       │ │ │                 │
       │ │ │                 │
       │ │ │                 │
       │ │ │                 │               ┌─Arduino ISP─┐
       │ │ └─────────────────┼───────────────┤13 SCK       │
       │ └───────────────────┼───────────────┤12 MISO      │
       └─────────────────────┼───────────────┤11 MOSI      │
                             └───────────────┤10 SS        │
                                             └─────────────┘
