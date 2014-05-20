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
#include <NXCTRL_oled_priv.h>

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
  NXCTRLPinMode(nDCBank, nDCPin, NXCTRL_OUTPUT);
  NXCTRLPinMode(nRSTBank, nRSTPin, NXCTRL_OUTPUT);

  pOLED->nDCBank = nDCBank;
  pOLED->nDCPin = nDCPin;
  pOLED->nRSTBank = nRSTBank;
  pOLED->nRSTPin = nRSTPin;
  pOLED->nSPIFD = nSPIFD;

  pOLED->nCursorX = pOLED->nCursorY = 0;

  NXCTRLDigitalWrite(pOLED->nRSTBank, pOLED->nRSTPin, NXCTRL_HIGH);
  NXCTRLSleep(100, 0);
  NXCTRLDigitalWrite(pOLED->nRSTBank, pOLED->nRSTPin, NXCTRL_LOW);
  NXCTRLSleep(100, 0);
  NXCTRLDigitalWrite(pOLED->nRSTBank, pOLED->nRSTPin, NXCTRL_HIGH);
  NXCTRLSleep(100, 0);

  NXCTRLOLEDCommand(pOLED, SSD1306_DISPLAYOFF);
  NXCTRLOLEDCommand(pOLED, SSD1306_SETDISPLAYCLOCKDIV);
  NXCTRLOLEDCommand(pOLED, 0x80);
  NXCTRLOLEDCommand(pOLED, SSD1306_SETMULTIPLEX);
  NXCTRLOLEDCommand(pOLED, 0x3F);
  NXCTRLOLEDCommand(pOLED, SSD1306_SETDISPLAYOFFSET);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, SSD1306_SETSTARTLINE | 0x0);
  NXCTRLOLEDCommand(pOLED, SSD1306_CHARGEPUMP);
  NXCTRLOLEDCommand(pOLED, 0x14);

  NXCTRLOLEDCommand(pOLED, SSD1306_MEMORYMODE);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, SSD1306_SEGREMAP | 0x1);
  NXCTRLOLEDCommand(pOLED, SSD1306_COMSCANDEC);
  NXCTRLOLEDCommand(pOLED, SSD1306_SETCOMPINS);
  NXCTRLOLEDCommand(pOLED, 0x12);
  NXCTRLOLEDCommand(pOLED, SSD1306_SETCONTRAST);
  NXCTRLOLEDCommand(pOLED, 0xCF);

  NXCTRLOLEDCommand(pOLED, SSD1306_SETPRECHARGE);
  NXCTRLOLEDCommand(pOLED, 0xF1);

  NXCTRLOLEDCommand(pOLED, SSD1306_SETVCOMDETECT);
  NXCTRLOLEDCommand(pOLED, 0x40);

  NXCTRLOLEDCommand(pOLED, SSD1306_COLUMNADDR);
  NXCTRLOLEDCommand(pOLED, 0);
  NXCTRLOLEDCommand(pOLED, 127);

  NXCTRLOLEDCommand(pOLED, SSD1306_PAGEADDR);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, 0x07);

  NXCTRLOLEDCommand(pOLED, SSD1306_DISPLAYALLON_RESUME);
  NXCTRLOLEDCommand(pOLED, SSD1306_NORMALDISPLAY);

  NXCTRLOLEDCommand(pOLED, SSD1306_DISPLAYON);

  memset(pOLED->rchBuf, 0, OLED_BUF_SIZE);
  NXCTRLOLEDUpdateDisplay(pOLED);
}

NXCTRL_VOID
NXCTRLOLEDUpdateDisplay (NXCTRLOLED *pOLED) {
  NXCTRLDigitalWrite(pOLED->nDCBank, pOLED->nDCPin, NXCTRL_HIGH);
  NXCTRLSPIWrite(pOLED->nSPIFD, pOLED->rchBuf, OLED_BUF_SIZE);
}

NXCTRL_VOID
NXCTRLOLEDClearDisplay (NXCTRLOLED *pOLED) {
 memset(pOLED->rchBuf, 0, OLED_BUF_SIZE);
}

NXCTRL_VOID
NXCTRLOLEDDisplayNormal (NXCTRLOLED *pOLED) {
  NXCTRLOLEDCommand(pOLED, SSD1306_NORMALDISPLAY);
}

NXCTRL_VOID
NXCTRLOLEDDisplayInverse (NXCTRLOLED *pOLED) {
  NXCTRLOLEDCommand(pOLED, SSD1306_INVERTDISPLAY);
}

