/*
 * NXCTRL BeagleBone Black Control Library
 *
 * System Information App Program
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
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <NXCTRL_appEx.h>

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8
#define MENU_SEL_CHAR               ((unsigned char)16)

#define DPY_IDLE_COUNT_MAX          300
#define MIN_ACTION_DURATION         200

#define MENU_IDX_COUNT              6

#define MENU_IDX_SYSTEM_MENU        0
#define MENU_IDX_UPDATE_MENU        1
#define MENU_IDX_BKHOME_MENU        2
#define MENU_IDX_BKCASTLE_MENU      3
#define MENU_IDX_RUN_MLREPL         4
#define MENU_IDX_EXIT_MENU          5

static NXCTRL_BOOL                  MENU_BUTTON_STATE = NXCTRL_LOW;
static NXCTRL_BOOL                  EXEC_BUTTON_STATE = NXCTRL_LOW;
static unsigned int                 DPY_IDLE_COUNT = 0;
static unsigned char                MENU_IDX = MENU_IDX_SYSTEM_MENU;
static NXCTRL_BOOL                  IN_MENU = NXCTRL_FALSE;
static unsigned long long           LAST_ACTION_TIME = 0;

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

static NXCTRL_VOID
displaySysInfo (LPNXCTRLAPP pApp) {
  struct sysinfo si;
  struct statvfs stvfs;
  int d, h, m;
  int t;
  char rch[22];
  
  statvfs("/", &stvfs);
  sysinfo(&si);
  t = getCPUTemp() / 1000;
  d = si.uptime/(3600*24);
  h = (si.uptime - (d*3600*24))/3600;
  m = (si.uptime - (d*3600*24) - (h*3600))/60;
  pApp->clearDisplay();
  pApp->setCursor(5*FONT_WIDTH, 0);
  pApp->writeSTR("SYSTEM INFO\n");

  pApp->setCursor(0, FONT_HEIGHT + 8);
  
  sprintf(rch, " LDA:  %2.1f %2.1f %2.1f\n",
          si.loads[0]/65536.0,
          si.loads[1]/65536.0,
          si.loads[2]/65536.0);
  pApp->writeSTR(rch);

  sprintf(rch, " RAM:  %03d %03d %03d\n",
          (int)(si.totalram/1024/1024),
          (int)(si.bufferram/1024/1024),
          (int)(si.freeram/1024/1024));
  pApp->writeSTR(rch);

  sprintf(rch, " DSK:  %1.1f %1.1f %1.1f\n",
          stvfs.f_blocks*stvfs.f_frsize/1024/1024/1024.0,
          (stvfs.f_blocks - stvfs.f_bavail)*stvfs.f_frsize/1024/1024/1024.0,
          stvfs.f_bavail*stvfs.f_frsize/1024/1024/1024.0);
  pApp->writeSTR(rch);

  sprintf(rch, " RUN: %3dD %02dH %02dM\n", d, h, m);
  pApp->writeSTR(rch);

  sprintf(rch, " TMP: %3dC\n", t);
  pApp->writeSTR(rch);

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
  pApp->writeSTR("SYS.INFO");
  pApp->drawLine(49, 6, 127, 6, NXCTRL_ON);
  pApp->setCursor(0, 16);

  if (MENU_IDX < 5)
    pApp->writeSTR(mkMenuSTR(rch, "SYSTEM>>", MENU_IDX_SYSTEM_MENU));
  pApp->writeSTR(mkMenuSTR(rch, "UPDATE INFO", MENU_IDX_UPDATE_MENU));
  pApp->writeSTR(mkMenuSTR(rch, "BACKUP (@HOME)", MENU_IDX_BKHOME_MENU));
  pApp->writeSTR(mkMenuSTR(rch, "BACKUP (@CSVR)", MENU_IDX_BKCASTLE_MENU));
  pApp->writeSTR(mkMenuSTR(rch, "START ML REPL", MENU_IDX_RUN_MLREPL));
  if (MENU_IDX >= 5)
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

  displaySysInfo(pApp);
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
          displaySysInfo(pApp);
          break;
        case MENU_IDX_SYSTEM_MENU:
          pApp->nCmd = 1;
          return;
        case MENU_IDX_UPDATE_MENU:
          IN_MENU = NXCTRL_FALSE;
          displaySysInfo(pApp);
          break;
        case MENU_IDX_BKHOME_MENU:
          pApp->clearDisplay();
          pApp->setCursor(3*FONT_WIDTH, 3*FONT_HEIGHT);
          pApp->writeSTR("SYNCING DATA...");
          pApp->updateDisplay();
          system("sudo -u chunsj /home/chunsj/bin/bkhome");
          IN_MENU = NXCTRL_FALSE;
          displaySysInfo(pApp);
          break;
        case MENU_IDX_BKCASTLE_MENU:
          pApp->clearDisplay();
          pApp->setCursor(3*FONT_WIDTH, 3*FONT_HEIGHT);
          pApp->writeSTR("SYNCING DATA...");
          pApp->updateDisplay();
          system("sudo -u chunsj /home/chunsj/bin/bkcastle");
          IN_MENU = NXCTRL_FALSE;
          displaySysInfo(pApp);
          break;
        case MENU_IDX_RUN_MLREPL:
          pApp->clearDisplay();
          pApp->setCursor(2*FONT_WIDTH, 3*FONT_HEIGHT);
          pApp->writeSTR("STARTING NREPL...");
          pApp->updateDisplay();
          system("sudo -u chunsj /home/chunsj/bin/ml-repl&");
          pApp->sleep(4000, 0);
          IN_MENU = NXCTRL_FALSE;
          displaySysInfo(pApp);
          break;
        default:
          break;
        }
      }
    }
  }
}
