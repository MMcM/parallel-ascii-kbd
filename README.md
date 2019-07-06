# Micro Switch ASCII Keyboard USB driver #

A virtual serial port driver for a Micro Switch 7-bit ASCII keyboard.

A scan of the hardware documentation that came with this keyboard can 
be found in the [wiki](https://github.com/MMcM/micro-switch-ascii-kbd/wiki/scanned/MicroSwitch.pdf).

This keyboard is mostly the same as the
[NCR one](http://www.computerhistory.org/collections/catalog/102672973)
in the Computer History Museum, except it has &Ntilde; instead of
semicolon and a few other characters moved as a result.

The state of the control and shift keys is also available separately,
so it would be possible to recover more of the key state from the
7-bit character. But, really, there aren't enough keys for this to be
a modern keyboard.

## Hardware ##

The board needs a lot of GPIO pins, so an actual Arduino is not a great choice.

* [Teensy 2.0](https://www.pjrc.com/teensy/index.html).
* [Adafruit Atmega32u4 breakout](http://www.ladyada.net/products/atmega32u4breakout/).

The microcontroller is labeled SW-20306 and is, I believe, a masked ROM version
of some AMI 4-bit MCU from the 70s. It is CMOS and Vcc is -12V DC. Vdd is +5V
DC, for the control signals, both inputs from the Hall Effect switches and
character output. A DC-DC isolating converter like the
[Mornsun](http://www.mornsun-power.com/public/pdf/DCDC/B_D-1W.pdf) B0512S-1W
seems to work fine.

### Connections ###

| PCB | Signal              | AVR |
|-----|---------------------|-----|
| 2   | -12V                |     |
| 3   | +5V                 | +5V |
| 4   | GND                 | GND |
| 10  | CONTROL             | PD3 |
| 12  | SHIFT 	            | PD1 |
|     |                     |     |
| A   | STROBE              | PD0 |
| C   | +5V                 | +5V |
| D   | GND                 | GND |
| F   | CHAR BIT 2          | PB1 |
| H   | CHAR BIT 1          | PB0 |
| J   | CHAR BIT 4          | PB3 |
| K   | CHAR BIT 3          | PB2 |
| L   | CHAR BIT 5          | PB4 |
| M   | CHAR BIT 6          | PB5 |
| N   | CHAR BIT 7          | PB6 |
