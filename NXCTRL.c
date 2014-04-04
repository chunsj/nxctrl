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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <NXCTRL.h>
#include "NXCTRL_private.h"

static NXCTRL_INT32 __hMem = NULL;

volatile NXCTRL_VOID *__CM_PER_ADDR    = NULL;
volatile NXCTRL_VOID *__CM_WKUP_ADDR   = NULL;
volatile NXCTRL_VOID *__GPIO0_ADDR     = NULL;
volatile NXCTRL_VOID *__GPIO1_ADDR     = NULL;
volatile NXCTRL_VOID *__GPIO2_ADDR     = NULL;
volatile NXCTRL_VOID *__GPIO3_ADDR     = NULL;
volatile NXCTRL_VOID *__CTRL_ADDR      = NULL;
volatile NXCTRL_VOID *__TSCADC_ADDR    = NULL;
volatile NXCTRL_VOID *__PWMSS0_ADDR    = NULL;
volatile NXCTRL_VOID *__PWMSS1_ADDR    = NULL;
volatile NXCTRL_VOID *__PWMSS2_ADDR    = NULL;

static NXCTRL_INT32  *__rpnBank[2];
static NXCTRL_UINT32 *__rpnPins[2];
static NXCTRL_UINT32 *__rpnCTRLs[2];
static NXCTRL_INT32  *__rpnPWMs[2];

volatile sig_atomic_t __nRunFLAG = 1;
static NXCTRL_SIGINT_HANDLER __pfnSIGINTHANDLER = NULL;

NXCTRL_VOID
NXCTRLSetSIGINTHandler (NXCTRL_SIGINT_HANDLER pfn) {
  __pfnSIGINTHANDLER = pfn;
}

NXCTRL_VOID
NXCTRLExitLoop (NXCTRL_VOID) {
  __nRunFLAG = 0;
}

static NXCTRL_VOID
__SIGINT_HANDLER (NXCTRL_INT32 nSIG) {
  fprintf(stderr, "SIGINT!\n");
  NXCTRLExitLoop();
  if (__pfnSIGINTHANDLER)
    __pfnSIGINTHANDLER();
}

NXCTRL_INT32
NXCTRLMain (NXCTRL_VOID) {
  extern NXCTRL_VOID NXCTRLSetup (NXCTRL_VOID);
  extern NXCTRL_VOID NXCTRLLoop (NXCTRL_VOID);

  signal(SIGINT, __SIGINT_HANDLER);

  if (!NXCTRLOpen())
    return -1;
  if (!NXCTRLTSCADCOpen())
    return -1;
  if (!NXCTRLPWMSSOpen())
    return -1;

  NXCTRLSetup();
  while (__nRunFLAG) NXCTRLLoop();

  NXCTRLPWMSSClose();
  NXCTRLTSCADCClose();
  NXCTRLClose();

  return 0;
}

static NXCTRL_VOID
__NXCTRLMapBankPinCTRLData (NXCTRL_VOID) {
  __rpnBank[0] = rnP8Bank;
  __rpnBank[1] = rnP9Bank;
  __rpnPins[0] = rnP8Pins;
  __rpnPins[1] = rnP9Pins;
  __rpnCTRLs[0] = rnP8CTRLs;
  __rpnCTRLs[1] = rnP9CTRLs;
  __rpnPWMs[0] = rnP8PWMs;
  __rpnPWMs[1] = rnP9PWMs;
}

#define __MMAP(v,s,addr,size) v = mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,__hMem,addr); if (v == MAP_FAILED) {fprintf(stderr, "NXCTRLOpen: %s mmap failed\n", s); return NXCTRL_FALSE;}

