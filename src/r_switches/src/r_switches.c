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
* File Name    : r_switches.c
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

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* NULL define. */
#include <stddef.h>
/* Board and MCU support. */
//#include "platform.h"
#include "iodefine.h"
/* Switches prototypes. */
#include "r_switches_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* This helps reduce the amount of unique code for each supported board. */
#define X_IRQ( x )   XX_IRQ( x )
#define XX_IRQ( x )  _ICU_IRQ##x

/* These macros define which IRQ pins are used for the switches. Note that these defintions cannot have parentheses 
   around them. */
#define SW1_IRQ_NUMBER     8
#define SW2_IRQ_NUMBER     9
#define SW3_IRQ_NUMBER     12

/* Number of switches on this board. */
#define SWITCHES_NUM            (3)
/* What is the first switch number? On most Renesas RDKs and RSKs switches start at 1. So there is not SW0. Putting this
   in here in case we get a board with a SW0. */
#define SWITCHES_STARTING_NUM   (1)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct
{
    bool    active;
    int32_t debounce_cnt;
} switch_t;

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#if SWITCHES_CFG_DETECTION_MODE == 1
/* Update Hz */
static uint32_t g_sw_debounce_cnts;
/* Used for debounce. */
switch_t g_switches[SWITCHES_NUM];
#endif

static bool sw_debounce(uint32_t sw_num);
static bool sw_active(uint32_t sw_num);
static void sw_call_callback(uint32_t sw_num);

/***********************************************************************************************************************
* Function Name: R_SWITCHES_Init
* Description  : Initializes pins to be input and interrupt on switch presses.
* Arguments    : detection_hz -
*                    The times per second that the user will call R_SWITCHES_Update(). NOTE: this is only when using
*                    polling mode. If you are using interrupt mode, then this argument will be ignored.
*                debouce_counts -
*                    The number of times to check the port value before accepting the change. The slower the rate at 
*                    which R_SWITCHES_Update() will likely lower this number.
* Return Value : none
***********************************************************************************************************************/
void R_SWITCHES_Init (uint32_t detection_hz, uint32_t debounce_counts)
{      
    /* The SW#_XXX defintions are common macros amongst different boards. To see the definitions for these macros
       see the board defintion file. For example, this file for the RSKRX63N is rskrx63n.h. */

    /* TODO: The MPC code below should be moved to r_bsp in one way or another. */
#if SWITCHES_CFG_DETECTION_MODE == 0

	PORT4.PDR.BIT.B0 = 0;
	PORT4.PMR.BIT.B0 = 1;

	PORT4.PDR.BIT.B1 = 0;
	PORT4.PMR.BIT.B1 = 1;

	PORT4.PDR.BIT.B2 = 0;
	PORT4.PMR.BIT.B2 = 1;
    /* The switches on the RDKRX63N are connected to the following pins/IRQ's    
    Switch  Port    IRQ
    ------  ----    ----
    SW1     P4.0    IRQ8
    SW2     P4.1    IRQ9
    SW3     P4.4    IRQ12
    */
	MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    MPC.P40PFS.BYTE = 0x40;    /* P40 is used as IRQ pin */
    MPC.P41PFS.BYTE = 0x40;    /* P40 is used as IRQ pin */
    MPC.P44PFS.BYTE = 0x40;    /* P40 is used as IRQ pin */
    MPC.PWPR.BIT.PFSWE = 0;
    MPC.PWPR.BIT.B0WI = 1;

    #if defined(SWITCHES_CFG_SW1_CALLBACK_FUNCTION)

    /* Set IRQ type (falling edge) */
    ICU.IRQCR[SW1_IRQ_NUMBER].BIT.IRQMD  = 0x01; 
    /* Set interrupt priority to 3 */
    _IPR( X_IRQ(SW1_IRQ_NUMBER) ) = 3;
    /* Clear any pending interrupts */
    _IR( X_IRQ(SW1_IRQ_NUMBER) ) = 0;
    /* Enable the interrupts */
    _IEN( X_IRQ(SW1_IRQ_NUMBER) )  = 1; 

    #endif

    #if defined(SWITCHES_CFG_SW2_CALLBACK_FUNCTION)

    /* Set IRQ type (falling edge) */
    ICU.IRQCR[SW2_IRQ_NUMBER].BIT.IRQMD  = 0x01; 
    /* Set interrupt priority to 3 */
    _IPR( X_IRQ(SW2_IRQ_NUMBER) ) = 3;
    /* Clear any pending interrupts */
    _IR( X_IRQ(SW2_IRQ_NUMBER) ) = 0;
    /* Enable the interrupts */
    _IEN( X_IRQ(SW2_IRQ_NUMBER) )  = 1; 

    #endif

    #if defined(SWITCHES_CFG_SW3_CALLBACK_FUNCTION)

    /* Set IRQ type (falling edge) */
    ICU.IRQCR[SW3_IRQ_NUMBER].BIT.IRQMD  = 0x01; 
    /* Set interrupt priority to 3 */
    _IPR( X_IRQ(SW3_IRQ_NUMBER) ) = 3;
    /* Clear any pending interrupts */
    _IR( X_IRQ(SW3_IRQ_NUMBER) ) = 0;
    /* Enable the interrupts */
    _IEN( X_IRQ(SW3_IRQ_NUMBER) )  = 1;    

    #endif

#else

    uint32_t i;

    /* This is based upon having 3 counts at 10Hz. */
    g_sw_debounce_cnts = debounce_counts;

    /* Init debounce structures. */
    for (i = 0; i < SWITCHES_NUM; i++)
    {
        g_switches[i].active = false;
        g_switches[i].debounce_cnt = 0;
    }

#endif /* SWITCHES_CFG_DETECTION_MODE */

} 

