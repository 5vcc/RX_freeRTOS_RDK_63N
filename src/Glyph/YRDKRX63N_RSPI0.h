/*-------------------------------------------------------------------------*
 * File:  YRDKRX63N_RSPI0.h
 *-------------------------------------------------------------------------*
 * Description:
 *    Y Renesas Development Kit (RDK) for Renesas RX63N Group CPU
 * RSPI Transmitter 0 Operation channel 3.  Routines for RSPI code running
 * on RX63N CPU, RSPI0.  This program uses RSPI0.  The Chip Select for the
 * flash device (LCD) is set to PC_2.  This is the RSPI channel used to
 * communicate with the ST7579 Graphics Display.
 *
 * The RSPI configuration has the baud rate generator set
 * for maximum speed, which is PLCK/2 or 25 MHz.
 *
 * Manufacturer: Renesas
 * Communications Protocol: SPI 3-Wire Transmitt only MASTER SPI
 * Transmitter Number: 0
 * channel: 3
 * Chip Select Port: PC.2
 * SPI Clock Speed: 25MHz
 * Slave Device: OKAYA LCD with ST7579 microprocessor
 *
 *-------------------------------------------------------------------------*/

#ifndef __GLYPH__YRDK_RX63N_SPI_0_HEADER_FILE
#define __GLYPH__YRDK_RX63N_SPI_0_HEADER_FILE

/******************************************************************************
Includes    Glyph Include   
******************************************************************************/
#include "Glyph.h"

/******************************************************************************
Prototypes for the Glyph Communications API
******************************************************************************/
void YRDKRX63N_CommandSend(int8_t cCommand) ;
void YRDKRX63N_DataSend(int8_t cData) ;
T_glyphError YRDKRX63N_RSPIOpen(T_glyphHandle aHandle);
void YRDKRX63N_RSPIStart(void) ;
void YRDKRX63N_RSPIEnd(void) ;
void YRDKRX63N_RSPITransferOutWords(int16_t sLowWord, int16_t sHighWord) ;
void YRDKRX63N_RSPITransferOutWord8Bit(uint8_t cData) ;

#endif /* __GLYPH__YRDK_RX63N_SPI_0_HEADER_FILE */


