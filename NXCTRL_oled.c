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

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define SPI_DELAY 200

NXCTRL_VOID
NXCTRLBITMAPInit (NXCTRLBITMAP *pBitmap, int nCols, int nRows) {
  register int i;
  pBitmap->nCols = nCols;
  pBitmap->nRows = nRows;
  pBitmap->nBytesPerCol = nRows / 8;
  for (i = 0; i < OLED_MAX_COL * OLED_MAX_ROW; i++)
    pBitmap->rchData[i] = 0;
}

NXCTRL_VOID
NXCTRLBITMAPClear (NXCTRLBITMAP *pBitmap) {
  register int i;
  for (i = 0; i < OLED_MAX_COL * OLED_MAX_ROW; i++)
    pBitmap->rchData[i] = 0;
}

NXCTRL_VOID
NXCTRLBITMAPDump (NXCTRLBITMAP *pBitmap) {
  register int i, j;
  for (i = 0; i < pBitmap->nRows; i++) {
    int nMemRow = i / 8;
    uint8_t nBitMask = 1 << (i % 8);
    char rchLine[BUFSIZ];
    rchLine[0] = 0;
    for (j = 0; pBitmap->nCols; j++) {
      int nMemCol = j;
      int nOffset = nMemRow + pBitmap->nRows / 8 * nMemCol;
      if (pBitmap->rchData[nOffset] & nBitMask)
        strcat(rchLine, "*");
      else
        strcat(rchLine, " ");
      printf("|%s|\n", rchLine);
    }
  }
}

NXCTRL_VOID
NXCTRLBITMAPDrawPixel (NXCTRLBITMAP *pBitmap, int x, int y, NXCTRL_BOOL on) {
  int nMemCol, nMemRow;
  uint8_t nBitMask;
  int nOffset;
  if (x < 0 || x >= pBitmap->nCols || y < 0 || y >= pBitmap->nRows)
    return;
  nMemCol = x;
  nMemRow = y / 8;
  nBitMask  = 1 << (y % 8);
  nOffset = nMemRow + pBitmap->nRows / 8 * nMemCol;
  if (on)
    pBitmap->rchData[nOffset] |= nBitMask;
  else
    pBitmap->rchData[nOffset] &= (0xFF - nBitMask);
}

NXCTRL_VOID
NXCTRLBITMAPClearBlock (NXCTRLBITMAP *pBitmap, int x0, int y0, int dx, int dy) {
  register int i, j;
  for (i = x0; i < x0 +dx; i++)
    for (j = y0; j < y0 + dy; j++)
      NXCTRLBITMAPDrawPixel(pBitmap, i, j, NXCTRL_OFF);
}

NXCTRL_VOID
NXCTRLOLEDInit (NXCTRLOLED *pOLED,
                int nSPIBank, int nCS, int nD1, int nCLK,
                int nDCBank, int nDCPin,
                int nRSBank, int nRSPin,
                int nCols, int nRows,
                int nBufferCols, int nBufferRows,
                int nSPIFD) {
  NXCTRLPinMux(nSPIBank, nCS, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_LOW);
  NXCTRLPinMux(nSPIBank, nD1, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_LOW);
  NXCTRLPinMux(nSPIBank, nCLK, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);

  NXCTRLPinMode(nRSBank, nRSPin, NXCTRL_OUTPUT);
  NXCTRLPinMode(nDCBank, nDCPin, NXCTRL_OUTPUT);

  NXCTRLDigitalWrite(nRSBank, nRSPin, NXCTRL_HIGH);
  NXCTRLDigitalWrite(nDCBank, nDCPin, NXCTRL_HIGH);

  pOLED->nCols = nCols;
  pOLED->nRows = nRows;
  pOLED->nBufferRows = nBufferRows;
  pOLED->nBufferCols = nBufferCols;
  pOLED->nMemBytes = pOLED->nBufferRows * pOLED->nCols / 8;
  pOLED->nDCBank = nDCBank;
  pOLED->nDCPin = nDCPin;
  pOLED->nRSBank = nRSBank;
  pOLED->nRSPin = nRSPin;
  pOLED->nColOffset = 0;
  pOLED->nSPIFD = nSPIFD;
  pOLED->bFlipped = NXCTRL_OFF;

  NXCTRLBITMAPInit(&(pOLED->bitmap), pOLED->nBufferCols, pOLED->nBufferRows);
}