NXCTRL_BOOL
NXCTRLOpen (NXCTRL_VOID) {
  if (__hMem) {
    fprintf(stderr, "NXCTRLOpen: memory is already mapped\n");
    return NXCTRL_FALSE;
  }

  if (getuid()) {
    fprintf(stderr, "NXCTRLOpen: root priliege required\n");
    return NXCTRL_FALSE;
  }

  __NXCTRLMapBankPinCTRLData();

  __hMem = open("/dev/mem", O_RDWR);
  if (__hMem == -1) {
    fprintf(stderr, "NXCTRLOpen: cannot open /dev/mem\n");
    return NXCTRL_FALSE;
  }

  __MMAP(__CM_PER_ADDR, "CM_PER", CM_PER_REG_ADDR, CM_PER_REG_SIZE);
  __CM_WKUP_ADDR = __CM_PER_ADDR + CM_WKUP_REG_OFFSET;

  __MMAP(__GPIO0_ADDR, "GPIO0", GPIO0_REG_ADDR, GPIO0_REG_SIZE);
  __MMAP(__GPIO1_ADDR, "GPIO1", GPIO1_REG_ADDR, GPIO1_REG_SIZE);
  __MMAP(__GPIO2_ADDR, "GPIO2", GPIO2_REG_ADDR, GPIO2_REG_SIZE);
  __MMAP(__GPIO3_ADDR, "GPIO3", GPIO3_REG_ADDR, GPIO3_REG_SIZE);

  __MMAP(__CTRL_ADDR, "CONTROL", CONTROL_REG_ADDR, CONTROL_REG_SIZE);

  __MMAP(__TSCADC_ADDR, "TSCADC", TSCADC_REG_ADDR, TSCADC_REG_SIZE);

  __MMAP(__PWMSS0_ADDR, "PWMSS0", PWMSS0_REG_ADDR, PWMSS0_REG_SIZE);
  __MMAP(__PWMSS1_ADDR, "PWMSS1", PWMSS1_REG_ADDR, PWMSS1_REG_SIZE);
  __MMAP(__PWMSS2_ADDR, "PWMSS2", PWMSS2_REG_ADDR, PWMSS2_REG_SIZE);
  
  ENABLE_GPIO0_CLK(NXCTRL_ON); ENABLE_GPIO0_OPTFCLK(NXCTRL_ON);
  ENABLE_GPIO1_CLK(NXCTRL_ON); ENABLE_GPIO1_OPTFCLK(NXCTRL_ON);
  ENABLE_GPIO2_CLK(NXCTRL_ON); ENABLE_GPIO2_OPTFCLK(NXCTRL_ON);
  ENABLE_GPIO3_CLK(NXCTRL_ON); ENABLE_GPIO3_OPTFCLK(NXCTRL_ON);

#if 0 // shoud I need this?
  U32REG_CM_PER_L3S_CLKCTRL |= BIT1;
  U32REG_CM_PER_L2_CLKSTCTRL |= BIT1;
  U32REG_CM_PER_L3_INSTR_CLKCTRL |= BIT1;
  U32REG_CM_PER_L3_CLKCTRL |= BIT1;
  U32REG_CM_PER_OCPWP_L3_CLKSTCTRL |= BIT1;
  U32REG_CM_PER_L4LS_CLKSTCTRL |= BIT1;
  U32REG_CM_PERL4LS_CLKCTRL |= BIT1;
#endif

  return NXCTRL_TRUE;
}

NXCTRL_VOID
NXCTRLClose (NXCTRL_VOID) {
  if (__hMem < 0) {
    fprintf(stderr, "NXCTRLClose: memory already closed\n");
    return;
  }

  munmap((NXCTRL_VOID *)__PWMSS2_ADDR, PWMSS2_REG_SIZE);
  munmap((NXCTRL_VOID *)__PWMSS1_ADDR, PWMSS1_REG_SIZE);
  munmap((NXCTRL_VOID *)__PWMSS1_ADDR, PWMSS1_REG_SIZE);
  munmap((NXCTRL_VOID *)__TSCADC_ADDR, TSCADC_REG_SIZE);
  munmap((NXCTRL_VOID *)__CTRL_ADDR, CONTROL_REG_SIZE);
  munmap((NXCTRL_VOID *)__GPIO3_ADDR, GPIO3_REG_SIZE);
  munmap((NXCTRL_VOID *)__GPIO2_ADDR, GPIO2_REG_SIZE);
  munmap((NXCTRL_VOID *)__GPIO1_ADDR, GPIO1_REG_SIZE);
  munmap((NXCTRL_VOID *)__GPIO0_ADDR, GPIO0_REG_SIZE);
  munmap((NXCTRL_VOID *)__CM_PER_ADDR, CM_PER_REG_SIZE);

  close(__hMem);
}

static inline NXCTRL_VOID
__NXCTRLTSCADCEnableCLKHack (NXCTRL_VOID) {
  int nFD;
  char rch[2];

  nFD = open("/sys/kernel/nxpmx/adcsw", O_RDWR);
  if (nFD < 0) {
    fprintf(stderr, "__TSCADC_enableCLK: cannot open adcsw file %s\n",
            "/sys/kernel/nxpmx/adcsw");
    return;
  }
  sprintf(rch, "%d", 1);
  write(nFD, rch, 1);
  close(nFD);
}

static inline NXCTRL_VOID
__NXCTRLTSCADCDisableCLKHack (NXCTRL_VOID) {
  int nFD;
  char rch[2];

  nFD = open("/sys/kernel/nxpmx/adcsw", O_RDWR);
  if (nFD < 0) {
    fprintf(stderr, "__TSCADC_enableCLK: cannot open adcsw file %s\n",
            "/sys/kernel/nxpmx/adcsw");
    return;
  }
  sprintf(rch, "%d", 0);
  write(nFD, rch, 1);
  close(nFD);
}

