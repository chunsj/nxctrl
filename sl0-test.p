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

// LED on/off using flag value from C driver program
// 2 byte flag data is used; one for pru firmware control, 
// the other for LED on/off.

#define LED_PIN   (1<<27)

.ORIGIN     0
.ENTRYPOINT START

START:
        // memcpy(offset(R0), CONST_PRUCFG + 4(255), 4)
        LBCO    R0, CONST_PRUCFG, 4, 4
        // R0 = R0 & ~(1<<4(31) & 0x1F)
        CLR     R0, R0, 4
        // memcpy(CONST_PRUCFG + 4(255), offset(R0), 4)
        SBCO    R0, CONST_PRUCFG, 4, 4

        // locate LED_PIN at R2
        MOV     R2, LED_PIN

LOOP1:
        // at first, RAM contains 2 bytes value of 1, 0
        LBCO    R1, CONST_PRUDRAM, 0, 2
        // exit if first byte is 0
        QBEQ    EXIT, R1.B0, 0

        // branch with the value at R2.B1(on/off flag)
        QBNE    LED_OFF, R1.B1, 1

        // flag is on, so light it on
        MOV     R3, GPIO0 | GPIO_SETDATAOUT
        SBBO    R2, R3, 0, 4
        QBA     LOOP1

        // flag is off, so light it off
LED_OFF:
        MOV     R3, GPIO0 | GPIO_CLEARDATAOUT
        SBBO    R2, R3, 0, 4
        QBA     LOOP1

EXIT:
        MOV     R31.B0, PRU0_ARM_INTERRUPT+16
        HALT
