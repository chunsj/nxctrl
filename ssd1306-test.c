/*
 * NXCTRL BeagleBone Black Control Library
 *
 * SSD1306 OLED SPI Test Program
 *
 * Copyright (C) 2014 Sungjin Chun <chunsj@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <NXCTRL.h>
#include <NXCTRL_oled.h>

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define BNK               NXCTRL_P9

#define SPI_CS0           17
#define SPI_D1            18
#define SPI_CLK           22

#define OLED_DC           13
#define OLED_RST          15
#define OLED_CS           SPI_CS0
#define OLED_DATA         SPI_D1
#define OLED_CLK          SPI_CLK

typedef struct __tagNXCTRLOLED {
  int nCols, nRows;
  int nBufferRows;
  int nMemBytes;
  uint8_t nDCBank, nDCPin;
  uint8_t nResetBank, nResetPin;
  int nColOffset;
} NXCTRLOLED;

NXCTRLOLED oled;

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int nFD;

  NXCTRLPinMux(BNK, SPI_CS0, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_LOW);
  NXCTRLPinMux(BNK, SPI_D1, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_LOW);
  NXCTRLPinMux(BNK, SPI_CLK, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);

  NXCTRLPinMode(BNK, OLED_RST, NXCTRL_OUTPUT);
  NXCTRLDigitalWrite(BNK, OLED_RST, NXCTRL_HIGH);
  NXCTRLPinMode(BNK, OLED_DC, NXCTRL_OUTPUT);
  NXCTRLDigitalWrite(BNK, OLED_DC, NXCTRL_HIGH);

  oled.nCols = 128;
  oled.nRows = 32;
  oled.nBufferRows = 64;
  oled.nMemBytes = oled.nBufferRows * oled.nCols / 8;
  oled.nDCBank = BNK;
  oled.nDCPin = OLED_DC;
  oled.nResetBank = BNK;
  oled.nResetPin = OLED_RST;
  oled.nColOffset = 0;

  nFD = open("/dev/spidev1.0", O_RDWR);

  close(nFD);
}

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
NXCTRLExitLoop();
}

int
main (void) {
return NXCTRLMain();
}
