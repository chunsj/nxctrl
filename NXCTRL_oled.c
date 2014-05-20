/*
 * NXCTRL BeagleBone Black Control Library
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
#include <stdarg.h>
#include <string.h>
#include <NXCTRL_oled.h>

NXCTRL_VOID
NXCTRLOLEDCommand (NXCTRLOLED *pOLED, NXCTRL_UINT8 chByte) {
  NXCTRL_UINT8 chCmd[1];
  chCmd[0] = chByte;
  NXCTRLDigitalWrite(pOLED->nDCBank, pOLED->nDCPin, NXCTRL_LOW);
  NXCTRLSPIWrite(pOLED->nSPIFD, chCmd, 1);
}

NXCTRL_VOID
NXCTRLOLEDData (NXCTRLOLED *pOLED, NXCTRL_UINT8 chByte) {
  NXCTRL_UINT8 chData[1];
  chData[0] = chByte;
  NXCTRLDigitalWrite(pOLED->nDCBank, pOLED->nDCPin, NXCTRL_HIGH);
  NXCTRLSPIWrite(pOLED->nSPIFD, chData, 1);
}

NXCTRL_VOID
NXCTRLOLEDInit (NXCTRLOLED *pOLED,
                NXCTRL_UINT8 nDCBank, NXCTRL_UINT8 nDCPin,
                NXCTRL_UINT8 nRSTBank, NXCTRL_UINT8 nRSTPin,
                int nSPIFD) {

  pOLED->nDCBank = nDCBank;
  pOLED->nDCPin = nDCPin;
  pOLED->nRSTBank = nRSTBank;
  pOLED->nRSTPin = nRSTPin;
  pOLED->nSPIFD = nSPIFD;

  NXCTRLDigitalWrite(pOLED->nRSTBank, pOLED->nRSTPin, NXCTRL_HIGH);
  NXCTRLSleep(1000, 0);
  NXCTRLDigitalWrite(pOLED->nRSTBank, pOLED->nRSTPin, NXCTRL_LOW);
  NXCTRLSleep(1000, 0);
  NXCTRLDigitalWrite(pOLED->nRSTBank, pOLED->nRSTPin, NXCTRL_HIGH);
  NXCTRLSleep(1000, 0);

  NXCTRLOLEDCommand(pOLED, 0xAE);
  NXCTRLOLEDCommand(pOLED, 0xD5);
  NXCTRLOLEDCommand(pOLED, 0x80);
  NXCTRLOLEDCommand(pOLED, 0xA8);
  NXCTRLOLEDCommand(pOLED, 0x3F);
  NXCTRLOLEDCommand(pOLED, 0xD3);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, 0x8D);
  NXCTRLOLEDCommand(pOLED, 0x14);

  NXCTRLOLEDCommand(pOLED, 0xDA);
  NXCTRLOLEDCommand(pOLED, 0x12);
  NXCTRLOLEDCommand(pOLED, 0x81);
  NXCTRLOLEDCommand(pOLED, 0xCF);

  NXCTRLOLEDCommand(pOLED, 0xD9);
  NXCTRLOLEDCommand(pOLED, 0xF1);

  NXCTRLOLEDCommand(pOLED, 0xDB);
  NXCTRLOLEDCommand(pOLED, 0x40);

  NXCTRLOLEDCommand(pOLED, 0x20);
  NXCTRLOLEDCommand(pOLED, 0x00);

  NXCTRLOLEDCommand(pOLED, 0x22);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, 0x07);

  NXCTRLOLEDCommand(pOLED, 0xA4);

  NXCTRLOLEDCommand(pOLED, 0xAF);

  memset(pOLED->rchBuf, 0, OLED_BUF_SIZE);
  NXCTRLDigitalWrite(pOLED->nDCBank, pOLED->nDCPin, NXCTRL_HIGH);
  NXCTRLSPIWrite(pOLED->nSPIFD, pOLED->rchBuf, OLED_BUF_SIZE);
}

NXCTRL_VOID
NXCTRLOLEDDisplayNormal (NXCTRLOLED *pOLED) {
  NXCTRLOLEDCommand(pOLED, 0xA6);
}

NXCTRL_VOID
NXCTRLOLEDDisplayInverse (NXCTRLOLED *pOLED) {
  NXCTRLOLEDCommand(pOLED, 0xA7);
}
