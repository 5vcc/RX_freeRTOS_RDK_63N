// ***********************************************************************
// LCD menu
// ***********************************************************************



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//************************************************************************
// Projekt includes						// Include your own functions here
//************************************************************************

#include "iodefine.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_glcd.h"
#include "r_switches_if.h"
#include "semphr.h"
#include "lcd_menu.h"
#include "diskio.h"
#include "ff.h"


void sw1_callback(void);
void sw2_callback(void);
void sw3_callback(void);
void openSD(void);

FATFS FatFs[_VOLUMES];	/* File system object */
FIL File[2];			/* File objects */
DIR Dir;				/* Directory object */
FILINFO Finfo;			/* File properties (fs/fl command) */
#if _USE_LFN
char Lfname[_MAX_LFN + 1];
#endif
char *ptr;
char fileNames[7][12];   /* This function assumes non-Unicode configuration */

void liga_led(void)
{
	/* All outputs LED in PORTE are on */
		PORTE.PODR.BYTE = 0x00;
}
void desliga_led(void)
{
	/* All outputs LED in PORTE are off */
		PORTE.PODR.BYTE = 0xFF;
}


bool btnDown = false;
bool btnUp = false;
bool btnEnter = false;
xSemaphoreHandle menuSemaphore;

// ***********************************************************************
// Macros
// ***********************************************************************
// Debounce macros
#define UP_KEY					btnUp
#define DOWN_KEY				btnDown
#define ENTER_KEY				btnEnter

// Delay macro
#define DELAY_MS(a)				DelayMs(a)

// LCD macros
#define LCDWriteChar(a)  		LCDWriteData(a)
#define LCDWriteFromROM(a,b,c) 	LCDWriteStringROM(a,b,c,1)

// ***********************************************************************
// Variable definitions
// ***********************************************************************
// General
static unsigned char selected = 1;			// Start with first entry (apart from header)

// Menu strings
const char menu_000[] = " Principal";  		// 0
const char menu_001[] = "  Liga LED";  		// 1
const char menu_002[] = "  Desl. LED";  	// 2
const char menu_003[] = "  Open SD";  		// 3
const char menu_004[] = "  Proximo";  		// 4

const char menu_100[] = " [Header1]";  		// 5
const char menu_101[] = "  Option101";  	// 6
const char menu_102[] = "  Option102";  	// 7
const char menu_103[] = "  Proximo";  		// 8
const char menu_104[] = "  Anterior";		// 9

const char menu_200[] = " [Header2]";  		// 10
const char menu_201[] = "  Option201";  	// 11
const char menu_202[] = "  Option202";  	// 12
const char menu_203[] = "  Proximo";  		// 13
const char menu_204[] = "  Anterior";		// 14

const char menu_300[] = " [g files]";  		// 15
char menu_301[] = "";  						// 16
char menu_302[] = "";  						// 17
char menu_303[] = "";  						// 18
const char menu_305[] = "  Anterior";  		// 19


// Array of entries
MenuEntry menu[] =
{
	{menu_000, 5,  0,  0,  0,  0}, 			// 0
	{menu_001, 5,  4,  2,  1,  liga_led},
	{menu_002, 5,  1,  3,  2,  desliga_led},
	{menu_003, 5,  2,  4, 16,  openSD},
	{menu_004, 5,  3,  1,  6,  0},			// 4

	{menu_100,  5,  0,  0,  0,  0},			// 5
	{menu_101,  5,  9,  7,  6,  0},
	{menu_102,  5,  6,  8,  7,  0},
	{menu_103,  5,  7,  9, 11,  0},
	{menu_104,  5,  8,  6,  1,  0},			// 9

	{menu_200,  5,  0,  0,  0,  0},			// 10
	{menu_201,  5, 14, 12, 11,  0},
	{menu_202,  5, 11, 13, 12,  0},
	{menu_203,  5, 12, 14, 16,  0},
	{menu_204,  5, 13, 11,  6,  0},			// 14

	{menu_300,  5,  0,  0,  0,  0},			// 15
	{fileNames[0],  5, 19, 17, 16,  0},
	{fileNames[1],  5, 16, 18, 17,  0},
	{fileNames[2],  5, 17, 19, 18,  0},
	{menu_305,  5, 18, 16,  11,  0}			// 19
};



