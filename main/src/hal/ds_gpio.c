#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "ds_gpio.h"
#include "ds_ft6336.h"

static const char *TAG = "ds_gpio";

/**
 * Touch pad gpio
 * T_RST: output    IO5
 * T_INT: input     IO4     pull-up,interrupt from rising edge and falling edge
*/
// #define GPIO_OUTPUT_IO_0    CONFIG_GPIO_OUTPUT_0
// #define GPIO_OUTPUT_IO_1    CONFIG_GPIO_OUTPUT_1
#define TP_GPIO_OUTPUT_RST      5
#define TP_GPIO_OUTPUT_RST_SEL  (1ULL<<TP_GPIO_OUTPUT_RST)
#define TP_GPIO_INPUT_INT       4
#define TP_GPIO_INPUT_INT_SEL   (1ULL<<TP_GPIO_INPUT_INT)

/**
 * CS:      output       IO27
 * D/C:     output       IO14
 * RES:     output       IO12
 * BUSY:    Input        IO13
*/
// #define GPIO_INPUT_IO_1     CONFIG_GPIO_INPUT_1
// #define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define SCREEN_GPIO_OUTPUT_CS       27
#define SCREEN_GPIO_OUTPUT_CS_SEL  (1ULL<<SCREEN_GPIO_OUTPUT_CS)
#define SCREEN_GPIO_OUTPUT_DC       14
#define SCREEN_GPIO_OUTPUT_DC_SEL  (1ULL<<SCREEN_GPIO_OUTPUT_DC)
#define SCREEN_GPIO_OUTPUT_RES      12
#define SCREEN_GPIO_OUTPUT_RES_SEL  (1ULL<<SCREEN_GPIO_OUTPUT_RES)
#define SCREEN_GPIO_INPUT_BUSY      13
#define SCREEN_GPIO_INPUT_BUSY_SEL  (1ULL<<SCREEN_GPIO_INPUT_BUSY)

#define ESP_INTR_FLAG_DEFAULT 0

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR tp_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);

    TPR_Structure.TouchSta |= TP_COORD_UD;				//触摸坐标有更新
}

static void IRAM_ATTR screen_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void gpio_tp_init(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = TP_GPIO_OUTPUT_RST_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of both rising and falling edge
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = TP_GPIO_INPUT_INT_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio interrupt type for one pin
    // gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(TP_GPIO_INPUT_INT, tp_isr_handler, (void*) TP_GPIO_INPUT_INT);
    ESP_LOGI(TAG,"GPIO TP INIT");
}

void gpio_screen_init(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO27
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_CS_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //bit mask of the pins that you want to set,e.g.GPIO14
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_DC_SEL;
        //configure GPIO with the given settings
    gpio_config(&io_conf);

    //bit mask of the pins that you want to set,e.g.GPIO12
    io_conf.pin_bit_mask = SCREEN_GPIO_OUTPUT_RES_SEL;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
        //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of falling edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //bit mask of the pins, use GPIO13 here
    io_conf.pin_bit_mask = SCREEN_GPIO_INPUT_BUSY_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    //install gpio isr service
    // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    // gpio_isr_handler_add(SCREEN_GPIO_INPUT_BUSY, screen_isr_handler, (void*) SCREEN_GPIO_INPUT_BUSY);

    ESP_LOGI(TAG,"GPIO SCREEN INIT");

    // printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());

    // int cnt = 0;
    // while(1) {
    //     printf("cnt: %d\n", cnt++);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
    //     gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
    // }
}

void gpio_set_screen_cs(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_CS, level);
}

void gpio_set_screen_dc(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_DC, level);
}

void gpio_set_screen_res(uint32_t level)
{
    gpio_set_level(SCREEN_GPIO_OUTPUT_RES, level);
}

uint32_t gpio_get_screen_busy(void)
{
    return gpio_get_level(SCREEN_GPIO_INPUT_BUSY);
}

void gpio_set_tp_rst(uint32_t level)
{
    gpio_set_level(TP_GPIO_OUTPUT_RST, level);
}
