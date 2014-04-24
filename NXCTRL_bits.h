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

#ifndef __NXCTRL_BITS_HEADER__
#define __NXCTRL_BITS_HEADER__

#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800
#define BIT12   0x00001000
#define BIT13   0x00002000
#define BIT14   0x00004000
#define BIT15   0x00008000
#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

typedef union tagINT16 {
  unsigned short v16;
  struct {
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
    unsigned char b6:1;
    unsigned char b7:1;
    unsigned char b8:1;
    unsigned char b9:1;
    unsigned char b10:1;
    unsigned char b11:1;
    unsigned char b12:1;
    unsigned char b13:1;
    unsigned char b14:1;
    unsigned char b15:1;
  } bit;
} INT16;

inline static char
bitINT16 (unsigned short v16, int i) {
#define _b(u,i) ((*(INT16 *)&(u)).bit.b##i)
#define _c(i) case i: return _b(v,i)
  INT16 v = { .v16 = v16 };
  switch (i) {
    _c(0);
    _c(1);
    _c(2);
    _c(3);
    _c(4);
    _c(5);
    _c(6);
    _c(7);
    _c(8);
    _c(9);
    _c(10);
    _c(11);
    _c(12);
    _c(13);
    _c(14);
    _c(15);
  }
#undef _c
#undef _b
  return -1;
}

#define setBIT16(v16,i) ((v16) |= BIT##i)
#define clrBIT16(v16,i) ((v16) &= ~BIT##i)
#define onBIT16(v16,i) ((v16) & BIT##i)
#define offBIT16(v16,i) !((v16) & BIT##i)

inline static void
printINT16 (unsigned short v16) {
  INT16 v = { .v16 = v16 };
  printf("%5d %d:%d:%d:%d %d:%d:%d:%d %d:%d:%d:%d %d:%d:%d:%d \n",
         v.v16,
         v.bit.b0, v.bit.b1, v.bit.b2, v.bit.b3, v.bit.b4, v.bit.b5,
         v.bit.b6, v.bit.b7, v.bit.b8, v.bit.b9, v.bit.b10, v.bit.b11,
         v.bit.b12, v.bit.b13, v.bit.b14, v.bit.b15);
}

typedef union tagINT32 {
  unsigned int v32;
  struct {
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
    unsigned char b6:1;
    unsigned char b7:1;
    unsigned char b8:1;
    unsigned char b9:1;
    unsigned char b10:1;
    unsigned char b11:1;
    unsigned char b12:1;
    unsigned char b13:1;
    unsigned char b14:1;
    unsigned char b15:1;
    unsigned char b16:1;
    unsigned char b17:1;
    unsigned char b18:1;
    unsigned char b19:1;
    unsigned char b20:1;
    unsigned char b21:1;
    unsigned char b22:1;
    unsigned char b23:1;
    unsigned char b24:1;
    unsigned char b25:1;
    unsigned char b26:1;
    unsigned char b27:1;
    unsigned char b28:1;
    unsigned char b29:1;
    unsigned char b30:1;
    unsigned char b31:1;
  } bit;
} INT32;

inline static char
bitINT32 (unsigned int v32, int i) {
#define _b(u,i) ((*(INT32 *)&(u)).bit.b##i)
#define _c(i) case i: return _b(v,i)
  INT32 v = { .v32 = v32 };
  switch (i) {
    _c(0);
    _c(1);
    _c(2);
    _c(3);
    _c(4);
    _c(5);
    _c(6);
    _c(7);
    _c(8);
    _c(9);
    _c(10);
    _c(11);
    _c(12);
    _c(13);
    _c(14);
    _c(15);
    _c(16);
    _c(17);
    _c(18);
    _c(19);
    _c(20);
    _c(21);
    _c(22);
    _c(23);
    _c(24);
    _c(25);
    _c(26);
    _c(27);
    _c(28);
    _c(29);
    _c(30);
    _c(31);
  }
#undef _c
#undef _b
  return -1;
}

#define setBIT32(v32,i) ((v32) |= BIT##i)
#define clrBIT32(v32,i) ((v32) &= ~BIT##i)
#define onBIT32(v32,i) ((v32) & BIT##i)
#define offBIT32(v32,i) !((v32) & BIT##i)

inline static void
printINT32 (unsigned int v32) {
  INT32 v = { .v32 = v32 };
  printf("%10u %d:%d:%d:%d %d:%d:%d:%d %d:%d:%d:%d %d:%d:%d:%d %d:%d:%d:%d %d:%d:%d:%d %d:%d:%d:%d %d:%d:%d:%d\n",
         v.v32,
         v.bit.b0, v.bit.b1, v.bit.b2, v.bit.b3, v.bit.b4, v.bit.b5,
         v.bit.b6, v.bit.b7, v.bit.b8, v.bit.b9, v.bit.b10, v.bit.b11,
         v.bit.b12, v.bit.b13, v.bit.b14, v.bit.b15,
         v.bit.b16, v.bit.b17, v.bit.b18, v.bit.b19, v.bit.b20, v.bit.b21,
         v.bit.b22, v.bit.b23, v.bit.b24, v.bit.b25, v.bit.b26, v.bit.b27,
         v.bit.b28, v.bit.b29, v.bit.b30, v.bit.b31);
}

static inline unsigned int
BITPOS(int n) {
  int b = 0;
  n--;
  if((n&0xffff) == 0xffff) {
    b+=16;
    n>>=16;
  }
  if((n&0xff) == 0xff) {
    b+=8;
    n>>=8;
  }
  if((n&0xf) == 0xf) {
    b+=4;
    n>>=4;
  }
  if((n&0x3) == 0x3) {
    b+=2;
    n>>=2;
  }
  if ((n&1) == 1) {
      b++;
      n>>=1;
  }
  return b;
}       

#endif /* __NXCTRL_BITS_HEADER__ */
