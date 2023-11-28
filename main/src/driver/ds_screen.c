#include "ds_screen.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "ds_gpio.h"
#include "ds_data_image.h"

// static const char *TAG = "ds_screen";

void EPD_interface_init(void)
{
  gpio_screen_init();
  spi_init();
}

void EPD_selftest()
{

  EPD_HW_Init();                 // Electronic paper initialization
  EPD_WhiteScreen_ALL(gImage_menu); // Refresh the picture in full screen
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  //////////////////////Partial refresh digital presentation//////////////////////////////////////
  // EPD_HW_Init();                           // Electronic paper initialization
  // EPD_SetRAMValue_BaseMap(gImage_basemap); // Partial refresh background color,Brush map is a must, please do not delete
  // vTaskDelay(100 / portTICK_PERIOD_MS);
  // EPD_Dis_Part(0, 32, gImage_num1, 32, 32); // x,y,DATA,resolution 32*32
  // EPD_Dis_Part(0, 32, gImage_num2, 32, 32); // x,y,DATA,resolution 32*32
  // EPD_Dis_Part(0, 32, gImage_num3, 32, 32); // x,y,DATA,resolution 32*32
  // EPD_Dis_Part(0, 32, gImage_num4, 32, 32); // x,y,DATA,resolution 32*32
  // EPD_Dis_Part(0, 32, gImage_num5, 32, 32); // x,y,DATA,resolution 32*32
  // EPD_Dis_Part(0, 32, gImage_num6, 32, 32); // x,y,DATA,resolution 32*32
  // EPD_Dis_Part(0, 32, gImage_num7, 32, 32); // x,y,DATA,resolution 32*32
  // EPD_Dis_Part(0, 32, gImage_num8, 32, 32); // x,y,DATA,resolution 32*32
  // EPD_Dis_Part(0, 32, gImage_num9, 32, 32); // x,y,DATA,resolution 32*32
  // vTaskDelay(2000 / portTICK_PERIOD_MS);
  // ////////////////////////////////////////////////////////////////////////
  // EPD_HW_Init();
  // EPD_WhiteScreen_White();
  EPD_DeepSleep(); // Sleep
}

