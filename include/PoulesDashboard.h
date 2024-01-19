
#include <stdio.h>
#include "driver/gpio.h"

#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"

//Entrée
#define Lecture_Temperature1 GPIO_NUM_25

//Capteurs
//#define Capteur_Temperature1  28DCF556B5013C52f

#define GPIO_DS18B20_0       (GPIO_NUM_25)
#define MAX_DEVICES          (8)
#define DS18B20_RESOLUTION   (12)
#define SAMPLE_PERIOD        (1000)   // milliseconds

void Init_Capteur_Temperature (OneWireBus *owb, DS18B20_Info *devices);
float Read_Capteur_Temperature(OneWireBus *CapteurOwb,DS18B20_Info *CapteurDevices);