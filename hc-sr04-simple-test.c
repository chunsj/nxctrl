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
#define SENSOR_BANK NXCTRL_P9

#define TRIGGER_PIN NXCTRL_PIN17
#define ECHO_PIN    NXCTRL_PIN18

void
NXCTRLSetup (void) {
  int nMaxCount = 5000000;
  int i;

  NXCTRLPinMode(SENSOR_BANK, TRIGGER_PIN, NXCTRL_OUTPUT);
  NXCTRLPinMode(SENSOR_BANK, ECHO_PIN, NXCTRL_INPUT_PULLDN);

  NXCTRLDigitalWrite(SENSOR_BANK, TRIGGER_PIN, NXCTRL_LOW);
  NXCTRLSleep(0, 2);
  NXCTRLDigitalWrite(SENSOR_BANK, TRIGGER_PIN, NXCTRL_HIGH);
  NXCTRLSleep(0, 10);
  NXCTRLDigitalWrite(SENSOR_BANK, TRIGGER_PIN, NXCTRL_LOW);

  i = 0;
  while (NXCTRLDigitalRead(SENSOR_BANK, ECHO_PIN) == NXCTRL_HIGH) {
    NXCTRLSleep(0, 1);
    if (i++ >= nMaxCount) {
      fprintf(stderr, "waited too long for echo low\n");
      return;
    }
  }

  i = 0;
  while (NXCTRLDigitalRead(SENSOR_BANK, ECHO_PIN) != NXCTRL_HIGH) {
    NXCTRLSleep(0, 1);
    if (i++ >= nMaxCount) {
      fprintf(stderr, "waited too long for echo response\n");
      return;
    }
  }

  printf("%d %.2f\n", i, i/2.0/29.1);
}

void
NXCTRLLoop (void) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
