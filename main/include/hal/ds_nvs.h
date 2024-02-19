#ifndef _DS_NVS_H_
#define _DS_NVS_H_

#include <stdbool.h>

typedef enum{
    NVS_WIFI_INFO_ERROR = 0,
    NVS_WIFI_INFO_NULL,
    NVS_WIFI_INFO_SAVE,
}NVS_WIFI_INFO_E;

void nvs_selfcheck(void);

void nvs_init(void);

bool nvs_read_wifi_data(void);

void nvs_save_wifi_data(void);

void nvs_clean_wifi_info();

#endif