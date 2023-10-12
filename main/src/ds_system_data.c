#include "ds_system_data.h"
#include <stdio.h>
#include <string.h>

#include "esp_log.h"

system_data_t   sysdata_handler;

static const char *TAG = "system_data";

void sysdata_init()
{
    memset(&sysdata_handler,0,sizeof(system_data_t));
    ESP_LOGI(TAG,"System data initialized");
}

system_data_t sysdata_get()
{
    return sysdata_handler;
}

void sysdata_set_wifi_info(char *p_ssid, uint32_t p_ssidlen, char *p_password, uint32_t p_passwordlen)
{
    sysdata_handler.ssid_len = p_ssidlen;
    sysdata_handler.password_len = p_passwordlen;
    memcpy(sysdata_handler.ssid,p_ssid,p_ssidlen);
    memcpy(sysdata_handler.password,p_password,p_passwordlen);
    sysdata_handler.ssid[p_ssidlen] = '\0';
    sysdata_handler.password[p_passwordlen] = '\0';
    printf("sysdata_set_wifi_info ssid len is:%d\n,password len is:%d\n",(int)p_ssidlen,(int)p_passwordlen);
    printf("sysdata_set_wifi_info sysdata ssid len is:%d\n,sysdata password len is:%d\n",(int)sysdata_handler.ssid_len,(int)sysdata_handler.password_len);
    printf("sysdata_set_wifi_info strlen ssid:%d\n",strlen(sysdata_handler.ssid));
    printf("sysdata_set_wifi_info strlen password:%d\n",strlen(sysdata_handler.password));
    printf("sysdata_set_wifi_info ssid:%s\n",sysdata_handler.ssid);
    printf("sysdata_set_wifi_info password:%s\n",sysdata_handler.password);
    ESP_LOGI(TAG,"SYSTEM DATA WRITTEN DONE");
}

void sysdata_print_wifi_info()
{
    ESP_LOGI(TAG,"WIFI SSID:");
    printf("\r\nwifi_SSID:");
    printf("%s\n",sysdata_handler.ssid);
    ESP_LOGI(TAG,"WIFI Password:");
    printf("\r\nwifi_Password:");
    printf("%s\n",sysdata_handler.password);
    printf("ssid len is:%d\n,password len is:%d\n",(int)sysdata_handler.ssid_len,(int)sysdata_handler.password_len);
    printf("\r\n");
}

SETTING_DATA_E sysdata_check_wifi_info()
{
    if(sysdata_handler.ssid_len != 0)
        return SETTING_DATA_HAS_WIFI_INFO;
    
    return SETTING_DATA_INIT;
}