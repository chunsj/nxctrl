/*
 * NXCTRL BeagleBone Black Control Library
 *
 * System Control Utilities App Program
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

#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/reboot.h>
#include <NXCTRL_appEx.h>

#define LOGO_WIDTH                  128
#define LOGO_HEIGHT                 64

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8
#define MENU_SEL_CHAR               ((unsigned char)16)

#define DPY_IDLE_COUNT_MAX          300
#define MIN_ACTION_DURATION         200

#define MENU_IDX_COUNT              8

#define MENU_IDX_GO_MAIN            0
#define MENU_IDX_GO_CONNINFO        1
#define MENU_IDX_GO_SYSINFO         2
#define MENU_IDX_GO_PERI            3
#define MENU_IDX_GO_SPARK           4
#define MENU_IDX_MENU_OFF           5
#define MENU_IDX_TURN_OFF           6
#define MENU_IDX_EXIT_MENU          7

static NXCTRL_BOOL                  MENU_BUTTON_STATE = NXCTRL_LOW;
static NXCTRL_BOOL                  EXEC_BUTTON_STATE = NXCTRL_LOW;
static unsigned char                DPY_IDLE_COUNT = 0;
static unsigned char                MENU_IDX = MENU_IDX_GO_MAIN;
static NXCTRL_BOOL                  IN_MENU = NXCTRL_FALSE;
static unsigned long long           LAST_ACTION_TIME = 0;

static NXCTRL_BOOL
canAction (NXCTRL_VOID) {
  struct timespec tm;
  unsigned long long timeInMillis;
  extern int clock_gettime(int, struct timespec *);
  clock_gettime(_POSIX_CPUTIME, &tm);
  timeInMillis = tm.tv_sec * 1000 + tm.tv_nsec/1000000;
  if ((timeInMillis - LAST_ACTION_TIME) > MIN_ACTION_DURATION) {
    LAST_ACTION_TIME = timeInMillis;
    return NXCTRL_TRUE;
  } else
    return NXCTRL_FALSE;
}

static NXCTRL_VOID
updateMenuButtonState (LPNXCTRLAPP pApp) {
  if (MENU_BUTTON_STATE == NXCTRL_LOW) {
    if (pApp->digitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN) == NXCTRL_HIGH) {
      MENU_BUTTON_STATE = NXCTRL_HIGH;
      DPY_IDLE_COUNT = 0;
    }
  } else {
    if (pApp->digitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN) == NXCTRL_LOW) {
      MENU_BUTTON_STATE = NXCTRL_LOW;
      DPY_IDLE_COUNT = 0;
    }
  }
}

static NXCTRL_VOID
updateExecButtonState (LPNXCTRLAPP pApp) {
  if (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    if (pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN) == NXCTRL_HIGH) {
      EXEC_BUTTON_STATE = NXCTRL_HIGH;
      DPY_IDLE_COUNT = 0;
    }
  } else {
    if (pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN) == NXCTRL_LOW) {
      EXEC_BUTTON_STATE = NXCTRL_LOW;
      DPY_IDLE_COUNT = 0;
    }
  }
}

static char *
mkMenuSTR (char *rch, const char *pszName, int nMenu) {
  sprintf(rch, "%c  %s\n",
          (MENU_IDX == nMenu ? MENU_SEL_CHAR : ' '),
          pszName);
  return rch;
}

static char *
mkAppMenuSTR (char *rch, const char *pszName, int nMenu) {
  sprintf(rch, "%c %c%s\n",
          (MENU_IDX == nMenu ? MENU_SEL_CHAR : ' '),
          MENU_SEL_CHAR-9,
          pszName);
  return rch;
}

static NXCTRL_VOID
displayMenu (LPNXCTRLAPP pApp) {
  char rch[21];

  pApp->clearDisplay();

  pApp->setCursor(0, 0);
  pApp->writeSTR("SYSTEM");
  pApp->drawLine(37, 6, 127, 6, NXCTRL_ON);
  pApp->setCursor(0, 16);

  if (MENU_IDX < 5)
    pApp->writeSTR(mkAppMenuSTR(rch, "MAIN", MENU_IDX_GO_MAIN));
  if (MENU_IDX < 6)
    pApp->writeSTR(mkAppMenuSTR(rch, "CONN.INFO", MENU_IDX_GO_CONNINFO));
  if (MENU_IDX < 7)
    pApp->writeSTR(mkAppMenuSTR(rch, "SYS.INFO", MENU_IDX_GO_SYSINFO));
  pApp->writeSTR(mkAppMenuSTR(rch, "PERI.DRV", MENU_IDX_GO_PERI));
  pApp->writeSTR(mkAppMenuSTR(rch, "SPARK CORE", MENU_IDX_GO_SPARK));
  if (MENU_IDX >= 5)
    pApp->writeSTR(mkMenuSTR(rch, "SCREEN OFF", MENU_IDX_MENU_OFF));
  if (MENU_IDX >= 6)
    pApp->writeSTR(mkMenuSTR(rch, "POWER OFF", MENU_IDX_TURN_OFF));
  if (MENU_IDX >= 7)
    pApp->writeSTR(mkMenuSTR(rch, "EXIT MENU", MENU_IDX_EXIT_MENU));

  pApp->updateDisplay();
}

NXCTRL_VOID
NXCTRLAPP_init (LPNXCTRLAPP pApp) {
  MENU_BUTTON_STATE = pApp->digitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN);
  EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  DPY_IDLE_COUNT = 0;
  MENU_IDX = MENU_IDX_GO_MAIN;
  IN_MENU = NXCTRL_FALSE;
  LAST_ACTION_TIME = 0;
  IN_MENU = NXCTRL_TRUE;

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
  updateMenuButtonState(pApp);
  updateExecButtonState(pApp);

  displayMenu(pApp);
  
  if (MENU_BUTTON_STATE == NXCTRL_ON) {
    if (IN_MENU) {
      if (canAction()) {
        MENU_IDX++;
        if (MENU_IDX >= MENU_IDX_COUNT)
          MENU_IDX = MENU_IDX_GO_MAIN;
        displayMenu(pApp);
      }
    } else {
      IN_MENU = NXCTRL_TRUE;
      displayMenu(pApp);
      canAction();
    }
  }

  if (EXEC_BUTTON_STATE == NXCTRL_ON) {
    if (IN_MENU) {
      if (canAction()) {
        switch (MENU_IDX) {
        case MENU_IDX_GO_MAIN:
          pApp->nCmd = 1234;
          return;
        case MENU_IDX_GO_CONNINFO:
          pApp->nCmd = 1234+3;
          return;
        case MENU_IDX_GO_SYSINFO:
          pApp->nCmd = 1234+4;
          return;
        case MENU_IDX_GO_PERI:
          pApp->nCmd = 1234+5;
          return;
        case MENU_IDX_GO_SPARK:
          pApp->nCmd = 1234+6;
          return;
        case MENU_IDX_EXIT_MENU:
          pApp->nCmd = 1;
          return;
        case MENU_IDX_TURN_OFF:
          pApp->clearDisplay();
          pApp->setCursor(3*FONT_WIDTH, 3*FONT_HEIGHT);
          pApp->writeSTR("TURNING OFF...");
          pApp->updateDisplay();
          sync();
          sync();
          sync();
          pApp->sleep(500, 0);
          //pApp->clearDisplay();
          //pApp->updateDisplay();
          sync();
          sync();
          sync();
          //reboot(RB_POWER_OFF);
          system("poweroff");
          break;
        case MENU_IDX_MENU_OFF:
          pApp->nCmd = 2;
          return;
        default:
          break;
        }
      }
    }
  }
}
