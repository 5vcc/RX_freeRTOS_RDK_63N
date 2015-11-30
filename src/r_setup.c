/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/*******************************************************************************
* File Name     : setup.c
* Version       : 1.00
* Device        : RX63N
* H/W Platform  : YRDKRX63N specific
* Description   : Implements required user settings for application
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY     Version     Description
*           20.06.2013     V.1.00      Initial version
*******************************************************************************/
/*Include FreeRTOS header files and io define*/
#include "iodefine.h"
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "queue.h"
#include "diskio.h"

#define	F_PCLK	96000000UL

volatile UINT Timer;		/* Performance timer (1kHz) */

void delay_ms (					/* Delay in unit of msec */
	UINT ms
)
{
	for (Timer = 0; Timer < ms; ) ;
}

/***********************************************************************************************
* Function name: R_CMT1_Setup
* Description  : Setup the tick timer for interrupt. Interrupt is generated for each 0.5 second
* Arguments    : none
* Return value : none
************************************************************************************************/
void R_CMT1_Setup( void )
{

    SYSTEM.PRCR.WORD = 0xA50B; /* Protect off */

    /* Enable compare match timer 0. */
    MSTP( CMT1 ) = 0;

    SYSTEM.PRCR.WORD = 0xA500; /* Protect on  */

    CMT1.CMCR.WORD = 0x0040;			/* CMIE=1, CKS=0(PCLK/8) */

    /* Interrupt is generated in each 0.5 second */
    /*PCKL = 48MHz
     * divide 512 => 93750Hz
     * Count (93750/2 = 46875) for 500ms
     */

    CMT1.CMCOR = F_PCLK / 8 / 1000 - 1;	/* Select clock divider */

    /* Enable the interrupt... */
    IEN(CMT1, CMI1 ) = 1;

    /* ...and set its priority to the application defined kernel priority. */
    IPR(CMT1, CMI1 ) = configKERNEL_INTERRUPT_PRIORITY;

    /* Start the timer. */
    CMT.CMSTR0.BIT.STR1 = 1;
}

/********************************************************************************
* Function name: R_Operating_frequency_set
* Description  : Configures the clock settings for each of the device clocks
* Arguments    : none
* Return value : none
*********************************************************************************/
void R_Operating_frequency_set(void)
{

    /*
    Clock Description              Frequency
    ----------------------------------------
    Input Clock Frequency............  12 MHz
    PLL frequency (x16).............. 192 MHz
    Internal Clock Frequency.........  96 MHz
    Peripheral Clock Frequency.......  48 MHz
    USB Clock Frequency..............  48 MHz
    External Bus Clock Frequency.....  24 MHz */

    volatile unsigned int i;

    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA50B;

    /* Uncomment if not using sub-clock */
    //SYSTEM.SOSCCR.BYTE = 0x01;          /* stop sub-clock */
    SYSTEM.SOSCCR.BYTE = 0x00;          /* Enable sub-clock for RTC */

    /* Wait 131,072 cycles * 12 MHz = 10.9 ms */
    SYSTEM.MOSCWTCR.BYTE = 0x0D;

    /* PLL wait is 4,194,304 cycles (default) * 192 MHz (12 MHz * 16) = 20.1 ms*/
    SYSTEM.PLLWTCR.BYTE = 0x04;

    /* x16 @PLL */
    SYSTEM.PLLCR.WORD = 0x0F00;

    /* EXTAL ON */
    SYSTEM.MOSCCR.BYTE = 0x00;

    /* PLL ON */
    SYSTEM.PLLCR2.BYTE = 0x00;

    for ( i = 0; i < 0x168; i++ )
    {
        /* Wait over 12ms */
    	/* 0x168 was used once */
        nop() ;
    }

    /* Setup system clocks
    SCKCR - System Clock Control Register
    b31:b28 FCK[3:0]  0x02 = Flash clock: PLL/4 = (192 / 4) = 48 MHz
    b27:b24 ICK[3:0]  0x01 = System clock: PLL/2 = (192 / 2) = 96 MHz
    b23     PSTOP1    0x00 = BCLK pin output is enabled
    b19:b16 BCK[3:0]  0x03 = BCLK: PLL/8 = 24 MHz
    b11:b8  PCKB[3:0] 0x02 = Peripheral clock B: PLL/4 = 48 MHz
    */
    SYSTEM.SCKCR.LONG = 0x21031222;     /* ICK=PLL/2,BCK,FCK,PCK=PLL/4 */

    /* Setup IEBUS and USB clocks
    SCKCR2 - System Clock Control Register 2
    b7:b4 UCK[3:0]   0x03 = USB clock is PLL/4 = 48 MHz
    b3:b0 IEBCK[3:0] 0x01 = IE Bus clock is PLL/2 = 96 MHz
    */
    SYSTEM.SCKCR2.WORD = 0x0031;

    /* ICLK, PCLKB, FCLK, BCLK, IECLK, and USBCLK all come from PLL circuit */
    SYSTEM.SCKCR3.WORD = 0x0400;

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;
}
/******************************************************************************
* Function name: vApplicationSetupTimerInterrupt
* Description  : Setup the tick timer for the OS - A requirement for FreeRTOS.
* Arguments    : none
* Return value : none
*******************************************************************************/
void vApplicationSetupTimerInterrupt()
{
	SYSTEM.PRCR.WORD = 0xA50B; /* Protect off */

	/* Enable compare match timer 0. */
	MSTP( CMT0 ) = 0;

	SYSTEM.PRCR.WORD = 0xA500; /* Protect on  */

	/* Divide the PCLK by 8 and Interrupt on compare match in CMCR register. */
	CMT0.CMCR.WORD = 0x00C0;

	/* Set the compare match value. */
	CMT0.CMCOR = (unsigned short) (((configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ) - 1) / 8);

	/* Enable the interrupt... */
	IEN(CMT0, CMI0 ) = 1;

	/* ...and set its priority to the application defined kernel priority. */
	IPR(CMT0, CMI0 ) = configKERNEL_INTERRUPT_PRIORITY;

	/* Start the timer. */
	CMT.CMSTR0.BIT.STR0 = 1;
}
/******************************************************************************
* Function name: Excep_CMT1_CMI1
* Description  : This is an ISR.
*                Vector 29 is the vector number of MCU
*                It is defined in iodefine.h
*---------------------------------------------------------*
* 1000Hz interval timer (CMT1)                            *
*---------------------------------------------------------*
* Arguments    : none
* Return value : none
*******************************************************************************/
#pragma interrupt (Excep_CMT1_CMI1(vect=VECT(CMT1, CMI1), enable))
void Excep_CMT1_CMI1(void)
{

	/* Increment performance timer */
	Timer++;

	/* Drive MMC/SD control module (mmc_rspi.c) */
	disk_timerproc();
}
