/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Simple PRU Test Program
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
#define PRU_PATH   "./pru2-test.bin"

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int nRet;
  tpruss_intc_initdata intc = PRUSS_INTC_INITDATA;

  // pr1_pru0_pru_r30_14
  NXCTRLPinMux(NXCTRL_P8, NXCTRL_PIN12, NXCTRL_MODE6, NXCTRL_PULLDN, NXCTRL_LOW);

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

  // load and run the PRU program
  if ((nRet = prussdrv_exec_program(PRU_NUM, PRU_PATH))) {
    fprintf(stderr, "prussdrv_exec_program() failed\n");
    exit(nRet);
  }

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
