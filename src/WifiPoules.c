#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <freertos/semphr.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"

#include "nvs_flash.h"
#include <sys/param.h>
#include <time.h>

//#include <PoulesAPI.h>
#include <PoulesTime.h>
#include <WifiPoules.h>
#include <PoulesServer.h>
#include <PoulesMail.h>
#include <PoulesAPI.h>


//static const char *TAG  = "PoulesPoules Wifi";
static bool demandeConnexion = false;
httpd_handle_t server_httpd = NULL;


// Gestionnaire d'évènements
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        if(demandeConnexion)
        {
            ESP_LOGI(TAG_WIFI , "Connexion ...");
            esp_wifi_connect();
        }
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        // reconnexion ?
        ESP_LOGI(TAG_WIFI , "Deconnexion !");
            esp_wifi_connect();
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
         // Create a task with the scheduled_task function as the entry point
        int erreur_config_receiver=0;
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI , "Adresse IP : " IPSTR, IP2STR(&event->ip_info.ip));
        obtain_time();
        ESP_LOGI(TAG_SCHEDULE, "Start " );
        erreur_config_receiver= xTaskCreate(&scheduled_task, "ScheduledTask", 2048, NULL, 5, NULL);
        if (erreur_config_receiver != 0)
        {    
            ESP_LOGI(TAG_SCHEDULE, "Task Ok " );
        }
        if (server_httpd == NULL) 
        {   ESP_LOGI(TAG_WIFI , "Demarrage serveur HTTP");
            server_httpd = start_webserver();
        }

        //test email
        char *message=NULL;
        message = (char *) calloc(1, BUF_SIZE);
        snprintf((char *) message, BUF_SIZE, "Adresse IP : %d:%d:%d:%d" , IP2STR(&event->ip_info.ip));
        Poules_Mail_content ("INIT sur IP : ",message) ;
        free (message);
        
       
    }
}

//static esp_err_t init_wifi()
esp_err_t init_wifi()
{
    // initialise la pile TCP/IP
    ESP_ERROR_CHECK(esp_netif_init());
    // crée une boucle d'évènements
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // crée 
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // initialise la configuration Wifi par défaut 
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // initialise le WiFi avec la configuration par défaut et démarre également la tâche WiFi.
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
   
    // installe le gestionnaire d'évènements Wifi
 
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    // définit le mode de fonctionnement station pour le WiFi
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // démarre le WiFi selon la configuration
    esp_err_t ret = esp_wifi_start();
    ESP_ERROR_CHECK(ret);

    return ret;
}

//static void connect_wifi()
void connect_wifi()
{
    // configure la connexion Wifi du point d'accès (AP)
    wifi_config_t wifi_config = {
        .sta = {
        .ssid = ESP_WIFI_SSID,
        .password = ESP_WIFI_PASSWORD,
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        .pmf_cfg = {
            .capable = true,
            .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    demandeConnexion = true;
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_start());
}


