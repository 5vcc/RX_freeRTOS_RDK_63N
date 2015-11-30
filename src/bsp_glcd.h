/*
*********************************************************************************************************
*
*                                         BOARD SUPPORT PACKAGE
*
*                                             Renesas RX63N
*                                                on the
*                                       YRDKRX63N Evaluation Board
*
*                                 (c) Copyright 2014, Micrium, Weston, FL
*                                          All Rights Reserved
*
* Filename      : bsp_glcd.h
* Version       : V1.00
* Programmer(s) : FGK
*                 JPB
*                 JM
*********************************************************************************************************
*/

#ifndef  BSP_GLCD_H_
#define  BSP_GLCD_H_


/*
*********************************************************************************************************
*                                              INCLUDES
*********************************************************************************************************
*/
#include  <stdint.h>
#include  "Glyph.h"


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  BSP_GLCD_FONT_SYMBOL   2u
#define  BSP_GLCD_FONT_LARGE    1u
#define  BSP_GLCD_FONT_SMALL    0u


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  BSP_GraphLCD_Init        (void);
void  BSP_GraphLCD_Clear       (void);
void  BSP_GraphLCD_SetFont     (       uint8_t   font);
void  BSP_GraphLCD_ClrLine     (       uint8_t   line);
void  BSP_GraphLCD_String      (       uint8_t   line,
                                const  uint8_t    *p_str);
void  BSP_GraphLCD_StringPos   (       uint8_t   line,
		                               uint8_t   pos,
                                const  uint8_t    *p_str);

void  BSP_GraphLCD_StringPosLen(       uint8_t   line,
									   uint8_t   pos,
                                const  uint8_t    *p_str,
                                       uint32_t   len_max);

void  BSP_GraphLCD_ProgBarPos  (       uint8_t   line,
                                       int8_t   pos);

void  BSP_GraphLCD_CharPixel   (       uint8_t   line,
		                               uint8_t   px,
		                               uint8_t     chr);

#endif