/////////////////EPD settings Functions/////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void EPD_HW_Init(void)
{
  gpio_set_screen_res(0);
  vTaskDelay(1 / portTICK_PERIOD_MS);
  gpio_set_screen_res(1); // hard reset
  vTaskDelay(1 / portTICK_PERIOD_MS);

  Epaper_READBUSY();
  spi_send_cmd(0x12); // SWRESET
  Epaper_READBUSY();

  spi_send_cmd(0x01); // Driver output control
  spi_send_data(0xC7);
  spi_send_data(0x00);
  spi_send_data(0x01);

  spi_send_cmd(0x11); // data entry mode
  spi_send_data(0x01);

  spi_send_cmd(0x44); // set Ram-X address start/end position
  spi_send_data(0x00);
  spi_send_data(0x18); // 0x0C-->(18+1)*8=200

  spi_send_cmd(0x45); // set Ram-Y address start/end position
  spi_send_data(0xC7);    // 0xC7-->(199+1)=200
  spi_send_data(0x00);
  spi_send_data(0x00);
  spi_send_data(0x00);

  spi_send_cmd(0x3C); // BorderWavefrom
  spi_send_data(0x05);

  spi_send_cmd(0x18);
  spi_send_data(0x80);

  spi_send_cmd(0x4E); // set RAM x address count to 0;
  spi_send_data(0x00);
  spi_send_cmd(0x4F); // set RAM y address count to 0X199;
  spi_send_data(0xC7);
  spi_send_data(0x00);
  Epaper_READBUSY();
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char *datas)
{
  unsigned int i;
  spi_send_cmd(0x24); // write RAM for black(0)/white (1)
  for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
  {
    spi_send_data(datas[i]);
  }
  EPD_Update();
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{
  spi_send_cmd(0x22);
  spi_send_data(0xF7);
  spi_send_cmd(0x20);
  Epaper_READBUSY();
}
void EPD_Part_Update(void)
{
  spi_send_cmd(0x22);
  spi_send_data(0xFF);
  spi_send_cmd(0x20);
  Epaper_READBUSY();
}
void EPD_DeepSleep(void)
{
  spi_send_cmd(0x10); // enter deep sleep
  spi_send_data(0x01);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}

// static void Epaper_READBUSY(void)
// {
//   int count = 0;
// 	unsigned char busy;
// 	do
// 	{
// 		busy = gpio_get_screen_busy();
// 		busy =!(busy & 0x01);
// 		vTaskDelay(10 / portTICK_PERIOD_MS);
// 		count ++;
// 		if(count >= 1000){
// 			printf("---------------time out ---\n");
// 			break;
// 		}
// 	}
// 	while(busy);
// }

void Epaper_READBUSY(void)
{
  uint8_t busy;
  while (1)
  { //=1 BUSY
    busy = gpio_get_screen_busy();
    if (busy == 0)
      break;
  }
}
///////////////////////////Part update//////////////////////////////////////////////
void EPD_SetRAMValue_BaseMap(const unsigned char *datas)
{
  unsigned int i;
  const unsigned char *datas_flag;
  datas_flag = datas;
  spi_send_cmd(0x24); // Write Black and White image to RAM
  for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
  {
    spi_send_data(datas[i]);
  }
  datas = datas_flag;
  spi_send_cmd(0x26); // Write Black and White image to RAM
  for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
  {
    spi_send_data(datas[i]);
  }
  EPD_Update();
}

void EPD_Dis_Part(unsigned int x_start, unsigned int y_start, const unsigned char *datas, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  unsigned int i;
  unsigned int x_end, y_start1, y_start2, y_end1, y_end2;
  x_start = x_start / 8; //
  x_end = x_start + PART_LINE / 8 - 1;

  y_start1 = 0;
  y_start2 = y_start;
  if (y_start >= 256)
  {
    y_start1 = y_start2 / 256;
    y_start2 = y_start2 % 256;
  }
  y_end1 = 0;
  y_end2 = y_start + PART_COLUMN - 1;
  if (y_end2 >= 256)
  {
    y_end1 = y_end2 / 256;
    y_end2 = y_end2 % 256;
  }

  // Add hardware reset to prevent background color change
  gpio_set_screen_res(0);              // Module reset
  vTaskDelay(10 / portTICK_PERIOD_MS); // At least 10ms delay
  gpio_set_screen_res(1);
  vTaskDelay(10 / portTICK_PERIOD_MS); // At least 10ms delay
                                       // Lock the border to prevent flashing
  spi_send_cmd(0x3C); // BorderWavefrom,
  spi_send_data(0x80);

  spi_send_cmd(0x44);  // set RAM x address start/end, in page 35
  spi_send_data(x_start);  // RAM x address start at 00h;
  spi_send_data(x_end);    // RAM x address end at 0fh(15+1)*8->128
  spi_send_cmd(0x45);  // set RAM y address start/end, in page 35
  spi_send_data(y_start2); // RAM y address start at 0127h;
  spi_send_data(y_start1); // RAM y address start at 0127h;
  spi_send_data(y_end2);   // RAM y address end at 00h;
  spi_send_data(y_end1);   // ????=0

  spi_send_cmd(0x4E); // set RAM x address count to 0;
  spi_send_data(x_start);
  spi_send_cmd(0x4F); // set RAM y address count to 0X127;
  spi_send_data(y_start2);
  spi_send_data(y_start1);

  spi_send_cmd(0x24); // Write Black and White image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    spi_send_data(datas[i]);
  }
  EPD_Part_Update();
}

/////////////////////////////////Single display////////////////////////////////////////////////

void EPD_WhiteScreen_Black(void)

{
  unsigned int i, k;
  spi_send_cmd(0x24); // write RAM for black(0)/white (1)
  for (k = 0; k < 250; k++)
  {
    for (i = 0; i < 25; i++)
    {
      spi_send_data(0x00);
    }
  }
  EPD_Update();
}

void EPD_WhiteScreen_White(void)

{
  unsigned int i, k;
  spi_send_cmd(0x24); // write RAM for black(0)/white (1)
  for (k = 0; k < 250; k++)
  {
    for (i = 0; i < 25; i++)
    {
      spi_send_data(0xff);
    }
  }
  EPD_Update();
}

//////////////////////////////////END//////////////////////////////////////////////////