// ***********************************************************************
// Show menu function
// ***********************************************************************
void show_menu(void)
{	unsigned char line_cnt;					// Count up lines on LCD
 	unsigned char from;
 	unsigned char till = 0;

	unsigned char temp;						// Save "from" temporarily for always visible header and cursor position
 
	 // Get beginning and end of current (sub)menu
 	while (till <= selected)
 	{	till += menu[till].num_menupoints;
 	}
	from = till - menu[selected].num_menupoints;
 	till--;

	temp = from;							// Save from for later use


	//--------------------------------------------------------------------
	// Always visible Header
	//--------------------------------------------------------------------
#if         defined USE_ALWAYS_VISIBLE_HEADER

	line_cnt = 1;							// Set line counter to one since first line is reserved for header

	// Write header
	LCDWriteFromROM(0,0,menu[temp].text);
	

	// Output for two row display becomes fairly easy
	#if defined USE_TWO_ROW_DISPLAY

		LCDWriteFromROM(0,UPPER_SPACE,menu[selected].text);
		gotoxy(0,UPPER_SPACE);
		LCDWriteChar(SELECTION_CHAR);

	#endif	// USE_TWO_ROW_DISPLAY


	// Output for four row display
	#if defined USE_FOUR_ROW_DISPLAY	||	defined		USE_THREE_ROW_DISPLAY

		// Output formatting for selection somewhere in between (sub)menu top and bottom
		if ( (selected >= (from + UPPER_SPACE)) && (selected <= (till - LOWER_SPACE)) )
    	{	from = selected - (UPPER_SPACE - 1);
			till = from + (DISPLAY_ROWS - 2);

	 		for (from; from<=till; from++)
	 		{	LCDWriteFromROM(0,line_cnt,menu[from].text);
	 	 	 	line_cnt++;
			}
	
			gotoxy(0, UPPER_SPACE);
			LCDWriteChar(SELECTION_CHAR);
		}

		// Output formatting for selection close to (sub)menu top and bottom 
		// (distance between selection and top/bottom defined as UPPER- and LOWER_SPACE)
		else
		{	// Top of (sub)menu
			if (selected < (from + UPPER_SPACE))
			{	from = selected;
				till = from + (DISPLAY_ROWS - 2);
	 			
				for (from; from<=till; from++)
	 			{	LCDWriteFromROM(0,line_cnt,menu[from].text);
	 	 	 		line_cnt++;
				}
	
				gotoxy(0, (UPPER_SPACE-1));
				LCDWriteChar(SELECTION_CHAR);
			}
		
			// Bottom of (sub)menu
			if (selected == till)
			{	from = till - (DISPLAY_ROWS - 2);
	
				for (from; from<=till; from++)
	 			{	LCDWriteFromROM(0,line_cnt,menu[from].text);
	 	 	 		line_cnt++;
				}
	
				gotoxy(0, (DISPLAY_ROWS - 1));
				LCDWriteChar(SELECTION_CHAR);
			}
		}

	#endif	// USE_FOUR_ROW_DISPLAY


	//--------------------------------------------------------------------
	// Header not always visible
	//--------------------------------------------------------------------
#else	// !defined USE_ALWAYS_VISIBLE_HEADER

	line_cnt = 0;							// Set line counter to zero since all rows will be written

	// Output formatting for selection somewhere in between (sub)menu top and bottom
	if ( (selected >= (from + UPPER_SPACE)) && (selected <= (till - LOWER_SPACE)) )
    {	from = selected - UPPER_SPACE;
		till = from + (DISPLAY_ROWS - 1);

 		for (from; from<=till; from++)
 		{
 			BSP_GraphLCD_ClrLine(line_cnt);
 			BSP_GraphLCD_StringPos(line_cnt,0,menu[from].text);
 	 	 	line_cnt++;
		}
 		BSP_GraphLCD_CharPixel(UPPER_SPACE,0,SELECTION_CHAR);
	}

	// Output formatting for selection close to (sub)menu top and bottom 
	// (distance between selection and top/bottom defined as UPPER- and LOWER_SPACE)
	else
	{	// Top of (sub)menu
		if (selected < (from + UPPER_SPACE))
		{ 	uint8_t dif = till;
			till = from + (DISPLAY_ROWS - 1);
			for (from; from<=till; from++)
 			{	BSP_GraphLCD_ClrLine(line_cnt);
 				if (from <= dif)
 				{
 					BSP_GraphLCD_StringPos(line_cnt,0,menu[from].text);
 				}
 	 	 		line_cnt++;
			}

			BSP_GraphLCD_CharPixel((selected-(till-UPPER_SPACE)),0,SELECTION_CHAR);
		}
	
		// Bottom of (sub)menu
		if (selected == till)
		{	from = till - (DISPLAY_ROWS - 1);

			for (from; from<=till; from++)
 			{	BSP_GraphLCD_ClrLine(line_cnt);
				BSP_GraphLCD_StringPos(line_cnt,0,menu[from].text);
 	 	 		line_cnt++;
			}

			BSP_GraphLCD_CharPixel((DISPLAY_ROWS - 1),0,SELECTION_CHAR);
		}
	}

#endif	// !defined USE_ALWAYS_VISIBLE_HEADER

}

