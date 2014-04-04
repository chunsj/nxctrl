/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Basic GPIO Test Program
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

#define BUTTON_PIN NXCTRL_PIN11
#define LED_PIN    NXCTRL_PIN17

#define DEBOUNCING_CLOCK_COUNT 200

#define LONG_DELAY  80
#define SHORT_DELAY 20

void
NXCTRLSetup (void) {
  NXCTRLPinMode(NXCTRL_P8, BUTTON_PIN, NXCTRL_INPUT_PULLDN);
  //NXCTRLPinDebound(NXCTRL_P8, BUTTON_PIN, DEBOUNCING_CLOCK_COUNT);
  NXCTRLPinMode(NXCTRL_P8, LED_PIN, NXCTRL_OUTPUT);
}

void
NXCTRLLoop (void) {
  static int nPrevDelay = LONG_DELAY;
  int nDelay = LONG_DELAY;

  if (NXCTRLDigitalRead(NXCTRL_P8, BUTTON_PIN) == NXCTRL_HIGH)
    nDelay = SHORT_DELAY;
  if (NXCTRLDigitalRead(NXCTRL_P8, BUTTON_PIN) == NXCTRL_LOW)
    nDelay = LONG_DELAY;

  if (nPrevDelay != nDelay) {
    nPrevDelay = nDelay;
    printf("%d - %d\n", NXCTRLDigitalRead(NXCTRL_P8, BUTTON_PIN), nDelay);
  }

  NXCTRLDigitalWrite(NXCTRL_P8, LED_PIN, NXCTRL_HIGH);
  NXCTRLSleep(nDelay, 0);
  NXCTRLDigitalWrite(NXCTRL_P8, LED_PIN, NXCTRL_LOW);
  NXCTRLSleep(nDelay, 0);
}

int
main (void) {
  return NXCTRLMain();
}
