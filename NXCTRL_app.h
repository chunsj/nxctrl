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

typedef NXCTRL_BOOL (*APPPINMUX) (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                  NXCTRL_MODE nMode, NXCTRL_PULLTYPE nPullUpDown,
                                  NXCTRL_BOOL bReceiverActive);
typedef NXCTRL_BOOL (*APPPINMODE) (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                   NXCTRL_DIR nDir);
typedef NXCTRL_VOID (*APPSLEEP) (NXCTRL_UINT32 msec, NXCTRL_UINT32 usec);
typedef NXCTRL_BOOL (*APPDIGITALREAD) (NXCTRL_BANK nBank, NXCTRL_PIN nPin);
typedef NXCTRL_VOID (*APPDIGITALWRITE) (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                        NXCTRL_BOOL bF);
typedef NXCTRL_UINT32 (*APPANALOGREAD) (NXCTRL_AIN nAIN);
typedef NXCTRL_VOID (*APPANALOGWRITE) (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                       NXCTRL_UINT32 nValue);
typedef NXCTRL_VOID (*APPSERVOWRITE) (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                      NXCTRL_UINT32 nDegree);

typedef NXCTRL_VOID (*APPCLEARDPY) (NXCTRL_VOID);
typedef NXCTRL_VOID (*APPUPDATEDPY) (NXCTRL_VOID);
typedef NXCTRL_VOID (*APPSETCURSOR) (NXCTRL_UINT8 nX, NXCTRL_UINT8 nY);
typedef NXCTRL_VOID (*APPWRITESTR) (const char *psz);

typedef struct __tagNXCTRLAPP {
  NXCTRL_VOID *pHandle;
  APPINITFN pfnInit;
  APPRUNFN pfnRun;
  APPCLEANFN pfnClean;

  APPPINMUX pfnPinMux;
  APPPINMODE pfnPinMode;
  APPSLEEP pfnSleep;
  APPDIGITALREAD pfnDigitalRead;
  APPDIGITALWRITE pfnDigitalWrite;
  APPANALOGREAD pfnAnalogRead;
  APPANALOGWRITE pfnAnalogWrite;
  APPSERVOWRITE pfnServoWrite;

  APPCLEARDPY pfnClearDisplay;
  APPUPDATEDPY pfnUpdateDisplay;
  APPSETCURSOR pfnSetCursor;
  APPWRITESTR pfnWriteSTR;
} NXCTRLAPP;

#endif /* __NXCTRL_APP_HEADER__ */
