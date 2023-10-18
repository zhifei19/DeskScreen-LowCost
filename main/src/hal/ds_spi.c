#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ds_spi.h"
#include "esp_log.h"
#include "ds_gpio.h"

const static char *TAG = "ds_spi";

spi_device_handle_t spi;

/*
 This code displays some fancy graphics on the 320x240 LCD on an ESP-WROVER_KIT board.
 This example demonstrates the use of both spi_device_transmit as well as
 spi_device_queue_trans/spi_device_get_trans_result and pre-transmit callbacks.

 Some info about the ILI9341/ST7789V: It has an C/D line, which is connected to a GPIO here. It expects this
 line to be low for a command and high for data. We use a pre-transmit callback here to control that
 line: every transaction has as the user-definable argument the needed state of the D/C line and just
 before the transaction is sent, the callback will set this line to the correct state.
*/

#define PIN_NUM_MISO 34
#define PIN_NUM_MOSI 26
#define PIN_NUM_CLK 25
#define PIN_NUM_CS 27

#define PIN_NUM_DC 14
#define PIN_NUM_RST 12

// To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
// but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
#define PARALLEL_LINES 16

/*
 The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct
{
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; // No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

/* Send a command to the Display. Uses spi_device_polling_transmit, which waits
 * until the transfer is complete.
 *
 * Since command transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
void spi_send_cmd(const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    gpio_set_screen_cs(0);
    gpio_set_screen_dc(0);
    memset(&t, 0, sizeof(t)); // Zero out the transaction
    t.length = 8;             // Command is 8 bits
    t.tx_buffer = &cmd;       // The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret = spi_device_polling_transmit(spi, &t); // Transmit!
    assert(ret == ESP_OK);                      // Should have had no issues.
    gpio_set_screen_cs(1);
}

/* Send data to the Display. Uses spi_device_polling_transmit, which waits until the
 * transfer is complete.
 *
 * Since data transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
void spi_send_data(const uint8_t data)
{
    esp_err_t ret;
    spi_transaction_t t;
    gpio_set_screen_cs(0);
    gpio_set_screen_dc(1);
    memset(&t, 0, sizeof(t));                   // Zero out the transaction
    t.length = 8;                         // Len is in bytes, transaction length is in bits.
    t.tx_buffer = &data;                         // Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret = spi_device_polling_transmit(spi, &t); // Transmit!
    assert(ret == ESP_OK);                      // Should have had no issues.
    gpio_set_screen_cs(1);
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_screen_dc(dc);
}

// // Initialize the display
// void display_init(spi_device_handle_t spi)
// {
//     int cmd = 0;
//     const lcd_init_cmd_t *lcd_init_cmds;

//     // Initialize non-SPI GPIOs
//     gpio_config_t io_conf = {};
//     io_conf.pin_bit_mask = ((1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST) | (1ULL << PIN_NUM_BCKL));
//     io_conf.mode = GPIO_MODE_OUTPUT;
//     io_conf.pull_up_en = true;
//     gpio_config(&io_conf);

//     // Reset the display
//     gpio_set_level(PIN_NUM_RST, 0);
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//     gpio_set_level(PIN_NUM_RST, 1);
//     vTaskDelay(100 / portTICK_PERIOD_MS);

//     // detect LCD type
//     uint32_t lcd_id = lcd_get_id(spi);
//     int lcd_detected_type = 0;
//     int lcd_type;

//     printf("LCD ID: %08" PRIx32 "\n", lcd_id);
//     if (lcd_id == 0)
//     {
//         // zero, ili
//         lcd_detected_type = LCD_TYPE_ILI;
//         printf("ILI9341 detected.\n");
//     }
//     else
//     {
//         // none-zero, ST
//         lcd_detected_type = LCD_TYPE_ST;
//         printf("ST7789V detected.\n");
//     }

// #ifdef CONFIG_LCD_TYPE_AUTO
//     lcd_type = lcd_detected_type;
// #elif defined(CONFIG_LCD_TYPE_ST7789V)
//     printf("kconfig: force CONFIG_LCD_TYPE_ST7789V.\n");
//     lcd_type = LCD_TYPE_ST;
// #elif defined(CONFIG_LCD_TYPE_ILI9341)
//     printf("kconfig: force CONFIG_LCD_TYPE_ILI9341.\n");
//     lcd_type = LCD_TYPE_ILI;
// #endif
//     if (lcd_type == LCD_TYPE_ST)
//     {
//         printf("LCD ST7789V initialization.\n");
//         lcd_init_cmds = st_init_cmds;
//     }
//     else
//     {
//         printf("LCD ILI9341 initialization.\n");
//         lcd_init_cmds = ili_init_cmds;
//     }

//     // Send all the commands
//     while (lcd_init_cmds[cmd].databytes != 0xff)
//     {
//         lcd_cmd(spi, lcd_init_cmds[cmd].cmd, false);
//         lcd_data(spi, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes & 0x1F);
//         if (lcd_init_cmds[cmd].databytes & 0x80)
//         {
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//         }
//         cmd++;
//     }

//     /// Enable backlight
//     gpio_set_level(PIN_NUM_BCKL, 0);
// }

void spi_init(void)
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64 * 8};
    spi_device_interface_config_t devcfg = {

        .clock_speed_hz = 15 * 1000 * 1000,      // Clock out at 10 MHz
        .mode = 0,                               // SPI mode 0
        .spics_io_num = PIN_NUM_CS,              // CS pin
        .queue_size = 7,                         // We want to be able to queue 7 transactions at a time
        //.pre_cb = lcd_spi_pre_transfer_callback, // Specify pre-transfer callback to handle D/C line
    };
    // Initialize the SPI bus
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_DISABLED);
    ESP_ERROR_CHECK(ret);
    // Attach the LCD to the SPI bus
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
}
