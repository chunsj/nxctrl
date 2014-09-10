/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Peripheral Driving App Program
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
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <NXCTRL_appEx.h>
#include <NXCTRL_bits.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <math.h>
#include <termios.h>
#include <lirc/lirc_client.h>

#define FONT_WIDTH                  6
#define FONT_HEIGHT                 8
#define MENU_SEL_CHAR               ((unsigned char)16)

#define DPY_IDLE_COUNT_MAX          300
#define MIN_ACTION_DURATION         200

#define MENU_IDX_COUNT              8

#define MENU_IDX_SYSTEM_MENU        0
#define MENU_IDX_UPDATE_MENU        1
#define MENU_IDX_GPS_MENU           2
#define MENU_IDX_P8_13_PWM_MENU     3
//#define MENU_IDX_P8_19_PWM_MENU     4
//#define MENU_IDX_AK8448_CFG_MENU    5
//#define MENU_IDX_AK8448_READ_MENU   6
#define MENU_IDX_TR_A3_MENU         4
//#define MENU_IDX_DCMOTOR_MENU       8
#define MENU_IDX_IR_MENU            5
#define MENU_IDX_PININFO_MENU       6
#define MENU_IDX_EXIT_MENU          7

#define GPS_TTY                     "/dev/ttyO1"
#define GPS_BAUDRATE                B9600

#define PRU_NUM                     PRU0
#define PRU_PATH                    "/usr/bin/ctrl-app.bin"

#define HCSR04_BANK                 NXCTRL_P8
#define HCSR04_MAX_CNT              1
#define HCSR04_MAX_DIST             100

#define TRIGGER_PIN                 NXCTRL_PIN11
#define ECHO_PIN                    NXCTRL_PIN15

#define PWM1_BANK                   NXCTRL_P8
#define PWM2_BANK                   NXCTRL_P8
#define PWM1_PIN                    NXCTRL_PIN13
#define PWM2_PIN                    NXCTRL_PIN19

#define SPI_CS0                     NXCTRL_PIN28  // BLUE
#define SPI_D1                      NXCTRL_PIN30  // RED
#define SPI_D0                      NXCTRL_PIN29  // YELLOW
#define SPI_CLK                     NXCTRL_PIN31  // GREEN
#define SPI_DEV                     "/dev/spidev2.0"
#define DELAY_USEC                  0

#define AKBANK                      NXCTRL_P8
#define AKPIN0                      NXCTRL_PIN45
#define AKPIN1                      NXCTRL_PIN46
#define AKPIN2                      NXCTRL_PIN43
#define AKPIN3                      NXCTRL_PIN44
#define AKPIN4                      NXCTRL_PIN41
#define AKPIN5                      NXCTRL_PIN42
#define AKPIN6                      NXCTRL_PIN39
#define AKPIN7                      NXCTRL_PIN40
#define AKPIN8                      NXCTRL_PIN27
#define AKPIN9                      NXCTRL_PIN29
#define AKCLK                       NXCTRL_PIN28
#define AKADCK                      NXCTRL_PIN16

#define DCBANK                      NXCTRL_P8
#define DCCTRL1                     NXCTRL_PIN26
#define DCCTRL2                     NXCTRL_PIN17
#define DCENABLE                    NXCTRL_PIN19

#define CHART_HEIGHT                50
#define CHART_WIDTH                 110

static NXCTRL_BOOL                  MENU_U_BUTTON_STATE = NXCTRL_LOW;
static NXCTRL_BOOL                  MENU_D_BUTTON_STATE = NXCTRL_LOW;
static NXCTRL_BOOL                  EXEC_BUTTON_STATE = NXCTRL_LOW;
static unsigned int                 DPY_IDLE_COUNT = 0;
static unsigned char                MENU_IDX = MENU_IDX_SYSTEM_MENU;
static NXCTRL_BOOL                  IN_MENU = NXCTRL_FALSE;
static unsigned long long           LAST_ACTION_TIME = 0;

