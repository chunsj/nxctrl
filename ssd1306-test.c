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

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int i;
  char ch;
  uint8_t nLSB;
  uint32_t nSpeed, nSPIMode;

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

  for (ch = 0; ch < 168; ch++) {
    if (ch == '\n') continue;
    NXCTRLOLEDWrite(&oled, ch);
  }
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(2000, 0);

  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  NXCTRLOLEDSetCursor(&oled, 0, 0);
  NXCTRLOLEDWrite(&oled, '1');
  NXCTRLOLEDWrite(&oled, '2');
  NXCTRLOLEDWrite(&oled, '3');
  NXCTRLOLEDWrite(&oled, '.');
  NXCTRLOLEDWrite(&oled, '4');
  NXCTRLOLEDWrite(&oled, '5');
  NXCTRLOLEDWrite(&oled, '6');
  NXCTRLOLEDUpdateDisplay(&oled);
  NXCTRLSleep(2000, 0);

  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  fprintf(stdout, "done\n"); fflush(stdout);
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

  if (strcasecmp(rchLine, "quit") == 0) {
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
