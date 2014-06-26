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

#include <NXCTRL_types.h>

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
typedef NXCTRL_VOID (*APPDRAWPIXEL) (NXCTRL_UINT8 nX, NXCTRL_UINT8 nY,
                                     NXCTRL_BOOL onOff);
typedef NXCTRL_VOID (*APPDRAWLINE) (NXCTRL_UINT8 nX0, NXCTRL_UINT8 nY0,
                                    NXCTRL_UINT8 nX1, NXCTRL_UINT8 nY1,
                                    NXCTRL_BOOL onOff);
typedef NXCTRL_VOID (*APPDRAWBITMAP) (NXCTRL_UINT8 nX, NXCTRL_UINT8 nY,
                                      NXCTRL_UINT8 *pBitmap,
                                      NXCTRL_UINT16 w, NXCTRL_UINT16 h,
                                      NXCTRL_BOOL onOff);
typedef NXCTRL_VOID (*APPDRAWBANNER) (NXCTRL_VOID);

typedef struct __tagNXCTRLAPP {
  NXCTRL_VOID *pData;

  APPPINMUX pinMux;
  APPPINMODE pinMode;
  APPSLEEP sleep;
  APPDIGITALREAD digitalRead;
  APPDIGITALWRITE digitalWrite;
  APPANALOGREAD analogRead;
  APPANALOGWRITE analogWrite;
  APPSERVOWRITE servoWrite;

  APPCLEARDPY clearDisplay;
  APPUPDATEDPY updateDisplay;
  APPSETCURSOR setCursor;
  APPWRITESTR writeSTR;
  APPDRAWPIXEL drawPixel;
  APPDRAWLINE drawLine;
  APPDRAWBITMAP drawBitmap;
  APPDRAWBANNER drawBanner;
} NXCTRLAPP;

#define MENU_BUTTON_BANK            NXCTRL_P8
#define MENU_BUTTON_PIN             NXCTRL_PIN29
#define EXEC_BUTTON_BANK            NXCTRL_P8
#define EXEC_BUTTON_PIN             NXCTRL_PIN31

#endif /* __NXCTRL_APP_HEADER__ */
