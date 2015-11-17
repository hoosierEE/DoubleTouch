// DualRTouchPanels.h
#ifndef PANEL_H
#define PANEL_H
#include <Arduino.h>

class DualRTouchPanels {
	// provides USB and RawHID interfaces to two resistive-touch devices,
	// oriented back-to-back.
	// Panels transmit touch coordinates over USB Serial as ASCII Text,
	// and (TODO) also over USB HID as an array of bytes.
	// Panels only transmit data when they detect one or more touches.
	// Panels also transmit an "all zeros" message following a touch,
	// which could be taken to mean "touch ended".
public:
	DualRTouchPanels(uint8_t fwd_pin, uint8_t rev_pin)
	{
		// Each DualRTouchPanels has 2 sides, so needs 2 chip select pins.
		// On a Teensy 3.1, for exapmle, hardware SPI uses pins 10 and 15.
		gotNewData = false;
		fwdrev[0] = Adafruit_STMPE610(fwd_pin);
		fwdrev[1] = Adafruit_STMPE610(rev_pin);
		if (!fwdrev[0].begin()) {
			Serial.println(F("Forward-side STMPE not found!"));
			while(1);
		}
		if (!fwdrev[1].begin()) {
			Serial.println(F("Reverse-side STMPE not found!"));
			while(1);
		}
	};
	~DualRTouchPanels() {};

	void update()
	{
		// Save coords of any touches in the buffer, overwriting old values.
		for (uint8_t i = 0; i < NUM_SIDES; ++i) {
			touch[i] = fwdrev[i].touched(); // was a side touched?
			if (touch[i]) {
				while (!fwdrev[i].bufferEmpty()) {
					// get the most recent data from the controller
					fwdrev[i].readData(&pack.xs[i],&pack.ys[i],&pack.zs[i]);
				}
				// reset the controller
				fwdrev[i].writeRegister8(STMPE_INT_STA, 0xFF);
			}
			gotNewData |= touch[i]; // true if either side was touched during update()
		}
	}

	void emitData()
	{
		// USB data comes out
		if (gotNewData) { // if either panel was touched
			sendPacketRawHID(); // send the packet (Raw HID) // testing
			sendPacketSerial(true); // send the packet (Serial)
			gotNewData = false;
		}
	}

private:

	uint8_t hb(uint16_t val)
	{
		// get the high byte of a 16-bit value, e.g. 0x1234 --> 0x12
		return (val>>8)& 0xff;
	}
	uint8_t lb(uint16_t val)
	{
		// get the low byte of a 16-bit value, e.g. 0x1234 --> 0x34
		return val & 0xff;
	}

	void sendPacketRawHID()
	{
		// Serialize `pack` and send the result to the RawHID output buffer

		// TODO: is this workable?
		// memcpy() might require a special ARM instruction, or cast to (char *)
		// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka3934.html
		// memcpy(buffer, &pack, sizeof(Packet)); // serialize 
		uint8_t hidpacket[64];

		// for testing/good-enough purposes, just do things the long way
		// front side
		hidpacket[0] = touch[0] ? hb(pack.xs[0]) : 0;
		hidpacket[1] = touch[0] ? lb(pack.xs[0]) : 0;
		hidpacket[2] = touch[0] ? hb(pack.ys[0]) : 0;
		hidpacket[3] = touch[0] ? lb(pack.ys[0]) : 0;
		hidpacket[4] = touch[0] ?    pack.zs[0]  : 0;
		
		// rear side
		hidpacket[5] = touch[1] ? hb(pack.xs[1]) : 0;
		hidpacket[6] = touch[1] ? lb(pack.xs[1]) : 0;
		hidpacket[7] = touch[1] ? hb(pack.ys[1]) : 0;
		hidpacket[8] = touch[1] ? lb(pack.ys[1]) : 0;
		hidpacket[9] = touch[1] ?    pack.zs[1]  : 0;
		for (uint8_t i = 10; i < 64; ++i) {
			hidpacket[i] = 0; // zero remaining bytes
		}
		RawHID.send(hidpacket, 10); // 10ms timeout
	}

	void sendPacketSerial(bool yesZ=false)
	{
		// Serial print format: 'x1 y1 x2 y2 ' (note trailing space)
		for (uint8_t i = 0; i < NUM_SIDES; ++i) {
			Serial.print(touch[i] ? pack.xs[i] : 0);
			Serial.print(" ");
			Serial.print(touch[i] ? pack.ys[i] : 0);
			Serial.print(" ");
			if (yesZ) {
				Serial.print(touch[i] ? pack.zs[i] : 0);
				Serial.print(" ");
			}
		}
		Serial.println("");
	}

	// stateful data and/or objects
	uint8_t buffer[64]; // byte array for both incoming and outgoing HID messages
	static const uint8_t NUM_SIDES{2};
	Adafruit_STMPE610 fwdrev[NUM_SIDES];
	bool touch[NUM_SIDES];
	bool gotNewData; // new data flag
	struct Packet {
		uint16_t xs[NUM_SIDES];
		uint16_t ys[NUM_SIDES];
		uint8_t zs[NUM_SIDES];
	} pack;
};

#endif // PANEL_H
