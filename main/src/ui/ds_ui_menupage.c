#include "ds_ui_menupage.h"
#include "freertos/task.h"
#include "ds_data_image.h"
#include "esp_log.h"

static const char *TAG = "ds_menupage";

void ds_ui_init_menu(void)
{
  ESP_LOGI(TAG, "ds_ui_init_menu start");
  EPD_HW_Init();                    // Electronic paper initialization
  EPD_WhiteScreen_ALL(gImage_test); // Refresh the picture in full screen
  vTaskDelay(500 / portTICK_PERIOD_MS);
  EPD_DeepSleep(); // Sleep
}

void ds_ui_show_menu(void)
{
  ESP_LOGI(TAG, "ds_ui_show_menu start");
  EPD_HW_Init();                        // Electronic paper initialization
  EPD_WhiteScreen_ALL(gImage_mainpage); // Refresh the picture in full screen
  vTaskDelay(500 / portTICK_PERIOD_MS);
  EPD_DeepSleep(); // Sleep
}
