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

NXCTRL_VOID
NXCTRLOLEDDrawFastHLine (NXCTRLOLED *pOLED,
                         NXCTRL_INT8 x, NXCTRL_INT8 y,
                         NXCTRL_INT8 w, NXCTRL_BOOL onOff) {
  register NXCTRL_UINT8 *pBuf;
  register NXCTRL_UINT8 chMask;
  
  if (y < 0 || y >= OLED_HEIGHT)
    return;

  if (x <0) {
    w += x;
    x = 0;
  }

  if ((x + w) > OLED_WIDTH)
    w = OLED_HEIGHT - x;

  if (w <= 0)
    return;

  pBuf = pOLED->rchBuf;
  pBuf += ((y/8) * OLED_WIDTH);
  pBuf += x;

  chMask = 1 << (y&7);

  if (onOff)
    while (w--)
      *pBuf++ |= chMask;
  else
    while (w--)
      *pBuf++ &= ~chMask;
}

NXCTRL_VOID
NXCTRLOLEDDrawFastVLine (NXCTRLOLED *pOLED,
                         NXCTRL_INT8 x, NXCTRL_INT8 y,
                         NXCTRL_INT8 h, NXCTRL_BOOL onOff) {
  register NXCTRL_UINT8 *pBuf = pOLED->rchBuf;
  register NXCTRL_UINT8 nMod;
  
  if (x < 0 || x >= OLED_WIDTH)
    return;

  if (y < 0) {
    h += y;
    y = 0;
  }

  if ((y + h) > OLED_HEIGHT)
    h = OLED_HEIGHT - y;

  if (h <= 0)
    return;

  pBuf += ((y/8) * OLED_WIDTH);
  pBuf += x;

  nMod = y & 7;
  if (nMod) {
    static NXCTRL_UINT8 rchPreMask[8] = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    register NXCTRL_UINT8 chMask;

    nMod = 8 - nMod;
    chMask = rchPreMask[nMod];
    
    if (h < nMod)
      chMask &= (0xFF >> (nMod - h));

    if (onOff)
      *pBuf |= chMask;
    else
      *pBuf &= ~chMask;

    if (h < nMod)
      return;

    h -= nMod;

    pBuf += OLED_WIDTH;
  }

  if (h >= 8) {
    register NXCTRL_UINT8 chVal = onOff ? 255 : 0;

    do {
      *pBuf = chVal;
      pBuf += OLED_WIDTH;
      h -= 8;
    } while(h >= 8);
  }

  if (h) {
    static NXCTRL_UINT8 rchPostMask[8] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    register NXCTRL_UINT8 chMask;
   
    nMod = h & 7;
    chMask = rchPostMask[nMod];

    if (onOff)
      *pBuf |= chMask;
    else
      *pBuf &= ~chMask;
  }
}

NXCTRL_VOID
NXCTRLOLEDDrawRect (NXCTRLOLED *pOLED,
                    NXCTRL_INT8 x, NXCTRL_INT8 y,
                    NXCTRL_INT8 w, NXCTRL_INT8 h,
                    NXCTRL_BOOL onOff) {
  NXCTRLOLEDDrawFastHLine(pOLED, x, y, w, onOff);
  NXCTRLOLEDDrawFastHLine(pOLED, x, y + h - 1, w, onOff);
  NXCTRLOLEDDrawFastVLine(pOLED, x, y, h, onOff);
  NXCTRLOLEDDrawFastVLine(pOLED, x + w - 1, y, h, onOff);
}

NXCTRL_VOID
NXCTRLOLEDFillRect (NXCTRLOLED *pOLED,
                    NXCTRL_INT8 x, NXCTRL_INT8 y,
                    NXCTRL_INT8 w, NXCTRL_INT8 h,
                    NXCTRL_BOOL onOff) {
  register int i;
  for (i = x; i < x + w; i++)
    NXCTRLOLEDDrawFastVLine(pOLED, i, y, h, onOff);
}