static NXCTRL_VOID
showIR (LPNXCTRLAPP pApp) {
  int fd;
  struct lirc_config *config;
  char *code;
  float fValue = 0.00;
  char rch[21];

  pApp->analogWrite(PWM1_BANK, PWM1_PIN, 0);
  pApp->sleep(100, 0);

  if ((fd = lirc_init("lirc", 1)) == -1) {
    pApp->clearDisplay();
    pApp->setCursor(6*FONT_WIDTH, 3*FONT_HEIGHT);
    pApp->writeSTR("IR ERROR");
    pApp->updateDisplay();
    pApp->sleep(1000, 0);
    return;
  }
  
  if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1) {
    pApp->clearDisplay();
    pApp->setCursor(5*FONT_WIDTH, 3*FONT_HEIGHT);
    pApp->writeSTR("IRFD ERROR");
    pApp->updateDisplay();
    pApp->sleep(1000, 0);
    return;
  }

  pApp->clearDisplay();
  pApp->setCursor(7*FONT_WIDTH, 0);
  pApp->writeSTR("IR TEST");
  pApp->setCursor(4*FONT_WIDTH, 3*FONT_HEIGHT);
  pApp->writeSTR("PRESS REMOTE");
  pApp->updateDisplay();

  MENU_U_BUTTON_STATE = NXCTRL_LOW;
  MENU_D_BUTTON_STATE = NXCTRL_LOW;

  if (lirc_readconfig(NULL, &config, NULL) == 0) {
    while (lirc_nextcode(&code) == 0) {
      MENU_U_BUTTON_STATE = pApp->digitalRead(MENU_U_BUTTON_BANK, MENU_U_BUTTON_PIN);
      if (MENU_U_BUTTON_STATE == NXCTRL_HIGH) {
        pApp->setCursor(6*FONT_WIDTH, 3*FONT_HEIGHT);
        pApp->writeSTR("TEST END");
        pApp->updateDisplay();
        free(code);
        break;
      }
      MENU_D_BUTTON_STATE = pApp->digitalRead(MENU_D_BUTTON_BANK, MENU_D_BUTTON_PIN);
      if (MENU_D_BUTTON_STATE == NXCTRL_HIGH) {
        pApp->setCursor(6*FONT_WIDTH, 3*FONT_HEIGHT);
        pApp->writeSTR("TEST END");
        pApp->updateDisplay();
        free(code);
        break;
      }

      if (!code) {
        pApp->sleep(100, 0);
        continue; 
      }

      pApp->clearDisplay();
      pApp->setCursor(7*FONT_WIDTH, 0);
      pApp->writeSTR("IR TEST");
      
      if (strstr(code, "KEY_MENU")) {
        pApp->setCursor(6*FONT_WIDTH, 3*FONT_HEIGHT);
        pApp->writeSTR("TEST END");
        pApp->updateDisplay();
        free(code);
        break;
      } else if (strstr(code, "KEY_VOLUMEUP")) {
        fValue += 1.0;
        if (fValue > 10.0) fValue = 10.0;
        pApp->analogWrite(PWM1_BANK, PWM1_PIN, pow(1000, fValue/10.0));
        sprintf(rch, "LIGHT(+): %d/10", (int)fValue);
        pApp->setCursor(3*FONT_WIDTH, 3*FONT_HEIGHT);
        pApp->writeSTR(rch);
        pApp->updateDisplay();
      } else if (strstr(code, "KEY_VOLUMEDOWN")) {
        fValue -= 1.0;
        if (fValue < 0.0) fValue = 0.0;
        pApp->analogWrite(PWM1_BANK, PWM1_PIN, pow(1000, fValue/10.0));
        sprintf(rch, "LIGHT(-): %d/10", (int)fValue);
        pApp->setCursor(3*FONT_WIDTH, 3*FONT_HEIGHT);
        pApp->writeSTR(rch);
        pApp->updateDisplay();
      } else if (strstr(code, "KEY_FORWARD")) {
        fValue = 10.0;
        pApp->analogWrite(PWM1_BANK, PWM1_PIN, pow(1000, fValue/10.0));
        sprintf(rch, "LIGHT(F): %d/10", (int)fValue);
        pApp->setCursor(3*FONT_WIDTH, 3*FONT_HEIGHT);
        pApp->writeSTR(rch);
        pApp->updateDisplay();
      } else if (strstr(code, "KEY_REWIND")) {
        fValue = 0.0;
        pApp->analogWrite(PWM1_BANK, PWM1_PIN, pow(1000, fValue/10.0));
        sprintf(rch, "LIGHT(R): %d/10", (int)fValue);
        pApp->setCursor(3*FONT_WIDTH, 3*FONT_HEIGHT);
        pApp->writeSTR(rch);
        pApp->updateDisplay();
      } else if (strstr(code, "KEY_PLAYPAUSE")) {
        if (fValue >= 10.0) fValue = 0;
        else fValue = 10.0;
        pApp->analogWrite(PWM1_BANK, PWM1_PIN, pow(1000, fValue/10.0));
        sprintf(rch, "LIGHT(P): %d/10", (int)fValue);
        pApp->setCursor(3*FONT_WIDTH, 3*FONT_HEIGHT);
        pApp->writeSTR(rch);
        pApp->updateDisplay();
      }

      free(code);

      pApp->sleep(100, 0);
    }

    lirc_freeconfig(config);
  } else {
    pApp->clearDisplay();
    pApp->setCursor(4*FONT_WIDTH, 3*FONT_HEIGHT);
    pApp->writeSTR("IRCFG ERROR");
    pApp->updateDisplay();
    pApp->sleep(1000, 0);
    return;
  }

  lirc_deinit();
  
  pApp->setCursor(0, 7*FONT_HEIGHT+1);
  pApp->writeSTR(" PRESS EXEC TO EXIT");
  pApp->updateDisplay();

  pApp->sleep(1000, 0);

  EXEC_BUTTON_STATE = NXCTRL_LOW;
  while (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    pApp->sleep(100, 0);
    EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  }

  pApp->analogWrite(PWM1_BANK, PWM1_PIN, 0);
  pApp->sleep(100, 0);
}

#if 0
inline static void
sprintINT16 (char *rch, unsigned short v16) {
  INT16 v = { .v16 = v16 };
  sprintf(rch, "%d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d",
          v.bit.b0, v.bit.b1, v.bit.b2, v.bit.b3, v.bit.b4, v.bit.b5,
          v.bit.b6, v.bit.b7, v.bit.b8, v.bit.b9, v.bit.b10, v.bit.b11,
          v.bit.b12, v.bit.b13, v.bit.b14, v.bit.b15);
}
#endif

#if 0
static int
__SPI_read (LPNXCTRLAPP pApp, int nFD) {
  int nStatus;
  struct spi_ioc_transfer xfer[2];
  char rchTXData[2];
  char rchRXData[2];
  char rch[22];

  // for register 0
  memset(xfer, 0, sizeof(xfer));

  rchTXData[0] = 0;
  rchTXData[1] = 0;
  rchRXData[0] = 0;
  rchRXData[1] = 0;

  rchTXData[0] |= BIT7; // read

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = 2;
  xfer[0].delay_usecs = DELAY_USEC;

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  pApp->writeSTR("R:");
  sprintINT16(rch, rchRXData[0] | (rchRXData[1] << 8));
  pApp->writeSTR(rch);

  // for register 1
  memset(xfer, 0, sizeof(xfer));

  rchTXData[0] = 0;
  rchTXData[1] = 0;
  rchRXData[0] = 0;
  rchRXData[1] = 0;

  rchTXData[0] |= BIT7;
  rchTXData[0] |= BIT0; // register 1

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = 2;
  xfer[0].delay_usecs = DELAY_USEC;

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  pApp->writeSTR("R:");
  sprintINT16(rch, rchRXData[0] | (rchRXData[1] << 8));
  pApp->writeSTR(rch);

  return 0;
}
#endif

