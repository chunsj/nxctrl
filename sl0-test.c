/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Simple ARM to PRU Data Transfer Program
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
#include <errno.h>
#include <NXCTRL.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#define PRU_NUM    PRU0 // use PRU_0
#define PRU_PATH   "./sl0-test.bin"

#define LED_PIN    NXCTRL_PIN17

static void *pPRUDataMem;
static unsigned char *pchPRUDataMemBytes;

static int
__init_registers (void) {
  prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pPRUDataMem);
  pchPRUDataMemBytes = (unsigned char *)pPRUDataMem;

  pchPRUDataMemBytes[0] = 1;
  pchPRUDataMemBytes[1] = 0;

  return 0;
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int nRet, i;
  tpruss_intc_initdata intc = PRUSS_INTC_INITDATA;
  
  NXCTRLPinMode(NXCTRL_P8, LED_PIN, NXCTRL_OUTPUT);

  // initialize PRU
  if ((nRet = prussdrv_init())) {
    fprintf(stderr, "prussdrv_init() failed\n");
    exit(nRet);
  }

  // open the interrupt
  if ((nRet = prussdrv_open(PRU_EVTOUT_0))) {
    fprintf(stderr, "prussdrv_open() failed: %s\n", strerror(errno));
    exit(nRet);
  }

  // initialize interrupt
  if ((nRet = prussdrv_pruintc_init(&intc))) {
    fprintf(stderr, "prussdrv_pruintc_init() failed\n");
    exit(nRet);
  }

  // initialize test registers and values
  if ((nRet = __init_registers())) {
    fprintf(stderr, "__init_registers() failed\n");
    exit(nRet);
  }

  // load and run the PRU program
  if ((nRet = prussdrv_exec_program(PRU_NUM, PRU_PATH))) {
    fprintf(stderr, "prussdrv_exec_program() failed\n");
    exit(nRet);
  }

  i = 5;

  while (i--) {
    pchPRUDataMemBytes[1] = 1;
    NXCTRLSleep(500, 0);
    pchPRUDataMemBytes[1] = 0;
    NXCTRLSleep(500, 0);
  }

  pchPRUDataMemBytes[0] = 0;

  // wait for PRU to generate interrupt for completion indication
  printf("waiting for interrupt from PRU0...\n");

  nRet = prussdrv_pru_wait_event(PRU_EVTOUT_0);

  printf("PRU program completed with: %d\n", nRet);

  // clear the event
  if (prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT))
    fprintf(stderr, "prussdrv_pru_clear_event() failed\n");

  // halt and disable the PRU
  if (prussdrv_pru_disable(PRU_NUM))
    fprintf(stderr, "prussdrv_pru_disable() failed\n");

  // release the PRU clocks and disable prussdrv module
  if (prussdrv_exit())
    fprintf(stderr, "prussdrv_exit() failed\n");
}

NXCTRL_VOID
NXCTRLLoop (NXCTRL_VOID) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
