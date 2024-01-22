#ifndef _DS_SYSTEM_DATA_H_
#define _DS_SYSTEM_DATA_H_

#define MAX_SSID_LEN        32
#define MAX_PASSWORD_LEN    32

#include <stdint.h>

typedef enum{
    BOOL_STATUS_DEACTIVE = 0,
    BOOL_STATUS_ACTIVE,
}BOOL_STATUS_E;

typedef enum{
    TP_ACTION_NULL,
    TP_ACTION_SHORT,
    TP_ACTION_LONG,
    TP_ACTION_MOVE_LEFT,
    TP_ACTION_MOVE_RIGHT,
    TP_ACTION_MOVE_UP,
    TP_ACTION_MOVE_DOWN,
}TP_ACTION_E;

typedef struct{
    int tp_interval_timecount;
    uint8_t status;
    uint16_t tp_start_x;
    uint16_t tp_start_y;
    uint16_t tp_stop_x;
    uint16_t tp_stop_y;
    TP_ACTION_E tp_action;
}TP_ACTION_MANAGE_T;

typedef struct {
    char ssid[MAX_SSID_LEN];
    uint32_t ssid_len;
    char password[MAX_PASSWORD_LEN];
    uint32_t password_len;

    TP_ACTION_MANAGE_T tp_action_manage;
    uint32_t tp_wakeup_timeleft;

    BOOL_STATUS_E netif_init_state;
}SYSTEM_DATA_T;

typedef enum{
    SETTING_DATA_INIT = 0,
    SETTING_DATA_HAS_WIFI_INFO,
}SETTING_DATA_E;


void sysdata_init(char *p_ssid, uint32_t p_ssidlen, char *p_password, uint32_t p_passwordlen);

SYSTEM_DATA_T sysdata_get();

void sysdata_set_wifi_info(char *p_ssid, uint32_t p_ssidlen, char *p_password, uint32_t p_passwordlen);

void sysdata_print_wifi_info();

SETTING_DATA_E sysdata_check_wifi_info();

void reset_tp_action_manage(void);

void check_tp_action(void);

void set_tp_action_manage_start_point(uint16_t x,uint16_t y);

void set_tp_action_manage_stop_point(uint16_t x,uint16_t y);

void count_tp_action_manage_time(void);

BOOL_STATUS_E get_netif_init_state(void);

void set_netif_init_state(BOOL_STATUS_E status);

uint8_t get_tp_action_status(void);

#endif