#ifndef _DS_SPIFFS_H_
#define _DS_SPIFFS_H

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

void spiffs_init(const char* base_path);

void spiffs_deinit(void);

void spiffs_test(void);

#endif