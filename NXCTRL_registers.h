/*
 * NXCTRL BeagleBone Black Control Library
 *
 * Copyright (C) 2014 Sungjin Chun <chunsj@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#ifndef __NXCTRL_REGISTERS_HEADER__
#define __NXCTRL_REGISTERS_HEADER__

#include <NXCTRL_bits.h>

#define GPIO0                           (0)
#define GPIO1                           (1)
#define GPIO2                           (2)
#define GPIO3                           (3)

#define GPIO0_REG_ADDR                  (0x44E07000)
#define GPIO0_REG_SIZE                  (0x44E07FFF-GPIO0_REG_ADDR+1)
#define GPIO1_REG_ADDR                  (0x4804C000)
#define GPIO1_REG_SIZE                  (0x4804CFFF-GPIO1_REG_ADDR+1)
#define GPIO2_REG_ADDR                  (0x481AC000)
#define GPIO2_REG_SIZE                  (0x481ACFFF-GPIO2_REG_ADDR+1)
#define GPIO3_REG_ADDR                  (0x481AE000)
#define GPIO3_REG_SIZE                  (0x481AEFFF-GPIO3_REG_ADDR+1)

#define GPIO_OE                         0x134      // 25.4.1.16
#define GPIO_SETDATAOUT                 0x194      // 25.4.1.26
#define GPIO_CLEARDATAOUT               0x190      // 25.4.1.25
#define GPIO_DATAIN                     0x138      // 25.4.1.17
#define GPIO_DEBOUNCENABLE              0x150      // 25.4.1.23
#define GPIO_DEBOUNCINGTIME             0x154      // 25.4.1.24

#define CONTROL_REG_ADDR                (0x44E10000)
#define CONTROL_REG_SIZE                (0x44E11FFF-CONTROL_REG_ADDR+1)

#define TSCADC_REG_ADDR                 (0x44E0D000)
#define TSCADC_REG_SIZE                 (0x44E0EFFF-TSCADC_REG_ADDR+1)

#define CM_PER_REG_ADDR                 (0x44E00000)
#define CM_PER_REG_SIZE                 (0x44E03FFF-CM_PER_REG_ADDR+1)
#define CM_WKUP_REG_ADDR                (0x44E00400)
#define CM_WKUP_REG_OFFSET              (CM_WKUP_REG_ADDR-CM_PER_REG_ADDR)

#define PWMSS0_REG_ADDR                 (0x48300000)
#define PWMSS0_REG_SIZE                 (0x48301FFF-PWMSS0_REG_ADDR+1)
#define PWMSS1_REG_ADDR                 (0x48302000)
#define PWMSS1_REG_SIZE                 (0x48303FFF-PWMSS1_REG_ADDR+1)
#define PWMSS2_REG_ADDR                 (0x48304000)
#define PWMSS2_REG_SIZE                 (0x48305FFF-PWMSS2_REG_ADDR+1)

#define CM_PER_L3S_CLKCTRL              (0x004)
#define CM_PER_L3_CLKSTCTRL             (0x00C)
#define CM_PER_L3_INSTR_CLKCTRL         (0x0DC)
#define CM_PER_L3_CLKCTRL               (0x0E0)
#define CM_PER_OCPWP_L3_CLKSTCTRL       (0x12C)
#define CM_PER_L4LS_CLKSTCTRL           (0x000)
#define CM_PER_L4LS_CLKCTRL             (0x060)

#define CM_WKUP_CLKSTCTRL               (0x000)
#define CM_WKUP_ADC_TSC_CLKCTRL         (0x0BC)

#define CM_WKUP_GPIO0_CLKCTRL           (0x008)
#define CM_PER_GPIO1_CLKCTRL            (0x0AC)
#define CM_PER_GPIO2_CLKCTRL            (0x0B0)
#define CM_PER_GPIO3_CLKCTRL            (0x0B4)

#define CM_PER_EPWMSS0_CLKCTRL          (0x0D4)
#define CM_PER_EPWMSS1_CLKCTRL          (0x0CC)
#define CM_PER_EPWMSS2_CLKCTRL          (0x0D8)

#define TSC_ADC_SS_CTRL                 (0x040)
#define TSC_ADC_SS_ADC_CLKDIV           (0x04C)
#define TSC_ADC_SS_STEPCONFIG(n)        (0x064+((n)*0x008))
#define TSC_ADC_SS_STEPDELAY(n)         (0x068+((n)*0x008))
#define TSC_ADC_SS_STEPENABLE           (0x054)
#define TSC_ADC_SS_IRQSTATUS            (0x028)
#define TSC_ADC_SS_IRQENABLE_SET        (0x02C)
#define TSC_ADC_SS_FIFODATA(n)          (0x100 + ((n)*0x100))

#define PWMSS_CTRL                      (0x664)
#define PWMSS_CLKCONFIG                 (0x008)

#define PWMSS_ECAP                      (0x100)
#define PWMSS_EQEP                      (0x180)
#define PWMSS_EPWM                      (0x200)

#define EPWM_TBCTL                      (0x000)
#define EPWM_TBPRD                      (0x00A)
#define EPWM_CMPCTL                     (0x00E)
#define EPWM_CMPA                       (0x012)
#define EPWM_CMPB                       (0x014)
#define EPWM_AQCTLA                     (0x016)
#define EPWM_AQCTLB                     (0x018)
#define EPWM_AQSFRC                     (0x01A)
#define EPWM_AQCSFRC                    (0x01C)

#define REGADDR(base,offset)                    ((base)+(offset))
#define U32REG(base,offset)                     *((volatile NXCTRL_UINT32 *)REGADDR(base,offset))
#define U16REG(base,offset)                     *((volatile NXCTRL_UINT16 *)REGADDR(base,offset))
#define ASU32REG(p)                             *((volatile NXCTRL_UINT32 *)(p))
#define ASU16REG(p)                             *((volatile NXCTRL_UINT16 *)(p))

#define U32REG_CM_WKUP_GPIO0_CLKCTRL            U32REG(__CM_WKUP_ADDR,CM_WKUP_GPIO0_CLKCTRL)
#define U32REG_CM_PER_GPIO1_CLKCTRL             U32REG(__CM_PER_ADDR,CM_PER_GPIO1_CLKCTRL)
#define U32REG_CM_PER_GPIO2_CLKCTRL             U32REG(__CM_PER_ADDR,CM_PER_GPIO2_CLKCTRL)
#define U32REG_CM_PER_GPIO3_CLKCTRL             U32REG(__CM_PER_ADDR,CM_PER_GPIO3_CLKCTRL)

#define ENABLE_GPIO0_CLK(f)                     (f) ? (U32REG_CM_WKUP_GPIO0_CLKCTRL |= BIT2) : (U32REG_CM_WKUP_GPIO0_CLKCTRL &= ~BIT2)
#define ENABLE_GPIO1_CLK(f)                     (f) ? (U32REG_CM_PER_GPIO1_CLKCTRL |= BIT2) : (U32REG_CM_PER_GPIO1_CLKCTRL &= ~BIT2)
#define ENABLE_GPIO2_CLK(f)                     (f) ? (U32REG_CM_PER_GPIO2_CLKCTRL |= BIT2) : (U32REG_CM_PER_GPIO2_CLKCTRL &= ~BIT2)
#define ENABLE_GPIO3_CLK(f)                     (f) ? (U32REG_CM_PER_GPIO3_CLKCTRL |= BIT2) : (U32REG_CM_PER_GPIO3_CLKCTRL &= ~BIT2)

#define ENABLE_GPIO0_OPTFCLK(f)                 (f) ? (U32REG_CM_WKUP_GPIO0_CLKCTRL |= BIT18) : (U32REG_CM_WKUP_GPIO0_CLKCTRL &= ~BIT18)
#define ENABLE_GPIO1_OPTFCLK(f)                 (f) ? (U32REG_CM_PER_GPIO1_CLKCTRL |= BIT18) : (U32REG_CM_PER_GPIO1_CLKCTRL &= ~BIT18)
#define ENABLE_GPIO2_OPTFCLK(f)                 (f) ? (U32REG_CM_PER_GPIO2_CLKCTRL |= BIT18) : (U32REG_CM_PER_GPIO2_CLKCTRL &= ~BIT18)
#define ENABLE_GPIO3_OPTFCLK(f)                 (f) ? (U32REG_CM_PER_GPIO3_CLKCTRL |= BIT18) : (U32REG_CM_PER_GPIO3_CLKCTRL &= ~BIT18)

#define U32REG_GPIO0_SETDATAOUT                 U32REG(__GPIO0_ADDR,GPIO_SETDATAOUT)
#define U32REG_GPIO1_SETDATAOUT                 U32REG(__GPIO1_ADDR,GPIO_SETDATAOUT)
#define U32REG_GPIO2_SETDATAOUT                 U32REG(__GPIO2_ADDR,GPIO_SETDATAOUT)
#define U32REG_GPIO3_SETDATAOUT                 U32REG(__GPIO3_ADDR,GPIO_SETDATAOUT)
#define U32REG_GPIO0_CLEARDATAOUT               U32REG(__GPIO0_ADDR,GPIO_CLEARDATAOUT)
#define U32REG_GPIO1_CLEARDATAOUT               U32REG(__GPIO1_ADDR,GPIO_CLEARDATAOUT)
#define U32REG_GPIO2_CLEARDATAOUT               U32REG(__GPIO2_ADDR,GPIO_CLEARDATAOUT)
#define U32REG_GPIO3_CLEARDATAOUT               U32REG(__GPIO3_ADDR,GPIO_CLEARDATAOUT)
#define U32REG_GPIO0_DATAIN                     U32REG(__GPIO0_ADDR,GPIO_DATAIN)
#define U32REG_GPIO1_DATAIN                     U32REG(__GPIO1_ADDR,GPIO_DATAIN)
#define U32REG_GPIO2_DATAIN                     U32REG(__GPIO2_ADDR,GPIO_DATAIN)
#define U32REG_GPIO3_DATAIN                     U32REG(__GPIO3_ADDR,GPIO_DATAIN)

#define U32REG_CM_WKUP_ADC_TSC_CLKCTRL          U32REG(__CM_WKUP_ADDR,CM_WKUP_ADC_TSC_CLKCTRL)

#define ENABLE_ADC_TSC_CLK(f)                   (f) ? (U32REG_CM_WKUP_ADC_TSC_CLKCTRL |= BIT2) : (U32REG_CM_WKUP_ADC_TSC_CLKCTRL &= ~BIT2)

#define U32REG_CM_PER_L3S_CLKCTRL               U32REG(__CM_PER_ADDR,CM_PER_L3S_CLKCTRL)
#define U32REG_CM_PER_L3_CLKSTCTRL              U32REG(__CM_PER_ADDR,CM_PER_L3_CLKSTCTRL)
#define U32REG_CM_L3_INSTR_CLKCTRL              U32REG(__CM_PER_ADDR,CM_PER_L3_INSTR_CLKCTRL)
#define U32REG_CM_L3_CLKCTRL                    U32REG(__CM_PER_ADDR,CM_PER_L3_CLKCTRL)
#define U32REG_CM_PER_OCPWP_L3_CLKSTCTRL        U32REG(__CM_PER_ADDR,CM_PER_OCPWP_L3_CLKSTCTRL)
#define U32REG_CM_PER_L4LS_CLKSTCTRL            U32REG(__CM_PER_ADDR,CM_PER_L4LS_CLKSTCTRL)
#define U32REG_CM_PER_L4LS_CLKCTRL              U32REG(__CM_PER_ADDR,CM_PER_L4LS_CLKCTRL)

#define U32REG_TSC_ADC_SS_CTRL                  U32REG(__TSCADC_ADDR,TSC_ADC_SS_CTRL)
#define U32REG_TSC_ADC_SS_ADC_CLKDIV            U32REG(__TSCADC_ADDR,TSC_ADC_SS_ADC_CLKDIV)
#define U32REG_TSC_ADC_SS_STEPCONFIG(n)         U32REG(__TSCADC_ADDR,TSC_ADC_SS_STEPCONFIG(n))
#define U32REG_TSC_ADC_SS_STEPDELAY(n)          U32REG(__TSCADC_ADDR,TSC_ADC_SS_STEPDELAY(n))
#define U32REG_TSC_ADC_SS_STEPENABLE            U32REG(__TSCADC_ADDR,TSC_ADC_SS_STEPENABLE)
#define U32REG_TSC_ADC_SS_IRQSTATUS             U32REG(__TSCADC_ADDR,TSC_ADC_SS_IRQSTATUS)
#define U32REG_TSC_ADC_SS_IRQENABLE_SET         U32REG(__TSCADC_ADDR,TSC_ADC_SS_IRQENABLE_SET)
#define U32REG_TSC_ADC_SS_FIFODATA(n)           U32REG(__TSCADC_ADDR,TSC_ADC_SS_FIFODATA(n))

#define ENABLE_TSC_ADC_SS_MODULE(f)             (f) ? (U32REG_TSC_ADC_SS_CTRL |= BIT0) : (U32REG_TSC_ADC_SS_CTRL &= ~BIT0)
#define ENABLE_TSC_ADC_SS_STEP_ID_TAG(f)        (f) ? (U32REG_TSC_ADC_SS_CTRL |= BIT1) : (U32REG_TSC_ADC_SS_CTRL &= ~BIT1)
#define ENABLE_TSC_ADC_SS_STEP_CONFIG_WRITE(f)  (f) ? (U32REG_TSC_ADC_SS_CTRL |= BIT2) : (U32REG_TSC_ADC_SS_CTRL &= ~BIT2)
#define SET_TSC_ADC_SS_ADC_CLKDIV(n)            U32REG_TSC_ADC_SS_ADC_CLKDIV = (0x00007FFF & ((n)-1))
#define SET_TSC_ADC_SS_STEPCONFIG_MODE(n,v)     U32REG_TSC_ADC_SS_STEPCONFIG(n) &= ~(BIT0|BIT1); U32REG_TSC_ADC_SS_STEPCONFIG(n) |= ((v)<<0)
#define SET_TSC_ADC_SS_STEPCONFIG_INP(n,p)      U32REG_TSC_ADC_SS_STEPCONFIG(n) &= ~(BIT19|BIT20|BIT21|BIT22); U32REG_TSC_ADC_SS_STEPCONFIG(n) |= ((p)<<19)
#define ENABLE_TSC_ADC_SS_STEPCONFIG_DIFF(n,f)  (f) ? (U32REG_TSC_ADC_SS_STEPCONFIG(n) |= BIT25) : (U32REG_TSC_ADC_SS_STEPCONFIG(n) &= ~BIT25)
#define SET_TSC_ADC_SS_STEPCONFIG_FIFO(n,f)     (f) ? (U32REG_TSC_ADC_SS_STEPCONFIG(n) |= BIT26) : (U32REG_TSC_ADC_SS_STEPCONFIG(n) &= ~BIT26)
#define SET_TSC_ADC_SS_STEPCONFIG_AVG(n,m)      U32REG_TSC_ADC_SS_STEPCONFIG(n) &= ~(BIT2|BIT3|BIT4); U32REG_TSC_ADC_SS_STEPCONFIG(n) |= ((m)<<2)
#define SET_TSC_ADC_SS_STEPENABLE(n, f)         U32REG_TSC_ADC_SS_STEPENABLE &= ~(1<<((n)+1)); U32REG_TSC_ADC_SS_STEPENABLE |= ((f)<<((n)+1))
#define CLEAR_TSC_ADC_SS_IRQSTATUS              U32REG_TSC_ADC_SS_IRQSTATUS = 0x7FF
#define SET_TSC_ADC_SS_IRQENABLE_SET(n)         U32REG_TSC_ADC_SS_IRQENABLE_SET |= (n)
#define SET_TSC_ADC_SS_STEPDELAY_SAMPLE(n,m)    U32REG_TSC_ADC_SS_STEPDELAY(n) &= ~(BIT24|BIT25|BIT26|BIT27|BIT28|BIT29|BIT30|BIT31); U32REG_TSC_ADC_SS_STEPDELAY(n) |= ((m)<<24)
#define SET_TSC_ADC_SS_STEPDELAY_OPEN(n,m)      U32REG_TSC_ADC_SS_STEPDELAY(n) &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7|BIT8|BIT9|BIT10|BIT11|BIT12|BIT13|BIT14|BIT15|BIT16|BIT17); U32REG_TSC_ADC_SS_STEPDELAY(n) |= ((m)<<0)

#define U32REG_CM_PER_EPWMSS0_CLKCTRL           U32REG(__CM_PER_ADDR,CM_PER_EPWMSS0_CLKCTRL)
#define U32REG_CM_PER_EPWMSS1_CLKCTRL           U32REG(__CM_PER_ADDR,CM_PER_EPWMSS1_CLKCTRL)
#define U32REG_CM_PER_EPWMSS2_CLKCTRL           U32REG(__CM_PER_ADDR,CM_PER_EPWMSS2_CLKCTRL)

#define ENABLE_EPWMSS0_CLK(f)                   (f) ? (U32REG_CM_PER_EPWMSS0_CLKCTRL |= BIT2) : (U32REG_CM_PER_EPWMSS0_CLKCTRL &= ~BIT2)
#define ENABLE_EPWMSS1_CLK(f)                   (f) ? (U32REG_CM_PER_EPWMSS1_CLKCTRL |= BIT2) : (U32REG_CM_PER_EPWMSS1_CLKCTRL &= ~BIT2)
#define ENABLE_EPWMSS2_CLK(f)                   (f) ? (U32REG_CM_PER_EPWMSS2_CLKCTRL |= BIT2) : (U32REG_CM_PER_EPWMSS2_CLKCTRL &= ~BIT2)

#define U32REG_PWMSS0_CLKCONFIG                 U32REG(__PWMSS0_ADDR,PWMSS_CLKCONFIG)
#define U32REG_PWMSS1_CLKCONFIG                 U32REG(__PWMSS1_ADDR,PWMSS_CLKCONFIG)
#define U32REG_PWMSS2_CLKCONFIG                 U32REG(__PWMSS2_ADDR,PWMSS_CLKCONFIG)

#define ENABLE_PWMSS0_CLK(f)                    (f) ? (U32REG_PWMSS0_CLKCONFIG |= (BIT0|BIT4|BIT8)) : (U32REG_PWMSS0_CLKCONFIG &= ~(BIT0|BIT4|BIT8))
#define ENABLE_PWMSS1_CLK(f)                    (f) ? (U32REG_PWMSS1_CLKCONFIG |= (BIT0|BIT4|BIT8)) : (U32REG_PWMSS1_CLKCONFIG &= ~(BIT0|BIT4|BIT8))
#define ENABLE_PWMSS2_CLK(f)                    (f) ? (U32REG_PWMSS2_CLKCONFIG |= (BIT0|BIT4|BIT8)) : (U32REG_PWMSS2_CLKCONFIG &= ~(BIT0|BIT4|BIT8))

#define U16REG_EPWMSS0_TBCTL                    U16REG(__PWMSS0_ADDR+PWMSS_EPWM,EPWM_TBCTL)
#define U16REG_EPWMSS1_TBCTL                    U16REG(__PWMSS1_ADDR+PWMSS_EPWM,EPWM_TBCTL)
#define U16REG_EPWMSS2_TBCTL                    U16REG(__PWMSS2_ADDR+PWMSS_EPWM,EPWM_TBCTL)

#define SET_EPWMSS0_TBCTL_CLKDIV(n)             U16REG_EPWMSS0_TBCTL &= ~(BIT10|BIT11|BIT12); U16REG_EPWMSS0_TBCTL |= ((n)<<10)
#define SET_EPWMSS0_TBCTL_HSPCLKDIV(n)          U16REG_EPWMSS0_TBCTL &= ~(BIT7|BIT8|BIT9); U16REG_EPWMSS0_TBCTL |= ((n)<<7)
#define SET_EPWMSS1_TBCTL_CLKDIV(n)             U16REG_EPWMSS1_TBCTL &= ~(BIT10|BIT11|BIT12); U16REG_EPWMSS1_TBCTL |= ((n)<<10)
#define SET_EPWMSS1_TBCTL_HSPCLKDIV(n)          U16REG_EPWMSS1_TBCTL &= ~(BIT7|BIT8|BIT9); U16REG_EPWMSS1_TBCTL |= ((n)<<7)
#define SET_EPWMSS2_TBCTL_CLKDIV(n)             U16REG_EPWMSS2_TBCTL &= ~(BIT10|BIT11|BIT12); U16REG_EPWMSS2_TBCTL |= ((n)<<10)
#define SET_EPWMSS2_TBCTL_HSPCLKDIV(n)          U16REG_EPWMSS2_TBCTL &= ~(BIT7|BIT8|BIT9); U16REG_EPWMSS2_TBCTL |= ((n)<<7)

#define SET_EPWMSS0_TBCTL_CTRMODE(n)            U16REG_EPWMSS0_TBCTL &= ~(BIT0|BIT1); U16REG_EPWMSS0_TBCTL |= ((n)<<0)
#define SET_EPWMSS1_TBCTL_CTRMODE(n)            U16REG_EPWMSS1_TBCTL &= ~(BIT0|BIT1); U16REG_EPWMSS1_TBCTL |= ((n)<<0)
#define SET_EPWMSS2_TBCTL_CTRMODE(n)            U16REG_EPWMSS2_TBCTL &= ~(BIT0|BIT1); U16REG_EPWMSS2_TBCTL |= ((n)<<0)

#define U16REG_EPWMSS0_TBPRD                    U16REG(__PWMSS0_ADDR+PWMSS_EPWM,EPWM_TBPRD)
#define U16REG_EPWMSS1_TBPRD                    U16REG(__PWMSS1_ADDR+PWMSS_EPWM,EPWM_TBPRD)
#define U16REG_EPWMSS2_TBPRD                    U16REG(__PWMSS2_ADDR+PWMSS_EPWM,EPWM_TBPRD)

#define SET_EPWMSS0_TBPRD(n)                    U16REG_EPWMSS0_TBPRD = (n)
#define SET_EPWMSS1_TBPRD(n)                    U16REG_EPWMSS1_TBPRD = (n)
#define SET_EPWMSS2_TBPRD(n)                    U16REG_EPWMSS2_TBPRD = (n)

#define U16REG_EPWMSS0_CMPCTL                   U16REG(__PWMSS0_ADDR+PWMSS_EPWM,EPWM_CMPCTL)
#define U16REG_EPWMSS1_CMPCTL                   U16REG(__PWMSS1_ADDR+PWMSS_EPWM,EPWM_CMPCTL)
#define U16REG_EPWMSS2_CMPCTL                   U16REG(__PWMSS2_ADDR+PWMSS_EPWM,EPWM_CMPCTL)

#define SET_EPWMSS0_CMPCTRL_LOADAMODE(n)        U16REG_EPWMSS0_CMPCTL &= ~(BIT0|BIT1); U16REG_EPWMSS0_CMPCTL |= ((n)<<0)
#define SET_EPWMSS0_CMPCTRL_LOADBMODE(n)        U16REG_EPWMSS0_CMPCTL &= ~(BIT2|BIT3); U16REG_EPWMSS0_CMPCTL |= ((n)<<2)
#define SET_EPWMSS1_CMPCTRL_LOADAMODE(n)        U16REG_EPWMSS1_CMPCTL &= ~(BIT0|BIT1); U16REG_EPWMSS1_CMPCTL |= ((n)<<0)
#define SET_EPWMSS1_CMPCTRL_LOADBMODE(n)        U16REG_EPWMSS1_CMPCTL &= ~(BIT2|BIT3); U16REG_EPWMSS1_CMPCTL |= ((n)<<2)
#define SET_EPWMSS2_CMPCTRL_LOADAMODE(n)        U16REG_EPWMSS2_CMPCTL &= ~(BIT0|BIT1); U16REG_EPWMSS2_CMPCTL |= ((n)<<0)
#define SET_EPWMSS2_CMPCTRL_LOADBMODE(n)        U16REG_EPWMSS2_CMPCTL &= ~(BIT2|BIT3); U16REG_EPWMSS2_CMPCTL |= ((n)<<2)

#define ENABLE_EPWMSS0_CMPCTL_SHDWAMODE(f)      (f) ? (U16REG_EPWMSS0_CMPCTL &= ~(BIT4)) : (U16REG_EPWMSS0_CMPCTL |= BIT4)
#define ENABLE_EPWMSS0_CMPCTL_SHDWBMODE(f)      (f) ? (U16REG_EPWMSS0_CMPCTL &= ~(BIT6)) : (U16REG_EPWMSS0_CMPCTL |= BIT6)
#define ENABLE_EPWMSS1_CMPCTL_SHDWAMODE(f)      (f) ? (U16REG_EPWMSS1_CMPCTL &= ~(BIT4)) : (U16REG_EPWMSS1_CMPCTL |= BIT4)
#define ENABLE_EPWMSS1_CMPCTL_SHDWBMODE(f)      (f) ? (U16REG_EPWMSS1_CMPCTL &= ~(BIT6)) : (U16REG_EPWMSS1_CMPCTL |= BIT6)
#define ENABLE_EPWMSS2_CMPCTL_SHDWAMODE(f)      (f) ? (U16REG_EPWMSS2_CMPCTL &= ~(BIT4)) : (U16REG_EPWMSS2_CMPCTL |= BIT4)
#define ENABLE_EPWMSS2_CMPCTL_SHDWBMODE(f)      (f) ? (U16REG_EPWMSS2_CMPCTL &= ~(BIT6)) : (U16REG_EPWMSS2_CMPCTL |= BIT6)

#define U16REG_EPWMSS0_CMPA                     U16REG(__PWMSS0_ADDR+PWMSS_EPWM,EPWM_CMPA)
#define U16REG_EPWMSS0_CMPB                     U16REG(__PWMSS0_ADDR+PWMSS_EPWM,EPWM_CMPB)
#define U16REG_EPWMSS1_CMPA                     U16REG(__PWMSS1_ADDR+PWMSS_EPWM,EPWM_CMPA)
#define U16REG_EPWMSS1_CMPB                     U16REG(__PWMSS1_ADDR+PWMSS_EPWM,EPWM_CMPB)
#define U16REG_EPWMSS2_CMPA                     U16REG(__PWMSS2_ADDR+PWMSS_EPWM,EPWM_CMPA)
#define U16REG_EPWMSS2_CMPB                     U16REG(__PWMSS2_ADDR+PWMSS_EPWM,EPWM_CMPB)

#define SET_EPWMSS0_CMPA(n)                     U16REG_EPWMSS0_CMPA = (n) & 0xFFFF
#define SET_EPWMSS0_CMPB(n)                     U16REG_EPWMSS0_CMPB = (n) & 0xFFFF
#define SET_EPWMSS1_CMPA(n)                     U16REG_EPWMSS1_CMPA = (n) & 0xFFFF
#define SET_EPWMSS1_CMPB(n)                     U16REG_EPWMSS1_CMPB = (n) & 0xFFFF
#define SET_EPWMSS2_CMPA(n)                     U16REG_EPWMSS2_CMPA = (n) & 0xFFFF
#define SET_EPWMSS2_CMPB(n)                     U16REG_EPWMSS2_CMPB = (n) & 0xFFFF

#define U16REG_EPWMSS0_AQCTLA                   U16REG(__PWMSS0_ADDR+PWMSS_EPWM,EPWM_AQCTLA)
#define U16REG_EPWMSS0_AQCTLB                   U16REG(__PWMSS0_ADDR+PWMSS_EPWM,EPWM_AQCTLB)
#define U16REG_EPWMSS1_AQCTLA                   U16REG(__PWMSS1_ADDR+PWMSS_EPWM,EPWM_AQCTLA)
#define U16REG_EPWMSS1_AQCTLB                   U16REG(__PWMSS1_ADDR+PWMSS_EPWM,EPWM_AQCTLB)
#define U16REG_EPWMSS2_AQCTLA                   U16REG(__PWMSS2_ADDR+PWMSS_EPWM,EPWM_AQCTLA)
#define U16REG_EPWMSS2_AQCTLB                   U16REG(__PWMSS2_ADDR+PWMSS_EPWM,EPWM_AQCTLB)

#define SET_EPWMSS0_AQCTLA(z,p,au,ad,bu,bd)     U16REG_EPWMSS0_AQCTLA = ((bd<<10)&(BIT10|BIT11))|((bu<<8)&(BIT8|BIT9))|((ad<<6)&(BIT6|BIT7))|((au<<4)&(BIT4|BIT5))|((p<<2)&(BIT2|BIT3))|((z<<0)&(BIT0|BIT1))
#define SET_EPWMSS0_AQCTLB(z,p,au,ad,bu,bd)     U16REG_EPWMSS0_AQCTLB = ((bd<<10)&(BIT10|BIT11))|((bu<<8)&(BIT8|BIT9))|((ad<<6)&(BIT6|BIT7))|((au<<4)&(BIT4|BIT5))|((p<<2)&(BIT2|BIT3))|((z<<0)&(BIT0|BIT1))
#define SET_EPWMSS1_AQCTLA(z,p,au,ad,bu,bd)     U16REG_EPWMSS1_AQCTLA = ((bd<<10)&(BIT10|BIT11))|((bu<<8)&(BIT8|BIT9))|((ad<<6)&(BIT6|BIT7))|((au<<4)&(BIT4|BIT5))|((p<<2)&(BIT2|BIT3))|((z<<0)&(BIT0|BIT1))
#define SET_EPWMSS1_AQCTLB(z,p,au,ad,bu,bd)     U16REG_EPWMSS1_AQCTLB = ((bd<<10)&(BIT10|BIT11))|((bu<<8)&(BIT8|BIT9))|((ad<<6)&(BIT6|BIT7))|((au<<4)&(BIT4|BIT5))|((p<<2)&(BIT2|BIT3))|((z<<0)&(BIT0|BIT1))
#define SET_EPWMSS2_AQCTLA(z,p,au,ad,bu,bd)     U16REG_EPWMSS2_AQCTLA = ((bd<<10)&(BIT10|BIT11))|((bu<<8)&(BIT8|BIT9))|((ad<<6)&(BIT6|BIT7))|((au<<4)&(BIT4|BIT5))|((p<<2)&(BIT2|BIT3))|((z<<0)&(BIT0|BIT1))
#define SET_EPWMSS2_AQCTLB(z,p,au,ad,bu,bd)     U16REG_EPWMSS2_AQCTLB = ((bd<<10)&(BIT10|BIT11))|((bu<<8)&(BIT8|BIT9))|((ad<<6)&(BIT6|BIT7))|((au<<4)&(BIT4|BIT5))|((p<<2)&(BIT2|BIT3))|((z<<0)&(BIT0|BIT1))

#define U16REG_EPWMSS0_AQSFRC                   U16REG(__PWMSS0_ADDR+PWMSS_EPWM,EPWM_AQSFRC)
#define U16REG_EPWMSS1_AQSFRC                   U16REG(__PWMSS1_ADDR+PWMSS_EPWM,EPWM_AQSFRC)
#define U16REG_EPWMSS2_AQSFRC                   U16REG(__PWMSS2_ADDR+PWMSS_EPWM,EPWM_AQSFRC)

#define SET_EPWMSS0_AQSFRC_ACTSFA(n)            U16REG_EPWMSS0_AQSFRC &= ~(BIT0|BIT1); U16REG_EPWMSS0_AQSFRC |= ((n)<<0)
#define SET_EPWMSS0_AQSFRC_ACTSFB(n)            U16REG_EPWMSS0_AQSFRC &= ~(BIT3|BIT4); U16REG_EPWMSS0_AQSFRC |= ((n)<<3)
#define SET_EPWMSS1_AQSFRC_ACTSFA(n)            U16REG_EPWMSS1_AQSFRC &= ~(BIT0|BIT1); U16REG_EPWMSS1_AQSFRC |= ((n)<<0)
#define SET_EPWMSS1_AQSFRC_ACTSFB(n)            U16REG_EPWMSS1_AQSFRC &= ~(BIT3|BIT4); U16REG_EPWMSS1_AQSFRC |= ((n)<<3)
#define SET_EPWMSS2_AQSFRC_ACTSFA(n)            U16REG_EPWMSS2_AQSFRC &= ~(BIT0|BIT1); U16REG_EPWMSS2_AQSFRC |= ((n)<<0)
#define SET_EPWMSS2_AQSFRC_ACTSFB(n)            U16REG_EPWMSS2_AQSFRC &= ~(BIT3|BIT4); U16REG_EPWMSS2_AQSFRC |= ((n)<<3)

//
// private register addresses; use with caution
//
extern volatile void *__CM_PER_ADDR;
extern volatile void *__CM_WKUP_ADDR;
extern volatile void *__GPIO0_ADDR;
extern volatile void *__GPIO1_ADDR;
extern volatile void *__GPIO2_ADDR;
extern volatile void *__GPIO3_ADDR;
extern volatile void *__CTRL_ADDR;
extern volatile void *__TSCADC_ADDR;
extern volatile void *__PWMSS0_ADDR;
extern volatile void *__PWMSS1_ADDR;
extern volatile void *__PWMSS2_ADDR;

#endif /* __NXCTRL_REGISTERS_HEADER__ */
