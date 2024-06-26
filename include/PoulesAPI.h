#include <stdio.h>
#include "driver/gpio.h"


//Global


#define MODE_SIMU 0
#define TAG_API "API"



//Declaration Config GPIO
//Sortie 
#define Led_Ouverte GPIO_NUM_16
#define Led_Fermee GPIO_NUM_17

#define Moteur_R1 GPIO_NUM_14
#define Moteur_R2 GPIO_NUM_15

//Entrée
#define Capteur_Porte_Haut GPIO_NUM_22
//#define Capteur_Porte_Bas  GPIO_NUM_4
#define Capteur_Porte_Bas   GPIO_NUM_5
#define Capteurs_Porte  0B00000000010000000000000000100000 // 22 & 5

//Declaration Moteur
#define  MOTEUR_ARRET  0
#define  MOTEUR_OUVERTURE   1
#define  MOTEUR_FERMETURE   2

//Declaration porte
#define  PORTE_NON_DEFINIE  0
#define  PORTE_OUVERTE   1
#define  PORTE_FERMEE  2
#define  PORTE_MILIEU  3

//Mouvement Porte
#define  PORTE_TIMEOUT   35
#define  PORTE_LAG_CAPTEUR  10
#define  PORTE_ALIGNE_CAPTEUR  1

#define  PORTE_EN_MVT 0
#define  PORTE_ALIGNEMENT 1
#define  PORTE_REPOS 2



struct struct_moteur {
 int Relais_Sens1;
 int Relais_Sens2;
 int Sens;
};

typedef  struct struct_moteur moteur;

struct struct_porte {
 int Capteur_Ouverte;
 int Capteur_Fermee;
 int Porte_Position;
 moteur Moteur_Porte;
 };

 typedef  struct struct_porte porte;

void blink_led(int Led, int led_state);
void configure_Output(int Sortie);
void configure_moteur(moteur *Parametre_Moteur);
void configure_Input(uint64_t  Parametre_pin_bit_mask);
int position_porte (void *Parametre_porte );
char *position_porte_texte(int PositionPorte);
void action_moteur(moteur *Parametre_Moteur,int Sens_Rotation);
void Task_Action_Porte(void *Parametre_Porte);
void Config_Struct_Porte(porte *myPorte);
void Init_Struct_Porte(porte *myPorte);
void Affiche_Struct_Porte(porte *myPorte);
void Action_Porte(porte *myPorte,char*  action);

//void Poules_Mail (message_mail *message);

//int init_wifi();
//void connect_wifi();