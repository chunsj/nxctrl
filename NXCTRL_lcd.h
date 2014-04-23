#ifndef __NXCTRL_LCD_HEADER__
#define __NXCTRL_LCD_HEADER__

#include <NXCTRL.h>

typedef struct __tagNXLCDHW {
  int nBits, nRows, nCols;
  int nRSBank, nSTRBBank;
  int nRSPin, nSTRBPin;
  int rnDataBanks[8];
  int rnDataPins[8];
  int nCX, nCY;
  int nCTRL;
} NXLCDHW, *NXLCDHWPtr;

int NXCTRLLCDInit (NXLCDHWPtr hwLCD,
                   int nRows, int nCols, int nBits,
                   int nRSBank, int nSTRBBank,
                   int nRS, int nSTRB,
                   int nB0, int nB1, int nB2, int nB3,
                   int nB4, int nB5, int nB6, int nB7,
                   int nD0, int nD1, int nD2, int nD3,
                   int nD4, int nD5, int nD6, int nD7);

void NXCTRLLCDHome (NXLCDHWPtr hwLCD);
void NXCTRLLCDClear (NXLCDHWPtr hwLCD);
void NXCTRLLCDDisplay (NXLCDHWPtr hwLCD, NXCTRL_BOOL f);
void NXCTRLLCDCursor (NXLCDHWPtr hwLCD, NXCTRL_BOOL f);
void NXCTRLLCDCursorBlink (NXLCDHWPtr hwLCD, NXCTRL_BOOL f);

void NXCTRLLCDPosition (NXLCDHWPtr hwLCD, int x, int y);

void NXCTRLLCDCharDefine (NXLCDHWPtr hwLCD, int nIdx, unsigned char rchData[8]);
void NXCTRLLCDCharPut (NXLCDHWPtr hwLCD, unsigned char ch);
void NXCTRLLCDStrPut (NXLCDHWPtr hwLCD, const char *psz);
void NXCTRLLCDStrPrint (NXLCDHWPtr hwLCD, const char *psz, ...);

#endif /* __NXCTRL_LCD_HEADER__ */
