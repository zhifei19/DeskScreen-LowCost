#ifndef _DS_FT6336_H_
#define _DS_FT6336_H_

#include <stdint.h>

#define GPIO_TP_RST_LOW 0
#define GPIO_TP_RST_HIGH 1

#define I2C_ADDR_FT6336 0x38

// FT6236 ���ּĴ�������
#define FT_DEVIDE_MODE 0x00    // FT6236ģʽ���ƼĴ���
#define FT_GEST_ID 0x01        // �����¼�����
#define FT_REG_NUM_FINGER 0x02 // ����״̬�Ĵ���

#define FT_TP1_REG 0X03 // ��һ�����������ݵ�ַ
#define FT_TP2_REG 0X09 // �ڶ������������ݵ�ַ
#define FT_TP3_REG 0X0F // ���������������ݵ�ַ
#define FT_TP4_REG 0X15 // ���ĸ����������ݵ�ַ
#define FT_TP5_REG 0X1B // ��������������ݵ�ַ

#define FT_ID_G_LIB_VERSION 0xA1  // �汾
#define FT_ID_G_MODE 0xA4         // FT6236�ж�ģʽ���ƼĴ���
#define FT_ID_G_THGROUP 0x80      // ������Чֵ���üĴ���
#define FT_ID_G_PERIODACTIVE 0x88 // ����״̬�������üĴ���
#define Chip_Vendor_ID 0xA3       // оƬID(0x36)
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

#define TP_PRES_DOWN 0x80 // ����������
#define TP_COORD_UD 0x40  // ����������±��

// ������������ݽṹ�嶨��
typedef struct
{
    uint8_t TouchSta; // ���������b7:����1/�ɿ�0; b6:0û�а�������/1�а�������;bit5:������bit4-bit0�����㰴����Ч��־����ЧΪ1���ֱ��Ӧ������5-1��
    uint16_t x[5];    // ֧��5�㴥������Ҫʹ��5������洢����������
    uint16_t y[5];

} TouchPointRefTypeDef;
extern TouchPointRefTypeDef TPR_Structure;

extern uint8_t touch_count; // ����������־λ

void ft6336_init(void);
void ft6336_scan(void);
void ft6336_task(void);

#endif