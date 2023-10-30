#include "ds_i2c.h"

static const char *TAG = "ds_i2c";

#define I2C_MASTER_SCL_IO           32      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           33      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define FT6336_TP_ADDR                 0x38        /*!< Slave address of the MPU9250 sensor */
// #define MPU9250_SENSOR_ADDR                 0x68        /*!< Slave address of the MPU9250 sensor */
// #define MPU9250_WHO_AM_I_REG_ADDR           0x75        /*!< Register addresses of the "who am I" register */

// #define MPU9250_PWR_MGMT_1_REG_ADDR         0x6B        /*!< Register addresses of the power managment register */
// #define MPU9250_RESET_BIT                   7


static esp_err_t i2c_master_set_addr(uint8_t data_addr)
{
    esp_err_t err = ESP_OK;

    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    assert (handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, (FT6336_TP_ADDR << 1) | I2C_MASTER_WRITE, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, data_addr, true);
    if (err != ESP_OK) {
        goto end;
    }

    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, handle, 1000/portTICK_PERIOD_MS);

end:
    i2c_cmd_link_delete_static(handle);
    return err;
}

esp_err_t i2c_master_write_data(uint8_t reg_addr, uint8_t *write_buffer, uint8_t write_size)
{
    esp_err_t err = ESP_OK;

    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    assert (handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, (FT6336_TP_ADDR << 1) | I2C_MASTER_WRITE, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, reg_addr, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write(handle, write_buffer, write_size, true);
    if (err != ESP_OK) {
        goto end;
    }

    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, handle, 1000/portTICK_PERIOD_MS);

end:
    i2c_cmd_link_delete_static(handle);
    return err;
}

esp_err_t i2c_master_read_data(uint8_t reg_addr, uint8_t *read_buffer,uint8_t read_size)
{
    esp_err_t err = ESP_OK;

    i2c_master_set_addr(reg_addr);

    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    assert (handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, (FT6336_TP_ADDR << 1) | I2C_MASTER_READ, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_read(handle, read_buffer, read_size, I2C_MASTER_LAST_NACK);
    if (err != ESP_OK) {
        goto end;
    }

    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, handle, 1000/portTICK_PERIOD_MS);

end:
    i2c_cmd_link_delete_static(handle);
    return err;
}

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    ESP_LOGI(TAG, "I2C initialized");
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


// void test_main(void)
// {
//     uint8_t data[2];
//     ESP_ERROR_CHECK(i2c_master_init());

//     /* Read the MPU9250 WHO_AM_I register, on power up the register should have the value 0x71 */
//     ESP_ERROR_CHECK(mpu9250_register_read(MPU9250_WHO_AM_I_REG_ADDR, data, 1));
//     ESP_LOGI(TAG, "WHO_AM_I = %X", data[0]);

//     /* Demonstrate writing by reseting the MPU9250 */
//     ESP_ERROR_CHECK(mpu9250_register_write_byte(MPU9250_PWR_MGMT_1_REG_ADDR, 1 << MPU9250_RESET_BIT));

//     ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
//     ESP_LOGI(TAG, "I2C de-initialized successfully");
// }
