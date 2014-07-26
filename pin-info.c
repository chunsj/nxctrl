/*
 * NXCTRL BeagleBone Black Control Library
 *
 * BBB GPIO Pin Information
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
#include <strings.h>

typedef struct __tagPININFO {
  char *pszPin;
  char *pszName;
  char *pszM7;
  char *pszM6;
  char *pszM5;
  char *pszM4;
  char *pszM3;
  char *pszM2;
  char *pszM1;
  char *pszM0;
  char *pszNote;
} PININFO;

#define DP(p,n,m7,m6,m5,m4,m3,m2,m1,m0,nt) { .pszPin = p, .pszName = n, .pszM7 = m7, .pszM6 = m6, .pszM5 = m5, .pszM4 = m4, .pszM3 = m3, .pszM2 = m2, .pszM1 = m1, .pszM0 = m0, .pszNote = nt }

PININFO riP8[] = {
  DP("P8_01", "DGND", "", "", "", "", "", "", "", "", "Ground"),
  DP("P8_02", "DGND", "", "", "", "", "", "", "", "", "Ground"),
  DP("P8_03", "GPIO1[6]", "GPIO1[6]", "", "", "", "", "", "MMC1_DAT6", "GPMC_AD6", "Board(EMMC2_PINS)"),
  DP("P8_04", "GPIO1[7]", "GPIO1[7]", "", "", "", "", "", "MMC1_DAT7", "GPMC_AD7", "Board(EMMC2_PINS)"),
  DP("P8_05", "GPIO1[2]", "GPIO1[2]", "", "", "", "", "", "MMC1_DAT2", "GPMC_AD2", "Board(EMMC2_PINS)"),
  DP("P8_06", "GPIO1[3]", "GPIO1[3]", "", "", "", "", "", "MMC1_DAT3", "GPMC_AD3", "Board(EMMC2_PINS)"),
  DP("P8_07", "TIMER4", "GPIO2[2]", "", "", "", "", "TIMER4", "", "GPMC_ADVN_ALE", ""),
  DP("P8_08", "TIMER7", "GPIO2[3]", "", "", "", "", "TIMER7", "", "GPMC_OEN_REN", ""),
  DP("P8_09", "TIMER5", "GPIO2[5]", "", "", "", "", "TIMER5", "", "GPMC_BE0N_CLE", ""),
  DP("P8_10", "TIMER6", "GPIO2[4]", "", "", "", "", "TIMER6", "", "GPMC_WEN", ""),
  DP("P8_11", "GPIO1[13]", "GPIO1[13]", "PR1_PRU0_PRU_R30_15", "", "EQEP2B_IN", "MMC2_DAT1", "MMC1_DAT5", "LCD_DATA18", "GPMC_AD13", ""),
  DP("P8_12", "GPIO1[12]", "GPIO1[12]", "PR1_PRU0_PRU_R30_14", "", "EQEP2A_IN", "MMC2_DAT0", "MMC1_DAT4", "LCD_DATA19", "GPMC_AD12", ""),
  DP("P8_13", "EHRPWM2B", "GPIO0[23]", "", "", "EHRPWM2B", "MMC2_DAT5", "MMC1_DAT1", "LCD_DATA22", "GPMC_AD9", ""),
  DP("P8_14", "GPIO0[26]", "GPIO0[26]", "", "", "EHRPWM2_TRIPZONE_IN", "MMC2_DAT6", "MMC1_DAT2", "LCD_DATA21", "GPMC_AD10", ""),
  DP("P8_15", "GPIO1[15]", "GPIO1[15]", "PR1_PRU0_PRU_R31_15", "", "EQEP2_STROBE", "MMC2_DAT3", "MMC1_DAT7", "LCD_DATA16", "GPMC_AD15", ""),
  DP("P8_16", "GPIO1[14]", "GPIO1[14]", "PR1_PRU0_PRU_R31_14", "", "EQEP2_INDEX", "MMC2_DAT2", "MMC1_DAT6", "LCD_DATA17", "GPMC_AD14", ""),
  DP("P8_17", "GPIO0[27]", "GPIO0[27]", "", "", "EHRPWM0_SYNCO", "MMC2_DAT7", "MMC1_DAT3", "LCD_DATA20", "GPMC_AD11", ""),
  DP("P8_18", "GPIO2[1]", "GPIO2[1]", "MCASP0_FSR", "", "", "MMC2_CLK", "GPMC_WAIT1", "LCD_MEMORY_CLK", "GPMC_CLK_MUX0", ""),
  DP("P8_19", "EHRPWM2A", "GPIO0[22]", "", "", "EHRPWM2A", "MMC2_DAT4", "MMC1_DAT0", "LCD_DATA23", "GPMC_AD8", ""),
  DP("P8_20", "GPIO1[31]", "GPIO1[31]", "PR1_PRU1_PRU_R31_13", "PR1_PRU1_PRU_R30_13", "", "", "MMC1_CMD", "GPMC_BE1N", "GPMC_CSN2", "Board(EMMC2_PINS)"),
  DP("P8_21", "GPIO1[30]", "GPIO1[30]", "PR1_PRU1_PRU_R31_12", "PR1_PRU1_PRU_R30_12", "", "", "MMC1_CLK", "GPMC_CLK", "GPMC_CSN1", "Board(EMMC2_PINS)"),
  DP("P8_22", "GPIO1[5]", "GPIO1[5]", "", "", "", "", "", "MMC1_DAT5", "GPMC_AD5", "Board(EMMC2_PINS)"),
  DP("P8_23", "GPIO1[4]", "GPIO1[4]", "", "", "", "", "", "MMC1_DAT4", "GPMC_AD4", "Board(EMMC2_PINS)"),
  DP("P8_24", "GPIO1[1]", "GPIO1[1]", "", "", "", "", "", "MMC1_DAT1", "GPMC_AD1", "Board(EMMC2_PINS)"),
  DP("P8_25", "GPIO1[0]", "GPIO1[0]", "", "", "", "", "", "MMC1_DAT0", "GPMC_AD0", "Board(EMMC2_PINS)"),
  DP("P8_26", "GPIO1[29]", "GPIO1[29]", "", "", "", "", "", "", "GPMC_CSN0", ""),
  DP("P8_27", "GPIO2[22]", "GPIO2[22]", "PR1_PRU1_PRU_R31_8", "PR1_PRU1_PRU_R30_8", "", "", "", "GPMC_A8", "LCD_VSYNC", "Board(HDMI_PINS)"),
  DP("P8_28", "GPIO2[24]", "GPIO2[24]", "PR1_PRU1_PRU_R31_10", "PR1_PRU1_PRU_R30_10", "", "", "", "GPMC_A10", "LCD_PCLK", "Board(HDMI_PINS)"),
  DP("P8_29", "GPIO2[23]", "GPIO2[23]", "PR1_PRU1_PRU_R31_9", "PR1_PRU1_PRU_R30_9", "", "", "", "GPMC_A9", "LCD_HSYNC", "Board(HDMI_PINS)"),
  DP("P8_30", "GPIO2[25]", "GPIO2[25]", "", "", "", "", "", "GPMC_A11", "LCD_AC_BIAS_EN", "Board(HDMI_PINS)"),
  DP("P8_31", "UART5_CTSN", "GPIO0[10]", "UART5_CTSN", "", "UART5_RXD", "MCASP0_AXR1", "EQEP1_INDEX", "GPMC_A18", "LCD_DATA14", "Board(HDMI_PINS)"),
  DP("P8_32", "UART5_RTSN", "GPIO0[11]", "UART5_RTSN", "", "MCASP0_AXR3", "MCASP0_AHCLKX", "EQEP1_STROBE", "GPMC_A19", "LCD_DATA15", "Board(HDMI_PINS)"),
  DP("P8_33", "UART4_RTSN", "GPIO0[9]", "UART4_RTSN", "", "MCASP_AXR3", "MCASP0_FSR", "EQEP1B_IN", "GPMC_A17", "LCD_DATA13", "Board(HDMI_PINS)"),
  DP("P8_34", "UART3_RTSN", "GPIO2[17]", "UART3_RTSN", "", "MCASP0_AXR2", "MCASP0_AHCLKR", "EHRPWM1B", "GPMC_A15", "LCD_DATA11", "Board(HDMI_PINS)"),
  DP("P8_35", "UART4_CTSN", "GPIO0[8]", "UART4_CTSN", "", "MCASP0_AXR2", "MCASP0_ACLKR", "EQEP1A_IN", "GPMC_A16", "LCD_DATA12", "Board(HDMI_PINS)"),
  DP("P8_36", "UART3_CTSN", "GPIO2[16]", "UART3_CTSN", "", "", "MCASP0_AXR0", "EHRPWM1A", "GPMC_A14", "LCD_DATA10", "Board(HDMI_PINS)"),
  DP("P8_37", "UART5_TXD", "GPIO2[14]", "UART2_CTSN", "", "UART5_TXD", "MCASP0_ACLKX", "EHRPWM1_TRIPZONE_IN", "GPMC_A12", "LCD_DATA8", "Board(HDMI_PINS)"),
  DP("P8_38", "UART5_RXD", "GPIO2[15]", "UART2_RTSN", "", "UART5_RXD", "MCASP0_FSX", "EHRPWM0_SYNCO", "GPMC_A13", "LCD_DATA9", "Board(HDMI_PINS)"),
  DP("P8_39", "GPIO2[12]", "GPIO2[12]", "PR1_PRU1_PRU_R31_6", "PR1_PRU1_PRU_R30_6", "", "EQEP2_INDEX", "", "GPMC_A6", "LCD_DATA6", "Board(HDMI_PINS)"),
  DP("P8_40", "GPIO2[13]", "GPIO2[13]", "PR1_PRU1_PRU_R31_7", "PR1_PRU1_PRU_R30_7", "PR1_EDIO_DATA_OUT7", "EQEP2_STROBE", "", "GPMC_A7", "LCD_DATA7", "Board(HDMI_PINS)"),
  DP("P8_41", "GPIO2[10]", "GPIO2[10]", "PR1_PRU1_PRU_R31_4", "PR1_PRU1_PRU_R30_4", "", "EQEP2A_IN", "", "GPMC_A4", "LCD_DATA4", "Board(HDMI_PINS)"),
  DP("P8_42", "GPIO2[11]", "GPIO2[11]", "PR1_PRU1_PRU_R31_5", "PR1_PRU1_PRU_R30_5", "", "EQEP2B_IN", "", "GPMC_A5", "LCD_DATA5", "Board(HDMI_PINS)"),
  DP("P8_43", "GPIO2[8]", "GPIO2[8]", "PR1_PRU1_PRU_R31_2", "PR1_PRU1_PRU_R30_2", "", "EHRPWM2_TRIPZONE_IN", "", "GPMC_A2", "LCD_DATA2", "Board(HDMI_PINS)"),
  DP("P8_44", "GPIO2[9]", "GPIO2[9]", "PR1_PRU1_PRU_R31_3", "PR1_PRU1_PRU_R30_3", "", "EHRPWM0_SYNCO", "", "GPMC_A3", "LCD_DATA3", "Board(HDMI_PINS)"),
  DP("P8_45", "GPIO2[6]", "GPIO2[6]", "PR1_PRU1_PRU_R31_0", "PR1_PRU1_PRU_R30_0", "", "EHRPWM2A", "", "GPMC_A0", "LCD_DATA0", "Board(HDMI_PINS)"),
  DP("P8_46", "GPIO2[7]", "GPIO2[7]", "PR1_PRU1_PRU_R31_1", "PR1_PRU1_PRU_R30_1", "", "EHRPWM2B", "", "GPMC_A1", "LCD_DATA1", "Board(HDMI_PINS)"),
};

PININFO riP9[] = {
  DP("P9_01", "GND",                 "",           "", "", "", "", "", "", "", "Ground"),
  DP("P9_02", "GND",                 "",           "", "", "", "", "", "", "", "Ground"),
  DP("P9_03", "3V3",                 "",           "", "", "", "", "", "", "", "250mA Max"),
  DP("P9_04", "3V3",                 "",           "", "", "", "", "", "", "", "250mA Max"),
  DP("P9_05", "VDD_5V",              "",           "", "", "", "", "", "", "", "1A Max(DC)"),
  DP("P9_06", "VDD_5V",              "",           "", "", "", "", "", "", "", "1A Max(DC)"),
  DP("P9_07", "5V",                  "",           "", "", "", "", "", "", "", "250mA Max"),
  DP("P9_08", "5V",                  "",           "", "", "", "", "", "", "", "250mA Max"),
  DP("P9_09", "PWR_BUT",             "",           "", "", "", "", "", "", "", "5V Pulled Up"),
  DP("P9_10", "SYS_RESET",           "",           "", "", "", "", "", "", "", ""),
  DP("P9_11", "UART4_RXD",           "GPIO0[30]",  "UART4_RXD_MUX2", "", "MMC1_SDCD", "RMII2_CRS_DV", "GPMC_CSN4", "MII2_CRS", "GPMC_WAIT0", ""),
  DP("P9_12", "GPIO1[28]",           "GPIO1[28]",  "MCASP0_ACLKR_MUX3", "", "GPMC_DIR", "MMC2_DAT3", "GPMC_CSN6", "MII2_COL", "GPMC_BE1N", ""),
  DP("P9_13", "UART4_TXD",           "GPIO0[31]",  "UART4_TXD_MUX2", "", "MMC2_SDCD", "RMII2_RXERR", "GPMC_CSN5", "MII2_RXERR", "GPMC_WPN", ""),
  DP("P9_14", "EHRPWM1A",            "GPIO1[18]",  "EHRPWM1A_MUX1", "", "GPMC_A18", "MMC2_DAT1", "RGMII2_TD3", "MII2_TXD3", "GPMC_A3", ""),
  DP("P9_15", "GPIO1[16]",           "GPIO1[16]",  "EHRPWM1_TRIPZONE_INPUT", "GPMC_A16", "MII2_TXEN", "RMII2_TCTL", "GMII2_TXEN", "GPMC_A0", "", ""),
  DP("P9_16", "EHRPWM1B",            "GPIO1[19]",  "EHRPWM1B_MUX1", "", "GPMC_A19", "MMC2_DAT2", "RGMII2_TD2", "MII2_TXD2", "GPMC_A3", ""),
  DP("P9_17", "I2C1_SCL",            "GPIO0[5]",   "", "", "PR1_UART0_TXD", "EHRPWM0_SYNCI", "I2C1_SCL", "MMC2_SDWP", "SPI0_CS0", ""),
  DP("P9_18", "I2C1_SDA",            "GPIO0[4]",   "", "", "PR1_UART0_RXD", "EHRPWM0_TRIPZONE", "I2C1_SDA", "MMC1_SDWP", "SPI0_D1", ""),
  DP("P9_19", "I2C2_SCL",            "GPIO0[13]",  "", "PR1_UART0_RTS_N", "SPI1_CS1", "I2C2_SCL", "DCAN0_RX", "TIMER5", "UART1_RTSN", "I2C2_PINS"),
  DP("P9_20", "I2C2_SDA",            "GPIO0[12]",  "", "PR1_UART0_CTS_N", "SPI1_CS0", "I2C2_SDA", "DCAN0_TX", "TIMER6", "UART1_CTSN", "I2C2_PINS"),
  DP("P9_21", "UART2_TXD",           "GPIO0[3]",   "EMU3_MUX1", "", "PR1_UART0_RTS_N", "EHRPWM0B", "I2C2_SCL", "UART2_TDX", "SPI0_D0", ""),
  DP("P9_22", "UART2_RXD",           "GPIO0[2]",   "EMU2_MUX1", "", "PR1_UART0_CTS_N", "EHRPWM0A", "I2C2_SDA", "UART2_RXD", "SPI0_SCLK", ""),
  DP("P9_23", "GPIO1[17]",           "GPIO1[17]",  "EHRPWM0_SYNCO", "", "GPMC_A17", "MMC2_DAT0", "RGMII2_RXDV", "GMII2_RXDV", "GPMC_A1", ""),
  DP("P9_24", "UART1_TXD",           "GPIO0[15]",  "PR1_PRU0_PRU_R31_16", "P1_UART0_TXD", "", "I2C1_SCL", "DCAN1_RX", "MMC2_SDWP", "UART1_TXD", ""),
  DP("P9_25", "GPIO3[21]",           "GPIO3[21]",  "PR1_PRU0_PRU_R31_7", "PR1_PRU0_PRU_R30_7", "EMU4_MUX2", "MCASP1_AXR1", "MCASP0_AXR3", "EQEP0_STROBE", "MCASP0_AHCLKX", "MCASP0_PINS"),
  DP("P9_26", "UART1_RXD",           "GPIO0[14]",  "PR1_PRU1_PRU_R31_16", "PR1_UART0_RXD", "", "I2C1_SDA", "DCAN1_TX", "MMC1_SDWP", "UART1_RXD", ""),
  DP("P9_27", "GPIO3[19]",           "GPIO3[19]",  "PR1_PRU0_PRU_R31_5", "PR1_PRU0_PRU_R30_5", "EMU2_MUX2", "MCASP1_FSX", "MCASP0_AXR3", "EQEP0B_IN", "MCASP0_FSR", ""),
  DP("P9_28", "SPI1_CS0",            "GPIO3[17]",  "PR1_PRU0_PRU_R31_3", "PR1_PRU0_PRU_R30_3", "ECAP2_IN_PWM2_OUT", "SPI1_CS0", "MCASP0_AXR2", "EHRPWM0_SYNCI", "MCASP0_AHCLKR", "MCASP0_PINS"),
  DP("P9_29", "SPI1_D0",             "GPIO3[15]",  "PR1_PRU0_PRU_R31_1", "PR1_PRU0_PRU_R30_1", "MMC1_SDCD_MUX1", "SPI1_D0", "", "EHRPWM0B", "MCASP0_FSX", "MCASP0_PINS"),
  DP("P9_30", "SPI1_D1",             "GPIO3[16]",  "PR1_PRU0_PRU_R31_2", "PR1_PRU0_PRU_R30_2", "MMC2_SDCD_MUX1", "SPI1_D1", "", "EHRPWM0_TRIPZONE", "MCASP0_AXR0", "MCASP0_PINS"),
  DP("P9_31", "SPI1_SCLK",           "GPIO3[14]",  "PR1_PRU0_PRU_R31_0", "PR1_PRU0_PRU_R30_0", "MMC0_SDCD_MUX1", "SPI1_SCLK", "", "EHRPWM0A", "MCASP0_ACLKX", "MCASP0_PINS"),
  DP("P9_32", "VADC",                "",           "", "", "", "", "", "", "", "1.8V Reference"),
  DP("P9_33", "AIN4",                "",           "", "", "", "", "", "", "", "1.8V"),
  DP("P9_34", "AGND",                "",           "", "", "", "", "", "", "", "ADC Ground"),
  DP("P9_35", "AIN6",                "",           "", "", "", "", "", "", "", "1.8V"),
  DP("P9_36", "AIN5",                "",           "", "", "", "", "", "", "", "1.8V"),
  DP("P9_37", "AIN2",                "",           "", "", "", "", "", "", "", "1.8V"),
  DP("P9_38", "AIN3",                "",           "", "", "", "", "", "", "", "1.8V"),
  DP("P9_39", "AIN0",                "",           "", "", "", "", "", "", "", "1.8V"),
  DP("P9_40", "AIN1",                "",           "", "", "", "", "", "", "", "1.8V"),
  DP("P9_41", "CLKOUT2/GPIO3[20]",   "GPIO0[20]/GPIO3[20]", "EMU3_MUX0/PR1_PRU0_PRU_R31_6", "PR1_PRU0_PRU_R31_16/PR1_PRU0_PRU_R30_6", "TIMER7_MUX1/EMU3", "CLKOUT2/MCASP1_AXR0", "TCLKIN", "EQEP0_INDEX", "XDMA_EVENT_INTR1/MCASP0_AXR1", "41A/B"),
  DP("P9_42", "GPIO0[7]/GPIO3[18]",  "GPIO0[7]/GPIO3[18]", "XDMA_EVENT_INTR2/PR1_PRU0_PRU_R31_4", "MMC0_SDWP/PR1_PRU0_PRU_R30_4", "SPI1_SCLK", "PR1_ECAP0_ECAP_CAPIN_APWM_O/MCASP1_ACLKX", "SPI1_CS1/MCASP0_AXR2", "UART3_TXD/EQEP0A_IN", "ECAP0_IN_PWM0_OUT/MCASP0_ACLKR", "42A/B, MCASP0_PINS"),
  DP("P9_43", "GND",                 "",           "", "", "", "", "", "", "", "Ground"),
  DP("P9_44", "GND",                 "",           "", "", "", "", "", "", "", "Ground"),
  DP("P9_45", "GND",                 "",           "", "", "", "", "", "", "", "Ground"),
  DP("P9_46", "GND",                 "",           "", "", "", "", "", "", "", "Ground"),
};

int
main (int argc, const char *argv[]) {
  PININFO *ri;
  int i;
  
  switch (argc) {
  case 2:
    if (!strcasecmp("P8", argv[1]))
      ri = riP8;
    else if (!strcasecmp("P9", argv[1]))
      ri = riP9;
    else {
      fprintf(stderr, "usage: pin-info [BANK] [PIN]\n");
      return 1;
    }

    for (i = 0; i < 46; i++) {
      printf("[%s] %-18s", ri[i].pszPin, ri[i].pszName);
      printf(" %s\n", ri[i].pszNote);
    }
    printf("\n");

    break;
  case 3:
    if (!strcasecmp("P8", argv[1]))
      ri = riP8;
    else if (!strcasecmp("P9", argv[1]))
      ri = riP9;
    else {
      fprintf(stderr, "usage: pin-info [BANK] [PIN]\n");
      return 1;
    }
    
    i = atoi(argv[2]) - 1;

    if (i > -1 && i < 46) {
      printf("[%s] %-18s", ri[i].pszPin, ri[i].pszName);
      printf(" %s\n", ri[i].pszNote);
      printf("  M7: %-40s M6: %-30s\n", ri[i].pszM7, ri[i].pszM6);
      printf("  M5: %-40s M4: %-30s\n", ri[i].pszM5, ri[i].pszM4);
      printf("  M3: %-40s M2: %-30s\n", ri[i].pszM3, ri[i].pszM2);
      printf("  M1: %-40s M0: %-30s\n", ri[i].pszM1, ri[i].pszM0);
    } else {
      fprintf(stderr, "usage: pin-info [BANK] [PIN]\n");
      return 1;
    }
    break;
  default:
    ri = riP8;
    for (i = 0; i < 46; i++) {
      printf("[%s] %-18s", ri[i].pszPin, ri[i].pszName);
      printf(" %s\n", ri[i].pszNote);
    }
    printf("\n");

    ri = riP9;
    for (i = 0; i < 46; i++) {
      printf("[%s] %-18s", ri[i].pszPin, ri[i].pszName);
      printf(" %s\n", ri[i].pszNote);
    }
    printf("\n");
  }
  return 0;
}
