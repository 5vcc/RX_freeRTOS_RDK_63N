/*
 * Menu.c
 *
 *  Created on: 19/12/2014
 *      Author: LAfonso01
 */
#include <stdio.h>
#include "iodefine.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Menu.h"
#include "bsp_glcd.h"
#include "r_switches_if.h"
#include "semphr.h"

typedef enum {
	MENU1 = 0,
	MENU2,
	MENU3,
	MENU4
}menus_t;


void *(*pfunc)(void *);

void menu1 (void);
void menu2 (void);
void menu3 (void);
void menu4 (void);

static void  PrintLCD (int8_t   line,
				   char    *format,
				   ...);

void sw1_callback(void);
void sw2_callback(void);
void sw3_callback(void);

menus_t menupos = 0;
bool	menuEnter = false;
xSemaphoreHandle menuSemaphore;

void menuController(void)
{
	vSemaphoreCreateBinary(menuSemaphore);
	while(1)
	{
		switch(menupos)
		{
			case MENU1: PrintLCD(0u,"Menu1");
						pfunc = &menu1;
				break;
			case MENU2: PrintLCD(0u,"Menu2");
						pfunc = &menu2;
				break;
			case MENU3: PrintLCD(0u,"Menu3");
						pfunc = &menu3;
				break;
			case MENU4: PrintLCD(0u,"Menu4");
						pfunc = &menu4;
				break;
			default: PrintLCD(0u,"Merda");
		}
		if (menuEnter != false)
		{
			pfunc(1);
			menuEnter = false;
		}
		xSemaphoreTake(menuSemaphore,portMAX_DELAY);
//		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

void menu1 (void)
{
	PrintLCD(1u,"Teste1");
}

void menu2 (void)
{
	PrintLCD(1u,"Teste2");
}

void menu3 (void)
{
	PrintLCD(1u,"Teste3");
}

void menu4 (void)
{
	PrintLCD(1u,"Teste4");
}

void sw1_callback(void)
{
	if (MENU4 > menupos)
		menupos++;
	xSemaphoreGive(menuSemaphore);
}

void sw2_callback(void)
{
	if (MENU1 < menupos)
		menupos--;
	xSemaphoreGive(menuSemaphore);
}

void sw3_callback(void)
{
	menuEnter = true;
	xSemaphoreGive(menuSemaphore);
}

/*
*********************************************************************************************************
*                                           AppTraceLCD
*
* Description : Output traces to LCD.
*
* Arguments   : line      Line to siplay message.
*
*               format    String of the message
*
* Returns     : none.
*
* Notes       : none.
*********************************************************************************************************
*/


static void  PrintLCD (int8_t   line,
				   char    *format,
				   ...)
{
    static  char  buffer[20 + 1];
            va_list    vArgs;


    va_start(vArgs, format);
    vsprintf((char *)buffer, (char const *)format, vArgs);
    va_end(vArgs);

    BSP_GraphLCD_ClrLine(line);
    BSP_GraphLCD_String(line, buffer);
}
