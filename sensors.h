#ifndef SENSORS_H_
#define SENSORS_H_

#include <iostream>
#include <string>

#define ARRAY_SIZE 10

using namespace std;

class Temp_Hum_Sensor{
    private:
        float temperature, humidity;
        float avg_temp[ARRAY_SIZE], avg_humidity[ARRAY_SIZE];

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
        float avg_soil_moisture[ARRAY_SIZE];
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