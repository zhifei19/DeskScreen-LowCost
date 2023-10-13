#include "ds_nvs.h"
#include <stdio.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "ds_system_data.h"

static const char *TAG = "ds_nvs";

NVS_WIFI_INFO_E wifi_config_flag = NVS_WIFI_INFO_NULL;

void nvs_init(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

NVS_WIFI_INFO_E nvs_read_data(void)
{
    esp_err_t err;
    nvs_handle_t my_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return NVS_WIFI_INFO_ERROR;
    }

    uint8_t wifi_config = 0;
    ESP_ERROR_CHECK(nvs_get_u8(my_handle, "WIFI_flag", &wifi_config));
    wifi_config_flag = wifi_config;
    if(wifi_config_flag == NVS_WIFI_INFO_SAVE)
    {
        ESP_LOGI(TAG,"Has WIFI config info");
        char local_ssid[32];
        char local_password[32];
        size_t ssid_len = sizeof(local_ssid);
        size_t password_len = sizeof(local_password);
        ESP_ERROR_CHECK(nvs_get_str(my_handle, "SSID", local_ssid,&ssid_len));
        ESP_ERROR_CHECK(nvs_get_str(my_handle, "Password", local_password,&password_len));
        ssid_len--;
        password_len--;
        sysdata_set_wifi_info(local_ssid, ssid_len, local_password, password_len);
        sysdata_print_wifi_info();
    }else{
        ESP_LOGI(TAG,"WIFI config info null");
    }
    nvs_close(my_handle);
    return wifi_config_flag;
}

void nvs_save_data(void)
{
    esp_err_t err;
    nvs_handle_t my_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        
        wifi_config_flag = NVS_WIFI_INFO_SAVE;
        ESP_ERROR_CHECK(nvs_set_u8(my_handle, "WIFI_flag", wifi_config_flag));
        ESP_ERROR_CHECK(nvs_set_str(my_handle, "SSID", sysdata_get().ssid));
        ESP_ERROR_CHECK(nvs_set_str(my_handle, "Password", sysdata_get().password));
        ESP_ERROR_CHECK(nvs_commit(my_handle));
        ESP_LOGI(TAG,"Data saved in NVS");
        nvs_close(my_handle);
    }
}
