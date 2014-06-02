/*
 * NXCTRL BeagleBone Black Control Library
 *
 * HC SR-04 Ultrasonic Sensor Program for CTRL APP
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

//
// XXX instead of using GPIO pin, pin mux them as GPO/GPI,
// which can make more precise timing (untested and not validated)
// P8 PIN11 (R30.15(OUT) as TRIGGER_PIN)
// P8 PIN15 (R31.15(IN) as ECHO PIN)
//

.ORIGIN     0
.ENTRYPOINT START

START:
        // memcpy(offset(R0), CONST_PRUCFG + 4(255), 4)
        LBCO    R0, CONST_PRUCFG, 4, 4
        // R0 = R0 & ~(1<<4(31) & 0x1F)
        CLR     R0, R0, 4
        // memcpy(CONST_PRUCFG + 4(255), offset(R0), 4)
        SBCO    R0, CONST_PRUCFG, 4, 4

TRIGGER:
        // make trigger pin low
        CLR     R30.T15

        // wait for 2 microseconds
        MOV     R0, 200
TRIGGER_LOW_LOOP:
        SUB     R0, R0, 1
        QBNE    TRIGGER_LOW_LOOP, R0, 0

        // make trigger pin high
        SET     R30.T15

        // delay count for 10 microseconds (200MHz/2 instruction = 10 ns per loop)
        // so, we need 1000 loops for 10 us
        MOV     R0, 1000
TRIGGER_DELAY_LOOP:
        SUB     R0, R0, 1
        QBNE    TRIGGER_DELAY_LOOP, R0, 0

        // make trigger pin low
        CLR     R30.T15

        // max loop count check
        MOV     R5, 5000000
ECHO_HIGH_WAIT_LOOP:
        // check max iter count
        SUB     R5, R5, 1
        MOV     R4, 0
        QBEQ    COMPLETE_MEASURE, R5, 0
        // check echo pin is high
        MOV     R2, R31
        AND     R2, R2.B1, 1<<7 // 15th bit
        QBNE    ECHO_HIGH_WAIT_LOOP, R2, 128 // 15th bit on

        // okay, now prepare for measure count
        MOV     R4, 0
START_MEASURE:
        // check measure completion
        MOV     R2, R31
        AND     R2, R2.B1, 1<<7 // 15th bit
        QBNE    COMPLETE_MEASURE, R2, 128 // 15th bit on

        // delay 1us, XXX how to consider above 3 instruction or 15ns into this?
        MOV     R0, 99
ECHO_WAIT_LOOP:
        SUB     R0, R0, 1
        QBNE    ECHO_WAIT_LOOP, R0, 0
        // add measure count
        ADD     R4, R4, 1
        // continue sample
        JMP     START_MEASURE

COMPLETE_MEASURE:
        // store result
        SBCO    R4, CONST_PRUDRAM, 0, 4
        // interrupt
        MOV     R31.B0, PRU0_ARM_INTERRUPT+16

        // wait for sonar signal delay
        MOV     R0, 5000000
DECAY_LOOP:
        SUB     R0, R0, 1
        QBNE    DECAY_LOOP, R0, 0

        JMP     TRIGGER
