/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Default Control App Program
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
#include <NXCTRL_appEx.h>

#define TMP36_PIN                   NXCTRL_A1

#define TMP36_DELTA                 0.00
#define CPUTEMP_BASE                56.0
#define CPUTEMP_SSG                 60.0
#define SSG_DELTA                   0.00
#define TMP_MAX_DELTA               9.00

#define TEMP_UPDATE_COUNT           20

#define LOGO_WIDTH                  128
#define LOGO_HEIGHT                 64

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8
#define MENU_SEL_CHAR               ((unsigned char)16)

#define DPY_IDLE_COUNT_MAX          300
#define MIN_ACTION_DURATION         200

#define MENU_IDX_COUNT              3

#define MENU_IDX_SYSTEM_MENU        0
#define MENU_IDX_UPDATE_TIME        1
#define MENU_IDX_EXIT_MENU          2

static NXCTRL_BOOL                  MENU_BUTTON_STATE = NXCTRL_LOW;
static NXCTRL_BOOL                  EXEC_BUTTON_STATE = NXCTRL_LOW;
static unsigned int                 DPY_IDLE_COUNT = 0;
static unsigned char                MENU_IDX = MENU_IDX_SYSTEM_MENU;
static NXCTRL_BOOL                  IN_MENU = NXCTRL_FALSE;
static unsigned long long           LAST_ACTION_TIME = 0;
static unsigned int                 DPY_UPDATE_TIME = 10000;
static float                        CPUTEMP = 0;

static unsigned int
getCPUTemp (NXCTRL_VOID) {
  const char *psz = "/sys/class/hwmon/hwmon0/device/temp1_input";
  //const char *psz = "/sys/devices/ocp.3/44e10448.bandgap/temp1_input";
  int nFD = open(psz, O_RDONLY);
  if (nFD < 0) {
    system("rmmod am335x_bandgap");
    system("modprobe am335x_bandgap");
    return 0;
  } else {
    char rch[8];
    int n = read(nFD, rch, 7);
    rch[n] = 0;
    n = atoi(rch);
    close(nFD);
    if (n > 120000) {
      system("rmmod am335x_bandgap");
      system("modprobe am335x_bandgap");
      return 0;
    }
    return n;
  }
  return 0;
}

static float
getTMP36Value (LPNXCTRLAPP pApp) {
  return pApp->analogRead(TMP36_PIN)*1.8/4096.0;
}