#if 0
static int
__SPI_write (LPNXCTRLAPP pApp, int nFD) {
  int nStatus;
  struct spi_ioc_transfer xfer[2];
  char rchTXData[2];
  char rchRXData[2];
  char rch[22];

  // for register 0
  memset(xfer, 0, sizeof(xfer));

  rchTXData[0] = 0;
  rchTXData[1] = 0;
  rchRXData[0] = 0;
  rchRXData[1] = 0;

  rchTXData[1] |= BIT7; // Clamp Mode
  rchTXData[1] |= BIT5; // For CIS (0V to +V)

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = 2;
  xfer[0].delay_usecs = DELAY_USEC;

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  pApp->writeSTR("T:");
  sprintINT16(rch, rchTXData[0] | (rchTXData[1] << 8));
  pApp->writeSTR(rch);

  // for register 1
  memset(xfer, 0, sizeof(xfer));

  rchTXData[0] = 0;
  rchTXData[1] = 0;
  rchRXData[0] = 0;
  rchRXData[1] = 0;

  rchTXData[0] |= BIT0; // register 1
  rchTXData[1] |= BIT5; // test pattern
  rchTXData[1] |= BIT3; // 10 bit

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = 2;
  xfer[0].delay_usecs = DELAY_USEC;

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  pApp->writeSTR("T:");
  sprintINT16(rch, rchTXData[0] | (rchTXData[1] << 8));
  pApp->writeSTR(rch);

  return 0;
}
#endif

