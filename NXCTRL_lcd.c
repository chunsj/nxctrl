#include <stdio.h>
#include <stdarg.h>
#include <NXCTRL_lcd.h>

#define NXCTRL_LCDCMD_CLEAR      BIT0
#define NXCTRL_LCDCMD_HOME       BIT1
#define NXCTRL_LCDCMD_ENTRY      BIT2
#define NXCTRL_LCDCMD_CTRL       BIT3
#define NXCTRL_LCDCMD_CDSHIFT    BIT4
#define NXCTRL_LCDCMD_FUNC       BIT5
#define NXCTRL_LCDCMD_CGRAM      BIT6
#define NXCTRL_LCDCMD_DGRAM      BIT7

#define NXCTRL_LCDENTRY_SH       BIT0
#define NXCTRL_LCDENTRY_ID       BIT1

#define NXCTRL_LCDCTRL_BLINK     BIT0
#define NXCTRL_LCDCTRL_CURSOR    BIT1
#define NXCTRL_LCDCTRL_DISPLAY   BIT2

#define NXCTRL_LCDFUNC_F         BIT2
#define NXCTRL_LCDFUNC_N         BIT3
#define NXCTRL_LCDFUNC_DL        BIT4

#define NXCTRL_LCDCDSHIFT_RL     BIT2

static void
__NXCTRLLCDStrobe (NXLCDHWPtr hwLCD) {
  NXCTRLDigitalWrite(hwLCD->nSTRBBank, hwLCD->nSTRBPin, NXCTRL_HIGH);
  NXCTRLSleep(50, 0);
  NXCTRLDigitalWrite(hwLCD->nSTRBBank, hwLCD->nSTRBPin, NXCTRL_LOW);
  NXCTRLSleep(50, 0);
}

static void
__NXCTRLLCDSendCMD (NXLCDHWPtr hwLCD, unsigned char nData) {
  int i;
  if (hwLCD->nBits == 4) {
    unsigned char nD4 = (nData >> 4) & 0x0F;
    for (i = 0; i < 4; i++) {
      NXCTRLDigitalWrite(hwLCD->rnDataBanks[i], hwLCD->rnDataPins[i], (nD4 & 1));
      nD4 >>= 1;
    }
    __NXCTRLLCDStrobe(hwLCD);

    nD4 = nData & 0x0F;
    for (i = 0; i < 4; i++) {
      NXCTRLDigitalWrite(hwLCD->rnDataBanks[i], hwLCD->rnDataPins[i], (nD4 & 1));
      nD4 >>= 1;
    }
  } else {
    for (i = 0; i < 8; i++) {
      NXCTRLDigitalWrite(hwLCD->rnDataBanks[i], hwLCD->rnDataPins[i], (nData & 1));
      nData >>= 1;
    }
  }
  __NXCTRLLCDStrobe(hwLCD);
}

static void
__NXCTRLLCDCommand (NXLCDHWPtr hwLCD, unsigned char nCMD) {
  NXCTRLDigitalWrite(hwLCD->nRSBank, hwLCD->nRSPin, NXCTRL_LOW);
  __NXCTRLLCDSendCMD(hwLCD, nCMD);
  NXCTRLSleep(2, 0);
}

static void
__NXCTRLLCDCommand4 (NXLCDHWPtr hwLCD, unsigned char nCMD) {
  int i;
  
  NXCTRLDigitalWrite(hwLCD->nRSBank, hwLCD->nRSPin, NXCTRL_LOW);

  for (i = 0; i < 4; i++) {
    NXCTRLDigitalWrite(hwLCD->rnDataBanks[i], hwLCD->rnDataPins[i],
                       (nCMD & 1));
    nCMD >>= 1;
  }

  __NXCTRLLCDStrobe(hwLCD);
}

