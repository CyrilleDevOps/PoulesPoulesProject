#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"

#include "esp_event.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include <sys/param.h>

#include <PoulesAPI.h>
#include "esp_log.h"


//#define ESP_WIFI_SSID       "Pepe&Co"
//#define ESP_WIFI_PASSWORD   "Rog31Dan37"
#define ESP_WIFI_SSID       "Freebox-09538F"
#define ESP_WIFI_PASSWORD   "uncinatis63-sagarem2-aient2?-cutit%"
#define NB_RESEAUX_MAX      16
#define WEB_SERVER_PORT     80

int count=0;

//static const char *TAG  = "PoulesPoules Wifi";
static bool demandeConnexion = false;
httpd_handle_t server_httpd = NULL;

httpd_handle_t start_webserver();
void stop_webserver(httpd_handle_t server);

// Gestionnaire d'évènements
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        if(demandeConnexion)
        {
            ESP_LOGI(TAG_WEB , "Connexion ...");
            esp_wifi_connect();
        }
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        // reconnexion ?
        ESP_LOGI(TAG_WEB , "Deconnexion !");
        stop_webserver(server_httpd);
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WEB , "Adresse IP : " IPSTR, IP2STR(&event->ip_info.ip));
        if (server_httpd == NULL) 
        {   ESP_LOGI(TAG_WEB , "Demarrage serveur HTTP");
            server_httpd = start_webserver();
        }
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

/*static bool scan_wifi()
{
    bool present = false;
    uint16_t number = NB_RESEAUX_MAX;
    wifi_ap_record_t ap_info[NB_RESEAUX_MAX];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    // démarre le scan
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

    ESP_LOGI(TAG_WEB , "Nb reseaux trouves = %u", ap_count);
    for (int i = 0; (i < NB_RESEAUX_MAX) && (i < ap_count); i++)
    {
        // cf. https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html?highlight=wifi_ap_record_t#_CPPv416wifi_ap_record_t
        ESP_LOGI(TAG_WEB , "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI(TAG_WEB , "RSSI \t\t%d", ap_info[i].rssi);
        ESP_LOGI(TAG_WEB , "Channel \t\t%d\n", ap_info[i].primary);
        if(strcmp((char *)ap_info[i].ssid, ESP_WIFI_SSID) == 0)
            present = true;
    }

    return present;
}
*/

static void restart_wifi()
{
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_start());
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
    restart_wifi();
}

// Gestionnaire d'URI pour une requête GET /porte
esp_err_t get_handler(httpd_req_t *req)
{
    char*   buf=NULL;
    char*  action;
    size_t buf_len;

        
    // recherche "Host" dans l'entête de la requête reçue
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) 
    {
        buf = malloc(buf_len);
        // copie la valeur dans un buffer
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) 
        {
            ESP_LOGI(TAG_WEB , "Header => Host: %s", buf);
        }
        free(buf);
    }

    // récupère la longueur de la chaîne de la requête
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) 
    {
        buf = malloc(buf_len);
        // récupère la requête
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) 
        {
            char param[32];
            ESP_LOGI(TAG_WEB , "Requete => %s", buf);
            
            // récupère la valeur d'un paramètre de la requête (exemple : ?param1=val1&param2=val2)
            if (httpd_query_key_value(buf, "action", param, sizeof(param)) == ESP_OK) 
            {
                ESP_LOGI(TAG_WEB , "Parametre => action=%s", param);
                action = param;
            }            
        }
        free(buf);
    }

    // définit le type de contenu HTTP (par défaut : "text/html")
    if (httpd_resp_set_type(req, "text/plain") == ESP_OK)
    {
        // ajoute des en-têtes supplémentaires
        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    }

    if (strcmp(action,"ouvre")==0 || strcmp(action,"ferme")==0 || strcmp(action,"statut")==0)
    {  
        count++;
        ESP_LOGD(TAG_WEB , "%d-Debut*************************************\n",count);
         if (strcmp(action,"ouvre")==0 )         //Ouvre_Porte();
        { 
        porte *myPorte=NULL;
        myPorte = malloc(sizeof(porte)+4);  
        Config_Struct_Porte(myPorte);   
        myPorte->Moteur_Porte.Sens = MOTEUR_OUVERTURE;
        ESP_LOGD(TAG_WEB , "Ouvre Porte (0:Arret-1:Ouvre-2:Ferme/0:action): %d\n",myPorte->Moteur_Porte.Sens);
        xTaskCreate(API_Action_Porte,"ACTION_PORTE",2048 ,myPorte,1,NULL);      
        }
         else if (strcmp(action,"ferme")==0 )    //Ferme_Porte();
        { 
        porte *myPorte=NULL;
        myPorte = malloc(sizeof(porte)+4);  
        Config_Struct_Porte(myPorte);        
        myPorte->Moteur_Porte.Sens = MOTEUR_FERMETURE;
        ESP_LOGD(TAG_WEB , "Ferme Porte (0:Arret-1:Ouvre-2:Ferme/0:action): %d\n",myPorte->Moteur_Porte.Sens);
        xTaskCreate(API_Action_Porte,"ACTION_PORTE",2048 ,myPorte,1,NULL);
        }  
        else if (strcmp(action,"statut")==0 )    //Ferme_Porte();
        {
        porte *myPorteStatus=NULL;
       
        myPorteStatus = malloc(sizeof(porte)+4);  
        Config_Struct_Porte(myPorteStatus); 
        ESP_LOGD(TAG_WEB, "Position de la Porte %d.\n", myPorteStatus->Porte_Position);
        action = position_porte_texte(myPorteStatus->Porte_Position);
        free(myPorteStatus);
        }
        ESP_LOGD(TAG_WEB , "%d-Fin ************************************\n",count);
    }
    char resp[] = "-It works!->";         // envoie une réponse   
    strcat(resp,action);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    
    return ESP_OK;
}

// Gestionnaire d'URI pour une requête POST /test
esp_err_t post_handler(httpd_req_t *req)
{
    char content[128];

    ESP_LOGI(TAG_WEB , "Longueur contenu POST : %d", req->content_len);

    // tronque si la longueur du contenu est supérieure au buffer
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);

    // déconnecté ?
    if (ret <= 0) 
    {
        // timeout ?
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) 
        {
            // retourne une erreur HTTP 408 (Request Timeout)
            httpd_resp_send_408(req);
        }
        
        return ESP_FAIL;
    }
    ESP_LOGI(TAG_WEB , "Contenu POST : %s", content);

    // envoie une réponse
    const char resp[] = "Test Reponse POST";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

httpd_handle_t start_webserver()
{
    // initialise la configuration par défaut du serveur HTTP
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = WEB_SERVER_PORT;

    // pour une requête GET /test
    httpd_uri_t uri_get = {
        .uri      = "/porte",
        .method   = HTTP_GET,
        .handler  = get_handler,
        .user_ctx = NULL
    };

    /* pour une requête POST /test
    httpd_uri_t uri_post = {
        .uri      = "/test",
        .method   = HTTP_POST,
        .handler  = post_handler,
        .user_ctx = NULL
    };
    */

    ESP_LOGI(TAG_WEB , "Port serveur HTTP : '%d'", config.server_port);

    httpd_handle_t server = NULL;
    // Démarre le serveur HTTP
    if (httpd_start(&server, &config) == ESP_OK) 
    {
        // enregistre les gestionnaires d'URI
        httpd_register_uri_handler(server, &uri_get);
        //httpd_register_uri_handler(server, &uri_post);
    }

    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server) 
    {
        // arrête le serveur HTTP
        httpd_stop(server);
    }
}
