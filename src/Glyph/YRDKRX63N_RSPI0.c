/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY
* DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved. */
/* Code written for Renesas by Future Designs, Inc. www.teamfdi.com */

/******************************************************************************
* File Name : YRDKRX63N_RSPI0.c
* Version : 1.00
* Device(s) : RSPI0 Communications
* Tool-Chain : Glyph - The Generic API for Graphics RSPI API version 1.00
* H/W Platform : RX63N Group CPU
* Description : Glyph API RX63N RSPI Transmitter 0 Operation channel 3
* Routines for RSPI code running on RX63N MCU, RSPI0.
* This program uses RSPI0.  The Chip Select for the flash device (LCD)
* is set to PC_2.
*       Manufacturer: Renesas
*       Communications Protocol: RSPI 3-Wire
*                                Transmit only
*                                MASTER
*       Transmitter Number: 0
*       channel: 3
*       Chip Select Port: PC.2
*       RSPI Clock Speed: 25MHz
******************************************************************************
* History :
* : 21.07.2010 1.00 First Release
******************************************************************************/

#define F_PCLK		96000000UL	/* PCLK frequency (configured by SCKCR.PCK) */
#define SCLK_FAST	12000000UL	/* SCLK frequency (R/W) */
#define	SCLK_SLOW	400000UL	/* SCLK frequency (Init) */

/******************************************************************************
Includes    YRDKRX63N RSPI Includes
******************************************************************************/
#include <iodefine.h>
#include "YRDKRX63N_RSPI0.h"

/******************************************************************************
Private Functions
******************************************************************************/
#if 0
static void StartResetLCD(void) ;
static void EndResetLCD(void) ;
#endif


/******************************************************************************
* ID : 60.0
* Outline : YRDKRX63N_RSPIOpen
* Include : YRDKRX63N_RSPI0.h
* Function Name: YRDKRX63N_RSPIOpen
* Description : Open and setup the communications channel RSPI0.
* Argument : aHandle - the Glyph handle to setup for the LCD and Communications.
* Return Value : 0=success, not 0= error
* Calling Functions : GlyphCommOpen
******************************************************************************/
T_glyphError YRDKRX63N_RSPIOpen(T_glyphHandle aHandle)
{
    SYSTEM.PRCR.WORD = 0xA50B;            /* Protect off */
    MPC.PWPR.BIT.B0WI = 0 ;             /* Unlock protection register */
    MPC.PWPR.BIT.PFSWE = 1 ;            /* Unlock MPC registers */

    MSTP(EDMAC) = 0 ;                   /* Power up ethernet block */

    /* Port 0 - DAC & ethernet IRQ */
    PORT0.PODR.BYTE = 0x00 ;    /* All outputs low to start */
    PORT0.PDR.BYTE  = 0x2F ;    /* P05/DA1 is an ouput, P0-P3 not bonded-out, all others are inputs or reserved */

    /* Port 1 - I2C and USB over-current & pull-up control */
    PORT1.PODR.BYTE = 0x00 ;    /* All outputs low to start */
    PORT1.PDR.BYTE  = 0x83 ;    /* AUD_R (P1.7) is an output, P10 & P11 not bonded out, all others are inputs
                                   (I2C lines setup by I2C driver later */

    /* Port 2 - USB control and some expansion signals */
    PORT2.PODR.BYTE = 0x02 ;    /* All outputs low to start except backlight enable */
    PORT2.PDR.BYTE  = 0x02 ;    /* All inputs except backlight enable - some will be overridden by USB driver later */

    /* Port 3 - Serial port & JTAG */
    PORT3.PODR.BYTE = 0x00 ;    /* All outputs low to start */
    PORT3.PDR.BIT.B2  = 0x01 ;  /* Transmit line for SCI6/ CAN 0 TxD is an output */

    PORT4.PODR.BYTE = 0x00 ;    /* These are all inputs */
    PORT4.PDR.BYTE  = 0x00 ;    /* Analog inputs and switches, all inputs */
    PORT4.PMR.BYTE  = 0x00 ;

    /* Port 5 -  */
    PORT5.PODR.BYTE = 0x00 ;    /* All outputs low to start */
    PORT5.PDR.BYTE  = 0xD3 ;    /* SCI 2 TxD, LCD_RS, PWMLP_OUT are outputs, P57 & P56 not bonded out */


    SYSTEM.PRCR.WORD = 0xA502;

    /* Turn on RSPI0 */
    MSTP(RSPI0) = 0;

    /* Disable writing to MSTP registers. */
    SYSTEM.PRCR.WORD = 0xA500;

    /* Setup ICU for RSPI */
    /* Set shared IPL for RSPI0 */
    IPR(RSPI0, SPRI0) = 3;
    /* Disable receive buffer full interrupt */
    IEN(RSPI0, SPRI0) = 0;
    /* Disable transmit buffer empty interrupt */
    IEN(RSPI0, SPTI0) = 0;
    /* Clear pending receive buffer full interrupts */
    IR(RSPI0, SPRI0) = 0 ;

    /* The following slave selects are based on the devices on the YRDKRX63N. */

    /* Set input/output ports */
    /* Port C -  SPI signals, chip selects, peripheral reset */
    /* Set to 0 initially. */
    PORTC.PODR.BYTE = 0xFD;
    /* All GPIO for now */
    PORTC.PMR.BYTE  = 0x00;
    /* PC5 is RSPCKA */
    MPC.PC5PFS.BYTE = 0x0D;
    /* PC6 is MOSIA */
    MPC.PC6PFS.BYTE = 0x0D;
    /* PC7 is MISOA */
    MPC.PC7PFS.BYTE = 0x0D;
    /* PC5-7 assigned to SPI peripheral */
    PORTC.PMR.BYTE  = 0xE0;
    /* Chip selects high to start */
    PORTC.PODR.BYTE = 0x17;
    /* All outputs except MISO */
    PORTC.PDR.BYTE  = 0x7F;


    /* Initialize SPI (per flowchart in hardware manual) */
    /* No loopback, CMos Output */
    RSPI0.SPPCR.BYTE = 0x00 ;
    /* Full Speed is 0x00  255 works */
    RSPI0.SPBR = 0x30 ;
    /* 16-bit data 1 frame 1 chip select */
    RSPI0.SPDCR.BYTE = 0x20 ;

    /* No parity no idle interrupts */
    RSPI0.SPCR2.BYTE = 0x00 ;
    /* MSB first 8-bit data, keep SSL low */
    RSPI0.SPCMD0.WORD = 0x0700 ;
    /* Enable RSPI 3wire in master mode with RSPI Enable Transmit Only and Interupt */
    RSPI0.SPCR.BYTE = 0xE9;

    RSPI0.SPBR = F_PCLK / 2 / SCLK_SLOW - 1;	/* Bit rate */


    return GLYPH_ERROR_NONE ;
}

