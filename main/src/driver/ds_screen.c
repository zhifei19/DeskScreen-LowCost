#include "ds_screen.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "ds_gpio.h"
#include "ds_data_image.h"

static const char *TAG = "ds_screen";

unsigned char oldData[2888];
unsigned char partFlag = 1;

////////////////////////////////////E-paper demo//////////////////////////////////////////////////////////
// Busy function
void lcd_chkstatus(void)
{
  uint8_t busy = 1;
  while (busy)
  { //=1 BUSY
    busy = gpio_get_screen_busy();
  }
}

// UC8151D
void EPD_Init(void)
{
  unsigned char i;
  for (i = 0; i < 3; i++)
  {
    gpio_set_screen_res(0);              // Module reset
    vTaskDelay(10 / portTICK_PERIOD_MS); // At least 10ms delay
    gpio_set_screen_res(1);
    vTaskDelay(10 / portTICK_PERIOD_MS); // At least 10ms delay
  }
  lcd_chkstatus();

  spi_send_cmd(0x00);  // panel setting
  spi_send_data(0x1f); // LUT from OTP??KW-BF   KWR-AF  BWROTP 0f BWOTP 1f
  spi_send_data(0x0D);

  spi_send_cmd(0x61); // resolution setting
  spi_send_data(EPD_WIDTH);
  spi_send_data(EPD_HEIGHT / 256);
  spi_send_data(EPD_HEIGHT % 256);

  spi_send_cmd(0x04);
  lcd_chkstatus(); // waiting for the electronic paper IC to release the idle signal

  spi_send_cmd(0X50);  // VCOM AND DATA INTERVAL SETTING
  spi_send_data(0x97); // WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}

void lut1(void)
{
  unsigned int count;
  spi_send_cmd(0x20);
  for (count = 0; count < 44; count++)
  {
    spi_send_data(lut_vcom1[count]);
  }

  spi_send_cmd(0x21);
  for (count = 0; count < 42; count++)
  {
    spi_send_data(lut_ww1[count]);
  }

  spi_send_cmd(0x22);
  for (count = 0; count < 42; count++)
  {
    spi_send_data(lut_bw1[count]);
  }

  spi_send_cmd(0x23);
  for (count = 0; count < 42; count++)
  {
    spi_send_data(lut_wb1[count]);
  }

  spi_send_cmd(0x24);
  for (count = 0; count < 42; count++)
  {
    spi_send_data(lut_bb1[count]);
  }
}
void EPD_Init_Part(void)
{
  unsigned char i;
  for (i = 0; i < 3; i++)
  {
    gpio_set_screen_res(0);              // Module reset
    vTaskDelay(10 / portTICK_PERIOD_MS); // At least 10ms delay
    gpio_set_screen_res(1);
    vTaskDelay(10 / portTICK_PERIOD_MS); // At least 10ms delay
  }
  lcd_chkstatus();

  spi_send_cmd(0x01); // POWER SETTING
  spi_send_data(0x03);
  spi_send_data(0x00);
  spi_send_data(0x2b);
  spi_send_data(0x2b);
  spi_send_data(0x03);

  spi_send_cmd(0x06);  // boost soft start
  spi_send_data(0x17); // A
  spi_send_data(0x17); // B
  spi_send_data(0x17); // C

  spi_send_cmd(0x00);  // panel setting
  spi_send_data(0xbf); // LUT from OTP??128x296
  spi_send_data(0x0D);

  spi_send_cmd(0x30);
  spi_send_data(0x3C); // 3A 100HZ   29 150Hz 39 200HZ  31 171HZ

  spi_send_cmd(0x61); // resolution setting
  spi_send_data(EPD_WIDTH);
  spi_send_data(EPD_HEIGHT / 256);
  spi_send_data(EPD_HEIGHT % 256);

  spi_send_cmd(0x82); // vcom_DC setting
  spi_send_data(0x12);
  lut1();

  spi_send_cmd(0x04);
  lcd_chkstatus();
}

void EPD_DeepSleep(void)
{
  spi_send_cmd(0X50);  // VCOM AND DATA INTERVAL SETTING
  spi_send_data(0xf7); // WBmode:VBDF 17|D7 VBDW 97 VBDB 57    WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

  spi_send_cmd(0X02);                   // power off
  lcd_chkstatus();                      // waiting for the electronic paper IC to release the idle signal
  vTaskDelay(100 / portTICK_PERIOD_MS); //!!!The delay here is necessary,100mS at least!!!
  spi_send_cmd(0X07);                   // deep sleep
  spi_send_data(0xA5);
}
// Full screen refresh update function
void EPD_Update(void)
{
  // Refresh
  spi_send_cmd(0x12);                 // DISPLAY REFRESH
  vTaskDelay(1 / portTICK_PERIOD_MS); //!!!The delay here is necessary, 200uS at least!!!
  lcd_chkstatus();                    // waiting for the electronic paper IC to release the idle signal
}

void EPD_WhiteScreen_ALL(const unsigned char *datas)
{
  unsigned int i;
  // Write Data
  spi_send_cmd(0x10); // Transfer old data
  for (i = 0; i < EPD_ARRAY; i++)
  {
    spi_send_data(0xFF); // Transfer the actual displayed data
  }
  spi_send_cmd(0x13); // Transfer new data
  for (i = 0; i < EPD_ARRAY; i++)
  {
    spi_send_data(datas[i]); // Transfer the actual displayed data
  }
  EPD_Update();
}
void EPD_WhiteScreen_White(void)
{
  unsigned int i;
  // Write Data
  spi_send_cmd(0x10); // Transfer old data
  for (i = 0; i < EPD_ARRAY; i++)
  {
    spi_send_data(0xFF);
  }
  spi_send_cmd(0x13); // Transfer new data
  for (i = 0; i < EPD_ARRAY; i++)
  {
    spi_send_data(0xFF); // Transfer the actual displayed data
    oldData[i] = 0xFF;
  }
  EPD_Update();
}

// Partial refresh of background display, this function is necessary, please do not delete it!!!
void EPD_SetRAMValue_BaseMap(const unsigned char *datas)
{
  unsigned int i;
  spi_send_cmd(0x10); // write old data
  for (i = 0; i < EPD_ARRAY; i++)
  {
    spi_send_data(0xFF);
  }
  spi_send_cmd(0x13); // write new data
  for (i = 0; i < EPD_ARRAY; i++)
  {
    spi_send_data(datas[i]);
    oldData[i] = datas[i];
  }
  EPD_Update();
}

void EPD_Dis_Part(unsigned int x_start, unsigned int y_start, const unsigned char *datas, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  unsigned int i, x_end, y_end;
  x_start = x_start - x_start % 8;
  x_end = x_start + PART_LINE - 1;
  y_end = y_start + PART_COLUMN - 1;

  EPD_Init_Part();
  spi_send_cmd(0x91);       // This command makes the display enter partial mode
  spi_send_cmd(0x90);       // resolution setting
  spi_send_data(x_start);   // x-start
  spi_send_data(x_end - 1); // x-end
  spi_send_data(y_start / 256);
  spi_send_data(y_start % 256); // y-start
  spi_send_data(y_end / 256);
  spi_send_data(y_end % 256 - 1); // y-end
  spi_send_data(0x28);

  spi_send_cmd(0x10); // writes Old data to SRAM
  if (partFlag == 1)
  {
    partFlag = 0;
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
      spi_send_data(0xFF);
  }
  else
  {
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
      spi_send_data(oldData[i]);
  }
  spi_send_cmd(0x13); // writes New data to SRAM.
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    spi_send_data(datas[i]);
    oldData[i] = datas[i];
  }
  EPD_Update();
}