// ***********************************************************************
// Browse menu function
// ***********************************************************************
void browse_menu(void)
{
	vSemaphoreCreateBinary(menuSemaphore);
	do
 	{	show_menu();
 		xSemaphoreTake(menuSemaphore,portMAX_DELAY);
	 	if (UP_KEY)
	 	{selected = menu[selected].up;
	 	 UP_KEY = false;
	 	}

     	if (DOWN_KEY)
	 	{selected = menu[selected].down;
	 	 DOWN_KEY = false;
	 	}

	 	if (ENTER_KEY)
	 	{
	   	 if (menu[selected].fp != 0)
	  	 menu[selected].fp();

	  	 selected = menu[selected].enter;
	  	 ENTER_KEY = false;
	 	}


 	}while(1);
}

// ***********************************************************************
// Add user functions & variables
// ***********************************************************************

void sw1_callback(void)
{
	UP_KEY = true;
	xSemaphoreGive(menuSemaphore);
}

void sw2_callback(void)
{
	DOWN_KEY = true;
	xSemaphoreGive(menuSemaphore);
}

void sw3_callback(void)
{
	ENTER_KEY = true;
	xSemaphoreGive(menuSemaphore);
}

void openSD(void)
{
	DSTATUS diskStatus = 1;
	FIL fil;       /* File object */
	char line[82]; /* Line buffer */
	char lineout[10]; /* Line buffer */
	UINT cnt;
	FRESULT fr;    /* FatFs return code */
	char i = 0;
	char *fn;
#if _USE_LFN
	Finfo.lfname = Lfname;
	Finfo.lfsize = sizeof Lfname;
#endif
	diskStatus = disk_initialize(0);
	if (diskStatus == 0)
	{
		/* Register work area to the default drive */
		fr = f_mount(&FatFs[0],"", 0);
		fr = f_opendir(&Dir, "");
		for(;;) {
			fr = f_readdir(&Dir, &Finfo);
			if ((fr != FR_OK) || !Finfo.fname[0]) break;
            if (Finfo.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
            fn = Finfo.fname;
#endif
            sprintf(fileNames[i], " %s", fn);
            i++;
		}
		/* Open a text file */
//		fr = f_open(&File[0], "teste2.txt", FA_READ);
//		//if (fr) return (int)fr;
//
//		/* Read all lines and display it */
//		fr = f_read(&File[0],lineout,sizeof(lineout),&cnt);
//		strcpy(menu[29].text,lineout,sizeof(lineout));
//		/* Close the file */
//		f_close(&fil);

	}
}
