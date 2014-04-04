/*
 * NXCTRL BeagleBone Black Control Library
 *
 * PWM Generation Test Program
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
#include <signal.h>
#include <NXCTRL.h>

#define PWM_BANK NXCTRL_P9
#define PWM_PIN  NXCTRL_PIN14
#define PWM_PMX  NXCTRL_MODE6

void
NXCTRLSetup (void) {
  NXCTRLPinMux(PWM_BANK, PWM_PIN, PWM_PMX, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLAnalogWrite(PWM_BANK, PWM_PIN, 128);
  NXCTRLSleep(5000, 0);
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
