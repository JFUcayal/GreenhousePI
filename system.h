#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <iostream>
#include <stdio.h>
#include <string>

#include "sensors.h"
#include "actuators.h"


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
        void data_processing();
        uint16_t adc_convert();
};


#endif