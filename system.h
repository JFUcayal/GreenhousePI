#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>

#include "sensors.h"
#include "actuators.h"


struct sensors_data {
    float temperature;
    float air_humidity;
    float soil_humidity;
    float water_level;
    float light_level;    
};

class System{
    private:
        float temp_define, hum_define, soil_h_define;
        bool light_define;
        Temp_Hum_Sensor temp_hum_sensor;
        LDR_Sensor light_sensor;
        Soil_Hum_Sensor soil_h_sensor; 
        Water_Level_Sensor water_sensor;
        
        Actuators relay_module;

    public:
        System();
        ~System();
        void init();
        void process_data(); 
        time_t get_time(); 
        sensors_data get_sensor_data(); 
        
};  


#endif