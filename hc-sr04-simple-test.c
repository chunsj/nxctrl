/*
 * NXCTRL BeagleBone Black Control Library
 *
 * NXCTRL API HC SR-04 Test Program
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

#define TRIGGER_PIN NXCTRL_PIN11
#define ECHO_PIN    NXCTRL_PIN17

void
NXCTRLSetup (void) {
  NXCTRLPinMode(NXCTRL_P8, TRIGGER_PIN, NXCTRL_OUTPUT);
  NXCTRLPinMode(NXCTRL_P8, ECHO_PIN, NXCTRL_INPUT_PULLDN);

  NXCTRLDigitalWrite(NXCTRL_P8, TRIGGER_PIN, NXCTRL_ON);
  NXCTRLSleep(0, 10);

  while (NXCTRLDigitalRead(NXCTRL_P8, ECHO_PIN) != NXCTRL_ON);
}

void
NXCTRLLoop (void) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