int
NXCTRLLCDInit (NXLCDHWPtr hwLCD,
               int nRows, int nCols, int nBits,
               int nRSBank, int nSTRBBank,
               int nRS, int nSTRB,
               int nB0, int nB1, int nB2, int nB3,
               int nB4, int nB5, int nB6, int nB7,
               int nD0, int nD1, int nD2, int nD3,
               int nD4, int nD5, int nD6, int nD7) {
  int i;
  unsigned char nFunc;
  
  if ((nRows < 0) || (nRows > 4)) {
    fprintf(stderr, "NXCTRLLCDInit: invalid row count: %d\n", nRows);
    return -1;
  }
  if ((nCols < 0) || (nCols > 20)) {
    fprintf(stderr, "NXCTRLLCDInit: invalid column count: %d\n", nCols);
    return -1;
  }
  if (!((nBits == 4) || (nBits == 8))) {
    fprintf(stderr, "NXCTRLLCDInit: invalid data bit count: %d\n", nBits);
    return -1;
  }

  hwLCD->nRSBank   = nRSBank;
  hwLCD->nSTRBBank = nSTRBBank;
  hwLCD->nRSPin    = nRS;
  hwLCD->nSTRBPin  = nSTRB;
  hwLCD->nBits     = nBits;
  hwLCD->nRows     = nRows;
  hwLCD->nCols     = nCols;
  hwLCD->nCX       = 0;
  hwLCD->nCY       = 0;
  hwLCD->nCTRL     = 0;

  hwLCD->rnDataBanks[0] = nB0;
  hwLCD->rnDataBanks[1] = nB1;
  hwLCD->rnDataBanks[2] = nB2;
  hwLCD->rnDataBanks[3] = nB3;
  hwLCD->rnDataBanks[4] = nB4;
  hwLCD->rnDataBanks[5] = nB5;
  hwLCD->rnDataBanks[6] = nB6;
  hwLCD->rnDataBanks[7] = nB7;

  hwLCD->rnDataPins[0] = nD0;
  hwLCD->rnDataPins[1] = nD1;
  hwLCD->rnDataPins[2] = nD2;
  hwLCD->rnDataPins[3] = nD3;
  hwLCD->rnDataPins[4] = nD4;
  hwLCD->rnDataPins[5] = nD5;
  hwLCD->rnDataPins[6] = nD6;
  hwLCD->rnDataPins[7] = nD7;

  NXCTRLPinMode(hwLCD->nRSBank, hwLCD->nRSPin, NXCTRL_OUTPUT);
  NXCTRLDigitalWrite(hwLCD->nRSBank, hwLCD->nRSPin, NXCTRL_LOW);
  NXCTRLPinMode(hwLCD->nSTRBBank, hwLCD->nSTRBPin, NXCTRL_OUTPUT);
  NXCTRLDigitalWrite(hwLCD->nSTRBBank, hwLCD->nSTRBPin, NXCTRL_LOW);

  for (i = 0; i < nBits; i++) {
    NXCTRLPinMode(hwLCD->rnDataBanks[i], hwLCD->rnDataPins[i], NXCTRL_OUTPUT);
    NXCTRLDigitalWrite(hwLCD->rnDataBanks[i], hwLCD->rnDataPins[i], NXCTRL_LOW);
  }

  NXCTRLSleep(50, 0);

  if (nBits == 4) {
    nFunc = NXCTRL_LCDCMD_FUNC | NXCTRL_LCDFUNC_DL;
    __NXCTRLLCDCommand4(hwLCD, nFunc >> 4); NXCTRLSleep(50, 0);
    __NXCTRLLCDCommand4(hwLCD, nFunc >> 4); NXCTRLSleep(50, 0);
    __NXCTRLLCDCommand4(hwLCD, nFunc >> 4); NXCTRLSleep(50, 0);
    nFunc = NXCTRL_LCDCMD_FUNC;
    __NXCTRLLCDCommand4(hwLCD, nFunc >> 4); NXCTRLSleep(50, 0);
  } else {
    nFunc = NXCTRL_LCDCMD_FUNC | NXCTRL_LCDFUNC_DL;
    __NXCTRLLCDCommand(hwLCD, nFunc); NXCTRLSleep(50, 0);
    __NXCTRLLCDCommand(hwLCD, nFunc); NXCTRLSleep(50, 0);
    __NXCTRLLCDCommand(hwLCD, nFunc); NXCTRLSleep(50, 0);
  }

  if (nRows > 1) {
    nFunc |= NXCTRL_LCDFUNC_N;
    __NXCTRLLCDCommand(hwLCD, nFunc); NXCTRLSleep(50, 0);
  }

  NXCTRLLCDDisplay(hwLCD, NXCTRL_ON);
  NXCTRLLCDCursor(hwLCD, NXCTRL_OFF);
  NXCTRLLCDCursorBlink(hwLCD, NXCTRL_OFF);
  NXCTRLLCDClear(hwLCD);

  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_ENTRY | NXCTRL_LCDENTRY_ID);
  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_CDSHIFT | NXCTRL_LCDCDSHIFT_RL);

  return 0;
}