NXCTRL_VOID
NXCTRLOLEDDrawCircle (NXCTRLOLED *pOLED,
                      NXCTRL_INT8 x0, NXCTRL_INT8 y0,
                      NXCTRL_INT8 r,
                      NXCTRL_BOOL onOff) {
  NXCTRL_INT16 f = 1 - r;
  NXCTRL_INT16 ddFx = 1;
  NXCTRL_INT16 ddFy = -2 * r;
  NXCTRL_INT16 x = 0;
  NXCTRL_INT16 y = r;

  NXCTRLOLEDDrawPixel(pOLED, x0, y0 + r, onOff);
  NXCTRLOLEDDrawPixel(pOLED, x0, y0 - r, onOff);
  NXCTRLOLEDDrawPixel(pOLED, x0 + r, y0, onOff);
  NXCTRLOLEDDrawPixel(pOLED, x0 - r, y0, onOff);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddFy += 2;
      f += ddFy;
    }
    x++;
    ddFx += 2;
    f += ddFx;

    NXCTRLOLEDDrawPixel(pOLED, x0 + x, y0 + y, onOff);
    NXCTRLOLEDDrawPixel(pOLED, x0 - x, y0 + y, onOff);
    NXCTRLOLEDDrawPixel(pOLED, x0 + x, y0 - y, onOff);
    NXCTRLOLEDDrawPixel(pOLED, x0 - x, y0 - y, onOff);
    NXCTRLOLEDDrawPixel(pOLED, x0 + y, y0 + x, onOff);
    NXCTRLOLEDDrawPixel(pOLED, x0 - y, y0 + x, onOff);
    NXCTRLOLEDDrawPixel(pOLED, x0 + y, y0 - x, onOff);
    NXCTRLOLEDDrawPixel(pOLED, x0 - y, y0 - x, onOff);
  }
}

NXCTRL_VOID
NXCTRLOLEDFillCircleHelper (NXCTRLOLED *pOLED,
                            NXCTRL_INT8 x0, NXCTRL_INT8 y0,
                            NXCTRL_INT8 r,
                            NXCTRL_INT8 nCornerName, NXCTRL_INT8 nDelta,
                            NXCTRL_BOOL onOff) {
  NXCTRL_INT16 f = 1 - r;
  NXCTRL_INT16 ddFx = 1;
  NXCTRL_INT16 ddFy = -2 * r;
  NXCTRL_INT16 x = 0;
  NXCTRL_INT16 y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddFy += 2;
      f += ddFy;
    }
    x++;
    ddFx += 2;
    f += ddFx;

    if (nCornerName & 0x1) {
      NXCTRLOLEDDrawFastVLine(pOLED,
                              x0 + x, y0 - y,
                              2*y + 1 + nDelta, onOff);
      NXCTRLOLEDDrawFastVLine(pOLED,
                              x0 + y, y0 - x,
                              2*x + 1 + nDelta, onOff);
    }
    if (nCornerName & 0x2) {
      NXCTRLOLEDDrawFastVLine(pOLED,
                              x0 - x, y0 - y,
                              2*y + 1 + nDelta, onOff);
      NXCTRLOLEDDrawFastVLine(pOLED,
                              x0 - y, y0 - x,
                              2*x + 1 + nDelta, onOff);
    }
  }
}

NXCTRL_VOID
NXCTRLOLEDFillCircle (NXCTRLOLED *pOLED,
                      NXCTRL_INT8 x0, NXCTRL_INT8 y0,
                      NXCTRL_INT8 r,
                      NXCTRL_BOOL onOff) {
  NXCTRLOLEDDrawFastVLine(pOLED, x0, y0 - r, 2*r + 1, onOff);
  NXCTRLOLEDFillCircleHelper(pOLED, x0, y0, r, 3, 0, onOff);
}

NXCTRL_VOID
NXCTRLOLEDDrawCircleHelper (NXCTRLOLED *pOLED,
                            NXCTRL_INT8 x0, NXCTRL_INT8 y0,
                            NXCTRL_INT8 r,
                            NXCTRL_INT8 nCornerName,
                            NXCTRL_BOOL onOff) {
  NXCTRL_INT16 f = 1 - r;
  NXCTRL_INT16 ddFx = 1;
  NXCTRL_INT16 ddFy = -2 * r;
  NXCTRL_INT16 x = 0;
  NXCTRL_INT16 y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddFy += 2;
      f += ddFy;
    }
    x++;
    ddFx += 2;
    f += ddFx;

    if (nCornerName & 0x4) {
      NXCTRLOLEDDrawPixel(pOLED, x0 + x, y0 + y, onOff);
      NXCTRLOLEDDrawPixel(pOLED, x0 + y, y0 + x, onOff);
    }
    if (nCornerName & 0x2) {
      NXCTRLOLEDDrawPixel(pOLED, x0 + x, y0 - y, onOff);
      NXCTRLOLEDDrawPixel(pOLED, x0 + y, y0 - x, onOff);
    }
    if (nCornerName & 0x8) {
      NXCTRLOLEDDrawPixel(pOLED, x0 - x, y0 + y, onOff);
      NXCTRLOLEDDrawPixel(pOLED, x0 - y, y0 + x, onOff);
    }
    if (nCornerName & 0x1) {
      NXCTRLOLEDDrawPixel(pOLED, x0 - x, y0 - y, onOff);
      NXCTRLOLEDDrawPixel(pOLED, x0 - y, y0 - x, onOff);
    }
  }
}

