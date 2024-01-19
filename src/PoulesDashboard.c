
//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"

#include "PoulesDashboard.h"

void Init_Capteur_Temperature (OneWireBus *owb,DS18B20_Info *ds18b20_info)
{
    // Create a 1-Wire bus, using the RMT timeslot driver

    OneWireBus_ROMCode rom_code;
    owb_status status;

    owb_use_crc(owb, true);  // enable CRC check for ROM code

    status = owb_read_rom(owb, &rom_code);
   if (status == OWB_STATUS_OK)
      {
       char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
       owb_string_from_rom_code(rom_code, rom_code_s, sizeof(rom_code_s));
       printf("Single device %s present\n", rom_code_s);
      }
    else
      {
        printf("An error occurred reading ROM code: %d", status);
      }
  
    // Create DS18B20 devices on the 1-Wire bus

    //ds18b20_info = ds18b20_malloc();  // heap allocation
    //devices = ds18b20_info;

    printf("Single device optimisations enabled\n");
    ds18b20_init_solo(ds18b20_info, owb);          // only one device on bus
    ds18b20_use_crc(ds18b20_info, true);           // enable CRC check on all reads
    ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);
    
    // Check for parasitic-powered devices
    bool parasitic_power = false;
    ds18b20_check_for_parasite_power(owb, &parasitic_power);
    if (parasitic_power) {
        printf("Parasitic-powered devices detected");
    }

    // In parasitic-power mode, devices cannot indicate when conversions are complete,
    // so waiting for a temperature conversion must be done by waiting a prescribed duration
    owb_use_parasitic_power(owb, parasitic_power);
}

//void Read_Capteur_Temperature ( OneWireBus *owb,DS18B20_Info *devices)
float Read_Capteur_Temperature(OneWireBus *CapteurOwb,DS18B20_Info *CapteurDevices)
{
  /*
  DS18B20_Info *devices;
  devices = ds18b20_info;
  */
  //TickType_t last_wake_time = xTaskGetTickCount();
  ds18b20_convert_all(CapteurOwb);

  // In this application all devices use the same resolution,
  // so use the first device to determine the delay
  ds18b20_wait_for_conversion(CapteurDevices);

  // Read the results immediately after conversion otherwise it may fail
  // (using printf before reading may take too long)
  float readings = 0;
  DS18B20_ERROR errors = 0;
  errors = ds18b20_read_temp(CapteurDevices, &readings);

  if (errors==0)
    {
      return (readings);
    }
  else 
  {
    return (errors);
  }
  

  // Print results in a separate loop, after all have been read
 // printf("\nTemperature readings (degrees C) %.1f errors:%d\n",readings,errors);

}





