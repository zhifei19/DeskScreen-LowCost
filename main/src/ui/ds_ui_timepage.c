#include "ds_ui_timepage.h"
#include "freertos/task.h"

#include "ds_data_image.h"
#include "esp_log.h"
#include "ds_paint.h"


static const char *TAG = "ds_timepage";

void ds_ui_init_time(void)
{
}

void ds_ui_show_time(void)
{
    ESP_LOGI(TAG, "ds_ui_show_time start");
    EPD_HW_Init();
    EPD_SetRAMValue_BaseMap(gImage_basemap);  //Partial refresh background color,Brush map is a must, please do not delete
    vTaskDelay(100 / portTICK_PERIOD_MS);
    EPD_Dis_Part(0, 200, gImage_back, 40, 40);
    // Paint_DrawString_CN(50, 80, "hello", WHITE,BLACK);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    EPD_DeepSleep(); // Sleep
    ESP_LOGI(TAG, "ds_ui_show_time end");
}