NXCTRL_BOOL
NXCTRLTSCADCOpen (NXCTRL_VOID) {
  int nFD;
  char rchPMX[16];
  int i, nMUX = 0x20;
  int nOffset = 10;
  int rnOffsets[10] = {
    0xB2C, 0xB28, 0xB24, 0xB20, 0xB1C, 0xB18, 0xB14, 0xB10,
    0xB30, 0xB34
  };
  const char *pszFN = "/sys/kernel/nxpmx/mux";

  // XXX this does not work
  ENABLE_ADC_TSC_CLK(NXCTRL_ON);
  // XXX so hack this
  __NXCTRLTSCADCEnableCLKHack();

  // XXX start of pinmux hack
  for (i = 0; i < nOffset; i++) {
    nFD = open(pszFN, O_RDWR);
    if (nFD == -1) {
      fprintf(stderr, "NXCTRLTSCADCOpen: cannot open pinmux file %s\n",
              pszFN);
      return NXCTRL_FALSE;
    }

    sprintf(rchPMX, "0x%x:0x%x", rnOffsets[i], nMUX);
    write(nFD, rchPMX, strlen(rchPMX));

    close(nFD);
  }

  return NXCTRL_TRUE;
}

NXCTRL_VOID
NXCTRLTSCADCClose (NXCTRL_VOID) {
  // XXX this does not work
  ENABLE_ADC_TSC_CLK(NXCTRL_OFF);
  // XXX so hack this
  __NXCTRLTSCADCDisableCLKHack();
}

static inline NXCTRL_VOID
__NXCTRLPWMSSEnableCLKHack (NXCTRL_VOID) {
  int nFD;
  char rch[2];
  const char *pszFN = "/sys/kernel/nxpmx/epwmsw";

  nFD = open(pszFN, O_RDWR);
  if (nFD < 0) {
    fprintf(stderr, "__NXCTRLPWMSSEnableCLKHack: cannot open epwmsw file %s\n",
            pszFN);
    return;
  }
  sprintf(rch, "%d", 1);
  write(nFD, rch, 1);
  close(nFD);
}

static inline NXCTRL_VOID
__NXCTRLPWMSSDisableCLKHack (NXCTRL_VOID) {
  int nFD;
  char rch[2];
  const char *pszFN = "/sys/kernel/nxpmx/epwmsw";

  nFD = open(pszFN, O_RDWR);
  if (nFD < 0) {
    fprintf(stderr, "__NXCTRLPWMSSDisableCLKHack: cannot open epwmsw file %s\n",
            pszFN);
    return;
  }
  sprintf(rch, "%d", 0);
  write(nFD, rch, 1);
  close(nFD);
}

NXCTRL_BOOL
NXCTRLPWMSSOpen (NXCTRL_VOID) {
  int nFD;
  char rchPMX[16];
  const char *pszFN = "/sys/kernel/nxpmx/mux";

  nFD = open(pszFN, O_RDWR);
  if (nFD == -1) {
    fprintf(stderr, "NXCTRLPWMSSOpen: cannot open pinmux file %s\n",
            pszFN);
    return NXCTRL_FALSE;
  }
  
  sprintf(rchPMX, "0x%x:0x%x", PWMSS_CTRL, 0x7);
  write(nFD, rchPMX, strlen(rchPMX));
  
  close(nFD);

  // XXX this does not work
  ENABLE_EPWMSS0_CLK(NXCTRL_ON);
  ENABLE_EPWMSS1_CLK(NXCTRL_ON);
  ENABLE_EPWMSS2_CLK(NXCTRL_ON);

  // XXX hack this
  __NXCTRLPWMSSEnableCLKHack();

  ENABLE_PWMSS0_CLK(NXCTRL_ON);
  ENABLE_PWMSS1_CLK(NXCTRL_ON);
  ENABLE_PWMSS2_CLK(NXCTRL_ON);

  return NXCTRL_TRUE;
}

NXCTRL_VOID
NXCTRLPWMSSClose (NXCTRL_VOID) {
  SET_EPWMSS0_AQCTLA(PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW);
  SET_EPWMSS0_AQCTLB(PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW);
  SET_EPWMSS1_AQCTLA(PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW);
  SET_EPWMSS1_AQCTLB(PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW);
  SET_EPWMSS2_AQCTLA(PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW);
  SET_EPWMSS2_AQCTLB(PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW, PWM_AQ_LOW);

  // XXX this does not work
  ENABLE_PWMSS0_CLK(NXCTRL_OFF);
  ENABLE_PWMSS1_CLK(NXCTRL_OFF);
  ENABLE_PWMSS2_CLK(NXCTRL_OFF);

  // XXX hack this
  __NXCTRLPWMSSDisableCLKHack();
}

