#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>

#include "user.h"
#include "sensors.h"
#include "actuators.h"

using namespace std;

int main(){

    User admin;
    string name = "admin";
    string pass = "123";

    //Login verification
    if(admin.login(pass)){

        //new parameters
        admin.set_parameters(25, 50, 50, true);
        
        Actuators bomba_agua1;

        bomba_agua1.set_Actuators_State(true);
        bomba_agua1.get_Actuators_State();


        Temp_Hum_Sensor sensor_temp;

        for(int i=0; i<13; i++){
            sensor_temp.get_Temperature();
            sensor_temp.get_Humidity();
            sleep(2);
        }
        
        
    }

    return 0;
}