/*
 * NXCTRL BeagleBone Black Control Library
 *
 * TSC ADC Test Program
 *
 * Copyright (C) 2014 Sungjin Chun <chunsj@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#define ANALOG_IN1  NXCTRL_A0
#define ANALOG_IN2  NXCTRL_A4

static int nCount = 0;

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  while (nCount < 40) {
    printf("%03d: %04d %04d\n", 1 + nCount++,
           NXCTRLAnalogRead(ANALOG_IN1),
           NXCTRLAnalogRead(ANALOG_IN2));
    NXCTRLSleep(500, 0);
  }
}

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
