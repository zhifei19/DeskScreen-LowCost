/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"

#include "ds_timer.h"
#include "ds_spiffs.h"
#include "ds_nvs.h"
#include "ds_system_data.h"
#include "ds_gpio.h"
#include "ds_ft6336.h"
#include "ds_screen.h"
#include "ds_wifi_ap.h"
#include "ds_http_server.h"
#include "ds_wifi_sta.h"
#include "ds_wifi_scan.h"
#include "ds_http_request.h"

static const char *TAG = "MAIN APP";

#define tsk1_PRIORITY    10
#define STACK_SIZE          2048

void vTaskCode(void *pvParameters)
{
    for(;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // printf("task 1 run... \n");
    }
}

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    ESP_LOGI(TAG, "Desktop Screen V1.1");

    xTaskCreate(vTaskCode, "NAME", STACK_SIZE, NULL, tsk1_PRIORITY, NULL);

    // ds_timer_init();

    const char* base_path = "/spiffs";
    spiffs_init(base_path);
    // spiffs_test();
    // spiffs_deinit();

    char *ssid = "123";
    char *password = "456";
    sysdata_set_wifi_info(ssid, strlen(ssid), password, strlen(password));
    printf("main ssid is: %s, password is: %s\n",ssid,password);
    printf("main ssid len is:%d\n,password len is:%d\n",(int)strlen(ssid),(int)strlen(password));
    nvs_init();
    nvs_save_data();
    nvs_read_data();

    // wifi_scan_start();

    // wifi_init_softap();
    // file_server_init(base_path);

    wifi_sta_init();

    // gpio_screen_init();
    // gpio_tp_init();

    ft6336_init();
    EPD_HW_Init(); 

    EPD_selftest();

    http_client_init();

    while(1)
    {
        ft6336_task();

        printf("System run...\n");

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