/******************************************************************************
* ID : 62.0
* Outline : YRDKRX63N_RSPIStart
* Include : YRDKRX63N_RSPI0.h
* Function Name: YRDKRX63N_RSPIStart
* Description : Sets the Chip Select Port to Low.
* Argument : none
* Return Value : none
* Calling Functions : YRDKRX63N_RSPITransferOutWords
******************************************************************************/
void YRDKRX63N_RSPIStart(void)
{

    /* Assert chip select */
   // PORTC.PODR.BIT.B4 = 1;
    /* Assert chip select */
    PORTC.PODR.BIT.B2 = 0 ;

}

/******************************************************************************
* ID : 63.0
* Outline : YRDKRX63N_RSPIEnd
* Include : YRDKRX63N_RSPI0.h
* Function Name: YRDKRX63N_RSPIEnd
* Description : Sets the Chip Select Port to High.
* Argument : none
* Return Value : none
* Calling Functions : YRDKRX63N_RSPITransferOutWords
******************************************************************************/
void YRDKRX63N_RSPIEnd(void)
{
    /* De-assert chip select */
    PORTC.PODR.BIT.B2 = 1 ;
}

/******************************************************************************
* ID : 64.0
* Outline : YRDKRX63N_RSPITransferOutWords
* Include : YRDKRX63N_RSPI0.h
* Function Name: YRDKRX63N_RSPITransferOutWords
* Description : Transmit out, the SPI Port 0, 32 Bits of data in Low and
* High format.
* Argument : sLowWord - first 8 bits to send
*          : sHighWord - second 8 bits to send
* Return Value : none
* Calling Functions : YRDKRX63N_RSPITransferOutWord8Bit
******************************************************************************/
void YRDKRX63N_RSPITransferOutWords(int16_t sLowWord, int16_t sHighWord)
{
    YRDKRX63N_RSPIStart() ;

    /* ensure transmit register is empty */
    while (RSPI0.SPSR.BIT.IDLNF);

    RSPI0.SPDR.LONG = (int32_t)sHighWord;

    while (IR(RSPI0, SPRI0) == 0);

    (void)RSPI0.SPDR.WORD.L;
    (void)RSPI0.SPDR.WORD.H;

    IR(RSPI0, SPRI0) = 0;

    YRDKRX63N_RSPIEnd() ;
}

/******************************************************************************
* ID : 65.0
* Outline : YRDKRX63N_RSPITransferOutWord8Bit
* Include : YRDKRX63N_RSPI0.h
* Function Name: YRDKRX63N_RSPITransferOutWord8Bit
* Description : Transmit out, the SPI Port 0, 8 Bits of data.
* Argument : cData - 8 bits to send
* Return Value : none
* Calling Functions : T_Comm_API.iWrite
******************************************************************************/
void YRDKRX63N_RSPITransferOutWord8Bit(uint8_t cData)
{
    YRDKRX63N_RSPITransferOutWords(0, (uint16_t)cData) ;
}

