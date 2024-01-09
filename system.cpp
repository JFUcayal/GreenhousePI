#include "system.h"
#include <iostream>
#include <stdio.h>

using namespace std;

System::System(){
    cout << "****************************************" << endl;
    cout << "System Created" << endl ;
}

System::~System(){
    cout << "****************************************" << endl;
    cout << "System Destroyed" << endl ;
}

/// @brief start-up sequence
void System::init(){

    //init commands
    system("modprobe i2c-dev");
    system("modprobe i2c-bcm2835");
    system("date 112123212023.00"); //substituir pelo RTC

    //Retirar define values da database

    //init comms

    
}

time_t System::get_time(){

    time_t curr_time;
    time(&curr_time);

    return curr_time;
}

/// @brief receber dados e enviar 
void System::process_data(){
    //insert data 
    
    

}

sensors_data System::get_sensor_data() {

    sensors_data system_data;

    system_data.temperature = temp_hum_sensor.get_Temperature();
    system_data.air_humidity = temp_hum_sensor.get_Humidity();
    system_data.soil_humidity = soil_h_sensor.get_soil_moisture();
    system_data.water_level = water_sensor.get_water_level();
    system_data.light_level = light_sensor.get_brightness_value();

    return system_data;
}
