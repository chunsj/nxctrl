/*
 * NXCTRL BeagleBone Black Control Library
 *
 * SSD1306 OLED SPI Test Program - Big Bitmap
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
#include <NXCTRL.h>
#include <NXCTRL_oled.h>
#include <NXCTRL_bitArray.h>

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <assert.h>
#include <termios.h>
#include <sys/stat.h>
#include <libnsbmp.h>

#define BNK               NXCTRL_P9

#define SPI_CS0           17
#define SPI_D0            21
#define SPI_D1            18
#define SPI_CLK           22

#define OLED_DC           15
#define OLED_RST          13
#define OLED_CS           SPI_CS0
#define OLED_DATA         SPI_D1
#define OLED_CLK          SPI_CLK

void *createBitmap (int nWidth, int nHeight, unsigned int nState);
void destroyBitmap (void *pBitmap);
unsigned char *getBitmapBuffer (void *pBitmap);
size_t getBitmapBPP (void *pBitmap);

unsigned char *loadBitmapFile (const char *pszPath, size_t *pnSize);
void warning (const char *pszCtx, bmp_result nCode);

NXCTRLOLED oled;
int nFD;

int BIG_BITMAP_W = 0;
int BIG_BITMAP_H = 0;

NXCTRLBITARRAY arrBits;

#define SCROLL_STEP_DIV 4

NXCTRL_VOID
__DrawBigBitmap (int nX, int nY) {
  int i, j;
  
  if ((nX + OLED_WIDTH) > BIG_BITMAP_W)
    nX = BIG_BITMAP_W - OLED_WIDTH - 1;
  if ((nY + OLED_HEIGHT) > BIG_BITMAP_H)
    nY = BIG_BITMAP_H - OLED_HEIGHT - 1;
  
  for (i = nX; i < (nX + OLED_WIDTH); i++) {
    for (j = nY; j < (nY + OLED_HEIGHT); j++) {
      if (NXCTRLBITARRAYGet(&arrBits, i + j * BIG_BITMAP_W))
        //if (bigBitmap[i + j*BIG_BITMAP_W] > 0)
        NXCTRLOLEDDrawPixel(&oled, (i - nX), (j - nY), NXCTRL_ON);
      else
        NXCTRLOLEDDrawPixel(&oled, (i - nX), (j - nY), NXCTRL_OFF);
    }
  }
}

struct termios tioOld, tioNew;

NXCTRL_VOID
INTC_HANDLER (NXCTRL_VOID) {
    NXCTRLOLEDClearDisplay(&oled);
    NXCTRLOLEDUpdateDisplay(&oled);
    tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
    close(nFD);
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  uint8_t nLSB;
  uint32_t nSpeed, nSPIMode;

  NXCTRLPinMux(BNK, SPI_CS0, NXCTRL_MODE0, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(BNK, SPI_D1, NXCTRL_MODE0, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(BNK, SPI_D0, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);
  NXCTRLPinMux(BNK, SPI_CLK, NXCTRL_MODE0, NXCTRL_PULLUP, NXCTRL_HIGH);

  nFD = open("/dev/spidev1.0", O_RDWR);

  nLSB = 0;
  ioctl(nFD, SPI_IOC_WR_LSB_FIRST, &nLSB);
  nSpeed = 20000000;
  ioctl(nFD, SPI_IOC_WR_MAX_SPEED_HZ, &nSpeed);
  nSPIMode = SPI_MODE_0;
  ioctl(nFD, SPI_IOC_WR_MODE, &nSPIMode);

  NXCTRLOLEDInit(&oled,
                 BNK, OLED_DC, BNK, OLED_RST,
                 nFD);

  NXCTRLOLEDDisplayNormal(&oled);
  NXCTRLOLEDClearDisplay(&oled);
  NXCTRLOLEDUpdateDisplay(&oled);

  tcgetattr(STDIN_FILENO, &tioOld);
  
  tioNew = tioOld;
  tioNew.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &tioNew);

  NXCTRLSetSIGINTHandler(INTC_HANDLER);
}

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
  NXCTRL_UINT8 ch;
  static int nPageX = 0, nPageY = 0;
  const int nPageWidth = OLED_WIDTH/SCROLL_STEP_DIV;
  const int nPageHeight = OLED_HEIGHT/SCROLL_STEP_DIV;
  const int nPageW = BIG_BITMAP_W/nPageWidth;
  const int nPageH = BIG_BITMAP_H/nPageHeight;

  ch = getchar();

  switch (ch) {
  case 'q':
    NXCTRLOLEDClearDisplay(&oled);
    NXCTRLOLEDUpdateDisplay(&oled);
    tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
    close(nFD);
    NXCTRLExitLoop();
    return;
  case 'r':
    nPageX = nPageY = 0;
    __DrawBigBitmap(nPageX*nPageWidth, nPageY*nPageHeight);
    NXCTRLOLEDUpdateDisplay(&oled);
    break;
  case 'h':
    nPageX--;
    if (nPageX < 0) nPageX = 0;
    __DrawBigBitmap(nPageX*nPageWidth, nPageY*nPageHeight);
    NXCTRLOLEDUpdateDisplay(&oled);
    break;
  case 'j':
    nPageY++;
    if (nPageY >= nPageH) nPageY = nPageH - 1;
    __DrawBigBitmap(nPageX*nPageWidth, nPageY*nPageHeight);
    NXCTRLOLEDUpdateDisplay(&oled);
    break;
  case 'k':
    nPageY--;
    if (nPageY < 0) nPageY = 0;
    __DrawBigBitmap(nPageX*nPageWidth, nPageY*nPageHeight);
    NXCTRLOLEDUpdateDisplay(&oled);
    break;
  case 'l':
    nPageX++;
    if (nPageX >= nPageW) nPageX = nPageW - 1;
    __DrawBigBitmap(nPageX*nPageWidth, nPageY*nPageHeight);
    NXCTRLOLEDUpdateDisplay(&oled);
    break;
  }
}

int
main (int argc, const char *argv[]) {
  bmp_bitmap_callback_vt bitmapCBs = {
    createBitmap,
    destroyBitmap,
    getBitmapBuffer,
    getBitmapBPP
  };
  bmp_result nCode;
  bmp_image bmp;
  size_t nSize;
  unsigned char *pData;
  int nRow, nCol;
  unsigned char *pImage;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s black_and_white.bmp\n", argv[0]);
    return 1;
  }

  pData = loadBitmapFile(argv[1], &nSize);

  bmp_create(&bmp, &bitmapCBs);
  nCode = bmp_analyse(&bmp, nSize, pData);
  if (nCode != BMP_OK) {
    warning("bmp_analyse", nCode);
    goto CLEANUP;
  }

  nCode = bmp_decode(&bmp);
  if (nCode != BMP_OK) {
    warning("bmp_code", nCode);
    if (nCode != BMP_INSUFFICIENT_DATA) {
      goto CLEANUP;
    }
  }

  BIG_BITMAP_W = bmp.width;
  BIG_BITMAP_H = bmp.height;

  NXCTRLBITARRAYInit(&arrBits, bmp.width * bmp.height);
  pImage = (unsigned char *)bmp.bitmap;
  for (nRow = 0; nRow != bmp.height; nRow++) {
    for (nCol = 0; nCol != bmp.width; nCol++) {
      size_t z = (nRow * bmp.width + nCol) * 4; // bytes per pixel
      if (pImage[z] > 0) // only B & W image
        NXCTRLBITARRAYSet(&arrBits, (nRow * bmp.width + nCol));
      else
        NXCTRLBITARRAYClear(&arrBits, (nRow * bmp.width + nCol));
    }
  }

CLEANUP:
  bmp_finalise(&bmp);
  NXCTRLBITARRAYFree(&arrBits);

  return NXCTRLMain();
}

unsigned char *
loadBitmapFile (const char *pszPath, size_t *pnDataSize) {
  FILE *pFile;
  struct stat st;
  unsigned char *pBuf;
  size_t nSize;
  size_t n;

  pFile = fopen(pszPath, "rb");
  if (!pFile) {
    perror(pszPath);
    exit(EXIT_FAILURE);
  }

  if (stat(pszPath, &st)) {
    perror(pszPath);
    exit(EXIT_FAILURE);
  }

  nSize = st.st_size;

  pBuf = malloc(nSize);
  if (!pBuf) {
    fprintf(stderr, "cannot allocate memory of size %lld\n",
            (long long int)nSize);
    exit(EXIT_FAILURE);
  }

  n = fread(pBuf, 1, nSize, pFile);
  if (n != nSize) {
    perror(pszPath);
    exit(EXIT_FAILURE);
  }

  fclose(pFile);

  *pnDataSize = nSize;

  return pBuf;
}

void
warning (const char *pszCtx, bmp_result nCode) {
  fprintf(stderr, "%s: ", pszCtx);
  switch (nCode) {
  case BMP_INSUFFICIENT_MEMORY:
    fprintf(stderr, "Insufficiant memory");
    break;
  case BMP_INSUFFICIENT_DATA:
    fprintf(stderr, "Insufficient data");
    break;
  case BMP_DATA_ERROR:
    fprintf(stderr, "Data error");
    break;
  default:
    fprintf(stderr, "Unknown code %d", nCode);
  }
  fprintf(stderr, "\n");
}

void *
createBitmap (int nW, int nH, unsigned int nState) {
  return calloc(nW * nH, 4); // bytes per pixel
}

unsigned char *
getBitmapBuffer (void *pBitmap) {
  assert(pBitmap);
  return pBitmap;
}

size_t
getBitmapBPP (void *pBitmap) {
  return 4; // bytes per pixel
}

void
destroyBitmap (void *pBitmap) {
  assert(pBitmap);
  free(pBitmap);
}
