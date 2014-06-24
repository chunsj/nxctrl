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
#include <NXCTRL_app.h>

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

#define PRU_NUM                     PRU0
#define PRU_PATH                    "/usr/bin/ctrl-app.bin"

#define USR_APP_PATH                "/usr/bin/ctrl-app.app"
#define REPORT_PATH                 "/usr/bin/ctrl-app-report"

#define HCSR04_BANK                 NXCTRL_P8
#define HCSR04_MAX_CNT              1
#define HCSR04_MAX_DIST             100

#define TRIGGER_PIN                 NXCTRL_PIN11
#define ECHO_PIN                    NXCTRL_PIN15

#define TMP36_PIN                   NXCTRL_A1

#define DPY_IDLE_COUNT_MAX          300
#define MIN_ACTION_DURATION         180
#define TMP36_UPDATE_PERIOD         20
#define TMP36_DELTA                 0.00
#define CPUTEMP_BASE                56.0
#define CPUTEMP_SSG                 60.0
#define SSG_DELTA                   0.00

#define MENU_IDX_CNT                6

#define MENU_IDX_TURN_OFF_MENU      0
#define MENU_IDX_CONN_INFO          1
#define MENU_IDX_SYSINFO            2
#define MENU_IDX_HCSR04             3
#define MENU_IDX_USERAPP            4
#define MENU_IDX_ESHUTDOWN          5

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8

int MENU_BUTTON_STATE = NXCTRL_LOW;
int EXEC_BUTTON_STATE = NXCTRL_LOW;
unsigned long long LAST_ACTION_TIME = 0;

int DPY_STATE = NXCTRL_OFF;
int DPY_IDLE_COUNT = 0;

int MENU_IDX = MENU_IDX_TURN_OFF_MENU;
NXCTRL_BOOL IN_EXEC = NXCTRL_FALSE;

float TMP36_VOLTAGE = 0;
int TMP36_UPDATE_CNT = 0;

NXCTRLOLED OLED;
int nSPIFD;

unsigned int
__CPUTemp (NXCTRL_VOID) {
  const char *psz = "/sys/class/hwmon/hwmon0/device/temp1_input";
  unsigned int n;
  FILE *pFile = fopen(psz, "r");
  if (!pFile) {
    system("rmmod am335x_bandgap");
    system("modprobe am335x_bandgap");
  }
  pFile = fopen(psz, "r");
  if (!pFile) {
    return 0;
  }
  fscanf(pFile, "%d", &n);
  fclose(pFile);
  if (n > 120) {
    system("rmmod am335x_bandgap");
    system("modprobe am335x_bandgap");
  }
  pFile = fopen(psz, "r");
  if (!pFile) {
    return 0;
  }
  fscanf(pFile, "%d", &n);
  fclose(pFile);
  return n;
}