NXCTRL_VOID
NXCTRLOLEDReset (NXCTRLOLED *pOLED) {
  NXCTRLDigitalWrite(pOLED->nRSBank, pOLED->nRSPin, NXCTRL_LOW);
  NXCTRLSleep(10, 0);
  NXCTRLDigitalWrite(pOLED->nRSBank, pOLED->nRSPin, NXCTRL_HIGH);
}

int
NXCTRLOLEDCommand (NXCTRLOLED *pOLED, uint8_t *pnCMD, uint8_t nCMDLength) {
  int nStatus;
  struct spi_ioc_transfer xfer[2];
  char rchTXData[nCMDLength];
  char rchRXData[nCMDLength];

  memset(xfer, 0, sizeof(xfer));

  memcpy(rchTXData, pnCMD, nCMDLength);

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = nCMDLength;
  xfer[0].delay_usecs = SPI_DELAY;

  nStatus = ioctl(pOLED->nSPIFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("NXCTRLOLEDCommand");
    return -1;
  }

  return 0;
}

int
NXCTRLOLEDData (NXCTRLOLED *pOLED, uint8_t *pnData, uint8_t nDataLength) {
  int nStatus;
  struct spi_ioc_transfer xfer[2];
  char rchTXData[255];
  char rchRXData[255];
  int nStart, nRemaining;

  NXCTRLDigitalWrite(pOLED->nDCBank, pOLED->nDCPin, NXCTRL_HIGH);

  nStart = 0;
  nRemaining = nDataLength;

  while (nRemaining > 0) {
    int nCount = nRemaining <= 255 ? nRemaining : 255;
    nRemaining -= nCount;

    memset(xfer, 0, sizeof(xfer));
    
    memcpy(rchTXData, pnData + nStart, nCount);

    xfer[0].tx_buf = (unsigned int)rchTXData;
    xfer[0].rx_buf = (unsigned int)rchRXData;
    xfer[0].len = nCount;
    xfer[0].delay_usecs = SPI_DELAY;

    nStatus = ioctl(pOLED->nSPIFD, SPI_IOC_MESSAGE(1), xfer);
    if (nStatus < 0) {
      NXCTRLDigitalWrite(pOLED->nDCBank, pOLED->nDCPin, NXCTRL_LOW);
      return -1;
    }

    nStart += nCount;
  }

  NXCTRLDigitalWrite(pOLED->nDCBank, pOLED->nDCPin, NXCTRL_LOW);
  return 0;
}