/* Only define interrupts in interrupt detection mode. */
#if SWITCHES_CFG_DETECTION_MODE == 0

    #if defined(SWITCHES_CFG_SW1_CALLBACK_FUNCTION)
/***********************************************************************************************************************
* Function name: sw1_isr
* Description  : Sample ISR for switch 1 input (must do hardware setup first!)
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
#pragma interrupt (sw1_isr (vect=_VECT(X_IRQ(SW1_IRQ_NUMBER))))
static void sw1_isr (void) 
{
    sw_call_callback(1);
}    
    #endif /* SWITCHES_CFG_SW1_CALLBACK_FUNCTION */
    
    #if defined(SWITCHES_CFG_SW2_CALLBACK_FUNCTION)
/***********************************************************************************************************************
* Function name: sw2_isr
* Description  : Sample ISR for switch 2 input (must do hardware setup first!)
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
#pragma interrupt (sw2_isr (vect=_VECT(X_IRQ(SW2_IRQ_NUMBER))))
static void sw2_isr (void) 
{
    sw_call_callback(2);
}    
    #endif /* SWITCHES_CFG_SW2_CALLBACK_FUNCTION */
    
    #if defined(SWITCHES_CFG_SW3_CALLBACK_FUNCTION)
/***********************************************************************************************************************
* Function name: sw3_isr
* Description  : Sample ISR for switch 3 input (must do hardware setup first!)
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
#pragma interrupt (sw3_isr (vect=_VECT(X_IRQ(SW3_IRQ_NUMBER))))
static void sw3_isr (void) 
{
    sw_call_callback(3);
}
    #endif /* SWITCHES_CFG_SW3_CALLBACK_FUNCTION */

#endif 

/* If using polling then the user must call the update function. */

/***********************************************************************************************************************
* Function name: R_SWITCHES_Update
* Description  : Polls switches and calls callback functions as needed. If you are using IRQ mode then this function
*                is not needed and can be removed if desired. It is left in so that code will not fail when switching
*                between polling or IRQ mode. 
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void R_SWITCHES_Update (void)
{
    uint32_t i;

#if SWITCHES_CFG_DETECTION_MODE == 1
    /* This code is only needed for polling mode. */

    for (i = 0; i < SWITCHES_NUM; i++)
    {
        if (true == sw_debounce(i+SWITCHES_STARTING_NUM))
        {
            /* Call callback function. */
            sw_call_callback(i+SWITCHES_STARTING_NUM);    
        }
    }
#endif /* SWITCHES_CFG_DETECTION_MODE */
}

/* This code is only needed for polling mode. */
#if SWITCHES_CFG_DETECTION_MODE == 1

