#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sqlite3.h> 
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

#include "system.h"
#include "sensors.h"
#include "actuators.h"
#include "Database.h"

using namespace std;

pthread_mutex_t mutex_ReadSensors = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Actuators = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_CalculateRef = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Database = PTHREAD_MUTEX_INITIALIZER;

System local_sys;

void sys_init(){

    local_sys.init();

    const char* dbName = "greenhouse.db";
    Database Greenhouse_Database(dbName);  

    time_t timestamp = local_sys.get_time();
    tm *tm_info = localtime(&timestamp);


    //DATABASE TESTS
    Greenhouse_Database.insertData(*tm_info, 10, 11, 12, 13, false);
    Greenhouse_Database.insertUser("admin2727", true);
    Greenhouse_Database.insertDefineValues(27, 28, 20, false);

}

void SetupThread(int prio, pthread_attr_t *pthread_attr, struct sched_param *pthread_param){
    
    int rr_min_priority, rr_max_priority;

    //Retrieve min & max prio values for scheduling policy
    rr_min_priority = sched_get_priority_min (SCHED_RR);
    rr_max_priority = sched_get_priority_max (SCHED_RR);

    //calculate and/or assign prio value to sched_param struct
    pthread_param -> sched_priority = prio;
    cout << "SCHED_RR priority range is " << rr_min_priority << " to " 
         << rr_max_priority << ": using " << pthread_param -> sched_priority << endl;

    //set attribute object with scheduling parameter
    pthread_attr_setschedparam (pthread_attr, pthread_param);

    //set scheduling attributes to be determined by attribute object
    pthread_attr_setinheritsched (pthread_attr, PTHREAD_EXPLICIT_SCHED);

    //set scheduling policy
    pthread_attr_setschedpolicy (pthread_attr, SCHED_RR);
}

void *tReadSensors(void *thread_ID){ 

    float real_time_temp,
          real_time_a_hum,
          real_time_s_hum,
          real_time_light_level, 
          real_time_water_level;

    sensors_data real_time_values;

    while(1) {
        pthread_mutex_lock(&mutex_ReadSensors);

        real_time_values = local_sys.get_sensor_data();

        pthread_mutex_unlock(&mutex_ReadSensors);

    
        real_time_temp = real_time_values.temperature;
        real_time_a_hum = real_time_values.air_humidity;
        real_time_s_hum = real_time_values.soil_humidity;
        real_time_light_level = real_time_light_level; 
        real_time_water_level = real_time_values.water_level;
    
        //msq_send() -> TEMP 
        //msq_send() -> A_HUM
        //msq_send() -> S_HUM
        //msq_send() -> LIGHT
        //msq_send() -> WATER

        sleep(1);

        //return NULL;
    }
}

void *tActuators(void *thread_ID){ 

    while(1) {
        pthread_mutex_lock(&mutex_Actuators);

        pthread_mutex_unlock(&mutex_Actuators);
    }
}

void *tCalculateRef(void *thread_ID){ 

    while(1) {
        pthread_mutex_lock(&mutex_CalculateRef);

        pthread_mutex_unlock(&mutex_CalculateRef);
    }
}
void *tDatabase(void *thread_ID){

    while(1) {
        pthread_mutex_lock(&mutex_Database);

        pthread_mutex_unlock(&mutex_Database);
    }
}


int main(){

    int a=0;

    sys_init();

    pthread_t readSensorsID, actuatorsID, calculateRefID, databaseID;  
    
    int rSensors = pthread_create(&readSensorsID, NULL, tReadSensors, NULL);
    //int rActuators = pthread_create(&actuatorsID, NULL, tActuators, NULL);
    //int rCalcRef = pthread_create(&calculateRefID, NULL, tCalculateRef, NULL);
    //int rDatabase = pthread_create(&databaseID, NULL, tDatabase, NULL);

    //pthread_create -> Returns 0 on success, or a positive error number on error
    if(rSensors /* || rActuators || rCalcRef || rDatabase */){
        cout << "Error -> Returns from pthread_create: " << rSensors << " - " /*
                                                         << rActuators << " - "
                                                         << rCalcRef << " - "
                                                         << rDatabase << " - " */ << endl; 
        exit(-1);
    }

    pthread_join(readSensorsID, NULL);
    //pthread_join(actuatorsID, NULL);
    //pthread_join(calculateRefID, NULL);
    //pthread_join(databaseID, NULL);    

    pthread_exit(NULL);

    /*
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
    */

    return 0;

/*
    Actuators bomba_agua1;

    bomba_agua1.set_Actuators_State(true);
    bomba_agua1.get_Actuators_State();
*/
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