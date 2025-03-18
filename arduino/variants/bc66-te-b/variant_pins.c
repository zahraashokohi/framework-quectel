/*
  BC66 - digital pins
    Created on: 08.03.2020
    Author: Georgi Angelov

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA   
 */

#include "variant.h"

// Array index is Arduino pin, resort table if you want

PinDescription pinsMap[PINNAME_END] = {
//   Quectel,           Mediatek, EINT callback
    {PINNAME_SPI_MISO,      0x09,   NULL}, //  0
    {PINNAME_SPI_MOSI,      0x0A,   NULL}, //  1
    {PINNAME_SPI_SCLK,      0x0B,   NULL}, //  2
    {PINNAME_SPI_CS,        0x08,   NULL}, //  3
    {PINNAME_NETLIGHT,      0x01,   NULL}, //  4, led
    {PINNAME_RI,            0x06,   NULL}, //  5, i2c-c
    {PINNAME_DCD,           0x07,   NULL}, //  6, i2c-d
    {PINNAME_CTS_AUX,       0x0E,   NULL}, //  7
    {PINNAME_RTS_AUX,       0x0F,   NULL}, //  8
    {PINNAME_GPIO1,         0x1C,   NULL}, //  9
    {PINNAME_RXD_AUX,       0x0C,   NULL}, // 10, no eint
    {PINNAME_TXD_AUX,       0x0D,   NULL}, // 11, no eint
    {PINNAME_GPIO2,         0x1B,   NULL}, // 12
    {PINNAME_GPIO3,         0x1A,   NULL}, // 13
    {PINNAME_GPIO4,         0x18,   NULL}, // 14
    {PINNAME_GPIO5,         0x19,   NULL}, // 15
    {PINNAME_RXD_DBG,       0x12,   NULL}, // 16, no eint
    {PINNAME_TXD_DBG,       0x16,   NULL}, // 17, no eint
    /* EX */
    {PINNAME_GPIO0,         0x1D,   NULL}, // 18, no eint
    {PINNAME_USB_MODE,      0x00,   NULL}, // 19, no eint
    {PINNAME_GPIO6,         0x1F,   NULL}, // 20, no eint
    {PINNAME_GPIO7,         0x21,   NULL}, // 21, no eint
    {PINNAME_GPIO8,         0x22,   NULL}, // 22, no eint
};