#define GET_GPIO(nBank,nPin) __rpnBank[(nBank)][((nPin) - 1)]

NXCTRL_BOOL
NXCTRLPinMux (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
              NXCTRL_MODE nMode, NXCTRL_PULLTYPE nPullUpDown,
              NXCTRL_BOOL bReceiverActive) {
  char rchFileName[1024];
  char rchPMX[16];
  int nFD, nOffset, nMUX;

  if (nBank != NXCTRL_P8 && nBank != NXCTRL_P9) {
    fprintf(stderr, "NXCTRLPinMux: unsupported bank\n");
    return NXCTRL_FALSE;
  }

  if (nPin < 1 || nPin > 46) {
    fprintf(stderr, "NXCTRLPinMux: unsupported pin number\n");
    return NXCTRL_FALSE;
  }

  if (GET_GPIO(nBank, nPin) == NAPIN) {
    fprintf(stderr, "NXCTRLPinMux: invalid pin position\n");
    return NXCTRL_FALSE;
  }
  
  sprintf(rchFileName, "/sys/kernel/nxpmx/mux");
  nFD = open(rchFileName, O_RDWR);
  if (nFD < 0) {
    fprintf(stderr, "NXCTRLPinMux: cannot open pinmux file %s\n", rchFileName);
    return NXCTRL_FALSE;
  }

  if (nBank == NXCTRL_P8)
    nOffset = rnP8CTRLs[nPin-1];
  else
    nOffset = rnP9CTRLs[nPin-1];

  nMUX = nMode;

  if (bReceiverActive)
    nMUX |= BIT5;
  else
    nMUX &= ~BIT5;

  switch (nPullUpDown) {
  case NXCTRL_PULLUP:
    nMUX |= BIT4;
    break;
  case NXCTRL_PULLDN:
    nMUX &= ~BIT4;
    break;
  default:
    nMUX |= BIT3;
    nMUX &= ~BIT4;
    break;
  }

  sprintf(rchPMX, "0x%x:0x%x", nOffset, nMUX);
  write(nFD, rchPMX, strlen(rchPMX));

  close(nFD);

  return NXCTRL_TRUE;
}

static NXCTRL_VOID
__NXCTRLWriteMode (NXCTRL_BANK nBank, NXCTRL_PIN nPin, NXCTRL_DIR nDir) {
  switch (nDir) {
  case NXCTRL_OUTPUT:
    NXCTRLPinMux(nBank, nPin, NXCTRL_MODE7, NXCTRL_NOPULL, NXCTRL_OFF);
    break;
  case NXCTRL_INPUT:
    NXCTRLPinMux(nBank, nPin, NXCTRL_MODE7, NXCTRL_NOPULL, NXCTRL_ON);
    break;
  case NXCTRL_OUTPUT_PULLUP:
    NXCTRLPinMux(nBank, nPin, NXCTRL_MODE7, NXCTRL_PULLUP, NXCTRL_OFF);
    break;
  case NXCTRL_INPUT_PULLUP:
    NXCTRLPinMux(nBank, nPin, NXCTRL_MODE7, NXCTRL_PULLUP, NXCTRL_ON);
    break;
  case NXCTRL_OUTPUT_PULLDN:
    NXCTRLPinMux(nBank, nPin, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_OFF);
    break;
  case NXCTRL_INPUT_PULLDN:
    NXCTRLPinMux(nBank, nPin, NXCTRL_MODE7, NXCTRL_PULLDN, NXCTRL_ON);
    break;
  }
}

#define GET_PIN(nBank,nPin)       (__rpnPins[(nBank)][((nPin) - 1)])
#define GET_CTRL_ADDR(nBank,nPin) (__rpnCTRLs[(nBank)][((nPin) - 1)]/4)

