//
// AVR TM1637 "Library" v1.01
// Enables control of TM1637 chip based modules, using direct port access.
//
// Copyright (c) 2015 IronCreek Software
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <stdint.h>

// ----------------------------------------------------------------------------
// Functions and parameters
// ----------------------------------------------------------------------------

// Parameters for setNumber()
#define TM_RIGHT 0x01
#define TM_LEFT 0x00

// Parameters for setNumberPad()
#define TM_PAD_SPACE 0x00
#define TM_PAD_0 0x3F

#define TM1637_DIGITS 4

// Control the TM1637 chip. This chip is usually attached
// to inexpensive 4 digit numeric displays. It's a generic
// 7-segment driver with key input capabilities though so
// it can be used for other I/O boards.
class TM1637 {
   public:
	// Construct a TM1637 object and initialize all the pins/ports used.
	TM1637();

	// Clear the 7-segment displays (only)
	void clear();

	// Set a single 7-segment display to the given byte value.
	// This allows direct control of the elements to do spinning animations etc.
	void setByte(const uint8_t position, const uint8_t b);

	// Display a single digit at the given position.
	// Position is left-to-right, starting with 0.
	void setDigit(const uint8_t position, const uint8_t digit);

	// Display an unsigned number at a given offset and alignment.
	void setNumber(uint32_t number, uint8_t offset = TM1637_DIGITS - 1,
				   const uint8_t align = TM_RIGHT);

	// Display an unsigned number at a given offset and pad it with 0's or
	// spaces to a desired with. This function is helpful when the numbers can
	// fluctuate in length (ex. 100, 5, 1000) to avoid flickering segments.
	void setNumberPad(uint32_t number, uint8_t offset, uint8_t width,
					  const uint8_t pad = TM_PAD_SPACE);

	// Display an unsigned number in hex format at a given offset and pad it
	// with 0's or spaces to a desired with.
	void setNumberHex(uint32_t number, uint8_t offset, uint8_t width,
					  const uint8_t pad = TM_PAD_SPACE);

	// Draw a character at a given position.
	// Not all characters are supported, check TM1637Font.h for an overview.
	void setChar(const uint8_t position, const char ch);

	// Display a string starting at a given offset.
	void setChars(const char *value, uint8_t offset = 0);

	// Scroll characters (scrolls <--- left)
	void scrollChars(const char *value);

	// Set which "dots" should be enabled.
	// Mask is mapped right to left (ex. 0x01 = right-most dot)
	void setDots(const uint8_t mask);

	// Set the brightness between 0 and 7
	void setBrightness(const uint8_t brightness);

   protected:
	uint8_t _dotMask;
};
