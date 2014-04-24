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
#include <NXCTRL_lcd.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define RSBANK      pLCD->nRSBank
#define RSPIN       pLCD->nRSPin
#define RWBANK      pLCD->nRWBank
#define RWPIN       pLCD->nRWPin
#define ENBANK      pLCD->nEnableBank
#define ENPIN       pLCD->nEnablePin
#define D0BANK      pLCD->rnDataBanks[0]
#define D1BANK      pLCD->rnDataBanks[1]
#define D2BANK      pLCD->rnDataBanks[2]
#define D3BANK      pLCD->rnDataBanks[3]
#define D4BANK      pLCD->rnDataBanks[4]
#define D5BANK      pLCD->rnDataBanks[5]
#define D6BANK      pLCD->rnDataBanks[6]
#define D7BANK      pLCD->rnDataBanks[7]
#define D0PIN       pLCD->rnDataPins[0]
#define D1PIN       pLCD->rnDataPins[1]
#define D2PIN       pLCD->rnDataPins[2]
#define D3PIN       pLCD->rnDataPins[3]
#define D4PIN       pLCD->rnDataPins[4]
#define D5PIN       pLCD->rnDataPins[5]
#define D6PIN       pLCD->rnDataPins[6]
#define D7PIN       pLCD->rnDataPins[7]

static void
__NXCTRLLCDPulseEnable (NXCTRLLCD *pLCD) {
  NXCTRLDigitalWrite(ENBANK, ENPIN, NXCTRL_LOW);
  NXCTRLSleep(0, 1);
  NXCTRLDigitalWrite(ENBANK, ENPIN, NXCTRL_HIGH);
  NXCTRLSleep(0, 1);
  NXCTRLDigitalWrite(ENBANK, ENPIN, NXCTRL_LOW);
  NXCTRLSleep(0, 100);
}

static void
__NXCTRLLCDWrite4Bits (NXCTRLLCD *pLCD, uint8_t nValue) {
  int i;
  for (i = 0; i < 4; i++)
    NXCTRLDigitalWrite(pLCD->rnDataBanks[i], pLCD->rnDataPins[i],
                       (nValue >> i) & 0x01);
  __NXCTRLLCDPulseEnable(pLCD);
}

static void
__NXCTRLLCDWrite8Bits (NXCTRLLCD *pLCD, uint8_t nValue) {
  int i;
  for (i = 0; i < 8; i++)
    NXCTRLDigitalWrite(pLCD->rnDataBanks[i], pLCD->rnDataPins[i],
                       (nValue >> i) & 0x01);
  __NXCTRLLCDPulseEnable(pLCD);
}

static void
__NXCTRLLCDSend (NXCTRLLCD *pLCD, uint8_t nValue, uint8_t nMode) {
  NXCTRLDigitalWrite(RSBANK, RSPIN, nMode);

  if (RWBANK == NXCTRL_P8 || RWBANK == NXCTRL_P9)
    if (RWPIN >= NXCTRL_PIN01 && RWPIN <= NXCTRL_PIN46)
      NXCTRLDigitalWrite(RWBANK, RWPIN, NXCTRL_LOW);

  if (pLCD->nDisplayFunction & LCD_8BITMODE)
    __NXCTRLLCDWrite8Bits(pLCD, nValue);
  else {
    __NXCTRLLCDWrite4Bits(pLCD, nValue>>4);
    __NXCTRLLCDWrite4Bits(pLCD, nValue);
  }
}

