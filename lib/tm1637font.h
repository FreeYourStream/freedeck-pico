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

#define TM_DOT 0x80

#define TM_MINUS 0x40
#define TM_PLUS 0x44
#define TM_BLANK 0x00
#define TM_DEGREES 0x63
#define TM_UNDERSCORE 0x08
#define TM_EQUALS 0x48
#define TM_CHAR_ERR 0x49

uint8_t inline TM1637_map_char(const char ch) {
  uint8_t rc = 0;

  switch (ch) {
  case '-':
    rc = TM_MINUS;
    break;
  case '+':
    rc = TM_PLUS;
    break;
  case ' ':
    rc = TM_BLANK;
    break;
  case '^':
    rc = TM_DEGREES;
    break;
  case '_':
    rc = TM_UNDERSCORE;
    break;
  case '=':
    rc = TM_EQUALS;
    break;
  default:
    break;
  }

  return rc;
}

//      Bits:                 Hex:
//        -- 0 --               -- 01 --
//       |       |             |        |
//       5       1            20        02
//       |       |             |        |
//        -- 6 --               -- 40 --
//       |       |             |        |
//       4       2            10        04
//       |       |             |        |
//        -- 3 --  .7           -- 08 --   .80

const uint8_t TM_DIGITS[] = {0x3F, // 0
                             0x06, // 1
                             0x5B, // 2
                             0x4F, // 3
                             0x66, // 4
                             0x6D, // 5
                             0x7D, // 6
                             0x07, // 7
                             0x7F, // 8
                             0x6F, // 9

                             // 0x3F,  // 0 a
                             // 0x06,  // 1 a
                             // 0x6D,  // 2 a
                             // 0x4F,  // 3 a
                             // 0x56,  // 4 a
                             // 0x5B,  // 5 a
                             // 0x7B,  // 6 a
                             // 0x0E,  // 7
                             // 0x7F,  // 8 a
                             // 0x5F,  // 9

                             0x77, // A
                             0x7C, // b
                             0x39, // C
                             0x5E, // d
                             0x79, // E
                             0x71, // F

                             // HEX DIGITS END

                             0x3D, // G
                             0x76, // H
                             0x06, // I
                             0x1F, // J
                             0x76, // K (same as H)
                             0x38, // L
                             0x15, // M
                             0x54, // n
                             0x3F, // O
                             0x73, // P
                             0x67, // Q
                             0x50, // r
                             0x6D, // S
                             0x78, // t
                             0x3E, // U
                             0x1C, // V
                             0x2A, // W
                             0x76, // X (same as H)
                             0x6E, // Y
                             0x5B, // Z
                             0x00};
