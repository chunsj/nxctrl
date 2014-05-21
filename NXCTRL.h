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

#ifndef __NXCTRL_HEADER__
#define __NXCTRL_HEADER__

#include <NXCTRL_types.h>
#include <NXCTRL_registers.h>

//
// User Defined Functions
//
NXCTRL_VOID          NXCTRLSetup (NXCTRL_VOID);
NXCTRL_VOID          NXCTRLLoop (NXCTRL_VOID);

//
// Run Loop Functions
//
NXCTRL_INT32         NXCTRLMain (NXCTRL_VOID);
NXCTRL_VOID          NXCTRLExitLoop (NXCTRL_VOID);
NXCTRL_VOID          NXCTRLSetSIGINTHandler (NXCTRL_SIGINT_HANDLER pfn);

//
// Control Functions
//
NXCTRL_BOOL          NXCTRLPinMux (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                   NXCTRL_MODE nMode, NXCTRL_PULLTYPE nPullUpDown,
                                   NXCTRL_BOOL bReceiverActive);
NXCTRL_BOOL          NXCTRLPinMode (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                    NXCTRL_DIR nDir);

NXCTRL_VOID          NXCTRLSleep (NXCTRL_UINT32 msec, NXCTRL_UINT32 usec);

NXCTRL_VOID          NXCTRLDigitalWrite (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                         NXCTRL_BOOL bF);
NXCTRL_BOOL          NXCTRLDigitalRead (NXCTRL_BANK nBank, NXCTRL_PIN nPin);

NXCTRL_UINT32        NXCTRLAnalogRead (NXCTRL_AIN nAIN);
NXCTRL_VOID          NXCTRLAnalogWrite (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                        NXCTRL_UINT32 nValue);

NXCTRL_VOID          NXCTRLServoWrite (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                       NXCTRL_UINT32 nDegree);

//
// Library Functions
//
NXCTRL_BOOL          NXCTRLOpen (NXCTRL_VOID);
NXCTRL_VOID          NXCTRLClose (NXCTRL_VOID);

NXCTRL_BOOL          NXCTRLTSCADCOpen (NXCTRL_VOID);
NXCTRL_VOID          NXCTRLTSCADCClose (NXCTRL_VOID);

NXCTRL_BOOL          NXCTRLPWMSSOpen (NXCTRL_VOID);
NXCTRL_VOID          NXCTRLPWMSSClose (NXCTRL_VOID);

NXCTRL_VOID          NXCTRLAnalogWriteEx (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                                          NXCTRL_UINT32 nValue,
                                          NXCTRL_UINT32 nPWMDiv,
                                          NXCTRL_UINT32 nFREQHZ, NXCTRL_UINT32 nRESOLUTION,
                                          NXCTRL_BOOL nLog);

NXCTRL_INT32         NXCTRLSPIWrite (int nSPIFD,
                                     const NXCTRL_UINT8 *rchTXBytes,
                                     NXCTRL_UINT8 *rchRXBytes,
                                     NXCTRL_UINT32 nLength);

#endif /* __NXCTRL_HEADER__ */
