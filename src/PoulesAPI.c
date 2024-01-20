//
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
//#include "esp_smtp.h"

#include "PoulesAPI.h"
#include "esp_log.h"


void configure_Output(int Sortie)
{
    gpio_reset_pin(Sortie);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(Sortie, GPIO_MODE_INPUT_OUTPUT);
}


void configure_Input(uint64_t  Parametre_pin_bit_mask)
{               
    gpio_config_t Parametre_Pin_Input;
    
    Parametre_Pin_Input.pin_bit_mask = Parametre_pin_bit_mask;
	Parametre_Pin_Input.mode = GPIO_MODE_INPUT; // <-- all combinations here
	Parametre_Pin_Input.pull_up_en = GPIO_PULLUP_ENABLE;
	Parametre_Pin_Input.pull_down_en = GPIO_PULLDOWN_DISABLE;
	Parametre_Pin_Input.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&Parametre_Pin_Input);
}

int position_porte (void *Parametre_porte )
{
    int s_Porte_Fermee,s_Porte_Ouverte;
    s_Porte_Ouverte=gpio_get_level(((porte*)Parametre_porte)->Capteur_Ouverte); 
    s_Porte_Fermee=gpio_get_level(((porte*)Parametre_porte)->Capteur_Fermee);

    if (s_Porte_Ouverte==0 && s_Porte_Fermee==1)
    {
     ESP_LOGW(TAG_API,"position_porte->OUVERTE : %d/%d \n",s_Porte_Ouverte,s_Porte_Fermee);
     return (PORTE_OUVERTE);
    }
    else if (s_Porte_Ouverte==1 && s_Porte_Fermee==0)
    {
      ESP_LOGW(TAG_API,"position_porte->FERMEE : %d/%d \n",s_Porte_Ouverte,s_Porte_Fermee);
      return (PORTE_FERMEE);
    }
    else if (s_Porte_Ouverte==1 && s_Porte_Fermee==1)
    {
      ESP_LOGW(TAG_API,"position_porte->AU MILIEU : %d/%d \n",s_Porte_Ouverte,s_Porte_Fermee);
      return (PORTE_MILIEU);
    }
    else
    {
      ESP_LOGW(TAG_API,"position_porte->UNKNOWN : %d/%d \n",s_Porte_Ouverte,s_Porte_Fermee);
      return (-1);
    }
}
char *position_porte_texte(int PositionPorte)
{
    char *Statut=NULL;
    ESP_LOGD(TAG_API, "Position de la Porte %d.\n", PositionPorte);
    if (PositionPorte==PORTE_OUVERTE)
            {Statut="Porte Ouverte";}
        else if (PositionPorte==PORTE_FERMEE)
            {Statut="Porte Fermée";}
        else if (PositionPorte==PORTE_MILIEU)
            {Statut="Porte Au Milieu";}
        else
            {Statut="Erreur Porte";}

return (Statut);

}

void configure_moteur(moteur *Parametre_Moteur)
{
    
    ESP_LOGD(TAG_API,"configure_moteur->Config Moteur R1/R2 : %d/%d \n",Parametre_Moteur->Relais_Sens1,Parametre_Moteur->Relais_Sens2);
    ESP_LOGD(TAG_API,"configure_moteur->Config Moteur Sens: %d \n",Parametre_Moteur->Sens);

    gpio_reset_pin(Parametre_Moteur->Relais_Sens1);
    gpio_reset_pin(Parametre_Moteur->Relais_Sens2);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(Parametre_Moteur->Relais_Sens1, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(Parametre_Moteur->Relais_Sens2, GPIO_MODE_INPUT_OUTPUT);

    Parametre_Moteur->Sens = 0;

}

void action_moteur(moteur *Parametre_Moteur,int Sens_Rotation)
{
    ESP_LOGD(TAG_API,"Action Moteur : %d/%d/%d \n",Parametre_Moteur->Relais_Sens1,Parametre_Moteur->Relais_Sens2,Sens_Rotation);

    gpio_set_level(Parametre_Moteur->Relais_Sens1, 0);
    gpio_set_level(Parametre_Moteur->Relais_Sens2, 0);  
    
    if (Sens_Rotation == MOTEUR_ARRET)
    {
        gpio_set_level(Parametre_Moteur->Relais_Sens1, 0);
        gpio_set_level(Parametre_Moteur->Relais_Sens2, 0);
    }
    else if (Sens_Rotation == MOTEUR_OUVERTURE )
    {
         gpio_set_level(Parametre_Moteur->Relais_Sens1, 1);
    }
    else if (Sens_Rotation == MOTEUR_FERMETURE )
    {
        gpio_set_level(Parametre_Moteur->Relais_Sens2, 1);

    }    
}

static void blink_led(int Led, int led_state)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    printf("Allume Led : %d/%d->%d \n",Led, gpio_get_level(Led),led_state);
    gpio_set_level(Led, led_state);
}

