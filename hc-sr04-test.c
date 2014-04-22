/*
 * NXCTRL BeagleBone Black Control Library
 *
 * HC SR-04 Untrasonic Sensor Test Program
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
#define PRU_PATH   "./hc-sr04-test.bin"

#define NUM_MEASURE  10

#define SENSOR_BANK  NXCTRL_P8

#define TRIGGER_PIN  NXCTRL_PIN11
#define ECHO_PIN     NXCTRL_PIN15

void
NXCTRLSetup (void) {
  int nRet, i;
  tpruss_intc_initdata intc = PRUSS_INTC_INITDATA;
  void *pPRUDataMem;
  unsigned int *pnPRUData;

  NXCTRLPinMux(SENSOR_BANK, TRIGGER_PIN, NXCTRL_MODE6, NXCTRL_PULLDN, NXCTRL_LOW);
  NXCTRLPinMux(SENSOR_BANK, ECHO_PIN, NXCTRL_MODE6, NXCTRL_PULLDN, NXCTRL_HIGH);

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

  prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pPRUDataMem);
  pnPRUData = (unsigned int *)pPRUDataMem;

  // load and run the PRU program
  if ((nRet = prussdrv_exec_program(PRU_NUM, PRU_PATH))) {
    fprintf(stderr, "prussdrv_exec_program() failed\n");
    exit(nRet);
  }

  // get measurements
  for (i = 0; i < NUM_MEASURE; i++) {
    // wait for PRU to generate interrupt for completion indication
    printf("waiting for interrupt from PRU0...\n");
    nRet = prussdrv_pru_wait_event(PRU_EVTOUT_0);

    // clear the event
    if (prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT))
      fprintf(stderr, "prussdrv_pru_clear_event() failed\n");

    // okay, we have data
    printf("%02d distance = %.1f cm\n", i, (float)pnPRUData[0]/2/29.1);
  }

  printf("PRU program completed with: %d\n", nRet);

  // halt and disable the PRU
  if (prussdrv_pru_disable(PRU_NUM))
    fprintf(stderr, "prussdrv_pru_disable() failed\n");

  // release the PRU clocks and disable prussdrv module
  if (prussdrv_exit())
    fprintf(stderr, "prussdrv_exit() failed\n");
}

void
NXCTRLLoop (void) {
  NXCTRLExitLoop();
}

int
main (void) {
  return NXCTRLMain();
}
