/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Revised BBB Control application with Two Button interface
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
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <NXCTRL.h>
#include <NXCTRL_oled.h>
#include <NXCTRL_bitArray.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

#include <sys/reboot.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

#include <dlfcn.h>
#include <NXCTRL_appEx.h>

#include <errno.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#define MENU_BUTTON_BANK            NXCTRL_P8
#define MENU_BUTTON_PIN             NXCTRL_PIN29
#define EXEC_BUTTON_BANK            NXCTRL_P8
#define EXEC_BUTTON_PIN             NXCTRL_PIN31

#define OLED_BANK                   NXCTRL_P9

#define SPI_CS0                     NXCTRL_PIN17
#define SPI_D0                      NXCTRL_PIN21
#define SPI_D1                      NXCTRL_PIN18
#define SPI_CLK                     NXCTRL_PIN22

#define OLED_DC                     NXCTRL_PIN15
#define OLED_RST                    NXCTRL_PIN13
#define OLED_CS                     SPI_CS0
#define OLED_DATA                   SPI_D1
#define OLED_CLK                    SPI_CLK

NXCTRLOLED OLED;
int nSPIFD;

const char *rpszApps[] = {
  "/usr/bin/app-main.app",
  "/usr/bin/app-sysutil.app",
  "/usr/bin/app-blank.app",
  "/usr/bin/app-conninfo.app",
  "/usr/bin/app-sysinfo.app",
  "/usr/bin/app-peri.app",
  "/usr/bin/app-spark.app"
};

int nCurrentAppIdx = 0;
int nPrevAppIdx = 0;
NXCTRL_VOID *pActiveAppHandle = NULL;
NXCTRLAPP activeApp;
APPINITFN appInit = NULL;
APPRUNFN appRun = NULL;
APPCLEANFN appClean = NULL;

NXCTRL_VOID
__ClearDisplay (NXCTRL_VOID) {
  NXCTRLOLEDClearDisplay(&OLED);
}

NXCTRL_VOID
__UpdateDisplay (NXCTRL_VOID) {
  NXCTRLOLEDUpdateDisplay(&OLED);
}

NXCTRL_VOID
__SetCursor (NXCTRL_UINT8 nX, NXCTRL_UINT8 nY) {
  NXCTRLOLEDSetCursor(&OLED, nX, nY);
}

NXCTRL_VOID
__WriteStringToOLED (const char *psz) {
  int n = strlen(psz);
  register int i;
  for (i = 0; i < n; i++)
    NXCTRLOLEDWrite(&OLED, psz[i]);
}

NXCTRL_VOID
__DrawPixel (NXCTRL_UINT8 nX, NXCTRL_UINT8 nY, NXCTRL_BOOL onOff) {
  NXCTRLOLEDDrawPixel(&OLED, nX, nY, onOff);
}

NXCTRL_VOID
__DrawLine (NXCTRL_UINT8 nX0, NXCTRL_UINT8 nY0,
            NXCTRL_UINT8 nX1, NXCTRL_UINT8 nY1, NXCTRL_BOOL onOff) {
  NXCTRLOLEDDrawLine(&OLED, nX0, nY0, nX1, nY1, onOff);
}

NXCTRL_VOID
__DrawBitmap (NXCTRL_UINT8 nX, NXCTRL_UINT8 nY,
              NXCTRL_UINT8 *pBitmap,
              NXCTRL_UINT8 nW, NXCTRL_UINT8 nH,
              NXCTRL_BOOL onOff) {
  //NXCTRLOLEDDrawBitmap(&OLED, nX, nY, pBitmap, nW, nH, onOff);
  NXCTRLBITARRAY arrBits;
  int i, j;
  arrBits.arr = pBitmap;
  arrBits.nBits = nW * nH;
  for (i = nX; i < (nX + nW); i++) {
    for (j = nY; j < (nY + nH); j++) {
      if (NXCTRLBITARRAYGet(&arrBits, i + j * nW))
        NXCTRLOLEDDrawPixel(&OLED, i, j, NXCTRL_ON);
      else
        NXCTRLOLEDDrawPixel(&OLED, i, j, NXCTRL_OFF);
    }
  }
}

NXCTRL_VOID
__DrawBanner (NXCTRL_VOID) {
  NXCTRLOLEDSetBanner(&OLED);
}

