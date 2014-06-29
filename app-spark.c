/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Spark Core Management App Program
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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <NXCTRL_appEx.h>

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8
#define MENU_SEL_CHAR               ((unsigned char)16)

#define DPY_IDLE_COUNT_MAX          300
#define MIN_ACTION_DURATION         200

#define MENU_IDX_COUNT              5

#define MENU_IDX_NEXT_APP           0
#define MENU_IDX_SYSTEM_MENU        1
#define MENU_IDX_FLASH_MENU         2
#define MENU_IDX_D7_ONOFF_MENU      3
#define MENU_IDX_EXIT_MENU          4

#define NEXT_APP_IDX                0 // from tc.c

static NXCTRL_BOOL                  MENU_BUTTON_STATE = NXCTRL_LOW;
static NXCTRL_BOOL                  EXEC_BUTTON_STATE = NXCTRL_LOW;
static unsigned int                 DPY_IDLE_COUNT = 0;
static unsigned char                MENU_IDX = MENU_IDX_SYSTEM_MENU;
static NXCTRL_BOOL                  IN_MENU = NXCTRL_FALSE;
static unsigned long long           LAST_ACTION_TIME = 0;
static NXCTRL_BOOL                  TOGGLE = NXCTRL_ON;

static NXCTRL_VOID
displayCoreInfo (LPNXCTRLAPP pApp) {
  char rchName[22], rchStatus[16], rchLine[1024], rchDummy[80];
  FILE *pFile;

  pApp->clearDisplay();
  pApp->setCursor(4*FONT_WIDTH, 0);
  pApp->writeSTR("SPARK CORE\n");

  pApp->setCursor(0, FONT_HEIGHT*3);
  pApp->writeSTR("    SEARCHING...");
  pApp->updateDisplay();

  pApp->setCursor(0, FONT_HEIGHT + 8);
  system("/usr/bin/spark-list > /tmp/sparkcoreinfo.txt");

  rchLine[0] = 0;
  pFile = fopen("/tmp/sparkcoreinfo.txt", "r");
  if (pFile) {
    fgets(rchLine, 1023, pFile);
    fclose(pFile);
    //fprintf(stderr, "%s\n", rchLine);
  }

  pApp->clearDisplay();
  pApp->setCursor(4*FONT_WIDTH, 0);
  pApp->writeSTR("SPARK CORE\n");

  if (strlen(rchLine) != 0) {
    sscanf(rchLine, "%s %s %s %s", rchName, rchDummy, rchDummy, rchStatus);
    sprintf(rchDummy, "%s: %s", rchName, rchStatus);
    pApp->setCursor(3*FONT_WIDTH, FONT_HEIGHT*3);
    pApp->writeSTR(rchDummy);
  } else {
    pApp->setCursor(0, FONT_HEIGHT*3);
    pApp->writeSTR("      NOT FOUND");
  }

  pApp->updateDisplay();
}

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
  sprintf(rch, "%c %s\n",
          (MENU_IDX == nMenu ? MENU_SEL_CHAR : ' '),
          pszName);
  return rch;
}

static NXCTRL_VOID
displayMenu (LPNXCTRLAPP pApp) {
  char rch[21];

  pApp->clearDisplay();

  pApp->setCursor(0, 0);
  pApp->writeSTR("SPARK CORE");
  pApp->drawLine(61, 6, 127, 6, NXCTRL_ON);
  pApp->setCursor(0, 16);

  pApp->writeSTR(mkMenuSTR(rch, "MAIN APP", MENU_IDX_NEXT_APP));
  pApp->writeSTR(mkMenuSTR(rch, "SYSTEM UTILS", MENU_IDX_SYSTEM_MENU));
  pApp->writeSTR(mkMenuSTR(rch, "FLASH TINKER", MENU_IDX_FLASH_MENU));
  pApp->writeSTR(mkMenuSTR(rch, "TOGGLE D7", MENU_IDX_D7_ONOFF_MENU));
  pApp->writeSTR(mkMenuSTR(rch, "EXIT MENU", MENU_IDX_EXIT_MENU));

  pApp->updateDisplay();
}

NXCTRL_VOID
NXCTRLAPP_init (LPNXCTRLAPP pApp) {
  MENU_BUTTON_STATE = pApp->digitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN);
  EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  DPY_IDLE_COUNT = 0;
  MENU_IDX = MENU_IDX_NEXT_APP;
  IN_MENU = NXCTRL_FALSE;
  LAST_ACTION_TIME = 0;

  while (MENU_BUTTON_STATE == NXCTRL_HIGH) {
    pApp->sleep(100, 0);
    MENU_BUTTON_STATE = pApp->digitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN);
  }

  displayCoreInfo(pApp);
}

NXCTRL_VOID
NXCTRLAPP_clean (LPNXCTRLAPP pApp) {
}

NXCTRL_VOID
NXCTRLAPP_run (LPNXCTRLAPP pApp) {
  updateMenuButtonState(pApp);
  updateExecButtonState(pApp);

  if (MENU_BUTTON_STATE != NXCTRL_HIGH && EXEC_BUTTON_STATE != NXCTRL_HIGH) {
    DPY_IDLE_COUNT++;
    if (DPY_IDLE_COUNT > DPY_IDLE_COUNT_MAX) {
      pApp->nCmd = 2;
      return;
    }
    return;
  }

  if (MENU_BUTTON_STATE == NXCTRL_ON) {
    if (IN_MENU) {
      if (canAction()) {
        MENU_IDX++;
        if (MENU_IDX >= MENU_IDX_COUNT)
          MENU_IDX = MENU_IDX_NEXT_APP;
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
        case MENU_IDX_EXIT_MENU:
          IN_MENU = NXCTRL_FALSE;
          displayCoreInfo(pApp);
          break;
        case MENU_IDX_SYSTEM_MENU:
          pApp->nCmd = 1;
          return;
        case MENU_IDX_NEXT_APP:
          pApp->nCmd = NEXT_APP_IDX;
          return;
        case MENU_IDX_FLASH_MENU:
          pApp->clearDisplay();
          pApp->setCursor(0, 3*FONT_HEIGHT);
          pApp->writeSTR("     FLASHING...");
          pApp->updateDisplay();
          system("/usr/bin/spark-flash-tinker");
          IN_MENU = NXCTRL_FALSE;
          displayCoreInfo(pApp);
          break;
        case MENU_IDX_D7_ONOFF_MENU:
          pApp->clearDisplay();
          pApp->setCursor(0, 3*FONT_HEIGHT);
          pApp->writeSTR("    TOGGLE D7...");
          pApp->updateDisplay();
          if (TOGGLE)
            system("/usr/bin/spark-off-d7");
          else
            system("/usr/bin/spark-on-d7");
          TOGGLE = TOGGLE ? NXCTRL_OFF : NXCTRL_ON;
          IN_MENU = NXCTRL_FALSE;
          displayCoreInfo(pApp);
          break;
        default:
          break;
        }
      }
    }
  }
}
