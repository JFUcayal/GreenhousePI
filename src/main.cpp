#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sqlite3.h> 
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <mqueue.h>
#include <cstring>
#include <vector>
#include <cmath> 
#include <ctime>
#include <cstdlib>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>

#include "system.h"
#include "sensors.h"
#include "actuators.h"
#include "Database.h"

using namespace std;


int main(){

    pid_t childPid = fork();

    if (childPid == -1) {
        std::cerr << "Error creating child process" << std::endl;
        return 1;
    }

    if (childPid == 0) {
        std::cout << "Process 2 code should be executed here" << std::endl;

        // Execute o processo_teste em segundo plano
        if (fork() == 0) {
            execl("/etc/mbed_sock_thread_2", "mbed_sock_thread_2", nullptr);
            std::cerr << "Error executing mbed_sock_thread_2" << std::endl;
            return 1;
        }

        // Saia do processo filho do Process 2
        exit(0);
    } else {
        std::cout << "Process 1: Executing specific operations" << std::endl;

        // Aguarde o término do processo filho do Process 2
        waitpid(childPid, nullptr, 0);
    }

    
        const char* dbName = "greenhouse.db";
        System local_sys(dbName);
        return 0;
    }

/*
    const char* dbName = "greenhouse.db";
    System local_sys(dbName);

    sensors_data kkk;   
    local_sys.getDataValuesDatabase(kkk);
    cout << "hours: " << kkk.timestamp << "    " << kkk.temperature << "  " << kkk.air_humidity << "   " << kkk.soil_humidity << "   " << kkk.water_level << "   " << kkk.light_level << endl;

    local_sys.insertUserDatabase("senhor zeca27", false);
*/

/*
int main(){  
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
