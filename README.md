# Parallel ASCII Keyboard USB driver #

A virtual serial port driver for a 7-bit ASCII keyboard.

## Purpose ##

This project represents a compromise in converting old keyboards to USB. When a keyboard only sends ASCII characters, emulating a PC keyboard is a bit pointless, since there are no modifier or function keys and no key up transitions.

For a typing test, connect a terminal program. For instance, on Linux,
```
cu -l /dev/ttyACM0 -s 115200
```

## Keyboards ##

This project was originally designed for the Micro Switch SW-series keyboard described [below](#micro-switch-sw-11234).

But it should work for a variety of keyboards that send ASCII characters via a parallel interface with a strobe line.
These were used in early hobbyist computers, including the original Apple (see [Operation Manual](https://archive.org/details/Apple-1_Operation_Manual_1976_Apple_a/page/n2)).

* Early Datanetics keyboards (see [notes](https://www.applefritter.com/node/2809) they took from Apple).
* Jameco JE610 ([datasheet](http://www.bitsavers.org/pdf/jameco/Jameco_JE_610_ASCII_Keyboard_Datasheet.pdf)).

## Hardware ##

The board needs a lot of GPIO pins, so an actual Arduino is not a great choice.

* [Teensy 2.0](https://www.pjrc.com/teensy/index.html).
* [Adafruit Atmega32u4 breakout](http://www.ladyada.net/products/atmega32u4breakout/).

### Connections ###

| Signal              | AVR |
|---------------------|-----|
| STROBE              | PD0 |
| CHAR BIT 1          | PB0 |
| CHAR BIT 2          | PB1 |
| CHAR BIT 3          | PB2 |
| CHAR BIT 4          | PB3 |
| CHAR BIT 5          | PB4 |
| CHAR BIT 6          | PB5 |
| CHAR BIT 7          | PB6 |

The rest of port D is read for direct keys, but these don't do anything currently.

## Micro Switch SW-11234 ##

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

### Build ###

```
PARALLEL_KBD_OPTS = -DCONTROL_NSHIFTS=2
```

## Micro Switch SD-16234 ##

This PCB seems to have been used in a variety of keyboards. The one I tested as a 99SD24-3.

Connection to the keyboard is through a 34-pin IDC connector.

### Connections ###

Needed for this:

| IDC | Signal              | AVR |
|-----|---------------------|-----|
| 1,2,|
|3,4,5| GND                 | GND |
|33,34|
|19,20| +5V                 | +5V |
|     |                     |     |
| 22  | STROBE              | PD0 |
| 10  | CHAR BIT 1          | PB0 |
|  8  | CHAR BIT 2          | PB1 |
|  7  | CHAR BIT 3          | PB2 |
|  9  | CHAR BIT 4          | PB3 |
| 11  | CHAR BIT 5          | PB4 |
| 13  | CHAR BIT 6          | PB5 |
| 15  | CHAR BIT 7          | PB6 |
| 17  | CHAR BIT 8          | PB7 |

Additional signals on this board not needed here:

| IDC | Signal |
|-----|--------|
| 12  | Jumper 38 (open) |
| 14  | Jumper 34 (open) |
| 12  | Jumper 30 (open) |
| 21  | Optocoupled transistor (E) |
| 23  | Optocoupled transistor (C) |
| 25  | RESET  |
| 27  | INT    |
| 29  | RD     |
| 31  | PSEN   |
| 24  | LED (not populated) |
| 26  | LED    |
| 28  | LED    |
| 30  | LED    |
| 32  | LED    |

### Build ###

```
PARALLEL_KBD_OPTS = -DCHAR_MASK=0xFF
```

## Digital LK01 ##

This is the main keyboard inside the VT05 display terminal and the LA32 printing terminal.

It comes in two variants. The earlier one, originally from Control Devices, uses
capacitive switches, but this proved to be unrealiable. The replacement uses a Stackpole
grid. The PCBs are generally similar and have compatible interfaces.

There is an LSI chip on the board that handles key scanning and mapping to a base
character.  After that, the terminal is [bit-paired](https://en.wikipedia.org/wiki/Bit-paired_keyboard),
clearing bits 6 &amp; 7 for `CTRL` and toggling bit 6 if bit 7 is set else bit 5 for `SHIFT`.
(See character chart [here](https://vt100.net/docs/vt05-rm/table1-1.html).) This chip needs -12V, but there
is a voltage converter in the keyboard itself.

They keyboard is connected to the terminal by a 40-pin ribbon cable. Only half of those are connected and there are only half as many again (ten) signals, each of which is wired to the two adjacent pins.

The schematic for the terminal is page 20/75 in [VT05 Engineering Drawings](http://bitsavers.org/pdf/dec/terminal/vt05/VT05_Engineering_Drawings_Jun71.pdf).
The schematic for a similar keyboard is page 13/19 of [LK40 Engineeering Drawings](http://www.bitsavers.org/pdf/dec/graphics/VT11/LK40_Engineering_Drawings_Dec77.pdf). This shows the connector pinout.

### Connections ###

| Berg  | Signal              | AVR |
|-------|---------------------|-----|
| A  B  | STROBE              | PD0 |
| K  L  | CHAR BIT 5          | PB4 |
| H  J  | CHAR BIT 6          | PB5 |
| C  D  | CHAR BIT 7          | PB6 |
| W  X  | GND                 | GND |
| Y  Z  | +5V                 | +5V |
| MM NN | CHAR BIT 3          | PB2 |
| PP RR | CHAR BIT 2          | PB1 |
| SS TT | CHAR BIT 4          | PB3 |
| UU VV | CHAR BIT 1          | PB0 |

### Build ###

```
PARALLEL_KBD_OPTS = -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING
```

## Consul 262.3 ##

A late Soviet Hall Effect keyboard.

* Even the keyboards with Cyrillic and Latin legends just send Latin characters.
* The arrow keys and numpad send characters with the high bit set.

Some scanned pages of documentation are in the [Wiki](https://github.com/MMcM/parallel-ascii-kbd/wiki).

### Connections ###

| Signal |  Pin  | AVR |
|--------|-------|-----|
| KD0    |   6   | PB0 |
| KD1    |  12   | PB1 |
| KD2    |   7   | PB2 |
| KD3    |   8   | PB3 |
| KD4    |  11   | PB4 |
| KD5    |   9   | PB5 |
| KD6    |  10   | PB6 |
| KD7    |   4   | PB7 |
| STROB  |  16   | PD0 |
| +5V    |   2   | 5V  |
| ⏚      | 1,3,5 | GND |

### Build ###

```
PARALLEL_KBD_OPTS = -DCHAR_MASK=0xFF -DCHAR_INVERT
```
