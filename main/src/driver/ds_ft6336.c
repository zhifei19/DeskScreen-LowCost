#include "ds_ft6336.h"
#include "ds_i2c.h"

#include "esp_log.h"
#include "ds_gpio.h"

const static char *TAG = "ds_ft6336";

const uint16_t FT6236_TPX_TBL[5] =
    {
        FT_TP1_REG,
        FT_TP2_REG,
        FT_TP3_REG,
        FT_TP4_REG,
        FT_TP5_REG};

// 坐标示意（FPC朝下）
////y轴////////    //264x176
//
//
// x轴
//
//
uint8_t touch_count = 0;

TouchPointRefTypeDef TPR_Structure;

void ft6336_init()
{
    uint8_t temp, r_data;

    gpio_tp_init();

    gpio_set_tp_rst(GPIO_TP_RST_LOW);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_tp_rst(GPIO_TP_RST_HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(i2c_master_init());
    vTaskDelay(100 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "ft6336 initialized");

    temp = 0;
    ESP_ERROR_CHECK(i2c_master_write_data(FT_DEVIDE_MODE, &temp, 1)); // 进入正常操作模式

    temp = 22;                                                         // 触摸有效值，22，越小越灵敏
    ESP_ERROR_CHECK(i2c_master_write_data(FT_ID_G_THGROUP, &temp, 1)); // 设置触摸有效值
    ESP_ERROR_CHECK(i2c_master_read_data(FT_ID_G_THGROUP, &r_data, 1));
    ESP_LOGD(TAG, "init THGROUP = %d \n", r_data);

    temp = 14; // 激活周期，不能小于12，最大14
    ESP_ERROR_CHECK(i2c_master_write_data(FT_ID_G_PERIODACTIVE, &temp, 1));
    ESP_ERROR_CHECK(i2c_master_read_data(FT_ID_G_PERIODACTIVE, &r_data, 1));
    ESP_LOGD(TAG, "init PERIODACTIVE = %d \n", r_data);

    temp = 0; // 中断方式改为轮询
    ESP_ERROR_CHECK(i2c_master_write_data(FT_ID_G_MODE, &temp, 1));
    ESP_ERROR_CHECK(i2c_master_read_data(FT_ID_G_MODE, &r_data, 1));
    ESP_LOGD(TAG, "init G_MODE = %d \n", r_data);
}

void ft6336_scan(void)
{
    uint8_t i = 0;
    uint8_t sta = 0;
    uint8_t buf[4] = {0};
    uint8_t gestid = 0;
    ESP_ERROR_CHECK(i2c_master_read_data(FT_REG_NUM_FINGER, &sta, 1)); // 读取触摸点的状态
    touch_count = sta;
    ESP_ERROR_CHECK(i2c_master_read_data(FT_GEST_ID, &gestid, 1)); // 读取触摸点的状态

    if (sta & 0x0f) // 判断是否有触摸点按下，0x02寄存器的低4位表示有效触点个数
    {
        TPR_Structure.TouchSta = ~(0xFF << (sta & 0x0F)); //~(0xFF << (sta & 0x0F))将点的个数转换为触摸点按下有效标志
        for (i = 0; i < 2; i++)                           // 分别判断触摸点1-5是否被按下
        {
            if (TPR_Structure.TouchSta & (1 << i))                                // 读取触摸点坐标
            {                                                                     // 被按下则读取对应触摸点坐标数据
                ESP_ERROR_CHECK(i2c_master_read_data(FT6236_TPX_TBL[i], buf, 4)); // 读取XY坐标值
                TPR_Structure.x[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
                TPR_Structure.y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                if ((buf[0] & 0XC0) != 0X80)
                {
                    TPR_Structure.x[i] = TPR_Structure.y[i] = 0; // 必须是contact事件，才认为有效
                    TPR_Structure.TouchSta &= 0xe0;              // 清除触摸点有效标记
                    return;
                }
            }
        }
        TPR_Structure.TouchSta |= TP_PRES_DOWN; // 触摸按下标记
    }
    else
    {
        if (TPR_Structure.TouchSta & TP_PRES_DOWN) // 之前是被按下的
            TPR_Structure.TouchSta &= ~0x80;       // 触摸松开标记
        else
        {
            TPR_Structure.x[0] = 0;
            TPR_Structure.y[0] = 0;
            TPR_Structure.TouchSta &= 0xe0; // 清除触摸点有效标记
        }
    }
}

void ft6336_task(void)
{
    if (TPR_Structure.TouchSta & TP_COORD_UD) // 触摸有按下
    {
        TPR_Structure.TouchSta = 0; // 清标记
        ft6336_scan();              // 读取触摸坐标

        switch (touch_count)
        {
        case 1:
            if ((TPR_Structure.x[0] != 0) && (TPR_Structure.y[0] != 0) && (TPR_Structure.x[0] < 200) && (TPR_Structure.y[0] < 200)) // 软件滤掉无效操作
            {
                ESP_LOGD(TAG,"触摸点个数：:%d", touch_count); // FT6336U最多支持两点触控
                ESP_LOGD(TAG,"x0:%d,y0:%d", TPR_Structure.x[0], TPR_Structure.y[0]);
            }
            break;
        case 2:
            if ((TPR_Structure.x[0] != 0) && (TPR_Structure.y[0] != 0) && (TPR_Structure.x[1] != 0) && (TPR_Structure.y[1] != 0) && (TPR_Structure.x[0] < 200) && (TPR_Structure.y[0] < 200) && (TPR_Structure.x[1] < 200) && (TPR_Structure.y[1] < 200)) // 软件滤掉无效操作
            {
                ESP_LOGD(TAG,"触摸点个数：:%d", touch_count); // FT6336U最多支持两点触控
                ESP_LOGD(TAG,"x0:%d,y0:%d", TPR_Structure.x[0], TPR_Structure.y[0]);
                ESP_LOGD(TAG,"x1:%d,y1:%d", TPR_Structure.x[1], TPR_Structure.y[1]);
            }
            break;

        default:
            break;
        }
        // 清空标记
        for (int i = 0; i < 2; i++)
        {
            TPR_Structure.x[i] = 0;
            TPR_Structure.y[i] = 0;
        }
    }
}
