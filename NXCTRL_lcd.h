#ifndef __NXCTRL_LCD_HEADER__
#define __NXCTRL_LCD_HEADER__

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

typedef struct __tagNXLCDHW {
  int nBits, nRows, nCols;
  int nRSPin, nSTRBPin;
  int rnDataPins[8];
  int nCX, nCY;
} *NXLCDHW;

#endif /* __NXCTRL_LCD_HEADER__ */
