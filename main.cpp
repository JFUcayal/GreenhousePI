#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sqlite3.h> 
#include <stdlib.h>

#include "system.h"
#include "sensors.h"
#include "actuators.h"
#include "Database.h"

using namespace std;

int main(){

    System local_sys;

    local_sys.init();


    const char* dbName = "greenhouse.db";
    Database Greenhouse_Database(dbName);  

    time_t timestamp = local_sys.get_time();
    tm *tm_info = localtime(&timestamp);


    //DATABASE TESTS
    Greenhouse_Database.insertData(*tm_info, 10, 11, 12, 13, false);
    Greenhouse_Database.insertUser("admin2727", true);
    Greenhouse_Database.insertDefineValues(27, 28, 20, false);


    float real_time_temp,
          real_time_a_hum,
          real_time_s_hum,
          real_time_light_level, 
          real_time_water_level;

    sensors_data real_time_values;

    for(int i=0; i<5; i++){

        real_time_values = local_sys.get_sensor_data();

        real_time_temp = real_time_values.temperature;
        real_time_a_hum = real_time_values.air_humidity;
        real_time_s_hum = real_time_values.soil_humidity;
        real_time_light_level = real_time_light_level; 
        real_time_water_level = real_time_values.water_level;

        sleep(1);
    }


/*
    Actuators bomba_agua1;

    bomba_agua1.set_Actuators_State(true);
    bomba_agua1.get_Actuators_State();
*/

    return 0;
}

/*   
    //CREATE DATABASE

    sqlite3* db;
    const char* dbName = "greenhouse.db";

    if (sqlite3_open(dbName, &db) != SQLITE_OK) {
        cerr << "Error open DB " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return 1;
    } else {
        cout << "Opened Database Successfully!" << endl;
    }

    const char* sql1 = "DROP TABLE USER";
    sqlite3_exec(db, sql1, 0, 0, 0);

    const char* sql = "CREATE TABLE USER("  \
                    "username TEXT PRIMARY KEY NOT NULL," \
                    "name TEXT," \
                    "password TEXT," \
                    "token BOOLEAN NOT NULL);";


    sqlite3_exec(db, sql, 0, 0, 0);


        const char* sql2 = "DROP TABLE DATA";
    sqlite3_exec(db, sql2, 0, 0, 0);


        const char* sql3 = "CREATE TABLE DATA("  \
                        "sampleTime TEXT PRIMARY KEY NOT NULL," \
                        "avgTemperature REAL NOT NULL," \
                        "avgHumidity REAL NOT NULL," \
                        "avgSoil_Humidity REAL NOT NULL," \
                        "avgWater_Level REAL NOT NULL," \
                        "luminosity_State BOOLEAN NOT NULL);";


    sqlite3_exec(db, sql3, 0, 0, 0);

    const char* sql4 = "DROP TABLE DEFINE_VALUES";
    sqlite3_exec(db, sql4, 0, 0, 0);

    const char* sql5 = "CREATE TABLE DEFINE_VALUES("  \
                  "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
                  "define_temperature REAL NOT NULL," \
                  "define_humidity REAL NOT NULL," \
                  "define_soilHumidity REAL NOT NULL," \
                  "light BOOLEAN NOT NULL);";


    sqlite3_exec(db, sql5, 0, 0, 0);

    sqlite3_close(db);
    return 0;
}
*/