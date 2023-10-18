#ifndef _DS_SCREEN_H_
#define _DS_SCREEN_H_

#include "ds_spi.h"

#define EPD_WIDTH   152
#define EPD_HEIGHT  152
#define EPD_ARRAY  EPD_WIDTH*EPD_HEIGHT/8  
//Full screen refresh display
void EPD_Init(void); 
void EPD_Init_180(void);	
void EPD_WhiteScreen_ALL(const unsigned char *datas);
void EPD_WhiteScreen_White(void);
void EPD_WhiteScreen_Black(void);
void EPD_DeepSleep(void);
//Partial refresh display 
void EPD_Init_Part(void);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_PartAll(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
								 
//Fast refresh display
//void EPD_Init_Fast(void);
//void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas);   
//GUI display				
void EPD_Init_GUI(void);												
void EPD_Display(unsigned char *Image); 
void EPD_interface_init(void);
void EPD_selftest(void);
#endif