NXCTRL_BOOL
__NextActionOkay (NXCTRL_VOID) {
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

NXCTRL_VOID
__WriteStringToOLED (const char *psz) {
  int n = strlen(psz);
  register int i;
  for (i = 0; i < n; i++)
    NXCTRLOLEDWrite(&OLED, psz[i]);
}

NXCTRL_VOID
__WriteDateTime (NXCTRL_VOID) {
  int i;
  char rch[22];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  struct sysinfo si;
  float fTmp = (TMP36_VOLTAGE + TMP36_DELTA - 0.5) * 100;
  float fCPUTemp = __CPUTemp()/1000.0;

  fCPUTemp = (fCPUTemp > 120) ? (fCPUTemp - 74) : fCPUTemp;
  if (fCPUTemp < CPUTEMP_SSG)
    fTmp -= (fCPUTemp > CPUTEMP_BASE) ? (fCPUTemp - CPUTEMP_BASE) : 0;
  else
    fTmp -= (fCPUTemp > CPUTEMP_BASE) ? (fCPUTemp - CPUTEMP_BASE + SSG_DELTA) : 0;

  sysinfo(&si);
  if (fTmp < -30 || fTmp > 50) fTmp = 0;
  sprintf(rch,
          "  %s%d/%s%d %s%d:%s%d:%s%d %2.0fC",
          (tm.tm_mon + 1) > 9 ? "" : "0", tm.tm_mon + 1,
          tm.tm_mday > 9 ? "" : "0", tm.tm_mday,
          tm.tm_hour > 9 ? "" : "0", tm.tm_hour,
          tm.tm_min > 9 ? "" : "0", tm.tm_min,
          tm.tm_sec > 9 ? "" : "0", tm.tm_sec,
          fTmp);
  for (i = strlen(rch); i < 21; i++)
    rch[i] = ' ';
  rch[21] = 0;
  NXCTRLOLEDSetCursor(&OLED, 0, 0);
  __WriteStringToOLED(rch);
}

NXCTRL_VOID
MENU_ACTION_TURN_OFF_MENU (NXCTRL_VOID) {
  DPY_IDLE_COUNT = 0;
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDUpdateDisplay(&OLED);
  DPY_STATE = NXCTRL_OFF;
  MENU_IDX = MENU_IDX_TURN_OFF_MENU;
  LAST_ACTION_TIME = 0;
}

NXCTRL_BOOL
__GetMacAddress (char *pszIFName, char *pszMacIP) {
  char rchIFName[BUFSIZ];
  FILE *pFile;
  int i;

  sprintf(rchIFName, "/sys/class/net/%s/address", pszIFName);
  pFile = fopen(rchIFName, "r");
  if (!pFile)
    return NXCTRL_FALSE;

  fscanf(pFile, "%s", pszMacIP);
  fclose(pFile);
  for (i = 0; i < strlen(pszMacIP); i++)
    pszMacIP[i] = toupper(pszMacIP[i]);
  return NXCTRL_TRUE;
}

NXCTRL_BOOL
__GetDefaultGW (char *pszGW) {
  FILE *pFile = fopen("/proc/net/route", "r");
  char rch[1024];
  char rchIFace[32], rchDest[32], rchGW[32];
  NXCTRL_BOOL bFound = NXCTRL_OFF;
  int i0, i1, i2, i3;
  
  if (!pFile)
    return NXCTRL_FALSE;
  
  while (fgets(rch, 1023, pFile)) {
    sscanf(rch, "%s %s %s", rchIFace, rchDest, rchGW);
    if (!strcmp(rchDest, "00000000")) {
      bFound = NXCTRL_ON;
      break;
    }
  }
  fclose(pFile);

  if (!bFound)
    return NXCTRL_FALSE;

  sscanf(rchGW, "%2x%2x%2x%2x", &i0, &i1, &i2, &i3);
  sprintf(pszGW, "%d.%d.%d.%d", i3, i2, i1, i0);

  return NXCTRL_TRUE;
}

NXCTRL_VOID
__PingToDefaultGW (NXCTRL_VOID) {
  char rch[1024], rchGW[32];
  if (__GetDefaultGW(rchGW)) {
    sprintf(rch, "ping -c 1 -W 1 %s >& /dev/null", rchGW);
    system(rch);
  }
}

NXCTRL_VOID
MENU_ACTION_CONN_INFO (NXCTRL_VOID) {
  struct ifaddrs *ifaddr, *ifa;
  int n;
  char rchHost[NI_MAXHOST];
  char rchBuffer[1024];
  char rchGW[32];

  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDSetCursor(&OLED, 0, 0);

  if (getifaddrs(&ifaddr) == -1) {
    __WriteStringToOLED("ERROR IN CONN INFO");
    NXCTRLOLEDUpdateDisplay(&OLED);
    return;
  }

  NXCTRLOLEDSetCursor(&OLED, FONT_WIDTH*3, 0);
  __WriteStringToOLED("CONNECTION INFO\n\n");

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL) continue;
    n = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                    rchHost, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
    if (n == 0) {
      memset(rchBuffer, 0, 1024);
      sprintf(rchBuffer, "%5s: %s\n", ifa->ifa_name, rchHost);
      __WriteStringToOLED(rchBuffer);
    }
  }
  if (__GetDefaultGW(rchGW)) {
    sprintf(rchBuffer, "%5s: %s\n", "gw", rchGW);
    __WriteStringToOLED(rchBuffer);
  }
  NXCTRLOLEDUpdateDisplay(&OLED);
  __PingToDefaultGW();
}

