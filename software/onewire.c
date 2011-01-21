/*
Copyright (c) 2007, Jim Studt

Updated to work with arduino-0008 and to include skip(OneWire* data) as of
2007/07/06. --RJL20

Modified to calculate the 8-bit CRC directly, avoiding the need for
the 256-byte lookup table to be loaded in RAM.  Tested in arduino-0010
-- Tom Pollard, Jan 23, 2008

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial data->portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Much of the code was inspired by Derek Yerger's code, though I don't
think much of that remains.  In any event that was..
    (copyleft) 2006 by Derek Yerger - Free to distribute freely.

	The CRC code was excerpted and inspired by the Dallas Semiconductor 
	sample code bearing this copyright.
	//---------------------------------------------------------------------------
	// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
	//
	// Permission is hereby granted, free of charge, to any person obtaining a
	// copy of this software and associated documentation files (the "Software"),
	// to deal in the Software without restriction, including without limitation
	// the rights to use, copy, modify, merge, publish, distribute, sublicense,
	// and/or sell copies of the Software, and to permit persons to whom the
	// Software is furnished to do so, subject to the following conditions:
	//
	// The above copyright notice and this permission notice shall be included
	// in all copies or substantial data->portions of the Software.
	//
	// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
	// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	// OTHER DEALINGS IN THE SOFTWARE.
	//
	// Except as contained in this notice, the name of Dallas Semiconductor
	// shall not be used except as stated in the Dallas Semiconductor
	// Branding Policy.
	//--------------------------------------------------------------------------
	*/

// Modified by Alex Crawford (stripped un-needed functionality)
// Jan. 21, 2011

#include "onewire.h"

unsigned char id[ID_LEN];

uint8_t read_ibutton_id() {
	uint8_t i;

	//Send read-rom command
	write(0x33);

	//Read the 8 byte iButton address
	for (i = 0; i < 8; i++) {
		id[i] = read();
	}

	//Verify the checksum
	if (id[ID_LEN - 1] == crc8()) {
		return 1;
	}

	return 0;
}

void write(uint8_t data) {
	uint8_t i;

	for (i = 0; i < 8; i++) {
		//Set mode to output
		MODE_REG |= (1 << IO_PIN);

		//Pull the line low
		OUTPUT_REG &= ~(1 << IO_PIN);
		if (data & 0x01) {
			delay_us(LOW_TIME);
		} else {
			delay_us(HIGH_TIME);
		}

		//Pull the line high
		OUTPUT_REG |= (1 << IO_PIN);
		if (data & 0x01) {
			delay_us(HIGH_TIME);
		} else {
			delay_us(LOW_TIME);
		}

		data >>= 1;
	}
}

uint8_t read() {
	uint8_t data = 0x00;
	uint8_t i;

	for (i = 0; i < 8; i++) {
		//Set the mode to output
		MODE_REG |= (1 << IO_PIN);

		//Pull the line low
		OUTPUT_REG &= ~(1 << IO_PIN);
		delay_us(1);

		//Set the mode to input
		MODE_REG &= ~(1 << IO_PIN);
		delay_us(5);

		//Read the bit
		data |= !!(INPUT_REG & (1 << IO_PIN));
		delay_us(50);
		
		data <<= 1;
	}

	return data;
}

void delay_us(unsigned long delay) {
	//8 MHz internal clock on the ATmega168

	//For a one or two microsecond delay, just return
	if (--delay == 0 || --delay == 0) {
		return;
	}

	//The following loop takes half a microsecond (4 cycles)
	//per iteration, so execute it twice for each microsecond
	delay /= 2;

	//Conpensate for the time taken by the preceeding commands
	delay--;

	//delay
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" //2 cycles
		"brne 1b" : "=w" (delay) : "0" (delay) //2 cycles
	);
}

uint8_t crc8() {
	uint8_t crc = 0;
	uint8_t i, count, iobyte;

	for (i = 0; i < ID_LEN - 1; i++) {
		iobyte = id[i];

		for (count = 0; count < 8; count++) {
			if (crc ^ iobyte) {
				crc >>= 1;
				crc ^= 0x8C;
			} else {
				crc >>= 1;
			}


			iobyte >>= 1;
		}
	}

	return crc;
}

