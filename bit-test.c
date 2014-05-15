#include <stdio.h>
#include <stdlib.h>
#include "NXCTRL_bits.h"

int
main (int argc, const char *argv[]) {
  short v16 = 0;

  printINT16(v16);
  //if (v16 & BIT0)
  if (onBIT16(v16, 0))
    printf("Have One at 0\n");

  //v16 |= BIT0;
  setBIT16(v16, 0);
  printINT16(v16);
  //if (v16 & BIT0)
  if (onBIT16(v16, 0))
    printf("Have One at 0\n");

  //v16 |= BIT1;
  setBIT16(v16, 1);
  printINT16(v16);
  if (v16 & BIT1)
    printf("Have One at 1\n");
  if (bitINT16(v16,1) == 1)
    printf("Have One at 1\n");

  //v16 &= ~BIT1;
  clrBIT16(v16, 1);
  printINT16(v16);
  if (v16 & BIT1)
    printf("Have One at 1\n");

  printINT16(0b0000000000000010);
  if (bitINT16(v16,1) == 1)
    printf("Have One at 1\n");

  printINT16(0x0068);
  printINT16(0x00AF);

  printINT16(0x1702);
  printINT16((0xFFFF/0x0080));

  printINT16(58594);
  printf("%d, %x\n", 58594, 58594);

  printINT16(0x8);
  printINT16(0x0023);

  char chCMD = 0;
  printINT16(chCMD);
  chCMD |= 0x18;
  printINT16(chCMD);
  chCMD <<= 3;
  printINT16(chCMD);
  unsigned short sCMD = chCMD;
  printINT16(sCMD);
  sCMD <<= 8;
  printINT16(sCMD);

  unsigned short nVolt = 0b1100000001101011;
  printINT16(nVolt);
  nVolt <<= 3;
  printINT16(nVolt);
  nVolt >>= 1;
  printINT16(nVolt);

  float fVolt = (nVolt * 3300.0 / 1024.0);
  printf("%.2f\n", fVolt);
  printf("%.2f\n", ((fVolt - 100.0) / 10.0) - 40.0);

  printf("%d %d\n", 0x44e10000, 0x44e10800);
  printINT32(0x44e10000);
  printINT32(0x44e10800);

  int nValue = 1;
  printINT32(nValue);
  printINT32(nValue+1);
  printINT32(nValue<<2);
  printINT32(BIT1);
  printINT32(BIT2);

  printINT16(0b00000000);
  printINT16(0b00000111);
  printINT16(0b00100111);
  printINT16(0x37);
  printINT16(0x27);

  printINT16(0x1FFF);
  printINT16(0xFF);

  printINT32(0x2);
  printINT32(1 << 18);

  printINT16(1<<4);
  printINT16(0x1F);

  int nS = 1;
  printINT32(nS);
  printINT32(nS >> (0 & 0x1F));
  printINT32(nS >> (1 & 0x1F));
  printINT32(nS >> (22 & 0x1F));

  int nV = 7 << 22;
  printINT32(7);
  printINT32(nV);
  printINT32(nV ^ nV);

  int nLSL = 0;
  nV = 1;
  nV = nV << (nLSL & 0x1F);
  printINT32(nV);
  nV = nV << (22 & 0x1F);
  printINT32(nV);

  int nR6 = 0;
  nR6 = nR6 | nV;
  printINT32(nR6);

  nLSL = nLSL + 1;
  nV = 1;
  nV = nV << (nLSL & 0x1F);
  nV = nV << (22 & 0x1F);

  nR6 = nR6 | nV;
  printINT32(nR6);

  nLSL = nLSL + 1;
  nV = 1;
  nV = nV << (nLSL & 0x1F);
  nV = nV << (22 & 0x1F);

  nR6 = nR6 | nV;
  printINT32(nR6);

  printINT32(0x44E11FFF - 0x44E10000);
  printf("%x\n", 8191);

  printINT32(0x01800000);
  printINT32(0x00780000);

  printINT32(131);
  printINT32(643);
  printINT32(BIT7);

  printINT32(BIT12);
  printf("%d\n", BITPOS(BIT12));

  printINT32(0x660000E3);
  printINT32(0x00F42400);
  printINT32(0x02FAF080);
  printf("0x%x\n", 0x02FAF080*2);

  return 0;
}
