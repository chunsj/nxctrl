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

NXLCDHW LCD;

static void
__WAIT (void) {
  printf("Press ENTER to continue: ");
  (void)fgetc(stdin);
}

void
NXCTRLSetup (void) {
  int nRet = NXCTRLLCDInit(&LCD,
                           2, 16, 4,
                           NXCTRL_P8, NXCTRL_P8,
                           NXCTRL_PIN11, NXCTRL_PIN12,
                           NXCTRL_P9, NXCTRL_P9, NXCTRL_P9, NXCTRL_P9,
                           -1, -1, -1, -1,
                           NXCTRL_PIN23, NXCTRL_PIN25, NXCTRL_PIN27, NXCTRL_PIN29,
                           -1, -1, -1, -1);
  if (nRet < 0) {
    fprintf(stderr, "NXCTRLLCDInit: initialization failed\n");
    return;
  }

  NXCTRLLCDPosition(&LCD, 0, 0);
  NXCTRLLCDStrPut(&LCD, "LCD TEST");

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
