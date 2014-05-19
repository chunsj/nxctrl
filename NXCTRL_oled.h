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

#ifndef __NXCTRL_OLED_HEADER__
#define __NXCTRL_OLED_HEADER__

#include <NXCTRL.h>
#include <inttypes.h>

#define OLED_MAX_COL      256
#define OLED_MAX_ROW      256

#define OLED_EXTERNAL_VCC                  0x1
#define OLED_SWITCH_CAP_VCC                0x2

#define OLED_SET_LOW_COLUMN                0x00
#define OLED_SET_HIGH_COLUMN               0x10
#define OLED_SET_MEMORY_MODE               0x20
#define OLED_SET_COL_ADDRESS               0x21
#define OLED_SET_PAGE_ADDRESS              0x22
#define OLED_RIGHT_HORIZ_SCROLL            0x26
#define OLED_LEFT_HORIZ_SCROLL             0x27
#define OLED_VERT_AND_RIGHT_HORIZ_SCROLL   0x29
#define OLED_VERT_AND_LEFT_HORIZ_SCROLL    0x2A
#define OLED_DEACTIVATE_SCROLL             0x2E
#define OLED_ACTIVATE_SCROLL               0x2F
#define OLED_SET_START_LINE                0x40
#define OLED_SET_CONTRAST                  0x81
#define OLED_CHARGE_PUMP                   0x8D
#define OLED_SEG_REMAP                     0xA0
#define OLED_SET_VERT_SCROLL_AREA          0xA3
#define OLED_DISPLAY_ALL_ON_RESUME         0xA4
#define OLED_DISPLAY_ALL_ON                0xA5
#define OLED_NORMAL_DISPLAY                0xA6
#define OLED_INVERT_DISPLAY                0xA7
#define OLED_DISPLAY_OFF                   0xAE
#define OLED_DISPLAY_ON                    0xAF
#define OLED_COM_SCAN_INC                  0xC0
#define OLED_COM_SCAN_DEC                  0xC8
#define OLED_SET_DISPLAY_OFFSET            0xD3
#define OLED_SET_COM_PINS                  0xDA
#define OLED_SET_VCOM_DETECT               0xDB
#define OLED_SET_DISPLAY_CLOCK_DIV         0xD5
#define OLED_SET_PRECHARGE                 0xD9
#define OLED_SET_MULTIPLEX                 0xA8

#define OLED_MODE_HORIZ                    0x00
#define OLED_MODE_VERT                     0x01
#define OLED_MODE_PAGE                     0x02

typedef struct __tagNXCTRLBITMAP {
  int nCols, nRows;
  int nBytesPerCol;
  uint8_t rchData[OLED_MAX_ROW*OLED_MAX_COL];
} NXCTRLBITMAP;

typedef struct __tagNXCTRLOLED {
  int nCols, nRows;
  int nBufferRows, nBufferCols;
  int nMemBytes;
  uint8_t nDCBank, nDCPin;
  uint8_t nRSBank, nRSPin;
  int nColOffset;
  int nSPIFD;
  NXCTRLBITMAP bitmap;
  NXCTRL_BOOL bFlipped;
} NXCTRLOLED;

NXCTRL_VOID NXCTRLBITMAPInit (NXCTRLBITMAP *pBitmap, int nCols, int nRows);
NXCTRL_VOID NXCTRLBITMAPClear (NXCTRLBITMAP *pBitmap);
NXCTRL_VOID NXCTRLBITMAPDump (NXCTRLBITMAP *pBitmap);
NXCTRL_VOID NXCTRLBITMAPDrawPixel (NXCTRLBITMAP *pBitmap, int x, int y, NXCTRL_BOOL on);
NXCTRL_VOID NXCTRLBITMAPClearBlock (NXCTRLBITMAP *pBitmap, int x0, int y0, int dx, int dy);

NXCTRL_VOID NXCTRLOLEDInit (NXCTRLOLED *pOLED,
                            int nSPIBank, int nCS, int nD1, int nCLK,
                            int nDCBank, int nDCPin,
                            int nRSBank, int nRSPin,
                            int nCols, int nRows,
                            int nBufferCols, int nBufferRows,
                            int nSPIFD);
NXCTRL_VOID NXCTRLOLEDReset (NXCTRLOLED *pOLED);
int NXCTRLOLEDCommand (NXCTRLOLED *pOLED, uint8_t *pnCMD, uint8_t nCMDLength);
int NXCTRLOLEDData (NXCTRLOLED *pOLED, uint8_t *pnData, uint8_t nDataLength);
NXCTRL_VOID NXCTRLOLEDBegin (NXCTRLOLED *pOLED, uint8_t nVCCState);
NXCTRL_VOID NXCTRLOLEDClearDisplay (NXCTRLOLED *pOLED);
NXCTRL_VOID NXCTRLOLEDInvertDisplay (NXCTRLOLED *pOLED);
NXCTRL_VOID NXCTRLOLEDFlipDisplay (NXCTRLOLED *pOLED, NXCTRL_BOOL flipped);
NXCTRL_VOID NXCTRLOLEDNormalDisplay (NXCTRLOLED *pOLED);
NXCTRL_VOID NXCTRLOLEDSetContrast (NXCTRLOLED *pOLED, uint8_t nContrast);
NXCTRL_VOID NXCTRLOLEDDisplayBlock (NXCTRLOLED *pOLED, NXCTRLBITMAP *pBitmap,
                                    int nRow, int nCol,
                                    int nColCount, int nColOffset);
NXCTRL_VOID NXCTRLOLEDDisplay (NXCTRLOLED *pOLED);
NXCTRL_VOID NXCTRLOLEDDisplayCols (NXCTRLOLED *pOLED, int nStartCol, int nCount);
NXCTRL_VOID NXCTRLOLEDDumpBuffer (NXCTRLOLED *pOLED);
NXCTRL_VOID NXCTRLOLEDDrawPixel (NXCTRLOLED *pOLED, int x, int y, NXCTRL_BOOL on);
NXCTRL_VOID NXCTRLOLEDDrawText (NXCTRLOLED *pOLED, int x, int y,
                                const char *psz, int n,
                                int nSize, int nSpace);
NXCTRL_VOID NXCTRLOLEDClearBlock (NXCTRLOLED *pOLED, int x0, int y0, int dx, int dy);

#endif /* __NXCTRL_OLED_HEADER__ */
