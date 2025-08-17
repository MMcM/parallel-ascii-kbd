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
These were used in early hobbyist computers, including the original Apple (see [below](#apple-1)).

## Hardware ##

The board needs a lot of GPIO pins, particularly all of PORTB, so an actual Arduino is not a great choice.

* [Teensy 2.0](https://www.pjrc.com/teensy/index.html).
* ~~[Adafruit Atmega32u4 breakout](http://www.ladyada.net/products/atmega32u4breakout/)~~ (discontinued).
* [Pololu A-Star 32U4 Mini SV](https://www.pololu.com/product/3145).

### Connections ###

Required:

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

Optional:

| Signal              | AVR |
|---------------------|-----|
| DIRECT KEY 1        | PD1 |
| DIRECT KEY 2        | PD2 |
| ...                 |     |
| DIRECT KEY 7        | PD7 |
| DIRECT KEY 8        | PF0 |
| DIRECT KEY 9        | PF1 |
| ...                 |     |
| BELL                | PC6 |
| READY ACK           | PC7 |

The rest of port D is read for direct keys. Currently the only supported actions for these are `HERE IS`, which sends the answerback sdtring, and `BREAK`, which does serial break.

There are two optional signals in the to-keyboard direction. `C6` is a bell, either a speaker / transducer directly or something with a trigger signal. `C7` is a ready / ack line, which can be used to time a `REPEAT` key or to let the keyboard track serial `DTR`.

## Micro Switch SW-11234 ##

* Board: 55SW5-2
* Encoder: SW-20306 (AMI)

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

Vdd is +5V DC, for the control signals, both inputs from the Hall Effect switches and
character output. The CMOS encoder needs Vgg of [-12V](https://github.com/MMcM/parallel-ascii-kbd/wiki/Encoding#-12vdc-for-mos).

### Connections ###

| PCB | Signal              | AVR |
|-----|---------------------|-----|
| 2   | -12V                |     |
| 3   | +5V                 | +5V |
| 4   | GND                 | GND |
| 10  | /CONTROL            | PD3 |
| 12  | /SHIFT              | PD1 |
|     |                     |     |
| A   | /STROBE             | PD0 |
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
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SW-11234 Keyboard\"" \
  -DDIRECT_KEYS=3 -DDIRECT_PORT_UNUSED=4 -DDIRECT_INVERT_MASK=5
```

## Micro Switch SW-11769 ##

* Board: 55SW1-10
* Encoders: RW-10038, RW-10039

The same switches and keycaps as the previous, in a similar layout. Uses only 5V logic.
Again control and shift keys is also available separately,

The board I tested had some hand-modifications.

### Connections ###

| PCB | Signal              | AVR |
|-----|---------------------|-----|
| 3   | +5V                 | +5V |
| 4   | GND                 | GND |
| 5   | CTRL                | PD2 |
|     |                     |     |
| A   | STROBE              | PD0 |
| C   | +5V                 | +5V |
| D   | GND                 | GND |
| E   | SHIFT               | PD1 |
| F   | CHAR BIT 2          | PB1 |
| H   | CHAR BIT 1          | PB0 |
| J   | CHAR BIT 4          | PB3 |
| K   | CHAR BIT 3          | PB2 |
| L   | CHAR BIT 5          | PB4 |
| M   | CHAR BIT 6          | PB5 |
| N   | CHAR BIT 7          | PB6 |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SW-11769 Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING -DDIRECT_KEYS=2
```

## Micro Switch SD-16234 ##

* Board: 99SD24-3
* Encoder: 8748

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
| 22  | /STROBE             | PD0 |
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
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SD-16234 Keyboard\"" -DCHAR_MASK=0xFF
```

## Micro Switch SD-16534 ##

* Board: 91SD30-3
* Encoder: 8048

This keyboard seems to be part of a Honeywell BKBD terminal.

Connection to the keyboard is through a DB-25 (with a special cable not all of whose signals are connected, though there would be no harm if they were).

### Connections ###

Needed for this:

| IDC | Signal              | AVR |
|-----|---------------------|-----|
|6,7,8|
|9    | GND                 | GND |
|10,11|
|12,13| +5V                 | +5V |
|     |                     |     |
|  4  | STROBE              | PD0 |
| 19  | CHAR BIT 1          | PB0 |
| 20  | CHAR BIT 2          | PB1 |
| 18  | CHAR BIT 3          | PB2 |
| 17  | CHAR BIT 4          | PB3 |
| 16  | CHAR BIT 5          | PB4 |
| 15  | CHAR BIT 6          | PB5 |
| 14  | CHAR BIT 7          | PB6 |
| 23  | CHAR BIT 8          | PB7 |
| 21  | /DATA SET READY     | PC7 |
|  2  | SPEAKER             | PC6*|

Note that the two low bits are reversed from what one might expect.

The DSR line can just be wired low. If connected to `PC7` and `READY_ACK_MODE` is defined as `READY_ACK_MODE_DTR`, it will be turned on as part of initialization, which causes the keyboard to send some kind of identification sequence.

The speaker connection should have a current-limiting resistor.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SD-16534 Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING -DCHAR_MASK=0xFF \
  -DREADY_ACK_MODE=READY_ACK_MODE_DTR -DREADY_ACK_ON_STATE=READY_ACK_ON_LOW
```

### Non-ASCII Variant ###

A very similar PCB, SD-16604, used on 108SD30-4, from a related terminal, has the same connections, with these direct switches:

| 23  | /BREAK              | PB7 |
| 25  | /SHIFT              | PD1 |
| 24  | /CAPS LOCK          | PD2 |
|  1  | /CTRL               | PD3 |
|  5  | /AUTO LF            | PD4 |
| 22  | /LOCAL              | PD5 |

However, the parallel signal on this board is the key number, left-to-right, top-to-bottom, and not ASCII, and unchanged for the shifts.

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SD-16604 Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING -DBELL_MODE=BELL_MODE_TONE \
  -DDIRECT_KEYS=5 -DDIRECT_INVERT_MASK=0x1F \
  -DREADY_ACK_MODE=READY_ACK_MODE_DTR -DREADY_ACK_ON_STATE=READY_ACK_ON_LOW \
  -DDEBUG_ACTIONS
```

## Micro Switch SC-15142 ##

* Board: 63ST13-1
* Encoder: 8048 (firmware SD-03041)

This keyboard seems to be part of a printing terminal.

Connection to the keyboard is through a 20-pin IDC shrouded header.

### Connections ###

| IDC | Signal              | AVR |
|-----|---------------------|-----|
|  1  | DEBOUNCE(?)         | PD3 |
|  4  | CHAR PARITY BIT     | PB7 |
|  5  | /BRK                | PD2 |
|  6  | INPUT(?)            | PC7 |
|  7  | /HERE IS            | PD1 |
|  8  | CHAR BIT 1          | PB0 |
| 10  | CHAR BIT 2          | PB1 |
| 11  | STROBE              | PD0 |
| 12  | CHAR BIT 3          | PB2 |
|13,15| +5V                 | +5V |
|14,16| GROUND              | GND |
| 17  | CHAR BIT 7          | PB6 |
| 18  | CHAR BIT 4          | PB3 |
| 19  | CHAR BIT 6          | PB5 |
| 20  | CHAR BIT 5          | PB4 |

The third direct signal seems to change several times for each key press.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SC-15142 Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING -DPARITY_CHECK=PARITY_ODD \
  -DDIRECT_KEYS=3 -DDIRECT_INVERT_MASK=7 -DENABLE_SOF_EVENTS -DDIRECT_DEBOUNCE=5 \
  -DDIRECT_KEY_1=DIRECT_HERE_IS -DDIRECT_KEY_2=DIRECT_BREAK
```

## Micro Switch SD-16192 ##

* Board: 78SD12-6
* Encoder: SW-20478K (AMI)

This keyboard is part of a terminal for the AM International AMText word processor. But many of the key legends do not match the character sent. This could be because it is used elsewhere or just due to the encoder chip.

Has a 20-pin edge connector.

Needs [-12V](https://github.com/MMcM/parallel-ascii-kbd/wiki/Encoding#-12vdc-for-mos).

### Connections ###

| IDC | Signal              | AVR |
|-----|---------------------|-----|
|  A  | CHAR BIT 0          | PB0 |
|  B  | CHAR BIT 1          | PB1 |
|  C  | CHAR BIT 2          | PB2 |
|  D  | CHAR BIT 3          | PB3 |
|  E  | CHAR BIT 4          | PB4 |
|  F  | CHAR BIT 5          | PB5 |
|  H  | CHAR BIT 6          | PB6 |
|  J  | CHAR BIT 7          | PB7 |
|  K  | DIRECT KEY 2        | PD2 |
|  L  | DIRECT KEY 3        | PD3 |
|  1  | DIRECT KEY 4        | PD4 |
|  2  | /STROBE             | PD0 |
|  3  | STROBE              |     |
|  4  | -12V                |     |
|  5  | TEST POINT P5       |     |
|  6  | DIRECT KEY 1 (RESET)| PD1 |
| 7,8 | +5V                 | VCC |
|9,10 | GROUND              | GND |

Most keys are 4B3B, 2.5 oz. sink pulse. The `RESET` key in the lower-left is 4B8B, 8.0 oz., but still a pulse, which is close to synchronized with the `STROBE` pulse. The three keys in the upper-right are 4B3K, timed repeat, so those signals remain on while the key is still pressed. All these direct keys also send a unique character code, so other than auto-repeat there is nothing gained by tracking them separately.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SD-16234 Keyboard\"" -DCHAR_MASK=0xFF \
  -DDIRECT_KEYS=4 -DDIRECT_INVERT_MASK=0xF -DDIRECT_KEY_1=DIRECT_BREAK
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
PARALLEL_KBD_OPTS = -DKEYBOARD="\"LK01 Keyboard\"" -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING
```

## Consul 262.3 ##

* Encoder: Tesla MH103

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
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Consol 262.3 Keyboard\"" -DCHAR_MASK=0xFF -DCHAR_INVERT
```

## Beehive B100 ##

The keyboard interface is described in the [Beehive B100 Computer Terminal - Maintenance Manual](http://www.computinghistory.org.uk/downloads/32424). Schematic 9 of 16 of the terminal gives the signals on the keyboard connector, an Augat 18-pin DIP.

### Connections ###

| DIP | Signal        | AVR |
|-----|---------------|-----|
| 1,9 | +5V           | VCC |
|11,18| GND           | GND |
|     |               |     |
|  8  | KB IN 1       | PB0 |
|  7  | KB IN 2       | PB1 |
|  6  | KB IN 3       | PB2 |
|  5  | KB IN 4       | PB3 |
|  4  | KB IN 5       | PB4 |
|  3  | KB IN 6       | PB5 |
|  2  | KB IN 7       | PB6 |
|     |               |     |
| 15  | /KEYSTROBE    | PD0 |
| 14  | /INTERNAL OPN | PD1 |
| 12  | /BREAK EN     | PD2 |
| 10  | /INSERT MODE  | PD3 |
| 17  | /AUX EN       | PD4 |
| 16  | /AUX ONL      | PD5 |
| 13  | /RESET        | PD7 |

* `INTERNAL OPN` is on in conjunction with the arrow and other navigation keys, which send the VT52 `ESC` code.
* `BREAK` is on for a set period, around 1/4 sec, no matter how long it is held down.
* The three keys above `ALPHA LOCK` toggle mode LEDs underneath and the next three signals, whether or not the optional window key caps, `INST CHAR`, `AUX ENBL`, `AUX ONLN`, are present.
* `RESET` is triggered by pressing `CTRL`, `SHIFT`, and `CLEAR HOME` together.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Beehive B100 Keyboard\"" \
  -DDIRECT_KEYS=7 -DDIRECT_INVERT_MASK=0x5F \
  -DDIRECT_ESC_PREFIX_MASK=1 -DDIRECT_ESC_PREFIX_VT100 \
  -DDIRECT_KEY_2=DIRECT_BREAK
```

## Amkey SNK-58 ##

Part of some kind of (T-Bar?) EIA test equipment. The keys legends have the ASCII CTRL character name on the corresponding key.

### Command Keys ###

These blue keys send codes with the eight it set.

| Legend   | Code |
|----------|------|
| HEX PAIR | 81   |
| CMND     | 82   |
| STEP     | 83   |
| RUN      | 84   |
| CLEAR    | 85   |
| <-       | 86   |
| STOP     | 87   |

### Connections ###

Connects with a 20-pin ribbon cable with a key pin missing.

The side opposite the key pin has power: starting from that end, two pins of +5VDC and eight pins of ground.

The keyed side has, after the missing pin, the strobe signal (idle high), then eight data bits in order 4, 5, 6, 7, 3, 2, 1, 0.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Amkey SNK-58 Keyboard\"" -DCHAR_MASK=0xFF
```

## Apple II / II Plus ##

Earlier versions of the keyboard had an NSC MM5740 encoder chip. This was replaced by a separate encoder daughter board with an SMC KR3600. (A keyboard without the daughter card requires scanning the matrix directly through the 26-pin connector, which is a different project.)

Note that the encoders do not generate lowercase characters. In the case of MM5740, this is a restriction of the chip. The KR3600 has 10 output pins to allow for selecting alphabetic shifting. And the encoder board has two jumpers that can be cut and a toggle switch installed to go from `B5` and `B6` to `B9` and `B8` for `K4` and `K5` and thereby get both cases.

Some sources:
* [Understanding the Apple II](https://archive.org/details/understanding_the_apple_ii/page/n167/mode/2up)
* [Apple II/II+ Keyboard Socket](http://wiki.apple2.org/index.php?title=Pinouts#Apple_II.2FII.2B_Keyboard_Socket)
* [Early Apple Keyboards](http://www.willegal.net/appleii/early-a2-keyboards.htm)

### Connections ###

A ribbon cable connected to a 16-pin DIP on the motherboard.

The encoder also needs [-12V](https://github.com/MMcM/parallel-ascii-kbd/wiki/Encoding#-12vdc-for-mos).

| DIP | Signal       | AVR |
|-----|--------------|-----|
|  1  | +5V          | VCC |
|  2  | STROBE       | PD0 |
|  3  | /RESET       | PD1 |
|  4  |              |     |
|  5  | K5           | PB5 |
|  6  | K4           | PB4 |
|  7  | K6           | PB6 |
|  8  | GROUND       | GND |
|  9  |              |     |
| 10  | K2           | PB2 |
| 11  | K3           | PB3 |
| 12  | K0           | PB0 |
| 13  | K1           | PB1 |
| 14  |              |     |
| 15  | -12V         |     |
| 16  |              |     |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Apple II Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING \
  -DDIRECT_KEYS=1 -DDIRECT_INVERT_MASK=1 -DDIRECT_DEBOUNCE=5 -DENABLE_SOF_EVENTS \
  -DDIRECT_KEY_1=DIRECT_BREAK
```

### Apple 1 ###

The original Apple also connected to the keyboard with a 16 pin DIP (`B4`) and was not manufactured with a keyboard of its own (see [Operation Manual](https://archive.org/details/Apple-1_Operation_Manual_1976_Apple_a/page/n2)). The pinout is not the same, however. It additionally needs +12V.

| DIP | Signal       | AVR |
|-----|--------------|-----|
|  1  | RESET        | PD1 |
|  2  | B4           | PB3 |
|  3  | B3           | PB2 |
|  4  | B2           | PB1 |
|  5  | B1           | PB0 |
|  6  | B5           | PB4 |
|  7  | B6           | PB5 |
|  8  | B7           | PB6 |
|  9  | GROUND       | PB4 |
| 10  | +12V         |     |
| 11  | -12V         |     |
| 12  | CLEAR SCREEN | PD2 |
| 13  |              |     |
| 14  | STROBE       | PD0 |
| 15  | B8           | PB7 |
| 16  | +5V          | VCC |

Examples:

* Early Datanetics keyboards (see [notes](https://www.applefritter.com/node/2809) they took from Apple).

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Apple I Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING \
  -DDIRECT_KEYS=2 -DDIRECT_INVERT_MASK=1 -DDIRECT_DEBOUNCE=5 -DENABLE_SOF_EVENTS \
  -DDIRECT_KEY_1=DIRECT_BREAK -DDIRECT_KEY_2=DIRECT_HERE_IS
```

## Maxi-Switch 216004 ##

* Encoder: SMC 3603, which seems to be the same as the KR3600.

### Connections ###

Has an 18-pin edge connector.

| Pin | Signal        | AVR |
|-----|---------------|-----|
| A   | K44 (BRK)     | D1  |
| B   | K44 (BRK)     | GND |
| C   | K14 (HERE IS) | D2  |
| D   | K14 (HERE IS) | GND |
| E   | REPT ENABLE   |     |
| F   | STROBE        | D0  |
| H   | B9            | *   |
| J   | B7            | B6  |
| K   | B6            | B5* |
| L   | B5            | B4  |
| M   | B4            | B3  |
| N   | B3            | B2  |
| P   | B2            | B1  |
| R   | B1            | B0  |
| S   | VCC           | VCC |
| T   | VDD           | GND |
| U   |               |     |
| V   |               |     |

A SPDT switch can be installed to select between encoder `B9` and `B6` for output `B5` allows choosing between uppercase-only and upper- and lowercase.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Maxi-Switch 216004 Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING \
  -DDIRECT_KEYS=2 -DDIRECT_INVERT_MASK=3 -DENABLE_SOF_EVENTS -DDIRECT_DEBOUNCE=5 \
  -DDIRECT_KEY_1=DIRECT_BREAK -DDIRECT_KEY_2=DIRECT_HERE_IS
```

## Jameco JE610 ##

* Encoder: GI AY-5-2376

Another keyboard sold to early hobbyists.
([Datasheet](http://www.bitsavers.org/pdf/jameco/Jameco_JE_610_ASCII_Keyboard_Datasheet.pdf)).

Output is via a 16-pin DIP (`J1`) and an 18-pin edge connector (`P1`).

### Connections ###

| J1 | P1 | Signal       | AVR |
|----|----|--------------|-----|
|  1 |  3 | NEG STROBE   | PD0 |
|  2 |  5 | D5           | PB5 |
|  3 |  7 | D0           | PB0 |
|  4 |  9 | D1           | PB1 |
|  5 | 11 | D2           | PB2 |
|  6 | 13 | D3           | PB3 |
|  7 | 15 | D4           | PB4 |
|  8 |1718| GROUND       | GND |
|  9 | 16 | -12V         |     |
| 10 | 14 | UD1          | PD1 |
| 11 | 12 | UD2          | PD2 |
| 12 | 10 | D6           | PB6 |
| 13 |  8 | D7           | PB7 |
| 14 |  6 | PARITY       |     |
| 15 |  4 | POS STROBE   |     |
| 16 | 1,2| +5V          | VCC |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"JE610 Keyboard\"" \
  -DDIRECT_KEYS=2 -DENABLE_SOF_EVENTS -DDIRECT_DEBOUNCE=5 \
  -DDIRECT_KEY_1=DIRECT_ANSWERBACK_2 -DANSWERBACK_2="\"UD1\\r\\n\"" \
  -DDIRECT_KEY_2=DIRECT_ANSWERBACK_3 -DANSWERBACK_3="\"UD2\\r\\n\""
```

## TEC EKA-9100 ##

([Schematic](https://archive.org/details/bitsavers_tec9650100awingsSep1974_20767226/page/n34/mode/1up)).
There is no program logic on this board: it is all done with 74-series TTL.

The keyboard connects via a DC-37 D-sub connector, with only 17 (15 distinct) signals.

### Connections ###

| DC | Signal      | AVR |
|----|-------------|-----|
|  1 | +5V         | VCC |
|  2 | +5V         | VCC |
|  6 | /KBDACK     | PC7 |
| 11 | DATA 1      | PB0 |
| 12 | DATA 2      | PB1 |
| 13 | DATA 3      | PB2 |
| 14 | DATA 4      | PB3 |
| 15 | DATA 5      | PB4 |
| 16 | DATA 6      | PB5 |
| 17 | DATA 7      | PB6 |
| 18 | /STROBE     | PD0 |
| 19 | /CLRSCN     | PD1 |
| 26 | /BELL       | PC6 |
| 27 | /BREAK      | PD2 |
| 31 | GND         | GND |
| 33 | CHASSIS GND |     |
| 36 | GND         | GND |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"TEC EKA Keyboard\"" \
  -DBELL_MODE=BELL_MODE_LOW \
  -DREADY_ACK_MODE=READY_ACK_MODE_KEY_ACK -DREADY_ACK_ON_STATE=READY_ACK_ON_LOW -DREADY_ACK_DELAY_MSEC=250 -DENABLE_SOF_EVENTS \
  -DDIRECT_KEYS=2 -DDIRECT_INVERT_MASK=3 -DDIRECT_DEBOUNCE=5 \
  -DDIRECT_KEY_1=DIRECT_HERE_IS -DDIRECT_KEY_2=DIRECT_BREAK
```

## Scientific Devices ##

Encoder: GI

This keyboard has no model number, although very similar looking ones are labeled KBMO two, suggesting that this might be KBMO one.
See, for instance, [here](http://www.vcfed.org/forum/showthread.php?68827-Are-these-parallel-ASCII&p=567157#post567157) and
[here](http://retro.hansotten.nl/kim-1-rev-b-ceramic-cpu-ascii-display-ascii-keyboard-brutech-4k-ram/).

Has a 24-pin edge connector, which is numbered 1-12 and 13-24 rather than A-N.

There are 7 LEDs lit directly by the data output signals. They therefore show the (LSB left, inverted) ASCII character while the key is help down.

Needs [-12V](https://github.com/MMcM/parallel-ascii-kbd/wiki/Encoding#-12vdc-for-mos) for the CMOS keyboard decoder.

### Connections ###

| Pin | Signal                            | AVR |
|-----|-----------------------------------|-----|
|  1  | -12V                              | VCC |
|  2  | DATA 1                            | PB0 |
|  3  | DATA 2                            | PB1 |
|  4  | DATA 3                            | PB2 |
|  5  | DATA 4                            | PB3 |
|  6  | DATA 5                            | PB4 |
|  7  | DATA 6                            | PB5 |
|  8  | DATA 7                            | PB6 |
|  9  | DIRECT 12 (R4 right, HERE IS)     | PF4 |
| 10  | DIRECT 13 (R3 right)              | PF5 |
| 11  | DIRECT 14 (R2 right)              | PF6 |
| 12  | DIRECT 15 (R1 right, BREAK)       | PF7 |
| 13  | /STROBE                           | PD0 |
| 14  | STROBE                            |     |
| 15  | GND                               | GND |
| 16  | +5V                               | VCC |
| 17  | GND                               | GND |
| 18  | GND                               | GND |
| 19  | DIRECT 1 (R1 second right, CLEAR) | PD1 |
| 20  | DIRECT 2 (R2 second right)        | PD2 |
| 21  | DIRECT 3 (R3 third right, RETURN) | PD3 |
| 22  | DIRECT 4 (R3 second right)        | PD4 |
| 23  | DIRECT 5 (R4 second right)        | PD5 |
| 24  | DIRECT 7 (R1 left)                | PD7 |

On this board, the trace for pin 21 has been cut and wires added so that that switch decodes from the matrix as ASCII `CR`.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Scientific Devices Keyboard\"" \
  -DDIRECT_KEYS=15 -DDIRECT_INVERT_MASK=0x7FFF -DENABLE_SOF_EVENTS -DDIRECT_DEBOUNCE=5 \
  -DDIRECT_KEY_12=DIRECT_HERE_IS -DDIRECT_KEY_15=DIRECT_BREAK \
  -DDIRECT_KEY_1=DIRECT_ANSWERBACK_2 -DANSWERBACK_2="\"Goodbye\\r\\n\""
```

## Maxi-Switch 2160094 ##

* Encoder: MCS-48 (firmware MS 0319019).

### Connections ###

Has 16-pin DIP and 18-pin edge connectors.

|DIP | Edge | Signal        | AVR |
|----|------|---------------|-----|
|  1 |  A   | K45           | D1  |
|  2 |  B   | K45           | GND |
|  3 |  C   | K15           | D2  |
|  4 |  D   | K15           | GND |
|  5 |  E   | /REPT         |     |
|  6 |  F   | /STROBE       | D0  |
|  7 |  H   | B5            | B5  |
|  8 |  J   | B6            | B6  |
|  9 |  K   | B4            | B4  |
| 10 |  L   | B3            | B3  |
| 11 |  M   | B2            | B2  |
| 12 |  N   | B1            | B1  |
| 13 |  P   | B0            | B0  |
| 14 |  R   | VCC           | VCC |
| 15 |  S   | GND           | GND |
| 16 |      | N/C           |     |
|    |  T   | JUMPER        |     |
|    |  U   | JUMPER        |     |
|    |  V   | JUMPER        |     |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Maxi-Switch 2160094 Keyboard\"" -DCHAR_INVERT
```

## Xerox 820 ASCII ##

Encoder: NEC 8048

The X928 ASCII only Maxi-Switch variant of the 820 keyboard pictured [here](http://www.bitsavers.org/pdf/xerox/820-II/pictures/820_Maxi-Switch_KBD/).

### Connections ###

Has a DB-25 connector.

| J2  | Signal  | AVR |
|-----|---------|-----|
|  1  | DB0     | B0  |
|  2  | DB1     | B1  |
|  3  | DB2     | B2  |
|  4  | DB3     | B3  |
|  5  | DB4     | B4  |
|  6  | DB5     | B5  |
|  7  | DB6     | B6  |
|  8  | DB7     | B7  |
|  9  | /STROBE | D0  |
|10-12| N/C     |     |
| 13  | +5V     | +5V |
|14-22| GND     | GND |
|23,24| N/C     |     |
| 25  | GND     | GND |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Xerox 820 ASCII Keyboard\""
```

## Datamedia 1520 ##

The PCB this was tested on is 3DAAA022, but similar ones should be the same. Encoding done with 74-series TTL.
([Schematics](https://archive.org/details/bitsavers_datamediaDechnicalManual_2838598/page/n39/mode/2up)).

Connection to the keyboard is through a DB-25 to a 30-pin edge connector.

The schematic for variants of this keyboard start on page 40 in [Datamedia Elite 1520A Technical Manual](http://bitsavers.org/pdf/datamedia/Datamedia_Elite_1520A_Technical_Manual.pdf).

### Connections ###

Needed for this:

| DB-25 | Edge | Signal     | AVR |
|-------|------|------------|-----|
|    21 |    1 | GND        | GND |
|    23 |    2 | +5V        | +5V |
|    22 |    3 | /RPT       | PD1 |
|     2 |    4 | /KBDTAPE   | PD2 |
|     4 |    5 |            |     |
|     6 |    6 |            |     |
|    18 |   13 | /BRK       | PD3 |
|       |   15 | /STRBDIS   |     |
|    14 |    A | GND        | GND |
|    16 |    B | +5V        | +5V |
|     1 |    C | /KB1       | PB0 |
|     3 |    D | /KB2       | PB1 |
|     5 |    E | /KB3       | PB2 |
|     7 |    F | /KB4       | PB3 |
|     9 |    H | /KB5       | PB4 |
|    11 |    J | /KB6       | PB5 |
|    13 |    K | /KB7       | PB6 |
|    20 |    L | /KBSTR     | PD0 |
|       |    M | /KBSTR     |     |
|    10 |    P | -12V       |     |
|     8 |    R |            | PD4 |
|    12 |    S |            |     |

-12V is only needed if the chip in slot 12 is present, which it was not in the one tested.

There is a jumper FF to connect switch 78 (LOAD TAPE) to GG switch 13 (TAPE), making them both (4), or alternatively EE to (R).
The default seems to be FF-GG, so both keys are the same.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Datamedia 1520\"" \
  -DCHAR_INVERT \
  -DDIRECT_KEYS=3 -DDIRECT_INVERT_MASK=7 -DENABLE_SOF_EVENTS -DDIRECT_DEBOUNCE=5 \
  -DDIRECT_KEY_3=DIRECT_BREAK
```

## Data General Dasher D2 ##

* Encoder: GI 20-04592-014, similar to the [AY-3-4592](https://archive.org/details/bitsavers_gidataBooknicsDataCatalog_36703971/page/n53/mode/2up).

Connection to the keyboard is through a 15-pin AMP connector to a 20-pin edge connector.

### Connections ###

The edge connector and the PCB use slightly different numbering conventions.

| AMP | Edge | PCB | Signal | AVR |
|-----|------|-----|--------|-----|
|   1 |    1 |   1 | GND    | GND |
|     |    A |   2 | GND    |     |
|   2 |    3 |   5 | /STB   | PD0 |
|   3 |    5 |   9 | D6     | PB5 |
|   4 |    C |   6 |        |     |
|   5 |    4 |   7 | D8     | PB7 |
|   6 |    D |   8 | D7     | PB6 |
|   7 |    6 |  11 | D4     | PB3 |
|   8 |    E |  10 | D5     | PB4 |
|   9 |    F |  12 | D3     | PB2 |
|  10 |    7 |  13 | D2     | PB1 |
|  11 |    H |  14 | D1     | PB0 |
|  12 |    K |  18 | D10    | PD1 |
|  13 |   10 |  19 | PRE    | VCC |
|  14 |    L |  20 | /BUSY  | VCC |
|  15 |    2 |   3 | VCC    | VCC |

The D10 bit is only used for BREAK.
The two input pins (reset and strobe inhibit) can just be connected to Vcc.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Dasher D2\"" \
  -DCHAR_MASK=0xFF \
  -DDIRECT_KEYS=1 -DENABLE_SOF_EVENTS -DDIRECT_INVERT_MASK=1 -DDIRECT_KEY_1=DIRECT_BREAK
```

## IDA K100 BE ##

* Encoder: 6805

A keyboard for the Apple II (and clones). Has function keys for sending various BASIC-language related macros.

A scan of the manual that came with this keyboard can be found in the
[wiki](https://github.com/MMcM/micro-switch-ascii-kbd/wiki/scanned/k-100.pdf).
The key legends do not always match.

### Connections ###

Keyboard itself connects to a DA-15. The keyboard came with some external boards to adapt to a 16-pin DIP.

| DIP | DA15 | PCB | Signal       | AVR |
|-----|------|-----|--------------|-----|
|  1  | 10   | 11  | +5V          | +5V |
|  2  |  6   | 10  | STB          | PD0 |
|  3  | 11   |  2  | S16 (RST)    | PD1 |
|  4  |      |     |              |     |
|  5  |  1   |  4  | D5           | PB5 |
|  6  |  3   |  5  | D4           | PB4 |
|  7  |  2   |  3  | D6           | PB6 |
|  8  |  7   |  1  | GND          | GND |
|  9  | 12   | 12  | S85 (filled) | PD2 |
| 10  | 14   |  7  | D2           | PB2 |
| 11  |  4   |  6  | D3           | PB3 |
| 12  | 15   |  9  | D0           | PB0 |
| 13  | 13   |  8  | D1           | PB1 |
| 14  | 12   | 12  |              |     |
| 15  |      |     | -12V (N/A)   |     |
| 16  |  9   | 13  | S83 (hollow) | PD3 |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"K100 Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING \
  -DDIRECT_KEYS=3 -DDIRECT_INVERT_MASK=7 -DDIRECT_DEBOUNCE=5 -DENABLE_SOF_EVENTS \
  -DDIRECT_KEY_1=DIRECT_BREAK
```

## Micro Switch SD-16046 ##

* Board: 78SD5-5
* Encoder: SW-20293K (AMI)

This is the keyboard for the IBM PT-2 terminal; photos on [Bitsavers](http://www.bitsavers.org/pdf/ibm/PT-2/keyboard/), perhaps from an eBay listing.

<b>Note</b>: this keyboard is <em>not</em> ASCII, but built more or less the same as ones that are.

Needs [-12V](https://github.com/MMcM/parallel-ascii-kbd/wiki/Encoding#-12vdc-for-mos).

### Connections ###

A 16 pin header with one pin missing.

| PCB | Signal              | AVR |
|-----|---------------------|-----|
| 1   | -12V                |     |
| 2   | GND                 | GND |
| 3   | +5V                 | +5V |
| 5   | /STROBE             | PD0 |
| 6   |                     | PD1 |
| 7   | CHAR BIT 8          | PB7 |
| 8   | CHAR BIT 7          | PB6 |
| 9   | CHAR BIT 6          | PB5 |
| 10  | CHAR BIT 5          | PB4 |
| 11  | CHAR BIT 4          | PB3 |
| 12  | CHAR BIT 3          | PB2 |
| 13  | CHAR BIT 2          | PB1 |
| 14  | CHAR BIT 1          | PB0 |
| 15  | CHAR PARITY BIT     | PD2 |
| 16  | /KEYBD RESET        | PD3 |

### Build ###

The `DEBUG_ACTIONS` is because this isn't really ASCII.

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"IBM PT-2\"" \
  -DCHAR_MASK=0xFF \
  -DDIRECT_KEYS=3 -DDIRECT_INVERT_MASK=7 -DDIRECT_KEY_3=DIRECT_BREAK \
  -DDEBUG_ACTIONS
```

## Micro Switch SD-16614 ##

* Board: 66SD5-1
* Encoder: SW-20411K (AMI)

Needs [-12V](https://github.com/MMcM/parallel-ascii-kbd/wiki/Encoding#-12vdc-for-mos).

### Connections ###

Has a 30-pin edge connector.

| Edge | Signal           | AVR |
|------|------------------|-----|
| 1    | +5V              | +5V |
| 5    | CHAR PARITY BIT  | PB7 |
| 6    | CHAR BIT 1       | PB0 |
| 7    | CHAR BIT 2       | PB1 |
| 8    | CHAR BIT 3       | PB2 |
| 9    | CHAR BIT 4       | PB3 |
| 10   | CHAR BIT 5       | PB4 |
| 11   | CHAR BIT 6       | PB5 |
| 12   | CHAR BIT 7       | PB6 |
| 13   | CHAR BIT 8       |     |
| A    | GND              | GND |
| B    | -12V             |     |
| F    | KEY PRESS        | PD0 |
| H    | /STROBE          |     |
| J    | SHIFT            | PD1 |
| K    | CTRL             | PD2 |
| N    | SW 94 (CONTINUE) | PD3 |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SD-16614 Keyboard\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING -DPARITY_CHECK=PARITY_ODD \
  -DDIRECT_KEYS=3
```

## Micro Switch SW-11373 ##

* Board: 74SW12-5
* Encoder: SW-20426K (AMI)

Needs [-12V](https://github.com/MMcM/parallel-ascii-kbd/wiki/Encoding#-12vdc-for-mos).

### Connections ###

Has a 36-pin edge connector. This connects to a cable with a DA-15.

| Edge | DA15 | Color  | Signal             | AVR |
|------|------|--------|--------------------|-----|
| A,1  | 15   | Red    | +5V                | +5V |
| B,2  | 14   | Black  | GND                | GND |
|  C   | 10   | Blue   | -12V               |     |
|  T   |      |        | SW 37 (SHIFT LOCK) |     |
|  U   |      |        | PARITY             |     |
|  V   |      |        | SW 56 (SHIFT)      |     |
|  5   |  4   | Orange | /CHAR BIT 4        | PB3 |
|  6   |  3   | Red    | /CHAR BIT 3        | PB2 |
|  7   |  2   | Brown  | /CHAR BIT 2        | PB1 |
|  8   |  1   | Black  | /CHAR BIT 1        | PB0 |
| 10   |  5   | Yellow | /CHAR BIT 5        | PB4 |
| 11   |  6   | Green  | /CHAR BIT 6        | PB5 |
| 12   |  7   | Blue   | /CHAR BIT 7        | PB6 |
| 15   |  9   | Violet | /STROBE            | PD0 |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SW-11373 Keyboard\"" -DCHAR_INVERT
```

## Micro Switch SW-10034 ##

* Boards: 59SW1-2, 64SW1-2
* Encoders: RW-10038, RW-10039

### Connections ###

Has a 24-pin edge connector, whose top is the parallel output and the bottom is direct keys.

One example has a cable ending in a DC-37 D-sub connector and no direct outputs.

A scan of the product sheet that came with one can be found in the
[wiki](https://github.com/MMcM/micro-switch-ascii-kbd/wiki/scanned/59sw1-2.pdf).

| DC | Edge | Color              | Signal      | AVR |
|----|------|--------------------|-------------|-----|
| 27 |  1   | Black              | GND         | GND |
| 35 |  2   | White w/ Red       | +5V         | +5V |
| 31 |  4   | Orange             | /CHAR BIT 4 | PB3 |
| 30 |  5   | Red                | /CHAR BIT 3 | PB2 |
| 29 |  6   | Violet White Black | /CHAR BIT 2 | PB1 |
| 28 |  7   | White w/ Black     | /CHAR BIT 1 | PB0 |
| 12 |  8   | Violet             | PARITY      | PB7 |
| 13 |  9   | White w/ Violet    | /CHAR BIT 7 | PB6 |
| 14 | 10   | Green              | /CHAR BIT 6 | PB5 |
| 32 | 11   | White w/ Green     | /CHAR BIT 5 | PB4 |
|  9 | 12   | White              | /STROBE     | PD0 |
|    |  N   |                    | SHIFT       | PD1 |
|    |  L   |                    | CTRL        | PD2 |
|    |  K   |                    | RPT         | PD3 |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"SW-10034 Keyboard\"" \
  -DCHAR_INVERT -DPARITY_CHECK=PARITY_ODD
  -DDIRECT_KEYS=3
```

## Licon 80-551077 ##

* Board: Cortron 55-500551
* Encoder: 80-551095

Needs [-12V](https://github.com/MMcM/parallel-ascii-kbd/wiki/Encoding#-12vdc-for-mos).

### Connections ###

Has a 36-pin edge connector.

| Edge | Signal      | AVR |
|------|-------------|-----|
|   1  | CHAR BIT 3  | PB2 |
|   2  | CHAR BIT 2  | PB1 |
|   3  | CHAR BIT 1  | PB0 |
|   4  | CHAR BIT 4  | PB3 |
|   5  | KEY PRESSED |     |
|   8  | /STROBE     | PD0 |
|  12  | REPT        | PD1 |
|  13  | HERE IS (?) | PD4 |
|  18  | +5V         | +5V |
|   A  | CHAR BIT 5  | PB4 |
|   B  | CHAR BIT 6  | PB5 |
|   C  | /MCA TERM   | PD2 |
|   H  | CHAR BIT 7  | PB6 |
|   P  | BRK         | PD3 |
|   T  | -12V        |     |
|   U  | GND         | GND |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Licon 80-551077 Keyboard\"" \
  -DDIRECT_KEYS=4 -DDIRECT_INVERT_MASK=0xD -DDIRECT_KEY_3=DIRECT_BREAK -DDIRECT_KEY_4=DIRECT_HERE_IS
```

## Key Tronic 65-0627 ##

Magnetic reed. There is no program logic on this board: it is all done with 74-series TTL.

### Connections ###

Has a 36-pin edge connector. This connects to a cable with a DB-25.

| Edge | DB | Color          | Signal      | AVR |
|------|----|----------------|-------------|-----|
|  A   |  5 | White w/ Black | CHAR BIT 5  | PB4 |
|  B   |  6 | Green          | CHAR BIT 6  | PB5 |
|  C   | 19 | Black w/ Red   | (no switch) | PD3 |
|  D   | 20 | Blue w/ Red    | (no switch) | PD4 |
|  E   | 21 | Red w/ White   | (no switch) | PD5 |
|  H   |  7 | Orange w/ Red  | CHAR BIT 7  | PB6 |
|  P   | 22 | Black w/ White | /BRK        | PD2 |
|  U   | 14 | Black          | GROUND      | GND |
|  1   |  3 | Green w/ White | CHAR BIT 3  | PB2 |
|  2   |  2 | White w/ Red   | CHAR BIT 2  | PB1 |
|  3   |  1 | Blue           | CHAR BIT 1  | PB0 |
|  4   |  4 | Blue w/ Black  | CHAR BIT 4  | PB3 |
|  8   | 15 | Red w/ Green   | /STROBE     | PD0 |
| 13   | 17 | Green w/ Black | /HERE IS    | PD1 |
| 18   | 18 | Orange         | +5V         | +5V |
|      | 13 | Blue w/ White  | BUZZER      |     |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"KTC 65-0627 Keyboard\"" \
  -DDIRECT_KEYS=5 -DDIRECT_INVERT_MASK=0x1F -DDIRECT_KEY_1=DIRECT_HERE_IS -DDIRECT_KEY_2=DIRECT_BREAK
```

## Incoterm K2x ##

* Board: Incoterm K26-02

See the [Field Engineering Technical Manual](https://archive.org/details/bitsavers_incotermFiicalManualSPD1020LogicDiagrams1972_25812785/page/n139/mode/2up).

### Connections ###

Has a DB-25 connector on the PCB.
A 9-pin connector inside the case connects to a row of eight GE 238 bulbs.

| DB | Signal     | AVR |
|  1 | CHAR BIT 1 | PB0 |
|  2 | CHAR BIT 2 | PB1 |
|  3 | CHAR BIT 3 | PB2 |
|  4 | CHAR BIT 4 | PB3 |
|  5 | CHAR BIT 5 | PB4 |
|  6 | CHAR BIT 6 | PB5 |
|  7 | CHAR BIT 7 | PB6 |
|  8 | /MODE      | PD1 |
|  9 | /STROBE    | PD0 |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Incoterm Keyboard\"" -DDIRECT_KEYS=1
```

## The Keyboard Company Numeric Keypad ##

An external numpad for the Apple II.

### Connections ###

The keypad itself has a 14-pin Molex connector. This, in turn, connects to a daughter card with two 16-pin DIP connectors, one male and one female, and a pair of buffers chips. See [Deskthority Wiki](https://deskthority.net/wiki/The_Keyboard_Company_Numeric_Keypad) for a photo.

In this way, the keypad passes through other ASCII codes from the main keyboard, as well as adding the numpad ones.

| P1 | Color           | Signal    | AVR |
|----|-----------------|-----------|-----|
|  1 | Violet          | Ground    | GND |
|  2 | Blue            | D5        | PB5 |
|  3 | Gray            | D4        | PB4 |
|  4 | White           | D3        | PB3 |
|  5 | Red             | D0        | PB0 |
|  6 | Orange          | D1        | PB1 |
|  7 | White w/ Gray   | D2        | PB2 |
|  8 | White w/ Brown  |           |     |
|  9 | Black           | +5V       | +5V |
| 10 | Brown           | STB OUT   | PD0 |
| 11 | Green           | KEY DOWN  |     |
| 12 | Yellow          | *KEY DOWN |     |
| 13 | White w/ Red    |           |     |
| 14 | White w/ Orange | STB IN    |     |

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"TKC Numpad\"" \
  -DCONTROL_STROBE_TRIGGER=TRIGGER_RISING -DCHAR_MASK=0x3F
```

## Writehander ##

An ASCII chording keyboard. See [1978 Interface Age](https://archive.org/details/197801InterfaceAgeV03I01/page/56/mode/2up).
Four chorded buttons control the low four bits; one of four other buttons controls the high bits and sends the character.

The version described there and [here](http://cini.classiccmp.org/pdf/kilobaud/Pet%20User%20Port%20Cookbook%20(0379).pdf) has an ACK signal, which the one here lacks. Instead, it has a key down signal and instead of jumpers brings out three signals with both polarities.

### Connections ###

The device comes with a 16-conductor 1mm ribbon cable.

|  # | Color  | Signal      | AVR |
|----|--------|-------------|-----|
|  1 | Brown  | CHAR BIT 1  | PB0 |
|  2 | Red    | +12V (NC)   |     |
|  3 | Orange | CHAR BIT 2  | PB1 |
|  4 | Yellow | Ground      | GND |
|  5 | Green  | CHAR BIT 3  | PB2 |
|  6 | Blue   | +5V         | +5V |
|  7 | Violet | CHAR BIT 4  | PB3 |
|  8 | Gray   | /KEY DOWN   |     |
|  9 | White  | CHAR BIT 5  | PB4 |
| 10 | Black  | KEY DOWN    |     |
| 11 | Brown  | CHAR BIT 6  | PB5 |
| 12 | Red    | /STROBE     |     |
| 13 | Orange | CHAR BIT 7  | PB6 |
| 14 | Yellow | STROBE      | PD0 |
| 15 | Green  | CHAR BIT 8  | PB7 |
| 16 | Blue   | /CHAR BIT 8 |     |

The high bit (parity) never changes; one can pick always high or always low; or just ignore it.

### Build ###

```
PARALLEL_KBD_OPTS = -DKEYBOARD="\"Writehander\""
```
