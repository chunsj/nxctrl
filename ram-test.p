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

.origin 0 // offset of the start of the code in PRU memory
.entrypoint START // program entry point, used by debugger only

START:
        // setup OCP and shared mem
        LBCO    R0, CONST_PRUCFG, 4, 4
        // clear SYSCFG[STANDBY_INIT] to enable OCP master port
        CLR     R0, R0, 4              
        SBCO    R0, CONST_PRUCFG, 4, 4

        // configure the programmable pointer register for PRU0 
        // by setting c28_pointer[15:0] field to 0x0120. this will
        // make C28 point to 0x00012000 (PRU shared RAM).
        MOV     R0, 0x00000120
        MOV     R1, CTPPR_0
        SBBO    R0, R1, 0, 1

        // configure the programmable pointer register for PRU0 
        // by setting c31_pointer[15:0] field to 0x0010. this will
        // make C31 point to 0x80001000 (DDR memory).
        MOV     R0, 0x00100000
        MOV     R1, CTPPR_1
        SBBO    R0, R1, 0, 1

        // load values from external DDR memory into registers R0/R1/R2
        LBCO    R0, CONST_DDR, 0, 12

        // store values from read from the DDR memory into PRU shared RAM
        SBCO    R0, CONST_PRUSHAREDRAM, 0, 12

        // send notification to host for program completion
        MOV     R31.B0, PRU0_ARM_INTERRUPT+16

        // halt the processor
        HALT