NXCTRL_VOID
MENU_ACTION_SYSINFO (NXCTRL_VOID) {
  struct sysinfo si;
  struct statvfs stvfs;
  int d, h, m;
  int t;
  char rch[22];
  char rchMacIP[20];
  statvfs("/", &stvfs);
  sysinfo(&si);
  t = __CPUTemp() / 1000;
  d = si.uptime/(3600*24);
  h = (si.uptime - (d*3600*24))/3600;
  m = (si.uptime - (d*3600*24) - (h*3600))/60;
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDSetCursor(&OLED, 4*FONT_WIDTH, 0);
  __WriteStringToOLED("SYSTEM STATUS\n\n");

  sprintf(rch, " LDA:  %2.1f %2.1f %2.1f\n",
          si.loads[0]/65536.0,
          si.loads[1]/65536.0,
          si.loads[2]/65536.0);
  __WriteStringToOLED(rch);

  sprintf(rch, " RAM:  %03d %03d %03d\n",
          (int)(si.totalram/1024/1024),
          (int)(si.bufferram/1024/1024),
          (int)(si.freeram/1024/1024));
  __WriteStringToOLED(rch);

  sprintf(rch, " DSK:  %1.1f %1.1f %1.1f\n",
          stvfs.f_blocks*stvfs.f_frsize/1024/1024/1024.0,
          (stvfs.f_blocks - stvfs.f_bavail)*stvfs.f_frsize/1024/1024/1024.0,
          stvfs.f_bavail*stvfs.f_frsize/1024/1024/1024.0);
  __WriteStringToOLED(rch);

  sprintf(rch, " RUN: %3dD %02dH %02dM\n", d, h, m);
  __WriteStringToOLED(rch);

  sprintf(rch, " TMP: %3dC\n", t);
  __WriteStringToOLED(rch);

  if (__GetMacAddress("wlan0", rchMacIP)) {
    sprintf(rch, " %s", rchMacIP);
    __WriteStringToOLED(rch);
  }

  NXCTRLOLEDUpdateDisplay(&OLED);
}

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
MENU_ACTION_RUN_APP (NXCTRL_VOID) {
  const char *pszAppPath = USR_APP_PATH;
  NXCTRL_VOID *pHandle = NULL;
  NXCTRLAPP app;
  APPINITFN pfnInit;
  APPRUNFN pfnRun;
  APPCLEANFN pfnClean;

  pHandle = dlopen(pszAppPath, RTLD_LAZY);
  if (!pHandle) {
    fprintf(stderr, "cannot load %s\n", pszAppPath);
    return;
  }

  pfnInit = (APPINITFN)dlsym(pHandle, APPINITFUNCTIONNAME);
  pfnRun = (APPRUNFN)dlsym(pHandle, APPRUNFUNCTIONNAME);
  pfnClean = (APPCLEANFN)dlsym(pHandle, APPCLEANFUNCTIONNAME);

  app.pData = NULL;
  app.pinMux = (APPPINMUX)NXCTRLPinMux;
  app.pinMode = (APPPINMODE)NXCTRLPinMode;
  app.sleep = (APPSLEEP)NXCTRLSleep;
  app.digitalRead = (APPDIGITALREAD)NXCTRLDigitalRead;
  app.digitalWrite = (APPDIGITALWRITE)NXCTRLDigitalWrite;
  app.analogRead = (APPANALOGREAD)NXCTRLAnalogRead;
  app.analogWrite = (APPANALOGWRITE)NXCTRLAnalogWrite;
  app.servoWrite = (APPSERVOWRITE)NXCTRLServoWrite;
  
  app.clearDisplay = (APPCLEARDPY)__ClearDisplay;
  app.updateDisplay = (APPUPDATEDPY)__UpdateDisplay;
  app.setCursor = (APPSETCURSOR)__SetCursor;
  app.writeSTR = (APPWRITESTR)__WriteStringToOLED;

  app.drawPixel = (APPDRAWPIXEL)__DrawPixel;
  app.drawLine = (APPDRAWLINE)__DrawLine;
  app.drawBitmap = (APPDRAWBITMAP)__DrawBitmap;
  app.drawBanner = (APPDRAWBANNER)__DrawBanner;

  if (!pfnInit || !pfnRun || !pfnClean) {
    fprintf(stderr, "cannot find required functions\n");
    dlclose(pHandle);
    return;
  }

  pfnInit(&app);
  pfnRun(&app);
  pfnClean(&app);
  
  dlclose(pHandle);
}

