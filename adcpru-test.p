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

// This program collects ADC from AIN0 with certain sampling rate.
// The collected data are stored into PRU shared memory(buffer) first.
// The host program(ADCCollector.c) will read the stored ADC data
// This program uses double buffering technique. 
// The host program can recognize the buffer status by buffer status variable
// 0 means empty, 1 means first buffer is ready, 2 means second buffer is ready.
// When each buffer is ready, host program read ADC data from the buffer.

#include "pru.hp"

#define TSCADC_REG_ADDR                 (0x44E0D000)
#define TSC_ADC_SS_CTRL                 (TSCADC_REG_ADDR + 0x040)
#define TSC_ADC_SS_STEPENABLE           (TSCADC_REG_ADDR + 0x054)
#define TSC_ADC_SS_STEPCONFIG1          (TSCADC_REG_ADDR + 0x064)
#define TSC_ADC_SS_FIFO0DATA            (TSCADC_REG_ADDR + 0x100)

#define BUFF_SIZE        0x00000FA0
#define HALF_POS         BUFF_SIZE/2

#define SAMPLING_RATE    16000
#define DELAY_MICRO_SECS (1000000 / SAMPLING_RATE)
#define CLOCK            200000000 // PRU CLK = 200MHz
#define CLOCKS_PER_LOOP  2
#define DELAYCOUNT       DELAY_MICRO_SECS * CLOCK / CLOCKS_PER_LOOP / 1000 / 1000 * 3

.MACRO DELAY
        MOV     R10, DELAYCOUNT
DELAY:
        SUB     R10, R10, 1
        QBNE    DELAY, R10, 0
.ENDM

.MACRO READADC
        // initialize buffer status (0: empty, 1: 1st buffer ready, 2: 2nd buffer ready)
        MOV     R2, 0
        // memcpy(CONST_PRUSHAREDRAM, offset(R2), 4)
        SBCO    R2, CONST_PRUSHAREDRAM, 0, 4

INITV:
        MOV     R5, 0
        MOV     R6, BUFF_SIZE

READ:
        // read ADC data from FIFO0DATA
        MOV     R2, TSC_ADC_SS_FIFO0DATA
        LBBO    R3, R2, 0, 4
        // move address offset
        ADD     R5, R5, 4
        // write ADC data to PRU shared RAM
        // memcpy(CONST_PRUSHAREDRAM + R5, offset(R3), 4)
        SBCO    R3, CONST_PRUSHAREDRAM, R5, 4

        DELAY

        SUB     R6, R6, 4
        MOV     R2, HALF_POS
        QBEQ    B1RDY, R6, R2 // 1st buffer ready
        QBEQ    B2RDY, R6, 0  // 2nd buffer ready
        QBA     READ

B1RDY:
        MOV     R2, 1
        SBCO    R2, CONST_PRUSHAREDRAM, 0, 4
        QBA     READ

B2RDY:
        MOV     R2, 2
        SBCO    R2, CONST_PRUSHAREDRAM, 0, 4
        QBA     INITV

        // finish
        MOV     R31.B0, PRU0_ARM_INTERRUPT+16
        HALT
.ENDM

.ORIGIN     0
.ENTRYPOINT START

START:
        // memcpy(offset(R0), CONST_PRUCFG + 4(255), 4)
        LBCO    R0, CONST_PRUCFG, 4, 4
        // R0 = R0 & ~(1<<4(31) & 0x1F)
        CLR     R0, R0, 4
        // memcpy(CONST_PRUCFG + 4(255), offset(R0), 4)
        SBCO    R0, CONST_PRUCFG, 4, 4

        // enable ADC & make STEPCONFIG writable
        MOV     R0, TSC_ADC_SS_CTRL
        MOV     R1, 0x00000005
        SBBO    R1, R0, 0, 4

        // enable STEPCONFIG1
        MOV     R0, TSC_ADC_SS_STEPENABLE
        MOV     R1, 0x00000002
        SBBO    R1, R0, 0, 4

        // configure STEPCONFIG1 as SW enabled, continuous
        MOV     R0, TSC_ADC_SS_STEPCONFIG1
        MOV     R1, 0x00000001
        SBBO    R1, R0, 0, 4

        READADC

