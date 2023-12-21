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
    system("date 112123212023.00");

    //criar sensores e atuadores
    
    //Temp_Hum_Sensor sensor_temp;
    //Soil_Hum_Sensor sensor_soil_h;  
}

/// @brief receber dados e enviar 
void System::process_data(){
    //insert data 
    
}


time_t System::get_time(){

    time_t curr_time;
    time(&curr_time);

    return curr_time;

}

