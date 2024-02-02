#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_server.h"

#include "nvs_flash.h"
#include <sys/param.h>


#include "PoulesServer.h"

#include "PoulesAPI.h"
//extern porte *myPorte;
int count=0;

//static const char *TAG  = "PoulesPoules Wifi";
//static bool demandeConnexion = false;
//httpd_handle_t server_httpd = NULL;

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
    {   count++;
        ESP_LOGD(TAG_WEB , "%d-Debut*************************************\n",count);
        porte *myPorte2=NULL;
        myPorte2 = malloc(sizeof(porte)+4);  
        Init_Struct_Porte(myPorte2);
        myPorte2->Porte_Position=position_porte (myPorte2);
        Action_Porte(myPorte2,action);
        ESP_LOGD(TAG_WEB , "%d-Fin ************************************\n",count);    
        char resp[] = "-It works!->";         // envoie une réponse   
        strcat(resp,position_porte_texte( myPorte2->Porte_Position));
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    }
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
