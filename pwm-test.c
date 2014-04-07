/*
 * NXCTRL BeagleBone Black Control Library
 *
 * PWM Generation Test Program
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
#include <signal.h>
#include <NXCTRL.h>

#define PWM_BANK NXCTRL_P9
#define PWM_PIN  NXCTRL_PIN14 // connect to LED for pulsing
#define PWM_PMX  NXCTRL_MODE6

#define PWM_RES   (1000)
#define PULSE_CNT (10)
#define PULSE_RES (10)
#define PULSE_TM  (80)

void
NXCTRLSetup (void) {
  int i, j;
  int nDelta = PWM_RES / PULSE_RES;
  NXCTRLPinMux(PWM_BANK, PWM_PIN, PWM_PMX, NXCTRL_PULLDN, NXCTRL_LOW);

  // 200 means 20% as 1000 is total
  for (j = 0; j < PULSE_CNT; j++) {
    for (i = 0; i < PULSE_RES; i++) {
      NXCTRLAnalogWrite(PWM_BANK, PWM_PIN, nDelta*(i+1));
      NXCTRLSleep(PULSE_TM, 0);
    }
    for (i = 0; i < PULSE_RES; i++) {
      NXCTRLAnalogWrite(PWM_BANK, PWM_PIN, PWM_RES - nDelta*(i+1));
      NXCTRLSleep(PULSE_TM, 0);
    }
  }
  NXCTRLAnalogWrite(PWM_BANK, PWM_PIN, 0);
  NXCTRLSleep(100, 0);
}

void
NXCTRLLoop (void) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
