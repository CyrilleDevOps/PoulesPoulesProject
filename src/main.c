//
//#include <PoulesAPI.h>
//#include <PoulesTime.h>
#include <WifiPoules.h>
#include <PoulesTasks.h>
//#include <PoulesMail.h>
//#include <PoulesDashboard.h>

//#include "esp32_rf_receiver.h"

//#include "freertos/FreeRTOS.h"
//#include "freertos/event_groups.h"

//#include "owb.h"
//#include "owb_rmt.h"
//#include "ds18b20.h"


#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include "nvs_flash.h"
//#include "PoulesLowPower.h"
//#include "PoulesGlobals.h"

#define TAG_MAIN "Start"
SemaphoreHandle_t mutexActionPorte = NULL;

/*
void app_main_wifi()

{   
    ESP_LOGI(TAG_MAIN, "Configuration Wifi");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    if(init_wifi() == ESP_OK)
    {
            connect_wifi();
    }
}

void app_main_NOWIFI()

{ 
  printf("DEBUT*************************************\n");
  porte *myPorte=NULL;
  myPorte = malloc(sizeof(porte)+4);  
  /
    //Configure Porte   
      
    printf("Init  SizeOf : porte ->%d\n",sizeof(porte));
    uint64_t  Capteurs_myPorte = 0B00000000000000000000000000110000; // 4 & 5
    configure_Input(Capteurs_myPorte);
    myPorte->Capteur_Ouverte=Capteur_Porte_Haut;
    myPorte->Capteur_Fermee=Capteur_Porte_Bas;
    myPorte->Porte_Position=position_porte(myPorte);

    //Configure Moteur    
      
    myPorte->Moteur_Porte.Relais_Sens1 = Moteur_R1;
    myPorte->Moteur_Porte.Relais_Sens2 = Moteur_R2;
    myPorte->Moteur_Porte.Sens = MOTEUR_ARRET;
    configure_moteur(&myPorte->Moteur_Porte);

    myPorte->Moteur_Porte.Sens = MOTEUR_OUVERTURE;

    printf("1*************************************\n");
    printf("myPorte.Capteur_Ouverte : %d \n",myPorte->Capteur_Ouverte);
    printf("myPorte.Capteur_Fermee: %d \n",myPorte->Capteur_Fermee);
    printf("myPorte.Position: %d \n",myPorte->Porte_Position);
    printf("myPorte.Moteur_R1: %d \n",myPorte->Moteur_Porte.Relais_Sens1);
    printf("myPorte.Moteur_R2: %d \n",myPorte->Moteur_Porte.Relais_Sens2);
     printf("myPorte.Sens_R2: %d \n",myPorte->Moteur_Porte.Sens);   
    printf("*************************************\n");
    
    /

    Config_Struct_Porte(myPorte);
    xTaskCreate(Task_Action_Porte,"ACTION_PORTE",2048 ,myPorte,1,NULL);
    //free(myPorte->Moteur_Porte);
    //free(myPorte);  
    printf("FIN*************************************\n");
}

void app_main_DS18B20()

{
DS18B20_Info *Mydevices= 0;
OneWireBus *Myowb=NULL;
owb_rmt_driver_info rmt_driver_info;
float readings = 0;

Mydevices = ds18b20_malloc();  // heap allocation
Myowb = owb_rmt_initialize(&rmt_driver_info, GPIO_DS18B20_0, RMT_CHANNEL_1, RMT_CHANNEL_0);

Init_Capteur_Temperature (Myowb,Mydevices);

readings=Read_Capteur_Temperature (Myowb,Mydevices);

printf("\nTemperature readings (degrees C) %.1f\n",readings);

ds18b20_free(&Mydevices);
owb_uninitialize(Myowb);

//
printf("Restarting now.\n");
fflush(stdout);
vTaskDelay(1000 / portTICK_PERIOD_MS);
esp_restart();
//
}

*/
void app_main() //Receiver 433
{
  int erreur_config_receiver=0;

    nvs_flash_init();
    if(init_wifi() == ESP_OK)
    {
        connect_wifi();
    } 
    


}

