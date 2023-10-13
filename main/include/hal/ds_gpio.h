#ifndef _DS_GPIO_H_
#define _DS_GPIO_H_



void gpio_tp_init(void);

void gpio_screen_init(void);

void gpio_set_screen_cs(uint32_t level);

void gpio_set_screen_dc(uint32_t level);

void gpio_set_screen_res(uint32_t level);

uint32_t gpio_get_screen_busy(void);

void gpio_set_tp_rst(uint32_t level);

#endif