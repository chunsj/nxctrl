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
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <NXCTRL.h>

#define SENSOR_BANK NXCTRL_P8

#define TRIGGER_PIN NXCTRL_PIN45
#define ECHO_PIN    NXCTRL_PIN46

void
NXCTRLSetup (void) {
  int nMaxCount = 5000000;
  int i;
  struct timeval tvStart;
  struct timeval tvEnd;
  unsigned long nStartTime, nEndTime;

  memset(&tvStart, 0, sizeof(tvStart));
  memset(&tvEnd, 0, sizeof(tvEnd));

  NXCTRLPinMux(SENSOR_BANK, TRIGGER_PIN, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(SENSOR_BANK, ECHO_PIN, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_HIGH);

  NXCTRLPinMode(SENSOR_BANK, TRIGGER_PIN, NXCTRL_OUTPUT);
  NXCTRLPinMode(SENSOR_BANK, ECHO_PIN, NXCTRL_INPUT_PULLDN);

  NXCTRLDigitalWrite(SENSOR_BANK, TRIGGER_PIN, NXCTRL_LOW);
  NXCTRLSleep(0, 2);
  NXCTRLDigitalWrite(SENSOR_BANK, TRIGGER_PIN, NXCTRL_HIGH);
  NXCTRLSleep(0, 10);
  NXCTRLDigitalWrite(SENSOR_BANK, TRIGGER_PIN, NXCTRL_LOW);

  i = 0;
  gettimeofday(&tvStart, NULL);
  while (NXCTRLDigitalRead(SENSOR_BANK, ECHO_PIN) != NXCTRL_HIGH) {
    gettimeofday(&tvStart, NULL);
    if (i++ >= nMaxCount) {
      fprintf(stderr, "waited too long for echo start response\n");
      return;
    }
  }

  i = 0;
  while (NXCTRLDigitalRead(SENSOR_BANK, ECHO_PIN) == NXCTRL_HIGH) {
    gettimeofday(&tvEnd, NULL);
    if (i++ >= nMaxCount) {
      fprintf(stderr, "waited too long for echo end response\n");
      return;
    }
  }

  nStartTime = 1000000 * tvStart.tv_sec + tvStart.tv_usec;
  nEndTime = 1000000 * tvEnd.tv_sec + tvEnd.tv_usec;

  printf("TIME: %lu\n", (nEndTime - nStartTime));
  printf("DIST: %.0f\n", (nEndTime - nStartTime)/2.0/29.1);
}

void
NXCTRLLoop (void) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
