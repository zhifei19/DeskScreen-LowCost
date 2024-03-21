#include "ds_ui_page_manage.h"
#include "ds_conf.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "ds_system_data.h"
#include "ds_http_request.h"
#include "esp_log.h"

#include "ds_ui_menupage.h"
#include "ds_ui_settingpage.h"
#include "ds_ui_timepage.h"
#include "ds_ui_tomatopage.h"
#include "ds_ui_weatherpage.h"
#include "ds_ui_wordpage.h"

#pragma pack(1)
typedef struct 
{
    /* data */
    uint8_t touch_x;
    uint8_t touch_y;
    TP_ACTION_E action;
    PAGE_TYPE_E current_state;
}UI_EVENT_T;


static QueueHandle_t ui_page_manager_queue = NULL;

static const char *TAG = "ds_page_manage";

PAGE_TYPE_E page_status = PAGE_TYPE_MENU;

void ds_ui_page_manage_send_event(TP_ACTION_E act, uint8_t t_x, uint8_t t_y)
{
    UI_EVENT_T evt;
    printf("ds_ui_page_manage_send_event act is %d\n",act);
    evt.action = act;
    evt.touch_x = t_x;
    evt.touch_y = t_y;
    printf("ds_ui_page_manage_send_event evt.action is %d\n",evt.action);
    xQueueSendFromISR(ui_page_manager_queue, &evt, NULL);

}

static void ui_page_task(void* arg)
{
    UI_EVENT_T evt;
    ESP_LOGI(TAG,"ui_page_task start");
    for(;;) {
        ESP_LOGI(TAG,"xQueueReceive start");
        xQueueReceive(ui_page_manager_queue, &evt, portMAX_DELAY);
        printf("ui_page_manager_queue \n\
                        evt.touch_x is %d\n\
                        evt.touch_y is %d\n\
                        evt.action is %d\n",
                        evt.touch_x, evt.touch_y, evt.action);
        ESP_LOGI(TAG,"xQueueReceive end");

        // page switching logic
        switch (page_status)
        {
        case PAGE_TYPE_INIT:
            ESP_LOGI(TAG,"PAGE_TYPE_INIT start");
            page_status = PAGE_TYPE_MENU;
            break;
        case PAGE_TYPE_MENU:
            ESP_LOGI(TAG,"PAGE_TYPE_MENU start");
            if (evt.action == TP_ACTION_SHORT)
            {
                if (evt.touch_x < SCREEN_HORIZONTAL/2 && evt.touch_y < SCREEN_VERTICAL/2)
                {
                    ESP_LOGI(TAG,"PAGE_TYPE_TIME get");
                    page_status = PAGE_TYPE_TIME;
                }
                else if (evt.touch_x >= SCREEN_HORIZONTAL/2 && evt.touch_y < SCREEN_VERTICAL/2)
                {
                    ESP_LOGI(TAG,"PAGE_TYPE_WEATHER get");
                    page_status = PAGE_TYPE_WEATHER;
                }
                else if (evt.touch_x < SCREEN_HORIZONTAL/2 && evt.touch_y >= SCREEN_VERTICAL/2)
                {
                    ESP_LOGI(TAG,"PAGE_TYPE_WORD get");
                    page_status = PAGE_TYPE_WORD;
                }
                else if (evt.touch_x >= SCREEN_HORIZONTAL/2 && evt.touch_y >= SCREEN_VERTICAL/2)
                {
                    ESP_LOGI(TAG,"PAGE_TYPE_TOMATO get");
                    page_status = PAGE_TYPE_TOMATO;
                }
                
            }
            else if (evt.action == TP_ACTION_MOVE_DOWN)
            {
                page_status = PAGE_TYPE_SETTING;
            }
            break;
        
        case PAGE_TYPE_TIME:
            ESP_LOGI(TAG,"PAGE_TYPE_TIME start");
            break;

        case PAGE_TYPE_WEATHER:
            ESP_LOGI(TAG,"PAGE_TYPE_WEATHER start");
            break;

        case PAGE_TYPE_WORD:
            ESP_LOGI(TAG,"PAGE_TYPE_WORD start");
            break;

        case PAGE_TYPE_TOMATO:
            /* code */
            ESP_LOGI(TAG,"PAGE_TYPE_TOMATO start");
            break;

        case PAGE_TYPE_SETTING:
            ESP_LOGI(TAG,"PAGE_TYPE_SETTING start");
            break;

        default:
            break;
        }
        
        switch (page_status)
        {
        case PAGE_TYPE_INIT:
            ds_ui_show_menu();
            // ds_ui_init_menu();
            ds_ui_init_time();
            ds_ui_init_weather();
            ds_ui_init_word();
            ds_ui_init_tomato();
            ds_ui_init_setting();
            break;
        case PAGE_TYPE_MENU:
            ds_ui_show_menu();
            break;
        
        case PAGE_TYPE_TIME:
            ds_http_request_type(HTTP_GET_TIME);
            ds_ui_show_time();
            break;

        case PAGE_TYPE_WEATHER:
            ds_http_request_type(HTTP_GET_WEATHER);
            ds_ui_show_weather();
            break;

        case PAGE_TYPE_WORD:
            ds_ui_show_word();
            break;

        case PAGE_TYPE_TOMATO:
            ds_ui_show_tomato();
            break;

        case PAGE_TYPE_SETTING:
            ds_ui_show_setting();
            break;

        default:
            break;
        }
    }
}

void ds_ui_page_manage_init()
{
    ui_page_manager_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(ui_page_task, "ui_page_task", UI_PAGE_MANAGER_STACKSIZE, NULL, UI_PAGE_MANAGER_PRIORITY, NULL);
}