NXCTRL_VOID
NXCTRLOLEDSetBanner (NXCTRLOLED *pOLED) {
  memcpy(pOLED->rchBuf, buffer, OLED_BUF_SIZE);
}

NXCTRL_VOID
NXCTRLOLEDDrawPixel (NXCTRLOLED *pOLED,
                     NXCTRL_INT8 x, NXCTRL_INT8 y,
                     NXCTRL_BOOL onOff) {
  NXCTRL_INT16 nOffset;
  
  if ((x < 0) || (x >= OLED_WIDTH) || (y < 0) || (y >= OLED_HEIGHT))
    return;

  nOffset = x + (y/8) * OLED_WIDTH;

  if (onOff)
    pOLED->rchBuf[nOffset] |= (1 << (y&7));
  else
    pOLED->rchBuf[nOffset] &= ~(1 << (y&7));
}

NXCTRL_VOID
NXCTRLOLEDDrawLine (NXCTRLOLED *pOLED,
                    NXCTRL_INT8 x0, NXCTRL_INT8 y0,
                    NXCTRL_INT8 x1, NXCTRL_INT8 y1,
                    NXCTRL_BOOL onOff) {
  NXCTRL_INT8 t, dx, dy, ystep;
  NXCTRL_INT16 err;
  NXCTRL_INT8 nSteep = abs(y1 - y0) > abs(x1 - x0);

  if (nSteep) {
    t = x0;
    x0 = y0;
    y0 = t;
    t = x1;
    x1 = y1;
    y1 = t;
  }

  if (x0 > x1) {
    t = x0;
    x0 = x1;
    x1 = t;
    t = y0;
    y0 = y1;
    y1 = t;
  }

  dx = x1 - x0;
  dy = abs(y1 - y0);

  err = dx / 2;

  if (y0 < y1)
    ystep = 1;
  else
    ystep = -1;

  for (; x0 <= x1; x0++) {
    if (nSteep)
      NXCTRLOLEDDrawPixel(pOLED, y0, x0, onOff);
    else
      NXCTRLOLEDDrawPixel(pOLED, x0, y0, onOff);
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

NXCTRL_VOID
NXCTRLOLEDDrawChar (NXCTRLOLED *pOLED,
                    NXCTRL_INT8 x, NXCTRL_INT8 y,
                    NXCTRL_UINT8 ch,
                    NXCTRL_BOOL onOffFG, NXCTRL_BOOL onOffBG) {
  NXCTRL_INT8 i, j;
  
  if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT) ||
      ((x + 6 - 1) < 0) || ((y + 8 - 1) < 0))
    return;

  for (i = 0; i < 6; i++) {
    NXCTRL_UINT8 nLine;
    if (i == 5)
      nLine = 0x0;
    else
      nLine = (*(const NXCTRL_UINT8 *)(font + (ch * 5) + i));
    for (j = 0; j < 8; j++) {
      if (nLine & 0x1) {
          NXCTRLOLEDDrawPixel(pOLED, x + i, y + j, onOffFG);
      } else if (onOffBG != onOffFG) {
          NXCTRLOLEDDrawPixel(pOLED, x + i, y + j, onOffBG);
      }
      nLine >>= 1;
    }
  }
}

NXCTRL_VOID
NXCTRLOLEDSetCursor (NXCTRLOLED *pOLED,
                     NXCTRL_INT8 x, NXCTRL_INT8 y) {
  pOLED->nCursorX = x;
  pOLED->nCursorY = y;
}

NXCTRL_VOID
NXCTRLOLEDWriteEx (NXCTRLOLED *pOLED, NXCTRL_UINT8 ch, NXCTRL_BOOL bWrap) {
  if (ch == '\n') {
    pOLED->nCursorY += 8;
    pOLED->nCursorX = 0;
  } else if (ch == '\r') {
  } else {
    NXCTRLOLEDDrawChar(pOLED, pOLED->nCursorX, pOLED->nCursorY,
                       ch,
                       NXCTRL_ON, NXCTRL_OFF);
    pOLED->nCursorX += 6;
    if (bWrap && (pOLED->nCursorX > (OLED_WIDTH - 6))) { // wrap
      pOLED->nCursorY += 8;
      pOLED->nCursorX = 0;
    }
  }
}

NXCTRL_VOID
NXCTRLOLEDWrite (NXCTRLOLED *pOLED, NXCTRL_UINT8 ch) {
  NXCTRLOLEDWriteEx(pOLED, ch, NXCTRL_ON);
}
