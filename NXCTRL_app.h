/*
 * NXCTRL BeagleBone Black Control Library
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

#ifndef __NXCTRL_APP_HEADER__
#define __NXCTRL_APP_HEADER__

#define APPINITFUNCTIONNAME  "NXCTRLAPP_init"
#define APPRUNFUNCTIONNAME   "NXCTRLAPP_run"
#define APPCLEANFUNCTIONNAME "NXCTRLAPP_clean"

typedef struct __tagNXCTRLAPP* LPNXCTRLAPP;

typedef NXCTRL_VOID (*APPINITFN) (LPNXCTRLAPP pApp);
typedef NXCTRL_VOID (*APPRUNFN) (LPNXCTRLAPP pApp);
typedef NXCTRL_VOID (*APPCLEANFN) (LPNXCTRLAPP pApp);

typedef struct __tagNXCTRLAPP {
  NXCTRL_VOID *pHandle;
  APPINITFN pfnInit;
  APPRUNFN pfnRun;
  APPCLEANFN pfnClean;
} NXCTRLAPP;

#endif /* __NXCTRL_APP_HEADER__ */
