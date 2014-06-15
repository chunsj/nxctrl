/*
 * NXCTRL BeagleBone Black Control Library
 *
 * BBB GPIO Pin Information
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
#include <signal.h>
#include <NXCTRL.h>

int
main (int argc, const char *argv[]) {
  const char *pszBankName = NULL;
  const char *pszPinName = NULL;
  if (argc == 1) {
    pszBankName = "ALL";
    pszPinName = "ALL";
  } else if (argc == 3) {
    pszPinName = argv[2];
    pszBankName = argv[1];
  } else {
    fprintf(stderr, "usage: pin-info [bankname pinnumber]\n");
    return 1;
  }

  printf("XXX List Pin Information for %s[%s]; such as mux\n", pszBankName, pszPinName);

  return 0;
}

void NXCTRLSetup (void) {}
void NXCTRLLoop (void) {}
