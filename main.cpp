#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sqlite3.h> 
#include <stdlib.h>

#include "system.h"
#include "user.h"
#include "sensors.h"
#include "actuators.h"
#include "Database.h"
#include "admin.h"


using namespace std;

int main(){

    System local_sys;
    local_sys.get_time();

    const char* dbName = "greenhouse.db";
    Database Greenhouse_Database(dbName);  

    //User admin;

    Admin admin1;

    string name = "admin";
    string pass = "123";


    //Login verification
    if(admin1.login(pass)){

        //new parameters
        admin1.set_parameters(25, 50, 50, true);
        
        Greenhouse_Database.insertData(1, 25, 50, 50, 8, false);
        Greenhouse_Database.showAllData();
        //Greenhouse_Database.deleteAllData();
        //Greenhouse_Database.showAllData();
        
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