NXCTRL_VOID
NXCTRLOLEDBegin (NXCTRLOLED *pOLED, uint8_t nVCCState) {
  uint8_t rchCMD[4];
  
  NXCTRLSleep(1, 0);
  NXCTRLOLEDReset(pOLED);

  rchCMD[0] = OLED_DISPLAY_OFF;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);

  rchCMD[0] = OLED_SET_DISPLAY_CLOCK_DIV;
  rchCMD[1] = 0x80;
  NXCTRLOLEDCommand(pOLED, rchCMD, 2);

  switch (pOLED->nRows) {
  case 32:
    rchCMD[0] = OLED_SET_MULTIPLEX;
    rchCMD[1] = 0x1F;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
    rchCMD[0] = OLED_SET_COM_PINS;
    rchCMD[1] = 0x02;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
    break;
  case 64:
    rchCMD[0] = OLED_SET_MULTIPLEX;
    rchCMD[1] = 0x3F;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
    rchCMD[0] = OLED_SET_COM_PINS;
    rchCMD[1] = 0x12;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
    break;
  default:
    fprintf(stderr, "NXCTRLOLEDBegin: rows should be either 32 or 64\n");
    return;
  }

  rchCMD[0] = OLED_SET_DISPLAY_OFFSET;
  rchCMD[1] = 0x00;
  NXCTRLOLEDCommand(pOLED, rchCMD, 2);

  rchCMD[0] = OLED_SET_START_LINE | 0x00;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);

  if (nVCCState == OLED_EXTERNAL_VCC) {
    rchCMD[0] = OLED_CHARGE_PUMP;
    rchCMD[1] = 0x10;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
  } else {
    rchCMD[0] = OLED_CHARGE_PUMP;
    rchCMD[1] = 0x14;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
  }

  rchCMD[0] = OLED_SET_MEMORY_MODE;
  rchCMD[1] = 0x00;
  NXCTRLOLEDCommand(pOLED, rchCMD, 2);

  rchCMD[0] = OLED_SEG_REMAP | 0x01;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);

  rchCMD[0] = OLED_COM_SCAN_DEC;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);

  rchCMD[0] = OLED_SET_CONTRAST;
  rchCMD[1] = 0x8F;
  NXCTRLOLEDCommand(pOLED, rchCMD, 2);

  if (nVCCState == OLED_EXTERNAL_VCC) {
    rchCMD[0] = OLED_SET_PRECHARGE;
    rchCMD[1] = 0x22;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
  } else {
    rchCMD[0] = OLED_SET_PRECHARGE;
    rchCMD[1] = 0xF1;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
  }

  rchCMD[0] = OLED_SET_VCOM_DETECT;
  rchCMD[1] = 0x40;
  NXCTRLOLEDCommand(pOLED, rchCMD, 2);

  rchCMD[0] = OLED_DISPLAY_ALL_ON_RESUME;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);

  rchCMD[0] = OLED_NORMAL_DISPLAY;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);

  rchCMD[0] = OLED_DISPLAY_ON;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);
}

NXCTRL_VOID
NXCTRLOLEDClearDisplay (NXCTRLOLED *pOLED) {
  NXCTRLBITMAPClear(&(pOLED->bitmap));
}

NXCTRL_VOID
NXCTRLOLEDInvertDisplay (NXCTRLOLED *pOLED) {
  uint8_t rchCMD[4];
  rchCMD[0] = OLED_INVERT_DISPLAY;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);
}

NXCTRL_VOID
NXCTRLOLEDFlipDisplay (NXCTRLOLED *pOLED, NXCTRL_BOOL flipped) {
  uint8_t rchCMD[4];
  pOLED->bFlipped = flipped;
  if (flipped) {
    rchCMD[0] = OLED_COM_SCAN_INC;
    NXCTRLOLEDCommand(pOLED, rchCMD, 1);
    rchCMD[0] = OLED_SEG_REMAP | 0x00;
    NXCTRLOLEDCommand(pOLED, rchCMD, 1);
  } else {
    rchCMD[0] = OLED_COM_SCAN_DEC;
    NXCTRLOLEDCommand(pOLED, rchCMD, 1);
    rchCMD[0] = OLED_SET_COM_PINS;
    rchCMD[1] = 0x02;
    NXCTRLOLEDCommand(pOLED, rchCMD, 2);
  }
}

NXCTRL_VOID
NXCTRLOLEDNormalDisplay (NXCTRLOLED *pOLED) {
  uint8_t rchCMD[4];
  rchCMD[0] = OLED_NORMAL_DISPLAY;
  NXCTRLOLEDCommand(pOLED, rchCMD, 1);
}

