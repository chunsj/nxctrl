/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Servo Control Test Program - not working yet
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

#define SERVO_BANK NXCTRL_P9
#define SERVO_PIN  NXCTRL_PIN14
#define SERVO_PMX  NXCTRL_MODE6

void
NXCTRLSetup (void) {
  int i;
  NXCTRLPinMux(SERVO_BANK, SERVO_PIN, SERVO_PMX, NXCTRL_PULLDN, NXCTRL_LOW);
  
  NXCTRLServoWrite(SERVO_BANK, SERVO_PIN, 0);
  NXCTRLSleep(500, 0);

  for (i = 0; i <= 180; i += 2) {
    NXCTRLServoWrite(SERVO_BANK, SERVO_PIN, i);
    NXCTRLSleep(20, 0);
  }

  NXCTRLServoWrite(SERVO_BANK, SERVO_PIN, 0);
  NXCTRLSleep(800, 0);
  NXCTRLServoWrite(SERVO_BANK, SERVO_PIN, 180);
  NXCTRLSleep(800, 0);
  NXCTRLServoWrite(SERVO_BANK, SERVO_PIN, 82);
  NXCTRLSleep(800, 0);
}

void
NXCTRLLoop (void) {
  NXCTRLAnalogWrite(SERVO_BANK, SERVO_PIN, 0);
  NXCTRLSleep(500, 0);
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
