#ifndef _DS_SCREEN_H_
#define _DS_SCREEN_H_

#include "ds_spi.h"

#define EPD_WIDTH   152
#define EPD_HEIGHT  152
#define EPD_ARRAY  EPD_WIDTH*EPD_HEIGHT/8  
//EPD settings Functions
void EPD_HW_Init(void); //Electronic paper initialization

//All screen update 
void EPD_WhiteScreen_ALL(const unsigned char * datas);

void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);

//Part update
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);

//EPD self test function
void EPD_selftest(void);

#endif