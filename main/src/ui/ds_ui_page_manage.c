#include "ds_ui_page_manage.h"
#include "ds_conf.h"
#include <stdint.h>
#include "freertos/task.h"
#include "freertos/queue.h"
#include "ds_system_data.h"

typedef struct 
{
    /* data */
    uint8_t touch_x;
    uint8_t touch_y;
    TP_ACTION_E action;
    PAGE_TYPE_E current_state;
}UI_EVENT_T;


static QueueHandle_t ui_page_manager_queue = NULL;

PAGE_TYPE_E page_status;

static void ui_page_task(void* arg)
{
    UI_EVENT_T evt;
    for(;;) {
        xQueueReceive(ui_page_manager_queue, &evt, portMAX_DELAY);
        switch (page_status)
        {
        case PAGE_TYPE_INIT:
            ds_ui_init_menu();
            ds_ui_init_time();
            ds_ui_init_weather();
            ds_ui_init_word();
            ds_ui_init_tomato();
            ds_ui_init_setting();
            page_status = PAGE_TYPE_MENU;
            break;
        case PAGE_TYPE_MENU:
            ds_ui_show_menu();
            if (evt.action == TP_ACTION_SHORT)
            {
                if (evt.touch_x < SCREEN_HORIZONTAL/2 && evt.touch_y < SCREEN_VERTICAL/2)
                {
                    page_status = PAGE_TYPE_TIME;
                }
                else if (evt.touch_x >= SCREEN_HORIZONTAL/2 && evt.touch_y < SCREEN_VERTICAL/2)
                {
                    page_status = PAGE_TYPE_WEATHER;
                }
                else if (evt.touch_x < SCREEN_HORIZONTAL/2 && evt.touch_y >= SCREEN_VERTICAL/2)
                {
                    page_status = PAGE_TYPE_WORD;
                }
                else if (evt.touch_x >= SCREEN_HORIZONTAL/2 && evt.touch_y >= SCREEN_VERTICAL/2)
                {
                    page_status = PAGE_TYPE_TOMATO;
                }
                
            }
            else if (evt.action == TP_ACTION_MOVE_DOWN)
            {
                page_status = PAGE_TYPE_SETTING;
            }
            
            
            break;
        
        case PAGE_TYPE_TIME:
            /* code */
            ds_ui_show_time();
            break;

        case PAGE_TYPE_WEATHER:
            /* code */
            ds_ui_show_weather();
            break;

        case PAGE_TYPE_WORD:
            /* code */
            ds_ui_show_word();
            break;

        case PAGE_TYPE_TOMATO:
            /* code */
            ds_ui_show_tomato();
            break;

        case PAGE_TYPE_SETTING:
            /* code */
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