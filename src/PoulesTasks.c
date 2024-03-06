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

#include "esp32_rf_receiver.h"

//#include <PoulesAPI.h>
#include "PoulesTime.h"
#include "WifiPoules.h"
#include "PoulesServer.h"
#include "PoulesMail.h"
#include "PoulesAPI.h"
#include "PoulesLowPower.h"
#include "PoulesTasks.h"

//static bool demandeConnexion = false;
//httpd_handle_t server_httpd = NULL;
porte *myPorte=NULL;
        
void TaskPorte()
{ 
        myPorte = malloc(sizeof(porte)+4);  
        Config_Struct_Porte(myPorte);
        myPorte->Porte_Position=position_porte (myPorte);
        
        if ( MODE_SIMU==1)
            {   blink_led(Led_Ouverte,1);
                blink_led(Led_Fermee,0);
            }
        //Affiche_Struct_Porte(myPorte); 
}

void TaskSchedulePorte()
{
        if ( MODE_SIMU==1)
            {   blink_led(Led_Ouverte,1);
                blink_led(Led_Fermee,0);
            }
        myPorte = malloc(sizeof(porte)+4);  
        Config_Struct_Porte(myPorte);
        myPorte->Porte_Position=position_porte (myPorte);    

        /// TASK SCHEDULE
        ESP_LOGI(TAG_SCHEDULE, "Start " );
        int erreur_config_receiver=0;
        erreur_config_receiver= xTaskCreate(&scheduled_task, "ScheduledTask", 12288, myPorte, 2, NULL);
        if (erreur_config_receiver != 0)
        {    
            ESP_LOGI(TAG_SCHEDULE, "Task Ok " );
        }
}

void TaskRF433()
{
        // initialize RF433 Receiver
        ESP_LOGI(TAG_WIFI, "Configuration receiver\n");
        int erreur_config_receiver=0;
        erreur_config_receiver= Config_receiver_rf433 ();
        ESP_LOGI(TAG_WIFI, "Configuration receiver : %d (0=erreur)\n",erreur_config_receiver);
        if (erreur_config_receiver != 0)
        {    
            xTaskCreate(&receiver_rf433, "receiver_rf433", 2048, myPorte, 3, NULL);
        }
}

void TaskDeepSleep()
{
        ///DEEP SLEEP
        setup_rtc_timer_wakeup();
        xTaskCreate(deep_sleep_task, "deep_sleep_task", 4096, NULL, 1, NULL);
}