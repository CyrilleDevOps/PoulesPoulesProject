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
//#include <WifiPoules.h>

static int Porte_En_Action=0;


static void initialize_sntp(void)
{
    ESP_LOGD(TAG_TIME, "Initializing SNTP");

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
        ESP_LOGD(TAG_TIME, "Waiting for system time to be set...-%d - (%d/%d)",timeinfo.tm_year, retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    // Print the current time
    ESP_LOGI(TAG_TIME, "Current time: %s", asctime(&timeinfo));
}

int CalculSleepingTime()
{   int SleepingTime = 0;
    time_t now;
    struct tm timeinfo;
    time(&now);
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    localtime_r(&now, &timeinfo);

    int Delta_Heure_Ouveture = timeinfo.tm_hour - HEURE_OUVERTURE;
    int Delta_Heure_Fermeture =timeinfo.tm_hour - HEURE_FERMETURE;

    if ( Delta_Heure_Ouveture <0 && Delta_Heure_Fermeture < 0)
    {SleepingTime = abs(Delta_Heure_Ouveture)*3600-timeinfo.tm_min;}
    else
        if ( Delta_Heure_Ouveture ==0)
        {SleepingTime = 600;}
        else
                if ( Delta_Heure_Ouveture >0 && Delta_Heure_Fermeture < 0)
                {SleepingTime = abs(Delta_Heure_Fermeture)*3600-timeinfo.tm_min;;}
                else
                    if ( Delta_Heure_Fermeture ==0)
                    {SleepingTime = 600;}
                    else
                        {SleepingTime = abs(24- timeinfo.tm_hour+HEURE_OUVERTURE)*3600-timeinfo.tm_min;}
    
    char *message=NULL;
    message = (char *) calloc(1, BUF_SIZE);
    snprintf((char *) message, BUF_SIZE, "SleepingTime :%d" , SleepingTime);
    Poules_Mail_content2 (2,"SleepingTime : ",message) ;
    free (message);  

    return (SleepingTime);


}


// Function to be executed by the task
void scheduled_task(void *pvParameter) 
{   
    short TimeToMovePorte = false;
    time_t now;
    struct tm timeinfo;
    time(&now);
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    ESP_LOGD(TAG_SCHEDULE,"Debut Mutex");
    xSemaphoreTake( mutexActionPorte, portMAX_DELAY );
    int GardeMutex = timeinfo.tm_min;
    porte *ThePorte=NULL;
    ThePorte = pvParameter;

    if (timeinfo.tm_hour == HEURE_OUVERTURE || timeinfo.tm_hour == HEURE_FERMETURE)
    {TimeToMovePorte = true;
     Poules_Mail_content2 (0,"Scheduling_Porte","Start Task") ;
     ESP_LOGI(TAG_SCHEDULE, "scheduled_task- Start %d",GardeMutex );
    }
    else
    {
     ESP_LOGI(TAG_SCHEDULE, "scheduled_task- NOT Start");
    }

    ESP_LOGD(TAG_SCHEDULE, "task : %d  \n",TimeToMovePorte);

    while (1) 
    {
        // Get the current time
        char*  action=NULL;
        time(&now);
        localtime_r(&now, &timeinfo);

        ESP_LOGD(TAG_SCHEDULE,"Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,TimeToMovePorte,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
       
        int condition_reset = 0;
        condition_reset = abs(timeinfo.tm_min-GardeMutex);
        if (condition_reset >=TEMPO_MUTEX &&
             condition_reset <=(60-TEMPO_MUTEX))
        {
            TimeToMovePorte = false;
        }

        
        if (TimeToMovePorte == false)
        {   
            ESP_LOGD(TAG_SCHEDULE,"Fin Mutex");
            xSemaphoreGive(mutexActionPorte);
            vTaskDelay(60000/portTICK_PERIOD_MS);
        }
        else
            {   
            if ((timeinfo.tm_hour == HEURE_OUVERTURE)&&
                (ThePorte->Porte_Position==PORTE_FERMEE||ThePorte->Porte_Position==PORTE_MILIEU))
                {ESP_LOGD(TAG_SCHEDULE, "Executing scheduled task OUVRE Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,Porte_En_Action,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
                ESP_LOGD(TAG_SCHEDULE , "Debut*************************************\n");    
                action="ouvre";
                 Poules_Mail_content2 (2,"Scheduling_Porte","Ouverture") ;
                 Action_Porte(ThePorte,action);
                 TimeToMovePorte = false;
                }
            if ((timeinfo.tm_hour == HEURE_FERMETURE) &&
                (ThePorte->Porte_Position==PORTE_OUVERTE))
                {
                ESP_LOGD(TAG_SCHEDULE, "Executing scheduled task FERME Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,Porte_En_Action,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
                ESP_LOGD(TAG_SCHEDULE , "Debut*************************************\n");
                action="ferme";
                Poules_Mail_content2 (2,"Scheduling_Porte","Fermeture") ;
                Action_Porte(ThePorte,action);
                TimeToMovePorte = false;
                }              
            }
        
        ESP_LOGD(TAG_SCHEDULE,"scheduled time :%d - Mutex :%d \n",timeinfo.tm_min,GardeMutex);
        vTaskDelay(60000/portTICK_PERIOD_MS);
    }

}

void scheduled_task_old(void *pvParameter) 
{   
    short TimeToMovePorte = false;
    time_t now;
    struct tm timeinfo;
    time(&now);
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    ESP_LOGD(TAG_SCHEDULE,"Debut Mutex");
    xSemaphoreTake( mutexActionPorte, portMAX_DELAY );
    int GardeMutex = timeinfo.tm_min;
    Porte_En_Action= 0;
    porte *ThePorte=NULL;
    ThePorte = pvParameter;

    if (timeinfo.tm_hour == HEURE_OUVERTURE || timeinfo.tm_hour == HEURE_FERMETURE)
    {TimeToMovePorte = true;
     Poules_Mail_content2 (0,"Scheduling_Porte","Start Task") ;
     ESP_LOGD(TAG_SCHEDULE, "scheduled_task- Start %d",GardeMutex );
    }
    else
    {
     xSemaphoreGive(mutexActionPorte);
    }

    ESP_LOGI(TAG_SCHEDULE, "task : %d  \n",TimeToMovePorte);

    while (TimeToMovePorte) {
        // Get the current time
        char*  action=NULL;
        time(&now);
        localtime_r(&now, &timeinfo);
        
        ESP_LOGD(TAG_SCHEDULE,"Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,Porte_En_Action,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
        
        int condition_reset = 0;

        condition_reset = timeinfo.tm_min-Porte_En_Action;
        if ((Porte_En_Action!=0) && (condition_reset >=TEMPO_TIME ))
        {
          Porte_En_Action=0;  
          GardeMutex = timeinfo.tm_min;
          ESP_LOGD(TAG_SCHEDULE , "Executing scheduled->Reset - Porte en Action : %d\n",Porte_En_Action); 
          xSemaphoreGive(mutexActionPorte);
        }
        
        condition_reset = abs(timeinfo.tm_min-GardeMutex);
        if ((Porte_En_Action==0) && 
            (condition_reset >=TEMPO_MUTEX &&
             condition_reset <=(60-TEMPO_MUTEX)))
        {
            GardeMutex = timeinfo.tm_min;
            ESP_LOGD(TAG_SCHEDULE,"Fin Mutex");
            xSemaphoreGive(mutexActionPorte);
            ESP_LOGD(TAG_SCHEDULE,"Fin Mutex2");
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
            
            ESP_LOGD(TAG_SCHEDULE, "Executing scheduled task OUVRE Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,Porte_En_Action,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
            ESP_LOGD(TAG_SCHEDULE , "Debut*************************************\n");          
            //ThePorte->Porte_Position=position_porte(ThePorte);
            if (ThePorte->Porte_Position==PORTE_FERMEE||ThePorte->Porte_Position==PORTE_MILIEU)
                {action="ouvre";
                 Porte_En_Action= timeinfo.tm_min;
                 Poules_Mail_content2 (2,"Scheduling_Porte","Ouverture") ;
                 Action_Porte(ThePorte,action);
                 TimeToMovePorte = false;
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
            ESP_LOGD(TAG_SCHEDULE, "Executing scheduled task FERME Position de la porte : % d - Porte en Action : %d - scheduled task  %d:%d:%d",ThePorte->Porte_Position,Porte_En_Action,timeinfo.tm_hour ,timeinfo.tm_min,timeinfo.tm_sec);
            ESP_LOGD(TAG_SCHEDULE , "Debut*************************************\n");
            //ThePorte->Porte_Position=position_porte(ThePorte);
            if (ThePorte->Porte_Position==PORTE_OUVERTE)
                {action="ferme";
                 Porte_En_Action= timeinfo.tm_min;
                 Poules_Mail_content2 (2,"Scheduling_Porte","Fermeture") ;
                 Action_Porte(ThePorte,action);
                 TimeToMovePorte = false;
                }
               
            ESP_LOGD(TAG_SCHEDULE , "Fin ************************************\n");    
            //free(myPorte);
        }
         
        // ESP_LOGD(TAG, "Executing scheduled task every minute at 10s");
        // Delay for 1 second before checking again
       
        //vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGD(TAG_SCHEDULE,"scheduled time :%d - Mutex :%d \n",timeinfo.tm_min,GardeMutex);
        vTaskDelay(3000/portTICK_PERIOD_MS);
    }
    xSemaphoreGive(mutexActionPorte);
}