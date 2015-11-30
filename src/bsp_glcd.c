/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
*
*               BSP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                            Renesas RX63N
*                                                on the
*                                      YRDKRX63N Evaluation Board
*
* Filename      : bsp_glcd.c
* Version       : V1.00
* Programmer(s) : FGK
*                 JPB
*                 JM
*********************************************************************************************************
*/
#include    <stdint.h>
#include    <stdbool.h>
#include  <bsp_glcd.h>
#include  <iodefine.h>
#include "FreeRTOS.h"
#include "semphr.h"


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

static  T_glyphHandle  BSP_GraphLCD;
static xSemaphoreHandle   GLCD_Mutex;


/*
*********************************************************************************************************
*                                      GRAPH LCD INITIALIZATION
*********************************************************************************************************
*/

void  BSP_GraphLCD_Init (void)
{
    T_glyphError  err;


    GLCD_Mutex = xSemaphoreCreateMutex();
    if (GLCD_Mutex == NULL) {
        return;
    }

    xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);
    err = GlyphOpen(&BSP_GraphLCD, 0);

    if (err == GLYPH_ERROR_NONE) {
        GlyphNormalScreen(BSP_GraphLCD);
        GlyphClearScreen(BSP_GraphLCD);
    }
    xSemaphoreGive(GLCD_Mutex);
}


/*
*********************************************************************************************************
*                                       GRAPH LCD CLEAR SCREEN
*********************************************************************************************************
*/

void  BSP_GraphLCD_Clear (void)
{
	xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);
    GlyphClearScreen(BSP_GraphLCD);
    xSemaphoreGive(GLCD_Mutex);
}


/*
*********************************************************************************************************
*                                         GRAPH LCD SET FONT
*********************************************************************************************************
*/

void  BSP_GraphLCD_SetFont (uint8_t  font)
{
	xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);
    switch (font) {
        case BSP_GLCD_FONT_LARGE:
             GlyphSetFont(BSP_GraphLCD, GLYPH_FONT_8_BY_8);
             break;

        case BSP_GLCD_FONT_SMALL:
             GlyphSetFont(BSP_GraphLCD, GLYPH_FONT_5_BY_7);
             break;
    }
    xSemaphoreGive(GLCD_Mutex);
}


/*
*********************************************************************************************************
*                                       GRAPH LCD PRINT STRING
*********************************************************************************************************
*/

void  BSP_GraphLCD_String (       uint8_t   line,
                           const  uint8_t    *p_str)
{
    uint8_t  y;


    y = line * 9u;

    xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);
    GlyphSetXY (BSP_GraphLCD, 0u, y);
    GlyphString(BSP_GraphLCD, (uint8_t *)p_str, strlen(p_str));
    xSemaphoreGive(GLCD_Mutex);
}


/*
*********************************************************************************************************
*                                        GRAPH LCD CLEAR LINE
*********************************************************************************************************
*/

void  BSP_GraphLCD_ClrLine (uint8_t  line)
{
    uint8_t  y;


    y = line * 9u;

    xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);
    GlyphEraseBlock(BSP_GraphLCD, 0u, y, 95u, y + 7u);
    xSemaphoreGive(GLCD_Mutex);
}


/*
*********************************************************************************************************
*                                 GRAPH LCD PRINT STRING AT POSITION
*********************************************************************************************************
*/

void  BSP_GraphLCD_StringPos (       uint8_t   line,
                                     uint8_t   pos,
                              const  uint8_t    *p_str)
{
    uint8_t  x;
    uint8_t  y;


    x = pos  * 5u;
    y = line * 9u;

    xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);

    GlyphSetXY (BSP_GraphLCD, x, y);
    GlyphString(BSP_GraphLCD, (uint8_t *)p_str, strlen(p_str));

    xSemaphoreGive(GLCD_Mutex);
}


/*
*********************************************************************************************************
*                                 GRAPH LCD PRINT PROGRESS BAR AT POSITION
*********************************************************************************************************
*/

void  BSP_GraphLCD_ProgBarPos (uint8_t  line,
                               int8_t  pos)
{
    T_glyphFont  font;
    uint8_t   x;
    uint8_t   y;
    uint8_t     chr;


    y =  line *  9u;
    x = (pos  * 86) / 100 + 2;

    xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);
    GlyphGetFont(BSP_GraphLCD, &font);

    chr = 6u;
    GlyphSetXY (BSP_GraphLCD, 0, y);
    GlyphString(BSP_GraphLCD, (uint8_t *)&chr, 1u);
    if (pos >= 0) {
        chr = 7u;
        GlyphSetXY (BSP_GraphLCD, x, y);
        GlyphString(BSP_GraphLCD, (uint8_t *)&chr, 1u);
    }

    GlyphSetFont(BSP_GraphLCD, font);
    xSemaphoreGive(GLCD_Mutex);
}


/*
*********************************************************************************************************
*                           GRAPH LCD PRINT STRING AT POSITION WITH MAX LEN
*********************************************************************************************************
*/

void  BSP_GraphLCD_StringPosLen (       uint8_t   line,
                                        uint8_t   pos,
                                 const  uint8_t    *p_str,
                                        uint32_t   len_max)
{
    uint8_t  x;
    uint8_t  y;
    uint32_t  len;


    x = pos  * 5u;
    y = line * 9u;
    if (strlen(p_str) < (len_max + 1u))
    {
    	len = strlen(p_str);
    }
    else
    {
    	len = len_max;
    }

    xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);
    GlyphSetXY (BSP_GraphLCD, x, y);
    GlyphString(BSP_GraphLCD, (uint8_t *)p_str, len);
    xSemaphoreGive(GLCD_Mutex);
}


/*
*********************************************************************************************************
*                             GRAPH LCD PRINT CHARACTER AT PIXEL POSITION
*********************************************************************************************************
*/

void  BSP_GraphLCD_CharPixel (uint8_t  line,
                              uint8_t  px,
                              uint8_t    chr)
{
    uint8_t  y;


    y = line * 9u;

    xSemaphoreTake(GLCD_Mutex,portMAX_DELAY);
    GlyphSetXY (BSP_GraphLCD, px, y);
    GlyphString(BSP_GraphLCD, (uint8_t *)&chr, 1);
    xSemaphoreGive(GLCD_Mutex);
}