static float
getFetchDistance (NXCTRL_VOID) {
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

static NXCTRL_VOID
traceA6 (LPNXCTRLAPP pApp) {
  register int i = 0, j;
  int rnADC[CHART_WIDTH];
  
  pApp->clearDisplay();
  pApp->setCursor(0, 0);
  pApp->writeSTR("Y");
  pApp->drawLine(8, 2, 8, 2+CHART_HEIGHT, NXCTRL_ON);
  pApp->drawLine(8, CHART_HEIGHT+2, 8+CHART_WIDTH, 2+CHART_HEIGHT, NXCTRL_ON);
  pApp->setCursor(8+CHART_WIDTH+2, CHART_HEIGHT);
  pApp->writeSTR("X");
  pApp->updateDisplay();

  pApp->setCursor(0, 7*FONT_HEIGHT+1);
  pApp->writeSTR(" PRESS MENU TO STOP");
  pApp->updateDisplay();

  MENU_U_BUTTON_STATE = NXCTRL_LOW;
  MENU_D_BUTTON_STATE = NXCTRL_LOW;

  while (1) {
    int nADCRaw = pApp->analogRead(NXCTRL_A6);
    int nADC = (int)((nADCRaw / 4095.0) * CHART_HEIGHT);

    if (nADC >= (CHART_HEIGHT*4.2/5)) nADC = CHART_HEIGHT*4.2/5;
    if (nADC <= (CHART_HEIGHT*1.6/5)) nADC = CHART_HEIGHT*1.6/5;

    nADC = (nADC - (CHART_HEIGHT*1.6/5)) / ((CHART_HEIGHT/5)*(4.2 - 1.6)) * CHART_HEIGHT;

    pApp->analogWrite(PWM1_BANK, PWM1_PIN, pow(1000, nADC*1.0/CHART_HEIGHT));
    
    if (i >= CHART_WIDTH) {
      i = CHART_WIDTH;

      MENU_U_BUTTON_STATE = pApp->digitalRead(MENU_U_BUTTON_BANK, MENU_U_BUTTON_PIN);
      if (MENU_U_BUTTON_STATE == NXCTRL_HIGH) { break; }
      MENU_D_BUTTON_STATE = pApp->digitalRead(MENU_D_BUTTON_BANK, MENU_D_BUTTON_PIN);
      if (MENU_D_BUTTON_STATE == NXCTRL_HIGH) { break; }
      
      for (j = 0; j < CHART_WIDTH - 1; j++) {
        if (rnADC[j])
          pApp->drawPixel(8 + 1 + j, CHART_HEIGHT - rnADC[j] + 2, NXCTRL_OFF);
        rnADC[j] = rnADC[j + 1];
        pApp->drawPixel(8 + 1 + j, CHART_HEIGHT - rnADC[j] + 2, NXCTRL_ON);
      }
      if (rnADC[j])
        pApp->drawPixel(8 + 1 + j, CHART_HEIGHT - rnADC[j] + 2, NXCTRL_OFF);
      rnADC[j] = nADC;
      pApp->drawPixel(8 + 1 + j, CHART_HEIGHT - rnADC[j] + 2, NXCTRL_ON);
      pApp->updateDisplay();
    } else {
      rnADC[i] = nADC;
      //pApp->drawLine(8 + 1 + i, CHART_HEIGHT - nADC + 2,
      //               8 + 1 + i, 2 + CHART_HEIGHT, NXCTRL_ON);
      pApp->drawPixel(8 + 1 + i, CHART_HEIGHT - rnADC[i] + 2, NXCTRL_ON);
      pApp->updateDisplay();
    }

    pApp->sleep(20, 0);
    i++;
  }

  pApp->setCursor(0, 7*FONT_HEIGHT+1);
  pApp->writeSTR(" PRESS EXEC TO EXIT");
  pApp->updateDisplay();

  pApp->sleep(1000, 0);

  EXEC_BUTTON_STATE = NXCTRL_LOW;
  while (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    pApp->sleep(100, 0);
    EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  }

  pApp->analogWrite(PWM1_BANK, PWM1_PIN, 0);
  pApp->sleep(100, 0);
}

#if 0
static NXCTRL_VOID
dcMotorTest (LPNXCTRLAPP pApp) {
  pApp->clearDisplay();
  pApp->setCursor(0, 3*FONT_HEIGHT);
  pApp->writeSTR("    DC MOTOR DRV");
  pApp->setCursor(0, 4*FONT_HEIGHT + 2);
  pApp->writeSTR("    H-BRIGDE DRV");
  pApp->updateDisplay();

  pApp->digitalWrite(DCBANK, DCCTRL1, NXCTRL_HIGH);
  pApp->digitalWrite(DCBANK, DCCTRL2, NXCTRL_LOW);
  pApp->analogWrite(DCBANK, DCENABLE, 540);
  pApp->sleep(3000, 0);

  pApp->digitalWrite(DCBANK, DCCTRL1, NXCTRL_LOW);
  pApp->digitalWrite(DCBANK, DCCTRL2, NXCTRL_LOW);
  pApp->analogWrite(DCBANK, DCENABLE, 0);
  pApp->sleep(1000, 0);

  pApp->digitalWrite(DCBANK, DCCTRL1, NXCTRL_LOW);
  pApp->digitalWrite(DCBANK, DCCTRL2, NXCTRL_HIGH);
  pApp->analogWrite(DCBANK, DCENABLE, 540);
  pApp->sleep(3000, 0);

  pApp->digitalWrite(DCBANK, DCCTRL1, NXCTRL_LOW);
  pApp->digitalWrite(DCBANK, DCCTRL2, NXCTRL_LOW);
  pApp->analogWrite(DCBANK, DCENABLE, 0);
  pApp->sleep(1000, 0);
}
#endif

static NXCTRL_VOID
pinInfo (LPNXCTRLAPP pApp) {
  pApp->clearDisplay();
  pApp->setCursor(0, 0);
  pApp->writeSTR("P9[1|3|5|7]  G|3|D5|5");
  pApp->setCursor(0, 10);
  pApp->writeSTR("P9[32|33|34]  VA|A4|G");
  pApp->setCursor(0, 20);
  pApp->writeSTR("P9[35|36]        A6|5");
  pApp->setCursor(0, 30);
  pApp->writeSTR("P9[37-40]    A2|3|0|1");
  pApp->setCursor(0, 40);
  pApp->writeSTR("P8[3-6|20-25]    EMMC");
  pApp->updateDisplay();

  pApp->sleep(2000, 0);

  pApp->setCursor(9, 7*FONT_HEIGHT+1);
  pApp->writeSTR("PRESS EXEC TO EXIT");
  pApp->updateDisplay();

  EXEC_BUTTON_STATE = NXCTRL_LOW;
  while (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    pApp->sleep(100, 0);
    EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  }
}

static NXCTRL_VOID
setTimeInUTC (const char *pszDatePart, const char *pszTimePart) {
  struct tm tm;
  struct timeval tv;
  char rch[3], rchTZ[128];
  char *pszOLDTZ;
  time_t tmNew;
  extern int setenv (const char *__name, const char *__value, int __replace);
  extern int unsetenv (const char *__name);
  extern int settimeofday (const struct timeval *tv, void *pIgnoreThis);

  snprintf(rch, 3, "%s", pszDatePart);
  tm.tm_mday = atoi(rch);
  snprintf(rch, 3, "%s", pszDatePart + 2);
  tm.tm_mon = atoi(rch) - 1;
  snprintf(rch, 3, "%s", pszDatePart + 4);
  tm.tm_year = 2000 + atoi(rch) - 1900;

  snprintf(rch, 3, pszTimePart);
  tm.tm_hour = atoi(rch);
  snprintf(rch, 3, pszTimePart + 2);
  tm.tm_min = atoi(rch);
  snprintf(rch, 3, pszTimePart + 4);
  tm.tm_sec = atoi(rch);

  pszOLDTZ = getenv("TZ");
  if (pszOLDTZ)
    sprintf(rchTZ, "%s", pszOLDTZ);
  
  if (setenv("TZ", "UTC", 1) < 0)
    return;

  tmNew = mktime(&tm);

  tv.tv_sec = tmNew;
  tv.tv_usec = 0;

  if (settimeofday(&tv, NULL) < 0) {
    if (pszOLDTZ)
      setenv("TZ", rchTZ, 1);
    else
      unsetenv("TZ");
    return;
  }

  if (pszOLDTZ)
    setenv("TZ", rchTZ, 1);
  else
    unsetenv("TZ");

  return;
}

static NXCTRL_VOID
displayGPSInfo (LPNXCTRLAPP pApp) {
  struct termios tio;
  int fdTTY;
  char c;
  char rch[1024], *psz;
  char rchLine[26];
  int nTimeUpdateCount = -1;
  
  pApp->clearDisplay();
  pApp->setCursor(0, 0);
  pApp->writeSTR("   GPS INFORMATION");
  pApp->updateDisplay();

  pApp->sleep(500, 0);

  memset(&tio, 0, sizeof(tio));
  tio.c_iflag = 0;
  tio.c_oflag = 0;
  tio.c_cflag = CS8 | CREAD | CLOCAL;
  tio.c_lflag = 0;
  tio.c_cc[VMIN] = 1;
  tio.c_cc[VTIME] = 5;
  if ((fdTTY = open(GPS_TTY, O_RDWR | O_NONBLOCK)) == -1) {
    fprintf(stderr, "cannot open GPS tty\n");
    return;
  }
  cfsetospeed(&tio, GPS_BAUDRATE);
  cfsetispeed(&tio, GPS_BAUDRATE);
  tcsetattr(fdTTY, TCSANOW, &tio);

  pApp->setCursor(0, 24);
  pApp->writeSTR("   GPS UNAVAILABLE");
  
  pApp->setCursor(2*FONT_WIDTH, 7*FONT_HEIGHT+1);
  pApp->writeSTR("HOLD EXEC TO EXIT");
  pApp->updateDisplay();

  rch[0] = '\0';
  psz = rch;

  EXEC_BUTTON_STATE = NXCTRL_LOW;
  while (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    if (read(fdTTY, &c, 1) > 0) {
      if (c == '\r' || c == '\n') {
        *psz = '\0';
        snprintf(rchLine, 25, "%s", rch);
        rchLine[25] = '\0';
        if (!strncasecmp("$GPGGA", rchLine, 6)) {
#if 0
          pApp->setCursor(0, 14);
          pApp->writeSTR(rchLine+4);
          pApp->updateDisplay();
#endif
        } else if (!strncasecmp("$GPGSA", rchLine, 6)) {
#if 0
          pApp->setCursor(0, 24);
          pApp->writeSTR(rchLine+4);
          pApp->updateDisplay();
#endif
        } else if (!strncasecmp("$GPRMC", rchLine, 6)) {
#if 0
          pApp->setCursor(0, 34);
          pApp->writeSTR(rchLine+4);
          pApp->updateDisplay();
#else
          char *token;
          char *pszSep = ", \r\n";
          char rchTime[7];
          // $GPRMC
          token = strtok(rch, pszSep);
          // time
          token = strtok(NULL, pszSep);
          snprintf(rchTime, 7, "%s", token);
          rchTime[6] = 0;
          // status code 'A' for valid data
          token = strtok(NULL, pszSep);
          if (strlen(token) && (token[0] == 'A')) {
            pApp->setCursor(0, 24);
            pApp->writeSTR("                    ");
            pApp->setCursor(7*FONT_WIDTH, 34);
            pApp->writeSTR("                    ");
            // pos
            token = strtok(NULL, pszSep);
            pApp->setCursor(0, 24);
            pApp->writeSTR("LAT: ");
            pApp->writeSTR(token);
            token = strtok(NULL, pszSep);
            pApp->writeSTR(token);
            token = strtok(NULL, pszSep);
            pApp->setCursor(0, 34);
            pApp->writeSTR("LON: ");
            pApp->writeSTR(token);
            token = strtok(NULL, pszSep);
            pApp->writeSTR(token);
            // ground speed & direction
            token = strtok(NULL, pszSep);
            pApp->setCursor(0, 44);
            pApp->writeSTR("SPD: ");
            pApp->writeSTR(token);
            pApp->writeSTR(" KTS ");
            token = strtok(NULL, pszSep);
            pApp->writeSTR(token);
            // date
            token = strtok(NULL, pszSep);
            pApp->setCursor(0, 14);
            pApp->writeSTR("ITM: ");
            pApp->writeSTR(token);
            pApp->writeSTR(" ");
            pApp->writeSTR(rchTime);
            if (nTimeUpdateCount < 0 || nTimeUpdateCount >= 100) {
              nTimeUpdateCount = 0;
              setTimeInUTC(token, rchTime);
            } else
              nTimeUpdateCount++;
          } else {
            if (strlen(rchTime)) {
              token = strtok(NULL, pszSep);
              token = strtok(NULL, pszSep);
              // date
              token = strtok(NULL, pszSep);
              pApp->setCursor(0, 24);
              pApp->writeSTR("  SEARCHING SATS...");
              pApp->setCursor(7*FONT_WIDTH, 34);
              pApp->writeSTR(rchTime);
              if (nTimeUpdateCount < 0 || nTimeUpdateCount >= 100) {
                nTimeUpdateCount = 0;
                setTimeInUTC(token, rchTime);
              } else
                nTimeUpdateCount++;
            } else {
              pApp->setCursor(0, 24);
              pApp->writeSTR("   GPS UNAVAILABLE");
            }
          }
          pApp->updateDisplay();
#endif
        } else if (!strncasecmp("$GPVTG", rchLine, 6)) {
#if 0
          pApp->setCursor(0, 44);
          pApp->writeSTR(rchLine+4);
          pApp->updateDisplay();
#endif
          pApp->sleep(500, 0);
        }
        rch[0] = '\0';
        psz = rch;
      } else {
        *psz++ = c;
        if (strlen(rch) > 256) {
          rch[0] = '\0';
          psz = rch;
        }
      }
    }
    EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  }

  close(fdTTY);
}

static NXCTRL_VOID
displayPeriInfo (LPNXCTRLAPP pApp) {
  register int i, n = HCSR04_MAX_CNT;
  float fs = 0;
  char rch[22];

  pApp->clearDisplay();
  pApp->setCursor(5*FONT_WIDTH, 0);
  pApp->writeSTR("PERIPHERAL\n");

  pApp->setCursor(0, FONT_HEIGHT + 8);

  for (i = 0; i < n; i++) {
    fs += getFetchDistance();
  }
  fs /= n;
  
  if (fs > HCSR04_MAX_DIST)
    sprintf(rch, "DIST(HCSR04): >%1.1fm\n", (HCSR04_MAX_DIST/100.0));
  else
    sprintf(rch, "DIST(HCSR04): %2.1fcm\n", fs);
  pApp->writeSTR(rch);

  sprintf(rch, "A0: %04d/4095\n", pApp->analogRead(NXCTRL_A0));
  pApp->writeSTR(rch);
  sprintf(rch, "A1: %04d/4095 (T)\n", pApp->analogRead(NXCTRL_A1));
  pApp->writeSTR(rch);
  sprintf(rch, "A2: %04d/4095\n", pApp->analogRead(NXCTRL_A2));
  pApp->writeSTR(rch);
  sprintf(rch, "A3: %04d/4095\n", pApp->analogRead(NXCTRL_A3));
  pApp->writeSTR(rch);

  pApp->updateDisplay();
}

static NXCTRL_VOID
runPWM1 (LPNXCTRLAPP pApp) {
  int i;
  int PWM_RES = 1000;
  int PULSE_RES = 200;
  int PULSE_TM = 8;
  float fLogPulse = log(PULSE_RES);
  
  pApp->clearDisplay();
  pApp->setCursor(0, 3*FONT_HEIGHT);
  pApp->writeSTR("    PWM ON P8:13");
  pApp->setCursor(0, 4*FONT_HEIGHT + 2);
  pApp->writeSTR("    PULSING LED");
  pApp->setCursor(0, 7*FONT_HEIGHT+1);
  pApp->writeSTR(" PRESS MENU TO STOP");
  pApp->updateDisplay();

  MENU_U_BUTTON_STATE = NXCTRL_LOW;
  MENU_D_BUTTON_STATE = NXCTRL_LOW;

  while(1) {
    for (i = PULSE_RES; i > 1; i--) {
      float logF = log(i) / fLogPulse;
      int nDC = (int)((1 - logF) * PWM_RES);
      pApp->analogWrite(PWM1_BANK, PWM1_PIN, nDC);
      pApp->sleep(PULSE_TM, 0);
    }
    for (i = 0; i < PULSE_RES; i++) {
      float logF = log(i+1) / fLogPulse;
      int nDC = (int)((1 - logF) * PWM_RES);
      pApp->analogWrite(PWM1_BANK, PWM1_PIN, nDC);
      pApp->sleep(PULSE_TM, 0);
    }
    MENU_U_BUTTON_STATE = pApp->digitalRead(MENU_U_BUTTON_BANK, MENU_U_BUTTON_PIN);
    if (MENU_U_BUTTON_STATE == NXCTRL_HIGH) { break; }
    MENU_D_BUTTON_STATE = pApp->digitalRead(MENU_D_BUTTON_BANK, MENU_D_BUTTON_PIN);
    if (MENU_D_BUTTON_STATE == NXCTRL_HIGH) { break; }
  }
  pApp->analogWrite(PWM1_BANK, PWM1_PIN, 0);
  pApp->sleep(100, 0);

  pApp->setCursor(0, 7*FONT_HEIGHT+1);
  pApp->writeSTR(" PRESS EXEC TO EXIT");
  pApp->updateDisplay();

  pApp->sleep(1000, 0);

  EXEC_BUTTON_STATE = NXCTRL_LOW;
  while (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    pApp->sleep(100, 0);
    EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  }
}

#if 0
static NXCTRL_VOID
runPWM2 (LPNXCTRLAPP pApp) {
  int i;
  
  pApp->clearDisplay();
  pApp->setCursor(0, 3*FONT_HEIGHT);
  pApp->writeSTR("    PWM ON P8:19");
  pApp->setCursor(0, 4*FONT_HEIGHT + 2);
  pApp->writeSTR("    SERVO CONTROL");
  pApp->updateDisplay();

  pApp->servoWrite(PWM2_BANK, PWM2_PIN, 82);
  pApp->sleep(800, 0);
  pApp->servoWrite(PWM2_BANK, PWM2_PIN, 30);
  pApp->sleep(800, 0);
  pApp->servoWrite(PWM2_BANK, PWM2_PIN, 150);
  pApp->sleep(800, 0);
  
  pApp->servoWrite(PWM2_BANK, PWM2_PIN, 0);
  pApp->sleep(500, 0);

  for (i = 0; i <= 180; i += 2) {
    pApp->servoWrite(PWM2_BANK, PWM2_PIN, i);
    pApp->sleep(20, 0);
  }

  pApp->servoWrite(PWM2_BANK, PWM2_PIN, 0);
  pApp->sleep(800, 0);
  pApp->servoWrite(PWM2_BANK, PWM2_PIN, 180);
  pApp->sleep(800, 0);
  pApp->servoWrite(PWM2_BANK, PWM2_PIN, 82);
  pApp->sleep(800, 0);

#if 0
  pApp->servoWrite(PWM2_BANK, PWM2_PIN, 0);
  pApp->sleep(500, 0);
#endif
}
#endif

#if 0
static NXCTRL_VOID
readAK8448Pins (LPNXCTRLAPP pApp, NXCTRL_BOOL rbRes[10]) {
  rbRes[0] = pApp->digitalRead(AKBANK, AKPIN0);
  rbRes[1] = pApp->digitalRead(AKBANK, AKPIN1);
  rbRes[2] = pApp->digitalRead(AKBANK, AKPIN2);
  rbRes[3] = pApp->digitalRead(AKBANK, AKPIN3);
  rbRes[4] = pApp->digitalRead(AKBANK, AKPIN4);
  rbRes[5] = pApp->digitalRead(AKBANK, AKPIN5);
  rbRes[6] = pApp->digitalRead(AKBANK, AKPIN6);
  rbRes[7] = pApp->digitalRead(AKBANK, AKPIN7);
  rbRes[8] = pApp->digitalRead(AKBANK, AKPIN8);
  rbRes[9] = pApp->digitalRead(AKBANK, AKPIN9);
}
#endif

#if 0
static NXCTRL_VOID
readAK8448 (LPNXCTRLAPP pApp) {
  register int i, j;
  NXCTRL_BOOL rbRes[10];

  pApp->clearDisplay();

  for (j = 0; j < 6; j++) {
    pApp->setCursor(0, j*FONT_HEIGHT);

    pApp->digitalWrite(AKBANK, AKADCK, NXCTRL_LOW);
    pApp->sleep(50, 0);
    pApp->digitalWrite(AKBANK, AKADCK, NXCTRL_HIGH);
    pApp->sleep(50, 0);
    readAK8448Pins(pApp, rbRes);

    for (i = 0; i < 10; i++)
      pApp->writeSTR(rbRes[i] ? "1" : "0");
    pApp->writeSTR(" ");
    pApp->updateDisplay();
    
    pApp->digitalWrite(AKBANK, AKADCK, NXCTRL_HIGH);
    pApp->sleep(50, 0);
    pApp->digitalWrite(AKBANK, AKADCK, NXCTRL_LOW);
    pApp->sleep(50, 0);
    readAK8448Pins(pApp, rbRes);

    for (i = 0; i < 10; i++)
      pApp->writeSTR(rbRes[i] ? "1" : "0");
    pApp->updateDisplay();
  }

  pApp->sleep(2000, 0);

  pApp->setCursor(0, 7*FONT_HEIGHT+1);
  pApp->writeSTR(" PRESS EXEC TO EXIT");
  pApp->updateDisplay();

  EXEC_BUTTON_STATE = NXCTRL_LOW;
  while (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    pApp->sleep(100, 0);
    EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  }
}
#endif

#if 0
static NXCTRL_VOID
runAK8448 (LPNXCTRLAPP pApp) {
  uint8_t nLSB;
  uint32_t nSpeed, nSPIMode;
  int nFD = open(SPI_DEV, O_RDWR);
  
  nLSB = 0;
  ioctl(nFD, SPI_IOC_WR_LSB_FIRST, &nLSB);
  nSpeed = 10000000;
  ioctl(nFD, SPI_IOC_WR_MAX_SPEED_HZ, &nSpeed);
  nSPIMode = SPI_MODE_3;
  ioctl(nFD, SPI_IOC_WR_MODE, &nSPIMode);

  pApp->clearDisplay();
  pApp->setCursor(5*FONT_WIDTH, 0);

  pApp->writeSTR("AK8448 CFG\n\n");

  __SPI_write(pApp, nFD);
  __SPI_read(pApp, nFD);
  
  close(nFD);

  pApp->updateDisplay();

  pApp->sleep(2000, 0);

  pApp->setCursor(0, 7*FONT_HEIGHT+1);
  pApp->writeSTR(" PRESS EXEC TO EXIT");
  pApp->updateDisplay();

  EXEC_BUTTON_STATE = NXCTRL_LOW;
  while (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    pApp->sleep(100, 0);
    EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  }
}
#endif

static NXCTRL_BOOL
canAction (NXCTRL_VOID) {
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

static NXCTRL_VOID
updateMenuButtonState (LPNXCTRLAPP pApp) {
  if (MENU_U_BUTTON_STATE == NXCTRL_LOW) {
    if (pApp->digitalRead(MENU_U_BUTTON_BANK, MENU_U_BUTTON_PIN) == NXCTRL_HIGH) {
      MENU_U_BUTTON_STATE = NXCTRL_HIGH;
      DPY_IDLE_COUNT = 0;
    }
  } else {
    if (pApp->digitalRead(MENU_U_BUTTON_BANK, MENU_U_BUTTON_PIN) == NXCTRL_LOW) {
      MENU_U_BUTTON_STATE = NXCTRL_LOW;
      DPY_IDLE_COUNT = 0;
    }
  }

  if (MENU_D_BUTTON_STATE == NXCTRL_LOW) {
    if (pApp->digitalRead(MENU_D_BUTTON_BANK, MENU_D_BUTTON_PIN) == NXCTRL_HIGH) {
      MENU_D_BUTTON_STATE = NXCTRL_HIGH;
      DPY_IDLE_COUNT = 0;
    }
  } else {
    if (pApp->digitalRead(MENU_D_BUTTON_BANK, MENU_D_BUTTON_PIN) == NXCTRL_LOW) {
      MENU_D_BUTTON_STATE = NXCTRL_LOW;
      DPY_IDLE_COUNT = 0;
    }
  }
}

static NXCTRL_VOID
updateExecButtonState (LPNXCTRLAPP pApp) {
  if (EXEC_BUTTON_STATE == NXCTRL_LOW) {
    if (pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN) == NXCTRL_HIGH) {
      EXEC_BUTTON_STATE = NXCTRL_HIGH;
      DPY_IDLE_COUNT = 0;
    }
  } else {
    if (pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN) == NXCTRL_LOW) {
      EXEC_BUTTON_STATE = NXCTRL_LOW;
      DPY_IDLE_COUNT = 0;
    }
  }
}