NXCTRL_VOID
NXCTRLOLEDDrawRoundRect (NXCTRLOLED *pOLED,
                         NXCTRL_INT8 x, NXCTRL_INT8 y,
                         NXCTRL_INT8 w, NXCTRL_INT8 h,
                         NXCTRL_INT8 r,
                         NXCTRL_BOOL onOff) {
  NXCTRLOLEDDrawFastHLine(pOLED, x + r, y, w - 2*r, onOff);
  NXCTRLOLEDDrawFastHLine(pOLED, x + r, y + h - 1, w - 2*r, onOff);
  NXCTRLOLEDDrawFastVLine(pOLED, x, y + r, h - 2*r, onOff);
  NXCTRLOLEDDrawFastVLine(pOLED, x + w - 1, y +r, h - 2*r, onOff);

  NXCTRLOLEDDrawCircleHelper(pOLED, x + r, y + r, r, 1, onOff);
  NXCTRLOLEDDrawCircleHelper(pOLED, x + w - r - 1, y + r, r, 2, onOff);
  NXCTRLOLEDDrawCircleHelper(pOLED, x + w - r - 1, y + h - r - 1, r, 4, onOff);
  NXCTRLOLEDDrawCircleHelper(pOLED, x + r, y + h - r - 1, r, 8, onOff);
}

NXCTRL_VOID
NXCTRLOLEDFillRoundRect (NXCTRLOLED *pOLED,
                         NXCTRL_INT8 x, NXCTRL_INT8 y,
                         NXCTRL_INT8 w, NXCTRL_INT8 h,
                         NXCTRL_INT8 r,
                         NXCTRL_BOOL onOff) {
  NXCTRLOLEDFillRect(pOLED, x + r, y, w - 2 * r, h, onOff);

  NXCTRLOLEDFillCircleHelper(pOLED, x + w - r - 1, y + r, r, 1, h - 2 * r - 1, onOff);
  NXCTRLOLEDFillCircleHelper(pOLED, x + r, y + r, r, 2, h - 2 * r - 1, onOff);
}

NXCTRL_VOID
NXCTRLOLEDDrawTriangle (NXCTRLOLED *pOLED,
                        NXCTRL_INT8 x0, NXCTRL_INT8 y0,
                        NXCTRL_INT8 x1, NXCTRL_INT8 y1,
                        NXCTRL_INT8 x2, NXCTRL_INT8 y2,
                        NXCTRL_BOOL onOff) {
  NXCTRLOLEDDrawLine(pOLED, x0, y0, x1, y1, onOff);
  NXCTRLOLEDDrawLine(pOLED, x1, y1, x2, y2, onOff);
  NXCTRLOLEDDrawLine(pOLED, x2, y2, x0, y0, onOff);
}

