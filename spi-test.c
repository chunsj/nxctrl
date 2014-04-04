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

#define SPI_CS0  17
#define SPI_D1   18
#define SPI_D0   21
#define SPI_CLK  22

const uint8_t INSTREAD  = 0b00000011;
const uint8_t INSTWRITE = 0b00000010;
const uint8_t INSTWREN  = 0b00000110;
const uint8_t INSTWRDI  = 0b00000100;
const uint8_t INSTRDSR  = 0b00000101;

static int
__SPI_read (int nFD, uint16_t nAddr, uint8_t *pnD) {
  int nStatus;
  struct spi_ioc_transfer xfer[2];
  char rchTXData[4];
  char rchRXData[4];
  
  memset(xfer, 0, sizeof(xfer));

  memcpy(rchTXData, &INSTREAD, sizeof(uint8_t));
  memcpy(rchTXData + 1, &nAddr, sizeof(uint16_t));
  memcpy(rchTXData + 1 + 2, pnD, sizeof(uint8_t));

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = 4;
  xfer[0].delay_usecs = 200;

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  *pnD = rchRXData[3];

  return 0;
}

static int
__SPI_write (int nFD, uint16_t nAddr, uint8_t nD) {
  int nStatus;
  struct spi_ioc_transfer xfer[2];
  char rchTXData[4];
  char rchRXData[4];

  memset(xfer, 0, sizeof(xfer));

  memcpy(rchTXData, &INSTWREN, sizeof(uint8_t));

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = 1;
  xfer[0].delay_usecs = 200;

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  memset(xfer, 0, sizeof(xfer));

  memcpy(rchTXData, &INSTWRITE, sizeof(uint8_t));
  memcpy(rchTXData + 1, &nAddr, sizeof(uint16_t));
  memcpy(rchTXData + 1 + 2, &nD, sizeof(uint8_t));

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = 4;
  xfer[0].delay_usecs = 200;

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }
  
  memset(xfer, 0, sizeof(xfer));

  memcpy(rchTXData, &INSTWRDI, sizeof(uint8_t));

  xfer[0].tx_buf = (unsigned int)rchTXData;
  xfer[0].rx_buf = (unsigned int)rchRXData;
  xfer[0].len = 1;
  xfer[0].delay_usecs = 200;

  nStatus = ioctl(nFD, SPI_IOC_MESSAGE(1), xfer);
  if (nStatus < 0) {
    perror("SPI_IOC_MESSAGE");
    return -1;
  }

  return 0;
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int nFD, i;
  uint8_t nLSB, nD;
  uint32_t nSpeed, nSPIMode;
  uint16_t nAddr;
  
  NXCTRLPinMux(NXCTRL_P9, SPI_CS0, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_LOW);
  NXCTRLPinMux(NXCTRL_P9, SPI_D1, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_LOW);
  NXCTRLPinMux(NXCTRL_P9, SPI_D0, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);
  NXCTRLPinMux(NXCTRL_P9, SPI_CLK, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);

  nFD = open("/dev/spidev1.0", O_RDWR);

  nLSB = 0;
  ioctl(nFD, SPI_IOC_WR_LSB_FIRST, &nLSB);
  nSpeed = 500000;
  ioctl(nFD, SPI_IOC_WR_MAX_SPEED_HZ, &nSpeed);
  nSPIMode = SPI_MODE_0;
  ioctl(nFD, SPI_IOC_WR_MODE, &nSPIMode);

  nAddr = 305;
  nD = -1;

  for (i = 0; i < 10; i++) {
    __SPI_read(nFD, nAddr, &nD);
    printf("%d: %d\n", i, nD);

    nD = (uint8_t)i;
    __SPI_write(nFD, nAddr, nD);

    NXCTRLSleep(5, 0);
  }
    
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
