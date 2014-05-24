/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Simple BBB Control application with Two Button interface
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

#define MENU_BUTTON_BANK            NXCTRL_P9
#define MENU_BUTTON_PIN             NXCTRL_PIN27
#define EXEC_BUTTON_BANK            NXCTRL_P8
#define EXEC_BUTTON_PIN             NXCTRL_PIN31

#define OLED_BANK                   NXCTRL_P9

#define SPI_CS0                     NXCTRL_PIN17
#define SPI_D0                      NXCTRL_PIN21
#define SPI_D1                      NXCTRL_PIN18
#define SPI_CLK                     NXCTRL_PIN22

#define OLED_DC                     NXCTRL_PIN13
#define OLED_RST                    NXCTRL_PIN15
#define OLED_CS                     SPI_CS0
#define OLED_DATA                   SPI_D1
#define OLED_CLK                    SPI_CLK

#define DPY_IDLE_COUNT_MAX          300
#define MIN_ACTION_DURATION         400

#define MENU_IDX_CNT                3

#define MENU_IDX_TURN_OFF_MENU      0
#define MENU_IDX_CONN_INFO          1
#define MENU_IDX_SHUTDOWN           2

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8

int MENU_BUTTON_STATE = NXCTRL_LOW;
int EXEC_BUTTON_STATE = NXCTRL_LOW;
long long LAST_ACTION_TIME = 0;

int DPY_STATE = NXCTRL_OFF;
int DPY_IDLE_COUNT = 0;

int MENU_IDX = MENU_IDX_TURN_OFF_MENU;
NXCTRL_BOOL IN_EXEC = NXCTRL_FALSE;

NXCTRLOLED OLED;
int nSPIFD;

NXCTRL_BOOL
__NextActionOkay (NXCTRL_VOID) {
  struct timespec tm;
  long long timeInMillis;
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
  char rch[20];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(rch,
          "%d-%s%d-%s%d %s%d:%s%d:%s%d",
          tm.tm_year + 1900,
          (tm.tm_mon + 1) > 9 ? "" : "0", tm.tm_mon + 1,
          tm.tm_mday > 9 ? "" : "0", tm.tm_mday,
          tm.tm_hour > 9 ? "" : "0", tm.tm_hour,
          tm.tm_min > 9 ? "" : "0", tm.tm_min,
          tm.tm_sec > 9 ? "" : "0", tm.tm_sec);
  rch[19] = 0;
  NXCTRLOLEDSetCursor(&OLED, FONT_WIDTH, 0);
  __WriteStringToOLED(rch);
}

NXCTRL_VOID
MENU_ACTION_TURN_OFF_MENU (NXCTRL_VOID) {
  DPY_IDLE_COUNT = 0;
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDUpdateDisplay(&OLED);
  DPY_STATE = NXCTRL_OFF;
  MENU_IDX = MENU_IDX_TURN_OFF_MENU;
}

NXCTRL_VOID
MENU_ACTION_CONN_INFO (NXCTRL_VOID) {
  struct ifaddrs *ifaddr, *ifa;
  int n;
  char rchHost[NI_MAXHOST];
  char rchBuffer[1024];

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
  NXCTRLOLEDUpdateDisplay(&OLED);
}

NXCTRL_VOID
MENU_ACTION_SHUTDOWN (NXCTRL_VOID) {
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDSetCursor(&OLED, 5*FONT_WIDTH, 3*FONT_HEIGHT);
  __WriteStringToOLED("SHUTDOWN...");
  NXCTRLOLEDUpdateDisplay(&OLED);

  sync();
  sync();
  sync();
  NXCTRLSleep(500, 0);
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDUpdateDisplay(&OLED);
  //reboot(RB_POWER_OFF);
  system("/usr/bin/poweroff");
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
    __WriteStringToOLED(" TURN OFF MENU\n");
  } else
    __WriteStringToOLED("  TURN OFF MENU\n");
    
  if (MENU_IDX == MENU_IDX_CONN_INFO) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" CONNECTION INFO\n");
  } else
    __WriteStringToOLED("  CONNECTION INFO\n");

  if (MENU_IDX == MENU_IDX_SHUTDOWN) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" EMERGENCY SHUTDOWN\n");
  } else
    __WriteStringToOLED("  EMERGENCY SHUTDOWN\n");
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

  __NextActionOkay();
  NXCTRLOLEDDisplayNormal(&OLED);
  __DisplayMenu();
  NXCTRLOLEDUpdateDisplay(&OLED);
  DPY_STATE = NXCTRL_ON;
}

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
  static int nTimeUpdateCount = 0;
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

  if (DPY_STATE == NXCTRL_OFF) {
    if (__NextActionOkay()) {
      if (MENU_BUTTON_STATE == NXCTRL_HIGH) {
        DPY_IDLE_COUNT = 0;
        NXCTRLOLEDDisplayNormal(&OLED);
        __DisplayMenu();
        NXCTRLOLEDUpdateDisplay(&OLED);
        DPY_STATE = NXCTRL_ON;
      }
    }
  } else {
    if (MENU_BUTTON_STATE == NXCTRL_HIGH || EXEC_BUTTON_STATE == NXCTRL_HIGH) {
      if (MENU_BUTTON_STATE == NXCTRL_HIGH) {
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
      if (EXEC_BUTTON_STATE == NXCTRL_HIGH) {
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
            case MENU_IDX_SHUTDOWN:
              MENU_ACTION_SHUTDOWN();
              break;
            }
          }
        }
      }
    } else {
      DPY_IDLE_COUNT++;
      if (DPY_IDLE_COUNT > DPY_IDLE_COUNT_MAX)
        MENU_ACTION_TURN_OFF_MENU();
      else {
        nTimeUpdateCount++;
        if (nTimeUpdateCount > 9) {
          nTimeUpdateCount = 0;
          if (!IN_EXEC) {
            __WriteDateTime();
            NXCTRLOLEDUpdateDisplay(&OLED);
          }
        }
      }
    }
  }

  NXCTRLSleep(100, 0);
}

int
main (void) {
  return NXCTRLMain();
}
