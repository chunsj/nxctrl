/*
 * NXCTRL BeagleBone Black Control Library
 *
 * SSD1306 OLED SPI Test Program
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
#include <string.h>
#include <strings.h>
#include <NXCTRL.h>
#include <NXCTRL_oled.h>

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

extern long int random (void);

#define BNK               NXCTRL_P9

#define SPI_CS0           17
#define SPI_D0            21
#define SPI_D1            18
#define SPI_CLK           22

#define OLED_DC           13
#define OLED_RST          15
#define OLED_CS           SPI_CS0
#define OLED_DATA         SPI_D1
#define OLED_CLK          SPI_CLK

NXCTRLOLED oled;
int nFD;

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const NXCTRL_UINT8 logo16_glcd_bmp[] = {
  0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000
};


#define NUMFLAKES 10
#define ANI_COUNT 50

NXCTRL_VOID
__Animate (NXCTRL_VOID) {
  NXCTRL_UINT8 rchIcons[NUMFLAKES][3];
  NXCTRL_UINT8 f, nCnt = 0;

  for (f = 0; f < NUMFLAKES; f++) {
    rchIcons[f][0] = random() % OLED_WIDTH;
    rchIcons[f][1] = 0;
    rchIcons[f][2] = random() % 5 + 1;
  }

  while (nCnt++ <= ANI_COUNT) {
    for (f = 0; f < NUMFLAKES; f++)
      NXCTRLOLEDDrawBitmap(&oled,
                           rchIcons[f][0], rchIcons[f][1],
                           logo16_glcd_bmp,
                           LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT,
                           NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
    NXCTRLSleep(200, 0);

    for (f = 0; f < NUMFLAKES; f++) {
      NXCTRLOLEDDrawBitmap(&oled,
                           rchIcons[f][0], rchIcons[f][1],
                           logo16_glcd_bmp, 
                           LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT,
                           NXCTRL_OFF);
      rchIcons[f][1] += rchIcons[f][2];

      if (rchIcons[f][1] > OLED_HEIGHT) {
        rchIcons[f][0] = random() % OLED_WIDTH;
        rchIcons[f][1] = 0;
        rchIcons[f][2] = random() % 5 + 1;
      }
    }
  }
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int i;
  char ch;
  uint8_t nLSB;
  uint32_t nSpeed, nSPIMode;
  NXCTRL_BOOL f;

  NXCTRLPinMux(BNK, SPI_CS0, NXCTRL_MODE0, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(BNK, SPI_D1, NXCTRL_MODE0, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(BNK, SPI_D0, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);
  NXCTRLPinMux(BNK, SPI_CLK, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);

  nFD = open("/dev/spidev1.0", O_RDWR);

  nLSB = 0;
  ioctl(nFD, SPI_IOC_WR_LSB_FIRST, &nLSB);
  nSpeed = 20000000;
  ioctl(nFD, SPI_IOC_WR_MAX_SPEED_HZ, &nSpeed);
  nSPIMode = SPI_MODE_0;
  ioctl(nFD, SPI_IOC_WR_MODE, &nSPIMode);

  fprintf(stdout, "Running Demo... "); fflush(stdout);

  NXCTRLOLEDInit(&oled,
                 BNK, OLED_DC, BNK, OLED_RST,
                 nFD);

  NXCTRLOLEDDisplayNormal(&oled);
  NXCTRLOLEDSetBanner(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(1000, 0);
  
  NXCTRLOLEDDisplayInverse(&oled);
  NXCTRLSleep(1000, 0);
  
  NXCTRLOLEDDisplayNormal(&oled);
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);
  
  NXCTRLOLEDDrawPixel(&oled, 10, 10, NXCTRL_ON);
  NXCTRLOLEDDrawPixel(&oled, 10, OLED_HEIGHT - 10, NXCTRL_ON);
  NXCTRLOLEDDrawPixel(&oled, OLED_WIDTH - 10, 10, NXCTRL_ON);
  NXCTRLOLEDDrawPixel(&oled, OLED_WIDTH - 10, OLED_HEIGHT - 10, NXCTRL_ON);
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(1000, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_WIDTH; i += 4) {
    NXCTRLOLEDDrawLine(&oled, 0, 0, i, OLED_HEIGHT - 1, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  for (i = 0; i < OLED_HEIGHT; i += 4) {
    NXCTRLOLEDDrawLine(&oled, 0, 0, OLED_WIDTH - 1, i, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_WIDTH; i += 4) {
    NXCTRLOLEDDrawLine(&oled, 0, OLED_HEIGHT - 1, i, 0, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  for (i = OLED_HEIGHT - 1; i >= 0; i -= 4) {
    NXCTRLOLEDDrawLine(&oled, 0, OLED_HEIGHT - 1, OLED_WIDTH - 1, i, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = OLED_WIDTH - 1; i >= 0 ; i -= 4) {
    NXCTRLOLEDDrawLine(&oled, OLED_WIDTH - 1, OLED_HEIGHT - 1, i, 0, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  for (i = OLED_HEIGHT - 1; i >= 0; i -= 4) {
    NXCTRLOLEDDrawLine(&oled, OLED_WIDTH - 1, OLED_HEIGHT - 1, 0, i, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_HEIGHT; i += 4) {
    NXCTRLOLEDDrawLine(&oled, OLED_WIDTH - 1, 0, 0, i, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  for (i = 0; i < OLED_WIDTH; i += 4) {
    NXCTRLOLEDDrawLine(&oled, OLED_WIDTH - 1, 0, i, OLED_HEIGHT - 1, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_HEIGHT/2; i += 2) {
    NXCTRLOLEDDrawRect(&oled,
                       i, i,
                       OLED_WIDTH - 2 * i, OLED_HEIGHT - 2 * i, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_HEIGHT/2; i += 3) {
    NXCTRLOLEDFillRect(&oled,
                       i, i,
                       OLED_WIDTH - i*2, OLED_HEIGHT - 2*i,
                       (i%2) == 0 ? NXCTRL_ON : NXCTRL_OFF);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_HEIGHT; i += 2) {
    NXCTRLOLEDDrawCircle(&oled,
                         OLED_WIDTH/2, OLED_HEIGHT/2,
                         i, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  NXCTRLOLEDDrawCircle(&oled,
                       OLED_WIDTH/2, OLED_HEIGHT/2,
                       30, NXCTRL_ON);
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  NXCTRLOLEDFillCircle(&oled,
                       OLED_WIDTH/2, OLED_HEIGHT/2,
                       30, NXCTRL_ON);
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_HEIGHT/2 - 2; i += 2) {
    NXCTRLOLEDDrawRoundRect(&oled,
                            i, i,
                            OLED_WIDTH - 2*i, OLED_HEIGHT - 2*i,
                            OLED_HEIGHT/4, NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_HEIGHT/2 - 2; i += 3) {
    NXCTRLOLEDFillRoundRect(&oled,
                            i, i,
                            OLED_WIDTH - 2*i, OLED_HEIGHT - 2*i,
                            OLED_HEIGHT/4,
                            (i%2) == 0 ? NXCTRL_ON : NXCTRL_OFF);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);

  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  NXCTRLOLEDDrawRoundRect(&oled,
                          10, 10,
                          OLED_WIDTH - 20, OLED_HEIGHT - 20,
                          OLED_HEIGHT/4, NXCTRL_ON);
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  NXCTRLOLEDFillRoundRect(&oled,
                          15, 15,
                          OLED_WIDTH - 30, OLED_HEIGHT - 30,
                          OLED_HEIGHT/4, NXCTRL_ON);
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (i = 0; i < OLED_HEIGHT / 2; i += 5) {
    NXCTRLOLEDDrawTriangle(&oled,
                           OLED_WIDTH/2, OLED_HEIGHT/2 - i,
                           OLED_WIDTH/2 - i, OLED_HEIGHT/2 + i,
                           OLED_WIDTH/2 + i, OLED_HEIGHT/2 + i,
                           NXCTRL_ON);
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  f = NXCTRL_ON;
  for (i = OLED_HEIGHT/2; i > 0; i -= 5) {
    NXCTRLOLEDFillTriangle(&oled,
                           OLED_WIDTH/2, OLED_HEIGHT/2 - i,
                           OLED_WIDTH/2 - i, OLED_HEIGHT/2 + i,
                           OLED_WIDTH/2 + i, OLED_HEIGHT/2 + i,
                           f);
    if (f) f = NXCTRL_OFF;
    else f = NXCTRL_ON;
    NXCTRLOLEDUpdateDisplay(&oled);
  }
  NXCTRLSleep(250, 0);
  
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  for (ch = 0; ch < 168; ch++) {
    if (ch == '\n') continue;
    NXCTRLOLEDWrite(&oled, ch);
  }
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(1000, 0);

  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  NXCTRLOLEDSetCursor(&oled, 46, 28);
  NXCTRLOLEDWrite(&oled, '1');
  NXCTRLOLEDWrite(&oled, '2');
  NXCTRLOLEDWrite(&oled, '3');
  NXCTRLOLEDWrite(&oled, '.');
  NXCTRLOLEDWrite(&oled, '4');
  NXCTRLOLEDWrite(&oled, '5');
  NXCTRLOLEDWrite(&oled, '6');
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(1000, 0);

  NXCTRLOLEDStartScrollRight(&oled, 0x00, 0x0F);
  NXCTRLSleep(2000, 0);
  NXCTRLOLEDStopScroll(&oled);
  NXCTRLSleep(1000, 0);
  NXCTRLOLEDStartScrollLeft(&oled, 0x00, 0x0F);
  NXCTRLSleep(2000, 0);
  NXCTRLOLEDStopScroll(&oled);
  NXCTRLSleep(1000, 0);
  NXCTRLOLEDStartScrollDiagRight(&oled, 0x00, 0x07);
  NXCTRLSleep(2000, 0);
  NXCTRLOLEDStartScrollDiagLeft(&oled, 0x00, 0x07);
  NXCTRLSleep(2000, 0);
  NXCTRLOLEDStopScroll(&oled);
  NXCTRLSleep(1000, 0);

  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  NXCTRLOLEDDrawBitmap(&oled,
                       30, 16,
                       logo16_glcd_bmp,
                       LOGO16_GLCD_WIDTH,
                       LOGO16_GLCD_HEIGHT,
                       NXCTRL_ON);
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(1000, 0);

  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  __Animate();
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  fprintf(stdout, "done\n"); fflush(stdout);

  NXCTRLOLEDSetCursor(&oled, 46, 28);
  NXCTRLOLEDWrite(&oled, 'D');
  NXCTRLOLEDWrite(&oled, 'I');
  NXCTRLOLEDWrite(&oled, 'S');
  NXCTRLOLEDWrite(&oled, 'P');
  NXCTRLOLEDWrite(&oled, 'L');
  NXCTRLOLEDWrite(&oled, 'A');
  NXCTRLOLEDWrite(&oled, 'Y');
  NXCTRLOLEDUpdateDisplay(&oled);
}

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
  char rchLine[BUFSIZ];
  int i;
  memset(rchLine, 0, BUFSIZ);
  fprintf(stdout, "DISPLAY: ");
  fgets(rchLine, 1024, stdin);
  if (strlen(rchLine) > 0)
    rchLine[strlen(rchLine)-1] = 0;

  if (strcasecmp(rchLine, ".quit") == 0) {
    NXCTRLOLEDClearDisplay(&oled);
    NXCTRLOLEDUpdateDisplay(&oled);
    close(nFD);
    NXCTRLExitLoop();
    return;
  }

  NXCTRLOLEDSetCursor(&oled, 0, 0);
  NXCTRLOLEDClearDisplay(&oled);

  for (i = 0; i < strlen(rchLine); i++)
    NXCTRLOLEDWrite(&oled, rchLine[i]);
  NXCTRLOLEDUpdateDisplay(&oled);
}

int
main (void) {
return NXCTRLMain();
}
