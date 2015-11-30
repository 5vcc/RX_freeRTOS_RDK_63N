/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_switches_if.h
* Description  : Functions for using switches with callback functions. 
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 17.01.2012 1.00    First Release            
*         : 17.02.2012 1.10    Added RSKRX210 support.     
*         : 08.03.2012 1.20    Added GetVersion() function (though it's really a macro).
*         : 04.06.2012 1.30    Code can now be interrupt or poll driven.
*         : 07.11.2012 1.40	   Added support for RSKRX111
*         : 14.02.2013 1.50    Updated to be compliant with FIT Module Spec v1.02. Callbacks now pass which switch
*                              was pressed.
***********************************************************************************************************************/

#ifndef SWITCHES_API_HEADER_FILE
#define SWITCHES_API_HEADER_FILE

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>
/* Used for configuring the code */
#include "r_switches_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Version Number of API. */
#define SWITCHES_VERSION_MAJOR           (1)
#define SWITCHES_VERSION_MINOR           (50)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* The structure that is sent as an argument to switch callback functions. To use this cast the (void * pdata) pointer
   to (sw_callback_arg_t *) and then you can deference the structure. */
typedef struct
{
    uint32_t switch_number;     //Which switch was pressed
} sw_callback_arg_t;

/***********************************************************************************************************************
Public Functions
***********************************************************************************************************************/
void R_SWITCHES_Init(uint32_t detection_hz, uint32_t debounce_counts);
void R_SWITCHES_Update(void);

/* Callback prototypes. */
#if defined(SWITCHES_CFG_SW1_CALLBACK_FUNCTION)
void SWITCHES_CFG_SW1_CALLBACK_FUNCTION(void * pdata);
#endif

#if defined(SWITCHES_CFG_SW2_CALLBACK_FUNCTION)
void SWITCHES_CFG_SW2_CALLBACK_FUNCTION(void * pdata);
#endif 

#if defined(SWITCHES_CFG_SW3_CALLBACK_FUNCTION)
void SWITCHES_CFG_SW3_CALLBACK_FUNCTION(void * pdata);
#endif

#endif /* SWITCHES_API_HEADER_FILE */

