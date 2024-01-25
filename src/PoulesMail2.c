/*#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
//#include "smtp_client.h"

#include <ESP_Mail_Client.h>
#include <esp_http_server.h>

// Définissez les informations de votre réseau WiFi
#define WIFI_SSID       "Votre_SSID"
#define WIFI_PASSWORD   "Votre_Mot_de_passe"

// Définissez les informations du serveur SMTP
#define SMTP_SERVER     "smtp.example.com"
#define SMTP_PORT       465  // Utilisez le port SSL (SMTPS)

// Définissez les informations de votre compte e-mail
#define EMAIL_FROM      "votre_adresse@example.com"
#define EMAIL_PASSWORD  "votre_mot_de_passe"
#define EMAIL_TO        "destinataire@example.com"
#define EMAIL_SUBJECT   "Objet du message"
#define EMAIL_BODY      "Corps du message"

static const char *TAG = "ESP32_SMTP";

// Tâche pour envoyer l'e-mail
static void send_email_task(void *pvParameters) {
    esp_smtp_handle_t client = esp_smtp_start(SMTP_SERVER, SMTP_PORT, EMAIL_FROM, EMAIL_PASSWORD);

    if (client == NULL) {
        ESP_LOGE(TAG, "Impossible de se connecter au serveur SMTP");
        vTaskDelete(NULL);
    }

    ESP_LOGI(TAG, "Envoi de l'e-mail...");

    esp_smtp_message_info_t message_info = {
        .to = EMAIL_TO,
        .subject = EMAIL_SUBJECT,
        .body = EMAIL_BODY
    };

    esp_smtp_send_message(client, &message_info);

    ESP_LOGI(TAG, "E-mail envoyé avec succès");

    esp_smtp_stop(client);

    vTaskDelete(NULL);
}

void app_main() {
    // Configuration du gestionnaire d'événements WiFi
    esp_event_loop_init(event_handler, NULL);

    // Configuration du WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    // Attendez que la connexion WiFi soit établie
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (esp_wifi_connect() == ESP_OK) {
            ESP_LOGI(TAG, "WiFi connecté");
            break;
        }
    }

    // Créez la tâche pour envoyer l'e-mail
    xTaskCreate(send_email_task, "send_email_task", 4096, NULL, 5, NULL);
}
*/