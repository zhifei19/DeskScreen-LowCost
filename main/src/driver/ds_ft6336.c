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

// ����ʾ�⣨FPC���£�
////y��////////    //264x176
//
//
// x��
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
    ESP_ERROR_CHECK(i2c_master_write_data(FT_DEVIDE_MODE, &temp, 1)); // ������������ģʽ

    temp = 22;                                                         // ������Чֵ��22��ԽСԽ����
    ESP_ERROR_CHECK(i2c_master_write_data(FT_ID_G_THGROUP, &temp, 1)); // ���ô�����Чֵ
    ESP_ERROR_CHECK(i2c_master_read_data(FT_ID_G_THGROUP, &r_data, 1));
    ESP_LOGD(TAG, "init THGROUP = %d \n", r_data);

    temp = 14; // �������ڣ�����С��12�����14
    ESP_ERROR_CHECK(i2c_master_write_data(FT_ID_G_PERIODACTIVE, &temp, 1));
    ESP_ERROR_CHECK(i2c_master_read_data(FT_ID_G_PERIODACTIVE, &r_data, 1));
    ESP_LOGD(TAG, "init PERIODACTIVE = %d \n", r_data);

    temp = 0; // �жϷ�ʽ��Ϊ��ѯ
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
    ESP_ERROR_CHECK(i2c_master_read_data(FT_REG_NUM_FINGER, &sta, 1)); // ��ȡ�������״̬
    touch_count = sta;
    ESP_ERROR_CHECK(i2c_master_read_data(FT_GEST_ID, &gestid, 1)); // ��ȡ�������״̬

    if (sta & 0x0f) // �ж��Ƿ��д����㰴�£�0x02�Ĵ����ĵ�4λ��ʾ��Ч�������
    {
        TPR_Structure.TouchSta = ~(0xFF << (sta & 0x0F)); //~(0xFF << (sta & 0x0F))����ĸ���ת��Ϊ�����㰴����Ч��־
        for (i = 0; i < 2; i++)                           // �ֱ��жϴ�����1-5�Ƿ񱻰���
        {
            if (TPR_Structure.TouchSta & (1 << i))                                // ��ȡ����������
            {                                                                     // ���������ȡ��Ӧ��������������
                ESP_ERROR_CHECK(i2c_master_read_data(FT6236_TPX_TBL[i], buf, 4)); // ��ȡXY����ֵ
                TPR_Structure.x[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
                TPR_Structure.y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                if ((buf[0] & 0XC0) != 0X80)
                {
                    TPR_Structure.x[i] = TPR_Structure.y[i] = 0; // ������contact�¼�������Ϊ��Ч
                    TPR_Structure.TouchSta &= 0xe0;              // �����������Ч���
                    return;
                }
            }
        }
        TPR_Structure.TouchSta |= TP_PRES_DOWN; // �������±��
    }
    else
    {
        if (TPR_Structure.TouchSta & TP_PRES_DOWN) // ֮ǰ�Ǳ����µ�
            TPR_Structure.TouchSta &= ~0x80;       // �����ɿ����
        else
        {
            TPR_Structure.x[0] = 0;
            TPR_Structure.y[0] = 0;
            TPR_Structure.TouchSta &= 0xe0; // �����������Ч���
        }
    }
}

void ft6336_task(void)
{
    if (TPR_Structure.TouchSta & TP_COORD_UD) // �����а���
    {
        TPR_Structure.TouchSta = 0; // ����
        ft6336_scan();              // ��ȡ��������

        switch (touch_count)
        {
        case 1:
            if ((TPR_Structure.x[0] != 0) && (TPR_Structure.y[0] != 0) && (TPR_Structure.x[0] < 200) && (TPR_Structure.y[0] < 200)) // ����˵���Ч����
            {
                ESP_LOGD(TAG,"�����������:%d", touch_count); // FT6336U���֧�����㴥��
                ESP_LOGD(TAG,"x0:%d,y0:%d", TPR_Structure.x[0], TPR_Structure.y[0]);
            }
            break;
        case 2:
            if ((TPR_Structure.x[0] != 0) && (TPR_Structure.y[0] != 0) && (TPR_Structure.x[1] != 0) && (TPR_Structure.y[1] != 0) && (TPR_Structure.x[0] < 200) && (TPR_Structure.y[0] < 200) && (TPR_Structure.x[1] < 200) && (TPR_Structure.y[1] < 200)) // ����˵���Ч����
            {
                ESP_LOGD(TAG,"�����������:%d", touch_count); // FT6336U���֧�����㴥��
                ESP_LOGD(TAG,"x0:%d,y0:%d", TPR_Structure.x[0], TPR_Structure.y[0]);
                ESP_LOGD(TAG,"x1:%d,y1:%d", TPR_Structure.x[1], TPR_Structure.y[1]);
            }
            break;

        default:
            break;
        }
        // ��ձ��
        for (int i = 0; i < 2; i++)
        {
            TPR_Structure.x[i] = 0;
            TPR_Structure.y[i] = 0;
        }
    }
}