NXCTRL_VOID
NXCTRLOLEDSetContrast (NXCTRLOLED *pOLED, uint8_t nContrast) {
  uint8_t rchCMD[4];
  rchCMD[0] = OLED_SET_CONTRAST;
  rchCMD[1] = nContrast;
  NXCTRLOLEDCommand(pOLED, rchCMD, 2);
}

NXCTRL_VOID
NXCTRLOLEDDisplayBlock (NXCTRLOLED *pOLED, NXCTRLBITMAP *pBitmap,
                        int nRow, int nCol,
                        int nColCount, int nColOffset) {
  int nPageCount = pBitmap->nRows >> 3;
  int nPageStart = nRow >> 3;
  int nPageEnd = nPageStart + nPageCount - 1;
  int nColStart = nCol;
  int nColEnd = nCol + nColCount + 1;
  uint8_t rchCMD[4];

  rchCMD[0] = OLED_SET_MEMORY_MODE;
  rchCMD[1] = OLED_MODE_VERT;
  NXCTRLOLEDCommand(pOLED, rchCMD, 2);

  rchCMD[0] = OLED_SET_PAGE_ADDRESS;
  rchCMD[1] = nPageStart;
  rchCMD[2] = nPageEnd;
  NXCTRLOLEDCommand(pOLED, rchCMD, 3);

  rchCMD[0] = OLED_SET_COL_ADDRESS;
  rchCMD[1] = nColStart;
  rchCMD[2] = nColEnd;
  NXCTRLOLEDCommand(pOLED, rchCMD, 3);

  NXCTRLOLEDData(pOLED,
                 pBitmap->rchData + (nColOffset * nPageCount),
                 nColCount * nPageCount);
}

NXCTRL_VOID
NXCTRLOLEDDisplay (NXCTRLOLED *pOLED) {
  NXCTRLOLEDDisplayBlock(pOLED, &(pOLED->bitmap), 0, 0, pOLED->nCols, pOLED->nColOffset);
}

NXCTRL_VOID
NXCTRLOLEDDisplayCols (NXCTRLOLED *pOLED, int nStartCol, int nCount) {
  NXCTRLOLEDDisplayBlock(pOLED, &(pOLED->bitmap),
                         0, nStartCol,
                         nCount, pOLED->nColOffset);
}

NXCTRL_VOID
NXCTRLOLEDDumpBuffer (NXCTRLOLED *pOLED) {
  NXCTRLBITMAPDump(&(pOLED->bitmap));
}


NXCTRL_VOID
NXCTRLOLEDDrawPixel (NXCTRLOLED *pOLED, int x, int y, NXCTRL_BOOL on) {
  NXCTRLBITMAPDrawPixel(&(pOLED->bitmap), x, y, on);
}

NXCTRL_VOID
NXCTRLOLEDDrawText (NXCTRLOLED *pOLED, int x, int y,
                    const char *psz, int n,
                    int nSize, int nSpace) {
  register int i, j, row, sx, sy;
  int nFontRows = 5; // XXX
  int nFontCols = 8; // XXX
  for (i = 0; i < n; i++) {
    int p = psz[i] * nFontCols;
    for (j = 0; i < nFontCols; j++) {
      uint8_t mask = NXCTRL_5X8_FONT[p];
      int py = y;
      p += 1;
      for (row = 0; row < nFontRows; row++) {
        for (sy = 0; sy < nSize; sy++) {
          int px = x;
          for (sx = 0; sx < nSize; sx++) {
            NXCTRLOLEDDrawPixel(pOLED, px, py, mask & 0x01);
            px += 1;
          }
          py += 1;
        }
        mask >>= 1;
      }
      x += nSize;
    }
    x += nSpace;
  }
}

NXCTRL_VOID
NXCTRLOLEDClearBlock (NXCTRLOLED *pOLED, int x0, int y0, int dx, int dy) {
  NXCTRLBITMAPClearBlock(&(pOLED->bitmap), x0, y0, dx, dy);
}
