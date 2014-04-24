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

#ifndef __NXCTRL_LCD_HEADER__
#define __NXCTRL_LCD_HEADER__

#include <NXCTRL.h>
#include <inttypes.h>

typedef struct __tagNXCTRLCD {
  uint8_t nRSBank, nRSPin;
  uint8_t nRWBank, nRWPin;
  uint8_t nEnableBank, nEnablePin;
  uint8_t rnDataBanks[8], rnDataPins[8];
  uint8_t nDisplayFunction;
  uint8_t nDisplayControl;
  uint8_t nDisplayMode;
  uint8_t nInitialized;
  uint8_t nLines, nCurrLine;
} NXCTRLLCD;

int NXCTRLLCDInit (NXCTRLLCD *pLCD,
                   int nRSBank, int nRSPin,
                   int nRWBank, int nRWPin,
                   int nEnableBank, int nEnablePin,
                   int nD0Bank, int nD0Pin,
                   int nD1Bank, int nD1Pin,
                   int nD2Bank, int nD2Pin,
                   int nD3Bank, int nD3Pin,
                   int nD4Bank, int nD4Pin,
                   int nD5Bank, int nD5Pin,
                   int nD6Bank, int nD6Pin,
                   int nD7Bank, int nD7Pin);

void NXCTRLLCDBegin (NXCTRLLCD *pLCD, uint8_t nCols, uint8_t nRows);

void NXCTRLLCDClear (NXCTRLLCD *pLCD);
void NXCTRLLCDHome (NXCTRLLCD *pLCD);

void NXCTRLLCDDisplay (NXCTRLLCD *pLCD, NXCTRL_BOOL f);
void NXCTRLLCDBlink (NXCTRLLCD *pLCD, NXCTRL_BOOL f);
void NXCTRLLCDCursor (NXCTRLLCD *pLCD, NXCTRL_BOOL f);
void NXCTRLLCDScrollDisplay (NXCTRLLCD *pLCD, NXCTRL_BOOL fToLeft);
void NXCTRLLCDAutoScroll (NXCTRLLCD *pLCD, NXCTRL_BOOL f);

void NXCTRLLCDCreateChar (NXCTRLLCD *pLCD, uint8_t nLoc, uint8_t rch[]);
void NXCTRLLCDSetCursor (NXCTRLLCD *pLCD, uint8_t nCol, uint8_t nRow);
size_t NXCTRLLCDWrite (NXCTRLLCD *pLCD, uint8_t nValue);
void NXCTRLLCDCommand (NXCTRLLCD *pLCD, uint8_t nValue);

size_t NXCTRLLCDPutChar (NXCTRLLCD *pLCD, char ch);
size_t NXCTRLLCDPutStr (NXCTRLLCD *pLCD, const char str[]);
size_t NXCTRLLCDPrint (NXCTRLLCD *pLCD, const char *msg, ...);

#endif /* __NXCTRL_LCD_HEADER__ */