static float
getTemperature (LPNXCTRLAPP pApp) {
  static int nUpdate = -1;
  static float fTemp = 0.0;

  if (nUpdate < 0 || nUpdate > TEMP_UPDATE_COUNT) {
    float fTmp = (getTMP36Value(pApp) + TMP36_DELTA - 0.5) * 100;
    float fCPUTemp = getCPUTemp()/1000.0;
    
    fCPUTemp = (fCPUTemp > 120) ? (fCPUTemp - 74) : fCPUTemp;
    if (fCPUTemp < CPUTEMP_SSG)
      fTmp -= (fCPUTemp > CPUTEMP_BASE) ? (fCPUTemp - CPUTEMP_BASE) : 0;
    else {
      float fD = (fCPUTemp > CPUTEMP_BASE) ?
        (fCPUTemp - CPUTEMP_BASE + SSG_DELTA) : 0;
      if (fD > TMP_MAX_DELTA) fD = TMP_MAX_DELTA;
      fTmp -= fD;
    }

    CPUTEMP = fCPUTemp;
    if (fTmp < -30 || fTmp > 50) fTmp = 0;

    fTemp = fTmp;
    nUpdate = 0;
  } else
    nUpdate++;

  return fTemp;
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
displayInfo (LPNXCTRLAPP pApp) {
  if (DPY_UPDATE_TIME < 9)
    return;
  else {
    register int i;
    char rch[22];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    float fTmp = getTemperature(pApp);
    int nWidth = 0;
    sprintf(rch,
            " %d/%s%d/%s%d %s%d:%s%d:%s%d",
            (tm.tm_year + 1900),
            (tm.tm_mon + 1) > 9 ? "" : "0", tm.tm_mon + 1,
            tm.tm_mday > 9 ? "" : "0", tm.tm_mday,
            tm.tm_hour > 9 ? "" : "0", tm.tm_hour,
            tm.tm_min > 9 ? "" : "0", tm.tm_min,
            tm.tm_sec > 9 ? "" : "0", tm.tm_sec);
    for (i = strlen(rch); i < 21; i++)
      rch[i] = ' ';
    rch[21] = 0;
    pApp->clearDisplay();
    pApp->setCursor(0, FONT_HEIGHT*2);
    pApp->writeSTR(rch);
    pApp->setCursor(3*FONT_WIDTH+0, FONT_HEIGHT*4);
    sprintf(rch, "TEMP: %2.0fC / %2.0fC", fTmp, CPUTEMP);
    pApp->writeSTR(rch);

    pApp->drawLine(2*FONT_WIDTH, FONT_HEIGHT*5+5, 127-2*FONT_WIDTH, FONT_HEIGHT*5+5, NXCTRL_ON);
    pApp->drawLine(2*FONT_WIDTH, FONT_HEIGHT*5+5, 2*FONT_WIDTH, FONT_HEIGHT*5+5+4, NXCTRL_ON);
    pApp->drawLine(127-2*FONT_WIDTH, FONT_HEIGHT*5+5, 127-2*FONT_WIDTH, FONT_HEIGHT*5+5+4, NXCTRL_ON);
    pApp->drawLine(2*FONT_WIDTH, FONT_HEIGHT*5+5+4, 127-2*FONT_WIDTH, FONT_HEIGHT*5+5+4, NXCTRL_ON);
    nWidth = (int)((0.0+20.0) / (50.0+20.0) * (127 - 4*FONT_WIDTH));
    pApp->drawPixel(2*FONT_WIDTH+nWidth, FONT_HEIGHT*5+6, NXCTRL_ON);
    nWidth = (int)((20.0+20.0) / (50.0+20.0) * (127 - 4*FONT_WIDTH));
    pApp->drawPixel(2*FONT_WIDTH+nWidth, FONT_HEIGHT*5+6, NXCTRL_ON);
    nWidth = (int)((40.0+20.0) / (50.0+20.0) * (127 - 4*FONT_WIDTH));
    pApp->drawPixel(2*FONT_WIDTH+nWidth, FONT_HEIGHT*5+6, NXCTRL_ON);
    nWidth = (int)((fTmp+20.0) / (50.0+20.0) * (127 - 4*FONT_WIDTH));
    pApp->drawLine(2*FONT_WIDTH, FONT_HEIGHT*5+7, 2*FONT_WIDTH+nWidth, FONT_HEIGHT*5+7, NXCTRL_ON);
    pApp->drawLine(2*FONT_WIDTH, FONT_HEIGHT*5+8, 2*FONT_WIDTH+nWidth, FONT_HEIGHT*5+8, NXCTRL_ON);

    pApp->updateDisplay();
  }
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
  if (nMenu == MENU_IDX_SYSTEM_MENU)
    rch[19] = MENU_SEL_CHAR;
  return rch;
}

static NXCTRL_VOID
displayMenu (LPNXCTRLAPP pApp) {
  char rch[21];

  pApp->clearDisplay();

  pApp->setCursor(0, 0);
  pApp->writeSTR("MAIN");
  pApp->drawLine(25, 6, 127, 6, NXCTRL_ON);
  pApp->setCursor(0, 16);

  pApp->writeSTR(mkMenuSTR(rch, "TC>>", MENU_IDX_SYSTEM_MENU));
  pApp->writeSTR(mkMenuSTR(rch, "UPDATE TIME", MENU_IDX_UPDATE_TIME));
  pApp->writeSTR(mkMenuSTR(rch, "EXIT MENU", MENU_IDX_EXIT_MENU));

  pApp->updateDisplay();
}

NXCTRL_VOID
NXCTRLAPP_init (LPNXCTRLAPP pApp) {
  MENU_BUTTON_STATE = pApp->digitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN);
  EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  DPY_IDLE_COUNT = 0;
  MENU_IDX = MENU_IDX_SYSTEM_MENU;
  IN_MENU = NXCTRL_FALSE;
  LAST_ACTION_TIME = 0;

  while (MENU_BUTTON_STATE == NXCTRL_HIGH) {
    pApp->sleep(100, 0);
    MENU_BUTTON_STATE = pApp->digitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN);
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

  if (!IN_MENU)
    displayInfo(pApp);

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
          MENU_IDX = MENU_IDX_SYSTEM_MENU;
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
          displayInfo(pApp);
          break;
        case MENU_IDX_SYSTEM_MENU:
          pApp->nCmd = 1;
          return;
        case MENU_IDX_UPDATE_TIME:
          pApp->clearDisplay();
          pApp->setCursor(FONT_WIDTH*5, FONT_HEIGHT*3);
          pApp->writeSTR("UPDATING...");
          pApp->updateDisplay();
          system("/usr/bin/ntpd -gq");
          IN_MENU = NXCTRL_FALSE;
          displayInfo(pApp);
          break;
        default:
          break;
        }
      }
    }
  }
}
