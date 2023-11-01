#ifndef _DS_SPI_H_
#define _DS_SPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

void spi_init(void);

void spi_send_cmd(const uint8_t cmd);

void spi_send_data(const uint8_t data);

void spi_selftest(void);

#endif