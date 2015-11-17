# Device Documentation
The `Double Touch` device implements a double-sided, fMRI-safe touch input device.  Electronic and/or ferromagnetic components are separated from the magnet by an extension cord (10+ meters of CAT-7 cable), allowing the `Adapter Box` to reside in the control room with the experimenter's computers and other non-MRI-safe equipment.

This project revolves around a [Teensy 3.1](http://www.pjrc.com/teensy/teensy31.html) microcontroller, programmed in C++ using [PlatformIO](http://platformio.org/).

### Hacking
To build/upload the microcontroller code (OSX 10.11.1 "El Capitan"):

1. Install [PlatformIO](http://platformio.org/#!/get-started)
2. `platformio run` from inside the `DoubleTouch/` folder and answer `y` if it asks you to install the Teensy platform

## Hardware Configuration
> **WARNING!**  Although a normal CAT-7 ethernet cable is used to make the electrical connection between the touch panels and the Adapter box, it *does not* use the same signal levels as ethernet, and mixing the two could be destructive for all devices involved.

The Teensy manages two [resistive-touch-screen controllers](http://www.adafruit.com/products/1571) over its SPI bus.

#### Inside the Adapter Enclosure
Teensy and STMPE610 connections are:

| Teensy Pin Name | Teensy Function | STMPE610 Pin              |
|----------------:|-----------------|---------------------------|
|              10 | CS (front)      | SDA/CS (front panel only) |
|              11 | DOUT            | SDI                       |
|              12 | DIN             | SDO                       |
|              13 | SCK             | SCL                       |
|              15 | CS (rear)       | SDA/CS (rear panel only)  |
|             GND | Ground          | GND                       |
|             VIN | Vin             | Vin                       |
|              -- | --              | MODE connects to 3Vo      |

#### Inside the Touch Device
X and Y pins for each touch panel connect to an RJ45 jack to allow the use of CAT-7 cable between the magnet room and the control room.

RJ45 Pinout (`f` stands for `Front`, `r` stands for `Rear`):

| RJ45 pin | Touch Overlay Side |
|---------:|--------------------|
|        1 | fy-                |
|        2 | fy+                |
|        3 | fx-                |
|        4 | rx+                |
|        5 | rx-                |
|        6 | fx+                |
|        7 | ry+                |
|        8 | ry-                |

> Note: opposing +/- connections form a twisted pair, intended to minimize noise and/or interference.

## Software Configuration
Modern operating systems usually provide drivers for both Serial-over-USB and Raw HID devices.  However, OSX users may have to [install FTDI drivers](https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/mac) (and reboot) in order to use the Serial interface.

## Communications Protocols
Both Serial-over-USB and Raw HID profiles are supported.  Messages are sent from the Adapter to the PC every 10ms.

#### Serial-over-USB
Message format is 6 values per line:

	fx fy fz rx ry rz

#### Raw HID
Message format is a 64-byte array:

	fx(MSB) fx(LSB) fz fy(MSB) fy(LSB) fz ... (remaining 58 bytes: all zeros)

> **Design Footnote**
> The original concept of this was to implement a USB Mouse, but this idea was scrapped due to the fact that a USB Mouse can only have one pointer at a time.  While it is possible to multiplex two "pointers" into a single mouse interface, I felt the extra complexity was not justified.  The most appropriate interface for this device is, in my opinion, a *multi-touch digitizer*.  This would expose functionality similar to that of a tablet computer, where two fingers can be used to zoom or scroll.
> 
> For now, a USB Digitizer is *not* supported because USB digitizer descriptors are not yet cross-platform, and implementing this device as a digitizer would tie users to a specific operating system.  Some of the links under the `resources/` folder may be helpful if someone wants to pursue that goal.
