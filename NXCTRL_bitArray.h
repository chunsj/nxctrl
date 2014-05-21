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

#ifndef __NXCTRL_BITARRAY_HEADER__
#define __NXCTRL_BITARRAY_HEADER__

#include <NXCTRL.h>

typedef struct __tagNXCTRLBITARRAY {
  NXCTRL_UINT8 *arr;
  NXCTRL_UINT32 nBits;
  NXCTRL_UINT32 nSize;
} NXCTRLBITARRAY;

NXCTRL_VOID NXCTRLBITARRAYInit (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBits);
NXCTRL_VOID NXCTRLBITARRAYFree (NXCTRLBITARRAY *pArr);

NXCTRL_VOID NXCTRLBITARRAYSet (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBit);
NXCTRL_VOID NXCTRLBITARRAYClear (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBit);

NXCTRL_BOOL NXCTRLBITARRAYGet (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBit);

#endif /* __NXCTRL_BITARRAY_HEADER__ */
