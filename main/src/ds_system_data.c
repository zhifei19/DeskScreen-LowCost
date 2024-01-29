#include "ds_system_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

SYSTEM_DATA_T   sysdata_handler;

static const char *TAG = "ds_system_data";

void sysdata_init(char *p_ssid, uint32_t p_ssidlen, char *p_password, uint32_t p_passwordlen)
{
    memset(&sysdata_handler,0,sizeof(SYSTEM_DATA_T));
    ESP_LOGI(TAG,"System data initialized");
    
    sysdata_set_wifi_info(p_ssid, p_ssidlen, p_password, p_passwordlen);
}

SYSTEM_DATA_T sysdata_get()
{
    return sysdata_handler;
}

void sysdata_set_wifi_info(char *p_ssid, uint32_t p_ssidlen, char *p_password, uint32_t p_passwordlen)
{
    sysdata_handler.ssid_len = p_ssidlen;
    sysdata_handler.password_len = p_passwordlen;
    memcpy(sysdata_handler.ssid,p_ssid,p_ssidlen);
    memcpy(sysdata_handler.password,p_password,p_passwordlen);
    sysdata_handler.ssid[p_ssidlen] = '\0';
    sysdata_handler.password[p_passwordlen] = '\0';
    ESP_LOGI(TAG,"main ssid is: %s, password is: %s\n",p_ssid,p_password);
}

void sysdata_print_wifi_info()
{
    ESP_LOGI(TAG,"WIFI SSID:");
    printf("\r\nwifi_SSID:");
    printf("%s\n",sysdata_handler.ssid);
    ESP_LOGI(TAG,"WIFI Password:");
    printf("\r\nwifi_Password:");
    printf("%s\n",sysdata_handler.password);
    printf("\r\n");
}

SETTING_DATA_E sysdata_check_wifi_info()
{
    if(sysdata_handler.ssid_len != 0)
        return SETTING_DATA_HAS_WIFI_INFO;
    
    return SETTING_DATA_INIT;
}

void reset_tp_action_manage(void)
{
    memset(&sysdata_handler,0,sizeof(SYSTEM_DATA_T));
    printf("reset_tp_action_manage\n");

    sysdata_handler.tp_action_manage.status = 1;
}

static int distance_x, distance_y, abs_x, abs_y;

void check_tp_action(void)
{
    printf("check_tp_action\n");
    sysdata_handler.tp_action_manage.status = 0;
    TP_ACTION_MANAGE_T *m_manage = &sysdata_handler.tp_action_manage;
    printf("start point x: %d, y:%d\n stop point x:%d, y%d\n",
                        m_manage->tp_start_x, m_manage->tp_start_y, 
                        m_manage->tp_stop_x, m_manage->tp_stop_y);
                        
    distance_x = m_manage->tp_stop_x - m_manage->tp_start_x;
    distance_y = m_manage->tp_stop_y - m_manage->tp_start_y;
    abs_x = abs(distance_x);
    abs_y = abs(distance_y);
    if(abs_x < 10 && abs_y < 10){
        //800ms
        if(m_manage->tp_interval_timecount > 80)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_LONG");
            m_manage->tp_action = TP_ACTION_LONG;
            // ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
        else
        {
            ESP_LOGI(TAG,"Action TP_ACTION_SHORT");
            m_manage->tp_action = TP_ACTION_SHORT;
            // ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
    }
    else
    {
        printf("distance_x: %d\n distance_y: %d\n abs_x: %d\n abs_y: %d\n",distance_x,distance_y,abs_x,abs_y);
        if(distance_x < -10 && abs_x > abs_y)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_MOVE_LEFT");
            m_manage->tp_action = TP_ACTION_MOVE_LEFT;
            // ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
        else if(distance_x > 10 && abs_x > abs_y)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_MOVE_RIGHT");
            m_manage->tp_action = TP_ACTION_MOVE_RIGHT;
            // ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
        else if(distance_y > 10 && abs_y > abs_x)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_MOVE_DOWN");
            m_manage->tp_action = TP_ACTION_MOVE_DOWN;
            // ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
        else if(distance_y < -10 && abs_y > abs_x)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_MOVE_UP");
            m_manage->tp_action = TP_ACTION_MOVE_UP;
            // ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
    }
}

void set_tp_action_manage_start_point(uint16_t x,uint16_t y)
{
    sysdata_handler.tp_action_manage.status ++;
    sysdata_handler.tp_action_manage.tp_start_x = x;
    sysdata_handler.tp_action_manage.tp_start_y = y;
}

void set_tp_action_manage_stop_point(uint16_t x,uint16_t y)
{
    sysdata_handler.tp_action_manage.tp_stop_x = x;
    sysdata_handler.tp_action_manage.tp_stop_y = y;
}

// 10ms once count
void count_tp_action_manage_time(void)
{
    sysdata_handler.tp_action_manage.tp_interval_timecount ++;
    //100s
    if(sysdata_handler.tp_action_manage.tp_interval_timecount > 10000){
        sysdata_handler.tp_action_manage.tp_interval_timecount = 10000;
    }
}

BOOL_STATUS_E get_netif_init_state(void)
{
    return sysdata_handler.netif_init_state;
}

void set_netif_init_state(BOOL_STATUS_E status)
{
    sysdata_handler.netif_init_state = status;
}

uint8_t get_tp_action_status(void)
{
    return sysdata_handler.tp_action_manage.status;
}


void set_system_data_tomato_info(uint8_t work_time, uint8_t reset_time, uint8_t time_count)
{

}


void set_system_data_city_info(char *p_city)
{

    
}
