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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <NXCTRL_bitArray.h>

NXCTRL_VOID
NXCTRLBITARRAYInit (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBits) {
  pArr->nBits = nBits;
  pArr->nSize = (nBits >> 3) + 1;
  pArr->arr = (NXCTRL_UINT8 *)malloc(pArr->nSize * sizeof(NXCTRL_UINT8));
  if (pArr->arr)
    memset(pArr->arr, 0, pArr->nSize * sizeof(*pArr->arr));
}

NXCTRL_VOID
NXCTRLBITARRAYFree (NXCTRLBITARRAY *pArr) {
  if (pArr->arr)
    free(pArr->arr);
}

NXCTRL_BOOL
NXCTRLBITARRAYCheck (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBit) {
  if (pArr->nBits < nBit) {
    fprintf(stderr, "NXCTRLBITARRAYCheck: %d is out of range in %d\n",
            nBit, pArr->nBits);
    return NXCTRL_FALSE;
  }
  return NXCTRL_TRUE;
}

NXCTRL_VOID
NXCTRLBITARRAYSet (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBit) {
  if (!NXCTRLBITARRAYCheck(pArr, nBit))
    return;
  pArr->arr[nBit>>3] |= 1 << (nBit % 8);
}

NXCTRL_VOID
NXCTRLBITARRAYClear (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBit) {
  if (!NXCTRLBITARRAYCheck(pArr, nBit))
    return;
  pArr->arr[nBit>>3] &= ~(1 << (nBit % 8));
}

NXCTRL_BOOL
NXCTRLBITARRAYGet (NXCTRLBITARRAY *pArr, NXCTRL_UINT32 nBit) {
  if (!NXCTRLBITARRAYCheck(pArr, nBit))
    return NXCTRL_OFF;
  return (NXCTRL_BOOL)(pArr->arr[nBit>>3] & (1 << (nBit % 8)));
}
