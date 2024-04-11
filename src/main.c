//

#include <WifiPoules.h>
#include <PoulesTasks.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include "nvs_flash.h"

#include <freertos/task.h>


#define TAG_MAIN "Start"
SemaphoreHandle_t mutexActionPorte = NULL;
RTC_DATA_ATTR int  Launch_WIFI=-99;


void app_main() 
{
  printf("****************Start %d \n",Launch_WIFI);
  ESP_LOGI(TAG_MAIN, "Start Main");
  
    if (Launch_WIFI <0)
    { Launch_Wifi();
 
    }
    else
    {
      // Création d'un mutex
        mutexActionPorte = xSemaphoreCreateMutex();
        if( mutexActionPorte != NULL )
        {
            /// DEEP SLEEP
            Launch_WIFI=0;
            
            TaskSchedulePorte();
            TaskDeepSleep();    
          /*
            printf( "Task Name\tStatus\tPrio\tHWM\tTask\tAffinity\n");
            //char stats_buffer[1024];
            vTaskList(stats_buffer);
            printf("%s\n", stats_buffer);
          */
        }
    }


}