void Poules_Mail (void *pvParameters) 
{/*
    while (1) {
        // Configure the email message
        smtp_message_t message = {
            .subject = "ESP32 Email Test",
            .to = RECIPIENT_EMAIL,
            .from = SENDER_EMAIL,
            .text = "This is a test email sent from ESP32 with ESP-IDF."
        };

        // Send the email
        if (esp_smtp_send(&message) == ESP_OK) {
            ESP_LOGI("EMAIL", "Email sent successfully");
        } else {
            ESP_LOGE("EMAIL", "Failed to send email");
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS);  // Delay before sending the next email
    }
    */
}


//Void API Porte
void API_Action_Porte(void *Parametre_Porte)
{
    /* Configure the peripheral according to the LED type */
    //Ouverture  Led2_Porte_Ouverte clignotante/ Led1_Porte_Fermee Fixe
    //Fermeture  Led2_Porte_Ouverte Fixe/ Led1_Porte_Fermee Clignotante
    //int Led1_Porte_Fermee= Led_16;
    //int Led2_Porte_Ouverte=Led_17; 
    int s_Mvt_Porte = PORTE_REPOS; //,s_Led1_Porte_Fermee,s_Led2_Porte_Ouverte;
    int Capteur_Porte_Ouverte, Capteur_Porte_Fermee,Position_Porte;  
    
    porte *ThePorte=NULL;
    //ThePorte = malloc(sizeof(porte)+4);
    ThePorte = Parametre_Porte;

    Capteur_Porte_Ouverte=ThePorte->Capteur_Ouverte;
    Capteur_Porte_Fermee=ThePorte->Capteur_Fermee;
    Position_Porte=ThePorte->Porte_Position;
    
    ESP_LOGD(TAG_API,"API_Action_Porte->Action_Porte.Sens (0:Arret-1:Ouvre-2:Ferme/0:action): %d\n",ThePorte->Moteur_Porte.Sens);
    ESP_LOGD(TAG_API,"API_Action_Porte->myPorte.Moteur_Sens 1/2: %d->%d \n",ThePorte->Moteur_Porte.Relais_Sens1,ThePorte->Moteur_Porte.Relais_Sens2);
    
    ESP_LOGV(TAG_API,"API_Action_Porte->myPorte.Moteur_Sens 1/2: %d->%d \n",ThePorte->Moteur_Porte.Relais_Sens1,ThePorte->Moteur_Porte.Relais_Sens2);
    ESP_LOGV(TAG_API,"API_Action_Porte->Action_Porte.Sens (0:Arret-1:Ouvre-2:Ferme/0:action): %d\n",ThePorte->Moteur_Porte.Sens);
    ESP_LOGV(TAG_API,"API_Action_Porte->myPorte.Moteur_Sens 1/2: %d->%d \n",ThePorte->Moteur_Porte.Relais_Sens1,ThePorte->Moteur_Porte.Relais_Sens2);

    //configure_Output(Led1_Porte_Fermee);
    //configure_Output(Led2_Porte_Ouverte);

    if (ThePorte->Moteur_Porte.Sens==MOTEUR_OUVERTURE&&Position_Porte==PORTE_FERMEE)
        {
            ESP_LOGW(TAG_API,"API_Action_Porte->OUVERTURE : %d/%d \n",ThePorte->Moteur_Porte.Sens,Position_Porte);
            s_Mvt_Porte=0;
        }
    else  if (ThePorte->Moteur_Porte.Sens==MOTEUR_FERMETURE&&Position_Porte==PORTE_OUVERTE)
        {
            ESP_LOGW(TAG_API,"API_Action_Porte->FERMETURE : %d/%d \n",ThePorte->Moteur_Porte.Sens,Position_Porte);
            s_Mvt_Porte=0;
        }
    else
        {
            ESP_LOGW(TAG_API,"API_Action_Porte->FORCE OUVERTURE : %d/%d \n",ThePorte->Moteur_Porte.Sens,Position_Porte);
            s_Mvt_Porte=0;
            ThePorte->Moteur_Porte.Sens=MOTEUR_OUVERTURE;
        }

    ESP_LOGD(TAG_API,"API_Action_Porte->Task*************************************\n");
    ESP_LOGD(TAG_API,"API_Action_Porte->myPorte.Capteur_Ouverte : %d->%d \n",Capteur_Porte_Ouverte,gpio_get_level(Capteur_Porte_Ouverte));
    ESP_LOGD(TAG_API,"API_Action_Porte->myPorte.Capteur_Fermee: %d->%d \n",Capteur_Porte_Fermee,gpio_get_level(Capteur_Porte_Fermee));
    ESP_LOGD(TAG_API,"API_Action_Porte->myPorte.Moteur_Sens 1/2: %d->%d \n",ThePorte->Moteur_Porte.Relais_Sens1,ThePorte->Moteur_Porte.Relais_Sens2);
    ESP_LOGD(TAG_API,"API_Action_Porte->myPorte.Position (2:Fermée-1:Ouverte): %d \n",Position_Porte);
    ESP_LOGD(TAG_API,"API_Action_Porte->Action_Porte.Sens (0:Arret-1:Ouvre-2:Ferme/0:action): %d/%d \n",ThePorte->Moteur_Porte.Sens,s_Mvt_Porte);
    ESP_LOGD(TAG_API,"API_Action_Porte->*************************************\n");

    /*
    s_Led1_Porte_Fermee=1;
    s_Led2_Porte_Ouverte=1;
    blink_led(Led2_Porte_Ouverte,s_Led2_Porte_Ouverte);
    blink_led(Led1_Porte_Fermee,s_Led1_Porte_Fermee);
    */

    int loopPorte=0;
    int alignePorte = 0;

    while ((s_Mvt_Porte==PORTE_EN_MVT || s_Mvt_Porte==PORTE_ALIGNEMENT) && loopPorte< PORTE_TIMEOUT )
    {   
        ESP_LOGV(TAG_API,"API_Action_Porte->%d-myPorte.Capteur_Ouverte : %d->%d \n",loopPorte,Capteur_Porte_Ouverte,gpio_get_level(Capteur_Porte_Ouverte));
        ESP_LOGV(TAG_API,"API_Action_Porte->%d-myPorte.Capteur_Fermee: %d->%d \n",loopPorte,Capteur_Porte_Fermee,gpio_get_level(Capteur_Porte_Fermee));
        if (ThePorte->Moteur_Porte.Sens==MOTEUR_OUVERTURE)
        {    
            //s_Led2_Porte_Ouverte=!s_Led2_Porte_Ouverte;
            s_Mvt_Porte=!gpio_get_level(Capteur_Porte_Ouverte);
            ESP_LOGW(TAG_API,"%d:o \n",loopPorte);
            ESP_LOGD(TAG_API,"API_Action_Porte->%d-Ouverture (Ouvre-> (0 : en mouvement/ 1 en alignement)): %d \n",loopPorte,s_Mvt_Porte);
        }
        else  if (ThePorte->Moteur_Porte.Sens==MOTEUR_FERMETURE)
        {
            //s_Led1_Porte_Fermee=!s_Led1_Porte_Fermee;    
            s_Mvt_Porte=!gpio_get_level(Capteur_Porte_Fermee);
            ESP_LOGW(TAG_API,"%d:f \n",loopPorte);
            ESP_LOGD(TAG_API,"API_Action_Porte->%d-Fermeture (Ferme->(0 : en mouvement/ 1 en alignement)): %d \n",loopPorte,s_Mvt_Porte);
        }

      if (s_Mvt_Porte==PORTE_ALIGNEMENT && alignePorte<PORTE_ALIGNE_CAPTEUR && loopPorte>=PORTE_LAG_CAPTEUR)
        {
           alignePorte++;
           ESP_LOGW(TAG_API,"API_Action_Porte->Alignement ->%d",alignePorte);
        }
        else if (s_Mvt_Porte==PORTE_ALIGNEMENT && alignePorte>=PORTE_ALIGNE_CAPTEUR)
        {
            s_Mvt_Porte=PORTE_REPOS;
            ESP_LOGW(TAG_API,"API_Action_Porte->Fin du mouvement");
        }
 
        /*
        blink_led(Led2_Porte_Ouverte,s_Led2_Porte_Ouverte);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        blink_led(Led1_Porte_Fermee,s_Led1_Porte_Fermee);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        */

        action_moteur(&ThePorte->Moteur_Porte,ThePorte->Moteur_Porte.Sens);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        loopPorte++;
    } 
    //s_Led2_Porte_Ouverte= !gpio_get_level(Capteur_Porte_Ouverte); 
    //s_Led1_Porte_Fermee= !gpio_get_level(Capteur_Porte_Fermee);   
    //blink_led(Led2_Porte_Ouverte,s_Led2_Porte_Ouverte);
    //blink_led(Led1_Porte_Fermee,s_Led1_Porte_Fermee);

    action_moteur(&ThePorte->Moteur_Porte,MOTEUR_ARRET);

 
    //free(ThePorte);  
    vTaskDelete( NULL );
}