float
__FetchDistance (NXCTRL_VOID) {
  int nRet;
  tpruss_intc_initdata nINTC = PRUSS_INTC_INITDATA;
  void *pPRUDataMem = NULL;
  unsigned int *pnPRUData = NULL;
  float fDist = 0.0f;

  // initialize PRU
  if ((nRet = prussdrv_init())) {
    fprintf(stderr, "prussdrv_init() failed\n");
    return 0;
  }

  // open the interrupt
  if ((nRet = prussdrv_open(PRU_EVTOUT_0))) {
    fprintf(stderr, "prussdrv_open() failed: %s\n", strerror(errno));
    return 0;
  }

  // initialize interrupt
  if ((nRet = prussdrv_pruintc_init(&nINTC))) {
    fprintf(stderr, "prussdrv_pruintc_init() failed\n");
    return 0;
  }

  prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pPRUDataMem);
  pnPRUData = (unsigned int *)pPRUDataMem;

  // load and run the PRU program
  if ((nRet = prussdrv_exec_program(PRU_NUM, PRU_PATH))) {
    fprintf(stderr, "prussdrv_exec_program() failed\n");
    return 0;
  }

  prussdrv_pru_wait_event(PRU_EVTOUT_0);
  if (prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT))
    fprintf(stderr, "prussdrv_pru_clear_event() failed\n");
  fDist = (float)pnPRUData[0]/2.0/29.1;

  // halt and disable the PRU
  if (prussdrv_pru_disable(PRU_NUM))
    fprintf(stderr, "prussdrv_pru_disable() failed\n");

  // release the PRU clocks and disable prussdrv module
  if (prussdrv_exit())
    fprintf(stderr, "prussdrv_exit() failed\n");

  return fDist;
}

NXCTRL_VOID
MENU_ACTION_HCSR04 (NXCTRL_VOID) {
  register int i, n = HCSR04_MAX_CNT;
  float fs = 0;
  char rch[22];

  for (i = 0; i < n; i++) {
    fs += (float)__FetchDistance();
  }
  fs /= n;
  
  if (fs > HCSR04_MAX_DIST)
    sprintf(rch, "DIST: > %d cm\n", HCSR04_MAX_DIST);
  else
    sprintf(rch, "DIST: %4.1f cm\n", fs);

  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDSetCursor(&OLED, 4*FONT_WIDTH, 3*FONT_HEIGHT);
  __WriteStringToOLED(rch);
  NXCTRLOLEDUpdateDisplay(&OLED);
}