/***********************************************************************************************************************
* Function Name: sw_debounce
* Description  : Sees if the switch is active or not and updates the debounce routine accordingly. If the number of 
*                debounce counts have been detected as active then the callback can be called.
* Arguments    : sw_num -
*                    Which switch to check.
* Return Value : true -
*                    Call the user callback function.
*                false -
*                    Do not call the user callback function.
***********************************************************************************************************************/
static bool sw_debounce (uint32_t sw_num)
{
    bool need_to_call_callback = false;

    if (true == sw_active(sw_num))
    {
        if (g_switches[sw_num].active != true)
        {
            if (++g_switches[sw_num].debounce_cnt >= g_sw_debounce_cnts)
            {           
                /* Set this to true so we only call the callback function once per press. */
                g_switches[sw_num].active = true;

                need_to_call_callback = true;    
            }
        }
    }
    else
    {
        /* Switch has been detected as pressed. Wait for it to be released. If we wanted we could have another alert
           on switch releases when active is set to false below. */
        if (0 == g_switches[sw_num].debounce_cnt)
        {
            g_switches[sw_num].active = false;
        }
        else
        {
            g_switches[sw_num].debounce_cnt--;
        }
    }

    return need_to_call_callback;
}

/***********************************************************************************************************************
* Function Name: sw_active
* Description  : Returns whether the current state of the switch.
* Arguments    : sw_num -
*                    Which switch to check.
* Return Value : true -
*                    Switch is currently active.
*                false -
*                    Switch is not active.
***********************************************************************************************************************/
static bool sw_active (uint32_t sw_num)
{
    bool active = false;

    if (1 == sw_num)
    {
        if (SW1 == SW_ACTIVE)
        {
            active = true;
        }
    }
    else if (2 == sw_num)
    {
        if (SW2 == SW_ACTIVE)
        {
            active = true;
        }
    }
    else
    {
        if (SW3 == SW_ACTIVE)
        {
            active = true;
        }
    }

    return active;
}

#endif /* SWITCHES_CFG_DETECTION_MODE == 1 */

/***********************************************************************************************************************
* Function Name: sw_call_callback
* Description  : Calls a switches callback function.
* Arguments    : sw_num -
*                    Which switches callback to call.
* Return Value : none
***********************************************************************************************************************/
static void sw_call_callback (uint32_t sw_num)
{
    /* Allocate space for structure to send in to callback. */
    sw_callback_arg_t   arg;

    /* Fill in structure members. */
    arg.switch_number = sw_num;

#if defined(SWITCHES_CFG_SW1_CALLBACK_FUNCTION)
    if (1 == sw_num)
    {
        if ((NULL != SWITCHES_CFG_SW1_CALLBACK_FUNCTION) && (NULL != SWITCHES_CFG_SW1_CALLBACK_FUNCTION))
        {       
            /* Call callback function. */
            SWITCHES_CFG_SW1_CALLBACK_FUNCTION((void *)&arg);
        }
    }
#endif

#if defined(SWITCHES_CFG_SW2_CALLBACK_FUNCTION)
    if (2 == sw_num)
    {
        if ((NULL != SWITCHES_CFG_SW2_CALLBACK_FUNCTION) && (NULL != SWITCHES_CFG_SW2_CALLBACK_FUNCTION))
        {       
            /* Call callback function. */
            SWITCHES_CFG_SW2_CALLBACK_FUNCTION((void *)&arg);
        }
    }
#endif

#if defined(SWITCHES_CFG_SW3_CALLBACK_FUNCTION)
    if (3 == sw_num)
    {
        if ((NULL != SWITCHES_CFG_SW3_CALLBACK_FUNCTION) && (NULL != SWITCHES_CFG_SW3_CALLBACK_FUNCTION))
        {       
            /* Call callback function. */
            SWITCHES_CFG_SW3_CALLBACK_FUNCTION((void *)&arg);
        }
    }
#endif
}

/***********************************************************************************************************************
* Function Name: R_SWITCHES_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25 
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
#pragma inline(R_SWITCHES_GetVersion)
uint32_t R_SWITCHES_GetVersion (void)
{
    /* These version macros are defined in r_switches_if.h. */
    return ((((uint32_t)SWITCHES_VERSION_MAJOR) << 16) | (uint32_t)SWITCHES_VERSION_MINOR);
} 


