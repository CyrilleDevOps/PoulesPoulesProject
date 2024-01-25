#include "esp_http_server.h"


httpd_handle_t start_webserver();
void stop_webserver(httpd_handle_t server);


#define NB_RESEAUX_MAX      16
#define WEB_SERVER_PORT     80

#define TAG_WEB "WEB SERVER"