int
NXCTRLLCDInit (NXCTRLLCD *pLCD,
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
               int nD7Bank, int nD7Pin) {
  int i;
  
  if (!pLCD) {
    fprintf(stderr, "NXCTRLLCDInit: invalid LCD\n");
    return -1;
  }

  RSBANK = nRSBank; RSPIN = nRSPin;
  RWBANK = nRWBank; RWPIN = nRWPin;
  ENBANK = nEnableBank; ENPIN = nEnablePin;

  D0BANK = nD0Bank; D0PIN = nD0Pin;
  D1BANK = nD1Bank; D1PIN = nD1Pin;
  D2BANK = nD2Bank; D2PIN = nD2Pin;
  D3BANK = nD3Bank; D3PIN = nD3Pin;
  D4BANK = nD4Bank; D4PIN = nD4Pin;
  D5BANK = nD5Bank; D5PIN = nD5Pin;
  D6BANK = nD6Bank; D6PIN = nD6Pin;
  D7BANK = nD7Bank; D7PIN = nD7Pin;

  NXCTRLPinMode(RSBANK, RSPIN, NXCTRL_OUTPUT);
  if (RWBANK == NXCTRL_P8 || RWBANK == NXCTRL_P9)
    if (RWPIN >= NXCTRL_PIN01 && RWPIN <= NXCTRL_PIN46)
      NXCTRLPinMode(RWBANK, RWPIN, NXCTRL_OUTPUT);
  NXCTRLPinMode(ENBANK, ENPIN, NXCTRL_OUTPUT);

  if (D0PIN >= NXCTRL_PIN01 && D4PIN >= 255) { // 4 bit mode
    pLCD->nDisplayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    for (i = 0; i < 4; i++)
      NXCTRLPinMode(pLCD->rnDataBanks[i], pLCD->rnDataPins[i], NXCTRL_OUTPUT);
  } else {
    pLCD->nDisplayFunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
    for (i = 0; i < 8; i++)
      NXCTRLPinMode(pLCD->rnDataBanks[i], pLCD->rnDataPins[i], NXCTRL_OUTPUT);
  }

  NXCTRLLCDBegin(pLCD, 16, 1);

  return 0;
}

void
NXCTRLLCDBegin (NXCTRLLCD *pLCD, uint8_t nCols, uint8_t nRows) {
  if (nRows > 1)
    pLCD->nDisplayFunction |= LCD_2LINE;

  pLCD->nLines = nRows;
  pLCD->nCurrLine = 0;

  // XXX for 5x10DOTS, how??? Refer Arduino's case
  //pLCD->nDisplayFunction |= LCD_5x10DOTS;

  NXCTRLSleep(50, 0);

  NXCTRLDigitalWrite(RSBANK, RSPIN, NXCTRL_LOW);
  NXCTRLDigitalWrite(ENBANK, ENPIN, NXCTRL_LOW);

  if (RWBANK == NXCTRL_P8 || RWBANK == NXCTRL_P9)
    if (RWPIN >= NXCTRL_PIN01 && RWPIN <= NXCTRL_PIN46)
      NXCTRLDigitalWrite(RWBANK, RWPIN, NXCTRL_LOW);

  if (!(pLCD->nDisplayFunction & LCD_8BITMODE)) { // 4 bit mode
    __NXCTRLLCDWrite4Bits(pLCD, 0x03);
    NXCTRLSleep(10, 0);
    __NXCTRLLCDWrite4Bits(pLCD, 0x03);
    NXCTRLSleep(10, 0);
    __NXCTRLLCDWrite4Bits(pLCD, 0x03);
    NXCTRLSleep(10, 0);
    __NXCTRLLCDWrite4Bits(pLCD, 0x02);
  } else {
    NXCTRLLCDCommand(pLCD, LCD_FUNCTIONSET | pLCD->nDisplayFunction);
    NXCTRLSleep(10, 0);
    NXCTRLLCDCommand(pLCD, LCD_FUNCTIONSET | pLCD->nDisplayFunction);
    NXCTRLSleep(10, 0);
    NXCTRLLCDCommand(pLCD, LCD_FUNCTIONSET | pLCD->nDisplayFunction);
  }

  NXCTRLLCDCommand(pLCD, LCD_FUNCTIONSET | pLCD->nDisplayFunction);

  pLCD->nDisplayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  NXCTRLLCDDisplay(pLCD, NXCTRL_ON);

  NXCTRLLCDClear(pLCD);

  pLCD->nDisplayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  NXCTRLLCDCommand(pLCD, LCD_ENTRYMODESET | pLCD->nDisplayMode);
}

void
NXCTRLLCDClear (NXCTRLLCD *pLCD) {
  NXCTRLLCDCommand(pLCD, LCD_CLEARDISPLAY);
  NXCTRLSleep(10, 0);
}