static char *
mkMenuSTR (char *rch, const char *pszName, int nMenu) {
  sprintf(rch, "%c %s\n",
          (MENU_IDX == nMenu ? MENU_SEL_CHAR : ' '),
          pszName);
  return rch;
}

static NXCTRL_VOID
displayMenu (LPNXCTRLAPP pApp) {
  char rch[21];

  pApp->clearDisplay();

  pApp->setCursor(0, 0);
  pApp->writeSTR("PERIPHERAL");
  pApp->drawLine(61, 6, 127, 6, NXCTRL_ON);
  pApp->setCursor(0, 16);

  if (MENU_IDX < 5)
    pApp->writeSTR(mkMenuSTR(rch, "TC>>", MENU_IDX_SYSTEM_MENU));
  if (MENU_IDX < 6)
    pApp->writeSTR(mkMenuSTR(rch, "UPDATE INFO", MENU_IDX_UPDATE_MENU));
  if (MENU_IDX < 7)
    pApp->writeSTR(mkMenuSTR(rch, "GPS INFO", MENU_IDX_GPS_MENU));
  if (MENU_IDX < 8)
    pApp->writeSTR(mkMenuSTR(rch, "LED PULSE", MENU_IDX_P8_13_PWM_MENU));
  //if (MENU_IDX < 9)
  //  pApp->writeSTR(mkMenuSTR(rch, "P8:19 PWM(SERVO)", MENU_IDX_P8_19_PWM_MENU));
  //if (MENU_IDX < 10)
  //  pApp->writeSTR(mkMenuSTR(rch, "SPIDEV:2(AK8448)", MENU_IDX_AK8448_CFG_MENU));
  //if (MENU_IDX >= 6)
  //  pApp->writeSTR(mkMenuSTR(rch, "AK8448 TEST", MENU_IDX_AK8448_READ_MENU));
  //if (MENU_IDX >= 7)
  pApp->writeSTR(mkMenuSTR(rch, "LIGHT TRACE", MENU_IDX_TR_A3_MENU));
  //if (MENU_IDX >= 8)
  //  pApp->writeSTR(mkMenuSTR(rch, "DC MOTOR DRV", MENU_IDX_DCMOTOR_MENU));
  if (MENU_IDX >= 5)
    pApp->writeSTR(mkMenuSTR(rch, "IR TEST", MENU_IDX_IR_MENU));
  if (MENU_IDX >= 6)
    pApp->writeSTR(mkMenuSTR(rch, "PIN INFO", MENU_IDX_PININFO_MENU));
  if (MENU_IDX >= 7)
    pApp->writeSTR(mkMenuSTR(rch, "EXIT MENU", MENU_IDX_EXIT_MENU));

  pApp->updateDisplay();
}