void
NXCTRLLCDHome (NXLCDHWPtr hwLCD) {
  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_HOME);
  hwLCD->nCX = 0;
  hwLCD->nCY = 0;
  NXCTRLSleep(5, 0);
}

void
NXCTRLLCDClear (NXLCDHWPtr hwLCD) {
  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_CLEAR);
  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_HOME);
  hwLCD->nCX = 0;
  hwLCD->nCY = 0;
  NXCTRLSleep(5, 0);
}

void
NXCTRLLCDDisplay (NXLCDHWPtr hwLCD, NXCTRL_BOOL f) {
  if (f)
    hwLCD->nCTRL |= NXCTRL_LCDCTRL_DISPLAY;
  else
    hwLCD->nCTRL &= ~NXCTRL_LCDCTRL_DISPLAY;

  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_CTRL | hwLCD->nCTRL);
}

void
NXCTRLLCDCursor (NXLCDHWPtr hwLCD, NXCTRL_BOOL f) {
  if (f)
    hwLCD->nCTRL |= NXCTRL_LCDCTRL_CURSOR;
  else
    hwLCD->nCTRL &= ~NXCTRL_LCDCTRL_CURSOR;

  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_CTRL | hwLCD->nCTRL);
}

void
NXCTRLLCDCursorBlink (NXLCDHWPtr hwLCD, NXCTRL_BOOL f) {
  if (f)
    hwLCD->nCTRL |= NXCTRL_LCDCTRL_BLINK;
  else
    hwLCD->nCTRL &= ~NXCTRL_LCDCTRL_BLINK;

  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_CTRL | hwLCD->nCTRL);
}

static const int ROWOFF[4] = { 0x00, 0x40, 0x14, 0x54 };

void
NXCTRLLCDPosition (NXLCDHWPtr hwLCD, int x, int y) {
  if ((x >= hwLCD->nCols) || (x < 0))
    return;
  if ((y >= hwLCD->nRows) || (y < 0))
    return;

  __NXCTRLLCDCommand(hwLCD, x + (NXCTRL_LCDCMD_DGRAM | ROWOFF[y]));

  hwLCD->nCY = x;
  hwLCD->nCY = y;
}

void
NXCTRLLCDCharDefine (NXLCDHWPtr hwLCD, int nIdx, unsigned char rchData[8]) {
  int i;
  
  __NXCTRLLCDCommand(hwLCD, NXCTRL_LCDCMD_CGRAM | ((nIdx & 7) << 3));
  NXCTRLDigitalWrite(hwLCD->nRSBank, hwLCD->nRSPin, NXCTRL_HIGH);

  for (i = 0; i < 8; i++)
    __NXCTRLLCDSendCMD(hwLCD, rchData[i]);
}

void
NXCTRLLCDCharPut (NXLCDHWPtr hwLCD, unsigned char ch) {
  NXCTRLDigitalWrite(hwLCD->nRSBank, hwLCD->nRSPin, NXCTRL_HIGH);
  __NXCTRLLCDSendCMD(hwLCD, ch);
  if (hwLCD->nCX++ == hwLCD->nCols) {
    hwLCD->nCX = 0;
    if (hwLCD->nCY++ == hwLCD->nRows)
      hwLCD->nCY = 0;
    __NXCTRLLCDCommand(hwLCD, hwLCD->nCX + (NXCTRL_LCDCMD_DGRAM | ROWOFF[hwLCD->nCY]));
  }
}

void
NXCTRLLCDStrPut (NXLCDHWPtr hwLCD, const char *psz) {
  while (*psz)
    NXCTRLLCDCharPut(hwLCD, *psz++);
}

void
NXCTRLStrPrint (NXLCDHWPtr hwLCD, const char *psz, ...) {
  va_list argp;
  char rchBuf[BUFSIZ];
  va_start(argp, psz);
  vsnprintf(rchBuf, BUFSIZ-1, psz, argp);
  va_end(argp);
  NXCTRLLCDStrPut(hwLCD, rchBuf);
}
