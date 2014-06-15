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
#include <signal.h>
#include <NXCTRL.h>
#include <NXCTRL_app.h>

NXCTRL_VOID
NXCTRLAPP_init (LPNXCTRLAPP pApp) {
  printf("AppInit\n");
}

NXCTRL_VOID
NXCTRLAPP_clean (LPNXCTRLAPP pApp) {
  printf("AppClean\n");
}

NXCTRL_VOID
NXCTRLAPP_run (LPNXCTRLAPP pApp) {
  printf("AppRun\n");
}
