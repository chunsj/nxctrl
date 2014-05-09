/*
 * NXCTRL BeagleBone Black Control Library
 *
 * SPI ADC(AK8448) Test Program
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
#include <NXCTRL.h>

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define SPI_CS0  17  // BLUE
#define SPI_D1   18  // RED
#define SPI_D0   21  // YELLOW
#define SPI_CLK  22  // GREEN

#define DELAY_USEC 200

static int
__SPI_read (int nFD) {
  int nStatus;
  struct spi_ioc_transfer xfer[2];
  char rchTXData[2];
  char rchRXData[2];

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

  printf("Press any key to continue..."); getchar();

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  printf("TX: "); printINT16(rchTXData[0] | (rchTXData[1] << 8));
  printf("RX: "); printINT16(rchRXData[0] | (rchRXData[1] << 8));

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

  printf("Press any key to continue..."); getchar();

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  printf("TX: "); printINT16(rchTXData[0] | (rchTXData[1] << 8));
  printf("RX: "); printINT16(rchRXData[0] | (rchRXData[1] << 8));

  return 0;
}

static int
__SPI_write (int nFD) {
  int nStatus;
  struct spi_ioc_transfer xfer[2];
  char rchTXData[2];
  char rchRXData[2];

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

  printf("Press any key to continue..."); getchar();

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  printf("TX: "); printINT16(rchTXData[0] | (rchTXData[1] << 8));
  printf("RX: "); printINT16(rchRXData[0] | (rchRXData[1] << 8));

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

  printf("Press any key to continue..."); getchar();

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  printf("TX: "); printINT16(rchTXData[0] | (rchTXData[1] << 8));
  printf("RX: "); printINT16(rchRXData[0] | (rchRXData[1] << 8));

  return 0;
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int nFD;
  uint8_t nLSB;
  uint32_t nSpeed, nSPIMode;
  
  NXCTRLPinMux(NXCTRL_P9, SPI_CS0, NXCTRL_MODE0, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(NXCTRL_P9, SPI_D1, NXCTRL_MODE0, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(NXCTRL_P9, SPI_D0, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);
  NXCTRLPinMux(NXCTRL_P9, SPI_CLK, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);

  nFD = open("/dev/spidev1.0", O_RDWR);

  nLSB = 0;
  ioctl(nFD, SPI_IOC_WR_LSB_FIRST, &nLSB);
  nSpeed = 1000000;
  ioctl(nFD, SPI_IOC_WR_MAX_SPEED_HZ, &nSpeed);
  nSPIMode = SPI_MODE_3;
  ioctl(nFD, SPI_IOC_WR_MODE, &nSPIMode);

  __SPI_write(nFD);
  __SPI_read(nFD);
  
  close(nFD);
}

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
