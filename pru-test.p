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

#include "pru.hp"

.ORIGIN     0
.ENTRYPOINT START

// To signal the host that we're done, we set bit 5 in our R31
// simultaneously with putting the number of the signal we want
// into R31 bits 0-3. See 5.2.2.2 in AM335x PRU-ICSS Reference Guide.
#define PRU0_R31_VEC_VALID (1<<5)
#define SIGNUM 3 // corresponds to PRU_EVTOUT_0

#define DELAY_SECONDS 5 // adjust this to experiment
#define CLOCKS_PER_LOOP 2 // loop contains two instructions, one clock each
#define DELAYCOUNT DELAY_SECONDS * PRU_CLOCK / CLOCKS_PER_LOOP

START:
        // enable OCP master port
        LBCO    R0, CONST_PRUCFG, 4, 4
        CLR     R0, R0, 4              // clear SYSCFG[STANDBY_INIT] to enable OCP master port
        SBCO    R0, CONST_PRUCFG, 4, 4

        // initialize loop counter
        MOV     R1, DELAYCOUNT
        
        // MOV   R2, 7<<22 => 7 is 111 and which means USR1,2,3 one in web example
        // here, I only interested in P8_17 which is GPIO0[27]
        MOV     R2, 1<<27
        MOV     R3, GPIO0 | GPIO_SETDATAOUT
        SBBO    R2, R3, 0, 4

        // wait for specified period of time
DELAY:
	SUB	R1, R1, 1     // decrement loop counter
	QBNE	DELAY, R1, 0  // repeat loop unless zero
        
        MOV     R2, 1<<27
        MOV     R3, GPIO0 | GPIO_CLEARDATAOUT
        SBBO    R2, R3, 0, 4

        //    tell host we're done, then halt
	MOV	R31.B0, PRU0_R31_VEC_VALID | SIGNUM
	HALT
