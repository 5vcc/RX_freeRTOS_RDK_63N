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
* File Name     : r_main1_task.c
* Version       : V.1.00
* Device        : RX63N
* H/W Platform  : YRDKRX63N specific
* OS            : FreeRTOS
* Description   : This sample will demonstrate basic of creation task
* Operation     : 1. Build and download the tutorial project to the RDK63N
*                 2. Click 'Restart' to start the software
*                 3. Two sets of LED on "Renesas RX63N" will flash.
*                    One set will flash after delay 500 tick count
*                    Another set will flash when interrupt happens
*                    until demo is stopped
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY     Version     Description
*           20.06.2013     V.1.00      Initial version
*******************************************************************************/

#include "iodefine.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_glcd.h"
#include "lcd_menu.h"
#include "r_switches_if.h"
#include "diskio.h"

extern void R_Operating_frequency_set();
extern void R_CMT1_Setup();

void Menu_task (void* param);
void main (void);
/******************************************************************************
* Function Name : LED_task
* Description   : Runs LED_task code.
*                 Change status of LED after 500 tick count
* Arguments     : void *param
* Return value  : none
*******************************************************************************/
void Menu_task (void * param)
{
	browse_menu();
}


/******************************************************************************
* Function name: main
* Description  : Main program function.
*                Set PORTE, PORTD is output
*                Set frequency for device
*                Turn off all LED of PORTE, PORTD
*                Setup and start CMT1, when this interrupt happens LED of PORTD will be flashed
*                Create Task 1 to control PORTE
*                Start scheduler
* Arguments    : none
* Return value : none
******************************************************************************/
void main(void)
{

	setpsw_i();
	/* PORTE is output */
	PORTE.PDR.BYTE  = 0xFF;

	/* All outputs LED in PORTE are off */
	PORTE.PODR.BYTE = 0xFF;

	/* PORTD is output */
	PORTD.PDR.BYTE  = 0xFF;

	/* All outputs LED in PORTD are off */
	PORTD.PODR.BYTE = 0xFF;

	/* Setup frequency for device */
	R_Operating_frequency_set();

	PORT4.PDR.BIT.B0 = 0;
	PORT4.PMR.BIT.B0 = 1;
	R_SWITCHES_Init(0,10);

	/* Setup Glyph */
	BSP_GraphLCD_Init();
	R_CMT1_Setup();


	/* Create Task 1 */
	xTaskCreate(Menu_task, "Task Menu", 10000, NULL, 1, NULL );

	/* Start scheduler */
	vTaskStartScheduler();
	while (1)
	{

	}
}

