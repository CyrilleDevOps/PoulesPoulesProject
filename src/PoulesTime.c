#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include <esp_sntp.h>

#include <sys/param.h>
#include "nvs_flash.h"

#include "PoulesTime.h"

#include "PoulesAPI.h"
#include "PoulesMail.h"
#include "PoulesGlobals.h"


static int Porte_En_Action=0;


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
    time_t now;
    struct tm timeinfo;
    time(&now);
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    ESP_LOGI(TAG_SCHEDULE,"Debut Mutex");
    xSemaphoreTake( mutexActionPorte, portMAX_DELAY );
    int GardeMutex = timeinfo.tm_min;
    Porte_En_Action= 0;
    porte *ThePorte=NULL;
    ThePorte = pvParameter;

    ESP_LOGI(TAG_SCHEDULE, "scheduled_task- Start %d",GardeMutex );
    ESP_LOGI(TAG_SCHEDULE, "task \n");

    while (1) {
        // Get the current time
        char*  action=NULL;
        time(&now);
        localtime_r(&now, &timeinfo);
        
        ESP_LOGI(TAG_SCHEDULE,"Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,Porte_En_Action,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
        
        int condition_reset = 0;

        condition_reset = timeinfo.tm_min-Porte_En_Action;
        if ((Porte_En_Action!=0) && (condition_reset >=TEMPO_TIME ))
        {
          Porte_En_Action=0;  
          GardeMutex = timeinfo.tm_min;
          ESP_LOGI(TAG_SCHEDULE , "Executing scheduled->Reset - Porte en Action : %d\n",Porte_En_Action); 
          xSemaphoreGive(mutexActionPorte);
        }
        
        condition_reset = abs(timeinfo.tm_min-GardeMutex);
        if ((Porte_En_Action==0) && 
            (condition_reset >=TEMPO_MUTEX &&
             condition_reset <=(60-TEMPO_MUTEX)))
        {
            GardeMutex = timeinfo.tm_min;
            ESP_LOGI(TAG_SCHEDULE,"Fin Mutex");
            xSemaphoreGive(mutexActionPorte);
            ESP_LOGI(TAG_SCHEDULE,"Fin Mutex2");
        }
        
        if (timeinfo.tm_hour == HEURE_OUVERTURE && 
             ((timeinfo.tm_min== 0 ||
             timeinfo.tm_min== 6 ||
             timeinfo.tm_min== 12 ||
             timeinfo.tm_min== 18||
             timeinfo.tm_min== 24 ||
             timeinfo.tm_min== 30 ||
             timeinfo.tm_min== 36 ||
             timeinfo.tm_min== 42||
             timeinfo.tm_min== 48 ||
             timeinfo.tm_min== 54 ) && 
             (Porte_En_Action==0)))
        {   
            
            ESP_LOGI(TAG_SCHEDULE, "Executing scheduled task OUVRE Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,Porte_En_Action,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
            ESP_LOGD(TAG_SCHEDULE , "Debut*************************************\n");          
            //ThePorte->Porte_Position=position_porte(ThePorte);
            if (ThePorte->Porte_Position==PORTE_FERMEE||ThePorte->Porte_Position==PORTE_MILIEU)
                {action="ouvre";
                 Porte_En_Action= timeinfo.tm_min;
                 Poules_Mail_content ("Scheduling_Porte","Ouverture") ;
                 Action_Porte(ThePorte,action);
                }
            ESP_LOGD(TAG_SCHEDULE , "Fin ************************************\n");    
            //free(myPorte);
        }
        
        if (timeinfo.tm_hour == HEURE_FERMETURE && 
            ((timeinfo.tm_min== 0 ||
             timeinfo.tm_min== 6 ||
             timeinfo.tm_min== 12 ||
             timeinfo.tm_min== 18||
             timeinfo.tm_min== 24 ||
             timeinfo.tm_min== 30 ||
             timeinfo.tm_min== 36 ||
             timeinfo.tm_min== 42||
             timeinfo.tm_min== 48 ||
             timeinfo.tm_min== 54 )  && 
  
 /*           ((timeinfo.tm_min== 3 ||
             timeinfo.tm_min== 9 ||
             timeinfo.tm_min== 15 ||
             timeinfo.tm_min== 21||
             timeinfo.tm_min== 27 ||
             timeinfo.tm_min== 33 ||
             timeinfo.tm_min== 39 ||
             timeinfo.tm_min== 45||
             timeinfo.tm_min== 45||
             timeinfo.tm_min== 51  ) && 

*/             (Porte_En_Action==0)))     
        {   
            ESP_LOGI(TAG_SCHEDULE, "Executing scheduled task FERME Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,Porte_En_Action,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
            ESP_LOGD(TAG_SCHEDULE , "Debut*************************************\n");
            //ThePorte->Porte_Position=position_porte(ThePorte);
            if (ThePorte->Porte_Position==PORTE_OUVERTE)
                {action="ferme";
                 Porte_En_Action= timeinfo.tm_min;
                 Poules_Mail_content ("Scheduling_Porte","Fermeture") ;
                 Action_Porte(ThePorte,action);
                }
               
            ESP_LOGD(TAG_SCHEDULE , "Fin ************************************\n");    
            //free(myPorte);
        }
         
        // ESP_LOGI(TAG, "Executing scheduled task every minute at 10s");
        // Delay for 1 second before checking again
       
        //vTaskDelay(pdMS_TO_TICKS(1000));

        printf("scheduled time :%d - Mutex :%d \n",timeinfo.tm_min,GardeMutex);
        vTaskDelay(3000/portTICK_PERIOD_MS);
  
    }
}