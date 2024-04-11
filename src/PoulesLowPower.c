#include <esp_sleep.h>
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"

#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "PoulesLowPower.h"
#include "PoulesMail.h"
#include "PoulesGlobals.h"

void deep_sleep_task(void *args)
{   
    ESP_LOGD(TAG_SLEEP,"Debut Mutex");
    xSemaphoreTake( mutexActionPorte, portMAX_DELAY );

    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;
        
    char *message=NULL;
    message = (char *) calloc(1, BUF_SIZE);
    snprintf((char *) message, BUF_SIZE, "Cause : %d  --  Elapse:%d" , ESP_SLEEP_WAKEUP_TIMER,sleep_time_ms);
    Poules_Mail_content2 (0,"Deep WakeUP : ",message) ;
    free (message);   
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_TIMER: {
            ESP_LOGD(TAG_SLEEP,"Wake up from timer. Time spent in deep sleep: %dms\n", sleep_time_ms);
            ESP_LOGD(TAG_SLEEP,"Fin Mutex");
            xSemaphoreGive( mutexActionPorte );
            break;
        }
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            ESP_LOGD(TAG_SLEEP,"Not a deep sleep reset\n");
            ESP_LOGD(TAG_SLEEP,"Fin Mutex");
            xSemaphoreGive( mutexActionPorte );
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    
    rtc_gpio_isolate(GPIO_NUM_12);
    ESP_LOGD(TAG_SLEEP,"Entering deep sleep\n");
    
    // get deep sleep enter time
    
    gettimeofday(&sleep_enter_time, NULL);
    
    // enter deep sleep
    esp_deep_sleep_start();
}

void setup_rtc_timer_wakeup(int wakeup_time_sec )
{   
    // const int wakeup_time_sec = WAKEUP_TIME;

    ESP_LOGI(TAG_SLEEP,"Enabling timer wakeup, %ds\n", wakeup_time_sec);
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_time_sec * uS_TO_S_FACTOR));
}