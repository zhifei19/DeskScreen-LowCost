#ifndef DS_CONF_H_
#define DS_CONF_H_

#define SPIFFS_BASE_PATH        "/spiffs"

#define SSID                    "123"
#define PASSWORD                "456"

#define SCREEN_VERTICAL                 200
#define SCREEN_HORIZONTAL               200


#define TASK_BACKGROUND_PRIORITY        10
#define TASK_BACKGROUND_STACKSIZE       2048

#define UI_PAGE_MANAGER_PRIORITY        10
#define UI_PAGE_MANAGER_STACKSIZE       2048

#define TASK_TIMER_PRIORITY             10
#define TASK_TIMER_STACKSIZE            2048

#define TASK_TP_GPIO_PRIORITY           10
#define TASK_TP_GPIO_STACKSIZE          2048

#define TASK_HTTP_REQUEST_PRIORITY      5
#define TASK_HTTP_REQUEST_STACKSIZE     8192

#define EPD_2IN9BC_WIDTH       200
#define EPD_2IN9BC_HEIGHT      200
#define IMAGE_SIZE (((EPD_2IN9BC_WIDTH % 8 == 0) ? (EPD_2IN9BC_WIDTH / 8 ) : (EPD_2IN9BC_WIDTH / 8 + 1)) * EPD_2IN9BC_HEIGHT)


// #define TP_DEBUG_ENABLE
// #define HEAP_SIZE_ENABLE



#endif