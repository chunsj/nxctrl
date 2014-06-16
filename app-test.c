/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Basic CTRL App Test Program
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
#include <NXCTRL.h>
#include <NXCTRL_app.h>

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8

NXCTRL_VOID
NXCTRLAPP_init (LPNXCTRLAPP pApp) {
  pApp->pfnClearDisplay();
  pApp->pfnSetCursor(FONT_WIDTH*3, FONT_HEIGHT*1);
  pApp->pfnWriteSTR("AppInit\n");
  pApp->pfnUpdateDisplay();
}

NXCTRL_VOID
NXCTRLAPP_clean (LPNXCTRLAPP pApp) {
  pApp->pfnSetCursor(FONT_WIDTH*3, FONT_HEIGHT*3);
  pApp->pfnWriteSTR("AppClean\n");
  pApp->pfnUpdateDisplay();
}

NXCTRL_VOID
NXCTRLAPP_run (LPNXCTRLAPP pApp) {
  char rch[BUFSIZ];
  pApp->pfnSetCursor(FONT_WIDTH*3, FONT_HEIGHT*5);
  sprintf(rch, "AppRun: %d\n", pApp->pfnAnalogRead(NXCTRL_A0));
  pApp->pfnWriteSTR(rch);
  pApp->pfnUpdateDisplay();
}
