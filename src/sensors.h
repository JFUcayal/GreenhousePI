#ifndef SENSORS_H_
#define SENSORS_H_

#include <iostream>
#include <string>

#include "buff.h"
#include "adc.h"

#define SAMPLE_SIZE 10 //10 values for average Temp/Hum calculations

using namespace std;

//classe sensor de temperatura e humidade
class Temp_Hum_Sensor{      
    private:
        float temperature, humidity;
        Avg_buff temp_sample_buff;
        Avg_buff hum_sample_buff;

    public:
        Temp_Hum_Sensor();
        ~Temp_Hum_Sensor();
        void init();
        float get_Temperature();        //devolve o valor de temperatura lido pelo sensor
        float get_Humidity();           //devolve o valor de humidade lido pelo sensor
};

//classe do sensor de luz
class LDR_Sensor{
    private:
        float brightness_value, min_light_threshold;
    public:
        LDR_Sensor();
        ~LDR_Sensor();
        float get_brightness_value();       //devolve o valor lido pelo sensor
};

//classe do sensor de humidade do solo
class Soil_Hum_Sensor{
    private:
        float soil_moisture;
        Avg_buff soilH_sample_buff;
    public:
        Soil_Hum_Sensor();
        ~Soil_Hum_Sensor();
        float get_soil_moisture();      //devolve o valor lido pelo sensor
};

//classe do sensor que mede o nível de água disponivel no reservatório
class Water_Level_Sensor{
    private:
        float water_Level;
    public:
        Water_Level_Sensor();
        ~Water_Level_Sensor();
        int get_water_level();      //devolve o valor lido pelo sensor
};


#endif
