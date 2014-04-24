/*
 * NXCTRL BeagleBone Black Control Library
 *
 * 16x2 Character LCD Test Program
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
#include <NXCTRL.h>
#include <NXCTRL_lcd.h>

#define RSBANK   NXCTRL_P8
#define RSPIN    NXCTRL_PIN11
#define ENBANK   NXCTRL_P8
#define ENPIN    NXCTRL_PIN12
#define D0BANK   NXCTRL_P9
#define D0PIN    NXCTRL_PIN12
#define D1BANK   NXCTRL_P9
#define D1PIN    NXCTRL_PIN15
#define D2BANK   NXCTRL_P9
#define D2PIN    NXCTRL_PIN23
#define D3BANK   NXCTRL_P9
#define D3PIN    NXCTRL_PIN25

NXCTRLLCD LCD;

static void
__WAIT (void) {
  printf("Press ENTER to continue: ");
  (void)fgetc(stdin);
}

void
NXCTRLSetup (void) {
  int nRet;

  NXCTRLPinMux(RSBANK, RSPIN, NXCTRL_MODE7, NXCTRL_NOPULL, NXCTRL_OFF);
  NXCTRLPinMux(ENBANK, ENPIN, NXCTRL_MODE7, NXCTRL_NOPULL, NXCTRL_OFF);
  NXCTRLPinMux(D0BANK, D0PIN, NXCTRL_MODE7, NXCTRL_NOPULL, NXCTRL_OFF);
  NXCTRLPinMux(D1BANK, D1PIN, NXCTRL_MODE7, NXCTRL_NOPULL, NXCTRL_OFF);
  NXCTRLPinMux(D2BANK, D2PIN, NXCTRL_MODE7, NXCTRL_NOPULL, NXCTRL_OFF);
  NXCTRLPinMux(D3BANK, D3PIN, NXCTRL_MODE7, NXCTRL_NOPULL, NXCTRL_OFF);

  nRet = NXCTRLLCDInit(&LCD,
                       RSBANK, RSPIN,
                       -1, -1,
                       ENBANK, ENPIN,
                       D0BANK, D0PIN,
                       D1BANK, D1PIN,
                       D2BANK, D2PIN,
                       D3BANK, D3PIN,
                       -1, -1,
                       -1, -1,
                       -1, -1,
                       -1, -1);
  if (nRet < 0) {
    fprintf(stderr, "NXCTRLLCDInit: initialization failed\n");
    return;
  }

  NXCTRLLCDBegin(&LCD, 16, 2);

  NXCTRLLCDSetCursor(&LCD, 0, 0);
  NXCTRLLCDPutStr(&LCD, "LCD TEST");

  __WAIT();
}

void
NXCTRLLoop (void) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
