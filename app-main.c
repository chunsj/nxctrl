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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <NXCTRL_appEx.h>

#define LOGO_WIDTH                  128
#define LOGO_HEIGHT                 64

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8

#define BANNER_TIME_SECS            3

NXCTRL_VOID
NXCTRLAPP_init (LPNXCTRLAPP pApp) {
}

NXCTRL_VOID
NXCTRLAPP_clean (LPNXCTRLAPP pApp) {
}

NXCTRL_VOID
NXCTRLAPP_run (LPNXCTRLAPP pApp) {
  register int i;
  char rch[22];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(rch,
          "  %s%d/%s%d %s%d:%s%d:%s%d",
          (tm.tm_mon + 1) > 9 ? "" : "0", tm.tm_mon + 1,
          tm.tm_mday > 9 ? "" : "0", tm.tm_mday,
          tm.tm_hour > 9 ? "" : "0", tm.tm_hour,
          tm.tm_min > 9 ? "" : "0", tm.tm_min,
          tm.tm_sec > 9 ? "" : "0", tm.tm_sec);
  for (i = strlen(rch); i < 21; i++)
    rch[i] = ' ';
  rch[21] = 0;
  pApp->clearDisplay();
  pApp->setCursor(0, 0);
  pApp->writeSTR(rch);
  pApp->updateDisplay();
  pApp->sleep(100, 0);
}
