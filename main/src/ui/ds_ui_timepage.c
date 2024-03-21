#include "ds_ui_timepage.h"
#include "freertos/task.h"

#include "ds_data_image.h"
#include "esp_log.h"
#include "ds_paint.h"
#include "ds_system_data.h"

static const char *TAG = "ds_timepage";

void ds_ui_init_time(void)
{
}

void ds_ui_show_time(void)
{
    int margin = 40;
    ESP_LOGI(TAG, "ds_ui_show_time start");
    // EPD_HW_Init();
    // EPD_SetRAMValue_BaseMap(gImage_basemap);  //Partial refresh background color,Brush map is a must, please do not delete
    // EPD_WhiteScreen_White();
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    // EPD_Dis_Part(0, 200, gImage_back, 40, 40);
    // EPD_Dis_Part(96, 96, gImage_colon, 40, 8);
    EPD_HW_Init();
    ds_screen_partial_data_add(0, 40, 0, 40, gImage_back);
    ds_screen_partial_data_add(96, 104, 96, 136, gImage_colon);
    SYSTEM_DATA_T sysdata =sysdata_get();
    ESP_LOGI(TAG, "sysdata_get().hour is %s\n",sysdata.hour);
    // Paint_DrawString_CN(50, 110, sysdata_get().hour, WHITE,BLACK);
    ds_screen_partial_data_copy();
    // Paint_DrawString_CN(50, 80, "hello", WHITE,BLACK);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    EPD_DeepSleep(); // Sleep
    ESP_LOGI(TAG, "ds_ui_show_time end");
}
