#ifndef _DS_WIFI_AP_H_
#define _DS_WIFI_AP_H_

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define ESP_AP_WIFI_SSID      CONFIG_ESP_AP_WIFI_SSID
#define ESP_AP_WIFI_PASS      CONFIG_ESP_AP_WIFI_PASSWORD
#define ESP_AP_WIFI_CHANNEL   CONFIG_ESP_AP_WIFI_CHANNEL
#define MAX_AP_STA_CONN       CONFIG_ESP_AP_MAX_STA_CONN

void wifi_init_softap(void);

#endif