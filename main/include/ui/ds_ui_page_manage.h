#ifndef DS_UI_PAGE_MANAGE_H
#define DS_UI_PAGE_MANAGE_H

#include "ds_system_data.h"

typedef enum 
{
    PAGE_TYPE_MENU = 0,
    PAGE_TYPE_TIME,
    PAGE_TYPE_WEATHER,
    PAGE_TYPE_WORD,
    PAGE_TYPE_TOMATO,
    PAGE_TYPE_SETTING,
    PAGE_TYPE_INIT,
}PAGE_TYPE_E;

void ds_ui_page_manage_send_event(TP_ACTION_E act, uint8_t t_x, uint8_t t_y);

void ds_ui_page_manage_init();

#endif