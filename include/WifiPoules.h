#include "esp_system.h"
#include "esp_wifi.h"

#include "esp_event.h"

#define TAG_WIFI  "PoulesPoules Wifi"


//#define ESP_WIFI_SSID       "Pepe&Co"
//#define ESP_WIFI_PASSWORD   "Rog31Dan37"
//#define ESP_WIFI_SSID       "Galaxy ToupikandFlooo"
//#define ESP_WIFI_PASSWORD   "ToupikTheBest"
#define ESP_WIFI_SSID       "Freebox-09538F"
#define ESP_WIFI_PASSWORD   "uncinatis63-sagarem2-aient2?-cutit%"
#define NB_RESEAUX_MAX      16
#define WEB_SERVER_PORT     80

//static esp_err_t init_wifi()
esp_err_t init_wifi();
void connect_wifi();
