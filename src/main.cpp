// Firmware for https://github.iu.edu/PBS-TSG/Block-Touch-Touch
// This program runs on a Teensy 3.1 microcontroller platform, and provides
// a USB interface for a pair of resistive touch devices.
#include "Adafruit_STMPE610.h"
#include "DualRTouchPanels.h"

// TIMING
const uint32_t interval{10000}; // microseconds
elapsedMicros timeout; // stateful timer, updates itself via operator overloading

// MODEL
DualRTouchPanels myPanel = DualRTouchPanels(10, 15); // use pin 10 and pin 15 on Teensy 3.1 for hardware SPI

// PROGRAM
void setup() {
  Serial.begin(9600); // default baud rate for many programs
}

void loop() {
  myPanel.update(); // update as fast as possible
  // ...but only output data at the given interval
  if (timeout >= interval) {
    timeout -= interval;
    myPanel.emitData(); // sends both HID packets and Serial text
  }
}

