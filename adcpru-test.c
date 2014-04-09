/*
 * NXCTRL BeagleBone Black Control Library
 *
 * TSCADC from PRU Test Program - modified from Youngtae Jo's code
 *
 * Copyright (C) 2014 Sungjin Chun <chunsj@gmail.com>
 *           (C) 2014 Youngtae Jo <youngtaejo@gmail.com>
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
#define PRU_PATH   "./adcpru-test.bin"

#define OFFSET_SHAREDRAM       2048
#define SAMPLING_PERIOD_SEC    4
#define SAMPLING_RATE          16000 // 16kHz
#define BUFF_LENGTH            SAMPLING_RATE
#define PRU_SHARED_BUFF_SIZE   500
#define CNT_ONE_SEC            SAMPLING_RATE / PRU_SHARED_BUFF_SIZE

NXCTRL_VOID
NXCTRLSetup (NXCTRL_VOID) {
  int nRet, nTargetBuf = 1;
  int i, nCnt = 0, nTotalCnt = 0;
  void *pSharedMem;
  unsigned int *pnSharedMem;
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

  // locate PRU shared RAM
  if ((nRet = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &pSharedMem))) {
    fprintf(stderr, "prussdrv_map_prumem() failed\n");
    exit(nRet);
  }

  pnSharedMem = (unsigned int *)pSharedMem;

  // load and run the PRU program
  if ((nRet = prussdrv_exec_program(PRU_NUM, PRU_PATH))) {
    fprintf(stderr, "prussdrv_exec_program() failed\n");
    exit(nRet);
  }

#if 1
  // process reading ADC
  while (1) {
    while (1) {
      if (pnSharedMem[OFFSET_SHAREDRAM] == 1 && nTargetBuf == 1) { // 1st buffer ready
        for (i = 0; i < PRU_SHARED_BUFF_SIZE; i++)
          printf("%d\n", pnSharedMem[OFFSET_SHAREDRAM + i + 1] & 0xFFF);
        nTargetBuf = 2;
        break;
      } else if (pnSharedMem[OFFSET_SHAREDRAM] == 2 && nTargetBuf == 2) { // 2nd buffer ready
        for (i = 0; i < PRU_SHARED_BUFF_SIZE; i++) 
          printf("%d\n", pnSharedMem[OFFSET_SHAREDRAM + PRU_SHARED_BUFF_SIZE + i + 1] & 0xFFF);
        nTargetBuf = 1;
        break;
      }
    }

    if (nCnt++ == CNT_ONE_SEC) {
      printf(".");
      nTotalCnt += nCnt;
      nCnt = 0;
    }

    if (nTotalCnt == CNT_ONE_SEC * SAMPLING_PERIOD_SEC) {
      printf("Sampling Completed\n");
      break;
    }
  }
#endif

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
