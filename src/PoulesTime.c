#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <esp_sntp.h>

#include <sys/param.h>
#include "nvs_flash.h"

#include <PoulesAPI.h>
#include <PoulesTime.h>
#include <PoulesMail.h>

static void initialize_sntp(void)
{
    ESP_LOGI(TAG_TIME, "Initializing SNTP");

    // Configure the SNTP service
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org"); // You can use a different NTP server if needed

    // Initialize the SNTP service
    sntp_init();
}

 void obtain_time(void)
{
    // Initialize the SNTP module
    initialize_sntp();

    // Wait for the time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    
    time(&now);
    localtime_r(&now, &timeinfo);

    while (timeinfo.tm_year < (2020- 1900) && ++retry < retry_count)
    {
        ESP_LOGI(TAG_TIME, "Waiting for system time to be set...-%d - (%d/%d)",timeinfo.tm_year, retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    // Print the current time
    ESP_LOGI(TAG_TIME, "Current time: %s", asctime(&timeinfo));
}

// Function to be executed by the task
void scheduled_task(void *pvParameter) 
{
    ESP_LOGI(TAG_SCHEDULE, "scheduled_task- Start " );
    ESP_LOGI(TAG_SCHEDULE, "task \n");

    while (1) {
        // Get the current time
        char*  action=NULL;
        time_t now;
        struct tm timeinfo;
        time(&now);
        setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
        tzset();
        localtime_r(&now, &timeinfo);


        if ((timeinfo.tm_hour == HEURE_OUVERTURE && timeinfo.tm_min == HEURE_MINUTE_OUVERTURE))
        {   ESP_LOGI(TAG_SCHEDULE, "Executing scheduled task every minute at 10s %d:%d:%d",timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
            ESP_LOGD(TAG_SCHEDULE , "Debut*************************************\n");
            Poules_Mail_content ("Scheduling_Porte","Ouverture") ;
            porte *myPorte=NULL;
            myPorte = malloc(sizeof(porte)+4);  
            Config_Struct_Porte(myPorte);
            if (myPorte->Porte_Position==PORTE_OUVERTE)
                {action="ouvre";}
            ESP_LOGD(TAG_SCHEDULE , "Fin ************************************\n");    
        }
        if ((timeinfo.tm_hour == HEURE_FERMETURE && timeinfo.tm_min == HEURE_MINUTE_FERMETURE))
        {   ESP_LOGI(TAG_SCHEDULE, "Executing scheduled task every minute at 10s %d:%d:%d",timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
            ESP_LOGD(TAG_SCHEDULE , "Debut*************************************\n");
            Poules_Mail_content ("Scheduling_Porte","Fermeture") ;
            porte *myPorte=NULL;
            myPorte = malloc(sizeof(porte)+4);  
            Config_Struct_Porte(myPorte);
            if (myPorte->Porte_Position==PORTE_FERMEE)
                {action="ferme";}
            ESP_LOGD(TAG_SCHEDULE , "Fin ************************************\n");    
        }
         
        // ESP_LOGI(TAG, "Executing scheduled task every minute at 10s");
        // Delay for 1 second before checking again
       
        //vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelay(3000/portTICK_PERIOD_MS);
    }
}