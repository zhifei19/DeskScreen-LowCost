#ifndef _DS_I2C_H_
#define _DS_I2C_H_

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

esp_err_t i2c_master_init(void);

esp_err_t i2c_master_write_data(uint8_t reg_addr, uint8_t *write_buffer, uint8_t write_size);

esp_err_t i2c_master_read_data(uint8_t reg_addr, uint8_t *read_buffer,uint8_t read_size);

#endif