NXCTRL_BOOL
NXCTRLPinMode (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
               NXCTRL_DIR nDir) {
  volatile NXCTRL_VOID *pCTRL = NULL;
  volatile NXCTRL_VOID *pREG = NULL;
  int nFD;
  const char *pszFN = "/sys/kernel/nxpmx/mux";
  
  if (nBank != NXCTRL_P8 && nBank != NXCTRL_P9) {
    fprintf(stderr, "NXCTRLPinMode unsupported bank\n");
    return NXCTRL_FALSE;
  }

  if (nPin < 1 || nPin > 46) {
    fprintf(stderr, "NXCTRLPinMode unsupported pin number\n");
    return NXCTRL_FALSE;
  }

  if (GET_GPIO(nBank, nPin) == NAPIN) {
    fprintf(stderr, "NXCTRLPinMode invalid pin position\n");
    return NXCTRL_FALSE;
  }
  
  nFD = open(pszFN, O_RDWR);
  if (nFD < 0) {
    fprintf(stderr, "NXCTRLPinMode cannot open pinmux file %s\n", pszFN);
    return NXCTRL_FALSE;
  }

  pCTRL = REGADDR(__CTRL_ADDR, GET_CTRL_ADDR(nBank,nPin));
  switch (GET_GPIO(nBank,nPin)) {
  case GPIO0:
    pREG = REGADDR(__GPIO0_ADDR, GPIO_OE);
    break;
  case GPIO1:
    pREG = REGADDR(__GPIO0_ADDR, GPIO_OE);
    break;
  case GPIO2:
    pREG = REGADDR(__GPIO0_ADDR, GPIO_OE);
    break;
  case GPIO3:
    pREG = REGADDR(__GPIO0_ADDR, GPIO_OE);
    break;
  }

  if (nDir == NXCTRL_OUTPUT) {
    ASU32REG(pCTRL) |= 0x0F;
    ASU32REG(pREG) &= ~(GET_PIN(nBank, nPin));
  } else if (nDir == NXCTRL_INPUT) {
    ASU32REG(pCTRL) |= 0x2F;
    ASU32REG(pREG) |= GET_PIN(nBank, nPin);
  } else if (nDir == NXCTRL_OUTPUT_PULLUP) {
    ASU32REG(pCTRL) |= 0x17;
    ASU32REG(pREG) &= ~(GET_PIN(nBank, nPin));
  } else if (nDir == NXCTRL_INPUT_PULLUP) {
    ASU32REG(pCTRL) |= 0x37;
    ASU32REG(pREG) |= GET_PIN(nBank, nPin);
  } else if (nDir == NXCTRL_OUTPUT_PULLDN) {
    ASU32REG(pCTRL) |= 0x07;
    ASU32REG(pREG) &= ~(GET_PIN(nBank, nPin));
  } else if (nDir == NXCTRL_INPUT_PULLDN) {
    ASU32REG(pCTRL) |= 0x27;
    ASU32REG(pREG) |= GET_PIN(nBank, nPin);
  }

  // XXX pCTRL does not work (privileged mode)
  __NXCTRLWriteMode(nBank, nPin, nDir);

  return NXCTRL_TRUE;
}

NXCTRL_VOID
NXCTRLSleep (NXCTRL_UINT32 msec, NXCTRL_UINT32 usec) {
  int usleep(NXCTRL_UINT32);
  usleep(msec*1000 + usec);
  return;
}

NXCTRL_VOID
NXCTRLDigitalWrite (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                    NXCTRL_BOOL bF) {
  int nGPIO = GET_GPIO(nBank, nPin);
  
  if (bF == NXCTRL_HIGH)
    switch (nGPIO) {
    case GPIO0:
      U32REG_GPIO0_SETDATAOUT = GET_PIN(nBank, nPin);
      break;
    case GPIO1:
      U32REG_GPIO1_SETDATAOUT = GET_PIN(nBank, nPin);
      break;
    case GPIO2:
      U32REG_GPIO2_SETDATAOUT = GET_PIN(nBank, nPin);
      break;
    case GPIO3:
      U32REG_GPIO3_SETDATAOUT = GET_PIN(nBank, nPin);
      break;
    }
  else if (bF == NXCTRL_LOW)
    switch (nGPIO) {
    case GPIO0:
      U32REG_GPIO0_CLEARDATAOUT = GET_PIN(nBank, nPin);
      break;
    case GPIO1:
      U32REG_GPIO1_CLEARDATAOUT = GET_PIN(nBank, nPin);
      break;
    case GPIO2:
      U32REG_GPIO2_CLEARDATAOUT = GET_PIN(nBank, nPin);
      break;
    case GPIO3:
      U32REG_GPIO3_CLEARDATAOUT = GET_PIN(nBank, nPin);
      break;
    }
}