NXCTRL_VOID
NXCTRLOLEDFillTriangle (NXCTRLOLED *pOLED,
                        NXCTRL_INT8 x0, NXCTRL_INT8 y0,
                        NXCTRL_INT8 x1, NXCTRL_INT8 y1,
                        NXCTRL_INT8 x2, NXCTRL_INT8 y2,
                        NXCTRL_BOOL onOff) {
  NXCTRL_INT16 a, b, y, last, t;
  NXCTRL_INT16 dx01, dx02, dx12, dy01, dy02, dy12, sa, sb;

  if (y0 > y1) {
    t = y0; y0 = y1; y1 = t;
    t = x0; x0 = x1; x1 = t;
  }
  if (y1 > y2) {
    t = y1; y1 = y2; y2 = t;
    t = x1; x1 = x2; x2 = t;
  }
  if (y0 > y1) {
    t = y0; y0 = y1; y1 = t;
    t = x0; x0 = x1; x1 = t;
  }

  if (y0 == y2) {
    a = b = x0;
    if (x1 < a) a = x1;
    else if (x1 > b) b = x1;
    if (x2 < a) a = x2;
    else if (x2 > b) b = x2;
    NXCTRLOLEDDrawFastHLine(pOLED, a, y0, b - a + 1, onOff);
    return;
  }

  dx01 = x1 - x0;
  dy01 = y1 - y0;
  dx02 = x2 - x0;
  dy02 = y2 - y0;
  dx12 = x2 - x1;
  dy12 = y2 - y1;
  sa = 0;
  sb = 0;

  if (y1 == y2) last = y1;
  else last = y1 - 1;

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    if (a > b) { t = a; a = b; b = t; }
    NXCTRLOLEDDrawFastHLine(pOLED, a, y, b - a + 1, onOff);
  }

  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    if (a > b) { t = a; a = b; b = t; }
    NXCTRLOLEDDrawFastHLine(pOLED, a, y, b - a + 1, onOff);
  }
}

NXCTRL_VOID
NXCTRLOLEDStartScrollRight (NXCTRLOLED *pOLED,
                            NXCTRL_UINT8 nStart,
                            NXCTRL_UINT8 nStop) {
  NXCTRLOLEDCommand(pOLED, SSD1306_RIGHT_HORIZONTAL_SCROLL);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, nStart);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, nStop);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, 0xFF);
  NXCTRLOLEDCommand(pOLED, SSD1306_ACTIVATE_SCROLL);
}

NXCTRL_VOID
NXCTRLOLEDStartScrollLeft (NXCTRLOLED *pOLED,
                           NXCTRL_UINT8 nStart,
                           NXCTRL_UINT8 nStop) {
  NXCTRLOLEDCommand(pOLED, SSD1306_LEFT_HORIZONTAL_SCROLL);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, nStart);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, nStop);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, 0xFF);
  NXCTRLOLEDCommand(pOLED, SSD1306_ACTIVATE_SCROLL);
}

NXCTRL_VOID
NXCTRLOLEDStartScrollDiagRight (NXCTRLOLED *pOLED,
                                NXCTRL_UINT8 nStart,
                                NXCTRL_UINT8 nStop) {
  NXCTRLOLEDCommand(pOLED, SSD1306_SET_VERTICAL_SCROLL_AREA);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, OLED_HEIGHT);
  NXCTRLOLEDCommand(pOLED, SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, nStart);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, nStop);
  NXCTRLOLEDCommand(pOLED, 0x01);
  NXCTRLOLEDCommand(pOLED, SSD1306_ACTIVATE_SCROLL);
}

NXCTRL_VOID
NXCTRLOLEDStartScrollDiagLeft (NXCTRLOLED *pOLED,
                               NXCTRL_UINT8 nStart,
                               NXCTRL_UINT8 nStop) {
  NXCTRLOLEDCommand(pOLED, SSD1306_SET_VERTICAL_SCROLL_AREA);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, OLED_HEIGHT);
  NXCTRLOLEDCommand(pOLED, SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, nStart);
  NXCTRLOLEDCommand(pOLED, 0x00);
  NXCTRLOLEDCommand(pOLED, nStop);
  NXCTRLOLEDCommand(pOLED, 0x01);
  NXCTRLOLEDCommand(pOLED, SSD1306_ACTIVATE_SCROLL);
}

NXCTRL_VOID
NXCTRLOLEDStopScroll (NXCTRLOLED *pOLED) {
  NXCTRLOLEDCommand(pOLED, SSD1306_DEACTIVATE_SCROLL);
}

NXCTRL_VOID
NXCTRLOLEDDrawBitmap (NXCTRLOLED *pOLED,
                      NXCTRL_INT8 x, NXCTRL_INT8 y,
                      const NXCTRL_UINT8 *pBitmap,
                      NXCTRL_UINT16 w, NXCTRL_UINT16 h,
                      NXCTRL_BOOL onOff) {
  NXCTRL_INT16 i, j, nWidth = (w + 7) / 8;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if ((*(const NXCTRL_UINT8 *)(pBitmap + j * nWidth + i / 8)) &
          (128 >> (i & 7)))
        NXCTRLOLEDDrawPixel(pOLED, x + i, y + j,  onOff);
    }
  }
}
