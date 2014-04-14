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

// This program collects ADC from AIN0

#include "pru.hp"

#define TSCADC_REG_ADDR                 (0x44E0D000)
#define TSC_ADC_SS_CTRL                 (TSCADC_REG_ADDR + 0x040)
#define TSC_ADC_SS_ADC_CLKDIV           (TSCADC_REG_ADDR + 0x04C)
#define TSC_ADC_SS_STEPENABLE           (TSCADC_REG_ADDR + 0x054)
#define TSC_ADC_SS_STEPCONFIG1          (TSCADC_REG_ADDR + 0x064)
#define TSC_ADC_SS_IRQSTATUS            (TSCADC_REG_ADDR + 0x028)
#define TSC_ADC_SS_IRQENABLESET         (TSCADC_REG_ADDR + 0x02C)
#define TSC_ADC_SS_FIFO0DATA            (TSCADC_REG_ADDR + 0x100)

.ORIGIN     0
.ENTRYPOINT START

START:
        // memcpy(offset(R0), CONST_PRUCFG + 4(255), 4)
        LBCO    R0, CONST_PRUCFG, 4, 4
        // R0 = R0 & ~(1<<4(31) & 0x1F)
        CLR     R0, R0, 4
        // memcpy(CONST_PRUCFG + 4(255), offset(R0), 4)
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

        // disable ADC(for configuration) & make STEPCONFIG writable
        // store step id tag
        MOV     R0, TSC_ADC_SS_CTRL
        MOV     R1, 0x00000006
        SBBO    R1, R0, 0, 4

        // set ADC CLKDIV
        MOV     R0, TSC_ADC_SS_ADC_CLKDIV
        MOV     R1, 0x00000000
        SBBO    R1, R0, 0, 4

        // configure STEPCONFIG1 as SW enabled, oneshot, 16 avg
        // channel 1(AIN0) as input
        MOV     R0, TSC_ADC_SS_STEPCONFIG1
        MOV     R1, 0x00000010
        SBBO    R1, R0, 0, 4

        // enable STEPCONFIG1
        MOV     R0, TSC_ADC_SS_STEPENABLE
        MOV     R1, 0x00000002
        SBBO    R1, R0, 0, 4

        // clear IRQSTATUS
        MOV     R0, TSC_ADC_SS_IRQSTATUS
        MOV     R1, 0x000007FF
        SBBO    R1, R0, 0, 4

        // set IRQ for end of sequence
        MOV     R0, TSC_ADC_SS_IRQENABLESET
        MOV     R1, 0x00000002
        SBBO    R1, R0, 0, 4

        // enable ADC
        MOV     R0, TSC_ADC_SS_CTRL
        MOV     R1, 0x00000007
        SBBO    R1, R0, 0, 4

        MOV     R2, 50 // takes almost 46 for ADC reading

CHKIRQ:
        // check irq status for reading
        MOV     R0, TSC_ADC_SS_IRQSTATUS
        LBBO    R1, R0, 0, 4
        AND     R1, R1, 0x00000002
        SUB     R2, R2, 1
        QBEQ    EXIT, R2, 0
        QBNE    CHKIRQ, R1, 0x00000002

EXIT:
        MOV     R0, TSC_ADC_SS_IRQSTATUS
        LBBO    R1, R0, 0, 4
        SBBO    R1, R0, 0, 4

        // store irq status
        SBCO    R1, CONST_PRUSHAREDRAM, 4, 4
        // store exit count
        SBCO    R2, CONST_PRUSHAREDRAM, 8, 4

        MOV     R0, TSC_ADC_SS_FIFO0DATA
        LBBO    R1, R0, 0, 4
        SBCO    R1, CONST_PRUSHAREDRAM, 0, 4

        // finish
        MOV     R31.B0, PRU0_ARM_INTERRUPT+16
        HALT
