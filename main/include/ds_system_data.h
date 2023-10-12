#ifndef _DS_SYSTEM_DATA_H_
#define _DS_SYSTEM_DATA_H_

#define MAX_SSID_LEN        32
#define MAX_PASSWORD_LEN    32

#include <stdint.h>

typedef struct {
    char ssid[MAX_SSID_LEN];
    uint32_t ssid_len;
    char password[MAX_PASSWORD_LEN];
    uint32_t password_len;
}system_data_t;

typedef enum{
    SETTING_DATA_INIT = 0,
    SETTING_DATA_HAS_WIFI_INFO,
}SETTING_DATA_E;

void sysdata_init();

system_data_t sysdata_get();

void sysdata_set_wifi_info(char *p_ssid, uint32_t p_ssidlen, char *p_password, uint32_t p_passwordlen);

void sysdata_print_wifi_info();

SETTING_DATA_E sysdata_check_wifi_info();

#endif