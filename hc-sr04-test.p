/*
 * NXCTRL BeagleBone Black Control Library
 *
 * HC SR-04 Ultrasonic Sensor Test Program
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

#define TRIGGER_PIN (1<<2)
#define ECHO_PIN    (1<<3)

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
        // for trigger pin
        MOV     R2, TRIGGER_PIN
        MOV     R3, GPIO2 | GPIO_SETDATAOUT
        SBBO    R2, R3, 0, 4

        // delay count for 10 microseconds (200MHz/2 instruction = 10 ns per loop)
        // so, we need 1000 loops for 10 us
        MOV     R0, 1000
TRIGGER_DELAY_LOOP:
        SUB     R0, R0, 1
        QBNE    TRIGGER_DELAY_LOOP, R0, 0

        // make trigger pin low
        MOV     R2, TRIGGER_PIN
        MOV     R3, GPIO2 | GPIO_CLEARDATAOUT
        SBBO    R2, R3, 0, 4

        // wait for echo pin to low
        MOV     R3, GPIO2 | GPIO_DATAIN
ECHO_LOW_WAIT_LOOP:
        // read GPIO2 data
        LBBO    R2, R3, 0, 4
        
        // select echo pin
        AND     R2, R2, ECHO_PIN

        // check echo pin is low
        QBNE    ECHO_LOW_WAIT_LOOP, R2, 0

        // okay, now prepare for measure count
        MOV     R4, 0
START_MEASURE:
        // read GPIO2 data
        LBBO    R2, R3, 0, 4
        
        // select echo pin
        AND     R2, R2, ECHO_PIN

        // check measure completion
        QBNE    COMPLETE_MEASURE, R2, 0

        // delay 1us
        MOV     R0, 100
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