NXCTRL_BOOL
NXCTRLDigitalRead (NXCTRL_BANK nBank, NXCTRL_PIN nPin) {
  int nGPIO = GET_GPIO(nBank, nPin);

  switch (nGPIO) {
  case GPIO0:
    return (NXCTRL_BOOL)((U32REG_GPIO0_DATAIN & GET_PIN(nBank, nPin)) != 0);
  case GPIO1:
    return (NXCTRL_BOOL)((U32REG_GPIO1_DATAIN & GET_PIN(nBank, nPin)) != 0);
  case GPIO2:
    return (NXCTRL_BOOL)((U32REG_GPIO2_DATAIN & GET_PIN(nBank, nPin)) != 0);
  case GPIO3:
    return (NXCTRL_BOOL)((U32REG_GPIO3_DATAIN & GET_PIN(nBank, nPin)) != 0);
  }

  return NXCTRL_OFF;
}

static NXCTRL_UINT32 nANALOG_STEP = 0;
static NXCTRL_UINT32 nANALOG_FIFO = 0;

NXCTRL_UINT32
NXCTRLAnalogRead (NXCTRL_AIN nAIN) {
  int nStatus = 0;
  int nLoopCount = 0;
  int nSTEP, nFIFO;
  NXCTRL_UINT32 nValue, nTAG;

  nSTEP = nANALOG_STEP++;
  nFIFO = nANALOG_FIFO++;

  if (nANALOG_STEP > 15) nANALOG_STEP = 0;
  if (nANALOG_FIFO > 1) nANALOG_FIFO = 0;
  
  ENABLE_TSC_ADC_SS_MODULE(NXCTRL_OFF);
  
  SET_TSC_ADC_SS_ADC_CLKDIV(0x1);
  ENABLE_TSC_ADC_SS_STEP_ID_TAG(NXCTRL_ON);
  ENABLE_TSC_ADC_SS_STEP_CONFIG_WRITE(NXCTRL_ON);

  ENABLE_TSC_ADC_SS_STEPCONFIG_DIFF(nSTEP, NXCTRL_OFF);
  SET_TSC_ADC_SS_STEPCONFIG_INP(nSTEP, nAIN);
  SET_TSC_ADC_SS_STEPCONFIG_FIFO(nSTEP, nFIFO);
  SET_TSC_ADC_SS_STEPCONFIG_MODE(nSTEP, 0x00);
  SET_TSC_ADC_SS_STEPCONFIG_AVG(nSTEP, 0x4);
  SET_TSC_ADC_SS_STEPDELAY_SAMPLE(nSTEP, 0x0);
  SET_TSC_ADC_SS_STEPDELAY_OPEN(nSTEP, 0x0);
  
  SET_TSC_ADC_SS_STEPENABLE(nSTEP, NXCTRL_ON);

  CLEAR_TSC_ADC_SS_IRQSTATUS;
  SET_TSC_ADC_SS_IRQENABLE_SET(BIT1);

  ENABLE_TSC_ADC_SS_MODULE(NXCTRL_ON);

  while (!((nStatus = U32REG_TSC_ADC_SS_IRQSTATUS) & BIT1)
         && (nLoopCount < 5)) {
    nLoopCount++;
    NXCTRLSleep(10, 0);
  }

  U32REG_TSC_ADC_SS_IRQSTATUS = BIT1;

  if (!(nStatus & BIT1))
    fprintf(stderr, "NXCTRLAnalogRead: invalid value emitted\n");

  nValue = U32REG_TSC_ADC_SS_FIFODATA(nFIFO);
  nTAG = (nValue & (BIT16|BIT17|BIT18|BIT19))>>16;

  if (nTAG != nSTEP)
    fprintf(stderr, "NXCTRLAnalogRead: unmatched tag\n");

  return (nValue & 0xFFF);
}

#define GET_PWM(nBank, nPin) __rpnPWMs[(nBank)][((nPin) - 1)]

#define PWM_MODULE_FREQ      (100000000)
#define PWM_TBCLK            (10000000)
#define HALF_FREQ_HZ         (500)
#define FREQ_HZ              (2*HALF_FREQ_HZ)

static inline NXCTRL_VOID
__NXCTRLComputeTBCLK (NXCTRL_UINT32 nTBCLK,
                      NXCTRL_UINT32 *pnCLKDIV, NXCTRL_UINT32 *pnHSPCLKDIV) {
  NXCTRL_UINT32 nCLKDiv = PWM_MODULE_FREQ / nTBCLK;
  NXCTRL_UINT32 nHSPCLKDiv, nLSPCLKDiv;
  NXCTRL_UINT32 nLSPCLKDivSetting = 0;
  NXCTRL_UINT32 nLimit = PWM_MODULE_FREQ / (128*14);

  if (nTBCLK < nLimit) {
    fprintf(stderr, "__NXCTRLComputeTBCLK: limit CLK %d[%d]\n",
            nTBCLK, nLimit);
    nCLKDiv = PWM_MODULE_FREQ / nLimit;
  }

  // note that TBCLK be SYSCLKOUT/(HSPCLKDIV * CLKDIV)
  if (nCLKDiv > 14) {
    nHSPCLKDiv = 0x7;
    nLSPCLKDiv = nCLKDiv / 14;
    while (nLSPCLKDiv > 1) {
      nLSPCLKDiv = nLSPCLKDiv >> 1;
      nLSPCLKDivSetting++;
    }
  } else {
    nHSPCLKDiv = nCLKDiv / 2;
    nLSPCLKDivSetting = 0x0;
  }

  *pnCLKDIV = nLSPCLKDivSetting;
  *pnHSPCLKDIV = nHSPCLKDiv;
}

