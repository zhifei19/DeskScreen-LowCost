#include "ds_system_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "esp_log.h"
#include "ds_ui_page_manage.h"
#include "ds_conf.h"

static SYSTEM_DATA_T sysdata_handler;

static const char *TAG = "ds_system_data";

void sysdata_init(char *p_ssid, uint32_t p_ssidlen, char *p_password, uint32_t p_passwordlen)
{
    memset(&sysdata_handler,0,sizeof(SYSTEM_DATA_T));
    ESP_LOGI(TAG,"System data initialized");
    
    sysdata_set_wifi_info(p_ssid, p_ssidlen, p_password, p_passwordlen);
}

SYSTEM_DATA_T sysdata_get()
{
    ESP_LOGI(TAG, "sysdata_get().hour is %s\n",sysdata_handler.hour);
    ESP_LOGI(TAG, "sysdata_get().password is %s\n",sysdata_handler.password);
    ESP_LOGI(TAG, "sysdata_get().test is %d\n",sysdata_handler.test);
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

#ifdef TP_DEBUG_ENABLE
    printf("start point x: %d, y:%d\n stop point x:%d, y%d\n",
                        m_manage->tp_start_x, m_manage->tp_start_y, 
                        m_manage->tp_stop_x, m_manage->tp_stop_y);
#endif // TP_DEBUG_ENABLE
                        
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
            ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
        else
        {
            ESP_LOGI(TAG,"Action TP_ACTION_SHORT");
            m_manage->tp_action = TP_ACTION_SHORT;
            ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
    }
    else
    {
#ifdef TP_DEBUG_ENABLE
        printf("distance_x: %d\n distance_y: %d\n abs_x: %d\n abs_y: %d\n",distance_x,distance_y,abs_x,abs_y);
#endif // TP_DEBUG_ENABLE
        if(distance_x < -50 && abs_x > abs_y)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_MOVE_LEFT");
            m_manage->tp_action = TP_ACTION_MOVE_LEFT;
            ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
        else if(distance_x > 50 && abs_x > abs_y)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_MOVE_RIGHT");
            m_manage->tp_action = TP_ACTION_MOVE_RIGHT;
            ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
        else if(distance_y > 50 && abs_y > abs_x)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_MOVE_DOWN");
            m_manage->tp_action = TP_ACTION_MOVE_DOWN;
            ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
            return;
        }
        else if(distance_y < -50 && abs_y > abs_x)
        {
            ESP_LOGI(TAG,"Action TP_ACTION_MOVE_UP");
            m_manage->tp_action = TP_ACTION_MOVE_UP;
            ds_ui_page_manage_send_event(m_manage->tp_action,m_manage->tp_stop_x,m_manage->tp_stop_y);
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


void set_system_data_wifi_info(char *ssid, char *pwd)
{
    uint8_t ssid_len = strlen(ssid);
    uint8_t pwd_len = strlen(pwd);
    memcpy(sysdata_handler.ssid, ssid, ssid_len);
    memcpy(sysdata_handler.password, pwd, pwd_len);
    sysdata_handler.ssid_len = ssid_len;
    sysdata_handler.password_len = pwd_len;
    ESP_LOGI(TAG,"WIFI data stored in system data");
}

void print_system_data_wifi_info(void)
{
    printf("Update WIFI SSID:");
    for(int i=0;i<sysdata_handler.ssid_len;i++)
    {
        printf("%c",sysdata_handler.ssid[i]);
    }
    printf("\nUpdate WIFI PASSWORD:");
    for(int i=0;i<sysdata_handler.password_len;i++)
    {
        printf("%c",sysdata_handler.password[i]);
    }
}


static uint8_t string_to_int(char *input)
{
    uint8_t len = strlen(input);
    uint8_t diff;
    int num = 0;
    for(int i=0;i<len;i++)
    {
        diff = input[0] - '0';
        diff = pow(10,len-i);
        num += diff;
    }
    printf("int is %d\n", num);
    return num;
}


void set_system_data_tomato_info(char *work_time, char *reset_time, char *time_count)
{
    uint8_t int_work_time;
    uint8_t int_reset_time;
    uint8_t int_time_count;
    int_work_time = string_to_int(work_time);
    int_reset_time = string_to_int(reset_time);
    int_time_count = string_to_int(time_count);

    sysdata_handler.tomato_work_time = int_work_time;
    sysdata_handler.tomato_reset_time = int_reset_time;
    sysdata_handler.tomato_time_count = int_time_count;
    ESP_LOGI(TAG,"Tomato clock data stored in system data");
}


void set_system_data_city_info(char *p_city)
{
    sysdata_handler.setting_city_len = strlen(p_city);
    memcpy(sysdata_handler.setting_city, p_city, sysdata_handler.setting_city_len);
    printf("city_len is %d\ncity name is %s",sysdata_handler.setting_city_len,sysdata_handler.setting_city);
    ESP_LOGI(TAG,"City data stored in system data");
}

void ds_update_system_time(char *hour, char *minute, char *second)
{
    strncpy(sysdata_handler.hour, hour, 2);
    strncpy(sysdata_handler.minute, minute, 2);
    strncpy(sysdata_handler.second, second, 2);
    printf("sysdata_handler.hour is %s\n",sysdata_handler.hour);
    printf("sysdata_handler.minute is %s\n",sysdata_handler.minute);
    printf("sysdata_handler.second is %s\n",sysdata_handler.second);
    sysdata_handler.test = 6;
}