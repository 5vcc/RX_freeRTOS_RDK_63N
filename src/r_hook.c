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
* File Name     : r_hook.c
* Version       : V.1.00
* Device        : RX63N
* H/W Platform  : YRDKRX63N specific
* Description   : Implements required user settings for FreeRTOS.
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY     Version     Description
*           20.06.2013     V.1.00      Initial version
*******************************************************************************/

#include "iodefine.h"
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "queue.h"

volatile unsigned long ulHighFrequencyTickCount = 0;

/******************************************************************************
* Function name: vApplicationStackOverflowHook
* Description  : Hook function for Stack Overflow failures - A requirement for
*                FreeRTOS
* Arguments    : As documented in FreeRTOS
*                xTaskHandle *pxTask -
*                   pointer to the task
*                signed char *pcTaskName
*                   pointer to the task name
* Return value : none
*******************************************************************************/
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{

}

/******************************************************************************
* Function name: vApplicationMallocFailedHook
* Description  : Hook function for Malloc failures - A requirement for FreeRTOS
* Arguments    : none
* Return value : none
*******************************************************************************/
void vApplicationMallocFailedHook()
{

}