#define PWM_RESOLUTION (1024)

NXCTRL_VOID
NXCTRLAnalogWrite (NXCTRL_BANK nBank, NXCTRL_PIN nPin,
                   NXCTRL_UINT32 nValue) {
  int nPWMInfo = GET_PWM(nBank, nPin);
  int nChannel = (nPWMInfo & BIT5 & ~(BIT0|BIT2|BIT3|BIT4)) >> 5;
  int nSS = (nPWMInfo & (BIT3|BIT4) & ~(BIT0|BIT1|BIT2|BIT5)) >> 3;
  NXCTRL_UINT32 nCLKDIV, nHSPCLKDIV, nPRDCNT = PWM_TBCLK/FREQ_HZ;
  NXCTRL_UINT32 nDCNT = (nValue * nPRDCNT) / PWM_RESOLUTION;

  if (nPWMInfo < 0) {
    fprintf(stderr, "NXCTRLAnalogWrite: invalid pwm pin\n");
    return;
  }

  __NXCTRLComputeTBCLK(PWM_TBCLK, &nCLKDIV, &nHSPCLKDIV);

  if (nValue == 0) {
    switch (nSS) {
    case 0:
      SET_EPWMSS0_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS0_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS0_TBCTL_CTRMODE(0x0);
      SET_EPWMSS0_TBPRD(nPRDCNT);
      // why should I match A and B?
      ENABLE_EPWMSS0_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS0_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS0_CMPA(nDCNT);
      ENABLE_EPWMSS0_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS0_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS0_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS0_AQCTLA(0x1,0x1,0x1,0x1,0x1,0x1);
        SET_EPWMSS0_AQSFRC_ACTSFA(0x1);
        break;
      case 1:
        SET_EPWMSS0_AQCTLB(0x1,0x1,0x1,0x1,0x1,0x1);
        SET_EPWMSS0_AQSFRC_ACTSFB(0x1);
        break;
      }
      break;
    case 1:
      SET_EPWMSS1_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS1_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS1_TBCTL_CTRMODE(0x0);
      SET_EPWMSS1_TBPRD(nPRDCNT);
      ENABLE_EPWMSS1_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS1_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS1_CMPA(nDCNT);
      ENABLE_EPWMSS1_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS1_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS1_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS1_AQCTLA(0x1,0x1,0x1,0x1,0x1,0x1);
        SET_EPWMSS1_AQSFRC_ACTSFA(0x1);
        break;
      case 1:
        SET_EPWMSS1_AQCTLB(0x1,0x1,0x1,0x1,0x1,0x1);
        SET_EPWMSS1_AQSFRC_ACTSFB(0x1);
        break;
      }
      break;
    case 2:
      SET_EPWMSS2_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS2_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS2_TBCTL_CTRMODE(0x0);
      SET_EPWMSS2_TBPRD(nPRDCNT);
      ENABLE_EPWMSS2_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS2_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS2_CMPA(nDCNT);
      ENABLE_EPWMSS2_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS2_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS2_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS2_AQCTLA(0x1,0x1,0x1,0x1,0x1,0x1);
        SET_EPWMSS2_AQSFRC_ACTSFA(0x1);
        break;
      case 1:
        SET_EPWMSS2_AQCTLB(0x1,0x1,0x1,0x1,0x1,0x1);
        SET_EPWMSS2_AQSFRC_ACTSFB(0x1);
        break;
      }
      break;
    }
  } else if (nValue == 0xFF) {
    switch (nSS) {
    case 0:
      SET_EPWMSS0_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS0_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS0_TBCTL_CTRMODE(0x0);
      SET_EPWMSS0_TBPRD(nPRDCNT);
      ENABLE_EPWMSS0_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS0_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS0_CMPA(nDCNT);
      ENABLE_EPWMSS0_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS0_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS0_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS0_AQCTLA(0x2,0x2,0x2,0x2,0x2,0x2);
        SET_EPWMSS0_AQSFRC_ACTSFA(0x2);
        break;
      case 1:
        SET_EPWMSS0_AQCTLB(0x2,0x2,0x2,0x2,0x2,0x2);
        SET_EPWMSS0_AQSFRC_ACTSFB(0x2);
        break;
      }
      break;
    case 1:
      SET_EPWMSS1_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS1_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS1_TBCTL_CTRMODE(0x0);
      SET_EPWMSS1_TBPRD(nPRDCNT);
      ENABLE_EPWMSS1_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS1_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS1_CMPA(nDCNT);
      ENABLE_EPWMSS1_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS1_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS1_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS1_AQCTLA(0x2,0x2,0x2,0x2,0x2,0x2);
        SET_EPWMSS1_AQSFRC_ACTSFA(0x2);
        break;
      case 1:
        SET_EPWMSS1_AQCTLB(0x2,0x2,0x2,0x2,0x2,0x2);
        SET_EPWMSS1_AQSFRC_ACTSFB(0x2);
        break;
      }
      break;
    case 2:
      SET_EPWMSS2_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS2_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS2_TBCTL_CTRMODE(0x0);
      SET_EPWMSS2_TBPRD(nPRDCNT);
      ENABLE_EPWMSS2_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS2_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS2_CMPA(nDCNT);
      ENABLE_EPWMSS2_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS2_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS2_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS2_AQCTLA(0x2,0x2,0x2,0x2,0x2,0x2);
        SET_EPWMSS2_AQSFRC_ACTSFA(0x2);
        break;
      case 1:
        SET_EPWMSS2_AQCTLB(0x2,0x2,0x2,0x2,0x2,0x2);
        SET_EPWMSS2_AQSFRC_ACTSFB(0x2);
        break;
      }
      break;
    }
  } else {
    switch (nSS) {
    case 0:
      SET_EPWMSS0_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS0_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS0_TBCTL_CTRMODE(0x0);
      SET_EPWMSS0_TBPRD(nPRDCNT);
      ENABLE_EPWMSS0_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS0_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS0_CMPA(nDCNT);
      ENABLE_EPWMSS0_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS0_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS0_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS0_AQCTLA(0x2,0x0,0x1,0x0,0x0,0x0);
        SET_EPWMSS0_AQSFRC_ACTSFA(0x0);
        break;
      case 1:
        SET_EPWMSS0_AQCTLB(0x2,0x0,0x1,0x0,0x0,0x0);
        SET_EPWMSS0_AQSFRC_ACTSFB(0x0);
        break;
      }
      break;
    case 1:
      SET_EPWMSS1_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS1_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS1_TBCTL_CTRMODE(0x0);
      SET_EPWMSS1_TBPRD(nPRDCNT);
      ENABLE_EPWMSS1_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS1_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS1_CMPA(nDCNT);
      ENABLE_EPWMSS1_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS1_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS1_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS1_AQCTLA(0x2,0x0,0x1,0x0,0x0,0x0);
        SET_EPWMSS1_AQSFRC_ACTSFA(0x0);
        break;
      case 1:
        SET_EPWMSS1_AQCTLB(0x2,0x0,0x1,0x0,0x0,0x0);
        SET_EPWMSS1_AQSFRC_ACTSFB(0x0);
        break;
      }
      break;
    case 2:
      SET_EPWMSS2_TBCTL_CLKDIV(nCLKDIV);
      SET_EPWMSS2_TBCTL_HSPCLKDIV(nHSPCLKDIV);
      SET_EPWMSS2_TBCTL_CTRMODE(0x0);
      SET_EPWMSS2_TBPRD(nPRDCNT);
      ENABLE_EPWMSS2_CMPCTL_SHDWAMODE(NXCTRL_OFF);
      SET_EPWMSS2_CMPCTRL_LOADAMODE(0x3);
      SET_EPWMSS2_CMPA(nDCNT);
      ENABLE_EPWMSS2_CMPCTL_SHDWBMODE(NXCTRL_OFF);
      SET_EPWMSS2_CMPCTRL_LOADBMODE(0x3);
      SET_EPWMSS2_CMPB(nDCNT);
      switch (nChannel) {
      case 0:
        SET_EPWMSS2_AQCTLA(0x2,0x0,0x1,0x0,0x0,0x0);
        SET_EPWMSS2_AQSFRC_ACTSFA(0x0);
        break;
      case 1:
        SET_EPWMSS2_AQCTLB(0x2,0x0,0x1,0x0,0x0,0x0);
        SET_EPWMSS2_AQSFRC_ACTSFB(0x0);
        break;
      }
      break;
    }
  }
}