NXCTRL_VOID
NXCTRLAPP_init (LPNXCTRLAPP pApp) {
  pApp->pinMux(PWM1_BANK, PWM1_PIN, NXCTRL_MODE4, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(PWM2_BANK, PWM2_PIN, NXCTRL_MODE4, NXCTRL_PULLDN, NXCTRL_LOW);
  
  pApp->pinMux(NXCTRL_P9, SPI_CS0, NXCTRL_MODE3, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(NXCTRL_P9, SPI_D1, NXCTRL_MODE3, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(NXCTRL_P9, SPI_D0, NXCTRL_MODE3, NXCTRL_PULLUP, NXCTRL_HIGH);
  pApp->pinMux(NXCTRL_P9, SPI_CLK, NXCTRL_MODE3, NXCTRL_PULLUP, NXCTRL_HIGH);

  pApp->pinMux(AKBANK, AKPIN0, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN1, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN2, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN3, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN4, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN5, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN6, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN7, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN8, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKPIN9, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKCLK, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(AKBANK, AKADCK, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);

  pApp->pinMode(AKBANK, AKPIN0, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN1, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN2, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN3, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN4, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN5, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN6, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN7, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN8, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKPIN9, NXCTRL_INPUT);
  pApp->pinMode(AKBANK, AKCLK, NXCTRL_OUTPUT);
  pApp->pinMode(AKBANK, AKADCK, NXCTRL_OUTPUT);

  pApp->pinMux(DCBANK, DCCTRL1, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  pApp->pinMux(DCBANK, DCCTRL2, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_LOW);
  //pApp->pinMux(DCBANK, DCENABLE, NXCTRL_MODE4, NXCTRL_PULLDN, NXCTRL_LOW); // same as PWM2

  pApp->pinMode(DCBANK, DCCTRL1, NXCTRL_OUTPUT);
  pApp->pinMode(DCBANK, DCCTRL2, NXCTRL_OUTPUT);

  MENU_U_BUTTON_STATE = pApp->digitalRead(MENU_U_BUTTON_BANK, MENU_U_BUTTON_PIN);
  MENU_D_BUTTON_STATE = pApp->digitalRead(MENU_D_BUTTON_BANK, MENU_D_BUTTON_PIN);
  EXEC_BUTTON_STATE = pApp->digitalRead(EXEC_BUTTON_BANK, EXEC_BUTTON_PIN);
  DPY_IDLE_COUNT = 0;
  MENU_IDX = MENU_IDX_SYSTEM_MENU;
  IN_MENU = NXCTRL_FALSE;
  LAST_ACTION_TIME = 0;

  while (MENU_U_BUTTON_STATE == NXCTRL_HIGH) {
    pApp->sleep(100, 0);
    MENU_U_BUTTON_STATE = pApp->digitalRead(MENU_U_BUTTON_BANK, MENU_U_BUTTON_PIN);
  }

  while (MENU_D_BUTTON_STATE == NXCTRL_HIGH) {
    pApp->sleep(100, 0);
    MENU_D_BUTTON_STATE = pApp->digitalRead(MENU_D_BUTTON_BANK, MENU_D_BUTTON_PIN);
  }

  displayPeriInfo(pApp);
}

NXCTRL_VOID
NXCTRLAPP_clean (LPNXCTRLAPP pApp) {
}

NXCTRL_VOID
NXCTRLAPP_run (LPNXCTRLAPP pApp) {
  updateMenuButtonState(pApp);
  updateExecButtonState(pApp);

  if (MENU_U_BUTTON_STATE != NXCTRL_HIGH && MENU_D_BUTTON_STATE != NXCTRL_HIGH &&
      EXEC_BUTTON_STATE != NXCTRL_HIGH) {
    DPY_IDLE_COUNT++;
    if (DPY_IDLE_COUNT > DPY_IDLE_COUNT_MAX) {
      pApp->nCmd = 2;
      return;
    }
    return;
  }

  if (MENU_U_BUTTON_STATE == NXCTRL_ON || MENU_D_BUTTON_STATE == NXCTRL_ON) {
    if (IN_MENU) {
      if (canAction()) {
        if (MENU_D_BUTTON_STATE == NXCTRL_ON) {
          if (MENU_IDX < MENU_IDX_COUNT - 1)
            MENU_IDX++;
          else
            MENU_IDX = 0;
        } else {
          if (MENU_IDX > 0)
            MENU_IDX--;
          else
            MENU_IDX = MENU_IDX_COUNT - 1;
        }
        displayMenu(pApp);
      }
    } else {
      IN_MENU = NXCTRL_TRUE;
      displayMenu(pApp);
      canAction();
    }
  }

  if (EXEC_BUTTON_STATE == NXCTRL_ON) {
    if (IN_MENU) {
      if (canAction()) {
        switch (MENU_IDX) {
        case MENU_IDX_EXIT_MENU:
          IN_MENU = NXCTRL_FALSE;
          displayPeriInfo(pApp);
          break;
        case MENU_IDX_SYSTEM_MENU:
          pApp->nCmd = 1;
          return;
        case MENU_IDX_UPDATE_MENU:
          IN_MENU = NXCTRL_FALSE;
          displayPeriInfo(pApp);
          break;
        case MENU_IDX_P8_13_PWM_MENU:
          IN_MENU = NXCTRL_FALSE;
          runPWM1(pApp);
          displayPeriInfo(pApp);
          break;
#if 0
        case MENU_IDX_P8_19_PWM_MENU:
          IN_MENU = NXCTRL_FALSE;
          runPWM2(pApp);
          displayPeriInfo(pApp);
          break;
        case MENU_IDX_AK8448_CFG_MENU:
          IN_MENU = NXCTRL_FALSE;
          runAK8448(pApp);
          displayPeriInfo(pApp);
          break;
        case MENU_IDX_AK8448_READ_MENU:
          IN_MENU = NXCTRL_FALSE;
          readAK8448(pApp);
          displayPeriInfo(pApp);
          break;
#endif
        case MENU_IDX_TR_A3_MENU:
          IN_MENU = NXCTRL_FALSE;
          traceA6(pApp);
          displayPeriInfo(pApp);
          break;
#if 0
        case MENU_IDX_DCMOTOR_MENU:
          IN_MENU = NXCTRL_FALSE;
          dcMotorTest(pApp);
          displayPeriInfo(pApp);
          break;
#endif
        case MENU_IDX_PININFO_MENU:
          IN_MENU = NXCTRL_FALSE;
          pinInfo(pApp);
          displayPeriInfo(pApp);
          break;
        case MENU_IDX_GPS_MENU:
          IN_MENU = NXCTRL_FALSE;
          displayGPSInfo(pApp);
          displayPeriInfo(pApp);
          break;
        case MENU_IDX_IR_MENU:
          IN_MENU = NXCTRL_FALSE;
          showIR(pApp);
          displayPeriInfo(pApp);
          break;
        default:
          break;
        }
      }
    }
  }
}
