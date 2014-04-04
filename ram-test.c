/*
 * NXCTRL BeagleBone Black Control Library
 *
 * PRU RAM Access Test Program
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

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#define PRU_NUM    PRU0 // use PRU_0
#define PRU_PATH   "./ram-test.bin"

#define DDR_BASEADDR     0x80000000
#define OFFSET_DDR       0x00001000
#define OFFSET_SHAREDRAM 2048

#define ADDEND1          0x12341234u
#define ADDEND2          0x43214321u
#define ADDEND3          0x98765432u

static int nFDMem;
static unsigned int *pMEMDDR;

static int
__init_registers (void) {
  unsigned int *pDDR1, *pDDR2, *pDDR3;

  nFDMem = open ("/dev/mem", O_RDWR);
  if (nFDMem < 0) {
    fprintf(stderr, "cannot open /dev/mem (%s)\n", strerror(errno));
    return -1;
  }

  pMEMDDR = mmap(0, 0x0FFFFFFF, PROT_WRITE | PROT_READ, MAP_SHARED, nFDMem, DDR_BASEADDR);
  if (pMEMDDR == NULL) {
    fprintf(stderr, "cannot map the device (%s)\n", strerror(errno));
    close(nFDMem);
    return -1;
  }


  pDDR1 = (void *)pMEMDDR + OFFSET_DDR;
  pDDR2 = (void *)pMEMDDR + OFFSET_DDR + 0x00000004;
  pDDR3 = (void *)pMEMDDR + OFFSET_DDR + 0x00000008;

  *(unsigned int *)pDDR1 = ADDEND1;
  *(unsigned int *)pDDR2 = ADDEND2;
  *(unsigned int *)pDDR3 = ADDEND3;

  return 0;
}

static int
__check_results (void) {
  void *pSharedMem;
  unsigned int *pnSharedInts;
  unsigned int nRes0, nRes1, nRes2;

  prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &pSharedMem);
  pnSharedInts = (unsigned int *)pSharedMem + OFFSET_SHAREDRAM;

  nRes0 = pnSharedInts[0];
  nRes1 = pnSharedInts[1];
  nRes2 = pnSharedInts[2];

  return ((nRes0 == ADDEND1) & (nRes1 == ADDEND2) & (nRes2 == ADDEND3));
}

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int nRet;
  tpruss_intc_initdata intc = PRUSS_INTC_INITDATA;
  
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

  // wait for PRU to generate interrupt for completion indication
  printf("waiting for interrupt from PRU0...\n");
  nRet = prussdrv_pru_wait_event(PRU_EVTOUT_0);
  printf("PRU program completed with: %d\n", nRet);

  // clear the event
  if (prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT))
    fprintf(stderr, "prussdrv_pru_clear_event() failed\n");

  // check results
  if ((nRet = __check_results())) {
    fprintf(stderr, "__check_results() failed\n");
    exit(nRet);
  }

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