NXCTRL_VOID
MENU_ACTION_ESHUTDOWN (NXCTRL_VOID) {
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDSetCursor(&OLED, 3*FONT_WIDTH, 3*FONT_HEIGHT);
  __WriteStringToOLED("TURNING OFF...");
  NXCTRLOLEDUpdateDisplay(&OLED);

  sync();
  sync();
  sync();
  NXCTRLSleep(500, 0);
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDUpdateDisplay(&OLED);
  reboot(RB_POWER_OFF);
  //system("/usr/bin/poweroff");
}

NXCTRL_VOID
INTC_HANDLER (NXCTRL_VOID) {
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDUpdateDisplay(&OLED);
  close(nSPIFD);
}

NXCTRL_VOID
__DisplayMenu (NXCTRL_VOID) {
  unsigned char chSel = 16;
  NXCTRLOLEDClearDisplay(&OLED);
  __WriteDateTime();
  NXCTRLOLEDSetCursor(&OLED, 0, 2*FONT_HEIGHT);

  if (MENU_IDX == MENU_IDX_TURN_OFF_MENU) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" MENU OFF\n");
  } else
    __WriteStringToOLED("  MENU OFF\n");
    
  if (MENU_IDX == MENU_IDX_CONN_INFO) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" CONNECTION INFO\n");
  } else
    __WriteStringToOLED("  CONNECTION INFO\n");

  if (MENU_IDX == MENU_IDX_SYSINFO) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" SYSTEM STATUS\n");
  } else
    __WriteStringToOLED("  SYSTEM STATUS\n");

  if (MENU_IDX == MENU_IDX_HCSR04) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" HCSR04 DISTANCE\n");
  } else
    __WriteStringToOLED("  HCSR04 DISTANCE\n");

  if (MENU_IDX == MENU_IDX_USERAPP) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" USER APPLICATION\n");
  } else
    __WriteStringToOLED("  USER APPLICATION\n");

  if (MENU_IDX == MENU_IDX_ESHUTDOWN) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" TURN OFF\n");
  } else
    __WriteStringToOLED("  TURN OFF\n");
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  uint8_t nLSB;
  uint32_t nSpeed, nSPIMode;
  
  NXCTRLPinMux(HCSR04_BANK, TRIGGER_PIN, NXCTRL_MODE6, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(HCSR04_BANK, ECHO_PIN, NXCTRL_MODE6, NXCTRL_PULLDN, NXCTRL_HIGH);

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

  __NextActionOkay();
  NXCTRLOLEDDisplayNormal(&OLED);
  __DisplayMenu();
  NXCTRLOLEDUpdateDisplay(&OLED);
  DPY_STATE = NXCTRL_ON;

  TMP36_VOLTAGE = NXCTRLAnalogRead(TMP36_PIN)*1.8/4096.0;
}

NXCTRL_VOID
__FetchMENUButtonState (NXCTRL_VOID) {
  if (MENU_BUTTON_STATE == NXCTRL_LOW) {
    if (NXCTRLDigitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN) == NXCTRL_HIGH) {
      MENU_BUTTON_STATE = NXCTRL_HIGH;
      DPY_IDLE_COUNT = 0;
    }
  } else {
    if (NXCTRLDigitalRead(MENU_BUTTON_BANK, MENU_BUTTON_PIN) == NXCTRL_LOW) {
      MENU_BUTTON_STATE = NXCTRL_LOW;
      DPY_IDLE_COUNT = 0;
    }
  }
}

NXCTRL_VOID
__FetchEXECButtonState (NXCTRL_VOID) {
  if (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    if (NXCTRLDigitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN) == NXCTRL_HIGH) {
      EXEC_BUTTON_STATE = NXCTRL_HIGH;
      DPY_IDLE_COUNT = 0;
    }
  } else {
    if (NXCTRLDigitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN) == NXCTRL_LOW) {
      EXEC_BUTTON_STATE = NXCTRL_LOW;
      DPY_IDLE_COUNT = 0;
    }
  }
}