// Full screen partial refresh display
void EPD_Dis_PartAll(const unsigned char *datas)
{
  unsigned int i;
  EPD_Init_Part();
  // Write Data
  spi_send_cmd(0x10); // Transfer old data
  for (i = 0; i < EPD_ARRAY; i++)
  {
    spi_send_data(oldData[i]); // Transfer the actual displayed data
  }
  spi_send_cmd(0x13); // Transfer new data
  for (i = 0; i < EPD_ARRAY; i++)
  {
    spi_send_data(datas[i]); // Transfer the actual displayed data
    oldData[i] = datas[i];
  }

  EPD_Update();
}

////////////////////////////////Other newly added functions////////////////////////////////////////////
// Display rotation 180 degrees initialization
void EPD_Init_180(void)
{
  unsigned char i;
  for (i = 0; i < 3; i++)
  {
    gpio_set_screen_res(0);              // Module reset
    vTaskDelay(10 / portTICK_PERIOD_MS); // At least 10ms delay
    gpio_set_screen_res(1);
    vTaskDelay(10 / portTICK_PERIOD_MS); // At least 10ms delay
  }
  lcd_chkstatus();

  spi_send_cmd(0x00);  // panel setting
  spi_send_data(0x13); // LUT from OTP??KW-BF   KWR-AF  BWROTP 0f BWOTP 1f
  spi_send_data(0x0D);

  spi_send_cmd(0x61); // resolution setting
  spi_send_data(EPD_WIDTH);
  spi_send_data(EPD_HEIGHT / 256);
  spi_send_data(EPD_HEIGHT % 256);

  spi_send_cmd(0x04);
  lcd_chkstatus(); // waiting for the electronic paper IC to release the idle signal

  spi_send_cmd(0X50);  // VCOM AND DATA INTERVAL SETTING
  spi_send_data(0x97); // WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}

void EPD_interface_init(void)
{
  gpio_screen_init();
  spi_init();
}

void EPD_selftest(void)
{
  EPD_Init();                            // Full screen refresh initialization.
  EPD_WhiteScreen_White();               // Clear screen function.
  EPD_DeepSleep();                       // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2s.
  /************Full display(2s)*******************/
  EPD_Init();                            // Full screen refresh initialization.
  EPD_WhiteScreen_ALL(gImage_1);         // To Display one image using full screen refresh.
  EPD_DeepSleep();                       // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2s.
}
/***********************************************************
            end file
***********************************************************/
