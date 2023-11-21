#ifndef SENSORS_H_
#define SENSORS_H_

#include <iostream>
#include <string>

#include "buff.h"
#include "adc.h"

#define SAMPLE_SIZE 10 //10 values for average Temp/Hum calculations

using namespace std;

class Temp_Hum_Sensor{
    private:
        float temperature, humidity;
        Avg_buff temp_sample_buff;
        Avg_buff hum_sample_buff;

    public:
        Temp_Hum_Sensor();
        ~Temp_Hum_Sensor();
        void init();
        float get_Temperature();
        float get_Humidity();
};

class LDR_Sensor{
    private:
        float brightness_value, min_light_threshold;
    public:
        LDR_Sensor();
        ~LDR_Sensor();
        float get_brightness_value();
};

class Soil_Hum_Sensor{
    private:
        float soil_moisture;
        Avg_buff soilH_sample_buff;
    public:
        Soil_Hum_Sensor();
        ~Soil_Hum_Sensor();
        float get_soil_moisture();
};

class Water_Level_Sensor{
    private:
        float water_Level;
    public:
        Water_Level_Sensor();
        ~Water_Level_Sensor();
        float get_water_level();
};


#endif