/******************************************************************************
* ID : 66.0
* Outline : SetSendCommand
* Include : YRDKRX63N_RSPI0.h
* Function Name: SetSendCommand
* Description : Set the RS line of the LCD.  The RS line is connected to
* output port P5.1.  Setting this output to 0 will allow data flow in the LCD.
* Argument : none
* Return Value : none
* Calling Functions : CommandSend, DataSend
******************************************************************************/
static void SetSendCommand(void)
{
    PORT5.PODR.BIT.B1 = 0 ; // LCD RS A0
}

/******************************************************************************
* ID : 67.0
* Outline : SetSendData
* Include : YRDKRX63N_RSPI0.h
* Function Name: SetSendData
* Description : Clear the RS line of the LCD.  The RS line is connected to
* output port P5.1.  Setting this output to 1 will stop data flow in the LCD.
* When data flow is stopped in the LCD the data is internally copied to the
* data buffer for use.
* Argument : none
* Return Value : none
* Calling Functions : CommandSend, DataSend
******************************************************************************/
static void SetSendData(void)
{
    PORT5.PODR.BIT.B1 = 1 ; // LCD RS A0
}

/******************************************************************************
* ID : 68.0
* Outline : StartResetLCD
* Include : YRDKRX63N_RSPI0.h
* Function Name: StartResetLCD
* Description : Set the A0 line of the LCD display.  This line is connected
* to output PC.3 of the RX63N.  This output is inversed.  Setting this output
* to 1 will cause the LCD to reset.
* Argument : none
* Return Value : none
* Calling Functions : ST7579_Config
******************************************************************************/
#if 0
static void StartResetLCD(void)
{
    PORTC.PODR.BIT.B3 = 1;
}
#endif

/******************************************************************************
* ID : 69.0
* Outline : EndResetLCD
* Include : YRDKRX63N_RSPI0.h
* Function Name: EndResetLCD
* Description : Clear the A0 line of the LCD display.  This line is connected
* to output PC.3 of the RX63N.  This output is inversed.  Setting this output
* to 0 will cause the LCD complete reset.  After a delay the LCD will be
* ready to utilize.
* Argument : none
* Return Value : none
* Calling Functions : ST7579_Config
******************************************************************************/
#if 0
static void EndResetLCD(void)
{
    PORTC.PODR.BIT.B3 = 0;
}
#endif

/******************************************************************************
* ID : 70.0
* Outline : YRDKRX63N_CommandSend
* Include : YRDKRX63N_RSPI0.h
* Function Name: YRDKRX63N_CommandSend
* Description : Send a command define to the LCD.  The defines for all commands
* available to send to the ST7579 LCD controller are defined in the header
* for this file RX63N_LCD.h.  The nCommand parameter should always be
* a define from that location.
* Argument : cCommand - 8 bits of data to be used as a command to the LCD
*          : aHandle - the Glyph handle setup by the LCD and Communications.
* Return Value : none
* Calling Functions : ST7579_Config, ST7579_SetSystemBiasBooster,
*                     ST7579_SetVO_Range, ST7579_SetFrameRate,
*                     ST7579_SetPage, ST7579_SetChar, ST7579_SetLine,
*                     ST7579_Send8bitsData, ST7579_Send16bitsCommand
******************************************************************************/
void YRDKRX63N_CommandSend(int8_t cCommand)
{
    /* Send Command To LCD */
    /* A0 = 0 */
    SetSendCommand() ;

    YRDKRX63N_RSPITransferOutWord8Bit(cCommand) ;

    /* A0 = 1 */
    SetSendData() ;
}

/******************************************************************************
* ID : 71.0
* Outline : YRDKRX63N_DataSend
* Include : YRDKRX63N_RSPI0.h
* Function Name: YRDKRX63N_DataSend
* Description : Send 8 bits of data to the LCD RAM at the current location.
* The current location is determined and set with local functions.
* Argument : cCommand - 8 bits of data to be sent to the LCD
*          : aHandle - the Glyph handle setup by the LCD and Communications.
* Return Value : none
* Calling Functions : ST7579_Config, ST7579_SetSystemBiasBooster,
*                     ST7579_SetVO_Range, ST7579_SetFrameRate,
*                     ST7579_SetPage, ST7579_SetChar, ST7579_SetLine,
*                     ST7579_Send8bitsData, ST7579_Send16bitsCommand
******************************************************************************/
void YRDKRX63N_DataSend(int8_t cData)
{
    /* A0 = 1 */
    SetSendData() ;

    YRDKRX63N_RSPITransferOutWord8Bit(cData) ;

    /* A0 = 1 */
    SetSendData() ;
}