NXCTRL_VOID
__loadApp (int nCmd) {
  if (pActiveAppHandle != NULL) {
    appClean(&activeApp);
    if (pActiveAppHandle)
      dlclose(pActiveAppHandle);
  }

  pActiveAppHandle = dlopen(rpszApps[nCmd], RTLD_LAZY);
  if (!pActiveAppHandle) {
    fprintf(stderr, "cannot load %s\n", rpszApps[nCmd]);
    return;
  }

  nCurrentAppIdx = nCmd;
  appInit = (APPINITFN)dlsym(pActiveAppHandle, APPINITFUNCTIONNAME);
  appRun = (APPRUNFN)dlsym(pActiveAppHandle, APPRUNFUNCTIONNAME);
  appClean = (APPCLEANFN)dlsym(pActiveAppHandle, APPCLEANFUNCTIONNAME);

  activeApp.pData = NULL;
  activeApp.pinMux = (APPPINMUX)NXCTRLPinMux;
  activeApp.pinMode = (APPPINMODE)NXCTRLPinMode;
  activeApp.sleep = (APPSLEEP)NXCTRLSleep;
  activeApp.digitalRead = (APPDIGITALREAD)NXCTRLDigitalRead;
  activeApp.digitalWrite = (APPDIGITALWRITE)NXCTRLDigitalWrite;
  activeApp.analogRead = (APPANALOGREAD)NXCTRLAnalogRead;
  activeApp.analogWrite = (APPANALOGWRITE)NXCTRLAnalogWrite;
  activeApp.servoWrite = (APPSERVOWRITE)NXCTRLServoWrite;
  
  activeApp.clearDisplay = (APPCLEARDPY)__ClearDisplay;
  activeApp.updateDisplay = (APPUPDATEDPY)__UpdateDisplay;
  activeApp.setCursor = (APPSETCURSOR)__SetCursor;
  activeApp.writeSTR = (APPWRITESTR)__WriteStringToOLED;

  activeApp.drawPixel = (APPDRAWPIXEL)__DrawPixel;
  activeApp.drawLine = (APPDRAWLINE)__DrawLine;
  activeApp.drawBitmap = (APPDRAWBITMAP)__DrawBitmap;
  activeApp.drawBanner = (APPDRAWBANNER)__DrawBanner;

  if (!appInit || !appRun || !appClean) {
    fprintf(stderr, "cannot find required functions\n");
    dlclose(pActiveAppHandle);
    pActiveAppHandle = NULL;
    return;
  }

  appInit(&activeApp);
}

NXCTRL_VOID
INTC_HANDLER (NXCTRL_VOID) {
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDUpdateDisplay(&OLED);
  close(nSPIFD);
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  uint8_t nLSB;
  uint32_t nSpeed, nSPIMode;
  
  NXCTRLPinMux(MENU_BUTTON_BANK, MENU_BUTTON_PIN, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMode(MENU_BUTTON_BANK, MENU_BUTTON_PIN, NXCTRL_INPUT_PULLDN);
  NXCTRLPinMode(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN, NXCTRL_INPUT_PULLDN);

  NXCTRLPinMux(OLED_BANK, SPI_CS0, NXCTRL_MODE0, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(OLED_BANK, SPI_D1, NXCTRL_MODE0, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(OLED_BANK, SPI_D0, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);
  NXCTRLPinMux(OLED_BANK, SPI_CLK, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);

  nSPIFD = open("/dev/spidev1.0", O_RDWR);

  nLSB = 0;
  ioctl(nSPIFD, SPI_IOC_WR_LSB_FIRST, &nLSB);
  nSpeed = 20000000;
  ioctl(nSPIFD, SPI_IOC_WR_MAX_SPEED_HZ, &nSpeed);
  nSPIMode = SPI_MODE_0;
  ioctl(nSPIFD, SPI_IOC_WR_MODE, &nSPIMode);

  NXCTRLOLEDInit(&OLED,
                 OLED_BANK, OLED_DC, OLED_BANK, OLED_RST,
                 nSPIFD);
  
  NXCTRLSetSIGINTHandler(INTC_HANDLER);
  NXCTRLSetSIGTERMHandler(INTC_HANDLER);

  NXCTRLOLEDDisplayNormal(&OLED);
  NXCTRLOLEDUpdateDisplay(&OLED);

  __loadApp(0);
}

// XXX ideas
// should have list/table of apps
// when init, default app is in active state
// menu button recalls menu display for given app and rotate menus afterward
// exec button executes given menu item
// every menu should have default items; turn off menu/go next app/turn off(?)
// every app should be shared object like ctrl-app.app
// it seems that every app should act cooperatively
// every app should check idle time and turn off screen if it can
// every app should provide menu which displays default action like apple
// menu in a Mac; default action/menu could be an app, I think
//
// app 1: displays clock and temperature
// app 2: displays system status
// app 3: displays network/wireless status/quality
// app 4, …: other sensor, custom apps

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
  activeApp.nCmd = -1;
  appRun(&activeApp);
  if (activeApp.nCmd != -1) {
    if (nCurrentAppIdx == 1) {
      if (activeApp.nCmd == 2) {
        nPrevAppIdx = 0;
        __loadApp(activeApp.nCmd);
        activeApp.nCmd = -1;
      } else
        __loadApp(nPrevAppIdx);
    } else {
      nPrevAppIdx = nCurrentAppIdx;
      __loadApp(activeApp.nCmd);
      activeApp.nCmd = -1;
    }
  }
  NXCTRLSleep(100, 0);
}

int
main (void) {
  return NXCTRLMain();
}