void
NXCTRLLCDHome (NXCTRLLCD *pLCD) {
  NXCTRLLCDCommand(pLCD, LCD_RETURNHOME);
  NXCTRLSleep(10, 0);
}

void
NXCTRLLCDDisplay (NXCTRLLCD *pLCD, NXCTRL_BOOL f) {
  if (f)
    pLCD->nDisplayControl |= LCD_DISPLAYON;
  else
    pLCD->nDisplayControl &= ~LCD_DISPLAYON;
  NXCTRLLCDCommand(pLCD, LCD_DISPLAYCONTROL | pLCD->nDisplayControl);
}

void
NXCTRLLCDBlink (NXCTRLLCD *pLCD, NXCTRL_BOOL f) {
  if (f)
    pLCD->nDisplayControl |= LCD_BLINKON;
  else
    pLCD->nDisplayControl &= ~LCD_BLINKON;
  NXCTRLLCDCommand(pLCD, LCD_DISPLAYCONTROL | pLCD->nDisplayControl);
}

void
NXCTRLLCDCursor (NXCTRLLCD *pLCD, NXCTRL_BOOL f) {
  if (f)
    pLCD->nDisplayControl |= LCD_CURSORON;
  else
    pLCD->nDisplayControl &= ~LCD_CURSORON;
  NXCTRLLCDCommand(pLCD, LCD_DISPLAYCONTROL | pLCD->nDisplayControl);
}

void
NXCTRLLCDScrollDisplay (NXCTRLLCD *pLCD, NXCTRL_BOOL fToLeft) {
  if (fToLeft)
    NXCTRLLCDCommand(pLCD, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
  else
    NXCTRLLCDCommand(pLCD, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void
NXCTRLLCDAutoScroll (NXCTRLLCD *pLCD, NXCTRL_BOOL f) {
  if (f)
    pLCD->nDisplayMode |= LCD_ENTRYSHIFTINCREMENT;
  else
    pLCD->nDisplayMode &= ~LCD_ENTRYSHIFTINCREMENT;
  NXCTRLLCDCommand(pLCD, LCD_ENTRYMODESET | pLCD->nDisplayMode);
}

void
NXCTRLLCDCreateChar (NXCTRLLCD *pLCD, uint8_t nLoc, uint8_t rch[]) {
  int i;
  nLoc &= 0x07;
  NXCTRLLCDCommand(pLCD, LCD_SETCGRAMADDR | (nLoc << 3));
  for (i = 0; i < 8; i++)
    NXCTRLLCDWrite(pLCD, rch[i]);
}

void
NXCTRLLCDSetCursor (NXCTRLLCD *pLCD, uint8_t nCol, uint8_t nRow) {
  int nRowOffsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if (nRow >= pLCD->nLines)
    nRow = pLCD->nLines - 1;
  NXCTRLLCDCommand(pLCD, LCD_SETDDRAMADDR | (nCol + nRowOffsets[nRow]));
}

void
NXCTRLLCDCommand (NXCTRLLCD *pLCD, uint8_t nValue) {
  __NXCTRLLCDSend(pLCD, nValue, NXCTRL_LOW);
}

size_t
NXCTRLLCDWrite (NXCTRLLCD *pLCD, uint8_t nValue) {
  __NXCTRLLCDSend(pLCD, nValue, NXCTRL_HIGH);
  return 1;
}

size_t
NXCTRLLCDPutChar (NXCTRLLCD *pLCD, char ch) {
  return NXCTRLLCDWrite(pLCD, ch);
}

size_t
NXCTRLLCDPutStr (NXCTRLLCD *pLCD, const char str[]) {
  int i;
  size_t n = strlen(str);
  for (i = 0; i < n; i++)
    NXCTRLLCDWrite(pLCD, str[i]);
  return n;
}

size_t
NXCTRLLCDPrint (NXCTRLLCD *pLCD, const char *msg, ...) {
  va_list argp;
  char rchBuf[BUFSIZ];
  va_start(argp, msg);
  vsnprintf(rchBuf, BUFSIZ-1, msg, argp);
  va_end(argp);
  return NXCTRLLCDPutStr(pLCD, rchBuf);
}
