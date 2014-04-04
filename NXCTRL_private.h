/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Copyright (C) 2014 Sungjin Chun <chunsj@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#define NAPIN (-1)
#define NOBIT (0x0)

static int rnP8Bank[] = {
  NAPIN, NAPIN, // 01/02
  GPIO1, GPIO1,
  GPIO1, GPIO1,
  GPIO2, GPIO2,
  GPIO2, GPIO2,
  GPIO1, GPIO1, // 11/12
  GPIO0, GPIO0,
  GPIO1, GPIO1,
  GPIO0, GPIO2,
  GPIO0, GPIO1,
  GPIO1, GPIO1, // 21/22
  GPIO1, GPIO1,
  GPIO1, GPIO1,
  GPIO2, GPIO2,
  GPIO2, GPIO2,
  GPIO0, GPIO0, // 31/32
  GPIO0, GPIO2,
  GPIO0, GPIO2,
  GPIO2, GPIO2,
  GPIO2, GPIO2,
  GPIO2, GPIO2, // 41/42
  GPIO2, GPIO2,
  GPIO2, GPIO2
};
static unsigned int rnP8Pins[] = {
  NOBIT, NOBIT, // 01/02
  BIT6, BIT7,
  BIT2, BIT3,
  BIT2, BIT3,
  BIT5, BIT4,
  BIT13, BIT12, // 11/12
  BIT23, BIT26,
  BIT15, BIT14,
  BIT27, BIT1,
  BIT22, BIT31,
  BIT30, BIT5,  // 21/22
  BIT4, BIT1,
  BIT0, BIT29,
  BIT22, BIT24,
  BIT23, BIT25,
  BIT10, BIT11, // 31/32
  BIT9, BIT17,
  BIT8, BIT16,
  BIT14, BIT15,
  BIT12, BIT13,
  BIT10, BIT11, // 41/42
  BIT8, BIT9,
  BIT6, BIT7
};
static unsigned int rnP8CTRLs[] = {
  NOBIT, NOBIT, // 01/02
  0x818, 0x81C,
  0x808, 0x80C,
  0x890, 0x894,
  0x89C, 0x898,
  0x834, 0x830, // 11/12
  0x824, 0x828,
  0x83C, 0x838,
  0x82C, 0x88C,
  0x820, 0x884,
  0x880, 0x814,  // 21/22
  0x810, 0x804,
  0x800, 0x87C,
  0x8E0, 0x8E8,
  0x8E4, 0x8EC,
  0x8D8, 0x8DC, // 31/32
  0x8D4, 0x8CC,
  0x8D0, 0x8C8,
  0x8C0, 0x8C4,
  0x8B8, 0x8BC,
  0x8B0, 0x8B4, // 41/42
  0x8A8, 0x8AC,
  0x8A0, 0x8A4
};
static int rnP8PWMs[] = {
  NAPIN, NAPIN, // 01/02
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN, // 11/12
  (BIT5|BIT4),    NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  (BIT4),    NAPIN,
  NAPIN, NAPIN, // 21/22
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN, // 31/32
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN, // 41/42
  NAPIN, NAPIN,
  NAPIN, NAPIN
};

static int rnP9Bank[] = {
  NAPIN, NAPIN, // 01/02
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  GPIO0, GPIO1, // 11/12
  GPIO0, GPIO1,
  GPIO1, GPIO1,
  GPIO0, GPIO0,
  GPIO0, GPIO0,
  GPIO0, GPIO0, // 21/22
  GPIO1, GPIO0,
  GPIO3, GPIO0,
  GPIO3, GPIO3,
  GPIO3, GPIO3,
  GPIO3, NAPIN, // 31/32
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  GPIO0, GPIO0, // 41/42
  NAPIN, NAPIN,
  NAPIN, NAPIN
};
static unsigned int rnP9Pins[] = {
  NOBIT, NOBIT, // 01/02
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  BIT30, BIT28, // 11/12
  BIT31, BIT18,
  BIT16, BIT19,
  BIT5, BIT4,
  BIT13, BIT12,
  BIT3, BIT2,   // 21/22
  BIT17, BIT15,
  BIT21, BIT14,
  BIT19, BIT17,
  BIT15, BIT16,
  BIT14, NOBIT, // 31/32
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  BIT20, BIT7,  // 41/42
  NOBIT, NOBIT,
  NOBIT, NOBIT
};
static unsigned int rnP9CTRLs[] = {
  NOBIT, NOBIT, // 01/02
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  0x870, 0x878, // 11/12
  0x874, 0x848,
  0x840, 0x84C,
  0x95C, 0x958,
  0x97C, 0x978,
  0x954, 0x950,   // 21/22
  0x844, 0x984,
  0x9AC, 0x980,
  0x9A4, 0x99C,
  0x994, 0x998,
  0x990, NOBIT, // 31/32
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  NOBIT, NOBIT,
  0x9B4, 0x964,  // 41/42
  NOBIT, NOBIT,
  NOBIT, NOBIT
};
static int rnP9PWMs[] = {
  NAPIN, NAPIN, // 01/02
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN, // 11/12
  NAPIN, (BIT3),
  NAPIN, (BIT5|BIT3),
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN, // 21/22
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  (BIT5), NAPIN,
  BIT0, NAPIN, // 31/32
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN,
  NAPIN, NAPIN, // 41/42
  NAPIN, NAPIN,
  NAPIN, NAPIN
};
