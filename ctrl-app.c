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
#include <sys/sysinfo.h>

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

#define HCSR04_BANK                 NXCTRL_P8

#define TRIGGER_PIN                 NXCTRL_PIN11
#define ECHO_PIN                    NXCTRL_PIN15

#define TMP36_PIN                   NXCTRL_A1

#define DPY_IDLE_COUNT_MAX          300
#define MIN_ACTION_DURATION         250
#define TMP36_UPDATE_PERIOD         20
#define TMP36_DELTA                 0.00
#define CPUTEMP_BASE                56.0
#define CPUTEMP_SSG                 60.0

#define MENU_IDX_CNT                5

#define MENU_IDX_TURN_OFF_MENU      0
#define MENU_IDX_CONN_INFO          1
#define MENU_IDX_SYSINFO            2
#define MENU_IDX_HCSR04             3
#define MENU_IDX_ESHUTDOWN          4

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8

int MENU_BUTTON_STATE = NXCTRL_LOW;
int EXEC_BUTTON_STATE = NXCTRL_LOW;
long long LAST_ACTION_TIME = 0;

int DPY_STATE = NXCTRL_OFF;
int DPY_IDLE_COUNT = 0;

int MENU_IDX = MENU_IDX_TURN_OFF_MENU;
NXCTRL_BOOL IN_EXEC = NXCTRL_FALSE;

float TMP36_VOLTAGE = 0;
int TMP36_UPDATE_CNT = 0;

NXCTRLOLED OLED;
int nSPIFD;

tpruss_intc_initdata nINTC = PRUSS_INTC_INITDATA;
void *pPRUDataMem = NULL;
unsigned int *pnPRUData = NULL;

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
    fTmp -= (fCPUTemp > CPUTEMP_BASE) ? (fCPUTemp - CPUTEMP_BASE + 0.7) : 0;

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
MENU_ACTION_SYSINFO (NXCTRL_VOID) {
  struct sysinfo si;
  int d, h, m;
  int t;
  char rch[22];
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

  sprintf(rch, " RUN: %3dD %02dH %02dM\n", d, h, m);
  __WriteStringToOLED(rch);

  sprintf(rch, " TMP: %3dC\n", t);
  __WriteStringToOLED(rch);

  NXCTRLOLEDUpdateDisplay(&OLED);
}

NXCTRL_VOID
MENU_ACTION_HCSR04 (NXCTRL_VOID) {
  char rch[22];
  prussdrv_pru_wait_event(PRU_EVTOUT_0);
  if (prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT))
    fprintf(stderr, "prussdrv_pru_clear_event() failed\n");
  sprintf(rch, "DIST: %4.1f cm\n", (float)pnPRUData[0]/2.0/29.1);
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDSetCursor(&OLED, 4*FONT_WIDTH, 3*FONT_HEIGHT);
  __WriteStringToOLED(rch);
  NXCTRLOLEDUpdateDisplay(&OLED);
}

NXCTRL_VOID
MENU_ACTION_ESHUTDOWN (NXCTRL_VOID) {
  NXCTRLOLEDClearDisplay(&OLED);
  NXCTRLOLEDSetCursor(&OLED, 5*FONT_WIDTH, 3*FONT_HEIGHT);
  __WriteStringToOLED("TURN OFF...");
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

  // halt and disable the PRU
  if (prussdrv_pru_disable(PRU_NUM))
    fprintf(stderr, "prussdrv_pru_disable() failed\n");

  // release the PRU clocks and disable prussdrv module
  if (prussdrv_exit())
    fprintf(stderr, "prussdrv_exit() failed\n");
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

  if (MENU_IDX == MENU_IDX_ESHUTDOWN) {
    NXCTRLOLEDWrite(&OLED, chSel);
    __WriteStringToOLED(" TURN OFF\n");
  } else
    __WriteStringToOLED("  TURN OFF\n");
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int nRet;
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

  // initialize PRU
  if ((nRet = prussdrv_init())) {
    fprintf(stderr, "prussdrv_init() failed\n");
    exit(nRet);
  }

  // open the interrupt
  if ((nRet = prussdrv_open(PRU_EVTOUT_0))) {
    fprintf(stderr, "prussdrv_open() failed: %s\n", strerror(errno));
    exit(nRet);
  }

  // initialize interrupt
  if ((nRet = prussdrv_pruintc_init(&nINTC))) {
    fprintf(stderr, "prussdrv_pruintc_init() failed\n");
    exit(nRet);
  }

  prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pPRUDataMem);
  pnPRUData = (unsigned int *)pPRUDataMem;

  // load and run the PRU program
  if ((nRet = prussdrv_exec_program(PRU_NUM, PRU_PATH))) {
    fprintf(stderr, "prussdrv_exec_program() failed\n");
    exit(nRet);
  }

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
    if (MENU_BUTTON_STATE == NXCTRL_HIGH) {
      DPY_IDLE_COUNT = 0;
      NXCTRLOLEDDisplayNormal(&OLED);
      __DisplayMenu();
      NXCTRLOLEDUpdateDisplay(&OLED);
      DPY_STATE = NXCTRL_ON;
      __NextActionOkay();
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
            case MENU_IDX_SYSINFO:
              MENU_ACTION_SYSINFO();
              break;
            case MENU_IDX_HCSR04:
              MENU_ACTION_HCSR04();
              break;
            case MENU_IDX_ESHUTDOWN:
              MENU_ACTION_ESHUTDOWN();
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

  TMP36_UPDATE_CNT++;
  if (TMP36_UPDATE_CNT > TMP36_UPDATE_PERIOD) {
    TMP36_UPDATE_CNT = 0;
    TMP36_VOLTAGE = NXCTRLAnalogRead(TMP36_PIN)*1.8/4096.0;
  }

  NXCTRLSleep(100, 0);
}

int
main (void) {
  return NXCTRLMain();
}
