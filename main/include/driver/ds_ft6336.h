#ifndef _DS_FT6336_H_
#define _DS_FT6336_H_

#include <stdint.h>

#define GPIO_TP_RST_LOW 0
#define GPIO_TP_RST_HIGH 1

#define I2C_ADDR_FT6336 0x38

// FT6236 部分寄存器定义
#define FT_DEVIDE_MODE 0x00    // FT6236模式控制寄存器
#define FT_GEST_ID 0x01        // 触摸事件类型
#define FT_REG_NUM_FINGER 0x02 // 触摸状态寄存器

#define FT_TP1_REG 0X03 // 第一个触摸点数据地址
#define FT_TP2_REG 0X09 // 第二个触摸点数据地址
#define FT_TP3_REG 0X0F // 第三个触摸点数据地址
#define FT_TP4_REG 0X15 // 第四个触摸点数据地址
#define FT_TP5_REG 0X1B // 第五个触摸点数据地址

#define FT_ID_G_LIB_VERSION 0xA1  // 版本
#define FT_ID_G_MODE 0xA4         // FT6236中断模式控制寄存器
#define FT_ID_G_THGROUP 0x80      // 触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE 0x88 // 激活状态周期设置寄存器
#define Chip_Vendor_ID 0xA3       // 芯片ID(0x36)
#define ID_G_FT6236ID 0xA8        // 0x11

#define FT6336_ADDR_DEVICE_MODE 0x00
#define FT6336_ADDR_TD_STATUS 0x02
#define FT6336_ADDR_TOUCH1_EVENT 0x03
#define FT6336_ADDR_TOUCH1_ID 0x05
#define FT6336_ADDR_TOUCH1_X 0x03
#define FT6336_ADDR_TOUCH1_Y 0x05

#define FT6336_ADDR_TOUCH2_EVENT 0x09
#define FT6336_ADDR_TOUCH2_ID 0x0B
#define FT6336_ADDR_TOUCH2_X 0x09
#define FT6336_ADDR_TOUCH2_Y 0x0B

#define FT6336_ADDR_FIRMARE_ID 0xA6

#define TP_PRES_DOWN 0x80 // 触屏被按下
#define TP_COORD_UD 0x40  // 触摸坐标更新标记

// 触摸点相关数据结构体定义
typedef struct
{
    uint8_t TouchSta; // 触摸情况，b7:按下1/松开0; b6:0没有按键按下/1有按键按下;bit5:保留；bit4-bit0触摸点按下有效标志，有效为1，分别对应触摸点5-1；
    uint16_t x[5];    // 支持5点触摸，需要使用5组坐标存储触摸点数据
    uint16_t y[5];

} TouchPointRefTypeDef;
extern TouchPointRefTypeDef TPR_Structure;

extern uint8_t touch_count; // 触摸点数标志位

void ft6336_init(void);
void ft6336_scan(void);
void ft6336_task(void);

#endif