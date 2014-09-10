/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Blank Screen App Program
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <NXCTRL_appEx.h>

static NXCTRL_BOOL                  EXEC_BUTTON_STATE = NXCTRL_LOW;

static NXCTRL_VOID
updateExecButtonState (LPNXCTRLAPP pApp) {
  if (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    if (pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN) == NXCTRL_HIGH) {
      EXEC_BUTTON_STATE = NXCTRL_HIGH;
    }
  } else {
    if (pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN) == NXCTRL_LOW) {
      EXEC_BUTTON_STATE = NXCTRL_LOW;
    }
  }
}

NXCTRL_VOID
NXCTRLAPP_init (LPNXCTRLAPP pApp) {
  EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);

  while (EXEC_BUTTON_STATE == NXCTRL_HIGH) {
    pApp->sleep(100, 0);
    EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  }
  pApp->clearDisplay();
  pApp->updateDisplay();
}

NXCTRL_VOID
NXCTRLAPP_clean (LPNXCTRLAPP pApp) {
}

NXCTRL_VOID
NXCTRLAPP_run (LPNXCTRLAPP pApp) {
  updateExecButtonState(pApp);

  if (EXEC_BUTTON_STATE == NXCTRL_ON) {
    pApp->nCmd = 0;
    return;
  }
}