NXCTRL_VOID
__TurnOnDPY (NXCTRL_VOID) {
  DPY_IDLE_COUNT = 0;
  NXCTRLOLEDDisplayNormal(&OLED);
  __DisplayMenu();
  NXCTRLOLEDUpdateDisplay(&OLED);
  DPY_STATE = NXCTRL_ON;
  __NextActionOkay();
}

NXCTRL_VOID
__OnMENUButtonActive (NXCTRL_VOID) {
  if (__NextActionOkay()) {
    if (!IN_EXEC)
      MENU_IDX++;
    else
      IN_EXEC = NXCTRL_FALSE;
    if (MENU_IDX >= MENU_IDX_CNT)
      MENU_IDX = MENU_IDX_TURN_OFF_MENU;
    __DisplayMenu();
    NXCTRLOLEDUpdateDisplay(&OLED);
  }
}

NXCTRL_VOID
__OnEXECButtonActive (NXCTRL_VOID) {
  if (__NextActionOkay()) {
    if (!IN_EXEC) {
      IN_EXEC = NXCTRL_TRUE;
      switch (MENU_IDX) {
      case MENU_IDX_TURN_OFF_MENU:
        MENU_ACTION_TURN_OFF_MENU();
        IN_EXEC = NXCTRL_FALSE;
        break;
      case MENU_IDX_CONN_INFO:
        MENU_ACTION_CONN_INFO();
        break;
      case MENU_IDX_SYSINFO:
        MENU_ACTION_SYSINFO();
        break;
      case MENU_IDX_HCSR04:
        MENU_ACTION_HCSR04();
        break;
      case MENU_IDX_USERAPP:
        MENU_ACTION_RUN_APP();
        break;
      case MENU_IDX_ESHUTDOWN:
        MENU_ACTION_ESHUTDOWN();
        break;
      }
    }
  }
}

NXCTRL_VOID
__UpdateDateTime (NXCTRL_VOID) {
  static int nTimeUpdateCount = 0;

  nTimeUpdateCount++;
  if (nTimeUpdateCount > 9) {
    nTimeUpdateCount = 0;
    if (!IN_EXEC) {
      __WriteDateTime();
      NXCTRLOLEDUpdateDisplay(&OLED);
    }
  }
}

NXCTRL_VOID
__UpdateTMP (NXCTRL_VOID) {
  TMP36_UPDATE_CNT++;
  if (TMP36_UPDATE_CNT > TMP36_UPDATE_PERIOD) {
    TMP36_UPDATE_CNT = 0;
    TMP36_VOLTAGE = NXCTRLAnalogRead(TMP36_PIN)*1.8/4096.0;
  }
}

#define MENU_ACTIVE (MENU_BUTTON_STATE == NXCTRL_HIGH)
#define EXEC_ACTIVE (EXEC_BUTTON_STATE == NXCTRL_HIGH)
#define BTN_ACTIVE  (MENU_ACTIVE || EXEC_ACTIVE)
#define DPY_OFF     (DPY_STATE == NXCTRL_OFF)

// XXX ideas
// should have list/table of apps
// when init, default app is in active state
// menu button recalls menu display for given app and rotate menus afterward
// exec button executes given menu item
// every menu should have default items; turn off menu/go next app/turn off(?)
// every app should be shared object like ctrl-app.app

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
  __FetchMENUButtonState();
  __FetchEXECButtonState();

  if (DPY_OFF) {
    if (MENU_ACTIVE)
      __TurnOnDPY();
  } else {
    if (BTN_ACTIVE) {
      if (MENU_ACTIVE)
        __OnMENUButtonActive();
      if (EXEC_ACTIVE)
        __OnEXECButtonActive();
    } else {
      DPY_IDLE_COUNT++;
      if (DPY_IDLE_COUNT > DPY_IDLE_COUNT_MAX)
        MENU_ACTION_TURN_OFF_MENU();
      else
        __UpdateDateTime();
    }
    __UpdateTMP();
  }

  NXCTRLSleep(100, 0);
}

int
main (void) {
  return NXCTRLMain();
}
