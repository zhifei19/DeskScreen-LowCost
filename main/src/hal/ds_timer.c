/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "ds_conf.h"
#include "ds_ft6336.h"
#include "ds_system_data.h"

static const char *TAG = "ds_timer";

QueueHandle_t timer_queue;

typedef struct {
    uint64_t isr_event_count;
} timer_queue_element_t;

timer_queue_element_t g_timer_event;

void vTimerTask(void *pvParameters)
{
    timer_queue_element_t ele;
    TP_POSITION_T position = {0};
    // int minus_status_count = 0, plus_status_count = 0;
    // UBaseType_t istack;
    for(;;)
    {
        xQueueReceive(timer_queue, &ele, portMAX_DELAY);
        //Just for temporary
        ele.isr_event_count = 0;
        count_tp_action_manage_time();
        if(get_tp_action_status()<=2 && get_tp_action_status()>0)
        {
            // printf("minus_status_count: %d \n",minus_status_count++);
            // ft6336_get_TouchPoint(&position);
            set_tp_action_manage_start_point(position.x, position.y);
            
        }
        else if(get_tp_action_status()>2)
        {
            // printf("plus_status_count: %d \n",plus_status_count++);
            // istack = uxTaskGetStackHighWaterMark(NULL);
            // printf("vTimerTask istack = %d\n", istack);
            ft6336_get_TouchPoint(&position);
            set_tp_action_manage_stop_point(position.x, position.y);

        }
    }
    
}

static bool IRAM_ATTR timer_10ms_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{

    g_timer_event.isr_event_count = 0;

    xQueueSendFromISR(timer_queue, &g_timer_event, NULL);
    // return whether we need to yield at the end of ISR
    return true;
}


void ds_timer_init(void)
{
    timer_queue = xQueueCreate(10, sizeof(timer_queue_element_t));
    if (!timer_queue) {
        ESP_LOGE(TAG, "Creating timer_queue failed");
        return;
    }

    ESP_LOGI(TAG, "Create timer handle");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_10ms_alarm,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    ESP_LOGI(TAG, "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    ESP_LOGI(TAG, "Start timer, stop it at alarm event");
    gptimer_alarm_config_t alarm_config1 = {
        .reload_count = 0,
        .alarm_count = 10000, // period = 10ms
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    
    xTaskCreate(vTimerTask, "v_Timer_Task", TASK_TIMER_STACKSIZE, NULL, TASK_TIMER_PRIORITY, NULL);

}