void Config_Struct_Porte(porte *myPorte)
{

    ESP_LOGD(TAG_API,"Config_Struct_Porte-DEBUT*************************************\n");
    //Configure Porte   
    uint64_t  Capteurs_myPorte = Capteurs_Porte;
    configure_Input(Capteurs_myPorte);
    myPorte->Capteur_Ouverte=Capteur_Porte_Haut;
    myPorte->Capteur_Fermee=Capteur_Porte_Bas;
    myPorte->Porte_Position=position_porte(myPorte);

    //Configure Moteur    
    myPorte->Moteur_Porte.Relais_Sens1 = Moteur_R1;
    myPorte->Moteur_Porte.Relais_Sens2 = Moteur_R2;
    myPorte->Moteur_Porte.Sens = MOTEUR_ARRET;
    configure_moteur(&myPorte->Moteur_Porte);
    
    ESP_LOGD(TAG_API,"Config_Struct_Porte->myPorte.Capteur_Ouverte : %d \n",myPorte->Capteur_Ouverte);
    ESP_LOGD(TAG_API,"Config_Struct_Porte->myPorte.Capteur_Fermee: %d \n",myPorte->Capteur_Fermee);
    ESP_LOGD(TAG_API,"Config_Struct_Porte->myPorte.Position: %d \n",myPorte->Porte_Position);
    ESP_LOGD(TAG_API,"Config_Struct_Porte->myPorte.Moteur_R1: %d \n",myPorte->Moteur_Porte.Relais_Sens1);
    ESP_LOGD(TAG_API,"Config_Struct_Porte->myPorte.Moteur_R2: %d \n",myPorte->Moteur_Porte.Relais_Sens2);
    ESP_LOGD(TAG_API,"Config_Struct_Porte->myPorte.Sens_R2: %d \n",myPorte->Moteur_Porte.Sens);   
    ESP_LOGD(TAG_API,"Config_Struct_Porte->Config_Struct_Porte-FIN*************************************\n");
}