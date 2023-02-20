#ifndef __EPD_2IN7_V2_H_
#define __EPD_2IN7_V2_H_

#include "DEV_Config.h"

// Display resolution
#define EPD_2IN7_V2_WIDTH       176
#define EPD_2IN7_V2_HEIGHT      264

void EPD_2IN7_V2_Init(void);
void EPD_2IN7_V2_Init_Fast(void);
void EPD_2IN7_V2_Init_4GRAY(void);
void EPD_2IN7_V2_Clear(void);
void EPD_2IN7_V2_Display(UBYTE *Image);
void EPD_2IN7_V2_Display_Fast(UBYTE *Image);
void EPD_2IN7_V2_Display_Base(UBYTE *Image);
void EPD_2IN7_V2_Display_Base_color(UBYTE color);
void EPD_2IN7_V2_Display_Partial(const UBYTE *Image, UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yende);
void EPD_2IN7_V2_4GrayDisplay(UBYTE *Image);
void EPD_2IN7_V2_Sleep(void);

#endif
