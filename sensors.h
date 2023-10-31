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
    Temp_Hum_Sensor(float sTemp, float sHum);
    ~Temp_Hum_Sensor();

    void init();
    float get_Temperature();
    float get_Humidity();
};

class LDR_Sensor{

};

class Soil_Hum_Sensor{

};

class Water_Level_Sensor{

};


#endif